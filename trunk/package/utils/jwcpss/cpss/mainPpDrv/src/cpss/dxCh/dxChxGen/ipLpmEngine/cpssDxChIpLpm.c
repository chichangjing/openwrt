/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpLpm.c
*
* DESCRIPTION:
*       the CPSS DXCH LPM Hierarchy manager .
*
* FILE REVISION NUMBER:
*       $Revision: 67 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssSkipList.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/prvCpssDxChIpLpmEngineLog.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamHsu.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmUtils.h>

#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
static GT_VOID myCpssOsFree
(
    IN GT_VOID* const memblock
)
{
    cpssOsFree_MemoryLeakageDbg(memblock,__FILE__,__LINE__);
}
#endif /*OS_MALLOC_MEMORY_LEAKAGE_DBG*/

/*******************************************************************************
 *  local vars                                                                 *
 ******************************************************************************/
GT_VOID *prvCpssDxChIplpmDbSL;

/*******************************************************************************
* lpmDbComp
*
* DESCRIPTION:
*   lpm DB compare function
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       data1Ptr        - (pointer to) the first lpm DB to compare.
*       data2Ptr        - (pointer to) the second lpm DB to compare.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_COMP_RES - equal ,smaller or bigger.
*
* COMMENTS:
*       none.
*
*
*******************************************************************************/
static GT_COMP_RES lpmDbComp
(
    IN GT_VOID *data1Ptr,
    IN GT_VOID *data2Ptr
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDb1Ptr,*lpmDb2Ptr;

    lpmDb1Ptr = data1Ptr;
    lpmDb2Ptr = data2Ptr;

    /* Check the null cases     */
    if((lpmDb1Ptr == NULL) && (lpmDb2Ptr == NULL))
    {
        return GT_EQUAL;
    }
    if (lpmDb1Ptr == NULL)
    {
        return GT_SMALLER;
    }
    if (lpmDb2Ptr == NULL)
    {
        return GT_GREATER;
    }

    /* Both data's are not NULL */
    if (lpmDb1Ptr->lpmDbId != lpmDb2Ptr->lpmDbId)
    {
        if (lpmDb1Ptr->lpmDbId > lpmDb2Ptr->lpmDbId)
        {
            return GT_GREATER;
        }
        else
        {
            return GT_SMALLER;
        }
    }

    return GT_EQUAL;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBCreate
*
* DESCRIPTION:
*       This function creates an LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmDBId               - the LPM DB id
*       shadowType            - the type of shadow to maintain
*       protocolStack         - the type of protocol stack this LPM DB support
*       memoryCfgPtr          - the memory configuration for this LPM DB
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_SUPPORTED         - request is not supported if partitioning
*                                  is disabled.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBCreate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT             shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT           *memoryCfgPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamLpmDbPtr = NULL;
    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *ramLpmDbPtr = NULL;
    GT_UINTPTR slIter;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT lpmShadowType;
    PRV_CPSS_DXCH_LPM_TCAM_CONFIG_STC tcamMemoryCfg;
    PRV_CPSS_DXCH_LPM_RAM_CONFIG_STC  ramMemoryCfg;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP protocolBitmap;
    GT_U32 i=0;

    CPSS_NULL_PTR_CHECK_MAC(memoryCfgPtr);
    PRV_CPSS_DXCH_IP_CHECK_SHADOW_TYPE_MAC(shadowType);
    PRV_CPSS_DXCH_IP_CHECK_PROTOCOL_STACK_MAC(protocolStack);

    cpssOsMemSet(&ramMemoryCfg, 0, sizeof(PRV_CPSS_DXCH_LPM_RAM_CONFIG_STC));

    if (prvCpssDxChIplpmDbSL == NULL)
    {
        prvCpssDxChIplpmDbSL = prvCpssSlInit(lpmDbComp,PRV_CPSS_MAX_PP_DEVICES_CNS);
        if (prvCpssDxChIplpmDbSL == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr != NULL)
    {
        /* can't redefine an lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }
    lpmDbPtr = (PRV_CPSS_DXCH_LPM_SHADOW_STC*)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_SHADOW_STC));
    if (lpmDbPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    retVal = prvCpssDxChLpmConvertIpShadowTypeToLpmShadowType(shadowType, &lpmShadowType);
    if (retVal != GT_OK)
    {
        cpssOsFree(lpmDbPtr);
        return retVal;
    }
    lpmHw = prvCpssDxChLpmGetHwType(lpmShadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            CPSS_NULL_PTR_CHECK_MAC(memoryCfgPtr->tcamDbCfg.indexesRangePtr);
            CPSS_NULL_PTR_CHECK_MAC(memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr);
            if (memoryCfgPtr->tcamDbCfg.partitionEnable == GT_TRUE)
            {
                if ((memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes == 0) &&
                    ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E)))
                {
                    cpssOsFree(lpmDbPtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                if ((memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes == 0) &&
                    (memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes == 0) &&
                    ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E)))
                {
                    cpssOsFree(lpmDbPtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            tcamMemoryCfg.indexesRange.firstIndex = memoryCfgPtr->tcamDbCfg.indexesRangePtr->firstIndex;
            tcamMemoryCfg.indexesRange.lastIndex = memoryCfgPtr->tcamDbCfg.indexesRangePtr->lastIndex;
            tcamMemoryCfg.partitionEnable = memoryCfgPtr->tcamDbCfg.partitionEnable;
            tcamMemoryCfg.tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes;
            tcamMemoryCfg.tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes;
            tcamMemoryCfg.tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes;
            tcamMemoryCfg.tcamManagerHandlerPtr = memoryCfgPtr->tcamDbCfg.tcamManagerHandlerPtr;

            retVal = prvCpssDxChLpmTcamDbCreate(&tcamLpmDbPtr, lpmShadowType,
                                                protocolStack, &tcamMemoryCfg);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            if ((memoryCfgPtr->ramDbCfg.numOfBlocks == 0) ||
                (memoryCfgPtr->ramDbCfg.numOfBlocks > PRV_CPSS_DXCH_LPM_RAM_MAX_NUM_OF_MEMORY_BLOCKS_CNS))
                {
                    cpssOsFree(lpmDbPtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            for (i=0;i<memoryCfgPtr->ramDbCfg.numOfBlocks;i++)
            {
                if (memoryCfgPtr->ramDbCfg.blocksSizeArray[i] == 0)
                {
                    cpssOsFree(lpmDbPtr);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }

            prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap(protocolStack, &protocolBitmap);

            ramMemoryCfg.numOfBlocks = memoryCfgPtr->ramDbCfg.numOfBlocks;
            for (i=0;i<memoryCfgPtr->ramDbCfg.numOfBlocks;i++)
            {
                /* convert bytes to lines */
                ramMemoryCfg.blocksSizeArray[i] = (memoryCfgPtr->ramDbCfg.blocksSizeArray[i]/4);
            }

            switch(memoryCfgPtr->ramDbCfg.blocksAllocationMethod)
            {
            case CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E:
                ramMemoryCfg.blocksAllocationMethod = PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E;
                break;
            case CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E:
                ramMemoryCfg.blocksAllocationMethod = PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E;
                break;
            default:
                cpssOsFree(lpmDbPtr);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            retVal = prvCpssDxChLpmRamDbCreate(&ramLpmDbPtr, protocolBitmap,&ramMemoryCfg);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (retVal == GT_OK)
    {
        /* record the lpm db */
        lpmDbPtr->lpmDbId = lpmDBId;
        lpmDbPtr->shadowType = lpmShadowType;
        if (lpmHw == PRV_CPSS_DXCH_LPM_HW_TCAM_E)
        {
            lpmDbPtr->shadow = tcamLpmDbPtr;
        }
        else
        {
            lpmDbPtr->shadow = ramLpmDbPtr;
        }
        retVal = prvCpssSlAdd(prvCpssDxChIplpmDbSL,lpmDbPtr,&slIter);
    }
    else
    {
        cpssOsFree(lpmDbPtr);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmDBCreate
*
* DESCRIPTION:
*       This function creates an LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmDBId               - the LPM DB id
*       shadowType            - the type of shadow to maintain
*       protocolStack         - the type of protocol stack this LPM DB support
*       memoryCfgPtr          - the memory configuration for this LPM DB
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_SUPPORTED         - request is not supported if partitioning
*                                  is disabled.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCreate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT             shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT           *memoryCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, shadowType, protocolStack, memoryCfgPtr));

    rc = internal_cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, memoryCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, shadowType, protocolStack, memoryCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBDelete
*
* DESCRIPTION:
*   This function deletes LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId                   - the LPM DB id (range 32 bits)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - the LPM DB is not empty
*       GT_FAIL                   - on error
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       Deleting LPM DB is allowed only if no VR present in the LPM DB (since
*       prefixes reside within VR it means that no prefixes present as well).
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBDelete
(
    IN  GT_U32      lpmDbId
)
{
    GT_STATUS  rc;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    /* search for the LPM DB */
    tmpLpmDb.lpmDbId = lpmDbId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            /* delete the LPM DB */
            rc = prvCpssDxChLpmTcamDbDelete((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow));
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            rc = prvCpssDxChLpmRamDbDelete((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow));
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (rc != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* remove the LPM DB from the LPM DB skip list */
    if (prvCpssSlDelete(prvCpssDxChIplpmDbSL,&tmpLpmDb) == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* free the LPM DB memory */
    cpssOsFree(lpmDbPtr);

    /* if the LPM DB skip list is empty, delete it */
    if (prvCpssSlIsEmpty(prvCpssDxChIplpmDbSL) == GT_TRUE)
    {
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
        if (prvCpssSlDestroy(prvCpssDxChIplpmDbSL,myCpssOsFree) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
#else /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
        if (prvCpssSlDestroy(prvCpssDxChIplpmDbSL,cpssOsFree) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
#endif /*! OS_MALLOC_MEMORY_LEAKAGE_DBG */
        prvCpssDxChIplpmDbSL = NULL;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmDBDelete
*
* DESCRIPTION:
*   This function deletes LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId                   - the LPM DB id (range 32 bits)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - the LPM DB is not empty
*       GT_FAIL                   - on error
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       Deleting LPM DB is allowed only if no VR present in the LPM DB (since
*       prefixes reside within VR it means that no prefixes present as well).
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDelete
(
    IN  GT_U32      lpmDbId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDbId));

    rc = internal_cpssDxChIpLpmDBDelete(lpmDbId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDbId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBConfigGet
*
* DESCRIPTION:
*       This function retrieves configuration of the LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId                       - the LPM DB id.
*
* OUTPUTS:
*       shadowTypePtr         - the type of shadow to maintain
*       protocolStackPtr      - the type of protocol stack this LPM DB support
*       memoryCfgPtr          - the memory configuration for this LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PTR               - NULL pointer.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBConfigGet
(
    IN  GT_U32                             lpmDBId,
    OUT CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT   *shadowTypePtr,
    OUT CPSS_IP_PROTOCOL_STACK_ENT         *protocolStackPtr,
    OUT CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT *memoryCfgPtr
)
{
    GT_STATUS                           rc;
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT lpmShadowType;
    PRV_CPSS_DXCH_LPM_TCAM_CONFIG_STC tcamMemoryCfg;
    PRV_CPSS_DXCH_LPM_RAM_CONFIG_STC  ramMemoryCfg;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP    protocolBitmap;
    GT_U32 i=0;

    CPSS_NULL_PTR_CHECK_MAC(shadowTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(protocolStackPtr);
    CPSS_NULL_PTR_CHECK_MAC(memoryCfgPtr);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    lpmShadowType = lpmDbPtr->shadowType;
    rc = prvCpssDxChLpmConvertLpmShadowTypeToIpShadowType(lpmShadowType, shadowTypePtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    lpmHw = prvCpssDxChLpmGetHwType(lpmShadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            CPSS_NULL_PTR_CHECK_MAC(memoryCfgPtr->tcamDbCfg.indexesRangePtr);
            CPSS_NULL_PTR_CHECK_MAC(memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr);
            rc = prvCpssDxChLpmTcamDbConfigGet((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                               protocolStackPtr, &tcamMemoryCfg);
            memoryCfgPtr->tcamDbCfg.indexesRangePtr->firstIndex =
                tcamMemoryCfg.indexesRange.firstIndex;
            memoryCfgPtr->tcamDbCfg.indexesRangePtr->lastIndex =
                tcamMemoryCfg.indexesRange.lastIndex;
            memoryCfgPtr->tcamDbCfg.partitionEnable = tcamMemoryCfg.partitionEnable;
            memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes =
                tcamMemoryCfg.tcamLpmManagerCapcityCfg.numOfIpv4Prefixes;
            memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes =
                tcamMemoryCfg.tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes;
            memoryCfgPtr->tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes =
                tcamMemoryCfg.tcamLpmManagerCapcityCfg.numOfIpv6Prefixes;
            memoryCfgPtr->tcamDbCfg.tcamManagerHandlerPtr = tcamMemoryCfg.tcamManagerHandlerPtr;
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            rc = prvCpssDxChLpmRamDbConfigGet((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                               &protocolBitmap,&ramMemoryCfg);
            prvCpssDxChLpmConvertProtocolBitmapToIpProtocolStack(protocolBitmap, protocolStackPtr);

            memoryCfgPtr->ramDbCfg.numOfBlocks = ramMemoryCfg.numOfBlocks;
            for (i=0;i<memoryCfgPtr->ramDbCfg.numOfBlocks;i++)
            {
                /* convert lines to bytes */
                memoryCfgPtr->ramDbCfg.blocksSizeArray[i] = (ramMemoryCfg.blocksSizeArray[i]*4);
            }
            switch (ramMemoryCfg.blocksAllocationMethod)
            {
                case PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E:
                    memoryCfgPtr->ramDbCfg.blocksAllocationMethod=CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E;
                    break;
                case PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E:
                    memoryCfgPtr->ramDbCfg.blocksAllocationMethod=CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E;
                    break;
                default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmDBConfigGet
*
* DESCRIPTION:
*       This function retrieves configuration of the LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId                       - the LPM DB id.
*
* OUTPUTS:
*       shadowTypePtr         - the type of shadow to maintain
*       protocolStackPtr      - the type of protocol stack this LPM DB support
*       memoryCfgPtr          - the memory configuration for this LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PTR               - NULL pointer.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBConfigGet
(
    IN  GT_U32                             lpmDBId,
    OUT CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT   *shadowTypePtr,
    OUT CPSS_IP_PROTOCOL_STACK_ENT         *protocolStackPtr,
    OUT CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT *memoryCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, shadowTypePtr, protocolStackPtr, memoryCfgPtr));

    rc = internal_cpssDxChIpLpmDBConfigGet(lpmDBId, shadowTypePtr, protocolStackPtr, memoryCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, shadowTypePtr, protocolStackPtr, memoryCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBCapacityUpdate
*
* DESCRIPTION:
*   This function updates the initial LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*       indexesRangePtr             - (pointer to) the range of TCAM indexes
*                                     available for this LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       tcamLpmManagerCapcityCfgPtr - the new capacity configuration. when
*                                     GT_TRUE: the TCAM is partitioned according
*                                     configuration. when partitionEnable in
*                                     was set to GT_TRUE this means new prefixes
*                                     partition, when this was set to GT_FALSE
*                                     this means the new prefixes guaranteed
*                                     allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success
*       GT_BAD_PARAM                - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*       GT_NOT_FOUND                - if the LPM DB id is not found
*       GT_NO_RESOURCE              - failed to allocate resources needed to the
*                                     new configuration
*
* COMMENTS:
*       This API is relevant only when using TCAM Manager. This API is used for
*       updating only the capacity configuration of the LPM. for updating the
*       lines reservation for the TCAM Manger use cpssDxChTcamManagerRangeUpdate.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBCapacityUpdate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC                    *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT                        lpmHw;
    PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC   lpmIndexesRange;
    PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC   lpmTcamManagerCapcityCfgPtr;

    CPSS_NULL_PTR_CHECK_MAC(indexesRangePtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamLpmManagerCapcityCfgPtr);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            lpmIndexesRange.firstIndex = indexesRangePtr->firstIndex;
            lpmIndexesRange.lastIndex = indexesRangePtr->lastIndex;
            lpmTcamManagerCapcityCfgPtr.numOfIpv4Prefixes =
                tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes;
            lpmTcamManagerCapcityCfgPtr.numOfIpv4McSourcePrefixes =
                tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes;
            lpmTcamManagerCapcityCfgPtr.numOfIpv6Prefixes =
                tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes;
            return prvCpssDxChLpmTcamDbCapacityUpdate((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                      &lpmIndexesRange,
                                                      &lpmTcamManagerCapcityCfgPtr);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmDBCapacityUpdate
*
* DESCRIPTION:
*   This function updates the initial LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*       indexesRangePtr             - (pointer to) the range of TCAM indexes
*                                     available for this LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       tcamLpmManagerCapcityCfgPtr - the new capacity configuration. when
*                                     GT_TRUE: the TCAM is partitioned according
*                                     configuration. when partitionEnable in
*                                     was set to GT_TRUE this means new prefixes
*                                     partition, when this was set to GT_FALSE
*                                     this means the new prefixes guaranteed
*                                     allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success
*       GT_BAD_PARAM                - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*       GT_NOT_FOUND                - if the LPM DB id is not found
*       GT_NO_RESOURCE              - failed to allocate resources needed to the
*                                     new configuration
*
* COMMENTS:
*       This API is relevant only when using TCAM Manager. This API is used for
*       updating only the capacity configuration of the LPM. for updating the
*       lines reservation for the TCAM Manger use cpssDxChTcamManagerRangeUpdate.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCapacityUpdate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBCapacityUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, indexesRangePtr, tcamLpmManagerCapcityCfgPtr));

    rc = internal_cpssDxChIpLpmDBCapacityUpdate(lpmDBId, indexesRangePtr, tcamLpmManagerCapcityCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, indexesRangePtr, tcamLpmManagerCapcityCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBCapacityGet
*
* DESCRIPTION:
*   This function gets the current LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*
* OUTPUTS:
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       partitionEnablePtr          - GT_TRUE:  the TCAM is partitioned according
*                                     to the capacityCfgPtr, any unused TCAM entries
*                                     were allocated to IPv4 UC entries.
*                                     GT_FALSE: TCAM entries are allocated on demand
*                                     while entries are guaranteed as specified
*                                     in capacityCfgPtr.
*       tcamLpmManagerCapcityCfgPtr - the current capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means current
*                                     prefixes partition, when this was set to
*                                     GT_FALSE this means the current guaranteed
*                                     prefixes allocation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NO_RESOURCE           - failed to allocate resources needed to the
*                                  new configuration.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBCapacityGet
(
    IN  GT_U32                                          lpmDBId,
    OUT GT_BOOL                                         *partitionEnablePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    *tcamLpmManagerCapcityCfgPtr
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC  *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC lpmIndexesRange;
    PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;
    GT_STATUS retVal;

    CPSS_NULL_PTR_CHECK_MAC(partitionEnablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexesRangePtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamLpmManagerCapcityCfgPtr);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }
    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamDbCapacityGet((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                     partitionEnablePtr,
                                                     &lpmIndexesRange,
                                                     &tcamLpmManagerCapcityCfg);
            indexesRangePtr->firstIndex = lpmIndexesRange.firstIndex;
            indexesRangePtr->lastIndex = lpmIndexesRange.lastIndex;
            tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes =
                tcamLpmManagerCapcityCfg.numOfIpv4Prefixes;
            tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes =
                tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes;
            tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes =
                tcamLpmManagerCapcityCfg.numOfIpv6Prefixes;
            return retVal;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmDBCapacityGet
*
* DESCRIPTION:
*   This function gets the current LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*
* OUTPUTS:
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       partitionEnablePtr          - GT_TRUE:  the TCAM is partitioned according
*                                     to the capacityCfgPtr, any unused TCAM entries
*                                     were allocated to IPv4 UC entries.
*                                     GT_FALSE: TCAM entries are allocated on demand
*                                     while entries are guaranteed as specified
*                                     in capacityCfgPtr.
*       tcamLpmManagerCapcityCfgPtr - the current capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means current
*                                     prefixes partition, when this was set to
*                                     GT_FALSE this means the current guaranteed
*                                     prefixes allocation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NO_RESOURCE           - failed to allocate resources needed to the
*                                  new configuration.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCapacityGet
(
    IN  GT_U32                                          lpmDBId,
    OUT GT_BOOL                                         *partitionEnablePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    *tcamLpmManagerCapcityCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBCapacityGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, partitionEnablePtr, indexesRangePtr, tcamLpmManagerCapcityCfgPtr));

    rc = internal_cpssDxChIpLpmDBCapacityGet(lpmDBId, partitionEnablePtr, indexesRangePtr, tcamLpmManagerCapcityCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, partitionEnablePtr, indexesRangePtr, tcamLpmManagerCapcityCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBCheetah2VrSupportCreate
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment in Cheetah2
*   devices with support for virtual router.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId          - the LPM DB id.
*       protocolStack    - the type of protocol stack this LPM DB support.
*       pclTcamCfgPtr    - (pointer to) the pcl tcam configuration
*       routerTcamCfgPtr - (pointer to) the router tcam configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - NULL pointer.
*
* COMMENTS:
*       Since LTT rules are shared between PCL and Router Tcam, several checks
*       are made to supplied parameters to make sure that all prefixes could be
*       inserted.
*
*       Note that LTT entries corresponding to routerTcamCfgPtr->tcamRange
*       are allocated to Lpm Manager.
*
*       Furthermore:
*
*       routerTcamCfgPtr->tcamRange.lastIndex -
*       routerTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Router Tcam lines).
*
*       pclTcamCfgPtr->tcamRange.lastIndex -
*       pclTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Pcl Tcam lines).
*
*       (routerTcamCfgPtr->tcamRange.lastIndex -
*        routerTcamCfgPtr->tcamRange.firstIndex + 1) * 5
*           must be larger or equal to
*       pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes
*
*       Multicast is not supported.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBCheetah2VrSupportCreate
(
    IN GT_U32                                                   lpmDBId,
    IN CPSS_IP_PROTOCOL_STACK_ENT                               protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr
)
{
    GT_STATUS                           retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC   *tcamLpmDbPtr = NULL;
    GT_UINTPTR                          slIter;

    CPSS_NULL_PTR_CHECK_MAC(pclTcamCfgPtr);
    CPSS_NULL_PTR_CHECK_MAC(routerTcamCfgPtr);
    PRV_CPSS_DXCH_IP_CHECK_PROTOCOL_STACK_MAC(protocolStack);

    if (prvCpssDxChIplpmDbSL == NULL)
    {
        prvCpssDxChIplpmDbSL = prvCpssSlInit(lpmDbComp,PRV_CPSS_MAX_PP_DEVICES_CNS);
        if (prvCpssDxChIplpmDbSL == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr != NULL)
    {
        /* can't redefine an lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    /* ok, create a new one */
    lpmDbPtr = (PRV_CPSS_DXCH_LPM_SHADOW_STC*)
                        cpssOsMalloc(sizeof(PRV_CPSS_DXCH_LPM_SHADOW_STC));
    if (lpmDbPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    retVal = prvCpssDxChLpmTcamDbCheetah2VrSupportCreate(&tcamLpmDbPtr,
                                                         protocolStack,
                                                         pclTcamCfgPtr,
                                                         routerTcamCfgPtr);
    if (retVal == GT_OK)
    {
        /* record the lpm db */
        lpmDbPtr->lpmDbId = lpmDBId;
        lpmDbPtr->shadowType = PRV_CPSS_DXCH_LPM_TCAM_CHEETAH2_SHADOW_E;
        lpmDbPtr->shadow = tcamLpmDbPtr;
        retVal = prvCpssSlAdd(prvCpssDxChIplpmDbSL,lpmDbPtr,&slIter);
    }
    else
    {
        cpssOsFree(lpmDbPtr);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmDBCheetah2VrSupportCreate
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment in Cheetah2
*   devices with support for virtual router.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId          - the LPM DB id.
*       protocolStack    - the type of protocol stack this LPM DB support.
*       pclTcamCfgPtr    - (pointer to) the pcl tcam configuration
*       routerTcamCfgPtr - (pointer to) the router tcam configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - NULL pointer.
*
* COMMENTS:
*       Since LTT rules are shared between PCL and Router Tcam, several checks
*       are made to supplied parameters to make sure that all prefixes could be
*       inserted.
*
*       Note that LTT entries corresponding to routerTcamCfgPtr->tcamRange
*       are allocated to Lpm Manager.
*
*       Furthermore:
*
*       routerTcamCfgPtr->tcamRange.lastIndex -
*       routerTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Router Tcam lines).
*
*       pclTcamCfgPtr->tcamRange.lastIndex -
*       pclTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Pcl Tcam lines).
*
*       (routerTcamCfgPtr->tcamRange.lastIndex -
*        routerTcamCfgPtr->tcamRange.firstIndex + 1) * 5
*           must be larger or equal to
*       pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes
*
*       Multicast is not supported.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCheetah2VrSupportCreate
(
    IN GT_U32                                                   lpmDBId,
    IN CPSS_IP_PROTOCOL_STACK_ENT                               protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBCheetah2VrSupportCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, protocolStack, pclTcamCfgPtr, routerTcamCfgPtr));

    rc = internal_cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack, pclTcamCfgPtr, routerTcamCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, protocolStack, pclTcamCfgPtr, routerTcamCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB. this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBDevListAdd
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if(numOfDevs)
    {
        CPSS_NULL_PTR_CHECK_MAC(devListArr);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamDbDevListAdd((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                    devListArr, numOfDevs);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamDbDevListAdd((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                    devListArr, numOfDevs);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB. this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDevListAdd
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBDevListAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, devListArr, numOfDevs));

    rc = internal_cpssDxChIpLpmDBDevListAdd(lpmDBId, devListArr, numOfDevs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, devListArr, numOfDevs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBDevsListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing LPM DB. this remove will
*   invoke a hot sync removal of the devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to remove from the
*                        LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBDevsListRemove
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if(numOfDevs > 0)
        CPSS_NULL_PTR_CHECK_MAC(devListArr);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamDbDevListRemove((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                       devListArr, numOfDevs);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamDbDevListRemove((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                       devListArr, numOfDevs);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmDBDevsListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing LPM DB. this remove will
*   invoke a hot sync removal of the devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to remove from the
*                        LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDevsListRemove
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBDevsListRemove);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, devListArr, numOfDevs));

    rc = internal_cpssDxChIpLpmDBDevsListRemove(lpmDBId, devListArr, numOfDevs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, devListArr, numOfDevs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBDevListGet
*
* DESCRIPTION:
*       This function retrieves the list of devices in an existing LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDbId        - the LPM DB id.
*       numOfDevsPtr   - (pointer to) the size of devListArray
*
* OUTPUTS:
*       numOfDevsPtr   - (pointer to) the number of devices retreived
*       devListArray   - array of device ids in the LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_SIZE              - in case not enough memory was allocated to
*                                  the device list
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The application should allocate memory for the device list array.
*       If the function returns GT_OK, then number of devices holds
*       the number of devices the function filled.
*       If the function returns GT_BAD_SIZE, then the memory allocated by the
*       application to the device list is not enough. In this case the
*       number of devices will hold the size of array needed.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBDevListGet
(
    IN    GT_U32                        lpmDbId,
    INOUT GT_U32                        *numOfDevsPtr,
    OUT   GT_U8                         devListArray[]
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(numOfDevsPtr);
    CPSS_NULL_PTR_CHECK_MAC(devListArray);

    tmpLpmDb.lpmDbId = lpmDbId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            return prvCpssDxChLpmTcamGetDevsFromShadow((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), devListArray, numOfDevsPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            return prvCpssDxChLpmRamDbDevListGet((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                 numOfDevsPtr, devListArray);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmDBDevListGet
*
* DESCRIPTION:
*       This function retrieves the list of devices in an existing LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDbId        - the LPM DB id.
*       numOfDevsPtr   - (pointer to) the size of devListArray
*
* OUTPUTS:
*       numOfDevsPtr   - (pointer to) the number of devices retreived
*       devListArray   - array of device ids in the LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_SIZE              - in case not enough memory was allocated to
*                                  the device list
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The application should allocate memory for the device list array.
*       If the function returns GT_OK, then number of devices holds
*       the number of devices the function filled.
*       If the function returns GT_BAD_SIZE, then the memory allocated by the
*       application to the device list is not enough. In this case the
*       number of devices will hold the size of array needed.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDevListGet
(
    IN    GT_U32                        lpmDbId,
    INOUT GT_U32                        *numOfDevsPtr,
    OUT   GT_U8                         devListArray[]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBDevListGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDbId, numOfDevsPtr, devListArray));

    rc = internal_cpssDxChIpLpmDBDevListGet(lpmDbId, numOfDevsPtr, devListArray);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDbId, numOfDevsPtr, devListArray));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmTcamLinesReserve
*
* DESCRIPTION:
*   This function reserves lines in the router TCAM. The LPM DB will not use
*   those lines for prefixes. Those lines will be considered as allocated
*   entries.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; xCat2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       linesToReserveArray       - lines to be reserved
*       numberOfLinesToReserve    - number of lines in linesToReserveArray
*       allocateReserveLinesAs    - relevant only when LPM DB is configured to
*                                   use dynamic TCAM partitioning.
*                                   Indicates as what entry type to treat the
*                                   reserved lines.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - all lines to reserve must reside in the TCAM section allocated
*           to the LPM DB
*         - all lines to reserve must be unique
*       In case the LPM DB is configured to use dynamic partitioning those
*       additional conditions must be matched:
*         - the sum of the lines in allocateReserveLinesAs must be equal to
*           the number of lines to reserve
*         - the lines in allocateReserveLinesAs must comply with the protocol
*           stacks supported by the LPM DB
*         - the LPM DB must have enough space to hold the allocateReserveLinesAs
*           as allocated entires.
*       Note that the parameters in allocateReserveLinesAs are in lines. Meaning
*       that if (allocateReserveLinesAs.numOfIpv4Prefixes == 1) it means that
*       one whole TCAM line will be considered allocated as IPv4 prefixes. In
*       4 columns TCAM it means that 4 IPv4 prefixes will be "allocated" in
*       this line to reserve.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmTcamLinesReserve
(
    IN  GT_U32                                          lpmDBId,
    IN  GT_U32                                          linesToReserveArray[],
    IN  GT_U32                                          numberOfLinesToReserve,
    IN  CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    allocateReserveLinesAs
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC                    *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT                        lpmHw;
    PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC   lpmTcamManagerCapcityCfg;

    CPSS_NULL_PTR_CHECK_MAC(linesToReserveArray);

    /* search for the LPM DB */
    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            /* the LPM DB must be empty when using this API (can't
               contain any prefixes or virtual routers) */
            lpmTcamManagerCapcityCfg.numOfIpv4Prefixes =
                allocateReserveLinesAs.numOfIpv4Prefixes;
            lpmTcamManagerCapcityCfg.numOfIpv4McSourcePrefixes =
                allocateReserveLinesAs.numOfIpv4McSourcePrefixes;
            lpmTcamManagerCapcityCfg.numOfIpv6Prefixes =
                allocateReserveLinesAs.numOfIpv6Prefixes;
            return prvCpssDxChLpmTcamLinesReserve((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                  linesToReserveArray,
                                                  numberOfLinesToReserve,
                                                  lpmTcamManagerCapcityCfg);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmTcamLinesReserve
*
* DESCRIPTION:
*   This function reserves lines in the router TCAM. The LPM DB will not use
*   those lines for prefixes. Those lines will be considered as allocated
*   entries.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; xCat2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       linesToReserveArray       - lines to be reserved
*       numberOfLinesToReserve    - number of lines in linesToReserveArray
*       allocateReserveLinesAs    - relevant only when LPM DB is configured to
*                                   use dynamic TCAM partitioning.
*                                   Indicates as what entry type to treat the
*                                   reserved lines.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - all lines to reserve must reside in the TCAM section allocated
*           to the LPM DB
*         - all lines to reserve must be unique
*       In case the LPM DB is configured to use dynamic partitioning those
*       additional conditions must be matched:
*         - the sum of the lines in allocateReserveLinesAs must be equal to
*           the number of lines to reserve
*         - the lines in allocateReserveLinesAs must comply with the protocol
*           stacks supported by the LPM DB
*         - the LPM DB must have enough space to hold the allocateReserveLinesAs
*           as allocated entires.
*       Note that the parameters in allocateReserveLinesAs are in lines. Meaning
*       that if (allocateReserveLinesAs.numOfIpv4Prefixes == 1) it means that
*       one whole TCAM line will be considered allocated as IPv4 prefixes. In
*       4 columns TCAM it means that 4 IPv4 prefixes will be "allocated" in
*       this line to reserve.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmTcamLinesReserve
(
    IN  GT_U32                                          lpmDBId,
    IN  GT_U32                                          linesToReserveArray[],
    IN  GT_U32                                          numberOfLinesToReserve,
    IN  CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    allocateReserveLinesAs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmTcamLinesReserve);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, linesToReserveArray, numberOfLinesToReserve, allocateReserveLinesAs));

    rc = internal_cpssDxChIpLpmTcamLinesReserve(lpmDBId, linesToReserveArray, numberOfLinesToReserve, allocateReserveLinesAs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, linesToReserveArray, numberOfLinesToReserve, allocateReserveLinesAs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
*
* DESCRIPTION:
*   The function uses available TCAM rules at the beginning of the TCAM range
*   allocated to the LPM DB and place there "do nothing" rules that will make
*   sure that Multicast traffic is matched by these rules and not by the
*   Unicast default rule.
*   When policy based routing Unicast traffic coexists with IP based Multicast
*   (S,G,V) bridging (used in IGMP and MLD protocols), there is a need to add
*   default Multicast rules in order to make sure that the Multicast traffic
*   will not be matched by the Unicast default rule.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       pclIdArrayLen             - size of the PCL ID array (APPLICABLE RANGES: 1..1048575)
*       pclIdArray                - array of PCL ID that may be in used by the LPM DB
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_BAD_PARAM              - if wrong value in any of the parameters
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_ALREADY_EXIST          - if default MC already set
*       GT_NOT_SUPPORTED          - if the LPM DB doesn't configured to
*                                   operate in policy based routing mode
*       GT_FULL                   - if TCAM is full
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - the LPM DB must be configured to use policy based routing
*       One default Multicast rule will be set for each protocol stack supported
*       by the LPM DB and for each PCL ID that may be used in the LPM DB (each
*       virtaul router uses unique PCL ID).
*       For each protocol stack, the Multicast default rules will use the
*       prefixes allocated for Unicast prefixes for this protocol. For example
*       if the application allocates 10 IPv4 Unicast prefixes, then after setting
*       one default Multicast prefix, only 9 IPv4 Unicast prefixes will be
*       available.
*       The default Multicast rules will capture all IPv4/IPv6 Multicast
*       traffic with the same assigned PCL ID. As a result all rules configured
*       to match IP Multicast traffic with same assigned PCL ID that reside
*       after the Multicast defaults will not be matched.
*       The default Multicast rules can not be deleted after set.
*       The default Multicast rules can be set no more than once.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
(
    IN  GT_U32  lpmDBId,
    IN  GT_U32  pclIdArrayLen,
    IN  GT_U32  pclIdArray[]
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC                        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT                            lpmHw;

    /* search for the LPM DB */
    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            return prvCpssDxChLpmTcamPolicyBasedRoutingDefaultMcSet(
                (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), pclIdArrayLen, pclIdArray);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
*
* DESCRIPTION:
*   The function uses available TCAM rules at the beginning of the TCAM range
*   allocated to the LPM DB and place there "do nothing" rules that will make
*   sure that Multicast traffic is matched by these rules and not by the
*   Unicast default rule.
*   When policy based routing Unicast traffic coexists with IP based Multicast
*   (S,G,V) bridging (used in IGMP and MLD protocols), there is a need to add
*   default Multicast rules in order to make sure that the Multicast traffic
*   will not be matched by the Unicast default rule.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       pclIdArrayLen             - size of the PCL ID array (APPLICABLE RANGES: 1..1048575)
*       pclIdArray                - array of PCL ID that may be in used by the LPM DB
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_BAD_PARAM              - if wrong value in any of the parameters
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_ALREADY_EXIST          - if default MC already set
*       GT_NOT_SUPPORTED          - if the LPM DB doesn't configured to
*                                   operate in policy based routing mode
*       GT_FULL                   - if TCAM is full
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - the LPM DB must be configured to use policy based routing
*       One default Multicast rule will be set for each protocol stack supported
*       by the LPM DB and for each PCL ID that may be used in the LPM DB (each
*       virtaul router uses unique PCL ID).
*       For each protocol stack, the Multicast default rules will use the
*       prefixes allocated for Unicast prefixes for this protocol. For example
*       if the application allocates 10 IPv4 Unicast prefixes, then after setting
*       one default Multicast prefix, only 9 IPv4 Unicast prefixes will be
*       available.
*       The default Multicast rules will capture all IPv4/IPv6 Multicast
*       traffic with the same assigned PCL ID. As a result all rules configured
*       to match IP Multicast traffic with same assigned PCL ID that reside
*       after the Multicast defaults will not be matched.
*       The default Multicast rules can not be deleted after set.
*       The default Multicast rules can be set no more than once.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
(
    IN  GT_U32  lpmDBId,
    IN  GT_U32  pclIdArrayLen,
    IN  GT_U32  pclIdArray[]
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, pclIdArrayLen, pclIdArray));

    rc = internal_cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet(lpmDBId, pclIdArrayLen, pclIdArray);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, pclIdArrayLen, pclIdArray));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
*
* DESCRIPTION:
*   The function gets whether the LPM DB is configured to support default MC
*   rules in policy based routing mode and the rule indexes and PCL ID of those
*   default rules.
*   Refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet for more details.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       lpmDBId                           - the LPM DB id.
*       defaultIpv4RuleIndexArrayLenPtr   - (pointer to) the allocated size of the array
*       defaultIpv6RuleIndexArrayLenPtr   - (pointer to) the allocated size of the array
*       pclIdArrayLenPtr                  - (pointer to) the allocated size of the array
*
* OUTPUTS:
*       defaultMcUsedPtr                  - (pointer to) whether default MC is used for this LPM DB
*       protocolStackPtr                  - (pointer to) protocol stack supported by this LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArray         - rule indexes of the default IPv4 MC (in case
*                                           the LPM DB support IPv4)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArrayLenPtr   - (pointer to) number of elements filled in the array
*       defaultIpv6RuleIndexArray         - rule indexes of the default IPv6 MC (in case
*                                           the LPM DB support IPv6)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv6RuleIndexArrayLenPtr   - (pointer to) number of elements filled in the array
*       pclIdArray                        - array of PCL ID that may be in used by the LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       pclIdArrayLenPtr                  - (pointer to) number of elements filled in the array
*
* RETURNS:
*       GT_OK                             - on success
*       GT_NOT_FOUND                      - if the LPM DB id is not found
*       GT_BAD_PTR                        - if one of the parameters is NULL pointer.
*       GT_FULL                           - if any of the arrays is not big enough
*       GT_FAIL                           - on failure
*       GT_NOT_APPLICABLE_DEVICE          - on not applicable device
*
* COMMENTS:
*       The rule indexes are according to explanations in cpssDxChPclRuleSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
(
    IN      GT_U32                          lpmDBId,
    OUT     GT_BOOL                         *defaultMcUsedPtr,
    OUT     CPSS_IP_PROTOCOL_STACK_ENT      *protocolStackPtr,
    OUT     GT_U32                          defaultIpv4RuleIndexArray[],
    INOUT   GT_U32                          *defaultIpv4RuleIndexArrayLenPtr,
    OUT     GT_U32                          defaultIpv6RuleIndexArray[],
    INOUT   GT_U32                          *defaultIpv6RuleIndexArrayLenPtr,
    OUT     GT_U32                          pclIdArray[],
    INOUT   GT_U32                          *pclIdArrayLenPtr
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC     *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT         lpmHw;

    CPSS_NULL_PTR_CHECK_MAC(defaultMcUsedPtr);
    CPSS_NULL_PTR_CHECK_MAC(protocolStackPtr);
    CPSS_NULL_PTR_CHECK_MAC(defaultIpv4RuleIndexArray);
    CPSS_NULL_PTR_CHECK_MAC(defaultIpv4RuleIndexArrayLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(defaultIpv6RuleIndexArray);
    CPSS_NULL_PTR_CHECK_MAC(defaultIpv6RuleIndexArrayLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(pclIdArray);
    CPSS_NULL_PTR_CHECK_MAC(pclIdArrayLenPtr);

    /* search for the LPM DB */
    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            return prvCpssDxChLpmTcamPolicyBasedRoutingDefaultMcGet(
                (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), defaultMcUsedPtr, protocolStackPtr,
                defaultIpv4RuleIndexArray, defaultIpv4RuleIndexArrayLenPtr,
                defaultIpv6RuleIndexArray, defaultIpv6RuleIndexArrayLenPtr,
                pclIdArray, pclIdArrayLenPtr);

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
*
* DESCRIPTION:
*   The function gets whether the LPM DB is configured to support default MC
*   rules in policy based routing mode and the rule indexes and PCL ID of those
*   default rules.
*   Refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet for more details.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       lpmDBId                           - the LPM DB id.
*       defaultIpv4RuleIndexArrayLenPtr   - (pointer to) the allocated size of the array
*       defaultIpv6RuleIndexArrayLenPtr   - (pointer to) the allocated size of the array
*       pclIdArrayLenPtr                  - (pointer to) the allocated size of the array
*
* OUTPUTS:
*       defaultMcUsedPtr                  - (pointer to) whether default MC is used for this LPM DB
*       protocolStackPtr                  - (pointer to) protocol stack supported by this LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArray         - rule indexes of the default IPv4 MC (in case
*                                           the LPM DB support IPv4)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArrayLenPtr   - (pointer to) number of elements filled in the array
*       defaultIpv6RuleIndexArray         - rule indexes of the default IPv6 MC (in case
*                                           the LPM DB support IPv6)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv6RuleIndexArrayLenPtr   - (pointer to) number of elements filled in the array
*       pclIdArray                        - array of PCL ID that may be in used by the LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       pclIdArrayLenPtr                  - (pointer to) number of elements filled in the array
*
* RETURNS:
*       GT_OK                             - on success
*       GT_NOT_FOUND                      - if the LPM DB id is not found
*       GT_BAD_PTR                        - if one of the parameters is NULL pointer.
*       GT_FULL                           - if any of the arrays is not big enough
*       GT_FAIL                           - on failure
*       GT_NOT_APPLICABLE_DEVICE          - on not applicable device
*
* COMMENTS:
*       The rule indexes are according to explanations in cpssDxChPclRuleSet.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
(
    IN      GT_U32                          lpmDBId,
    OUT     GT_BOOL                         *defaultMcUsedPtr,
    OUT     CPSS_IP_PROTOCOL_STACK_ENT      *protocolStackPtr,
    OUT     GT_U32                          defaultIpv4RuleIndexArray[],
    INOUT   GT_U32                          *defaultIpv4RuleIndexArrayLenPtr,
    OUT     GT_U32                          defaultIpv6RuleIndexArray[],
    INOUT   GT_U32                          *defaultIpv6RuleIndexArrayLenPtr,
    OUT     GT_U32                          pclIdArray[],
    INOUT   GT_U32                          *pclIdArrayLenPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, defaultMcUsedPtr, protocolStackPtr, defaultIpv4RuleIndexArray, defaultIpv4RuleIndexArrayLenPtr, defaultIpv6RuleIndexArray, defaultIpv6RuleIndexArrayLenPtr, pclIdArray, pclIdArrayLenPtr));

    rc = internal_cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet(lpmDBId, defaultMcUsedPtr, protocolStackPtr, defaultIpv4RuleIndexArray, defaultIpv4RuleIndexArrayLenPtr, defaultIpv6RuleIndexArray, defaultIpv6RuleIndexArrayLenPtr, pclIdArray, pclIdArrayLenPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, defaultMcUsedPtr, protocolStackPtr, defaultIpv4RuleIndexArray, defaultIpv4RuleIndexArrayLenPtr, defaultIpv6RuleIndexArray, defaultIpv6RuleIndexArrayLenPtr, pclIdArray, pclIdArrayLenPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*       vrConfigPtr         - (pointer to) Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*       virtual router is used.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmVirtualRouterAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
)
{
    GT_STATUS                               retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC            *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT                lpmHw;
    PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC    tcamVrConfig;
    PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC     ramVrConfig;

    CPSS_NULL_PTR_CHECK_MAC(vrConfigPtr);

    /* at least one of the ipv4/6 uc/mc features must be supported */
    if ((vrConfigPtr->supportIpv4Uc == GT_FALSE) &&
        (vrConfigPtr->supportIpv6Uc == GT_FALSE) &&
        (vrConfigPtr->supportIpv4Mc == GT_FALSE) &&
        (vrConfigPtr->supportIpv6Mc == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
        (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_FAST_BOOT_E))
    {
        return GT_OK;
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* search for the LPM DB */
    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
    case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            prvCpssDxChLpmConvertIpVrConfigToTcamVrConfig(lpmDbPtr->shadowType,
                                                          vrConfigPtr, &tcamVrConfig);
            retVal = prvCpssDxChLpmTcamVirtualRouterAdd((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                        vrId, &tcamVrConfig);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            prvCpssDxChLpmConvertIpVrConfigToRamVrConfig(vrConfigPtr, &ramVrConfig);
            retVal = prvCpssDxChLpmRamVirtualRouterAdd((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                        vrId, &ramVrConfig);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*       vrConfigPtr         - (pointer to) Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*       virtual router is used.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmVirtualRouterAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, vrConfigPtr));

    rc = internal_cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, vrConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, vrConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmVirtualRouterSharedAdd
*
* DESCRIPTION:
*   This function adds a shared virtual router in system for specific LPM DB.
*   Prefixes that reside within shared virtual router will participate in the
*   lookups of all virtual routers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*       vrConfigPtr         - (pointer to) Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       1.  Only one shared virtual router can exists at any time within a
*           given LPM DB.
*       2.  Virtual router ID that is used for non-shared virtual router can't
*           be used for the shared virtual router and via versa.
*       3.  Shared prefixes can't overlap non-shared prefixes. If the shared
*           virtual router supports shared prefixes type then adding non-shared
*           virtual router that supports the same prefixes type will fail.
*           Also, if a non-shared virtual router that supports prefixes type
*           exists, then adding a shared virtual router that supports the same
*           prefixes type will fail.
*       4.  When the shared virtual router supports IPv4 UC prefixes, then the
*           the following will apply:
*             - The <match all> default TCAM entry will not be written to TCAM
*               (however TCAM entry will still be allocated to this entry)
*             - The following prefixes will be added to match all non-MC traffic:
*               0x00/1, 0x80/2, 0xC0/3, 0xF0/4
*           Same applies when the shared virtual router supports IPv6 UC
*           prefixes. The prefixes added to match all non-MC traffic are:
*           0x00/1, 0x80/2, 0xC0/3, 0xE0/4, 0xF0/5, 0xF8/6, 0xFC/7, 0xFE/8
*       5.  The application should not delete the non-MC prefixes (when added).
*       6.  Manipulation of the default UC will not be applied on the non-MC
*           prefixes. The application should manipulate those non-MC entries
*           directly when needed.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmVirtualRouterSharedAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC            *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT                lpmHw;
    PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC    tcamVrConfig;

    CPSS_NULL_PTR_CHECK_MAC(vrConfigPtr);

    /* at least one of the ipv4/6 uc/mc features must be supported */
    if ((vrConfigPtr->supportIpv4Uc == GT_FALSE) &&
        (vrConfigPtr->supportIpv6Uc == GT_FALSE) &&
        (vrConfigPtr->supportIpv4Mc == GT_FALSE) &&
        (vrConfigPtr->supportIpv6Mc == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* search for the LPM DB */
    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            prvCpssDxChLpmConvertIpVrConfigToTcamVrConfig(lpmDbPtr->shadowType,
                                                          vrConfigPtr, &tcamVrConfig);
            return prvCpssDxChLpmTcamVirtualRouterSharedAdd((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                            vrId, &tcamVrConfig);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterSharedAdd
*
* DESCRIPTION:
*   This function adds a shared virtual router in system for specific LPM DB.
*   Prefixes that reside within shared virtual router will participate in the
*   lookups of all virtual routers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*       vrConfigPtr         - (pointer to) Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       1.  Only one shared virtual router can exists at any time within a
*           given LPM DB.
*       2.  Virtual router ID that is used for non-shared virtual router can't
*           be used for the shared virtual router and via versa.
*       3.  Shared prefixes can't overlap non-shared prefixes. If the shared
*           virtual router supports shared prefixes type then adding non-shared
*           virtual router that supports the same prefixes type will fail.
*           Also, if a non-shared virtual router that supports prefixes type
*           exists, then adding a shared virtual router that supports the same
*           prefixes type will fail.
*       4.  When the shared virtual router supports IPv4 UC prefixes, then the
*           the following will apply:
*             - The <match all> default TCAM entry will not be written to TCAM
*               (however TCAM entry will still be allocated to this entry)
*             - The following prefixes will be added to match all non-MC traffic:
*               0x00/1, 0x80/2, 0xC0/3, 0xF0/4
*           Same applies when the shared virtual router supports IPv6 UC
*           prefixes. The prefixes added to match all non-MC traffic are:
*           0x00/1, 0x80/2, 0xC0/3, 0xE0/4, 0xF0/5, 0xF8/6, 0xFC/7, 0xFE/8
*       5.  The application should not delete the non-MC prefixes (when added).
*       6.  Manipulation of the default UC will not be applied on the non-MC
*           prefixes. The application should manipulate those non-MC entries
*           directly when needed.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterSharedAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmVirtualRouterSharedAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, vrConfigPtr));

    rc = internal_cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, vrConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, vrConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       vrConfigPtr         - (pointer to) Virtual router configuration.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmVirtualRouterGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    OUT CPSS_DXCH_IP_LPM_VR_CONFIG_STC          *vrConfigPtr
)
{
    GT_STATUS                               retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC            *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT                lpmHw;
    PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC    tcamVrConfig;
    PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC     ramVrConfig;

    cpssOsMemSet(&tcamVrConfig, 0, sizeof(tcamVrConfig));
    cpssOsMemSet(&ramVrConfig, 0, sizeof(ramVrConfig));

    CPSS_NULL_PTR_CHECK_MAC(vrConfigPtr);

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamVirtualRouterGet((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                        vrId, &tcamVrConfig);
            prvCpssDxChLpmConvertTcamVrConfigToIpVrConfig(lpmDbPtr->shadowType,
                                                          &tcamVrConfig, vrConfigPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamVirtualRouterGet((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                        vrId, &ramVrConfig);
            prvCpssDxChLpmConvertRamVrConfigToIpVrConfig(&ramVrConfig, vrConfigPtr);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       vrConfigPtr         - (pointer to) Virtual router configuration.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    OUT CPSS_DXCH_IP_LPM_VR_CONFIG_STC          *vrConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmVirtualRouterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, vrConfigPtr));

    rc = internal_cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, vrConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, vrConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmVirtualRouterDel
*
* DESCRIPTION:
*   This function removes a virtual router in system for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmVirtualRouterDel
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamVirtualRouterDel((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), vrId);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamVirtualRouterDel((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow), vrId);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterDel
*
* DESCRIPTION:
*   This function removes a virtual router in system for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterDel
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmVirtualRouterDel);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId));

    rc = internal_cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - (pointer to) the route entry info accosiated with this
*                         UC prefix.
*       override        - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT routeEntryInfo;

    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntryInfo,0,sizeof(routeEntryInfo));

    if (prefixLen > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if ((ipAddr.arIP[0] >= 224) && (ipAddr.arIP[0] <= 239))
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertIpRouteEntryToLpmRouteEntry(lpmDbPtr->shadowType,
                                                     nextHopInfoPtr,
                                                     &routeEntryInfo);
    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4UcPrefixAdd((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                       vrId, ipAddr, prefixLen,
                                                       &routeEntryInfo, override);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4UcPrefixAdd((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                       vrId, ipAddr, prefixLen,
                                                       &(routeEntryInfo.routeEntry), override);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - (pointer to) the route entry info accosiated with this
*                         UC prefix.
*       override        - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4UcPrefixAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, override));

    rc = internal_cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, override);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, override));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv4PrefixArrayLen - length of UC prefix array
*       ipv4PrefixArrayPtr - (pointer to) the UC prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of prefixes' lengths is too big
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    CPSS_NULL_PTR_CHECK_MAC(ipv4PrefixArrayPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4UcPrefixBulkAdd((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                           ipv4PrefixArrayLen,
                                                           ipv4PrefixArrayPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4UcPrefixBulkAdd((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                           ipv4PrefixArrayLen,
                                                           ipv4PrefixArrayPtr);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv4PrefixArrayLen - length of UC prefix array
*       ipv4PrefixArrayPtr - (pointer to) the UC prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of prefixes' lengths is too big
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4UcPrefixAddBulk);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, ipv4PrefixArrayLen, ipv4PrefixArrayPtr));

    rc = internal_cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, ipv4PrefixArrayLen, ipv4PrefixArrayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, ipv4PrefixArrayLen, ipv4PrefixArrayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH - If the given prefix doesn't exist in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPADDR                            ipAddr,
    IN GT_U32                               prefixLen
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if (prefixLen > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((ipAddr.arIP[0] >= 224) && (ipAddr.arIP[0] <= 239))
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4UcPrefixDel((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                       vrId, ipAddr, prefixLen);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4UcPrefixDel((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                       vrId, ipAddr, prefixLen);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH - If the given prefix doesn't exist in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPADDR                            ipAddr,
    IN GT_U32                               prefixLen
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4UcPrefixDel);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddr, prefixLen));

    rc = internal_cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddr, prefixLen));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv4PrefixArrayLen - length of UC prefix array
*       ipv4PrefixArrayPtr - (pointer to) the UC prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of the prefixes' lengths is too big
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if(ipv4PrefixArrayLen)
    {
        CPSS_NULL_PTR_CHECK_MAC(ipv4PrefixArrayPtr);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4UcPrefixBulkDel((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                           ipv4PrefixArrayLen,
                                                           ipv4PrefixArrayPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4UcPrefixBulkDel((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                           ipv4PrefixArrayLen,
                                                           ipv4PrefixArrayPtr);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv4PrefixArrayLen - length of UC prefix array
*       ipv4PrefixArrayPtr - (pointer to) the UC prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of the prefixes' lengths is too big
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4UcPrefixDelBulk);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, ipv4PrefixArrayLen, ipv4PrefixArrayPtr));

    rc = internal_cpssDxChIpLpmIpv4UcPrefixDelBulk(lpmDBId, ipv4PrefixArrayLen, ipv4PrefixArrayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, ipv4PrefixArrayLen, ipv4PrefixArrayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4UcPrefixesFlush((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                           vrId);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4UcPrefixesFlush((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                           vrId);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4UcPrefixesFlush);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId));

    rc = internal_cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId, vrId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr       - (pointer to) the route entry info associated with
*                              this UC prefix, if found.
*       tcamRowIndexPtr      - (pointer to) the TCAM row index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - (pointer to) the TCAM column index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT routeEntryInfo;

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntryInfo,0,sizeof(routeEntryInfo));

    if (prefixLen > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if ((ipAddr.arIP[0] >= 224) && (ipAddr.arIP[0] <= 239))
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4UcPrefixGet((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                       vrId, ipAddr, prefixLen,
                                                       &routeEntryInfo,
                                                       tcamRowIndexPtr,
                                                       tcamColumnIndexPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4UcPrefixSearch((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                         vrId, ipAddr, prefixLen,
                                                         &(routeEntryInfo.routeEntry));
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry(lpmDbPtr->shadowType,
                                                     &routeEntryInfo,
                                                     nextHopInfoPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr       - (pointer to) the route entry info associated with
*                              this UC prefix, if found.
*       tcamRowIndexPtr      - (pointer to) the TCAM row index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - (pointer to) the TCAM column index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4UcPrefixSearch);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));

    rc = internal_cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - (pointer to) the ip address of the found entry
*       prefixLenPtr         - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr       - (pointer to) the route entry info associated
*                              with this UC prefix, if found.
*       tcamRowIndexPtr      - (pointer to) the TCAM row index of this uc prefix
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - (pointer to) the TCAM column index of this uc
*                              prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *tcamRowIndexPtr,
    OUT GT_U32                                      *tcamColumnIndexPtr

)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT routeEntryInfo;

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntryInfo,0,sizeof(routeEntryInfo));

    CPSS_NULL_PTR_CHECK_MAC(ipAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    if (*prefixLenPtr > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if ((ipAddrPtr->arIP[0] >= 224) && (ipAddrPtr->arIP[0] <= 239))
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4UcPrefixGetNext((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                           vrId, ipAddrPtr,
                                                           prefixLenPtr,
                                                           &routeEntryInfo,
                                                           tcamRowIndexPtr,
                                                           tcamColumnIndexPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4UcPrefixGetNext((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                           vrId, ipAddrPtr,
                                                           prefixLenPtr,
                                                           &(routeEntryInfo.routeEntry));
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry(lpmDbPtr->shadowType,
                                                     &routeEntryInfo,
                                                     nextHopInfoPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - (pointer to) the ip address of the found entry
*       prefixLenPtr         - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr       - (pointer to) the route entry info associated
*                              with this UC prefix, if found.
*       tcamRowIndexPtr      - (pointer to) the TCAM row index of this uc prefix
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - (pointer to) the TCAM column index of this uc
*                              prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *tcamRowIndexPtr,
    OUT GT_U32                                      *tcamColumnIndexPtr

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4UcPrefixGetNext);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddrPtr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));

    rc = internal_cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, ipAddrPtr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddrPtr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4UcPrefixGet
*
* DESCRIPTION:
*       This function gets a given ip address, find LPM match in the trie and
*       returns the prefix length and pointer to the next hop information bound
*       to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr         - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr       - (pointer to) the route entry info associated with
*                              this UC prefix, if found.
*       tcamRowIndexPtr      - (pointer to) the TCAM row index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - (pointer to) the TCAM column index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4UcPrefixGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    OUT GT_U32                                  *prefixLenPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT routeEntryInfo;

     /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(prefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntryInfo,0,sizeof(routeEntryInfo));

    if ((ipAddr.arIP[0] >= 224) && (ipAddr.arIP[0] <= 239))
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4UcPrefixGet((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                         vrId, ipAddr, prefixLenPtr,
                                                         &(routeEntryInfo.routeEntry));
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry(lpmDbPtr->shadowType,
                                                     &routeEntryInfo,
                                                     nextHopInfoPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGet
*
* DESCRIPTION:
*       This function gets a given ip address, find LPM match in the trie and
*       returns the prefix length and pointer to the next hop information bound
*       to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr         - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr       - (pointer to) the route entry info associated with
*                              this UC prefix, if found.
*       tcamRowIndexPtr      - (pointer to) the TCAM row index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - (pointer to) the TCAM column index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    OUT GT_U32                                  *prefixLenPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4UcPrefixGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));

    rc = internal_cpssDxChIpLpmIpv4UcPrefixGet(lpmDBId, vrId, ipAddr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP multicast route for a particular/all source and a particular
*   group address. this is done for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual private network identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup             - The IP multicast group address.
*       ipGroupPrefixLen    - The number of bits that are actual valid in,
*                             the ipGroup.
*       ipSrc               - the root address for source base multi tree
*                             protocol.
*       ipSrcPrefixLen      - The number of bits that are actual valid in,
*                             the ipSrc.
*       mcRouteLttEntryPtr  - (pointer to) the LTT entry pointing to the MC
*                             route entry associated with this MC route.
*       override            - whether to override an mc Route pointer for the
*                             given prefix
*       tcamDefragmentationEnable - whether to enable performance costing
*                             de-fragmentation process in the case that there
*                             is no place to insert the prefix. To point of the
*                             process is just to make space for this prefix.
*                             relevant only if the LPM DB was created with
*                             partitionEnable = GT_FALSE.
*                             (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                              DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - if one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPADDR                    ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPADDR                    ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      tcamDefragmentationEnable
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC routeEntry;

    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntry,0,sizeof(routeEntry));

    if (ipGroupPrefixLen > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipSrcPrefixLen > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* IPv4 group prefix must be in the range 224/4 - 239/4
       therefore the first octate must start with 0xE               */
    if((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] & 0xF0) != 0xE0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(lpmHw, mcRouteLttEntryPtr, &routeEntry);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4McEntryAdd((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                      vrId, ipGroup,
                                                      ipGroupPrefixLen, ipSrc,
                                                      ipSrcPrefixLen, &routeEntry,
                                                      override,
                                                      tcamDefragmentationEnable);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4McEntryAdd((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                      vrId, ipGroup,
                                                      ipGroupPrefixLen, ipSrc,
                                                      ipSrcPrefixLen, &routeEntry,
                                                      override);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP multicast route for a particular/all source and a particular
*   group address. this is done for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual private network identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup             - The IP multicast group address.
*       ipGroupPrefixLen    - The number of bits that are actual valid in,
*                             the ipGroup.
*       ipSrc               - the root address for source base multi tree
*                             protocol.
*       ipSrcPrefixLen      - The number of bits that are actual valid in,
*                             the ipSrc.
*       mcRouteLttEntryPtr  - (pointer to) the LTT entry pointing to the MC
*                             route entry associated with this MC route.
*       override            - whether to override an mc Route pointer for the
*                             given prefix
*       tcamDefragmentationEnable - whether to enable performance costing
*                             de-fragmentation process in the case that there
*                             is no place to insert the prefix. To point of the
*                             process is just to make space for this prefix.
*                             relevant only if the LPM DB was created with
*                             partitionEnable = GT_FALSE.
*                             (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                              DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - if one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPADDR                    ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPADDR                    ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      tcamDefragmentationEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4McEntryAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, override, tcamDefragmentationEnable));

    rc = internal_cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, override, tcamDefragmentationEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, override, tcamDefragmentationEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPADDR    ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPADDR    ipSrc,
    IN GT_U32       ipSrcPrefixLen
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if (ipGroupPrefixLen > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipSrcPrefixLen > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* IPv4 group prefix must be in the range 224/4 - 239/4
       therefore the first octate must start with 0xE               */
    if((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] & 0xF0) != 0xE0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4McEntryDel((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), vrId,
                                                      ipGroup, ipGroupPrefixLen,
                                                      ipSrc, ipSrcPrefixLen);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4McEntryDel((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow), vrId,
                                                      ipGroup, ipGroupPrefixLen,
                                                      ipSrc, ipSrcPrefixLen);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPADDR    ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPADDR    ipSrc,
    IN GT_U32       ipSrcPrefixLen
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4McEntryDel);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen));

    rc = internal_cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4McEntriesFlush((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                          vrId);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4McEntriesFlush((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                          vrId);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4McEntriesFlush);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId));

    rc = internal_cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - the virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - (pointer to) the ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - (pointer to) the ipGroup prefix Length.
*       ipSrcPtr            - (pointer to) the ip Source address to get the next
*                             entry for.
*       ipSrcPrefixLenPtr   - (pointer to) ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr                 - (pointer to) the next ip Group address
*       ipGroupPrefixLenPtr        - (pointer to) the ipGroup prefix Length
*       ipSrcPtr                   - (pointer to) the next ip Source address
*       ipSrcPrefixLenPtr          - (pointer to) ipSrc prefix length
*       mcRouteLttEntryPtr         - (pointer to) the LTT entry pointer pointing
*                                    to the MC route.
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - (pointer to) TCAM group row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamGroupColumnIndexPtr    - (pointer to) TCAM group column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr         - (pointer to) TCAM source row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcColumnIndexPtr      - (pointer to) TCAM source column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The value of ipGroupPtr must be a valid value, it
*       means that it exists in the IP-Mc Table, unless this is the first
*       call to this function, then it's value is 0.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4McEntryGetNext
(
    IN    GT_U32                        lpmDBId,
    IN    GT_U32                        vrId,
    INOUT GT_IPADDR                     *ipGroupPtr,
    INOUT GT_U32                        *ipGroupPrefixLenPtr,
    INOUT GT_IPADDR                     *ipSrcPtr,
    INOUT GT_U32                        *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    OUT GT_U32                          *tcamGroupRowIndexPtr,
    OUT GT_U32                          *tcamGroupColumnIndexPtr,
    OUT GT_U32                          *tcamSrcRowIndexPtr,
    OUT GT_U32                          *tcamSrcColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC routeEntry;

    CPSS_NULL_PTR_CHECK_MAC(ipGroupPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipGroupPrefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipSrcPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipSrcPrefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupColumnIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcColumnIndexPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntry,0,sizeof(routeEntry));

    if (*ipGroupPrefixLenPtr > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (*ipSrcPrefixLenPtr > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* IPv4 group prefix must be in the range 224/4 - 239/4
       therefore the first octate must start with 0xE               */
    if((*ipGroupPrefixLenPtr!=0) && (ipGroupPtr->arIP[0] & 0xF0) != 0xE0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    else
        if (*ipGroupPrefixLenPtr == 0)
        {
            ipGroupPtr->arIP[0] = 0xE0;
            *ipGroupPrefixLenPtr=4;
        }


    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4McEntryGetNext((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                          vrId,
                                                          ipGroupPtr,
                                                          ipGroupPrefixLenPtr,
                                                          ipSrcPtr,
                                                          ipSrcPrefixLenPtr,
                                                          &routeEntry,
                                                          tcamGroupRowIndexPtr,
                                                          tcamGroupColumnIndexPtr,
                                                          tcamSrcRowIndexPtr,
                                                          tcamSrcColumnIndexPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4McEntryGetNext((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                          vrId,
                                                          ipGroupPtr,
                                                          ipGroupPrefixLenPtr,
                                                          ipSrcPtr,
                                                          ipSrcPrefixLenPtr,
                                                          &routeEntry);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(lpmHw, &routeEntry, mcRouteLttEntryPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - the virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - (pointer to) the ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - (pointer to) the ipGroup prefix Length.
*       ipSrcPtr            - (pointer to) the ip Source address to get the next
*                             entry for.
*       ipSrcPrefixLenPtr   - (pointer to) ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr                 - (pointer to) the next ip Group address
*       ipGroupPrefixLenPtr        - (pointer to) the ipGroup prefix Length
*       ipSrcPtr                   - (pointer to) the next ip Source address
*       ipSrcPrefixLenPtr          - (pointer to) ipSrc prefix length
*       mcRouteLttEntryPtr         - (pointer to) the LTT entry pointer pointing
*                                    to the MC route.
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - (pointer to) TCAM group row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamGroupColumnIndexPtr    - (pointer to) TCAM group column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr         - (pointer to) TCAM source row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcColumnIndexPtr      - (pointer to) TCAM source column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The value of ipGroupPtr must be a valid value, it
*       means that it exists in the IP-Mc Table, unless this is the first
*       call to this function, then it's value is 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryGetNext
(
    IN    GT_U32                        lpmDBId,
    IN    GT_U32                        vrId,
    INOUT GT_IPADDR                     *ipGroupPtr,
    INOUT GT_U32                        *ipGroupPrefixLenPtr,
    INOUT GT_IPADDR                     *ipSrcPtr,
    INOUT GT_U32                        *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    OUT GT_U32                          *tcamGroupRowIndexPtr,
    OUT GT_U32                          *tcamGroupColumnIndexPtr,
    OUT GT_U32                          *tcamSrcRowIndexPtr,
    OUT GT_U32                          *tcamSrcColumnIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4McEntryGetNext);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipGroupPtr, ipGroupPrefixLenPtr, ipSrcPtr, ipSrcPrefixLenPtr, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamGroupColumnIndexPtr, tcamSrcRowIndexPtr, tcamSrcColumnIndexPtr));

    rc = internal_cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, ipGroupPtr, ipGroupPrefixLenPtr, ipSrcPtr, ipSrcPrefixLenPtr, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamGroupColumnIndexPtr, tcamSrcRowIndexPtr, tcamSrcColumnIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipGroupPtr, ipGroupPrefixLenPtr, ipSrcPtr, ipSrcPrefixLenPtr, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamGroupColumnIndexPtr, tcamSrcRowIndexPtr, tcamSrcColumnIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChIpLpmIpv4McEntrySearch
*
* DESCRIPTION:
*   This function returns the multicast (ipSrc, ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc + ipGroup
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr         - (pointer to) the LTT entry pointer pointing
*                                    to the MC route entry associated with this
*                                    MC route.
*       tcamGroupRowIndexPtr       - (pointer to) TCAM group row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamGroupColumnIndexPtr    - (pointer to) TCAM group column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr         - (pointer to) TCAM source row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcColumnIndexPtr      - (pointer to) TCAM source column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv4McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPADDR                   ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPADDR                   ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamGroupColumnIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr,
    OUT GT_U32                      *tcamSrcColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC routeEntry;

    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupColumnIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcColumnIndexPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntry,0,sizeof(routeEntry));

    if (ipGroupPrefixLen > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipSrcPrefixLen > 32)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* IPv4 group prefix must be in the range 224/4 - 239/4
       therefore the first octate must start with 0xE               */
    if((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] & 0xF0) != 0xE0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv4McEntryGet((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                      vrId,
                                                      ipGroup,
                                                      ipGroupPrefixLen,
                                                      ipSrc,
                                                      ipSrcPrefixLen,
                                                      &routeEntry,
                                                      tcamGroupRowIndexPtr,
                                                      tcamGroupColumnIndexPtr,
                                                      tcamSrcRowIndexPtr,
                                                      tcamSrcColumnIndexPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv4McEntryGet((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                      vrId,
                                                      ipGroup,
                                                      ipGroupPrefixLen,
                                                      ipSrc,
                                                      ipSrcPrefixLen,
                                                      &routeEntry);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(lpmHw, &routeEntry, mcRouteLttEntryPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntrySearch
*
* DESCRIPTION:
*   This function returns the multicast (ipSrc, ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc + ipGroup
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr         - (pointer to) the LTT entry pointer pointing
*                                    to the MC route entry associated with this
*                                    MC route.
*       tcamGroupRowIndexPtr       - (pointer to) TCAM group row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamGroupColumnIndexPtr    - (pointer to) TCAM group column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr         - (pointer to) TCAM source row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcColumnIndexPtr      - (pointer to) TCAM source column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPADDR                   ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPADDR                   ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamGroupColumnIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr,
    OUT GT_U32                      *tcamSrcColumnIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv4McEntrySearch);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamGroupColumnIndexPtr, tcamSrcRowIndexPtr, tcamSrcColumnIndexPtr));

    rc = internal_cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamGroupColumnIndexPtr, tcamSrcRowIndexPtr, tcamSrcColumnIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamGroupColumnIndexPtr, tcamSrcRowIndexPtr, tcamSrcColumnIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix in a Virtual Router
*   for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - (pointer to) the route entry info accosiated with this
*                         UC prefix.
*       override        - override the existing entry if it already exists
*       tcamDefragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If prefix length is too big.
*       GT_ERROR                 - If the vrId was not created yet.
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPV6ADDR                              ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override,
    IN GT_BOOL                                  tcamDefragmentationEnable
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT routeEntryInfo;

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntryInfo,0,sizeof(routeEntryInfo));

    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);

    if (prefixLen > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipAddr.arIP[0] == 255)
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertIpRouteEntryToLpmRouteEntry(lpmDbPtr->shadowType,
                                                     nextHopInfoPtr,
                                                     &routeEntryInfo);
    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6UcPrefixAdd((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                       vrId,
                                                       ipAddr,
                                                       prefixLen,
                                                       &routeEntryInfo,
                                                       override,
                                                       tcamDefragmentationEnable);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6UcPrefixAdd((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                       vrId,
                                                       ipAddr,
                                                       prefixLen,
                                                       &(routeEntryInfo.routeEntry),
                                                       override);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix in a Virtual Router
*   for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - (pointer to) the route entry info accosiated with this
*                         UC prefix.
*       override        - override the existing entry if it already exists
*       tcamDefragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If prefix length is too big.
*       GT_ERROR                 - If the vrId was not created yet.
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPV6ADDR                              ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override,
    IN GT_BOOL                                  tcamDefragmentationEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6UcPrefixAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, override, tcamDefragmentationEnable));

    rc = internal_cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, override, tcamDefragmentationEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, override, tcamDefragmentationEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv6PrefixArrayLen - length of UC prefix array
*       ipv6PrefixArrayPtr - (pointer to) the UC prefix array
*       tcamDefragmentationEnable - whether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr,
    IN GT_BOOL                              tcamDefragmentationEnable
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    CPSS_NULL_PTR_CHECK_MAC(ipv6PrefixArrayPtr);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6UcPrefixBulkAdd((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                           ipv6PrefixArrayLen,
                                                           ipv6PrefixArrayPtr,
                                                           tcamDefragmentationEnable);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6UcPrefixBulkAdd((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                           ipv6PrefixArrayLen,
                                                           ipv6PrefixArrayPtr);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv6PrefixArrayLen - length of UC prefix array
*       ipv6PrefixArrayPtr - (pointer to) the UC prefix array
*       tcamDefragmentationEnable - whether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr,
    IN GT_BOOL                              tcamDefragmentationEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6UcPrefixAddBulk);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, ipv6PrefixArrayLen, ipv6PrefixArrayPtr, tcamDefragmentationEnable));

    rc = internal_cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, ipv6PrefixArrayLen, ipv6PrefixArrayPtr, tcamDefragmentationEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, ipv6PrefixArrayLen, ipv6PrefixArrayPtr, tcamDefragmentationEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exist in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipAddr,
    IN GT_U32                               prefixLen
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if (prefixLen > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipAddr.arIP[0] == 255)
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6UcPrefixDel((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                       vrId, ipAddr, prefixLen);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6UcPrefixDel((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                       vrId, ipAddr, prefixLen);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exist in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipAddr,
    IN GT_U32                               prefixLen
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6UcPrefixDel);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddr, prefixLen));

    rc = internal_cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddr, prefixLen));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv6 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv6PrefixArrayLen - length of UC prefix array
*       ipv6PrefixArrayPtr - (pointer to) the UC prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big, or
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if(ipv6PrefixArrayLen)
    {
        CPSS_NULL_PTR_CHECK_MAC(ipv6PrefixArrayPtr);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6UcPrefixBulkDel((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                           ipv6PrefixArrayLen,
                                                           ipv6PrefixArrayPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6UcPrefixBulkDel((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                           ipv6PrefixArrayLen,
                                                           ipv6PrefixArrayPtr);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv6 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv6PrefixArrayLen - length of UC prefix array
*       ipv6PrefixArrayPtr - (pointer to) the UC prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big, or
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6UcPrefixDelBulk);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, ipv6PrefixArrayLen, ipv6PrefixArrayPtr));

    rc = internal_cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, ipv6PrefixArrayLen, ipv6PrefixArrayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, ipv6PrefixArrayLen, ipv6PrefixArrayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6UcPrefixesFlush((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), vrId);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6UcPrefixesFlush((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow), vrId);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6UcPrefixesFlush);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId));

    rc = internal_cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId, vrId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - (pointer to) the route entry info associated with
*                         this UC prefix, if found.
*       tcamRowIndexPtr - (pointer to) the TCAM row index of this uc prefix, if
*                         found
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - (pointer to) the TCAM column index of this uc
*                            prefix, if found.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT routeEntryInfo;

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntryInfo,0,sizeof(routeEntryInfo));

    if (prefixLen > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipAddr.arIP[0] == 255)
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6UcPrefixGet((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                       vrId, ipAddr, prefixLen,
                                                       &routeEntryInfo,
                                                       tcamRowIndexPtr,
                                                       tcamColumnIndexPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6UcPrefixSearch((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                         vrId, ipAddr, prefixLen,
                                                         &(routeEntryInfo.routeEntry));
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry(lpmDbPtr->shadowType,
                                                     &routeEntryInfo,
                                                     nextHopInfoPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - (pointer to) the route entry info associated with
*                         this UC prefix, if found.
*       tcamRowIndexPtr - (pointer to) the TCAM row index of this uc prefix, if
*                         found
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - (pointer to) the TCAM column index of this uc
*                            prefix, if found.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6UcPrefixSearch);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));

    rc = internal_cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddr, prefixLen, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - (pointer to) the ip address of the found entry
*       prefixLenPtr    - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr  - (pointer to) the route entry info associated with
*                         this UC prefix, if found.
*       tcamRowIndexPtr - (pointer to) the TCAM row index of this uc prefix. if
*                         found.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - (pointer to) the TCAM column index of this uc
*                            prefix, if found.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT   GT_U32                                    *tcamRowIndexPtr,
    OUT   GT_U32                                    *tcamColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT routeEntryInfo;

    CPSS_NULL_PTR_CHECK_MAC(ipAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntryInfo,0,sizeof(routeEntryInfo));

    if (*prefixLenPtr > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipAddrPtr->arIP[0] == 255)
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6UcPrefixGetNext((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                           vrId,
                                                           ipAddrPtr,
                                                           prefixLenPtr,
                                                           &routeEntryInfo,
                                                           tcamRowIndexPtr,
                                                           tcamColumnIndexPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6UcPrefixGetNext((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                           vrId,
                                                           ipAddrPtr,
                                                           prefixLenPtr,
                                                           &(routeEntryInfo.routeEntry));
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry(lpmDbPtr->shadowType,
                                                     &routeEntryInfo,
                                                     nextHopInfoPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - (pointer to) the ip address of the found entry
*       prefixLenPtr    - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr  - (pointer to) the route entry info associated with
*                         this UC prefix, if found.
*       tcamRowIndexPtr - (pointer to) the TCAM row index of this uc prefix. if
*                         found.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - (pointer to) the TCAM column index of this uc
*                            prefix, if found.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT   GT_U32                                    *tcamRowIndexPtr,
    OUT   GT_U32                                    *tcamColumnIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6UcPrefixGetNext);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddrPtr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));

    rc = internal_cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, ipAddrPtr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddrPtr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6UcPrefixGet
*
* DESCRIPTION:
*       This function gets a given ip address, find LPM match in the trie and
*       returns the prefix length and pointer to the next hop information bound
*       to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr    - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr  - (pointer to) the route entry info associated with
*                         this UC prefix, if found
*       tcamRowIndexPtr - (pointer to) the TCAM row index of this uc prefix, if
*                         found
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - (pointer to) the TCAM column index of this uc
*                            prefix, if found
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6UcPrefixGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    OUT GT_U32                                  *prefixLenPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT routeEntryInfo;

         /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(prefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntryInfo,0,sizeof(routeEntryInfo));

    if (ipAddr.arIP[0] == 255)
    {
        /* Multicast range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6UcPrefixGet((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                         vrId, ipAddr, prefixLenPtr,
                                                         &(routeEntryInfo.routeEntry));
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry(lpmDbPtr->shadowType,
                                                     &routeEntryInfo,
                                                     nextHopInfoPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGet
*
* DESCRIPTION:
*       This function gets a given ip address, find LPM match in the trie and
*       returns the prefix length and pointer to the next hop information bound
*       to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr    - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr  - (pointer to) the route entry info associated with
*                         this UC prefix, if found
*       tcamRowIndexPtr - (pointer to) the TCAM row index of this uc prefix, if
*                         found
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - (pointer to) the TCAM column index of this uc
*                            prefix, if found
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    OUT GT_U32                                  *prefixLenPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6UcPrefixGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipAddr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));

    rc = internal_cpssDxChIpLpmIpv6UcPrefixGet(lpmDBId, vrId, ipAddr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipAddr, prefixLenPtr, nextHopInfoPtr, tcamRowIndexPtr, tcamColumnIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - (pointer to) the LTT entry pointing to the MC route
*                            entry associated with this MC route.
*       override        - whether to override the mcRoutePointerPtr for the
*                         given prefix
*       tcamDefragmentationEnable - whether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented .
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPV6ADDR                  ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPV6ADDR                  ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      tcamDefragmentationEnable
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC routeEntry;

    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntry,0,sizeof(routeEntry));

    if (ipGroupPrefixLen > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipSrcPrefixLen > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* IPv6 multicast group prefix address format ff00::/8 */
    if ((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] != 0xFF))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry(lpmHw, mcRouteLttEntryPtr, &routeEntry);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6McEntryAdd((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                      vrId,
                                                      ipGroup,
                                                      ipGroupPrefixLen,
                                                      ipSrc,
                                                      ipSrcPrefixLen,
                                                      &routeEntry,
                                                      override,
                                                      tcamDefragmentationEnable);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6McEntryAdd((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                      vrId,
                                                      ipGroup,
                                                      ipGroupPrefixLen,
                                                      ipSrc,
                                                      ipSrcPrefixLen,
                                                      &routeEntry,
                                                      override);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - (pointer to) the LTT entry pointing to the MC route
*                            entry associated with this MC route.
*       override        - whether to override the mcRoutePointerPtr for the
*                         given prefix
*       tcamDefragmentationEnable - whether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented .
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPV6ADDR                  ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPV6ADDR                  ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      tcamDefragmentationEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6McEntryAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, override, tcamDefragmentationEnable));

    rc = internal_cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, override, tcamDefragmentationEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, override, tcamDefragmentationEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPV6ADDR  ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPV6ADDR  ipSrc,
    IN GT_U32       ipSrcPrefixLen
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if (ipGroupPrefixLen > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipSrcPrefixLen > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* IPv6 multicast group prefix address format ff00::/8 */
    if ((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] != 0xFF))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6McEntryDel((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                      vrId, ipGroup,
                                                      ipGroupPrefixLen,
                                                      ipSrc, ipSrcPrefixLen);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6McEntryDel((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                      vrId, ipGroup,
                                                      ipGroupPrefixLen,
                                                      ipSrc, ipSrcPrefixLen);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPV6ADDR  ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPV6ADDR  ipSrc,
    IN GT_U32       ipSrcPrefixLen
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6McEntryDel);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen));

    rc = internal_cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6McEntriesFlush((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), vrId);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6McEntriesFlush((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow), vrId);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6McEntriesFlush);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId));

    rc = internal_cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - the LPM DB id
*       vrId                - the virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - (pointer to) the ip Group address to get the next
*                             entry for
*       ipGroupPrefixLenPtr - (pointer to) the ipGroup prefix length
*       ipSrcPtr            - (pointer to) the ip Source address to get the next entry for
*       ipSrcPrefixLenPtr   - (pointer to) the ipSrc prefix length
*
* OUTPUTS:
*       ipGroupPtr           - (pointer to) the next ip Group address.
*       ipGroupPrefixLenPtr  - (pointer to) ipGroup prefix length.
*       ipSrcPtr             - (pointer to) the next ip Source address.
*       ipSrcPrefixLenPtr    - (pointer to) ipSrc prefix length.
*       mcRouteLttEntryPtr   - (pointer to) the LTT entry pointer pointing to
*                              the MC route entry associated with this MC route
*       tcamGroupRowIndexPtr - (pointer to) TCAM group row  index
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr   - (pointer to) TCAM source row  index.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The values of (ipGroupPtr,ipGroupPrefixLenPtr) must be a valid
*       values, it means that they exist in the IP-Mc Table, unless this is
*       the first call to this function, then the value of (ipGroupPtr,
*       ipSrcPtr) is (0,0).
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6McEntryGetNext
(
    IN    GT_U32                      lpmDBId,
    IN    GT_U32                      vrId,
    INOUT GT_IPV6ADDR                 *ipGroupPtr,
    INOUT GT_U32                      *ipGroupPrefixLenPtr,
    INOUT GT_IPV6ADDR                 *ipSrcPtr,
    INOUT GT_U32                      *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                        *tcamGroupRowIndexPtr,
    OUT GT_U32                        *tcamSrcRowIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC routeEntry;

    CPSS_NULL_PTR_CHECK_MAC(ipGroupPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipGroupPrefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipSrcPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipSrcPrefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcRowIndexPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntry,0,sizeof(routeEntry));

    if (*ipGroupPrefixLenPtr > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (*ipSrcPrefixLenPtr > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* IPv6 multicast group prefix address format ff00::/8 */
    if ((*ipGroupPrefixLenPtr != 0) && (ipGroupPtr->arIP[0] != 0xFF))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    else
        if (*ipGroupPrefixLenPtr == 0)
        {
            ipGroupPtr->arIP[0] = 0xFF;
            *ipGroupPrefixLenPtr=8;
        }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6McEntryGetNext((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                          vrId,
                                                          ipGroupPtr,
                                                          ipGroupPrefixLenPtr,
                                                          ipSrcPtr,
                                                          ipSrcPrefixLenPtr,
                                                          &routeEntry,
                                                          tcamGroupRowIndexPtr,
                                                          tcamSrcRowIndexPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6McEntryGetNext((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                          vrId,
                                                          ipGroupPtr,
                                                          ipGroupPrefixLenPtr,
                                                          ipSrcPtr,
                                                          ipSrcPrefixLenPtr,
                                                          &routeEntry);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(lpmHw, &routeEntry, mcRouteLttEntryPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - the LPM DB id
*       vrId                - the virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - (pointer to) the ip Group address to get the next
*                             entry for
*       ipGroupPrefixLenPtr - (pointer to) the ipGroup prefix length
*       ipSrcPtr            - (pointer to) the ip Source address to get the next entry for
*       ipSrcPrefixLenPtr   - (pointer to) the ipSrc prefix length
*
* OUTPUTS:
*       ipGroupPtr           - (pointer to) the next ip Group address.
*       ipGroupPrefixLenPtr  - (pointer to) ipGroup prefix length.
*       ipSrcPtr             - (pointer to) the next ip Source address.
*       ipSrcPrefixLenPtr    - (pointer to) ipSrc prefix length.
*       mcRouteLttEntryPtr   - (pointer to) the LTT entry pointer pointing to
*                              the MC route entry associated with this MC route
*       tcamGroupRowIndexPtr - (pointer to) TCAM group row  index
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr   - (pointer to) TCAM source row  index.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The values of (ipGroupPtr,ipGroupPrefixLenPtr) must be a valid
*       values, it means that they exist in the IP-Mc Table, unless this is
*       the first call to this function, then the value of (ipGroupPtr,
*       ipSrcPtr) is (0,0).
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryGetNext
(
    IN    GT_U32                      lpmDBId,
    IN    GT_U32                      vrId,
    INOUT GT_IPV6ADDR                 *ipGroupPtr,
    INOUT GT_U32                      *ipGroupPrefixLenPtr,
    INOUT GT_IPV6ADDR                 *ipSrcPtr,
    INOUT GT_U32                      *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                        *tcamGroupRowIndexPtr,
    OUT GT_U32                        *tcamSrcRowIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6McEntryGetNext);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipGroupPtr, ipGroupPrefixLenPtr, ipSrcPtr, ipSrcPrefixLenPtr, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamSrcRowIndexPtr));

    rc = internal_cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, ipGroupPtr, ipGroupPrefixLenPtr, ipSrcPtr, ipSrcPrefixLenPtr, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamSrcRowIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipGroupPtr, ipGroupPrefixLenPtr, ipSrcPtr, ipSrcPrefixLenPtr, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamSrcRowIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   This function returns the multicast (ipSrc, ipGroup) entry, used to find
*   specific multicast adrress entry, and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
*       vrId              - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroup           - The ip Group address to get the entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr   - (pointer to) the LTT entry pointer pointing to
*                              the MC route entry associated with this MC route
*       tcamGroupRowIndexPtr - (pointer to) TCAM group row  index
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr   - (pointer to) TCAM source row  index
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmIpv6McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPV6ADDR                 ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPV6ADDR                 ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC routeEntry;

    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcRowIndexPtr);

    cpssOsMemSet(&tmpLpmDb,0,sizeof(tmpLpmDb));
    cpssOsMemSet(&routeEntry,0,sizeof(routeEntry));

    if (ipGroupPrefixLen > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (ipSrcPrefixLen > 128)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (vrId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* IPv6 multicast group prefix address format ff00::/8 */
    if ((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] != 0xFF))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamIpv6McEntryGet((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow),
                                                      vrId,
                                                      ipGroup,
                                                      ipGroupPrefixLen,
                                                      ipSrc,
                                                      ipSrcPrefixLen,
                                                      &routeEntry,
                                                      tcamGroupRowIndexPtr,
                                                      tcamSrcRowIndexPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            retVal = prvCpssDxChLpmRamIpv6McEntryGet((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow),
                                                      vrId,
                                                      ipGroup,
                                                      ipGroupPrefixLen,
                                                      ipSrc,
                                                      ipSrcPrefixLen,
                                                      &routeEntry);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry(lpmHw, &routeEntry, mcRouteLttEntryPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   This function returns the multicast (ipSrc, ipGroup) entry, used to find
*   specific multicast adrress entry, and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
*       vrId              - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroup           - The ip Group address to get the entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr   - (pointer to) the LTT entry pointer pointing to
*                              the MC route entry associated with this MC route
*       tcamGroupRowIndexPtr - (pointer to) TCAM group row  index
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr   - (pointer to) TCAM source row  index
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPV6ADDR                 ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPV6ADDR                 ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmIpv6McEntrySearch);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamSrcRowIndexPtr));

    rc = internal_cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamSrcRowIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen, mcRouteLttEntryPtr, tcamGroupRowIndexPtr, tcamSrcRowIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBMemSizeGet
*
* DESCRIPTION:
*   This function gets the memory size needed to export the Lpm DB,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*
* OUTPUTS:
*   lpmDbSizePtr      - (pointer to) the table size calculated (in bytes)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBMemSizeGet
(
    IN    GT_U32     lpmDBId,
    OUT   GT_U32     *lpmDbSizePtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    /* perform size calculating in one iteration*/
    GT_U32      iterationSize   = 0xffffffff;

    GT_UINTPTR  iterPtr         = 0;

    CPSS_NULL_PTR_CHECK_MAC(lpmDbSizePtr);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChIpLpmDbGetL3(lpmDBId,lpmDbSizePtr,NULL,
                                             &iterationSize,&iterPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_TBD_BOOKMARK
            retVal = GT_OK;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmDBMemSizeGet
*
* DESCRIPTION:
*   This function gets the memory size needed to export the Lpm DB,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*
* OUTPUTS:
*   lpmDbSizePtr      - (pointer to) the table size calculated (in bytes)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBMemSizeGet
(
    IN    GT_U32     lpmDBId,
    OUT   GT_U32     *lpmDbSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBMemSizeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, lpmDbSizePtr));

    rc = internal_cpssDxChIpLpmDBMemSizeGet(lpmDBId, lpmDbSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, lpmDbSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBExport
*
* DESCRIPTION:
*   This function exports the Lpm DB into the preallocated memory,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - (pointer to) block data size supposed to be
*                               exported.
*                               in current iteration.
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - (pointer to) block data size that was not used.
*                               in current iteration.
*       lpmDbMemBlockPtr      - (pointer to) allocated for lpm DB memory area.
*       iterPtr               - (pointer to) the iterator, if = 0 then the
*                               operation is done.
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBExport
(
    IN    GT_U32     lpmDBId,
    OUT   GT_U32     *lpmDbMemBlockPtr,
    INOUT GT_U32     *lpmDbMemBlockSizePtr,
    INOUT GT_UINTPTR *iterPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    CPSS_NULL_PTR_CHECK_MAC(lpmDbMemBlockPtr);
    CPSS_NULL_PTR_CHECK_MAC(lpmDbMemBlockSizePtr);
    CPSS_NULL_PTR_CHECK_MAC(iterPtr);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChIpLpmDbGetL3(lpmDBId, NULL, lpmDbMemBlockPtr,
                                             lpmDbMemBlockSizePtr, iterPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_TBD_BOOKMARK
            retVal = GT_OK;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmDBExport
*
* DESCRIPTION:
*   This function exports the Lpm DB into the preallocated memory,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - (pointer to) block data size supposed to be
*                               exported.
*                               in current iteration.
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - (pointer to) block data size that was not used.
*                               in current iteration.
*       lpmDbMemBlockPtr      - (pointer to) allocated for lpm DB memory area.
*       iterPtr               - (pointer to) the iterator, if = 0 then the
*                               operation is done.
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBExport
(
    IN    GT_U32     lpmDBId,
    OUT   GT_U32     *lpmDbMemBlockPtr,
    INOUT GT_U32     *lpmDbMemBlockSizePtr,
    INOUT GT_UINTPTR *iterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBExport);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, lpmDbMemBlockPtr, lpmDbMemBlockSizePtr, iterPtr));

    rc = internal_cpssDxChIpLpmDBExport(lpmDBId, lpmDbMemBlockPtr, lpmDbMemBlockSizePtr, iterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, lpmDbMemBlockPtr, lpmDbMemBlockSizePtr, iterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChIpLpmDBImport
*
* DESCRIPTION:
*   This function imports the Lpm DB recived and reconstruct it,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - (pointer to) block data size supposed to be
*                               imported.
*                               in current iteration.
*       lpmDbMemBlockPtr      - (pointer to) allocated for lpm DB memory area.
*       iterPtr               - (pointer to) the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - (pointer to) block data size that was not used.
*                               in current iteration.
*       iterPtr               - (pointer to) the iterator, if = 0 then the
*                               operation is done.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChIpLpmDBImport
(
    IN    GT_U32     lpmDBId,
    IN    GT_U32     *lpmDbMemBlockPtr,
    INOUT GT_U32     *lpmDbMemBlockSizePtr,
    INOUT GT_UINTPTR *iterPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT lpmHw;

    CPSS_NULL_PTR_CHECK_MAC(lpmDbMemBlockPtr);
    CPSS_NULL_PTR_CHECK_MAC(iterPtr);
    CPSS_NULL_PTR_CHECK_MAC(lpmDbMemBlockSizePtr);

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChIpLpmDbSetL3(lpmDBId, lpmDbMemBlockPtr,
                                              lpmDbMemBlockSizePtr, iterPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_TBD_BOOKMARK
            retVal = GT_OK;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmDBImport
*
* DESCRIPTION:
*   This function imports the Lpm DB recived and reconstruct it,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - (pointer to) block data size supposed to be
*                               imported.
*                               in current iteration.
*       lpmDbMemBlockPtr      - (pointer to) allocated for lpm DB memory area.
*       iterPtr               - (pointer to) the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - (pointer to) block data size that was not used.
*                               in current iteration.
*       iterPtr               - (pointer to) the iterator, if = 0 then the
*                               operation is done.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBImport
(
    IN    GT_U32     lpmDBId,
    IN    GT_U32     *lpmDbMemBlockPtr,
    INOUT GT_U32     *lpmDbMemBlockSizePtr,
    INOUT GT_UINTPTR *iterPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChIpLpmDBImport);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, lpmDBId, lpmDbMemBlockPtr, lpmDbMemBlockSizePtr, iterPtr));

    rc = internal_cpssDxChIpLpmDBImport(lpmDBId, lpmDbMemBlockPtr, lpmDbMemBlockSizePtr, iterPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, lpmDBId, lpmDbMemBlockPtr, lpmDbMemBlockSizePtr, iterPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChIpLpmDbGetL3
*
* DESCRIPTION:
*       Retrieves a specific shadow's ip Table memory Size needed and info
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       iterationSizePtr   - (pointer to) data size in bytes supposed to be processed
*                            in current iteration.
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       tableSizePtr  - (pointer to) the table size calculated (in bytes)
*       tablePtr      - (pointer to) the table size info block
*       iterationSizePtr   - (pointer to) data size in bytes left after iteration .
*       iterPtr       - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case the LPM DB uses TCAM manager and creates the TCAM manager
*       internally, then the LPM DB is also responsible for exporting and
*       importing the TCAM manager data. The TCAM manager must be imported
*       before VR, UC and MC entries.
*       If the LPM uses TCAM manager (internally or externally) then the LPM DB
*       must update the TCAM manager client callback functions.
*
*       Data is arranged in entries of different types in the following order:
*        - TCAM manager entries, if needed. Each TCAM manager entry is 1k
*          of the TCAM manager HSU data (last entry is up to 1k).
*        - VR entry per virtual router in the LPM DB. If the LPM DB contains
*          no VR, then this section is empty.
*        - Dummy VR entry. Note that the dummy VR entry will always exists,
*          even if there are no VR in the LPM DB
*        - If the LPM DB supports IPv4 protocol then for each virtual router in
*          the LPM DB the following section will appear. The order of the
*          virtual routers is according to the virtual router ID. In case
*          there are no virtual routers, this section will be empty.
*            - UC entry for each IPv4 UC entry other than the default.
*            - Dummy UC entry.
*            - MC entry for each IPv4 MC entry other than the default.
*            - Dummy MC entry
*        - If the LPM DB supports IPv6 protocol then for each virtual router in
*          the LPM DB the following section will appear. The order of the
*          virtual routers is according to the virtual router ID. In case
*          there are no virtual routers, this section will be empty.
*            - UC entry for each IPv6 UC entry other than the default.
*            - Dummy UC entry.
*            - MC entry for each IPv6 MC entry other than the default.
*            - Dummy MC entry.
*       Note that if the LPM DB doesn't support a protocol stack then the
*       sections related to this protocol stack will be empty (will not include
*       the dummy entries as well).
*       If a VR doesn't support one of the prefix types then the section for
*       this prefix type will be empty but will include dummy.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbGetL3
(
    IN     GT_U32                       lpmDBId,
    OUT    GT_U32                       *tableSizePtr,
    OUT    GT_VOID                      *tablePtr,
    INOUT  GT_U32                       *iterationSizePtr,
    INOUT  GT_UINTPTR                   *iterPtr
)
{
    GT_STATUS                           retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT            lpmHw;

    CPSS_NULL_PTR_CHECK_MAC(iterPtr);
    CPSS_NULL_PTR_CHECK_MAC(iterationSizePtr);

    if (*iterationSizePtr == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* get the LPM DB */
    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamDbGet((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), tableSizePtr,
                                             tablePtr, iterationSizePtr, iterPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_TBD_BOOKMARK
            retVal = GT_OK;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChIpLpmDbSetL3
*
* DESCRIPTION:
*       Set the data needed for core IP shadow reconstruction used after HSU.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       tablePtr            - (pointer to)the table size info block.
*       iterationSizePtr    - (pointer to) data size in bytes supposed to be processed.
*                             in current iteration.
*       iterPtr             - the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterationSizePtr    - (pointer to) data size in bytes left after iteration .
*       iterPtr             - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       Refer to prvCpssDxChIpLpmDbGetL3.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbSetL3
(
    IN     GT_U32                       lpmDBId,
    IN     GT_VOID                      *tablePtr,
    INOUT  GT_U32                       *iterationSizePtr,
    INOUT  GT_UINTPTR                   *iterPtr
)
{
    GT_STATUS                               retVal;
    PRV_CPSS_DXCH_LPM_SHADOW_STC            *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT                lpmHw;

    CPSS_NULL_PTR_CHECK_MAC(tablePtr);
    CPSS_NULL_PTR_CHECK_MAC(iterPtr);
    CPSS_NULL_PTR_CHECK_MAC(iterationSizePtr);
    if (*iterationSizePtr == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* get the LPM DB */
    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    switch (lpmHw)
    {
        case PRV_CPSS_DXCH_LPM_HW_TCAM_E:
            retVal = prvCpssDxChLpmTcamDbSet((PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow), tablePtr,
                                             iterationSizePtr, iterPtr);
            break;

        case PRV_CPSS_DXCH_LPM_HW_RAM_E:
            CPSS_TBD_BOOKMARK
            retVal = GT_OK;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChIpLpmDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of LPM.
*       NOTE: function not 'free' the allocated memory. only detach from it ,
*             and restore DB to 'pre-init' state
*
*             The assumption is that the 'cpssOsMalloc' allocations will be not
*             valid any more by the application , so no need to 'free' each and
*             every allocation !
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbRelease
(
    void
)
{
    /* we detach the base of the DB and application can call again the :
        create function (cpssDxChIpLpmDBCreate)
    */

    /* we not indend to 'clean the DB' and free all allocations !!! */
    prvCpssDxChIplpmDbSL = NULL;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpLpmDbIdGetNext
*
* DESCRIPTION:
*       This function retrieve next LPM DB ID from LPM DBs Skip List
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       slIteratorPtr - The iterator Id that was returned from the last call to
*                       this function.
* OUTPUTS:
*       lpmDbIdPtr    - retrieved LPM DB ID
*       slIteratorPtr - The iteration Id to be sent in the next call to this
*                       function to get the next data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_NO_MORE               - on absence of elements in skip list
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbIdGetNext
(
    OUT     GT_U32      *lpmDbIdPtr,
    INOUT   GT_UINTPTR  *slIteratorPtr
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr = NULL;
    CPSS_NULL_PTR_CHECK_MAC(slIteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(lpmDbIdPtr);

    lpmDbPtr = (PRV_CPSS_DXCH_LPM_SHADOW_STC *)prvCpssSlGetNext(prvCpssDxChIplpmDbSL,slIteratorPtr);
    if (lpmDbPtr == NULL)
    {
        return /* it's not error for log */ GT_NO_MORE;
    }
    *lpmDbIdPtr = lpmDbPtr->lpmDbId;
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpLpmLogRouteEntryTypeGet
*
* DESCRIPTION:
*   This function gets route entry type according to lpmDBId
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*
* OUTPUTS:
*   entryTypePtr     - GT_TRUE if route entry is pclIpUcAction
*                       GT_FALSE if route entry is ipLttEntry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmLogRouteEntryTypeGet
(
    IN    GT_U32     lpmDBId,
    OUT   GT_BOOL    *entryTypePtr
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the LPM DB */
        return GT_BAD_PTR;
    }
    if ((lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_CHEETAH_SHADOW_E) || (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        *entryTypePtr = GT_TRUE;
    }
    else
    {
        *entryTypePtr = GT_FALSE;
    }
    return GT_OK;
}
