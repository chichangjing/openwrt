/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiterDevInfo.h
*
* DESCRIPTION:
*       dev info need by Pizza Arbiter 
*
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_PORT_PIZZA_ARBITER_DEV_INFO_H
#define __PRV_CPSS_DXCH_PORT_PIZZA_ARBITER_DEV_INFO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterActiverPortGroupList.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiterProfile.h>

/*-------------------------------------------------------
**  HW interface definition
**-------------------------------------------------------
** any function shall return GT_OK in case when if all parameters (namely devNum , sliceNum, portNum or pointers) are correct.
** hwFUN_devSpecific_SliceGetState : 
**             if slice is enabled *p_isOccupied equal to GT_TRUE,    *p_portNum contains portNum ocuppied the slice
**                otherwise         *p_isOccupied equal to GT_FALSE , *p_portNum is undefined 
**                                                              (it is desired that it contains INVALIDE_PORT 0xFFFF) 
*/

/*******************************************************************************
* PRV_CPSS_DXCH_PORT_PIZZA_DEV_PORTGROUP_SLICES_INIT_FUNC
*
* DESCRIPTION:
*      pointer to function that Init all arbiter' unit 
*      (sets number of slice to use , strict priority to CPU bit
*      and disables all slices of the device.)
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*       portGroupId   - port group id
*       slicesNumberPerGop - number of slices to be used
*       txQDefPtr     - tx pizza definiton (repetition counter how many times the pizza shall be repeated
*                                 for port group with CPU connected)
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_BAD_PTR      - on NULL pointer
*       GT_OUT_OF_RANGE - sliceNum2Init is greater than available
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (* PRV_CPSS_DXCH_PORT_PIZZA_DEV_PORTGROUP_SLICES_INIT_FUNC)
(
    IN GT_U8  devNum,
	IN GT_U32 portGroupId,
	IN GT_U32 slicesNumberPerGop,
	IN CPSS_DXCH_TXQDEF_STC * txQDefPtr
);


/*******************************************************************************
* PRV_CPSS_DXCH_PORT_PIZZA_DEV_SLICE_OCCUPY_FUNC
*
* DESCRIPTION:
*      The function occupy the slice (i.e. set the spicific slice 
*      be asigned to specific port for all pizza arbiter units
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       sliceId     - slice to be occupied
*       localPort   - local port id (inside portGroup) to which slice is assigned
*       slicesNumberPerGop - slice number to be configured
*       txQDefPtr   - txQ definition 
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_BAD_PTR      - on NULL pointer
*       GT_OUT_OF_RANGE - sliceId or portId are greater than available
*       GT_ABORT        - on verification error
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (* PRV_CPSS_DXCH_PORT_PIZZA_DEV_SLICE_OCCUPY_FUNC)
(
    IN GT_U8 devNum, 
	IN GT_U32 portGroupIdx, 
	IN GT_U32 sliceID,
	IN GT_PHYSICAL_PORT_NUM localPortNum, 
	IN GT_U32 slicesNumberPerGop,
	IN CPSS_DXCH_TXQDEF_STC * txQDefPtr
);


/*******************************************************************************
* PRV_CPSS_DXCH_PORT_PIZZA_DEV_SLICE_RELEASE_FUNC
*
* DESCRIPTION:
*      The function release the slice (i.e. set the spicific slice 
*      to be disable to any port) for all Pizza Arbiter units)
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       sliceId     - slice to be occupied
*       localPort   - local port id (inside portGroup) to which slice is assigned
*       slicesNumberPerGop - slice number to be configured
*       txQDefPtr   - txQ definition 
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_BAD_PTR      - on NULL pointer
*       GT_OUT_OF_RANGE - sliceId or portId are greater than available
*       GT_ABORT        - on verification error
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (* PRV_CPSS_DXCH_PORT_PIZZA_DEV_SLICE_RELEASE_FUNC) 
(
    IN GT_U8 devNum, 
	IN GT_U32 portGroupIdx, 
	IN GT_U32 sliceID, 
	IN GT_U32 slicesNumberPerGop,
	IN CPSS_DXCH_TXQDEF_STC * txQDefPtr
);

/*******************************************************************************
* PRV_CPSS_DXCH_PORT_PIZZA_DEV_SLICE_STATE_GET_FUNC
*
* DESCRIPTION:
*      The function gets the state of the slice (i.e. if slace is disable or enable
*      and if it enbaled to which port it is assigned) for BM unit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       sliceId     - slice to be released
*       slicesNumberPerGop - slice number to be configured
*       txQDefPtr   - txQ definition 
*
* OUTPUTS:
*       isOccupiedPtr - place to store whether slice is occupied
*        portNumPtr   - plavce to store to which port slice is assigned
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_OUT_OF_RANGE - sliceId are greater than available
*       GT_BAD_PTR      - isOccupiedPtr or portNumPtr are NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (* PRV_CPSS_DXCH_PORT_PIZZA_DEV_SLICE_STATE_GET_FUNC)
(
    IN   GT_U8 devNum, 
	IN   GT_U32 portGroupIdx, 
	IN   GT_U32 sliceID, 
	OUT  GT_BOOL * p_isOccupied, 
	OUT  GT_PHYSICAL_PORT_NUM * p_portNum
);

/*----------------------------------------------------------------------------------------------*/
/* 4 function  for HW inteface                                                                  */
/*    init all slices            portGroupSlicesInitFunPtr()                                    */
/*    slice enable               sliceEnableFunPtr()                                            */
/*    slice disable              sliceDisableFunPtr()                                           */
/*    slice get state            sliceGetStateFunPtr()                                          */
/*----------------------------------------------------------------------------------------------*/
typedef struct PRV_CPSS_DXCH_PORT_PIZZA_SLICE_MODIF_FUN_STCT
{
	PRV_CPSS_DXCH_PORT_PIZZA_DEV_PORTGROUP_SLICES_INIT_FUNC   portGroupSlicesInitFunPtr;
    PRV_CPSS_DXCH_PORT_PIZZA_DEV_SLICE_OCCUPY_FUNC            sliceOccupyFunPtr;
    PRV_CPSS_DXCH_PORT_PIZZA_DEV_SLICE_RELEASE_FUNC           sliceReleaseFunPtr;
    PRV_CPSS_DXCH_PORT_PIZZA_DEV_SLICE_STATE_GET_FUNC         sliceGetStateFunPtr;
}PRV_CPSS_DXCH_PORT_PIZZA_SLICE_MODIF_FUN_STC;


/*******************************************************************************
* PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC 
*
* DESCRIPTION:
*       Device specific information related to Pizza Arbiter operation
*
* FIELDS:
*       devFamilyType    	- device family Type 
*       devRevision      	- device Revision
*       devCoreClock     	- core clock in MHz
*       totalPortGroups  	- total port groups on device
*       activePortGroupList - list of active port groups
*       sliceModFun         - device specific slice modification functions
*             -- Init()
*             -- SliceEnable()
*             -- SliceDisable()
*             -- SliceGetState()
*
*
*******************************************************************************/

typedef struct PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STCT
{
    CPSS_PP_FAMILY_TYPE_ENT  devFamilyType;
    GT_U32                   devRevision;
    GT_U32                   devCoreClock;
	GT_U32                   totalPortGroups;
	CPSS_DXCH_PIZZA_PROFILE_STC *                portGroupPizzaProfile[PRV_CPSS_GEN_PP_MAX_PORTS_IN_PORT_GROUP_CNS];  /* 0 - default profile*/
	PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC     activePortGroupList;
    PRV_CPSS_DXCH_PORT_PIZZA_SLICE_MODIF_FUN_STC sliceModFun;
}PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC;


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfDevInfoBuild
*
* DESCRIPTION:
*       build relevant dev info from central data base 
*
* APPLICABLE DEVICES:
*       Lion2  Bobcat2
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - dev number
*
* OUTPUTS:
*       pizzaDevInfoPtr - pointer to dev info structure
*
* RETURNS:
*       	GT_BAD_PTR  - on pizzaDevInfoPtr is zero
*           GT_OK       - on success          
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfDevInfoBuild
(
    IN    GT_U8 devNum,
    OUT   PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC *pizzaDevInfoPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

