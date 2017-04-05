/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmNodesTree.h
*
* DESCRIPTION:
*       TM Tree APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTmNodesTreeh
#define __cpssTmNodesTreeh

#include <cpss/generic/cpssTypes.h>

/*******************************************************************************
* cpssTmTreeChangeStatus
*
* DESCRIPTION:
*       Change the tree DeQ status. 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       status          - Tree status.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmTreeChangeStatus
(
    IN GT_U8    devNum,
    IN GT_BOOL  status
);



/*******************************************************************************
* cpssTmTreeStatusGet
*
* DESCRIPTION:
*       Retrieve tree DeQ status. 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
* OUTPUTS:
*       statusPtr       -(pointer to)  Tree status.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_PTR               - on NULL statusPtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmTreeStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *statusPtr
);


/*******************************************************************************
* cpssTmTreeDwrrPrioSet
*
* DESCRIPTION:
*       Change the tree DWRR priority. 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       priosPtr        - Priority array pointer.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Set priosPtr[i] = GT_FALSE/GT_TRUE, if DWRR for prio i is disabled/enabled.
*******************************************************************************/
GT_STATUS cpssTmTreeDwrrPrioSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  *priosPtr
);

/*******************************************************************************
* cpssTmTreeDwrrPrioGet
*
* DESCRIPTION:
*       Retrieve tree DWRR priority. 
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
* OUTPUTS:
*       priosPtr        - Status per priority array pointer.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PTR               - on NULL priosPtr.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Set priosPtr[i] = GT_FALSE/GT_TRUE, if DWRR for prio i is disabled/enabled.
*******************************************************************************/
GT_STATUS cpssTmTreeDwrrPrioGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *priosPtr
);

#endif 	    /* __cpssTmNodesTreeh */

