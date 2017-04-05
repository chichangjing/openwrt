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
* cpssDxChTmGlueFlowControl.c
*
* DESCRIPTION:
*       Traffic Manager Glue - Flow Control API implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/tmGlue/private/prvCpssDxChTmGlueLog.h>

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueFlowControl.h>
#include <cpss/dxCh/dxChxGen/tmGlue/private/prvCpssDxChTmGluePfc.h>

/* check that the TM port index is valid return GT_BAD_PARAM on error */
#define PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_PORT_INDEX_CHECK_MAC(_tmPortInd) \
    if ((_tmPortInd) >= 192)                                                \
    {                                                                       \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                          \
    }
/* resolves register index and start bit by TM port index */
#define PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_REG_INDEX_BIT_OFSSET_MAC(_tmPortInd, _index, _bit) \
    _index = _tmPortInd / 3;                                                                  \
    _bit = 10 * (_tmPortInd % 3);

static CPSS_PORT_SPEED_ENT prvSpeedsEnumsArr[] =
{
    CPSS_PORT_SPEED_10_E,
    CPSS_PORT_SPEED_100_E,
    CPSS_PORT_SPEED_1000_E,
    CPSS_PORT_SPEED_2500_E,
    CPSS_PORT_SPEED_5000_E,
    CPSS_PORT_SPEED_10000_E,
    CPSS_PORT_SPEED_12000_E,
    CPSS_PORT_SPEED_20000_E,
    CPSS_PORT_SPEED_40000_E,
    CPSS_PORT_SPEED_11800_E,
    CPSS_PORT_SPEED_47200_E
};

static GT_U32 prvSpeedsValsArr[] = 
{
    10,
    100,
    1000,
    2500,
    5000,
    10000,
    12000,
    20000,
    40000,
    11800,
    47200
};

    
/*******************************************************************************
* internal_cpssDxChTmGlueFlowControlEgressEnableSet
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
*       to support back-pressure from physical ports to TM Ports.
* 
*       Caelum global ingressEnable is used along with
*       Caelum port cpssDxChTmGluePfcResponseModeSet.
*******************************************************************************/
static GT_STATUS internal_cpssDxChTmGlueFlowControlEnableSet
(
    IN GT_U8                    devNum,
    IN GT_BOOL                  egressEnable,
    IN GT_BOOL                  ingressEnable
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32  data;            /* HW value */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E  | CPSS_BOBCAT3_E );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    data = (egressEnable == GT_FALSE) ? 0 : 1;

    regAddr = PRV_DXCH_REG1_UNIT_TM_FCU_MAC(devNum).TMFCUGlobalConfigs;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, data); /* TM FCU Egress */

    if(rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum)) /* IS_BOBK_DEV_MAC(devNum) */
    {
        data = (ingressEnable == GT_FALSE) ? 0 : 1;

        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, data); /* TM FCU Ingress */

        /* for BC2 TM FCU Ingress is configured internally
           by cpssDxChTmGluePfcResponseModeSet */
    }

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueFlowControlEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, egressEnable, ingressEnable));

    rc = internal_cpssDxChTmGlueFlowControlEnableSet(devNum, egressEnable, ingressEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, egressEnable, ingressEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueFlowControlEnableGet
*
* DESCRIPTION:
*       Get state of Flow Control for TM.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       egressEnablePtr  - GT_TRUE / GT_FALSE - enable / disable egress Flow Control for TM.
*       ingressEnablePtr - GT_TRUE / GT_FALSE - enable / disable ingress Flow Control for TM,
*                          relevant for Caelum only.  
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
static GT_STATUS internal_cpssDxChTmGlueFlowControlEnableGet
(
    IN GT_U8                    devNum,
    OUT GT_BOOL                *egressEnablePtr,
    OUT GT_BOOL                *ingressEnablePtr
)
{
    GT_STATUS rc;      /* return code */
    GT_U32    regAddr; /* register address */
    GT_U32    data;    /* HW data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E  | CPSS_BOBCAT3_E );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(egressEnablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TM_FCU_MAC(devNum).TMFCUGlobalConfigs;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 1, 1, &data);  /* TM FCU Egress */    
    if(rc != GT_OK)
    {
        return rc;
    }

    *egressEnablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    if (PRV_CPSS_SIP_5_15_CHECK_MAC(devNum)) /* IS_BOBK_DEV_MAC(devNum) */
    {
        CPSS_NULL_PTR_CHECK_MAC(ingressEnablePtr);

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &data); /* TM FCU Ingress */

        *ingressEnablePtr = (data == 0) ? GT_FALSE : GT_TRUE;
    }

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueFlowControlEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, egressEnablePtr, ingressEnablePtr));

    rc = internal_cpssDxChTmGlueFlowControlEnableGet(devNum, egressEnablePtr, ingressEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, egressEnablePtr, ingressEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueFlowControlEgressCounterSet
*
* DESCRIPTION:
*       Set value of the TM Egress Flow Control counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
static GT_STATUS internal_cpssDxChTmGlueFlowControlEgressCounterSet
(
    IN GT_U8 devNum,
    IN GT_U32 tmPortInd,
    IN GT_U32 value
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    index;    /* word index */
    GT_U32    bit;      /* register bit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E  | CPSS_BOBCAT3_E );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_PORT_INDEX_CHECK_MAC(tmPortInd);

    if(value >= BIT_10)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_REG_INDEX_BIT_OFSSET_MAC(tmPortInd, index, bit);

    regAddr = PRV_DXCH_REG1_UNIT_TM_FCU_MAC(devNum).TMEgrFlowCtrlCntrs[index];

    /* Set TM Egress Flow Control Counters - TM FC Queue Counter */
    return prvCpssHwPpSetRegField(devNum, regAddr, bit, 10, value);
}

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
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueFlowControlEgressCounterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tmPortInd, value));

    rc = internal_cpssDxChTmGlueFlowControlEgressCounterSet(devNum, tmPortInd, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tmPortInd, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueFlowControlEgressCounterGet
*
* DESCRIPTION:
*       Get value of the TM Egress Flow Control counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
static GT_STATUS internal_cpssDxChTmGlueFlowControlEgressCounterGet
(
    IN GT_U8 devNum,
    IN GT_U32 tmPortInd,
    OUT GT_U32 *valuePtr
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    index;    /* word index */
    GT_U32    bit;      /* register bit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E  | CPSS_BOBCAT3_E );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_PORT_INDEX_CHECK_MAC(tmPortInd);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_REG_INDEX_BIT_OFSSET_MAC(tmPortInd, index, bit);

    regAddr = PRV_DXCH_REG1_UNIT_TM_FCU_MAC(devNum).TMEgrFlowCtrlCntrs[index];

    /* Get TM Egress Flow Control Counters - TM FC Queue Counter */
    return prvCpssHwPpGetRegField(devNum, regAddr, bit, 10, valuePtr);
}

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
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueFlowControlEgressCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tmPortInd, valuePtr));

    rc = internal_cpssDxChTmGlueFlowControlEgressCounterGet(devNum, tmPortInd, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tmPortInd, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueFlowControlEgressThresholdsSet
*
* DESCRIPTION:
*       Set XON/XOFF TM Egress Flow Control thresholds values.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
static GT_STATUS internal_cpssDxChTmGlueFlowControlEgressThresholdsSet
(
    IN GT_U8 devNum,
    IN GT_U32 tmPortInd,
    IN GT_U32 xOffThreshold,
    IN GT_U32 xOnThreshold
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    index;    /* word index */
    GT_U32    bit;      /* register bit */
    GT_STATUS rc;       /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E  | CPSS_BOBCAT3_E );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_PORT_INDEX_CHECK_MAC(tmPortInd);

    if(xOffThreshold >= BIT_10)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(xOnThreshold >= BIT_10)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_REG_INDEX_BIT_OFSSET_MAC(tmPortInd, index, bit);

    regAddr = PRV_DXCH_REG1_UNIT_TM_FCU_MAC(devNum).TMEgrFlowCtrlXOFFThresholds[index];
    /* Set TM Egress Flow Control XOFF Thresholds - TM FC Queue XOFF */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, bit, 10, xOffThreshold);
    if(rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_TM_FCU_MAC(devNum).TMEgrFlowCtrlXONThresholds[index];
    /* Set TM Egress Flow Control XON Thresholds - TM FC Queue XON */
    return prvCpssHwPpSetRegField(devNum, regAddr, bit, 10, xOnThreshold);
}

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
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueFlowControlEgressThresholdsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tmPortInd, xOffThreshold, xOnThreshold));

    rc = internal_cpssDxChTmGlueFlowControlEgressThresholdsSet(devNum, tmPortInd, xOffThreshold, xOnThreshold);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tmPortInd, xOffThreshold, xOnThreshold));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueFlowControlEgressThresholdsGet
*
* DESCRIPTION:
*       Get XON/XOFF TM Egress Flow Control thresholds values.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
static GT_STATUS internal_cpssDxChTmGlueFlowControlEgressThresholdsGet
(
    IN GT_U8 devNum,
    IN GT_U32 tmPortInd,
    OUT GT_U32 *xOffThresholdPtr,
    OUT GT_U32 *xOnThresholdPtr
)
{
    GT_U32    regAddr;  /* register address */
    GT_U32    index;    /* word index */
    GT_U32    bit;      /* register bit */
    GT_STATUS rc;       /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E  | CPSS_BOBCAT3_E );

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_PORT_INDEX_CHECK_MAC(tmPortInd);
    CPSS_NULL_PTR_CHECK_MAC(xOffThresholdPtr);
    CPSS_NULL_PTR_CHECK_MAC(xOnThresholdPtr);

    PRV_CPSS_DXCH_TM_GLUE_FLOW_CONTROL_REG_INDEX_BIT_OFSSET_MAC(tmPortInd, index, bit);

    regAddr = PRV_DXCH_REG1_UNIT_TM_FCU_MAC(devNum).TMEgrFlowCtrlXOFFThresholds[index];
    /* Get TM Egress Flow Control XOFF Thresholds - TM FC Queue XOFF */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, bit, 10, xOffThresholdPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_TM_FCU_MAC(devNum).TMEgrFlowCtrlXONThresholds[index];
    /* Get TM Egress Flow Control XON Thresholds - TM FC Queue XON */
    return prvCpssHwPpGetRegField(devNum, regAddr, bit, 10, xOnThresholdPtr);
}

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
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueFlowControlEgressThresholdsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tmPortInd, xOffThresholdPtr, xOnThresholdPtr));

    rc = internal_cpssDxChTmGlueFlowControlEgressThresholdsGet(devNum, tmPortInd, xOffThresholdPtr, xOnThresholdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tmPortInd, xOffThresholdPtr, xOnThresholdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChTmGlueFlowControlPortSpeedByTimerGet
*
* DESCRIPTION:
*       Get port speed by timer calibration value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
*
*******************************************************************************/
static GT_STATUS prvCpssDxChTmGlueFlowControlPortSpeedByTimerGet
(
    IN  GT_U32              coreClock,
    IN  GT_U32              timerValue,
    OUT CPSS_PORT_SPEED_ENT *speedPtr
)
{
    GT_U32      value;           /* register value */
    GT_FLOAT32  tmpValue;        /* temporary value */
    GT_U32      calcSpeed;    /* port speed in Mbps */
    GT_U32      calcTimerValue;  /* calculated timer value */
    GT_STATUS   rc = GT_OK;
    GT_U32      i;
    CPSS_PORT_SPEED_ENT speedEnum = CPSS_PORT_SPEED_NA_E;

    value = (1 << 15);
    tmpValue = (GT_FLOAT32)(coreClock * (1.0 * 512 / 320) * value); 
    calcSpeed = (GT_U32)(0.5 + tmpValue / timerValue);

    /* find accurate speed, as calculate speed from timer value has very small deviation */
    for (i = 0; i < sizeof(prvSpeedsValsArr) / sizeof(prvSpeedsValsArr[0]); i++)
    {
        if (calcSpeed <= prvSpeedsValsArr[i])
        {
            calcSpeed = prvSpeedsValsArr[i];
            speedEnum = prvSpeedsEnumsArr[i];
            break;
        }
    }

    if (speedEnum == CPSS_PORT_SPEED_NA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, "Wrong Timer Value: %d, calcSpeed: %d", timerValue, calcSpeed);
    }

    /* verify timer value corresponding to calcSpeed */
    tmpValue = (GT_FLOAT32)(512 * coreClock) / (GT_FLOAT32)(320 * calcSpeed);
    /*calcTimerValue = (GT_U32)(tmpValue * value + 0.5) ;*/
    tmpValue = (tmpValue * value);
    calcTimerValue = (GT_U32)(tmpValue);

    if ((GT_FLOAT32)calcTimerValue != tmpValue)
        calcTimerValue++;

    if (calcTimerValue != timerValue)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, "Wrong Timer Value: %d, calcTimerValue: %d", timerValue, calcTimerValue);
    }

    *speedPtr = speedEnum;

    return rc;
}


/*******************************************************************************
* prvCpssDxChTmGlueFlowControlTimerByPortSpeedGet
*
* DESCRIPTION:
*       Get timer calibration value by port speed.
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
*
*******************************************************************************/
static GT_STATUS prvCpssDxChTmGlueFlowControlTimerByPortSpeedGet
(
    IN  GT_U32              coreClock,
    IN  CPSS_PORT_SPEED_ENT speedEnum,
    OUT GT_U32              *timerValuePtr
)
{
    GT_U32      value;              /* register value */
    GT_FLOAT32  tmpValue;           /* temporary value */
    GT_U32      speedForCalc = 0;   /* port speed in Mbps */
    GT_U32      calcTimerValue;
    GT_STATUS   rc = GT_OK;
    GT_U32      i;

    for (i = 0; i < sizeof(prvSpeedsEnumsArr) / sizeof(prvSpeedsEnumsArr[0]); i++)
    {
        if (speedEnum == prvSpeedsEnumsArr[i])
        {
            speedForCalc = prvSpeedsValsArr[i];
            break;
        }
    }

    if (speedForCalc == 0)
    {    
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, "Unsupported Speed: %d", speedEnum);
    }

    /* Calculate port speed calibration value.
       value is number of PP core clocks needed to pause TX for 512 bits of
           line rate.
       Jira TMIP-187:
        ReqVal = CORE_CLK*1000000*PFC_UNITS/(PortSpeed*UPDATE_RATE)
        ConfigVal = Round(ReqVal * 2^15)
        */
    value = (1 << 15);
    tmpValue = (GT_FLOAT32)(512 * coreClock) / (GT_FLOAT32)(320 * speedForCalc);
    tmpValue = (tmpValue * value);
    calcTimerValue = (GT_U32)(tmpValue);

    if ((GT_FLOAT32)calcTimerValue != tmpValue)
        calcTimerValue++;

    *timerValuePtr = calcTimerValue;

    return rc;
}

/*******************************************************************************
* prvCpssDxChTmGlueFlowControlPortSpeedSet
*
* DESCRIPTION:
*       Set port speed calibration value.
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
    IN CPSS_PORT_SPEED_ENT      speed
)
{
    GT_STATUS               rc = GT_OK;
    GT_PHYSICAL_PORT_NUM    pfcPortNum;
    GT_U32                  value = 0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E  | CPSS_BOBCAT3_E );

    /* PFC with TM supported only for first 63 physical ports for BC2 version < B0 */
    rc = prvCpssDxChTmGluePfcPortMappingGet(devNum, portNum, &pfcPortNum);

    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChTmGlueFlowControlTimerByPortSpeedGet(PRV_CPSS_PP_MAC(devNum)->coreClock, speed, &value);

    if (rc != GT_OK)
    {
        return rc;
    }

    /* Set Port Speed Entry - Port Speed */
    return prvCpssDxChWriteTableEntryField(devNum,
                 PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E,
                 pfcPortNum,
                 PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                 0, 32, value);
}

/*******************************************************************************
* internal_cpssDxChTmGlueFlowControlPortSpeedSet
*
* DESCRIPTION:
*       Set PFC calibration value by port speed.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
static GT_STATUS internal_cpssDxChTmGlueFlowControlPortSpeedSet
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc;

    rc = prvCpssDxChTmGlueFlowControlPortSpeedSet(devNum, portNum, speed);

    return rc;
}

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
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat3.
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
*   This API should be called on changing of TM Enabled physical port speed
*   mapped to the PFC port, as a result of:
*       1) port speed has changed. (cpssDxChPortModeSpeedSet)
*       2) PFC port mapping has changed (cpssDxChTmGluePfcPortMappingSet).
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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueFlowControlPortSpeedSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, speed));

    rc = internal_cpssDxChTmGlueFlowControlPortSpeedSet(devNum, portNum, speed);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, speed));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTmGlueFlowControlPortSpeedGet
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
static GT_STATUS internal_cpssDxChTmGlueFlowControlPortSpeedGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_PORT_SPEED_ENT  *speedPtr
)
{
    GT_STATUS               rc = GT_OK;
    GT_PHYSICAL_PORT_NUM    pfcPortNum;
    GT_U32                  value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E  | CPSS_BOBCAT3_E );

    /* PFC with TM supported only for first 63 physical ports for BC2 version < B0 */
    rc = prvCpssDxChTmGluePfcPortMappingGet(devNum, portNum, &pfcPortNum);

    if (rc != GT_OK)
    {
        return rc;
    }

    /* Get Port Speed Entry - Port Speed */
    rc = prvCpssDxChReadTableEntryField(devNum,
                 PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E,
                 pfcPortNum,
                 PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                 0, 32, &value);

    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChTmGlueFlowControlPortSpeedByTimerGet(PRV_CPSS_PP_MAC(devNum)->coreClock, value, speedPtr);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTmGlueFlowControlPortSpeedGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, speedPtr));

    rc = internal_cpssDxChTmGlueFlowControlPortSpeedGet(devNum, portNum, speedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, speedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

#define cpssDxChTmGlueFlowControl_debug_def

#ifdef cpssDxChTmGlueFlowControl_debug_def

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>

/*******************************************************************************
* prvCpssDxChTmGlueFlowControlPortSpeedTest
*
* DESCRIPTION:
*       Test/Update port 
* 
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Lion3.
*
* INPUTS:
*       devNum      - device number.
*       startPort   - from tm pfc port.
*       maxPort     - to tm pfc port.
*       startCnode  - from  cnode.
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChTmGlueFlowControlPortSpeedTest
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum
)
{
    GT_STATUS   retVal;
    GT_STATUS   rc = GT_OK;
    GT_U32      i;
    CPSS_PORT_SPEED_ENT setSpeed;
    CPSS_PORT_SPEED_ENT getSpeed;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    for (i = 0; i < sizeof(prvSpeedsEnumsArr) / sizeof(prvSpeedsEnumsArr[0]); i++)
    {
        setSpeed = prvSpeedsEnumsArr[i];

        retVal = cpssDxChTmGlueFlowControlPortSpeedSet(devNum,
                                                   portNum,
                                                   setSpeed);
        if(retVal != GT_OK)
        {
            cpssOsPrintf("TmGlueFlowControlPortSpeedSet, Error: dev: %d, port: %d, setSpeed: %d, rc: %d\n",
                         devNum, portNum, setSpeed, retVal);  

            rc = retVal;
        }

        retVal = cpssDxChTmGlueFlowControlPortSpeedGet(devNum,
                                                   portNum,
                                                   &getSpeed);
        if(retVal != GT_OK)
        {
            cpssOsPrintf("TmGlueFlowControlPortSpeedGet, Error: dev: %d, port: %d, setSpeed: %d, rc: %d\n",
                         devNum, portNum, setSpeed, retVal);  

            rc = retVal;
        }

        cpssOsPrintf("prvCpssDxChTmGlueFlowControlPortSpeedTest: dev: %d, port: %d, setSpeed: %d, getSpeed: %d\n",
                     devNum, portNum, setSpeed, getSpeed);  
    }

    return rc;
}


/*******************************************************************************
* prvCpssDxChTmGlueFlowControlPortSpeedGet
*
* DESCRIPTION:
*       Get port speed and HW calibration value.
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
*       speedPtr    - (pointer to) port speed in MB
*       PfcTimer    - (pointer to) PfcTimer Value
*
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
GT_STATUS prvCpssDxChTmGlueFlowControlPortSpeedGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *speedPtr,
    OUT GT_U32                  *pfcTimerValue
)
{
    GT_STATUS           rc = GT_OK;
    GT_U32              speedForCalc = 0;
    CPSS_PORT_SPEED_ENT speed1 = CPSS_PORT_SPEED_NA_E;
    CPSS_PORT_SPEED_ENT speed2 = CPSS_PORT_SPEED_NA_E;
    GT_U32              value;           /* register value */
    GT_PHYSICAL_PORT_NUM pfcPortNum = 0;
    GT_U32              i;


    /* get portSpeed by HW calibration Value */
    rc = cpssDxChTmGlueFlowControlPortSpeedGet(devNum, portNum, &speed1);

    if (rc != GT_OK)
    {
        cpssOsPrintf("TmGlueFlowControlPortSpeedGet, Error: dev: %d, port: %d, rc %d\n", devNum, portNum, rc);
        return rc;
    }

   /* PFC with TM supported only for first 63 physical ports for BC2 version < B0 */
    rc = prvCpssDxChTmGluePfcPortMappingGet(devNum, portNum, &pfcPortNum);

    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortSpeedGet(devNum, portNum, &speed2);
    if (rc != GT_OK)
    {
        cpssOsPrintf("cpssDxChPortSpeedGet, Error: dev: %d, port: %d, rc %d\n", devNum, portNum, rc);
        return rc;
    }

    if (speed1 != speed2)
    {
        cpssOsPrintf("cpssDxChTmGlueFlowControlPortSpeedGet: speed: %d!\n", speed1);
        cpssOsPrintf("cpssDxChPortSpeedGet:                  speed: %d!\n", speed2);
    }

    for (i = 0; i < sizeof(prvSpeedsEnumsArr) / sizeof(prvSpeedsEnumsArr[0]); i++)
    {
        if (speed1 == prvSpeedsEnumsArr[i])
        {
            speedForCalc = prvSpeedsValsArr[i];
            break;
        }
    }

    /* Get Port Speed Entry - Port Speed */
    rc =  prvCpssDxChReadTableEntryField(devNum,
                 PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E,
                 pfcPortNum,
                 PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                 0, 32, &value);

    if (rc != GT_OK)
    {
        return rc;
    }

    *speedPtr = speedForCalc;
    *pfcTimerValue = value;

    return rc;
}

GT_STATUS prvCpssDxChTmGlueFlowControlPortSpeedDump
(
    IN GT_U8    devNum,
    IN GT_U32   fromPort,
    IN GT_U32   toPort
)
{
    GT_U32 pfcTimerValue;
    GT_U32 speed;
    GT_STATUS rc = GT_OK;
    GT_U32 port;

    for (port = fromPort; port <= toPort; port++)
    {
        rc = prvCpssDxChTmGlueFlowControlPortSpeedGet(devNum, port, &speed, &pfcTimerValue);

        if (rc != GT_OK)
        {
            return rc;
        }

        cpssOsPrintf("dev: %d, port: %02d, speed: %d[MB], pfcTimer: %d\n",
                     devNum,
                     port,
                     speed,
                     pfcTimerValue);
    }

    return rc;
}


#endif

