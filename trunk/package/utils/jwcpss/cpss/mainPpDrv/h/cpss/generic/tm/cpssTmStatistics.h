/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmStatistics.h
*
* DESCRIPTION:
*       TM statistics gathering APIs. 
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTmStatisticsh
#define __cpssTmStatisticsh

#include <cpss/generic/tm/cpssTmPublicDefs.h>


/*******************************************************************************
* cpssTmQmrPktStatisticsGet
*
* DESCRIPTION:
*       Read QMR Packet Statistics.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*
* OUTPUTS:
*       pktCntPtr       - (pointer to) QMR Pkt Statistics structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmQmrPktStatisticsGet
(
    IN   GT_U8                           devNum,
    OUT  CPSS_TM_QMR_PKT_STATISTICS_STC  *pktCntPtr
);


/*******************************************************************************
* cpssTmRcbPktStatisticsGet
*
* DESCRIPTION:
*       Read RCB Packet Statistics.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*
* OUTPUTS:
*       pktCntPtr       - (pointer to) RCB Pkt Statistics structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssTmRcbPktStatisticsGet
(
    IN   GT_U8                           devNum,
    OUT  CPSS_TM_RCB_PKT_STATISTICS_STC  *pktCntPtr
);


#endif 	    /* __cpssTmStatisticsh */

