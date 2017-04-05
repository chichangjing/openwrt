/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChVirtualTcamSegments.c
*
* DESCRIPTION:
*       The CPSS DXCH High Level Virtual TCAM Manager - segments DB
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/virtualTcam/cpssDxChVirtualTcam.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/private/prvCpssDxChVirtualTcam.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/private/prvCpssDxChVirtualTcamDb.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

/**********************************************************************************/
/* segments Table (the alone feature for physical layout)                      */
/**********************************************************************************/

/* Debug trace support */
GT_BOOL prvCpssDxChVirtualTcamDebugTraceEnable = GT_FALSE;

void prvCpssDxChVirtualTcamDebugTraceEnableSet(GT_BOOL enable)
{
    prvCpssDxChVirtualTcamDebugTraceEnable = enable;
}

#define PRV_DBG_TRACE(_x)                                               \
    if (prvCpssDxChVirtualTcamDebugTraceEnable != GT_FALSE)             \
        {cpssOsPrintf("TRACE: "); cpssOsPrintf _x; cpssOsPrintf("\n");}

#define PRV_DUMP_SEG(_s) \
    if (prvCpssDxChVirtualTcamDebugTraceEnable != GT_FALSE)             \
    {                                                                   \
        cpssOsPrintf("SEG:0x%08X(%3.3d),Blocks use, %s  %s \n",         \
        _s->rowsBase, _s->rowsAmount,                                   \
        prvCpssDxChVirtualTcamDbSegmentColumnsEnumNames[                \
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(  \
                _s->segmentColumnsMap,0)],                              \
        prvCpssDxChVirtualTcamDbSegmentColumnsEnumNames[                \
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(  \
                _s->segmentColumnsMap,1)]);                             \
    }

/* exception debug support */
GT_U32 prvCpssDxChVirtualTcamDbSegmentTableExceptionCounter = 0;

void prvCpssDxChVirtualTcamDbSegmentTableException()
{
    prvCpssDxChVirtualTcamDbSegmentTableExceptionCounter ++;
    if (prvCpssDxChVirtualTcamDebugTraceEnable != GT_FALSE)
    {
        cpssOsPrintf("***** Segment Table Exception ******\n");
    }
}

void prvCpssDxChVirtualTcamDbSegmentTableExceptionReset()
{
    prvCpssDxChVirtualTcamDbSegmentTableExceptionCounter ++;
}

void prvCpssDxChVirtualTcamDbSegmentTableExceptionPrint()
{
    cpssOsPrintf(
        "Exceprion counter: %d\n",
         prvCpssDxChVirtualTcamDbSegmentTableExceptionCounter);
}

/* this function only for debugging purposes */
void prvCpssDxChVirtualTcamDbSegmentTableMakeSpaceEnableSet(
    GT_U32 vTcamMngId, GT_BOOL enable)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *pVTcamMngPtr;

    pVTcamMngPtr = prvCpssDxChVirtualTcamDbVTcamMngGet(vTcamMngId);

    if (pVTcamMngPtr == NULL) return;

    pVTcamMngPtr->disableRearrange =
        ((enable == GT_FALSE) ? GT_TRUE : GT_FALSE);
}

PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_CHAR_PTR
    prvCpssDxChVirtualTcamDbSegmentColumnsEnumNames[
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E] =
{
    "COL NONE ",
    "COL 5   ",
    "COL 4,5 ",
    "COL 0..3",
    "COL 0..4",
    "COL ALL "
} ;

void prvCpssDxChVirtualTcamDbSegmentTableDumpTree
(
    IN     PRV_CPSS_AVL_TREE_ID  treeId
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1; /* segment  */
    GT_BOOL                 seeked;      /* node seeked                   */
    PRV_CPSS_AVL_TREE_PATH  path;        /* tree iterator                 */
    GT_VOID_PTR             dbEntryPtr;  /* pointer to entry in DB        */

    if (prvCpssDxChVirtualTcamDebugTraceEnable == GT_FALSE)
    {
        return;
    }

    cpssOsPrintf("SEG Tree Dump\n");

    seeked = prvCpssAvlPathSeek(
            treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    segPtr1 = dbEntryPtr;
    while (seeked != GT_FALSE)
    {
        /* print */
        cpssOsPrintf(
            "0x%08X(%3.3d) columnsMap 0x%08X lookup %04X vTCAM %04X\n",
            segPtr1->rowsBase, segPtr1->rowsAmount,
            segPtr1->segmentColumnsMap,
            segPtr1->lookupId, segPtr1->vTcamId);
        /* next segment */
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr1 = dbEntryPtr;
    }
}

/* prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMergeAndSplitStep trace */

GT_BOOL prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable = GT_FALSE;

GT_VOID prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnableSet(GT_BOOL enable)
{
    prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable = enable;
}

#define PRV_PRIMARY_TREE_MRG_SPLIT_TRACE_MAC(x) \
    if (prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable != GT_FALSE) \
        {cpssOsPrintf("\n*** PrimaryTreeMerge&Split ***\n"); cpssOsPrintf x; cpssOsPrintf("\n");}

/* debugging mechanism for prvCpssAvlItemInsert and prvCpssAvlItemRemove */

GT_BOOL prvCpssDxChVirtualTcamSegmentTableAvlItemDebugPrintEnable = GT_FALSE;

GT_VOID prvCpssDxChVirtualTcamDbSegmentTableAvlItemDebugPrintSet(GT_BOOL enable)
{
    prvCpssDxChVirtualTcamSegmentTableAvlItemDebugPrintEnable = enable;
}

GT_STATUS prvCpssDxChVirtualTcamSegmentTableAvlItemInsert
(
    IN  PRV_CPSS_AVL_TREE_ID                            treeId,
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1;

    if ((segPtr->rowsAmount == 0) || (segPtr->segmentColumnsMap == 0))
    {
        prvCpssDxChVirtualTcamDbSegmentTableException();
        return GT_BAD_PARAM;
    }

    rc = prvCpssAvlItemInsert(treeId, segPtr);
    if (rc == GT_OK) return GT_OK;
    prvCpssDxChVirtualTcamDbSegmentTableException();
    if (prvCpssDxChVirtualTcamSegmentTableAvlItemDebugPrintEnable == GT_FALSE)
    {
        return rc;
    }
    /* print */
    cpssOsPrintf("prvCpssAvlItemInsert failed: \n");
    cpssOsPrintf(
        "0x%08X(%3.3d) columnsMap 0x%08X lookup %04X vTCAM %04X\n",
        segPtr->rowsBase, segPtr->rowsAmount,
        segPtr->segmentColumnsMap,
        segPtr->lookupId, segPtr->vTcamId);
    segPtr1 = prvCpssAvlSearch(treeId, segPtr);
    if (segPtr1 == NULL)
    {
        cpssOsPrintf("prvCpssAvlSearch found nothing equal \n");
    }
    else
    {
        cpssOsPrintf("prvCpssAvlSearch found equal: \n");
        cpssOsPrintf(
            "0x%08X(%3.3d) columnsMap 0x%08X lookup %04X vTCAM %04X\n",
            segPtr1->rowsBase, segPtr1->rowsAmount,
            segPtr1->segmentColumnsMap,
            segPtr1->lookupId, segPtr1->vTcamId);
    }
    return rc;
}

GT_VOID* prvCpssDxChVirtualTcamSegmentTableAvlItemRemove
(
    IN  PRV_CPSS_AVL_TREE_ID                            treeId,
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1;

    segPtr1 = prvCpssAvlItemRemove(treeId, segPtr);
    if (segPtr1 != NULL) return segPtr1;
    prvCpssDxChVirtualTcamDbSegmentTableException();
    if (prvCpssDxChVirtualTcamSegmentTableAvlItemDebugPrintEnable == GT_FALSE)
    {
        return segPtr1;
    }
    /* print */
    cpssOsPrintf("prvCpssAvlItemRemove failed: \n");
    cpssOsPrintf(
        "0x%08X(%3.3d) columnsMap 0x%08X lookup %04X vTCAM %04X\n",
        segPtr->rowsBase, segPtr->rowsAmount,
        segPtr->segmentColumnsMap,
        segPtr->lookupId, segPtr->vTcamId);
    segPtr1 = prvCpssAvlSearch(treeId, segPtr);
    if (segPtr1 == NULL)
    {
        cpssOsPrintf("prvCpssAvlSearch found nothing equal \n");
    }
    else
    {
        cpssOsPrintf("prvCpssAvlSearch found equal: \n");
        cpssOsPrintf(
            "0x%08X(%3.3d) columnsMap 0x%08X lookup %04X vTCAM %04X\n",
            segPtr1->rowsBase, segPtr1->rowsAmount,
            segPtr1->segmentColumnsMap,
            segPtr1->lookupId, segPtr1->vTcamId);
    }
    return NULL;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableSegColMapTo10ByteColumnsBitmap
*
* DESCRIPTION:
*       Convert segment columns map to bitmap of 10-byte rule places.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       segmentColumnsMap - segment columns map
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Calcuilated bitmap.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U32 prvCpssDxChVirtualTcamDbSegmentTableSegColMapTo10ByteColumnsBitmap
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP segmentColumnsMap
)
{
    GT_U32 i, totalBmp, blockBmp;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns;

    totalBmp = 0;
    for (i = 0; (i < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_MAX_SUPPORTED_CNS); i++)
    {
        segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            segmentColumnsMap, i);
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_BITMAP_OF_COLUMNS_MAC(
            segmentColumns, blockBmp);
        totalBmp |=
            (blockBmp <<
             (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_BITBAPS_WIDTH_CNS * i));

    }
    return totalBmp;
}

/* row capacity */
static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableRowCapacity
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT rowType,
    IN  GT_U32                                        ruleSize
)
{
    if (ruleSize == 0)
    {
        return 0;
    }
    switch (rowType)
    {
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E:
            return (6 / ruleSize);
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E:
            return ((ruleSize == 4) ? 1 : 0);
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E:
            return ((ruleSize == 5) ? 1 : 0);
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E:
            return (2 / ruleSize);
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E:
            return (1 / ruleSize);
        default: return 0;
    }
}

/* rule alignment */
static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableRuleAlignmentGet
(
    IN  GT_U32                                        ruleSize
)
{
    switch (ruleSize)
    {
        case 1:  return 1;
        case 2:  return 2;
        case 3:  return 3;
        case 4:
        case 5:
        case 6:  return 6;
        default: return 0;
    }
}

/* get Columns type and rule multiplier */
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableBestColumnsTypeGet
(
    IN  GT_U32                                         ruleSize,
    OUT PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT  *columnsTypePtr,
    OUT  GT_U32                                        *placeMultiplierPtr
)
{
    switch (ruleSize)
    {
        case 1:
        case 2:
        case 3:
        case 6:
            *columnsTypePtr = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E;
            *placeMultiplierPtr = prvCpssDxChVirtualTcamDbSegmentTableRowCapacity(
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E,
                ruleSize);
            break;
        case 4:
            *columnsTypePtr = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E;
            *placeMultiplierPtr = 1;
            break;
        case 5:
            *columnsTypePtr = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E;
            *placeMultiplierPtr = 1;
            break;
        default: return GT_BAD_PARAM;
    }
    return GT_OK;
}

/* rule segment type for search free space */
static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableStartColumnsTypeGet
(
    IN  GT_U32                                        ruleSize
)
{
    switch (ruleSize)
    {
        case 1:  return PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E;
        case 2:  return PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E;
        case 3:  return PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E;
        case 4:  return PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E;
        case 5:  return PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E;
        case 6:  return PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E;
        default: return 0;
    }
}


/* vTCAM size granularity - amount of rules in the full row of block */
static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableVTcamSizeGranularity
(
    IN  GT_U32      ruleSize
)
{
    switch (ruleSize)
    {
        default:
        case 6:
        case 5:
        case 4: return 1;
        case 3: return 2;
        case 2: return 3;
        case 1: return 6;
    }
}

static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToLookupOrderRank
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns
)
{
    switch (segmentColumns)
    {
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E:
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E:
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E:
            return 0;
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E:
            return 1;
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E:
            return 2;
        default:
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E:
            return 3;
    }
}

static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToFreeSpaceRank
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns
)
{
    switch (segmentColumns)
    {
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E:
            return 4;
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E:
            return 3;
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E:
            return 2;
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E:
            return 1;
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E:
            return 0;
        default: /* should not occur */
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E:
            prvCpssDxChVirtualTcamDbSegmentTableException();
            return 100;
    }
}
/* segments in lookup order */
static GT_COMP_RES prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompare
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* ptr1,
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* ptr2
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP b1, b2;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT c1,c2;
    GT_U32 i, rank1, rank2;

    if (ptr1->rowsBase > ptr2->rowsBase)
        return GT_GREATER;
    if (ptr1->rowsBase < ptr2->rowsBase)
        return GT_SMALLER;

    b1 = ptr1->segmentColumnsMap;
    b2 = ptr2->segmentColumnsMap;
    for (i = 0; (i < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_MAX_SUPPORTED_CNS); i++)
    {
        c1 = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(b1,i);
        c2 = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(b2,i);
        if (c1 == c2) continue;

        rank1 = prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToLookupOrderRank(c1);
        rank2 = prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToLookupOrderRank(c2);

        if (rank1 > rank2) return GT_GREATER; /* ptr1 is greater */
        if (rank1 < rank2) return GT_SMALLER;
    }

    return GT_EQUAL;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFunc
*
* DESCRIPTION:
*       AVL Tree compare function for segments in lookup order.
*       Used only internal.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       data_ptr1        - (pointer to) first segment DB entry
*       data_ptr2        - (pointer to) second segment DB entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_EQUAL, GT_GREATER or GT_SMALLER.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_COMP_RES prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFunc
(
    IN  GT_VOID    *data_ptr1,
    IN  GT_VOID    *data_ptr2
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* ptr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* ptr2;

    ptr1 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC*)data_ptr1;
    ptr2 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC*)data_ptr2;

    /* lookup id ignored */
    return prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompare(
        ptr1, ptr2);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFreeSegFunc
*
* DESCRIPTION:
*       AVL Tree compare function for free segments in lookup order.
*       Used only internal.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       data_ptr1        - (pointer to) first segment DB entry
*       data_ptr2        - (pointer to) second segment DB entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_EQUAL, GT_GREATER or GT_SMALLER.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_COMP_RES prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFreeSegFunc
(
    IN  GT_VOID    *data_ptr1,
    IN  GT_VOID    *data_ptr2
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* ptr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* ptr2;

    ptr1 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC*)data_ptr1;
    ptr2 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC*)data_ptr2;

    if (ptr1->lookupId > ptr2->lookupId) return GT_GREATER;
    if (ptr1->lookupId < ptr2->lookupId) return GT_SMALLER;

    /* lookup id ignored */
    return prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompare(
        ptr1, ptr2);
}

static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableMaxRulesAmountPerColumnsTypeGet
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns,
    IN  GT_U32                                                  size
)
{
    if ((size < 1) || (size > 6))
    {
        /* should never occur */
        prvCpssDxChVirtualTcamDbSegmentTableException();
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
            (" prvCpssDxChVirtualTcamDbSegmentTableMaxRulesAmountPerColumnsTypeGet size"));
        return 0;
    }

    switch (segmentColumns)
    {
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E:
            return 0;
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E:
            return (6 / size);
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E:
            return (2 / size);
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E:
            return (1 / size);
        /* sizes below not used / splitted for 10,20,30 */
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E:
            return ((size == 5) ? 1 : 0);
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E:
            return ((size == 4) ? 1 : 0);
        default:
            /* should never occur */
            prvCpssDxChVirtualTcamDbSegmentTableException();
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssDxChVirtualTcamDbSegmentTableMaxRulesAmountPerColumnsTypeGet wrong columns"));
            return 0;
    }
}

static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* ptr1,
    IN  GT_U32                                           size
)
{
    GT_U32 i, v;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns;

    v = 0;
    for (i = 0; (i < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_MAX_SUPPORTED_CNS); i++)
    {
        segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            ptr1->segmentColumnsMap, i);
        v += prvCpssDxChVirtualTcamDbSegmentTableMaxRulesAmountPerColumnsTypeGet(
            segmentColumns, size);
    }
    return v;
}

/* merges 2 colun maps */
/* returns GT_OK or GT_FAIL */
/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableColumBitmapsMerge
*
* DESCRIPTION:
*       Mergres two column maps.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       columnsMap1         - first merged columns map
*       columnsMap2         - second merged columns map
*       differentBlocksOnly - GT_TRUE  - to check that at least one of merged values
*                                       per horizontal block is empty
*                             GT_FALSE - no check.
*       sameBlocksOnly      - GT_TRUE  - to check that all values per horizontal block
*                                        both empty or both not empty
*                             GT_FALSE - no check.
*       singleBlock         - GT_TRUE  - to check that one of values per horizontal block
*                                        not empty for no more than one horizontal block
*                             GT_FALSE - no check.
*
* OUTPUTS:
*       mergedColumnsMapPtr - (pointer to) result bitmap
*
* RETURNS:
*       GT_OK or GT_FAIL.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableColumBitmapsMerge
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP   columnsMap1,
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP   columnsMap2,
    IN  GT_BOOL                                             differentBlocksOnly,
    IN  GT_BOOL                                             sameBlocksOnly,
    IN  GT_BOOL                                             singleBlock,
    OUT PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP   *mergedColumnsMapPtr
)
{
    GT_U32                                              i;
    GT_U32                                              numOfNotEmptyInColumn;
    GT_U32                                              numOfNotEmptyColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT       segmentColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT       segmentColumns1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT       segmentColumns2;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT       complementColumns1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP   mergedColumnsMap;

    numOfNotEmptyColumns = 0;
    mergedColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
    for (i = 0; (i < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_MAX_SUPPORTED_CNS); i++)
    {
        segmentColumns1 = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            columnsMap1, i);
        segmentColumns2 = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            columnsMap2, i);
        numOfNotEmptyInColumn = 0;
        if (segmentColumns1
            != PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E) numOfNotEmptyInColumn++;
        if (segmentColumns2
            != PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E) numOfNotEmptyInColumn++;

        if ((differentBlocksOnly != GT_FALSE) && (numOfNotEmptyInColumn > 1))
        {
            return GT_FAIL;
        }
        if ((sameBlocksOnly != GT_FALSE) && (numOfNotEmptyInColumn == 1))
        {
            return GT_FAIL;
        }
        if (singleBlock != GT_FALSE)
        {
            numOfNotEmptyColumns += ((numOfNotEmptyInColumn != 0) ? 1 : 0);
            if (numOfNotEmptyColumns > 1)
            {
                return GT_FAIL;
            }
        }

        if (numOfNotEmptyInColumn == 0) continue;

        if (numOfNotEmptyInColumn == 1)
        {
            /* exact one of them not empty, merge it */
            segmentColumns =
                ((segmentColumns1 != PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
                 ? segmentColumns1 : segmentColumns2);
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                mergedColumnsMap, i, segmentColumns);
        }
        else /*numOfNotEmptyInColumn == 2*/
        {
            /* check than the merged value is a full block row */
            /* other merging not supported                     */
            complementColumns1 =
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_COMPLEMENT_MAC(
                    segmentColumns1);
            if (complementColumns1 != segmentColumns2)
            {
                return GT_FAIL;
            }
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                mergedColumnsMap, i, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E);
        }
    }

    *mergedColumnsMapPtr = mergedColumnsMap;
    return GT_OK;
}

/* free space segments in order of "the worst preffered" */
static GT_COMP_RES prvCpssDxChVirtualTcamDbSegmentTableFreeCompare
(
    IN  GT_VOID    *data_ptr1,
    IN  GT_VOID    *data_ptr2
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* ptr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* ptr2;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns2;
    GT_U32 i;
    GT_U32 s1, s2;

    ptr1 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC*)data_ptr1;
    ptr2 = (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC*)data_ptr2;

    if (ptr1->lookupId > ptr2->lookupId)
        return GT_GREATER;
    if (ptr1->lookupId < ptr2->lookupId)
        return GT_SMALLER;

    /* preference rank */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
        ptr1->segmentColumnsMap, i, segmentColumns1);
    s1 = prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToFreeSpaceRank(
        segmentColumns1);
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
        ptr2->segmentColumnsMap, i, segmentColumns2);
    s2 = prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToFreeSpaceRank(
        segmentColumns2);

    if (s1 > s2)
        return GT_GREATER;
    if (s1 < s2)
        return GT_SMALLER;

    if (ptr1->rowsAmount > ptr2->rowsAmount)
        return GT_GREATER;
    if (ptr1->rowsAmount < ptr2->rowsAmount)
        return GT_SMALLER;

    return prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompare(
        ptr1, ptr2);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCreate
*
* DESCRIPTION:
*       Create Segment Table for TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*       segTabCfgPtr      - (pointer to) segments table configuration.
*       fillSegFreeTrees  - skip the filling of segFreePrefTree,segFreeLuTree with buffers
                from segmentsPool
*
* OUTPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*                           Keeps created objects handles.
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
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableCreate
(
    INOUT PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *pVTcamMngPtr,
    IN    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_CFG_STC       *segTabCfgPtr,
    IN    GT_BOOL                                              fillSegFreeTrees
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC* blockPtr; /* pointer to block info */
    GT_STATUS     rc;              /* return code                        */
    GT_U32        i,j;             /* loop index                         */
    GT_U32        blocksInRow;     /* number of blocks In Row            */
    GT_U32        blocksInColumn;  /* number of blocks In blocksInColumn */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC* segPtr; /* pointer to sedment entry */

    /* set NULL values to allocated pointers */
    pVTcamMngPtr->blockInfArr           = NULL;
    pVTcamMngPtr->segmentsPool          = NULL;
    pVTcamMngPtr->segNodesPoolFreePref  = NULL;
    pVTcamMngPtr->segNodesPoolFreeLu    = NULL;
    pVTcamMngPtr->segNodesPool          = NULL;
    pVTcamMngPtr->segFreePrefTree       = NULL;
    pVTcamMngPtr->segFreeLuTree         = NULL;

    cpssOsMemCpy(
        &(pVTcamMngPtr->tcamSegCfg), segTabCfgPtr,
        sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_CFG_STC));
    blocksInRow =
        (segTabCfgPtr->tcamColumnsAmount / segTabCfgPtr->lookupColumnAlignment);
    blocksInColumn =
        (segTabCfgPtr->tcamRowsAmount / segTabCfgPtr->lookupRowAlignment);
    pVTcamMngPtr->blocksAmount = (blocksInRow * blocksInColumn);
    pVTcamMngPtr->blockInfArr =
        (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC*)
        cpssOsMalloc(pVTcamMngPtr->blocksAmount *
            sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC));
    if (pVTcamMngPtr->blockInfArr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    rc = cpssBmPoolCreate(
        sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC),
        CPSS_BM_POOL_4_BYTE_ALIGNMENT_E,
         segTabCfgPtr->maxSegmentsInDb,
        &(pVTcamMngPtr->segmentsPool));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
        return rc;
    }
    /* pool for global free segments tree ordered by allocation preference */
    rc = prvCpssAvlMemPoolCreate(
        segTabCfgPtr->maxSegmentsInDb,
        &(pVTcamMngPtr->segNodesPoolFreePref));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
        return rc;
    }
    /* pool for global free segments tree ordered by lookup */
    rc = prvCpssAvlMemPoolCreate(
        segTabCfgPtr->maxSegmentsInDb,
        &(pVTcamMngPtr->segNodesPoolFreeLu));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
        return rc;
    }
    /* pool for trees for segments             */
    /*per - vTCAM trees for used segments      */
    /*temporary tree for vTCAM rearrangement   */
    rc = prvCpssAvlMemPoolCreate(
        (segTabCfgPtr->maxSegmentsInDb * 2),
        &(pVTcamMngPtr->segNodesPool));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
        return rc;
    }
    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbSegmentTableFreeCompare,
        pVTcamMngPtr->segNodesPoolFreePref,
        &(pVTcamMngPtr->segFreePrefTree));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
        return rc;
    }
    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFreeSegFunc,
        pVTcamMngPtr->segNodesPoolFreeLu,
        &(pVTcamMngPtr->segFreeLuTree));
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
        return rc;
    }

    for (i = 0; (i < pVTcamMngPtr->blocksAmount); i++)
    {
        blockPtr                 = &(pVTcamMngPtr->blockInfArr[i]);
        blockPtr->columnsBase    = ((i % blocksInRow) * segTabCfgPtr->lookupColumnAlignment);
        blockPtr->rowsBase       = ((i / blocksInRow) * segTabCfgPtr->lookupRowAlignment);
        blockPtr->columnsAmount  = segTabCfgPtr->lookupColumnAlignment;
        blockPtr->rowsAmount     = segTabCfgPtr->lookupRowAlignment;
        blockPtr->lookupId       =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS;
        for (j = 0; (j < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E); j++)
        {
            blockPtr->freeSpace[j] = 0;
        }
        blockPtr->freeSpace[PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E]
            = blockPtr->rowsAmount;

        if(fillSegFreeTrees == GT_FALSE)
        {
            /* skip the filling of segFreePrefTree,segFreeLuTree with buffers
                from segmentsPool */
            continue;
        }

        segPtr    = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
        if (segPtr == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }
        cpssOsMemSet(segPtr, 0, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
        segPtr->rowsBase          = blockPtr->rowsBase;
        segPtr->rowsAmount        = blockPtr->rowsAmount;
        segPtr->segmentColumnsMap =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
            segPtr->segmentColumnsMap,
            (blockPtr->columnsBase / segTabCfgPtr->lookupColumnAlignment),
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E);
        segPtr->vTcamId           =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_VTAM_ID_FREE_SPACE_CNS;
        segPtr->lookupId          =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS;
        segPtr->rulesAmount       = 0xFFFFFFFF; /* not relevant */
        segPtr->ruleSize          = 0xFF;       /* not relevant */
        segPtr->baseLogicalIndex  = 0xFFFFFFFF; /* not relevant */
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            pVTcamMngPtr->segFreePrefTree, segPtr);
        if (rc != GT_OK)
        {
            prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
            return rc;
        }
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            pVTcamMngPtr->segFreeLuTree, segPtr);
        if (rc != GT_OK)
        {
            prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableDelete
*
* DESCRIPTION:
*       Delete Segment Table for TCAM. (Free used memory)
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
*                           Deleted objects handles set to NULL.
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
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableDelete
(
    INOUT PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *pVTcamMngPtr
)
{
    GT_STATUS     rc, rcFinal;   /* return code                        */

    rcFinal = GT_OK;

    if (pVTcamMngPtr->segFreeLuTree != NULL)
    {
        /* during tree deleting all data will be freed from segmentsPool */
        rc = prvCpssAvlTreeDelete(
            pVTcamMngPtr->segFreeLuTree,
            (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
            (GT_VOID*)pVTcamMngPtr->segmentsPool);
        pVTcamMngPtr->segFreeLuTree = NULL;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssAvlTreeDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->segNodesPoolFreeLu != NULL)
    {
        rc = prvCpssAvlMemPoolDelete(pVTcamMngPtr->segNodesPoolFreeLu);
        pVTcamMngPtr->segNodesPoolFreeLu    = NULL;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssAvlMemPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->segFreePrefTree != NULL)
    {
        /* segments data freed destroing Luukup sorted tree */
        rc = prvCpssAvlTreeDelete(
            pVTcamMngPtr->segFreePrefTree,
            (GT_VOIDFUNCPTR)NULL,
            (GT_VOID*)NULL);
        pVTcamMngPtr->segFreePrefTree = NULL;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssAvlTreeDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->segNodesPoolFreePref != NULL)
    {
        rc = prvCpssAvlMemPoolDelete(pVTcamMngPtr->segNodesPoolFreePref);
        pVTcamMngPtr->segNodesPoolFreePref    = NULL;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssAvlMemPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->segNodesPool != NULL)
    {
        rc = prvCpssAvlMemPoolDelete(pVTcamMngPtr->segNodesPool);
        pVTcamMngPtr->segNodesPool    = NULL;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("prvCpssAvlMemPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->segmentsPool != NULL)
    {
        rc = cpssBmPoolDelete(pVTcamMngPtr->segmentsPool);
        pVTcamMngPtr->segmentsPool    = NULL;
        if (rc != GT_OK)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("cpssBmPoolDelete returned %d", rc));
            rcFinal = rc;
        }
    }
    if (pVTcamMngPtr->blockInfArr != NULL)
    {
        cpssOsFree(pVTcamMngPtr->blockInfArr);
        pVTcamMngPtr->blockInfArr = NULL;
    }
    return rcFinal;
}

/*******************************************************************************
* copyVtcamSegmentsLayout
*
* DESCRIPTION:
*       copy from source vtcam segments in source TCAM manager to
*                 target vtcam segments in target TCAM manager
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       srcVtcamInfoPtr  - (pointer to) the source virtual TCAM
*       trgVtcamInfoPtr  - (pointer to) the target virtual TCAM
*
* OUTPUTS:
*       trgVtcamInfoPtr  - (pointer to) the updated target virtual TCAM
*
* RETURNS :
*       GT_OK                   - on success
*       GT_FAIL                 - on error
*       GT_NO_RESOURCE          - not enough resources
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS copyVtcamSegmentsLayout
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC               *trgVTcamMngPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC      *srcVtcamInfoPtr,
    INOUT PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC   *trgVtcamInfoPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *srcSegPtr; /* segment from src TCAM */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *trgBuffPtr;/* buffer in target TCAM */
    PRV_CPSS_AVL_TREE_PATH  path;            /* tree iterator                 */
    GT_VOID_PTR             dbEntryPtr;      /* pointer to entry in DB        */
    GT_BOOL found;

    CPSS_LOG_INFORMATION_MAC("start loop on the tree from the source TCAM \n");
    /* loop on the tree from the source TCAM */
    found = prvCpssAvlPathSeek(
            srcVtcamInfoPtr->segmentsTree, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    srcSegPtr = dbEntryPtr;
    while (found != GT_FALSE)
    {
        CPSS_LOG_INFORMATION_MAC("start handle segment from the source vtcam : rowsBase[%d] rowsAmount[%d] \n",
            srcSegPtr->rowsBase,
            srcSegPtr->rowsAmount);

        /* get new buffer from the target TCAM */
        trgBuffPtr = cpssBmPoolBufGet(trgVTcamMngPtr->segmentsPool);
        if (trgBuffPtr == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "failed on cpssBmPoolBufGet");
        }
        /* copy data from the source segment */
        *trgBuffPtr = *srcSegPtr;

        /* add the new buffer to the trees of the target vTCAM */
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            trgVtcamInfoPtr->segmentsTree, trgBuffPtr);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
        }

        /* jump to next segment in the source vTCAM */
        found = prvCpssAvlPathSeek(
                srcVtcamInfoPtr->segmentsTree, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        srcSegPtr = dbEntryPtr;
    }

    CPSS_LOG_INFORMATION_MAC("copy tcamInfo and rulesAmount[%d] from the source TCAM \n",
        srcVtcamInfoPtr->rulesAmount);

    trgVtcamInfoPtr->tcamInfo    = srcVtcamInfoPtr->tcamInfo;
    trgVtcamInfoPtr->rulesAmount = srcVtcamInfoPtr->rulesAmount;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCopy
*
* DESCRIPTION:
*       Copy Segment Table from source TCAM to target TCAM.
*       also copy ALL source vtcam segments in source TCAM manager to
*                     target vtcam segments in target TCAM manager
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       srcVTcamMngPtr      - (pointer to) source VTCAM Manager structure
*       trgVTcamMngPtr      - (pointer to) target VTCAM Manager structure
*       allocSkeletonOnly   - indication to allocate only skeleton
* OUTPUTS:
*       trgVTcamMngPtr      - (pointer to) updated target VTCAM Manager structure
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*       GT_NO_RESOURCE           - not enough resources
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableCopy
(
    IN    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *srcVTcamMngPtr,
    INOUT PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *trgVTcamMngPtr,
    IN    GT_BOOL                                             allocSkeletonOnly
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *srcSegPtr; /* segment from src TCAM */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *trgBuffPtr;/* buffer in target TCAM */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* stcVtcamInfoPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* trgVtcamInfoPtr;
    PRV_CPSS_AVL_TREE_PATH  path;            /* tree iterator                 */
    GT_VOID_PTR             dbEntryPtr;      /* pointer to entry in DB        */
    GT_BOOL found;
    GT_U32  ii;

    if(allocSkeletonOnly == GT_FALSE)
    {
        CPSS_LOG_INFORMATION_MAC("start loop on the tree from the source TCAM \n");
        /* loop on the tree from the source TCAM */
        found = prvCpssAvlPathSeek(
                srcVTcamMngPtr->segFreeLuTree, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        srcSegPtr = dbEntryPtr;
        while (found != GT_FALSE)
        {
            CPSS_LOG_INFORMATION_MAC("start handle segment from the source TCAM : rowsBase[%d] rowsAmount[%d] columnsUse[0x%08X]\n",
                srcSegPtr->rowsBase,
                srcSegPtr->rowsAmount,
                srcSegPtr->segmentColumnsMap);

            /* get new buffer from the target TCAM */
            trgBuffPtr = cpssBmPoolBufGet(trgVTcamMngPtr->segmentsPool);
            if (trgBuffPtr == NULL)
            {
                prvCpssDxChVirtualTcamDbSegmentTableException();
                CPSS_LOG_INFORMATION_MAC("fail to get new buffer from the target TCAM \n");
                rc = GT_NO_RESOURCE;
                goto cleanExit_lbl;
            }
            /* copy data from the source segment */
            *trgBuffPtr = *srcSegPtr;

            /* add the new buffer to the 2 trees of the target TCAM */
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                trgVTcamMngPtr->segFreePrefTree, trgBuffPtr);
            if (rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("fail to add node to target TCAM segFreePrefTree \n");
                goto cleanExit_lbl;
            }

            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                trgVTcamMngPtr->segFreeLuTree, trgBuffPtr);
            if (rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("fail to add node to target TCAM segFreeLuTree \n");
                goto cleanExit_lbl;
            }

            /* jump to next segment in the source TCAM */
            found = prvCpssAvlPathSeek(
                    srcVTcamMngPtr->segFreeLuTree, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                    path, &dbEntryPtr /* use dedicated var to avoid warnings */);
            srcSegPtr = dbEntryPtr;
        }

        /* copy array of blocks including lookup mappings and free space statistics */
        cpssOsMemCpy(
            trgVTcamMngPtr->blockInfArr, srcVTcamMngPtr->blockInfArr,
            (sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC)
                * srcVTcamMngPtr->blocksAmount));

        CPSS_LOG_INFORMATION_MAC("ended loop on the tree from the source TCAM \n");

        CPSS_LOG_INFORMATION_MAC("start copy to the new TCAM manager the vTCAMs used segments of the orig TCAM manager \n");
    }
    else
    {
        /* do not copy info */
    }

    /* in orig TCAM : loop on all vTCAMs and copy to target TCAM proper vTCAM */
    for(ii = 0 ; ii < srcVTcamMngPtr->vTcamCfgPtrArrSize ; ii++)
    {
        stcVtcamInfoPtr = srcVTcamMngPtr->vTcamCfgPtrArr[ii];
        if(stcVtcamInfoPtr == NULL)
        {
            continue;
        }

        /* allocate vtcam with segments tree support */
        trgVtcamInfoPtr =
            prvCpssDxChVirtualTcamDbAllocWithOnlySegmentTable(trgVTcamMngPtr,
                ii,/*vTcamId*/
                &stcVtcamInfoPtr->tcamInfo,
                allocSkeletonOnly);
        if (trgVtcamInfoPtr == NULL)
        {
            CPSS_LOG_INFORMATION_MAC("fail to allocate memory for vTCAM [%d] \n",
                ii);
            rc = GT_OUT_OF_CPU_MEM;
            goto cleanExit_lbl;
        }

        if(allocSkeletonOnly == GT_TRUE)
        {
            /* do not copy info */
            continue;
        }

        CPSS_LOG_INFORMATION_MAC("copy used segments from vTCAM [%d] to the new TCAM manager \n",
            ii);

        rc = copyVtcamSegmentsLayout(trgVTcamMngPtr,stcVtcamInfoPtr,trgVtcamInfoPtr);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("fail to copy used segments from vTCAM [%d] \n",
                ii);
            goto cleanExit_lbl;
        }
    }

    rc = GT_OK;

cleanExit_lbl:
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("due to error : delete the segment table of the target TCAM \n");

        prvCpssDxChVirtualTcamDbSegmentTableDeleteWithVtcamsSegments(trgVTcamMngPtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableDeleteWithVtcamsSegments
*
* DESCRIPTION:
*       1. delete TCAM manager that hold only 'segments table' .
*       2. delete all the vTCAMs of this TCAM manager (their 'segments table')
*
*       NOTE : DO NOT free the CPU memory of vTcamMngPtr ,
*                              vTcamMngPtr->vTcamCfgPtrArr
*                              vTcamCfgPtrArr[ii]
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngPtr  - (pointer to) the TCAM manager
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableDeleteWithVtcamsSegments
(
    OUT PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC   *vTcamMngPtr
)
{
    GT_STATUS rc,finalRc = GT_OK;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U32  ii;

    /* loop on all vTCAMs and delete their segment tree */
    for(ii = 0 ; ii < vTcamMngPtr->vTcamCfgPtrArrSize ; ii++)
    {
        vtcamInfoPtr = vTcamMngPtr->vTcamCfgPtrArr[ii];
        if(vtcamInfoPtr == NULL)
        {
            continue;
        }

        /* delete the segment tree of the VTCAM */
        rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamFree(
            vTcamMngPtr,  vtcamInfoPtr);
        if(rc != GT_OK)
        {
            /* don't stop */
            finalRc = rc;
        }
    }
    /* Delete Segment Table for TCAM. */
    rc = prvCpssDxChVirtualTcamDbSegmentTableDelete(vTcamMngPtr);
    if(rc != GT_OK)
    {
        finalRc = rc;
    }

    return finalRc;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableUpdateBlockFreeSpace
*
* DESCRIPTION:
*       Decrease or increase per block counters for segment removed from free space.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       segPtr          - (pointer to) segment removed from free space.
*       decrease        - GT_TRUE - decrease, GT_FALSE - increase
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDxChVirtualTcamDbSegmentTableUpdateBlockFreeSpace
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr,
    IN     GT_BOOL                                                    decrease
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns;
    GT_U32                                          blocksInRow;
    GT_U32                                          blockHorzIdx ;
    GT_U32                                          blockIdx;

    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    for (blockHorzIdx = 0; (blockHorzIdx < blocksInRow); blockHorzIdx++)
    {
        segmentColumns =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, blockHorzIdx);

        if (segmentColumns ==
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E) continue;

        blockIdx =
            ((blocksInRow *
              (segPtr->rowsBase
               / pVTcamMngPtr->tcamSegCfg.lookupRowAlignment)) +
            blockHorzIdx);

        if (decrease != GT_FALSE)
        {
            if (pVTcamMngPtr->blockInfArr[blockIdx].freeSpace[segmentColumns]
                < segPtr->rowsAmount)
            {
                prvCpssDxChVirtualTcamDbSegmentTableException();
                PRV_DBG_TRACE(("Block free space underflow."));
                pVTcamMngPtr->blockInfArr[blockIdx].freeSpace[segmentColumns] = 0;
                return;
            }
            pVTcamMngPtr->blockInfArr[blockIdx].freeSpace[segmentColumns] -=
                segPtr->rowsAmount;
        }
        else
        {
            pVTcamMngPtr->blockInfArr[blockIdx].freeSpace[segmentColumns] +=
                segPtr->rowsAmount;
        }
    }
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCropSegmentVertical
*
* DESCRIPTION:
*       Split segment to 3 vertical pairs: low, cropped, high.
*       Used to crop part of free segment allocating or drugging vTcam and
*       to free part of vTCAM segment on resize.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tree1           - first tree including segments (NULL means tree not used)
*       tree2           - second tree including segments (NULL means tree not used)
*       segPtr          - (pointer to) segment being splitted.
*       rowsBase        - rows base of cropped segment.
*       rowsAmount      - amount of rows in cropped segment
*       putBack         - GT_TRUE  - put back to trees
*                         GT_FALSE - leave removed from trees
*
* OUTPUTS:
*       segPtr          - (pointer to) updated segment - cropped part.
*       lowSegPtrPtr    - (pointer to) (pointer to) low remainder of segment
*                         (NULL parameter - caller not needs expects the value)
*                         (NULL value - no low remainder)
*       highSegPtrPtr   - (pointer to) (pointer to) high remainder of segment
*                         (NULL parameter - caller not needs expects the value)
*                         (NULL value - no high remainder)
*
* RETURNS:
*       GT_OK           - on success
*       other           - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableCropSegmentVertical
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       tree1,
    IN     PRV_CPSS_AVL_TREE_ID                                       tree2,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr,
    IN     GT_U32                                                     rowsBase,
    IN     GT_U32                                                     rowsAmount,
    IN     GT_BOOL                                                    putBack,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            **lowSegPtrPtr,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            **highSegPtrPtr
)
{
    GT_STATUS                                                  rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr1;

    /* default values of output parameters */
    if (lowSegPtrPtr != NULL)
    {
        *lowSegPtrPtr = NULL;
    }
    if (highSegPtrPtr != NULL)
    {
        *highSegPtrPtr = NULL;
    }

    if ((rowsBase == segPtr->rowsBase) &&
        ((rowsBase + rowsAmount) == (segPtr->rowsBase + segPtr->rowsAmount)))
    {
        if (putBack == GT_FALSE)
        {
            /* remove updated segment from both trees */
            if (tree1 != NULL)
            {
                prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                    tree1, (GT_VOID*)segPtr);
            }
            if (tree2 != NULL)
            {
                prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                    tree2, (GT_VOID*)segPtr);
            }
        }
        return GT_OK;
    }

    if ((rowsAmount == 0) ||
        (rowsBase < segPtr->rowsBase) ||
        ((rowsBase + rowsAmount) > (segPtr->rowsBase + segPtr->rowsAmount)))
    {
        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, "wrong segment cropped bounds");
    }

    /* remove updated segment from both trees */
    if (tree1 != NULL)
    {
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            tree1, (GT_VOID*)segPtr);
    }
    if (tree2 != NULL)
    {
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            tree2, (GT_VOID*)segPtr);
    }

    /* split low part */
    if (segPtr->rowsBase < rowsBase)
    {
        /* put back not cropped low rows */
        segPtr1 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
        if (segPtr1 == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "cpssBmPoolBufGet failed");
        }
        cpssOsMemCpy(segPtr1, segPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
        segPtr1->rowsBase   = segPtr->rowsBase;
        segPtr1->rowsAmount = rowsBase - segPtr->rowsBase;
        /* update resulting segment */
        segPtr->rowsBase    = rowsBase;
        segPtr->rowsAmount -= segPtr1->rowsAmount;
        if (tree1 != NULL)
        {
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree1, segPtr1);
            if (rc != GT_OK)
            {
                cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
            }
        }
        if (tree2 != NULL)
        {
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree2, segPtr1);
            if (rc != GT_OK)
            {
                prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(tree1, segPtr1);
                cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
            }
        }
        if (lowSegPtrPtr != NULL)
        {
            *lowSegPtrPtr = segPtr1;
        }
    }

    /* split high part */
    /* assumed segPtr->rowsBase == rowsBase */
    if (segPtr->rowsAmount > rowsAmount)
    {
        /* put back not cropped low rows */
        segPtr1 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
        if (segPtr1 == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "cpssBmPoolBufGet failed");
        }
        cpssOsMemCpy(segPtr1, segPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
        segPtr1->rowsBase = segPtr->rowsBase + rowsAmount;
        segPtr1->rowsAmount = segPtr->rowsAmount - rowsAmount;
        /* update resulting segment */
        /*segPtr->rowsBase unchanged */
        segPtr->rowsAmount = rowsAmount;
        if (tree1 != NULL)
        {
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree1, segPtr1);
            if (rc != GT_OK)
            {
                cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
            }
        }
        if (tree2 != NULL)
        {
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree2, segPtr1);
            if (rc != GT_OK)
            {
                prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(tree1, segPtr1);
                cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
            }
        }
        if (highSegPtrPtr != NULL)
        {
            *highSegPtrPtr = segPtr1;
        }
    }

    /* put back updated segment to both trees */
    if (putBack != GT_FALSE)
    {
        if (tree1 != NULL)
        {
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree1, segPtr);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
            }
        }
        if (tree2 != NULL)
        {
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree2, segPtr);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorizontal
*
* DESCRIPTION:
*       Split segment to 2 horizontal pairs: cropped and remainder.
*       Used to crop part of free segment allocating or drugging vTcam and
*       to free part of vTCAM segment on resize.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*       tree1             - first tree including segments (NULL means tree not used)
*       tree2             - second tree including segments (NULL means tree not used)
*       segPtr            - (pointer to) segment being splitted.
*       segmentColumnsMap - columns map of cropped segment.
*       putBack         - GT_TRUE  - put back to trees
*                         GT_FALSE - leave removed from trees
*
* OUTPUTS:
*       segPtr            - (pointer to) updated segment - cropped part.
*       remainSegPtrPtr   - (pointer to) (pointer to) remainder of segment
*                           (NULL parameter - caller not needs expects the value)
*                           (NULL value - no low remainder)
*
* RETURNS:
*       GT_OK           - on success
*       other           - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorizontal
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       tree1,
    IN     PRV_CPSS_AVL_TREE_ID                                       tree2,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP          segmentColumnsMap,
    IN     GT_BOOL                                                    putBack,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            **remainSegPtrPtr
)
{
    GT_STATUS                                                  rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              segmentColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              croppedColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              remainColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP          remainColumnsMap;
    GT_U32                                                     blocksInRow;
    GT_U32                                                     blockHorzIdx ;

    /* default values of output parameters */
    if (remainSegPtrPtr != NULL)
    {
        *remainSegPtrPtr = NULL;
    }

    if (segPtr->segmentColumnsMap == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS)
    {
        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, "wrong segment columns found");
    }

    if (segmentColumnsMap == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS)
    {
        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, "nothing to crop required");
    }

    if (segPtr->segmentColumnsMap == segmentColumnsMap)
    {
        if (putBack == GT_FALSE)
        {
            /* remove updated segment from both trees */
            if (tree1 != NULL)
            {
                prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                    tree1, (GT_VOID*)segPtr);
            }
            if (tree2 != NULL)
            {
                prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                    tree2, (GT_VOID*)segPtr);
            }
        }
        return GT_OK;
    }

    /* calculate remainder segment columns map*/
    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    remainColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
    for (blockHorzIdx = 0; (blockHorzIdx < blocksInRow); blockHorzIdx++)
    {
        segmentColumns =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, blockHorzIdx);
        croppedColumns =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segmentColumnsMap, blockHorzIdx);

        /* this block-wide-column in remainder already contain NONE */
        if (segmentColumns == croppedColumns) continue;

        /* nothing cropped from this block - all it has is thye remainder */
        if (croppedColumns ==
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
        {
            remainColumns = segmentColumns;
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                remainColumnsMap, blockHorzIdx, remainColumns);
            continue;
        }

        if (segmentColumns !=
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
        {
            /* splitting not full block row not supported */
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, "wrong segment cropped columns");
        }
        remainColumns =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_COMPLEMENT_MAC(
                croppedColumns);
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
            remainColumnsMap, blockHorzIdx, remainColumns);
    }

    /* remove updated segment from both trees */
    if (tree1 != NULL)
    {
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            tree1, (GT_VOID*)segPtr);
    }
    if (tree2 != NULL)
    {
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            tree2, (GT_VOID*)segPtr);
    }

    /* split remainder part */
    /* assumed  remainColumnsMap != PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS */
    /* put back not cropped columns */
    segPtr1 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
    if (segPtr1 == NULL)
    {
        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "cpssBmPoolBufGet failed");
    }
    /* new segment - remainder */
    cpssOsMemCpy(segPtr1, segPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
    segPtr1->segmentColumnsMap = remainColumnsMap;
    /* update old segment to be cropped part */
    segPtr-> segmentColumnsMap = segmentColumnsMap;
    if (tree1 != NULL)
    {
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree1, segPtr1);
        if (rc != GT_OK)
        {
            cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
        }
    }
    if (tree2 != NULL)
    {
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree2, segPtr1);
        if (rc != GT_OK)
        {
            prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(tree1, segPtr1);
            cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
        }
    }

    if (remainSegPtrPtr != NULL)
    {
        *remainSegPtrPtr = segPtr1;
    }

    /* put back updated segment to both trees */
    if (putBack != GT_FALSE)
    {
        if (tree1 != NULL)
        {
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree1, segPtr);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
            }
        }
        if (tree2 != NULL)
        {
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(tree2, segPtr);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "prvCpssAvlItemInsert failed");
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCropSegmentFirstHorzBlock
*
* DESCRIPTION:
*       Split segment to 2 horizontal pairs:
*       cropped first not empty horizontal block and remainder.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*       tree1             - first tree including segments (NULL means tree not used)
*       tree2             - second tree including segments (NULL means tree not used)
*       segPtr            - (pointer to) segment being splitted.
*       putBack           - GT_TRUE  - put back to trees
*                           GT_FALSE - leave removed from trees
*
* OUTPUTS:
*       segPtr            - (pointer to) updated segment - cropped part.
*       horzBlockIndexPtr - (pointer to) first not empty horizontal block index
*                           (NULL parameter - caller not needs expects the value)
*       remainSegPtrPtr   - (pointer to) (pointer to) remainder of segment
*                           (NULL parameter - caller not needs expects the value)
*                           (NULL value - no low remainder)
*
* RETURNS:
*       GT_OK           - on success
*       other           - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableCropSegmentFirstHorzBlock
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       tree1,
    IN     PRV_CPSS_AVL_TREE_ID                                       tree2,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr,
    IN     GT_BOOL                                                    putBack,
    OUT    GT_U32                                                     *horzBlockIndexPtr,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            **remainSegPtrPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              segmentColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP          segmentColumnsMap;
    GT_U32                                                     blockHorzIdx ;

    /* colpiler warning prevent */
    segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E;

    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
        segPtr->segmentColumnsMap, blockHorzIdx, segmentColumns);

    segmentColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;

    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
        segmentColumnsMap, blockHorzIdx, segmentColumns);

    if (horzBlockIndexPtr != NULL)
    {
        *horzBlockIndexPtr = blockHorzIdx;
    }

    return prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorizontal(
        pVTcamMngPtr, tree1, tree2,
        segPtr, segmentColumnsMap, putBack, remainSegPtrPtr);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorzBlockBitmap
*
* DESCRIPTION:
*       Split segment to 2 horizontal pairs:
*       cropped horizontal block bitmap and remainder.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*       tree1             - first tree including segments (NULL means tree not used)
*       tree2             - second tree including segments (NULL means tree not used)
*       segPtr            - (pointer to) segment being splitted.
*       horzBlockIndexPtr - horizontal block bitmasp to include
*       segmentColumnsMap - columns map of cropped segment.
*       putBack           - GT_TRUE  - put back to trees
*                           GT_FALSE - leave removed from trees
*
* OUTPUTS:
*       segPtr            - (pointer to) updated segment - cropped part.
*       remainSegPtrPtr   - (pointer to) (pointer to) remainder of segment
*                           (NULL parameter - caller not needs expects the value)
*                           (NULL value - no low remainder)
*
* RETURNS:
*       GT_OK           - on success
*       other           - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorzBlockBitmap
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       tree1,
    IN     PRV_CPSS_AVL_TREE_ID                                       tree2,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr,
    IN     GT_U32                                                     horzBlockBitmap,
    IN     GT_BOOL                                                    putBack,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            **remainSegPtrPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              segmentColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP          segmentColumnsMap;
    GT_U32                                                     blocksInRow;
    GT_U32                                                     blockHorzIdx ;

    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    segmentColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
    for (blockHorzIdx = 0; (blockHorzIdx < blocksInRow); blockHorzIdx++)
    {
        if ((horzBlockBitmap & (1 << blockHorzIdx)) == 0) continue;
        segmentColumns =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, blockHorzIdx);
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
            segmentColumnsMap, blockHorzIdx, segmentColumns);
    }

    return prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorizontal(
        pVTcamMngPtr, tree1, tree2, segPtr,
        segmentColumnsMap, putBack, remainSegPtrPtr);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegment
*
* DESCRIPTION:
*       Chain free segment to trees.
*       Segment data in data pool, but not in both trees - will be added.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       segPtr          - (pointer to) segment being added to trees.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegment
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr
)
{
    GT_STATUS rc, rcTotal; /* return code             */
    GT_BOOL   fullBlock;   /* full Block              */
    GT_U32    blocksInRow; /* number of blocks In Row */
    GT_U32    blockIdx;    /* blocks Index            */
    GT_U32    blockHorzIdx ; /* blocks horizontal Index */
    GT_U32    j;           /* loop Index              */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT  segmentColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP  checkColumnsMap;

    rcTotal = GT_OK;
    segPtr->vTcamId = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_VTAM_ID_FREE_SPACE_CNS;

    fullBlock = GT_TRUE; /* initial, will be overridden */

    if (segPtr->rowsAmount != pVTcamMngPtr->tcamSegCfg.lookupRowAlignment)
    {
        fullBlock = GT_FALSE;
    }
    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    /* find first Horizontal block in columns using map */
    /* assumed, but not checked that segment not covers other blocks */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
        segPtr->segmentColumnsMap, blockHorzIdx, segmentColumns);

    /* check that it is one column segment */
    checkColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
        checkColumnsMap, blockHorzIdx, segmentColumns);
    if (segPtr->segmentColumnsMap != checkColumnsMap)
    {
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
            ("ChainFreeSegment: Error, Multi column segment"));
    }

    if (segmentColumns != PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
    {
        fullBlock = GT_FALSE;
    }

    blockIdx =
        ((blocksInRow *
          (segPtr->rowsBase
           / pVTcamMngPtr->tcamSegCfg.lookupRowAlignment)) +
        blockHorzIdx);

    if (fullBlock != GT_FALSE)
    {
        segPtr->lookupId = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS;
        pVTcamMngPtr->blockInfArr[blockIdx].lookupId =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS;
        for (j = 0; (j < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E); j++)
        {
            pVTcamMngPtr->blockInfArr[blockIdx].freeSpace[j] = 0;
        }
        j = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E;
        pVTcamMngPtr->blockInfArr[blockIdx].freeSpace[j] =
            pVTcamMngPtr->tcamSegCfg.lookupRowAlignment;
    }
    else
    {
        /* segmentColumns found in segmentColumnsMap */
        pVTcamMngPtr->blockInfArr[blockIdx].freeSpace[segmentColumns] +=
            segPtr->rowsAmount;
    }

    segPtr->rulesAmount       = 0xFFFFFFFF; /* not relevant */
    segPtr->ruleSize          = 0xFF;       /* not relevant */
    segPtr->baseLogicalIndex  = 0xFFFFFFFF; /* not relevant */
    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
        pVTcamMngPtr->segFreePrefTree, segPtr);
    if (rc != GT_OK)
    {
        /* should never occur */
        prvCpssDxChVirtualTcamDbSegmentTableException();
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
            ("prvCpssAvlItemInsert failed"));
        rcTotal = rc;
    }
    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
        pVTcamMngPtr->segFreeLuTree, segPtr);
    if (rc != GT_OK)
    {
        /* should never occur */
        prvCpssDxChVirtualTcamDbSegmentTableException();
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
            ("prvCpssAvlItemInsert failed"));
        rcTotal = rc;
    }
    return rcTotal;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit
*
* DESCRIPTION:
*       Chain free segment to trees with horizontal splitting.
*       Segment data in data pool, but not in both trees - will be added.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       segPtr          - (pointer to) segment being added to trees.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr
)
{
    GT_STATUS                                                rc, rcTotal;   /* return code    */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC          *segPtr1;      /* segment        */
    GT_U32                                                   blocksInRow;   /* blocks In Row  */
    GT_U32                                                   blocksIdx;     /* blocks Index   */
    GT_U32                                                   lastBlocksIdx; /* blocks Index   */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT            segmentColumns; /* segment Columns */

    rcTotal = GT_OK;

    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    lastBlocksIdx = 0;
    for (blocksIdx = 0; (blocksIdx < blocksInRow); blocksIdx++)
    {
        segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            segPtr->segmentColumnsMap, blocksIdx);
        if (segmentColumns == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
        {
            continue;
        }
        lastBlocksIdx = blocksIdx;
    }

    for (blocksIdx = 0; (blocksIdx < blocksInRow); blocksIdx++)
    {
        segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            segPtr->segmentColumnsMap, blocksIdx);
        if (segmentColumns == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
        {
            continue;
        }
        if (lastBlocksIdx != blocksIdx)
        {
            /* not last horizontal block - new segment*/
            segPtr1 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
            if (segPtr1 == NULL)
            {
                /* should never occur */
                prvCpssDxChVirtualTcamDbSegmentTableException();
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                    ("cpssBmPoolBufGet failed"));
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
            }
            cpssOsMemCpy(
                segPtr1, segPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
        }
        else
        {
            /* last horizontal block - update segment*/
            segPtr1 = segPtr;
        }
        segPtr1->segmentColumnsMap =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
            segPtr1->segmentColumnsMap, blocksIdx, segmentColumns);
        rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegment(
            pVTcamMngPtr, segPtr1);
        if (rc != GT_OK)
        {
            rcTotal = rc;
        }
    }

    return rcTotal;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMergeIsNeeeded
*
* DESCRIPTION:
*       Check is Merge free space segments needed
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       segPtr1         - (pointer to) first segment
*       segPtr2         - (pointer to) second segment
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE            - merge needed
*       GT_FALSE           - merge not needed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMergeIsNeeeded
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC              *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr2
)
{
    GT_U32                  nextBlockBase;   /* next Block Base               */
    GT_U32                  w;               /* work data                     */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;  /* segment  */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT    segmentColumns1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT    segmentColumns2;
    GT_U32                  blockHorzIdx1, blockHorzIdx2;

    if (segPtr2->rowsBase < segPtr1->rowsBase)
    {
        /* swap arguments */
        segPtr  = segPtr2;
        segPtr2 = segPtr1;
        segPtr1 = segPtr;
    }

    /* assumed segPtr2->rowsBase >= segPtr1->rowsBase */

    if (segPtr2->rowsBase > (segPtr1->rowsBase + segPtr1->rowsAmount))
    {
        /* vertical separated */
        return GT_FALSE;
    }

    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
        segPtr1->segmentColumnsMap, blockHorzIdx1, segmentColumns1);
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
        segPtr2->segmentColumnsMap, blockHorzIdx2, segmentColumns2);
    if (blockHorzIdx1 != blockHorzIdx2)
    {
        /* different blocks - assumed that segment is a part of one block */
        return GT_FALSE;
    }

    /* check vertical merge */
    if (segPtr2->rowsBase == (segPtr1->rowsBase + segPtr1->rowsAmount))
    {
        if (segmentColumns1 != segmentColumns2)
        {
            /* different columns */
            return GT_FALSE;
        }
        w = segPtr1->rowsBase + pVTcamMngPtr->tcamSegCfg.lookupRowAlignment;
        nextBlockBase = w - (w % pVTcamMngPtr->tcamSegCfg.lookupRowAlignment);
        if (segPtr2->rowsBase >= nextBlockBase)
        {
            /* different blocks */
            return GT_FALSE;
        }
        return GT_TRUE;
    }

    /* check horizontal merge */
    /* assumed segPtr2->rowsBase >= segPtr1->rowsBase */
    /* assumed segPtr2->rowsBase < (segPtr1->rowsBase + segPtr1->rowsAmount) */
    if (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_COMPLEMENT_MAC(
        segmentColumns1) == segmentColumns2)
    {
        return GT_TRUE;
    }
    return GT_FALSE;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMergeNeighbors
*
* DESCRIPTION:
*       Merge neighbor (horizontal and vertical) free space segments
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
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMergeNeighbors
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr
)
{
    GT_STATUS                                       rc, rcTotal;  /* return code  */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1; /* segment  */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr2; /* segment  */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;  /* segment  */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC bookmark; /* to find after update */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns; /* segment Columns */
    GT_U32                     blockHorzIdx ;   /* blocks horizontal Index       */
    GT_BOOL                    seg1Seeked;      /* bookmark seeked               */
    PRV_CPSS_AVL_TREE_PATH     seg1Path;        /* tree iterator                 */
    PRV_CPSS_AVL_TREE_PATH     seg2Path;        /* tree iterator                 */
    GT_VOID_PTR                dbEntryPtr;      /* pointer to entry in DB        */
    GT_BOOL                    mergeNeeded;     /* merge needed                  */
    GT_U32                     r1,r2,r3,r4;     /* work data                     */
    GT_U32                     bufUsed; /* work data                   */
    GT_U32                     low, high;       /* work data                     */

    rcTotal = GT_OK;

    /* first low of pair checked to merging */
    seg1Seeked = prvCpssAvlPathSeek(
            pVTcamMngPtr->segFreeLuTree, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            seg1Path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    /* to prevent compiler warnings only */
    segPtr1 = dbEntryPtr;
    if (seg1Seeked == GT_FALSE)
    {
        return GT_OK;
    }
    cpssOsMemCpy(&bookmark, segPtr1, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
    while (seg1Seeked != GT_FALSE)
    {
        segPtr1 = dbEntryPtr;

        mergeNeeded = GT_FALSE;
        cpssOsMemCpy(seg2Path, seg1Path, sizeof(PRV_CPSS_AVL_TREE_PATH));
        while (GT_FALSE != prvCpssAvlPathSeek(
                pVTcamMngPtr->segFreeLuTree, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                seg2Path, &dbEntryPtr))
        {
            segPtr2 = dbEntryPtr;
            if (segPtr2->rowsBase > (segPtr1->rowsBase + segPtr1->rowsAmount))
            {
                mergeNeeded = GT_FALSE;
                break;
            }

            mergeNeeded = prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMergeIsNeeeded(
                pVTcamMngPtr, segPtr1, segPtr2);
            if (mergeNeeded == GT_FALSE)
            {
                continue;
            }

            cpssOsMemCpy(&bookmark, segPtr1, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));

            /* remove from trees (but not from pool) */
            prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                pVTcamMngPtr->segFreePrefTree, (GT_VOID*)segPtr1);
            prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                pVTcamMngPtr->segFreeLuTree, (GT_VOID*)segPtr1);
            prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                pVTcamMngPtr->segFreePrefTree, (GT_VOID*)segPtr2);
            prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                pVTcamMngPtr->segFreeLuTree, (GT_VOID*)segPtr2);

            prvCpssDxChVirtualTcamDbSegmentTableUpdateBlockFreeSpace(
                pVTcamMngPtr, segPtr1, GT_TRUE/*decrease*/);
            prvCpssDxChVirtualTcamDbSegmentTableUpdateBlockFreeSpace(
                pVTcamMngPtr, segPtr2, GT_TRUE/*decrease*/);

            if (segPtr2->rowsBase == (segPtr1->rowsBase + segPtr1->rowsAmount))
            {
                /* vertical merge */
                segPtr1->rowsAmount += segPtr2->rowsAmount;
                cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
                /* insert fixed data */
                rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegment(
                    pVTcamMngPtr, segPtr1);
                if (rc != GT_OK) rcTotal = rc;
            }
            else
            {
                /* horizontal merge */
                /* vertical bounds */
                r1 = segPtr1->rowsBase;
                r2 = segPtr2->rowsBase;
                r3 = segPtr1->rowsBase + segPtr1->rowsAmount;
                r4 = segPtr2->rowsBase + segPtr2->rowsAmount;
                /* assumed r1 <= r2 */
                /* vertical bounds of parts */
                /* before merged: r1  .. (r2 - 1)    */
                /* merged:        r2  .. (low - 1)   */
                /* after  merged: low .. (high - 1)  */
                if (r3 < r4)
                {
                    low = r3;
                    high = r4;
                }
                else
                {
                    low = r4;
                    high = r3;
                }
                bufUsed = 0;
                if (r1 < r2)
                {
                    /* freed : r1  .. (r2 - 1)    */
                    segPtr1->rowsAmount = r2 - r1;
                    /* insert fixed data */
                    rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegment(
                        pVTcamMngPtr, segPtr1);
                    if (rc != GT_OK) rcTotal = rc;
                    bufUsed |= 1;
                }
                if (low < high)
                {
                    /* freed : low .. (high - 1)  */
                    if (r3 > r4)
                    {
                        segPtr2->segmentColumnsMap = segPtr1->segmentColumnsMap;
                    }
                    segPtr2->rowsBase = low;
                    segPtr2->rowsAmount = high - low;
                    /* insert fixed data */
                    rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegment(
                        pVTcamMngPtr, segPtr2);
                    if (rc != GT_OK) rcTotal = rc;
                    bufUsed |= 2;
                }
                if ((bufUsed & 1) == 0)
                {
                    segPtr = segPtr1;
                    bufUsed |= 1;
                }
                else if ((bufUsed & 2) == 0)
                {
                    segPtr = segPtr2;
                    bufUsed |= 2;
                }
                else
                {
                    segPtr = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
                    if (segPtr == NULL)
                    {
                        /* should never occur */
                        prvCpssDxChVirtualTcamDbSegmentTableException();
                        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                            ("cpssBmPoolBufGet failed"));
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
                    }
                    cpssOsMemCpy(segPtr, segPtr1, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
                }
                /* merged:        r2  .. (low - 1)   */
                /* update columns in used block to be ALL */
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
                    segPtr->segmentColumnsMap, blockHorzIdx, segmentColumns);
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                    segPtr->segmentColumnsMap, blockHorzIdx,
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E);
                segPtr->rowsBase = r2;
                segPtr->rowsAmount = low - r2;
                /* insert fixed data */
                rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegment(
                    pVTcamMngPtr, segPtr);
                if (rc != GT_OK) rcTotal = rc;
                /* free unused buffers */
                if ((bufUsed & 1) == 0)
                {
                    cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
                }
                if ((bufUsed & 2) == 0)
                {
                    cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
                }
            }
            break;
        }

        /* next low of pair checked to merging */
        if (mergeNeeded == GT_FALSE)
        {
            seg1Seeked = prvCpssAvlPathSeek(
                    pVTcamMngPtr->segFreeLuTree, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                    seg1Path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        }
        else
        {
            seg1Seeked = prvCpssAvlPathFind(
                pVTcamMngPtr->segFreeLuTree, PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
                &bookmark, seg1Path, &dbEntryPtr);
        }
    }

    return rcTotal;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMerge
*
* DESCRIPTION:
*       Merge free space segments
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
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMerge
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr
)
{
    /* NOT HERE !!!                              */
    /* add moving all 10-byte and 20-byte rules  */
    /* after free 40-byte and 50-byte places     */
    /* to whole rows drugging along owner vTCAMs */
    prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMergeNeighbors(pVTcamMngPtr);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableFindFirstCoveringSegment
*
* DESCRIPTION:
*       Find first segment partially covering the given range of 10-byte rules
*       in lookup ordered tree.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr       - (pointer to) VTCAM Manager structure
*       segLuTree          - lookup oredered tree
*       rowsBase           - rows base of memory being partially covered
*       rowsAmount         - amount of rows in memory being partially covered
*       horzBlockIndex     - horizontal block index
*       segmentColumns     - columns in given horizontal block
*
* OUTPUTS:
*       outPath            - path to the found segment.
*       outSegPtrPtr       - (pointer to)(pointer to) the found segment.
*
* RETURNS:
*       GT_OK           - on success
*       other           - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableFindFirstCoveringSegment
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                              segLuTree,
    IN     GT_U32                                            rowsBase,
    IN     GT_U32                                            rowsAmount,
    IN     GT_U32                                            horzBlockIndex,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT     segmentColumns,
    OUT    PRV_CPSS_AVL_TREE_PATH                            outPath,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC   **outSegPtrPtr
)
{
    GT_U32                                                     lookupId;
    GT_U32                                                     rowsBound;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            segPattern;
    GT_BOOL                                                    found;
    PRV_CPSS_AVL_TREE_PATH                                     startPath;
    PRV_CPSS_AVL_TREE_PATH                                     path;
    GT_VOID                                                    *segVoidPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *savedSegPtr;
    PRV_CPSS_AVL_TREE_PATH                                     savedPath;
    GT_U32                                                     blocksInRow;
    GT_U32                                                     blockIndex;
    GT_U32                                                     blockRowsBase;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              foundSegmentColumns;
    GT_U32                                                     col10byteBitmap;
    GT_U32                                                     foundCol10byteBitmap;

    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_BITMAP_OF_COLUMNS_MAC(
        segmentColumns, col10byteBitmap);

    rowsBound = (rowsBase + rowsAmount);
    if (rowsBound > pVTcamMngPtr->tcamSegCfg.tcamRowsAmount)
    {
        return GT_BAD_PARAM;
    }
    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    if (horzBlockIndex > blocksInRow)
    {
        return GT_BAD_PARAM;
    }
    blockIndex =
        (((rowsBase / pVTcamMngPtr->tcamSegCfg.lookupRowAlignment)
         * blocksInRow) + horzBlockIndex);
    lookupId = pVTcamMngPtr->blockInfArr[blockIndex].lookupId;

    blockRowsBase =
        (rowsBase - (rowsBase % pVTcamMngPtr->tcamSegCfg.lookupRowAlignment));

    cpssOsMemSet(&segPattern, 0, sizeof(segPattern));
    segPattern.lookupId          = lookupId;
    segPattern.rowsBase          = rowsBase;
    segPattern.rowsAmount        = 1;
    segPattern.segmentColumnsMap =
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
        segPattern.segmentColumnsMap, horzBlockIndex, segmentColumns);

    /* the first approximation */
    found = prvCpssAvlPathFind(
        segLuTree,
        PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E,
        &segPattern, path, &segVoidPtr);
    if (found == GT_FALSE)
    {
        found = prvCpssAvlPathSeek(
                segLuTree,
                PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
                path, &segVoidPtr);
    }
    if (found == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* save found path */
    cpssOsMemCpy(startPath, path, sizeof(PRV_CPSS_AVL_TREE_PATH));

    /* look for first segment that covers the given rowsBase */
    /* search to low direction                               */
    savedSegPtr = NULL;
    do
    {
        segPtr = segVoidPtr;

        if (segPtr->lookupId != lookupId) break;

        /* another horisontal blocks row reached */
        if (segPtr->rowsBase < blockRowsBase) break;

        /* more than all relevant range */
        if (segPtr->rowsBase >= rowsBound) continue;

        foundSegmentColumns =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, horzBlockIndex);
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_BITMAP_OF_COLUMNS_MAC(
            foundSegmentColumns, foundCol10byteBitmap);
        /* another columns in the given horizontal block */
        if ((col10byteBitmap & foundCol10byteBitmap) == 0) continue;

        /* save correct result that possibly is the best */
        /* assumed (segPtr->rowsBase < rowsBound) */
        if ((segPtr->rowsBase + segPtr->rowsAmount) > rowsBase)
        {
            savedSegPtr = segPtr;
            cpssOsMemCpy(savedPath, path, sizeof(PRV_CPSS_AVL_TREE_PATH));
        }

        if (segPtr->rowsBase <= rowsBase)
        {
            if ((segPtr->rowsBase + segPtr->rowsAmount) > rowsBase)
            {
                cpssOsMemCpy(outPath, path, sizeof(PRV_CPSS_AVL_TREE_PATH));
                *outSegPtrPtr = segPtr;
                return GT_OK;
            }
            break;
        }
    } while (GT_FALSE != prvCpssAvlPathSeek(
            segLuTree,
            PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E,
            path, &segVoidPtr));

    if (savedSegPtr != NULL)
    {
        outPath       = savedPath;
        *outSegPtrPtr = savedSegPtr;
        return GT_OK;
    }

    cpssOsMemCpy(path, startPath, sizeof(PRV_CPSS_AVL_TREE_PATH));

    while (GT_FALSE != prvCpssAvlPathSeek(
            segLuTree,
            PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
            path, &segVoidPtr))
    {
        segPtr = segVoidPtr;

        if (segPtr->lookupId != lookupId) break;

        if (segPtr->rowsBase >= rowsBound) break;

        foundSegmentColumns =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, horzBlockIndex);
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_BITMAP_OF_COLUMNS_MAC(
            foundSegmentColumns, foundCol10byteBitmap);
        /* another columns in the given horizontal block */
        if ((col10byteBitmap & foundCol10byteBitmap) == 0) continue;

        if ((segPtr->rowsBase + segPtr->rowsAmount) > rowsBase)
        {
            cpssOsMemCpy(outPath, path, sizeof(PRV_CPSS_AVL_TREE_PATH));
            *outSegPtrPtr = segPtr;
            return GT_OK;
        }
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableRemoveMemoryFromFree
*
* DESCRIPTION:
*       Remove memory from free space DB.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr       - (pointer to) VTCAM Manager structure
*       lookupId           - lookup Id (needed when marking busy parts of free block)
*       rowsBase           - rows base of memory removed from free DB
*       rowsAmount         - amount of rows of memory removed from free DB
*       segmentColumnsMap  - columns map used by memory removed from free DB
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       other           - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableRemoveMemoryFromFree
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     GT_U32                                                     lookupId,
    IN     GT_U32                                                     rowsBase,
    IN     GT_U32                                                     rowsAmount,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP          segmentColumnsMap
)
{
    GT_STATUS                                                  rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              segmentColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              foundColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP          checkColumnsMap;
    PRV_CPSS_AVL_TREE_PATH                                     path;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *segPtr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            segCopy;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC       *blockPtr;
    GT_U32                                                     bitmap;
    GT_U32                                                     blocksInRow;
    GT_U32                                                     blockHorzIdx ;
    GT_U32                                                     blockIdx;
    GT_U32                                                     segBound;
    GT_U32                                                     bound;
    GT_U32                                                     cropRowsBase;
    GT_U32                                                     cropRowsAmount;
    GT_BOOL                                                    putCroppedBack;

    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    bitmap = 0;
    for (blockHorzIdx = 0; (blockHorzIdx < blocksInRow); blockHorzIdx++)
    {
        segmentColumns =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segmentColumnsMap, blockHorzIdx);
        if (segmentColumns
            != PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
        {
            bitmap |= (1 << blockHorzIdx);
        }
    }

    while (bitmap != 0)
    {
        for (blockHorzIdx = 0; (blockHorzIdx < blocksInRow); blockHorzIdx++)
        {
            if ((bitmap & (1 << blockHorzIdx)) == 0) continue;

            segmentColumns =
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                    segmentColumnsMap, blockHorzIdx);
            while (1)
            {
                rc = prvCpssDxChVirtualTcamDbSegmentTableFindFirstCoveringSegment(
                    pVTcamMngPtr, pVTcamMngPtr->segFreeLuTree,
                    rowsBase, rowsAmount, blockHorzIdx, segmentColumns,
                    path, &segPtr);
                if (rc != GT_OK)
                {
                    /* no more in this horizontal block */
                    bitmap &= (~ (1 << blockHorzIdx));
                    break;
                }

                if (segPtr->lookupId ==
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS)
                {
                    blockIdx =
                        ((blocksInRow *
                          (segPtr->rowsBase / pVTcamMngPtr->tcamSegCfg.lookupRowAlignment)) +
                        blockHorzIdx);
                    if (blockIdx >= pVTcamMngPtr->blocksAmount)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }

                    blockPtr                 = &(pVTcamMngPtr->blockInfArr[blockIdx]);
                    if (blockPtr->lookupId !=
                        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS)
                    {
                        prvCpssDxChVirtualTcamDbSegmentTableException();
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                    if (pVTcamMngPtr->tcamSegCfg.lookupMapFuncPtr != NULL)
                    {
                        rc = pVTcamMngPtr->tcamSegCfg.lookupMapFuncPtr(
                            pVTcamMngPtr->tcamSegCfg.cookiePtr,
                            blockPtr->rowsBase,
                            blockPtr->columnsBase,
                            lookupId);
                        if (rc != GT_OK)
                        {
                            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                                ("lookupMapFuncPtr returned %d", rc));
                            return rc;
                        }
                        blockPtr->lookupId = lookupId;
                    }
                    /* update segment lookup Id */
                    prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                        pVTcamMngPtr->segFreeLuTree, (GT_VOID*)segPtr);
                    prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                        pVTcamMngPtr->segFreePrefTree, (GT_VOID*)segPtr);
                    segPtr->lookupId = lookupId;
                    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                        pVTcamMngPtr->segFreeLuTree, segPtr);
                    if (rc != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
                    }
                    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                        pVTcamMngPtr->segFreePrefTree, segPtr);
                    if (rc != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
                    }
                }

                cropRowsBase =
                    (rowsBase > segPtr->rowsBase) ? rowsBase : segPtr->rowsBase;
                segBound = (segPtr->rowsBase + segPtr->rowsAmount);
                bound    = (rowsBase + rowsAmount);
                cropRowsAmount =
                    (((segBound < bound) ? segBound : bound) - cropRowsBase);

                foundColumns =
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                        segPtr->segmentColumnsMap, blockHorzIdx);
                if ((foundColumns != PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
                    && (foundColumns != segmentColumns))
                {
                    /* not supported or not relevant */
                     prvCpssDxChVirtualTcamDbSegmentTableException();
                     PRV_DBG_TRACE(("RemoveMemoryFromFree - not supported found columns."));
                    bitmap &= (~ (1 << blockHorzIdx));
                    break;
                }

                putCroppedBack = GT_FALSE; /* default */
                /* check that segment has exactly the same columns that should be freed */
                if (segPtr->segmentColumnsMap != segmentColumnsMap)
                {
                    putCroppedBack = GT_TRUE;
                }

                rc = prvCpssDxChVirtualTcamDbSegmentTableCropSegmentVertical(
                    pVTcamMngPtr, pVTcamMngPtr->segFreePrefTree, pVTcamMngPtr->segFreeLuTree,
                    segPtr, cropRowsBase, cropRowsAmount, putCroppedBack,
                    NULL /*lowSegPtrPtr*/, NULL /*highSegPtrPtr*/);
                if (rc != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
                }

                /* need horizontal split after vertical split */
                if (putCroppedBack != GT_FALSE)
                {
                    /* decreased per block statistic source */
                    cpssOsMemCpy(&segCopy, segPtr, sizeof(segCopy));
                    segCopy.segmentColumnsMap =
                        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                        segCopy.segmentColumnsMap, blockHorzIdx, foundColumns);

                    checkColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                        checkColumnsMap, blockHorzIdx, segmentColumns);
                    rc = prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorizontal(
                        pVTcamMngPtr, pVTcamMngPtr->segFreePrefTree, pVTcamMngPtr->segFreeLuTree,
                        segPtr, checkColumnsMap, GT_FALSE /*putBack*/, &segPtr1 /*remainSegPtrPtr*/);
                    if (rc != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
                    }
                    /* update per block statistics */
                    prvCpssDxChVirtualTcamDbSegmentTableUpdateBlockFreeSpace(
                        pVTcamMngPtr, &segCopy, GT_TRUE/*decrease*/);
                    prvCpssDxChVirtualTcamDbSegmentTableUpdateBlockFreeSpace(
                        pVTcamMngPtr, segPtr1, GT_FALSE/*decrease*/);
                }
                else
                {
                    /* update per block statistics */
                    prvCpssDxChVirtualTcamDbSegmentTableUpdateBlockFreeSpace(
                        pVTcamMngPtr, segPtr, GT_TRUE/*decrease*/);
                }

                cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr);
            }
        }
    }

    return GT_OK;
}

/* Per block row bitmap functions prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmap...*/

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapAllocate
*
* DESCRIPTION:
*       Allocate per Block Row bitmap.
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
*       None.
*
* RETURNS:
*       Address of bitmap or NULL.
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32* prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapAllocate
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC *pVTcamMngPtr
)
{
    GT_U32 horzBlocksInRow;
    GT_U32 blockRowsAmount;
    GT_U32 bitmapSize;

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    blockRowsAmount = (horzBlocksInRow * pVTcamMngPtr->tcamSegCfg.tcamRowsAmount);
    bitmapSize = (((blockRowsAmount + 31) / 32) * sizeof(GT_U32));
    return (GT_U32*)cpssOsMalloc(bitmapSize);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapFree
*
* DESCRIPTION:
*       Free per Block Row bitmap.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       bitmapPtr       - (pointer to) Block Row bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapFree
(
    IN     GT_U32                             *bitmapPtr
)
{
   cpssOsFree(bitmapPtr);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapReset
*
* DESCRIPTION:
*       Reset per Block Row bitmap.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       bitmapPtr       - (pointer to) Block Row bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapReset
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC *pVTcamMngPtr,
    IN     GT_U32                             *bitmapPtr
)
{
    GT_U32 horzBlocksInRow;
    GT_U32 blockRowsAmount;
    GT_U32 bitmapSize;

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    blockRowsAmount = (horzBlocksInRow * pVTcamMngPtr->tcamSegCfg.tcamRowsAmount);
    bitmapSize = (((blockRowsAmount + 31) / 32) * sizeof(GT_U32));
    cpssOsMemSet(bitmapPtr, 0, bitmapSize);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSet
*
* DESCRIPTION:
*       Set bit in per Block Row bitmap.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       bitmapPtr       - (pointer to) Block Row bitmap
*       rowIndex        - row index
*       horzBlockIndex  - horzontal block index
*       value           - 0 or 1, any other converted to 1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSet
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC *pVTcamMngPtr,
    IN     GT_U32                             *bitmapPtr,
    IN     GT_U32                             rowIndex,
    IN     GT_U32                             horzBlockIndex,
    IN     GT_U32                             value
)
{
    GT_U32 horzBlocksInRow;
    GT_U32 bitFullIndex;
    GT_U32 bitIndex;
    GT_U32 wordIndex;
    GT_U32 bitMask;

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    if (rowIndex >= pVTcamMngPtr->tcamSegCfg.tcamRowsAmount)
    {
        return;
    }
    if (horzBlockIndex >= horzBlocksInRow)
    {
        return;
    }

    bitFullIndex = ((rowIndex * horzBlocksInRow) + horzBlockIndex);
    wordIndex    = bitFullIndex / 32;
    bitIndex     = bitFullIndex % 32;
    bitMask      = (1 << bitIndex);
    if (value)
    {
        bitmapPtr[wordIndex] |= bitMask;
    }
    else
    {
        bitmapPtr[wordIndex] &= (~ bitMask);
    }
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapGet
*
* DESCRIPTION:
*       Get bit in per Block Row bitmap.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       bitmapPtr       - (pointer to) Block Row bitmap
*       rowIndex        - row index
*       horzBlockIndex  - horzontal block index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       value           - 0 or 1
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapGet
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC *pVTcamMngPtr,
    IN     GT_U32                             *bitmapPtr,
    IN     GT_U32                             rowIndex,
    IN     GT_U32                             horzBlockIndex
)
{
    GT_U32 horzBlocksInRow;
    GT_U32 bitFullIndex;
    GT_U32 bitIndex;
    GT_U32 wordIndex;

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    if (rowIndex >= pVTcamMngPtr->tcamSegCfg.tcamRowsAmount)
    {
        return 0;
    }
    if (horzBlockIndex >= horzBlocksInRow)
    {
        return 0;
    }

    bitFullIndex = ((rowIndex * horzBlocksInRow) + horzBlockIndex);
    wordIndex    = bitFullIndex / 32;
    bitIndex     = bitFullIndex % 32;
    return ((bitmapPtr[wordIndex] >> bitIndex) & 1);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSetForFreeRows
*
* DESCRIPTION:
*       Set bits of per Block Row bitmap for Free TCAM rows of given type.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       bitmapPtr       - (pointer to) Block Row bitmap
*       lookupId        - lookup id
*       freeSpaceType   - type of ree space
*       value           - 0 or 1, any other converted to 1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSetForFreeRows
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC             *pVTcamMngPtr,
    IN     GT_U32                                         *bitmapPtr,
    IN     GT_U32                                         lookupId,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT  freeSpaceType,
    IN     GT_U32                                         value
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC  segPattern;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;
    PRV_CPSS_AVL_TREE_PATH                           path;
    PRV_CPSS_AVL_TREE_FIND_ENT                       findType;
    GT_VOID                                          *dbEntryPtr;
    GT_BOOL                                          seeked;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT    segmentColumns;
    GT_U32                                           horzBlocksInRow;
    GT_U32                                           horzBlockIdx;
    GT_U32                                           rowIdx;
    GT_U32                                           rowHigh;

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    cpssOsMemSet(&segPattern, 0, sizeof(segPattern));
    segPattern.lookupId = lookupId;
    findType = PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E;

    for (seeked = prvCpssAvlPathFind(
            pVTcamMngPtr->segFreeLuTree, findType,
            &segPattern, path, &dbEntryPtr);
        (GT_FALSE != seeked);
        seeked = prvCpssAvlPathSeek(
            pVTcamMngPtr->segFreeLuTree,
            PRV_CPSS_AVL_TREE_SEEK_NEXT_E, path, &dbEntryPtr))
    {
        segPtr = dbEntryPtr;
        if (segPtr->lookupId != lookupId)
        {
            break;
        }

        for (horzBlockIdx = 0; (horzBlockIdx < horzBlocksInRow); horzBlockIdx++)
        {
            segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, horzBlockIdx);
            if (segmentColumns == freeSpaceType)
            {
                rowHigh = segPtr->rowsBase + segPtr->rowsAmount;
                for (rowIdx = segPtr->rowsBase; (rowIdx < rowHigh); rowIdx++)
                {
                    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSet(
                        pVTcamMngPtr, bitmapPtr, rowIdx, horzBlockIdx, value);
                }
            }
        }
    }
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCountAvailableSpace
*
* DESCRIPTION:
*       Count available free space for allocation.
*
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       lookupId        - lookup Id
*       ruleSize        - rule Size (measured in 10-byte units)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       amount of rules that can be allocated
*       without moving rules of other vTCAMs
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableCountAvailableSpace
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC *pVTcamMngPtr,
    IN     GT_U32                             lookupId,
    IN     GT_U32                             ruleSize
)
{
    GT_U32      freeSpace[PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E];
    GT_U32      i,j; /* loop counters */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC *blockPtr;
    GT_U32      availbleRulePlaces;
    GT_U32      placeCapacity;

    for (j = 0; (j < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E); j++)
    {
        freeSpace[j] = 0;
    }

    for (i = 0; (i < pVTcamMngPtr->blocksAmount); i++)
    {
        blockPtr = &(pVTcamMngPtr->blockInfArr[i]);
        if (blockPtr->lookupId != lookupId)
        {
            continue;
        }
        for (j = 0; (j < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E); j++)
        {
            freeSpace[j] += blockPtr->freeSpace[j];
        }
    }

    availbleRulePlaces = 0;
    for (j = 0; (j < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E); j++)
    {
        placeCapacity = prvCpssDxChVirtualTcamDbSegmentTableRowCapacity(
                j, ruleSize);
            availbleRulePlaces += (freeSpace[j] * placeCapacity);
    }

    return availbleRulePlaces;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCountFreeBlocksSpace
*
* DESCRIPTION:
*       Count space in free blocks (not mapped to any lookups).
*
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       ruleSize        - rule Size (measured in 10-byte units)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       amount of rules that can be allocated
*       without moving rules of other vTCAMs
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableCountFreeBlocksSpace
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC *pVTcamMngPtr,
    IN     GT_U32                             ruleSize
)
{
    GT_U32      i; /* loop counter */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC *blockPtr;
    GT_U32      blocksAmount;
    GT_U32      availbleRulePlaces;
    GT_U32      placeCapacity;

    blocksAmount = 0;
    for (i = 0; (i < pVTcamMngPtr->blocksAmount); i++)
    {
        blockPtr = &(pVTcamMngPtr->blockInfArr[i]);
        if (blockPtr->lookupId !=
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS)
        {
            continue;
        }
        blocksAmount ++;
    }

    placeCapacity = prvCpssDxChVirtualTcamDbSegmentTableRowCapacity(
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, ruleSize);
    availbleRulePlaces =
        (blocksAmount * pVTcamMngPtr->tcamSegCfg.lookupRowAlignment * placeCapacity);

    return availbleRulePlaces;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCountUsedSpacePerTypesInVTcam
*
* DESCRIPTION:
*       Count used space per segment columns type of and size of using rules in given vTCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       bitmapPtr       - (pointer to) bitmap that contains 1
*                         for any row that contain any amount of free space
*       vTcamCfgPtr     - (pointer to) VTCAM configuration structure
*       neededFreeSpace - needed Free Space in the same block row
*       segmentColumns  - type of segment columns using per row
*       stopThreshold   - if found such amount of rows - stop conting
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Amount of counted segment rows (may be stopped due to stopThreshold).
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableCountUsedSpacePerTypesInVTcam
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     GT_U32                                                     *bitmapPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC                *vTcamCfgPtr,
    IN     GT_BOOL                                                    neededFreeSpace,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              segmentColumns,
    IN     GT_U32                                                     stopThreshold
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;      /* segment pointer             */
    PRV_CPSS_AVL_TREE_PATH                          path;         /* tree iterator               */
    GT_VOID                                         *dbEntryPtr;  /* segment void pointer        */
    PRV_CPSS_AVL_TREE_SEEK_ENT                      seekType;     /* seek type                   */
    GT_U32                                          rowsCount;    /* rows count                  */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   foundSegmentColumns;/* found segment Columns */
    GT_U32                                          horzBlocksInRow; /* horzontal Blocks In Row  */
    GT_U32                                          horzBlockIdx;    /* horzontal Block Index    */
    GT_U32                                          rowIdx;       /* row index                   */
    GT_U32                                          rowHigh;      /* row index high bound        */
    GT_U32                                          freeSpaceBit; /* bit from birmap             */

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    rowsCount = 0;
    seekType = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;
    while (GT_TRUE == prvCpssAvlPathSeek(
            vTcamCfgPtr->segmentsTree, seekType, path, &dbEntryPtr))
    {
        seekType = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        segPtr   = dbEntryPtr;
        for (horzBlockIdx = 0; (horzBlockIdx < horzBlocksInRow); horzBlockIdx++)
        {
            foundSegmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, horzBlockIdx);
            if (foundSegmentColumns != segmentColumns) continue;

            if (neededFreeSpace == GT_FALSE)
            {
                rowsCount += segPtr->rowsAmount;
                if (rowsCount >= stopThreshold)
                {
                    return rowsCount;
                }
            }
            else
            {
                rowHigh = segPtr->rowsBase + segPtr->rowsAmount;
                for (rowIdx = segPtr->rowsBase; (rowIdx < rowHigh); rowIdx++)
                {
                    freeSpaceBit = prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapGet(
                        pVTcamMngPtr, bitmapPtr, rowIdx, horzBlockIdx);
                    if (freeSpaceBit)
                    {
                        rowsCount ++;
                        if (rowsCount >= stopThreshold)
                        {
                            return rowsCount;
                        }
                    }
                }
            }
        }
    }
    return rowsCount;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCountUsedSpacePerTypes
*
* DESCRIPTION:
*       Count used space per segment columns type of and size of using rules.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       bitmapPtr       - (pointer to) bitmap that contains 1
*                         for any row that contain any amount of free space
*       neededFreeSpace - needed Free Space in the same block row
*       lookupId        - lookup Id
*       ruleSize        - rule Size (measured in 10-byte units)
*       segmentColumns  - type of segment columns using per row
*       stopThreshold   - if found such amount of rows - stop conting
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Amount of counted segment rows (may be stopped due to stopThreshold).
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 prvCpssDxChVirtualTcamDbSegmentTableCountUsedSpacePerTypes
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     GT_U32                                                     *bitmapPtr,
    IN     GT_BOOL                                                    neededFreeSpace,
    IN     GT_U32                                                     lookupId,
    IN     GT_U32                                                     ruleSize,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              segmentColumns,
    IN     GT_U32                                                     stopThreshold
)
{
    GT_U32 vTcamId;                                               /* vTCAM Id                    */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC     *vTcamCfgPtr; /* vTCAM Configuration pointer */
    GT_U32                                          rowsCount;    /* rows count                  */
    GT_U32                                          vTcamLookupId;/* vTCAM lookup Id             */
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT            enumRuleSize; /* enum rule size              */

    enumRuleSize = (CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E + ruleSize - 1);
    rowsCount = 0;
    for (vTcamId = 0; (vTcamId < pVTcamMngPtr->vTcamCfgPtrArrSize); vTcamId++)
    {
        vTcamCfgPtr = pVTcamMngPtr->vTcamCfgPtrArr[vTcamId];
        if (vTcamCfgPtr == NULL) continue;

        if (vTcamCfgPtr->tcamInfo.ruleSize != enumRuleSize) continue;

        vTcamLookupId = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_MAC(
            vTcamCfgPtr->tcamInfo.clientGroup, vTcamCfgPtr->tcamInfo.hitNumber);
        if (vTcamLookupId != lookupId) continue;

        rowsCount += prvCpssDxChVirtualTcamDbSegmentTableCountUsedSpacePerTypesInVTcam(
            pVTcamMngPtr, bitmapPtr, vTcamCfgPtr,
            neededFreeSpace, segmentColumns, stopThreshold);
        if (rowsCount >= stopThreshold)
        {
            return rowsCount;
        }
    }
    return rowsCount;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMergeAndSplitStep
*
* DESCRIPTION:
*       Splits or merges primary tree segments to be vTCAM space.
*       Parts containing same rows (but different columns) merged to one segment.
*       Parts remaining after merge splitted to the segments.
*       When the last segment more than needed for request it also splitted
*       to two segments.
*       Rule Amounts not updated.
*       SegmentColumnsMap merged only in different horizontal blocks NONE + OTHER.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       treeId          - primary tree for allocation.
*       first           - GT_TRUE - start from first segment
*                         GT_FALSE - start from bookmark
*       bookmarkPtr     - (pointer to) bookmark, see "first"
*
* OUTPUTS:
*       bookmarkPtr     - (pointer to) bookmark, saved after function call
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*       GT_NO_MORE               - not needed split
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMergeAndSplitStep
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       treeId,
    IN     GT_BOOL                                                    first,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC            *bookmarkPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1; /* segment   */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr2; /* segment   */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr ; /* segment   */
    GT_STATUS               rc;          /* return code                    */
    GT_BOOL                 seeked;      /* node seeked                    */
    PRV_CPSS_AVL_TREE_PATH  path;        /* tree iterator                  */
    GT_VOID_PTR             dbEntryPtr;  /* pointer to entry in DB         */
    GT_U32                  horzBlocksInRow; /* horizontal Blocks In Row   */
    GT_U32                  horzBlocksIndex; /* horizontal Blocks Index    */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns1;/*Columns*/
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns2;/*Columns*/
    GT_BOOL                 verticalMerge; /* vertical Merge               */
    GT_U32                  blockBase;     /* block Base                   */

    if (first != GT_FALSE)
    {
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr1 = dbEntryPtr;
        if (seeked == GT_FALSE)
        {
            return /* do not treat as error */ GT_NO_MORE;
        }
    }
    else
    {
        seeked = prvCpssAvlPathFind(
                treeId, PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
                bookmarkPtr, path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr1 = dbEntryPtr;
        if (seeked == GT_FALSE)
        {
            return /* do not treat as error */ GT_NO_MORE;
        }
    }
    seeked = prvCpssAvlPathSeek(
            treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
            path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    segPtr2 = dbEntryPtr;
    if (seeked == GT_FALSE)
    {
        return /* do not treat as error */ GT_NO_MORE;
    }

    verticalMerge = GT_FALSE;
    while (seeked != GT_FALSE)
    {
        if ((segPtr1->rowsBase + segPtr1->rowsAmount) > segPtr2->rowsBase)
        {
            /* segments have common rows */
            break;
        }

        if (((segPtr1->rowsBase + segPtr1->rowsAmount) == segPtr2->rowsBase)
            && (segPtr1->segmentColumnsMap == segPtr2->segmentColumnsMap))
        {
            blockBase = segPtr2->rowsBase -
                (segPtr2->rowsBase % pVTcamMngPtr->tcamSegCfg.lookupRowAlignment);
            if (segPtr1->rowsBase >= blockBase)
            {
                /* vertical merge */
                verticalMerge = GT_TRUE;
                break;
            }
        }

        segPtr1 = segPtr2;
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr2 = dbEntryPtr;
    }

    if (seeked == GT_FALSE)
    {
        /* no update needed */
        return /* do not treat as error */ GT_NO_MORE;
    }

    cpssOsMemCpy(bookmarkPtr, segPtr1, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));

    if (verticalMerge != GT_FALSE)
    {
        PRV_PRIMARY_TREE_MRG_SPLIT_TRACE_MAC((
            "Vertical before %d(%d, 0x%X)+%d(%d, 0x%X)",
            segPtr1->rowsBase, segPtr1->rowsAmount, segPtr1->segmentColumnsMap,
            segPtr2->rowsBase, segPtr2->rowsAmount, segPtr2->segmentColumnsMap));

        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(treeId, segPtr1);
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(treeId, segPtr2);
        segPtr1->rowsAmount += segPtr2->rowsAmount;
        cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(treeId, segPtr1);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }

        PRV_PRIMARY_TREE_MRG_SPLIT_TRACE_MAC((
        "Vertical result %d(%d, 0x%X)",
        segPtr1->rowsBase, segPtr1->rowsAmount, segPtr1->segmentColumnsMap));

        return GT_OK;
    }

    PRV_PRIMARY_TREE_MRG_SPLIT_TRACE_MAC((
        "Horizontal before %d(%d, 0x%X)+%d(%d, 0x%X)",
        segPtr1->rowsBase, segPtr1->rowsAmount, segPtr1->segmentColumnsMap,
        segPtr2->rowsBase, segPtr2->rowsAmount, segPtr2->segmentColumnsMap));

    /* split part before common                       */
    /* assumed segPtr1->rowsBase <= segPtr2->rowsBase */
    if (segPtr1->rowsBase < segPtr2->rowsBase)
    {
        segPtr = segPtr1;
        rc =  prvCpssDxChVirtualTcamDbSegmentTableCropSegmentVertical(
            pVTcamMngPtr, treeId, NULL /*tree2*/,
            segPtr,
            segPtr->rowsBase,
            (segPtr2->rowsBase - segPtr->rowsBase),
            GT_TRUE /*putBack*/,
            NULL /*lowSegPtrPtr*/,
            &segPtr1 /*highSegPtrPtr*/);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
        if (segPtr1 == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        PRV_PRIMARY_TREE_MRG_SPLIT_TRACE_MAC((
            "Horizontal -- vertical1 %d(%d, 0x%X)+%d(%d, 0x%X)",
            segPtr->rowsBase, segPtr->rowsAmount, segPtr->segmentColumnsMap,
            segPtr1->rowsBase, segPtr1->rowsAmount, segPtr1->segmentColumnsMap));
    }

    /* split part after common                        */
    /* assumed segPtr1->rowsBase == segPtr2->rowsBase */
    if (segPtr1->rowsAmount != segPtr2->rowsAmount)
    {
        /* swap pointers if needed to cause segPtr1->rowsAmount be greater */
        if (segPtr1->rowsAmount < segPtr2->rowsAmount)
        {
            segPtr  = segPtr1;
            segPtr1 = segPtr2;
            segPtr2 = segPtr;
        }
        rc =  prvCpssDxChVirtualTcamDbSegmentTableCropSegmentVertical(
            pVTcamMngPtr, treeId, NULL /*tree2*/,
            segPtr1,
            segPtr1->rowsBase,
            segPtr2->rowsAmount,
            GT_TRUE /*putBack*/,
            NULL /*lowSegPtrPtr*/,
            NULL /*highSegPtrPtr*/);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
        PRV_PRIMARY_TREE_MRG_SPLIT_TRACE_MAC((
            "Horizontal -- vertical2 %d(%d, 0x%X)",
            segPtr1->rowsBase, segPtr1->rowsAmount, segPtr1->segmentColumnsMap));
    }

    /* update */
    prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(treeId, segPtr1);
    prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(treeId, segPtr2);

    /* merge segPtr1 and segPtr2 */
    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);
    for (horzBlocksIndex = 0; (horzBlocksIndex < horzBlocksInRow); horzBlocksIndex++)
    {
        segmentColumns1 = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            segPtr1->segmentColumnsMap, horzBlocksIndex);
        segmentColumns2 = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            segPtr2->segmentColumnsMap, horzBlocksIndex);
        if (segmentColumns1
            == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                segPtr1->segmentColumnsMap, horzBlocksIndex, segmentColumns2);
        }
    }

    cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(treeId, segPtr1);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    PRV_PRIMARY_TREE_MRG_SPLIT_TRACE_MAC((
        "Horizontal -- result %d(%d, 0x%X)",
        segPtr1->rowsBase, segPtr1->rowsAmount, segPtr1->segmentColumnsMap));
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableLuOrderedTreeMerge
*
* DESCRIPTION:
*       Splits or merges lookup ordered segments to be vTCAM space.
*       Parts containing same rows (but different columns) merged to one segment.
*       Parts remaining after merge splitted to new segments.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       allocRequestPtr - (pointer to) allocation request structure
*       treeId          - primary tree for allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableLuOrderedTreeMerge
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       treeId
)
{
    GT_STATUS               rc;          /* return code                        */
    GT_BOOL                 first;       /* start iteration                    */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC seg;      /* segment       */

    /* split and merge free spaces */
    first = GT_TRUE;
    while (1)
    {
        rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMergeAndSplitStep(
            pVTcamMngPtr, treeId, first, &seg /*bookmark*/);
        first = GT_FALSE;
        if (rc == GT_NO_MORE)
        {
            break; /* normal finish */
        }
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableUpdateRuleAmounts
*
* DESCRIPTION:
*       Update ruleAmount field in all segments of the tree.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       treeId          - segments tree.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDxChVirtualTcamDbSegmentTableUpdateRuleAmounts
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       treeId
)
{
    PRV_CPSS_AVL_TREE_SEEK_ENT                      seekType;     /* seek type                   */
    PRV_CPSS_AVL_TREE_PATH                          path;         /* tree path                   */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;      /* segment pointer             */
    GT_VOID                                         *dbEntryPtr;  /* void segment pointer        */
    GT_U32                                          rulesPerRow;  /* rules Per Row               */
    GT_U32                                          horzBlocksInRow; /* horzontal Blocks per row */
    GT_U32                                          horzBlockIdx;    /* horzontal Block Index    */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns;  /* segment columns          */

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    seekType = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;
    while (GT_FALSE != prvCpssAvlPathSeek(
            treeId, seekType, path, &dbEntryPtr))
    {
        seekType = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        segPtr   = dbEntryPtr;

        rulesPerRow = 0;
        for (horzBlockIdx = 0; (horzBlockIdx < horzBlocksInRow); horzBlockIdx++)
        {
            segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, horzBlockIdx);

            if (segmentColumns
                == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E) continue;

            rulesPerRow += prvCpssDxChVirtualTcamDbSegmentTableRowCapacity(
                segmentColumns, segPtr->ruleSize);
        }
        segPtr->rulesAmount = (segPtr->rowsAmount * rulesPerRow);
    }
}

/*******************************************************************************
* segmentTableVTcamBaseLogicalIndexUpdate
*
* DESCRIPTION:
*       update the baseLogicalIndex in the segments of the vtcam
*       and the rulesAmount of the vtcam
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void segmentTableVTcamBaseLogicalIndexUpdate
(
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC                *tcamCfgPtr
)
{
    PRV_CPSS_AVL_TREE_SEEK_ENT  seekMode;
    PRV_CPSS_AVL_TREE_PATH  currentPath;
    GT_VOID_PTR             dbEntryPtr;/* pointer to entry in DB */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntryPtr;
    GT_U32    totalRules = 0;

    seekMode = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;

    while (GT_TRUE == prvCpssAvlPathSeek(tcamCfgPtr->segmentsTree,seekMode,
                currentPath, &dbEntryPtr))
    {
        seekMode = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        segEntryPtr = dbEntryPtr;

        segEntryPtr->baseLogicalIndex  = totalRules;
        totalRules += segEntryPtr->rulesAmount;
    }

    /* update the total rules amount */
    tcamCfgPtr->rulesAmount = totalRules;

    return ;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableLogToPhyIndexTabGenerate
*
* DESCRIPTION:
*       Generate table for conversion logical indexes to physical indexs of rules.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       treeId          - tree of vTCAM segments.
*
* OUTPUTS:
*       rulePhysicalIndexArr - buffer to fulfill by physical indexes
*
* RETURNS:
*       None
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvCpssDxChVirtualTcamDbSegmentTableLogToPhyIndexTabGenerate
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                        treeId,
    OUT    GT_U16                                      *rulePhysicalIndexArr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;  /* segment       */
    GT_VOID_PTR                                     dbEntryPtr;  /* pointer to entry in DB       */
    GT_BOOL                                         seeked;       /* node seeked                 */
    PRV_CPSS_AVL_TREE_PATH                          path;         /* tree iterator               */
    GT_U32                                          horzBlocksInRow; /* horzontal Blocks per row */
    GT_U32                                          horzBlockIdx;    /* horzontal Block Index    */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns;  /* segment columns          */
    GT_U32                                          columnsBitmap;   /* columns bitmap           */
    GT_U32                                          oneColumnBitmap; /* one column bitmap        */
    GT_U32                                          ruleColMask; /* rule Columns mask             */
    GT_U32                                          column;      /* TCAM column                   */
    GT_U32                                          w;           /* work variable                 */
    GT_U32                                          row;         /* row  index                    */
    GT_U32                                          rowBound;    /* row  Bound index              */
    GT_U32                                          logIndex;    /* rule log Index                */

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    seeked = prvCpssAvlPathSeek(
            treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    segPtr = dbEntryPtr;

    while (seeked != GT_FALSE)
    {
        columnsBitmap = 0;
        for (horzBlockIdx = 0; (horzBlockIdx < horzBlocksInRow); horzBlockIdx++)
        {
            segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, horzBlockIdx);

            if (segmentColumns
                == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E) continue;

            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_BITMAP_OF_COLUMNS_MAC(
                segmentColumns, oneColumnBitmap);

            columnsBitmap |=
                (oneColumnBitmap
                 << (horzBlockIdx * PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_BITBAPS_WIDTH_CNS));
        }
        ruleColMask = ((1 << segPtr->ruleSize) - 1);
        rowBound = segPtr->rowsBase + segPtr->rowsAmount;
        logIndex = segPtr->baseLogicalIndex;
        for (row = segPtr->rowsBase; (row < rowBound); row++)
        {
            for (column = 0;
                  (column < pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount);
                  column += prvCpssDxChVirtualTcamDbSegmentTableRuleAlignmentGet(segPtr->ruleSize))
            {
                w = ruleColMask << column;
                if ((columnsBitmap & w) == w)
                {
                    rulePhysicalIndexArr[logIndex] =
                        (GT_U16)((row * pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount) + column);
                    logIndex ++;
                }
            }
        }
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr = dbEntryPtr;
    }
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableMoveRulesToNewLoayout
*
* DESCRIPTION:
*       Generate table for conversion logical indexes to physical indexs of rules.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       rulesAmount             - amount of rules.
*       ruleSize                - rule size mwasured in 10-byte rules
*       ruleValidBmpPtr         - (pointer to) bitmap of valid rules.
*       srcRulePhysicalIndexArr - (pointer to) array of source rules physical indexes
*       dstRulePhysicalIndexArr - (pointer to) array of destination rules physical indexes
*
* OUTPUTS:
*       rulePhysicalIndexArr - buffer to fulfill by physical indexes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableMoveRulesToNewLoayout
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *pVTcamMngPtr,
    IN     GT_U32                                      rulesAmount,
    IN     GT_U32                                      ruleSize,
    IN     GT_U32                                      *ruleValidBmpPtr,
    IN     GT_U16                                      *srcRulePhysicalIndexArr,
    IN     GT_U16                                      *dstRulePhysicalIndexArr
)
{
    GT_U32        vTcamMngId;/* vTCAM Manager Id */
    GT_STATUS     rc;        /* return code          */
    GT_U32        lowIdx;    /* low index            */
    GT_U32        highBound; /* high bound           */
    GT_U32        idx;       /* index                */
    GT_COMP_RES   compRes;   /* comparization result */

    vTcamMngId = pVTcamMngPtr->vTcamMngId;

    /* to avoid compiler warnings only */
    lowIdx    = 0;
    compRes   = GT_EQUAL;

    highBound = 0;
    while (highBound < rulesAmount)
    {
        /* find move sequence - all source greater than all desinations or all less */
        /* sequence low index  */
        for (idx = highBound; (idx < rulesAmount); idx++)
        {
            if (srcRulePhysicalIndexArr[idx] != dstRulePhysicalIndexArr[idx])
            {
                lowIdx = idx;
                compRes =
                    ((srcRulePhysicalIndexArr[idx] < dstRulePhysicalIndexArr[idx])
                     ? GT_SMALLER : GT_GREATER);
                break;
            }
        }

        /* next sequence not found */
        if (idx >= rulesAmount) break;

        /* sequence high bound         */
        /* default to updated if found */
        highBound = rulesAmount;
        if (compRes == GT_SMALLER)
        {
            for (idx = (lowIdx + 1); (idx < rulesAmount); idx++)
            {
                if (srcRulePhysicalIndexArr[idx] >= dstRulePhysicalIndexArr[idx])
                {
                    highBound = idx;
                    break;
                }
            }
        }
        else /*compRes == GT_GREATER */
        {
            for (idx = (lowIdx + 1); (idx < rulesAmount); idx++)
            {
                if (srcRulePhysicalIndexArr[idx] <= dstRulePhysicalIndexArr[idx])
                {
                    highBound = idx;
                    break;
                }
            }
        }
        /* sequence move  */
        if (compRes == GT_SMALLER)
        {
            /* destiation indexes are greater - begin from the greatest      */
            /* the condition supports descending loop ending at (lowIdx == 0)*/
            for (idx = (highBound - 1); ((idx + 1) >= (lowIdx + 1)); idx--)
            {
                /* bypass invalid rules */
                if ((ruleValidBmpPtr[idx / 32] & (1 << (idx % 32))) == 0) continue;

                /* move rule */
                rc = pVTcamMngPtr->tcamSegCfg.ruleMoveFuncPtr(
                    (GT_VOID*)((char*)0 + vTcamMngId) /*cookiePtr*/,
                    srcRulePhysicalIndexArr[idx],
                    dstRulePhysicalIndexArr[idx],
                    ruleSize);
                if (rc != GT_OK) return rc;
            }
        }
        else /*compRes == GT_GREATER */
        {
            /* destiation indexes are less - begin from the least */
            for (idx = lowIdx; (idx < highBound); idx++)
            {
                /* bypass invalid rules */
                if ((ruleValidBmpPtr[idx / 32] & (1 << (idx % 32))) == 0) continue;

                /* move rule */
                rc = pVTcamMngPtr->tcamSegCfg.ruleMoveFuncPtr(
                    (GT_VOID*)((char*)NULL + vTcamMngId) /*cookiePtr*/,
                    srcRulePhysicalIndexArr[idx],
                    dstRulePhysicalIndexArr[idx],
                    ruleSize);
                if (rc != GT_OK) return rc;
            }
        }
    }
    return GT_OK;
}

/* data for 40 and 50 byte rules */
/*
 * Typedef: struct SEG_AND_SIZE_CHECK_STC
 *
 * Description:
 *     This structure of DB free segments conversion ways:
 *
 * Fields:
 *       usedRuleSize        - size of rules (measured in 10 byte units) being moved
 *       segmentColumns      - busy columns with rules being moved
 *       sourceRowsAmount    - amount of rows with rules being moved
 *       spentFreeFullRows   - amount of fully free rows used for conversion
 *       freedRowsRemainder  - type of freed rows as segment columns
 *       freedRowsAmount     - amount of freed as segment columns for reqired role places
 *       remainderRowsType   - type of remained segment columns
 *       remainderRowsAmount - amount of remained rows with segment columns
 *
 *  Comments:
 *     sourceRowsAmount (of segmentColumns typed rows used by old rules)
 *     + spentFreeFullRows (of fully free rows)
 *     =>
 *     freedRowsAmount (of places for new rules in freedRowsRemainder rows)
 *     + (reoreded old rules in remainderRowsAmount of remainderRowsType rows)
 *
 */
typedef struct
{
    GT_U32                                        usedRuleSize;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns;
    GT_U32                                        sourceRowsAmount;
    GT_U32                                        spentFreeFullRows;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT freedRowsRemainder;
    GT_U32                                        freedRowsAmount;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT remainderRowsType;
    GT_U32                                        remainderRowsAmount;
} SEG_AND_SIZE_CHECK_STC;

typedef SEG_AND_SIZE_CHECK_STC *SEG_AND_SIZE_CHECK_PTR;

static SEG_AND_SIZE_CHECK_STC places_for_60_byte_rules[] =
{
    /* 3 * (40 free + 20 used) => (1 * (3 * 20 used)) + (2 * 60 free) */
    {2, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E,  3, 0,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, 2,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, 1},
    /* 3 * (40 free + (2 * 10 used)) => (1 * (6 * 10 used)) + (2 * 60 free) */
    {1, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E,  3, 0,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, 2,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, 1},
    /* 6 * (50 free + 10 used) => (1 * (6 * 10 used)) + (5 * 60 free) */
    {1, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E,   6, 0,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, 5,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, 1},
};
static GT_U32 places_for_60_byte_rules_size =
    (sizeof(places_for_60_byte_rules) / sizeof(places_for_60_byte_rules[0]));

static SEG_AND_SIZE_CHECK_STC places_for_50_byte_rules[] =
{
    /* 3 * (40 free + 20 used) => (1 * (3 * 20)) + (2 * free_rows) */
    {2, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E,  3, 0,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, 2,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, 1},
    /* 5 * (40 free + (2 * 10 used)) => (1 * (6 * 10)) + (4 * (50 free + 10 used)) */
    {1, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E,  5, 0,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E, 4,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, 1},
    /* (1 * (6 * 10 used) + 5 free_rows) => 6 * (50 free + 10 used) */
    {1, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, 1, 5,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E, 6,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, 0}
};
static GT_U32 places_for_50_byte_rules_size =
    (sizeof(places_for_50_byte_rules) / sizeof(places_for_50_byte_rules[0]));

static SEG_AND_SIZE_CHECK_STC places_for_40_byte_rules[] =
{
    /* This conversion frees FULL ROW and MUST be the first in this array */
    /* 2 * (50 free + 10 used) => (40 free + (2 * 10 used)) + (1 * free_row) */
    {1, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E,   2, 0,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E, 1,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, 1},

    /* (1 * (6 * 10 used) + 2 free_rows) => 3 * (40 free + (2 * 10 used)) */
    {1, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, 1, 2,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E, 3,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, 0},
    /* (1 * (3 * 20 used) + 2 free_rows) => 3 * (40 free + (1 * 20 used)) */
    {2, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, 1, 2,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E, 3,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, 0}
};
static GT_U32 places_for_40_byte_rules_size =
    (sizeof(places_for_40_byte_rules) / sizeof(places_for_40_byte_rules[0]));

/* covert TCAM Rule Size to goal segment type */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_CONVERT_GOAL_MAC(      \
    _rule_size, _goal)                                                   \
    switch (_rule_size)                                                  \
    {                                                                    \
        case 1: case 2: case 3: case 6:                                  \
            _goal = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E;     \
            break;                                                       \
        case 4:                                                          \
            _goal = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E;    \
            break;                                                       \
        case 5:                                                          \
            _goal = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E;   \
            break;                                                       \
        default: return _error_retVal;                                   \
    }

/* Queues support */

/* maximal amount of segments used in one conversion */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_SIZE_CNS 6

typedef struct
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_PTR  seg_ptr[
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_SIZE_CNS];
    GT_U32  used_rows[PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_SIZE_CNS];
    GT_U32 head;  /* index of valid or == tail */
    GT_U32 tail;  /* index of free space or == head */
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_STC;

#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_RESET_MAC(_que)           \
    {_que.head = 0; _que.tail = 0;}

/* add to queue tail */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_TAIL_ADD_MAC(_que, _s, _u) \
    {                                                                      \
        GT_U32 __t = ((_que.tail + 1)                                      \
            % PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_SIZE_CNS);           \
        if (__t != _que.head) /* que is not full */                        \
        {                                                                  \
            _que.seg_ptr[_que.tail] = _s;                                  \
            _que.used_rows[_que.tail] = _u;                                \
            _que.tail = __t;                                               \
        }                                                                  \
    }

/* add to queue head */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_HEAD_ADD_MAC(_que, _s, _u) \
    {                                                                      \
        GT_U32 __h = (                                                     \
        (_que.head + PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_SIZE_CNS - 1) \
            % PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_SIZE_CNS);           \
        if (__h != _que.tail) /* que is not full */                        \
        {                                                                  \
            _que.head = __h;                                               \
            _que.seg_ptr[_que.head] = _s;                                  \
            _que.used_rows[_que.head] = _u;                                \
        }                                                                  \
    }

/* delete from que */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_DEL_MAC(_que, _s, _u)     \
    {                                                                     \
        if (_que.head != _que.tail) /* que is not empty */                \
        {                                                                 \
            _s = _que.seg_ptr[_que.head];                                 \
            _u = _que.used_rows[_que.head];                               \
            _que.head = ((_que.head + 1)                                  \
                % PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_SIZE_CNS);      \
        }                                                                 \
        else                                                              \
        {                                                                 \
            _s = NULL;                                                    \
            _u = 0;                                                       \
        }                                                                 \
    }

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableColumnsConvertionGet
*
* DESCRIPTION:
*       Get rule for convertion segment columns dimentions.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       goalSegColumns    - goal segment columns type
*       srcSegColumns     - source segment columns type
*       usedRuleSize      - rule size (measured in 10-byte units)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Found conversion rule or null.
*
* COMMENTS:
*
*******************************************************************************/
static SEG_AND_SIZE_CHECK_STC* prvCpssDxChVirtualTcamDbSegmentTableColumnsConvertionGet
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   goalSegColumns,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   srcSegColumns,
    IN     GT_U32                                          usedRuleSize
)
{
    /* static */
    static GT_BOOL initialized = GT_FALSE;
    /* arrays support rule size 1..6 and all source segment column types */
    static SEG_AND_SIZE_CHECK_PTR cnv_60bytes[7][PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E];
    static SEG_AND_SIZE_CHECK_PTR cnv_50bytes[7][PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E];
    static SEG_AND_SIZE_CHECK_PTR cnv_40bytes[7][PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E];

    if (initialized == GT_FALSE)
    {
        SEG_AND_SIZE_CHECK_STC                        *places_for_rules_ptr;
        GT_U32                                        places_for_rules_size;
        GT_U32                                        i;
        GT_U32                                        ruleSize;
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT columns;

        cpssOsMemSet(cnv_60bytes, 0, sizeof(cnv_60bytes));
        places_for_rules_ptr = places_for_60_byte_rules;
        places_for_rules_size = places_for_60_byte_rules_size;
        for (i = 0; (i < places_for_rules_size); i++)
        {
            columns = places_for_rules_ptr[i].segmentColumns;
            ruleSize = places_for_rules_ptr[i].usedRuleSize;
            cnv_60bytes[ruleSize][columns] = &(places_for_rules_ptr[i]);
        }

        cpssOsMemSet(cnv_50bytes, 0, sizeof(cnv_50bytes));
        places_for_rules_ptr = places_for_50_byte_rules;
        places_for_rules_size = places_for_50_byte_rules_size;
        for (i = 0; (i < places_for_rules_size); i++)
        {
            columns = places_for_rules_ptr[i].segmentColumns;
            ruleSize = places_for_rules_ptr[i].usedRuleSize;
            cnv_50bytes[ruleSize][columns] = &(places_for_rules_ptr[i]);
        }

        cpssOsMemSet(cnv_40bytes, 0, sizeof(cnv_40bytes));
        places_for_rules_ptr = places_for_40_byte_rules;
        places_for_rules_size = places_for_40_byte_rules_size;
        for (i = 0; (i < places_for_rules_size); i++)
        {
            columns = places_for_rules_ptr[i].segmentColumns;
            ruleSize = places_for_rules_ptr[i].usedRuleSize;
            cnv_40bytes[ruleSize][columns] = &(places_for_rules_ptr[i]);
        }

        initialized = GT_TRUE;
    }

    if ((usedRuleSize > 6))
    {
        return NULL;
    }

    if (srcSegColumns >= PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E)
    {
        return NULL;
    }

    switch (goalSegColumns)
    {
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E:
            return cnv_60bytes[usedRuleSize][srcSegColumns];
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E:
            return cnv_40bytes[usedRuleSize][srcSegColumns];
        case PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E:
            return cnv_50bytes[usedRuleSize][srcSegColumns];
        default: return NULL;
    }
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCheckFreableSpace
*
* DESCRIPTION:
*       Checks freable space availibility for allocation.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr         - (pointer to) VTCAM Manager structure
*       bitmapPtr            - (pointer to) bitmap that contains 1
*                              for any row that contain any amount of free space
*       lookupId             - lookup Id
*       ruleSize             - rule Size (measured in 10-byte units)
*       requiredRulesAmount  - amount of rules needed to be allocated.
*
* OUTPUTS:
*        GT_TRUE - can be allocted, otherwise GT_FALSE.
*
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_BOOL prvCpssDxChVirtualTcamDbSegmentTableCheckFreableSpace
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     GT_U32                                                     *bitmapPtr,
    IN     GT_U32                                                     lookupId,
    IN     CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT                       ruleSize,
    IN     GT_U32                                                     requiredRulesAmount
)
{
    GT_U32      i; /* loop counters */
    SEG_AND_SIZE_CHECK_STC *places_for_rules_ptr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC *blockPtr;
    GT_U32      freableRulePlaces;
    GT_U32      freableFullRows;
    GT_U32      freeFullRows;
    GT_U32      placeCapacity;
    GT_U32      free40BytePlaces;
    GT_U32      free50BytePlaces;
    GT_U32      rowsThreshold;
    GT_U32      spentFreeRows;
    GT_U32      rowsAmount;
    GT_U32      maxBundlesAmount;
    GT_U32      bundlesAmount;
    GT_U32      bundlesBySpentFreeRows;
    GT_U32      bundlesByRequiredRows;
    GT_U32      foundBundlesAmount;
    GT_U32      places_for_rules_size;
    GT_BOOL     neededFreeSpace;
    GT_U32      ruleSizeVal;
    GT_U32      freedPlacesPerBundle;
    GT_U32      freedFullRowsPerBundle;
    GT_U32      spentFullRowsPerBundle;

    /* defaults to be overridden */

    ruleSizeVal = (ruleSize - CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E + 1);

    switch (ruleSizeVal)
    {
        case 6:
        case 3:
        case 2:
        case 1:
            placeCapacity = prvCpssDxChVirtualTcamDbSegmentTableRowCapacity(
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E,
                    ruleSizeVal);
            free40BytePlaces = 0;
            free50BytePlaces = 0;
            for (i = 0; (i < pVTcamMngPtr->blocksAmount); i++)
            {
                blockPtr = &(pVTcamMngPtr->blockInfArr[i]);
                if (blockPtr->lookupId != lookupId)
                {
                    continue;
                }
                free40BytePlaces +=
                    blockPtr->freeSpace[PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E];
                free50BytePlaces +=
                    blockPtr->freeSpace[PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E];
            }
            freableRulePlaces = 0;
            /* move remainders of 6 50-byte places to one line - 5 lines free */
            freableRulePlaces += ((free50BytePlaces / 6) * 5 * placeCapacity);
            /* move remainders of 3 40-byte places to one line - 2 lines free */
            freableRulePlaces += ((free40BytePlaces / 3) * 2 * placeCapacity);
            return ((requiredRulesAmount <= freableRulePlaces) ? GT_TRUE : GT_FALSE);
        default:
            break;
    }

    freeFullRows = 0;
    for (i = 0; (i < pVTcamMngPtr->blocksAmount); i++)
    {
        blockPtr = &(pVTcamMngPtr->blockInfArr[i]);
        if (blockPtr->lookupId != lookupId)
        {
            continue;
        }
        freeFullRows += blockPtr->freeSpace[PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E];
    }

    freableRulePlaces   = 0;
    spentFreeRows       = 0;
    freableFullRows     = 0;
    switch (ruleSizeVal)
    {
        case 5:
            places_for_rules_ptr = places_for_50_byte_rules;
            places_for_rules_size = places_for_50_byte_rules_size;
            break;
        case 4:
            places_for_rules_ptr = places_for_40_byte_rules;
            places_for_rules_size = places_for_40_byte_rules_size;
            break;
        default:
            prvCpssDxChVirtualTcamDbSegmentTableException();
            return GT_FALSE; /* never occurs - cases 1-3,6 upper */
    }

    for (i = 0; (i < places_for_rules_size); i++)
    {
        /* Conversion rules that free Full Rows (if any) must be first in the list */
        /* to be processed before Conversion rules that spend  Full Rows           */

        /* full free rows already counted as possible rule places by caller */
        /* Only differnce can be added here.                                */
        freedPlacesPerBundle  = places_for_rules_ptr[i].freedRowsAmount;
        spentFullRowsPerBundle = places_for_rules_ptr[i].spentFreeFullRows;
        freedFullRowsPerBundle = 0;
        if ((places_for_rules_ptr[i].remainderRowsType
            == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
            && (places_for_rules_ptr[i].remainderRowsAmount != 0))
        {
            /* only for free 2*50 => 40 + 60 (remainderRowsAmount != 0) */
            freedFullRowsPerBundle = places_for_rules_ptr[i].remainderRowsAmount;
        }

        /* aproximate threshold - by requirement */
        bundlesByRequiredRows =
            ((requiredRulesAmount - (freableRulePlaces + freableFullRows))
             / (freedPlacesPerBundle + freedFullRowsPerBundle - spentFullRowsPerBundle));
        maxBundlesAmount = bundlesByRequiredRows;
        /* maxBundlesAmount increased due to deviding remainder lost */
        maxBundlesAmount ++;
        /* aproximate threshold - by spent free rows */
        bundlesBySpentFreeRows = 0;
        if (spentFullRowsPerBundle != 0)
        {
            bundlesBySpentFreeRows =
                ((freeFullRows + freableFullRows - spentFreeRows)
                 / spentFullRowsPerBundle);
            if (maxBundlesAmount > bundlesBySpentFreeRows)
            {
                maxBundlesAmount = bundlesBySpentFreeRows;
            }
        }
        rowsThreshold =
            (maxBundlesAmount * places_for_rules_ptr[i].sourceRowsAmount);
        neededFreeSpace =
            ((places_for_rules_ptr[i].segmentColumns
              == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
                ? GT_FALSE : GT_TRUE);
        /* results of counting */
        rowsAmount = prvCpssDxChVirtualTcamDbSegmentTableCountUsedSpacePerTypes(
            pVTcamMngPtr, bitmapPtr, neededFreeSpace, lookupId,
            places_for_rules_ptr[i].usedRuleSize,
            places_for_rules_ptr[i].segmentColumns,
            rowsThreshold);
        foundBundlesAmount = (rowsAmount / places_for_rules_ptr[i].sourceRowsAmount);
        bundlesAmount =
            ((maxBundlesAmount < foundBundlesAmount)
                ? maxBundlesAmount : foundBundlesAmount);
        freableRulePlaces   +=
            (bundlesAmount * freedPlacesPerBundle);
        freableFullRows     +=
            (bundlesAmount * freedFullRowsPerBundle);
        spentFreeRows       +=
            (bundlesAmount * spentFullRowsPerBundle);

        /* only validity check */
        if ((freeFullRows + freableFullRows) < spentFreeRows)
        {
           prvCpssDxChVirtualTcamDbSegmentTableException();
           PRV_DBG_TRACE(("SegmentTableCheckFreableSpace Exception"));
           return GT_FALSE;
        }
        if (requiredRulesAmount <=
              (freableRulePlaces + freableFullRows - spentFreeRows))
        {
            return GT_TRUE;
        }
    }

    return GT_FALSE;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamSplitForRearrange
*
* DESCRIPTION:
*       Split segment to crop from it part that all columns in the same
*       rows of block are free space. The cropped segment with it's neighbor
*       free space can be used for Rearrange.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       vTcamCfgPtr     - (pointer to) VTCAM configuration structure
*       bitmapPtr       - (pointer to) bitmap that contains 1
*                         for any row that contain any amount of free space
*       goalFreeColumns - the free space type that is a reordering goal
*                         (keep such free space)
*       segPtr          - (pointer to) segment for splitting
*
* OUTPUTS:
*       splittedPtr        - (pointer to) GT_TRUE  - segment spluitted
*                                         GT_FALSE - DB unchanged
*       segUseablePtrPtr   - (pointer to) (pointer to) cropped part of segment.
*                            NULL if nothing to crop.
*       horzBlockBmpPtr    - (pointer to) horizontal block bitmap
*                            wirh horizontal blocks having usable free space
*
*
*
* RETURNS:
*       GT_OK           - success, other - error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamSplitForRearrange
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC              *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC     *vTcamCfgPtr,
    IN     GT_U32                                          *bitmapPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   goalFreeColumns,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr,
    OUT    GT_BOOL                                         *splittedPtr,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC **segUseablePtrPtr,
    OUT    GT_U32                                          *horzBlockBmpPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   keptColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr2;
    GT_STATUS                                       rc;           /* return code                 */
    GT_U32                                          horzBlocksInRow; /* horzontal Blocks In Row  */
    GT_U32                                          horzBlockIdx;    /* horzontal Block Index    */
    GT_U32                                          rowIdx;       /* row index                   */
    GT_U32                                          rowHigh;      /* row index high bound        */
    GT_U32                                          freeSpaceBit; /* bit from birmap             */
    GT_U32                                          fullBlockBitmap; /* full Block Bitmap        */
    GT_U32                                          firstBlockBitmap; /* first Block Bitmap      */
    GT_U32                                          blockBitmap;  /* block Bitmap                */
    GT_U32                                          firstRowBase; /* first Row Base              */
    GT_U32                                          breakRowBase; /* break Row Base              */

    /* default results */
    *splittedPtr      = GT_FALSE;
    *segUseablePtrPtr = NULL;
    *horzBlockBmpPtr  = 0;

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    keptColumns =
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_COMPLEMENT_MAC(goalFreeColumns);

    fullBlockBitmap = 0;
    for (horzBlockIdx = 0; (horzBlockIdx < horzBlocksInRow); horzBlockIdx++)
    {
        segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            segPtr->segmentColumnsMap, horzBlockIdx);
        if (segmentColumns
            == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
        {
            /* block not used in segment */
            continue;
        }
        if (segmentColumns
            == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
        {
            /* no neigbor space */
            continue;
        }

        if (segmentColumns == keptColumns)
        {
            /* even if the neigbor space is free we need keep it */
            continue;
        }
        fullBlockBitmap |= (1 << horzBlockIdx);
    }

    if (fullBlockBitmap == 0)
    {
        /* split is unusable */
        return GT_OK;
    }

    /* look for first row with free space */
    firstBlockBitmap = 0;
    firstRowBase     = 0xFFFFFFFF; /* for compiler only */
    rowHigh = segPtr->rowsBase + segPtr->rowsAmount;
    for (rowIdx = segPtr->rowsBase; (rowIdx < rowHigh); rowIdx++)
    {
        breakRowBase = 0;
        blockBitmap = 0;
        for (horzBlockIdx = 0; (horzBlockIdx < horzBlocksInRow); horzBlockIdx++)
        {
            if ((fullBlockBitmap & (1 << horzBlockIdx)) == 0) continue;

            freeSpaceBit = prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapGet(
                pVTcamMngPtr, bitmapPtr, rowIdx, horzBlockIdx);
            blockBitmap |= (freeSpaceBit << horzBlockIdx);
        }
        if ((blockBitmap != 0) && (firstBlockBitmap == 0))
        {
            firstBlockBitmap = blockBitmap;
            firstRowBase = rowIdx;
        }
        if (blockBitmap != firstBlockBitmap)
        {
            break;
        }
    }
    breakRowBase = rowIdx;

    *horzBlockBmpPtr = firstBlockBitmap;

    if (firstBlockBitmap == 0)
    {
        /* no usable rows found */
        return GT_OK;
    }

    if ((firstRowBase == segPtr->rowsBase)
        && (breakRowBase == (segPtr->rowsBase + segPtr->rowsAmount)))
    {
        *splittedPtr      = GT_FALSE;
        *segUseablePtrPtr = segPtr;
        return GT_OK;
    }

    *splittedPtr      = GT_TRUE;

    rc = prvCpssDxChVirtualTcamDbSegmentTableCropSegmentVertical(
        pVTcamMngPtr, vTcamCfgPtr->segmentsTree, NULL /*tree2*/,
        segPtr, firstRowBase, (breakRowBase - firstRowBase), GT_TRUE /*putBack*/,
        &segPtr1, &segPtr2);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    *segUseablePtrPtr = segPtr;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamHorzSplitByBitmap
*
* DESCRIPTION:
*       Horizontal split segment by given bitmap.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       vTcamCfgPtr     - (pointer to) VTCAM configuration structure
*       bitmapPtr       - (pointer to) bitmap that contains 1
*                         for any row that contain any amount of free space
*       goalFreeColumns - the free space type that is a reordering goal
*                         (keep such free space)
*       segPtr          - (pointer to) segment for splitting
*
* OUTPUTS:
*       splittedPtr        - (pointer to) GT_TRUE  - segment spluitted
*                                         GT_FALSE - DB unchanged
*       segUseablePtrPtr   - (pointer to) (pointer to) cropped part of segment.
*                            NULL if nothing to crop.
*
*
* RETURNS:
*       GT_OK           - success, other - error.
*
* COMMENTS:
*       If segment covers more then one horizontal block it will be
*       vertical splitted to allow undependant using the horizontal parts.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamHorzSplitByBitmap
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC              *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC     *vTcamCfgPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr,
    IN     GT_U32                                          horzBlockBmp,
    OUT    GT_BOOL                                         *splittedPtr,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC **segUseablePtrPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1;
    GT_STATUS                                       rc;           /* return code                 */

    /* default results */
    *splittedPtr      = GT_FALSE;
    *segUseablePtrPtr = NULL;

    rc = prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorzBlockBitmap(
        pVTcamMngPtr, vTcamCfgPtr->segmentsTree, NULL /*tree2*/,
        segPtr, horzBlockBmp, GT_TRUE /*putBack*/, &segPtr1);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on CropSegmentHorzBlockBitmap");
    }

    *splittedPtr = (segPtr1 == NULL) ? GT_FALSE : GT_TRUE;
    *segUseablePtrPtr = segPtr;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeAddSegsWithHoles
*
* DESCRIPTION:
*       Adding segments with usable holes to tree.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       vTcamCfgPtr     - (pointer to) VTCAM configuration structure
*       bitmapPtr       - (pointer to) bitmap that contains 1
*                         for any row that contain any amount of free space
*       goalFreeColumns - the free space type that is a reordering goal
*                         (keep such free space)
*       srcVTcamSegTree - tree to add found segments to it
*       dstVTcamSegTree - tree to add replacing segments
*       tmpFreeRowsTree - tree of temporary free full rows
*       spaceThreshold  - amount of required neighbor free space
*                         mesured in goalFreeColumns units.
*
* OUTPUTS:
*       spaceFoundPtr   - (pointer to) found neighbor free space
*                         mesured in goalFreeColumns.
*
*
* RETURNS:
*       GT_OK           - success, other - error.
*
* COMMENTS:
*       Treated conversions not requiring additional empty rows.
*       Supported conversions:
*       - (6 * rows_5)  ==> (1 * rows_all) + [5 * rows_none]
*       - (2 * rows_5)  ==> (1 * rows_45)  + [1 * rows_none]
*       - (3 * rows_45) ==> (1 * rows_all) + [2 * rows_none] <both 10 and 20 bytes>
*       - (6 * rows_45) ==> (1 * rows_all) + (5 * rows_5) <10 bytes>
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeAddSegsWithHoles
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC            *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC   *vTcamCfgPtr,
    IN     GT_U32                                        *bitmapPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT goalFreeColumns,
    IN     PRV_CPSS_AVL_TREE_ID                          srcVTcamSegTree,
    IN     PRV_CPSS_AVL_TREE_ID                          dstVTcamSegTree,
    IN     PRV_CPSS_AVL_TREE_ID                          tmpFreeRowsTree,
    IN     GT_U32                                        spaceThreshold,
    OUT    GT_U32                                        *spaceFoundPtr
)
{
    GT_STATUS                                       rc;           /* return code */
    GT_U32                                          ruleSize;     /* rule size in 10-byte units  */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;      /* segment pointer             */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1;     /* segment pointer             */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr2;     /* segment pointer             */
    GT_VOID                                         *dbEntryPtr;  /* segment void pointer        */
    GT_BOOL                                         splitted;     /*splitted                     */
    GT_BOOL                                         splitted1;    /*splitted                     */
    GT_U32                                          horzBlockBmp; /* horisontal blocks with free space*/
    GT_U32                                          horzBlocksInRow; /*horz Blocks In Row        */
    GT_U32                                          horzBlockIdx; /*horz Blocks Index            */
    SEG_AND_SIZE_CHECK_STC                          *place_ptr;   /* coversion type rule         */
    GT_U32                                          srcRowsRemainder[
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E]; /*remainder of rows per type */
    GT_U32                                          i;             /* loop index*/
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns;/* source columns type        */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns1;/* source columns type        */
    GT_U32                                          freedSegRows;  /* freed Segment Rows         */
    GT_U32                                          remainderSegRows;/* remainder Segment Rows   */
    GT_U32                                          segRows;       /* Segment Row                */
    GT_U32                                          boundles;      /* boundles                   */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_STC segQueue[
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E];
    GT_U32                                          usedRows;      /* usedRows                   */
    GT_U32                                          dstRows;       /* rows in dst segments       */
    GT_U32                                          cleanUpRows;   /* cleanUpRows in queue       */
    GT_U32                                          rulesPerRow;  /* rules Per Row               */
    PRV_CPSS_AVL_TREE_ITERATOR_STC                  treeIterator; /* tree Iterator               */
    GT_U32                                          fullFreeRowsRemainder; /*fullFreeRowsRemainder */

    ruleSize =
        (vTcamCfgPtr->tcamInfo.ruleSize - CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E + 1);

    *spaceFoundPtr = 0;

    for (i = 0; (i < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E); i++)
    {
        srcRowsRemainder[i] = 0;
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_RESET_MAC(segQueue[i]);
    }

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    segPtr1      = NULL;
    PRV_CPSS_AVL_TREE_ITERATOR_INIT_MAC(
        treeIterator, vTcamCfgPtr->segmentsTree, GT_TRUE);
    while (GT_TRUE == prvCpssAvlIterationDo(&treeIterator, &dbEntryPtr))
    {
        /* bypass already processed segments */
        if (NULL != prvCpssAvlSearch(srcVTcamSegTree, dbEntryPtr)) continue;
        segPtr   = dbEntryPtr;

        /* output parameters - got values here for comlier only */
        splitted     = GT_FALSE;
        segPtr1      = NULL;
        horzBlockBmp = 0;
        rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamSplitForRearrange(
            pVTcamMngPtr, vTcamCfgPtr, bitmapPtr,
            goalFreeColumns, segPtr, &splitted, &segPtr1, &horzBlockBmp);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on SegmentTableVTcamSplitForRearrange");
        }

        if (segPtr1 == NULL) continue;
        if (horzBlockBmp == 0)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            continue; /* should never occur */
        }
        segPtr = segPtr1;

        /* find first used horizontal block index                   */
        /* and treat this horizontal block only                     */
        for (horzBlockIdx = 0; (horzBlockIdx < horzBlocksInRow); horzBlockIdx++)
        {
            if ((horzBlockBmp & (1 << horzBlockIdx)) != 0) break;
        }
        horzBlockBmp = (1 << horzBlockIdx);

        /* crop usable horizontal blocks */
        rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamHorzSplitByBitmap(
            pVTcamMngPtr, vTcamCfgPtr, segPtr, horzBlockBmp, &splitted1, &segPtr1);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on SegmentTableVTcamHorzSplitByBitmap");
        }
        if (splitted1 != GT_FALSE)
        {
            splitted = GT_TRUE;
        }
        segPtr = segPtr1;

        if (splitted != GT_FALSE)
        {
            PRV_CPSS_AVL_TREE_ITERATOR_FIND_MAC(
                treeIterator, segPtr1, PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E);
            PRV_CPSS_AVL_TREE_ITERATOR_RESEEK_MAC(treeIterator);
        }

        segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
            segPtr1->segmentColumnsMap, horzBlockIdx);

        place_ptr = prvCpssDxChVirtualTcamDbSegmentTableColumnsConvertionGet(
            goalFreeColumns, segmentColumns, ruleSize);
        if (place_ptr == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_INFORMATION_MAC(
                "VTcamRearrangeAddSegsWithHoles bug: not supported segmentColumns(no conversion)");
            continue;
        }
        if (place_ptr->spentFreeFullRows != 0)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_INFORMATION_MAC(
                "VTcamRearrangeAddSegsWithHoles bug: not supported segmentColumns(needed free rows)");
            continue;
        }

        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            srcVTcamSegTree, segPtr1);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
        }

        srcRowsRemainder[segmentColumns] += segPtr1->rowsAmount;
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_TAIL_ADD_MAC(
            segQueue[segmentColumns], segPtr1, 0);

        if (srcRowsRemainder[segmentColumns] >= place_ptr->sourceRowsAmount)
        {
            boundles = srcRowsRemainder[segmentColumns] / place_ptr->sourceRowsAmount;
            fullFreeRowsRemainder = 0;
            freedSegRows = 0;
            remainderSegRows = 0;

            if (place_ptr->freedRowsRemainder
                 == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
            {
                fullFreeRowsRemainder += ((place_ptr->freedRowsAmount) * boundles);
            }
            else
            {
                freedSegRows = ((place_ptr->freedRowsAmount) * boundles);
            }

            if (place_ptr->remainderRowsType
                 == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
            {
                fullFreeRowsRemainder += ((place_ptr->remainderRowsAmount) * boundles);
            }
            else
            {
                remainderSegRows = ((place_ptr->remainderRowsAmount) * boundles);
            }

            dstRows = freedSegRows + remainderSegRows + fullFreeRowsRemainder;

            /* for compiler only, (freedSegRows + remainderSegRows) != 0 */
            segPtr = NULL;
            usedRows = 0;
            while ((freedSegRows + remainderSegRows + fullFreeRowsRemainder) != 0)
            {
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_DEL_MAC(
                    segQueue[segmentColumns], segPtr, usedRows);
                if (segPtr == NULL)
                {
                    prvCpssDxChVirtualTcamDbSegmentTableException();
                    CPSS_LOG_ERROR_AND_RETURN_MAC(
                        GT_FAIL,
                        "SegmentTableVTcamRearrangeAddSegsWithHoles bug - empty queue");
                }

                /* recalculate horzBlockIdx for retrieved segment */
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
                    segPtr->segmentColumnsMap, horzBlockIdx, segmentColumns1);
                if (segmentColumns != segmentColumns1)
                {
                    prvCpssDxChVirtualTcamDbSegmentTableException();
                    CPSS_LOG_ERROR_AND_RETURN_MAC(
                        GT_FAIL,
                        "SegmentTableVTcamRearrangeAddSegsWithHoles bug - other segmentColumns");
                }

                if (freedSegRows != 0)
                {
                    segPtr2 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
                    if (segPtr2 == NULL)
                    {
                        prvCpssDxChVirtualTcamDbSegmentTableException();
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
                    }
                    cpssOsMemCpy(segPtr2, segPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
                    segPtr2->segmentColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                        segPtr2->segmentColumnsMap, horzBlockIdx, place_ptr->freedRowsRemainder);
                    rulesPerRow = prvCpssDxChVirtualTcamDbSegmentTableRowCapacity(
                        place_ptr->freedRowsRemainder, ruleSize);
                    segPtr2->rowsBase = segPtr->rowsBase + usedRows;
                    segRows = segPtr->rowsAmount - usedRows;
                    if (segRows > freedSegRows)
                    {
                        segRows = freedSegRows;
                    }
                    segPtr2->rowsAmount = segRows;
                    segPtr2->rulesAmount = (segRows * rulesPerRow);
                    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                        dstVTcamSegTree, segPtr2);
                    if (rc != GT_OK)
                    {
                        cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
                        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
                    }
                    usedRows += segRows;
                    freedSegRows -= segRows;
                }

                if (usedRows >= segPtr->rowsAmount) continue;

                if (remainderSegRows != 0)
                {
                    segPtr2 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
                    if (segPtr2 == NULL)
                    {
                        prvCpssDxChVirtualTcamDbSegmentTableException();
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
                    }
                    cpssOsMemCpy(segPtr2, segPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
                    segPtr2->segmentColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                        segPtr2->segmentColumnsMap, horzBlockIdx, place_ptr->remainderRowsType);
                    rulesPerRow = prvCpssDxChVirtualTcamDbSegmentTableRowCapacity(
                        place_ptr->remainderRowsType, ruleSize);
                    segPtr2->rowsBase = segPtr->rowsBase + usedRows;
                    segRows = segPtr->rowsAmount - usedRows;
                    if (segRows > remainderSegRows)
                    {
                        segRows = remainderSegRows;
                    }
                    segPtr2->rowsAmount = segRows;
                    segPtr2->rulesAmount = (segRows * rulesPerRow);
                    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                        dstVTcamSegTree, segPtr2);
                    if (rc != GT_OK)
                    {
                        cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
                        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
                    }
                    usedRows += segRows;
                    remainderSegRows -= segRows;
                }

                if (usedRows >= segPtr->rowsAmount) continue;

                if (fullFreeRowsRemainder != 0)
                {
                    segPtr2 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
                    if (segPtr2 == NULL)
                    {
                        prvCpssDxChVirtualTcamDbSegmentTableException();
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
                    }
                    cpssOsMemCpy(segPtr2, segPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
                    segPtr2->segmentColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                        segPtr2->segmentColumnsMap, horzBlockIdx,
                        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E);
                    segPtr2->rowsBase = segPtr->rowsBase + usedRows;
                    segRows = segPtr->rowsAmount - usedRows;
                    if (segRows > fullFreeRowsRemainder)
                    {
                        segRows = fullFreeRowsRemainder;
                    }
                    segPtr2->rowsAmount = segRows;
                    segPtr2->rulesAmount = 0xFFFFFFFF;
                    segPtr2->ruleSize = 0xFF;
                    segPtr2->baseLogicalIndex = 0xFFFFFFFF;
                    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                        tmpFreeRowsTree, segPtr2);
                    if (rc != GT_OK)
                    {
                        cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
                        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
                    }
                    usedRows += segRows;
                    fullFreeRowsRemainder -= segRows;
                }
            }

            /* put back the remainder */
            if ((segPtr != NULL) && (usedRows < segPtr->rowsAmount))
            {
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_HEAD_ADD_MAC(
                    segQueue[segmentColumns], segPtr, usedRows);
            }

            /* count found usable space */
            *spaceFoundPtr += (place_ptr->freedRowsAmount * boundles);
            if ((place_ptr->remainderRowsType
                == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
                && (place_ptr->remainderRowsAmount != 0))
            {
                /* freed lines without destination segment */
                *spaceFoundPtr += (place_ptr->remainderRowsAmount * boundles);
            }

            /* clean up the queue redundant rows */
            cleanUpRows =
                (boundles * place_ptr->sourceRowsAmount) - dstRows;

            while (cleanUpRows > 0)
            {
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_DEL_MAC(
                    segQueue[segmentColumns], segPtr, usedRows);
                if (segPtr == NULL)
                {
                    prvCpssDxChVirtualTcamDbSegmentTableException();
                    CPSS_LOG_ERROR_AND_RETURN_MAC(
                        GT_FAIL,
                        "SegmentTableVTcamRearrangeAddSegsWithHoles bug - empty queue");
                }
                segRows = segPtr->rowsAmount - usedRows;
                if (cleanUpRows >= segRows)
                {
                    cleanUpRows -= segRows;
                    /* drop all of dequed segment */
                    continue;
                }
                else
                {
                    /* put back adjusted segment and break */
                    usedRows += cleanUpRows;
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_HEAD_ADD_MAC(
                        segQueue[segmentColumns], segPtr, usedRows);
                    cleanUpRows = 0; /*break*/
                }
            }
            /* remainded amount of source rows in the queue */
            srcRowsRemainder[segmentColumns] -= (boundles * place_ptr->sourceRowsAmount);
        }

        if (*spaceFoundPtr >= spaceThreshold)
        {
            /* required freeed rows amount reached */
            break;
        }
    }

    /* Cleanup last segments not used freing */
    for (i = 0; (i < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E); i++)
    {
        if (srcRowsRemainder[i] == 0) continue;
        while (1)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_QUEUE_DEL_MAC(
                segQueue[i], segPtr, usedRows);
            if (segPtr == NULL) break;
            if (usedRows != 0)
            {
                /* split segment */
                rulesPerRow = (segPtr->rulesAmount / segPtr->rowsAmount);

                /* prepare the high part */
                segPtr2 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
                if (segPtr2 == NULL)
                {
                    prvCpssDxChVirtualTcamDbSegmentTableException();
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
                }
                cpssOsMemCpy(segPtr2, segPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
                segPtr2->rowsBase = segPtr->rowsBase + usedRows;
                segPtr2->rowsAmount = segPtr->rowsAmount - usedRows;
                segPtr2->rulesAmount = (rulesPerRow * segPtr2->rowsAmount);

                /* the low part - remove from trees before update */
                prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                    vTcamCfgPtr->segmentsTree, segPtr);
                prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                    srcVTcamSegTree, segPtr);

                /* update  the low part */
                segPtr->rowsAmount = usedRows;
                segPtr->rulesAmount = (rulesPerRow * segPtr->rowsAmount);

                /* the low part - add to trees after update */
                rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                    vTcamCfgPtr->segmentsTree, segPtr);
                if (rc != GT_OK)
                {
                    cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
                }
                rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                    srcVTcamSegTree, segPtr);
                if (rc != GT_OK)
                {
                    cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
                }

                /* the high part - add to VTCAM tree only */
                rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                    vTcamCfgPtr->segmentsTree, segPtr2);
                if (rc != GT_OK)
                {
                    cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr2);
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
                }
            }
            else
            {
                /* drop segment from replaced segments tree */
                prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
                    srcVTcamSegTree, segPtr);
            }
        }
    }

    return GT_OK;
}

/* internal state maschine data of function                           */
/* prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeRetrieveFreeRows */
/* iterator of free rows segments used by rearranging                 */
typedef struct
{
    enum
    {
        PRV_REARRANGE_FREE_ROWS_INIT_E = 0,
        PRV_REARRANGE_FREE_ROWS_TEMP_TREE_E,
        PRV_REARRANGE_FREE_ROWS_GLOBAL_TREE_INIT_E,
        PRV_REARRANGE_FREE_ROWS_GLOBAL_TREE_NEXT_E,
        PRV_REARRANGE_FREE_ROWS_NO_MORE_E
    }                                   stage;
    PRV_CPSS_AVL_TREE_PATH              path;
} PRV_REARRANGE_FREE_ROWS_ITERATOR_STC;

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeRetrieveFreeRows
*
* DESCRIPTION:
*       Retrieve next full-rows segment from temporary free full rows
*       tree or from permanent free space tree.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr        - (pointer to) VTCAM Manager structure
*       lookupId            - lookup Id.
*       tmpFreeRowsTree     - tree of temporary free full rows
*       freeRowsIteratorPtr - (pointer to)free Rows Iterator
*
* OUTPUTS:
*       segPtrPtr   - (pointer to)(pointer to) found full-rows segment
*
* RETURNS:
*       GT_TRUE - found next, GT_FALSE - no more.
*
* COMMENTS:
*
*******************************************************************************/
static GT_BOOL prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeRetrieveFreeRows
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC               *pVTcamMngPtr,
    IN     GT_U32                                           lookupId,
    IN     PRV_CPSS_AVL_TREE_ID                             tmpFreeRowsTree,
    INOUT  PRV_REARRANGE_FREE_ROWS_ITERATOR_STC             *freeRowsIteratorPtr,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC  **segPtrPtr
)
{
    GT_BOOL                                         found;
    GT_VOID                                         *dataPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC segKey;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;
    GT_U32                                          fullRowHorzBlockIdx;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns;

    while (1) /* each stage returns result or pass to ther stage */
    {
        switch (freeRowsIteratorPtr->stage)
        {
            case PRV_REARRANGE_FREE_ROWS_INIT_E:
                found = prvCpssAvlPathSeek(
                    tmpFreeRowsTree, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
                    freeRowsIteratorPtr->path, &dataPtr);
                if (found == GT_FALSE)
                {
                    freeRowsIteratorPtr->stage = PRV_REARRANGE_FREE_ROWS_GLOBAL_TREE_INIT_E;
                    continue;
                }
                freeRowsIteratorPtr->stage = PRV_REARRANGE_FREE_ROWS_TEMP_TREE_E;
                *segPtrPtr = dataPtr;
                return GT_TRUE;

            case PRV_REARRANGE_FREE_ROWS_TEMP_TREE_E:
                found = prvCpssAvlPathSeek(
                    tmpFreeRowsTree, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                    freeRowsIteratorPtr->path, &dataPtr);
                if (found == GT_FALSE)
                {
                    freeRowsIteratorPtr->stage = PRV_REARRANGE_FREE_ROWS_GLOBAL_TREE_INIT_E;
                    continue;
                }
                /* freeRowsIteratorPtr->stage unchanged */
                *segPtrPtr = dataPtr;
                return GT_TRUE;

            case PRV_REARRANGE_FREE_ROWS_GLOBAL_TREE_INIT_E:
                /* start search of free full rous in blocks mapped to the same lookup */
                cpssOsMemSet(&segKey, 0, sizeof(segKey));
                segKey.lookupId = lookupId;
                segKey.segmentColumnsMap =
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                    segKey.segmentColumnsMap, 0, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E);
                found = prvCpssAvlPathFind(
                        pVTcamMngPtr->segFreePrefTree,
                        PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
                        &segKey, freeRowsIteratorPtr->path, &dataPtr);
                segPtr = dataPtr;

                if ((found == GT_FALSE)
                    || (segPtr->lookupId != lookupId))
                {
                    freeRowsIteratorPtr->stage = PRV_REARRANGE_FREE_ROWS_NO_MORE_E;
                    return GT_FALSE;
                }

                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
                    segPtr->segmentColumnsMap, fullRowHorzBlockIdx, segmentColumns);
                if (segmentColumns !=
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
                {
                    freeRowsIteratorPtr->stage = PRV_REARRANGE_FREE_ROWS_NO_MORE_E;
                    return GT_FALSE;
                }
                freeRowsIteratorPtr->stage = PRV_REARRANGE_FREE_ROWS_GLOBAL_TREE_NEXT_E;
                *segPtrPtr = dataPtr;
                return GT_TRUE;

            case PRV_REARRANGE_FREE_ROWS_GLOBAL_TREE_NEXT_E:
                found = prvCpssAvlPathSeek(
                    pVTcamMngPtr->segFreePrefTree, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                    freeRowsIteratorPtr->path, &dataPtr);
                segPtr = dataPtr;

                if ((found == GT_FALSE)
                    || (segPtr->lookupId != lookupId))
                {
                    freeRowsIteratorPtr->stage = PRV_REARRANGE_FREE_ROWS_NO_MORE_E;
                    return GT_FALSE;
                }

                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
                    segPtr->segmentColumnsMap, fullRowHorzBlockIdx, segmentColumns);
                if (segmentColumns !=
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
                {
                    freeRowsIteratorPtr->stage = PRV_REARRANGE_FREE_ROWS_NO_MORE_E;
                    return GT_FALSE;
                }
                freeRowsIteratorPtr->stage = PRV_REARRANGE_FREE_ROWS_GLOBAL_TREE_NEXT_E;
                *segPtrPtr = dataPtr;
                return GT_TRUE;

            case PRV_REARRANGE_FREE_ROWS_NO_MORE_E:
            default: return GT_NO_MORE;
        }
    }
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeAddSegsWithFreeRows
*
* DESCRIPTION:
*       Adding segments with whole rows to src tree and dst tree,
*       adding free segments with whole rows to dst tree.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       vTcamCfgPtr     - (pointer to) VTCAM configuration structure
*       goalFreeColumns - the free space type that is a reordering goal
*                         (keep such free space)
*       srcVTcamSegTree - tree to add found segments to it
*       dstVTcamSegTree - tree to add replacing segments
*       tmpFreeRowsTree - tree of temporary free full rows
*       spaceThreshold  - amount of required neighbor free space
*                         mesured in goalFreeColumns units.
*                         It is a bound of difference of freed places amount
*                         ans spent full-free-rows amount.
*
* OUTPUTS:
*       spaceFoundPtr   - (pointer to) found neighbor free space
*                         mesured in goalFreeColumns.
*                         It is a difference of freed places amount
*                         ans spent full-free-rows amount.
*
*
* RETURNS:
*       GT_OK           - success, other - error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeAddSegsWithFreeRows
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC            *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC   *vTcamCfgPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT goalFreeColumns,
    IN     PRV_CPSS_AVL_TREE_ID                          srcVTcamSegTree,
    IN     PRV_CPSS_AVL_TREE_ID                          dstVTcamSegTree,
    IN     PRV_CPSS_AVL_TREE_ID                          tmpFreeRowsTree,
    IN     GT_U32                                        spaceThreshold,
    OUT    GT_U32                                        *spaceFoundPtr
)
{
    GT_STATUS                                       rc;           /* return code */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;      /* segment pointer             */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1;     /* segment pointer             */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segFreePtr;  /* segment pointer             */
    PRV_CPSS_AVL_TREE_PATH                          path;        /* tree iterator               */
    GT_U32                                          vTcamLookupId;/* lookupId                    */
    GT_VOID                                         *dbEntryPtr;  /* segment void pointer        */
    GT_BOOL                                         splitted;     /*splitted                     */
    GT_U32                                          horzBlockBmp; /* horisontal blocks with free space*/
    GT_U32                                          horzBlocksInRow; /*horz Blocks In Row        */
    GT_U32                                          horzBlockIdx; /*horz Blocks Index            */
    GT_U32                                          fullRowHorzBlockIdx; /*horz Blocks Index     */
    SEG_AND_SIZE_CHECK_STC                          *place_ptr;   /* coversion type rule         */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns;/* source columns type        */
    GT_BOOL                                         found;         /* found                      */
    GT_U32                                          freeLuRows;    /* free Rows per lookup       */
    GT_U32                                          ruleSize;      /* ruleSize in 10-byte units  */
    GT_U32                                          boundlesThreshold; /* boundles Threshold     */
    GT_U32                                          boundlesRemainder;/* boundles Remainder      */
    GT_U32                                          freeSegRows;   /* freed Segment Rows         */
    GT_U32                                          rulesPerDstRow;/* rules Per Dst Row          */
    GT_U32                                          freeRowsRemainder; /*remainder of free rows  */
    GT_U32                                          freeRowsSpent;   /*spent free rows           */
    PRV_CPSS_AVL_TREE_ITERATOR_STC                  treeIterator; /* tree Iterator               */
    GT_U32                                          segHorzBlocks;/* Segment Horizontal blocks   */
    GT_U32                                          segMaxRows;  /* Segment maximal rows amount  */
    GT_U32                                          freedPerBundle;/* places freed Per Bundle    */
    PRV_REARRANGE_FREE_ROWS_ITERATOR_STC            freeRowsIterator; /* full-free-rows Iterator */
    PRV_CPSS_AVL_TREE_SEEK_ENT                      seekType;      /* seek Type                  */

    *spaceFoundPtr = 0;

    if (goalFreeColumns == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
    {
        /* nothing to do */
        return GT_OK;
    }

    vTcamLookupId = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_MAC(
        vTcamCfgPtr->tcamInfo.clientGroup, vTcamCfgPtr->tcamInfo.hitNumber);

    ruleSize =
        (vTcamCfgPtr->tcamInfo.ruleSize - CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E + 1);

    rulesPerDstRow = prvCpssDxChVirtualTcamDbSegmentTableRowCapacity(
        goalFreeColumns, ruleSize);

    horzBlocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
        / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    /* total free full rows in blocks mapped for the same lookup */
    freeLuRows = prvCpssDxChVirtualTcamDbSegmentTableCountAvailableSpace(
        pVTcamMngPtr, vTcamLookupId, pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment/*ruleSize*/);
    /* and free rows in temporary tree */
    seekType = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;
    while (GT_FALSE != prvCpssAvlPathSeek(
            tmpFreeRowsTree, seekType, path, &dbEntryPtr))
    {
        seekType = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        segPtr   = dbEntryPtr;
        /* temporary full free rows segments cover only one horizontal block */
        freeLuRows += segPtr->rowsAmount;
    }

    /* all supported coversions adding free rows            */
    /* convert one full row (10 * 6) or (20 * 3)            */
    /* several free rows to one column                      */
    /* (6 * 10) + (5 * free60) => (6 * (free50 + 10))       */
    /* (6 * 10) + (2 * free60) => (3 * (free40 + (2 * 10))) */
    /* (3 * 20) + (2 * free60) => (3 * (free40 + 20))       */
    /* so calculated once only for COLUMNS_ALL_E            */

    place_ptr = prvCpssDxChVirtualTcamDbSegmentTableColumnsConvertionGet(
        goalFreeColumns, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E, ruleSize);
    if (place_ptr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(
            GT_FAIL, "VTcamRearrangeAddSegsWithFreeRows: place_ptr == NULL");
    }

    if (place_ptr->sourceRowsAmount != 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(
            GT_FAIL, "VTcamRearrangeAddSegsWithFreeRows: sourceRowsAmount != 1");
    }

    if (place_ptr->freedRowsRemainder ==
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(
            GT_FAIL, "VTcamRearrangeAddSegsWithFreeRows: freedRowsRemainder is empty");
    }

    freedPerBundle = place_ptr->freedRowsAmount - place_ptr->spentFreeFullRows;
    boundlesThreshold =
        ((spaceThreshold + freedPerBundle - 1) / freedPerBundle);

    boundlesRemainder = (freeLuRows / place_ptr->spentFreeFullRows);
    if (boundlesRemainder > boundlesThreshold)
    {
        boundlesRemainder = boundlesThreshold;
    }

    if (boundlesRemainder == 0)
    {
        /* nothing to do */
        return GT_OK;
    }

    freeRowsIterator.stage = PRV_REARRANGE_FREE_ROWS_INIT_E;
    found = prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeRetrieveFreeRows(
        pVTcamMngPtr, vTcamLookupId, tmpFreeRowsTree,
        &freeRowsIterator, &segFreePtr);
    if (found == GT_FALSE)
    {
        /* should not occur, free rows found */
        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(
            GT_FAIL, "VTcamRearrangeAddSegsWithFreeRows: no segments with free space");
    }

    freeSegRows = 0;
    freeRowsRemainder = 0;
    freeRowsSpent = 0;

    /* loop by vTCAM segments */
    PRV_CPSS_AVL_TREE_ITERATOR_INIT_MAC(
        treeIterator, vTcamCfgPtr->segmentsTree, GT_TRUE);
    while (GT_TRUE == prvCpssAvlIterationDo(&treeIterator, &dbEntryPtr))
    {
        /* bypass already processed segments */
        if (NULL != prvCpssAvlSearch(srcVTcamSegTree, dbEntryPtr)) continue;
        segPtr   = dbEntryPtr;

        horzBlockBmp = 0;
        segHorzBlocks = 0;
        for (horzBlockIdx = 0; (horzBlockIdx < horzBlocksInRow); horzBlockIdx++)
        {
            segmentColumns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                segPtr->segmentColumnsMap, horzBlockIdx);
            if (segmentColumns == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
            {
                horzBlockBmp |= (1 << horzBlockIdx);
                segHorzBlocks ++;
            }
        }

        if (horzBlockBmp == 0) continue;

        rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamHorzSplitByBitmap(
            pVTcamMngPtr, vTcamCfgPtr, segPtr, horzBlockBmp, &splitted, &segPtr1);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on SegmentTableVTcamHorzSplitByBitmap");
        }
        segPtr = segPtr1;

        if (splitted != GT_FALSE)
        {
            /* upate tree iterator */
            PRV_CPSS_AVL_TREE_ITERATOR_FIND_MAC(
                treeIterator, segPtr1, PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E);
            PRV_CPSS_AVL_TREE_ITERATOR_RESEEK_MAC(treeIterator);
        }

        segMaxRows = (boundlesRemainder / segHorzBlocks);
        if (segMaxRows < segPtr->rowsAmount)
        {
            /* split last segment to needed and not needed parts */
            /* new part - high */
            rc = prvCpssDxChVirtualTcamDbSegmentTableCropSegmentVertical(
                pVTcamMngPtr, vTcamCfgPtr->segmentsTree, NULL /*tree2*/,
                segPtr, segPtr->rowsBase, segMaxRows, GT_TRUE /*putBack*/,
                NULL /*lowSegPtrPtr*/, NULL /*highSegPtrPtr*/);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
            }
            /* upate tree iterator */
            PRV_CPSS_AVL_TREE_ITERATOR_FIND_MAC(
                treeIterator, segPtr1, PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E);
        }

        /* replaced segment */
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            srcVTcamSegTree, segPtr);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
        }

        /* first replacing segment - at the rows of found vTcam segment */
        segPtr1 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
        if (segPtr1 == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }
        cpssOsMemCpy(segPtr1, segPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
        segPtr1->segmentColumnsMap =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
        for (horzBlockIdx = 0; (horzBlockIdx < horzBlocksInRow); horzBlockIdx++)
        {
            if ((horzBlockBmp & (1 << horzBlockIdx)) == 0) continue;
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                segPtr1->segmentColumnsMap, horzBlockIdx, place_ptr->freedRowsRemainder);
        }
        segPtr1->rulesAmount = rulesPerDstRow * segPtr1->rowsAmount;
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(dstVTcamSegTree, segPtr1);
        if (rc != GT_OK)
        {
            cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
        }
        freeSegRows += (segHorzBlocks * segPtr1->rowsAmount);

        /* additional replacing segments (used full free rows) */
        freeRowsRemainder +=
            (segPtr->rowsAmount * segHorzBlocks * place_ptr->spentFreeFullRows);
        freeRowsSpent +=
            (segPtr->rowsAmount * segHorzBlocks * place_ptr->spentFreeFullRows);

        /* if previous free space segment wasted - find the next */
        /* free sements assumed having single horizontal block   */
        while (freeRowsRemainder >= segFreePtr->rowsAmount)
        {
            /* update columns */
            segPtr1 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
            if (segPtr1 == NULL)
            {
                prvCpssDxChVirtualTcamDbSegmentTableException();
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
            }
            cpssOsMemCpy(segPtr1, segFreePtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
                segPtr1->segmentColumnsMap, fullRowHorzBlockIdx, segmentColumns);
            segPtr1->segmentColumnsMap =
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                segPtr1->segmentColumnsMap,
                fullRowHorzBlockIdx, place_ptr->freedRowsRemainder);

            /* add to destination segments */
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
                dstVTcamSegTree, segPtr1);
            if (rc != GT_OK)
            {
                cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
            }
            freeRowsRemainder -= segFreePtr->rowsAmount;
            freeSegRows       += segFreePtr->rowsAmount;

            if (freeSegRows >= (spaceThreshold + freeRowsSpent))
            {
                /* required amount reached */
                segFreePtr = NULL;
                break;
            }

            /* next full free rows segment */
            found = prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeRetrieveFreeRows(
                pVTcamMngPtr, vTcamLookupId, tmpFreeRowsTree,
                &freeRowsIterator, &segFreePtr);
            if (found == GT_FALSE)
            {
                /* no free full rows more */
                segFreePtr = NULL;
                break;
            }
        }

        if (segFreePtr == NULL) break;
    }

    /* free rows remainder */
    if ((segFreePtr != NULL) && (freeRowsRemainder > 0))
    {
        /* update columns */
        segPtr1 = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
        if (segPtr1 == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }
        cpssOsMemCpy(segPtr1, segFreePtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
        if (freeRowsRemainder < segPtr1->rowsAmount)
        {
            segPtr1->rowsAmount = freeRowsRemainder;
        }
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
            segPtr1->segmentColumnsMap, fullRowHorzBlockIdx, segmentColumns);
        segPtr1->segmentColumnsMap =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
            segPtr1->segmentColumnsMap,
            fullRowHorzBlockIdx, place_ptr->freedRowsRemainder);

        /* add to destination segments */
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            dstVTcamSegTree, segPtr1);
        if (rc != GT_OK)
        {
            cpssBmPoolBufFree(pVTcamMngPtr->segmentsPool, segPtr1);
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
        }
        freeRowsRemainder -= segFreePtr->rowsAmount;
        freeSegRows       += segFreePtr->rowsAmount;
    }

    *spaceFoundPtr += freeSegRows;
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrange
*
* DESCRIPTION:
*       Rearrange given vTCAM to convert freable space for allocation.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       vTcamCfgPtr     - (pointer to) VTCAM configuration structure
*       vTcamId         - vTcam Id
*       ruleSize        - rule Size (measured in 10-byte units) of required rules.
*       rulesAmount     - amount of required rules.
*
* OUTPUTS:
*       freedRulesPtr   - (pointer to) amount of rule places freed by rearranging
*
*
* RETURNS:
*       GT_OK           - success, other - error.
*
* COMMENTS:
*       vTCAM rearranged to free given amount of places for rules of given size.
*       It can reach this goal and stop leaving segments that can also be rearranged as
*       they were. It also can rearrange all possible segments and to free less rule places
*       than required. Both these results are success.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrange
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC            *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC   *vTcamCfgPtr,
    IN     GT_U32                                        vTcamId,
    IN     GT_U32                                        ruleSize,
    IN     GT_U32                                        rulesAmount,
    OUT    GT_U32                                        *freedRulesPtr
)
{
    GT_STATUS               rc;                   /* return code */
    PRV_CPSS_AVL_TREE_ID    srcVTcamSegTree;      /*  tree  */
    PRV_CPSS_AVL_TREE_ID    dstVTcamSegTree;      /*  tree  */
    PRV_CPSS_AVL_TREE_ID    tmpFreeRowsTree;      /*  tree  */
    GT_U32                  placeMultiplier;      /* converts 60-byte rows to 10,20,30 byte rules */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   goalFreeColumns;
    GT_U32                  goalColumnsThreshold; /* goal free Columns Threshold*/
    GT_U32                  goalColumnsFreed;     /* goal free Columns Freed    */
    GT_U32                  goalColumnsNum;       /* goal free Columns amount   */
    GT_U32                  *freePlacesBitmapPtr; /* bitmap of free places      */
    GT_U32                  lookupId;
    PRV_CPSS_AVL_TREE_SEEK_ENT                      seekType;     /* seek type  */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;      /* segment pointer             */
    PRV_CPSS_AVL_TREE_PATH                          path;         /* tree iterator               */
    GT_VOID                                         *dbEntryPtr;  /* segment void pointer        */
    GT_U16                                          *oldRulePhysicalIndexArr; /* old table log -> phy*/
    GT_U32                                          vTcamRuleSize; /* vTcam Rule Size            */

    /* clear, should be updated */
    *freedRulesPtr = 0;

    vTcamRuleSize =
        (vTcamCfgPtr->tcamInfo.ruleSize - CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E + 1);

    if (vTcamRuleSize > 2)
    {
        /* cannot be rearranged */
        return GT_OK;
    }

    if (vTcamCfgPtr->rulesAmount == 0)
    {
        /* cannot be rearranged - related also to vTCAM currentry being allocated */
        return GT_OK;
    }

    rc = prvCpssDxChVirtualTcamDbSegmentTableBestColumnsTypeGet(
        ruleSize, &goalFreeColumns, &placeMultiplier);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on ColumnsTypeGet");
    }

    srcVTcamSegTree  = NULL;
    dstVTcamSegTree  = NULL;
    tmpFreeRowsTree  = NULL;

    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFunc,
        pVTcamMngPtr->segNodesPool,
        &(srcVTcamSegTree));
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlTreeCreate");
    }
    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFunc,
        pVTcamMngPtr->segNodesPool,
        &(dstVTcamSegTree));
    if (rc != GT_OK)
    {
        prvCpssAvlTreeDelete(
            srcVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlTreeCreate");
    }

    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFunc,
        pVTcamMngPtr->segNodesPool,
        &(tmpFreeRowsTree));
    if (rc != GT_OK)
    {
        prvCpssAvlTreeDelete(
            srcVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        prvCpssAvlTreeDelete(
            dstVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlTreeCreate");
    }

    if (prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable != GT_FALSE)
    {
        cpssOsPrintf("vTcamCfgPtr->segmentsTree before reordering\n");
        prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree(vTcamCfgPtr->segmentsTree);
    }

    lookupId = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_MAC(
        vTcamCfgPtr->tcamInfo.clientGroup, vTcamCfgPtr->tcamInfo.hitNumber);

    freePlacesBitmapPtr =
        prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapAllocate(pVTcamMngPtr);
    if (freePlacesBitmapPtr == NULL)
    {
        prvCpssAvlTreeDelete(
            srcVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        prvCpssAvlTreeDelete(
            dstVTcamSegTree,
            (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
            (GT_VOID*)pVTcamMngPtr->segmentsPool);
        prvCpssAvlTreeDelete(
            tmpFreeRowsTree,
            (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
            (GT_VOID*)pVTcamMngPtr->segmentsPool);
        CPSS_LOG_ERROR_AND_RETURN_MAC(
            GT_OUT_OF_CPU_MEM, "failed on BlockRowBitmapAllocate");
    }
    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapReset(
        pVTcamMngPtr, freePlacesBitmapPtr);
    /* only 40-byte and 50-byte side holes actually supported */
    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSetForFreeRows(
        pVTcamMngPtr, freePlacesBitmapPtr, lookupId,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E,
        1 /*value*/);
    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSetForFreeRows(
        pVTcamMngPtr, freePlacesBitmapPtr, lookupId,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E,
        1 /*value*/);

    goalColumnsThreshold =
        ((rulesAmount + placeMultiplier - 1) / placeMultiplier);

    goalColumnsFreed = 0;

    goalColumnsNum = 0;
    rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeAddSegsWithHoles(
        pVTcamMngPtr, vTcamCfgPtr, freePlacesBitmapPtr,
        goalFreeColumns, srcVTcamSegTree, dstVTcamSegTree, tmpFreeRowsTree,
        goalColumnsThreshold, &goalColumnsNum);

    goalColumnsFreed     += goalColumnsNum;
    goalColumnsThreshold -=
        ((goalColumnsNum < goalColumnsThreshold)
            ? goalColumnsNum : goalColumnsThreshold);
    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapFree(freePlacesBitmapPtr);

    if (rc != GT_OK)
    {
        prvCpssAvlTreeDelete(
            srcVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        prvCpssAvlTreeDelete(
            dstVTcamSegTree,
            (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
            (GT_VOID*)pVTcamMngPtr->segmentsPool);
        prvCpssAvlTreeDelete(
            tmpFreeRowsTree,
            (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
            (GT_VOID*)pVTcamMngPtr->segmentsPool);
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on RearrangeAddSegsWithHoles");
    }

    if (prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable != GT_FALSE)
    {
        cpssOsPrintf("tmpFreeRowsTree tree\n");
        prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree(tmpFreeRowsTree);
    }

    if (goalColumnsThreshold > 0)
    {
        goalColumnsNum = 0;
        rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrangeAddSegsWithFreeRows(
            pVTcamMngPtr, vTcamCfgPtr, goalFreeColumns,
            srcVTcamSegTree, dstVTcamSegTree, tmpFreeRowsTree,
            goalColumnsThreshold, &goalColumnsNum);
        goalColumnsFreed     += goalColumnsNum;
        goalColumnsThreshold -=
            ((goalColumnsNum < goalColumnsThreshold)
                ? goalColumnsNum : goalColumnsThreshold);
        if (rc != GT_OK)
        {
            prvCpssAvlTreeDelete(
                srcVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
            prvCpssAvlTreeDelete(
                dstVTcamSegTree,
                (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
                (GT_VOID*)pVTcamMngPtr->segmentsPool);
            prvCpssAvlTreeDelete(
                tmpFreeRowsTree,
                (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
                (GT_VOID*)pVTcamMngPtr->segmentsPool);
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on RearrangeAddSegsWithFreeRows");
        }
    }

    *freedRulesPtr =
        (goalColumnsFreed * prvCpssDxChVirtualTcamDbSegmentTableRowCapacity(
            goalFreeColumns, ruleSize));

    if (prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable != GT_FALSE)
    {
        cpssOsPrintf("srcVTcamSegTree tree\n");
        prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree(srcVTcamSegTree);
        cpssOsPrintf("dstVTcamSegTree tree\n");
        prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree(dstVTcamSegTree);
    }

    prvCpssAvlTreeDelete(
        tmpFreeRowsTree, (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
        (GT_VOID*)pVTcamMngPtr->segmentsPool);

    /* remove replaced segments from vTCAM tree and insert to free space */
    while (GT_FALSE != prvCpssAvlPathSeek(
            srcVTcamSegTree, PRV_CPSS_AVL_TREE_SEEK_FIRST_E, path, &dbEntryPtr))
    {
        segPtr   = dbEntryPtr;

        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            srcVTcamSegTree, segPtr);

        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            vTcamCfgPtr->segmentsTree, segPtr);

        rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit(
            pVTcamMngPtr, segPtr);
        if (rc != GT_OK)
        {
            prvCpssAvlTreeDelete(
                srcVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
            prvCpssAvlTreeDelete(
                dstVTcamSegTree,
                (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
                (GT_VOID*)pVTcamMngPtr->segmentsPool);
            CPSS_LOG_ERROR_AND_RETURN_MAC(
                rc, "failed on TableChainFreeSegment");
        }
    }

    /* delete replaced segments tree (empty) before removing from free */
    prvCpssAvlTreeDelete(
        srcVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);

    /* merge free space segments */
    rc = prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMergeNeighbors(pVTcamMngPtr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(
            rc, "failed on FreeSpaceMergeNeighbors");
    }

    /* remove replaced segments from free space */
    seekType = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;
    while (GT_FALSE != prvCpssAvlPathSeek(
            dstVTcamSegTree, seekType, path, &dbEntryPtr))
    {
        seekType = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        segPtr   = dbEntryPtr;

        rc = prvCpssDxChVirtualTcamDbSegmentTableRemoveMemoryFromFree(
            pVTcamMngPtr, lookupId,
            segPtr->rowsBase, segPtr->rowsAmount, segPtr->segmentColumnsMap);
        if (rc != GT_OK)
        {
            prvCpssAvlTreeDelete(
                dstVTcamSegTree,
                (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
                (GT_VOID*)pVTcamMngPtr->segmentsPool);
            CPSS_LOG_ERROR_AND_RETURN_MAC(
                rc, "failed on FreeSpaceMergeNeighbors");
        }

        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            vTcamCfgPtr->segmentsTree, segPtr);
        if (rc != GT_OK)
        {
            prvCpssAvlTreeDelete(
                dstVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
            CPSS_LOG_ERROR_AND_RETURN_MAC(
                rc, "failed on TcamSegmentTableAvlItemInsert");
        }
    }

    /* delete replacing segments tree */
    prvCpssAvlTreeDelete(
        dstVTcamSegTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);

    /* final merge free space segments */
    rc = prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMergeNeighbors(pVTcamMngPtr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(
            rc, "failed on FreeSpaceMergeNeighbors");
    }

    /* update vTcamId and ruleSize fields */
    seekType = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;
    while (GT_FALSE != prvCpssAvlPathSeek(
            vTcamCfgPtr->segmentsTree, seekType, path, &dbEntryPtr))
    {
        seekType = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        segPtr   = dbEntryPtr;
        segPtr->vTcamId = vTcamId;
        segPtr->ruleSize = vTcamRuleSize;
    }

    /* merge vTCAM new layout segments */
    rc = prvCpssDxChVirtualTcamDbSegmentTableLuOrderedTreeMerge(
        pVTcamMngPtr, vTcamCfgPtr->segmentsTree);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(
            rc, "failed on FreeSpaceMergeNeighbors");
    }

    /* update rule amounts after splits and merges */
    prvCpssDxChVirtualTcamDbSegmentTableUpdateRuleAmounts(
        pVTcamMngPtr, vTcamCfgPtr->segmentsTree);

    /* update start segment logical indexes */
    segmentTableVTcamBaseLogicalIndexUpdate(vTcamCfgPtr);

    if (prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable != GT_FALSE)
    {
        cpssOsPrintf("vTcamCfgPtr->segmentsTree after merging\n");
        prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree(vTcamCfgPtr->segmentsTree);
    }

    /* save old table logical => physical rule indexes */
    oldRulePhysicalIndexArr = vTcamCfgPtr->rulePhysicalIndexArr;

    /* update Locical => physical table and move rules */
    /* bypassed for dummy vTcam Manager used for checking avalibility */
    if (oldRulePhysicalIndexArr != NULL)
    {
        /* create new table logical => physical rule indexes */
        vTcamCfgPtr->rulePhysicalIndexArr =
            (GT_U16*)cpssOsMalloc(sizeof(GT_U16) * vTcamCfgPtr->rulesAmount);
        if (vTcamCfgPtr->rulePhysicalIndexArr == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        prvCpssDxChVirtualTcamDbSegmentTableLogToPhyIndexTabGenerate(
            pVTcamMngPtr,
            vTcamCfgPtr->segmentsTree,
            vTcamCfgPtr->rulePhysicalIndexArr);

        /* move rules from old to new location */
        rc = prvCpssDxChVirtualTcamDbSegmentTableMoveRulesToNewLoayout(
            pVTcamMngPtr, vTcamCfgPtr->rulesAmount, vTcamRuleSize,
            vTcamCfgPtr->usedRulesBitmapArr,
            oldRulePhysicalIndexArr /*srcRulePhysicalIndexArr*/,
            vTcamCfgPtr->rulePhysicalIndexArr /*dstRulePhysicalIndexArr*/);

        /* free old bitmap (saved) */
        cpssOsFree(oldRulePhysicalIndexArr);

        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(
                rc, "failed on TableMoveRulesToNewLoayout");
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableMakeSpace
*
* DESCRIPTION:
*       Drugs existing vTCAMs of 10-bytes and 20-bytes rules
*       to provide required space if needed rearranging contents
*       of blocks already mapped to given vTCAM.
*       If trere is no enough memory only - maps additional blocks to lookup.
*
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       allocRequestPtr - (pointer to) allocation request structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableMakeSpace
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC    *allocRequestPtr
)
{
    GT_U32                                        requestRemainder;
    GT_U32                                        availableInLookup;
    GT_U32                                        availableInFreeBlocks;
    GT_U32                                        vTcamId;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC   *vTcamCfgPtr;
    GT_STATUS                                     rc;
    GT_U32                                        freedRules;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT          ruleSizeEnum;
    GT_U32                                        *freePlacesBitmapPtr;
    GT_BOOL                                       rulePlacesAvailable;
    GT_U32                                        lookupId;


    requestRemainder = allocRequestPtr->rulesAmount;

    availableInLookup = prvCpssDxChVirtualTcamDbSegmentTableCountAvailableSpace(
        pVTcamMngPtr, allocRequestPtr->lookupId, allocRequestPtr->ruleSize);
    if (availableInLookup >= requestRemainder)
    {
        return GT_OK;
    }

    requestRemainder -= availableInLookup;

    availableInFreeBlocks = prvCpssDxChVirtualTcamDbSegmentTableCountFreeBlocksSpace(
        pVTcamMngPtr, allocRequestPtr->ruleSize);
    if (availableInFreeBlocks >= requestRemainder)
    {
        return GT_OK;
    }

    requestRemainder -= availableInFreeBlocks;

    freePlacesBitmapPtr =
        prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapAllocate(pVTcamMngPtr);
    if (freePlacesBitmapPtr == NULL)
    {

        CPSS_LOG_ERROR_AND_RETURN_MAC(
            GT_OUT_OF_CPU_MEM, "failed on BlockRowBitmapAllocate");
    }

    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapReset(
        pVTcamMngPtr, freePlacesBitmapPtr);
    /* only 40-byte and 50-byte side holes actually supported */
    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSetForFreeRows(
        pVTcamMngPtr, freePlacesBitmapPtr, allocRequestPtr->lookupId,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E,
        1 /*value*/);
    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSetForFreeRows(
        pVTcamMngPtr, freePlacesBitmapPtr, allocRequestPtr->lookupId,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E,
        1 /*value*/);
    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapSetForFreeRows(
        pVTcamMngPtr, freePlacesBitmapPtr, allocRequestPtr->lookupId,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E,
        1 /*value*/);

    ruleSizeEnum =
        (CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E + allocRequestPtr->ruleSize - 1);
    rulePlacesAvailable = prvCpssDxChVirtualTcamDbSegmentTableCheckFreableSpace(
        pVTcamMngPtr, freePlacesBitmapPtr,
        allocRequestPtr->lookupId, ruleSizeEnum,
        requestRemainder/*requiredRulesAmount*/);
    prvCpssDxChVirtualTcamDbSegmentTableBlockRowBitmapFree(freePlacesBitmapPtr);
    if (rulePlacesAvailable == GT_FALSE)
    {
        /* temporary - until comressing other lookups yet not implented */
        return GT_FULL;
    }

    /* freing blocks rearranging vTCAMs in the same lookup */
    for (vTcamId = 0; (vTcamId < pVTcamMngPtr->vTcamCfgPtrArrSize); vTcamId++)
    {
        vTcamCfgPtr = pVTcamMngPtr->vTcamCfgPtrArr[vTcamId];
        if (vTcamCfgPtr == NULL) continue;
        lookupId =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_MAC(
                vTcamCfgPtr->tcamInfo.clientGroup,
                vTcamCfgPtr->tcamInfo.hitNumber);
        if (allocRequestPtr->lookupId != lookupId) continue;
        rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamRearrange(
            pVTcamMngPtr, vTcamCfgPtr, vTcamId, allocRequestPtr->ruleSize,
            requestRemainder, &freedRules);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(
                rc, "failed on VTcamRearrange");
        }
        requestRemainder -=
            ((requestRemainder <= freedRules) ? requestRemainder : freedRules);
        if (requestRemainder == 0) break;
    }

    if (requestRemainder == 0)
    {
        return GT_OK;
    }

    /* freing space comressing other lookups yet not implented */
    return GT_FULL;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeBuildCycle
*
* DESCRIPTION:
*       Cycle of Build tree of free TCAM segments for allocation to vTCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr     - (pointer to) VTCAM Manager structure
*       lookupId         - searched lookup Id
*       ruleSize         - rule size measured in 10-byte units
*       requiredRulesNum - number of required rules
*       treeId           - (pointer to) id of tree accomulaing segments.
*       startFreeColumns - type of row in free segment to start search
*       stopFreeColumns  - type of row in found segment to stop search
*                          in spcified PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E - no stop
*       ruleConuterPtr   - counter of rule places incremented with rule acconulation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeBuildCycle
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     GT_U32                                                     lookupId,
    IN     GT_U32                                                     ruleSize,
    IN     GT_U32                                                     requiredRulesNum,
    IN     PRV_CPSS_AVL_TREE_ID                                       treeId,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              startFreeColumns,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT              stopFreeColumns,
    INOUT  GT_U32                                                     *ruleCounterPtr
)
{
    GT_STATUS               rc;          /* return code                         */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC seg;     /* search key      */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr; /* found segment   */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   segmentColumns;/*segment Col*/
    GT_BOOL                 found;       /* node found                          */
    GT_BOOL                 seeked;      /* node seeked                         */
    PRV_CPSS_AVL_TREE_PATH  path;        /* tree iterator                       */
    GT_U32                  rulesPerRow; /* number of rules per row             */
    GT_VOID_PTR             dbEntryPtr;  /* pointer to entry in DB              */
    GT_U32                  horzBlockIdx;/* horizontal block index              */

    /* gather segments in blocks mapped to the same lookup id */
    /* with the best Columns Map                              */
    cpssOsMemSet(&seg, 0, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
    seg.lookupId = lookupId;
    /* the worst candidate preferred */
    seg.rowsAmount = 1;    /* the smallest           */
    seg.segmentColumnsMap =
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
        seg.segmentColumnsMap, 0, startFreeColumns);

    found = prvCpssAvlPathFind(
        pVTcamMngPtr->segFreePrefTree, PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
        &seg, path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    segPtr = dbEntryPtr;
    if (found == GT_FALSE)
    {
        return GT_OK;
    }

    while ((segPtr->lookupId == lookupId)
           && ((*ruleCounterPtr) < requiredRulesNum))
    {
        if (stopFreeColumns !=
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)
        {
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(
                segPtr->segmentColumnsMap, horzBlockIdx, segmentColumns);
            if (prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToFreeSpaceRank(
                segmentColumns)
                >= prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToFreeSpaceRank(
                    stopFreeColumns))
            {
                return GT_OK;
            }
        }
        rulesPerRow = prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
            segPtr, ruleSize);
        if (rulesPerRow != 0)
        {
            *ruleCounterPtr += (rulesPerRow * segPtr->rowsAmount);
            rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(treeId, segPtr);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on prvCpssAvlItemInsert");
            }
        }
        seeked = prvCpssAvlPathSeek(
                pVTcamMngPtr->segFreePrefTree, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr = dbEntryPtr;
        if (seeked == GT_FALSE)
        {
            break;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeBuild
*
* DESCRIPTION:
*       Build tree of free TCAM segments for allocation to vTCAM.
*       Does no changes in old DB tree/pool.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       allocRequestPtr - (pointer to) allocation request structure
*
* OUTPUTS:
*       treeIdPtr       - (pointer to) id of tree containig space to be
*                         allocated in lookup order. Segments yet not
*                         united / splitted for segments using the same
*                         rows. Yet not clientId mapped and not vTcam updated
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeBuild
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC    *allocRequestPtr,
    OUT    PRV_CPSS_AVL_TREE_ID                                       *treeIdPtr
)
{
    GT_STATUS               rc;          /* return code                        */
    PRV_CPSS_AVL_TREE_ID    treeId;      /* id of a new tree                   */
    GT_U32                  rulesNum;    /* number of rules in passed segments */
    GT_U32                  placeMultiplier;      /* converts 60-byte rows to 10,20,30 byte rules */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   goalFreeColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT   startFreeColumns;
    GT_U32                                          goalFreeColumnsRank;
    GT_U32                                          startFreeColumnsRank;

    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFunc,
        pVTcamMngPtr->segNodesPool, &treeId);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "prvCpssAvlTreeCreate failed");
    }

    rc = prvCpssDxChVirtualTcamDbSegmentTableBestColumnsTypeGet(
        allocRequestPtr->ruleSize, &goalFreeColumns, &placeMultiplier);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on ColumnsTypeGet");
    }

    startFreeColumns =
        prvCpssDxChVirtualTcamDbSegmentTableStartColumnsTypeGet(
            allocRequestPtr->ruleSize);

    rulesNum = 0;

    goalFreeColumnsRank =
        prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToFreeSpaceRank(
            goalFreeColumns);
    startFreeColumnsRank =
        prvCpssDxChVirtualTcamDbSegmentTableVTcamSegmentColumnsToFreeSpaceRank(
            startFreeColumns);


    /* gather segments in blocks mapped to the same lookup id */
    /* with the best Columns Map                              */
    rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeBuildCycle(
        pVTcamMngPtr, allocRequestPtr->lookupId,
        allocRequestPtr->ruleSize, allocRequestPtr->rulesAmount,
        treeId, goalFreeColumns,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, &rulesNum);
    if (rc != GT_OK)
    {
        prvCpssAvlTreeDelete(
            treeId, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on PrimaryTreeBuildCycle");
    }

    if (rulesNum >= allocRequestPtr->rulesAmount)
    {
        CPSS_LOG_INFORMATION_MAC("Successfully allocated [%d] rules as requested , and there are at least [%d] free rules suitable for this client \n",
            allocRequestPtr->rulesAmount , rulesNum - allocRequestPtr->rulesAmount);

        *treeIdPtr = treeId;
        return GT_OK;
    }

    if (goalFreeColumnsRank > startFreeColumnsRank)
    {
        /* gather segments in blocks mapped to the same lookup id */
        /* with any matching Columns Map                          */
        rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeBuildCycle(
            pVTcamMngPtr, allocRequestPtr->lookupId,
            allocRequestPtr->ruleSize, allocRequestPtr->rulesAmount,
            treeId, startFreeColumns, goalFreeColumnsRank, &rulesNum);
        if (rc != GT_OK)
        {
            prvCpssAvlTreeDelete(
                treeId, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on PrimaryTreeBuildCycle");
        }

        if (rulesNum >= allocRequestPtr->rulesAmount)
        {
            CPSS_LOG_INFORMATION_MAC("Successfully allocated [%d] rules as requested , and there are at least [%d] free rules suitable for this client \n",
                allocRequestPtr->rulesAmount , rulesNum - allocRequestPtr->rulesAmount);

            *treeIdPtr = treeId;
            return GT_OK;
        }
    }

    /* gather segments that are free blocks */
    rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeBuildCycle(
        pVTcamMngPtr, PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS,
        allocRequestPtr->ruleSize, allocRequestPtr->rulesAmount,
        treeId, goalFreeColumns,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E, &rulesNum);
    if (rc != GT_OK)
    {
        prvCpssAvlTreeDelete(
            treeId, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on PrimaryTreeBuildCycle");
    }

    if (rulesNum >= allocRequestPtr->rulesAmount)
    {
        CPSS_LOG_INFORMATION_MAC("Successfully allocated [%d] rules as requested , and there are at least [%d] free rules suitable for this client \n",
            allocRequestPtr->rulesAmount , rulesNum - allocRequestPtr->rulesAmount);

        *treeIdPtr = treeId;
        return GT_OK;
    }

    /* no enough free space found */
    prvCpssAvlTreeDelete(
        treeId, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);

    /* we may get here from resize or auto-resize or 'memory availability check' */
    /* each one of them should decide how to register error or not */

    CPSS_LOG_INFORMATION_MAC("There are only [%d] (out of[%d] requested) free rules suitable for this client \n",
        rulesNum,allocRequestPtr->rulesAmount);

    return /* do not register as error */ GT_FULL;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeAllocAndMap
*
* DESCRIPTION:
*       Remove primary tree from free space and map free blocks to lookup.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       allocRequestPtr - (pointer to) allocation request structure
*       treeId          - primary tree for allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       other           - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeAllocAndMap
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC    *allocRequestPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       treeId
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr; /* found segment  */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC* blockPtr; /* pointer to block info */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT segmentColumns;
    GT_STATUS               rc;          /* return code                        */
    GT_U32                  blocksInRow; /* number of blocks In Row            */
    GT_U32                  blockHorzIdx;/* block In Row index                 */
    GT_BOOL                 seeked;      /* node seeked                        */
    PRV_CPSS_AVL_TREE_PATH  path;        /* tree iterator                      */
    GT_U32                  blockIdx;    /* block index                        */
    GT_VOID_PTR             dbEntryPtr;  /* pointer to entry in DB             */

    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    /* remove segments from tree of free segments */
    seeked = prvCpssAvlPathSeek(
            treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            path, &dbEntryPtr);

    while (seeked != GT_FALSE)
    {
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            pVTcamMngPtr->segFreePrefTree, dbEntryPtr);
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            pVTcamMngPtr->segFreeLuTree, dbEntryPtr);
        segPtr = dbEntryPtr;
        prvCpssDxChVirtualTcamDbSegmentTableUpdateBlockFreeSpace(
            pVTcamMngPtr, segPtr, GT_TRUE/*decrease*/);
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr);
    }

    /* block mapping to lookup */
    seeked = prvCpssAvlPathSeek(
            treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    segPtr = dbEntryPtr;
    while (seeked != GT_FALSE)
    {
        if (segPtr->lookupId  != allocRequestPtr->lookupId)
        {
            for (blockHorzIdx = 0; (blockHorzIdx < blocksInRow); blockHorzIdx++)
            {
                segmentColumns =
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                        segPtr->segmentColumnsMap, blockHorzIdx);
                if (segmentColumns ==
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E) continue;
                blockIdx =
                    ((blocksInRow * (segPtr->rowsBase / pVTcamMngPtr->tcamSegCfg.lookupRowAlignment)) +
                    blockHorzIdx);
                if (blockIdx >= pVTcamMngPtr->blocksAmount)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }

                blockPtr                 = &(pVTcamMngPtr->blockInfArr[blockIdx]);
                if (blockPtr->lookupId == allocRequestPtr->lookupId)
                {
                    /* already mapped as needed */
                    continue;
                }
                if (blockPtr->lookupId !=
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                if (pVTcamMngPtr->tcamSegCfg.lookupMapFuncPtr != NULL)
                {
                    rc = pVTcamMngPtr->tcamSegCfg.lookupMapFuncPtr(
                        pVTcamMngPtr->tcamSegCfg.cookiePtr,
                        blockPtr->rowsBase,
                        blockPtr->columnsBase,
                        allocRequestPtr->lookupId);
                    if (rc != GT_OK)
                    {
                        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                            ("lookupMapFuncPtr returned %d", rc));
                        return rc;
                    }
					blockPtr->lookupId = allocRequestPtr->lookupId;
                }
            }

            segPtr->lookupId   = allocRequestPtr->lookupId;
        }
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr = dbEntryPtr;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMergeAndSplit
*
* DESCRIPTION:
*       Splits or merges primary tree segments to be vTCAM space.
*       Parts containing same rows (but different columns) merged to one segment.
*       Parts remaining after merge splitted to new segments.
*       When the last segment more than needed for request it also splitted
*       to two segments.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       allocRequestPtr - (pointer to) allocation request structure
*       treeId          - primary tree for allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMergeAndSplit
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC    *allocRequestPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       treeId
)
{
    GT_STATUS               rc;          /* return code                        */
    GT_U32                  rulesPerRow; /* rules per row                      */
    GT_U32                  numOfRules;  /* number of rules                    */
    GT_U32                  totalRules;  /* number of rules                    */
    GT_U32                  extraRules;  /* redundant rules                    */
    GT_U32                  extraRows;   /* redundant rows                     */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1; /* segment       */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;  /* segment       */
    GT_VOID_PTR             segPtr2;     /* segment                            */
    GT_BOOL                 seeked;      /* node seeked                        */
    GT_BOOL                 found;       /* node found                         */
    PRV_CPSS_AVL_TREE_PATH  path;        /* tree iterator                      */
    GT_VOID_PTR             dbEntryPtr;  /* pointer to entry in DB             */


    /* split and merge free spaces */
    rc = prvCpssDxChVirtualTcamDbSegmentTableLuOrderedTreeMerge(
        pVTcamMngPtr, treeId);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* free redundant space in chain */
    totalRules = 0;

    seeked = prvCpssAvlPathSeek(
            treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    segPtr1 = dbEntryPtr;
    while (seeked != GT_FALSE)
    {
        rulesPerRow = prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
            segPtr1, allocRequestPtr->ruleSize);
        numOfRules  = rulesPerRow * segPtr1->rowsAmount;
        totalRules  += numOfRules;
        if (totalRules >= allocRequestPtr->rulesAmount)
        {
            break;
        }
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr1 = dbEntryPtr;
    }

    if (totalRules < allocRequestPtr->rulesAmount)
    {
        /* should not occur */
        prvCpssDxChVirtualTcamDbSegmentTableException();
        return GT_FAIL;
    }

    if (seeked == GT_FALSE)
    {
        return GT_OK;
    }

    /* free space tail */
    while (1)
    {
        found = prvCpssAvlPathFind(
            treeId, PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
            segPtr1, path, &segPtr2);
        if (found == GT_FALSE)
        {
            /* should not occur */
            prvCpssDxChVirtualTcamDbSegmentTableException();
            break;
        }
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &segPtr2);
        if (seeked == GT_FALSE)
        {
            break;
        }
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            treeId, segPtr2);
        rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit(
            pVTcamMngPtr, segPtr2);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if (totalRules == allocRequestPtr->rulesAmount)
    {
        /* no extra rules */
        return GT_OK;
    }

    /* split last segment */
    extraRules = totalRules - allocRequestPtr->rulesAmount;
    rulesPerRow = prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
        segPtr1, allocRequestPtr->ruleSize);
    if (extraRules >= rulesPerRow)
    {
        /* split last rows */
        extraRows = extraRules / rulesPerRow;
        segPtr1->rowsAmount -= extraRows;

        /* split horizontal between blocks */
        segPtr = cpssBmPoolBufGet(pVTcamMngPtr->segmentsPool);
        if (segPtr == NULL)
        {
            /* should never occur */
            prvCpssDxChVirtualTcamDbSegmentTableException();
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
                ("cpssBmPoolBufGet failed"));
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }
        cpssOsMemCpy(segPtr, segPtr1, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
        segPtr->rowsBase    = (segPtr1->rowsBase + segPtr1->rowsAmount);
        segPtr->rowsAmount  = extraRows;
        rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit(
            pVTcamMngPtr, segPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeSplitUnusedColumns
*
* DESCRIPTION:
*       Horizontal split and free not useable space of segments.
*       Segments retieved from free space tree covered oly one horizontal block,
*       but already merged with horizontal neighbors (if were).
*       The needed split relevant only for 40 and 50 byte rules that
*       got full block row segments.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       allocRequestPtr - (pointer to) allocation request structure
*       treeId          - primary tree for allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeSplitUnusedColumns
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC    *allocRequestPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       treeId
)
{
    GT_STATUS               rc;          /* return code                        */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;  /* segment       */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr1; /* segment       */
    GT_BOOL                 seeked;      /* node seeked                        */
    PRV_CPSS_AVL_TREE_PATH  path;        /* tree iterator                      */
    GT_VOID_PTR             dbEntryPtr;  /* pointer to entry in DB             */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT     goalFreeColumns;
    GT_U32                                            placeMultiplier;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP goalColumnsMap;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT     segmentColumns;
    GT_U32                                            blocksInRow;
    GT_U32                                            blockHorzIdx ;

    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    rc = prvCpssDxChVirtualTcamDbSegmentTableBestColumnsTypeGet(
        allocRequestPtr->ruleSize, &goalFreeColumns, &placeMultiplier);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed on ColumnsTypeGet");
    }

    if (goalFreeColumns
        == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
    {
        /* split dedicateded only go 40 and 50 byte rules */
        return GT_OK;
    }

    seeked = prvCpssAvlPathSeek(
            treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    segPtr = dbEntryPtr;
    while (seeked != GT_FALSE)
    {
        /* get ColumnsMap of useable part */
        goalColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
        for (blockHorzIdx = 0; (blockHorzIdx < blocksInRow); blockHorzIdx++)
        {
            segmentColumns =
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                    segPtr->segmentColumnsMap, blockHorzIdx);
            if ((segmentColumns == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E)
                || (segmentColumns == goalFreeColumns))
            {
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                    goalColumnsMap, blockHorzIdx, goalFreeColumns);
            }
        }

        /* nothing to free */
        if (segPtr->segmentColumnsMap == goalColumnsMap)
        {
            seeked = prvCpssAvlPathSeek(
                    treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                    path, &dbEntryPtr /* use dedicated var to avoid warnings */);
            segPtr = dbEntryPtr;
            continue;
        }

        /* crop useable part and get remainder */
        rc = prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorizontal(
            pVTcamMngPtr, treeId, NULL /*tree2*/,
            segPtr, goalColumnsMap, GT_TRUE /*putBack*/, &segPtr1);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
        if (segPtr1 == NULL)
        {
            /* must never occur */
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        /* free remainder columns */
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            treeId, segPtr1);

        rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit(
            pVTcamMngPtr, segPtr1);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        /* updated seg - must be found */
        seeked = prvCpssAvlPathFind(
                treeId, PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
                segPtr, path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr = dbEntryPtr;

        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr = dbEntryPtr;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeReduceHalfRow
*
* DESCRIPTION:
*       reduce last half row from the allocation if got too much rules.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       allocRequestPtr - (pointer to) allocation request structure
*       treeId          - primary tree for allocation.
*
* OUTPUTS:
*       didChangePtr    - indication that the allocation tree changed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeReduceHalfRow
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC    *allocRequestPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       treeId,
    IN     GT_U32                                                     totalRules,
    OUT    GT_BOOL                                                    *didChangePtr
)
{
    GT_STATUS               rc;          /* return code                        */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr; /* segment       */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *bestSegPtr = NULL;  /* segment       */
    PRV_CPSS_AVL_TREE_PATH  path;        /* tree iterator                      */
    GT_VOID_PTR             dbEntryPtr;  /* pointer to entry in DB             */
    PRV_CPSS_AVL_TREE_SEEK_ENT seekType;
    GT_U32  moreThanRequested = totalRules - allocRequestPtr->rulesAmount;
    GT_U32  bestValue = 0;
    GT_U32  newValue;
    GT_U32  bestBlockHorzIdx = 0;
    GT_U32  bestRank = 0;
    GT_U32  maxRulesPerFullRow; /* max rules per full row               */
    GT_U32  maxRulesPerBlockRow; /* max rules per block row             */
    GT_U32  blocksInRow;
    GT_U32  blockHorzIdx ;
    GT_U32  blocksSegmentInRow = 0;
    GT_U32  bestBlocksSegmentInRow = 0;
    GT_U32  rank;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT     segmentColumns;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP segmentColumnsMap;

    *didChangePtr = GT_FALSE;

    if(moreThanRequested == 0)
    {
        return GT_OK;
    }

    /* calculate maximal rule amount per full TCAM row */
    blocksInRow =
        (pVTcamMngPtr->tcamSegCfg.tcamColumnsAmount
         / pVTcamMngPtr->tcamSegCfg.lookupColumnAlignment);

    maxRulesPerBlockRow =
        prvCpssDxChVirtualTcamDbSegmentTableMaxRulesAmountPerColumnsTypeGet(
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E,
            allocRequestPtr->ruleSize);
    maxRulesPerFullRow = (maxRulesPerBlockRow * blocksInRow);

    if (moreThanRequested >= maxRulesPerFullRow)
    {
        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,
            "DB ERROR : the [%d B] must allocated too much more then requested",
            allocRequestPtr->ruleSize*10);
    }

    do
    {
        bestValue = 0;
        bestRank = 0;
        bestSegPtr = NULL;
        seekType = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;

        /* look for segment from 2 sizes of the floor */
        /* find segment with the most rules in row */
        while(GT_TRUE == prvCpssAvlPathSeek(
                    treeId, seekType,
                    path, &dbEntryPtr /* use dedicated var to avoid warnings */))
        {
            segPtr = dbEntryPtr;
            seekType = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;

            /* amount of blocks intersecting with segment */
            blocksSegmentInRow = 0;
            for (blockHorzIdx = 0; (blockHorzIdx < blocksInRow); blockHorzIdx++)
            {
                segmentColumns =
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                        segPtr->segmentColumnsMap, blockHorzIdx);
                if (segmentColumns ==
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E) continue;
                blocksSegmentInRow ++;
            }

            /* the best segment/block-column to split and free */
            for (blockHorzIdx = 0; (blockHorzIdx < blocksInRow); blockHorzIdx++)
            {
                segmentColumns =
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                        segPtr->segmentColumnsMap, blockHorzIdx);
                if (segmentColumns ==
                    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E) continue;
                newValue =
                    prvCpssDxChVirtualTcamDbSegmentTableMaxRulesAmountPerColumnsTypeGet(
                        segmentColumns, allocRequestPtr->ruleSize);

                if (moreThanRequested < newValue) continue;

                rank = 0;
                if (blocksSegmentInRow == 1) rank ++;
                if (segPtr->rowsAmount == 1) rank ++;

                /* better with priority for one-block and one-row segments */
                if ((newValue > bestValue) ||
                    ((newValue == bestValue) && (rank > bestRank)))
                {
                    bestValue = newValue;
                    bestSegPtr = segPtr;
                    bestBlockHorzIdx = blockHorzIdx;
                    bestRank = rank;
                    bestBlocksSegmentInRow = blocksSegmentInRow;
                }

                /* better will no be found */
                if ((newValue == maxRulesPerBlockRow)
                    && (rank == 2)) break;
            }
        }

        if(bestSegPtr == NULL)
        {
            /* no more (or any) option to reduce the amount */
            break;
        }

        if (bestSegPtr->rowsAmount > 1)
        {
            /* split last row */
            rc = prvCpssDxChVirtualTcamDbSegmentTableCropSegmentVertical(
                pVTcamMngPtr, treeId, NULL /*tree2*/,
                bestSegPtr, (bestSegPtr->rowsBase + bestSegPtr->rowsAmount - 1), 1,
                GT_TRUE /*putBack*/,
                &segPtr /*lowSegPtrPtr*/, NULL /*highSegPtrPtr*/);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "CropSegmentVertical failed");
            }
            if (segPtr == NULL)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, "NULL low-part pointer");
            }
            segPtr->rulesAmount =
                (prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
                    segPtr, allocRequestPtr->ruleSize)
                 * segPtr->rowsAmount);
        }

        if (bestBlocksSegmentInRow > 1)
        {
            segmentColumnsMap = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS;
            segmentColumns =
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(
                    bestSegPtr->segmentColumnsMap, bestBlockHorzIdx);
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(
                segmentColumnsMap, bestBlockHorzIdx, segmentColumns);
            rc = prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorizontal(
                pVTcamMngPtr, treeId, NULL /*tree2*/,
                bestSegPtr,  segmentColumnsMap,
                GT_TRUE /*putBack*/, &segPtr /*remainSegPtrPtr*/);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "CropSegmentVertical failed");
            }
            if (segPtr == NULL)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, "NULL low-part pointer");
            }
            segPtr->rulesAmount =
                (prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
                    segPtr, allocRequestPtr->ruleSize)
                 * segPtr->rowsAmount);
        }

        /* state that this segment is now released to the free list of the TCAM manager */
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            treeId, bestSegPtr);
        rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit(
            pVTcamMngPtr, bestSegPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        *didChangePtr = GT_TRUE;

        if(bestValue > moreThanRequested)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, "DB error: bestValue[%d] > totalRules[%d]",
                bestValue,totalRules);
        }
        else
        if (bestValue == moreThanRequested)
        {
            /* reached best effort */
            break;
        }

        moreThanRequested -= bestValue;
    }
    while(1);

    return GT_OK;
}

/*debug dedicated*/
GT_BOOL prvCpssDxChVirtualTcamDbSegmentTableVTcamAllocateUseGranularity = GT_TRUE;

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamAllocate
*
* DESCRIPTION:
*       Allocate memory for Virtual TCAM in Segment Table for TCAM.
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
*       allocRequestPtr - (pointer to) allocation request structure
*
* OUTPUTS:
*       tcamCfgPtr      - (pointer to)virtual TCAM structure that updated
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamAllocate
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC                *tcamCfgPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC    *allocRequestPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr;  /* segment       */
    GT_STATUS               rc;          /* return code                        */
    PRV_CPSS_AVL_TREE_ID    treeId = NULL; /* tree Id                          */
    GT_BOOL                 seeked;      /* node seeked                        */
    PRV_CPSS_AVL_TREE_PATH  path;        /* tree iterator                      */
    GT_U32                  rulesPerRow; /* rules per row                      */
    GT_U32                  totalRules;  /* number of rules                    */
    GT_VOID_PTR             dbEntryPtr;  /* pointer to entry in DB             */
    GT_BOOL                 didChange = GT_FALSE;   /* indication that tree changed  */
    GT_U32                  granularity; /* rule amount granularity            */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC    allocRequestCopy;

    if (prvCpssDxChVirtualTcamDbSegmentTableVTcamAllocateUseGranularity != GT_FALSE)
    {
        granularity =
            prvCpssDxChVirtualTcamDbSegmentTableVTcamSizeGranularity(
                allocRequestPtr->ruleSize);
        if (granularity != 1)
        {
            allocRequestCopy = *allocRequestPtr;
            allocRequestPtr  = &allocRequestCopy;
            allocRequestPtr->rulesAmount += (granularity - 1);
            allocRequestPtr->rulesAmount -= (allocRequestPtr->rulesAmount % granularity);
        }
    }

    /*PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
        ("prvCpssDxChVirtualTcamDbSegmentTableVTcamAllocate")); */

    /*PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(("Free space tree dump"));
    prvCpssDxChVirtualTcamDbSegmentTableDumpTree(pVTcamMngPtr->segFreeLuTree); */

    CPSS_LOG_INFORMATION_MAC("request for[%d] rules for lookupId[%d] vTcamId[%d] ruleAlignment[%d B] ruleSize[%d B] \n",
        allocRequestPtr->rulesAmount,
        allocRequestPtr->lookupId,
        allocRequestPtr->vTcamId,
        allocRequestPtr->ruleAlignment * 10,
        allocRequestPtr->ruleSize/*1..6*/ * 10);

    if (pVTcamMngPtr->disableRearrange == GT_FALSE)
    {
        rc = prvCpssDxChVirtualTcamDbSegmentTableMakeSpace(
            pVTcamMngPtr, allocRequestPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeBuild(
        pVTcamMngPtr, allocRequestPtr, &treeId);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeAllocAndMap(
        pVTcamMngPtr, allocRequestPtr, treeId);
    if (rc != GT_OK)
    {
        prvCpssAvlTreeDelete(treeId, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        /* should not occur if not a bug */
        prvCpssDxChVirtualTcamDbSegmentTableException();
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
            ("prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeAllocAndMap failed\n"));
        return rc;
    }

    rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMergeAndSplit(
        pVTcamMngPtr, allocRequestPtr, treeId);
    if (rc != GT_OK)
    {
        prvCpssAvlTreeDelete(treeId, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
            ("prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMergeAndSplit failed\n"));
        return rc;
    }

    rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeSplitUnusedColumns(
        pVTcamMngPtr, allocRequestPtr, treeId);
    if (rc != GT_OK)
    {
        prvCpssAvlTreeDelete(treeId, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(
            ("prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeSplitUnusedColumns failed\n"));
        return rc;
    }

recalcRulesInTheTree_lbl:
    /* update allocated segments */
    totalRules = 0;
    seeked = prvCpssAvlPathSeek(
            treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    segPtr = dbEntryPtr;

    while (seeked != GT_FALSE)
    {
        rulesPerRow = prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
            segPtr, allocRequestPtr->ruleSize);
        segPtr->vTcamId           = allocRequestPtr->vTcamId;
        segPtr->lookupId          = allocRequestPtr->lookupId;
        segPtr->rulesAmount       = (rulesPerRow * segPtr->rowsAmount);
        segPtr->ruleSize          = allocRequestPtr->ruleSize;
        segPtr->baseLogicalIndex  = totalRules;
        totalRules               += segPtr->rulesAmount;
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr = dbEntryPtr;
        /* adding to additional trees (for resizing/defrag) should be added here */
    }

    tcamCfgPtr->segmentsTree = treeId;
    tcamCfgPtr->rulesAmount  = totalRules;
    tcamCfgPtr->rulePhysicalIndexArr = NULL;

    if(didChange == GT_FALSE)
    {
        /* reduce last half row from the allocation if got too much rules */
        rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeReduceHalfRow(
            pVTcamMngPtr, allocRequestPtr, treeId,totalRules, &didChange);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }

        if(didChange == GT_TRUE)
        {
            goto recalcRulesInTheTree_lbl;
        }
    }

    if (tcamCfgPtr->rulesAmount)
    {

        /* logical to physical conversion array */
        tcamCfgPtr->rulePhysicalIndexArr =
            (GT_U16*)cpssOsMalloc(sizeof(GT_U16) * tcamCfgPtr->rulesAmount);
        if (tcamCfgPtr->rulePhysicalIndexArr == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }
        prvCpssDxChVirtualTcamDbSegmentTableLogToPhyIndexTabGenerate(
            pVTcamMngPtr,
            tcamCfgPtr->segmentsTree,
            tcamCfgPtr->rulePhysicalIndexArr);
    }

    /*PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(("vTCAM space tree dump"));
    prvCpssDxChVirtualTcamDbSegmentTableDumpTree(tcamCfgPtr->segmentsTree);*/

    if(totalRules > allocRequestPtr->rulesAmount)
    {
        CPSS_LOG_INFORMATION_MAC("Successfully reserve [%d] rules ([%d] more than requested[%d]) for the client \n",
            totalRules,totalRules-allocRequestPtr->rulesAmount,allocRequestPtr->rulesAmount);
    }
    else
    {
        CPSS_LOG_INFORMATION_MAC("Successfully reserve [%d] rules (as requested) for the client \n",
            totalRules);
    }

    /* split and merge free spaces */
    rc = prvCpssDxChVirtualTcamDbSegmentTableLuOrderedTreeMerge(
        pVTcamMngPtr, tcamCfgPtr->segmentsTree);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* update rule amounts after splits and merges */
    prvCpssDxChVirtualTcamDbSegmentTableUpdateRuleAmounts(
        pVTcamMngPtr, tcamCfgPtr->segmentsTree);

    prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMerge(pVTcamMngPtr);

    if (prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable != GT_FALSE)
    {
        cpssOsPrintf("\n\n\n End of prvCpssDxChVirtualTcamDbSegmentTableVTcamAllocate \n\n\n");
        prvCpssDxChVirtualTcamDbVTcamDumpFullTcamLayout(pVTcamMngPtr->vTcamMngId);
        cpssOsPrintf("current vTcam %d Segments tree \n", allocRequestPtr->vTcamId);
        prvCpssDxChVirtualTcamDbVTcamDumpByField(
            pVTcamMngPtr->vTcamMngId, allocRequestPtr->vTcamId, "segmentsTree");
        cpssOsPrintf("allocated vTcamSegments tree \n");
        prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree(tcamCfgPtr->segmentsTree);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableFreeWithSplit
*
* DESCRIPTION:
*       Put all segments of TCAM memory from the given tree
*       to global list of free segments.
*       Split segments that covers more then one block.
*       (related only to columns bitmap, block never covers rows of different flors)
*       Delete the given tree.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       treeId          - tree of allocated segments.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_RESOURCE           - on no TCAM space
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableFreeWithSplit
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    IN     PRV_CPSS_AVL_TREE_ID                                       treeId
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr ; /* segment  */
    GT_BOOL                 seeked;      /* node seeked                   */
    PRV_CPSS_AVL_TREE_PATH  path;        /* tree iterator                 */
    GT_VOID_PTR             dbEntryPtr;  /* pointer to entry in DB        */

    seeked = prvCpssAvlPathSeek(
            treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            path, &dbEntryPtr /* use dedicated var to avoid warnings */);
    segPtr = dbEntryPtr;
    while (seeked != GT_FALSE)
    {
        /* remove */
        prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            treeId, (GT_VOID*)segPtr);

        /* insert */
        prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit(
            pVTcamMngPtr, segPtr);

        /* next segment */
        seeked = prvCpssAvlPathSeek(
                treeId, PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
                path, &dbEntryPtr /* use dedicated var to avoid warnings */);
        segPtr = dbEntryPtr;
    }

    prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMerge(pVTcamMngPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamFree
*
* DESCRIPTION:
*       Free memory of Virtual TCAM in Segment Table for TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*
* OUTPUTS:
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*                         freed objects handlers set to NULL
*
* RETURNS:
*       GT_OK                   - on success
*       GT_NOT_INITIALIZED      - the given tree has not been initialized.
*       GT_BAD_STATE            - In case the tree is not empty.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamFree
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                         *pVTcamMngPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC                *tcamCfgPtr
)
{
    GT_STATUS     rc, rcFinal;              /* return code */

    rcFinal = GT_OK;

    /* free logical to physical conversion array */
    if (tcamCfgPtr->rulePhysicalIndexArr != NULL)
    {
        cpssOsFree(tcamCfgPtr->rulePhysicalIndexArr);
        tcamCfgPtr->rulePhysicalIndexArr = NULL;
    }

    if (tcamCfgPtr->segmentsTree != NULL)
    {
        rc = prvCpssDxChVirtualTcamDbSegmentTableFreeWithSplit(
            pVTcamMngPtr, tcamCfgPtr->segmentsTree);
        if (rc != GT_OK)
        {
            rcFinal = rc;
        }

        rc = prvCpssAvlTreeDelete(
            tcamCfgPtr->segmentsTree, (GT_VOIDFUNCPTR)NULL, (GT_VOID*)NULL);
        if (rc != GT_OK)
        {
            rcFinal = rc;
        }

        tcamCfgPtr->segmentsTree = NULL;

        /*PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(("Free space tree dump"));
        prvCpssDxChVirtualTcamDbSegmentTableDumpTree(pVTcamMngPtr->segFreeLuTree);*/
    }

    return rcFinal;
}

/*******************************************************************************
* segmentTableAllocSegmentAndAdd
*
* DESCRIPTION:
*       get buffer for segment and add it to the vtcam segments tree
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       vTcamCfgPtr            - (pointer to)virtual TCAM structure
*       entryPtr               - (pointer to) entry to add
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_STATE            - DB is in bad state
*       GT_NO_RESOURCE          - no buffers for segments
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS segmentTableAllocSegmentAndAdd
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *vTcamCfgPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *entryPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *buffPtr;

    buffPtr = cpssBmPoolBufGet(vTcamMngDBPtr->segmentsPool);
    if (buffPtr == NULL)
    {
        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    *buffPtr = *entryPtr;
    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
        vTcamCfgPtr->segmentsTree, buffPtr);

    return rc;
}

/*******************************************************************************
* segmentTableTouchCheckAndResolve
*
* DESCRIPTION:
*       check if one segment 'touching' another segment.
*       if 'yes' resolve the 'touch' of segments , by update segment 1
*       Vertical merge only.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       segEntry1Ptr            - (pointer to)segment 1 entry to check
*       segEntry2Ptr            - (pointer to)segment 2 entry to check
*
* OUTPUTS:
*       didTouchPtr           - (pointer to) indication that did touch.
*                               GT_TRUE - seg1,2 did touch
*                               GT_FALSE - seg1,2 have no touch
*       segEntry1Ptr            - (pointer to)updated segment 1 entry.
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_STATE            - DB is in bad state
*       GT_NO_RESOURCE          - no buffers for segments
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS segmentTableTouchCheckAndResolve
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntry1Ptr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntry2Ptr,
    OUT    GT_BOOL                                      *didTouchPtr
)
{
    GT_U32  maxRowsInSegment;
    /**********************/
    /* check for touch    */
    /**********************/
    *didTouchPtr = GT_FALSE;

    maxRowsInSegment = vTcamMngDBPtr->tcamSegCfg.lookupRowAlignment;

    /* check that the 2 segments can be combined under the same block */
    if((segEntry1Ptr->rowsBase / maxRowsInSegment) !=
       (segEntry2Ptr->rowsBase / maxRowsInSegment))
    {
        return GT_OK;
    }
    /* check if segment 2 and segment 1 hold the same columns ...
       check if the ranges touch each other  */
    if(segEntry2Ptr->segmentColumnsMap != segEntry1Ptr->segmentColumnsMap)
    {
        return GT_OK;
    }

    /*segment 2 and segment 1 hold the same columns*/
    if(segEntry2Ptr->rowsBase == (segEntry1Ptr->rowsBase + segEntry1Ptr->rowsAmount))
    {
        /* segment 2 start right after segment 1  */
        /* so add the rows amount and the rules amount */
    }
    else
    /* check if segment 1 start after segment 2 */
    if(segEntry1Ptr->rowsBase == (segEntry2Ptr->rowsBase + segEntry2Ptr->rowsAmount))
    {
        /* segment 1 start right after segment 2  */
        /* so update segment 1 to start where segment 2 starts */
        /* and add the rows amount and the rules amount */
        segEntry1Ptr->rowsBase = segEntry2Ptr->rowsBase;
    }
    else
    {
        return GT_OK;
    }

    /* add the rows amount and the rules amount */
    segEntry1Ptr->rowsAmount  += segEntry2Ptr->rowsAmount;
    segEntry1Ptr->rulesAmount += segEntry2Ptr->rulesAmount;

    /* indicate that only need to free the buffer and not the range */
    segEntry2Ptr->rowsAmount = 0;

    *didTouchPtr = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* segmentTableCollideCheckAndResolve
*
* DESCRIPTION:
*       check if one segment 'collide' with another segment or even 'touching'
*       another segment.
*       if 'yes' resolve the 'collide' of new and current segment
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       vTcamCfgPtr            - (pointer to)virtual TCAM structure
*       segEntry1Ptr            - (pointer to)segment 1 entry to check
*       segEntry2Ptr            - (pointer to)segment 2 entry to check
*
* OUTPUTS:
*       didCollidePtr           - (pointer to) indication that did collide.
*                               GT_TRUE - seg1,2 did collide and need more iterations
*                                   on 'segEntry2Ptr' in the vtcam tree
*                               GT_FALSE - seg1,2 have no collide
*       segEntry1Ptr            - (pointer to)updated segment 1 entry.
*       segEntry2Ptr            - (pointer to)updated segment 2 entry to check.
*                                   no more iterations needed when rowsAmount == 0
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_STATE            - DB is in bad state
*       GT_NO_RESOURCE          - no buffers for segments
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS segmentTableCollideCheckAndResolve
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *vTcamCfgPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntry1Ptr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntry2Ptr,
    OUT    GT_BOOL                                      *didCollidePtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC origSeg1Data;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC origSeg2Data;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC segEntry3;
    GT_U32  lastRow1,lastRow2;
    GT_U32  numRowsSeg3;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP  segmentColumnsMap3;
    GT_U32  rulesPerRow1,rulesPerRow2; /* number of rules per row            */
    GT_U32  totalRulesAmount = 0;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *tmp_segEntry1Ptr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *tmp_segEntry2Ptr;
    GT_BOOL didTouch;
    GT_U32  tmpValue;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP   mergedColumnsMap;

    *didCollidePtr = GT_FALSE;

    if(segEntry2Ptr->rowsAmount == 0)
    {
        /* no collide any more */
        return GT_OK;
    }
    /************************/
    /* check for collide    */
    /************************/
    /* check if segment 2 base is in range of segment 1 */
    if(segEntry2Ptr->rowsBase < (segEntry1Ptr->rowsBase + segEntry1Ptr->rowsAmount) &&
       segEntry2Ptr->rowsBase >= segEntry1Ptr->rowsBase)
    {
        /* segment 2 base is in range of segment 1 */
        tmp_segEntry1Ptr = segEntry1Ptr;
        tmp_segEntry2Ptr = segEntry2Ptr;
    }
    else
    /* check if segment 1 base is in range of segment 2 */
    if(segEntry1Ptr->rowsBase < (segEntry2Ptr->rowsBase + segEntry2Ptr->rowsAmount) &&
       segEntry1Ptr->rowsBase >= segEntry2Ptr->rowsBase)
    {
        /* segment 1 base is in range of segment 2 */
        tmp_segEntry1Ptr = segEntry2Ptr;
        tmp_segEntry2Ptr = segEntry1Ptr;
    }
    else
    {
        /*  check if one segment 'touching' another segment.
            if 'yes' resolve the 'touch' of segments
        */
        rc = segmentTableTouchCheckAndResolve(vTcamMngDBPtr,
                segEntry1Ptr,
                segEntry2Ptr,
                &didTouch);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(didTouch == GT_TRUE)
        {
            *didCollidePtr = GT_TRUE;
            segEntry2Ptr->rowsAmount = 0;/* no more iterations needed */
            return GT_OK;
        }

        return GT_OK;
    }

    *didCollidePtr = GT_TRUE;

    rulesPerRow1 = prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
        tmp_segEntry1Ptr, tmp_segEntry1Ptr->ruleSize);

    /* make segment 1 to hold the part before the common */
    /* make segment 2 to hold the common part */
    /* make 'additional' segment to hold the part after the common */
    /* save orig values */
    origSeg1Data = *tmp_segEntry1Ptr;
    origSeg2Data = *tmp_segEntry2Ptr;

    /* make segment 1 to hold the part before the common */
    tmp_segEntry1Ptr->rowsAmount = tmp_segEntry2Ptr->rowsBase - tmp_segEntry1Ptr->rowsBase;

    /* make segment 2 to hold the common part */

    /* update segment 2 columns maps */
    rc =  prvCpssDxChVirtualTcamDbSegmentTableColumBitmapsMerge(
        tmp_segEntry2Ptr->segmentColumnsMap, origSeg1Data.segmentColumnsMap,
        GT_FALSE /*differentBlocksOnly*/,
        GT_FALSE /*sameBlocksOnly*/,
        GT_FALSE /*singleBlock*/,
        &mergedColumnsMap);
    if (rc != GT_OK)
    {
        return rc;
    }
    tmp_segEntry2Ptr->segmentColumnsMap = mergedColumnsMap;

    /* calc rulesPerRow2 after the |= of the BMP !!! */
    rulesPerRow2 = prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
        tmp_segEntry2Ptr, tmp_segEntry2Ptr->ruleSize);

    lastRow1 = origSeg1Data.rowsBase + origSeg1Data.rowsAmount - 1;
    lastRow2 = origSeg2Data.rowsBase + origSeg2Data.rowsAmount - 1;
    if(lastRow1 <= lastRow2)
    {
        /* update the number of rows in segment 2 */
        tmp_segEntry2Ptr->rowsAmount -= (lastRow2 - lastRow1);

        numRowsSeg3 = (lastRow2 - lastRow1);
        /* segment 3 derive from segment 2 */
        segmentColumnsMap3 = origSeg2Data.segmentColumnsMap;
    }
    else /* lastRow1 > lastRow2 */
    {
        /* no need to update the number of rows in segment 2 */

        numRowsSeg3 = (lastRow1 - lastRow2);
        /* segment 3 derive from segment 1 */
        segmentColumnsMap3 = origSeg1Data.segmentColumnsMap;
    }

    if(tmp_segEntry1Ptr->rowsAmount == 0)
    {
        /* there is no part before the common */
        /* take info from segment 2 into segment 1 */
        *tmp_segEntry1Ptr = *tmp_segEntry2Ptr;
        /* state segment 2 comes after segment 1*/
        tmp_segEntry2Ptr->rowsAmount = numRowsSeg3;
        tmp_segEntry2Ptr->rowsBase = tmp_segEntry1Ptr->rowsBase + tmp_segEntry1Ptr->rowsAmount;
        tmp_segEntry2Ptr->segmentColumnsMap = segmentColumnsMap3;

        numRowsSeg3 = 0;/* no need for additional allocation of segment */

        /* SWAP values of rulesPerRow1,rulesPerRow2 */
        tmpValue = rulesPerRow1;
        rulesPerRow1 = rulesPerRow2;
        rulesPerRow2 = tmpValue;
    }

    /* update the rules amount */
    tmp_segEntry1Ptr->rulesAmount = rulesPerRow1 * tmp_segEntry1Ptr->rowsAmount;
    tmp_segEntry2Ptr->rulesAmount = rulesPerRow2 * tmp_segEntry2Ptr->rowsAmount;

    totalRulesAmount = tmp_segEntry1Ptr->rulesAmount + tmp_segEntry2Ptr->rulesAmount;

    if(numRowsSeg3)
    {
        segEntry3 = *tmp_segEntry2Ptr;
        /* state segment 3 comes after segment 2*/
        segEntry3.rowsBase = tmp_segEntry2Ptr->rowsBase + tmp_segEntry2Ptr->rowsAmount;
        segEntry3.rowsAmount = numRowsSeg3;
        segEntry3.segmentColumnsMap = segmentColumnsMap3;
        /* calc rulesPerRow2 after the change of the BMP !!! */
        rulesPerRow2 = prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
            &segEntry3, segEntry3.ruleSize);
        segEntry3.rulesAmount = rulesPerRow2 * segEntry3.rowsAmount;

        totalRulesAmount += segEntry3.rulesAmount;

        /* this third segment will need to iterate for collide with the next element in the tree */
    }
    else
    {
        /* avoid compiler warning:
           potentially uninitialized local variable 'segEntry3' used*/
        cpssOsMemSet(&segEntry3,0,sizeof(segEntry3));
    }

    if(totalRulesAmount != (origSeg1Data.rulesAmount + origSeg2Data.rulesAmount))
    {
        /* ERROR : wrong calculations ! */

        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* so final conclusions :
       seg1 is already in the tree of vtcam
       if seg3 exists it should be used as 'seg 2' for the next call of this function.
            seg 2 need to be added to the tree
       else seg 2        should be used            for the next call of this function.
            seg 2 need to be added to the tree
            and no more iterations from caller for this segment
    */

    if(segEntry2Ptr->rowsAmount)
    {
        /* add content of segment 2 to the vtcam */
        rc = segmentTableAllocSegmentAndAdd(vTcamMngDBPtr,vTcamCfgPtr,segEntry2Ptr);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(numRowsSeg3)
        {
            /* the caller function we re-iterate to add those 'left overs' into
                the primary tree */
            *segEntry2Ptr = segEntry3;
        }
        else
        {
            segEntry2Ptr->rowsAmount = 0;/* no more iterations needed */
        }
    }


    return GT_OK;
}


/*******************************************************************************
* segmentTableVTcamCombineSingleElement
*
* DESCRIPTION:
*       Combine single element into Virtual TCAM.
*       the function will free the 'buffer' to the 'pool' if the buffer is not
*       needed any more.
*
*       the new element try to bind to existing elements and may cause to
*       enlarge/lower/keep the number of segments in the vtcam
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       vTcamCfgPtr            - (pointer to)virtual TCAM structure
*       segEntryPtr            - (pointer to)segment entry to bind with
* OUTPUTS:
*       vTcamCfgPtr            - (pointer to)updated virtual TCAM structure
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_STATE            - DB is in bad state
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS segmentTableVTcamCombineSingleElement
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *primaryTcamCfgPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntryPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_AVL_TREE_ID    avlTree;
    PRV_CPSS_AVL_TREE_PATH  currentPath;
    PRV_CPSS_AVL_TREE_SEEK_ENT  seekMode;
    GT_VOID_PTR             dbEntryPtr;/* pointer to entry in DB */
    GT_BOOL collide;

    avlTree = primaryTcamCfgPtr->segmentsTree;

    if (prvCpssAvlIsEmpty(primaryTcamCfgPtr->segmentsTree) != GT_FALSE)
    {
        /* expanding null-sized vTCAM */
        return segmentTableAllocSegmentAndAdd(
            vTcamMngDBPtr, primaryTcamCfgPtr, segEntryPtr);
    }

startFullLookupOnTree_lbl:/*start Full Lookup On Tree*/
    seekMode = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;

    while (GT_TRUE == prvCpssAvlPathSeek(avlTree,seekMode,
                currentPath, &dbEntryPtr))
    {
        seekMode = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;

        /* check if one segment 'collide' with another segment or even 'touching'
           another segment.
           if 'yes' resolve the 'collide' of new and current segment */
        rc = segmentTableCollideCheckAndResolve(vTcamMngDBPtr,primaryTcamCfgPtr,
                dbEntryPtr,
                segEntryPtr,
                &collide);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(segEntryPtr->rowsAmount == 0)
        {
            break;
        }

        if(collide == GT_TRUE)
        {
            /* we need to start over again since we changed the tree */
            goto startFullLookupOnTree_lbl;
        }
    }

    if(segEntryPtr->rowsAmount)
    {
        /* there is no touch / collide , so just add the segment to the tree */
        /* add content of segment 2 to the vtcam */
        rc = segmentTableAllocSegmentAndAdd(vTcamMngDBPtr,primaryTcamCfgPtr,segEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        segEntryPtr->rowsAmount = 0;/* no more iterations needed */
    }

    return GT_OK;
}

/*******************************************************************************
* segmentTableVTcamSegmentRemove
*
* DESCRIPTION:
*       remove segment from the vtcam that not used any more.
*       NOTE: this function NOT intended to free the rows/columns it uses.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       vTcamCfgPtr            - (pointer to)virtual TCAM structure
* OUTPUTS:
*       vTcamCfgPtr            - (pointer to)updated virtual TCAM structure
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_STATE            - DB is in bad state
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void segmentTableVTcamSegmentRemove
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *vTcamCfgPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segPtr
)
{
    /* delete current from the tree */
    prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
        vTcamCfgPtr->segmentsTree, segPtr);

    if(segPtr->rowsAmount != 0)
    {
        /* free the range of the segment back to the 'free' ranges */
        prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit(
            vTcamMngDBPtr, segPtr);
    }
    else
    {
        /* free it's buffer from pool */
        cpssBmPoolBufFree(vTcamMngDBPtr->segmentsPool, segPtr);
    }

    return ;
}

/*******************************************************************************
* segmentTableVTcamFullTouchCheckAndCombine
*
* DESCRIPTION:
*       when there are no collides any more between the segments (not suppose to be)
*       but there may still be 'touching' segments that can be combined
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       vTcamCfgPtr            - (pointer to)virtual TCAM structure
* OUTPUTS:
*       vTcamCfgPtr            - (pointer to)updated virtual TCAM structure
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_STATE            - DB is in bad state
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS segmentTableVTcamFullTouchCheckAndCombine
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *vTcamCfgPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_AVL_TREE_SEEK_ENT  seekMode;
    PRV_CPSS_AVL_TREE_PATH  currentPath;
    GT_VOID_PTR             dbEntryPtr;/* pointer to entry in DB */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntryPrevPtr;/*previous*/
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntryCurrPtr;/*current*/
    GT_BOOL didTouch;
    static const GT_U32 maxNumToDelete = 5;
    GT_VOID_PTR toDelArr[5] = {0};
    GT_U32  toDelIndex;/*index in toDelArr[] */
    GT_U32  ii;

startFullLookupOnTree_lbl:/*start Full Lookup On Tree*/

    seekMode = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;
    segEntryPrevPtr = NULL;
    toDelIndex = 0;

    while (GT_TRUE == prvCpssAvlPathSeek(vTcamCfgPtr->segmentsTree,seekMode,
                currentPath, &dbEntryPtr))
    {
        seekMode = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        segEntryCurrPtr = dbEntryPtr;

        if(segEntryPrevPtr)
        {
            rc = segmentTableTouchCheckAndResolve(vTcamMngDBPtr,
                    segEntryPrevPtr,
                    segEntryCurrPtr,
                    &didTouch);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(didTouch == GT_TRUE)
            {
                /* the 2 segments combined into 'previous' */

                if(toDelIndex < maxNumToDelete)
                {
                    /* indicate to delete current */
                    toDelArr[toDelIndex] = dbEntryPtr;
                    toDelIndex++;
                }
                else /* toDelArr[] is full*/
                {
                    for(ii = 0 ; ii < toDelIndex; ii++)
                    {
                        /* remove segments from the vtcam that not used any more. */
                        segmentTableVTcamSegmentRemove(vTcamMngDBPtr,
                            vTcamCfgPtr,
                            toDelArr[ii]);
                    }

                    /* we need to start over again since we changed the tree */
                    goto startFullLookupOnTree_lbl;
                }

                /* continue to 'next' without updating the 'previous' */
                continue;
           }
        }

        segEntryPrevPtr = segEntryCurrPtr;
    }

    /* delete from the tree the leftovers */
    for(ii = 0 ; ii < toDelIndex; ii++)
    {
        /* remove segments from the vtcam that not used any more. */
        segmentTableVTcamSegmentRemove(vTcamMngDBPtr,
            vTcamCfgPtr,
            toDelArr[ii]);
    }

    return GT_OK;
}


/*******************************************************************************
* segmentTableOverlapCheckAndSplit
*
* DESCRIPTION:
*       check if tow segments share some rows (end from one segment and start from other segment)
*       if 'yes' : create new segment with the shared rows and remove it from
*       the other 2 segments.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       vTcamCfgPtr            - (pointer to)virtual TCAM structure
*       segEntry1Ptr            - (pointer to)segment 1 entry to check (lower segment)
*       segEntry2Ptr            - (pointer to)segment 2 entry to check (higher segment)
*
* OUTPUTS:
*       didSplitPtr           - (pointer to) indication that did split.
*                               GT_TRUE - seg1,2 changed and added seg3 (split)
*                               GT_FALSE - seg1,2 not changed
*       segEntry1Ptr            - (pointer to)updated segment 1 entry
*       segEntry2Ptr            - (pointer to)updated segment 2 entry
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_STATE            - DB is in bad state
*       GT_NO_RESOURCE          - no buffers for segments
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS segmentTableOverlapCheckAndSplit
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *vTcamCfgPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntry1Ptr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntry2Ptr,
    OUT    GT_BOOL                                      *didSplitPtr
)
{
    GT_STATUS   rc;
    GT_U32  boundOfLow, boundOfHigh, boundOfShared;
    GT_U32  numSharedRows, sharedBase;
    GT_U32  tmp_rulesPerRow; /* number of rules per row in segment */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *lowSegPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *highSegPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *midSegPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP   mergedColumnsMap;
    GT_U32  rulesPerRowInShared; /* number of rules per row in shared segment */
    GT_BOOL lowSegmentUsed, highSegmentUsed;

    /**********************/
    /* check for split    */
    /**********************/
    *didSplitPtr = GT_FALSE;

    if (segEntry1Ptr->rowsBase <= segEntry2Ptr->rowsBase)
    {
        lowSegPtr   = segEntry1Ptr;
        highSegPtr  = segEntry2Ptr;
    }
    else
    {
        lowSegPtr   = segEntry2Ptr;
        highSegPtr  = segEntry1Ptr;
    }

    if (highSegPtr->rowsBase >= (lowSegPtr->rowsBase + lowSegPtr->rowsAmount))
    {
        /* segment 2 not share rows with segment 1 */
        return GT_OK;
    }

    /* merge the columns with segment 1 */
    rc =  prvCpssDxChVirtualTcamDbSegmentTableColumBitmapsMerge(
        lowSegPtr->segmentColumnsMap, highSegPtr->segmentColumnsMap,
        GT_FALSE /*differentBlocksOnly*/,
        GT_FALSE /*sameBlocksOnly*/,
        GT_FALSE /*singleBlock*/,
        &mergedColumnsMap);
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    sharedBase  = highSegPtr->rowsBase;
    boundOfLow  = lowSegPtr->rowsBase + lowSegPtr->rowsAmount;
    boundOfHigh = highSegPtr->rowsBase + highSegPtr->rowsAmount;
    boundOfShared = ((boundOfLow <= boundOfHigh) ? boundOfLow : boundOfHigh);
    numSharedRows = boundOfShared - highSegPtr->rowsBase;

    prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            vTcamCfgPtr->segmentsTree, lowSegPtr);
    prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            vTcamCfgPtr->segmentsTree, highSegPtr);

    lowSegmentUsed  = GT_FALSE;
    highSegmentUsed = GT_FALSE;

    /* low part below shared if exists */
    if (lowSegPtr->rowsBase < sharedBase)
    {
        tmp_rulesPerRow = (lowSegPtr->rulesAmount / lowSegPtr->rowsAmount);
        lowSegPtr->rowsAmount = (sharedBase - lowSegPtr->rowsBase);
        lowSegPtr->rulesAmount = (tmp_rulesPerRow * lowSegPtr->rowsAmount);
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            vTcamCfgPtr->segmentsTree, lowSegPtr);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
        lowSegmentUsed  = GT_TRUE;
    }

    /* high part upper shared if exists - case when high segment ends upper then shared */
    if (boundOfHigh > boundOfShared)
    {
        tmp_rulesPerRow = (highSegPtr->rulesAmount / highSegPtr->rowsAmount);
        highSegPtr->rowsBase = boundOfShared;
        highSegPtr->rowsAmount = (boundOfHigh - boundOfShared);
        highSegPtr->rulesAmount = (tmp_rulesPerRow * highSegPtr->rowsAmount);
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            vTcamCfgPtr->segmentsTree, highSegPtr);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
        highSegmentUsed = GT_TRUE;
    }

    /* high part upper shared if exists - case when low segment ends upper then high (and shared) */
    /* updated buffer that was belong to high segment                                             */
    if (boundOfLow > boundOfShared)
    {
        if (highSegmentUsed == GT_TRUE)
        {
            /* should never occur */
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        highSegPtr->segmentColumnsMap = lowSegPtr->segmentColumnsMap;
        tmp_rulesPerRow = (lowSegPtr->rulesAmount / lowSegPtr->rowsAmount);
        highSegPtr->rowsBase = boundOfShared;
        highSegPtr->rowsAmount = (boundOfLow - boundOfShared);
        highSegPtr->rulesAmount = (tmp_rulesPerRow * highSegPtr->rowsAmount);
        rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
            vTcamCfgPtr->segmentsTree, highSegPtr);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
        highSegmentUsed = GT_TRUE;
    }

    midSegPtr = NULL;
    if (lowSegmentUsed == GT_FALSE)
    {
        midSegPtr = lowSegPtr;
        lowSegmentUsed  = GT_TRUE;
    }
    else if (highSegmentUsed == GT_FALSE)
    {
        midSegPtr = highSegPtr;
        highSegmentUsed = GT_TRUE;
    }
    else
    {
        midSegPtr = cpssBmPoolBufGet(vTcamMngDBPtr->segmentsPool);
        if (midSegPtr == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }
    }

    if (highSegmentUsed == GT_FALSE)
    {
        cpssBmPoolBufFree(vTcamMngDBPtr->segmentsPool, highSegPtr);
    }

    if (midSegPtr != lowSegPtr)
    {
        cpssOsMemCpy(
            midSegPtr, lowSegPtr,
            sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC));
    }

    midSegPtr->rowsBase          = sharedBase;
    midSegPtr->rowsAmount        = numSharedRows;
    midSegPtr->segmentColumnsMap = mergedColumnsMap;
    rulesPerRowInShared = prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
        midSegPtr,
        midSegPtr->ruleSize);
    midSegPtr->rulesAmount = midSegPtr->rowsAmount * rulesPerRowInShared;

    rc = prvCpssDxChVirtualTcamSegmentTableAvlItemInsert(
        vTcamCfgPtr->segmentsTree, midSegPtr);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    *didSplitPtr = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* segmentTableVTcamFullOverlapCheckAndSplit
*
* DESCRIPTION:
*      the segments from the 'secondary' tree were merged into
*      the 'primary' tree.
*
*      and now the 'primary tree' may hold consecutive segments that share some rows
*      (end from one segment and start from other segment)
*      for such case need to create new segment (split) with the shared rows and
*      remove it from the other 2 segments.
*
*      otherwise segmentTableVTcamBaseLogicalIndexUpdate(...) and
*      prvCpssDxChVirtualTcamDbSegmentTableLogToPhyIndexTabGenerate(...)
*      will not build proper values in primaryTcamCfgPtr->rulePhysicalIndexArr[]
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       vTcamCfgPtr            - (pointer to)virtual TCAM structure
* OUTPUTS:
*       vTcamCfgPtr            - (pointer to)updated virtual TCAM structure
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_STATE            - DB is in bad state
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS segmentTableVTcamFullOverlapCheckAndSplit
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *vTcamCfgPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_AVL_TREE_SEEK_ENT  seekMode;
    PRV_CPSS_AVL_TREE_PATH  currentPath;
    GT_VOID_PTR             dbEntryPtr;/* pointer to entry in DB */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntryPrevPtr;/*previous*/
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntryCurrPtr;/*current*/
    GT_BOOL didSplit;

startFullLookupOnTree_lbl:/*start Full Lookup On Tree*/

    seekMode = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;
    segEntryPrevPtr = NULL;

    while (GT_TRUE == prvCpssAvlPathSeek(vTcamCfgPtr->segmentsTree,seekMode,
                currentPath, &dbEntryPtr))
    {
        seekMode = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        segEntryCurrPtr = dbEntryPtr;

        if(segEntryPrevPtr)
        {
            /* check if the 2 segments need to split to add new segment with the shared info */
            rc = segmentTableOverlapCheckAndSplit(vTcamMngDBPtr,vTcamCfgPtr,
                    segEntryPrevPtr,
                    segEntryCurrPtr,
                    &didSplit);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(didSplit == GT_TRUE)
            {
                /* the 2 segments split into 3 segments */
                /* we need to start over again since we changed the tree */
                goto startFullLookupOnTree_lbl;
            }
        }

        segEntryPrevPtr = segEntryCurrPtr;
    }

    return GT_OK;
}


/*******************************************************************************
* segmentTableVTcamFullCombine
*
* DESCRIPTION:
*       add segmentsTree of secondary Virtual TCAM into primary Virtual TCAM.
*       the function free any secondary resource related to segmentsTree that
*       not needed any more.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       primaryTcamCfgPtr      - (pointer to)primary virtual TCAM structure
*       secondaryTcamCfgPtr    - (pointer to)secondary virtual TCAM structure
*
* OUTPUTS:
*       primaryTcamCfgPtr      - (pointer to)updated primary virtual TCAM structure
*
* RETURNS:
*       GT_OK                   - on success
*       GT_NOT_INITIALIZED      - the given tree has not been initialized.
*       GT_BAD_STATE            - In case the tree is not empty.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS segmentTableVTcamFullCombine
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *primaryTcamCfgPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *secondaryTcamCfgPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_AVL_TREE_ID    avlSecondary;
    PRV_CPSS_AVL_TREE_PATH  pathSecondary;
    GT_VOID_PTR             dbEntryPtr;/* pointer to entry in DB */

    avlSecondary = secondaryTcamCfgPtr->segmentsTree;

    /* get element from secondary */
    while (GT_TRUE == prvCpssAvlPathSeek(avlSecondary,
                PRV_CPSS_AVL_TREE_SEEK_FIRST_E,/* we remove the first so we keep popping first*/
                pathSecondary, &dbEntryPtr))
    {
        /* remove the element from the secondary , before we may modify it's content */
        if(NULL == prvCpssDxChVirtualTcamSegmentTableAvlItemRemove(
            avlSecondary, dbEntryPtr))
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* combine this element into the primary */
        rc = segmentTableVTcamCombineSingleElement(vTcamMngDBPtr,
            primaryTcamCfgPtr,dbEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* restore the element to the pool of free */
        rc = cpssBmPoolBufFree(vTcamMngDBPtr->segmentsPool, dbEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamCombine
*
* DESCRIPTION:
*       Combine segmentsTree of secondary Virtual TCAM into primary Virtual TCAM.
*       the function free any secondary resource related to segmentsTree that
*       not needed any more.
*       the function fully align the info in the tree of primaryTcamCfgPtr->segmentsTree
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr          - (pointer to)virtual TCAM manager structure
*       primaryTcamCfgPtr      - (pointer to)primary virtual TCAM structure
*       secondaryTcamCfgPtr    - (pointer to)secondary virtual TCAM structure
*
* OUTPUTS:
*       primaryTcamCfgPtr      - (pointer to)updated primary virtual TCAM structure
*
* RETURNS:
*       GT_OK                   - on success
*       GT_NOT_INITIALIZED      - the given tree has not been initialized.
*       GT_BAD_STATE            - In case the tree is not empty.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamCombine
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *primaryTcamCfgPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *secondaryTcamCfgPtr
)
{
    GT_STATUS   rc;

    /* combine segments from secondary into primary */
    rc = segmentTableVTcamFullCombine(vTcamMngDBPtr,primaryTcamCfgPtr,secondaryTcamCfgPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable != GT_FALSE)
    {
        cpssOsPrintf("after segmentTableVTcamFullCombine\n");
        prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree(primaryTcamCfgPtr->segmentsTree);
    }

    /***************************************************************/
    /* the vtcam now hold in the segments tree the needed segments */
    /* from the secondary tree                                     */
    /* and the secondary tree is no longer valid                   */
    /***************************************************************/

    /* there are no collides any more between the segments (not suppose to be) */
    /* but there may still be 'touching' segments that can be combined */
    rc = segmentTableVTcamFullTouchCheckAndCombine(vTcamMngDBPtr,primaryTcamCfgPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable != GT_FALSE)
    {
        cpssOsPrintf("after segmentTableVTcamFullTouchCheckAndCombine\n");
        prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree(primaryTcamCfgPtr->segmentsTree);
    }

    /* the segments from the 'secondary' tree were merged into
       the 'primary' tree.

       and now the 'primary tree' may hold consecutive segments that share some rows
       (end from one segment and start from other segment)
       for such case need to create new segment (split) with the shared rows and
       remove it from the other 2 segments.

       otherwise segmentTableVTcamBaseLogicalIndexUpdate(...) and
        prvCpssDxChVirtualTcamDbSegmentTableLogToPhyIndexTabGenerate(...)
        will not build proper values in primaryTcamCfgPtr->rulePhysicalIndexArr[]
   */
    rc = segmentTableVTcamFullOverlapCheckAndSplit(vTcamMngDBPtr,primaryTcamCfgPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeMrgSplTraceEnable != GT_FALSE)
    {
        cpssOsPrintf("after segmentTableVTcamFullOverlapCheckAndSplit\n");
        prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree(primaryTcamCfgPtr->segmentsTree);
    }

    /* update rule amounts */
    prvCpssDxChVirtualTcamDbSegmentTableUpdateRuleAmounts(
        vTcamMngDBPtr, primaryTcamCfgPtr->segmentsTree);

    /* update the baseLogicalIndex in the segments of the vtcam */
    /* and primaryTcamCfgPtr->rulesAmount */
    segmentTableVTcamBaseLogicalIndexUpdate(primaryTcamCfgPtr);

    /* realloc logical to physical conversion array */
    if (primaryTcamCfgPtr->rulePhysicalIndexArr != NULL)
    {
        cpssOsFree(primaryTcamCfgPtr->rulePhysicalIndexArr);
    }
    primaryTcamCfgPtr->rulePhysicalIndexArr =
        (GT_U16*)cpssOsMalloc(
            (sizeof(GT_U16) * primaryTcamCfgPtr->rulesAmount));
    if (primaryTcamCfgPtr->rulePhysicalIndexArr == NULL)
    {
        prvCpssDxChVirtualTcamDbSegmentTableException();
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* Re-Generate table for conversion logical indexes to physical indexes of rules. */
    prvCpssDxChVirtualTcamDbSegmentTableLogToPhyIndexTabGenerate(
        vTcamMngDBPtr,
        primaryTcamCfgPtr->segmentsTree,
        primaryTcamCfgPtr->rulePhysicalIndexArr);

    /* check to compress the 'free' segments */
    prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMerge(vTcamMngDBPtr);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableDetachSpace
*
* DESCRIPTION:
*      the function detach the last rules from the vtcam and restore them into
*       'free' segments tree
*       the function may remove one or more segments that hold the rules range.
*       the function may split one more segment to reduce number of rules in it.
*           NOTE: updates the vTcamInfoPtr->rulesAmount according to actual number of
*                 rules that can be used by the vtcam after the operation.
*                 this value may be more than numOfRulesToKeep due to rules granularity
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr    - (pointer to)virtual TCAM manager structure
*       vTcamCfgPtr      - (pointer to) virtual TCAM structure
*       vTcamId          - virtual TCAM id
*       fromEndOrStart  - the type of direction
*                       GT_TRUE  - hole from end   of range
*                       GT_FALSE - hole from start of range
*       numOfRulesToKeep - number of rules that need to keep , so may release
*                          what ever is more than that.
*                           NOTE: there may be 'align' issues that may release
*                           a little less rules.
*
* OUTPUTS:
*       vTcamCfgPtr      - (pointer to)updated virtual TCAM structure
*
* RETURNS:
*       GT_OK                   - on success
*       GT_NOT_INITIALIZED      - the given tree has not been initialized.
*       GT_BAD_STATE            - In case the tree is not empty.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableDetachSpace
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *vTcamCfgPtr,
    IN     GT_U32                                       vTcamId,
    IN     GT_BOOL                                      fromEndOrStart,
    IN     GT_U32                                       numOfRulesToKeep
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntryPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *newSplitSegPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC *segEntryToSplitPtr = NULL;
    PRV_CPSS_AVL_TREE_ID    avlTree;
    PRV_CPSS_AVL_TREE_PATH  currentPath;
    GT_VOID_PTR             dbEntryPtr;/* pointer to entry in DB */
    static const GT_U32 maxNumToDelete = 5;
    GT_VOID_PTR toDelArr[5] = {0};
    GT_U32  toDelIndex;/*index in toDelArr[] */
    GT_U32  maxRulesToRemove;/*max rules that may be removed due to rules granularity*/
    PRV_CPSS_AVL_TREE_SEEK_ENT  seekMode;
    GT_U32  totalRemoved = 0;
    GT_U32  ii;
    GT_U32  rulesPerRow; /* number of rules per row            */
    GT_U32  lastSegNumRules;/* number of rules in the last segment left in the vtcam */
    GT_U32  firstSegNumRules;/* number of rules in the first segment left in the vtcam */

    CPSS_LOG_INFORMATION_MAC("start request to keep [%d] out of [%d] rules \n",
        numOfRulesToKeep , vTcamCfgPtr->rulesAmount);

    if(vTcamCfgPtr->rulesAmount < numOfRulesToKeep)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    maxRulesToRemove = vTcamCfgPtr->rulesAmount - numOfRulesToKeep;

    /* find first segment in the vtcam that covers the new needed size */

    avlTree = vTcamCfgPtr->segmentsTree;

startFullLookupOnTree_lbl:/*start Full Lookup On Tree*/
    if(fromEndOrStart == GT_TRUE)
    {
        seekMode = PRV_CPSS_AVL_TREE_SEEK_LAST_E;
    }
    else
    {
        seekMode = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;
    }
    toDelIndex = 0;

    while (GT_TRUE == prvCpssAvlPathSeek(avlTree,seekMode,
                currentPath, &dbEntryPtr))
    {
        if(fromEndOrStart == GT_TRUE)
        {
            seekMode = PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E;
        }
        else
        {
            seekMode = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        }

        segEntryPtr = dbEntryPtr;

        if((segEntryPtr->rulesAmount + totalRemoved) <= maxRulesToRemove)
        {
            /* this segment can be fully removed from the list of vtcam */
            CPSS_LOG_INFORMATION_MAC("remove segment fully with [%d] rules\n",
                segEntryPtr->rulesAmount);

            if(toDelIndex < maxNumToDelete)
            {
                /* indicate to delete current */
                toDelArr[toDelIndex] = dbEntryPtr;
                toDelIndex++;
            }
            else /* toDelArr[] is full*/
            {
                for(ii = 0 ; ii < toDelIndex; ii++)
                {
                    /* remove segments from the vtcam that not used any more. */
                    segmentTableVTcamSegmentRemove(vTcamMngDBPtr,
                        vTcamCfgPtr,
                        toDelArr[ii]);
                }

                /* we need to start over again since we changed the tree */
                goto startFullLookupOnTree_lbl;
            }
        }
        else
        {
            /* this segment need to reduce the number of rules it hold in
                current segment , and free the rest */
            segEntryToSplitPtr = segEntryPtr;
            break;
        }

        totalRemoved += segEntryPtr->rulesAmount;
    }

    /* delete from the tree the leftovers */
    for(ii = 0 ; ii < toDelIndex; ii++)
    {
        /* remove segments from the vtcam that not used any more. */
        segmentTableVTcamSegmentRemove(vTcamMngDBPtr,
            vTcamCfgPtr,
            toDelArr[ii]);
    }

    if(segEntryToSplitPtr)
    {
        /* this segment need to reduce the number of rules it hold in
            current segment , and free the rest */
        rulesPerRow = prvCpssDxChVirtualTcamDbSegmentTableMaxRowRulesAmountGet(
            segEntryToSplitPtr,
            (vTcamCfgPtr->tcamInfo.ruleSize+1));

        /*since we split this segment , get new buffer from the pool*/
        newSplitSegPtr = cpssBmPoolBufGet(vTcamMngDBPtr->segmentsPool);
        if (newSplitSegPtr == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableException();
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }

        /* copy the original values */
        *newSplitSegPtr = *segEntryToSplitPtr;

        if(fromEndOrStart == GT_TRUE)
        {
            /* calculate values for the segment that is kept in the vtcam */
            lastSegNumRules = newSplitSegPtr->rulesAmount - (maxRulesToRemove - totalRemoved);
            /* new number of rows */
            segEntryToSplitPtr->rowsAmount =
                (lastSegNumRules + (rulesPerRow - 1)) / rulesPerRow;

            /* new number of rules */
            segEntryToSplitPtr->rulesAmount = segEntryToSplitPtr->rowsAmount * rulesPerRow;

            /* calculate the value that need to 'free' */
            newSplitSegPtr->rowsAmount  -= segEntryToSplitPtr->rowsAmount;
            newSplitSegPtr->rulesAmount -= segEntryToSplitPtr->rulesAmount;
            newSplitSegPtr->rowsBase    += segEntryToSplitPtr->rowsAmount;
        }
        else
        {
            /* calculate values for the segment that is kept in the vtcam */
            firstSegNumRules = vTcamCfgPtr->rulesAmount - (numOfRulesToKeep + totalRemoved);

            /* calculate the value that need to 'free' */
            /* new number of rows */
            newSplitSegPtr->rowsAmount =
                firstSegNumRules / rulesPerRow;

            /* new number of rules */
            newSplitSegPtr->rulesAmount = newSplitSegPtr->rowsAmount * rulesPerRow;

            /* calculate the kept segment */
            segEntryToSplitPtr->rowsBase += newSplitSegPtr->rowsAmount;
            segEntryToSplitPtr->rowsAmount  -= newSplitSegPtr->rowsAmount;
            segEntryToSplitPtr->rulesAmount -= newSplitSegPtr->rulesAmount;
        }

        totalRemoved += newSplitSegPtr->rulesAmount;

        if(newSplitSegPtr->rowsAmount == 0)
        {
            CPSS_LOG_INFORMATION_MAC("no reduction on the last segment (due to granularity) \n");

            cpssBmPoolBufFree(vTcamMngDBPtr->segmentsPool,newSplitSegPtr);
        }
        else
        {
            CPSS_LOG_INFORMATION_MAC("reduce number of rules by [%d] left the segment with [%d] rules \n",
                newSplitSegPtr->rulesAmount , segEntryToSplitPtr->rulesAmount);

            /* free the range of the segment back to the 'free' ranges */
            rc = prvCpssDxChVirtualTcamDbSegmentTableChainFreeSegmentWithSplit(
                vTcamMngDBPtr, newSplitSegPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }


    {
        GT_BOOL didChange;
        GT_U32  totalRules = vTcamCfgPtr->rulesAmount - totalRemoved;
        PRV_CPSS_AVL_TREE_ID treeId = vTcamCfgPtr->segmentsTree;
        PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC *pVTcamMngPtr = vTcamMngDBPtr;
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC allocRequest,*allocRequestPtr = &allocRequest;

        allocRequest.lookupId =
            PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_MAC(
                vTcamCfgPtr->tcamInfo.clientGroup, vTcamCfgPtr->tcamInfo.hitNumber);

        allocRequest.vTcamId  = vTcamId;
        allocRequest.rulesAmount = numOfRulesToKeep;
        switch (vTcamCfgPtr->tcamInfo.ruleSize)
        {
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E:
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E:
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E:
                allocRequest.ruleAlignment = vTcamCfgPtr->tcamInfo.ruleSize + 1;
                break;
            default:
                allocRequest.ruleAlignment = 6;
                break;
        }
        allocRequest.ruleSize =
            (vTcamCfgPtr->tcamInfo.ruleSize - CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E + 1);

        /* reduce last half row from the allocation if got too much rules */
        rc = prvCpssDxChVirtualTcamDbSegmentTablePrimaryTreeReduceHalfRow(
            pVTcamMngPtr, allocRequestPtr, treeId,totalRules, &didChange);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }


    /* update rule amounts */
    prvCpssDxChVirtualTcamDbSegmentTableUpdateRuleAmounts(
        vTcamMngDBPtr, vTcamCfgPtr->segmentsTree);

    /* update the baseLogicalIndex in the segments of the vtcam */
    /* and primaryTcamCfgPtr->rulesAmount */
    segmentTableVTcamBaseLogicalIndexUpdate(vTcamCfgPtr);

    CPSS_LOG_INFORMATION_MAC("ended request . actual keep [%d] rules (requested [%d]) , after removed [%d]\n",
        vTcamCfgPtr->rulesAmount,numOfRulesToKeep,totalRemoved);

    if(fromEndOrStart == GT_TRUE)
    {
        /* NOTE:
            no need to update vTcamCfgPtr->rulePhysicalIndexArr
            because it only hold 'extra' mapping , but it not harm the operation.
        */
    }
    else
    {
        /* NOTE:
            no need to update vTcamCfgPtr->rulePhysicalIndexArr
            because it was needed to be recalculated by the caller before this stage.
        */
    }

    /* check to compress the 'free' segments */
    prvCpssDxChVirtualTcamDbSegmentTableFreeSpaceMerge(vTcamMngDBPtr);

    return GT_OK;
}


