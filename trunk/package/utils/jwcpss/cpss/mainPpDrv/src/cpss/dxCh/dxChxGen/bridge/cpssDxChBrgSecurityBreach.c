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
* cpssDxChBrgSecurityBreach.c
*
* DESCRIPTION:
*       CPSS DxCh implementation for Bridge Security Breach Functionality
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 19 $
*
*******************************************************************************/

/* get the device info and common info */
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSecurityBreach.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachPortVlanDropCntrModeSet
*
* DESCRIPTION:
*      Set Port/VLAN Security Breach Drop Counter to count security breach
*      dropped packets based on there ingress port or their assigned VID.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       cntrCfgPtr          - (pointer to) security breach port/vlan counter
*                             mode and it's configuration parameters.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachPortVlanDropCntrModeSet
(
    IN GT_U8                                        dev,
    IN CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC     *cntrCfgPtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_U32      finalPortGroupId;/* the port group Id that will be used for configuration that
                                    relate to specific port */
    GT_U32       localPort;      /* local port - support multi-port-groups device */
    GT_U32      fieldOffset;    /* The start bit number in the register */
    GT_U32      fieldLength;    /* The number of bits to be written to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(cntrCfgPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                    bridgeEngineConfig.bridgeGlobalConfig1;
        fieldOffset = 17;
    }
    else
    {
        /* get address of Bridge Configuration Register1 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.bridgeGlobalConfigRegArray[1];
        fieldOffset = 19;
    }

    switch (cntrCfgPtr->dropCntMode)
    {
        case CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E:

            /* called without portGroupId , loop done inside the driver */
            rc = prvCpssHwPpSetRegField(dev, regAddr, fieldOffset, 1, 0);
            if (rc != GT_OK)
                return rc;

            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
            {
                /* check port validity */
                PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(dev, cntrCfgPtr->port);
            }
            else
            {
                /* check port validity */
                PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev, cntrCfgPtr->port);
            }

            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            finalPortGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, cntrCfgPtr->port);
            /* loop on all port groups :
                on the port group that 'own' the port , set the needed configuration
                on other port groups put 'NULL port'
            */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

            {
                if(finalPortGroupId == portGroupId)
                {
                    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,cntrCfgPtr->port);
                }
                else
                {
                    localPort = PRV_CPSS_DXCH_NULL_PORT_NUM_CNS;
                }

                if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
                {
                    regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                                bridgeEngineConfig.bridgeSecurityBreachDropCntrCfg0;
                    fieldOffset = 0;
                    fieldLength = 13;
                }
                else
                {
                    fieldOffset = 1;
                    fieldLength = 6;
                }
                rc = prvCpssHwPpPortGroupSetRegField(dev, portGroupId,regAddr, fieldOffset, fieldLength, localPort);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
            break;

        case CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E:
            finalPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

            rc = prvCpssHwPpSetRegField(dev, regAddr, fieldOffset, 1, 1);
            if (rc != GT_OK)
                return rc;

            /* check vlan validity */
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(cntrCfgPtr->vlan);


            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                            bridgeEngineConfig.bridgeSecurityBreachDropCntrCfg1;
                fieldOffset = 0;
                fieldLength = 13;
            }
            else
            {
                fieldOffset = 7;
                fieldLength = 12;
            }
            rc = prvCpssHwPpSetRegField(dev, regAddr, fieldOffset, fieldLength,
                                       cntrCfgPtr->vlan);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(rc == GT_OK)
    {
        /* save the info for 'get' configuration and 'read' counters */
        PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.
                securBreachDropCounterInfo.counterMode = cntrCfgPtr->dropCntMode;
        PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.
                securBreachDropCounterInfo.portGroupId = finalPortGroupId;
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgSecurBreachPortVlanDropCntrModeSet
*
* DESCRIPTION:
*      Set Port/VLAN Security Breach Drop Counter to count security breach
*      dropped packets based on there ingress port or their assigned VID.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       cntrCfgPtr          - (pointer to) security breach port/vlan counter
*                             mode and it's configuration parameters.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortVlanDropCntrModeSet
(
    IN GT_U8                                        dev,
    IN CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC     *cntrCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachPortVlanDropCntrModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, cntrCfgPtr));

    rc = internal_cpssDxChBrgSecurBreachPortVlanDropCntrModeSet(dev, cntrCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, cntrCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachPortVlanDropCntrModeGet
*
* DESCRIPTION:
*       Get mode and it's configuration parameters for security breach
*       Port/VLAN drop counter.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*
* OUTPUTS:
*       cntrCfgPtr          - (pointer to) security breach port/vlan counter
*                             mode and it's configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachPortVlanDropCntrModeGet
(
    IN  GT_U8                                        dev,
    OUT CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC     *cntrCfgPtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to read from register */
    GT_U32      portGroupId;    /*t he port group Id - support multi-port-groups device */
    GT_U32      fieldOffset;    /* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(cntrCfgPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                    bridgeEngineConfig.bridgeGlobalConfig1;
        fieldOffset = 17;
    }
    else
    {
        /* get address of Bridge Configuration Register1 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.bridgeGlobalConfigRegArray[1];
        fieldOffset = 19;
    }

    /* get the info from DB */
    portGroupId = PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.
                securBreachDropCounterInfo.portGroupId;

    /* get security bridge counter mode, vlan and port */
    rc = prvCpssHwPpPortGroupGetRegField(dev, portGroupId,regAddr, 1, fieldOffset, &value);
    if (rc != GT_OK)
        return rc;

    if ((value >> (fieldOffset - 1)) & 0x1)
    {
        /* counts all packets assigned with VID */
        cntrCfgPtr->dropCntMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_VLAN_E;
    }
    else
    {
        /* counts all packets received on security breach drop port*/
        cntrCfgPtr->dropCntMode = CPSS_BRG_SECUR_BREACH_DROP_COUNT_PORT_E;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
            bridgeEngineConfig.bridgeSecurityBreachDropCntrCfg0;

        rc = prvCpssHwPpPortGroupGetRegField(dev, portGroupId,regAddr, 0, 13, &value);
        if (rc != GT_OK)
            return rc;
        cntrCfgPtr->port = value;

        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
            bridgeEngineConfig.bridgeSecurityBreachDropCntrCfg1;

        rc = prvCpssHwPpPortGroupGetRegField(dev, portGroupId,regAddr, 0, 13, &value);
        if (rc != GT_OK)
            return rc;

        cntrCfgPtr->vlan = (GT_U16)value;
    }
    else
    {
        cntrCfgPtr->port = (value & 0x3F);
        cntrCfgPtr->vlan = (GT_U16)((value >> 6) & 0xFFF);
    }

    /* support multi-port-groups device , convert local port to global port */
    cntrCfgPtr->port = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(dev,portGroupId,cntrCfgPtr->port);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgSecurBreachPortVlanDropCntrModeGet
*
* DESCRIPTION:
*       Get mode and it's configuration parameters for security breach
*       Port/VLAN drop counter.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*
* OUTPUTS:
*       cntrCfgPtr          - (pointer to) security breach port/vlan counter
*                             mode and it's configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortVlanDropCntrModeGet
(
    IN  GT_U8                                        dev,
    OUT CPSS_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC     *cntrCfgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachPortVlanDropCntrModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, cntrCfgPtr));

    rc = internal_cpssDxChBrgSecurBreachPortVlanDropCntrModeGet(dev, cntrCfgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, cntrCfgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachGlobalDropCntrGet
*
* DESCRIPTION:
*       Get values of security breach global drop counter. Global security breach
*       counter counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach global counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachGlobalDropCntrGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *counValuePtr
)
{
    return cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet(dev,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                            counValuePtr);
}

/*******************************************************************************
* cpssDxChBrgSecurBreachGlobalDropCntrGet
*
* DESCRIPTION:
*       Get values of security breach global drop counter. Global security breach
*       counter counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach global counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachGlobalDropCntrGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *counValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachGlobalDropCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, counValuePtr));

    rc = internal_cpssDxChBrgSecurBreachGlobalDropCntrGet(dev, counValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, counValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachPortVlanCntrGet
*
* DESCRIPTION:
*       Get value of security breach Port/Vlan drop counter. This counter counts
*       all of the packets received according to Port/Vlan security breach drop
*       count mode configuration and dropped due to any security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach Port/Vlan drop counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachPortVlanCntrGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *counValuePtr
)
{
    return cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet(dev,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                          counValuePtr);
}

/*******************************************************************************
* cpssDxChBrgSecurBreachPortVlanCntrGet
*
* DESCRIPTION:
*       Get value of security breach Port/Vlan drop counter. This counter counts
*       all of the packets received according to Port/Vlan security breach drop
*       count mode configuration and dropped due to any security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach Port/Vlan drop counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortVlanCntrGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *counValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachPortVlanCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, counValuePtr));

    rc = internal_cpssDxChBrgSecurBreachPortVlanCntrGet(dev, counValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, counValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachNaPerPortSet
*
* DESCRIPTION:
*       Enable/Disable per port the unknown Source Addresses to be considered as
*       security breach event. When enabled, the NA message is not sent to CPU
*       and this address is not learned.
*       Only relevant in controlled learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - physical device number
*       portNum             - port number
*       enable              - GT_TRUE - New Address is security breach event
*                             GT_FALSE - New Address is not security breach event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachNaPerPortSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_BOOL              enable
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <NEW_SRC_ADDR_SECURITY_BREACH>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_NEW_SRC_ADDR_SECURITY_BREACH_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* get address of Ingress Port Bridge Configuration Register0 */
        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 20, 1, value);
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgSecurBreachNaPerPortSet
*
* DESCRIPTION:
*       Enable/Disable per port the unknown Source Addresses to be considered as
*       security breach event. When enabled, the NA message is not sent to CPU
*       and this address is not learned.
*       Only relevant in controlled learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - physical device number
*       portNum             - port number
*       enable              - GT_TRUE - New Address is security breach event
*                             GT_FALSE - New Address is not security breach event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachNaPerPortSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN GT_BOOL              enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachNaPerPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgSecurBreachNaPerPortSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachNaPerPortGet
*
* DESCRIPTION:
*       Get per port the status of NA security breach event (enabled/disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - physical device number
*       portNum             - port number
*
* OUTPUTS:
*       enablePtr           - GT_TRUE - NA is security breach event
*                             GT_FALSE - NA is not security breach event
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachNaPerPortGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    OUT GT_BOOL             *enablePtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to read from register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <NEW_SRC_ADDR_SECURITY_BREACH>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_NEW_SRC_ADDR_SECURITY_BREACH_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
        if(rc != GT_OK)
            return rc;

    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* get address of Ingress Port Bridge Configuration Register0 */
        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 20, 1, &value);

        if(rc != GT_OK)
            return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgSecurBreachNaPerPortGet
*
* DESCRIPTION:
*       Get per port the status of NA security breach event (enabled/disabled).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - physical device number
*       portNum             - port number
*
* OUTPUTS:
*       enablePtr           - GT_TRUE - NA is security breach event
*                             GT_FALSE - NA is not security breach event
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachNaPerPortGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    OUT GT_BOOL             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachNaPerPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgSecurBreachNaPerPortGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachMovedStaticAddrSet
*
* DESCRIPTION:
*       When the FDB entry is signed as a static one, then this entry is not
*       subject to aging. In addition, there may be cases where the interface
*       does't match the interface from which this packet was received. In this
*       case, this feature enables/disables to regard moved static adresses as
*       a security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       enable              - GT_TRUE - moved static addresses are regarded as
*                                   security breach event and are dropped.
*                             GT_FALSE - moved static addresses are not regarded
*                                   as security breach event and are forwarded.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachMovedStaticAddrSet
(
    IN GT_U8                dev,
    IN GT_BOOL              enable
)
{
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to write to register */
    GT_U32      fieldOffset;    /* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                    bridgeEngineConfig.bridgeGlobalConfig0;
        fieldOffset = 8;
    }
    else
    {
        /* get address of Bridge Global Configuration Register0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
            bridgeGlobalConfigRegArray[0];
        fieldOffset = 14;
    }

    value = BOOL2BIT_MAC(enable);

    return prvCpssHwPpSetRegField(dev, regAddr, fieldOffset, 1, value);
}

/*******************************************************************************
* cpssDxChBrgSecurBreachMovedStaticAddrSet
*
* DESCRIPTION:
*       When the FDB entry is signed as a static one, then this entry is not
*       subject to aging. In addition, there may be cases where the interface
*       does't match the interface from which this packet was received. In this
*       case, this feature enables/disables to regard moved static adresses as
*       a security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       enable              - GT_TRUE - moved static addresses are regarded as
*                                   security breach event and are dropped.
*                             GT_FALSE - moved static addresses are not regarded
*                                   as security breach event and are forwarded.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachMovedStaticAddrSet
(
    IN GT_U8                dev,
    IN GT_BOOL              enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachMovedStaticAddrSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, enable));

    rc = internal_cpssDxChBrgSecurBreachMovedStaticAddrSet(dev, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachMovedStaticAddrGet
*
* DESCRIPTION:
*       Get if static addresses that are moved, are regarded as Security Breach
*       or not (this is only in case that the fdb entry is static or the
*       interface does't match the interface from which this packet was received).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*
* OUTPUTS:
*       enablePtr           - GT_TRUE - moved static addresses are regarded as
*                                   security breach event and are dropped.
*                             GT_FALSE - moved static addresses are not regarded
*                                   as security breach event and are forwarded.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachMovedStaticAddrGet
(
    IN  GT_U8                dev,
    OUT GT_BOOL              *enablePtr
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32      regAddr;        /* hw register address */
    GT_U32      value;          /* value to write to register */
    GT_U32      fieldOffset;    /* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                    bridgeEngineConfig.bridgeGlobalConfig0;
        fieldOffset = 8;
    }
    else
    {
        /* get address of Bridge Global Configuration Register0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
            bridgeGlobalConfigRegArray[0];
        fieldOffset = 14;
    }

    rc = prvCpssHwPpGetRegField(dev, regAddr, fieldOffset, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgSecurBreachMovedStaticAddrGet
*
* DESCRIPTION:
*       Get if static addresses that are moved, are regarded as Security Breach
*       or not (this is only in case that the fdb entry is static or the
*       interface does't match the interface from which this packet was received).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*
* OUTPUTS:
*       enablePtr           - GT_TRUE - moved static addresses are regarded as
*                                   security breach event and are dropped.
*                             GT_FALSE - moved static addresses are not regarded
*                                   as security breach event and are forwarded.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachMovedStaticAddrGet
(
    IN  GT_U8                dev,
    OUT GT_BOOL              *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachMovedStaticAddrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, enablePtr));

    rc = internal_cpssDxChBrgSecurBreachMovedStaticAddrGet(dev, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* securBreachEventRegInfoGet
*
* DESCRIPTION:
*       Get for register info for given security breach event .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - device number
*       eventType           - security breach event type.
*                             The acceptable events are:
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*                               CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE :
*                                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
* OUTPUTS:
*       regAddrPtr      - (pointer to) register's address
*       startBitPtr     - (pointer to) start bit number in the register
*       numOfBitsPtr    - (pointer to) the number of bits in the register
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS securBreachEventRegInfoGet(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    OUT GT_U32                              *regAddrPtr,
    OUT GT_U32                              *startBitPtr,
    OUT GT_U32                              *numOfBitsPtr
)
{
    GT_U32      regAddr;    /* register's address                   */
    GT_U32      setBit;     /* bit number to set in the register    */
    GT_U32      numOfBits;  /* the number of bits to write to register */

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        numOfBits = 3;

        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                    bridgeEngineConfig.bridgeCommandConfig0;
    }
    else
    {
        numOfBits = 1;

        if (eventType == CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E)
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                            bridgeGlobalConfigRegArray[0];
        else
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                            bridgeGlobalConfigRegArray[1];
    }

    switch (eventType) {

        case CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
            {
                setBit = 0;
            }
            else
            if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(dev))
            {
                setBit = 0;
            }
            else
            {
                setBit = 25;
            }
            break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
            {
                setBit = 6;
            }
            else
            {
                setBit = 26;
            }
            break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
            {
                setBit = 9;
            }
            else
            {
                setBit = 27;
            }
            break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
            {
                setBit = 3;
            }
            else
            {
                setBit = 30;
            }
            break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E:
             setBit = 15;
             break;

        case CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
            {
                setBit = 27;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;

        default:
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *regAddrPtr = regAddr;
    *startBitPtr = setBit;
    *numOfBitsPtr = numOfBits;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachEventDropModeSet
*
* DESCRIPTION:
*       Set for given security breach event it's drop mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       eventType           - security breach event type.
*       dropMode            - soft or hard drop mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachEventDropModeSet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    IN CPSS_DROP_MODE_TYPE_ENT              dropMode
)
{
    CPSS_PACKET_CMD_ENT                  command;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(dropMode == CPSS_DROP_MODE_SOFT_E)
    {
        command = CPSS_PACKET_CMD_DROP_SOFT_E;
    }
    else
    if(dropMode == CPSS_DROP_MODE_HARD_E)
    {
        command = CPSS_PACKET_CMD_DROP_HARD_E;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return cpssDxChBrgSecurBreachEventPacketCommandSet(dev,eventType,command);

}

/*******************************************************************************
* cpssDxChBrgSecurBreachEventDropModeSet
*
* DESCRIPTION:
*       Set for given security breach event it's drop mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       eventType           - security breach event type.
*       dropMode            - soft or hard drop mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventDropModeSet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    IN CPSS_DROP_MODE_TYPE_ENT              dropMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachEventDropModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, eventType, dropMode));

    rc = internal_cpssDxChBrgSecurBreachEventDropModeSet(dev, eventType, dropMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, eventType, dropMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachEventDropModeGet
*
* DESCRIPTION:
*      Get for given security breach event it's drop mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       eventType           - security breach event type.
*
* OUTPUTS:
*       dropModePtr         - (pointer to) drop mode - soft or hard.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachEventDropModeGet
(
    IN  GT_U8                               dev,
    IN  CPSS_BRG_SECUR_BREACH_EVENTS_ENT    eventType,
    OUT CPSS_DROP_MODE_TYPE_ENT             *dropModePtr
)
{
    GT_STATUS   rc;         /* return code                          */
    CPSS_PACKET_CMD_ENT                  command;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(dropModePtr);

    rc =  cpssDxChBrgSecurBreachEventPacketCommandGet(dev,eventType,&command);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(command == CPSS_PACKET_CMD_DROP_HARD_E)
    {
        *dropModePtr = CPSS_DROP_MODE_HARD_E;
    }
    else
    if(command == CPSS_PACKET_CMD_DROP_SOFT_E)
    {
        *dropModePtr = CPSS_DROP_MODE_SOFT_E;
    }
    else
    {
        /* the packet command can't not be converted to drop mode */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgSecurBreachEventDropModeGet
*
* DESCRIPTION:
*      Get for given security breach event it's drop mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - physical device number
*       eventType           - security breach event type.
*
* OUTPUTS:
*       dropModePtr         - (pointer to) drop mode - soft or hard.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventDropModeGet
(
    IN  GT_U8                               dev,
    IN  CPSS_BRG_SECUR_BREACH_EVENTS_ENT    eventType,
    OUT CPSS_DROP_MODE_TYPE_ENT             *dropModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachEventDropModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, eventType, dropModePtr));

    rc = internal_cpssDxChBrgSecurBreachEventDropModeGet(dev, eventType, dropModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, eventType, dropModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChSecurBreachMsgGet
*
* DESCRIPTION:
*       Reads the Security Breach message update. When a security breach occures
*       and the CPU had read the previous message (the message is locked untill
*       CPU reads it, after it the new message can be written) the security
*       massage parameters are updated. Those parameters include: MAC address of
*       the breaching packet, security breach port, VID and security breach code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*
* OUTPUTS:
*       sbMsgPtr    - (pointer to) security breach message
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChSecurBreachMsgGet
(
    IN  GT_U8                           dev,
    OUT CPSS_BRG_SECUR_BREACH_MSG_STC   *sbMsgPtr
)
{
    GT_STATUS                           rc;
    GT_U32                              regAddr;      /* register address */
    GT_U32                              regVal;       /* register value */
    CPSS_BRG_SECUR_BREACH_EVENTS_ENT    breachCode; /* for conversion Cheetah2's breach Code */
    GT_U32                              portGroupId; /*the port group Id - support
                                                       multi-port-groups device */
    GT_U32                              portGroupsBmp; /* port groups to query -
                                                          support multi-port-groups device */
    GT_U32                               localPort;    /* Local port number for specific port
                                                         group */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(sbMsgPtr);

    /* get address of Security Breach Status Register0 */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                    securityBreachStatus.securityBreachStatus0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                        bridgeSecurBreachStatusRegArray[0];
    }


    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
    {
        portGroupId = PRV_CPSS_PP_MAC(dev)->portGroupsInfo.firstActivePortGroup;
    }
    else
    {
        portGroupId =
            PRV_CPSS_PP_MAC(dev)->portGroupsInfo.secureBreachNextPortGroupToServe;

        /* start with bmp of all active port groups */
        portGroupsBmp = PRV_CPSS_PP_MAC(dev)->portGroupsInfo.activePortGroupsBmp;


        /* look for port group to query */
        while(0 == (portGroupsBmp & (1 << portGroupId)))
        {
            if(portGroupId > PRV_CPSS_PP_MAC(dev)->portGroupsInfo.lastActivePortGroup)
            {
                portGroupId = PRV_CPSS_PP_MAC(dev)->portGroupsInfo.firstActivePortGroup;
                break;
            }

            portGroupId++;
        }

        /* save the info about the next port group to be served */
        PRV_CPSS_PP_MAC(dev)->portGroupsInfo.secureBreachNextPortGroupToServe =
            (portGroupId + 1);
    }


    rc = prvCpssHwPpPortGroupReadRegister(dev, portGroupId, regAddr, &regVal);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* Take Bits[31:0] of source MAC address of breaching packet */
    sbMsgPtr->macSa.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(regVal, 0, 8);
    sbMsgPtr->macSa.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(regVal, 8, 8);
    sbMsgPtr->macSa.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(regVal, 16, 8);
    sbMsgPtr->macSa.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(regVal, 24, 8);

    /* Read Security Breach Status Register1 */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                    securityBreachStatus.securityBreachStatus1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                    bridgeSecurBreachStatusRegArray[1];
    }

    rc = prvCpssHwPpPortGroupReadRegister(dev, portGroupId, regAddr, &regVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Take Bits[32:47] of source MAC address of breaching packet */
    sbMsgPtr->macSa.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(regVal, 0, 8);
    sbMsgPtr->macSa.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(regVal, 8, 8);

    /* Read Security Breach Status Register2 */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                    securityBreachStatus.securityBreachStatus2;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
                                        bridgeSecurBreachStatusRegArray[2];
    }

    rc = prvCpssHwPpPortGroupReadRegister(dev, portGroupId, regAddr, &regVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        CPSS_TBD_BOOKMARK/* need to support ePort --> need to change GT_U8 to GT_PORT_NUM */
        sbMsgPtr->port = (GT_U8)U32_GET_FIELD_MAC(regVal, 0, 13);
        breachCode     = U32_GET_FIELD_MAC(regVal, 22, 8);
    }
    else
    {
        /* local source port on which the security breaching packet was received */
        localPort = (GT_U8)U32_GET_FIELD_MAC(regVal, 16, 6);

        /* Convert Local port to Global one */
        sbMsgPtr->port =
            PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(dev, portGroupId, localPort);

        /* check the type of the Cheetah family */
        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            breachCode = U32_GET_FIELD_MAC(regVal, 22, 5);
        }
        else
        {
            breachCode = U32_GET_FIELD_MAC(regVal, 0, 4);

            if (breachCode > 8)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* convert Cheetah values to Cheetah2 values */
            if (breachCode == 7)
            {
                /* CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NA_E */
                breachCode = 2;
            }
            else if (breachCode == 8)
            {
                /* GT_SECUR_BREACH_CODE_VLAN_NA_E */
                breachCode = 20;
            }
            /* CPSS_BRG_SECUR_BREACH_EVENTS_FDB_ENTRY_E is the same for both Cheetas */
            else if (breachCode > 1)
            {
                /* increase the breach code for Cheetah2 value */
                breachCode++;
            }
        }
    }

    /* type of security breach, hw values are identical to sw */
    sbMsgPtr->code = breachCode;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
                    securityBreachStatus.securityBreachStatus3;

        rc = prvCpssHwPpPortGroupReadRegister(dev, portGroupId, regAddr, &regVal);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* vid assigned to the security breaching packet */
        sbMsgPtr->vlan = (GT_U16)U32_GET_FIELD_MAC(regVal, 0, 13);
    }
    else
    {
        /* vid assigned to the security breaching packet */
        sbMsgPtr->vlan = (GT_U16)U32_GET_FIELD_MAC(regVal, 4, 12);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChSecurBreachMsgGet
*
* DESCRIPTION:
*       Reads the Security Breach message update. When a security breach occures
*       and the CPU had read the previous message (the message is locked untill
*       CPU reads it, after it the new message can be written) the security
*       massage parameters are updated. Those parameters include: MAC address of
*       the breaching packet, security breach port, VID and security breach code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*
* OUTPUTS:
*       sbMsgPtr    - (pointer to) security breach message
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChSecurBreachMsgGet
(
    IN  GT_U8                           dev,
    OUT CPSS_BRG_SECUR_BREACH_MSG_STC   *sbMsgPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChSecurBreachMsgGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, sbMsgPtr));

    rc = internal_cpssDxChSecurBreachMsgGet(dev, sbMsgPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, sbMsgPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet
*
* DESCRIPTION:
*       Get value of security breach Port/Vlan drop counter. This counter counts
*       all of the packets received according to Port/Vlan security breach drop
*       count mode configuration and dropped due to any security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach Port/Vlan drop counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet
(
    IN  GT_U8                   dev,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *counValuePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(counValuePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(dev, portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
            bridgeDropCntrAndSecurityBreachDropCntrs.portVLANSecurityBreachDropCntr;
    }
    else
    {
        /* get address of Global Security Breach Filter Counter */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
            bridgePortVlanSecurBreachDropCounter;
    }

    rc = prvCpssPortGroupsBmpCounterSummary(dev, portGroupsBmp,
                                                regAddr, 0, 32,
                                                counValuePtr, NULL);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet
*
* DESCRIPTION:
*       Get value of security breach Port/Vlan drop counter. This counter counts
*       all of the packets received according to Port/Vlan security breach drop
*       count mode configuration and dropped due to any security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach Port/Vlan drop counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet
(
    IN  GT_U8                   dev,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *counValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portGroupsBmp, counValuePtr));

    rc = internal_cpssDxChBrgSecurBreachPortGroupPortVlanCntrGet(dev, portGroupsBmp, counValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portGroupsBmp, counValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet
*
* DESCRIPTION:
*       Get values of security breach global drop counter. Global security breach
*       counter counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach global counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet
(
    IN  GT_U8                   dev,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *counValuePtr
)
{
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(counValuePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(dev, portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(dev).
            bridgeDropCntrAndSecurityBreachDropCntrs.globalSecurityBreachFilterCntr;
    }
    else
    {
        /* get address of Global Security Breach Filter Counter */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.
            bridgeGlobalSecurBreachDropCounter;
    }

    return prvCpssPortGroupsBmpCounterSummary(dev, portGroupsBmp,
                                                  regAddr, 0, 32,
                                                  counValuePtr, NULL);
}

/*******************************************************************************
* cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet
*
* DESCRIPTION:
*       Get values of security breach global drop counter. Global security breach
*       counter counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       counValuePtr    - (pointer to) secure breach global counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet
(
    IN  GT_U8                   dev,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *counValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portGroupsBmp, counValuePtr));

    rc = internal_cpssDxChBrgSecurBreachPortGroupGlobalDropCntrGet(dev, portGroupsBmp, counValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portGroupsBmp, counValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachEventPacketCommandSet
*
* DESCRIPTION:
*       Set for given security breach event it's packet command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - device number
*       eventType           - security breach event type.
*                             The acceptable events are:
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*                               CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE :
*                                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       command             - packet command. (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E;
*                                   CPSS_PACKET_CMD_DROP_SOFT_E)
*                             for Bobcat2; Caelum; Bobcat3 : also (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_FORWARD_E;
*                                   CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
*                                   CPSS_PACKET_CMD_TRAP_TO_CPU_E)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this function similar to cpssDxChBrgSecurBreachEventDropModeSet , but
*       allow additional packet commands.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachEventPacketCommandSet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    IN CPSS_PACKET_CMD_ENT                  command
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register's address                   */
    GT_U32      setBit;     /* bit number to set in the register    */
    GT_U32      value;      /* the value to write to register       */
    GT_U32      numOfBits;  /* the number of bits to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        /* convert SW command to HW value */
        PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value, command);
    }
    else
    {
        /* convert SW command to HW value */
        if(command == CPSS_PACKET_CMD_DROP_HARD_E)
        {
            value = 1;
        }
        else
        if(command == CPSS_PACKET_CMD_DROP_SOFT_E)
        {
            value = 0;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* get register info for this event */
    rc = securBreachEventRegInfoGet(dev , eventType ,&regAddr , &setBit , &numOfBits);
    if(rc != GT_OK)
    {
        return rc;
    }


    return prvCpssHwPpSetRegField(dev, regAddr, setBit, numOfBits, value);
}

/*******************************************************************************
* cpssDxChBrgSecurBreachEventPacketCommandSet
*
* DESCRIPTION:
*       Set for given security breach event it's packet command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - device number
*       eventType           - security breach event type.
*                             The acceptable events are:
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*                               CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE :
*                                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*       command             - packet command. (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_DROP_HARD_E;
*                                   CPSS_PACKET_CMD_DROP_SOFT_E)
*                             for Bobcat2; Caelum; Bobcat3 : also (APPLICABLE VALUES:
*                                   CPSS_PACKET_CMD_FORWARD_E;
*                                   CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
*                                   CPSS_PACKET_CMD_TRAP_TO_CPU_E)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this function similar to cpssDxChBrgSecurBreachEventDropModeSet , but
*       allow additional packet commands.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventPacketCommandSet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    IN CPSS_PACKET_CMD_ENT                  command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachEventPacketCommandSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, eventType, command));

    rc = internal_cpssDxChBrgSecurBreachEventPacketCommandSet(dev, eventType, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, eventType, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgSecurBreachEventPacketCommandGet
*
* DESCRIPTION:
*       Get for given security breach event it's packet command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - device number
*       eventType           - security breach event type.
*                             The acceptable events are:
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*                               CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE :
*                                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
* OUTPUTS:
*       commandPtr           - (pointer to) packet command.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this function similar to cpssDxChBrgSecurBreachEventDropModeGet , but
*       allow additional packet commands.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgSecurBreachEventPacketCommandGet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
{
    GT_STATUS   rc;         /* return code                          */
    GT_U32      regAddr;    /* register's address                   */
    GT_U32      setBit;     /* bit number to get in the register    */
    GT_U32      value;      /* the value to read from register       */
    GT_U32      numOfBits;  /* the number of bits to read from register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    /* get register info for this event */
    rc = securBreachEventRegInfoGet(dev , eventType, &regAddr , &setBit , &numOfBits);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpGetRegField(dev, regAddr, setBit, numOfBits, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE)
    {
        /* convert HW value to SW command*/
        PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC((*commandPtr) , value);
    }
    else
    {
        /* convert HW value to SW command*/
        (*commandPtr) = value ?
            CPSS_PACKET_CMD_DROP_HARD_E :
            CPSS_PACKET_CMD_DROP_SOFT_E;
    }


    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgSecurBreachEventPacketCommandGet
*
* DESCRIPTION:
*       Get for given security breach event it's packet command.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev                 - device number
*       eventType           - security breach event type.
*                             The acceptable events are:
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*                                 CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*                               CPSS_BRG_SECUR_BREACH_EVENTS_FRAME_TYPE :
*                                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
* OUTPUTS:
*       commandPtr           - (pointer to) packet command.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - bad input parameters.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this function similar to cpssDxChBrgSecurBreachEventDropModeGet , but
*       allow additional packet commands.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgSecurBreachEventPacketCommandGet
(
    IN GT_U8                                dev,
    IN CPSS_BRG_SECUR_BREACH_EVENTS_ENT     eventType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgSecurBreachEventPacketCommandGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, eventType, commandPtr));

    rc = internal_cpssDxChBrgSecurBreachEventPacketCommandGet(dev, eventType, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, eventType, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


