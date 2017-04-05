/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortSlicesRxDMACTU.h
*
* DESCRIPTION:
*       TxQ Unit Slices Manipulation Interface
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __CPSSDXCH_PORTPIZZA_SLICES_TX_Q_H
#define __CPSSDXCH_PORTPIZZA_SLICES_TX_Q_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxQHWDef.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiterProfile.h>

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSlicesNumSet
*
* DESCRIPTION:
*      The function sets slice number used in Pizza Arbiter for BM unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       slicesNum   - slices number to be set in pizza arbiter
*       isCpuAttached - is cpuu attached to core
*       txQDefPtr   - (pointer to) tx definitoon 
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_HW_ERROR     - hw error
*       GT_OUT_OF_RANGE - number of slice to be set is greater than available
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQSlicesNumSet
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   slicesNum,
    IN GT_BOOL  isCpuAttached,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSlicesNumGet
*
* DESCRIPTION:
*      The function get slice number used in Pizza Arbiter and strict priority state bit
*      for BM unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       txQDefPtr   - (ponter to)txq definiton
*
* OUTPUTS:
*       slicesNumPtr      - place to store slices number used in pizza arbiter
*       slicesNumInGopPtr - where to store slice number in GOP
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_BAD_PTR      - bad slicesNumPtr, enableStrictPriority4CPUPtr
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQSlicesNumGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  CPSS_DXCH_TXQDEF_STC *txQDefPtr,
    OUT GT_U32               *slicesNumPtr,
    OUT GT_U32               *slicesNumInGopPtr
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSliceOccupy
*
* DESCRIPTION:
*      The function occupy the slice (i.e. set the spicific slice
*      be asigned to specific port for BM unit
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       sliceId     - slice to be occupied
*       portId      - local port id (inside portGroup) to which slice is assigned
*       slicesNumberPerGop - number of slices in pizza
*       txQDefPtr   - (ponter to)txq definiton
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_OUT_OF_RANGE - sliceId or portId are greater than available
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQSliceOccupy
(
    IN GT_U8   devNum,
    IN GT_U32  portGroupId,
    IN GT_U32  sliceId,
    IN GT_PHYSICAL_PORT_NUM portId,
    IN GT_U32  slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSliceRelease
*
* DESCRIPTION:
*      The function release the slice (i.e. set the spicific slice
*      to be disable to any port) for BM unit)
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       sliceId     - slice to be released
*       slicesNumberPerGop - number of slices in pizza
*       txQDefPtr   - (ponter to)txq definiton
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_OUT_OF_RANGE - sliceId or portId are greater than available
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQSliceRelease
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceId,
    IN GT_U32 slicesNumberPerGop,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
);


/*******************************************************************************
* prvLion2PortPizzaArbiterTxQSliceGetState
*
* DESCRIPTION:
*      The function gets the state of the slice (i.e. if slace is disable or enable
*      and if it enbaled to which port it is assigned) for BM unit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupId - port group id
*       sliceId     - slice to get state of
*
* OUTPUTS:
*       isOccupiedPtr - place to store whether slice is occupied
*       portNumPtr   - plavce to store to which port slice is assigned
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
GT_STATUS prvLion2PortPizzaArbiterTxQSliceGetState
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceId,
    OUT GT_BOOL              *isOccupiedPtr,
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQInit
*
* DESCRIPTION:
*      Init BM unit (sets number of slice to use , strict priority to CPU bit
*      and disables all slices of the device.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       portGroupId   - port group id
*       sliceNum2Init - number of slices to be used
*       txQDefPtr   - (ponter to)txq definiton
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, portGroupId
*       GT_OUT_OF_RANGE - sliceNum2Init is greater than available
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvLion2PortPizzaArbiterTxQInit
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceNum2Init,
    IN CPSS_DXCH_TXQDEF_STC *txQDefPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

