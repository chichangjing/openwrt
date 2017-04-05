/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChVirtualTcamDb.h
*
* DESCRIPTION:
*       The CPSS DXCH Virtual TCAM Manager internal batabase
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChVirtualTcamDb_h
#define __prvCpssDxChVirtualTcamDb_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/virtualTcam/cpssDxChVirtualTcam.h>
#include <cpssCommon/private/prvCpssAvlTree.h>

extern GT_BOOL prvCpssDxChVirtualTcamDebugTraceEnable;

/* temporary prints, will be changed to be empty */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_DBG_PRINT(_x)         \
    if (prvCpssDxChVirtualTcamDebugTraceEnable != GT_FALSE) \
    {                                                       \
        cpssOsPrintf("line %4.4d: ",__LINE__);              \
        cpssOsPrintf _x;                                    \
        cpssOsPrintf("\n");                                 \
    }

/* HW index indicating 'not used' */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS   0xFFFFFFFF

typedef struct{
    GT_U32  oldLogicalIndex;
    GT_U32  newLogicalIndex;
}PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RESIZE_HW_INDEX_MAP_INFO_STC;

/* convert rule size to step size */
#define STEP_SIZE_MAC(ruleSize) ((ruleSize)>=CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E) ?  6 : ((ruleSize)+1)

/* dynamic allocated array */
extern PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RESIZE_HW_INDEX_MAP_INFO_STC    *prvCpssDxChVirtualTcamDbResizeHwIndexDbArr;
extern GT_U32                           prvCpssDxChVirtualTcamDbNumEntries_resizeHwIndexDbArr;

/**********************************************************************************/
/* Common                                                                         */
/**********************************************************************************/

/**********************************************************************************/
/* Virtual TCAMs Segment Data and structures                                                        */
/**********************************************************************************/

/* This library should use lookup concept for pair of client group and hit number */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_MAC(client_group, hit_number) \
    ((client_group << 8) | hit_number)
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_TO_CLIENT_GROUP_MAC(lookup_id) \
    (lookup_id >> 8)
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_TO_HIT_NUMBER_MAC(lookup_id) \
    (lookup_id & 0xFF)

/* value for free space in segments table */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_VTAM_ID_FREE_SPACE_CNS 0xFFFFFFFF
/* value for free block in TCAM blocks table */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_FREE_BLOCK_CNS 0xFFFFFFFF

/* amount of rules added / removed by autoresize */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_AUTO_RESIZE_GRANULARITY_CNS 12

/*
 * Typedef: function PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_MAP_FUNC
 *
 * Description:
 *     This Function maps TCAM segment to lookup (i.e. pair of client_group and hit_number).
 *
 * INPUTS:
 *       cookiePtr      - device info passed to callback function.
 *       baseRow        - base row of mapped segment.
 *       baseColumn     - base column of mapped segment.
 *       lookupId       - id of lookup for mapping.
 *
 *  Comments:
 *
 */
typedef GT_STATUS PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_MAP_FUNC
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      baseRow,
    IN  GT_U32      baseColumn,
    IN  GT_U32      lookupId
);

/*
 * Typedef: function PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_RULE_INVALIDATE_FUNC
 *
 * Description:
 *     This Function invalidates rule in TCAM.
 *
 * INPUTS:
 *       cookiePtr            - device info passed to callback function.
 *       rulePhysicalIndex    - rule physical index.
 *       ruleSize             - rule size in TCAM minimal rules (10 bytes)
 *
 *  Comments:
 *
 */
typedef GT_STATUS PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_RULE_INVALIDATE_FUNC
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      rulePhysicalIndex,
    IN  GT_U32      ruleSize
);

/*
 * Typedef: function PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_RULE_MOVE_FUNC
 *
 * Description:
 *     This Function moves rule in TCAM.
 *
 * INPUTS:
 *       cookiePtr               - device info passed to callback function.
 *       srcRulePhysicalIndex    - source rule physical index.
 *       dstRulePhysicalIndex    - destination rule physical index.
 *       ruleSize                - rule size in TCAM minimal rules (10 bytes)
 *
 *  Comments:
 *
 */
typedef GT_STATUS PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_RULE_MOVE_FUNC
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      srcRulePhysicalIndex,
    IN  GT_U32      dstRulePhysicalIndex,
    IN  GT_U32      ruleSize
);

/*
 * Typedef: struct PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_CFG_STC
 *
 * Description:
 *     This structure of DB segments table configuration
 *
 * Fields:
 *       tcamColumnsAmount      - number of columns in TCAM.
 *                                for all supported devices it is 12.
 *       tcamRowsAmount         - Amount of rows in TCAM.
 *
 *       Lookup mapping granularity
 *       lookupColumnAlignment  - alignment of base column of partition that can be mapped for lookup
 *                                For Bobcat2 and Bobcat3 - 6, for Caelum - 2
 *                                But current version should specify 6 for any.
 *       lookupRowAlignment     - alignment of base row of partition that can be mapped for lookup
 *                                For all supported devices 256
 *       cookiePtr              - device info to be passed to callbacks
 *       lookupMapFuncPtr       - (pointer to)callback function for HW mapping of TCAM partitions
 *                                to lookup (i.e. pair of client_group and hit_number).
 *       ruleInvalidateFuncPtr  - (pointer to)callback function for HW rule invalidate
 *       ruleMoveFuncPtr        - (pointer to)callback function for HW rule move
 *       maxSegmentsInDb      - maximal amount of segments in the table.
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32                                                      tcamColumnsAmount;
    GT_U32                                                      tcamRowsAmount;
    GT_U32                                                      lookupColumnAlignment;
    GT_U32                                                      lookupRowAlignment;
    GT_VOID                                                     *cookiePtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_LOOKUP_MAP_FUNC       *lookupMapFuncPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_RULE_INVALIDATE_FUNC  *ruleInvalidateFuncPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_RULE_MOVE_FUNC        *ruleMoveFuncPtr;
    GT_U32                                                      maxSegmentsInDb;
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_CFG_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC
 *
 * Description:
 *     This structure of DB segments table allocation request
 *
 * Fields:
 *       lookupId      - lookup Id (combined from client group and hit index).
 *       vTcamId       - vTCAM Id  (for vTCAM that will contain memory).
 *       rulesAmount   - Amount of rules in vTCAM.
 *       ruleAlignment - rule Alignment.
 *       ruleSize      - rule  Size.
 *                       (APPLICABLE  RANGES: 1..6)
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32      lookupId;
    GT_U32      vTcamId;
    GT_U32      rulesAmount;
    GT_U32      ruleAlignment;
    GT_U32      ruleSize;
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC;

/*
 * Typedef: enum PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT
 *
 * Description:
 *     Columns covered by TCAM memory segment (in block scope).
 * Enumerations:
 *     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E  - no columns used (for parallel horizontal blocks)
 *     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E     - column 5, remainder after 50-byte rule
 *     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E    - columns 4..5, remainder after 40-byte rule
 *     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E  - columns 0..3, 40-byte rule
 *     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E - columns 0..4, 50-byte rule
 *     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E   - all columns (0..5), 60-byte rule
 *     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E  - amount of members in this ENUM
 *
 *  Comments:
 *  Complement for x is (ALL==5 - x) 3-columns type not supported
 */
typedef enum
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E  = 0, /* don't change any values */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E     = 1, /* 1 column */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E    = 2, /* 2 columns */
    /* 3 columns type not supported */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E  = 3, /* 4 columns */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E = 4, /* 5 columns */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E   = 5, /* 6 columns */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E  = 6
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ENT;

/* bitmaps of 10-byte columns in one horizontal block */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_BITBAPS_MAC          \
    {                                                                  \
        /* PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E  */ 0,     \
        /* PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_5_E     */ 0x20,  \
        /* PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_45_E    */ 0x30,  \
        /* PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_0123_E  */ 0x0F,  \
        /* PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_01234_E */ 0x1F,  \
        /* PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E   */ 0x3F   \
    }

/* width of bitmaps of 10-byte columns in one horizontal block */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_BITBAPS_WIDTH_CNS 6

/* map of horizontal blocks using in segment */
typedef GT_U32 PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP;

/* row bitmap empty for all horizontal blocks */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_EMPTY_CNS 0

/* Max supported amount of block columns */
/* this value can be increased up to 8, but yet not needed */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_MAX_SUPPORTED_CNS 2

/* row bitmap complement */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_COMPLEMENT_MAC(_x) \
    (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ALL_E - _x)

/* row bitmap of 10-byte columns by enum value */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_BITMAP_OF_COLUMNS_MAC(_enum, _bmp) \
    {static GT_U8 __arr[] = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_BITBAPS_MAC; _bmp = __arr[_enum];}

/* enum value by row bitmap of 10-byte columns */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_COLUMNS_BY_BITMAP_MAC(_bmp, _enum) \
    {                                                                                        \
        static GT_U8 __arr[] = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_BITBAPS_MAC;        \
        GT_U32 __i;                                                                          \
        _enum = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E;                            \
        for (__i = 0; (__i < (sizeof(__arr) / sizeof(__arr[0]))); __i++)                     \
        {                                                                                    \
            if (__arr[__i] == _bmp) {_enum = __i;}                                           \
        }                                                                                    \
    }

/* set and get SEG_COLUMNS to the ROW MAP */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_SET_MAC(_rowMap,_horzBlock,_columns) \
    _rowMap = (_rowMap & (~ (0xF << (4 * _horzBlock)))) | (_columns << (4 * _horzBlock))
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(_rowMap,_horzBlock) \
    ((_rowMap >> (4 * _horzBlock))& 0xF)

/* find first Horizontal block in columns using map */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_FIND_FIRST_MAC(_rowMap,_horzBlock,_columns) \
    _columns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E;                                      \
    for (_horzBlock = 0;                                                                              \
        (_horzBlock < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_MAX_SUPPORTED_CNS);                   \
        _horzBlock++)                                                                                 \
    {                                                                                                 \
        _columns = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP_GET_MAC(                         \
            _rowMap, _horzBlock);                                                                     \
        if (_columns  != PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_NONE_E)                            \
        {                                                                                             \
            break;                                                                                    \
        }                                                                                             \
    }


/* names for dump */
typedef char* PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_CHAR_PTR;

extern  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_CHAR_PTR
    prvCpssDxChVirtualTcamDbSegmentColumnsEnumNames[
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E];

/*
 * Typedef: struct PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC
 *
 * Description:
 *     This structure of DB segments table entry
 *
 * Fields:
 *       vTcamId           - id of owning vTcam (or constant for free space)
 *       baseLogicalIndex  - first logical index of the segment
 *       rulesAmount       - rules amount
 *       ruleSize          - size of rule in columns
 *       lookupId          - lookup Id. (Used to search block where to move)
 *       rowsBase          - base row number.
 *       rowsAmount        - amount of rows.
 *       segmentColumnsMap - compressed array of enum values per horizontal block
 *                           describing used space.
 *
 *  Comments:
 *
 */
typedef struct
{
    /* memory owner */
    GT_U32                                            vTcamId;
    GT_U32                                            baseLogicalIndex;
    GT_U32                                            rulesAmount;
    GT_U32                                            ruleSize;
    /* memory layout */
    GT_U32                                            lookupId;
    GT_U32                                            rowsBase;
    GT_U32                                            rowsAmount;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_ROW_MAP segmentColumnsMap;
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC;

typedef PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC
    *PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_PTR;

/*
 * Typedef: struct PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC
 *
 * Description:
 *     This structure of DB segments block state
 *
 * Fields:
 *       columnsBase      - base column number.
 *       rowsBase         - base row number.
 *       columnsAmount    - amount of columns.
 *       rowsAmount       - amount of rows.
 *       freeSpace        - free space statistics fo all types of free places.
 *       lookupId         - lookup Id. (or constant for free block)
 *
 *  Comments:
 *
 */
typedef struct
{
    /* initialized only */
    GT_U32      columnsBase;
    GT_U32      rowsBase;
    GT_U32      columnsAmount;
    GT_U32      rowsAmount;
    /* updated */
    GT_U32      lookupId;
    GT_U32      freeSpace[PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_COLUMNS_LAST_E];
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC;

/* typedef for pointer to structure */
typedef PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC
    *PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_PTR;

/**********************************************************************************/
/* Virtual TCAMs CFG Table                                                        */
/**********************************************************************************/

/*
 * Typedef: struct PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC
 *
 * Description:
 *     This structure of DB of vTCAMs.
 *
 * Fields:
 *       tcamInfo             - structure with data for vTCAM create.
 *       rulesAmount          - amount of rules
 *       usedRulesAmount      - amount of used rules
 *       usedRulesBitmapArr   - (pointer to)array of 32-bit words containing
 *                              1 (used) or 0 (unused) rules state per logical
 *                              index.
 *
 *       rulePhysicalIndexArr - (pointer to)array of physical indexes of rules.
 *                       Used to fast conversion logical index to physical.
 *                       Recalculated at any segment move.
 *                       Reallocated on Resize.
 *      segmentsTree          - tree of segments of space in TCAM
 *                              using PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ENTRY_STC
 *
 ************start of priority derive vTCAMs only***********
 *
 *      ruleIdIdTree          - head of Tree with key vTcamId+RuleId
 *                              uses shared nodes pool ruleIdIdNodesPool
 *                              used to convert ruleId to logical index
 *                              using PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC
 *      ruleIdIndexTree       - head of Tree with key vTcamId+logicalIndex
 *                              uses shared nodes pool ruleIdIndexNodesPool
 *                              used to convert logical index to ruleId
 *                              using PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC
 *      priorityPriTree       - head of Tree with key vTcamId+priority
 *                              uses shared nodes pool priorityPriNodesPool
 *                              used to convert priority to logical index
 *                              using PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC
 *      priorityIndexTree     - head of Tree with key vTcamId+priority
 *                              used to convert logical index to priority
 *                              using PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC
 *
 ************end of priority derive vTCAMs only***********
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_DXCH_VIRTUAL_TCAM_INFO_STC tcamInfo;
    /* handles of dynamic created objects will be added */
    GT_U32                          rulesAmount;
    GT_U32                          usedRulesAmount;
    GT_U32                          *usedRulesBitmapArr;
    GT_U16                          *rulePhysicalIndexArr;
    PRV_CPSS_AVL_TREE_ID            segmentsTree;
    /* search AVL trees for priority derive vTCAMs only */
    PRV_CPSS_AVL_TREE_ID            ruleIdIdTree;
    PRV_CPSS_AVL_TREE_ID            ruleIdIndexTree;
    PRV_CPSS_AVL_TREE_ID            priorityPriTree;
    PRV_CPSS_AVL_TREE_ID            priorityIndexTree;
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC;

/* typedef for pointer to structure */
typedef PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC
    *PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_PTR;

/* bitmap of devices used by Vitrtual TCAM Manarer */
typedef GT_U32 PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP[PRV_CPSS_MAX_PP_DEVICES_CNS/32];

/* macro to return error when the tcam manager hold no devices */
#define  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(_vTcamMngId) \
    if(0 == vTcamMngDB[_vTcamMngId]->numOfDevices)                             \
    {                                                                          \
       /* the vTcamMngDB[_vTcamMngId]->devsBitmap[] is all zero's */           \
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE,                 \
        "The tcam manager hold no devices");                                   \
    }

/* macro to return error when the priority parameter is out of range */
#define PRV_CPSS_DXCH_VIRTUAL_TCAM_PRIORITY_PARAM_CHECK_MAC(_priority)  \
    if(_priority >= _64K)                                               \
    {                                                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"The priority[%d] is out of range[0..%d]", \
            _priority,_64K);                                            \
    }

/*
 * Typedef: struct PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC
 *
 * Description:
 *     This structure describes Virtual TCAM Manager.
 *
 * Fields:
 *     vTcamMngId       - VTCAM Manager Id.
 *     disableRearrange - rearrange existing vTCAMs on TCAM space allocation
 *                        when there is no enough ready free space
 *                        GT_FALSE  - rearrange(default),
 *                        GT_TRUE - no rearrange (used for space availability checking).
 *     devsBitmap       - bitmap of used devices
 *     numOfDevices     - number of devices in devsBitmap
 *     devFamily        - device family served by this vTcam manager instance
 *     tcamSegCfg       - configuration of segment manager
 *     blocksAmount       amount of TCAM blocks in array
 *     blockInfArr      - array of TCAM blocks that be used for allocation
 *                        TCAM manager can use no all of blocks.
 *                        Blocks can be defined with granularity greater
 *                        than HW supports.
 *     segmentsPool            - pool for segment structures
 *     segNodesPoolFreePref    - pool for nodes of segment allocation preferences tree
 *     segFreePrefTree         - tree of free space segments sorted by allocation preferences
 *     segNodesPoolFreeLu      - pool for nodes of segment lookup ordered tree
 *     segFreeLuTree           - tree of free space segments sorted by lookup order
 *     segNodesPool            - pool for nodes of segment access trees
 *                               (vTcam layout and temporary trees)
 *     -- rule Id table for priority driven vTCAMs only --
 *     ruleIdPool             - pool of Rule Id Table Entries
 *     ruleIdIdNodesPool      - pool of Nodes of Tree with key RuleId (Rule Id Table)
 *     ruleIdIndexNodesPool   - pool of Nodes of Tree with key logicalIndex (Rule Id Table)
 *     -- Priority table for priority driven vTCAMs only --
 *     priorityPool           - pool of Priority Table Entries
 *     priorityNodesPool      - pool of Nodes of Tree with key Priority (Priority Table)
 *     priorityIndexNodesPool - pool of Nodes of Tree with key logicalIndex (Priority Table)
 *     ------------------------------------------------------------
 *     vTcamCfgArrSize - size of array of vTCAMs Infos (maximal amount of vTCAMs).
 *     vTcamCfgArr     - (pointer to)array of pointers to vTCAMs Infos.
 *     --------
 *      tcamNumEntriesInSmallestRuleSize - the number of rules that the tcam hold.
 *                          the 'units' are of 'rules of smallest size' that supported by the TCAM.
 *                          for example : sip5 : 10B rules
 *                                      xCat3 TTI/PCL : 24B rules
 *                          NOTE: this parameter allow BC2 device to be compatible
 *                                 with BOBK device with 1/2 the tcam size
 *  Comments:
 *
 */
typedef struct
{
    GT_U32                                                 vTcamMngId;
    GT_BOOL                                                disableRearrange;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP             devsBitmap;
    GT_U32                                                 numOfDevices;
    CPSS_PP_FAMILY_TYPE_ENT                                devFamily;
    /* segments related */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_CFG_STC          tcamSegCfg;
    GT_U32                                                 blocksAmount;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_BLOCK_INFO_STC  *blockInfArr;
    CPSS_BM_POOL_ID                                        segmentsPool;
    CPSS_BM_POOL_ID                                        segNodesPoolFreePref;
    PRV_CPSS_AVL_TREE_ID                                   segFreePrefTree;
    CPSS_BM_POOL_ID                                        segNodesPoolFreeLu;
    PRV_CPSS_AVL_TREE_ID                                   segFreeLuTree;
    CPSS_BM_POOL_ID                                        segNodesPool;
    /* rule Id table */
    CPSS_BM_POOL_ID                                        ruleIdPool;
    CPSS_BM_POOL_ID                                        ruleIdIdNodesPool;
    CPSS_BM_POOL_ID                                        ruleIdIndexNodesPool;
    /* priority table */
    CPSS_BM_POOL_ID                                        priorityPool;
    CPSS_BM_POOL_ID                                        priorityPriNodesPool;
    CPSS_BM_POOL_ID                                        priorityIndexNodesPool;
    /* vTCAMs related */
    GT_U32                                                 vTcamCfgPtrArrSize;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_PTR            *vTcamCfgPtrArr;

    /* tcam num rules info */
    GT_U32                                                  tcamNumEntriesInSmallestRuleSize;

    /* handles of dynamic created objects will be added */
} PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC;

/* typedef for pointer to structure */
typedef PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC
    *PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_PTR;

/* macro PRV_CPSS_VIRTUAL_TCAM_MNG_DEV_BMP_SET_MAC
    to set a device in bitmap of devices in the TCAM manager database

  devsBitmap - of type PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP
                device bitmap

  devNum - the device num to set in the device bitmap
*/
#define PRV_CPSS_VIRTUAL_TCAM_MNG_DEV_BMP_SET_MAC(devsBitmap, devNum)   \
    (devsBitmap)[(devNum)>>5] |= 1 << ((devNum) & 0x1f)

/* macro PRV_CPSS_VIRTUAL_TCAM_MNG_DEV_BMP_CLEAR_MAC
    to clear a device from the bitmap of devices in the TCAM manager database

  devsBitmap - of type PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEV_BITMAP
                device bitmap

  devNum - the device num to set in the device bitmap
*/
#define PRV_CPSS_VIRTUAL_TCAM_MNG_DEV_BMP_CLEAR_MAC(devsBitmap, devNum)   \
    (devsBitmap)[(devNum)>>5] &= ~(1 << ((devNum) & 0x1f))

/* macro PRV_CPSS_VIRTUAL_TCAM_MNG_IS_DEV_BMP_SET_MAC
    to check if device is set in the bitmap of devices in the TCAM manager database

  devsBitmap - of type PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEV_BITMAP
                device bitmap

  devNum - the device num to set in the device bitmap

  return 0 -- device not set in bmp
  return 1 -- device set in bmp
*/
#define PRV_CPSS_VIRTUAL_TCAM_MNG_IS_DEV_BMP_SET_MAC(devsBitmap, devNum)   \
    (((devsBitmap)[(devNum)>>5] & (1 << ((devNum) & 0x1f))) ? 1 : 0)

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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCropSegmentVertical
*
* DESCRIPTION:
*       Split segment to 3 vertical pairs: low, cropped, high.
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
*                         (NULL value - no low reminder)
*       highSegPtrPtr   - (pointer to) (pointer to) high remainder of segment
*                         (NULL parameter - caller not needs expects the value)
*                         (NULL value - no high reminder)
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorizontal
*
* DESCRIPTION:
*       Split segment to 2 horizontal pairs: cropped and remainer.
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
*                           (NULL value - no low reminder)
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCropSegmentFirstHorzBlock
*
* DESCRIPTION:
*       Split segment to 2 horizontal pairs:
*       cropped first not empty horizontal block and remainer.
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
*                           (NULL value - no low reminder)
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCropSegmentHorzBlockBitmap
*
* DESCRIPTION:
*       Split segment to 2 horizontal pairs:
*       cropped horizontal block bitmap and remainer.
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
*                           (NULL value - no low reminder)
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamCfgTableCreate
*
* DESCRIPTION:
*       Create vTcam Configuration Table for TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*       maxEntriesInDb    - maximal amount entries in DB.
*
* OUTPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*                           Allocation result stored to it.
*       .
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
GT_STATUS prvCpssDxChVirtualTcamDbVTcamCfgTableCreate
(
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC       *pVTcamMngPtr,
    IN     GT_U32                                   maxEntriesInDb
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamCfgTableDelete
*
* DESCRIPTION:
*       Delete all vTcams and vTcam Configuration Table for TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*
* OUTPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*                           Freed meory Pointer set to NULL.
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
GT_STATUS prvCpssDxChVirtualTcamDbVTcamCfgTableDelete
(
    INOUT   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC    *pVTcamMngPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamCfgTableVTcamAdd
*
* DESCRIPTION:
*       Add new vTcam.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*       tcamInfoPtr     - (pointer to) vTCAM Info
*
* OUTPUTS:
*       None.
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
GT_STATUS prvCpssDxChVirtualTcamDbVTcamCfgTableVTcamAdd
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                 *pVTcamMngPtr,
    IN  GT_U32                                              vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_INFO_STC                     *tcamInfoPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamCfgTableVTcamRemove
*
* DESCRIPTION:
*       Remove vTcam.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*
* OUTPUTS:
*       None.
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
GT_STATUS prvCpssDxChVirtualTcamDbVTcamCfgTableVTcamRemove
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *pVTcamMngPtr,
    IN  GT_U32                                              vTcamId
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamCfgTableVTcamGet
*
* DESCRIPTION:
*       Get vTcam Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamInfoPtr     - (pointer to) vTCAM Configuration
*
* OUTPUTS:
*       tcamCfgPtrPtr   - (pointer to)(pointer to) vTCAM Configuration
*
*       None.
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
GT_STATUS prvCpssDxChVirtualTcamDbVTcamCfgTableVTcamGet
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *pVTcamMngPtr,
    IN   GT_U32                                              vTcamId,
    OUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC         **tcamCfgPtrPtr
);

/**********************************************************************************/
/* segments Table (the alone feature for physical layout)                      */
/**********************************************************************************/

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableCreate
*
* DESCRIPTION:
*       Create Segment Table for TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableDelete
*
* DESCRIPTION:
*       Delete Segment Table for TCAM. (Free used memory)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

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
);

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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbAllocWithOnlySegmentTable
*
* DESCRIPTION:
*       allocate vtcam with segments tree
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngPtr  - (pointer to) the TCAM manager
*       vTcamId      - the vtcam ID to allocate for
*       tcamInfoPtr  - (pointer to) the vtcam info
*       allocSkeletonOnly   - indication to allocate only skeleton
* OUTPUTS:
*       vTcamMngPtr  - (pointer to) the TCAM manager , with the added vtcam
*                       allocation at index vTcamId.
*
* RETURNS :
*       pointer to allocated vtcam memory (on NULL)
*
* COMMENTS:
*
*******************************************************************************/
PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* prvCpssDxChVirtualTcamDbAllocWithOnlySegmentTable
(
    INOUT PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC      *vTcamMngPtr,
    IN GT_U32                                   vTcamId,
    IN CPSS_DXCH_VIRTUAL_TCAM_INFO_STC         *tcamInfoPtr,
    IN    GT_BOOL                               allocSkeletonOnly
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamAllocate
*
* DESCRIPTION:
*       Allocate memory for Virtual TCAM in Segment Table for TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*       allocRequestPtr - (pointer to) allocation request structure
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
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC   *allocRequestPtr
);

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
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
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
);

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
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

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
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
    IN   GT_BOOL                                        fromEndOrStart,
    IN     GT_U32                                       numOfRulesToKeep
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamSizeGet
*
* DESCRIPTION:
*       Get Size of Virtual TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*
* OUTPUTS:
*       sizePtr             - (pointer to) size of vTCAM in rules
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
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamSizeGet
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC             *pVTcamMngPtr,
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC    *tcamCfgPtr,
    OUT GT_U32                                         *sizePtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableVTcamResize
*
* DESCRIPTION:
*       Resise Virtual TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*       toInsert                - GT_TRUE - insert rules, GT_FALSE - delete rules
*       logicalIndex            - logical index of rule.
*       baseRuleLogicalIndex    - logical index of base rule.
*
* OUTPUTS:
*       None.
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
GT_STATUS prvCpssDxChVirtualTcamDbSegmentTableVTcamResize
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC             *pVTcamMngPtr,
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC    *tcamCfgPtr,
    IN  GT_BOOL                                        toInsert,
    IN  GT_U32                                         baseRuleLogicalIndex,
    IN  GT_U32                                         rulesAmount
);

/**********************************************************************************/
/* Rule Id Table                                                                  */
/**********************************************************************************/

/*
 * Typedef: struct PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC
 *
 * Description:
 *     This structure of DB Rule Id Entry
 *
 * Fields:
 *       ruleId               - rule id,
 *       logicalIndex         - logical Index.
 *       applicationDataPtr   - application data pointer.
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32      ruleId;
    GT_U32      logicalIndex;
    GT_VOID     *applicationDataPtr;
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC;

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableCreate
*
* DESCRIPTION:
*       Create Rule Id Table for TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableDelete
*
* DESCRIPTION:
*       Delete Rule Id Table for TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesCreate
*
* DESCRIPTION:
*       Create Local per-vTcam Access Trees for Rule Id Table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesDelete
*
* DESCRIPTION:
*       Delete Local per-vTcam Access Trees for Rule Id Table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableVTcamResizeCommit
*
* DESCRIPTION:
*       Commit Resise of Virtual TCAM.
*       Rules supposed aready moved - just update DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       toInsert                - GT_TRUE - insert rules, GT_FALSE - delete rules
*       logicalIndex            - logical index of rule.
*       baseRuleLogicalIndex    - logical index of base rule.
*
* OUTPUTS:
*       None.
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
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableVTcamResizeCommit
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC             *pVTcamMngPtr,
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC    *tcamCfgPtr,
    IN  GT_BOOL                                        toInsert,
    IN  GT_U32                                         baseRuleLogicalIndex,
    IN  GT_U32                                         rulesAmount
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryAdd
*
* DESCRIPTION:
*       Add rule Id entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*       entryPtr        - (pointer to) rule Id entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_STATE             - on ruleId or/and logicalIndex already used
*       GT_FAIL                  - on error
*
* COMMENTS:
*       treePathArr               - (pointer to) tree path array.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryAdd
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC               *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC      *tcamCfgPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC  *entryPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryGet
*
* DESCRIPTION:
*       Get rule Id entry by rule Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       ruleId                  - ruleId to find.
*
* OUTPUTS:
*       entryPtr                - (pointer to) find rule entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - the logical index was not found.
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryGet
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC       *tcamCfgPtr,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                     ruleId,
    OUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC   *entryPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryFind
*
* DESCRIPTION:
*       Find entry Id rule by logical index.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       logicalIndex            - logical index to search.
*       findType                - find type: see enum.
*
* OUTPUTS:
*       treePathArr             - (pointer to) tree path array.
*       entryPtr                - (pointer to) find rule entry.
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
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryFind
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC       *tcamCfgPtr,
    IN   GT_U32                                            logicalIndex,
    IN   PRV_CPSS_AVL_TREE_FIND_ENT                        findType,
    OUT  PRV_CPSS_AVL_TREE_PATH                            treePathArr,
    OUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC   *entryPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntrySeek
*
* DESCRIPTION:
*       Seek rule entry by logical Id order.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       seekType                - seek type: see enum.
*       treePathArr             - (pointer to) tree path array.
*
* OUTPUTS:
*       treePathArr             - (pointer to) tree path array.
*       entryPtr                - (pointer to) find rule entry.
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
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntrySeek
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC       *tcamCfgPtr,
    IN     PRV_CPSS_AVL_TREE_SEEK_ENT                        seekType,
    INOUT  PRV_CPSS_AVL_TREE_PATH                            treePathArr,
    OUT    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC   *entryPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryRemove
*
* DESCRIPTION:
*       Remove rule Id entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       ruleId                  - ruleId to remove.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_STATE             - on ruleId or/and logicalIndex already used
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryRemove
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC       *tcamCfgPtr,
    IN   GT_U32                                             ruleId
);

/**********************************************************************************/
/* Rule Id Table rule ranges utilities                                                                 */
/**********************************************************************************/

/*
 * Typedef: function PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_RULE_INVALIDATE_FUNC
 *
 * Description:
 *     This Function invalidates rule in TCAM.
 *
 * INPUTS:
 *       cookiePtr            - device info passed to callback function.
 *       ruleLogicalIndex     - rule logial index.
 *       ruleSize             - size of rules used in virtual TCAM.
 *
 *  Comments:
 *
 */
typedef GT_STATUS PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_RULE_INVALIDATE_FUNC
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      ruleLogicalIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT      ruleSize
);

/*
 * Typedef: function PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_RULE_MOVE_FUNC
 *
 * Description:
 *     This Function move/copy rule in TCAM.
 *
 * INPUTS:
 *       cookiePtr               - device info passed to callback function.
 *       srcRuleLogicalIndex     - source rule logical index.
 *       dstRuleLogicalIndex     - destination rule logical index.
 *       ruleSize                - size of rules used in virtual TCAM.
 *       moveOrCopy              - indication that we move or Copy
 *                                  GT_TRUE - move.
 *                                  GT_FALSE - copy.
 *  Comments:
 *
 */
typedef GT_STATUS PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_RULE_MOVE_FUNC
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      srcRuleLogicalIndex,
    IN  GT_U32      dstRuleLogicalIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT      ruleSize,
    IN  GT_BOOL     moveOrCopy
);

/*
 * Typedef: struct PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_HW_CONTEXT_STC
 *
 * Description:
 *     This structure of DB Rule Id table HW context
 *
 * Fields:
 *       cookiePtr              - (pointer to)device info to be passed to callbacks
 *       ruleInvalidateFuncPtr  - (pointer to)callback function for HW rule invalidate
 *       ruleMoveFuncPtr        - (pointer to)callback function for HW rule move
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_VOID                                                        *cookiePtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_RULE_INVALIDATE_FUNC *ruleInvalidateFuncPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_RULE_MOVE_FUNC       *ruleMoveFuncPtr;
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_HW_CONTEXT_STC;

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryRangeGetRulesAmount
*
* DESCRIPTION:
*       Gets amount of rules in ginen range,.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       baseLogicalIndex        - base Logical Index of the range.
*       rangeSize               - size of the range in Logical Indexes
*
* OUTPUTS:
*       rulesAmountPtr       - (pointer to) amount of rules in the range
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryRangeGetRulesAmount
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC       *tcamCfgPtr,
    IN   GT_U32                                            baseLogicalIndex,
    IN   GT_U32                                            rangeSize,
    OUT  GT_U32                                            *rulesAmountPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryRangeDrag
*
* DESCRIPTION:
*       Move range of rules to a new range keeping logical index order of rules in all vTcam.
*       Used only to reorganize free space for future using.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       hwContextPtr            - (pointer to) HW Context
*       srcLogicalIndex         - base Logical Index of the source range.
*       srcRangeSize            - size of source range in Logical Indexes
*       dstLogicalIndex         - base Logical Index of the destination range.
*       dstRangeSize            - size of destination range in Logical Indexes
*       nearHighBound           - GT_TRUE - gather moved records near High Bound of new range
*                                 GT_FALSE - gather moved records near Low Bound of new range
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_STATE             - on ruleId or/and logicalIndex already used
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryRangeDrag
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                        *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC               *tcamCfgPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_HW_CONTEXT_STC  *hwContextPtr,
    IN   GT_U32                                                    srcLogicalIndex,
    IN   GT_U32                                                    srcRangeSize,
    IN   GT_U32                                                    dstLogicalIndex,
    IN   GT_U32                                                    dstRangeSize,
    IN   GT_BOOL                                                   nearHighBound
);

/**********************************************************************************/
/* Priority Table                                                                 */
/**********************************************************************************/
/*
 * Typedef: struct PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC
 *
 * Description:
 *     This structure of DB priority Entry
 *
 * Fields:
 *       priority             - priority.
 *       baseLogIndex    - base Logical Index of range
 *       rangeSize       - size of range, can be 0
 *                        (to reserve space for yet not created ranges)
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32      priority;
    GT_U32      baseLogIndex;
    GT_U32      rangeSize;
} PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC;

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableCreate
*
* DESCRIPTION:
*       Create Priority Table for TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableDelete
*
* DESCRIPTION:
*       Delete Priority Table for TCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesCreate
*
* DESCRIPTION:
*       Create Local per-vTcam Access Trees for Priority Table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete
*
* DESCRIPTION:
*       Delete Local per-vTcam Access Trees for Priority Table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableEntrySet
*
* DESCRIPTION:
*       Add or Update Logical Index Range to Priority Table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*       entryPtr        - (pointer to) entry with info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       1. The given range should not overlap any existing range.
*       2. if priority0 >= priority1 then baseLogIndex0 >= baseLogIndex1
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableEntrySet
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC           *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *tcamCfgPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC   *entryPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableEntryGet
*
* DESCRIPTION:
*       Get Logical Index Range from Priority Table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*       priority        - rules priority.
*
* OUTPUTS:
*       entryPtr        - (pointer to) entry with info
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       For not existing range returned *rangeSizePtr == 0 and
*       *baseLogIndexPtr points to the end of the last range
*       that should be before the required range, i.e. the range with
*       minimal priority greater the given (if not exist - *baseLogIndexPtr == 0)
*       This value can be used as base of this not existing range adding it.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableEntryGet
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *tcamCfgPtr,
    IN   GT_U32                                       priority,
    OUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC   *entryPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableEntryFind
*
* DESCRIPTION:
*       Find entry Id rule by logical index.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*       tcamCfgPtr        - (pointer to)virtual TCAM structure
*       logicalIndex      - logical index to search.
*       findType          - find type: see enum.
*
* OUTPUTS:
*       treePathArr       - (pointer to) tree path array.
*       entryPtr          - (pointer to) entry with info
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_FOUND      - entry not found
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableEntryFind
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC            *tcamCfgPtr,
    IN   GT_U32                                                  logicalIndex,
    IN   PRV_CPSS_AVL_TREE_FIND_ENT                              findType,
    OUT  PRV_CPSS_AVL_TREE_PATH                                  treePathArr,
    OUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC       *entryPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableEntrySeek
*
* DESCRIPTION:
*       Seek rule entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr        - (pointer to) VTCAM Manager structure
*       tcamCfgPtr          - (pointer to)virtual TCAM structure
*       seekType            - seek type: see enum.
*       treePathArr         - (pointer to) tree path array.
*
* OUTPUTS:
*       treePathArr         - (pointer to) tree path array.
*       entryPtr          - (pointer to) entry with info
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
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableEntrySeek
(
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC               *pVTcamMngPtr,
    IN     PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC      *tcamCfgPtr,
    IN     PRV_CPSS_AVL_TREE_SEEK_ENT                       seekType,
    INOUT  PRV_CPSS_AVL_TREE_PATH                           treePathArr,
    OUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC       *entryPtr
);

/**********************************************************************************/
/* Utility                                                                */
/**********************************************************************************/

/*******************************************************************************
* prvCpssDxChVirtualTcamDbUtilNewRuleLogIndexGet
*
* DESCRIPTION:
*       Get place for new rule.
*       If needed move existing rules.
*       If needed resize vTCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId              - vTCAM Manager Id
*                                 (APPLICABLE RANGES: 0..31)
*       vTcamId                 - virtual TCAM Id
*                                 (APPLICABLE RANGES: 0..0xFFFF)
*       origIndexForPriorityUpdate - the original index of the entry that doing
*                               'priority update'
*                               value ORIG_INDEX_NOT_USED_CNS meaning that it is
*                               not relevant.
*       vTcamMaxSize            - maximal size of vTCAM. (no resize to greater).
*                                 The current size retrieved from Segment Table.
*       hwContextPtr            - (pointer to)HW context for moving existing rules.
*       priority                - new rule priority
*       position                - position in range of rule of the
*                                 specified priority (see enum)
*       sizeIncrement           - amount of rules to be added if resize will needed.
*       recalled                - recalled due to autoresize
*
* OUTPUTS:
*       logIndexPtr         - (pointer to) Logical Index of new rule
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*       GT_FULL                  - on table full (no free logical index)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbUtilNewRuleLogIndexGet
(
    IN   GT_U32                                                    vTcamMngId,
    IN   GT_U32                                                    vTcamId,
    IN   GT_U32                                                    origIndexForPriorityUpdate ,
    IN   GT_U32                                                    vTcamMaxSize,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_HW_CONTEXT_STC  *hwContextPtr,
    IN   GT_U32                                                    priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position,
    IN   GT_U32                                                    sizeIncrement,
    IN   GT_BOOL                                                   recalled,
    OUT  GT_U32                                                    *logIndexPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbUtilVTcamReduce
*
* DESCRIPTION:
*       Compress vTCAM and free part of it.
*       If needed move existing rules.
*       Resize vTCAM.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr     - (pointer to) VTCAM Manager structure
*       tcamCfgPtr       - (pointer to)virtual TCAM structure
*       hwContextPtr     - (pointer to)HW context for moving existing rules.
*       priority         - priority used to seek the prefered place for removing
*                          free space.
*       sizeDecrement    - amount of rules free.
*
* OUTPUTS:
*       None.
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
GT_STATUS prvCpssDxChVirtualTcamDbUtilVTcamReduce
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                        *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC               *tcamCfgPtr,
    IN   GT_U32                                                    vTcamMaxSize,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_HW_CONTEXT_STC  *hwContextPtr,
    IN   GT_U32                                                    priority,
    IN   GT_U32                                                    sizeDecrement
);


/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamMngGet
*
* DESCRIPTION:
*       Get DB info about a vTcam manager.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the DB of the vTcam.
*       NULL on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC* prvCpssDxChVirtualTcamDbVTcamMngGet
(
    IN  GT_U32                                      vTcamMngId
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamGet
*
* DESCRIPTION:
*       Get DB info about a vTcam in vTcam manager.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the DB of the vTcam.
*       NULL on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* prvCpssDxChVirtualTcamDbVTcamGet
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst
*
* DESCRIPTION:
*       The function returns the first devNum that is bound to the tcam manager.
*       it must be called before any iteration on the device list of the vTcam.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       devNumPtr       - (pointer to) hold 'first' devNum .
*
* RETURNS:
*       GT_OK       - iterator initialization OK
*       GT_NO_MORE  - no devices attached to the vTcamMngId
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst
(
    IN  GT_U32  vTcamMngId,
    OUT GT_U8   *devNumPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbDeviceIteratorGetNext
*
* DESCRIPTION:
*       The function returns the next devNum that is bound to the tcam manager.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       devNumPtr       - (pointer to) hold 'current' devNum .
*
* OUTPUTS:
*       devNumPtr       - (pointer to) hold 'next' devNum .
*
* RETURNS:
*       0       - no more devices. ('current' device is the last one)
*       1       - devNumPtr hold 'next' devNum
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_U32  prvCpssDxChVirtualTcamDbDeviceIteratorGetNext
(
    IN  GT_U32  vTcamMngId,
    INOUT GT_U8   *devNumPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbManagerDump
*
* DESCRIPTION:
*       Dump tcam info without ‘vtcam info’
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssDxChVirtualTcamDbManagerDump
(
    IN  GT_U32                                      vTcamMngId
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamDump
*
* DESCRIPTION:
*       Dump DB info about a vTcam in vTcam manager.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssDxChVirtualTcamDbVTcamDump
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamDumpBlockFreeSpace
*
* DESCRIPTION:
*       Dump block free space.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssDxChVirtualTcamDbVTcamDumpBlockFreeSpace
(
    IN  GT_U32                                      vTcamMngId
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamDumpByField
*
* DESCRIPTION:
*       Dump DB info about a vTcam in vTcam manager of specific field.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       fieldNamePtr      - name of the required field
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssDxChVirtualTcamDbVTcamDumpByField
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  GT_CHAR_PTR                                 fieldNamePtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamDumpFullTcamLayout
*
* DESCRIPTION:
*       Dump DB info about full TCAM layout.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssDxChVirtualTcamDbVTcamDumpFullTcamLayout
(
    IN  GT_U32                                      vTcamMngId
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamRulesLayoutVerify
*
* DESCRIPTION:
*       Verify DB info about Virtual TCAM Rules layout.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId          - Virtual TCAM Manager Id
*                             (APPLICABLE RANGES: 0..31)
*       vTcamId             - unique Id of  Virtual TCAM
*       stopAfterFirstError - GT_TRUE  - return after first error
*                           - GT_FALSE - continue checking
*       printErrorMessage   - GT_TRUE  - print error messages
*                             GT_FALSE - count errors only
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - no errors found,
*       other               - the code of the last error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbVTcamRulesLayoutVerify
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  GT_BOOL                                     stopAfterFirstError,
    IN  GT_BOOL                                     printErrorMessage
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVerifyFullTcamLayout
*
* DESCRIPTION:
*       Verify DB info about full TCAM layout.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId          - Virtual TCAM Manager Id
*       stopAfterFirstError - GT_TRUE  - return after first error
*                           - GT_FALSE - continue checking
*       printErrorMessage   - GT_TRUE  - print error messages
*                             GT_FALSE - count errors only
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - no errors found,
*       other               - the code of the last error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbVerifyFullTcamLayout
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_BOOL                                     stopAfterFirstError,
    IN  GT_BOOL                                     printErrorMessage
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree
*
* DESCRIPTION:
*       print the avl segments tree..
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       treeIdPtr      - a pointer to the id of the segments tree
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssDxChVirtualTcamDbVTcamDumpSegmentrTree
(
    IN PRV_CPSS_AVL_TREE_ID treeIdPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVerifyAllTrees
*
* DESCRIPTION:
*       Verify trees in DB - matching data to compaire functions.
*       Should detect corruption caused by data updated without removing and inserting
*       related tree items.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId          - Virtual TCAM Manager Id
*       stopAfterFirstError - GT_TRUE  - return after first error
*                           - GT_FALSE - continue checking
*       printErrorMessage   - GT_TRUE  - print error messages
*                             GT_FALSE - count errors only
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - no errors found,
*       other               - the code of the last error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbVerifyAllTrees
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_BOOL                                     stopAfterFirstError,
    IN  GT_BOOL                                     printErrorMessage
);

/*******************************************************************************
* prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFunc
*
* DESCRIPTION:
*       AVL Tree compare function for segments in lookup order.
*       Used only internal.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
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
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChVirtualTcamDb_h */

