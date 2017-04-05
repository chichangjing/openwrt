/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgNestVlan.c
*
* DESCRIPTION:
*       CPSS DxCh Nested VLANs facility implementation.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgNestVlan.h>

/*******************************************************************************
* internal_cpssDxChBrgNestVlanAccessPortSet
*
* DESCRIPTION:
*       Configure given port as Nested VLAN access port.
*       The VID of all the packets received on Nested VLAN access port is
*       discarded and they are assigned with the Port VID that set by
*       cpssDxChBrgVlanPortVidSet(). This VLAN assignment may be subsequently
*       overridden by the protocol based VLANs or policy based VLANs algorithms.
*       When a packet received on an access port is transmitted via a core port
*       or a cascading port, a VLAN tag is added to the packet, in addition to
*       any existing VLAN tag.
*       The 802.1p User Priority field of this added tag may be one of the
*       following, based on the ModifyUP QoS parameter set to the packet at
*       the end of the Ingress pipe:
*              - If ModifyUP is "Modify", it is the UP extracted from the
*              QoS Profile table entry that configured by the
*              cpssDxChCosProfileEntrySet().
*              - If ModifyUP is "Not Modify", it is the original packet
*              802.1p User Priority field, if it is tagged. If untagged,
*              it is ingress port's default user priority that configured by
*              cpssDxChPortDefaultUPSet().
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - PP's device number.
*       portNum - port number or CPU port number.
*                 In eArch devices portNum is default ePort.
*       enable  -  GT_TRUE  - port is Nested VLAN Access Port.
*                  GT_FALSE - port is usual port (Core or Customer).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To complete Access Port Configuration do the following:
*       For DxCh1, DxCh2 and DxCh3 devices:
*           -  by API cpssDxChBrgNestVlanEtherTypeSelectSet(), set
*              the Port VLAN Selection to the CPSS_VLAN_ETHERTYPE0_E;
*           -  update egress Port VLAN EtherType;
*           -  by API cpssDxChBrgVlanForcePvidEnable(), set
*              Port VID Assignment mode.
*           To complete Core Port Configuration, select CPSS_VLAN_ETHERTYPE1_E
*           for Usual Port.
*           To complete Customers Bridges(bridges that don't employ Nested VLANs)
*           Configuration, select CPSS_VLAN_ETHERTYPE0_E for Usual Port.
*       For xCat and above devices:
*           See CPSS user guide how to configure Nested VLAN or TR101 features.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgNestVlanAccessPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_BOOL         enable
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      hwData;      /* data to write to HW */
    GT_U32      fieldOffset; /* offset of the field in the VLAN QoS Table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* Convert boolean value to the HW format */
    hwData = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_NESTED_VLAN_ACCESS_PORT_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            fieldOffset = 28;
        }
        else
        {
            fieldOffset = 24;
        }

        /* Update port mode in the VLAN and QoS Configuration Entry */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                               (GT_U32)portNum,
                                               0,       /* start at word 0 */
                                               fieldOffset, /* start at bit 24 or 28 */
                                               1,       /* 1 bit */
                                               hwData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgNestVlanAccessPortSet
*
* DESCRIPTION:
*       Configure given port as Nested VLAN access port.
*       The VID of all the packets received on Nested VLAN access port is
*       discarded and they are assigned with the Port VID that set by
*       cpssDxChBrgVlanPortVidSet(). This VLAN assignment may be subsequently
*       overridden by the protocol based VLANs or policy based VLANs algorithms.
*       When a packet received on an access port is transmitted via a core port
*       or a cascading port, a VLAN tag is added to the packet, in addition to
*       any existing VLAN tag.
*       The 802.1p User Priority field of this added tag may be one of the
*       following, based on the ModifyUP QoS parameter set to the packet at
*       the end of the Ingress pipe:
*              - If ModifyUP is "Modify", it is the UP extracted from the
*              QoS Profile table entry that configured by the
*              cpssDxChCosProfileEntrySet().
*              - If ModifyUP is "Not Modify", it is the original packet
*              802.1p User Priority field, if it is tagged. If untagged,
*              it is ingress port's default user priority that configured by
*              cpssDxChPortDefaultUPSet().
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - PP's device number.
*       portNum - port number or CPU port number.
*                 In eArch devices portNum is default ePort.
*       enable  -  GT_TRUE  - port is Nested VLAN Access Port.
*                  GT_FALSE - port is usual port (Core or Customer).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To complete Access Port Configuration do the following:
*       For DxCh1, DxCh2 and DxCh3 devices:
*           -  by API cpssDxChBrgNestVlanEtherTypeSelectSet(), set
*              the Port VLAN Selection to the CPSS_VLAN_ETHERTYPE0_E;
*           -  update egress Port VLAN EtherType;
*           -  by API cpssDxChBrgVlanForcePvidEnable(), set
*              Port VID Assignment mode.
*           To complete Core Port Configuration, select CPSS_VLAN_ETHERTYPE1_E
*           for Usual Port.
*           To complete Customers Bridges(bridges that don't employ Nested VLANs)
*           Configuration, select CPSS_VLAN_ETHERTYPE0_E for Usual Port.
*       For xCat and above devices:
*           See CPSS user guide how to configure Nested VLAN or TR101 features.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgNestVlanAccessPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_BOOL         enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgNestVlanAccessPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChBrgNestVlanAccessPortSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgNestVlanAccessPortGet
*
* DESCRIPTION:
*       Gets configuration of the given port (Nested VLAN access port
*       or Core/Customer port).
*       The VID of all the packets received on Nested VLAN access port is
*       discarded and they are assigned with the Port VID that set by
*       cpssDxChBrgVlanPortVidSet(). This VLAN assignment may be subsequently
*       overridden by the protocol based VLANs or policy based VLANs algorithms.
*       When a packet received on an access port is transmitted via a core port
*       or a cascading port, a VLAN tag is added to the packet, in addition to
*       any existing VLAN tag.
*       The 802.1p User Priority field of this added tag may be one of the
*       following, based on the ModifyUP QoS parameter set to the packet at
*       the end of the Ingress pipe:
*              - If ModifyUP is "Modify", it is the UP extracted from the
*              QoS Profile table entry that configured by the
*              cpssDxChCosProfileEntrySet().
*              - If ModifyUP is "Not Modify", it is the original packet
*              802.1p User Priority field, if it is tagged. If untagged,
*              it is ingress port's default user priority that configured by
*              cpssDxChPortDefaultUPSet().
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - PP's device number.
*       portNum     - physical or CPU port number.
*                     In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr   - Pointer to the Boolean value:
*                     GT_TRUE   - port is Nested VLAN Access Port.
*                     GT_FALSE  - port is usual port:
*                                      - Core Port in case of
*                                         CPSS_VLAN_ETHERTYPE1_E selected;
*                                      - Customers Bridges (bridges that don't
*                                         employ Nested VLANs) in case of
*                                         CPSS_VLAN_ETHERTYPE0_E selected;
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum values.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgNestVlanAccessPortGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      hwData;             /* data to write to HW */
    GT_STATUS   retStatus = GT_OK;  /* generic return status code */
    GT_U32      fieldOffset;        /* offset of the field in the VLAN QoS Table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read the value from pre-tti-lookup-ingress-eport table  */
        retStatus = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_NESTED_VLAN_ACCESS_PORT_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &hwData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            fieldOffset = 28;
        }
        else
        {
            fieldOffset = 24;
        }
        /* Read port mode from VLAN and QoS Configuration Entry */
        retStatus = prvCpssDxChReadTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             (GT_U32)portNum,
                                             0,        /* start at word 0 */
                                             fieldOffset, /* start at bit 24 or 28 */
                                             1,        /* 1 bit */
                                             &hwData);
    }
    if(GT_OK == retStatus)
    {
        /* Convert gathered HW value to the SW format */
        *enablePtr = (hwData == 1) ? GT_TRUE : GT_FALSE;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChBrgNestVlanAccessPortGet
*
* DESCRIPTION:
*       Gets configuration of the given port (Nested VLAN access port
*       or Core/Customer port).
*       The VID of all the packets received on Nested VLAN access port is
*       discarded and they are assigned with the Port VID that set by
*       cpssDxChBrgVlanPortVidSet(). This VLAN assignment may be subsequently
*       overridden by the protocol based VLANs or policy based VLANs algorithms.
*       When a packet received on an access port is transmitted via a core port
*       or a cascading port, a VLAN tag is added to the packet, in addition to
*       any existing VLAN tag.
*       The 802.1p User Priority field of this added tag may be one of the
*       following, based on the ModifyUP QoS parameter set to the packet at
*       the end of the Ingress pipe:
*              - If ModifyUP is "Modify", it is the UP extracted from the
*              QoS Profile table entry that configured by the
*              cpssDxChCosProfileEntrySet().
*              - If ModifyUP is "Not Modify", it is the original packet
*              802.1p User Priority field, if it is tagged. If untagged,
*              it is ingress port's default user priority that configured by
*              cpssDxChPortDefaultUPSet().
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - PP's device number.
*       portNum     - physical or CPU port number.
*                     In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr   - Pointer to the Boolean value:
*                     GT_TRUE   - port is Nested VLAN Access Port.
*                     GT_FALSE  - port is usual port:
*                                      - Core Port in case of
*                                         CPSS_VLAN_ETHERTYPE1_E selected;
*                                      - Customers Bridges (bridges that don't
*                                         employ Nested VLANs) in case of
*                                         CPSS_VLAN_ETHERTYPE0_E selected;
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum values.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgNestVlanAccessPortGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgNestVlanAccessPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChBrgNestVlanAccessPortGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgNestVlanEtherTypeSelectSet
*
* DESCRIPTION:
*       Sets selected port VLAN EtherType.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - PP's device number.
*       portNum         - physical or CPU port number.
*       ingressVlanSel  - selected port VLAN EtherType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum or ingressVlanSel values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For the VLAN EtherType coherency between Ingress and Egress,
*       the VLAN EtherType shall be configured to match the ingressVlanSel.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgNestVlanEtherTypeSelectSet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    IN  CPSS_ETHER_MODE_ENT     ingressVlanSel
)
{
    GT_U32                          hwData;         /* data to write to HW */
    GT_STATUS                       retStatus;      /* gen return status code */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC  *regsAddrPtr;   /* pointer to reg address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* Get EhterType HW format */
    switch (ingressVlanSel)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            hwData = 0;
            break;
        case CPSS_VLAN_ETHERTYPE1_E:
            hwData = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Set selected VLAN EtherType in the VLAN and QoS Configuration Entry */
    retStatus = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         (GT_U32)portNum,
                                         0,         /* start at word 0 */
                                         22,        /* start at bit 22 */
                                         1,         /* 1 bit */
                                         hwData);

    /********************************************************/
    /* For the VLAN EtherType coherency between Ingress and */
    /* Egress, update Egress VLAN EtherType Select Register */
    /********************************************************/

    if (GT_OK == retStatus)
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* Update portNum in case of CPU Port */
        if (localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                localPort = 27;
            }
            else
            {
                localPort = 31;
            }
        }

        regsAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
        retStatus = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                               regsAddrPtr->bridgeRegs.egressVlanEtherTypeSel,
                               (GT_U32)localPort, /* start at portNum bit */
                               1,               /* 1 bit */
                               hwData);
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChBrgNestVlanEtherTypeSelectSet
*
* DESCRIPTION:
*       Sets selected port VLAN EtherType.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - PP's device number.
*       portNum         - physical or CPU port number.
*       ingressVlanSel  - selected port VLAN EtherType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum or ingressVlanSel values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For the VLAN EtherType coherency between Ingress and Egress,
*       the VLAN EtherType shall be configured to match the ingressVlanSel.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgNestVlanEtherTypeSelectSet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    IN  CPSS_ETHER_MODE_ENT     ingressVlanSel
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgNestVlanEtherTypeSelectSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ingressVlanSel));

    rc = internal_cpssDxChBrgNestVlanEtherTypeSelectSet(devNum, portNum, ingressVlanSel);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ingressVlanSel));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgNestVlanEtherTypeSelectGet
*
* DESCRIPTION:
*       Gets selected port VLAN EtherType.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - PP's device number.
*       portNum             - physical or CPU port number.
*
* OUTPUTS:
*       ingressVlanSelPtr   - pointer to selected port VLAN EtherType.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum values.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChBrgNestVlanEtherTypeSelectGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT CPSS_ETHER_MODE_ENT     *ingressVlanSelPtr
)
{
    GT_U32      hwData;     /* data to write to HW */
    GT_STATUS   retStatus;  /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(ingressVlanSelPtr);

    /* Read selected VLAN EtherType from the VLAN and QoS Configuration Entry */
    retStatus = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        (GT_U32)portNum,
                                        0, /* start at word 0 */
                                        22,/* start at bit 24 */
                                        1, /* 1 bit */
                                        &hwData);
    if(GT_OK == retStatus)
    {
        /* Convert gathered HW value to the SW format */
        *ingressVlanSelPtr = (hwData == 1) ?
            CPSS_VLAN_ETHERTYPE1_E : CPSS_VLAN_ETHERTYPE0_E;
    }

    return retStatus;
}

/*******************************************************************************
* cpssDxChBrgNestVlanEtherTypeSelectGet
*
* DESCRIPTION:
*       Gets selected port VLAN EtherType.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - PP's device number.
*       portNum             - physical or CPU port number.
*
* OUTPUTS:
*       ingressVlanSelPtr   - pointer to selected port VLAN EtherType.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum values.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgNestVlanEtherTypeSelectGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT CPSS_ETHER_MODE_ENT     *ingressVlanSelPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgNestVlanEtherTypeSelectGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, ingressVlanSelPtr));

    rc = internal_cpssDxChBrgNestVlanEtherTypeSelectGet(devNum, portNum, ingressVlanSelPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, ingressVlanSelPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
