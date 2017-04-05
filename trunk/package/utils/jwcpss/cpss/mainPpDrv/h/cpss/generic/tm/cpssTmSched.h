/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmSched.h
*
* DESCRIPTION:
*       TM Scheduler APIs. 
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTmSchedh
#define __cpssTmSchedh

#include <cpss/generic/tm/cpssTmPublicDefs.h>


/*******************************************************************************
* cpssTmSchedPortExternalBpSet
*
* DESCRIPTION:
*       Enable/Disable Port External Backpressure.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       portExtBp       - Enable/Disable port external BP.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
* COMMENTS:
*       to enable Backpressure response from TM Ports to physical ports, 
*       egressEnable at cpssDxChTmGlueFlowControlEnableSet must also be enabled.
*******************************************************************************/
GT_STATUS cpssTmSchedPortExternalBpSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   portExtBp
);


/*******************************************************************************
* cpssTmSchedPeriodicSchemeConfig
*
* DESCRIPTION:
*       Configure Periodic Scheme.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       paramsPtr       - (pointer to) scheduling parameters structure.
* OUTPUTS:
*		none
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_VALUE             - on not found parameter or parameter's value can't be obtained.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1. The API may be invoked once in a system lifetime.
*       2. To indicate the shaping is disabled for a level the
*          periodicState field must be set to GT_FALSE.
*       3  paramsPtr is handled as 5 elements array including also a
*          port level scheduling configuration.
*******************************************************************************/
GT_STATUS cpssTmSchedPeriodicSchemeConfig
(
    IN GT_U8                                devNum,
    IN CPSS_TM_LEVEL_PERIODIC_PARAMS_STC    *paramsPtr
);


/*******************************************************************************
* cpssTmShapingPeriodicUpdateEnableSet
*
* DESCRIPTION:
*       Enable/Disable periodic update for a given level: Q, A, B, C or Port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Level to enable/disable shaping for.
*       status          - Enable/Disable shaping.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       If periodic update will be disabled for some level, traffic through nodes with shaping-enabled eligible functions will be stopped !
*		
*******************************************************************************/
GT_STATUS cpssTmShapingPeriodicUpdateEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_TM_LEVEL_ENT        level,
    IN GT_BOOL                  status
);

/*******************************************************************************
* cpssTmShapingPeriodicUpdateStatusGet
*
* DESCRIPTION:
*        Read periodc update status for all TM levels.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       statusPtr       - (pointer to) scheduling parameters structure.
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssTmShapingPeriodicUpdateStatusGet
(
    IN GT_U8								devNum,
    IN CPSS_TM_LEVEL_PERIODIC_PARAMS_STC    *statusPtr
);



/*******************************************************************************
* cpssTmSchedPortLvlDwrrBytesPerBurstLimitSet
*
* DESCRIPTION:
*       Set the number of DWRR bytes per burst limit for all ports.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       bytes           - Number of bytes per burst limit.
*
* OUTPUTS:
*       None.
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
GT_STATUS cpssTmSchedPortLvlDwrrBytesPerBurstLimitSet
(
    IN GT_U8    devNum,
    IN GT_U32   bytes
);

/*******************************************************************************
* cpssTmPortQuantumLimitsGet 
*
* DESCRIPTION:
*       get maximum and minimum possible quantum values and quantum value granularity for port.
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
*       quantumLimits	- (pointer to) structure containing quantum limits.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Since set/update port parameters APIs are note in bytes resolution but Quantums units,
*       applicable values are minQuantum/resolution, maxQuantum/resolution.
*******************************************************************************/
GT_STATUS cpssTmPortQuantumLimitsGet 
(
    IN GT_U8						 devNum,
    OUT CPSS_TM_QUANTUM_LIMITS_STC  *quantumLimitsPtr
);

/*******************************************************************************
* cpssTmNodeQuantumLimitsGet
*
* DESCRIPTION:
*       get maximum and minimum possible quantum values and quantum value granularity for nodes.
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
*       quantumLimitsPtr - (pointer to) quantum limits structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       Since set/update node parameters APIs are note in bytes resolution but Quantums units,
*       applicable values are minQuantum/resolution, maxQuantum/resolution.
*******************************************************************************/
GT_STATUS cpssTmNodeQuantumLimitsGet
(
    IN GT_U8						 devNum,
    OUT CPSS_TM_QUANTUM_LIMITS_STC  *quantumLimitsPtr
);


#endif 	    /* __cpssTmSchedh */

