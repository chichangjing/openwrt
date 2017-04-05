/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortDynamicPAUnitBW.h
*
* DESCRIPTION:
*       bobcat2 and higher dynamic (algorithmic) pizza arbiter 
*
* FILE REVISION NUMBER:
*       $Revision: 3$
*******************************************************************************/
#ifndef __PRV_CPSS_DXCH_PORT_DYNAMIC_PA_UNIT_BW_H
#define __PRV_CPSS_DXCH_PORT_DYNAMIC_PA_UNIT_BW_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPATypeDef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfUnitBWSet
*
* DESCRIPTION:
*       set pipe BW for specific unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       unit     - unit type
*       bwInGBit - bandwidth in GBits
*
* OUTPUTS:
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
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfUnitBWSet
(
    IN GT_U8                     devNum,
    IN CPSS_DXCH_PA_UNIT_ENT unit,
    IN GT_U32                    bwInGBit
);


/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfUnitBWGet
*
* DESCRIPTION:
*       Get pipe BW for specific unit
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       unit     - unit type
*
* OUTPUTS:
*       bwInGBitPtr - bandwidth in GBits
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on wrong ptr
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfUnitBWGet
(
    IN  GT_U8                     devNum,
    IN  CPSS_DXCH_PA_UNIT_ENT unit,
    OUT GT_U32                   *bwInGBitPtr
);
/*******************************************************************************
* prvCpssDxChPortDynamicPizzaArbiterIfUnitBWInit
*
* DESCRIPTION:
*       Inti pipe BW for all units depending on device id
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortDynamicPizzaArbiterIfUnitBWInit
(
    GT_U8 devNum
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

