/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiter.h
*
* DESCRIPTION:
*       define Pizza Arbiter for Lion2 A0/B0 
*
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/

#ifndef __private_CPSS_DXCH_PORT_PIZZA_ARBITER_H
#define __private_CPSS_DXCH_PORT_PIZZA_ARBITER_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterDevInfo.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/*----------------------------------------------------------------------------------------------*/
/* Table for selection of HW interface function for different devices                            */
/* shall be used in cpss blocks that uses Pizza Arbiter                                         */
/*----------------------------------------------------------------------------------------------*/
typedef struct DEV_REV_TYPE_TO_SLICE_MANIPUL_FUN_STC
{
    CPSS_PP_FAMILY_TYPE_ENT                         devFamilyType;
    GT_U32                                          devRevision;
    /* */
    PRV_CPSS_DXCH_PORT_PIZZA_SLICE_MODIF_FUN_STC                        sliceManipuldFun;
}DEV_REV_TYPE_TO_SLICE_MANIPUL_FUN_STC;

typedef struct LIST_DEV_REV_TYPE_TO_SLICE_MANIPUL_FUN_STCT
{
    GT_U32                           size;
    DEV_REV_TYPE_TO_SLICE_MANIPUL_FUN_STC * pArr;
}LIST_DEV_REV_TYPE_TO_SLICE_MANIPUL_FUN_STC;

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
);

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
**            |  PizzaArbiterDevice  (has several port group)   |
**            |    -PizzaArbiterDevInit                         |
**            |    -PizzaArbiterDevPortSetState                 |               +----------------------------------+
**            |    -PizzaArbiterDevPortGetState                 |-------------->|  Active PortGroup List           |
**            |    -PizzaArbiterDevSliceGetState                |               |     - ActivePortGroupListSearch  |
**            +-------------------------------------------------+               +----------------------------------+
**                     |                       
**                    \|/
**            +-------------------------------------------------+                        
**            |  PizzaArbiterDevPortGroup                       |--+                        
**            |    -PizzaArbiterDevPortGroupInit                |  |            +----------------------------------+         
**            |    -PizzaArbiterDevPortGroupPortSetState        |  |----------->|  PizzaProfile                    |    
**            |    -PizzaArbiterDevPortGroupPortGetState        |  |            |         - GetPizzaByMode         |
**            |    -PizzaArbiterDevPortGroupSliceGetState       |  |            |                                  |
**            +-------------------------------------------------+  |            +----------------------------------+
**                +------------------------------------------------+                   
**                     |                       
**                     |                       
**                     |                       
**                    \|/
**            +-------------------------------------------------+                        
**            |  Slice Manipulation Function                    |
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

/*------------------------------------------------------------------------------------------*/
/* high level interface Pizza interface                                                     */
/*     prvCpssDxChPizzaArbiterDeviceInit()   - init device and all slices                	*/
/*     prvCpssDxChPizzaArbiterPortGetState() - returns number of slices used by port     	*/ 
/*     prvCpssDxChPizzaArbiterPortSetState() - assign to port specified number of slices 	*/
/*     prvCpssDxChPizzaArbiterSliceGetState() - returns port number if is used by some port */ 
/*------------------------------------------------------------------------------------------*/


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
);

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
);


typedef enum 
{
     PA_PORT_ACTION_VERIFY,
     PA_PORT_ACTION_PERFORM
}PA_PORT_ACTION_ENT;

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
    IN PA_PORT_ACTION_ENT action
);

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
);



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif

