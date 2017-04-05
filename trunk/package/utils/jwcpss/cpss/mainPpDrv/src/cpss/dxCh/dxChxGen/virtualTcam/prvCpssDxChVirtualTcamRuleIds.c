/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChVirtualTcamRuleIds.c
*
* DESCRIPTION:
*       The CPSS DXCH High Level Virtual TCAM Manager - Rule Ids and priorities DB
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/virtualTcam/cpssDxChVirtualTcam.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/private/prvCpssDxChVirtualTcam.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/private/prvCpssDxChVirtualTcamDb.h>

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableCreate
*
* DESCRIPTION:
*       Create Rule Id Table for TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       maxEntriesInDb  - maximal amount entries in DB.
*
* OUTPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*                         allocated objects handlers updated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableCreate
(
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC    *pVTcamMngPtr,
    IN     GT_U32                                maxEntriesInDb
)
{
    GT_STATUS     rc;              /* return code                        */

    /* set NULL values to allocated pointers */
    pVTcamMngPtr->ruleIdPool            = CPSS_BM_POOL_NULL_ID;
    pVTcamMngPtr->ruleIdIdNodesPool     = CPSS_BM_POOL_NULL_ID;
    pVTcamMngPtr->ruleIdIndexNodesPool  = CPSS_BM_POOL_NULL_ID;

    rc = cpssBmPoolCreate(
        sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC),
        CPSS_BM_POOL_4_BYTE_ALIGNMENT_E,
        maxEntriesInDb,
        &(pVTcamMngPtr->ruleIdPool));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbRuleIdTableDelete(pVTcamMngPtr);
        return rc;
    }
    rc = prvCpssAvlMemPoolCreate(
        maxEntriesInDb,
        &(pVTcamMngPtr->ruleIdIdNodesPool));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbRuleIdTableDelete(pVTcamMngPtr);
        return rc;
    }
    rc = prvCpssAvlMemPoolCreate(
        maxEntriesInDb,
        &(pVTcamMngPtr->ruleIdIndexNodesPool));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbRuleIdTableDelete(pVTcamMngPtr);
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableDelete
*
* DESCRIPTION:
*       Delete Rule Id Table for TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*
* OUTPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*                         freed objects handlers set to NULL
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableDelete
(
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC    *pVTcamMngPtr
)
{
    GT_STATUS     rc, rcFinal;              /* return code */

    rcFinal = GT_OK;

    if (pVTcamMngPtr->ruleIdIndexNodesPool != CPSS_BM_POOL_NULL_ID)
    {
        rc = prvCpssAvlMemPoolDelete(pVTcamMngPtr->ruleIdIndexNodesPool);
        pVTcamMngPtr->ruleIdIndexNodesPool    = CPSS_BM_POOL_NULL_ID;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssAvlMemPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->ruleIdIdNodesPool != CPSS_BM_POOL_NULL_ID)
    {
        rc = prvCpssAvlMemPoolDelete(pVTcamMngPtr->ruleIdIdNodesPool);
        pVTcamMngPtr->ruleIdIdNodesPool    = CPSS_BM_POOL_NULL_ID;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssAvlMemPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->ruleIdPool != CPSS_BM_POOL_NULL_ID)
    {
        rc = cpssBmPoolDelete(pVTcamMngPtr->ruleIdPool);
        pVTcamMngPtr->ruleIdPool    = CPSS_BM_POOL_NULL_ID;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("cpssBmPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    return rcFinal;
}

/* trivial compare functions for Rule Id table entries */

static GT_COMP_RES prvCpssDxChVirtualTcamDbRuleIdTableRuleIdCompare
(
    IN  GT_VOID    *data_ptr1,
    IN  GT_VOID    *data_ptr2
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC* ptr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC* ptr2;

    ptr1 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC*)data_ptr1;
    ptr2 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC*)data_ptr2;

    if (ptr1->ruleId > ptr2->ruleId)
        return GT_GREATER;
    if (ptr1->ruleId < ptr2->ruleId)
        return GT_SMALLER;
    return GT_EQUAL;
}

static GT_COMP_RES prvCpssDxChVirtualTcamDbRuleIdTableLogicalIndexCompare
(
    IN  GT_VOID    *data_ptr1,
    IN  GT_VOID    *data_ptr2
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC* ptr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC* ptr2;

    ptr1 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC*)data_ptr1;
    ptr2 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC*)data_ptr2;

    if (ptr1->logicalIndex > ptr2->logicalIndex)
        return GT_GREATER;
    if (ptr1->logicalIndex < ptr2->logicalIndex)
        return GT_SMALLER;
    return GT_EQUAL;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesCreate
*
* DESCRIPTION:
*       Create Local per-vTcam Access Trees for Rule Id Table.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*
* OUTPUTS:
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*                         allocated objects handlers updated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesCreate
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *pVTcamMngPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *tcamCfgPtr
)
{
    GT_STATUS     rc;              /* return code                        */

    /* set NULL values to allocated pointers */
    tcamCfgPtr->ruleIdIdTree     = NULL;
    tcamCfgPtr->ruleIdIndexTree  = NULL;

    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbRuleIdTableRuleIdCompare,
        pVTcamMngPtr->ruleIdIdNodesPool,
        &(tcamCfgPtr->ruleIdIdTree));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesDelete(pVTcamMngPtr, tcamCfgPtr);
        return rc;
    }
    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbRuleIdTableLogicalIndexCompare,
        pVTcamMngPtr->ruleIdIndexNodesPool,
        &(tcamCfgPtr->ruleIdIndexTree));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesDelete(pVTcamMngPtr, tcamCfgPtr);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesDelete
*
* DESCRIPTION:
*       Delete Local per-vTcam Access Trees for Rule Id Table.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*
* OUTPUTS:
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*                         freed objects handlers updated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesDelete
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *pVTcamMngPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *tcamCfgPtr
)
{
    GT_STATUS     rc, rcFinal;              /* return code */

    /* compiler warning prevent */
    pVTcamMngPtr = pVTcamMngPtr;

    rcFinal = GT_OK;

    if (tcamCfgPtr->ruleIdIdTree != NULL)
    {
        rc = prvCpssAvlTreeDelete(
            tcamCfgPtr->ruleIdIdTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        if (rc != GT_OK)
        {
            rcFinal = rc;
        }
        tcamCfgPtr->ruleIdIdTree     = NULL;
    }
    if (tcamCfgPtr->ruleIdIndexTree != NULL)
    {
        rc = prvCpssAvlTreeDelete(
            tcamCfgPtr->ruleIdIndexTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        if (rc != GT_OK)
        {
            rcFinal = rc;
        }
        tcamCfgPtr->ruleIdIndexTree  = NULL;
    }
    return rcFinal;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableCreate
*
* DESCRIPTION:
*       Create Priority Table for TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*       maxEntriesInDb    - maximal amount entries in DB.
*
* OUTPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*                           Handle to allocted table updated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableCreate
(
    INOUT   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *pVTcamMngPtr,
    IN      GT_U32                              maxEntriesInDb
)
{
    GT_STATUS     rc;              /* return code                        */

    /* set NULL values to allocated pointers */
    pVTcamMngPtr->priorityPool            = CPSS_BM_POOL_NULL_ID;
    pVTcamMngPtr->priorityPriNodesPool    = CPSS_BM_POOL_NULL_ID;
    pVTcamMngPtr->priorityIndexNodesPool  = CPSS_BM_POOL_NULL_ID;

    rc = cpssBmPoolCreate(
        sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC),
        CPSS_BM_POOL_4_BYTE_ALIGNMENT_E,
         maxEntriesInDb,
        &(pVTcamMngPtr->priorityPool));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbPriorityTableDelete(pVTcamMngPtr);
        return rc;
    }
    rc = prvCpssAvlMemPoolCreate(
        maxEntriesInDb,
        &(pVTcamMngPtr->priorityPriNodesPool));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbPriorityTableDelete(pVTcamMngPtr);
        return rc;
    }
    rc = prvCpssAvlMemPoolCreate(
        maxEntriesInDb,
        &(pVTcamMngPtr->priorityIndexNodesPool));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbPriorityTableDelete(pVTcamMngPtr);
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableDelete
*
* DESCRIPTION:
*       Delete Priority Table for TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*
* OUTPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*                           Handle to freed table set to NULL
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableDelete
(
    INOUT   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *pVTcamMngPtr
)
{
    GT_STATUS     rc, rcFinal;              /* return code */

    rcFinal = GT_OK;

    if (pVTcamMngPtr->priorityIndexNodesPool != CPSS_BM_POOL_NULL_ID)
    {
        rc = prvCpssAvlMemPoolDelete(pVTcamMngPtr->priorityIndexNodesPool);
        pVTcamMngPtr->priorityIndexNodesPool    = CPSS_BM_POOL_NULL_ID;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssAvlMemPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->priorityPriNodesPool != CPSS_BM_POOL_NULL_ID)
    {
        rc = prvCpssAvlMemPoolDelete(pVTcamMngPtr->priorityPriNodesPool);
        pVTcamMngPtr->priorityPriNodesPool    = CPSS_BM_POOL_NULL_ID;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssAvlMemPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->priorityPool != CPSS_BM_POOL_NULL_ID)
    {
        rc = cpssBmPoolDelete(pVTcamMngPtr->priorityPool);
        pVTcamMngPtr->priorityPool    = CPSS_BM_POOL_NULL_ID;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("cpssBmPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    return rcFinal;
}

/* trivial compare functions for Priority table entries */

static GT_COMP_RES prvCpssDxChVirtualTcamDbPriorityTablePriorityCompare
(
    IN  GT_VOID    *data_ptr1,
    IN  GT_VOID    *data_ptr2
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC* ptr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC* ptr2;

    ptr1 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC*)data_ptr1;
    ptr2 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC*)data_ptr2;

    if (ptr1->priority > ptr2->priority)
        return GT_GREATER;
    if (ptr1->priority < ptr2->priority)
        return GT_SMALLER;
    return GT_EQUAL;
}

static GT_COMP_RES prvCpssDxChVirtualTcamDbPriorityTableLogicalIndexCompare
(
    IN  GT_VOID    *data_ptr1,
    IN  GT_VOID    *data_ptr2
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC* ptr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC* ptr2;

    ptr1 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC*)data_ptr1;
    ptr2 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC*)data_ptr2;

    if (ptr1->baseLogIndex > ptr2->baseLogIndex)
        return GT_GREATER;
    if (ptr1->baseLogIndex < ptr2->baseLogIndex)
        return GT_SMALLER;
    return GT_EQUAL;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesCreate
*
* DESCRIPTION:
*       Create Local per-vTcam Access Trees for Priority Table.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*
* OUTPUTS:
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*                         allocated objects handlers updated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesCreate
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *pVTcamMngPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *tcamCfgPtr
)
{
    GT_STATUS     rc;              /* return code                        */

    /* set NULL values to allocated pointers */
    tcamCfgPtr->priorityPriTree     = NULL;
    tcamCfgPtr->priorityIndexTree  = NULL;

    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbPriorityTablePriorityCompare,
        pVTcamMngPtr->priorityPriNodesPool,
        &(tcamCfgPtr->priorityPriTree));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete(pVTcamMngPtr, tcamCfgPtr);
        return rc;
    }
    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbPriorityTableLogicalIndexCompare,
        pVTcamMngPtr->priorityIndexNodesPool,
        &(tcamCfgPtr->priorityIndexTree));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete(pVTcamMngPtr, tcamCfgPtr);
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete
*
* DESCRIPTION:
*       Delete Local per-vTcam Access Trees for Priority Table.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*
* OUTPUTS:
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*                         freed objects handlers updated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *pVTcamMngPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *tcamCfgPtr
)
{
    GT_STATUS     rc, rcFinal;              /* return code */

    /* compiler warning prevent */
    pVTcamMngPtr = pVTcamMngPtr;

    rcFinal = GT_OK;

    if (tcamCfgPtr->priorityPriTree != NULL)
    {
        rc = prvCpssAvlTreeDelete(
            tcamCfgPtr->priorityPriTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        if (rc != GT_OK)
        {
            rcFinal = rc;
        }
        tcamCfgPtr->priorityPriTree     = NULL;
    }
    if (tcamCfgPtr->priorityIndexTree != NULL)
    {
        rc = prvCpssAvlTreeDelete(
            tcamCfgPtr->priorityIndexTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        if (rc != GT_OK)
        {
            rcFinal = rc;
        }
        tcamCfgPtr->priorityIndexTree  = NULL;
    }
    return rcFinal;
}



