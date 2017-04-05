/*******************************************************************************
*              (c), Copyright 2013, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChProtection.c
*
* DESCRIPTION:
*       CPSS DxCh Protection Switching APIs
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/protection/cpssDxChProtection.h>
#include <cpss/dxCh/dxChxGen/protection/private/prvCpssDxChProtectionLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>

/*******************************************************************************
* internal_cpssDxChProtectionEnableSet
*
* DESCRIPTION:
*       Globally enable/disable protection switching
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - protection switching status:
*                     GT_TRUE: enable protection switching
*                     GT_FALSE: disable protection switching
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
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

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).ePort.ePortGlobalConfig;

    hwData = BOOL2BIT_MAC(enable);
    return prvCpssHwPpSetRegField(devNum, regAddr, 1, 1, hwData);
}

/*******************************************************************************
* cpssDxChProtectionEnableSet
*
* DESCRIPTION:
*       Globally enable/disable protection switching
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - protection switching status:
*                     GT_TRUE: enable protection switching
*                     GT_FALSE: disable protection switching
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChProtectionEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionEnableGet
*
* DESCRIPTION:
*       Get the global enabling status of protection switching
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) protection switching status:
*                     GT_TRUE: protection switching is enabled
*                     GT_FALSE: protection switching is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
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

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).ePort.ePortGlobalConfig;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 1, 1, &hwData);
    if (rc == GT_OK)
    {
        *enablePtr = BIT2BOOL_MAC(hwData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChProtectionEnableGet
*
* DESCRIPTION:
*       Get the global enabling status of protection switching
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) protection switching status:
*                     GT_TRUE: protection switching is enabled
*                     GT_FALSE: protection switching is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChProtectionEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionTxEnableSet
*
* DESCRIPTION:
*       Enable/disable TX protection switching on port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - TX protection switching status on the port:
*                     GT_TRUE: TX protection switching is enabled on the port
*                     GT_FALSE: TX protection switching is disabled on the port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for 1:1 confguration:
*       If the target ePort is enabled for TX protection switching and target
*       ePort's associated <LOC Status> is 1 (Loss of Continuity), then the target
*       ePort LSB is toggled and the packet is sent over the protection path.
*       It's meaningless to call this function with an odd port number. However
*       the function will set the entry if the port is odd and will return GT_OK
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionTxEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    IN  GT_BOOL     enable
)
{
    GT_U32      hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    hwData = BOOL2BIT_MAC(enable);
    return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_LION3_TABLE_EQ_TX_PROTECTION_E,
                                           portNum,
                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                           0,
                                           1,
                                           hwData);
}

/*******************************************************************************
* cpssDxChProtectionTxEnableSet
*
* DESCRIPTION:
*       Enable/disable TX protection switching on port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - TX protection switching status on the port:
*                     GT_TRUE: TX protection switching is enabled on the port
*                     GT_FALSE: TX protection switching is disabled on the port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for 1:1 confguration:
*       If the target ePort is enabled for TX protection switching and target
*       ePort's associated <LOC Status> is 1 (Loss of Continuity), then the target
*       ePort LSB is toggled and the packet is sent over the protection path.
*       It's meaningless to call this function with an odd port number. However
*       the function will set the entry if the port is odd and will return GT_OK
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionTxEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    IN  GT_BOOL     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionTxEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChProtectionTxEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionTxEnableGet
*
* DESCRIPTION:
*       Get the enabling status of TX protection switching on port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) TX protection switching status on the port:
*                     GT_TRUE: TX protection switching is enabled on the port
*                     GT_FALSE: TX protection switching is disabled on the port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for 1:1 confguration:
*       If the target ePort is enabled for TX protection switching and target
*       ePort's associated <LOC Status> is 1 (Loss of Continuity), then the target
*       ePort LSB is toggled and the packet is sent over the protection path.
*       It's meaningless to call this function with an odd port number. However
*       the function will get the entry if the port is odd and will return GT_OK
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionTxEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      hwData;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_EQ_TX_PROTECTION_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        0,
                                        1,
                                        &hwData);
    *enablePtr = BIT2BOOL_MAC(hwData);
    return rc;
}

/*******************************************************************************
* cpssDxChProtectionTxEnableGet
*
* DESCRIPTION:
*       Get the enabling status of TX protection switching on port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) TX protection switching status on the port:
*                     GT_TRUE: TX protection switching is enabled on the port
*                     GT_FALSE: TX protection switching is disabled on the port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for 1:1 confguration:
*       If the target ePort is enabled for TX protection switching and target
*       ePort's associated <LOC Status> is 1 (Loss of Continuity), then the target
*       ePort LSB is toggled and the packet is sent over the protection path.
*       It's meaningless to call this function with an odd port number. However
*       the function will get the entry if the port is odd and will return GT_OK
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionTxEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionTxEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChProtectionTxEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionPortToLocMappingSet
*
* DESCRIPTION:
*       Set the mapping between a port to a LOC (Loss of Continuity) status bit
*       index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       locTableIndex   - an index in the Protection LOC table
*                        (APPLICABLE RANGES: 0..2047)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on out of range index
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionPortToLocMappingSet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    IN  GT_U32      locTableIndex
)
{
    GT_U32  numBits;
    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    numBits = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 13 : 11;

    if (locTableIndex >=  (GT_U32)(BIT_0<<numBits))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_LOC_MAPPING_E,
                                           portNum,
                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                           0,
                                           numBits,
                                           locTableIndex);
}

/*******************************************************************************
* cpssDxChProtectionPortToLocMappingSet
*
* DESCRIPTION:
*       Set the mapping between a port to a LOC (Loss of Continuity) status bit
*       index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       locTableIndex   - an index in the Protection LOC table
*                        (APPLICABLE RANGES: 0..2047)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on out of range index
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionPortToLocMappingSet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    IN  GT_U32      locTableIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionPortToLocMappingSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, locTableIndex));

    rc = internal_cpssDxChProtectionPortToLocMappingSet(devNum, portNum, locTableIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, locTableIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionPortToLocMappingGet
*
* DESCRIPTION:
*       Get the mapping between a port to a LOC (Loss of Continuity) status bit
*       index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*
* OUTPUTS:
*       locTableIndexPtr - (pointer to) an index in the Protection LOC table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionPortToLocMappingGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_U32      *locTableIndexPtr
)
{
    GT_U32  numBits;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(locTableIndexPtr);

    numBits = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 13 : 11;

    return prvCpssDxChReadTableEntryField(devNum,
                                          PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_LOC_MAPPING_E,
                                          portNum,
                                          PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                          0,
                                          numBits,
                                          locTableIndexPtr);
}

/*******************************************************************************
* cpssDxChProtectionPortToLocMappingGet
*
* DESCRIPTION:
*       Get the mapping between a port to a LOC (Loss of Continuity) status bit
*       index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*
* OUTPUTS:
*       locTableIndexPtr - (pointer to) an index in the Protection LOC table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionPortToLocMappingGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_U32      *locTableIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionPortToLocMappingGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, locTableIndexPtr));

    rc = internal_cpssDxChProtectionPortToLocMappingGet(devNum, portNum, locTableIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, locTableIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionLocStatusSet
*
* DESCRIPTION:
*       Set LOC (Loss of Continuity) status
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - LOC index
*                     (APPLICABLE RANGES: 0..2047)
*       status      - LOC status for the specified index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The LOC status is set automatically by the device when LOC is detected
*       if <Enable Protection LOC Update> and <Keepalive Aging Enable> are set
*       in the corresponding OAM entry.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionLocStatusSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  index,
    IN  CPSS_DXCH_PROTECTION_LOC_STATUS_ENT     status
)
{
    GT_U32      regAddr;
    GT_U32      hwData;
    GT_U32      offset;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((status != CPSS_DXCH_PROTECTION_LOC_NOT_DETECTED_E) &&
        (status != CPSS_DXCH_PROTECTION_LOC_DETECTED_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Protection LOC table contains 32 bits per entry */
    offset = index % 32;

    /* Set the mask bit - all the bits except this one will be written to Protection LOC table */
    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).ePort.protectionLOCWriteMask;
    hwData = ~(1 << offset);
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    hwData = (status == CPSS_DXCH_PROTECTION_LOC_NOT_DETECTED_E ? 0 : 1);

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum)) CPSS_TBD_BOOKMARK_BOBCAT3
    {
        /* set the same config to the 'TX' table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                   PRV_CPSS_DXCH_BOBCAT3_TABLE_EQ_TX_PROTECTION_LOC_E,
                   index,
                   PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                   0,
                   1,
                   hwData);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_LION3_TABLE_EQ_PROTECTION_LOC_E,
                                           index,
                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                           0,
                                           1,
                                           hwData);
}

/*******************************************************************************
* cpssDxChProtectionLocStatusSet
*
* DESCRIPTION:
*       Set LOC (Loss of Continuity) status
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - LOC index
*                     (APPLICABLE RANGES: 0..2047)
*       status      - LOC status for the specified index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The LOC status is set automatically by the device when LOC is detected
*       if <Enable Protection LOC Update> and <Keepalive Aging Enable> are set
*       in the corresponding OAM entry.
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionLocStatusSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  index,
    IN  CPSS_DXCH_PROTECTION_LOC_STATUS_ENT     status
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionLocStatusSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, status));

    rc = internal_cpssDxChProtectionLocStatusSet(devNum, index, status);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, status));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionLocStatusGet
*
* DESCRIPTION:
*       Get LOC (Loss of Continuity) status
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - LOC index
*                     (APPLICABLE RANGES: 0..2047)
*
* OUTPUTS:
*       statusPtr   - (pointer to) LOC status for the specified index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionLocStatusGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  index,
    OUT CPSS_DXCH_PROTECTION_LOC_STATUS_ENT     *statusPtr
)
{
    GT_U32      hwData;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_EQ_PROTECTION_LOC_E,
                                        index,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                        0,
                                        1,
                                        &hwData);
    *statusPtr = (hwData == 0 ?
                  CPSS_DXCH_PROTECTION_LOC_NOT_DETECTED_E :
                  CPSS_DXCH_PROTECTION_LOC_DETECTED_E);
    return rc;
}

/*******************************************************************************
* cpssDxChProtectionLocStatusGet
*
* DESCRIPTION:
*       Get LOC (Loss of Continuity) status
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - LOC index
*                     (APPLICABLE RANGES: 0..2047)
*
* OUTPUTS:
*       statusPtr   - (pointer to) LOC status for the specified index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionLocStatusGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  index,
    OUT CPSS_DXCH_PROTECTION_LOC_STATUS_ENT     *statusPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionLocStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, statusPtr));

    rc = internal_cpssDxChProtectionLocStatusGet(devNum, index, statusPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, statusPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionRxExceptionPacketCommandSet
*
* DESCRIPTION:
*       Set the packet command for protection switching RX exception.
*       RX exception occurs if one of the following happens:
*       - traffic receiced from the working path (in TTI action: <RX Enable
*         Protection Switching> = 1 and <RX Is Protection Path> = 0) and the LOC
*         bit associated with the working path is set
*       - traffic receiced from the protection path (in TTI action: <RX Enable
*         Protection Switching> = 1 and <RX Is Protection Path> = 1) and the LOC
*         bit associated with the working path is unset
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       command     - the packet command
*                     (APPLICABLE VALUES:
*                       CPSS_PACKET_CMD_FORWARD_E
*                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                       CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                       CPSS_PACKET_CMD_DROP_HARD_E
*                       CPSS_PACKET_CMD_DROP_SOFT_E)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionRxExceptionPacketCommandSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PACKET_CMD_ENT     command
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

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(hwData,command);

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).ePort.protectionSwitchingRXExceptionConfig;

    return prvCpssHwPpSetRegField(devNum, regAddr, 8, 3, hwData);
}

/*******************************************************************************
* cpssDxChProtectionRxExceptionPacketCommandSet
*
* DESCRIPTION:
*       Set the packet command for protection switching RX exception.
*       RX exception occurs if one of the following happens:
*       - traffic receiced from the working path (in TTI action: <RX Enable
*         Protection Switching> = 1 and <RX Is Protection Path> = 0) and the LOC
*         bit associated with the working path is set
*       - traffic receiced from the protection path (in TTI action: <RX Enable
*         Protection Switching> = 1 and <RX Is Protection Path> = 1) and the LOC
*         bit associated with the working path is unset
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       command     - the packet command
*                     (APPLICABLE VALUES:
*                       CPSS_PACKET_CMD_FORWARD_E
*                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                       CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                       CPSS_PACKET_CMD_DROP_HARD_E
*                       CPSS_PACKET_CMD_DROP_SOFT_E)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionRxExceptionPacketCommandSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PACKET_CMD_ENT     command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionRxExceptionPacketCommandSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, command));

    rc = internal_cpssDxChProtectionRxExceptionPacketCommandSet(devNum, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionRxExceptionPacketCommandGet
*
* DESCRIPTION:
*       Get the packet command for protection switching RX exception.
*       RX exception occurs if one of the following happens:
*       - traffic receiced from the working path (in TTI action: <RX Enable
*         Protection Switching> = 1 and <RX Is Protection Path> = 0) and the LOC
*         bit associated with the working path is set
*       - traffic receiced from the protection path (in TTI action: <RX Enable
*         Protection Switching> = 1 and <RX Is Protection Path> = 1) and the LOC
*         bit associated with the working path is unset
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       commandPtr  - (pointer to) the packet command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionRxExceptionPacketCommandGet
(
    IN  GT_U8                   devNum,
    OUT CPSS_PACKET_CMD_ENT     *commandPtr
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
    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).ePort.protectionSwitchingRXExceptionConfig;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 8, 3, &hwData);
    if (rc == GT_OK)
    {
        PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(*commandPtr,hwData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChProtectionRxExceptionPacketCommandGet
*
* DESCRIPTION:
*       Get the packet command for protection switching RX exception.
*       RX exception occurs if one of the following happens:
*       - traffic receiced from the working path (in TTI action: <RX Enable
*         Protection Switching> = 1 and <RX Is Protection Path> = 0) and the LOC
*         bit associated with the working path is set
*       - traffic receiced from the protection path (in TTI action: <RX Enable
*         Protection Switching> = 1 and <RX Is Protection Path> = 1) and the LOC
*         bit associated with the working path is unset
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       commandPtr  - (pointer to) the packet command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionRxExceptionPacketCommandGet
(
    IN  GT_U8                   devNum,
    OUT CPSS_PACKET_CMD_ENT     *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionRxExceptionPacketCommandGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, commandPtr));

    rc = internal_cpssDxChProtectionRxExceptionPacketCommandGet(devNum, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionRxExceptionCpuCodeSet
*
* DESCRIPTION:
*       Set the packet CPU/drop code for protection switching RX exception
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       cpuCode     - the CPU/drop code
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The CPU/drop code is not relevant when the protection switching RX
*       exception packet command is configured to CPSS_PACKET_CMD_FORWARD_E.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionRxExceptionCpuCodeSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT    cpuCode
)
{
    GT_U32      regAddr;
    GT_U32      hwData;
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChNetIfCpuToDsaCode(cpuCode, &dsaCpuCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).ePort.protectionSwitchingRXExceptionConfig;

    hwData = (GT_U32)dsaCpuCode;
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 8, hwData);
}

/*******************************************************************************
* cpssDxChProtectionRxExceptionCpuCodeSet
*
* DESCRIPTION:
*       Set the packet CPU/drop code for protection switching RX exception
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       cpuCode     - the CPU/drop code
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The CPU/drop code is not relevant when the protection switching RX
*       exception packet command is configured to CPSS_PACKET_CMD_FORWARD_E.
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionRxExceptionCpuCodeSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT    cpuCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionRxExceptionCpuCodeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCode));

    rc = internal_cpssDxChProtectionRxExceptionCpuCodeSet(devNum, cpuCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChProtectionRxExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get the packet CPU/drop code for protection switching RX exception
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to) the CPU/drop code
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The CPU/drop code is not relevant when the protection switching RX
*       exception packet command is configured to CPSS_PACKET_CMD_FORWARD_E.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChProtectionRxExceptionCpuCodeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *cpuCodePtr
)
{
    GT_U32      regAddr;
    GT_U32      hwData;
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).ePort.protectionSwitchingRXExceptionConfig;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 8, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    dsaCpuCode = (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)hwData;
    return prvCpssDxChNetIfDsaToCpuCode(dsaCpuCode, cpuCodePtr);
}

/*******************************************************************************
* cpssDxChProtectionRxExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get the packet CPU/drop code for protection switching RX exception
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to) the CPU/drop code
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The CPU/drop code is not relevant when the protection switching RX
*       exception packet command is configured to CPSS_PACKET_CMD_FORWARD_E.
*
*******************************************************************************/
GT_STATUS cpssDxChProtectionRxExceptionCpuCodeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *cpuCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChProtectionRxExceptionCpuCodeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCodePtr));

    rc = internal_cpssDxChProtectionRxExceptionCpuCodeGet(devNum, cpuCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

