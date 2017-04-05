/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortEee.c
*
* DESCRIPTION:
*       CPSS DxCh Port's IEEE 802.3az Energy Efficient Ethernet (EEE) Support.
*       NOTEs:
*       1. The device support relevant when the PHY works in EEE slave mode.
*          When the PHY works in EEE master mode, the device is not involved in
*          EEE processing.
*       2. The feature not relevant when port is not 'tri-speed'
*       3. LPI is short for 'Low Power Idle'
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortEee.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/*
 * typedef: enum EEE_FIELD_ENT
 *
 * Description: Enumeration of EEE fields in registers.
 *
 * Enumerations:
 *   Li_limit - field in control 0
 *   Ts_limit - field in control 0
 *
 *   LPI_request_enable - field in control 1
 *   LPI_request_force - field in control 1
 *   LPI_manual_mode - field in control 1
 *   Tw_limit - field in control 1
 *
 *   LPI_Status_Register_full - all bits of LPI_Status_Register
 *
 */
typedef enum{
    Li_limit,            /* control 0 */
    Ts_limit,            /* control 0 */

    LPI_request_enable,  /* control 1 */
    LPI_request_force,   /* control 1 */
    LPI_manual_mode,     /* control 1 */
    Tw_limit,            /* control 1 */

    LPI_Status_Register_full, /* full register */

    EEE_FIELD__MUST_BE_LAST_E
}EEE_FIELD_ENT;

/*
 * typedef: enum EEE_REG_ENT
 *
 * Description: Enumeration of EEE registers.
 *
 * Enumerations:
 *   LPI_control_0_Register - register control 0
 *   LPI_control_1_Register - register control 1
 *   LPI_Status_Register - register LPI_Status_Register
 *
 */
typedef enum{
    LPI_control_0_Register,
    LPI_control_1_Register,
    LPI_Status_Register
}EEE_REG_ENT;

/*
 * eee_port_fields_info
 *
 * Description: DB of positions of fields of EEE
 *
 * Fields:
 *  regId    - the register-Id
 *  startBit - start bit of the field
 *  numOfBits- num of bits of the field
 *
*/
static struct{
    EEE_REG_ENT  regId;
    GT_U32  startBit;
    GT_U32  numOfBits;
}eee_port_fields_info[EEE_FIELD__MUST_BE_LAST_E] =
{
     {LPI_control_0_Register,0,8}/*Li_limit,             control 0 */
    ,{LPI_control_0_Register,8,8}/*Ts_limit,             control 0 */

    ,{LPI_control_1_Register,0,1}/*LPI_request_enable,   control 1 */
    ,{LPI_control_1_Register,1,1}/*LPI_request_force,    control 1 */
    ,{LPI_control_1_Register,2,1}/*LPI_manual_mode,      control 1 */
    ,{LPI_control_1_Register,4,12}/*Tw_limit,            control 1 */

    ,{LPI_Status_Register,0,32}/*LPI_Status_Register_full,  full register */

};

/*******************************************************************************
* eee_setPerPort
*
* DESCRIPTION:
*       EEE : get field info
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portMacNum - the MAC number !!!
*       fieldId   - the field info to get
* OUTPUTS:
*       regAddrPtr   - (pointer to) the register address to access
*       startBitPtr  - (pointer to) startBit of the field to access
*       numBitsPtr   - (pointer to) number of bit of the field to access
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong fieldId , regId
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS   eee_fieldInfoGet(
    IN GT_U8        devNum,
    IN GT_PHYSICAL_PORT_NUM portMacNum,
    IN EEE_FIELD_ENT fieldId,
    OUT GT_U32  *regAddrPtr,
    OUT GT_U32  *startBitPtr,
    OUT GT_U32  *numBitsPtr
)
{
    GT_U32  regAddr;
    struct dxch_perPortRegs *portRegsPtr;

    if(fieldId >= EEE_FIELD__MUST_BE_LAST_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    portRegsPtr = &PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portMacNum];

    switch(eee_port_fields_info[fieldId].regId)
    {
        case LPI_control_0_Register:
            regAddr = portRegsPtr->eeeLpiControl[0];
            break;
        case LPI_control_1_Register:
            regAddr = portRegsPtr->eeeLpiControl[1];
            break;
        case LPI_Status_Register:
            regAddr = portRegsPtr->eeeLpiStatus;
            break;
        default :
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *regAddrPtr = regAddr;
    *startBitPtr = eee_port_fields_info[fieldId].startBit;
    *numBitsPtr  = eee_port_fields_info[fieldId].numOfBits;

    return GT_OK;
}


/*******************************************************************************
* eee_setPerPort
*
* DESCRIPTION:
*       EEE : set 'per port' field
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (the port must have MAC)
*       fieldId   - the field to set
*       value     - value to set to the field
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong portNum
*       GT_OUT_OF_RANGE          - on value out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS   eee_setPerPort(
    IN GT_U8        devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN EEE_FIELD_ENT fieldId,
    IN GT_U32       value
)
{
    GT_STATUS   rc;
    GT_U32 regAddr;       /* register address */
    GT_U32 portMacNum; /* Mac number */
    GT_U32 portGroupId; /*the port group Id - support multi port groups device */
    GT_U32 startBit,numBits;

    /* check portNum and get converted MAC port number */
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* convert the 'global mac number' to portGroupId,local port -- supporting multi port group device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    rc = eee_fieldInfoGet(devNum,portMacNum,fieldId,&regAddr,&startBit,&numBits);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(value >= (GT_U32)(1 << numBits))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    return  prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,
                               regAddr, startBit, numBits, value);
}

/*******************************************************************************
* eee_getPerPort
*
* DESCRIPTION:
*       EEE : get 'per port' field
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (the port must have MAC)
*       fieldId   - the field to get
* OUTPUTS:
*       valuePtr  - (pointer to) the value read
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong portNum
*       GT_OUT_OF_RANGE          - on value out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS   eee_getPerPort(
    IN GT_U8        devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN EEE_FIELD_ENT fieldId,
    OUT GT_U32       *valuePtr
)
{
    GT_STATUS   rc;
    GT_U32 regAddr;       /* register address */
    GT_U32 portMacNum; /* Mac number */
    GT_U32 portGroupId; /*the port group Id - support multi port groups device */
    GT_U32 startBit,numBits;

    /* check portNum and get converted MAC port number */
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    /* convert the 'global mac number' to portGroupId,local port -- supporting multi port group device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);

    rc = eee_fieldInfoGet(devNum,portMacNum,fieldId,&regAddr,&startBit,&numBits);
    if (rc != GT_OK)
    {
        return rc;
    }

    return  prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,
                               regAddr, startBit, numBits, valuePtr);
}

/*******************************************************************************
* internal_cpssDxChPortEeeLpiRequestEnableSet
*
* DESCRIPTION:
*       Set port enable/disable transmitting of LPI signaling to the PHY and
*       identification of LPI on reception.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEeeLpiRequestEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E |
        CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    return eee_setPerPort(devNum,portNum,LPI_request_enable,BOOL2BIT_MAC(enable));
}
/*******************************************************************************
* cpssDxChPortEeeLpiRequestEnableSet
*
* DESCRIPTION:
*       Set port enable/disable transmitting of LPI signaling to the PHY and
*       identification of LPI on reception.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiRequestEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEeeLpiRequestEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChPortEeeLpiRequestEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortEeeLpiRequestEnableGet
*
* DESCRIPTION:
*       Get port's enable/disable transmitting of LPI signaling to the PHY and
*       identification of LPI on reception.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       enablePtr   -  (pointer to)
*                       GT_TRUE  - enabled
*                       GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEeeLpiRequestEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    GT_U32    value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E |
        CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    rc  = eee_getPerPort(devNum,portNum,LPI_request_enable,&value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}
/*******************************************************************************
* cpssDxChPortEeeLpiRequestEnableGet
*
* DESCRIPTION:
*       Get port's enable/disable transmitting of LPI signaling to the PHY and
*       identification of LPI on reception.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       enablePtr   -  (pointer to)
*                       GT_TRUE  - enabled
*                       GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiRequestEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEeeLpiRequestEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChPortEeeLpiRequestEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortEeeLpiManualModeSet
*
* DESCRIPTION:
*       Set port manual mode (enabled/disabled/forced) for LPI signaling.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - the manual mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum or mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEeeLpiManualModeSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT  mode
)
{
    GT_STATUS   rc;
    GT_U32      value1,value2;/* values to set to HW */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E |
        CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    switch(mode)
    {
        case CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_DISABLED_E:
            value1 = 0;
            value2 = 0xFFFFFFFF;/*skip the register setting*/
            break;
        case CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENABLED_E:
            value1 = 1;
            value2 = 0;
            break;
        case CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_FORCED_E:
            value1 = 1;
            value2 = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    }

    if (value2 != 0xFFFFFFFF)
    {
        /* start with field 'force' to make operation 'atomic' */
        rc = eee_setPerPort(devNum,portNum,LPI_request_force,value2);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return eee_setPerPort(devNum,portNum,LPI_manual_mode,value1);
}
/*******************************************************************************
* cpssDxChPortEeeLpiManualModeSet
*
* DESCRIPTION:
*       Set port manual mode (enabled/disabled/forced) for LPI signaling.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - the manual mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum or mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiManualModeSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEeeLpiManualModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mode));

    rc = internal_cpssDxChPortEeeLpiManualModeSet(devNum, portNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortEeeLpiManualModeGet
*
* DESCRIPTION:
*       Get port manual mode (enabled/disabled/forced) for LPI signaling.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       modePtr   - (pointer to) the manual mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEeeLpiManualModeGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT  *modePtr
)
{
    GT_STATUS   rc;
    GT_U32      value; /* value to read from HW */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E |
        CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    rc = eee_getPerPort(devNum,portNum,LPI_manual_mode,&value);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (value == 0)
    {
        *modePtr = CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_DISABLED_E;
        return GT_OK;
    }

    rc = eee_getPerPort(devNum,portNum,LPI_request_force,&value);

    *modePtr = (value == 0) ?
        CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENABLED_E :
        CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_FORCED_E;

    return rc;
}
/*******************************************************************************
* cpssDxChPortEeeLpiManualModeGet
*
* DESCRIPTION:
*       Get port manual mode (enabled/disabled/forced) for LPI signaling.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       modePtr   - (pointer to) the manual mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiManualModeGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEeeLpiManualModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modePtr));

    rc = internal_cpssDxChPortEeeLpiManualModeGet(devNum, portNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortEeeLpiTimeLimitsSet
*
* DESCRIPTION:
*       Set port LPI timers.
*       NOTE: The 'time' granularity is 1 uSec for 1000 Mbps and 10 uSec for 100 Mbps.
*       so the function set value 'as is'.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       twLimit   - The minimum time from LPI de-assertion until valid data can be sent .
*                   (APPLICABLE RANGES: 0..4095)
*       liLimit   - The minimum time since going into LPI and allowing LPI de-assert.
*                   (APPLICABLE RANGES: 0..254)
*       tsLimit   - The minimum time from emptying of Tx FIFO and LPI assert.
*                   (APPLICABLE RANGES: 0..255)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_OUT_OF_RANGE          - on out of range : twLimit , liLimit , tsLimit
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEeeLpiTimeLimitsSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  twLimit,
    IN  GT_U32                  liLimit,
    IN  GT_U32                  tsLimit
)
{
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E |
        CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    /* all other fields are checked inside 'eee_setPerPort' */
    /* but this field not supports the full 8 bits */
    if (liLimit >= 255)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    rc = eee_setPerPort(devNum,portNum,Tw_limit,twLimit);
    if(rc != GT_OK)
    {
        return rc;
    }
    rc = eee_setPerPort(devNum,portNum,Li_limit,liLimit);
    if(rc != GT_OK)
    {
        return rc;
    }
    return eee_setPerPort(devNum,portNum,Ts_limit,tsLimit);
}
/*******************************************************************************
* cpssDxChPortEeeLpiTimeLimitsSet
*
* DESCRIPTION:
*       Set port LPI timers.
*       NOTE: The 'time' granularity is 1 uSec for 1000 Mbps and 10 uSec for 100 Mbps.
*       so the function set value 'as is'.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       twLimit   - The minimum time from LPI de-assertion until valid data can be sent .
*                   (APPLICABLE RANGES: 0..4095)
*       liLimit   - The minimum time since going into LPI and allowing LPI de-assert.
*                   (APPLICABLE RANGES: 0..254)
*       tsLimit   - The minimum time from emptying of Tx FIFO and LPI assert.
*                   (APPLICABLE RANGES: 0..255)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_OUT_OF_RANGE          - on out of range : twLimit , liLimit , tsLimit
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiTimeLimitsSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  twLimit,
    IN  GT_U32                  liLimit,
    IN  GT_U32                  tsLimit
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEeeLpiTimeLimitsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, twLimit, liLimit, tsLimit));

    rc = internal_cpssDxChPortEeeLpiTimeLimitsSet(devNum, portNum, twLimit, liLimit, tsLimit);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, twLimit, liLimit, tsLimit));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortEeeLpiTimeLimitsGet
*
* DESCRIPTION:
*       Get port LPI timers.
*       NOTE: The 'time' granularity is 1 uSec for 1000 Mbps and 10 uSec for 100 Mbps.
*       so the function get value 'as is'.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
* OUTPUTS:
*       twLimitPtr - (pointer to) The minimum time from LPI de-assertion until valid data can be sent .
*       liLimitPtr - (pointer to) The minimum time since going into LPI and allowing LPI de-assert.
*       tsLimitPtr - (pointer to) The minimum time from emptying of Tx FIFO and LPI assert.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEeeLpiTimeLimitsGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *twLimitPtr,
    OUT GT_U32                  *liLimitPtr,
    OUT GT_U32                  *tsLimitPtr
)
{
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E |
        CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(twLimitPtr);
    CPSS_NULL_PTR_CHECK_MAC(liLimitPtr);
    CPSS_NULL_PTR_CHECK_MAC(tsLimitPtr);

    rc = eee_getPerPort(devNum,portNum,Tw_limit,twLimitPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    rc = eee_getPerPort(devNum,portNum,Li_limit,liLimitPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
    return eee_getPerPort(devNum,portNum,Ts_limit,tsLimitPtr);
}

/*******************************************************************************
* cpssDxChPortEeeLpiTimeLimitsGet
*
* DESCRIPTION:
*       Get port LPI timers.
*       NOTE: The 'time' granularity is 1 uSec for 1000 Mbps and 10 uSec for 100 Mbps.
*       so the function get value 'as is'.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
* OUTPUTS:
*       twLimitPtr - (pointer to) The minimum time from LPI de-assertion until valid data can be sent .
*       liLimitPtr - (pointer to) The minimum time since going into LPI and allowing LPI de-assert.
*       tsLimitPtr - (pointer to) The minimum time from emptying of Tx FIFO and LPI assert.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiTimeLimitsGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *twLimitPtr,
    OUT GT_U32                  *liLimitPtr,
    OUT GT_U32                  *tsLimitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEeeLpiTimeLimitsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, twLimitPtr, liLimitPtr, tsLimitPtr));

    rc = internal_cpssDxChPortEeeLpiTimeLimitsGet(devNum, portNum, twLimitPtr, liLimitPtr, tsLimitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, twLimitPtr, liLimitPtr, tsLimitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}




/*******************************************************************************
* internal_cpssDxChPortEeeLpiStatusGet
*
* DESCRIPTION:
*       Get port LPI status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
* OUTPUTS:
*       statusPtr - (pointer to) the status info.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEeeLpiStatusGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_DXCH_PORT_EEE_LPI_STATUS_STC *statusPtr
)
{
    GT_STATUS   rc;
    GT_U32      value; /* value to read from HW */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E |
        CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    rc = eee_getPerPort(devNum,portNum,LPI_Status_Register_full,&value);

    statusPtr->macTxPathLpi     = BIT2BOOL_MAC(((value >> 4) & 1));
    statusPtr->macTxPathLpWait  = BIT2BOOL_MAC(((value >> 3) & 1));
    statusPtr->macRxPathLpi     = BIT2BOOL_MAC(((value >> 2) & 1));
    statusPtr->pcsTxPathLpi     = BIT2BOOL_MAC(((value >> 1) & 1));
    statusPtr->pcsRxPathLpi     = BIT2BOOL_MAC(((value >> 0) & 1));

    return rc;
}

/*******************************************************************************
* cpssDxChPortEeeLpiStatusGet
*
* DESCRIPTION:
*       Get port LPI status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
* OUTPUTS:
*       statusPtr - (pointer to) the status info.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiStatusGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_DXCH_PORT_EEE_LPI_STATUS_STC *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEeeLpiStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, statusPtr));

    rc = internal_cpssDxChPortEeeLpiStatusGet(devNum, portNum, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
