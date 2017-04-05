/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenHsu.h
*
* DESCRIPTION:
*       Includes generic HSU definitions.
*
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

#ifndef __prvCpssGenHsuh
#define __prvCpssGenHsuh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/systemRecovery/hsu/cpssGenHsu.h>

/*
 * Typedef: struct PRV_CPSS_HSU_DATA_BLOCK_HEADER_STC
 *
 * Description: HSU data block header
 *
 * Fields:
 *      type            - type of HSU data block
 *      length          - length of stored data, including this header (in bytes)
 *      version         - stored data format version
 *      reserved1       - reserved for future needs
 *      reserved2       - reserved for future needs
 *
 */
typedef struct
{
    CPSS_HSU_DATA_TYPE_ENT      type;
    GT_U32                      length;
    GT_U32                      version;
    GT_U32                      reserved1;
    GT_U32                      reserved2;

} PRV_CPSS_HSU_DATA_BLOCK_HEADER_STC;


/*
 * Typedef: enum PRV_CPSS_HSU_ACTION_TYPE_ENT
 *
 * Description: It describes import/export action.
 *
 * values:
 *      PRV_CPSS_HSU_EXPORT_E                 - export action
 *      PRV_CPSS_HSU_IMPORT_E                 - import action
 */
typedef enum
{
    PRV_CPSS_HSU_EXPORT_E,
    PRV_CPSS_HSU_IMPORT_E
}PRV_CPSS_HSU_ACTION_TYPE_ENT;


 /*
 * Typedef: enum PRV_CPSS_GEN_HSU_DATA_STAGE_ENT
 *
 * Description: It represents an global shadow iterator stage
 *
 * values:
 *      PRV_CPSS_GEN_HSU_CPSS_STAGE_E                - global cpss generic stage
 *      PRV_CPSS_GEN_HSU_DRV_STAGE_E                 - global driver stage
 *      PRV_CPSS_GEN_HSU_LAST_STAGE_E                - global last stage
 */
typedef enum
{
    PRV_CPSS_GEN_HSU_CPSS_STAGE_E,
    PRV_CPSS_GEN_HSU_DRV_STAGE_E,
    PRV_CPSS_GEN_HSU_LAST_STAGE_E
}PRV_CPSS_GEN_HSU_DATA_STAGE_ENT;

/*
 * Typedef: struct PRV_CPSS_HSU_GEN_ITERATOR_STC
 *
 * Description: This struct that holds HSU generic shadow iterator
 *
 * Fields:
 *      magic                      - iterator magic number
 *      currStage                  - the current generic stage
 *      currStageMemPtr            - the current memory address we're working on
 *      currentStageRemainedSize   - memory size remained till the end of stage.
 *      currentStageHsuBlockMemPtr - HSU memory address
 */
typedef struct
{
    GT_U32                                    magic;
    PRV_CPSS_GEN_HSU_DATA_STAGE_ENT           currStage;
    GT_U8                                    *currStageMemPtr;
    GT_U32                                    currentStageRemainedSize;
    GT_U8                                    *currentStageHsuBlockMemPtr;
}PRV_CPSS_HSU_GEN_ITERATOR_STC;



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
*       ExMx.
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
);


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
*       ExMx.
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
);

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
*       ExMx.
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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenHsuh */

