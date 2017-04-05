/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiterTables.h
*
* DESCRIPTION:
*       Pizza arbiter Table definitons
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/

#ifndef __private_CPSS_DXCH_PORT_PIZZA_ARBITER_TABLES_H
#define __private_CPSS_DXCH_PORT_PIZZA_ARBITER_TABLES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h> 
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiterProfile.h>

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
);

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
    OUT  GT_U32 *sliceNumPtr, 
    IN   PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT portState
);


/*
//GT_STATUS PortSpeed2SliceNumInitEmpty(INOUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC * pSpeed2SlicePtr);
//GT_STATUS PortSpeed2SliceNumInit     (INOUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC * pSpeed2SlicePtr,     
//                                         IN CPSS_PORT_SPEED_ENT speed,
//                                         IN GT_U32              slicesNum);
*/
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
);

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
);

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
);

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
);


/*-------------------------------------------------------------------------
** the following oprations are used with the CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC
**            - Init()
**            - Append()
**            - Find()
** 
** GT_STATUS PortGroupSpeed2SliceNumListInit  (INOUT CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * portGroupSpeed2SliceTblPtr);
** GT_STATUS PortGroupSpeed2SliceNumListAppend(INOUT CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * portGroupSpeed2SliceTblPtr,
**                                                IN GT_U32 portN,
**                                                IN CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC * portSpeed2SliceNumListPtr);
** GT_STATUS PortGroupSpeed2SliceNumListFind  (INOUT CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * portGroupSpeed2SliceTblPtr,
**                                                IN GT_U32              portNum,
**                                                IN CPSS_PORT_SPEED_ENT portSpeed,
**                                               OUT GT_U32 *  sliceNumUsedPtr);
** GT_STATUS portGroupSpeed2SliceNumListCmp   (IN CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * tbl1Ptr,
**                                             IN CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * tbl2Ptr);
**
** 
*/

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
);

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
);

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
);

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
);


/*-------------------------------------------------------
**  Pizza Profile Definition
**-------------------------------------------------------
*/



/*---------------------------------------------------------------------*/
/* Port Priority Graph representation in sparse Matrix form            */
/*    for each port list of ports having priority smaller than its one */
/*    End of list is indicated by CPSS_PA_INVALID_PORT  value          */
/*    Empty list consists of CPSS_PA_INVALID_PORT value                */
/*---------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------
** GT_STATUS PortPrioritiesMatrixInit               (INOUT CPSS_DXCH_PORT_PRIORITY_MATR_STC * matrDst);
** GT_STATUS PortPrioritiesMatrixCopy               (INOUT CPSS_DXCH_PORT_PRIORITY_MATR_STC * matrDst, 
**                                                     IN CPSS_DXCH_PORT_PRIORITY_MATR_STC * matrSrc);
** GT_STATUS PortPrioritiesMatrixPortPriorityListGet(INOUT CPSS_DXCH_PORT_PRIORITY_MATR_STC * pPriorityMatr, 
**                                                    OUT CPSS_DXCH_PIZZA_PORT_ID **pPortPriorityList, 
**                                                     IN GT_PHYSICAL_PORT_NUM portNum);
** GT_STATUS PortPrioritiesListIsPriorTo            (INOUT CPSS_DXCH_PIZZA_PORT_ID *pPortPriorityList, 
**                                                    OUT GT_BOOL *p_isPrior, 
**                                                     IN GT_PHYSICAL_PORT_NUM portNum);
**-------------------------------------------------------------------------------------*/
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
);

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
);

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
);


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
);


/*
GT_STATUS PortPizzaCfgSlicesListInit(OUT CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC * pizzaPtr);
GT_STATUS PortPizzaCfgSlicesListCopy(OUT CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC * dstPizzaPtr,
                                      IN CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC * srcPizzaPtr);
*/
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
);

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
);


/*--------------------------------------------------------------------*/
/* PortPizzaCfgSlicesListPool - Allocator                                         */
/*--------------------------------------------------------------------*/

#define PortPizzaCfgSlicesListPoolSIZE_CNS  20
typedef struct PortPizzaCfgSlicesListPool_STC
{
    GT_U32                      portPizzaCfgCur;
    GT_U32                      portPizzaCfgRemains;
    CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC  portPizzaCfgArr[PortPizzaCfgSlicesListPoolSIZE_CNS];
}PortPizzaCfgSlicesListPool_STC;
/*
GT_STATUS PortPizzaCfgSlicesListPoolInit (INOUT PortPizzaCfgSlicesListPool_STC * poolPtr);
GT_STATUS PortPizzaCfgSlicesListPoolAlloc(INOUT PortPizzaCfgSlicesListPool_STC * poolPtr, 
                                            OUT CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC ** pPizzaPtrPtr);
*/

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
);

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
);


/*--------------------------------------------------------------------*/
/* Pizza : Pizza Config for each possible number of slices            */
/*                          port priority graph                       */
/*--------------------------------------------------------------------*/
/*
GT_STATUS PortPizzaCfgSliceListBySlicesNumInit(INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPizzaDstPtr,
                                                  IN GT_U32 portNum,
                                                  IN GT_U32 totalSlices);

GT_STATUS PortPizzaCfgSliceListBySlicesNumAppend(INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPizzaDstPtr, 
                                                    IN GT_U32 sliceNum2Config,
                                                    IN CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC * pizzaPtr);

GT_STATUS PortPizzaCfgSliceListBySlicesNumCopy(INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPizzaDstPtr,
                                                  IN CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPizzaSrcPtr); 

GT_STATUS PortPizzaCfgSliceListBySlicesNumCopy_wAlloc(INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPizzaDstPtr,
                                                         IN CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPizzaSrcPtr,
                                                         IN PortPizzaCfgSlicesListPool_STC * pPoolPtr);

GT_STATUS PortPizzaCfgSliceListBySlicesNumGet(INOUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPizza, 
                                                OUT CPSS_DXCH_PIZZA_SLICE_ID **          ppSliceList,
                                                 IN GT_U32                     portNum,
                                                 IN PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT         sliceNum2Config);
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
);

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
);

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
);

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
);


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
);


/*---------------------------------------------------*/
/* Iterator for */
/*---------------------------------------------------*/
typedef struct PortPizzaCfgSliceNum2SlicesListIter_STC
{
    CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPortPizzaCfgSliceNum2SlicesListPtr;
    SliceNumXPortPizzaCfgSlicesList_STC             * pSliceNumXPortPizzaCfgSlicesListPtr;
}PortPizzaCfgSliceNum2SlicesListIter_STC;

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
    INOUT PortPizzaCfgSliceNum2SlicesListIter_STC         *pIterPtr, 
       IN CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *portPizzaCfgSliceNum2SlicesListPtr
);

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
);

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
);

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
);


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
);



/*---------------------------------------------------------------------------*/
/* List : pizza for specific <devices type, revision, core clock             */
/*---------------------------------------------------------------------------*/

typedef struct DevRevTypeClockPizza_STC
{
    /* key */
    CPSS_PP_FAMILY_TYPE_ENT                         devFamilyType;
    GT_U32                                          devRevision;
    GT_U32                                          devCoreClock;  /* if devClock == 0 does not matter what clock is*/ 
    /* data */
    CPSS_DXCH_PIZZA_PROFILE_STC                             *  pizzaProfilePtr; 
}DevRevTypeClockPizza_STC;

typedef struct ListDevRevTypeClockPizza_STC
{
    GT_U32 size;
    DevRevTypeClockPizza_STC * pArr;
}ListDevRevTypeClockPizza_STC;

/*
** GT_STATUS ListDevRevPizza_Get(INOUT ListDevRevTypeClockPizza_STC       *   pListPtr,
**                                     OUT CPSS_DXCH_PIZZA_PROFILE_STC     **  ppPizzaProfilePtr,
**                                     IN  CPSS_PP_FAMILY_TYPE_ENT  devFamilyType,
**                                     IN  GT_U32                   devRevision,
**                                     IN  GT_U32                   devCoreClock
**                              );
*/

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
);


/*--------------------------------*/
/* default Profiles               */
/*--------------------------------*/
extern const CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC prvPortGroupPortSpeed2SliceNumDefault;
extern const ListDevRevTypeClockPizza_STC    prvListDevRevTypeClock2PizzaDefault;

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif


