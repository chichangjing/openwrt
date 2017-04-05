/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortSlicesTxDMA.h
*
* DESCRIPTION:
*       Rx DMA CTU Unit Slices Manipulation Interface
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#ifndef __CPSSDXCH_PORTPIZZA_SLICES_TX_DMA_H
#define __CPSSDXCH_PORTPIZZA_SLICES_TX_DMA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h> 
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/*--------------------------------------------------*/
/* TxDMA slices manipulation                        */
/*--------------------------------------------------*/
/*  TxDMA control register
** 0-4 bits  number of slices to use, 1-31 
**                                    0 if equal to 32
** 5-5 bits  enable the strict priority for CPU port
*/
/* TxDMA slice manipulation register field
** 0-3   slice is enable to port N (4 bit for port number 
** 4-4   slice is enable/disable
*/
#define TxDMA_TOTAL_SLICES_CNS               32

#define TxDMA_CTRL_SLICE_NUM_BITS_CNS        0x1F
#define TxDMA_CTRL_STRICT_PRIORITY_BIT_CNS   0x20

#define TxDMA_SLICE_PORT_BITS_CNS            0x0F
#define TxDMA_SLICE_ENABLE_BIT_CNS           0x10


/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMASlicesNumSet
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
*       enableStrictPriority4CPU - strict priority to CPU bit shall be set or not
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
GT_STATUS prvLion2PortPizzaArbiterTxDMASlicesNumSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 slicesNum,
    IN GT_BOOL enableStrictPriority4CPU
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMASlicesNumGet
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
*       enableStrictPriority4CPUPtr - where to store strict priority to CPU bit 
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
GT_STATUS prvLion2PortPizzaArbiterTxDMASlicesNumGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId,
    OUT GT_U32  *slicesNumPtr,
    OUT GT_BOOL *enableStrictPriority4CPUPtr
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMASliceOccupy
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
GT_STATUS prvLion2PortPizzaArbiterTxDMASliceOccupy
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceId,
    IN GT_PHYSICAL_PORT_NUM portId
);


/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMASliceRelease
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
GT_STATUS prvLion2PortPizzaArbiterTxDMASliceRelease
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceId
);


/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMASliceStateSet
*
* DESCRIPTION:
*      The function sets the state of the slice (i.e. if slace is disable or enable
*      and if it enbaled to which port it is assigned) for TXDMA unit.
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
*       isEnabled   - is slace enabled
*       portId      - pord id slace is enabled to
*
* OUTPUTS:
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
GT_STATUS prvLion2PortPizzaArbiterTxDMASliceStateSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceId,
    IN GT_BOOL              isEnabled, 
    IN GT_PHYSICAL_PORT_NUM portId
);


/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMASliceGetState
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
*       sliceId     - slice to be released
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
GT_STATUS prvLion2PortPizzaArbiterTxDMASliceGetState
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  GT_U32 sliceId,
    OUT GT_BOOL              *isOccupiedPtr, 
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
);


/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMAInit
*
* DESCRIPTION:
*      Init TxDMA unit (sets number of slice to use , strict priority to CPU bit 
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
GT_STATUS prvLion2PortPizzaArbiterTxDMAInit
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceNum2Init
);




/*-------------------------------------------------*/
/* 360 MHz 12 Slices                               */
/*-------------------------------------------------*/

/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_SliceOccupy
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
GT_STATUS prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_SliceOccupy
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceId,
    IN GT_PHYSICAL_PORT_NUM portId
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_SliceRelease
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
GT_STATUS prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_SliceRelease
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceId
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_SliceGetState
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
*       sliceId     - slice to be released
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
GT_STATUS prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_SliceGetState
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  GT_U32 sliceId,
    OUT GT_BOOL              *isOccupiedPtr, 
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
);

/*******************************************************************************
* prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_Init
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
GT_STATUS prvLion2PortPizzaArbiterTxDMA_360Mhz_12Slices_Init
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 sliceNum2Init
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
