/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChHsu.h
*
* DESCRIPTION:
*       CPSS DxCh HSU facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#ifndef __cpssDxChHsuh
#define __cpssDxChHsuh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/* user want to make import/export by one iteration */
#define CPSS_DXCH_HSU_SINGLE_ITERATION_CNS 0xFFFFFFFF


/*
 * typedef: enum CPSS_DXCH_HSU_DATA_TYPE_ENT
 *
 * Description:
 *      Types of cpss data for export/import/size_calculations actions during
 *      HSU process. It could be cpss internal data  structures, shadow tables,
 *      global variables, data per feature/mechanism.
 *
 * Enumerations:
 *
 *    CPSS_DXCH_HSU_DATA_TYPE_LPM_DB_E          - LPM DB HSU data type
 *    CPSS_DXCH_HSU_DATA_TYPE_GLOBAL_E          - Global HSU data type
 *    CPSS_DXCH_HSU_DATA_TYPE_ALL_E             - ALL HSU data type
 */

typedef enum
{
    CPSS_DXCH_HSU_DATA_TYPE_LPM_DB_E,
    CPSS_DXCH_HSU_DATA_TYPE_GLOBAL_E,
    CPSS_DXCH_HSU_DATA_TYPE_ALL_E
} CPSS_DXCH_HSU_DATA_TYPE_ENT;

/*
 * Typedef: struct CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC
 *
 * Description: HSU data block header
 *
 * Fields:
 *      type            - type of HSU data block
 *      length          - length of stored data
 *      version         - stored data format version
 *      reserved1       - reserved for future needs
 *      reserved2       - reserved for future needs
 *
 */
typedef struct CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STCT
{
    GT_U32                                  type;
    GT_U32                                  length;
    GT_U32                                  version;
    GT_U32                                  reserved1;
    GT_U32                                  reserved2;
}CPSS_DXCH_HSU_DATA_BLOCK_HEADER_STC;


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
);

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
    INOUT  GT_UINTPTR                    *iteratorPtr,
    INOUT  GT_U32                        *hsuBlockMemSizePtr,
    IN     GT_U8                         *hsuBlockMemPtr,
    OUT    GT_BOOL                       *exportCompletePtr
);

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
    INOUT  GT_UINTPTR                    *iteratorPtr,
    INOUT  GT_U32                        *hsuBlockMemSizePtr,
    IN     GT_U8                         *hsuBlockMemPtr,
    OUT    GT_BOOL                       *importCompletePtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChHsuh */
