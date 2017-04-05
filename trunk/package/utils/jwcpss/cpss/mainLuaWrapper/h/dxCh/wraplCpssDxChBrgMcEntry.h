/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChBrgFdb.c
*
* DESCRIPTION:
*       A lua wrapper for bridge multicast entries.
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/


#include <cpss/generic/cpssTypes.h>


/*******************************************************************************
* prvCpssDxChDoesMulticastGroupIndexExist
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Check's that trunk exists.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum                - device number
*       vidx                  - multicast group index
*
* OUTPUTS:
*       doesMulticastGroupIndexExist         
*                             - multicast group index existence
*       errorMessagePtr       - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDoesMulticastGroupIndexExist
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vidx,
    OUT GT_BOOL                 *doesMulticastGroupIndexExist, 
    OUT GT_CHAR_PTR             *errorMessagePtr
);

