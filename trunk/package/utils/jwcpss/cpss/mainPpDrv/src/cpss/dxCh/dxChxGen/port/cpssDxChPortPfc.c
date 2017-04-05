/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPfc.c
*
* DESCRIPTION:
*       CPSS implementation for Priority Flow Control functionality.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 41 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPfc.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>

/* LLFC support 78 ports in sip 5.20 */
#define LLFC_NUM_PORTS_SIP_5_20_CNS  78

/*******************************************************************************
* internal_cpssDxChPortPfcEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC (Priority Flow Control) response functionality.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       pfcEnable  - PFC enable option.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC enable option
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        If PFC response is enabled, the shaper’s baseline must be
*        at least 0x3FFFC0, see:
*        cpssDxChPortTxShaperBaselineSet.
*        Note: Triggering cannot be disabled by this API.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcEnableSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_ENABLE_ENT pfcEnable
)
{
    GT_U32 value;         /* register value */
    GT_U32 regAddr;       /* register address */
    GT_STATUS rc;         /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    if (pfcEnable==CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_ONLY_E)
    {
        value=0;
    }
    else if (pfcEnable==CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E)
    {
        value=1;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Set Enable Priority Based FC in TTI Unit Global Config reg. */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;
    }
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 24, 1, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Set value in Flow Control Response Config reg. */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).flowCtrlResponse.flowCtrlResponseConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dq.flowControl.flowControlConfig;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, value);
}

/*******************************************************************************
* cpssDxChPortPfcEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC (Priority Flow Control) response functionality.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       pfcEnable  - PFC enable option.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC enable option
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        If PFC response is enabled, the shaper’s baseline must be
*        at least 0x3FFFC0, see:
*        cpssDxChPortTxShaperBaselineSet.
*        Note: Triggering cannot be disabled by this API.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcEnableSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_ENABLE_ENT pfcEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pfcEnable));

    rc = internal_cpssDxChPortPfcEnableSet(devNum, pfcEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pfcEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcEnableGet
*
* DESCRIPTION:
*       Get the status of PFC response functionality.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       pfcEnablePtr  - (pointer to) PFC enable option.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Triggering is enabled by default.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcEnableGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_ENABLE_ENT *pfcEnablePtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 responseValue; /* registers value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(pfcEnablePtr);

    /* Get <PfcEn> field in the PFC Trigger Global Config reg.
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    rc =  prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &trigValue);
    if (rc != GT_OK)
    {
        return rc;
    }*/

    /* Get Enable Priority Based FC in TTI Unit Global Config reg. */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;
    }
    rc =  prvCpssHwPpGetRegField(devNum, regAddr, 24, 1, &responseValue);
    if (rc != GT_OK)
    {
        return rc;
    }
    *pfcEnablePtr = ((responseValue == 0) ?
            CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_ONLY_E :
            CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcEnableGet
*
* DESCRIPTION:
*       Get the status of PFC response functionality.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       pfcEnablePtr  - (pointer to) PFC enable option.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Triggering is enabled by default.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcEnableGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_ENABLE_ENT *pfcEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pfcEnablePtr));

    rc = internal_cpssDxChPortPfcEnableGet(devNum, pfcEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pfcEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcProfileIndexSet
*
* DESCRIPTION:
*       Binds a source port to a PFC profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcProfileIndexSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   profileIndex
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 fieldOffset;/* the start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    if (profileIndex >= BIT_3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCPortProfile[(portNum >> 3)];
        fieldOffset = (portNum%8)*3;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcSourcePortProfile;
        regAddr += (portNum/8)*4;
        fieldOffset = (portNum%8)*3;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr,fieldOffset, 3, profileIndex);
}

/*******************************************************************************
* cpssDxChPortPfcProfileIndexSet
*
* DESCRIPTION:
*       Binds a source port to a PFC profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileIndexSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   profileIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcProfileIndexSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, profileIndex));

    rc = internal_cpssDxChPortPfcProfileIndexSet(devNum, portNum, profileIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, profileIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcProfileIndexGet
*
* DESCRIPTION:
*       Gets the port's PFC profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcProfileIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *profileIndexPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
                    CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCPortProfile[(portNum >> 3)];
        fieldOffset = (portNum%8)*3;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcSourcePortProfile;
        regAddr += (portNum/8)*4;
        fieldOffset = (portNum%8)*3;
    }

    return prvCpssHwPpGetRegField(devNum, regAddr,fieldOffset, 3, profileIndexPtr);
}

/*******************************************************************************
* cpssDxChPortPfcProfileIndexGet
*
* DESCRIPTION:
*       Gets the port's PFC profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileIndexGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *profileIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcProfileIndexGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, profileIndexPtr));

    rc = internal_cpssDxChPortPfcProfileIndexGet(devNum, portNum, profileIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, profileIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       profileIndex - profile index (APPLICABLE RANGES: 0..7)
*       tcQueue      - traffic class queue (APPLICABLE RANGES: 0..7)
*       pfcProfileCfgPtr - pointer to PFC Profile configurations
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcProfileQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(pfcProfileCfgPtr);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if (profileIndex > 7 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (pfcProfileCfgPtr->xoffThreshold >= BIT_29 ||
            pfcProfileCfgPtr->xonThreshold >= BIT_29)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if (pfcProfileCfgPtr->xoffThreshold >= BIT_11 ||
            pfcProfileCfgPtr->xonThreshold >= BIT_11)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_E;

        rc = prvCpssDxChWriteTableEntry(devNum, tableType, ((profileIndex<<3) + tcQueue), &(pfcProfileCfgPtr->xoffThreshold));
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcProfileXoffThresholds;
        regAddr += profileIndex*0x20 + tcQueue*0x4;

        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 11, pfcProfileCfgPtr->xoffThreshold);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_E;

        return prvCpssDxChWriteTableEntry(devNum, tableType, ((profileIndex<<3) + tcQueue), &(pfcProfileCfgPtr->xonThreshold));
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcProfileXonThresholds;
        regAddr += profileIndex*0x20 + tcQueue*0x4;

        return prvCpssHwPpSetRegField(devNum, regAddr, 0, 11, pfcProfileCfgPtr->xonThreshold);
    }
}

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       profileIndex - profile index (APPLICABLE RANGES: 0..7)
*       tcQueue      - traffic class queue (APPLICABLE RANGES: 0..7)
*       pfcProfileCfgPtr - pointer to PFC Profile configurations
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcProfileQueueConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, tcQueue, pfcProfileCfgPtr));

    rc = internal_cpssDxChPortPfcProfileQueueConfigSet(devNum, profileIndex, tcQueue, pfcProfileCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, tcQueue, pfcProfileCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       profileIndex - profile index (APPLICABLE RANGES: 0..7)
*       tcQueue      - traffic class queue (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       pfcProfileCfgPtr - pointer to PFC Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcProfileQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(pfcProfileCfgPtr);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if (profileIndex > 7 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_E;

        rc = prvCpssDxChReadTableEntry(devNum, tableType, ((profileIndex<<3) + tcQueue), &(pfcProfileCfgPtr->xoffThreshold));
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcProfileXoffThresholds;
        regAddr += profileIndex*0x20 + tcQueue*0x4;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 11, &(pfcProfileCfgPtr->xoffThreshold));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        tableType = PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_E;

        return prvCpssDxChReadTableEntry(devNum, tableType, ((profileIndex<<3) + tcQueue), &(pfcProfileCfgPtr->xonThreshold));
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcProfileXonThresholds;
        regAddr += profileIndex*0x20 + tcQueue*0x4;
        return prvCpssHwPpGetRegField(devNum, regAddr, 0, 11, &(pfcProfileCfgPtr->xonThreshold));
    }
}

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       profileIndex - profile index (APPLICABLE RANGES: 0..7)
*       tcQueue      - traffic class queue (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       pfcProfileCfgPtr - pointer to PFC Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcProfileQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC     *pfcProfileCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcProfileQueueConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, tcQueue, pfcProfileCfgPtr));

    rc = internal_cpssDxChPortPfcProfileQueueConfigGet(devNum, profileIndex, tcQueue, pfcProfileCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, tcQueue, pfcProfileCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcCountingModeSet
*
* DESCRIPTION:
*       Sets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcCountMode - PFC counting mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcCountingModeSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  pfcCountMode
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    switch(pfcCountMode)
    {
        case CPSS_DXCH_PORT_PFC_COUNT_BUFFERS_MODE_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_PFC_COUNT_PACKETS_E:
            value = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* For lion2 and above configure TXQ unit as well */
    if (PRV_CPSS_PP_MAC(devNum)->appDevFamily != CPSS_LION_E)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) != GT_TRUE)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.llfcRegs.pfcGlobalConfigReg;
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, value);
            if (rc != GT_OK)
            {
                    return rc;
            }
        }
    }

    /* TXQ unit in Lion, CPFC unit in Lion2 and above*/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCTriggerGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    }
    return prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, value);

}

/*******************************************************************************
* cpssDxChPortPfcCountingModeSet
*
* DESCRIPTION:
*       Sets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcCountMode - PFC counting mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCountingModeSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  pfcCountMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcCountingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pfcCountMode));

    rc = internal_cpssDxChPortPfcCountingModeSet(devNum, pfcCountMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pfcCountMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcCountingModeGet
*
* DESCRIPTION:
*       Gets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       pfcCountModePtr - (pointer to) PFC counting mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcCountingModeGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  *pfcCountModePtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(pfcCountModePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCTriggerGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    }
    rc =  prvCpssHwPpGetRegField(devNum, regAddr, 1, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *pfcCountModePtr = (CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT)value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcCountingModeGet
*
* DESCRIPTION:
*       Gets PFC counting mode.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       pfcCountModePtr - (pointer to) PFC counting mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCountingModeGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT  *pfcCountModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcCountingModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pfcCountModePtr));

    rc = internal_cpssDxChPortPfcCountingModeGet(devNum, pfcCountModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pfcCountModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcGlobalDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable PFC global drop.
*                    GT_FALSE: Disable PFC global drop.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To configure drop threshold use cpssDxChPortPfcGlobalQueueConfigSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcGlobalDropEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_U32 bitIndex;  /* bit index */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        bitIndex = 10;
    }
    else
    {
        bitIndex = 9;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCTriggerGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    }
    return prvCpssHwPpSetRegField(devNum, regAddr, bitIndex, 1, value);
}

/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable PFC global drop.
*                    GT_FALSE: Disable PFC global drop.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To configure drop threshold use cpssDxChPortPfcGlobalQueueConfigSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalDropEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcGlobalDropEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPortPfcGlobalDropEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcGlobalDropEnableGet
*
* DESCRIPTION:
*       Gets the current status of PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of PFC functionality
*                     GT_TRUE:  PFC global drop enabled.
*                     GT_FALSE: PFC global drop disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcGlobalDropEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 bitIndex;  /* bit index */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        bitIndex = 10;
    }
    else
    {
        bitIndex = 9;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCTriggerGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.pfcRegs.pfcGlobalConfigReg;
    }
    rc =  prvCpssHwPpGetRegField(devNum, regAddr, bitIndex, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableGet
*
* DESCRIPTION:
*       Gets the current status of PFC global drop.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of PFC functionality
*                     GT_TRUE:  PFC global drop enabled.
*                     GT_FALSE: PFC global drop disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalDropEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcGlobalDropEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPortPfcGlobalDropEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcGlobalQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum        - device number
*       tcQueue       - traffic class queue (APPLICABLE RANGES: 0..7)
*       xoffThreshold - XOFF threshold
*                       (APPLICABLE RANGES: Lion, Lion2 0..0x7FF)
*                       (APPLICABLE RANGES: Bobcat2, Caelum, Bobcat3 0..0x1FFFFFFF)
*       dropThreshold - Drop threshold. When a global counter with given tcQueue
*                       crosses up the dropThreshold the packets are dropped
*                       (APPLICABLE RANGES: Lion, Lion2 0..0x7FF)
*                       (APPLICABLE RANGES: Bobcat2, Caelum, Bobcat3 0..0x1FFFFFFF)
*       xonThreshold  - XON threshold
*                       (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                       (APPLICABLE RANGES: 0..0x1FFFFFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. dropThreshold is used when PFC global drop is enabled.
*       See cpssDxChPortPfcGlobalDropEnableSet.
*       2. All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcGlobalQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_U32   xoffThreshold,
    IN GT_U32   dropThreshold,
    IN GT_U32   xonThreshold
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 value;     /* register value */
    GT_STATUS rc;     /* function return value */
    GT_U32 portGroupsNum; /* number of port groups in device*/
    GT_U32 i;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (xoffThreshold >= BIT_29 || dropThreshold >= BIT_29 || xonThreshold >= BIT_29)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if (xoffThreshold >= BIT_11 || dropThreshold >= BIT_11)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCGroupOfPortsTCXoffThresholds[tcQueue][0];
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 29, xoffThreshold);
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCGroupOfPortsTCDropThresholds[tcQueue][0];
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 29, dropThreshold);
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCGroupOfPortsTCXonThresholds[tcQueue][0];
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 29, xonThreshold);
        if (rc != GT_OK)
        {
            return rc;
        }

    }
    else
    {
        value = xoffThreshold | (dropThreshold << 11);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.pfcRegs.pfcGlobaGroupOfPortsThresholds;
        regAddr += tcQueue*0x4;

        portGroupsNum = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ?
            PRV_CPSS_LION_PORT_GROUPS_NUM_CNS : PRV_CPSS_LION2_PORT_GROUPS_NUM_CNS;

        /* set the value to all port groups*/
        for (i=0; i < portGroupsNum; i++)
        {
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 22, value);
            if (rc != GT_OK)
            {
                return rc;
            }
            regAddr += 0x20;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum        - device number
*       tcQueue       - traffic class queue (APPLICABLE RANGES: 0..7)
*       xoffThreshold - XOFF threshold
*                       (APPLICABLE RANGES: Lion, Lion2 0..0x7FF)
*                       (APPLICABLE RANGES: Bobcat2, Caelum, Bobcat3 0..0x1FFFFFFF)
*       dropThreshold - Drop threshold. When a global counter with given tcQueue
*                       crosses up the dropThreshold the packets are dropped
*                       (APPLICABLE RANGES: Lion, Lion2 0..0x7FF)
*                       (APPLICABLE RANGES: Bobcat2, Caelum, Bobcat3 0..0x1FFFFFFF)
*       xonThreshold  - XON threshold
*                       (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                       (APPLICABLE RANGES: 0..0x1FFFFFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. dropThreshold is used when PFC global drop is enabled.
*       See cpssDxChPortPfcGlobalDropEnableSet.
*       2. All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalQueueConfigSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_U32   xoffThreshold,
    IN GT_U32   dropThreshold,
    IN GT_U32   xonThreshold
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcGlobalQueueConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueue, xoffThreshold, dropThreshold, xonThreshold));

    rc = internal_cpssDxChPortPfcGlobalQueueConfigSet(devNum, tcQueue, xoffThreshold, dropThreshold, xonThreshold);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueue, xoffThreshold, dropThreshold, xonThreshold));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcGlobalQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       tcQueue      - traffic class queue (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       xoffThresholdPtr - (pointer to) Xoff threshold.
*       dropThresholdPtr - (pointer to) Drop threshold. When a global counter with given tcQueue
*                       crosses up the dropThreshold the packets are dropped.
*       xonThresholdPtr  - (pointer to) Xon threshold.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcGlobalQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *xoffThresholdPtr,
    OUT GT_U32   *dropThresholdPtr,
    OUT GT_U32   *xonThresholdPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 value;     /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(xoffThresholdPtr);
    CPSS_NULL_PTR_CHECK_MAC(dropThresholdPtr);
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        CPSS_NULL_PTR_CHECK_MAC(xonThresholdPtr);
    }
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCGroupOfPortsTCXoffThresholds[tcQueue][0];
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 29, xoffThresholdPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCGroupOfPortsTCDropThresholds[tcQueue][0];
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 29, dropThresholdPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCGroupOfPortsTCXonThresholds[tcQueue][0];
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 29, xonThresholdPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.pfcRegs.pfcGlobaGroupOfPortsThresholds;
        regAddr += tcQueue*0x4;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 22, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
        *xoffThresholdPtr = value & 0x7FF;
        *dropThresholdPtr = value >> 11;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       tcQueue      - traffic class queue (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       xoffThresholdPtr - (pointer to) Xoff threshold.
*       dropThresholdPtr - (pointer to) Drop threshold. When a global counter with given tcQueue
*                       crosses up the dropThreshold the packets are dropped.
*       xonThresholdPtr  - (pointer to) Xon threshold.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in buffers or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcGlobalQueueConfigGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *xoffThresholdPtr,
    OUT GT_U32   *dropThresholdPtr,
    OUT GT_U32   *xonThresholdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcGlobalQueueConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueue, xoffThresholdPtr, dropThresholdPtr, xonThresholdPtr));

    rc = internal_cpssDxChPortPfcGlobalQueueConfigGet(devNum, tcQueue, xoffThresholdPtr, dropThresholdPtr, xonThresholdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueue, xoffThresholdPtr, dropThresholdPtr, xonThresholdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcTimerMapEnableSet
*
* DESCRIPTION:
*       Enables mapping of PFC timer to priority queue for given scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile.
*       enable     - Determines whether PFC timer to Priority Queue map
*                    is used.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcTimerMapEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_BOOL                                 enable

)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);

    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).flowCtrlResponse.schedulerProfilePFCTimerToPriorityQueueMapEnable[profileSet];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dq.flowControl.schedulerProfileTcToPriorityMapEnable[profileSet];
    }
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, value);
}

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableSet
*
* DESCRIPTION:
*       Enables mapping of PFC timer to priority queue for given scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile.
*       enable     - Determines whether PFC timer to Priority Queue map
*                    is used.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerMapEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_BOOL                                 enable

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcTimerMapEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileSet, enable));

    rc = internal_cpssDxChPortPfcTimerMapEnableSet(devNum, profileSet, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileSet, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcTimerMapEnableGet
*
* DESCRIPTION:
*       Get the status of PFS timer to priority queue mapping for given
*       scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile Set
*
* OUTPUTS:
*       enablePtr   - (pointer to) status of PFC timer to Priority Queue
*                      mapping.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcTimerMapEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT GT_BOOL                                *enablePtr

)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).flowCtrlResponse.schedulerProfilePFCTimerToPriorityQueueMapEnable[profileSet];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dq.flowControl.schedulerProfileTcToPriorityMapEnable[profileSet];
    }
    rc =  prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableGet
*
* DESCRIPTION:
*       Get the status of PFS timer to priority queue mapping for given
*       scheduler profile.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       profileSet - the Tx Queue scheduler profile Set
*
* OUTPUTS:
*       enablePtr   - (pointer to) status of PFC timer to Priority Queue
*                      mapping.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerMapEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    OUT GT_BOOL                                *enablePtr

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcTimerMapEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileSet, enablePtr));

    rc = internal_cpssDxChPortPfcTimerMapEnableGet(devNum, profileSet, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileSet, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcTimerToQueueMapSet
*
* DESCRIPTION:
*       Sets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcTimer     - PFC timer (0..7)
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_OUT_OF_RANGE          - on out of traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcTimerToQueueMapSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    IN  GT_U32    tcQueue

)
{
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if (pfcTimer >= 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).flowCtrlResponse.PFCTimerToPriorityQueueMap[pfcTimer];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dq.flowControl.pfcTimerToPriorityMapping[pfcTimer];
    }
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 3, tcQueue);
}

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapSet
*
* DESCRIPTION:
*       Sets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       pfcTimer     - PFC timer (0..7)
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_OUT_OF_RANGE          - on out of traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerToQueueMapSet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    IN  GT_U32    tcQueue

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcTimerToQueueMapSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pfcTimer, tcQueue));

    rc = internal_cpssDxChPortPfcTimerToQueueMapSet(devNum, pfcTimer, tcQueue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pfcTimer, tcQueue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcTimerToQueueMapGet
*
* DESCRIPTION:
*       Gets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       pfcTimer        - PFC timer (0..7).
*
* OUTPUTS:
*       tcQueuePtr      - (pointer to) traffic class queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcTimerToQueueMapGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    OUT GT_U32    *tcQueuePtr

)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(tcQueuePtr);
    if (pfcTimer >= 8)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).flowCtrlResponse.PFCTimerToPriorityQueueMap[pfcTimer];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dq.flowControl.pfcTimerToPriorityMapping[pfcTimer];
    }

    rc =  prvCpssHwPpGetRegField(devNum, regAddr, 0, 3, tcQueuePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapGet
*
* DESCRIPTION:
*       Gets PFC timer to priority queue map.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       pfcTimer        - PFC timer (0..7).
*
* OUTPUTS:
*       tcQueuePtr      - (pointer to) traffic class queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTimerToQueueMapGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    pfcTimer,
    OUT GT_U32    *tcQueuePtr

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcTimerToQueueMapGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, pfcTimer, tcQueuePtr));

    rc = internal_cpssDxChPortPfcTimerToQueueMapGet(devNum, pfcTimer, tcQueuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, pfcTimer, tcQueuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcShaperToPortRateRatioSet
*
* DESCRIPTION:
*       Sets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*       shaperToPortRateRatio - shaper rate to port speed ratio
*                               in percentage (0..100).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range shaper rate to port speed ratio
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcShaperToPortRateRatioSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    IN  GT_U32                                  shaperToPortRateRatio

)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    if (shaperToPortRateRatio > 100)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (shaperToPortRateRatio == 0)
    {
        value = 0;
    }
    else
    {
        /* PFCFactor = ROUNDUP(shaperToPortRateRatio*128/100) - 1 */
        value = (shaperToPortRateRatio*128)/100 -
            (((shaperToPortRateRatio*128)%100 == 0) ? 1 :0);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).flowCtrlResponse.schedulerProfilePriorityQueuePFCFactor[profileSet][tcQueue];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dq.flowControl.tcProfilePfcFactor[tcQueue][profileSet];
    }
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 7, value);
}

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioSet
*
* DESCRIPTION:
*       Sets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*       shaperToPortRateRatio - shaper rate to port speed ratio
*                               in percentage (0..100).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range shaper rate to port speed ratio
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    IN  GT_U32                                  shaperToPortRateRatio

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcShaperToPortRateRatioSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileSet, tcQueue, shaperToPortRateRatio));

    rc = internal_cpssDxChPortPfcShaperToPortRateRatioSet(devNum, profileSet, tcQueue, shaperToPortRateRatio);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileSet, tcQueue, shaperToPortRateRatio));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcShaperToPortRateRatioGet
*
* DESCRIPTION:
*       Gets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*
* OUTPUTS:
*       shaperToPortRateRatioPtr - (pointer to)shaper rate to port speed ratio
*                               in percentage.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcShaperToPortRateRatioGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    OUT GT_U32                                 *shaperToPortRateRatioPtr

)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(shaperToPortRateRatioPtr);
    PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet, devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).flowCtrlResponse.schedulerProfilePriorityQueuePFCFactor[profileSet][tcQueue];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dq.flowControl.tcProfilePfcFactor[tcQueue][profileSet];
    }

    rc =  prvCpssHwPpGetRegField(devNum, regAddr, 0, 7, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (value == 0)
    {
        *shaperToPortRateRatioPtr = 0;
    }
    else
    {
        /* shaperToPortRateRatio = ROUNDDOWN(PFCFactor*100/128) + 1 */
        *shaperToPortRateRatioPtr = (value*100)/128 + 1;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioGet
*
* DESCRIPTION:
*       Gets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum                - device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*
* OUTPUTS:
*       shaperToPortRateRatioPtr - (pointer to)shaper rate to port speed ratio
*                               in percentage.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcShaperToPortRateRatioGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet,
    IN  GT_U8                                   tcQueue,
    OUT GT_U32                                 *shaperToPortRateRatioPtr

)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcShaperToPortRateRatioGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileSet, tcQueue, shaperToPortRateRatioPtr));

    rc = internal_cpssDxChPortPfcShaperToPortRateRatioGet(devNum, profileSet, tcQueue, shaperToPortRateRatioPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileSet, tcQueue, shaperToPortRateRatioPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPfcCascadeEnableSet
*
* DESCRIPTION:
*       Enable/Disable insertion of DSA tag for PFC frames
*       transmitted on given port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*       enable     - GT_TRUE - PFC packets are transmitted with a DSA tag.
*                    GT_FALSE - PFC packets are transmitted without DSA tag.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPfcCascadeEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 value;      /* register value */
    GT_U32 portMacNum; /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum, portMacNum);

    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).FCACtrl;

        /* set PFC Cascade Port Enable */
        return prvCpssHwPpPortGroupSetRegField(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                regAddr, 9, 1, value);
    }
    else
    {
        if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
            regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
            regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
            regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 3;
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
            regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
            regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
            regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 15;
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
            regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
            regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
            regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 15;
        }

        return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    }
}

/*******************************************************************************
* internal_cpssDxChPortPfcForwardEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of PFC frames to the ingress
*       pipeline of a specified port.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number.
*       portNum - physical port number (CPU port not supported)
*       enable  - GT_TRUE:  forward PFC frames to the ingress pipe,
*                 GT_FALSE: do not forward PFC frames to the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcForwardEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_BOOL enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 value;      /* register value */
    GT_U32 portMacNum;      /* MAC number */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    value = BOOL2BIT_MAC(enable);

    if (CPSS_PP_FAMILY_DXCH_XCAT2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                    perPortRegs[portMacNum].serialParameters1;
        return prvCpssDrvHwPpPortGroupSetRegField(devNum,
                                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                        regAddr, 1, 1, value);
    }

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].serialParameters1;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 1;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 6;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 6;
    }

    return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
}

/*******************************************************************************
* cpssDxChPortPfcForwardEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of PFC frames to the ingress
*       pipeline of a specified port.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number.
*       portNum - physical port number (CPU port not supported)
*       enable  - GT_TRUE:  forward PFC frames to the ingress pipe,
*                 GT_FALSE: do not forward PFC frames to the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcForwardEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcForwardEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortPfcForwardEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortPfcForwardEnableGet
*
* DESCRIPTION:
*       Get status of PFC frames forwarding
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr   - current forward PFC frames status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcForwardEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* return code */
    GT_U32 offset;     /* bit number inside register       */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* type of mac unit of port */
    GT_U32          portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].serialParameters1;
        if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 1, 1, &value)) != GT_OK)
            return rc;
    }
    else
    {
        portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum);
        PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,portMacType,&regAddr);
        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            offset = 6;
        }
        else
        {
            if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].serialParameters1;
            }

            offset = 1;
        }

        if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset, 1, &value)) != GT_OK)
            return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcForwardEnableGet
*
* DESCRIPTION:
*       Get status of PFC frames forwarding
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr   - current forward PFC frames status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcForwardEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcForwardEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortPfcForwardEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcLossyDropQueueEnableSet
*
* DESCRIPTION:
*       Enable/Disable lossy drop for packets with given traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*       enable     - GT_TRUE: Lossy - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are dropped.
*                  - GT_FALSE: Lossless - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are not dropped.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The device provides a burst absortion system designed to sustain contain
*       the packets in case of an incast type traffic (many-to-one).
*       Two thresholds are used:
*       1. Lossy drop - packets packets with lossy TC are dropped in
*          congested port group.
*       2. PFC Xoff threshold - PFC is sent to all of the ports in the port group
*          with all timers set to 0xFFFF.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcLossyDropQueueEnableSet
(
    IN GT_U8     devNum,
    IN GT_U8     tcQueue,
    IN GT_BOOL   enable
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 value;     /* register value */
    GT_U32 bitIndex;  /* bit index of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E );

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.lossyDropConfig;
        bitIndex = tcQueue;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.globalConfig.dualDevIdAndLossyDropConfig;
        bitIndex = tcQueue + 1;
    }

    value = BOOL2BIT_MAC(enable);
    return prvCpssHwPpSetRegField(devNum, regAddr, bitIndex, 1, value);
}

/*******************************************************************************
* cpssDxChPortPfcLossyDropQueueEnableSet
*
* DESCRIPTION:
*       Enable/Disable lossy drop for packets with given traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*       enable     - GT_TRUE: Lossy - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are dropped.
*                  - GT_FALSE: Lossless - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are not dropped.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The device provides a burst absortion system designed to sustain contain
*       the packets in case of an incast type traffic (many-to-one).
*       Two thresholds are used:
*       1. Lossy drop - packets packets with lossy TC are dropped in
*          congested port group.
*       2. PFC Xoff threshold - PFC is sent to all of the ports in the port group
*          with all timers set to 0xFFFF.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcLossyDropQueueEnableSet
(
    IN GT_U8     devNum,
    IN GT_U8     tcQueue,
    IN GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcLossyDropQueueEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueue, enable));

    rc = internal_cpssDxChPortPfcLossyDropQueueEnableSet(devNum, tcQueue, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueue, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcLossyDropQueueEnableGet
*
* DESCRIPTION:
*       Get the status of lossy drop on given traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*
* OUTPUTS:
*       enablePtr  (pointer to)
*                  - GT_TRUE: Lossy - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are dropped.
*                  - GT_FALSE: Lossless - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are not dropped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcLossyDropQueueEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 value;     /* register value */
    GT_U32 bitIndex;  /* bit index of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E );

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.lossyDropConfig;
        bitIndex = tcQueue;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.globalConfig.dualDevIdAndLossyDropConfig;
        bitIndex = tcQueue + 1;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitIndex, 1, &value);
    if (rc != GT_OK)
        return rc;
    *enablePtr = BIT2BOOL_MAC(value);
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcLossyDropQueueEnableGet
*
* DESCRIPTION:
*       Get the status of lossy drop on given traffic class queue.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*
* OUTPUTS:
*       enablePtr  (pointer to)
*                  - GT_TRUE: Lossy - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are dropped.
*                  - GT_FALSE: Lossless - when lossy drop threshold is
*                    reached, packets assigned with given tcQueue are not dropped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcLossyDropQueueEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     tcQueue,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcLossyDropQueueEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueue, enablePtr));

    rc = internal_cpssDxChPortPfcLossyDropQueueEnableGet(devNum, tcQueue, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueue, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcLossyDropConfigSet
*
* DESCRIPTION:
*       Set lossy drop configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       lossyDropConfigPtr - (pointer to) lossy drop configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The device provides a burst absortion system designed to sustain contain
*       the packets in case of an incast type traffic (many-to-one).
*       Two thresholds are used:
*       1. Lossy drop - packets packets with lossy TC are dropped in
*          congested port group.
*       2. PFC Xoff threshold - PFC is sent to all of the ports in the port group
*          with all timers set to 0xFFFF.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcLossyDropConfigSet
(
    IN GT_U8     devNum,
    IN CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC *lossyDropConfigPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 value;     /* register value */
    GT_U32 bitIndex;  /* bit index of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E );

    CPSS_NULL_PTR_CHECK_MAC(lossyDropConfigPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.lossyDropConfig;
        bitIndex = 8;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.globalConfig.dualDevIdAndLossyDropConfig;
        bitIndex = 9;
    }

    value = (BOOL2BIT_MAC(lossyDropConfigPtr->fromCpuLossyDropEnable) << 2) |
            (BOOL2BIT_MAC(lossyDropConfigPtr->toTargetSnifferLossyDropEnable) << 1) |
             BOOL2BIT_MAC(lossyDropConfigPtr->toCpuLossyDropEnable);
    return prvCpssHwPpSetRegField(devNum, regAddr, bitIndex, 3, value);
}

/*******************************************************************************
* cpssDxChPortPfcLossyDropConfigSet
*
* DESCRIPTION:
*       Set lossy drop configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       lossyDropConfigPtr - (pointer to) lossy drop configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The device provides a burst absortion system designed to sustain contain
*       the packets in case of an incast type traffic (many-to-one).
*       Two thresholds are used:
*       1. Lossy drop - packets packets with lossy TC are dropped in
*          congested port group.
*       2. PFC Xoff threshold - PFC is sent to all of the ports in the port group
*          with all timers set to 0xFFFF.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcLossyDropConfigSet
(
    IN GT_U8     devNum,
    IN CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC *lossyDropConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcLossyDropConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lossyDropConfigPtr));

    rc = internal_cpssDxChPortPfcLossyDropConfigSet(devNum, lossyDropConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lossyDropConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcLossyDropConfigGet
*
* DESCRIPTION:
*       Get lossy drop configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       lossyDropConfigPtr - (pointer to) lossy drop configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The device provides a burst absortion system designed to sustain contain
*       the packets in case of an incast type traffic (many-to-one).
*       Two thresholds are used:
*       1. Lossy drop - packets packets with lossy TC are dropped in
*          congested port group.
*       2. PFC Xoff threshold - PFC is sent to all of the ports in the port group
*          with all timers set to 0xFFFF.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcLossyDropConfigGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC *lossyDropConfigPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 value;     /* register value */
    GT_U32 bitIndex;  /* bit index of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E );

    CPSS_NULL_PTR_CHECK_MAC(lossyDropConfigPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.lossyDropConfig;
        bitIndex = 8;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.globalConfig.dualDevIdAndLossyDropConfig;
        bitIndex = 9;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitIndex, 3, &value);
    if (rc != GT_OK)
        return rc;

    lossyDropConfigPtr->fromCpuLossyDropEnable = BIT2BOOL_MAC(((value >> 2) & 1));
    lossyDropConfigPtr->toTargetSnifferLossyDropEnable = BIT2BOOL_MAC(((value >> 1) & 1));
    lossyDropConfigPtr->toCpuLossyDropEnable = BIT2BOOL_MAC((value & 1));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPfcLossyDropConfigGet
*
* DESCRIPTION:
*       Get lossy drop configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       lossyDropConfigPtr - (pointer to) lossy drop configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The device provides a burst absortion system designed to sustain contain
*       the packets in case of an incast type traffic (many-to-one).
*       Two thresholds are used:
*       1. Lossy drop - packets packets with lossy TC are dropped in
*          congested port group.
*       2. PFC Xoff threshold - PFC is sent to all of the ports in the port group
*          with all timers set to 0xFFFF.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcLossyDropConfigGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_PORT_PFC_LOSSY_DROP_CONFIG_STC *lossyDropConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcLossyDropConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lossyDropConfigPtr));

    rc = internal_cpssDxChPortPfcLossyDropConfigGet(devNum, lossyDropConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lossyDropConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcTerminateFramesEnableSet
*
* DESCRIPTION:
*       Enable/Disable the ingress control pipe to parse and terminate received
*       flow control frames (PRC/802.3x)
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number.
*       enable      - enable termination of flow control frames
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       If the configuration is disabled, flow control frames are processed by
*       the control pipe as regular data frames (that is they are subject to
*       learning, forwarding, filtering and mirroring) but they are not
*       terminated and the PFC timers are not extracted.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcTerminateFramesEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;
    GT_U32      hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;

    hwData = BOOL2BIT_MAC(enable);
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 29, 1, hwData);
}

/*******************************************************************************
* cpssDxChPortPfcTerminateFramesEnableSet
*
* DESCRIPTION:
*       Enable/Disable the ingress control pipe to parse and terminate received
*       flow control frames (PRC/802.3x)
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number.
*       enable      - enable termination of flow control frames
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       If the configuration is disabled, flow control frames are processed by
*       the control pipe as regular data frames (that is they are subject to
*       learning, forwarding, filtering and mirroring) but they are not
*       terminated and the PFC timers are not extracted.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTerminateFramesEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcTerminateFramesEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPortPfcTerminateFramesEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcTerminateFramesEnableGet
*
* DESCRIPTION:
*       Get whether the ingress control pipe to parse and terminate received
*       flow control frames (PRC/802.3x) is enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) termination of flow control frames enabling
*                     status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       If the configuration is disabled, flow control frames are processed by
*       the control pipe as regular data frames (that is they are subject to
*       learning, forwarding, filtering and mirroring) but they are not
*       terminated and the PFC timers are not extracted.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcTerminateFramesEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      regAddr;
    GT_U32      hwData;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 29, 1, &hwData);
    if (rc == GT_OK)
    {
        *enablePtr = BIT2BOOL_MAC(hwData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortPfcTerminateFramesEnableGet
*
* DESCRIPTION:
*       Get whether the ingress control pipe to parse and terminate received
*       flow control frames (PRC/802.3x) is enabled
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) termination of flow control frames enabling
*                     status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       If the configuration is disabled, flow control frames are processed by
*       the control pipe as regular data frames (that is they are subject to
*       learning, forwarding, filtering and mirroring) but they are not
*       terminated and the PFC timers are not extracted.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcTerminateFramesEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcTerminateFramesEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPortPfcTerminateFramesEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortUnrecognizedMacControlFramesCmdSet
*
* DESCRIPTION:
*       Set the unrecognized MAC control frames command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       command         - the command. valid values:
*                           CPSS_PACKET_CMD_FORWARD_E
*                           CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                           CPSS_PACKET_CMD_DROP_HARD_E
*                           CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or command
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortUnrecognizedMacControlFramesCmdSet
(
    IN GT_U8                    devNum,
    IN CPSS_PACKET_CMD_ENT      command
)
{
    GT_U32      regAddr;
    GT_U32      value;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,command);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 14, 3, value);
}

/*******************************************************************************
* cpssDxChPortUnrecognizedMacControlFramesCmdSet
*
* DESCRIPTION:
*       Set the unrecognized MAC control frames command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       command         - the command. valid values:
*                           CPSS_PACKET_CMD_FORWARD_E
*                           CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                           CPSS_PACKET_CMD_DROP_HARD_E
*                           CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or command
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortUnrecognizedMacControlFramesCmdSet
(
    IN GT_U8                    devNum,
    IN CPSS_PACKET_CMD_ENT      command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortUnrecognizedMacControlFramesCmdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, command));

    rc = internal_cpssDxChPortUnrecognizedMacControlFramesCmdSet(devNum, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortUnrecognizedMacControlFramesCmdGet
*
* DESCRIPTION:
*       Get the unrecognized MAC control frames command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       commandPtr      - (pointer to) the command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortUnrecognizedMacControlFramesCmdGet
(
    IN  GT_U8                   devNum,
    OUT CPSS_PACKET_CMD_ENT     *commandPtr
)
{
    GT_U32      regAddr;
    GT_U32      value;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 14, 3, &value);
    if (rc == GT_OK)
    {
        PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(*commandPtr,value);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortUnrecognizedMacControlFramesCmdGet
*
* DESCRIPTION:
*       Get the unrecognized MAC control frames command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       commandPtr      - (pointer to) the command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortUnrecognizedMacControlFramesCmdGet
(
    IN  GT_U8                   devNum,
    OUT CPSS_PACKET_CMD_ENT     *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortUnrecognizedMacControlFramesCmdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, commandPtr));

    rc = internal_cpssDxChPortUnrecognizedMacControlFramesCmdGet(devNum, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortFlowControlPacketsCntGet
*
* DESCRIPTION:
*       Get the number of received and dropped Flow Control packets
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       receivedCntPtr  - the number of received packets
*       droppedCntPtr   - the number of dropped packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are cleared on read.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortFlowControlPacketsCntGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *receivedCntPtr,
    OUT GT_U32      *droppedCntPtr
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(receivedCntPtr);
    CPSS_NULL_PTR_CHECK_MAC(droppedCntPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).receivedFlowCtrlPktsCntr;
    rc = prvCpssPortGroupsCounterSummary(devNum, regAddr, 0, 16, receivedCntPtr,NULL);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).droppedFlowCtrlPktsCntr;
    return prvCpssPortGroupsCounterSummary(devNum, regAddr, 0, 16, droppedCntPtr,NULL);
}

/*******************************************************************************
* cpssDxChPortFlowControlPacketsCntGet
*
* DESCRIPTION:
*       Get the number of received and dropped Flow Control packets
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       receivedCntPtr  - the number of received packets
*       droppedCntPtr   - the number of dropped packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are cleared on read.
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlPacketsCntGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *receivedCntPtr,
    OUT GT_U32      *droppedCntPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortFlowControlPacketsCntGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, receivedCntPtr, droppedCntPtr));

    rc = internal_cpssDxChPortFlowControlPacketsCntGet(devNum, receivedCntPtr, droppedCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, receivedCntPtr, droppedCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcQueueCounterGet
*
* DESCRIPTION:
*       Get the number buffers/packets per proirity queue.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum     - device number
*       tcQueue    - traffic class queue on the port
*
* OUTPUTS:
*       cntPtr     - (pointer to) number buffers/packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, tcQueue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       Buffer or packet counting mode is set by cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcQueueCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   tcQueue,
    OUT GT_U32  *cntPtr
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(cntPtr);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCGroupOfPortsTCCntr[0][tcQueue];
    rc = prvCpssPortGroupsCounterSummary(devNum, regAddr, 0, 29, cntPtr, NULL);

    return rc;
}

/*******************************************************************************
* cpssDxChPortPfcQueueCounterGet
*
* DESCRIPTION:
*       Get the number buffers/packets per proirity queue.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum     - device number
*       tcQueue    - traffic class queue on the port
*
* OUTPUTS:
*       cntPtr     - (pointer to) number buffers/packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, tcQueue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       Buffer or packet counting mode is set by cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcQueueCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   tcQueue,
    OUT GT_U32  *cntPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcQueueCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueue, cntPtr));

    rc = internal_cpssDxChPortPfcQueueCounterGet(devNum, tcQueue, cntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueue, cntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcSourcePortToPfcCounterSet
*
* DESCRIPTION:
*       Set PFC mapping of a physical port to a PFC counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       pfcCounterNum   - PFC counter (APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_OUT_OF_RANGE          - on wrong pfcCounterNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcSourcePortToPfcCounterSet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32  pfcCounterNum
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    if (pfcCounterNum >= BIT_7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCSourcePortToPFCIndexMap[portNum];

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 7, pfcCounterNum);
}

/*******************************************************************************
* cpssDxChPortPfcSourcePortToPfcCounterSet
*
* DESCRIPTION:
*       Set PFC mapping of a physical port to a PFC counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       pfcCounterNum   - PFC counter (APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_OUT_OF_RANGE          - on wrong pfcCounterNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcSourcePortToPfcCounterSet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32  pfcCounterNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcSourcePortToPfcCounterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, pfcCounterNum));

    rc = internal_cpssDxChPortPfcSourcePortToPfcCounterSet(devNum, portNum, pfcCounterNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, pfcCounterNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortPfcSourcePortToPfcCounterGet
*
* DESCRIPTION:
*       Get PFC mapping of a physical port to a PFC counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       pfcCounterNumPtr- (pointer to) PFC counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcSourcePortToPfcCounterGet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *pfcCounterNumPtr
)
{
    GT_U32  regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(pfcCounterNumPtr);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum, portNum);

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(devNum).PFCSourcePortToPFCIndexMap[portNum];

    return prvCpssHwPpGetRegField(devNum, regAddr, 0, 7, pfcCounterNumPtr);
}

/*******************************************************************************
* cpssDxChPortPfcSourcePortToPfcCounterGet
*
* DESCRIPTION:
*       Get PFC mapping of a physical port to a PFC counter.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       pfcCounterNumPtr- (pointer to) PFC counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcSourcePortToPfcCounterGet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *pfcCounterNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcSourcePortToPfcCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, pfcCounterNumPtr));

    rc = internal_cpssDxChPortPfcSourcePortToPfcCounterGet(devNum, portNum, pfcCounterNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, pfcCounterNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortPfcCounterGet
*
* DESCRIPTION:
*       Get PFC counter value per PFC counter and traffic class.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       tcQueue         - traffic class queue (APPLICABLE RANGES: 0..7)
*       pfcCounterNum   - PFC counter (APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       pfcCounterValuePtr - (pointer to) PFC counter value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_OUT_OF_RANGE          - on wrong pfcCounterNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortPfcCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   tcQueue,
    IN  GT_U32  pfcCounterNum,
    OUT GT_U32  *pfcCounterValuePtr
)
{
    GT_STATUS rc;  /* return code */
    PRV_CPSS_DXCH_TABLE_ENT tableType;
    GT_U32  maxNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(pfcCounterValuePtr);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        maxNum  = LLFC_NUM_PORTS_SIP_5_20_CNS;
    }
    else
    {
        maxNum = 128;
    }

    if (pfcCounterNum >= maxNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, "pfcCounterNum[%d] is out of range [0..%d]",
            pfcCounterNum , maxNum-1);
    }

    tableType = PRV_CPSS_DXCH_LION3_TABLE_PFC_LLFC_COUNTERS_E;

    rc = prvCpssDxChReadTableEntry(devNum, tableType, ((pfcCounterNum<<3) + tcQueue), pfcCounterValuePtr);

    return rc;
}

/*******************************************************************************
* cpssDxChPortPfcCounterGet
*
* DESCRIPTION:
*       Get PFC counter value per PFC counter and traffic class.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number
*       tcQueue         - traffic class queue (APPLICABLE RANGES: 0..7)
*       pfcCounterNum   - PFC counter (APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       pfcCounterValuePtr - (pointer to) PFC counter value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_OUT_OF_RANGE          - on wrong pfcCounterNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortPfcCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   tcQueue,
    IN  GT_U32  pfcCounterNum,
    OUT GT_U32  *pfcCounterValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortPfcCounterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueue, pfcCounterNum, pfcCounterValuePtr));

    rc = internal_cpssDxChPortPfcCounterGet(devNum, tcQueue, pfcCounterNum, pfcCounterValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueue, pfcCounterNum, pfcCounterValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

