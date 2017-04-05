/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortDynamicPizzaArbiter.c
*
* DESCRIPTION:
*       CPSS implementation for SerDes configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_PORT_DYNAMIC_PIZZA_ARBITER_H
#define __PRV_CPSS_DXCH_PORT_DYNAMIC_PIZZA_ARBITER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAUnitBW.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPAUnitDrv.h>


extern GT_STATUS prvCpssDxChPortDynamicPizzaArbiterSpeedConv
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speedEnm,
    OUT GT_U32              *speedPtr 
);


extern GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfInit
(
    IN  GT_U8  devNum
);

/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfTMBWSet
*
* DESCRIPTION:
*       define BW of TM units
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
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
extern GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfTMBWSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  bwMbps
);


extern GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfCheckSupport
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  portSpeedInMBit
);

extern GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfConfigure
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  portSpeedInMBit
);

extern GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelCheckSupport
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  portSpeedInMBit
);

/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelConfigure
*
* DESCRIPTION:
*       Configure Pizza Arbiter accoringly to new port's i/f mode and speed
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfIlknChannelConfigure
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  portSpeedInMBit
);


extern GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfDevStateGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    OUT CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC *pizzaDeviceStatePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
