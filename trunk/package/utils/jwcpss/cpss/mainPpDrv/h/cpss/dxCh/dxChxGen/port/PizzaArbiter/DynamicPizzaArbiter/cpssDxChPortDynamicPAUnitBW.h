/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortDynamicPAUnitBW.h
*
* DESCRIPTION:
*       bobcat2 and higher dynamic (algorithmic) pizza arbiter 
*
* FILE REVISION NUMBER:
*       $Revision: 2$
*******************************************************************************/
#ifndef __CPSS_DXCH_PORT_DYNAMIC_PA_UNIT_BW_H
#define __CPSS_DXCH_PORT_DYNAMIC_PA_UNIT_BW_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPATypeDef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum
{
	 CPSS_DXCH_MIN_SPEED_INVALID_E = 0
	,CPSS_DXCH_MIN_SPEED_500_Mbps_E
	,CPSS_DXCH_MIN_SPEED_1000_Mbps_E
	,CPSS_DXCH_MIN_SPEED_2000_Mbps_E
	,CPSS_DXCH_MIN_SPEED_5000_Mbps_E
	,CPSS_DXCH_MIN_SPEED_10000_Mbps_E
	,CPSS_DXCH_MIN_SPEED_MAX
}CPSS_DXCH_MIN_SPEED_ENT;

/*******************************************************************************
* cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet
*
* DESCRIPTION:
*       set pipe BW for TxDMA, RXDMA, TxQ, Tx-FIFO, Eth-Tx-FIFO and slice resolution for TxQ
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum               - device number
*       pipeBandwithInGbps   - pipe bandwidth in GBits
*       minimalPortSpeedMBps - min port speed resolution in MBps
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, combination of BW and slice resolution
*       GT_FAIL         - on error
*
* COMMENTS:
*       a.	API does not change system BW, it only informs pizza arbiter about requested BW.
*       b.	API returns GT_BAD_PARAM, if number of available slices * resolution is smaller than requested BW,
*           ex: Bobcat2 has 340 slices, default resolution is 0.5G , max BW that can be supported is 340[slices] * 0.5[Gbps/slice] = 170 [Gpbs]
*           ex: if one like to support BW 240Gpbs, than ceil(240[Gpbs]/340[slices]) = 1[Gpbs/slice] = 1000 [Mpbs/slice]
*       c.	API checks whether there are ports with speed smaller than resolution. If any is present, API returns with error (GT_FAIL).
*       d.  API cpssDxChPortModeSpeedSet() will return error, if requested speed is smaller than speed resolution.
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet
(
    IN GT_U8                     devNum,
    IN GT_U32                    pipeBandwithInGbps,
    IN CPSS_DXCH_MIN_SPEED_ENT   minimalPortSpeedMBps
);

/*******************************************************************************
* cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionGet
*
* DESCRIPTION:
*       get pipe BW for TxDMA, RXDMA, TxQ, Tx-FIFO, Eth-Tx-FIFO and slice resolution for TxQ
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum                       - device number
*
* OUTPUTS:
*       pipeBandwithInGbpsPtr           - pipe bandwidth in GBits
*       minimalSliceResolutionInMBpsPtr - min port speed resolution in MBps
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - wrong devNum, combination of BW and slice resolution
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionGet
(
    IN  GT_U8                     devNum,
    OUT GT_U32                   *pipeBandwithInGbpsPtr,
    OUT CPSS_DXCH_MIN_SPEED_ENT  *minimalSliceResolutionInMBpsPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif



