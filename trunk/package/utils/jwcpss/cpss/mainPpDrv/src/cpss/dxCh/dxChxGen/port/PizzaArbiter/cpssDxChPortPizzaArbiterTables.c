/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPizzaArbiterTables.c
*
* DESCRIPTION:
*       Pizza arbiter Table for Lion2 A0 360/480 GHz clock (12/16 slices)
                                      B0 360/480 GHz clock (24/32 slices)
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterTables.h>

/*--------------------------------------------------------------------------*/
/*    Conversion table from slice number per port to portState              */
/*                      and vise versa                                      */
/*    returns   GT_OK         if slice number/port state is defined         */
/*              GT_BAD_VALUE  if not slice number/port state is not defined */
/*--------------------------------------------------------------------------*/

/*******************************************************************************
* sliceNum2PortStateStateConvertFromNum
*
* DESCRIPTION:
*         Conversion number of slices to port state
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       sliceNum     - slice number to convert into state
* OUTPUTS:
*       portStatePtr -   where to store teh result
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - bad pointer to store result
*
* COMMENTS:
*       shall be removed , currently is nonsense
*
*******************************************************************************/
GT_STATUS sliceNum2PortStateStateConvertFromNum
(
    IN   GT_U32 sliceNum,
    OUT  PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT *portStatePtr
)
{
    if (portStatePtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    *portStatePtr = sliceNum;
    return GT_OK;
}

/*******************************************************************************
* sliceNum2PortStateStateConvert2Num
*
* DESCRIPTION:
*         Conversion of port state to number of slice number
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       portState      slice number to convert into state
*
* OUTPUTS:
*       sliceNumPtr   where to store teh result
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - bad pointer to store result
*
* COMMENTS:
*       shall be removed , currently is nonsense
*
*******************************************************************************/
GT_STATUS sliceNum2PortStateStateConvert2Num
(
    OUT GT_U32 *sliceNumPtr,
    IN  PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT portState
)
{
    if (sliceNumPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    *sliceNumPtr = portState;
    return GT_OK;
}


/*----------------------------------------------------------*/
/* Speed 2 Slice Conversion                                 */
/*----------------------------------------------------------*/
/*******************************************************************************
* PortSpeed2SliceNumInitEmpty
*
* DESCRIPTION:
*         init speed 2 slice structure
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       speed2SlicePtr - (pointer to) speed 2 slice converter
*
* OUTPUTS:
*       speed2SlicePtr - (pointer to) speed 2 slice converter
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - bad pointer to store result
*
* COMMENTS:
*       shall be removed , currently is nonsense
*
*******************************************************************************/
static GT_STATUS PortSpeed2SliceNumInitEmpty
(
    INOUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC *speed2SlicePtr
)
{
    if (NULL == speed2SlicePtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    speed2SlicePtr->speed     = CPSS_PORT_SPEED_NA_E;
    speed2SlicePtr->slicesNum = CPSS_INVALID_SLICE_NUM;
    return GT_OK;
}

/*******************************************************************************
* PortSpeed2SliceNumInit
*
* DESCRIPTION:
*         init speed 2 slice structure to specific value
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       speed2SlicePtr - (pointer to) speed 2 slice converter
*       speed          - port speed
*       slicesNum      - corresponding slice number
*
* OUTPUTS:
*       speed2SlicePtr - (pointer to) speed 2 slice converter
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - bad pointer to store result
*
* COMMENTS:
*       shall be removed , currently is nonsense
*
*******************************************************************************/
static GT_STATUS PortSpeed2SliceNumInit
(
    INOUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC *speed2SlicePtr,
    IN    CPSS_PORT_SPEED_ENT speed,
    IN    GT_U32              slicesNum
)
{
    if (NULL == speed2SlicePtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    speed2SlicePtr->speed     = speed;
    speed2SlicePtr->slicesNum = slicesNum;
    return GT_OK;
}

/*******************************************************************************
* portSpeed2SliceNumListInit
*
* DESCRIPTION:
*      Init speed 2 slice num list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       listPtr      - pointer to Object
*       size          - port group id
*
* OUTPUTS:
*       listPtr      - pointer to Object
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - bad devNum, portGroupId
*
* COMMENTS:
*       List is array of CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC, size is privided by user.
*       List shall be ended by CPSS_INVALID_SLICE_NUM is last item in list
*       Initially all items are initialized as empty (i.e. slices number is initialized
*       CPSS_INVALID_SLICE_NUM
*
*******************************************************************************/
GT_STATUS portSpeed2SliceNumListInit
(
    INOUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC *listPtr,
    IN    GT_U32 size
)
{
    GT_U32 i;
    GT_STATUS rc;
    if (NULL == listPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    for (i = 0 ; i < size ; i++)
    {
        rc = PortSpeed2SliceNumInitEmpty(&listPtr[i]);
        if (rc != GT_OK)
        {
            return rc;
        }

    }
    return GT_OK;
}

/*******************************************************************************
* portSpeed2SliceNumListCopy
*
* DESCRIPTION:
*      Init speed 2 slice num list to another list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       listPtr       - pointer Object (destination)
*       listSrcPtr    - pointer to src
*
* OUTPUTS:
*       listPtr       - pointer Object (destination)
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - bad devNum, portGroupId
*
* COMMENTS:
*       List is array of CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC, shall be ended by CPSS_INVALID_SLICE_NUM in
*       last item of list
*
*******************************************************************************/
GT_STATUS portSpeed2SliceNumListCopy
(
    INOUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC *listPtr,
    IN  CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC   *listSrcPtr
)
{
    GT_U32 i;
    GT_STATUS rc;
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC * pSrcPtr;
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC * pDstPtr;
    if (NULL == listPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == listSrcPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    pDstPtr = listPtr;
    pSrcPtr = listSrcPtr;
    for (i = 0 ; pSrcPtr->slicesNum != CPSS_INVALID_SLICE_NUM ; i++, pSrcPtr++,pDstPtr++)
    {
        rc = PortSpeed2SliceNumInit(pDstPtr,pSrcPtr->speed,pSrcPtr->slicesNum);
        if (rc != GT_OK)
        {
            return rc;
        }

    }
    return GT_OK;
}


/*******************************************************************************
* PortSpeed2SliceNumListLen
*
* DESCRIPTION:
*      return the length of the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       listPtr      - pointer Object
*
* OUTPUTS:
*       listPtr      - pointer Object
*       lenPtr       - pointer to length of list
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pListPtr
*
* COMMENTS:
*       List is array of CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC, shall be ended by CPSS_INVALID_SLICE_NUM in
*       last item of list
*
*******************************************************************************/
GT_STATUS PortSpeed2SliceNumListLen
(
     INOUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC *listPtr,
     OUT   GT_U32 *lenPtr
)
{
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC * ptr;
    GT_U32 i;

    if (NULL == listPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == lenPtr )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    *lenPtr = 0;
    ptr = listPtr;
    for (i = 0 ; ptr->slicesNum != CPSS_INVALID_SLICE_NUM ; i++, ptr++)
    {
    }
    *lenPtr = i+1;
    return GT_OK;

}

/*******************************************************************************
* portSpeed2SliceNumListFind
*
* DESCRIPTION:
*      find slices num by speed in list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       listPtr       - pointer Object
*       portSpeed     - port speed
*
* OUTPUTS:
*       listPtr         - pointer Object *
*       sliceNumUsedPtr - size of list
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pListPtr
*
* COMMENTS:
*       List is array of CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC, shall be ended by CPSS_INVALID_SLICE_NUM in
*       last item of list
*
*******************************************************************************/
GT_STATUS portSpeed2SliceNumListFind
(
    INOUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC *listPtr,
    IN    CPSS_PORT_SPEED_ENT portSpeed,
    OUT   GT_U32      *sliceNumUsedPtr
)
{
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC * ptr;
    GT_U32 i;

    if (NULL == listPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == sliceNumUsedPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    ptr = listPtr;
    for (i = 0 ; ptr->slicesNum != CPSS_INVALID_SLICE_NUM ; i++, ptr++)
    {
        if (ptr->speed == portSpeed)
        {
            *sliceNumUsedPtr = ptr->slicesNum;
            return GT_OK;
        }
    }
    *sliceNumUsedPtr = 0;
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* portGroupSpeed2SliceNumListInit
*
* DESCRIPTION:
*      Init list of port group : pointer to Speed2Slice number list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       portGroupSpeed2SliceTblPtr      - pointer Object
*
* OUTPUTS:
*       portGroupSpeed2SliceTblPtr      - pointer Object
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pListPtr
*
* COMMENTS:
*       List is array of CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC, shall be ended by CPSS_PA_INVALID_PORT in
*       last item of list
*
*******************************************************************************/
GT_STATUS portGroupSpeed2SliceNumListInit
(
    INOUT CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *portGroupSpeed2SliceTblPtr
)
{
    GT_U32 i;
    CPSS_DXCH_PORT_SPEED_TO_SLICENUM_LIST_ITEM_STC * pItem;

    if (NULL == portGroupSpeed2SliceTblPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    pItem = &portGroupSpeed2SliceTblPtr->arr[0];
    for (i = 0 ; i<sizeof(portGroupSpeed2SliceTblPtr->arr)/sizeof(portGroupSpeed2SliceTblPtr->arr[0]); i++, pItem++)
    {
        pItem->portN                     = CPSS_PA_INVALID_PORT;
        pItem->portSpeed2SliceNumListPtr = NULL;

    }
    return GT_OK;
}


/*******************************************************************************
* portGroupSpeed2SliceNumListAppend
*
* DESCRIPTION:
*      list of port group : pointer to Speed2Slice number list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       portGroupSpeed2SliceTblPtr      - pointer Object
*       portNum                         - port number
*       portSpeed2SliceNumListPtr       - pointer to corresponding speed 2 slice conversion table
*
* OUTPUTS:
*       portGroupSpeed2SliceTblPtr      - pointer Object
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pListPtr
*
* COMMENTS:
*       List is array of CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC, shall be ended by CPSS_PA_INVALID_PORT in
*       last item of list
*
*******************************************************************************/
GT_STATUS portGroupSpeed2SliceNumListAppend
(
    INOUT CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *portGroupSpeed2SliceTblPtr,
    IN    GT_PORT_NUM portNum,
    IN    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC *portSpeed2SliceNumListPtr
)
{
    GT_U32 i;
    CPSS_DXCH_PORT_SPEED_TO_SLICENUM_LIST_ITEM_STC * pItem;

    if (NULL == portGroupSpeed2SliceTblPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (NULL == portGroupSpeed2SliceTblPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    pItem = &portGroupSpeed2SliceTblPtr->arr[0];
    for (i = 0 ; i < sizeof(portGroupSpeed2SliceTblPtr->arr)/sizeof(portGroupSpeed2SliceTblPtr->arr[0])-1; i++, pItem++)
    {
        if (pItem->portN == portNum)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }
        if (pItem->portSpeed2SliceNumListPtr == NULL)
        {
            pItem->portN = portNum;
            pItem->portSpeed2SliceNumListPtr = portSpeed2SliceNumListPtr;
            return GT_OK;
        }
    }
    /* no place to insert new port */
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
}


/*******************************************************************************
* portGroupSpeed2SliceNumListFind
*
* DESCRIPTION:
*      search in list of <port group : pointer to Speed2Slice number list>
*          number of sslices to be used for specific port and speed
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       portGroupSpeed2SliceTblPtr      - pointer Object
*       portNum                         - port number
*       portSpeed                       - speed to convert
*
* OUTPUTS:
*       portGroupSpeed2SliceTblPtr      - pointer Object
*       sliceNumUsedPtr                 - where to store result
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pListPtr
*       GT_NOT_FOUND    - if not found the port or speed
* COMMENTS:
*       List is array of CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC, shall be ended by CPSS_PA_INVALID_PORT in
*       last item of list
*
*******************************************************************************/
GT_STATUS portGroupSpeed2SliceNumListFind
(
    INOUT CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *portGroupSpeed2SliceTblPtr,
    IN    GT_PORT_NUM         portNum,
    IN    CPSS_PORT_SPEED_ENT portSpeed,
    OUT   GT_U32             *sliceNumUsedPtr
)
{
    GT_U32 i;
    CPSS_DXCH_PORT_SPEED_TO_SLICENUM_LIST_ITEM_STC * pItem;

    pItem = &portGroupSpeed2SliceTblPtr->arr[0];

    if (NULL == portGroupSpeed2SliceTblPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == sliceNumUsedPtr )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    for (i = 0 ; pItem->portSpeed2SliceNumListPtr != NULL; i++, pItem++)
    {
        if (pItem->portN == portNum)
        {
            return portSpeed2SliceNumListFind(pItem->portSpeed2SliceNumListPtr,portSpeed,sliceNumUsedPtr);
        }
    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* portGroupSpeed2SliceNumListCmp
*
* DESCRIPTION:
*      compare two speed to slice conversion tables and 
*          returns GT_OK if equals.
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       tbl1Ptr      - pointer to first table
*       tbl2Ptr      - pointer to first table
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success, if equal
*       GT_BAD_PTR      - pListPtr
*       GT_FAIL         - on success, if no equal
*
* COMMENTS:
*       List is array of CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC, shall be ended by CPSS_PA_INVALID_PORT in
*       last item of list 
*
*******************************************************************************/
GT_STATUS portGroupSpeed2SliceNumListCmp
(
    IN CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *tbl1Ptr,
    IN CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *tbl2Ptr
)
{
    GT_U32 portNum;
    GT_U32 portIdx;
    GT_U32 sliceIdx;
    CPSS_DXCH_PORT_SPEED_TO_SLICENUM_LIST_ITEM_STC * list1Ptr;
    CPSS_DXCH_PORT_SPEED_TO_SLICENUM_LIST_ITEM_STC * list2Ptr;

    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC  * listItem1Ptr;
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC  * listItem2Ptr;

    if (NULL == tbl1Ptr || NULL == tbl2Ptr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    portNum = sizeof(tbl1Ptr->arr)/sizeof(tbl1Ptr->arr[0]);
    for (portIdx = 0 ; portIdx < portNum ; portIdx++)
    {
        list1Ptr = &tbl1Ptr->arr[portIdx];
        list2Ptr = &tbl2Ptr->arr[portIdx];
        if (list1Ptr->portN != list2Ptr->portN)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        if (list1Ptr->portN == CPSS_PA_INVALID_PORT)
        {
            break;
        }

        listItem1Ptr = &list1Ptr->portSpeed2SliceNumListPtr[0];
        listItem2Ptr = &list2Ptr->portSpeed2SliceNumListPtr[0];
        for (sliceIdx = 0 ; listItem1Ptr->slicesNum != CPSS_INVALID_SLICE_NUM ; sliceIdx++, listItem1Ptr++, listItem2Ptr++)
        {
            if (listItem1Ptr->speed != listItem2Ptr->speed)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
            if (listItem1Ptr->slicesNum != listItem2Ptr->slicesNum)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }
    }
    return GT_OK;
}


/*----------------------------------------------------------*/
/* Port Priority Matrixt                                    */
/*----------------------------------------------------------*/
/*******************************************************************************
* portPrioritiesMatrixInit
*
* DESCRIPTION:
*      port priority list Matrix Initialization
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       matrPtr      - pointer Object
*
* OUTPUTS:
*       matrPtr      - pointer Object
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pListPtr
*
* COMMENTS:
*       Matrix is binary. It is implemented as sparse array (i.e. array of list)
*       if M(i,j) == 1  means that port i has greater priority than port j
*       and can use slices of port j
*
*******************************************************************************/
GT_STATUS portPrioritiesMatrixInit
(
    INOUT CPSS_DXCH_PORT_PRIORITY_MATR_STC *matrPtr
)
{
    GT_U32      i;
    GT_U32      portNum;
    CPSS_DXCH_PIZZA_PORT_ID  * portPriorityListDstPtr;

    if (NULL == matrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    matrPtr->portN = 0;
    for (portNum = 0 ; portNum < PIZZA_MAX_PORTS_PER_PORTGROUP_CNS; portNum++)
    {
        portPriorityListDstPtr = &matrPtr->portPriorityList[portNum][0];
        for (i = 0 ; i < PORTPRIORITY_MAX_CNS ; i++)
        {
            portPriorityListDstPtr[i] = CPSS_PA_INVALID_PORT;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* portPrioritiesMatrixCopy
*
* DESCRIPTION:
*      port priority list Matrix copy
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       matrDstPtr   - Dst Matrix
*       matrSrcPtr   - Src Matrix
*
* OUTPUTS:
*       matrDstPtr   - Dst Matrix
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pListPtr
*       GT_CREATE_ERROR - not enough place
*
* COMMENTS:
*       Matrix is binary. It is implemented as sparse array (i.e. array of list)
*       if M(i,j) == 1  means that port i has greater priority than port j
*       and can use slices of port j
*
*******************************************************************************/
GT_STATUS portPrioritiesMatrixCopy
(
    INOUT CPSS_DXCH_PORT_PRIORITY_MATR_STC *matrDstPtr,
    IN  CPSS_DXCH_PORT_PRIORITY_MATR_STC   *matrSrcPtr
)
{
    GT_STATUS   rc;
    GT_U32      portNum;
    GT_U32      i;
    CPSS_DXCH_PIZZA_PORT_ID  * portPriorityListSrcPtr;
    CPSS_DXCH_PIZZA_PORT_ID  * portPriorityListDstPtr;

    if (NULL == matrDstPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == matrDstPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    /* Clear Priority Lists */
    rc = portPrioritiesMatrixInit(matrDstPtr);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* copy */
    matrDstPtr->portN = matrSrcPtr->portN;
    for (portNum = 0 ; portNum < matrDstPtr->portN; portNum++)
    {
        portPriorityListSrcPtr = &matrSrcPtr->portPriorityList[portNum][0];
        portPriorityListDstPtr = &matrDstPtr->portPriorityList[portNum][0];
        for (i = 0 ; portPriorityListSrcPtr[i] != CPSS_PA_INVALID_PORT ; i++)
        {
            if (i == PORTPRIORITY_MAX_CNS)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_CREATE_ERROR, LOG_ERROR_NO_MSG);
            }
            portPriorityListDstPtr[i] = portPriorityListSrcPtr[i];
        }
        portPriorityListDstPtr[i] = CPSS_PA_INVALID_PORT;
    }
    return GT_OK;
}

/*---------------------------------------------------------------*/
/* Port Priority                                                 */
/* if port is present in list it means it has lower priority     */
/*      than port-list owner                                     */
/* End of list is indicated by INVALID_PORT value                */
/* returns                                                       */
/*   GT_OK if all parameters are OK                              */
/*   GT_BAD_PTR otherwise                                        */
/*   in case GT_OK                                               */
/*      isPriorPtr = GT_TRUE  if port portNum is found in list   */
/*                   GT_FALSE if port portNum is not found       */
/*---------------------------------------------------------------*/
/*******************************************************************************
* PortPrioritiesMatrixPortPriorityListGet
*
* DESCRIPTION:
*      Search port priority list for specific port
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       portPriorityListPtr - list
*       portNum       - port num
*
* OUTPUTS:
*       portPriorityListPtr - list
*       isPriorPtr : pointer to where store the results
*
* RETURNS:
*       GT_OK           - on success,
*       GT_FALSE        - port not found
*       GT_BAD_PTR      - pListPtr
*
* COMMENTS:
*       CPSS_PA_INVALID_PORT shall be last item in the list
*******************************************************************************/
GT_STATUS portPrioritiesListIsPriorTo
(
    INOUT CPSS_DXCH_PIZZA_PORT_ID *portPriorityListPtr,
       IN GT_PHYSICAL_PORT_NUM     portNum,
      OUT GT_BOOL                 *isPriorPtr
)
{
    CPSS_DXCH_PIZZA_PORT_ID * pPort;

    if (NULL == portPriorityListPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (isPriorPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    for (pPort = portPriorityListPtr; *pPort  != CPSS_PA_INVALID_PORT;  pPort ++)
    {
        if (*pPort == portNum)
        {
            *isPriorPtr = GT_TRUE;
            return GT_OK;
        }
    }
    *isPriorPtr = GT_FALSE;
    return GT_OK;
}


/*******************************************************************************
* portPrioritiesMatrixPortPriorityListGet
*
* DESCRIPTION:
*      Get port priority list from Priority Matrix
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       priorityMatrPtr - Dst Matrix
*       portNum       - port num
*
* OUTPUTS:
*       priorityMatrPtr - Dst Matrix
*       portPriorityListPtrPtr - pointer to port priority list where result is stored
*
* RETURNS:
*       GT_OK           - on success,
*       GT_OUT_OF_RANGE - port not found
*       GT_BAD_PTR      - pListPtr
*
* COMMENTS:
*       Matrix is binary. It is implemented as sparse array (i.e. array of list)
*       if M(i,j) == 1  means that port i has greater priority than port j
*       and can use slices of port j
*
*******************************************************************************/
GT_STATUS portPrioritiesMatrixPortPriorityListGet
(
    INOUT CPSS_DXCH_PORT_PRIORITY_MATR_STC *priorityMatrPtr,
       IN GT_PHYSICAL_PORT_NUM              portNum,
      OUT CPSS_DXCH_PIZZA_PORT_ID         **portPriorityListPtrPtr
)
{
    if (NULL == priorityMatrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == portPriorityListPtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (portNum >= priorityMatrPtr->portN )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    *portPriorityListPtrPtr = &priorityMatrPtr->portPriorityList[portNum][0];
    return GT_OK;
}


/*-----------------------------------------------------
**   function implementation
**-----------------------------------------------------
*/
/*******************************************************************************
* portPizzaCfgSliceListBySlicesNumInit
*
* DESCRIPTION:
*      init  structure CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pizzaConfPtr - list
*       portNum      - number of ports
*       totalSlices  - number of slices
*
* OUTPUTS:
*       pizzaConfPtr : list 
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS portPizzaCfgSliceListBySlicesNumInit
(
    INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *pizzaConfPtr,
       IN GT_PORT_NUM portNum,
       IN GT_U32 totalSlices
)
{
    GT_U32 i;

    if (NULL == pizzaConfPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    pizzaConfPtr->portNum 	   = portNum;
    pizzaConfPtr->totalSlices = totalSlices;
    for (i = 0 ; i < sizeof(pizzaConfPtr->pizzaCfg)/sizeof(pizzaConfPtr->pizzaCfg[0]); i++ )
    {
        pizzaConfPtr->pizzaCfg[i].sliceNum2Config = 0;
        pizzaConfPtr->pizzaCfg[i].pizzaPtr        = NULL;
    }
    return GT_OK;
}

/*******************************************************************************
* portPizzaCfgSliceListBySlicesNumAppend
*
* DESCRIPTION:
*      append to list an item
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pizzaConfPtr - pizza configuration
*       sliceNum2Config - number of slice
*       pizzaPtr        - pointer to prtial Pizza
*
* OUTPUTS:
*       pizzaConfPtr - pizza configuration
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*       GT_CREATE_ERROR - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS portPizzaCfgSliceListBySlicesNumAppend
(
    INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *pizzaConfPtr,
       IN GT_U32 sliceNum2Config,
       IN CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *pizzaPtr
)
{
    GT_U32 i;
    if (NULL == pizzaConfPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    /* search for empty space*/
    for (i = 0 ; i < sizeof(pizzaConfPtr->pizzaCfg)/sizeof(pizzaConfPtr->pizzaCfg[0]) ; i++)
    {
        if (pizzaConfPtr->pizzaCfg[i].sliceNum2Config == 0)
        {
            if (pizzaConfPtr->pizzaCfg[i].pizzaPtr == NULL)
            {
                pizzaConfPtr->pizzaCfg[i].sliceNum2Config = sliceNum2Config;
                pizzaConfPtr->pizzaCfg[i].pizzaPtr        = pizzaPtr;
                return GT_OK;
            }
        }
    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_CREATE_ERROR, LOG_ERROR_NO_MSG);
}


/*******************************************************************************
* portPizzaCfgSliceListBySlicesNumCopy
*
* DESCRIPTION:
*      copy one list into another
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pPizzaDstPtr - list dst
*       pPizzaSrcPtr - list src
*
* OUTPUTS:
*       pPizzaDstPtr : pointer to where store the results
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS portPizzaCfgSliceListBySlicesNumCopy
(
    INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *pPizzaDstPtr,
       IN CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *pPizzaSrcPtr
)
{
    GT_STATUS rc;
    GT_U32 i;
    if (NULL == pPizzaDstPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == pPizzaSrcPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    rc = portPizzaCfgSliceListBySlicesNumInit(pPizzaDstPtr,pPizzaSrcPtr->portNum,pPizzaSrcPtr->totalSlices);
    if (GT_OK != rc)
    {
        return rc;
    }
    for (i = 0 ; i < sizeof(pPizzaSrcPtr->pizzaCfg)/sizeof(pPizzaSrcPtr->pizzaCfg[0]) ; i++)
    {
        if (pPizzaSrcPtr->pizzaCfg[i].sliceNum2Config == 0)
        {
            break;
        }
        pPizzaDstPtr->pizzaCfg[i].sliceNum2Config = pPizzaSrcPtr->pizzaCfg[i].sliceNum2Config;
        pPizzaDstPtr->pizzaCfg[i].pizzaPtr        = pPizzaSrcPtr->pizzaCfg[i].pizzaPtr;
    }
    return GT_OK;
}

/*******************************************************************************
* PortPizzaCfgSliceListBySlicesNumCopy_wAlloc
*
* DESCRIPTION:
*      copy one list into another with allocation from pool
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pPizzaDstPtr - destination
*       pPizzaSrcPtr - src
*       pPoolPtr     - pool from which list is allocated
*
* OUTPUTS:
*       pPizzaDstPtr : pointer to where store the results
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS portPizzaCfgSliceListBySlicesNumCopy_wAlloc
(
    INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *pPizzaDstPtr,
       IN CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *pPizzaSrcPtr,
       IN PortPizzaCfgSlicesListPool_STC *pPoolPtr
)
{
    GT_STATUS rc;
    GT_U32 i;
    CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC * pPizzaPtr;

    if (NULL == pPizzaDstPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == pPizzaSrcPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == pPoolPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    rc = portPizzaCfgSliceListBySlicesNumInit(pPizzaDstPtr,pPizzaSrcPtr->portNum,pPizzaSrcPtr->totalSlices);
    if (GT_OK != rc )
    {
        return rc;
    }

    for (i = 0 ; i < sizeof(pPizzaSrcPtr->pizzaCfg)/sizeof(pPizzaSrcPtr->pizzaCfg[0]) ; i++)
    {
        if (pPizzaSrcPtr->pizzaCfg[i].sliceNum2Config == 0)
        {
            break;
        }
        rc = portPizzaCfgSlicesListPoolAlloc(pPoolPtr, /*OUT*/&pPizzaPtr);
        if (GT_OK != rc ) /* No more items to alloc , cann't proceed */
        {
            return rc;
        }
        rc = portPizzaCfgSlicesListCopy(pPizzaPtr,pPizzaSrcPtr->pizzaCfg[i].pizzaPtr);
        if (GT_OK != rc )
        {
            return rc;
        }

        pPizzaDstPtr->pizzaCfg[i].sliceNum2Config = pPizzaSrcPtr->pizzaCfg[i].sliceNum2Config;
        pPizzaDstPtr->pizzaCfg[i].pizzaPtr        = pPizzaPtr;
    }
    return GT_OK;
}

/*******************************************************************************
* portPizzaCfgSliceListBySlicesNumGet
*
* DESCRIPTION:
*      get list of slices to be used by port when alloced
*      specific number of slices
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pizzaPtr - pointer to pizza
*       portNum      - port Index
*       sliceNum2Config  - number of slices
*
* OUTPUTS:
*       pizzaPtr : pointer to where store the results
*       ppSliceListPtrPtr - pointer to slice list
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*       GT_BAD_VALUE     - port is out of range
*       GT_NOT_SUPPORTED - the specified slice number configuration is not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS portPizzaCfgSliceListBySlicesNumGet
(
    INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *pizzaPtr,
      OUT CPSS_DXCH_PIZZA_SLICE_ID                       **ppSliceListPtrPtr,
       IN GT_PORT_NUM                                      portNum,
       IN PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT         sliceNum2Config
)
{
    GT_U32 i;
    static CPSS_DXCH_PIZZA_SLICE_ID emptyList = CPSS_PA_INVALID_SLICE; 
    CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC * pPizzaCfg;
    SliceNumXPortPizzaCfgSlicesList_STC * pSliceXPortPizzaCfgSliceList;

    * ppSliceListPtrPtr = (CPSS_DXCH_PIZZA_SLICE_ID *)NULL;

    if (sliceNum2Config == PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT_NONE_CNS)
    {
        * ppSliceListPtrPtr = &emptyList;
        return GT_OK;
    }

    pSliceXPortPizzaCfgSliceList = &pizzaPtr->pizzaCfg[0];
    for (i = 0 ; i < sizeof(pizzaPtr->pizzaCfg)/sizeof(pizzaPtr->pizzaCfg[0]) ; i++, pSliceXPortPizzaCfgSliceList++)
    {
        if (pSliceXPortPizzaCfgSliceList->sliceNum2Config == sliceNum2Config)
        {
            pPizzaCfg = pSliceXPortPizzaCfgSliceList->pizzaPtr;
            if (pPizzaCfg == NULL)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            if (portNum >= pizzaPtr->portNum)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
            }
            *ppSliceListPtrPtr = &pPizzaCfg->portConfArr[portNum][0];
            return GT_OK;
        }
    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

}

/*-------------------------------------------------------------------*/
/*                                                                   */
/*-------------------------------------------------------------------*/
/*******************************************************************************
* portPizzaCfgSliceNum2SlicesListIterInit
*
* DESCRIPTION:
*      init iterator of port's slices list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pIterPtr                            - pointer to iterator
*       portPizzaCfgSliceNum2SlicesListPtr  - pointer to list
*
* OUTPUTS:
*       pIterPtr                            - pointer to iterator
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*
* COMMENTS:
*       Usage :
*         PortPizzaCfgSliceNum2SlicesListIter_STC iter;
*         SliceNumXPortPizzaCfgSlicesList_STC  * pSliceNumXPortPizzaCfgSlicesListPtr;
*
*         PortPizzaCfgSliceNum2SlicesListIterInit(&iter);
*         PortPizzaCfgSliceNum2SlicesListIterReste(&iter);
*         while (GT_OK == (rc = PortPizzaCfgSliceNum2SlicesListIterCurGet(&iter,
*                                                                         &pSliceNumXPortPizzaCfgSlicesListPtr)
*         {
*               // User action
*               PortPizzaCfgSliceNum2SlicesListIterGotoNext(&iter);
*         }
*******************************************************************************/
GT_STATUS portPizzaCfgSliceNum2SlicesListIterInit
(
    INOUT PortPizzaCfgSliceNum2SlicesListIter_STC             *pIterPtr,
       IN CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC     *portPizzaCfgSliceNum2SlicesListPtr
)
{
    if (NULL == pIterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == portPizzaCfgSliceNum2SlicesListPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    pIterPtr->pPortPizzaCfgSliceNum2SlicesListPtr = portPizzaCfgSliceNum2SlicesListPtr;
    pIterPtr->pSliceNumXPortPizzaCfgSlicesListPtr = &portPizzaCfgSliceNum2SlicesListPtr->pizzaCfg[0];
    return GT_OK;
}

/*******************************************************************************
* portPizzaCfgSliceNum2SlicesListIterReset
*
* DESCRIPTION:
*      reset the iterator to initial state
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pIterPtr                            - pointer to iterator
*
* OUTPUTS:
*       pIterPtr                            - pointer to iterator
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*
* COMMENTS:
*       Usage :
*         PortPizzaCfgSliceNum2SlicesListIter_STC iter;
*         SliceNumXPortPizzaCfgSlicesList_STC  * pSliceNumXPortPizzaCfgSlicesListPtr;
*
*         PortPizzaCfgSliceNum2SlicesListIterInit(&iter);
*         PortPizzaCfgSliceNum2SlicesListIterReste(&iter);
*         while (GT_OK == (rc = PortPizzaCfgSliceNum2SlicesListIterCurGet(&iter,
*                                                                         &pSliceNumXPortPizzaCfgSlicesListPtr)
*         {
*               // User action
*               PortPizzaCfgSliceNum2SlicesListIterGotoNext(&iter);
*         }
*******************************************************************************/
GT_STATUS portPizzaCfgSliceNum2SlicesListIterReset
(
    INOUT PortPizzaCfgSliceNum2SlicesListIter_STC *pIterPtr
)
{
    if (NULL == pIterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    pIterPtr->pSliceNumXPortPizzaCfgSlicesListPtr = &pIterPtr->pPortPizzaCfgSliceNum2SlicesListPtr->pizzaCfg[0];
    return GT_OK;
}


/*******************************************************************************
* portPizzaCfgSliceNum2SlicesListIterCurGet
*
* DESCRIPTION:
*      get current item of the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pIterPtr                            - pointer to iterator
*
* OUTPUTS:
*       pIterPtr                            - pointer to iterator
*       ppSliceNumXPortPizzaCfgSlicesListPtrPtr = (pointer to)current slice list
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*
* COMMENTS:
*       Usage :
*         PortPizzaCfgSliceNum2SlicesListIter_STC iter;
*         SliceNumXPortPizzaCfgSlicesList_STC  * pSliceNumXPortPizzaCfgSlicesListPtr;
*
*         PortPizzaCfgSliceNum2SlicesListIterInit(&iter);
*         PortPizzaCfgSliceNum2SlicesListIterReste(&iter);
*         while (GT_OK == (rc = PortPizzaCfgSliceNum2SlicesListIterCurGet(&iter,
*                                                                         &pSliceNumXPortPizzaCfgSlicesListPtr)
*         {
*               // User action
*               PortPizzaCfgSliceNum2SlicesListIterGotoNext(&iter);
*         }
*******************************************************************************/
GT_STATUS portPizzaCfgSliceNum2SlicesListIterCurGet
(
    INOUT PortPizzaCfgSliceNum2SlicesListIter_STC *pIterPtr,
      OUT SliceNumXPortPizzaCfgSlicesList_STC    **ppSliceNumXPortPizzaCfgSlicesListPtrPtr
)
{
    if (NULL == pIterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == ppSliceNumXPortPizzaCfgSlicesListPtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    * ppSliceNumXPortPizzaCfgSlicesListPtrPtr = NULL;
    if (pIterPtr->pSliceNumXPortPizzaCfgSlicesListPtr->sliceNum2Config == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_MORE, LOG_ERROR_NO_MSG);
    }
    *ppSliceNumXPortPizzaCfgSlicesListPtrPtr = pIterPtr->pSliceNumXPortPizzaCfgSlicesListPtr;
    return GT_OK;
}

/*******************************************************************************
* PortPizzaCfgSliceNum2SlicesListIterGotoNext
*
* DESCRIPTION:
*      goto to the nxt item
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pIterPtr                                - pointer to iterator
*
* OUTPUTS:
*       pIterPtr                                - pointer to iterator
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*
* COMMENTS:
*       Usage :
*         PortPizzaCfgSliceNum2SlicesListIter_STC iter;
*         SliceNumXPortPizzaCfgSlicesList_STC  * pSliceNumXPortPizzaCfgSlicesListPtr;
*
*         PortPizzaCfgSliceNum2SlicesListIterInit(&iter);
*         PortPizzaCfgSliceNum2SlicesListIterReste(&iter);
*         while (GT_OK == (rc = PortPizzaCfgSliceNum2SlicesListIterCurGet(&iter,
*                                                                         &pSliceNumXPortPizzaCfgSlicesListPtr)
*         {
*               // User action
*               PortPizzaCfgSliceNum2SlicesListIterGotoNext(&iter);
*         }
*******************************************************************************/
GT_STATUS portPizzaCfgSliceNum2SlicesListIterGotoNext
(
    INOUT PortPizzaCfgSliceNum2SlicesListIter_STC *pIterPtr
)
{
    if (NULL == pIterPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (pIterPtr->pSliceNumXPortPizzaCfgSlicesListPtr->sliceNum2Config == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_MORE, LOG_ERROR_NO_MSG);
    }
    pIterPtr->pSliceNumXPortPizzaCfgSlicesListPtr ++;
    return GT_OK;
}


/*-------------------------------------------------------------------*/
/*                                                                   */
/*-------------------------------------------------------------------*/
/*******************************************************************************
* pizzaProfileInit
*
* DESCRIPTION:
*      init the pizza Profile to empty one
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pPizzaProfilePtr  - pointer to Profile
*
* OUTPUTS:
*       pPizzaProfilePtr  - pointer to iterator
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pizzaProfileInit
(
    INOUT CPSS_DXCH_PIZZA_PROFILE_STC *pPizzaProfilePtr
)
{
    GT_U32 i;
    if (pPizzaProfilePtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    pPizzaProfilePtr->portNum        = 0;
    pPizzaProfilePtr->maxSliceNum    = 0;
    pPizzaProfilePtr->txQDef.pizzaRepetitionNum = 0;
    pPizzaProfilePtr->txQDef.cpuPortDef.type = CPSS_DXCH_CPUPORT_TYPE_UNDEFINED_E;
    pPizzaProfilePtr->txQDef.cpuPortDef.cpuPortNum = 0;
    for (i = 0 ; i < sizeof(pPizzaProfilePtr->txQDef.cpuPortDef.cpuPortSlices)/sizeof(pPizzaProfilePtr->txQDef.cpuPortDef.cpuPortSlices[0]); i++)
    {
        pPizzaProfilePtr->txQDef.cpuPortDef.cpuPortSlices[i] = 0;
    }
    pPizzaProfilePtr->regModePizzaPtr      = (CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)NULL;
    pPizzaProfilePtr->extModePizzaPtr      = (CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)NULL;
    pPizzaProfilePtr->pPortPriorityMatrPtr = (CPSS_DXCH_PORT_PRIORITY_MATR_STC *)NULL;
    return GT_OK;
}


/*******************************************************************************
* PortPizzaCfgSlicesListInit
*
* DESCRIPTION:
*      init  structure CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       pizzaPtr - list
*
* OUTPUTS:
*       pizzaPtr - list
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pizzaPtr is NULL
*
* COMMENTS:
*       initialization to slice list for each port to CPSS_PA_INVALID_SLICE
*******************************************************************************/
GT_STATUS portPizzaCfgSlicesListInit
(
    INOUT CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *pizzaPtr
)
{
    GT_U32 portIdx;
    GT_U32 i;
    CPSS_DXCH_PIZZA_SLICE_ID * pSliceList;

    if (NULL == pizzaPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    pizzaPtr->maxPortNum = 0;
    for (portIdx = 0 ; portIdx <  sizeof(pizzaPtr->portConfArr)/sizeof(pizzaPtr->portConfArr[0]); portIdx++)
    {
        pSliceList = &pizzaPtr->portConfArr[portIdx][0];
        for (i = 0 ; i < PIZZA_MAX_SLICES_PER_PORT_CNS+1 ; i++)
        {
            pSliceList[i] = CPSS_PA_INVALID_SLICE;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* portPizzaCfgSlicesListCopy
*
* DESCRIPTION:
*      copy one structure into another
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       dstPizzaPtr - destination structure
*       srcPizzaPtr - source structure
*
* OUTPUTS:
*       dstPizzaPtr - destination structure
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pizzaPtr is NULL
*
* COMMENTS:
*       initialization to slice list for each port to CPSS_PA_INVALID_SLICE
*******************************************************************************/
GT_STATUS portPizzaCfgSlicesListCopy
(
    INOUT CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *dstPizzaPtr,
       IN CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *srcPizzaPtr
)
{
    GT_STATUS rc;
    GT_U32 portIdx;
    GT_U32 i;
    CPSS_DXCH_PIZZA_SLICE_ID * pSliceListSrcPtr;
    CPSS_DXCH_PIZZA_SLICE_ID * pSliceListDstPtr;

    if (NULL == srcPizzaPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (NULL == dstPizzaPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = portPizzaCfgSlicesListInit(dstPizzaPtr);
    if (GT_OK != rc )
    {
        return rc;
    }
    dstPizzaPtr->maxPortNum = srcPizzaPtr->maxPortNum;
    for (portIdx = 0 ; portIdx < dstPizzaPtr->maxPortNum; portIdx++)
    {
        pSliceListSrcPtr = &srcPizzaPtr->portConfArr[portIdx][0];
        pSliceListDstPtr = &dstPizzaPtr->portConfArr[portIdx][0];
        for (i = 0 ; pSliceListSrcPtr[i] != CPSS_PA_INVALID_SLICE ; i++)
        {
            if ( i >= PIZZA_MAX_SLICES_PER_PORT_CNS)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_CREATE_ERROR, LOG_ERROR_NO_MSG);
            }
            pSliceListDstPtr[i] = pSliceListSrcPtr[i];
        }
    }

    return GT_OK;
}

/*--------------------------------------------------------------------*/
/* PortPizzaCfgSlicesListPool - Allocator                             */
/*--------------------------------------------------------------------*/
/*******************************************************************************
* portPizzaCfgSlicesListPoolInit
*
* DESCRIPTION:
*      init  structure PortPizzaCfgSlicesListPool_STC
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       poolPtr - pointer to pool
*
* OUTPUTS:
*       poolPtr - pointer to pool
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS portPizzaCfgSlicesListPoolInit
(
    INOUT PortPizzaCfgSlicesListPool_STC *poolPtr
)
{
    GT_U32 i;
    GT_STATUS rc;
    if (NULL == poolPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    poolPtr->portPizzaCfgCur = 0;
    poolPtr->portPizzaCfgRemains = sizeof(poolPtr->portPizzaCfgArr)/sizeof(poolPtr->portPizzaCfgArr[0]);
    for ( i = 0 ; i < poolPtr->portPizzaCfgRemains ; i++)
    {
        rc = portPizzaCfgSlicesListInit(&poolPtr->portPizzaCfgArr[i]);
        if (GT_OK != rc )
        {
            return rc;
        }

    }
    return GT_OK;
}

/*******************************************************************************
* portPizzaCfgSlicesListPoolAlloc
*
* DESCRIPTION:
*      init  structure PortPizzaCfgSlicesListPool_STC
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       poolPtr - pointer to pool 
*
* OUTPUTS:
*       poolPtr - pointer to pool 
*       pPizzaPtrPtr : pointer to where store the results
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS portPizzaCfgSlicesListPoolAlloc
(
    INOUT PortPizzaCfgSlicesListPool_STC                *poolPtr,
      OUT CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC **pPizzaPtrPtr
)
{
    if (NULL == poolPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (poolPtr->portPizzaCfgRemains == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_MORE, LOG_ERROR_NO_MSG);
    }
    *pPizzaPtrPtr = &poolPtr->portPizzaCfgArr[poolPtr->portPizzaCfgCur];
    poolPtr->portPizzaCfgCur ++ ;
    poolPtr->portPizzaCfgRemains--;
    return GT_OK;
}


/*-------------------------------------------------------------------------*/
/*    Select Pizza Definition by PP Family type                            */
/*                                  revision                               */
/*                                  dev clock                              */
/*    returns   GT_OK               if suitable pizza found                */
/*              GT_NOT_SUPPORTED    if suitable pizza not found            */
/*-------------------------------------------------------------------------*/
/*******************************************************************************
* ListDevRevClockPizzaGet
*
* DESCRIPTION:
*       Select Pizza Definition by PP Family type
*                                  revision
*                                  dev clock
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       pListPtr       - list of profiles
*       devFamilyType  - device family
*       devRevision    - device revison
*       devCoreClock   - clock speed in MHz
*
* OUTPUTS:
*       pListPtr       - list of profiles
*       ppPizzaProfilePtrPtr - (*ppPizzaDefPtr == pizza if device suppoted)
*                                        NULL  otherwise
* RETURNS:
*       GT_OK.              if suitable pizza found
*       GT_NOT_SUPPORTED    if suitable pizza not found
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS ListDevRevClockPizzaGet
(
    INOUT ListDevRevTypeClockPizza_STC     *pListPtr,
    IN    CPSS_PP_FAMILY_TYPE_ENT           devFamilyType,
    IN    GT_U32                            devRevision,
    IN    GT_U32                            devCoreClock,
    OUT   CPSS_DXCH_PIZZA_PROFILE_STC     **ppPizzaProfilePtrPtr
)
{
    GT_U32 i;
    DevRevTypeClockPizza_STC * pDevRevClockPizza;
    GT_U32 revision;

    if (ppPizzaProfilePtrPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    pDevRevClockPizza = pListPtr->pArr;

    /* forward compatible code - support the future revisions       */
    /* find maximal revison in the table that not exceeds the given */
    revision = 0;
    for (i = 0 ; i < pListPtr->size ; i++, pDevRevClockPizza++)
    {
        if ((pDevRevClockPizza->devRevision <= devRevision)
            && (pDevRevClockPizza->devRevision > revision))
        {
            revision = pDevRevClockPizza->devRevision;
        }
        if (revision == devRevision)
        {
            break;
        }
    }

    pDevRevClockPizza = pListPtr->pArr;
    for (i = 0 ; i < pListPtr->size ; i++, pDevRevClockPizza++)
    {
        if (   pDevRevClockPizza->devFamilyType == devFamilyType
            && pDevRevClockPizza->devRevision   == revision)
        {
            if (pDevRevClockPizza->devCoreClock  == devCoreClock || pDevRevClockPizza->devCoreClock == 0 )
            {
                *ppPizzaProfilePtrPtr = pDevRevClockPizza->pizzaProfilePtr;
                return GT_OK;
            }
        }
    }
    *ppPizzaProfilePtrPtr =(CPSS_DXCH_PIZZA_PROFILE_STC*) NULL;
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
}



/*******************************************************************************
* prvCpssDxChPizzaArbiterSystemProfileGet
*
* DESCRIPTION:
*       Select Pizza Definition by PP Family type
*                                  revision
*                                  dev clock
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       devFamilyType  - device family
*       devRevision    - device revison
*       devCoreClock   - clock speed in MHz
*
* OUTPUTS:
*       ppPizzaProfilePtrPtr (*ppPizzaDefPtr == pizza if device suppoted)
*                                        NULL  otherwise
* RETURNS:
*       GT_OK.              if suitable pizza found
*       GT_NOT_SUPPORTED    if suitable pizza not found
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPizzaArbiterSystemProfileGet
(
    IN  CPSS_PP_FAMILY_TYPE_ENT    devFamilyType,
    IN  GT_U32                     devRevision,
    IN  GT_U32                     devCoreClock,
    OUT CPSS_DXCH_PIZZA_PROFILE_STC **ppPizzaProfilePtrPtr
)
{
    if (NULL == ppPizzaProfilePtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    *ppPizzaProfilePtrPtr = (CPSS_DXCH_PIZZA_PROFILE_STC *)NULL;
    return ListDevRevClockPizzaGet((ListDevRevTypeClockPizza_STC *)&prvListDevRevTypeClock2PizzaDefault,
                                /*IN*/ devFamilyType,
                                /*IN*/ devRevision,
                                /*IN*/ devCoreClock,
                                /*OUT*/ppPizzaProfilePtrPtr);
}



