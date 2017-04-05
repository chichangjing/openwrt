/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortSlicesTxQHWDef.h
*
* DESCRIPTION:
*       TxQ Unit Slices HW definion
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/
#ifndef __CPSSDXCH_PORTPIZZA_SLICES_TXQ_HWDEF_H
#define __CPSSDXCH_PORTPIZZA_SLICES_TXQ_HWDEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>


/*--------------------------------------------------*/
/* TxQ slices manipulation                        */
/*--------------------------------------------------*/
/*  TxQ control register
** 0-7 bits  number of slices to use,
*/
/* TxQ slice manipulation register field
** 0-3   slice is enable to port N (4 bit for port number)
** 4-4   slice is enable/disable
*/
#define TxQ_TOTAL_AVAIBLE_SLICES_CNS       252 /* 42 * 6 */

#define TxQ_CTRL_SLICE_NUM_BITS_CNS        0xFF

#define TxQ_SLICE_PORT_BITS_CNS            0x0F
#define TxQ_SLICE_ENABLE_BIT_CNS           0x10


/*******************************************************************************
* prvLion2PortPizzaArbiterTxQDrvSlicesNumSet
*
* DESCRIPTION:
*      The function sets slice number used in DRV of TxQ unit
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
*       slicesNum2Configure - slices number to be set in pizza arbiter
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
GT_STATUS prvLion2PortPizzaArbiterTxQDrvSlicesNumSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 slicesNum2Configure
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQDrvSlicesNumGet
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
*
* OUTPUTS:
*       slicesNumPtr   - place to store slices number used in pizza arbiter
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
GT_STATUS prvLion2PortPizzaArbiterTxQDrvSlicesNumGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    OUT GT_U32  *slicesNumPtr
);


/*******************************************************************************
* prvLion2PortPizzaArbiterTxQDrvSliceStateSet
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
*       sliceId     - slice to be set
*       isEnabled   - whether the slice shall be enebled or diabled
*       portId      - local port id (inside portGroup) to which slice is assigned
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
GT_STATUS prvLion2PortPizzaArbiterTxQDrvSliceStateSet
(
    IN GT_U8   devNum,
    IN GT_U32  portGroupId,
    IN GT_U32  sliceId,
    IN GT_BOOL              isEnabled, 
    IN GT_PHYSICAL_PORT_NUM portId
);


/*******************************************************************************
* prvLion2PortPizzaArbiterTxQDrvSliceStateGet
*
* DESCRIPTION:
*      The function get the slice state(i.e. set the spicific slice
*      be asigned to specific port for TxQ unit and whether it enabled
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
*
* OUTPUTS:
*       isEnabledPtr - place to store whether slice is occupied
*       portIdPtr   - plavce to store to which port slice is assigned
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
GT_STATUS prvLion2PortPizzaArbiterTxQDrvSliceStateGet
(
    IN GT_U8   devNum,
    IN GT_U32  portGroupId,
    IN GT_U32  sliceId,
    OUT GT_BOOL              *isEnabledPtr, 
    OUT GT_PHYSICAL_PORT_NUM *portIdPtr
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxQDrvInit
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
GT_STATUS prvLion2PortPizzaArbiterTxQDrvInit
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceNum2Init
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

