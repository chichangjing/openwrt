/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgPrvEdgeVlan.c
*
* DESCRIPTION:
*       cpss Dx-Ch implementation for Prestera Private Edge VLANs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgPrvEdgeVlan.h>

/*******************************************************************************
* internal_cpssDxChBrgPrvEdgeVlanEnable
*
* DESCRIPTION:
*       This function enables/disables the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE to enable the feature,
*                GT_FALSE to disable the feature
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPrvEdgeVlanEnable
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;      /* value to write into a register */
    GT_U32  fieldOffset;/* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig0;
        fieldOffset = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[0];
        fieldOffset = 30;
    }

    value = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanEnable
*
* DESCRIPTION:
*       This function enables/disables the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE to enable the feature,
*                GT_FALSE to disable the feature
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanEnable
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPrvEdgeVlanEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChBrgPrvEdgeVlanEnable(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPrvEdgeVlanEnableGet
*
* DESCRIPTION:
*       Get status of the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE to enable the feature,
*                   GT_FALSE to disable the feature
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPrvEdgeVlanEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_STATUS rc;
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;      /* register value */
    GT_U32  fieldOffset;/* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig0;
        fieldOffset = 14;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[0];
        fieldOffset = 30;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &value);
    if(GT_OK != rc)
    {
        return rc;
    }

    *enablePtr = (value == 0) ? GT_FALSE : GT_TRUE;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanEnableGet
*
* DESCRIPTION:
*       Get status of the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE to enable the feature,
*                   GT_FALSE to disable the feature
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPrvEdgeVlanEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChBrgPrvEdgeVlanEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPrvEdgeVlanPortEnable
*
* DESCRIPTION:
*       Enable/Disable a specified port to operate in Private Edge VLAN mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number to set state.
*       enable    - GT_TRUE for enabled, GT_FALSE for disabled
*       dstPort   - the destination "uplink" port to which all traffic
*                   received on srcPort&srcDev is forwarded.  This parameter
*                   is ignored if disabling the mode.
*       dstHwDev  - the destination "uplink" physical HW device to which all
*                   traffic received on srcPort&srcDev is forwarded.  This
*                   parameter is ignored if disabling the mode.
*       dstTrunk  - the destination is a trunk member. This parameter
*                   is ignored if disabling the mode.
*                   GT_TRUE - dstPort hold value of trunkId , and dstHwDev is
*                             ignored
*                   GT_FALSE - dstPort hold value of port in device dstHwDev
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad portNum or devNum or dstPort or dstHwDev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPrvEdgeVlanPortEnable
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable,
    IN GT_PORT_NUM      dstPort,
    IN GT_HW_DEV_NUM    dstHwDev,
    IN GT_BOOL          dstTrunk
)
{
    GT_STATUS rc;
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;      /* value to write into a register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_HW_DEV_NUM tempdstHwDev;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    
   /* dstPort and dstHwDev are meaningless in the disable case:*/
    /* replace them by port#0 and srcDev for checking needs  */
    if(enable == GT_FALSE)
    {
        dstPort = 0;
        dstHwDev  = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
        dstTrunk = GT_FALSE;
    }
    else if (dstTrunk == GT_FALSE)
    {
        PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
            dstHwDev, dstPort);

        tempdstHwDev = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
            dstHwDev, dstPort);

        dstPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
            dstHwDev, dstPort);
        dstHwDev = tempdstHwDev;
    }

    if(dstTrunk == GT_TRUE)
    {
        dstHwDev = PRV_CPSS_HW_DEV_NUM_MAC(devNum);/* HW ignore this value */


        if(dstPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if(dstHwDev  > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum) ||
           dstPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        value = BOOL2BIT_MAC(enable);
        /* set the <PORT_PVLAN_EN>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
        if(rc != GT_OK)
            return rc;

        value = BOOL2BIT_MAC(dstTrunk);
        /* set the <PORT_VLAN_IS_TRUNK>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_VLAN_IS_TRUNK_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
        if(rc != GT_OK)
            return rc;

        value = dstPort;
        /* set the <PORT_PVLAN_TRG_EPORT_TRUNK_NUM>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_TRG_EPORT_TRUNK_NUM_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
        if(rc != GT_OK)
            return rc;

        value = dstHwDev;
        /* set the <PORT_PVLAN_TRG_DEV>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_TRG_DEV_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

        /* on dual hw device, in the case target port is routing port, in
           order to fulfill routing triggering requirements, the dstHwDev
           must be same device as the one portNum resides in */
        if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(PRV_CPSS_PP_MAC(devNum)->hwDevNum) &&
           ((dstTrunk == GT_FALSE) && (dstPort == 61)) && /* router port */
           (portGroupId >= (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups / 2)))/* hemisphere 1 */
        {
                dstHwDev = PRV_CPSS_HW_DEV_NUM_MAC(devNum) + 1;
        }

        value = ((enable == GT_TRUE) ? 1 : 0) |
                (((dstTrunk == GT_TRUE) ? 1 : 0) << 1) |
                (dstPort << 2) ;
        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr,23 ,9 ,value);
        if(rc != GT_OK)
        {
            return rc;
        }

        value = dstHwDev;

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr + 0x10 ,0 ,5 ,value);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortEnable
*
* DESCRIPTION:
*       Enable/Disable a specified port to operate in Private Edge VLAN mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number to set state.
*       enable    - GT_TRUE for enabled, GT_FALSE for disabled
*       dstPort   - the destination "uplink" port to which all traffic
*                   received on srcPort&srcDev is forwarded.  This parameter
*                   is ignored if disabling the mode.
*       dstHwDev  - the destination "uplink" physical HW device to which all
*                   traffic received on srcPort&srcDev is forwarded.  This
*                   parameter is ignored if disabling the mode.
*       dstTrunk  - the destination is a trunk member. This parameter
*                   is ignored if disabling the mode.
*                   GT_TRUE - dstPort hold value of trunkId , and dstHwDev is
*                             ignored
*                   GT_FALSE - dstPort hold value of port in device dstHwDev
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad portNum or devNum or dstPort or dstHwDev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortEnable
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable,
    IN GT_PORT_NUM      dstPort,
    IN GT_HW_DEV_NUM    dstHwDev,
    IN GT_BOOL          dstTrunk
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPrvEdgeVlanPortEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable, dstPort, dstHwDev, dstTrunk));

    rc = internal_cpssDxChBrgPrvEdgeVlanPortEnable(devNum, portNum, enable, dstPort, dstHwDev, dstTrunk);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable, dstPort, dstHwDev, dstTrunk));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPrvEdgeVlanPortEnableGet
*
* DESCRIPTION:
*       Get enabling/disabling status to operate in Private Edge VLAN mode
*       and destination parameters for a specified port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number to set state.
*
* OUTPUTS:
*       enablePtr - pointer to status of enabling/disabling a specified port
*                   to operate in Private Edge VLAN mode.
*                   GT_TRUE for enabled,
*                   GT_FALSE for disabled
*       dstPortPtr  - pointer to the destination "uplink" port to which
*                     all trafficreceived on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstHwDevPtr - pointer to the destination "uplink" physical HW device to
*                     which all traffic received on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstTrunkPtr - pointer to the destination is a trunk member.
*                     This parameter is ignored if disabling the mode.
*                     GT_TRUE - dstPort hold value of trunkId , and dstHwDev is
*                               ignored
*                     GT_FALSE - dstPort hold value of port in device dstHwDev
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad portNum or devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPrvEdgeVlanPortEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr,
    OUT GT_PORT_NUM     *dstPortPtr,
    OUT GT_HW_DEV_NUM   *dstHwDevPtr,
    OUT GT_BOOL         *dstTrunkPtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;    /* address of register */
    GT_U32      value;      /* register field value */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(dstPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(dstHwDevPtr);
    CPSS_NULL_PTR_CHECK_MAC(dstTrunkPtr);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <PORT_PVLAN_EN>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
        if(rc != GT_OK)
            return rc;

        *enablePtr = BIT2BOOL_MAC(value);
        /* get the <PORT_VLAN_IS_TRUNK>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_VLAN_IS_TRUNK_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
        if(rc != GT_OK)
            return rc;

        *dstTrunkPtr = BIT2BOOL_MAC(value);

        /* get the <PORT_PVLAN_TRG_EPORT_TRUNK_NUM>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_TRG_EPORT_TRUNK_NUM_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
        if(rc != GT_OK)
            return rc;

        *dstPortPtr = value;

        /* get the <PORT_PVLAN_TRG_DEV>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_TRG_DEV_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
        if(rc != GT_OK)
            return rc;

        *dstHwDevPtr = value;
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr,23 ,9 ,&value);
        if(rc != GT_OK)
        {
            return rc;
        }

        if((value & 0x1) == 1)
        {
            *enablePtr = GT_TRUE;
            *dstPortPtr = (GT_U8)(value >> 2);

            if(((value >> 1) & 0x1) == 1)
            {
                *dstTrunkPtr = GT_TRUE;
            }
            else
            {
                *dstTrunkPtr = GT_FALSE;
                rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,
                                                        regAddr + 0x10 ,0 ,5 ,&value);
                if(rc != GT_OK)
                {
                    return rc;
                }
                *dstHwDevPtr = (GT_U8)value;
                /* on dual hw device, in the case target port is routing port, in
                   order to fulfill routing triggering requirements, the dstHwDev
                   must be same device as the one portNum resides in */
                if(PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(PRV_CPSS_PP_MAC(devNum)->hwDevNum) &&
                   (((*dstTrunkPtr) == GT_FALSE) && ((*dstPortPtr) == 61)) && /* router port */
                   (portGroupId >= (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups / 2)))/* hemisphere 0 */
                {
                    if((*dstHwDevPtr)%2)
                        (*dstHwDevPtr) &= (~BIT_0);
                }
            }
        }
        else
        {
            *enablePtr = GT_FALSE;
        }

    }
    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortEnableGet
*
* DESCRIPTION:
*       Get enabling/disabling status to operate in Private Edge VLAN mode
*       and destination parameters for a specified port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number to set state.
*
* OUTPUTS:
*       enablePtr - pointer to status of enabling/disabling a specified port
*                   to operate in Private Edge VLAN mode.
*                   GT_TRUE for enabled,
*                   GT_FALSE for disabled
*       dstPortPtr  - pointer to the destination "uplink" port to which
*                     all trafficreceived on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstHwDevPtr - pointer to the destination "uplink" physical HW device to
*                     which all traffic received on srcPort&srcDev is forwarded.
*                     This parameter is ignored if disabling the mode.
*       dstTrunkPtr - pointer to the destination is a trunk member.
*                     This parameter is ignored if disabling the mode.
*                     GT_TRUE - dstPort hold value of trunkId , and dstHwDev is
*                               ignored
*                     GT_FALSE - dstPort hold value of port in device dstHwDev
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad portNum or devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr,
    OUT GT_PORT_NUM     *dstPortPtr,
    OUT GT_HW_DEV_NUM   *dstHwDevPtr,
    OUT GT_BOOL         *dstTrunkPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPrvEdgeVlanPortEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr, dstPortPtr, dstHwDevPtr, dstTrunkPtr));

    rc = internal_cpssDxChBrgPrvEdgeVlanPortEnableGet(devNum, portNum, enablePtr, dstPortPtr, dstHwDevPtr, dstTrunkPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr, dstPortPtr, dstHwDevPtr, dstTrunkPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet
*
* DESCRIPTION:
*       Enable/Disable per port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number or CPU port to set
*       enable  - GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_BOOL         enable
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;    /* address of register */
    GT_U32      value;      /* value to write into a register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    value = (GT_TRUE == enable) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* set the <ALL_PKT_TO_PVLAN_UPLINK_EN>*/
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_ALL_PKT_TO_PVLAN_UPLINK_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        value);
    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 22, 1, value);
    }

    return rc;

}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet
*
* DESCRIPTION:
*       Enable/Disable per port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number or CPU port to set
*       enable  - GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_BOOL         enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet
*
* DESCRIPTION:
*       Get Enable state per Port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number or CPU port to set.
*
* OUTPUTS:
*       enablePtr GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;       /* return status */
    GT_U32  regAddr;    /* address of register */
    GT_U32  value;      /* value to write into a register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* get the <ALL_PKT_TO_PVLAN_UPLINK_EN>*/
        rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_L2I_EPORT_TABLE_FIELDS_ALL_PKT_TO_PVLAN_UPLINK_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);
        if(rc != GT_OK)
            return rc;

    }
    else
    {
        PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        regAddr = PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum);

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 22, 1, &value);
        if(rc != GT_OK)
            return rc;

    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet
*
* DESCRIPTION:
*       Get Enable state per Port forwarding control taffic to Private Edge
*       VLAN Uplink. PVE port can be disabled from trapping or mirroring
*       bridged packets to the CPU. In this case, as long as the packet is not
*       assigned a HARD DROP or SOFT DROP, the packet is unconditionally
*       assigned a FORWARD command with the ingress ports configured
*       PVE destination (PVLAN Uplink).
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number or CPU port to set.
*
* OUTPUTS:
*       enablePtr GT_TRUE  - forward control traffic to PVE Uplink
*                 GT_FALSE - not forward control traffic to PVE Uplink,
*                            trapped or mirrored to CPU packets are sent to
*                            CPU port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



