/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChVlan.c
*
* DESCRIPTION:
*       A lua wrapper for vlan functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

/* max number of MRU indexes */
#define WRL_PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS     (7)

#ifndef __wraplCpssDxChVlan__
#define __wraplCpssDxChVlan__

#include <cpssCommon/wrapCpssDebugInfo.h>


/*******************************************************************************
* prvCpssDxChDoesVlanExist
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Check's that vlan exists.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum                           - device number
*        vlanId                           - vlan id
*       P_CALLING_FORMAL_DATA             - general debug information 
*                                           (environment variables etc); could 
*                                           be empty
* 
* OUTPUTS:
*        doesVlanExistPtr                 - vlan validness property
*        errorMessagePtr                  - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDoesVlanExist
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    OUT GT_BOOL                             *doesVlanExistPtr,
    OUT GT_CHAR_PTR                         *errorMessagePtrPtr     
    P_CALLING_FORMAL_DATA
);

#endif /* __wraplCpssDxChVlan__ */
