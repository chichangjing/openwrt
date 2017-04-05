/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChTrunk.h
*
* DESCRIPTION:
*       A lua wrapper for trunk functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __wraplCpssDxChTrunk__
#define __wraplCpssDxChTrunk__

#include <cpssCommon/wrapCpssDebugInfo.h>


#include <cpss/generic/cpssTypes.h>


/*******************************************************************************
* prvCpssDxChDoesTrunkExist
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
*       trunkId               - trunk id
*       P_CALLING_FORMAL_DATA - general debug information (environment variables 
*                               etc); could be empty
*
* OUTPUTS:
*       doesVlanExist         - vlan existen
*       errorMessagePtr       - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDoesTrunkExist
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT GT_BOOL                 *doesVlanExist, 
    OUT GT_CHAR_PTR             *errorMessagePtr
    P_CALLING_FORMAL_DATA
);


/*******************************************************************************
* prvCpssDxChIsDevicePortTrunkMember
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking that dev/port pair is member of passed trunk.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       trunkId             - trunk if
*       hwDevNum            - hardware device number
*       hwPortNum           - hardware port number
*       P_CALLING_FORMAL_DATA - general debug information (environment variables 
*                               etc); could be empty
* 
* OUTPUTS:
*       isMemberInTrunkPtr - property that dev/port pair are members of trunk
*       errorMessagePtr    - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       operation execution staus
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIsDevicePortTrunkMember
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  GT_PHYSICAL_PORT_NUM    hwDevNum,
    IN  GT_HW_DEV_NUM           hwPortNum,    
    OUT GT_BOOL                 *isMemberInTrunkPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr    
    P_CALLING_FORMAL_DATA
);

#endif /* __wraplCpssDxChTrunk__ */
