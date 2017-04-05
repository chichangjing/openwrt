/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* prvCpssDxChTmGlue.h
*
* DESCRIPTION:
*       Traffic Manager Glue - common private PFC declarations.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#ifndef __prvCpssDxChTmGlueh
#define __prvCpssDxChTmGlueh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/* External Memory Initialization result flags */
/* Initialized DRAM and PHY only, don't touch TM */
#define PRV_CPSS_DXCH_TM_GLUE_DRAM_INIT_DONE_DDR_PHY_CNS  0x40000000
/* Initialized TM */
#define PRV_CPSS_DXCH_TM_GLUE_DRAM_INIT_DONE_TM_CNS       0x80000000
/* Initialized flags mask */
#define PRV_CPSS_DXCH_TM_GLUE_DRAM_INIT_DONE_MASK_CNS   \
    (PRV_CPSS_DXCH_TM_GLUE_DRAM_INIT_DONE_DDR_PHY_CNS | \
    PRV_CPSS_DXCH_TM_GLUE_DRAM_INIT_DONE_TM_CNS)



/*******************************************************************************
* prvCpssDxChTmGlueFlowControlPortSpeedSet
*
* DESCRIPTION:
*       Set port speed calibration value
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number.
*       speed       - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       This table is the calibration value to be multiplied to the value in the PFC header
*       to be alligned to the number of cycles according to the port speed.
*
*******************************************************************************/
GT_STATUS prvCpssDxChTmGlueFlowControlPortSpeedSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN  CPSS_PORT_SPEED_ENT     speed
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChTmGlueh */


