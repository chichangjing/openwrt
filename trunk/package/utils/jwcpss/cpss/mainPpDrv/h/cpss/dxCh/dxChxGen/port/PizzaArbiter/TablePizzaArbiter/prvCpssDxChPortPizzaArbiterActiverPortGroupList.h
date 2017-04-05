#ifndef __PRV_CPSS_DXCH_PORT_PIZZA_ARBITER_PORT_GROUP_LIST_H
#define __PRV_CPSS_DXCH_PORT_PIZZA_ARBITER_PORT_GROUP_LIST_H

/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiterActivePortGroupList.h
*
* DESCRIPTION:
*       port greoup list
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#define PRV_CPSS_BAD_PORTGROUP_CNS              0xFF
#define PRV_CPSS_MAX_PORT_GROUP_NUM_CNS        16

/*******************************************************************************
* PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC 
*
* DESCRIPTION:
*       list of active port group on PP of fixed length(up to max portgroup on PP 
*
* FIELDS:
*       maxSize      - max size of list
*       size         - actual size of list
*       list         - list itsels
*
* 		interfaces:
* 			ActivePortGroupListInit() -- init the list
*           		         Append() -- Append PortGroup to list
*                            Search() -- search the portGroup in the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pPortGroupList    - pointer to PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC structure
*       portGroupId       - port group id (core Id)
*
* OUTPUTS:
*       Search()
*       	pIsFoundPtr   - GT_TRUE if port group is in list 
*                         - GT_FALSE othewise
* RETURNS:
*       Append()
*       	GT_OUT_OF_RANGE : if there is no place to store port group id
*           GT_OK.            
*       Search()
*           GT_OK.            any case 
* COMMENTS:
*
*******************************************************************************/
typedef struct PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LISTt_STCT
{
	GT_U32 size;
	GT_U32 maxSize;
	GT_U32 list[PRV_CPSS_MAX_PORT_GROUP_NUM_CNS];
}PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC;


/*******************************************************************************
* ActivePortGroupListInit
*
* DESCRIPTION:
*      The function inits PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC structure to be empty 
*      list
*
* APPLICABLE DEVICES:
*       none
*
* NOT APPLICABLE DEVICES:
*       none
*
* INPUTS:
*       portGroupListPtr - structure to be initialized
*
* OUTPUTS:
*       portGroupListPtr - structure to be initialized
*
* RETURNS:
*       GT_BAD_PTR - if pPortGroupList is NULL
*       GT_OK      -    
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS ActivePortGroupListInit  
(
    INOUT PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *portGroupListPtr
);


/*******************************************************************************
* ActivePortGroupListInit
*
* DESCRIPTION:
*      The function inits PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC structure to be empty 
*      list
*
* APPLICABLE DEVICES:
*       none
*
* NOT APPLICABLE DEVICES:
*       none
*
* INPUTS:
*       portGroupListPtr     - structure to be initialized
*       portGroupListFromPtr - list to copy from 
*
* OUTPUTS:
*       portGroupListPtr - structure to be initialized
*
* RETURNS:
*       GT_BAD_PTR - if pPortGroupList is NULL
*       GT_OK           
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS ActivePortGroupListCopy
(
    INOUT PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *portGroupListPtr,
       IN PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *portGroupListFromPtr
);

/*******************************************************************************
* ActivePortGroupListAppend
*
* DESCRIPTION:
*      The function appends pord group to list of port gropupsinits 
*
* APPLICABLE DEVICES:
*       none
*
* NOT APPLICABLE DEVICES:
*       none
*
* INPUTS:
*       portGroupListPtr - port group list Ptr
*       portGroupId      - port group to append to the list
*
* OUTPUTS:
*       portGroupListPtr - port group list Ptr
*
* RETURNS:
*       GT_BAD_PTR if pPortGroupList is NULL
*       GT_NO_RESOURCE if no place remains 
*       GT_OK           
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS ActivePortGroupListAppend
(
    INOUT PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *portGroupListPtr, 
       IN GT_U32 portGroupId
);


/*******************************************************************************
* ActivePortGroupListSearch
*
* DESCRIPTION:
*      The function search pord group list for specific port group
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       portGroupListPtr - port group list 
*       portGroupId      - port group to append to the list
*
* OUTPUTS:
*       portGroupListPtr - port group list 
*       isFoundPtr       - pointer to boolean where result of search is stored   GT_TRUE  if found,  GT_FALSE if not found.
*
* RETURNS:
*       GT_BAD_PTR  = on pPortGroupList is NULL or pIsFoundPtr is NULL
*       GT_OK       - on sucess
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS ActivePortGroupListSearch
(
    INOUT PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *portGroupListPtr, 
       IN GT_U32   portGroupId, 
      OUT GT_BOOL *isFoundPtr
);  

/*******************************************************************************
* ActivePortGroupListBuildFromBmp
*
* DESCRIPTION:
*      The function build list of active portgroup by bitmap
*
* APPLICABLE DEVICES:
*       none
*
* NOT APPLICABLE DEVICES:
*       none
*
* INPUTS:
*       activePortGroupListPtr - port group list 
*       portGroupBmp           - port group bitmap
*       firstActivePortGroup   - first active  port groups
*       lastActivePortGroup    - last active  port groups
*
* OUTPUTS:
*       activePortGroupListPtr  - pointer to boolean where result of search is stored 
*
* RETURNS:
*       GT_BAD_PTR  if pPortGroupList is NULL
*       GT_OK in any case 
*
* COMMENTS:
*       if numOfPortGroups == 0 uses max available (i.e. 32 )
*
*******************************************************************************/
GT_STATUS ActivePortGroupListBuildFromBmp
(
    INOUT PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *activePortGroupListPtr,
       IN GT_U32 portGroupBmp,
       IN GT_U32 firstActivePortGroup,
       IN GT_U32 lastActivePortGroup
);



typedef struct ActivePortGroupListIter_STCT
{
    PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC * activePortGroupListPtr;
	GT_U32 cur;
}ActivePortGroupListIter_STC;

/*******************************************************************************
* ActivePortGroupListIterInit
*
* DESCRIPTION:
*      The function build iterator over list of active portgroup
*
* APPLICABLE DEVICES:
*       none
*
* NOT APPLICABLE DEVICES:
*       none
*
* INPUTS:
*       iterPtr         - iterator struct
*       portGroupListPtr   - port group list 
*
* OUTPUTS:
*       iterPtr         - iterator struct
*
* RETURNS:
*       GT_BAD_PTR  -om pPortGroupList is NULL
*       GT_OK       -on sucess
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS ActivePortGroupListIterInit
(
    INOUT ActivePortGroupListIter_STC              *iterPtr,
       IN PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *portGroupListPtr
);

/*******************************************************************************
* ActivePortGroupListIterReset
*
* DESCRIPTION:
*      The function resets iterator over list of active portgroup
*
* APPLICABLE DEVICES:
*       none
*
* NOT APPLICABLE DEVICES:
*       none
*
* INPUTS:
*       iterPtr         - iterator struct
*
* OUTPUTS:
*       iterPtr         = iterator struct
*
* RETURNS:
*       GT_BAD_PTR  - on iterPtr is NULL
*       GT_OK       - on sucess
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS ActivePortGroupListIterReset
(
    INOUT ActivePortGroupListIter_STC *iterPtr
);

/*******************************************************************************
* ActivePortGroupListIterGetCur
*
* DESCRIPTION:
*      The function get current port id 
*
* APPLICABLE DEVICES:
*       none
*
* NOT APPLICABLE DEVICES:
*       none
*
* INPUTS:
*       iterPtr         - iterator struct
*
* OUTPUTS:
*       iterPtr         - iterator struct
*     portIdxPtr        - (pointer to) portIdx
*
* RETURNS:
*       GT_BAD_PTR  - on iterPtr is NULL or portIdxPtr is NULL
*       GT_OK       - on sucess
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS ActivePortGroupListIterGetCur
(
    INOUT ActivePortGroupListIter_STC *iterPtr,
      OUT GT_U32 *portIdxPtr
);

/*******************************************************************************
* ActivePortGroupListIterGotNext
*
* DESCRIPTION:
*      The function go to next item in list of active portgroup
*
* APPLICABLE DEVICES:
*       none
*
* NOT APPLICABLE DEVICES:
*       none
*
* INPUTS:
*       iterPtr         - iterator struct
*
* OUTPUTS:
*       iterPtr         - iterator struct
*
* RETURNS:
*       GT_BAD_PTR  - on iterPtr is NULL or portIdxPtr is NULL
*       GT_OK       - on sucess
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS ActivePortGroupListIterGotNext
(
    INOUT ActivePortGroupListIter_STC *iterPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
