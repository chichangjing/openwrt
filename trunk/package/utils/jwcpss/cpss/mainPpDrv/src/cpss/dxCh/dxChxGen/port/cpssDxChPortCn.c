/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortCn.c
*
* DESCRIPTION:
*       CPSS implementation for Port Congestion Notification API.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 50 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCn.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>

/*******************************************************************************
* internal_cpssDxChPortCnModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification (CN) mechanism.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable congestion notification mechanism.
*                    GT_FALSE: Disable congestion notification mechanism.
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
*       Do not use the global enable/disable CCFC under traffic - instead use
*       cpssDxChPortCnQueueAwareEnableSet to enable/disable queue for BCN.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnModeEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;          /* register address */
    GT_U32 regOffset;        /* register offset */
    GT_STATUS rc;            /* function return value */
    GT_U32 controlRegVal;    /* BCN Control register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).tailDrop.tailDropConfig.globalTailDropConfig;
            regOffset = 9;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.congestNotification.cnGlobalConfigReg;
            regOffset = 0;
        }

        /* set CnEn */
        rc = prvCpssHwPpSetRegField(devNum, regAddr, regOffset, 1, BOOL2BIT_MAC(enable));
        if (rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;
        }

        /* set CNM Termination Mode in TTI */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 18, 2, BOOL2BIT_MAC(enable));
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        if (enable == GT_TRUE)
        {
            controlRegVal = 0xD;
        }
        else
        {
            controlRegVal = 0x0;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;

        /* set BCN Control register */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 4, controlRegVal);
        if (rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiUnitConfig;

        /* set Enable/Disable identification of CCFC frames in TTI */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 19, 1, BOOL2BIT_MAC(enable));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification (CN) mechanism.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable congestion notification mechanism.
*                    GT_FALSE: Disable congestion notification mechanism.
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
*       Do not use the global enable/disable CCFC under traffic - instead use
*       cpssDxChPortCnQueueAwareEnableSet to enable/disable queue for BCN.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnModeEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnModeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPortCnModeEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnModeEnableGet
*
* DESCRIPTION:
*       Gets the current status of Congestion Notification (CN) mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of Congestion Notification mode
*                     GT_TRUE:  Congestion Notification mode enabled.
*                     GT_FALSE: Congestion Notification mode disabled.
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
static GT_STATUS internal_cpssDxChPortCnModeEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    value;     /* register value */
    GT_U32    regAddr;   /* register address */
    GT_U32    regOffset; /* register offset */
    GT_STATUS rc;        /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).tailDrop.tailDropConfig.globalTailDropConfig;
        regOffset = 9;
    }
    else if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnGlobalConfigReg;
        regOffset = 0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;
        regOffset = 0;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, regOffset, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnModeEnableGet
*
* DESCRIPTION:
*       Gets the current status of Congestion Notification (CN) mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of Congestion Notification mode
*                     GT_TRUE:  Congestion Notification mode enabled.
*                     GT_FALSE: Congestion Notification mode disabled.
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
GT_STATUS cpssDxChPortCnModeEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnModeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPortCnModeEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnProfileSet
*
* DESCRIPTION:
*       Binds a port to a Congestion Notification profile.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (0..7).
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
*       1. To disable port for triggering CN frames bind port to profile with
*          all thresholds set to maximum.
*       2. On Lion device CN Profile is part of the Tail-Drop Profile.
*          To bind port to Tail-Drop Profile use cpssDxChPortTxBindPortToDpSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnProfileSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   profileIndex
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U32  localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (profileIndex > 7 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.
                portsBcnProfileReg0 + 4 * (localPort / 10);

    fieldOffset = (localPort % 10) * 3;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                     fieldOffset, 3, profileIndex);
}

/*******************************************************************************
* cpssDxChPortCnProfileSet
*
* DESCRIPTION:
*       Binds a port to a Congestion Notification profile.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (0..7).
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
*       1. To disable port for triggering CN frames bind port to profile with
*          all thresholds set to maximum.
*       2. On Lion device CN Profile is part of the Tail-Drop Profile.
*          To bind port to Tail-Drop Profile use cpssDxChPortTxBindPortToDpSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   profileIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnProfileSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, profileIndex));

    rc = internal_cpssDxChPortCnProfileSet(devNum, portNum, profileIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, profileIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnProfileGet
*
* DESCRIPTION:
*       Gets the port's Congestion Notification profile.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
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
static GT_STATUS internal_cpssDxChPortCnProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *profileIndexPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U32  localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(profileIndexPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.
                portsBcnProfileReg0 + 4 * (localPort / 10);

    fieldOffset = (localPort % 10) * 3;

    return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                     fieldOffset, 3, profileIndexPtr);
}

/*******************************************************************************
* cpssDxChPortCnProfileGet
*
* DESCRIPTION:
*       Gets the port's Congestion Notification profile.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
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
GT_STATUS cpssDxChPortCnProfileGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *profileIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnProfileGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, profileIndexPtr));

    rc = internal_cpssDxChPortCnProfileGet(devNum, portNum, profileIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, profileIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnProfileQueueThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*       threshold    - buffer limit threshold for triggering CN frame on a given
*                      tc queue (0..0x1FFF).
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
*       To bind port to CN profile in Lion use
*          cpssDxChPortCnProfileQueueConfigSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnProfileQueueThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN GT_U32   threshold
)
{
    GT_STATUS rc;    /* function return value */
    GT_U32 offset;   /* offset from the beginning of the entry */
    GT_U32 temp;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if (profileIndex > 7 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (threshold > 0x1FFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    offset = tcQueue * 13;

    temp = 32 - offset%32;

    if (temp >= 13)
    {
        return prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                               profileIndex,offset /32 ,offset % 32,13,threshold);
    }
    else
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                               profileIndex,offset /32 ,offset % 32,temp,threshold);
        if (rc != GT_OK)
        {
            return rc;
        }

        return prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                               profileIndex,(offset /32)+1 ,0,13 - temp,threshold>>temp);

    }
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*       threshold    - buffer limit threshold for triggering CN frame on a given
*                      tc queue (0..0x1FFF).
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
*       To bind port to CN profile in Lion use
*          cpssDxChPortCnProfileQueueConfigSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U8    tcQueue,
    IN GT_U32   threshold
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnProfileQueueThresholdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, tcQueue, threshold));

    rc = internal_cpssDxChPortCnProfileQueueThresholdSet(devNum, profileIndex, tcQueue, threshold);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, tcQueue, threshold));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnProfileQueueThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                      triggering CN frame on a given tc queue.
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
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnProfileQueueThresholdGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *thresholdPtr
)
{
    GT_STATUS rc;     /* function return value */
    GT_U32 value;     /* register value */
    GT_U32 offset;
    GT_U32 temp;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(thresholdPtr);

    if (profileIndex > 7 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    offset = tcQueue * 13;

    temp = 32 - offset%32;

    if (temp >= 13)
    {
        return prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                              profileIndex,offset/32,offset%32,13,thresholdPtr);
    }
    else
    {
        rc = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                            profileIndex,offset/32,offset%32,temp,&value);
        if (rc != GT_OK)
        {
            return rc;
        }
        *thresholdPtr = value;
        rc = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                            profileIndex,(offset/32)+1,0,13-temp,&value);
        if (rc != GT_OK)
        {
            return rc;
        }
        *thresholdPtr |= (value << temp);
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                      triggering CN frame on a given tc queue.
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueThresholdGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   profileIndex,
    IN  GT_U8    tcQueue,
    OUT GT_U32   *thresholdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnProfileQueueThresholdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, tcQueue, thresholdPtr));

    rc = internal_cpssDxChPortCnProfileQueueThresholdGet(devNum, profileIndex, tcQueue, thresholdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, tcQueue, thresholdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnProfileThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       threshold    - buffer limit threshold
*                      for triggering CN frame (0..0x1FFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnProfileThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U32   threshold
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (profileIndex > 7 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (threshold > 0x1FFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                              profileIndex,3,8,13,threshold);
}

/*******************************************************************************
* cpssDxChPortCnProfileThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       threshold    - buffer limit threshold
*                      for triggering CN frame (0..0x1FFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileThresholdSet
(
    IN GT_U8    devNum,
    IN GT_U32   profileIndex,
    IN GT_U32   threshold
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnProfileThresholdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, threshold));

    rc = internal_cpssDxChPortCnProfileThresholdSet(devNum, profileIndex, threshold);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, threshold));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnProfileThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                     triggering CN frame.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnProfileThresholdGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  profileIndex,
    OUT GT_U32  *thresholdPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(thresholdPtr);

    if (profileIndex > 7 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
                                              profileIndex,3,8,13,thresholdPtr);
}

/*******************************************************************************
* cpssDxChPortCnProfileThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                     triggering CN frame.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileThresholdGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  profileIndex,
    OUT GT_U32  *thresholdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnProfileThresholdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileIndex, thresholdPtr));

    rc = internal_cpssDxChPortCnProfileThresholdGet(devNum, profileIndex, thresholdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileIndex, thresholdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnQueueAwareEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*       enable     - GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
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
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnQueueAwareEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 value;     /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    value = BOOL2BIT_MAC(enable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 4 + tcQueue, 1, value);
}

/*******************************************************************************
* cpssDxChPortCnQueueAwareEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*       enable     - GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueAwareEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnQueueAwareEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueue, enable));

    rc = internal_cpssDxChPortCnQueueAwareEnableSet(devNum, tcQueue, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueue, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnQueueAwareEnableGet
*
* DESCRIPTION:
*       Gets the status of Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
* OUTPUTS:
*       enablePtr  - (pointer to) status of Congestion Notification awareness
*                    GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnQueueAwareEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 value;     /* register value */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 4 + tcQueue, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnQueueAwareEnableGet
*
* DESCRIPTION:
*       Gets the status of Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
* OUTPUTS:
*       enablePtr  - (pointer to) status of Congestion Notification awareness
*                    GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueAwareEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    tcQueue,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnQueueAwareEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueue, enablePtr));

    rc = internal_cpssDxChPortCnQueueAwareEnableGet(devNum, tcQueue, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueue, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnFrameQueueSet
*
* DESCRIPTION:
*       Set traffic class queue associated with CN frames generated by device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue associated with CN frames (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CN frames must be enqueued to priority queues disabled to trigger CN.
*       Bobcat2; Caelum; Bobcat3 - The device use 'traffic class' from qosProfileId set by
*       cpssDxChPortCnMessageGenerationConfigSet()
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnFrameQueueSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue
)
{
    GT_U32 regAddr;   /* register address */
    GT_U32 fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnGlobalConfigReg;
        fieldOffset = 1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;
        fieldOffset = 12;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 3, tcQueue);
}

/*******************************************************************************
* cpssDxChPortCnFrameQueueSet
*
* DESCRIPTION:
*       Set traffic class queue associated with CN frames generated by device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue associated with CN frames (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CN frames must be enqueued to priority queues disabled to trigger CN.
*       Bobcat2; Caelum; Bobcat3 - The device use 'traffic class' from qosProfileId set by
*       cpssDxChPortCnMessageGenerationConfigSet()
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFrameQueueSet
(
    IN GT_U8    devNum,
    IN GT_U8    tcQueue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnFrameQueueSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueue));

    rc = internal_cpssDxChPortCnFrameQueueSet(devNum, tcQueue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnFrameQueueGet
*
* DESCRIPTION:
*       Get traffic class queue associated with CN frames generated by device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       tcQueuePtr - (pointer to) traffic class queue associated with CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnFrameQueueGet
(
    IN  GT_U8    devNum,
    OUT GT_U8    *tcQueuePtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 value;     /* register value */
    GT_U32 fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(tcQueuePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnGlobalConfigReg;
        fieldOffset = 1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnControlReg;
        fieldOffset = 12;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 3, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *tcQueuePtr = (GT_U8)value;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnFrameQueueGet
*
* DESCRIPTION:
*       Get traffic class queue associated with CN frames generated by device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       tcQueuePtr - (pointer to) traffic class queue associated with CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFrameQueueGet
(
    IN  GT_U8    devNum,
    OUT GT_U8    *tcQueuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnFrameQueueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, tcQueuePtr));

    rc = internal_cpssDxChPortCnFrameQueueGet(devNum, tcQueuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, tcQueuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CN frames.
*       This EtherType also inserted in the CN header of generated CN frames.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number.
*       etherType   - EtherType for CN frames (0..0xFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range etherType values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       The CN EtherType must be unique in the system.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(etherType > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).CNAndFCConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ccfcEthertype;
    }
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, etherType);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).CNMHeaderConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, etherType);
}

/*******************************************************************************
* cpssDxChPortCnEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CN frames.
*       This EtherType also inserted in the CN header of generated CN frames.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number.
*       etherType   - EtherType for CN frames (0..0xFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range etherType values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       The CN EtherType must be unique in the system.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnEtherTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherType));

    rc = internal_cpssDxChPortCnEtherTypeSet(devNum, etherType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnEtherTypeGet
*
* DESCRIPTION:
*       Gets the EtherType for CN frames. The EtherType inserted in the CN
*       header.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       etherTypePtr - (pointer to) EtherType for CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnEtherTypeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *etherTypePtr
)
{
    GT_U32    regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    /* we are reading only ccfcEthertype register because it reflects the
       value in the hdrAltCnmHeaderConfig as well */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).CNAndFCConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ccfcEthertype;
    }
    return prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 16, etherTypePtr);
}

/*******************************************************************************
* cpssDxChPortCnEtherTypeGet
*
* DESCRIPTION:
*       Gets the EtherType for CN frames. The EtherType inserted in the CN
*       header.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       etherTypePtr - (pointer to) EtherType for CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnEtherTypeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *etherTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnEtherTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherTypePtr));

    rc = internal_cpssDxChPortCnEtherTypeGet(devNum, etherTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnSpeedIndexSet
*
* DESCRIPTION:
*       Sets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       portSpeedIndex - port speed index (0..7).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range port speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Congested ports should use Port Speed Index 1 to 6 if
*          Panic Pause is used.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnSpeedIndexSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   portSpeedIndex
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U32  localPort;   /* local port - support multi-port-groups device */
    GT_U32 txqPortNum;  /* TXQ port number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, portNum, txqPortNum);

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CCFCSpeedIndex[(txqPortNum >> 3)];
        fieldOffset = (txqPortNum % 8) * 3;

        return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 3, portSpeedIndex);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        if (portSpeedIndex > 7 )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.
                    portsSpeedIndexesReg0 + 4 * (localPort / 10);

        fieldOffset = (localPort % 10) * 3;

        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                         fieldOffset, 3, portSpeedIndex);
    }
}

/*******************************************************************************
* cpssDxChPortCnSpeedIndexSet
*
* DESCRIPTION:
*       Sets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       portSpeedIndex - port speed index (0..7).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range port speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Congested ports should use Port Speed Index 1 to 6 if
*          Panic Pause is used.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSpeedIndexSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   portSpeedIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnSpeedIndexSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portSpeedIndex));

    rc = internal_cpssDxChPortCnSpeedIndexSet(devNum, portNum, portSpeedIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portSpeedIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnSpeedIndexGet
*
* DESCRIPTION:
*       Gets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       portNum    -      - port number.
*
* OUTPUTS:
*       portSpeedIndexPtr - (pointer to) port speed index.
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
static GT_STATUS internal_cpssDxChPortCnSpeedIndexGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32   *portSpeedIndexPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */
    GT_U32  localPort;   /* local port - support multi-port-groups device */
    GT_U32 txqPortNum;  /* TXQ port number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(portSpeedIndexPtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, portNum, txqPortNum);

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CCFCSpeedIndex[(txqPortNum >> 3)];
        fieldOffset = (txqPortNum % 8) * 3;

        return prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 3, portSpeedIndexPtr);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.
                    portsSpeedIndexesReg0 + 4 * (localPort / 10);

        fieldOffset = (localPort % 10) * 3;

        return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                         fieldOffset, 3, portSpeedIndexPtr);
    }
}

/*******************************************************************************
* cpssDxChPortCnSpeedIndexGet
*
* DESCRIPTION:
*       Gets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       portNum    -      - port number.
*
* OUTPUTS:
*       portSpeedIndexPtr - (pointer to) port speed index.
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
GT_STATUS cpssDxChPortCnSpeedIndexGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32   *portSpeedIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnSpeedIndexGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, portSpeedIndexPtr));

    rc = internal_cpssDxChPortCnSpeedIndexGet(devNum, portNum, portSpeedIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, portSpeedIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortCnFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable generation of flow control packets as result of CN
*       frame termination or Panic Pause.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable. Flow control packets can be issued.
*                    GT_FALSE: disable. Flow control packets can't be issued.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnFcEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldOffset; /* the start bit number in the register */
    GT_U32 value;       /* register value */
    GT_STATUS rc;       /* function return value */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* used MAC unit */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, portNum);

    value = BOOL2BIT_MAC(enable);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
        {
            if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
                continue;

            if(portMacType >= PRV_CPSS_PORT_XG_E)
            {
                PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
                fieldOffset = 15;
            }
            else
            {
                PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
                fieldOffset = 2;
            }

            if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,fieldOffset, 1, value);

                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }
    else /*lion*/
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).enFCTriggerByCNFrameOnPort[OFFSET_TO_WORD_MAC(portNum)];
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.queue.congestNotification.cnFcEn[OFFSET_TO_WORD_MAC(portNum)];
        }

        fieldOffset = OFFSET_TO_BIT_MAC(portNum);

        rc = prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            rc = prvCpssDxChHwEgfEftFieldSet(devNum, portNum,
                PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_FC_TRIGGER_BY_CN_FRAME_ON_PORT_ENABLE_E,
                value);
            if (rc != GT_OK )
            {
                return rc;
            }
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.egr.filterConfig.egrCnFcEn[OFFSET_TO_WORD_MAC(portNum)];

            rc = prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
            if (rc != GT_OK )
            {
                return rc;
            }
        }
    }
    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortCnFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable generation of flow control packets as result of CN
*       frame termination or Panic Pause.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable. Flow control packets can be issued.
*                    GT_FALSE: disable. Flow control packets can't be issued.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnFcEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortCnFcEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnFcEnableGet
*
* DESCRIPTION:
*       Gets the status of generation of flow control packets as result of CN
*       frame termination or Panic Pause on a given port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable. Flow control packets is issued.
*                    GT_FALSE: disable. Flow control packets is not issued.
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
static GT_STATUS internal_cpssDxChPortCnFcEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 fieldOffset;/* the start bit number in the register */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* function return value */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);


    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
            fieldOffset = 15;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
            fieldOffset = 2;
        }

        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else /*lion*/
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).enFCTriggerByCNFrameOnPort[OFFSET_TO_WORD_MAC(portNum)];
            fieldOffset = OFFSET_TO_BIT_MAC(portNum);

            rc = prvCpssHwPpGetRegField(devNum, regAddr,fieldOffset, 1, &value);
            if (rc != GT_OK)
            {
                return rc;
            }

        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
             txqVer1.egr.filterConfig.egrCnFcEn[OFFSET_TO_WORD_MAC(portNum)];
            fieldOffset = OFFSET_TO_BIT_MAC(portNum);

            rc = prvCpssHwPpGetRegField(devNum, regAddr,fieldOffset, 1, &value);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnFcEnableGet
*
* DESCRIPTION:
*       Gets the status of generation of flow control packets as result of CN
*       frame termination or Panic Pause on a given port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable. Flow control packets is issued.
*                    GT_FALSE: disable. Flow control packets is not issued.
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
GT_STATUS cpssDxChPortCnFcEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnFcEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortCnFcEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnPauseTriggerEnableSet
*
* DESCRIPTION:
*       Enable the CN triggering engine to trigger a pause frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable.
*                    GT_FALSE: disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        To enable proper operation of CCFC, this configuration must be
*        enabled for all network ports and disabled for all cascade ports.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnPauseTriggerEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;     /* the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    value = BOOL2BIT_MAC(enable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnPauseTriggerEnableReg;


    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, portNum, 1, value);
}

/*******************************************************************************
* cpssDxChPortCnPauseTriggerEnableSet
*
* DESCRIPTION:
*       Enable the CN triggering engine to trigger a pause frame.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable.
*                    GT_FALSE: disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        To enable proper operation of CCFC, this configuration must be
*        enabled for all network ports and disabled for all cascade ports.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPauseTriggerEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnPauseTriggerEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortCnPauseTriggerEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnPauseTriggerEnableGet
*
* DESCRIPTION:
*       Gets the status triggering engine a pause frame on a given port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable.
*                    GT_FALSE: disable.
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
static GT_STATUS internal_cpssDxChPortCnPauseTriggerEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;     /* the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnPauseTriggerEnableReg;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,portNum, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnPauseTriggerEnableGet
*
* DESCRIPTION:
*       Gets the status triggering engine a pause frame on a given port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable.
*                    GT_FALSE: disable.
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
GT_STATUS cpssDxChPortCnPauseTriggerEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnPauseTriggerEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortCnPauseTriggerEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnFcTimerSet
*
* DESCRIPTION:
*       Sets timer value for the IEEE 802.3x/PFC frame issued as result
*       of CN frame termination.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       index          - index (0..7)
*       timer          - 802.3x/PFC pause time (0..0xFFFF).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_OUT_OF_RANGE          - on out of range pause time value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. xCat,xCat3: Panic Pause uses Port Speed Index 0 and 7 to indicate
*          XON and XOFF. Port timer configurations 0 and 7 must be configured
*          to 0x0 and 0xFFFF if Panic Pause is used.
*       2. xCat,xCat3: index is always congested port speed index (CCFC mode).
*       3. Lion, xCat2: index determined by
*          cpssDxChPortCnPrioritySpeedLocationSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnFcTimerSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   index,
    IN GT_U32   timer
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 portMacNum;  /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);

    if (index > 7 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (timer > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).portSpeedTimer[index];
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).portSpeedTimer[index];
        }

        return prvCpssHwPpSetRegField(devNum, regAddr, 0, 16, timer);
    }
    else if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

        if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].
                            macRegsPerType[PRV_CPSS_PORT_GE_E].ccfcFcTimer + (index<<2);
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
            regDataArray[PRV_CPSS_PORT_GE_E].fieldData = timer;
            regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 16;
            regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 0;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].
                            macRegsPerType[PRV_CPSS_PORT_XG_E].ccfcFcTimer + (index<<2);
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
            regDataArray[PRV_CPSS_PORT_XG_E].fieldData = timer;
            regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 16;
            regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 0;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].
                            macRegsPerType[PRV_CPSS_PORT_XLG_E].ccfcFcTimer + (index<<2);
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
            regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = timer;
            regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 16;
            regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 0;
        }

        return prvCpssDxChPortMacConfiguration(devNum, portNum, regDataArray);
    }
    else
    {
        GT_U32 portGroupId; /* the port group Id - support multi-port-groups device */

        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_XG_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                perPortRegs[portNum].ccfcFcTimerBaseXg;

            if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
                (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
            {
                timer = (timer >> 8) | ((timer & 0xFF) << 8);
            }
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                perPortRegs[portNum].ccfcFcTimerBaseGig;
        }

        regAddr += index*4;

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,0, 16, timer);
    }
}

/*******************************************************************************
* cpssDxChPortCnFcTimerSet
*
* DESCRIPTION:
*       Sets timer value for the IEEE 802.3x/PFC frame issued as result
*       of CN frame termination.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       index          - index (0..7)
*       timer          - 802.3x/PFC pause time (0..0xFFFF).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_OUT_OF_RANGE          - on out of range pause time value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. xCat,xCat3: Panic Pause uses Port Speed Index 0 and 7 to indicate
*          XON and XOFF. Port timer configurations 0 and 7 must be configured
*          to 0x0 and 0xFFFF if Panic Pause is used.
*       2. xCat,xCat3: index is always congested port speed index (CCFC mode).
*       3. Lion, xCat2: index determined by
*          cpssDxChPortCnPrioritySpeedLocationSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcTimerSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32   index,
    IN GT_U32   timer
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnFcTimerSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, index, timer));

    rc = internal_cpssDxChPortCnFcTimerSet(devNum, portNum, index, timer);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, index, timer));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnFcTimerGet
*
* DESCRIPTION:
*       Gets timer value for the IEEE 802.3x/PFC frame issued as result
*       of CN frame termination.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       index          - index (0..7).
*
* OUTPUTS:
*       timerPtr       - (pointer to) 802.3x/PFC Pause time
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. xCat,xCat3: index is always congested port speed index (CCFC mode).
*       2. Lion, xCat2: index determined by
*          cpssDxChPortCnPrioritySpeedLocationSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnFcTimerGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32   index,
    OUT GT_U32   *timerPtr
)
{
    GT_U32 regAddr;  /* register address */
    GT_U32 portGroupId;   /* the port group Id - support multi-port-groups device */
    GT_STATUS rc;    /* function return value */
    GT_U32 value;    /* register value */
    GT_U32 portMacNum;      /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(timerPtr);

    if (index > 7 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))

    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            regAddr = PRV_DXCH_REG_UNIT_GOP_FCA_MAC(devNum, portMacNum).portSpeedTimer[index];
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(devNum, portMacNum).portSpeedTimer[index];
        }

        rc =  prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, timerPtr);
        return rc;
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum].
                macRegsPerType[PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum)].ccfcFcTimer;
    }
    else
    {
        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                perPortRegs[portNum].ccfcFcTimerBaseXg;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                perPortRegs[portNum].ccfcFcTimerBaseGig;
        }
    }
    regAddr += index*4;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,0, 16, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_XG_E &&
      ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)))
    {
        *timerPtr = (value >> 8) | ((value & 0xFF) << 8);
    }
    else
    {
        *timerPtr = value;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnFcTimerGet
*
* DESCRIPTION:
*       Gets timer value for the IEEE 802.3x/PFC frame issued as result
*       of CN frame termination.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       index          - index (0..7).
*
* OUTPUTS:
*       timerPtr       - (pointer to) 802.3x/PFC Pause time
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. xCat,xCat3: index is always congested port speed index (CCFC mode).
*       2. Lion, xCat2: index determined by
*          cpssDxChPortCnPrioritySpeedLocationSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFcTimerGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    IN  GT_U32   index,
    OUT GT_U32   *timerPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnFcTimerGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, index, timerPtr));

    rc = internal_cpssDxChPortCnFcTimerGet(devNum, portNum, index, timerPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, index, timerPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortCnPanicPauseThresholdsSet
*
* DESCRIPTION:
*       Sets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       enable      - GT_TRUE: Enable Panic Pause.
*                     GT_FALSE: Disable Panic Pause.
*       xoffLimit   - X-OFF limit (0..1FFF).Relevant only if enable==GT_TRUE.
*       xonLimit    - X-ON limit (0..1FFF).Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range xoffLimit or
*                                  xonLimit value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Port timer configurations 0 and 7 must be configured
*           to 0x0 and 0xFFFF, see cpssDxChPortCnFcTimerSet.
*          Congested ports should use Port Speed Index 1 to 6,
*          see cpssDxChPortCnSpeedIndexSet.
*       2. Panic Pause cannot operate together with the IEEE 802.3x triggers.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnPanicPauseThresholdsSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U32   xoffLimit,
    IN GT_U32   xonLimit
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    /* for disable set to maximum value */
    if (enable == GT_FALSE)
    {
        xoffLimit = 0x1FFF;
        xonLimit = 0x1FFF;
    }
    if (xoffLimit > 0x1FFF || xonLimit > 0x1FFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnGlobalPauseThresholdsReg;
    value = xoffLimit | xonLimit << 13;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 26, value);
}

/*******************************************************************************
* cpssDxChPortCnPanicPauseThresholdsSet
*
* DESCRIPTION:
*       Sets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       enable      - GT_TRUE: Enable Panic Pause.
*                     GT_FALSE: Disable Panic Pause.
*       xoffLimit   - X-OFF limit (0..1FFF).Relevant only if enable==GT_TRUE.
*       xonLimit    - X-ON limit (0..1FFF).Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range xoffLimit or
*                                  xonLimit value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Port timer configurations 0 and 7 must be configured
*           to 0x0 and 0xFFFF, see cpssDxChPortCnFcTimerSet.
*          Congested ports should use Port Speed Index 1 to 6,
*          see cpssDxChPortCnSpeedIndexSet.
*       2. Panic Pause cannot operate together with the IEEE 802.3x triggers.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPanicPauseThresholdsSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U32   xoffLimit,
    IN GT_U32   xonLimit
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnPanicPauseThresholdsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable, xoffLimit, xonLimit));

    rc = internal_cpssDxChPortCnPanicPauseThresholdsSet(devNum, enable, xoffLimit, xonLimit);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable, xoffLimit, xonLimit));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnPanicPauseThresholdsGet
*
* DESCRIPTION:
*       Gets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       enablePtr    - (pointer to):
*                      GT_TRUE: Enable Panic Pause.
*                      GT_FALSE: Disable Panic Pause.
*       xoffLimitPtr - (pointer to) the X-OFF limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*       xonLimitPtr  - (pointer to) the X-ON limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnPanicPauseThresholdsGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U32   *xoffLimitPtr,
    OUT GT_U32   *xonLimitPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(xoffLimitPtr);
    CPSS_NULL_PTR_CHECK_MAC(xonLimitPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bcnRegs.bcnGlobalPauseThresholdsReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 26, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = (value == 0x3FFFFFF)? GT_FALSE:GT_TRUE;
    *xoffLimitPtr = value & 0x1FFF;
    *xonLimitPtr = value >> 13;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnPanicPauseThresholdsGet
*
* DESCRIPTION:
*       Gets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       enablePtr    - (pointer to):
*                      GT_TRUE: Enable Panic Pause.
*                      GT_FALSE: Disable Panic Pause.
*       xoffLimitPtr - (pointer to) the X-OFF limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*       xonLimitPtr  - (pointer to) the X-ON limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPanicPauseThresholdsGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_U32   *xoffLimitPtr,
    OUT GT_U32   *xonLimitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnPanicPauseThresholdsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr, xoffLimitPtr, xonLimitPtr));

    rc = internal_cpssDxChPortCnPanicPauseThresholdsGet(devNum, enablePtr, xoffLimitPtr, xonLimitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr, xoffLimitPtr, xonLimitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnTerminationEnableSet
*
* DESCRIPTION:
*       Enable/Disable termination of CNM (Congestion Notification Message) on given port.
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
*       enable     - GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnTerminationEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* register value */
    GT_STATUS rc;       /* function return value */
    GT_U32 fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, portNum);

    value = (enable == GT_TRUE)? 0 : 1;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).enCNFrameTxOnPort[OFFSET_TO_WORD_MAC(portNum)];
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnEnCnFrameTx[OFFSET_TO_WORD_MAC(portNum)];
    }
    fieldOffset = OFFSET_TO_BIT_MAC(portNum);

    rc = prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChHwEgfEftFieldSet(devNum,portNum,
            PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_CN_FRAME_TX_ON_PORT_ENABLE_E,
            value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.egr.filterConfig.egrCnEn[OFFSET_TO_WORD_MAC(portNum)];

        return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
    }
}

/*******************************************************************************
* cpssDxChPortCnTerminationEnableSet
*
* DESCRIPTION:
*       Enable/Disable termination of CNM (Congestion Notification Message) on given port.
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
*       enable     - GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnTerminationEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnTerminationEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortCnTerminationEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnTerminationEnableGet
*
* DESCRIPTION:
*       Gets the status of CNM termination on given port.
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
*       enablePtr  - (pointer to)
*                    GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
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
static GT_STATUS internal_cpssDxChPortCnTerminationEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* register value */
    GT_STATUS rc;       /* function return value */
    GT_U32 fieldOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChHwEgfEftFieldGet(devNum,portNum,
            PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_CN_FRAME_TX_ON_PORT_ENABLE_E,
            &value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                txqVer1.egr.filterConfig.egrCnEn[OFFSET_TO_WORD_MAC(portNum)];
        fieldOffset = OFFSET_TO_BIT_MAC(portNum);

        rc = prvCpssHwPpGetRegField(devNum, regAddr,fieldOffset, 1, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnTerminationEnableGet
*
* DESCRIPTION:
*       Gets the status of CNM termination on given port.
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
*       enablePtr  - (pointer to)
*                    GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
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
GT_STATUS cpssDxChPortCnTerminationEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnTerminationEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortCnTerminationEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       profileSet      - profile set.
*       tcQueue         - traffic class queue (0..7).
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CN Profile is part of the Tail-Drop Profile.
*       To bind port to Tail-Drop Profile use cpssDxChPortTxBindPortToDpSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnProfileQueueConfigSet
(
    IN GT_U8                               devNum,
    IN CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN GT_U8                               tcQueue,
    IN CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 regOffset; /* register offset */
    GT_U32 fieldOffset; /* field offset */
    GT_U32 fieldLenght;  /* field length */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(cnProfileCfgPtr);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet,devNum);

    if (cnProfileCfgPtr->threshold > 0x1FFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2 A0 revision has erratum FE-535006
           - The number of CN Tail Drop profiles is limited to 8 when QCN is enabled */
        if((!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum)) && (profileSet > CPSS_PORT_TX_DROP_PROFILE_8_E))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).tailDrop.tailDropConfig.profileCNMTriggeringEnable[profileSet];
        fieldOffset = tcQueue;
    }
    else
    {
        /* set CN awareness */
        regOffset = (profileSet/4) * 0x4;
        fieldOffset = (profileSet%4)*8 + tcQueue;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnPrioQueueEnProfile + regOffset;
    }
    rc = prvCpssHwPpSetRegField(devNum, regAddr,fieldOffset, 1,
                                   BOOL2BIT_MAC(cnProfileCfgPtr->cnAware));
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set threshold */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).profilePriorityQueueCNThreshold[profileSet][tcQueue];
        fieldLenght = 16;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnProfileThreshold[tcQueue][profileSet];
        fieldLenght = 13;
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, fieldLenght, cnProfileCfgPtr->threshold);
    return rc;
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number.
*       profileSet      - profile set.
*       tcQueue         - traffic class queue (0..7).
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CN Profile is part of the Tail-Drop Profile.
*       To bind port to Tail-Drop Profile use cpssDxChPortTxBindPortToDpSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueConfigSet
(
    IN GT_U8                               devNum,
    IN CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN GT_U8                               tcQueue,
    IN CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnProfileQueueConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileSet, tcQueue, cnProfileCfgPtr));

    rc = internal_cpssDxChPortCnProfileQueueConfigSet(devNum, profileSet, tcQueue, cnProfileCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileSet, tcQueue, cnProfileCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       profileSet   - profile set.
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnProfileQueueConfigGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_U8                               tcQueue,
    OUT CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
)
{
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 regOffset;    /* register offset */
    GT_U32 fieldOffset;  /* field offset */
    GT_U32 fieldLenght;  /* field length */
    GT_U32 value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(cnProfileCfgPtr);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet,devNum);

    /* get CN awareness */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* Bobcat2 A0 revision has erratum FE-535006
           - The number of CN Tail Drop profiles is limited to 8 when QCN is enabled */
        if((!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum)) && (profileSet > CPSS_PORT_TX_DROP_PROFILE_8_E))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).tailDrop.tailDropConfig.profileCNMTriggeringEnable[profileSet];
        fieldOffset = tcQueue;
    }
    else
    {
        regOffset = (profileSet/4) * 0x4;
        fieldOffset = (profileSet%4)*8 + tcQueue;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnPrioQueueEnProfile + regOffset;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    cnProfileCfgPtr->cnAware = BIT2BOOL_MAC(value);

    regOffset = profileSet*0x20 + tcQueue* 0x4;

    /* get threshold */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).profilePriorityQueueCNThreshold[profileSet][tcQueue];
        fieldLenght = 16;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnProfileThreshold[tcQueue][profileSet];
        fieldLenght = 13;
    }
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, fieldLenght, &(cnProfileCfgPtr->threshold));
    return rc;
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       profileSet   - profile set.
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnProfileQueueConfigGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  GT_U8                               tcQueue,
    OUT CPSS_PORT_CN_PROFILE_CONFIG_STC     *cnProfileCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnProfileQueueConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileSet, tcQueue, cnProfileCfgPtr));

    rc = internal_cpssDxChPortCnProfileQueueConfigGet(devNum, profileSet, tcQueue, cnProfileCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileSet, tcQueue, cnProfileCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnQueueStatusModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable queue status mode. When enabled, CNM is sent to a
*       configurable target that examines the queue status.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: CNM is sent to a targetDev/targetPort.
*                    GT_FALSE: CNM is sent to a sampled packet's source.
*       targetHwDev- Target device for the CNM.
*                    Relevant only if enable==GT_TRUE.
*       targetPort - Target port for the CNM.
*                    Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range targetDev or targetPort
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnQueueStatusModeEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable,
    IN GT_HW_DEV_NUM    targetHwDev,
    IN GT_PORT_NUM      targetPort
)
{
    GT_U32 value;       /* register value */
    GT_U32 regAddr;     /* register address */
    GT_U32 fieldSize;   /* register field size */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    if (enable == GT_TRUE)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            if ((targetHwDev > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum)) ||
                 targetPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            value = 1 | (targetHwDev << 1) | (targetPort << 11);
        }
        else
        {
            if (targetHwDev > 31 || targetPort > 63)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            value = 1 | (targetHwDev & 0x1F) << 1 | (targetPort & 0x3F) << 6;
        }
    }
    else
    {
        value = 0;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNTargetAssignment;
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            fieldSize = 26;
        }
        else
        {
            fieldSize = 24;
        }

        return prvCpssHwPpSetRegField(devNum, regAddr, 0, fieldSize, value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;

        return prvCpssHwPpSetRegField(devNum, regAddr, 8, 12, value);
    }
}

/*******************************************************************************
* cpssDxChPortCnQueueStatusModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable queue status mode. When enabled, CNM is sent to a
*       configurable target that examines the queue status.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: CNM is sent to a targetDev/targetPort.
*                    GT_FALSE: CNM is sent to a sampled packet's source.
*       targetHwDev- Target device for the CNM.
*                    Relevant only if enable==GT_TRUE.
*       targetPort - Target port for the CNM.
*                    Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range targetDev or targetPort
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueStatusModeEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable,
    IN GT_HW_DEV_NUM    targetHwDev,
    IN GT_PORT_NUM      targetPort
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnQueueStatusModeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable, targetHwDev, targetPort));

    rc = internal_cpssDxChPortCnQueueStatusModeEnableSet(devNum, enable, targetHwDev, targetPort);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable, targetHwDev, targetPort));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnQueueStatusModeEnableGet
*
* DESCRIPTION:
*        Gets the current status of queue status mode.
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
*       enablePtr     - (pointer to) status of queue status mode.
*                       GT_TRUE: CNM is sent to a targetDev/targetPort.
*                       GT_FALSE: CNM is sent to a sampled packet's source.
*       targetHwDevPtr- (pointer to) Target device for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*       targetPortPtr - (pointer to) Target port for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnQueueStatusModeEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr,
    OUT GT_HW_DEV_NUM   *targetHwDevPtr,
    OUT GT_PORT_NUM     *targetPortPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32 fieldSize; /* register field size */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(targetHwDevPtr);
    CPSS_NULL_PTR_CHECK_MAC(targetPortPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNTargetAssignment;
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            fieldSize = 26;
        }
        else
        {
            fieldSize = 24;
        }
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, fieldSize, &value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 8, 12, &value);
    }
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value & 0x1);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        *targetHwDevPtr = ((value >> 1) & 0x3FF);
        *targetPortPtr = (value >> 11) & 0x1FFF;
    }
    else
    {
        *targetHwDevPtr = ((value >> 1) & 0x1F);
        *targetPortPtr = ((value >> 6) & 0x3f);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnQueueStatusModeEnableGet
*
* DESCRIPTION:
*        Gets the current status of queue status mode.
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
*       enablePtr     - (pointer to) status of queue status mode.
*                       GT_TRUE: CNM is sent to a targetDev/targetPort.
*                       GT_FALSE: CNM is sent to a sampled packet's source.
*       targetHwDevPtr- (pointer to) Target device for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*       targetPortPtr - (pointer to) Target port for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnQueueStatusModeEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr,
    OUT GT_HW_DEV_NUM   *targetHwDevPtr,
    OUT GT_PORT_NUM     *targetPortPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnQueueStatusModeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr, targetHwDevPtr, targetPortPtr));

    rc = internal_cpssDxChPortCnQueueStatusModeEnableGet(devNum, enablePtr, targetHwDevPtr, targetPortPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr, targetHwDevPtr, targetPortPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnSampleEntrySet
*
* DESCRIPTION:
*       Sets CN sample interval entry.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*       entryIndex  - index: qFb 3 msb. (0..7)
*       entryPtr   - (pointer to) entry settings.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.To implement CCFC functionality set entryPtr fields to:
*          interval = (MTU+256b)/16
*          randBitmap = 0
*       2.To achieve uniform distribution of random values, clear the interval
*         to be randomized to 0. For example, if interval = 0xF0 and
*         randBitmap = 0x0F then the sampling interval is a random value
*         in [0xF0,0xFF].
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnSampleEntrySet
(
    IN GT_U8    devNum,
    IN GT_U8    entryIndex,
    IN CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    if (entryIndex > 7)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (entryPtr->interval > 0xFFFF ||
        entryPtr->randBitmap > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    value = entryPtr->interval | entryPtr->randBitmap << 16;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNSampleTbl[entryIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnSampleTbl;
        regAddr += entryIndex*0x4;
    }

    return prvCpssHwPpWriteRegister(devNum, regAddr, value);
}

/*******************************************************************************
* cpssDxChPortCnSampleEntrySet
*
* DESCRIPTION:
*       Sets CN sample interval entry.
*
* APPLICABLE DEVICES:
*         Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*       entryIndex  - index: qFb 3 msb. (0..7)
*       entryPtr   - (pointer to) entry settings.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.To implement CCFC functionality set entryPtr fields to:
*          interval = (MTU+256b)/16
*          randBitmap = 0
*       2.To achieve uniform distribution of random values, clear the interval
*         to be randomized to 0. For example, if interval = 0xF0 and
*         randBitmap = 0x0F then the sampling interval is a random value
*         in [0xF0,0xFF].
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSampleEntrySet
(
    IN GT_U8    devNum,
    IN GT_U8    entryIndex,
    IN CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnSampleEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, entryPtr));

    rc = internal_cpssDxChPortCnSampleEntrySet(devNum, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnSampleEntryGet
*
* DESCRIPTION:
*       Gets CN sample interval entry.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*       entryIndex  - index: qFb 3 msb. (0..7)
* OUTPUTS:
*       entryPtr   - (pointer to) entry settings.
*
* RETURNS:
*       GT_OK                    - on success
*
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnSampleEntryGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    entryIndex,
    OUT CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(entryPtr);
    if (entryIndex > 7)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNSampleTbl[entryIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.cnSampleTbl;
        regAddr += entryIndex*0x4;
    }
    rc = prvCpssHwPpReadRegister(devNum, regAddr, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    entryPtr->interval = value & 0xFFFF;
    entryPtr->randBitmap = value >> 16;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnSampleEntryGet
*
* DESCRIPTION:
*       Gets CN sample interval entry.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*       entryIndex  - index: qFb 3 msb. (0..7)
* OUTPUTS:
*       entryPtr   - (pointer to) entry settings.
*
* RETURNS:
*       GT_OK                    - on success
*
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnSampleEntryGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    entryIndex,
    OUT CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC *entryPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnSampleEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, entryPtr));

    rc = internal_cpssDxChPortCnSampleEntryGet(devNum, entryIndex, entryPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, entryPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnFbCalcConfigSet
*
* DESCRIPTION:
*       Sets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To implement CCFC functionality set fbCalcCfgPtr fields to:
*          deltaEnable = 0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnFbCalcConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 wExpValue; /* wExp hw value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(fbCalcCfgPtr);

    if (fbCalcCfgPtr->wExp > 7 || fbCalcCfgPtr->wExp < -8 ||
        fbCalcCfgPtr->fbMin > 0xFFFFF || fbCalcCfgPtr->fbMax > 0xFFFFF ||
        fbCalcCfgPtr->fbLsb > 0x1F)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* Wexp is given in 2s complement format */
    if (fbCalcCfgPtr->wExp >= 0)
    {
        wExpValue = (GT_U32)fbCalcCfgPtr->wExp;
    }
    else
    {
        wExpValue =(GT_U32)(16 + fbCalcCfgPtr->wExp);
    }

    /* set Feedback Calc Configurations register */
    value = BOOL2BIT_MAC(fbCalcCfgPtr->deltaEnable) |
            fbCalcCfgPtr->fbLsb << 1 |
            wExpValue << 6;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).feedbackCalcConfigs;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.fbCalcConfigReg;
    }
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 10, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set Feedback MIN register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).feedbackMIN;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.fbMinReg;
    }
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 20, fbCalcCfgPtr->fbMin);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set Feedback MAX register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).feedbackMAX;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.fbMaxReg;
    }
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 20, fbCalcCfgPtr->fbMax);

}

/*******************************************************************************
* cpssDxChPortCnFbCalcConfigSet
*
* DESCRIPTION:
*       Sets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To implement CCFC functionality set fbCalcCfgPtr fields to:
*          deltaEnable = 0
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFbCalcConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnFbCalcConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, fbCalcCfgPtr));

    rc = internal_cpssDxChPortCnFbCalcConfigSet(devNum, fbCalcCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, fbCalcCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnFbCalcConfigGet
*
* DESCRIPTION:
*       Gets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnFbCalcConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 wExpValue; /* wExp hw value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(fbCalcCfgPtr);

    /* get Feedback Calc Configurations register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).feedbackCalcConfigs;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.fbCalcConfigReg;
    }
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 10, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    fbCalcCfgPtr->deltaEnable = BIT2BOOL_MAC(value & 0x1);
    fbCalcCfgPtr->fbLsb = (value >> 1) & 0x1F;

    /* Wexp is given in 2s complement format */
    wExpValue = (value >> 6) & 0xF;
    if (wExpValue < 8)
    {
        fbCalcCfgPtr->wExp = wExpValue;
    }
    else
    {
        fbCalcCfgPtr->wExp = wExpValue - 16;
    }


    /* get Feedback MIN register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).feedbackMIN;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.fbMinReg;
    }
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 20, &(fbCalcCfgPtr->fbMin));
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get Feedback MAX register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).feedbackMAX;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.queue.congestNotification.fbMaxReg;
    }
    return prvCpssHwPpGetRegField(devNum, regAddr, 0, 20, &(fbCalcCfgPtr->fbMax));
}

/*******************************************************************************
* cpssDxChPortCnFbCalcConfigGet
*
* DESCRIPTION:
*       Gets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnFbCalcConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC *fbCalcCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnFbCalcConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, fbCalcCfgPtr));

    rc = internal_cpssDxChPortCnFbCalcConfigGet(devNum, fbCalcCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, fbCalcCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnPacketLengthSet
*
* DESCRIPTION:
*       Sets packet length used by the CN frames triggering logic.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       packetLength - packet length.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or packet length.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Configuring a value other than CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E
*       should be used when the packet length is unknown, e.g. Cut-through mode.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnPacketLengthSet
(
    IN GT_U8                               devNum,
    IN CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT packetLength
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    switch (packetLength)
    {
        case CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_CN_LENGTH_1_5_KB_E:
            value = 1;
            break;
        case CPSS_DXCH_PORT_CN_LENGTH_2_KB_E:
            value = 2;
            break;
        case CPSS_DXCH_PORT_CN_LENGTH_10_KB_E:
            value = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNGlobalConfig;
        return prvCpssHwPpSetRegField(devNum, regAddr, 2, 2, value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;
        return prvCpssHwPpSetRegField(devNum, regAddr, 6, 2, value);
    }
}

/*******************************************************************************
* cpssDxChPortCnPacketLengthSet
*
* DESCRIPTION:
*       Sets packet length used by the CN frames triggering logic.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number.
*       packetLength - packet length.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or packet length.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Configuring a value other than CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E
*       should be used when the packet length is unknown, e.g. Cut-through mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPacketLengthSet
(
    IN GT_U8                               devNum,
    IN CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT packetLength
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnPacketLengthSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, packetLength));

    rc = internal_cpssDxChPortCnPacketLengthSet(devNum, packetLength);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, packetLength));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnPacketLengthGet
*
* DESCRIPTION:
*       Gets packet length used by the CN frames triggering logic.
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
*       packetLengthPtr - (pointer to) packet length.
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
static GT_STATUS internal_cpssDxChPortCnPacketLengthGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT  *packetLengthPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(packetLengthPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNGlobalConfig;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 2, 2, &value);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 6, 2, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }
    *packetLengthPtr = (CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT)value;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnPacketLengthGet
*
* DESCRIPTION:
*       Gets packet length used by the CN frames triggering logic.
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
*       packetLengthPtr - (pointer to) packet length.
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
GT_STATUS cpssDxChPortCnPacketLengthGet
(
    IN  GT_U8    devNum,
    OUT CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT  *packetLengthPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnPacketLengthGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, packetLengthPtr));

    rc = internal_cpssDxChPortCnPacketLengthGet(devNum, packetLengthPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, packetLengthPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnMessageGenerationConfigSet
*
* DESCRIPTION:
*       Sets CNM generation configuration.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum - device number.
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnMessageGenerationConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_U32 scaleFactorHwValue;
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(cnmGenerationCfgPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if ((cnmGenerationCfgPtr->qosProfileId > 0x3FF) ||
            (cnmGenerationCfgPtr->sourceId >= BIT_12) ||
            (cnmGenerationCfgPtr->scaleFactor != 256 && cnmGenerationCfgPtr->scaleFactor != 512) ||
            (cnmGenerationCfgPtr->isRouted > 1) ||
            (cnmGenerationCfgPtr->version > 0xF))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* set CN Descriptor Attributes Register */
        value = (cnmGenerationCfgPtr->sourceId |
                (BOOL2BIT_MAC(cnmGenerationCfgPtr->overrideUp) << 23) |
                (cnmGenerationCfgPtr->isRouted << 22) |
                (cnmGenerationCfgPtr->qosProfileId << 12));
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNDescAttributes;
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 24, value);
        if(GT_OK != rc)
        {
            return rc;
        }

        /* Set HA CNM Header Configuration Register */
        scaleFactorHwValue = (cnmGenerationCfgPtr->scaleFactor == 256) ? 1 : 0;
        value = cnmGenerationCfgPtr->version << 16 |
                scaleFactorHwValue << 20;
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).CNMHeaderConfig;
        rc = prvCpssHwPpWriteRegBitMask(devNum, regAddr, 0x1F0000, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* set HA CPID Register 0  and CPID Register 1*/
        value = ((cnmGenerationCfgPtr->cpidMsb[4] << 12) |
                 (cnmGenerationCfgPtr->cpidMsb[5] << 4) |
                 (cnmGenerationCfgPtr->cpidMsb[6] >> 4));

        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).CPIDReg0;
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 20, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        value = (cnmGenerationCfgPtr->cpidMsb[3] |
                     (cnmGenerationCfgPtr->cpidMsb[2] << 8) |
                     (cnmGenerationCfgPtr->cpidMsb[1] << 16) |
                     (cnmGenerationCfgPtr->cpidMsb[0] << 24));
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).CPIDReg1;
        rc = prvCpssHwPpWriteRegister(devNum, regAddr, value);
        if (rc != GT_OK)
        {
            return rc;
        }

         /* set CN Global Configurations register */
        value = BOOL2BIT_MAC(cnmGenerationCfgPtr->cnUntaggedEnable);

        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNGlobalConfig;
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* set HA Congestion Configuration Register */
        value = BOOL2BIT_MAC(cnmGenerationCfgPtr->forceCnTag);
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).congestionNotificationConfig;
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, value);
        if (rc != GT_OK)
        {
            return rc;
        }

    }
    else if((CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)||
            (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        if (cnmGenerationCfgPtr->qosProfileId > 0x7F ||
            cnmGenerationCfgPtr->isRouted > 1 ||
            (cnmGenerationCfgPtr->overrideUp == GT_TRUE && cnmGenerationCfgPtr->cnmUp > 7) ||
            cnmGenerationCfgPtr->defaultVlanId > 0xFFF ||
            (cnmGenerationCfgPtr->scaleFactor != 256 &&
             cnmGenerationCfgPtr->scaleFactor != 512) ||
            cnmGenerationCfgPtr->version > 0xF ||
            cnmGenerationCfgPtr->flowIdTag > 0xFFFF)

        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* set Congestion Notification Configuration register */
        value = cnmGenerationCfgPtr->defaultVlanId |
                (BOOL2BIT_MAC(cnmGenerationCfgPtr->overrideUp) << 16) |
                ((cnmGenerationCfgPtr->cnmUp & 0x7) << 17) |
                (cnmGenerationCfgPtr->isRouted << 20) |
                (cnmGenerationCfgPtr->qosProfileId << 21) |
                (BOOL2BIT_MAC(cnmGenerationCfgPtr->forceCnTag) << 28);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnConfig;
        rc = prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x1FFF0FFF, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* set CNM Header Configuration register*/
        scaleFactorHwValue = (cnmGenerationCfgPtr->scaleFactor == 256) ? 1 : 0;

        value = cnmGenerationCfgPtr->version << 16 |
                scaleFactorHwValue << 23 |
                ((!BOOL2BIT_MAC(cnmGenerationCfgPtr->appendPacket)) << 25);


        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
        rc = prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0x28F0000, value);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* set CPID Register 0  and CPID Register 1*/
        value = ((cnmGenerationCfgPtr->cpidMsb[4] << 14) |
                     (cnmGenerationCfgPtr->cpidMsb[5] << 6) |
                     (cnmGenerationCfgPtr->cpidMsb[6] >> 2));


        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCpidReg0;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 22, value);
        if (rc != GT_OK)
        {
            return rc;
        }
        value = (cnmGenerationCfgPtr->cpidMsb[3] |
                     (cnmGenerationCfgPtr->cpidMsb[2] << 8) |
                     (cnmGenerationCfgPtr->cpidMsb[1] << 16) |
                     (cnmGenerationCfgPtr->cpidMsb[0] << 24));
        regAddr += 4;
        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, value);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* set QCN CN-tag FlowID register*/
        value = cnmGenerationCfgPtr->flowIdTag;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnTagFlowId;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* set CN Global Configurations register */
        value = BOOL2BIT_MAC(cnmGenerationCfgPtr->cnUntaggedEnable);

        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNGlobalConfig;
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, value);
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;
            rc = prvCpssHwPpSetRegField(devNum, regAddr, 22, 1, value);
        }
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else /* XCAT2 */
    {
        if (cnmGenerationCfgPtr->qosProfileId > 0x7F ||
            cnmGenerationCfgPtr->defaultVlanId > 0xFFF)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* set Congestion Notification Configuration register */
        value = cnmGenerationCfgPtr->defaultVlanId |
                (cnmGenerationCfgPtr->qosProfileId << 21);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnConfig;
        rc = prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 0xFE00FFF, value);
        if (rc != GT_OK)
        {
            return rc;
        }

        value = (!BOOL2BIT_MAC(cnmGenerationCfgPtr->appendPacket));
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 25, 1, value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnMessageGenerationConfigSet
*
* DESCRIPTION:
*       Sets CNM generation configuration.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum - device number.
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageGenerationConfigSet
(
    IN GT_U8    devNum,
    IN CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnMessageGenerationConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cnmGenerationCfgPtr));

    rc = internal_cpssDxChPortCnMessageGenerationConfigSet(devNum, cnmGenerationCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cnmGenerationCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnMessageGenerationConfigGet
*
* DESCRIPTION:
*       Gets CNM generation configuration.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnMessageGenerationConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(cnmGenerationCfgPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        cpssOsMemSet(cnmGenerationCfgPtr, 0, sizeof(CPSS_PORT_CNM_GENERATION_CONFIG_STC));

        /* Get CN Descriptor Attributes Register */
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNDescAttributes;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 24, &value);
        if(GT_OK != rc)
        {
            return rc;
        }
        cnmGenerationCfgPtr->sourceId = U32_GET_FIELD_MAC(value, 0, 12);
        cnmGenerationCfgPtr->qosProfileId = U32_GET_FIELD_MAC(value, 12, 10);
        cnmGenerationCfgPtr->isRouted = U32_GET_FIELD_MAC(value, 22, 1);
        cnmGenerationCfgPtr->overrideUp = BIT2BOOL_MAC((U32_GET_FIELD_MAC(value, 23, 1)));

        /* Get HA CNM Header Configuration Register */
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).CNMHeaderConfig;
        rc = prvCpssHwPpReadRegister(devNum, regAddr, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
        cnmGenerationCfgPtr->scaleFactor = (U32_GET_FIELD_MAC(value, 20, 1) == 1) ? 256 : 512;
        cnmGenerationCfgPtr->version = U32_GET_FIELD_MAC(value, 16, 4);

        /* set HA CPID Register 0  and CPID Register 1*/
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).CPIDReg0;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 20, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
        cnmGenerationCfgPtr->cpidMsb[4] = U32_GET_FIELD_MAC(value, 12, 8);
        cnmGenerationCfgPtr->cpidMsb[5] = U32_GET_FIELD_MAC(value, 4, 8);
        cnmGenerationCfgPtr->cpidMsb[6] = U32_GET_FIELD_MAC(value, 0, 4) << 4;


        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).CPIDReg1;
        rc = prvCpssHwPpReadRegister(devNum, regAddr, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
        cnmGenerationCfgPtr->cpidMsb[3] = U32_GET_FIELD_MAC(value, 0, 8);
        cnmGenerationCfgPtr->cpidMsb[2] = U32_GET_FIELD_MAC(value, 8, 8);
        cnmGenerationCfgPtr->cpidMsb[1] = U32_GET_FIELD_MAC(value, 16, 8);
        cnmGenerationCfgPtr->cpidMsb[0] = U32_GET_FIELD_MAC(value, 24, 8);

         /* Get CN Global Configurations register */
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNGlobalConfig;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 6, 1, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
        cnmGenerationCfgPtr->cnUntaggedEnable = BIT2BOOL_MAC(value);

        /* set HA Congestion Configuration Register */
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).congestionNotificationConfig;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
        cnmGenerationCfgPtr->forceCnTag = BIT2BOOL_MAC(value);

    }
    else if((CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)||
       (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        /* get Congestion Notification Configuration register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnConfig;
        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr,&value);
        if (rc != GT_OK)
        {
            return rc;
        }
        cnmGenerationCfgPtr->defaultVlanId = (GT_U16)(value & 0xFFF);
        cnmGenerationCfgPtr->overrideUp = BIT2BOOL_MAC((value>>16) & 0x1);
        cnmGenerationCfgPtr->cnmUp = (GT_U8)((value >> 17) & 0x7);
        cnmGenerationCfgPtr->isRouted = (GT_U8)((value >> 20) & 0x1);
        cnmGenerationCfgPtr->qosProfileId = (value >> 21) & 0x7F;
        cnmGenerationCfgPtr->forceCnTag = BIT2BOOL_MAC((value>>28) & 0x1);

        /* get CNM Header Configuration register*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);
        if (rc != GT_OK)
        {
            return rc;
        }

        cnmGenerationCfgPtr->version = value >> 16 & 0xf;
        cnmGenerationCfgPtr->scaleFactor = ((value >> 23) & 0x1) ? 256 : 512;
        cnmGenerationCfgPtr->appendPacket =  BIT2BOOL_MAC((!(value>>25) & 0x1));

        /* get CPID Register 0  and CPID Register 1*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCpidReg0;
        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);
        if (rc != GT_OK)
        {
            return rc;
        }

        cnmGenerationCfgPtr->cpidMsb[4] = (GT_U8)((value >> 14) & 0xFF);
        cnmGenerationCfgPtr->cpidMsb[5] = (GT_U8)((value >> 6) & 0xFF);
        cnmGenerationCfgPtr->cpidMsb[6] = (GT_U8)((value << 2) & 0xFF);

        regAddr += 4;
        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr,&value);
        if (rc != GT_OK)
        {
            return rc;
        }
        cnmGenerationCfgPtr->cpidMsb[0] = (GT_U8)((value >> 24) & 0xFF);
        cnmGenerationCfgPtr->cpidMsb[1] = (GT_U8)((value >> 16) & 0xFF);
        cnmGenerationCfgPtr->cpidMsb[2] = (GT_U8)((value >> 8) & 0xFF);
        cnmGenerationCfgPtr->cpidMsb[3] = (GT_U8)(value & 0xFF);

        /* get QCN CN-tag FlowID register*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnTagFlowId;
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0,16, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
        cnmGenerationCfgPtr->flowIdTag = value;

        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNGlobalConfig;
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 6, 1, &value);
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.congestNotification.cnGlobalConfigReg;
            rc = prvCpssHwPpGetRegField(devNum, regAddr, 22,1, &value);
        }
        if (rc != GT_OK)
        {
            return rc;
        }
        cnmGenerationCfgPtr->cnUntaggedEnable = BIT2BOOL_MAC(value);
    }
    else /* XCAT2 */
    {
        /* get Congestion Notification Configuration register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnConfig;
        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr,&value);
        if (rc != GT_OK)
        {
            return GT_OK;
        }
        cnmGenerationCfgPtr->defaultVlanId = (GT_U16)(value & 0xFFF);
        cnmGenerationCfgPtr->qosProfileId = (value >> 21) & 0x7F;

        /* get CNM Header Configuration register*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltCnmHeaderConfig;
        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);
        if (rc != GT_OK)
        {
            return rc;
        }

        cnmGenerationCfgPtr->appendPacket =  BIT2BOOL_MAC((!(value>>25) & 0x1));
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnMessageGenerationConfigGet
*
* DESCRIPTION:
*       Gets CNM generation configuration.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageGenerationConfigGet
(
    IN  GT_U8    devNum,
    OUT CPSS_PORT_CNM_GENERATION_CONFIG_STC *cnmGenerationCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnMessageGenerationConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cnmGenerationCfgPtr));

    rc = internal_cpssDxChPortCnMessageGenerationConfigGet(devNum, cnmGenerationCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cnmGenerationCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnPrioritySpeedLocationSet
*
* DESCRIPTION:
*       Sets location of the priority/speed bits in the CNM header for parsing
*       and generation of CNM.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum                  - device number.
*       prioritySpeedLocation   - priority speed location.
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
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnPrioritySpeedLocationSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT prioritySpeedLocation
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32    startBit;          /* start bit of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    switch (prioritySpeedLocation)
    {
        case CPSS_DXCH_PORT_CN_PRIORITY_SPEED_CPID_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_CN_SDU_UP_E:
            value = 1;
            break;
        case CPSS_DXCH_PORT_CN_SPEED_E:
            value = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
        startBit = 16;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        startBit = 19;
    }

    rc =  prvCpssDrvHwPpSetRegField(devNum, regAddr, startBit, 2, value);
    if (rc != GT_OK)
    {
        return rc;
    }
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;
    }
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 2, value);

}

/*******************************************************************************
* cpssDxChPortCnPrioritySpeedLocationSet
*
* DESCRIPTION:
*       Sets location of the priority/speed bits in the CNM header for parsing
*       and generation of CNM.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum                  - device number.
*       prioritySpeedLocation   - priority speed location.
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
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPrioritySpeedLocationSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT prioritySpeedLocation
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnPrioritySpeedLocationSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, prioritySpeedLocation));

    rc = internal_cpssDxChPortCnPrioritySpeedLocationSet(devNum, prioritySpeedLocation);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, prioritySpeedLocation));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnPrioritySpeedLocationGet
*
* DESCRIPTION:
*       Gets location of the index (priority/speed) bits in the CNM header
*       for parsing and generation of CNM.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       prioritySpeedLocationPtr - (pointer to) priority speed location.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnPrioritySpeedLocationGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT *prioritySpeedLocationPtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    GT_U32    startBit;          /* start bit of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(prioritySpeedLocationPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
        startBit = 16;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        startBit = 19;
    }

    rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, startBit, 2, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *prioritySpeedLocationPtr = (CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT)value;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnPrioritySpeedLocationGet
*
* DESCRIPTION:
*       Gets location of the index (priority/speed) bits in the CNM header
*       for parsing and generation of CNM.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       prioritySpeedLocationPtr - (pointer to) priority speed location.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnPrioritySpeedLocationGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT *prioritySpeedLocationPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnPrioritySpeedLocationGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, prioritySpeedLocationPtr));

    rc = internal_cpssDxChPortCnPrioritySpeedLocationGet(devNum, prioritySpeedLocationPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, prioritySpeedLocationPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*
 * typedef: PRV_CPSS_PORT_CN_DB_4_QCN_MSG_ENT
 *
 * Description:  Data base type for egf and qcn messages to pfc.
  *
 * Enumerations:
 *         PRV_CPSS_PORT_CN_DB_4_QCN_MSG_CCFC_E - CCFC data base type
 *         PRV_CPSS_PORT_CN_DB_4_QCN_MSG_QCN_E  - QCN  data base type
 *
 */
typedef enum
{
     PRV_CPSS_PORT_CN_DB_4_QCN_MSG_CCFC_E = 0,
     PRV_CPSS_PORT_CN_DB_4_QCN_MSG_QCN_E
}PRV_CPSS_PORT_CN_DB_4_QCN_MSG_ENT;

/*******************************************************************************
* prvCpssDxChPortCnEgfQcnMsg2PfcTypeSet
*
* DESCRIPTION:
*       confgure Data base type for egf and qcn messages to pfc.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*         type      - data base type
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number,portNum,tc
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortCnEgfQcnMsg2PfcTypeSet
(
    IN  GT_U8                         devNum,
    IN  PRV_CPSS_PORT_CN_DB_4_QCN_MSG_ENT type
)
{
    GT_STATUS rc;
    GT_U32 regAddr;
    GT_U32 regVal;
    GT_U32 bitIndex;  /* bit index */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);


    switch (type)
    {
        case PRV_CPSS_PORT_CN_DB_4_QCN_MSG_CCFC_E:
            regVal = 0;
        break;
        case PRV_CPSS_PORT_CN_DB_4_QCN_MSG_QCN_E:
            regVal = 1;
        break;
        default:
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        bitIndex = 13;
    }
    else
    {
        bitIndex = 10;
    }


    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->TXQ.pfc.PFCTriggerGlobalConfig;
    rc = prvCpssHwPpPortGroupSetRegField(devNum, 0, regAddr,
                                            bitIndex,
                                            1,
                                            regVal);
    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortCnMessageTypeSet
*
* DESCRIPTION:
*       Sets congestion notification message type - QCN or CCFC
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*       mType       - CN message type
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
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnMessageTypeSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT   mType
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */
    PRV_CPSS_PORT_CN_DB_4_QCN_MSG_ENT type;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    switch (mType)
    {
        case CPSS_DXCH_PORT_CN_MESSAGE_TYPE_QCN_E:
            value = 0;
            type = PRV_CPSS_PORT_CN_DB_4_QCN_MSG_QCN_E;
            break;
        case CPSS_DXCH_PORT_CN_MESSAGE_TYPE_CCFC_E:
            value = 1;
            type = PRV_CPSS_PORT_CN_DB_4_QCN_MSG_CCFC_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNGlobalConfig;

    rc =  prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortCnEgfQcnMsg2PfcTypeSet(devNum,type);
    return rc;
}

/*******************************************************************************
* cpssDxChPortCnMessageTypeSet
*
* DESCRIPTION:
*       Sets congestion notification message type - QCN or CCFC
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*       mType       - CN message type
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
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageTypeSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT   mType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnMessageTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mType));

    rc = internal_cpssDxChPortCnMessageTypeSet(devNum, mType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortCnMessageTypeGet
*
* DESCRIPTION:
*       Gets congestion notification message type - QCN or CCFC
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       mTypePtr    - (pointer to) CN message type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnMessageTypeGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT   *mTypePtr
)
{
    GT_U32 value;     /* register value */
    GT_U32 regAddr;   /* register address */
    GT_STATUS rc;     /* function return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(mTypePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(devNum).CNGlobalConfig;

    rc =  prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &value);
    if(GT_OK != rc)
    {
        return rc;
    }

    *mTypePtr = (value == 0) ? CPSS_DXCH_PORT_CN_MESSAGE_TYPE_QCN_E : CPSS_DXCH_PORT_CN_MESSAGE_TYPE_CCFC_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnMessageTypeGet
*
* DESCRIPTION:
*       Gets congestion notification message type - QCN or CCFC
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       mTypePtr    - (pointer to) CN message type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageTypeGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_DXCH_PORT_CN_MESSAGE_TYPE_ENT   *mTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnMessageTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mTypePtr));

    rc = internal_cpssDxChPortCnMessageTypeGet(devNum, mTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortCnMessageTriggeringStateSet
*
* DESCRIPTION:
*       Sets CNM Triggering State Variables
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       tcQueue     - traffic class queue (0..7).
*       qcnSampleInterval -  parameter that defines the probability to trigger
*                     the next CN frame based on the current value of the
*                     calculated feedback (Fb), measured in Bytes.
*                     (APPLICABLE RANGES: 0..0xFFFFF)
*       qlenOld     - a snapshot of current length of the queue in buffers,
*                     taken when the QcnSampleInterval expires.
*                     (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number,portNum,tc
*       GT_OUT_OF_RANGE          - on wrong qcnSampleInterval, qlenOld
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       qlenOld reset value and QcnSampleInterval init value must be set by the
*       application prior to configuring the queue to be CN aware
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnMessageTriggeringStateSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U8                   tcQueue,
    IN  GT_U32                  qcnSampleInterval,
    IN  GT_U32                  qlenOld
)
{
    GT_STATUS               rc;             /* return code */
    GT_U32                  txqPortNum;     /* TXQ port number */
    GT_U32                  hwArray[2];     /* table HW data */
    GT_U32                  tableIndex;     /* table index */
    PRV_CPSS_DXCH_TABLE_ENT tableType;      /* table type */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, portNum, txqPortNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);

    if((qcnSampleInterval >= BIT_20) || (qlenOld >= BIT_16))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    U32_SET_FIELD_IN_ENTRY_MAC(hwArray, 0, 20, qcnSampleInterval);
    U32_SET_FIELD_IN_ENTRY_MAC(hwArray, 20, 16, qlenOld);

    tableIndex = tcQueue | (txqPortNum << 3);
    tableType = PRV_CPSS_DXCH_LION3_TABLE_CN_SAMPLE_INTERVALS_E;

    rc = prvCpssDxChWriteTableEntry(devNum, tableType, tableIndex, hwArray);

    return rc;
}

/*******************************************************************************
* cpssDxChPortCnMessageTriggeringStateSet
*
* DESCRIPTION:
*       Sets CNM Triggering State Variables
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       tcQueue     - traffic class queue (0..7).
*       qcnSampleInterval -  parameter that defines the probability to trigger
*                     the next CN frame based on the current value of the
*                     calculated feedback (Fb), measured in Bytes.
*                     (APPLICABLE RANGES: 0..0xFFFFF)
*       qlenOld     - a snapshot of current length of the queue in buffers,
*                     taken when the QcnSampleInterval expires.
*                     (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number,portNum,tc
*       GT_OUT_OF_RANGE          - on wrong qcnSampleInterval, qlenOld
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       qlenOld reset value and QcnSampleInterval init value must be set by the
*       application prior to configuring the queue to be CN aware
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageTriggeringStateSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U8                   tcQueue,
    IN  GT_U32                  qcnSampleInterval,
    IN  GT_U32                  qlenOld
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnMessageTriggeringStateSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tcQueue, qcnSampleInterval, qlenOld));

    rc = internal_cpssDxChPortCnMessageTriggeringStateSet(devNum, portNum, tcQueue, qcnSampleInterval, qlenOld);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tcQueue, qcnSampleInterval, qlenOld));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortCnMessageTriggeringStateGet
*
* DESCRIPTION:
*       Gets CNM Triggering State Variables
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       tcQueue     - traffic class queue (0..7).
*
* OUTPUTS:
*       qcnSampleIntervalPtr -  (pointer to) parameter that defines the probability to trigger
*                     the next CN frame based on the current value of the
*                     calculated feedback (Fb), measured in Bytes.
*       qlenOldPtr  - (pointer to) snapshot of current length of the queue in buffers,
*                     taken when the QcnSampleInterval expires.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number,portNum,tc
*       GT_OUT_OF_RANGE          - on wrong qcnSampleInterval, qlenOld
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       qlenOld reset value and QcnSampleInterval init value must be set by the
*       application prior to configuring the queue to be CN aware
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortCnMessageTriggeringStateGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U8                   tcQueue,
    OUT GT_U32                  *qcnSampleIntervalPtr,
    OUT GT_U32                  *qlenOldPtr
)
{
    GT_STATUS               rc;             /* return code */
    GT_U32                  txqPortNum;     /* TXQ port number */
    GT_U32                  hwArray[2];     /* table HW data */
    GT_U32                  tableIndex;     /* table index */
    PRV_CPSS_DXCH_TABLE_ENT tableType;      /* table type */
    GT_U32                  regValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(devNum, portNum, txqPortNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(tcQueue);
    CPSS_NULL_PTR_CHECK_MAC(qcnSampleIntervalPtr);
    CPSS_NULL_PTR_CHECK_MAC(qlenOldPtr);

    tableIndex = tcQueue | (txqPortNum << 3);
    tableType = PRV_CPSS_DXCH_LION3_TABLE_CN_SAMPLE_INTERVALS_E;

    rc = prvCpssDxChReadTableEntry(devNum, tableType, tableIndex, hwArray);
    if(GT_OK != rc)
    {
        return rc;
    }

    U32_GET_FIELD_IN_ENTRY_MAC(hwArray, 0, 20, regValue);
    *qcnSampleIntervalPtr = regValue;

    U32_GET_FIELD_IN_ENTRY_MAC(hwArray, 20, 16, regValue);
    *qlenOldPtr = regValue;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCnMessageTriggeringStateGet
*
* DESCRIPTION:
*       Gets CNM Triggering State Variables
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       tcQueue     - traffic class queue (0..7).
*
* OUTPUTS:
*       qcnSampleIntervalPtr -  (pointer to) parameter that defines the probability to trigger
*                     the next CN frame based on the current value of the
*                     calculated feedback (Fb), measured in Bytes.
*       qlenOldPtr  - (pointer to) snapshot of current length of the queue in buffers,
*                     taken when the QcnSampleInterval expires.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number,portNum,tc
*       GT_OUT_OF_RANGE          - on wrong qcnSampleInterval, qlenOld
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       qlenOld reset value and QcnSampleInterval init value must be set by the
*       application prior to configuring the queue to be CN aware
*
*******************************************************************************/
GT_STATUS cpssDxChPortCnMessageTriggeringStateGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U8                   tcQueue,
    OUT GT_U32                  *qcnSampleIntervalPtr,
    OUT GT_U32                  *qlenOldPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortCnMessageTriggeringStateGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, tcQueue, qcnSampleIntervalPtr, qlenOldPtr));

    rc = internal_cpssDxChPortCnMessageTriggeringStateGet(devNum, portNum, tcQueue, qcnSampleIntervalPtr, qlenOldPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, tcQueue, qcnSampleIntervalPtr, qlenOldPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


