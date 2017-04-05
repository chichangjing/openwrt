/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPmStc.c
*
* DESCRIPTION:
*       CPSS DxCh Sampling To CPU (STC) API implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 21 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/mirror/private/prvCpssDxChMirrorLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChStc.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>

/*******************************************************************************
* prvCpssDxChStcBusyWait
*
* DESCRIPTION:
*       Local busy wait function, for Control STC, to check that the previous
*       action was finished.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       portGroupId        - the portGroupId , to support multi-port-groups device
*       regAddr       - Ingress STC Control Access Register address
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChStcBusyWait
(
    IN GT_U8                                devNum,
    IN GT_U32                               portGroupId,
    IN GT_U32                               regAddr
)
{
    /* wait for bit 0 to clear */
    return prvCpssPortGroupBusyWait(devNum,portGroupId,regAddr,0,GT_FALSE);
}

/*******************************************************************************
* prvIngressStcTableIndirectAccess
*
* DESCRIPTION:
*               Local auxiliary function
*               Write into or Read From the HW statistical rate limit table
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*       accessAction  - read/write table entry
*       ingrStcLimitPtr  - pointer for new sFlow sample threshold
*                       may be NULL if not needed
*       ingrStcSampledCountPtr - pointer for current number of sFlow sampled packets
*
* OUTPUTS:
*       ingrStcLimitPtr   - pointer for current sFlow sample threshold
*                        may be NULL if not needed
*       ingrStcSampledCountPtr - pointer for current number of sFlow sampled packets
*       ingrStcCounterPtr - pointer for current value of countdown sample threshold counter
*       ingValRdyPtr      - pointer for the new value existance
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/

static GT_STATUS prvIngressStcTableIndirectAccess
(
    IN GT_U8                                devNum,
    IN GT_PHYSICAL_PORT_NUM                 portNum,
    IN PRV_CPSS_DXCH_ACCESS_ACTION_ENT      accessAction,
    INOUT GT_U32                            *ingrStcLimitPtr,
    INOUT GT_U32                            *ingrStcSampledCountPtr,
    OUT GT_U32                              *ingrStcCounterPtr,
    OUT GT_U32                              *ingValRdyPtr
)
{
    GT_U32      ingStcTblAccessCtrlReg;
    GT_U32      ingStcTblWord0ValueReg;
    GT_U32      ingStcTblWord1ValueReg; /* read only */
    GT_U32      ingStcTblWord2ValueReg;
    GT_U32      controlValue, dataValue;
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    ingStcTblAccessCtrlReg = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
        ingStcTblAccessCtrlReg;
    ingStcTblWord0ValueReg = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
        ingStcTblWord0ValueReg;
    ingStcTblWord1ValueReg = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
        ingStcTblWord1ValueReg;
    ingStcTblWord2ValueReg = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
        ingStcTblWord2ValueReg;

    rc = prvCpssDxChStcBusyWait(devNum,portGroupId, ingStcTblAccessCtrlReg);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set the read/write trigger bit */
    controlValue = 1;

    if (accessAction == PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E)
    {
        if ((ingrStcLimitPtr == NULL) && (ingrStcSampledCountPtr == NULL))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        controlValue |= (1 << 1);
        controlValue |= (localPort << 2);

        if (ingrStcLimitPtr != NULL)
        {
            controlValue |= (1 << 7);

            dataValue = *ingrStcLimitPtr;
            dataValue |= (1 << 30); /* IngressSTCNewLimValRdy */
            rc  = prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId,ingStcTblWord0ValueReg,
                                              dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if (ingrStcSampledCountPtr != NULL)
        {
            controlValue |= (1 << 8);

            dataValue = *ingrStcSampledCountPtr;
            rc  = prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId,ingStcTblWord2ValueReg,
                                              dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* non-direct table --- do the write action */
        rc  = prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId,ingStcTblAccessCtrlReg,
                                          controlValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* busy wait for the control reg to show "write was done" */
        rc = prvCpssDxChStcBusyWait(devNum, portGroupId,ingStcTblAccessCtrlReg);
        if (rc != GT_OK)
        {
            return rc;
        }

    }
    else
    {
        if ((ingrStcLimitPtr == NULL) && (ingrStcSampledCountPtr == NULL) &&
            (ingrStcCounterPtr == NULL))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        controlValue |= (localPort << 2);

        /* non-direct table --- do the read action */
        rc  = prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId,ingStcTblAccessCtrlReg,
                                          controlValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* busy wait for the control reg to show "read was done" */
        rc = prvCpssDxChStcBusyWait(devNum, portGroupId,ingStcTblAccessCtrlReg);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (ingrStcLimitPtr != NULL)
        {
            rc  = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,ingStcTblWord0ValueReg,
                                             &dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            *ingrStcLimitPtr = dataValue & 0x3FFFFFFF;

            if(ingValRdyPtr != NULL)
            {
                *ingValRdyPtr    = (dataValue >> 30);
            }

        }

        if (ingrStcCounterPtr != NULL)
        {
            rc  = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,ingStcTblWord1ValueReg,
                                             &dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            *ingrStcCounterPtr = dataValue & 0x3FFFFFFF;
        }

        if (ingrStcSampledCountPtr != NULL)
        {
            rc  = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,ingStcTblWord2ValueReg,
                                             &dataValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            *ingrStcSampledCountPtr = dataValue & 0xFFFF;
        }
    }

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcIngressCountModeSet
*
* DESCRIPTION:
*       Set the type of packets subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       mode      - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E - All packets without
*                                                         any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcIngressCountModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_STC_COUNT_MODE_ENT    mode
)
{
    GT_U32      hwValue;        /* value to write to register */
    GT_U32      regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (mode)
    {
        case CPSS_DXCH_STC_COUNT_ALL_PACKETS_E:
            hwValue = 0;
            break;

        case CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E:
            hwValue = 1;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            ingrSTCConfig.ingrSTCConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, hwValue);
}

/*******************************************************************************
* cpssDxChStcIngressCountModeSet
*
* DESCRIPTION:
*       Set the type of packets subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       mode      - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E - All packets without
*                                                         any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcIngressCountModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_STC_COUNT_MODE_ENT    mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcIngressCountModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChStcIngressCountModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcIngressCountModeGet
*
* DESCRIPTION:
*       Get the type of packets that are subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       modePtr   - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E = All packets without
*                                                           any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcIngressCountModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_STC_COUNT_MODE_ENT    *modePtr
)
{
    GT_U32      hwValue;        /* value to write to register   */
    GT_U32      regAddr;        /* register address             */
    GT_STATUS   rc;             /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* get Ingress STC Configuration register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            ingrSTCConfig.ingrSTCConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &hwValue);

    *modePtr = (hwValue == 0) ? CPSS_DXCH_STC_COUNT_ALL_PACKETS_E :
                                CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E;

    return rc;
}

/*******************************************************************************
* cpssDxChStcIngressCountModeGet
*
* DESCRIPTION:
*       Get the type of packets that are subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       modePtr   - CPSS_DXCH_STC_COUNT_ALL_PACKETS_E = All packets without
*                                                           any MAC-level errors.
*                   CPSS_DXCH_STC_COUNT_NON_DROPPED_PACKETS_E -
*                                                       only non-dropped packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcIngressCountModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_STC_COUNT_MODE_ENT    *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcIngressCountModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChStcIngressCountModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcReloadModeSet
*
* DESCRIPTION:
*       Set the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       mode      - Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,STC type or mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcReloadModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT                  stcType,
    IN  CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT     mode
)
{
    GT_U32      hwValue;        /* value to write to register */
    GT_U32      regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* get register address according to STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* get Ingress STC Configuration register address */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                    ingrSTCConfig.ingrSTCConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
            }
            break;

        case CPSS_DXCH_STC_EGRESS_E:
            /* get Statistical and CPU-Triggered Egress Mirroring to
               Analyzer Port Configuration register address */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCStatisticalTxSniffConfig;
            }
            else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                    statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (mode)
    {
        case CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E:
            hwValue = 0;
            break;

        case CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E:
            hwValue = 1;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, hwValue);
}

/*******************************************************************************
* cpssDxChStcReloadModeSet
*
* DESCRIPTION:
*       Set the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       mode      - Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,STC type or mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcReloadModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT                  stcType,
    IN  CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT     mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcReloadModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stcType, mode));

    rc = internal_cpssDxChStcReloadModeSet(devNum, stcType, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stcType, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcReloadModeGet
*
* DESCRIPTION:
*       Get the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       modePtr   - (pointer to) Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcReloadModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT                  stcType,
    OUT CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT     *modePtr
)
{
    GT_U32      hwValue;        /* value to write to register   */
    GT_U32      regAddr;        /* register address             */
    GT_STATUS   rc;             /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* get register address according to STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* get Ingress STC Configuration register address */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                    ingrSTCConfig.ingrSTCConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
            }
            break;

        case CPSS_DXCH_STC_EGRESS_E:
            /* get Statistical and CPU-Triggered Egress Mirroring to
               Analyzer Port Configuration register address */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCStatisticalTxSniffConfig;
            }
            else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                    statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 1, 1, &hwValue);

    *modePtr = (hwValue == 0) ? CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E :
                                CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E;

    return rc;
}

/*******************************************************************************
* cpssDxChStcReloadModeGet
*
* DESCRIPTION:
*       Get the type of Sampling To CPU (STC) count reload mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       modePtr   - (pointer to) Sampling to CPU (STC) Reload mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_DXCH_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcReloadModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT                  stcType,
    OUT CPSS_DXCH_STC_COUNT_RELOAD_MODE_ENT     *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcReloadModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stcType, modePtr));

    rc = internal_cpssDxChStcReloadModeGet(devNum, stcType, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stcType, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcEnableSet
*
* DESCRIPTION:
*       Global Enable/Disable Sampling To CPU (STC).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       enable    - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcEnableSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_BOOL                     enable
)
{
    GT_U32      hwValue;        /* value to write to register   */
    GT_U32      regAddr;        /* register address             */
    GT_U32      bitOffset;      /* bit offset inside register   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* get Ingress STC Configuration register address */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                    ingrSTCConfig.ingrSTCConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
            }
            bitOffset = 2;
            break;

        case CPSS_DXCH_STC_EGRESS_E:
            /* get Statistical and CPU-Triggered Egress Mirroring to
               Analyzer Port Configuration register address */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCStatisticalTxSniffConfig;
            }
            else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                    statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
            }
            bitOffset = 0;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwValue = BOOL2BIT_MAC(enable);

    return prvCpssHwPpSetRegField(devNum, regAddr, bitOffset, 1, hwValue);
}

/*******************************************************************************
* cpssDxChStcEnableSet
*
* DESCRIPTION:
*       Global Enable/Disable Sampling To CPU (STC).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       enable    - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcEnableSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_BOOL                     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stcType, enable));

    rc = internal_cpssDxChStcEnableSet(devNum, stcType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stcType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcEnableGet
*
* DESCRIPTION:
*       Get the global status of Sampling To CPU (STC) (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       enablePtr - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcEnableGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_BOOL                     *enablePtr
)
{

    GT_U32      hwValue;        /* value to write to register   */
    GT_U32      regAddr;        /* register address             */
    GT_U32      bitOffset;      /* bit offset inside register   */
    GT_STATUS   rc;             /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            /* get Ingress STC Configuration register address */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                    ingrSTCConfig.ingrSTCConfig;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sFlowRegs.sFlowControl;
            }
            bitOffset = 2;
            break;

        case CPSS_DXCH_STC_EGRESS_E:
            /* get Statistical and CPU-Triggered Egress Mirroring to
               Analyzer Port Configuration register address */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCStatisticalTxSniffConfig;
            }
            else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                    statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
            }
            bitOffset = 0;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitOffset, 1, &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChStcEnableGet
*
* DESCRIPTION:
*       Get the global status of Sampling To CPU (STC) (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       enablePtr - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcEnableGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, stcType, enablePtr));

    rc = internal_cpssDxChStcEnableGet(devNum, stcType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, stcType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcPortLimitSet
*
* DESCRIPTION:
*       Set Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       limit     - Count Down Limit (APPLICABLE RANGES: 0..0x3FFFFFFF)
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - limit is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcPortLimitSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_U32                      limit
)
{
    GT_STATUS   rc;     /* retrun code */
    GT_U32  hwValue;    /* value to write to register */
    GT_U32  regAddr;    /* register address           */
    GT_U32  regAddr2;   /* register address for Egress monitoring */
    GT_U32  bitOffset;  /* bit offset in the register */
    GT_BOOL mirrEnable; /* mirror enabling indication */
    GT_U32  mirrIndex = 0;  /* mirror interface index */
    GT_BOOL readBeforeTxQWrite; /* read previous register before TxQ register write */
    GT_U32  readPortGroupId; /* port group id iterator for read after write */
    GT_U32  readRegData;     /* register data */
    GT_U32  data[3];    /* Egress STC table entry */
    GT_U32  portTxq;    /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (limit >= BIT_30)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    hwValue = (limit | (1 << 30));

    /* get register address according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, port);
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                return prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E,
                                            port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            0,
                                            31,
                                            hwValue);
            }
            else
            {
                /* Indirect Table write */
                return prvIngressStcTableIndirectAccess(devNum, port,
                                                 PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E,
                                                 &limit,
                                                 NULL, NULL, NULL);
            }

        case CPSS_DXCH_STC_EGRESS_E:
            PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, port, portTxq);
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                /* in eArch this per physical is not used */
                regAddr2 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
                bitOffset = 0;
            }
            else
            {
                regAddr2 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trSrcSniff;
                bitOffset = port % 32;

                if(PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
                {
                    regAddr2 += 0x64 * (port >> 5);
                }
            }

            /* Get Egress mirroring state */
            rc = cpssDxChMirrorTxPortGet(devNum, port, GT_TRUE, &mirrEnable, &mirrIndex);
            if ( GT_OK != rc)
            {
                return rc;
            }

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
            {
                readBeforeTxQWrite = GT_TRUE;
            }
            else
            {
                readBeforeTxQWrite = GT_FALSE;
            }

            /* If Egress STC is enabled on port and Egress mirroring is not enabled on port, */
            /* enable Egress monitoring on port. */
            if( (limit > 0) && (GT_FALSE == mirrEnable) )
            {
                if(regAddr2 != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
                {
                    rc = prvCpssHwPpSetRegField(devNum, regAddr2, bitOffset, 1, 1);
                    if (GT_OK != rc)
                    {
                        return rc;
                    }

                    /* In multi port group devices need to guaranty that all registers
                       were updated before TxQ register write. Because EQ registers
                       are per port group but TxQ register is only one in device.
                       This is possible that EQ registers in some port groups are
                       updated after TxQ. Need to avoid such problem by read EQ
                       register after write to TxQ one. */
                    if (readBeforeTxQWrite != GT_FALSE)
                    {
                        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,readPortGroupId)
                        {
                            rc = prvCpssHwPpPortGroupReadRegister (devNum, readPortGroupId, regAddr2, &readRegData);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }
                        }
                        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,readPortGroupId)
                    }
                }
            }

            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word0 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord0[port];
                rc = prvCpssHwPpPortGroupWriteRegister(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                     regAddr,hwValue);
            }
            else
            {
                if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E))
                {
                    PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                        info_PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E.stcEgressLimitShadow[port] = limit;

                    data[0] = hwValue;
                    data[1] = 0;
                    data[2] = 0;

                    rc = prvCpssDxChWriteTableEntry(devNum,
                                                PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                                portTxq,/* global port */
                                                data);
                }
                else
                {
                    rc = prvCpssDxChWriteTableEntryField(devNum,
                                                PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                                portTxq,/* global port */
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                0,
                                                31,
                                                hwValue);
                }
            }

            if (GT_OK != rc)
            {
                return rc;
            }

            /* If Egress STC is disabled on port and Egress mirroring is not enabled on port, */
            /* disable Egress monitoring on port. */
            if( (0 == limit) && (GT_FALSE == mirrEnable) )
            {
                if(regAddr2 != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
                {
                    rc = prvCpssHwPpSetRegField(devNum, regAddr2, bitOffset, 1, 0);
                    if (GT_OK != rc)
                    {
                        return rc;
                    }
                }
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChStcPortLimitSet
*
* DESCRIPTION:
*       Set Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       limit     - Count Down Limit (APPLICABLE RANGES: 0..0x3FFFFFFF)
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - limit is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortLimitSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_U32                      limit
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcPortLimitSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, stcType, limit));

    rc = internal_cpssDxChStcPortLimitSet(devNum, port, stcType, limit);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, stcType, limit));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcPortLimitGet
*
* DESCRIPTION:
*       Get Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       limitPtr  - pointer to Count Down Limit
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcPortLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *limitPtr
)
{
    GT_U32  regAddr;  /* register address */
    GT_U32  portTxq;  /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(limitPtr);

    /* get register address according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, port);
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                return prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E,
                                            port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            0,
                                            30,
                                            limitPtr);
            }
            else
            {
                /* Indirect Table read */
                return prvIngressStcTableIndirectAccess(devNum, port,
                                                 PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
                                                 limitPtr,
                                                 NULL, NULL, NULL);
            }

        case CPSS_DXCH_STC_EGRESS_E:
            PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, port, portTxq);
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word0 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord0[port];
                return prvCpssHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                        regAddr, 0, 30, limitPtr);
            }
            else
            {
                if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E))
                {
                    *limitPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                        info_PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E.stcEgressLimitShadow[port];
                    return GT_OK;
                }
                else
                {
                    return prvCpssDxChReadTableEntryField(devNum,
                                                PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                                portTxq,/* global port */
                                                PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                0,
                                                30,
                                                limitPtr);
                }
            }

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

}

/*******************************************************************************
* cpssDxChStcPortLimitGet
*
* DESCRIPTION:
*       Get Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       limitPtr  - pointer to Count Down Limit
*                   when limit value is 1 - every packet is sampled to CPU
*                   when limit value is 0 - there is no sampling
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *limitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcPortLimitGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, stcType, limitPtr));

    rc = internal_cpssDxChStcPortLimitGet(devNum, port, stcType, limitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, stcType, limitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcPortReadyForNewLimitGet
*
* DESCRIPTION:
*       Check whether Sampling to CPU is ready to get new STC Limit Value per
*       port.
*       The function cpssDxChStcPortLimitSet sets new limit value.
*       But only after device finishes handling of new limit value the
*       cpssDxChStcPortLimitSet may be called once more.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       isReadyPtr - (pointer to) Is Ready value
*                    GT_TRUE = Sampling to CPU ready to get new STC Limit Value
*                    GT_FALSE = Sampling to CPU handles STC Limit Value yet and
*                               is not ready to get new value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcPortReadyForNewLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_BOOL                     *isReadyPtr
)
{
    GT_STATUS   rc;         /* return code                */
    GT_U32      regAddr;    /* register address           */
    GT_U32      hwValue;    /* value to read from register */
    GT_U32      limit = 0;
    GT_U32      portTxq;    /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(isReadyPtr);

    /* get register address according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, port);
            /* Indirect Table read */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E,
                                            port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            30,
                                            1,
                                            &hwValue);
            }
            else
            {
                rc = prvIngressStcTableIndirectAccess(devNum, port,
                                                 PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
                                                 &limit,
                                                 NULL, NULL, &hwValue);
            }

            break;
        case CPSS_DXCH_STC_EGRESS_E:
            PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, port, portTxq);
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word0 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord0[port];
                rc = prvCpssHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                        regAddr, 30, 1, &hwValue);
            }
            else
            {

                if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }

                rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            portTxq,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            30,
                                            1,
                                            &hwValue);
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    *isReadyPtr = BIT2BOOL_MAC(1 - hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChStcPortReadyForNewLimitGet
*
* DESCRIPTION:
*       Check whether Sampling to CPU is ready to get new STC Limit Value per
*       port.
*       The function cpssDxChStcPortLimitSet sets new limit value.
*       But only after device finishes handling of new limit value the
*       cpssDxChStcPortLimitSet may be called once more.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       isReadyPtr - (pointer to) Is Ready value
*                    GT_TRUE = Sampling to CPU ready to get new STC Limit Value
*                    GT_FALSE = Sampling to CPU handles STC Limit Value yet and
*                               is not ready to get new value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortReadyForNewLimitGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_BOOL                     *isReadyPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcPortReadyForNewLimitGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, stcType, isReadyPtr));

    rc = internal_cpssDxChStcPortReadyForNewLimitGet(devNum, port, stcType, isReadyPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, stcType, isReadyPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcPortCountdownCntrGet
*
* DESCRIPTION:
*       Get STC Countdown Counter per port.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Count down counter. This is the number of
*                   packets left to send/receive in order that a packet will be
*                   sampled to CPU and a new value will be loaded.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcPortCountdownCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  portTxq;    /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cntrPtr);

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, port);
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                return prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E,
                                            port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            32,
                                            30,
                                            cntrPtr);
            }
            else
            {
                /* Indirect Table read */
                return prvIngressStcTableIndirectAccess(devNum, port,
                                                 PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
                                                 NULL,
                                                 NULL, cntrPtr, NULL);
            }

        case CPSS_DXCH_STC_EGRESS_E:
            PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, port, portTxq);
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word1 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord1[port];
                return  prvCpssHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                         regAddr, 0, 30, cntrPtr);
            }
            else
            {
                if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }

                return prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            portTxq,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            32,
                                            30,
                                            cntrPtr);
            }

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

}

/*******************************************************************************
* cpssDxChStcPortCountdownCntrGet
*
* DESCRIPTION:
*       Get STC Countdown Counter per port.
*       This counter is decremented for each packet received on this port and is
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Count down counter. This is the number of
*                   packets left to send/receive in order that a packet will be
*                   sampled to CPU and a new value will be loaded.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortCountdownCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcPortCountdownCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, stcType, cntrPtr));

    rc = internal_cpssDxChStcPortCountdownCntrGet(devNum, port, stcType, cntrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, stcType, cntrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcPortSampledPacketsCntrSet
*
* DESCRIPTION:
*       Set the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       cntr      - STC Sampled to CPU packet's counter (APPLICABLE RANGES: 0..0xFFFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_OUT_OF_RANGE          - on wrong cntr
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcPortSampledPacketsCntrSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_U32                      cntr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  portTxq;    /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (cntr >= BIT_16)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, port);
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                return prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E,
                                            port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            64,
                                            16,
                                            cntr);
            }
            else
            {
                /* Indirect Table write */
                return prvIngressStcTableIndirectAccess(devNum, port,
                                                 PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E,
                                                 NULL, &cntr, NULL, NULL);
            }

        case CPSS_DXCH_STC_EGRESS_E:
            PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, port, portTxq);
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word2 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord2[port];
                return prvCpssHwPpPortGroupSetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                         regAddr, 0, 16, cntr);
            }
            else
            {
                if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }

                return prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            portTxq,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            64,
                                            16,
                                            cntr);
            }

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

}

/*******************************************************************************
* cpssDxChStcPortSampledPacketsCntrSet
*
* DESCRIPTION:
*       Set the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       cntr      - STC Sampled to CPU packet's counter (APPLICABLE RANGES: 0..0xFFFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_OUT_OF_RANGE          - on wrong cntr
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortSampledPacketsCntrSet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    IN  GT_U32                      cntr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcPortSampledPacketsCntrSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, stcType, cntr));

    rc = internal_cpssDxChStcPortSampledPacketsCntrSet(devNum, port, stcType, cntr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, stcType, cntr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcPortSampledPacketsCntrGet
*
* DESCRIPTION:
*       Get the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Sampled to CPU packet's counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcPortSampledPacketsCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
)
{

    GT_U32    regAddr;  /* register address */
    GT_U32    portTxq;  /* MAC number       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cntrPtr);

    /* get register address and bitOffset according to
        STC type (Ingress/Egress) */
    switch (stcType)
    {
        case CPSS_DXCH_STC_INGRESS_E:
            PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, port);
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                return prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E,
                                            port,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            64,
                                            16,
                                            cntrPtr);
            }
            else
            {
                /* Indirect Table read */
                return prvIngressStcTableIndirectAccess(devNum, port,
                                                 PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
                                                 NULL, cntrPtr, NULL, NULL);
            }

        case CPSS_DXCH_STC_EGRESS_E:
            PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, port, portTxq);
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
            {
                /* get Port Egress STC Table Entry Word2 */
                regAddr = PRV_CPSS_DXCH_PP_MAC(devNum)->regsAddr.sFlowRegs.
                                                        egrStcTblWord2[port];
                return  prvCpssHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
                        regAddr, 0, 16, cntrPtr);
            }
            else
            {
                if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                }

                return prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
                                            portTxq,/* global port */
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            64,
                                            16,
                                            cntrPtr);
            }
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
}

/*******************************************************************************
* cpssDxChStcPortSampledPacketsCntrGet
*
* DESCRIPTION:
*       Get the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Sampled to CPU packet's counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number,port number or STC type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - This request is not supportted.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChStcPortSampledPacketsCntrGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        port,
    IN  CPSS_DXCH_STC_TYPE_ENT      stcType,
    OUT GT_U32                      *cntrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcPortSampledPacketsCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, stcType, cntrPtr));

    rc = internal_cpssDxChStcPortSampledPacketsCntrGet(devNum, port, stcType, cntrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, stcType, cntrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcEgressAnalyzerIndexSet
*
* DESCRIPTION:
*       Set the egress STC analyzer index.
*
*       feature behavior:
*       If a packet is marked for both egress STC and egress mirroring , the
*       packet is only replicated once, depending on a configurable priority
*       between the mechanisms. The configurable <STC analyzer index> determines
*       the priority of STC compared to mirroring to the analyzer.
*       Note:
*       1. that STC does not actually use this index to determine the target
*          of the mirrored packet.
*       2. the indexes used by egress mirroring engines are in range 0..6.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable analyzer index for egress STC.
*                  - GT_TRUE - enable analyzer index.
*                  - GT_FALSE - no analyzer index.
*      index     - Analyzer index. (APPLICABLE RANGES: 0..14)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function uses same parameters as mirroring functions like
*       cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(devNum,enable,index).
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcEgressAnalyzerIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;        /* register address */
    GT_U32  analyzerIndex;/* analyzer index */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if(enable == GT_TRUE)
    {
        analyzerIndex = index + 1;
    }
    else
    {
        analyzerIndex = 0;
    }

    if(analyzerIndex >= BIT_4)/* 4 bits */
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).
            statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCAnalyzerIndex;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 4, analyzerIndex);

    return rc;
}

/*******************************************************************************
* cpssDxChStcEgressAnalyzerIndexSet
*
* DESCRIPTION:
*       Set the egress STC analyzer index.
*
*       feature behavior:
*       If a packet is marked for both egress STC and egress mirroring , the
*       packet is only replicated once, depending on a configurable priority
*       between the mechanisms. The configurable <STC analyzer index> determines
*       the priority of STC compared to mirroring to the analyzer.
*       Note:
*       1. that STC does not actually use this index to determine the target
*          of the mirrored packet.
*       2. the indexes used by egress mirroring engines are in range 0..6.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable analyzer index for egress STC.
*                  - GT_TRUE - enable analyzer index.
*                  - GT_FALSE - no analyzer index.
*      index     - Analyzer index. (APPLICABLE RANGES: 0..14)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function uses same parameters as mirroring functions like
*       cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(devNum,enable,index).
*
*******************************************************************************/
GT_STATUS cpssDxChStcEgressAnalyzerIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcEgressAnalyzerIndexSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable, index));

    rc = internal_cpssDxChStcEgressAnalyzerIndexSet(devNum, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChStcEgressAnalyzerIndexGet
*
* DESCRIPTION:
*       Get the egress STC analyzer index.
*
*       feature behavior:
*       If a packet is marked for both egress STC and egress mirroring , the
*       packet is only replicated once, depending on a configurable priority
*       between the mechanisms. The configurable <STC analyzer index> determines
*       the priority of STC compared to mirroring to the analyzer.
*       Note:
*       1. that STC does not actually use this index to determine the target
*          of the mirrored packet.
*       2. the indexes used by egress mirroring engines are in range 0..6.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*      devNum    - device number.
*
*
* OUTPUTS:
*      enablePtr    - (pointer to) global enable/disable analyzer index for egress STC.
*                  - GT_TRUE - enable analyzer index.
*                  - GT_FALSE - no analyzer index.
*      indexPtr     - (pointer to) Analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function uses same parameters as mirroring functions like
*       cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet(devNum,enablePtr,indexPtr).
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChStcEgressAnalyzerIndexGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;        /* register address */
    GT_U32  analyzerIndex;/* analyzer index */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).
            statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCAnalyzerIndex;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 4, &analyzerIndex);

    if(analyzerIndex == 0)
    {
        *enablePtr = GT_FALSE;
        *indexPtr = 0;
    }
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = analyzerIndex - 1;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChStcEgressAnalyzerIndexGet
*
* DESCRIPTION:
*       Get the egress STC analyzer index.
*
*       feature behavior:
*       If a packet is marked for both egress STC and egress mirroring , the
*       packet is only replicated once, depending on a configurable priority
*       between the mechanisms. The configurable <STC analyzer index> determines
*       the priority of STC compared to mirroring to the analyzer.
*       Note:
*       1. that STC does not actually use this index to determine the target
*          of the mirrored packet.
*       2. the indexes used by egress mirroring engines are in range 0..6.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*      devNum    - device number.
*
*
* OUTPUTS:
*      enablePtr    - (pointer to) global enable/disable analyzer index for egress STC.
*                  - GT_TRUE - enable analyzer index.
*                  - GT_FALSE - no analyzer index.
*      indexPtr     - (pointer to) Analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function uses same parameters as mirroring functions like
*       cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet(devNum,enablePtr,indexPtr).
*
*******************************************************************************/
GT_STATUS cpssDxChStcEgressAnalyzerIndexGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChStcEgressAnalyzerIndexGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr, indexPtr));

    rc = internal_cpssDxChStcEgressAnalyzerIndexGet(devNum, enablePtr, indexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr, indexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


