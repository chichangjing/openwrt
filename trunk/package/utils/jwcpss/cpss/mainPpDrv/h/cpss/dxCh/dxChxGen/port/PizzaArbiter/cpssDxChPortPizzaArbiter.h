/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPizzaArbiter.h
*
* DESCRIPTION:
*       external interface for Pizza Arbiter
*
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/
#ifndef __CPSS_DXCH_PORT_PIZZA_ARBITER_H
#define __CPSS_DXCH_PORT_PIZZA_ARBITER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterTables.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterProfileStorage.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterDevInfo.h>
/* slices management for specific units */
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesRxDMA.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesRxDMACTU.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxDMA.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxDMACTU.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesBM.h>
#include <cpss/dxCh/dxChxGen/port/SliceManipulation/private/prvCpssDxChPortSlicesTxQ.h>



/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfInit
*
* DESCRIPTION:
*       Pizza arbiter initialization in all Units where it's present
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       I forced to place this function here, because it needs number of port
*       group where CPU port is connected and there is just no more suitable
*       place.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfInit
(
    IN GT_U8 devNum
);

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfCheckSupport
*
* DESCRIPTION:
*       Check whether Pizza Arbiter can be configured to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       portSpeed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfCheckSupport
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     portSpeed
);

/*******************************************************************************
* prvCpssDxChPortIfPizzaArbiterConfigure
*
* DESCRIPTION:
*       Configure Pizza Arbiter accoringly to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       portSpeed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfConfigure
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  CPSS_PORT_SPEED_ENT      portSpeed /* new port speed to be configured */
);


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfDelete
*
* DESCRIPTION:
*       Delete Pizza Arbiter
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_NOT_SUPPORTED - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfDelete
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum
);


/*******************************************************************************
* cpssDxChPortPizzaArbiterIfUserTableSet
*
* DESCRIPTION:
*       Set profile and/or speed conversion table
*       for specific device : port group
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - device number
*       portGroupBmp  - port group bitmap
*       pizzaProfilePtr - pointer to profile
*       portGroupPortSpeed2SliceNumPtr - pointer to speed 2 slice converion list
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_NO_RESOURCE  - no memory at memory pools (increase constants !!!)
*       GT_FAIL         - on error
*       GT_ALREADY_EXIST - profile or speed conversion table already exists
*
* COMMENTS:
*       1. Suppose to be called before cpssInitSystem()
*       2. Traffic on all ports of PORT GROUP at the DEVICE shall be stopped
*       3. cpssDxChPortPizzaArbiterIfUserTableSet has two pointers. Only non-NULL pointers
*          are processed.
*       4. In function cpssDxChPortPizzaArbiterIfUserTableSet() non-zero user profile is processed
*          first. If it already exists processing is stopped that means speed conversion
*          table is not processed.
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterIfUserTableSet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupBmp,
    IN  CPSS_DXCH_PIZZA_PROFILE_STC                          *pizzaProfilePtr,
    IN  CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *portGroupPortSpeed2SliceNumPtr
);


/*******************************************************************************
* cpssDxChPortPizzaArbiterIfUserTableGet
*
* DESCRIPTION:
*       Get profile and/or speed conversion table
*       for specific device : port group
*          initially searches among user defined tables and
*            if it is not found there , searches in system tables
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - device number
*       portGroupBmp  - port group bitmap
*
* OUTPUTS:
*       pizzaProfilePtrPtrPtr - pointer to profile
*       portGroupPortSpeed2SliceNumPtrPtr - pointer to speed 2 slice converion list
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - any zero pointer
*       GT_NOT_FOUND    - if any table is found
*
* COMMENTS:
*       Only first port group is used.
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterIfUserTableGet
(
    IN   GT_U8 devNum,
    IN   GT_U32 portGroupBmp,
    OUT  CPSS_DXCH_PIZZA_PROFILE_STC                          **pizzaProfilePtrPtrPtr,
    OUT  CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC **portGroupPortSpeed2SliceNumPtrPtr
);

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileIterInit
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       This function inits iterator
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
*
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*       	IterInit(&iter);
*       	IterReset(&iter);
*       	while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*       	{
*           	 // user action for each received data e.g. print
*            	IterGotoNext(&iter)
*       	}
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileIterInit
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC *iterPtr
);

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileIterReset
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function resets iterator to beginning of the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       iterPtr    - pointer to iterator
*
* OUTPUTS:
*       iterPtr    - pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*                // user action for each received data e.g. print
*                IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileIterReset
(
    INOUT  PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC *iterPtr
);

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileIterGetCur
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function returns current item
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
*
* OUTPUTS:
*       iterPtr    pointer to iterator
*       devNumPtr   - device number
*       portGroupIdxPtr  - port group
*       pizzaProfilePtrPtr - pointer to profile
*
* RETURNS:
*       GT_OK       - on success
*       GT_NO_MORE  - all elements were passed already
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileIterGetCur
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC *iterPtr,
    OUT   GT_U8  *devNumPtr,
    OUT   GT_U32 *portGroupIdxPtr,
    OUT   CPSS_DXCH_PIZZA_PROFILE_STC **pizzaProfilePtrPtr
);


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfProfileIterGotoNext
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function goes to next item in the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfProfileIterGotoNext
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC *iterPtr
);



/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterInit
*
* DESCRIPTION:
*       Set user speed conversion table list iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       This function inits iterator
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       iterPtr    pointer to iterator
*
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterInit
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC *iterPtr
);

/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterReset
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function resets iterator to beginning of the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
*
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterReset
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC *iterPtr
);


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGetCur
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function returns current item
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    - pointer to iterator
*
* OUTPUTS:
*       iterPtr    - pointer to iterator
*       devNumPtr        - device number
*       portGroupIdxPtr  - port group
*       portGroupPortSpeed2SliceNumPtrPtr - pointer to slice consversion table
*
* RETURNS:
*       GT_OK       - on success
*       GT_NO_MORE  - all elements were passed already
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGetCur
(
    INOUT  PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC *iterPtr,
    OUT    GT_U8  *devNumPtr,
    OUT    GT_U32 *portGroupIdxPtr,
    OUT    CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC **portGroupPortSpeed2SliceNumPtrPtr
);


/*******************************************************************************
* prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGotoNext
*
* DESCRIPTION:
*       Set user profile iterator
*       traverse over list of profiles/speed conv tables
*       returns dev, portGroup,  profile or speed conversion table
*       this function goes to next item in the list
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       iterPtr    pointer to iterator
* OUTPUTS:
*       iterPtr    pointer to iterator
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - invalide ptr
*
* COMMENTS:
*       Usage :
*           PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC iter;
*           IterInit(&iter);
*           IterReset(&iter);
*           while (GT_OK == (rc = IterGetCur(&iter, &dev,&portGroup))
*           {
*               // user action for each received data e.g. print
*               IterGotoNext(&iter)
*           }
*******************************************************************************/
GT_STATUS prvCpssDxChPortPizzaArbiterIfSpeed2SliceConvTblIterGotoNext
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC *iterPtr
);


/*******************************************************************************
* cpssDxChPortPizzaArbiterIfPortStateGet
*
* DESCRIPTION:
*       Get port state(how many slice it is configured)
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*       portGroupId   - port group
*       localPortNum  - local port number
*
* OUTPUTS:
*       sliceNumUsedPtr -- number of slice occupied by port.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterIfPortStateGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupId,
    IN  GT_PHYSICAL_PORT_NUM localPortNum,
    OUT GT_U32              *sliceNumUsedPtr
);


/*******************************************************************************
* cpssDxChPortPizzaArbiterIfSliceStateGet
*
* DESCRIPTION:
*       Get port slice state(is slice occupied by any port)
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portGroupIdx  - port group
*       sliceID       - sliceID
*
* OUTPUTS:
*       isOccupiedPtr - whether slice is occupied by any port
*       portNumPtr    - number of port occupied the slice .
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterIfSliceStateGet
(
    IN  GT_U8 devNum,
    IN  GT_U32 portGroupIdx,
    IN  GT_U32 sliceID,
    OUT GT_BOOL              *isOccupiedPtr,
    OUT GT_PHYSICAL_PORT_NUM *portNumPtr
);




#define UNIT_TOTAL_SLICES RxDMA_TOTAL_SLICES_CNS
#if    (UNIT_TOTAL_SLICES != RxDMA_TOTAL_SLICES_CNS)     \
    || (UNIT_TOTAL_SLICES != RxDMACTU_TOTAL_SLICES_CNS)  \
    || (UNIT_TOTAL_SLICES != TxDMA_TOTAL_SLICES_CNS)     \
    || (UNIT_TOTAL_SLICES != TxDMACTU_TOTAL_SLICES_CNS)  \
    || (UNIT_TOTAL_SLICES != BM_TOTAL_SLICES_CNS)
    #error Slices number all units shall be same
#endif

/* CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC */
typedef struct
{
    GT_U32  totalConfiguredSlices;
    GT_U32  totalSlicesOnUnit;
    GT_BOOL strictPriorityBit;
    GT_BOOL slice_enable[UNIT_TOTAL_SLICES];
    GT_U32  slice_occupied_by[UNIT_TOTAL_SLICES];
}CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC;

/* CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC */
typedef struct
{
    GT_U32  totalConfiguredSlices;
    GT_U32  slicesNumInGop;
    GT_BOOL slice_enable[TxQ_TOTAL_AVAIBLE_SLICES_CNS];
    GT_U32  slice_occupied_by[TxQ_TOTAL_AVAIBLE_SLICES_CNS];
    CPSS_DXCH_CPUPORT_TYPE_ENT  cpu_port_type;
    GT_U32  cpu_port_sliceId          [CPSS_PA_NUMBER_OF_CPU_SLICES+1];
    GT_BOOL cpu_port_slice_is_enable  [CPSS_PA_NUMBER_OF_CPU_SLICES+1];
    GT_U32  cpu_port_slice_occupied_by[CPSS_PA_NUMBER_OF_CPU_SLICES+1];
}CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC;

/* CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC */
typedef struct
{
    CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC  rxDMA;
    CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC  rxDMA_CTU;
    CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC  txDMA;
    CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC  txDMA_CTU;
    CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC  BM;
    CPSS_DXCH_UNIT_TXQ_SLICES_PIZZA_ARBITER_STATE_STC TxQ;
}CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC;

/* CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC */
typedef struct
{
    GT_U32  totalConfiguredSlices;
    GT_U32  totalSlicesOnUnit;
    GT_BOOL workConservingBit;
    GT_BOOL pizzaMapLoadEnBit;
    GT_BOOL slice_enable[1500];
    GT_U32  slice_occupied_by[1500];
}CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC;

/* CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC */
typedef struct
{
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC  rxDMA;
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC  txDMA;
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC  TxQ;
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC  txFIFO;
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC  ethFxFIFO;
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC  ilknTxFIFO;
}CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC;

/* CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC */
typedef struct
{
    CPSS_DXCH_PA_UNIT_ENT                             unitList[CPSS_DXCH_PA_UNIT_MAX_E+1]; /* last item CPSS_DXCH_PA_UNIT_UNDEFINED_E */
    CPSS_DXCH_BOBCAT2_SLICES_PIZZA_ARBITER_STATE_STC  unitState[CPSS_DXCH_PA_UNIT_MAX_E];
}CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC;


/* CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC */
typedef struct
{
    CPSS_PP_FAMILY_TYPE_ENT devType;
    union
    {
        CPSS_DXCH_LION2_DEV_PIZZA_ARBITER_STATE_STC    lion2;
        CPSS_DXCH_BOBCAT2_DEV_PIZZA_ARBITER_STATE_STC  bc2;
        CPSS_DXCH_BOBK_DEV_PIZZA_ARBITER_STATE_STC     bobK;
    }devState;
}CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC;


/*******************************************************************************
* cpssDxChPortPizzaArbiterDevStateInit
*
* DESCRIPTION:
*       Init the structure CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
*
* INPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* OUTPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on pizzaDeviceStatePtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterDevStateInit
(
    INOUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
);

/*******************************************************************************
* cpssDxChPortPizzaArbiterDevStateGet
*
* DESCRIPTION:
*       Lion 2,3 Pizza Arbiter State which includes state of all
*         -
*         - state of slices
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portGroupId  - number of port group
*
* OUTPUTS:
*       pizzaDeviceStatePtr - pointer to structure describing the state of each units.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterDevStateGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    OUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
);


/*******************************************************************************
* gtLion2PortPizzaArbiterIfStateDump      -- print register state from Pizza Point of view
* prvCpssDxChPortPizzaArbiterIfDump        -- print register dump
* cpssDxChPortPizzaArbiterIfTablesDump     -- print list of user tables
*    all defined in appDemo\boardConfig\gtDbLion2_96XG.c
*******************************************************************************/
/*
** GT_STATUS gtLion2PortPizzaArbiterIfStateDump(IN  GT_U8  devNum,
**                                               IN  GT_U32 portGroupId);
**
** GT_STATUS gtLion2PortPizzaArbiterIfRegistersDump(IN GT_U8  devNum,
** 										             IN GT_U32 portGroupId);
**
** GT_STATUS cpssDxChPortPizzaArbiterIfTablesDump  ( IN GT_U8  devNum,
**                                                   IN GT_U32 portGroupId);
**
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

