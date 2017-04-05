/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCfg.h
*
* DESCRIPTION:
*       private DxCh initialization PPs functions.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __prvCpssDxChCfgh
#define __prvCpssDxChCfgh

#include <cpss/generic/cpssTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvCpssDxChCfgAddrUpInit
*
* DESCRIPTION:
*       This function initializes the Core Address Update mechanism, by
*       allocating the AU descs. block.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to init the Au unit for.
*       auDescBlockPtr  - (pointer to)A block to allocate the Au descriptors from.
*       auDescBlockSize - Size in bytes of Au descBlock.
*       fuDescBlockPtr  - (pointer to)A block to allocate the Fu descriptors from.
*                         Valid if useFuQueue is GT_TRUE.
*       fuDescBlockSize - Size in bytes of Fu descBlock.
*                         Valid if useFuQueue is GT_TRUE.
*       useFuQueue      - GT_TRUE  - Fu queue is used - for DxCh2 devices only.
*                       - GT_FALSE - Fu queue is unused.
*       useDoubleAuq    - Support configuration of two AUQ memory regions.
*                         GT_TRUE - CPSS manages two AU Queues with the same size:
*                         auqCfg->auDescBlockSize / 2.
*                         GT_FALSE - CPSS manages single AU Queue with size:
*                         auqCfg->auDescBlockSize.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChCfgAddrUpInit
(
    IN GT_U8    devNum,
    IN GT_U8    *auDescBlockPtr,
    IN GT_U32   auDescBlockSize,
    IN GT_U8    *fuDescBlockPtr,
    IN GT_U32   fuDescBlockSize,
    IN GT_BOOL  useFuQueue,
    IN GT_BOOL  useDoubleAuq
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __prvCpssDxChCfgh */
