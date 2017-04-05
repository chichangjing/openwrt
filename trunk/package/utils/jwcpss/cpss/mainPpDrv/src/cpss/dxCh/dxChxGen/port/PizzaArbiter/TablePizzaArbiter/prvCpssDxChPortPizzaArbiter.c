/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiter.c
*
* DESCRIPTION:
*       Pizza Arbiter implementation
*
* FILE REVISION NUMBER:
*       $Revision: 60 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterTables.h>


/*---------------------------------------------------------------------------*/
/*  selection of slice manipulation function set by devFamily,revision       */                     
/*---------------------------------------------------------------------------*/
/*******************************************************************************
* prvCpssDxChDevRevTypeSliceManipulFunGet
*
* DESCRIPTION:
*       select slice manipulation function set from the table by devFamilyType and revision
* 
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       listPtr         - list of functions' sets.
*       devFamilyType   - device family
*       devRevision     - device reviosion
*
* OUTPUTS:
*       sliceManipulFunPtrPtr - (pointer to ) pointer to slice manipulation function set
* 
* RETURNS:
*       GT_OK.              on sucess
*       GT_BAD_PTR          on bad pointer
*       GT_NOT_SUPPORTED    on suitable set not found
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  prvCpssDxChDevRevTypeSliceManipulFunGet
(
    IN  LIST_DEV_REV_TYPE_TO_SLICE_MANIPUL_FUN_STC *listPtr,
    IN  CPSS_PP_FAMILY_TYPE_ENT  devFamilyType,
    IN  GT_U32                   devRevision,
    OUT PRV_CPSS_DXCH_PORT_PIZZA_SLICE_MODIF_FUN_STC  **sliceManipulFunPtrPtr
)
{
    GT_U32 i;
    DEV_REV_TYPE_TO_SLICE_MANIPUL_FUN_STC * pDevRevTypeSliceEnDisFun;

    if (NULL == listPtr || NULL == sliceManipulFunPtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    pDevRevTypeSliceEnDisFun = listPtr->pArr;
    for (i = 0 ; i < listPtr->size ; i++, pDevRevTypeSliceEnDisFun++)
    {
        if (   pDevRevTypeSliceEnDisFun->devFamilyType == devFamilyType
            && pDevRevTypeSliceEnDisFun->devRevision   == devRevision)
        {
            *sliceManipulFunPtrPtr  = &(pDevRevTypeSliceEnDisFun->sliceManipuldFun);
            return GT_OK;
        }
    }
    *sliceManipulFunPtrPtr = (PRV_CPSS_DXCH_PORT_PIZZA_SLICE_MODIF_FUN_STC *)NULL;
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
}

/*-------------------------------------------------------------------------------------
**
**             +------------------------------------------------+     
**             |  Interface                                     |     knows nothing about 
**             |    -prvCpssDxChPizzaArbiterInit                |          - whether port group is active or not  
**             |    -prvCpssDxChPizzaArbiterPortSetState        |          - How to use pizza profile
**             |    -prvCpssDxChPizzaArbiterPortGetState        |          - How manu units there are in systems
**             |    -prvCpssDxChPizzaArbiterSliceGetState       |     
**             +------------------------------------------------+     
**                     |                       
**                    \|/
**            +-------------------------------------------------+       
**            |  PizzaArbiterDevice                             |
**            |    -PizzaArbiterDevInit                         |
**            |    -PizzaArbiterDevPortSetState                 |               +----------------------------------+
**            |    -PizzaArbiterDevPortGetState                 |-------------->|  Active PortGroup List           |
**            |    -PizzaArbiterDevSliceGetState                |               |     - ActivePortGroupListSearch  |
**            +-------------------------------------------------+               +----------------------------------+
**                     |                       
**                    \|/
**            +-------------------------------------------------+                        
**            |  PizzaArbiterDevPortGroup                       |                        
**            |    -PizzaArbiterDevPortGroupInit                |               +----------------------------------+         
**            |    -PizzaArbiterDevPortGroupPortSetState        |-------------->|  PizzaProfile                    |    
**            |    -PizzaArbiterDevPortGroupPortGetState        |               |         - GetPizzaByMode         |
**            |    -PizzaArbiterDevPortGroupSliceGetState       |               |                                  |
**            |-------------------------------------------------|               +----------------------------------+
**            |    -PizzaArbiterDevPortGroupGetStateAndSlices   |
**            |    -PortPizzaCfgSliceListBySlicesNumGet         |              Algorithm is build around the procedure
**            |    -PizzaPortGroupStateBuildListSlicesToOccupiedAndReleased       -Get port state (i.e. list of all slices it occupies
**            |    -PizzaPortGroupStateVerifySlicesCouldBeOccupied                -Get list of slices to be occupied by Pizza Profile/mode/request
**            |    -PizzaPortGroupStateOccupySlices             |                 -Build 2 new list : 
**            |    -PizzaPortGroupStateReleaseSlices            |                          -list of slices to be occupped that are still not. 
**            +-------------------------------------------------+                          -build list of slices to be release
**                     |                                                          -Check slices to be occupied that are really could be occupied
**                    \|/                                                               i.e. them are not assigned to port with greatest priority
**            +-------------------------------------------------+                 -Occupy slices to be occuppied        
**            |  Slice Manipulation Function                    |                 -Release slieses to be released
**            |    -Init                                        |        
**            |    -SliceOccupy                                 |        
**            |    -SliceRelease                                |---------------------+   
**            |    -SliceGetState                               |                     |
**            |    -PizzaArbiterDevPortGroupSliceGetState       |----+                |
**            +-------------------------------------------------+    |                |
**                     |            |            |          |        |                |
**                    \|/           |           \|/         |       \|/               |
**            +-------------------+ | +-------------------+ | +-------------------+   |
**            |  RxDMA            | | |  RxDMA CTU        | | |  TxDMA            |   |
**            |    -Init          | | |    -Init          | | |    -Init          |   |
**            |    -SlicesNumSet  | | |    -SlicesNumSet  | | |    -SlicesNumSet  |   |
**            |    -SlicesNumGet  | | |    -SlicesNumGet  | | |    -SlicesNumGet  |   |
**            |    -SliceOccupy   | | |    -SliceOccupy   | | |    -SliceOccupy   |   |
**            |    -SliceRelease  | | |    -SliceRelease  | | |    -SliceRelease  |   |
**            |    -SliceGetState | | |    -SliceGetState | | |    -SliceGetState |   |
**            +-------------------+ | +-------------------+ | +-------------------+   |
**                                  |                       |                         |
**                                 \|/                     \|/                       \|/
**                      +-------------------+   +-------------------+   +-------------------+  In Lion2 B0 the number of slices is doubled 
**                      |  TxDMA CTU        |   |  BM               |   |  TxQ              |  against original pizza. But ONLY TxQ knows 
**                      |    -Init          |   |    -Init          |   |    -Init          |  about that !!!
**                      |    -SlicesNumSet  |   |    -SlicesNumSet  |   |    -SlicesNumSet  |
**                      |    -SlicesNumGet  |   |    -SlicesNumGet  |   |    -SlicesNumGet  |
**                      |    -SliceOccupy   |   |    -SliceOccupy   |   |    -SliceOccupy   |
**                      |    -SliceRelease  |   |    -SliceRelease  |   |    -SliceRelease  |
**                      |    -SliceGetState |   |    -SliceGetState |   |    -SliceGetState |
**                      +-------------------+   +-------------------+   +-------------------+
** 
** 
** 
**------------------------------------------------------------------------------------------*/


/*------------------------------------------------------*/
/*  PizzaArbiterDevPortGroup                            */
/*    function :                                        */
/*          Init()                                      */
/*          GetStateAndSlices() - returns list of       */ 
/*                                occupied slices and   */
/*                                port state (number of */
/*                                occupied slices       */
/*          GetState()          - returns port state    */
/*          SetState()            assign slices to port */
/*                                according to mode and */
/*                                pizza                 */
/*          SliceGetState       - returns TRUE if slice */
/*                                is occupying and by   */
/*                                which port            */
/*------------------------------------------------------*/
/*******************************************************************************
* PizzaArbiterDevPortGroupInit
*
* DESCRIPTION:
*     inteface to Pizza Arbiter : 
*            DeviceInit()    init the device pizza and slices on various queues
*            PortGetState()  get port state : how many slices are assigned to port
*            PortSetState()  assign specified number of slices to port according to Pizza
* 
* APPLICABLE DEVICES:
*     Lion2.
*
* NOT APPLICABLE DEVICES:
*     DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*     devNum          - device number
*     portGroupIdx    - port group id
*     pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
* 
* RETURNS:
*     GT_OK.              on sucess
*     GT_NOT_SUPPORTED    if suitable pizza not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevPortGroupInit
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupIdx,
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr
)
{
    GT_STATUS              rc;
    CPSS_DXCH_PIZZA_PROFILE_STC    *  pizzaProfilePtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);


    if (NULL == pizzaDevInfoPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    pizzaProfilePtr = pizzaDevInfoPtr->portGroupPizzaProfile[portGroupIdx];

    rc = pizzaDevInfoPtr->sliceModFun.portGroupSlicesInitFunPtr(devNum,
                                                         portGroupIdx,
                                                         pizzaProfilePtr->maxSliceNum,
                                                         &pizzaProfilePtr->txQDef);
    if (rc != GT_OK) 
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* PizzaArbiterDevPortGroupGetStateAndSlices
*
* DESCRIPTION:
*        pizza arbiter device get port state on specific core on device 
*        and assign list of slices occupied by port
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       portNum         - local port number
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
*       currentlyUsedSliceListPtr - (pointer to )list of slices used by port currently
*       statePtr        - number of slices used by the port
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevPortGroupGetStateAndSlices
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupIdx,
    IN GT_PHYSICAL_PORT_NUM portNum, 
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    OUT CPSS_DXCH_PIZZA_SLICE_ID *currentlyUsedSliceListPtr,
    OUT PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT *statePtr
)
{
    GT_STATUS rc;
    CPSS_DXCH_PIZZA_SLICE_ID sliceIdx;
    GT_BOOL isOccupied;
    GT_PHYSICAL_PORT_NUM  portNumRead;
    GT_U32                slicesN = 0;
    CPSS_DXCH_PIZZA_PROFILE_STC    *  pPizzaProfilePtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == pizzaDevInfoPtr || NULL == currentlyUsedSliceListPtr || NULL == statePtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    pPizzaProfilePtr = pizzaDevInfoPtr->portGroupPizzaProfile[portGroupIdx];

    for (sliceIdx = 0 ; sliceIdx < pPizzaProfilePtr->maxSliceNum; sliceIdx++)
    {
        rc = pizzaDevInfoPtr->sliceModFun.sliceGetStateFunPtr(/*IN*/devNum,portGroupIdx,
                                                           /*IN*/sliceIdx,
                                                           /*OUT*/&isOccupied,&portNumRead);
        if (GT_OK != rc)
        {
            return rc;
        }
        if (isOccupied == GT_TRUE)
        {
            if (portNum == portNumRead)
            {
                currentlyUsedSliceListPtr[slicesN] = sliceIdx;
                slicesN ++;
            }
        }
    }
    currentlyUsedSliceListPtr[slicesN] = CPSS_PA_INVALID_SLICE;
    rc = sliceNum2PortStateStateConvertFromNum(/*IN*/slicesN,/*OUT*/statePtr);
    if (GT_OK != rc)
    {
        return rc;
    }
    return GT_OK;
}



/*******************************************************************************
* PizzaArbiterDevPortGroupPortGetState
*
* DESCRIPTION:
*        pizza arbiter device get port state on specific core on device 
*        (how many slices are occupied by port)
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       portNum         - local port number
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
*       statePtr        - number of slices used by the port
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevPortGroupPortGetState
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupIdx,
    IN GT_PHYSICAL_PORT_NUM portNum, 
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    OUT PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT *statePtr
)
{
    GT_STATUS rc;
    GT_U32 sliceIdx;
    GT_BOOL isOccupied;
    GT_PHYSICAL_PORT_NUM  portNumRead;
    GT_U32                slicesN = 0;
    CPSS_DXCH_PIZZA_PROFILE_STC    *  pPizzaProfilePtr;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == pizzaDevInfoPtr || NULL == statePtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    pPizzaProfilePtr = pizzaDevInfoPtr->portGroupPizzaProfile[portGroupIdx];

    for (sliceIdx = 0 ; sliceIdx < pPizzaProfilePtr->maxSliceNum; sliceIdx++)
    {
        rc = pizzaDevInfoPtr->sliceModFun.sliceGetStateFunPtr(/*IN*/devNum,portGroupIdx,sliceIdx,/*OUT*/&isOccupied,&portNumRead);
        if (GT_OK != rc)
        {
            return rc;
        }
        if (isOccupied == GT_TRUE)
        {
            if (portNum == portNumRead)
            {
                slicesN ++;
            }
        }
    }
    rc = sliceNum2PortStateStateConvertFromNum(/*IN*/slicesN,/*OUT*/statePtr);
    if (GT_OK != rc)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* PizzaPortGroupStateVerifySlicesCouldBeOccupied
*
* DESCRIPTION:
*        verify whether slices from the list could be occupy (assigned them to specific port)
*        if them belongs to other port check at port priority matrix, whether the given port
*        can take over slices of other port
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       portNum         - port to which slices are assigned
*       sliceListPtr    - list of slices
*       portPriorityMatrPtr - port priority matrix
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*       GT_FAIL       on the slice cann't be occupied
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaPortGroupStateVerifySlicesCouldBeOccupied
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupIdx,
    IN GT_PHYSICAL_PORT_NUM portNum, 
    IN CPSS_DXCH_PIZZA_SLICE_ID              *sliceListPtr,
    IN CPSS_DXCH_PORT_PRIORITY_MATR_STC      *portPriorityMatrPtr,
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr
)
{
    GT_BOOL   isPrior;
    GT_STATUS rc;
    GT_U32    sliceIdx;
    GT_U32    i;
    GT_PHYSICAL_PORT_NUM    portNum2Check;
    CPSS_DXCH_PIZZA_PORT_ID      * pPortPriorityList;
    GT_BOOL  isOccuppied;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == sliceListPtr || portPriorityMatrPtr == NULL  || NULL == pizzaDevInfoPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = portPrioritiesMatrixPortPriorityListGet(portPriorityMatrPtr, /*IN*/portNum,/*OUT*/&pPortPriorityList);
    if (GT_OK != rc )
    {
        return rc;
    }

    for (i = 0 ; sliceListPtr [i] != CPSS_PA_INVALID_SLICE ; i++)
    {
        sliceIdx = sliceListPtr [i];
        pizzaDevInfoPtr->sliceModFun.sliceGetStateFunPtr(devNum,portGroupIdx,sliceIdx,&isOccuppied,&portNum2Check);
        if (GT_TRUE == isOccuppied)
        {
            rc = portPrioritiesListIsPriorTo(pPortPriorityList,/*IN*/portNum2Check,/*OUT*/&isPrior);
            if (GT_OK != rc )
            {
                return rc;
            }
            if (isPrior == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }
    }
    return GT_OK;
}


/*******************************************************************************
* PizzaPortGroupStateOccupySlices
*
* DESCRIPTION:
*        occupy slices from the list (assign them to specific port)
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       portNum         - port to which slices are assigned
*       sliceListPtr    - list of slices
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaPortGroupStateOccupySlices
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupIdx,
    IN GT_PHYSICAL_PORT_NUM portNum, 
    IN CPSS_DXCH_PIZZA_SLICE_ID              *sliceListPtr,
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr
)
{
    GT_STATUS rc;
    GT_U32    sliceIdx;
    GT_U32    i;
    CPSS_DXCH_PIZZA_PROFILE_STC    *  pizzaProfilePtr;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == sliceListPtr || NULL == pizzaDevInfoPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }


    pizzaProfilePtr = pizzaDevInfoPtr->portGroupPizzaProfile[portGroupIdx];

    for (i = 0 ; sliceListPtr [i] != CPSS_PA_INVALID_SLICE ; i++)
    {
        sliceIdx = sliceListPtr[i];
        rc = (*pizzaDevInfoPtr->sliceModFun.sliceOccupyFunPtr)(devNum, portGroupIdx, sliceIdx,portNum,
                                                             pizzaProfilePtr->maxSliceNum, 
                                                             &pizzaProfilePtr->txQDef);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* PizzaPortGroupStateReleaseSlices
*
* DESCRIPTION:
*        release slices from the list
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       sliceListPtr    - list of slices
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaPortGroupStateReleaseSlices
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupIdx,
    IN CPSS_DXCH_PIZZA_SLICE_ID              *sliceListPtr,
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr
)
{
    GT_STATUS rc;
    GT_U32    sliceIdx;
    GT_U32    i;
    CPSS_DXCH_PIZZA_PROFILE_STC *  pizzaProfilePtr;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == sliceListPtr || NULL == pizzaDevInfoPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }


    pizzaProfilePtr = pizzaDevInfoPtr->portGroupPizzaProfile[portGroupIdx];

    for (i = 0 ; sliceListPtr [i] != CPSS_PA_INVALID_SLICE ; i++)
    {
        sliceIdx = sliceListPtr[i];
        rc = (*pizzaDevInfoPtr->sliceModFun.sliceReleaseFunPtr)(devNum,portGroupIdx,sliceIdx,
                                                              pizzaProfilePtr->maxSliceNum, 
                                                              &pizzaProfilePtr->txQDef);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*----------------------------------------------------*/
/* By 2 lists to 1. List of currently Used Slices     */
/*               2. List of slice to be used          */
/*    build two new list                              */
/*        list of slices to be occupied               */
/*        list of slices to be released               */
/*    actuallty build simmetrical difference of       */
/*           2 input lists                            */
/*----------------------------------------------------*/
/*******************************************************************************
* PizzaPortGroupStateBuildListSlicesToOccupiedAndReleased
*
* DESCRIPTION:
*        build list if slices to be occupied and to be released 
*        from list of currently used slices and slices to be used 
*        actuallty build simmetrical difference of 2 input lists                            
*        Slices that are common at two list already used. They shall be 
*        not touched. (simmetrical difference)
* 
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       sliceListToBeUsedPtr      - (pointer to) list of slices to be used
*       currentlyUsedSliceListPtr - (pointer to) list of slices currently used.
*
* OUTPUTS:
*       sliceListToBeOccupiedPtr  - (pointer to) list of slices to be occupied
*       sliceListToBeReleasedPtr  - (pointer to) list of slices to be released
* 
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID PizzaPortGroupStateBuildListSlicesToOccupiedAndReleased
(
      IN CPSS_DXCH_PIZZA_SLICE_ID *sliceListToBeUsedPtr,
      IN CPSS_DXCH_PIZZA_SLICE_ID *currentlyUsedSliceListPtr,
     OUT CPSS_DXCH_PIZZA_SLICE_ID *sliceListToBeOccupiedPtr,
     OUT CPSS_DXCH_PIZZA_SLICE_ID *sliceListToBeReleasedPtr
)
{
    CPSS_DXCH_PIZZA_SLICE_ID dummyList = CPSS_PA_INVALID_SLICE;
    GT_U32 indToBeUsed;
    GT_U32 indCurrentlyUsed;
    GT_U32 indToBeOccupied;
    GT_U32 indToBeReleased;
    CPSS_DXCH_PIZZA_SLICE_ID sliceToBeUsed;
    CPSS_DXCH_PIZZA_SLICE_ID sliceCurrentlyUsed;

    if (NULL == currentlyUsedSliceListPtr)
    {
        currentlyUsedSliceListPtr = &dummyList;
    }
    if (NULL == sliceListToBeUsedPtr)
    {
        sliceListToBeUsedPtr = &dummyList;
    }

    /* I suppose that SliceListtoBeUsed and pCurrentlyUsedSliceList are sorted in increasing order !!!! */
    /* use merge sort algorithm for build */
    indToBeUsed = 0;
    indCurrentlyUsed = 0;
    indToBeOccupied  = 0;
    indToBeReleased  = 0;

    sliceToBeUsed       = sliceListToBeUsedPtr[indToBeUsed];
    sliceCurrentlyUsed  = currentlyUsedSliceListPtr[indCurrentlyUsed];

    while (sliceToBeUsed != CPSS_PA_INVALID_SLICE && sliceCurrentlyUsed != CPSS_PA_INVALID_SLICE)
    {
        if (sliceToBeUsed == sliceCurrentlyUsed)   /* shall be used and already used , do nothing */
        {
            indCurrentlyUsed++;
            indToBeUsed++;
            sliceToBeUsed       = sliceListToBeUsedPtr[indToBeUsed];
            sliceCurrentlyUsed  = currentlyUsedSliceListPtr[indCurrentlyUsed];
            continue;
        }
        if (sliceToBeUsed < sliceCurrentlyUsed)   /* shall be used and is not in use, add to the list of be occupied */
        {
            sliceListToBeOccupiedPtr[indToBeOccupied] = sliceToBeUsed;
            indToBeOccupied++;
            indToBeUsed++;
            sliceToBeUsed  = sliceListToBeUsedPtr[indToBeUsed];
            continue;
        }
        /* sliceToBeUsed > sliceCurrentlyUsed */ /* currently used slice, but shall be released */
        sliceListToBeReleasedPtr[indToBeReleased] = sliceCurrentlyUsed;
        indToBeReleased++;
        indCurrentlyUsed++;
        sliceCurrentlyUsed  = currentlyUsedSliceListPtr[indCurrentlyUsed];
    }
    if (sliceToBeUsed != CPSS_PA_INVALID_SLICE)  /*  currentlyUsedList is ended --> append all slotes to be occupied */
    {
        while (sliceToBeUsed != CPSS_PA_INVALID_SLICE)
        {
            sliceListToBeOccupiedPtr[indToBeOccupied] = sliceToBeUsed;
            indToBeOccupied++;
            indToBeUsed++;
            sliceToBeUsed  = sliceListToBeUsedPtr[indToBeUsed];
        }
    }
    else /* sliceCurrentlyUsed != INVALIDE_SLICE */ /* to be used List is endded --> all remaining currently used append to be released */
    {
        while(sliceCurrentlyUsed != CPSS_PA_INVALID_SLICE)
        {
            sliceListToBeReleasedPtr[indToBeReleased] = sliceCurrentlyUsed;
            indToBeReleased++;
            indCurrentlyUsed++;
            sliceCurrentlyUsed  = currentlyUsedSliceListPtr[indCurrentlyUsed];
        }
    }
    sliceListToBeOccupiedPtr[indToBeOccupied] = CPSS_PA_INVALID_SLICE;
    sliceListToBeReleasedPtr[indToBeReleased] = CPSS_PA_INVALID_SLICE;
}

/*----------------------------------------------------*/
/* returs regular mode Pizza or extended mode pizza   */
/* depending on isExtModeEanble parameter             */
/*----------------------------------------------------*/
/*******************************************************************************
* PizzaArbiterDevPortGroupGetPizzaByMode
*
* DESCRIPTION:
*        select pizza from profile 
*        (assign several slices to port according to profile)
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       pizzaProfilePtr - (pointer to) profile 
*       isExtModeEnable - is extended mode (GT_TRUE) or regular mode (GT_TRUE) 
*
* OUTPUTS:
*       pizzaPtrPtr     - (pointer to pointer to ) pizza
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*       GT_NOT_SUPPORTED if pizza that mode does not exists
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevPortGroupGetPizzaByMode
(
    IN  CPSS_DXCH_PIZZA_PROFILE_STC    *pizzaProfilePtr,
    IN  GT_BOOL                         isExtModeEnable,
    OUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC **pizzaPtrPtr
)
{
    CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pizzaPtr;

    if (NULL == pizzaProfilePtr || NULL == pizzaPtrPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    *pizzaPtrPtr = (CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)NULL;
    if (GT_TRUE == isExtModeEnable) /* Extended Mode set !!! */
    {
        pizzaPtr = (CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)(pizzaProfilePtr->extModePizzaPtr);
    }
    else
    {
        pizzaPtr = (CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)(pizzaProfilePtr->regModePizzaPtr);
    }
    if (pizzaPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    *pizzaPtrPtr = pizzaPtr;
    return GT_OK;
}


/*******************************************************************************
* PizzaArbiterDevPortGroupPortSetState
*
* DESCRIPTION:
*        pizza arbiter device set port state on specific core on device 
*        (assign several slices to port according to profile)
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       portIdx         - local port number
*       isExtModeEnable - is extended mode (GT_TRUE) or regular(GT_FALSE) mode used
*       state           - number of slices to be assigned for the port
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*       action          - Verify feasibility / perform 
*
* OUTPUTS:
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*       GT_BAD_PARAM  bad core
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevPortGroupPortSetState
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupIdx,
    IN GT_PHYSICAL_PORT_NUM portIdx, 
    IN GT_BOOL isExtModeEnable,
    IN PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT state, 
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    IN PA_PORT_ACTION_ENT   action
)
{
    GT_STATUS rc;
    CPSS_DXCH_PIZZA_PROFILE_STC    *  pPizzaProfilePtr;

    CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPizza;

    CPSS_DXCH_PIZZA_SLICE_ID *   pSliceListToBeUsed;
    CPSS_DXCH_PIZZA_SLICE_ID *  pCurrentlyUsedSliceList;
    CPSS_DXCH_PORT_PIZZA_CFG_SLICE_LIST_STC currentlyUsedSliceList;

    CPSS_DXCH_PORT_PIZZA_CFG_SLICE_LIST_STC sliceListToBeReleased;
    CPSS_DXCH_PORT_PIZZA_CFG_SLICE_LIST_STC sliceListToBeOccupied;

    PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT  curPortState;

 
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);


    if (NULL == pizzaDevInfoPtr )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (action != PA_PORT_ACTION_VERIFY && action != PA_PORT_ACTION_PERFORM)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get the pizza definition by device parameters */
    pPizzaProfilePtr = pizzaDevInfoPtr->portGroupPizzaProfile[portGroupIdx];

    if (portIdx >= pPizzaProfilePtr->portNum) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get the specific pizza from pizza definition by mode (regular/extended) */
    rc = PizzaArbiterDevPortGroupGetPizzaByMode(/*IN*/pPizzaProfilePtr,/*IN*/isExtModeEnable, /*OUT*/&pPizza);
    if (GT_OK != rc )
    {
        return rc;
    }

    /* get list of currently used slices -- collect from slices state */
    pCurrentlyUsedSliceList = &currentlyUsedSliceList[0];
    rc = PizzaArbiterDevPortGroupGetStateAndSlices(/*IN*/devNum, 
                                                   /*IN*/portGroupIdx,
                                                   /*IN*/portIdx, 
                                                   /*IN*/pizzaDevInfoPtr,
                                                   /*OUT*/pCurrentlyUsedSliceList,
                                                   /*OUT*/&curPortState);
    if (GT_OK != rc )
    {
        return rc;
    }

    /* get list of slices to be used by state : external input*/
    rc = portPizzaCfgSliceListBySlicesNumGet(pPizza,
                                                /*OUT*/&pSliceListToBeUsed,
                                                /*IN*/portIdx,
                                                /*IN*/state);
    if (GT_OK != rc )
    {
        return rc;
    }

    if (state != PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT_NONE_CNS)
    {
        if (pSliceListToBeUsed[0] == CPSS_PA_INVALID_SLICE)  /* empty list !!! , not supported configuration */
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
    }

    PizzaPortGroupStateBuildListSlicesToOccupiedAndReleased( /*IN*/pSliceListToBeUsed,
                                                             /*IN*/pCurrentlyUsedSliceList,
                                                             /*OUT*/&sliceListToBeOccupied[0],
                                                             /*OUT*/&sliceListToBeReleased[0]);

    rc = PizzaPortGroupStateVerifySlicesCouldBeOccupied(/*IN*/devNum,
                                                         /*IN*/portGroupIdx,
                                                         /*IN*/portIdx, 
                                                         /*IN*/&sliceListToBeOccupied[0],
                                                         /*IN*/pPizzaProfilePtr->pPortPriorityMatrPtr,
                                                         /*IN*/pizzaDevInfoPtr);
    if (GT_OK != rc )  /* cann't configure : slices are occupied already by some other port with higher priority */
    {
        return rc;
    }

    if (action == PA_PORT_ACTION_VERIFY)
    {
        return GT_OK;
    }

    rc = PizzaPortGroupStateOccupySlices(/*IN*/devNum,
                                          /*IN*/portGroupIdx,
                                          /*IN*/portIdx, 
                                          /*IN*/&sliceListToBeOccupied[0],
                                          /*IN*/pizzaDevInfoPtr);
    if (GT_OK != rc )
    {
        return rc;
    }
    rc = PizzaPortGroupStateReleaseSlices(/*IN*/devNum,
                                           /*IN*/portGroupIdx,
                                           /*IN*/&sliceListToBeReleased[0],
                                           /*IN*/pizzaDevInfoPtr);
    if (GT_OK != rc )
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* PizzaArbiterDevPortGroupSliceGetState
*
* DESCRIPTION:
*        pizza arbiter device get slice state for specidic core on device 
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       sliceID         - slice id
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
*       isOccupiedPtr   - (pointer to) is slice occupied
*       portNumPtr      - (pointer to) to which port it is assigned
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*       GT_BAD_PARAM  bad core
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevPortGroupSliceGetState
(
    IN  GT_U8 devNum,       
    IN  GT_U32 portGroupIdx,
    IN  GT_U32 sliceID, 
    IN  PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    OUT GT_BOOL              *isOccupiedPtr, 
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
)
{
    GT_STATUS rc;


    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == pizzaDevInfoPtr|| NULL == isOccupiedPtr || NULL == portNumPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    /* The control over sliceNum remains for HW functions since number of slices in HW can be bigger than that
       configured by Pizza Arbiter */
    /*
    if (sliceID >= pGroupPort->totalSlices)
    {
        return GT_BAD_PARAM;
    }
    */
    rc = pizzaDevInfoPtr->sliceModFun.sliceGetStateFunPtr(/*IN*/devNum,
                                                          /*IN*/portGroupIdx,
                                                          /*IN*/sliceID,
                                                          /*OUT*/isOccupiedPtr, portNumPtr);
    return rc;
}



/*------------------------------------------------------------------------------*/
/* device as set of portGroups                                                  */
/*------------------------------------------------------------------------------*/
/*******************************************************************************
* PizzaArbiterDevInit
*
* DESCRIPTION:
*        init pizza arbiter device (init all cores on the device)
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*       GT_BAD_PARAM  bad core
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevInit  
(
    IN GT_U8 devNum,       
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr
)
{
    GT_U32 i;
    GT_STATUS rc;
    GT_U32 portGroupIdx;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == pizzaDevInfoPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    for (i = 0 ; i < pizzaDevInfoPtr->activePortGroupList.size; i++) 
    {
        portGroupIdx = pizzaDevInfoPtr->activePortGroupList.list[i];
        rc = PizzaArbiterDevPortGroupInit(devNum,portGroupIdx,pizzaDevInfoPtr);
        if (rc != GT_OK) 
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* PizzaArbiterDevSetState
*
* DESCRIPTION:
*        pizza arbiter device get port state (how many slices are assigned to port according to profile)
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       portNum         - local port number
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
*       statePtr        - number of slices assigned for the port
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*       GT_BAD_PARAM  bad core
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevGetState
(
    IN  GT_U8 devNum,       
    IN  GT_U32 portGroupIdx, 
    IN  GT_PHYSICAL_PORT_NUM portNum, 
    IN  PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    OUT PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT *statePtr
)
{
    GT_STATUS rc;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == pizzaDevInfoPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (portGroupIdx >= pizzaDevInfoPtr->totalPortGroups)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = PizzaArbiterDevPortGroupPortGetState(/*IN*/devNum,
                                              /*IN*/portGroupIdx,
                                              /*IN*/portNum,
                                              /*IN*/pizzaDevInfoPtr,
                                              /*OUT*/statePtr);
    return rc;
}



/*******************************************************************************
* PizzaArbiterDevSetState
*
* DESCRIPTION:
*        pizza arbiter device set port state on specific core on device 
*        (assign several slices to port according to profile)
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       portNum         - local port number
*       isExtModeEnable - is extended mode (GT_TRUE) or regular(GT_FALSE) mode used
*       state           - number of slices to be assigned for the port
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*       action          - perform assigment or verify
*
* OUTPUTS:
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*       GT_BAD_PARAM  bad core
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevSetState
(
    IN GT_U8 devNum,       
    IN GT_U32 portGroupIdx, 
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL isExtModeEnable,
    IN PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT state,
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    IN PA_PORT_ACTION_ENT   action
)
{
    GT_STATUS rc;
    GT_BOOL   isPortGroupEnable;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);


    if (NULL == pizzaDevInfoPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (portGroupIdx >= pizzaDevInfoPtr->totalPortGroups)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = ActivePortGroupListSearch(&pizzaDevInfoPtr->activePortGroupList,portGroupIdx,/*OUT*/&isPortGroupEnable);
    if (rc != GT_OK) 
    {
        return rc;
    }
    if (isPortGroupEnable == GT_FALSE) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = PizzaArbiterDevPortGroupPortSetState(/*IN*/devNum,
                                              /*IN*/portGroupIdx,
                                              /*IN*/portNum,
                                              /*IN*/isExtModeEnable,
                                              /*IN*/state,
                                              /*IN*/pizzaDevInfoPtr,
                                              /*IN*/action);
    if (rc != GT_OK) 
    {
        return rc;
    }
    return rc;
}

/*******************************************************************************
* PizzaArbiterDevSliceGetState
*
* DESCRIPTION:
*        pizza arbiter device get slice state for specidic on device 
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       sliceID         - slice id
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
*       isOccupiedPtr   - (pointer to) is slice occupied
*       portNumPtr      - (pointer to) to which port it is assigned
* 
* RETURNS:
*       GT_OK.        on sucess
*       GT_BAD_PTR    on bad pointer
*       GT_BAD_PARAM  bad core
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS PizzaArbiterDevSliceGetState
(
    IN GT_U8 devNum,       
    IN GT_U32 portGroupIdx, 
    IN GT_U32 sliceID, 
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    OUT GT_BOOL               *isOccupiedPtr, 
    OUT GT_PHYSICAL_PORT_NUM  *portNumPtr
)
{
    GT_STATUS rc;
    GT_BOOL isPortGroupEnable;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == pizzaDevInfoPtr|| NULL == isOccupiedPtr || NULL == portNumPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = ActivePortGroupListSearch(&pizzaDevInfoPtr->activePortGroupList,portGroupIdx,/*OUT*/&isPortGroupEnable);
    if (rc != GT_OK) 
    {
        return rc;
    }
    if (isPortGroupEnable == GT_FALSE) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = PizzaArbiterDevPortGroupSliceGetState(/*IN*/devNum,       
                                               /*IN*/portGroupIdx,
                                               /*IN*/sliceID, 
                                               /*IN*/pizzaDevInfoPtr,
                                               /*OUT*/isOccupiedPtr,portNumPtr);
    if (rc != GT_OK) 
    {
        return rc;
    }
    return rc;
}


/*******************************************************************************
* prvCpssDxChPizzaArbiterDeviceInit
*
* DESCRIPTION:
*       inteface to Pizza Arbiter : 
*            DeviceInit()    init the device pizza and slices on various queues
*            PortGetState()  get port state : how many slices are assigned to port
*            PortSetState()  assign specified number of slices to port according to Pizza
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
* 
* RETURNS:
*       GT_OK.          - on suceess
*       GT_BAD_PTR      - on bad pointer
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPizzaArbiterDeviceInit  
(
    IN GT_U8 devNum,       
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (pizzaDevInfoPtr == NULL) 
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = PizzaArbiterDevInit(devNum,pizzaDevInfoPtr);
    if (rc != GT_OK) 
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPizzaArbiterPortGetState
*
* DESCRIPTION:
*      PortGetState()  get port state : how many slices are assigned to port
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       portNum         - local port number (inside the port group)
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
*       sliceNumUsedPtr - sliceNumber atored at  address pSliceNumUsed
*
* RETURNS:
*       GT_OK.              operation finished successfully
*       GT_BAD_PTR          pizzaDevInfoPtr == NULL or pSliceNumUsed == NULL
*       GT_NOT_SUPPORTED    device not supprted
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPizzaArbiterPortGetState 
(
    IN  GT_U8 devNum, 
    IN  GT_U32 portGroupIdx, 
    IN  GT_PHYSICAL_PORT_NUM portNum, 
    IN  PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    OUT GT_U32 *sliceNumUsedPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT state;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (sliceNumUsedPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (pizzaDevInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = PizzaArbiterDevGetState(/*IN*/devNum, 
                                 /*IN*/portGroupIdx, 
                                 /*IN*/portNum,
                                 /*IN*/pizzaDevInfoPtr,/*OUT*/&state);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = sliceNum2PortStateStateConvert2Num(/*OUT*/sliceNumUsedPtr, /*IN*/state);
    return rc;
}


/*******************************************************************************
* prvCpssDxChPizzaArbiterPortSetState
*
* DESCRIPTION:
*            PortSetState()  set the port state : assign slices to port 
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       portNum         - local port number (inside the port group)
*       sliceNum2Use    - number of slice to 
*       isExtModeEnable - use regular mode pizza if FALSE; use extended mode pizza if TRUE
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*       action          - perform assignment or check feasibility
*
* OUTPUTS:
*
* RETURNS:
*       GT_BAD_PTR          if pizzaDevInfoPtr is NULL
*       GT_NOT_SUPPORTED    if suitable pizza not found, slices nnumber to be assigned not supported for this port 
*       GT_OK.              if operation finished by success.
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPizzaArbiterPortSetState 
(
    IN GT_U8 devNum, 
    IN GT_U32 portGroupIdx, 
    IN GT_PHYSICAL_PORT_NUM portNum, 
    IN GT_BOOL isExtModeEnable,
    IN GT_U32  sliceNum2Use,
    IN PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    IN PA_PORT_ACTION_ENT  action
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT state;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (pizzaDevInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    if (action != PA_PORT_ACTION_VERIFY && action != PA_PORT_ACTION_PERFORM)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    rc = sliceNum2PortStateStateConvertFromNum(/*IN*/sliceNum2Use,/*OUT*/&state);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = PizzaArbiterDevSetState(/*IN*/devNum, 
                                 /*IN*/portGroupIdx, 
                                 /*IN*/portNum,
                                 /*IN*/isExtModeEnable,/*IN*/state,pizzaDevInfoPtr,
                                 /*IN*/action);
    if (GT_OK != rc)
    {
        return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPizzaArbiterSliceGetState
*            
* DESCRIPTION:
*        get state of specific slice (behind lies assumption : 
*        the pizza is equal for each unit, i.e. 
*        PizzaArbiter knows nothing about the differences between units 
*        Differences appear at the layer Slice Manipualtion function 
* 
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portGroupIdx    - port group id
*       sliceID         - slice id which state to obtain
*       pizzaDevInfoPtr - device info required for Pizza Arbiter Operation
*
* OUTPUTS:
*       isOccupiedPtr : is slice occupied
*       portNumPtr    : if the slice is occupied , by which port it is occupied
* RETURNS:
*       GT_OK.              if suitable pizza found 
        GT_BAD_PTR          if any pointer is NULL
*       GT_NOT_SUPPORTED    if suitable pizza not found
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPizzaArbiterSliceGetState
(
    IN  GT_U8 devNum, 
    IN  GT_U32 portGroupIdx, 
    IN  GT_U32 sliceID, 
    IN  PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr,
    OUT GT_BOOL              *isOccupiedPtr, 
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E 
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E 
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    if (NULL == isOccupiedPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (NULL == portNumPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }
    if (pizzaDevInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    rc = PizzaArbiterDevSliceGetState(/*IN*/devNum, 
                                   /*IN*/portGroupIdx, 
                                   /*IN*/sliceID, 
                                   /*IN*/pizzaDevInfoPtr,
                                   /*OUT*/isOccupiedPtr, portNumPtr);
    return rc;
}

