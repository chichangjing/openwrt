/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTmSched.c
*
* DESCRIPTION:
*       TM Scheduler APIs. 
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/generic/tm/private/prvCpssGenTmLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/tm/cpssTmServices.h>
#include <cpss/generic/tm/cpssTmSched.h>
#include <tm_sched.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h> 


/*******************************************************************************
* internal_cpssTmSchedPortExternalBpSet
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
static GT_STATUS internal_cpssTmSchedPortExternalBpSet
(
   IN GT_U8     devNum,
   IN GT_BOOL   portExtBp
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

#if 0 /* TBD */
    TM_DEV_CHECK_MAC(devNum);
#endif

    ret = tm_sched_general_config(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)portExtBp);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
*       to enable Backpressure response from TM Ports to physical ports, 
*       egressEnable at cpssDxChTmGlueFlowControlEnableSet must also be enabled.
*******************************************************************************/
GT_STATUS cpssTmSchedPortExternalBpSet
(
   IN GT_U8     devNum,
   IN GT_BOOL   portExtBp
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmSchedPortExternalBpSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portExtBp));

    rc = internal_cpssTmSchedPortExternalBpSet(devNum, portExtBp);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portExtBp));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssTmSchedPeriodicSchemeConfig
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
*       paramsPtr       - Per level parameters array pointer.
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
*       1. The API may be invoked once in a system lifetime.
*       2. To indicate the shaping is disabled for a level the
*          periodicState field must be set to GT_FALSE.
*       3  paramsPtr is handled as 5 elements array including also a
*          port level scheduling configuration.
*******************************************************************************/
static GT_STATUS internal_cpssTmSchedPeriodicSchemeConfig
(
    IN GT_U8                                    devNum,
    IN CPSS_TM_LEVEL_PERIODIC_PARAMS_STC        *paramsPtr
)
{
    int         i;
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    struct tm_level_periodic_params prms[P_LEVEL+1];

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
    CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                       | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

#if 0 /* TBD */
    TM_DEV_CHECK_MAC(devNum);
#endif

    for(i=Q_LEVEL; i<=P_LEVEL; i++)
	{
        prms[i].per_state = (uint8_t)(paramsPtr[i].periodicState);
        prms[i].shaper_dec = (uint8_t)(paramsPtr[i].shaperDecoupling);
    }
    ret = tm_configure_periodic_scheme(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_BAD_VALUE             - on not found parameter or parameter's value can't be obtained.
*       GT_BAD_SIZE              - on maxBw token size can not be presented even when using resolution.
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
    IN GT_U8                                    devNum,
    IN CPSS_TM_LEVEL_PERIODIC_PARAMS_STC        *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmSchedPeriodicSchemeConfig);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, paramsPtr));

    rc = internal_cpssTmSchedPeriodicSchemeConfig(devNum, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssTmShapingPeriodicUpdateEnableSet
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
static GT_STATUS internal_cpssTmShapingPeriodicUpdateEnableSet
(
   IN GT_U8                 devNum,
   IN CPSS_TM_LEVEL_ENT     level,
   IN GT_BOOL               status
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );
    ret = tm_periodic_update_status_set(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle,
                                   level,
                                   (uint8_t)status);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
   IN GT_U8                 devNum,
   IN CPSS_TM_LEVEL_ENT     level,
   IN GT_BOOL               status
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmShapingPeriodicUpdateEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, level, status));

    rc = internal_cpssTmShapingPeriodicUpdateEnableSet(devNum, level, status);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, level, status));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmShapingPeriodicUpdateStatusGet
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
*       paramsPtr       - Per level parameters array pointer.
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
*         paramsPtr is handled as 5 elements array including also a
*         port level scheduling configuration.
*******************************************************************************/
static GT_STATUS internal_cpssTmShapingPeriodicUpdateStatusGet
(
    IN GT_U8                                    devNum,
    IN CPSS_TM_LEVEL_PERIODIC_PARAMS_STC        *paramsPtr
)
{
    int         i;
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    struct tm_level_periodic_params prms[P_LEVEL+1];

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
    CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                       | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

#if 0 /* TBD */
    TM_DEV_CHECK_MAC(devNum);
#endif
    ret = tm_periodic_update_status_get(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, prms);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    for(i=Q_LEVEL; i<=P_LEVEL; i++)
	{
        paramsPtr[i].periodicState    = prms[i].per_state;
        paramsPtr[i].shaperDecoupling = prms[i].shaper_dec;
    }
    return rc;
}

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
*       paramsPtr       - Per level parameters array pointer.
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
*       1. The API may be invoked once in a system lifetime.
*       2. To indicate the shaping is disabled for a level the
*          periodicState field must be set to GT_FALSE.
*       3  paramsPtr is handled as 5 elements array including also a
*          port level scheduling configuration.
*******************************************************************************/
GT_STATUS cpssTmShapingPeriodicUpdateStatusGet
(
    IN GT_U8                                    devNum,
    IN CPSS_TM_LEVEL_PERIODIC_PARAMS_STC        *paramsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmSchedPeriodicSchemeConfig);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, paramsPtr));

    rc = internal_cpssTmShapingPeriodicUpdateStatusGet(devNum, paramsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, paramsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}








/*******************************************************************************
* internal_cpssTmSchedPortLvlDwrrBytesPerBurstLimitSet
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
static GT_STATUS internal_cpssTmSchedPortLvlDwrrBytesPerBurstLimitSet
(
    IN GT_U8    devNum,
    IN GT_U32   bytes
)
{
    int         ret = 0;
    GT_STATUS   rc = GT_OK;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_port_level_set_dwrr_bytes_per_burst_limit(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, (uint8_t)bytes);
    rc = XEL_TO_CPSS_ERR_CODE(ret);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmSchedPortLvlDwrrBytesPerBurstLimitSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, bytes));

    rc = internal_cpssTmSchedPortLvlDwrrBytesPerBurstLimitSet(devNum, bytes);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, bytes));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssTmPortQuantumLimitsGet 
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
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmPortQuantumLimitsGet 
(
    IN GT_U8						 devNum,
    OUT CPSS_TM_QUANTUM_LIMITS_STC  *quantumLimitsPtr
)
{
	int							ret = 0;
    GT_STATUS					rc = GT_OK;
    struct tm_quantum_limits	quantum_limits;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_sched_get_port_quantum_limits(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, &quantum_limits);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
	if (rc) return rc;

	quantumLimitsPtr->minQuantum = quantum_limits.minQuantum;
	quantumLimitsPtr->maxQuantum = quantum_limits.maxQuantum;
	quantumLimitsPtr->resolution = quantum_limits.resolution;

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmPortQuantumLimitsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, quantumLimitsPtr));

    rc = internal_cpssTmPortQuantumLimitsGet(devNum, quantumLimitsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, quantumLimitsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssTmNodeQuantumLimitsGet
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
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssTmNodeQuantumLimitsGet
(
    IN GT_U8						 devNum,
    OUT CPSS_TM_QUANTUM_LIMITS_STC  *quantumLimitsPtr
)
{
	int							ret = 0;
    GT_STATUS					rc = GT_OK;
    struct tm_quantum_limits	quantum_limits;

    PRV_CPSS_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E
                                           | CPSS_LION2_E | CPSS_BOBCAT3_E | CPSS_PUMA_E | CPSS_PUMA3_E  );

    ret = tm_sched_get_node_quantum_limits(PRV_CPSS_PP_MAC(devNum)->tmInfo.tmHandle, &quantum_limits);
    rc = XEL_TO_CPSS_ERR_CODE(ret);
	if (rc) return rc;

	quantumLimitsPtr->minQuantum = quantum_limits.minQuantum;
	quantumLimitsPtr->maxQuantum = quantum_limits.maxQuantum;
	quantumLimitsPtr->resolution = quantum_limits.resolution;

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssTmNodeQuantumLimitsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, quantumLimitsPtr));

    rc = internal_cpssTmNodeQuantumLimitsGet(devNum, quantumLimitsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, quantumLimitsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

