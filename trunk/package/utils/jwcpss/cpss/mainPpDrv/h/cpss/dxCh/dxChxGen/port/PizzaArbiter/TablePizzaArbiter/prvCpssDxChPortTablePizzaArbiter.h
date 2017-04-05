/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortTablePizzaArbiter.h
*
* DESCRIPTION:
*       CPSS implementation for table pizza arbiter 
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#ifndef __PRV_CPSS_DXCH_TABLE_PIZZA_ARBITER_H
#define __PRV_CPSS_DXCH_TABLE_PIZZA_ARBITER_H

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>



/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfInit
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

GT_STATUS prvCpssDxChPortTablePizzaArbiterIfInit
(
    IN  GT_U8                   devNum
);

/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfCheckSupport
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
GT_STATUS prvCpssDxChPortTablePizzaArbiterIfCheckSupport
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     portSpeed
);

/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfConfigure
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
GT_STATUS prvCpssDxChPortTablePizzaArbiterIfConfigure
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_SPEED_ENT     portSpeed
);

/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfDelete
*
* DESCRIPTION:
*       Delete port from Pizza Arbiter 
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
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
GT_STATUS prvCpssDxChPortTablePizzaArbiterIfDelete
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
);

/*******************************************************************************
* prvCpssDxChPortTablePizzaArbiterIfDevStateGet
*
* DESCRIPTION:
*       Lion 2 Pizza Arbiter State which includes state of all
*         -
*         - state of slices
*
* APPLICABLE DEVICES:
*        Lion2; 
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Cat2; Bobacat2
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
GT_STATUS prvCpssDxChPortTablePizzaArbiterIfDevStateGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    OUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
);



#endif

