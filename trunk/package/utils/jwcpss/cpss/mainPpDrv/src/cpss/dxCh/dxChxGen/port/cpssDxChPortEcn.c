/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortEcn.c
*
* DESCRIPTION:
*       CPSS DxCh Port ECN APIs implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortEcn.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>

/*******************************************************************************
* internal_cpssDxChPortEcnMarkingEnableSet
*
* DESCRIPTION:
*       Enable/Disable ECN marking.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       protocolStack - type of IP stack
*       enable  - GT_TRUE: ECN marking enable
*                 GT_FALSE: ECN marking disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or protocolStack parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEcnMarkingEnableSet
(
    IN GT_U8                               devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    IN GT_BOOL                             enable
)
{
    GT_U32                      regAddr;        /* register address */
    GT_U32                      fieldValue;     /* field Value      */
    GT_U32                      fieldOffset;    /* field offset     */
    GT_U32                      fieldSize;      /* field size     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;

    fieldValue = BOOL2BIT_MAC(enable);
    fieldSize = 1;
    switch (protocolStack)
    {
       case CPSS_IP_PROTOCOL_IPV4_E:
          /*Enable ECN marking for IPv4*/
            fieldOffset = 22;
            break;
       case CPSS_IP_PROTOCOL_IPV6_E:
          /*Enable ECN marking for IPv6*/
            fieldOffset = 23;
            break;
       case CPSS_IP_PROTOCOL_IPV4V6_E:
          /*Enable ECN marking for dual stack*/
            fieldOffset = 22;
            fieldValue *= 3; /* assign 22,23 bits to be b00 or b11 */
            fieldSize = 2;
            break;
       default:
           CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, fieldSize, fieldValue);
}

/*******************************************************************************
* cpssDxChPortEcnMarkingEnableSet
*
* DESCRIPTION:
*       Enable/Disable ECN marking.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       protocolStack - type of IP stack
*       enable  - GT_TRUE: ECN marking enable
*                 GT_FALSE: ECN marking disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or protocolStack parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortEcnMarkingEnableSet
(
    IN GT_U8                               devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    IN GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEcnMarkingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, protocolStack, enable));

    rc = internal_cpssDxChPortEcnMarkingEnableSet(devNum, protocolStack, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, protocolStack, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortEcnMarkingEnableGet
*
* DESCRIPTION:
*       Get status of ECN marking.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       protocolStack - type of IP stack
*
* OUTPUTS:
*       enablePtr  - GT_TRUE: ECN marking enable
*                 GT_FALSE: ECN marking disable
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or protocolStack parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEcnMarkingEnableGet
(
    IN GT_U8                               devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_U32                      regAddr;        /* register address */
    GT_U32                      bitValue;       /* bit value        */
    GT_STATUS                   rc;             /* return code      */
    GT_U32                      fieldOffset;    /* register field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfig;

    switch (protocolStack)
    {
       case CPSS_IP_PROTOCOL_IPV4_E:
          /*Get status of ECN assignment for IPv4 header*/
           fieldOffset = 22;
           break;
       case CPSS_IP_PROTOCOL_IPV6_E:
          /*Get status of ECN assignment for IPv6 header*/
           fieldOffset = 23;
           break;
       default:
          CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &bitValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(bitValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortEcnMarkingEnableGet
*
* DESCRIPTION:
*       Get status of ECN marking.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       protocolStack - type of IP stack
*
* OUTPUTS:
*       enablePtr  - GT_TRUE: ECN marking enable
*                 GT_FALSE: ECN marking disable
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or protocolStack parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortEcnMarkingEnableGet
(
    IN GT_U8                               devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    OUT GT_BOOL                            *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEcnMarkingEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, protocolStack, enablePtr));

    rc = internal_cpssDxChPortEcnMarkingEnableGet(devNum, protocolStack, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, protocolStack, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortEcnMarkingTailDropProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable ECN marking per profile according to Tail Drop limits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       profileSet - tail drop profile ID
*       enablersPtr  - (pointer to) struct of enablers
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or profileSet
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEcnMarkingTailDropProfileEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  CPSS_DXCH_PORT_ECN_ENABLERS_STC     *enablersPtr
)
{
    GT_U32 regAddr;    /* register address     */
    GT_U32 regData;    /* register value       */
    GT_STATUS rc;      /* return code */
    
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(enablersPtr);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet, devNum);


    regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).tailDrop.tailDropConfig.profileECNMarkEn[profileSet];

    regData = (BOOL2BIT_MAC(enablersPtr->sharedPoolLimit) << 2) | 
              (BOOL2BIT_MAC(enablersPtr->tcLimit) << 3) |
              (BOOL2BIT_MAC(enablersPtr->portLimit) << 1) |
              (BOOL2BIT_MAC(enablersPtr->tcDpLimit));

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 4, regData);
    return rc;
}

/*******************************************************************************
* cpssDxChPortEcnMarkingTailDropProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable ECN marking per profile according to Tail Drop limits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       profileSet - tail drop profile ID
*       enablersPtr  - (pointer to) struct of enablers
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or profileSet
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortEcnMarkingTailDropProfileEnableSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    IN  CPSS_DXCH_PORT_ECN_ENABLERS_STC     *enablersPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEcnMarkingTailDropProfileEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileSet, enablersPtr));

    rc = internal_cpssDxChPortEcnMarkingTailDropProfileEnableSet(devNum, profileSet, enablersPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileSet, enablersPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortEcnMarkingTailDropProfileEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable ECN marking per profile according to Tail Drop limits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       profileSet - tail drop profile ID
*
* OUTPUTS:
*       enablersPtr  - (pointer to) struct of enablers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or profileSet
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortEcnMarkingTailDropProfileEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    OUT CPSS_DXCH_PORT_ECN_ENABLERS_STC     *enablersPtr
)
{
    GT_U32 regAddr;    /* register address     */
    GT_U32 regData;    /* register value       */
    GT_STATUS rc;      /* return code */
    
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(enablersPtr);
    PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet, devNum);


    regAddr = PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(devNum).tailDrop.tailDropConfig.profileECNMarkEn[profileSet];

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 4, &regData);
    if(GT_OK != rc)
    {
        return rc;
    }

    enablersPtr->tcDpLimit = BIT2BOOL_MAC(U32_GET_FIELD_MAC(regData, 0, 1));
    enablersPtr->portLimit = BIT2BOOL_MAC(U32_GET_FIELD_MAC(regData, 1, 1));
    enablersPtr->sharedPoolLimit = BIT2BOOL_MAC(U32_GET_FIELD_MAC(regData, 2, 1));
    enablersPtr->tcLimit =  BIT2BOOL_MAC(U32_GET_FIELD_MAC(regData, 3, 1));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortEcnMarkingTailDropProfileEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable ECN marking per profile according to Tail Drop limits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       profileSet - tail drop profile ID
*
* OUTPUTS:
*       enablersPtr  - (pointer to) struct of enablers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or profileSet
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortEcnMarkingTailDropProfileEnableGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet,
    OUT CPSS_DXCH_PORT_ECN_ENABLERS_STC     *enablersPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortEcnMarkingTailDropProfileEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, profileSet, enablersPtr));

    rc = internal_cpssDxChPortEcnMarkingTailDropProfileEnableGet(devNum, profileSet, enablersPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, profileSet, enablersPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

