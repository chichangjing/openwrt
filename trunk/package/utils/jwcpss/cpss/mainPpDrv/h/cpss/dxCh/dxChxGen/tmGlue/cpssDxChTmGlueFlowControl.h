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
* cpssDxChTmGlueFlowControl.h
*
* DESCRIPTION:
*       Traffic Manager Glue - Flow Control API declaration.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#ifndef __cpssDxChTmGlueFlowControlh
#define __cpssDxChTmGlueFlowControlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/*******************************************************************************
* cpssDxChTmGlueFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable Flow Control for TM.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       egressEnable - GT_TRUE / GT_FALSE - enable / disable egress Flow Control for TM.
*       ingressEnable- GT_TRUE / GT_FALSE - enable / disable ingress Flow Control for TM,
*                      relevant for Caelum only.  
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
*       egressEnable must be enabled whenever the TM is used
*       to response back-pressure from physical ports to TM Ports,
*       used along with cpssTmSchedPortExternalBpSet.
* 
*       Caelum global ingressEnable is used along with
*       Caelum port cpssDxChTmGluePfcResponseModeSet.
*******************************************************************************/
GT_STATUS cpssDxChTmGlueFlowControlEnableSet
(
    IN GT_U8                    devNum,
    IN GT_BOOL                  egressEnable,
    IN GT_BOOL                  ingressEnable
);

/*******************************************************************************
* cpssDxChTmGlueFlowControlEnableGet
*
* DESCRIPTION:
*       Get state of Flow Control for TM.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       egressEnablePtr  - (pointer to) egress flow control state.
*       ingressEnablePtr - (pointer to) ingress flow control state.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueFlowControlEnableGet
(
    IN GT_U8                    devNum,
    OUT GT_BOOL                *egressEnablePtr,
    OUT GT_BOOL                *ingressEnablePtr
);

/*******************************************************************************
* cpssDxChTmGlueFlowControlEgressCounterSet
*
* DESCRIPTION:
*       Set value of the TM Egress Flow Control counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       tmPortInd       - TM port index.
*                         (APPLICABLE RANGES: 0..191).
*       value           - value of the TM egress flow control counter.
*                         (APPLICABLE RANGES: 0..1023).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number, TM port index.
*       GT_OUT_OF_RANGE          - on wrong counter value.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueFlowControlEgressCounterSet
(
    IN GT_U8 devNum,
    IN GT_U32 tmPortInd,
    IN GT_U32 value
);

/*******************************************************************************
* cpssDxChTmGlueFlowControlEgressCounterGet
*
* DESCRIPTION:
*       Get value of the TM Egress Flow Control counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       tmPortInd       - TM port index.
*                         (APPLICABLE RANGES: 0..191).
* OUTPUTS:
*       valuePtr        - (poiter to) value of the TM egress flow control counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number or TM port index.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueFlowControlEgressCounterGet
(
    IN GT_U8 devNum,
    IN GT_U32 tmPortInd,
    OUT GT_U32 *valuePtr
);

/*******************************************************************************
* cpssDxChTmGlueFlowControlEgressThresholdsSet
*
* DESCRIPTION:
*       Set XON/XOFF TM Egress Flow Control thresholds values.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       tmPortInd       - TM port index.
*                         (APPLICABLE RANGES: 0..191).
*       xOffThreshold    - TM FC Queue XOFF threshold.
*                         (APPLICABLE RANGES: 0..1023).
*       xOnThreshold     - TM FC Queue XON threshold.
*                         (APPLICABLE RANGES: 0..1023).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number, TM port index.
*       GT_HW_ERROR              - on hardware error.
*       GT_OUT_OF_RANGE          - on wrong XOFF/XON threshold.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueFlowControlEgressThresholdsSet
(
    IN GT_U8 devNum,
    IN GT_U32 tmPortInd,
    IN GT_U32 xOffThreshold,
    IN GT_U32 xOnThreshold
);

/*******************************************************************************
* cpssDxChTmGlueFlowControlEgressThresholdsGet
*
* DESCRIPTION:
*       Get XON/XOFF TM Egress Flow Control thresholds values.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       tmPortInd       - TM port index.
*                         (APPLICABLE RANGES: 0..191).
* OUTPUTS:
*       xOffThresholdPtr - (pointer to) TM FC Queue XOFF threshold.
*       xOnThresholdPtr  - (pointer to) TM FC Queue XON threshold.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number or TM port index.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueFlowControlEgressThresholdsGet
(
    IN GT_U8 devNum,
    IN GT_U32 tmPortInd,
    OUT GT_U32 *xOffThresholdPtr,
    OUT GT_U32 *xOnThresholdPtr
);

/*******************************************************************************
* cpssDxChTmGlueFlowControlPortSpeedSet
*
* DESCRIPTION:
*       Set PFC calibration value by port speed.
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
*   	This API should be called on changing of TM Enabled	physical port speed
*   	mapped to the PFC port, as a result of:
*   		1) port speed has changed. (cpssDxChPortModeSpeedSet)
*   		2) PFC port mapping has changed (cpssDxChTmGluePfcPortMappingSet).
* 
*       This table is the calibration value to be multiplied to the value in the PFC header
*       to be alligned to the number of cycles according to the port speed.
*        
*******************************************************************************/
GT_STATUS cpssDxChTmGlueFlowControlPortSpeedSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_SPEED_ENT      speed
);

/*******************************************************************************
* cpssDxChTmGlueFlowControlPortSpeedGet
*
* DESCRIPTION:
*       Get port speed from its PFC calibration value set by
*       cpssDxChTmGlueFlowControlPortSpeedSet.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Lion3.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number.
*
* OUTPUTS:
*       speedPtr    - (pointer of) CPSS_PORT_SPEED_ENT.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number.
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTmGlueFlowControlPortSpeedGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_PORT_SPEED_ENT  *speedPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTmGlueFlowControlh */


