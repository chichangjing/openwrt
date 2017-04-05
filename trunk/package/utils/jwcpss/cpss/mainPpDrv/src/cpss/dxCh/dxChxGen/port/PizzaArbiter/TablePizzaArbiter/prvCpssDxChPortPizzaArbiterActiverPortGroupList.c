/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiterActivePortGroupList.c
*
* DESCRIPTION:
*       dev info need by Pizza Arbiter 
*
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterActiverPortGroupList.h>

/*******************************************************************************
* ActivePortGroupList data structures
*
* DESCRIPTION:
*       list of active port group on PP of fixed length(up to max portgroup on PP )
*
* APPLICABLE DEVICES:
*        
*
* NOT APPLICABLE DEVICES:
*        
*
* Function : 
*       Init()
*       Append()
*       Search()
*
* INPUTS:
*
* OUTPUTS:
*       Init() : None
*       Append() : GT_OK , if no error.
*                  GT_OUT_OF_RANGE , if no place 
*       Search() : GT_OK,
*                  *pIsFound = GT_TRUE if found
*                              GT_FALSE if not
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/


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
)
{
    GT_U32 i;
    
    if (NULL == portGroupListPtr) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    portGroupListPtr->size = 0;
    portGroupListPtr->maxSize = sizeof(portGroupListPtr->list)/ sizeof(portGroupListPtr->list[0]);
    for (i = 0 ; i < portGroupListPtr->maxSize; i++)
    {
        portGroupListPtr->list[i] = 0;
    }
    return GT_OK;
}


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
)
{
    GT_U32 i;
    GT_STATUS rc;
    
    if (NULL == portGroupListPtr || NULL == portGroupListFromPtr) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = ActivePortGroupListInit(portGroupListPtr);
    if (GT_OK != rc)
    {
        return rc;
    }

    for (i = 0 ; i < portGroupListFromPtr->size; i++)
    {
        rc = ActivePortGroupListAppend(portGroupListPtr,portGroupListFromPtr->list[i]);
        if (GT_OK != rc)
        {
            return rc;
        }

    }
    return GT_OK;
}


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
)
{
    if (NULL == portGroupListPtr) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (portGroupListPtr->size == portGroupListPtr->maxSize) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    portGroupListPtr->list[portGroupListPtr->size] = portGroupId;
    portGroupListPtr->size++;
    return GT_OK;
}


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
)  
{
    GT_U32 i;
    
    if (NULL == portGroupListPtr) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (NULL == isFoundPtr ) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    for (i = 0 ; i < portGroupListPtr->size; i++)
    {
        if (portGroupListPtr->list[i] == portGroupId)
        {
            *isFoundPtr = GT_TRUE;
            return GT_OK;
        }
    }
    *isFoundPtr = GT_FALSE;
    return GT_OK;
}


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
)
{
    GT_STATUS rc;
    GT_U32 portGroupId;        /* number of port group */
    GT_U32 mask;

    if (NULL == activePortGroupListPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    ActivePortGroupListInit  (activePortGroupListPtr);

    mask = 1 << firstActivePortGroup;
    for(portGroupId = firstActivePortGroup; portGroupId <= lastActivePortGroup; portGroupId++)
    { 

        if(mask == (portGroupBmp & mask))
        {                                            
            rc = ActivePortGroupListAppend(activePortGroupListPtr,portGroupId);
            if (GT_OK != rc) 
            {
                return rc;
            }
        }
        mask *= 2;
    }
    return GT_OK;
}



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
)
{
    GT_STATUS rc;
    if (NULL == iterPtr || NULL == portGroupListPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    iterPtr->activePortGroupListPtr = portGroupListPtr;
    rc = ActivePortGroupListIterReset(iterPtr);
    if (GT_OK != rc)
    {
        return rc;
    }
    return GT_OK;
}

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
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    iterPtr->cur = 0;
    return GT_OK;
}

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
)
{
    if (NULL == iterPtr || NULL == portIdxPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (iterPtr->cur < iterPtr->activePortGroupListPtr->size)
    {
        *portIdxPtr = iterPtr->activePortGroupListPtr->list[iterPtr->cur];
    }
    else
    {
        *portIdxPtr = PRV_CPSS_BAD_PORTGROUP_CNS;
    }
    return GT_OK;
}

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
)
{
    if (NULL == iterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (iterPtr->cur < iterPtr->activePortGroupListPtr->size)
    {
        iterPtr->cur++;
    }
    return GT_OK;

}
