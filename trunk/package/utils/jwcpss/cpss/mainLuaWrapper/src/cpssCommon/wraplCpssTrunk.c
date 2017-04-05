/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssTrunk.c
*
* DESCRIPTION:
*       A lua wrapper for trunk functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/


#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpssCommon/wraplCpssTrunk.h>


#include <lua.h>


/*******************************************************************************
* prvCpssIsDevicePortBetweenTrunkMembers
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking that dev/port pair is between passed trunk members.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hwDevNum            - HW device number
*       portNum             - port number
*       numberOfTrunkMembers
*                           - number of passed trunk members
*       membersArray        - passed trunk members
* 
* OUTPUTS:
*       isMemberInTrunkPtr  - property that dev/port pair are members of trunk
*       errorMessagePtr     - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       operation execution staus
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssIsDevicePortBetweenTrunkMembers
(
    IN  GT_HW_DEV_NUM           hwDevNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  numberOfTrunkMembers,
    IN  CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS],
    OUT GT_BOOL                 *isMemberInTrunkPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{
    GT_U32                      trunkMemberIndex;
    GT_STATUS                   status              = GT_OK;    
    
    *isMemberInTrunkPtr = GT_FALSE;
    *errorMessagePtr    = NULL;
    
    for (trunkMemberIndex = 0; 
         trunkMemberIndex < numberOfTrunkMembers; 
         trunkMemberIndex++)
    {
        if ((hwDevNum == membersArray[trunkMemberIndex].hwDevice) &&
            (portNum == membersArray[trunkMemberIndex].port))
        {
            *isMemberInTrunkPtr = GT_TRUE;
            break;
        }
    }
    
    return status;    
}

