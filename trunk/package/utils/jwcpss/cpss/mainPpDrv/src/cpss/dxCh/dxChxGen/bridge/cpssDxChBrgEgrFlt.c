/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgEgrFlt.c
*
* DESCRIPTION:
*       Egress filtering facility DxCh cpss implementation
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgEgrFlt.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

/*******************************************************************************
* internal_cpssDxChBrgPortEgrFltUnkEnable
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unknown Unicast packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       portNum  - CPU port, port number
*       enable   - GT_TRUE: Unknown Unicast packets are filtered and are
*                           not forwarded to this port.
*                  GT_FALSE: Unknown Unicast packets are not filtered and may
*                           be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgrFltUnkEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        return prvCpssDxChHwEgfEftFieldSet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_UNKNOWN_UC_FILTER_ENABLE_E,
            BOOL2BIT_MAC(enable));
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(dev,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                     localPort, 1, BOOL2BIT_MAC(enable));
        }
        else
        {
            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                        filterConfig.unknownUcFilterEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, BOOL2BIT_MAC(enable));
        }
    }
}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUnkEnable
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unknown Unicast packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       portNum  - CPU port, port number
*       enable   - GT_TRUE: Unknown Unicast packets are filtered and are
*                           not forwarded to this port.
*                  GT_FALSE: Unknown Unicast packets are not filtered and may
*                           be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUnkEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgrFltUnkEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enable));

    rc = internal_cpssDxChBrgPortEgrFltUnkEnable(dev, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPortEgrFltUnkEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unknown Unicast packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - CPU port, port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unknown unicast packets filtering
*                   GT_TRUE:  Unknown Unicast packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: Unknown Unicast packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgrFltUnkEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        rc = prvCpssDxChHwEgfEftFieldGet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_UNKNOWN_UC_FILTER_ENABLE_E,
            &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(dev,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                                           PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                                           localPort, 1, &value);
        }
        else
        {
            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                        filterConfig.unknownUcFilterEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, &value);
        }
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUnkEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unknown Unicast packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - CPU port, port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unknown unicast packets filtering
*                   GT_TRUE:  Unknown Unicast packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: Unknown Unicast packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUnkEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgrFltUnkEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enablePtr));

    rc = internal_cpssDxChBrgPortEgrFltUnkEnableGet(dev, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPortEgrFltUregMcastEnable
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unregistered Multicast packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       portNum  - CPU port, port number
*       enable   - GT_TRUE: Unregistered Multicast packets are filtered and
*                           are not forwarded to this port.
*                  GT_FALSE:Unregistered Multicast packets are not filtered
*                           and may be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgrFltUregMcastEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        return prvCpssDxChHwEgfEftFieldSet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_UNREGED_MC_FILTER_ENABLE_E,
            BOOL2BIT_MAC(enable));
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(dev,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter2,
                     localPort, 1, BOOL2BIT_MAC(enable));
        }
        else
        {
            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                        filterConfig.unregisteredMcFilterEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, BOOL2BIT_MAC(enable));
        }
    }
}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregMcastEnable
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unregistered Multicast packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       portNum  - CPU port, port number
*       enable   - GT_TRUE: Unregistered Multicast packets are filtered and
*                           are not forwarded to this port.
*                  GT_FALSE:Unregistered Multicast packets are not filtered
*                           and may be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregMcastEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgrFltUregMcastEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enable));

    rc = internal_cpssDxChBrgPortEgrFltUregMcastEnable(dev, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPortEgrFltUregMcastEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unregistered Multicast packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - CPU port, port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unregistered multicast packets filtering
*                   GT_TRUE:  Unregistered Multicast packets are filtered and
*                             are not forwarded to this port.
*                   GT_FALSE: Unregistered Multicast packets are not filtered
*                             and may be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgrFltUregMcastEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        rc = prvCpssDxChHwEgfEftFieldGet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_UNREGED_MC_FILTER_ENABLE_E,
            &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(dev,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                                           PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter2,
                                           localPort, 1, &value);
        }
        else
        {
            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                        filterConfig.unregisteredMcFilterEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, &value);
        }
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregMcastEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unregistered Multicast packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - CPU port, port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unregistered multicast packets filtering
*                   GT_TRUE:  Unregistered Multicast packets are filtered and
*                             are not forwarded to this port.
*                   GT_FALSE: Unregistered Multicast packets are not filtered
*                             and may be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregMcastEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgrFltUregMcastEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enablePtr));

    rc = internal_cpssDxChBrgPortEgrFltUregMcastEnableGet(dev, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN Egress Filtering on specified device for Bridged
*       Known Unicast packets.
*       If enabled the VLAN egress filter verifies that the egress port is a
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: VLAN egress filtering verifies that the egress
*                          port is a member of the packet's VLAN classification
*                 GT_FLASE: the VLAN egress filtering check is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       VLAN egress filtering is required by 802.1Q, but if desired, this
*       mechanism can be disabled, thus allowing "leaky VLANs".
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        return prvCpssHwPpSetRegField(dev,
            PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(dev).global.SHTGlobalConfigs,
            0,1,BOOL2BIT_MAC(enable));
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return prvCpssHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                 29, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 0, 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgVlanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN Egress Filtering on specified device for Bridged
*       Known Unicast packets.
*       If enabled the VLAN egress filter verifies that the egress port is a
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: VLAN egress filtering verifies that the egress
*                          port is a member of the packet's VLAN classification
*                 GT_FLASE: the VLAN egress filtering check is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       VLAN egress filtering is required by 802.1Q, but if desired, this
*       mechanism can be disabled, thus allowing "leaky VLANs".
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEgressFilteringEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, enable));

    rc = internal_cpssDxChBrgVlanEgressFilteringEnable(dev, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgVlanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on specified device for Bridged Known Unicast packets.
*      If enabled the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged known unicast packets filtering
*                   GT_TRUE:   VLAN egress filtering verifies that the egress
*                              port is a member of the packet's VLAN classification
*                   GT_FLASE: the VLAN egress filtering check is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgVlanEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        rc = prvCpssHwPpGetRegField(dev,
            PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(dev).global.SHTGlobalConfigs,
            0,1,&value);
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        rc = prvCpssHwPpGetRegField(dev,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                                       29, 1, &value);
    }
    else
    {
        rc =  prvCpssHwPpGetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 0, 1, &value);
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgVlanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on specified device for Bridged Known Unicast packets.
*      If enabled the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged known unicast packets filtering
*                   GT_TRUE:   VLAN egress filtering verifies that the egress
*                              port is a member of the packet's VLAN classification
*                   GT_FLASE: the VLAN egress filtering check is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgVlanEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgVlanEgressFilteringEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, enablePtr));

    rc = internal_cpssDxChBrgVlanEgressFilteringEnableGet(dev, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgRoutedUnicastEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN egress filtering on Routed Unicast packets.
*       If disabled, the destination port may or may not be a member of the VLAN.
*       If enabled, the VLAN egress filter verifies that the egress port is a
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: Egress filtering is enabled
*                  GT_FLASE: Egress filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgRoutedUnicastEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        return prvCpssHwPpSetRegField(dev,
            PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(dev).global.SHTGlobalConfigs,
            1,1,BOOL2BIT_MAC(enable));
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return prvCpssHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                 28, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 1, 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgRoutedUnicastEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN egress filtering on Routed Unicast packets.
*       If disabled, the destination port may or may not be a member of the VLAN.
*       If enabled, the VLAN egress filter verifies that the egress port is a
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: Egress filtering is enabled
*                  GT_FLASE: Egress filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedUnicastEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgRoutedUnicastEgressFilteringEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, enable));

    rc = internal_cpssDxChBrgRoutedUnicastEgressFilteringEnable(dev, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgRoutedUnicastEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on Routed Unicast packets.
*      If disabled, the destination port may or may not be a member of the VLAN.
*      If enabled, the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed unicast packets filtering
*                   GT_TRUE:  Egress filtering is enabled
*                   GT_FLASE: Egress filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgRoutedUnicastEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        rc = prvCpssHwPpGetRegField(dev,
            PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(dev).global.SHTGlobalConfigs,
            1,1,&value);
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        rc = prvCpssHwPpGetRegField(dev,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                                       28, 1, &value);
    }
    else
    {
        rc = prvCpssHwPpGetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 1, 1, &value);
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgRoutedUnicastEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on Routed Unicast packets.
*      If disabled, the destination port may or may not be a member of the VLAN.
*      If enabled, the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed unicast packets filtering
*                   GT_TRUE:  Egress filtering is enabled
*                   GT_FLASE: Egress filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedUnicastEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgRoutedUnicastEgressFilteringEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, enablePtr));

    rc = internal_cpssDxChBrgRoutedUnicastEgressFilteringEnableGet(dev, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgRoutedSpanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable STP egress Filtering on Routed Packets.
*       If disabled the packet may be forwarded to its egress port,
*       regardless of its Span State.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: Span state filtering is enabled
*                  GT_FLASE: Span state filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgRoutedSpanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        return prvCpssHwPpSetRegField(dev,
            PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(dev).global.SHTGlobalConfigs,
            2,1,BOOL2BIT_MAC(enable));
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return prvCpssHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                 31, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        return prvCpssHwPpSetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 2, 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChBrgRoutedSpanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable STP egress Filtering on Routed Packets.
*       If disabled the packet may be forwarded to its egress port,
*       regardless of its Span State.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev      - device number
*       enable   - GT_TRUE: Span state filtering is enabled
*                  GT_FLASE: Span state filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedSpanEgressFilteringEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgRoutedSpanEgressFilteringEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, enable));

    rc = internal_cpssDxChBrgRoutedSpanEgressFilteringEnable(dev, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgRoutedSpanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the STP egress Filtering current state (enable/disable)
*      on Routed packets.
*      If disabled the packet may be forwarded to its egress port,
*      regardless of its Span State.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed packets filtering
*                   GT_TRUE:  Span state filtering is enabled
*                   GT_FLASE: Span state filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgRoutedSpanEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        rc = prvCpssHwPpGetRegField(dev,
            PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(dev).global.SHTGlobalConfigs,
            2,1,&value);
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        rc = prvCpssHwPpGetRegField(dev,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilter,
                                       31, 1, &value);
    }
    else
    {
        rc = prvCpssHwPpGetRegField(dev,
                 PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.sht.global.shtGlobalConfig,
                 2, 1, &value);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgRoutedSpanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the STP egress Filtering current state (enable/disable)
*      on Routed packets.
*      If disabled the packet may be forwarded to its egress port,
*      regardless of its Span State.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed packets filtering
*                   GT_TRUE:  Span state filtering is enabled
*                   GT_FLASE: Span state filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgRoutedSpanEgressFilteringEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgRoutedSpanEgressFilteringEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, enablePtr));

    rc = internal_cpssDxChBrgRoutedSpanEgressFilteringEnableGet(dev, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPortEgrFltUregBcEnable
*
* DESCRIPTION:
*       Enables or disables egress filtering of unregistered broadcast packets.
*       Unregistered broadcast packets are:
*        - packets with destination MAC ff-ff-ff-ff-ff-ff
*        - destination MAC address lookup not finds matching entry
*        - packets were not routed
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       dev     - device number
*       portNum - port number
*       enable  - GT_TRUE  - enable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are dropped
*                 GT_FALSE - disable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are not dropped
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgrFltUregBcEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    /* cheetah2 support it , but cheetah not */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(dev, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        return prvCpssDxChHwEgfEftFieldSet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_UNREGED_BC_FILTER_ENABLE_E,
            BOOL2BIT_MAC(enable));
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilterUnregBc,
                     localPort, 1, BOOL2BIT_MAC(enable));
        }
        else
        {
            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                        txqVer1.egr.filterConfig.unregisteredBcFilterEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, BOOL2BIT_MAC(enable));
        }
    }
}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregBcEnable
*
* DESCRIPTION:
*       Enables or disables egress filtering of unregistered broadcast packets.
*       Unregistered broadcast packets are:
*        - packets with destination MAC ff-ff-ff-ff-ff-ff
*        - destination MAC address lookup not finds matching entry
*        - packets were not routed
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       dev     - device number
*       portNum - port number
*       enable  - GT_TRUE  - enable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are dropped
*                 GT_FALSE - disable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are not dropped
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregBcEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgrFltUregBcEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enable));

    rc = internal_cpssDxChBrgPortEgrFltUregBcEnable(dev, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPortEgrFltUregBcEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      of unregistered broadcast packets.
*      Unregistered broadcast packets are:
*      - packets with destination MAC ff-ff-ff-ff-ff-ff
*      - destination MAC address lookup not finds matching entry
*      - packets were not routed
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       dev         - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) unregistered broadcast packets filtering
*                   GT_TRUE  - enable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are dropped
*                   GT_FALSE - disable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgrFltUregBcEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    /* cheetah2 support it , but cheetah not */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(dev, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        rc = prvCpssDxChHwEgfEftFieldGet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_UNREGED_BC_FILTER_ENABLE_E,
            &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,portNum);


        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                                           PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilterUnregBc,
                                           localPort, 1, &value);
        }
        else
        {
            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                        txqVer1.egr.filterConfig.unregisteredBcFilterEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, &value);
        }
    }

    if (rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPortEgrFltUregBcEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      of unregistered broadcast packets.
*      Unregistered broadcast packets are:
*      - packets with destination MAC ff-ff-ff-ff-ff-ff
*      - destination MAC address lookup not finds matching entry
*      - packets were not routed
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       dev         - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) unregistered broadcast packets filtering
*                   GT_TRUE  - enable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are dropped
*                   GT_FALSE - disable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltUregBcEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgrFltUregBcEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enablePtr));

    rc = internal_cpssDxChBrgPortEgrFltUregBcEnableGet(dev, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPortEgressMcastLocalEnable
*
* DESCRIPTION:
*       Enable/Disable sending Multicast packets back to its source
*       port on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev     - device number
*       portNum - port number
*       enable  - Boolean value:
*                 GT_TRUE  - Multicast packets may be sent back to
*                            their source port on the local device.
*                 GT_FALSE - Multicast packets are not sent back to
*                            their source port on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For xCat and above devices to enable local switching of Multicast,
*       unknown Unicast, and Broadcast traffic, both egress port configuration
*       and the field in the VLAN entry (by function
*       cpssDxChBrgVlanLocalSwitchingEnableSet) must be enabled.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgressMcastLocalEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        return prvCpssDxChHwEgfShtFieldSet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_MC_LOCAL_ENABLE_E,
                GT_TRUE, /*accessPhysicalPort*/
                GT_FALSE, /*accessEPort*/
                BOOL2BIT_MAC(enable));
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.mcastLocalEnableConfig,
                     localPort, 1, BOOL2BIT_MAC(enable));
        }
        else
        {
            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                        filterConfig.mcLocalEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, BOOL2BIT_MAC(enable));
        }
    }
}

/*******************************************************************************
* cpssDxChBrgPortEgressMcastLocalEnable
*
* DESCRIPTION:
*       Enable/Disable sending Multicast packets back to its source
*       port on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev     - device number
*       portNum - port number
*       enable  - Boolean value:
*                 GT_TRUE  - Multicast packets may be sent back to
*                            their source port on the local device.
*                 GT_FALSE - Multicast packets are not sent back to
*                            their source port on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For xCat and above devices to enable local switching of Multicast,
*       unknown Unicast, and Broadcast traffic, both egress port configuration
*       and the field in the VLAN entry (by function
*       cpssDxChBrgVlanLocalSwitchingEnableSet) must be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgressMcastLocalEnable
(
    IN GT_U8            dev,
    IN GT_PHYSICAL_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgressMcastLocalEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enable));

    rc = internal_cpssDxChBrgPortEgressMcastLocalEnable(dev, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPortEgressMcastLocalEnableGet
*
* DESCRIPTION:
*      This function gets current state (enable/disable) for sending
*      Multicast packets back to its source port on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) sending Multicast packets back to its source
*                   GT_TRUE  - Multicast packets may be sent back to
*                              their source port on the local device.
*                   GT_FALSE - Multicast packets are not sent back to
*                              their source port on the local device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgressMcastLocalEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        rc = prvCpssDxChHwEgfShtFieldGet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_MC_LOCAL_ENABLE_E,
                GT_TRUE, /*accessPhysicalPort*/
                &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                                           PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.mcastLocalEnableConfig,
                                           localPort, 1, &value);
        }
        else
        {
            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                        filterConfig.mcLocalEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, &value);
        }
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPortEgressMcastLocalEnableGet
*
* DESCRIPTION:
*      This function gets current state (enable/disable) for sending
*      Multicast packets back to its source port on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) sending Multicast packets back to its source
*                   GT_TRUE  - Multicast packets may be sent back to
*                              their source port on the local device.
*                   GT_FALSE - Multicast packets are not sent back to
*                              their source port on the local device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgressMcastLocalEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgressMcastLocalEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enablePtr));

    rc = internal_cpssDxChBrgPortEgressMcastLocalEnableGet(dev, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPortEgrFltIpMcRoutedEnable
*
* DESCRIPTION:
*      Enable/Disable egress filtering for IP Multicast Routed packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       dev      - device number
*       portNum  - port number
*       enable   - GT_TRUE: IP Multicast Routed packets are filtered and are
*                           not forwarded to this port.
*                  GT_FALSE: IP Multicast Routed packets are not filtered and may
*                           be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgrFltIpMcRoutedEnable
(
    IN GT_U8            dev,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(dev,
           CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        return prvCpssDxChHwEgfShtFieldSet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_IPMC_ROUTED_FILTER_ENABLE_E,
                GT_TRUE, /*accessPhysicalPort*/
                GT_TRUE, /*accessEPort*/
                BOOL2BIT_MAC(enable));
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilterIpMcRouted,
                     localPort, 1, BOOL2BIT_MAC(enable));
        }
        else
        {
            return prvCpssHwPpPortGroupSetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                        filterConfig.ipmcRoutedFilterEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, BOOL2BIT_MAC(enable));
        }
    }
}

/*******************************************************************************
* cpssDxChBrgPortEgrFltIpMcRoutedEnable
*
* DESCRIPTION:
*      Enable/Disable egress filtering for IP Multicast Routed packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       dev      - device number
*       portNum  - port number
*       enable   - GT_TRUE: IP Multicast Routed packets are filtered and are
*                           not forwarded to this port.
*                  GT_FALSE: IP Multicast Routed packets are not filtered and may
*                           be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltIpMcRoutedEnable
(
    IN GT_U8            dev,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgrFltIpMcRoutedEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enable));

    rc = internal_cpssDxChBrgPortEgrFltIpMcRoutedEnable(dev, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgPortEgrFltIpMcRoutedEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for IP Multicast Routed packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       dev             - device number
*       portNum         - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) IP Multicast Routed packets filtering
*                   GT_TRUE:  IP Multicast Routed packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: IP Multicast Routed packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgPortEgrFltIpMcRoutedEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(dev,
           CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(dev) == GT_TRUE)
    {
        rc = prvCpssDxChHwEgfShtFieldGet(dev,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_IPMC_ROUTED_FILTER_ENABLE_E,
                GT_FALSE, /*accessPhysicalPort*/
                &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,portNum);

            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                                           PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->bridgeRegs.egressFilterIpMcRouted,
                                           localPort, 1, &value);
        }
        else
        {
            rc = prvCpssHwPpPortGroupGetRegField(dev,portGroupId,
                     PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->txqVer1.egr.
                        filterConfig.ipmcRoutedFilterEn[OFFSET_TO_WORD_MAC(portNum)],
                     OFFSET_TO_BIT_MAC(portNum), 1, &value);
        }
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgPortEgrFltIpMcRoutedEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for IP Multicast Routed packets on the specified egress port.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       dev             - device number
*       portNum         - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) IP Multicast Routed packets filtering
*                   GT_TRUE:  IP Multicast Routed packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: IP Multicast Routed packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgPortEgrFltIpMcRoutedEnableGet
(
    IN  GT_U8           dev,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgPortEgrFltIpMcRoutedEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, dev, portNum, enablePtr));

    rc = internal_cpssDxChBrgPortEgrFltIpMcRoutedEnableGet(dev, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, dev, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortFilteringEnableSet
*
* DESCRIPTION:
*       Enable/Disable ePort VLAN Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       enable  - GT_FALSE - disable ePort VLAN Egress Filtering.
*                 GT_TRUE  - enable ePort VLAN Egress Filtering.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortFilteringEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    return prvCpssDxChHwEgfShtFieldSet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_EGRESS_VLAN_FILTER_E,
            GT_TRUE, /*accessPhysicalPort*/
            GT_TRUE, /*accessEPort*/
            BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortFilteringEnableSet
*
* DESCRIPTION:
*       Enable/Disable ePort VLAN Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       enable  - GT_FALSE - disable ePort VLAN Egress Filtering.
*                 GT_TRUE  - enable ePort VLAN Egress Filtering.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortFilteringEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortFilteringEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgEgrFltVlanPortFilteringEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortFilteringEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state of ePort VLAN Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - egress port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                    GT_FALSE - disable ePort VLAN Egress Filtering.
*                    GT_TRUE  - enable ePort VLAN Egress Filtering.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortFilteringEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_STATUS   rc;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChHwEgfShtFieldGet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_EGRESS_VLAN_FILTER_E,
            GT_FALSE, /*accessPhysicalPort*/
            &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortFilteringEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state of ePort VLAN Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - egress port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                    GT_FALSE - disable ePort VLAN Egress Filtering.
*                    GT_TRUE  - enable ePort VLAN Egress Filtering.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortFilteringEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortFilteringEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgEgrFltVlanPortFilteringEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortVidSelectModeSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filtering VID selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       vidSelectMode  - VID selection mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortVidSelectModeSet
(
    IN  GT_U8                                                devNum,
    IN  CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT  vidSelectMode
)
{
    GT_STATUS   rc;       /* return code      */
    GT_U32      data;     /* HW value         */
    GT_U32      regAddr;  /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->EGF_eft.egrFilterConfigs.egrFiltersGlobal;

    switch (vidSelectMode)
    {
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_TAG1_E:
            data = 0;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_EVLAN_E:
            data = 1;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ORIG_VLAN_E:
            data = 2;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 12, 2, data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortVidSelectModeSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filtering VID selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       vidSelectMode  - VID selection mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortVidSelectModeSet
(
    IN  GT_U8                                                devNum,
    IN  CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT  vidSelectMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortVidSelectModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vidSelectMode));

    rc = internal_cpssDxChBrgEgrFltVlanPortVidSelectModeSet(devNum, vidSelectMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vidSelectMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortVidSelectModeGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filtering VID selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       vidSelectModePtr  - (pointer to) VID selection mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on unexpected HW contents
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortVidSelectModeGet
(
    IN  GT_U8                                                devNum,
    OUT CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT  *vidSelectModePtr
)
{
    GT_STATUS   rc;       /* return code      */
    GT_U32      data;     /* HW value         */
    GT_U32      regAddr;  /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(vidSelectModePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->EGF_eft.egrFilterConfigs.egrFiltersGlobal;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 12, 2, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (data)
    {
        case 0:
            *vidSelectModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_TAG1_E;
            break;
        case 1:
            *vidSelectModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_EVLAN_E;
            break;
        case 2:
            *vidSelectModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ORIG_VLAN_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortVidSelectModeGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filtering VID selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       vidSelectModePtr  - (pointer to) VID selection mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on unexpected HW contents
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortVidSelectModeGet
(
    IN  GT_U8                                                devNum,
    OUT CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_VID_SELECT_MODE_ENT  *vidSelectModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortVidSelectModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vidSelectModePtr));

    rc = internal_cpssDxChBrgEgrFltVlanPortVidSelectModeGet(devNum, vidSelectModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vidSelectModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortVidMappingSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filtering VID Mapping Table entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vid           - source VLAN Id.
*       vidIndex      - target VLAN Id.
*                       (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - on out-of-range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortVidMappingSet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vid,
    IN  GT_U32           vidIndex
)
{
    GT_STATUS   rc;   /* return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vid);

    if (vidIndex >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChWriteTableEntry(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_MAPPER_E,
        vid, &vidIndex);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortVidMappingSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filtering VID Mapping Table entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vid           - source VLAN Id.
*       vidIndex      - target VLAN Id.
*                       (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - on out-of-range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortVidMappingSet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vid,
    IN  GT_U32           vidIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortVidMappingSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vid, vidIndex));

    rc = internal_cpssDxChBrgEgrFltVlanPortVidMappingSet(devNum, vid, vidIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vid, vidIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortVidMappingGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filtering VID Mapping Table entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       vid          - source VLAN Id.
*
* OUTPUTS:
*       vidIndexPtr  - (pointer to)target VLAN Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortVidMappingGet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vid,
    OUT GT_U32           *vidIndexPtr
)
{
    GT_STATUS   rc;   /* return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VLAN_INDEX_CHECK_MAC(devNum, vid);
    CPSS_NULL_PTR_CHECK_MAC(vidIndexPtr);

    rc = prvCpssDxChReadTableEntry(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_MAPPER_E,
        vid, vidIndexPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortVidMappingGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filtering VID Mapping Table entry.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       vid          - source VLAN Id.
*
* OUTPUTS:
*       vidIndexPtr  - (pointer to)target VLAN Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortVidMappingGet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vid,
    OUT GT_U32           *vidIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortVidMappingGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vid, vidIndexPtr));

    rc = internal_cpssDxChBrgEgrFltVlanPortVidMappingGet(devNum, vid, vidIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vid, vidIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortAccessModeSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filtering Table access mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       fltTabAccessMode  - Filtering Table access mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortAccessModeSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT fltTabAccessMode
)
{
    GT_STATUS   rc;       /* return code      */
    GT_U32      data;     /* HW value         */
    GT_U32      regAddr;  /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->EGF_sht.global.eportVlanEgrFiltering;

    switch (fltTabAccessMode)
    {
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_256_E:
            data = 0;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_512_E:
            data = 1;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_1K_E:
            data = 2;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_2K_E:
            data = 3;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_4K_E:
            data = 4;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_8K_E:
            data = 5;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_16K_E:
            data = 6;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_32K_E:
            data = 7;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 3, data);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.portVlanfltTabAccessMode = fltTabAccessMode;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortAccessModeSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filtering Table access mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       fltTabAccessMode  - Filtering Table access mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortAccessModeSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT fltTabAccessMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortAccessModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, fltTabAccessMode));

    rc = internal_cpssDxChBrgEgrFltVlanPortAccessModeSet(devNum, fltTabAccessMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, fltTabAccessMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortAccessModeGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filtering Table access mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       fltTabAccessModePtr  - (pointer to)
*                             Filtering Table access mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortAccessModeGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT *fltTabAccessModePtr
)
{
    GT_STATUS   rc;       /* return code      */
    GT_U32      data;     /* HW value         */
    GT_U32      regAddr;  /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(fltTabAccessModePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->EGF_sht.global.eportVlanEgrFiltering;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 3, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (data)
    {
        case 0:
            *fltTabAccessModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_256_E;
            break;
        case 1:
            *fltTabAccessModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_512_E;
            break;
        case 2:
            *fltTabAccessModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_1K_E;
            break;
        case 3:
            *fltTabAccessModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_2K_E;
            break;
        case 4:
            *fltTabAccessModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_4K_E;
            break;
        case 5:
            *fltTabAccessModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_8K_E;
            break;
        case 6:
            *fltTabAccessModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_16K_E;
            break;
        case 7:
            *fltTabAccessModePtr = CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_32K_E;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* should never occur */
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortAccessModeGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filtering Table access mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       fltTabAccessModePtr  - (pointer to)
*                             Filtering Table access mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortAccessModeGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_ENT *fltTabAccessModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortAccessModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, fltTabAccessModePtr));

    rc = internal_cpssDxChBrgEgrFltVlanPortAccessModeGet(devNum, fltTabAccessModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, fltTabAccessModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChBrgEgrFltVlanPortMemberTableIndexGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filter Table index.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vidIndex - VID index (after mapping)
*       portNum  - port number
*
* OUTPUTS:
*       indexPtr - (pointer to) index in the table
*       indexPortPtr - (pointer to) index in the entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - portNum is out of range based on the
*                                  egress evlan access mode
*       GT_FAIL                  - on not expected dlobal configuration
*
* COMMENTS:
*                  The table entry is 256 bits for 8 LSB of port.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChBrgEgrFltVlanPortMemberTableIndexGet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vidIndex,
    IN  GT_PORT_NUM      portNum,
    OUT GT_U32           *indexPtr,
    OUT GT_U32           *indexPortPtr
)
{
    GT_U32 portHighBits; /* amount of high bits of port in the index    */
    GT_U32 vidLowBits;   /* amount of low bits of vidIndex in the index */
    GT_U32  globalBitIndex;/* global bit index as if the table was single line */
    GT_U32 entryWidth;/* number of bit in entry */

    switch (PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.portVlanfltTabAccessMode)
    {
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_256_E:
            portHighBits = 0;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_512_E:
            portHighBits = 1;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_1K_E:
            portHighBits = 2;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_2K_E:
            portHighBits = 3;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_4K_E:
            portHighBits = 4;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_8K_E:
            portHighBits = 5;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_16K_E:
            portHighBits = 6;
            break;
        case CPSS_DXCH_BRG_EGR_FLT_VLAN_PORT_ACCESS_MODE_32K_E:
            portHighBits = 7;
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* not expected */
    }

    if (portNum >> (8+portHighBits))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    vidLowBits = 12 - portHighBits;

    globalBitIndex = /* 20 bits value */
            (U32_GET_FIELD_MAC(vidIndex,0,vidLowBits) << (8+portHighBits)) | /* the bits of the vidIndex */
            (U32_GET_FIELD_MAC(portNum,0,(portHighBits+8))) ;/* the bits of the port*/


    if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        entryWidth = 256;
    }
    else
    {
        entryWidth = 64;
    }

    *indexPtr    = globalBitIndex / entryWidth;
    *indexPortPtr = globalBitIndex % entryWidth;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortMemberSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filter Table ePort-VLAN membership.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vidIndex - VID index (after mapping)
*       portNum  - egress port number
*       isMember - GT_FALSE - ePort is not VLAN member, packet will be filtered.
*                  GT_TRUE  - ePort is VLAN member, packet will not be filtered.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - portNum is out of range based on the
*                                  egress evlan access mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortMemberSet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vidIndex,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          isMember
)
{
    GT_STATUS   rc;    /* return code */
    GT_U32      index; /* table index */
    GT_U32      fieldIndex; /* field index */
    GT_U32      data;  /*HW data      */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    rc = prvCpssDxChBrgEgrFltVlanPortMemberTableIndexGet(
        devNum, vidIndex, portNum, &index , &fieldIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    data = BOOL2BIT_MAC(isMember);

    rc = prvCpssDxChWriteTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EPORT_FILTER_E,
        index,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        fieldIndex /*offset*/, 1/*length*/, data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortMemberSet
*
* DESCRIPTION:
*       Set ePort VLAN Egress Filter Table ePort-VLAN membership.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vidIndex - VID index (after mapping)
*       portNum  - egress port number
*       isMember - GT_FALSE - ePort is not VLAN member, packet will be filtered.
*                  GT_TRUE  - ePort is VLAN member, packet will not be filtered.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - portNum is out of range based on the
*                                  egress evlan access mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortMemberSet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vidIndex,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          isMember
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortMemberSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vidIndex, portNum, isMember));

    rc = internal_cpssDxChBrgEgrFltVlanPortMemberSet(devNum, vidIndex, portNum, isMember);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vidIndex, portNum, isMember));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltVlanPortMemberGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filter Table ePort-VLAN membership.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vidIndex - VID index (after mapping)
*       portNum  - egress port number
*
* OUTPUTS:
*       isMemberPtr - (pointer to)
*                  GT_FALSE - ePort is not VLAN member, packet will be filtered.
*                  GT_TRUE  - ePort is VLAN member, packet will not be filtered.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - portNum is out of range based on the
*                                  egress evlan access mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltVlanPortMemberGet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vidIndex,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *isMemberPtr
)
{
    GT_STATUS   rc;    /* return code */
    GT_U32      index; /* table index */
    GT_U32      fieldIndex; /* field index */
    GT_U32      data;  /*HW data      */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(isMemberPtr);

    rc = prvCpssDxChBrgEgrFltVlanPortMemberTableIndexGet(
        devNum, vidIndex, portNum, &index ,&fieldIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChReadTableEntryField(
        devNum,
        PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EPORT_FILTER_E,
        index,
        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
        fieldIndex /*offset*/, 1/*length*/, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *isMemberPtr = BIT2BOOL_MAC(data);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgEgrFltVlanPortMemberGet
*
* DESCRIPTION:
*       Get ePort VLAN Egress Filter Table ePort-VLAN membership.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       vidIndex - VID index (after mapping)
*       portNum  - egress port number
*
* OUTPUTS:
*       isMemberPtr - (pointer to)
*                  GT_FALSE - ePort is not VLAN member, packet will be filtered.
*                  GT_TRUE  - ePort is VLAN member, packet will not be filtered.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - portNum is out of range based on the
*                                  egress evlan access mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltVlanPortMemberGet
(
    IN  GT_U8            devNum,
    IN  GT_U32           vidIndex,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *isMemberPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltVlanPortMemberGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, vidIndex, portNum, isMemberPtr));

    rc = internal_cpssDxChBrgEgrFltVlanPortMemberGet(devNum, vidIndex, portNum, isMemberPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, vidIndex, portNum, isMemberPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgEgrMeshIdConfigurationSet
*
* DESCRIPTION:
*       Set configuration of Mesh ID for the egress ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_FALSE: MESH ID filtering is not enabled
*                      GT_TRUE:  MESH ID filtering is enabled
*       meshIdOffset - The MESH ID assigned to a packet is conveyed in the SrcID
*                      assigned to the packet.
*                      This configuration specifies the location of the LSB of
*                      the MESH ID in the SrcID field assigned to a packet
*                      (APPLICABLE RANGES: 0..11)
*
*       meshIdSize   - Specifies the number of bits that are used for
*                      the MESH ID in the SrcID field.
*                      Together with <MESH ID offset>, the MESH ID assigned to
*                      a packet can be extracted:
*                      Packet's MESH ID = <SST ID> (<MESH ID size> - 1 +
*                      <MESH ID offset> : <MESH ID offset>)
*                      (APPLICABLE RANGES: 1..4)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_OUT_OF_RANGE          - on wrong meshIdOffset and meshIdSize
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrMeshIdConfigurationSet
(
    IN GT_U8                    devNum,
    IN GT_BOOL                  enable,
    IN GT_U32                   meshIdOffset,
    IN GT_U32                   meshIdSize
)
{
    GT_STATUS   rc;         /* return code      */
    GT_U32      regAddr;    /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if((meshIdOffset > 11) || (meshIdSize > 4))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    regAddr = PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(devNum).global.meshIdConfigs;

   rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 4, meshIdOffset);
    if(rc != GT_OK)
        return rc;

    if (enable==GT_FALSE)
    {
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 4, 3, 0);
    }
    else
    {
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 4, 3, meshIdSize);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrMeshIdConfigurationSet
*
* DESCRIPTION:
*       Set configuration of Mesh ID for the egress ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_FALSE: MESH ID filtering is not enabled
*                      GT_TRUE:  MESH ID filtering is enabled
*       meshIdOffset - The MESH ID assigned to a packet is conveyed in the SrcID
*                      assigned to the packet.
*                      This configuration specifies the location of the LSB of
*                      the MESH ID in the SrcID field assigned to a packet
*                      (APPLICABLE RANGES: 0..11)
*
*       meshIdSize   - Specifies the number of bits that are used for
*                      the MESH ID in the SrcID field.
*                      Together with <MESH ID offset>, the MESH ID assigned to
*                      a packet can be extracted:
*                      Packet's MESH ID = <SST ID> (<MESH ID size> - 1 +
*                      <MESH ID offset> : <MESH ID offset>)
*                      (APPLICABLE RANGES: 1..4)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_OUT_OF_RANGE          - on wrong meshIdOffset and meshIdSize
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrMeshIdConfigurationSet
(
    IN GT_U8                    devNum,
    IN GT_BOOL                  enable,
    IN GT_U32                   meshIdOffset,
    IN GT_U32                   meshIdSize
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrMeshIdConfigurationSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable, meshIdOffset, meshIdSize));

    rc = internal_cpssDxChBrgEgrMeshIdConfigurationSet(devNum, enable, meshIdOffset, meshIdSize);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable, meshIdOffset, meshIdSize));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrMeshIdConfigurationGet
*
* DESCRIPTION:
*       Get configuration of Mesh ID for the egress ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*
*       enablePtr       - (pointer to)
*                       GT_FALSE: MESH ID filtering is not enabled
*                       GT_TRUE:  MESH ID filtering is enabled
*       meshIdOffsetPtr - (pointer to) The MESH ID assigned to a packet is conveyed
*                       in the SrcID assigned to the packet.
*                       This configuration specifies the location of the LSB of
*                       the MESH ID in the SrcID field assigned to a packet
*                       (APPLICABLE RANGES: 0..11)
*
*       meshIdSizePtr   - (pointer to) Specifies the number of bits that are used
*                       for the MESH ID in the SrcID field.
*                       Together with <MESH ID offset>, the MESH ID assigned to
*                       a packet can be extracted:
*                       Packet's MESH ID = <SST ID> (<MESH ID size> - 1 +
*                       <MESH ID offset> : <MESH ID offset>)
*                       (APPLICABLE RANGES: 1..4)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrMeshIdConfigurationGet
(
    IN  GT_U8                    devNum,
    OUT GT_BOOL                  *enablePtr,
    OUT GT_U32                   *meshIdOffsetPtr,
    OUT GT_U32                   *meshIdSizePtr
)
{
    GT_STATUS rc;       /* return code      */
    GT_U32  regAddr;    /* register address */
    GT_U32  regData;    /* register data    */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(meshIdOffsetPtr);
    CPSS_NULL_PTR_CHECK_MAC(meshIdSizePtr);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(devNum).global.meshIdConfigs;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 8, &regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    *meshIdOffsetPtr = U32_GET_FIELD_MAC(regData,0,4);
    *meshIdSizePtr = U32_GET_FIELD_MAC(regData,4,3);
    if(*meshIdSizePtr==0)
    {
        *enablePtr = GT_FALSE;
    }
    else
    {
        *enablePtr = GT_TRUE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrMeshIdConfigurationGet
*
* DESCRIPTION:
*       Get configuration of Mesh ID for the egress ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*
*       enablePtr       - (pointer to)
*                       GT_FALSE: MESH ID filtering is not enabled
*                       GT_TRUE:  MESH ID filtering is enabled
*       meshIdOffsetPtr - (pointer to) The MESH ID assigned to a packet is conveyed
*                       in the SrcID assigned to the packet.
*                       This configuration specifies the location of the LSB of
*                       the MESH ID in the SrcID field assigned to a packet
*                       (APPLICABLE RANGES: 0..11)
*
*       meshIdSizePtr   - (pointer to) Specifies the number of bits that are used
*                       for the MESH ID in the SrcID field.
*                       Together with <MESH ID offset>, the MESH ID assigned to
*                       a packet can be extracted:
*                       Packet's MESH ID = <SST ID> (<MESH ID size> - 1 +
*                       <MESH ID offset> : <MESH ID offset>)
*                       (APPLICABLE RANGES: 1..4)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrMeshIdConfigurationGet
(
    IN  GT_U8                    devNum,
    OUT GT_BOOL                  *enablePtr,
    OUT GT_U32                   *meshIdOffsetPtr,
    OUT GT_U32                   *meshIdSizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrMeshIdConfigurationGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr, meshIdOffsetPtr, meshIdSizePtr));

    rc = internal_cpssDxChBrgEgrMeshIdConfigurationGet(devNum, enablePtr, meshIdOffsetPtr, meshIdSizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr, meshIdOffsetPtr, meshIdSizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChBrgEgrPortMeshIdSet
*
* DESCRIPTION:
*     Set the Mesh ID number for Source-based (Split Horizon)
*     Filtering per ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - egress port number
*       meshId   - The Egress Mesh ID number for Source-based (Split Horizon)
*                  Filtering per ePort. (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_OUT_OF_RANGE          - on wrong meshId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrPortMeshIdSet
(
    IN GT_U8           devNum,
    IN GT_PORT_NUM     portNum,
    IN GT_U32          meshId
)
{
    GT_STATUS  rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(meshId > 15)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChHwEgfShtFieldSet(devNum,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_MESH_ID_E,
                GT_TRUE, /*accessPhysicalPort*/
                GT_TRUE, /*accessEPort*/
                meshId);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrPortMeshIdSet
*
* DESCRIPTION:
*     Set the Mesh ID number for Source-based (Split Horizon)
*     Filtering per ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - egress port number
*       meshId   - The Egress Mesh ID number for Source-based (Split Horizon)
*                  Filtering per ePort. (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_OUT_OF_RANGE          - on wrong meshId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrPortMeshIdSet
(
    IN GT_U8           devNum,
    IN GT_PORT_NUM     portNum,
    IN GT_U32          meshId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrPortMeshIdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, meshId));

    rc = internal_cpssDxChBrgEgrPortMeshIdSet(devNum, portNum, meshId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, meshId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrPortMeshIdGet
*
* DESCRIPTION:
*     Get the Mesh ID number for Source-based (Split Horizon)
*     Filtering per ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - egress port number
*
* OUTPUTS:
*       meshIdPtr- (pointer to) The Egress Mesh ID number for Source-based
*                  (Split Horizon) Filtering per ePort.(APPLICABLE RANGES: 0..15)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrPortMeshIdGet
(
    IN GT_U8           devNum,
    IN GT_PORT_NUM     portNum,
    OUT GT_U32         *meshIdPtr
)
{
    GT_U32  rc;
    GT_U32  data;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(meshIdPtr);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChHwEgfShtFieldGet(devNum,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_MESH_ID_E,
            GT_FALSE, /*accessPhysicalPort*/
            &data);

    if(rc!=GT_OK)
        return rc;

    *meshIdPtr = data;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrPortMeshIdGet
*
* DESCRIPTION:
*     Get the Mesh ID number for Source-based (Split Horizon)
*     Filtering per ePort.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - egress port number
*
* OUTPUTS:
*       meshIdPtr- (pointer to) The Egress Mesh ID number for Source-based
*                  (Split Horizon) Filtering per ePort.(APPLICABLE RANGES: 0..15)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrPortMeshIdGet
(
    IN GT_U8           devNum,
    IN GT_PORT_NUM     portNum,
    OUT GT_U32         *meshIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrPortMeshIdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, meshIdPtr));

    rc = internal_cpssDxChBrgEgrPortMeshIdGet(devNum, portNum, meshIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, meshIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltPortVid1FilteringEnableSet
*
* DESCRIPTION:
*       Enable/Disable port VID1 Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       enable  - GT_FALSE - disable filtering
*                 GT_TRUE  - enable filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltPortVid1FilteringEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
        CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChHwEgfShtFieldSet(devNum,portNum,
                   PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_DROP_ON_EPORT_VID1_MISMATCH_E,
                   GT_TRUE, /*accessPhysicalPort*/
                   GT_TRUE, /*accessEPort*/
                   BOOL2BIT_MAC(enable));
    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrFltPortVid1FilteringEnableSet
*
* DESCRIPTION:
*       Enable/Disable port VID1 Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       enable  - GT_FALSE - disable filtering
*                 GT_TRUE  - enable filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortVid1FilteringEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltPortVid1FilteringEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgEgrFltPortVid1FilteringEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChBrgEgrFltPortVid1FilteringEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state of port VID1 Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - egress port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                    GT_FALSE - the filtering is enabled
*                    GT_TRUE  - the filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltPortVid1FilteringEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
)
{
    GT_U32  rc;
    GT_U32  data;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChHwEgfShtFieldGet(devNum,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_DROP_ON_EPORT_VID1_MISMATCH_E,
            GT_FALSE, /*accessPhysicalPort*/
            &data);

    if(rc != GT_OK)
        return rc;

    *enablePtr = data;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrFltPortVid1FilteringEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state of port VID1 Egress Filtering.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - egress port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                    GT_FALSE - the filtering is enabled
*                    GT_TRUE  - the filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortVid1FilteringEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltPortVid1FilteringEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgEgrFltPortVid1FilteringEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltPortVid1Set
*
* DESCRIPTION:
*       Set port associated VID1. Relevant to egress VID1 Filtering per port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       vid1    - VID1 associated with port (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on vid1 value out of an applicable range
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltPortVid1Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U16           vid1
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
        CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChHwEgfShtFieldSet(devNum,portNum,
                   PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_EPORT_ASSOCIATED_VID1_E,
                   GT_TRUE, /*accessPhysicalPort*/
                   GT_TRUE, /*accessEPort*/
                   vid1);
    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrFltPortVid1Set
*
* DESCRIPTION:
*       Set port associated VID1. Relevant to egress VID1 Filtering per port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*       vid1    - VID1 associated with port (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on vid1 value out of an applicable range
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortVid1Set
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_U16           vid1
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltPortVid1Set);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, vid1));

    rc = internal_cpssDxChBrgEgrFltPortVid1Set(devNum, portNum, vid1);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, vid1));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltPortVid1Get
*
* DESCRIPTION:
*       Get port associated VID1. Relevant to egress VID1 Filtering per port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*
* OUTPUTS:
*       vid1Ptr    - (pointer to) VID1 associated with port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL vid1Ptr
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltPortVid1Get
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_U16           *vid1Ptr
)
{
    GT_U32  rc;
    GT_U32  data;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(vid1Ptr);

    if (!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    rc = prvCpssDxChHwEgfShtFieldGet(devNum,portNum,
            PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_EPORT_ASSOCIATED_VID1_E,
            GT_FALSE, /*accessPhysicalPort*/
            &data);

    if(rc != GT_OK)
        return rc;

    *vid1Ptr = (GT_U16)data;

    return rc;

}

/*******************************************************************************
* cpssDxChBrgEgrFltPortVid1Get
*
* DESCRIPTION:
*       Get port associated VID1. Relevant to egress VID1 Filtering per port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - egress port number
*
* OUTPUTS:
*       vid1Ptr    - (pointer to) VID1 associated with port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL vid1Ptr
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortVid1Get
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_U16           *vid1Ptr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltPortVid1Get);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, vid1Ptr));

    rc = internal_cpssDxChBrgEgrFltPortVid1Get(devNum, portNum, vid1Ptr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, vid1Ptr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltPortLinkEnableSet
*
* DESCRIPTION:
*       Enable/disable state of 'Link filter' on specified port on specified device.
*       set the port as 'force link up' (no filtering) , or as 'force link down' (filtering)
*
* APPLICABLE DEVICES:
*        Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       enable   - GT_TRUE  for     filtering the port (considered as 'link down')
*                       packets are filtered and are not forwarded to this port.
*                  GT_FALSE for not filtering the port (considered as 'link up')
*                       packets are not filtered and may be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltPortLinkEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32  value;      /* data to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E |
          CPSS_BOBCAT2_E | CPSS_CAELUM_E);

    value = (enable == GT_TRUE) ? 0/*force link down*/ : 1/*force link up*/;

    rc = prvCpssDxChHwEgfEftFieldSet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_PHYSICAL_PORT_LINK_STATUS_MASK_E,
        value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgEgrFltPortLinkEnableSet
*
* DESCRIPTION:
*       Enable/disable egress Filtering for 'Link state' on specified port on specified device.
*       set the port as 'force link up' (no filtering) , or as 'force link down' (filtering)
*
* APPLICABLE DEVICES:
*        Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       enable   - GT_TRUE  for     filtering the port (considered as 'link down')
*                       packets are filtered and are not forwarded to this port.
*                  GT_FALSE for not filtering the port (considered as 'link up')
*                       packets are not filtered and may be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortLinkEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltPortLinkEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgEgrFltPortLinkEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgEgrFltPortLinkEnableGet
*
* DESCRIPTION:
*       This function gets the egress Filtering current state (enable/disable)
*       for 'Link state' on specified port on specified device.
*       is the port as 'force link up' (no filtering) , or as 'force link down' (filtering) ?
*
* APPLICABLE DEVICES:
*        Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr- (pointer to) the filter state :
*                  GT_TRUE  for     filtering the port (considered as 'link down')
*                       packets are filtered and are not forwarded to this port.
*                  GT_FALSE for not filtering the port (considered as 'link up')
*                       packets are not filtered and may be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgEgrFltPortLinkEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL          *enablePtr
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32  value;      /* data to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E |
          CPSS_BOBCAT2_E | CPSS_CAELUM_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    rc = prvCpssDxChHwEgfEftFieldGet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_PHYSICAL_PORT_LINK_STATUS_MASK_E,
        &value);

    if (rc != GT_OK)
    {
        return rc;
    }

    if(value == 0)
    {
        *enablePtr = GT_TRUE;
    }
    else
    {
        *enablePtr = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgEgrFltPortLinkEnableGet
*
* DESCRIPTION:
*       This function gets the egress Filtering current state (enable/disable)
*       for 'Link state' on specified port on specified device.
*       is the port as 'force link up' (no filtering) , or as 'force link down' (filtering) ?
*
* APPLICABLE DEVICES:
*        Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr- (pointer to) the filter state :
*                  GT_TRUE  for     filtering the port (considered as 'link down')
*                       packets are filtered and are not forwarded to this port.
*                  GT_FALSE for not filtering the port (considered as 'link up')
*                       packets are not filtered and may be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgEgrFltPortLinkEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgEgrFltPortLinkEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgEgrFltPortLinkEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

