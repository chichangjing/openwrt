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
* cpssDxChVnt.c
*
* DESCRIPTION:
*     CPSS DXCH Virtual Network Tester (VNT) Technology facility implementation.
*     VNT features:
*     - Operation, Administration, and Maintenance (OAM).
*     - Connectivity Fault Management (CFM).
*
* FILE REVISION NUMBER:
*       $Revision: 33$
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>
#include <cpss/dxCh/dxChxGen/vnt/private/prvCpssDxChVntLog.h>

/*******************************************************************************
* internal_cpssDxChVntOamPortLoopBackModeEnableSet
*
* DESCRIPTION:
*      Enable/Disable 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*       enable   - GT_TRUE:  Enable OAM loopback mode.
*                  GT_FALSE: Disable OAM loopback mode.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*      For xCat and above not need rule for OAM PDU trap.
*      Use cpssDxChVntOamPortPduTrapEnableSet for it.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntOamPortLoopBackModeEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    fieldOffset;       /* The start bit number in the register */
    GT_U32    data;              /* reg sub field data */
    GT_STATUS rc;                /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    data = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to TTI-Physical-Port-Attribute table */
         rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_OAM_LINK_LAYER_LOOPBACK_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            data);
        if(rc !=GT_OK)
        {
            return rc;
        }

        if(portNum < PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum))
        {
            /* set the EGF_EFT  */
            rc = prvCpssDxChHwEgfEftFieldSet(devNum,portNum,
                PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_OAM_LOOPBACK_FILTER_ENABLE_E,
                data);
        }

        return rc;
    }

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if(CPSS_CPU_PORT_NUM_CNS == portNum)
        {
            fieldOffset = 31;
        }
        else
        {
            fieldOffset = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        }
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                           bridgeRegs.vntReg.vntOamLoopbackConfReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.oamLoopbackFilterEn[OFFSET_TO_WORD_MAC(portNum)];

        fieldOffset = OFFSET_TO_BIT_MAC(portNum);
    }

    /* For xCat and above devices enable/disable OAM Link Layer Loopback */
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* write to port-vlan-qos table  */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,/* port */
                                             1,/* start at word 1 */
                                             30,/* start at bit 0*/
                                             1, /* 1 bit */
                                             data);
        if(rc !=GT_OK)
        {
            return rc;
        }
    }

    /* Enable/Disable Loopback mode */
    return prvCpssHwPpPortGroupSetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, fieldOffset, 1, data);
}

/*******************************************************************************
* cpssDxChVntOamPortLoopBackModeEnableSet
*
* DESCRIPTION:
*      Enable/Disable 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*       enable   - GT_TRUE:  Enable OAM loopback mode.
*                  GT_FALSE: Disable OAM loopback mode.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*      For xCat and above not need rule for OAM PDU trap.
*      Use cpssDxChVntOamPortPduTrapEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortLoopBackModeEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntOamPortLoopBackModeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChVntOamPortLoopBackModeEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntOamPortLoopBackModeEnableGet
*
* DESCRIPTION:
*      Get 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable OAM loopback mode.
*                     GT_FALSE: Disable OAM loopback mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*      For xCat and above not need rule for OAM PDU trap.
*      Use cpssDxChVntOamPortPduTrapEnableSet for it.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntOamPortLoopBackModeEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_U32      regAddr;      /* register address             */
    GT_U32      fieldOffset;  /* The start bit number in the register */
    GT_STATUS   rc;           /* return status                */
    GT_U32      hwValue;      /* value to read from register  */
    GT_U32      hwData;       /* the data read from the table */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read from TTI-Physical-Port-Attribute table */
         rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_OAM_LINK_LAYER_LOOPBACK_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &hwData);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(portNum < PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum))
        {
            /* get the EGF_EFT  */
            rc = prvCpssDxChHwEgfEftFieldGet(devNum,portNum,
                PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_OAM_LOOPBACK_FILTER_ENABLE_E,
                &hwValue);
            /* TTI and EGF_EFT need to be enabled */
            hwValue &= hwData;
        }
        else
        {
            hwValue = hwData;
        }


        *enablePtr = BIT2BOOL_MAC(hwValue);

        return rc;
    }

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if(CPSS_CPU_PORT_NUM_CNS == portNum)
        {
            fieldOffset = 31;
        }
        else
        {
            fieldOffset = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                         bridgeRegs.vntReg.vntOamLoopbackConfReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.oamLoopbackFilterEn[OFFSET_TO_WORD_MAC(portNum)];

        fieldOffset = OFFSET_TO_BIT_MAC(portNum);
    }

    rc = prvCpssHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, fieldOffset, 1, &hwValue);

    if(rc != GT_OK)
    {
        return rc;
    }


    /* For xCat and above devices get OAM Link Layer Loopback mode */
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* read from port-vlan-qos table  */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,/* port */
                                             1,/* start at word 1 */
                                             30,/* start at bit 0*/
                                             1, /* 1 bit */
                                             &hwData);
        if(rc !=GT_OK)
        {
            return rc;
        }

        /* To enable OAM Loopback mode for xCat and above devices,
           OAM Link Layer Loopback and OAM Loopback should be enabled */

        hwValue &= hwData;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;

}

/*******************************************************************************
* cpssDxChVntOamPortLoopBackModeEnableGet
*
* DESCRIPTION:
*      Get 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable OAM loopback mode.
*                     GT_FALSE: Disable OAM loopback mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*      For xCat and above not need rule for OAM PDU trap.
*      Use cpssDxChVntOamPortPduTrapEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortLoopBackModeEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntOamPortLoopBackModeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChVntOamPortLoopBackModeEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntOamPortUnidirectionalEnableSet
*
* DESCRIPTION:
*      Enable/Disable the port for unidirectional transmit.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:   Enable the port for unidirectional transmit.
*                  GT_FALSE:  Disable the port for unidirectional transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntOamPortUnidirectionalEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM    port,
    IN GT_BOOL  enable
)
{
    GT_STATUS               rc;             /* return status                */
    GT_U32                  regAddr;        /* register address             */
    GT_U32                  value;           /* register field data          */
    GT_U32                  portMac;        /* port MAC number              */
    PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   regDataArray[PRV_CPSS_PORT_NOT_APPLICABLE_E];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);
    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,port,portMac);

    value = BOOL2BIT_MAC(enable);

    /* Enable/Disable the port for unidirectional transmit */
    if((CPSS_CPU_PORT_NUM_CNS == port) &&
        (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMac,PRV_CPSS_PORT_GE_E,&regAddr);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portMac,&regAddr);
    }

    if(prvCpssDxChPortMacConfigurationClear(regDataArray) != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_INIT_ERROR, LOG_ERROR_NO_MSG);

    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_GE_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_GE_E].fieldOffset = 0;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMac,PRV_CPSS_PORT_XG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XG_E].fieldOffset = 8;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMac,PRV_CPSS_PORT_XLG_E,&regAddr);
    if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        regDataArray[PRV_CPSS_PORT_XLG_E].regAddr = regAddr;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldData = value;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldLength = 1;
        regDataArray[PRV_CPSS_PORT_XLG_E].fieldOffset = 8;
    }

    rc = prvCpssDxChPortMacConfiguration(devNum, port, regDataArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    if (GT_TRUE == PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.enabled)
    {
        if(enable)
        {
            rc = prvCpssDxChPortEnableSet(devNum,port,GT_FALSE);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssDxChPortPcsLoopbackEnableSet(devNum,port,GT_TRUE);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            enable = CPSS_PORTS_BMP_IS_PORT_SET_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                        info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.pcsLoopbackBmpPtr,portMac);

            rc = prvCpssDxChPortPcsLoopbackEnableSet(devNum,port,BIT2BOOL_MAC(enable));
            if (rc != GT_OK)
            {
                return rc;
            }

            enable = CPSS_PORTS_BMP_IS_PORT_SET_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                        info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E.portEnableBmpPtr,portMac);

            rc = prvCpssDxChPortEnableSet(devNum,port,BIT2BOOL_MAC(enable));
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVntOamPortUnidirectionalEnableSet
*
* DESCRIPTION:
*      Enable/Disable the port for unidirectional transmit.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:   Enable the port for unidirectional transmit.
*                  GT_FALSE:  Disable the port for unidirectional transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortUnidirectionalEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM    port,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntOamPortUnidirectionalEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, enable));

    rc = internal_cpssDxChVntOamPortUnidirectionalEnableSet(devNum, port, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntOamPortUnidirectionalEnableGet
*
* DESCRIPTION:
*      Gets the current status of unidirectional transmit for the port.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       port        - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to current status of unidirectional transmit:
*                     GT_TRUE:   Enable the port for unidirectional transmit.
*                     GT_FALSE:  Disable the port for unidirectional transmit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - if feature not supported by port
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntOamPortUnidirectionalEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    port,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS               rc = GT_OK;     /* return status                */
    GT_U32                  regAddr;        /* register address             */
    GT_U32                  offset;         /* offset in register           */
    GT_U32                  hwValue;        /* value to read from register  */
    PRV_CPSS_PORT_TYPE_ENT  portMacType;    /* port MAC type                */
    GT_U32                  portMac;        /* port MAC number              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,port,portMac);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMac);

    /* Get current status of unidirectional transmit */

    /* 10 Gb ports */
    if(portMacType >= PRV_CPSS_PORT_XG_E ||
           ((CPSS_CPU_PORT_NUM_CNS == port) &&
            (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily)))
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portMac,portMacType,&regAddr);
        offset=8;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portMac,&regAddr);
        offset=0;
    }

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    rc = prvCpssHwPpPortGroupGetRegField(devNum,PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMac),
                                         regAddr,
                                         offset,
                                         1,
                                         &hwValue);

    if(GT_OK != rc)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVntOamPortUnidirectionalEnableGet
*
* DESCRIPTION:
*      Gets the current status of unidirectional transmit for the port.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       port        - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to current status of unidirectional transmit:
*                     GT_TRUE:   Enable the port for unidirectional transmit.
*                     GT_FALSE:  Disable the port for unidirectional transmit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - if feature not supported by port
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortUnidirectionalEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM    port,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntOamPortUnidirectionalEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, port, enablePtr));

    rc = internal_cpssDxChVntOamPortUnidirectionalEnableGet(devNum, port, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, port, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntCfmEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CFM (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       etherType   - CFM (Connectivity Fault Management) EtherType,
*                     (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum.
*       GT_OUT_OF_RANGE          - on wrong etherType values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntCfmEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    if(etherType > 0xFFFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).CFMEtherType;
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).CFMEtherType;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vntReg.vntCfmEtherTypeConfReg;
    }

    /* Set CFM EtherType */
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 16, etherType);

}

/*******************************************************************************
* cpssDxChVntCfmEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CFM (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       etherType   - CFM (Connectivity Fault Management) EtherType,
*                     (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum.
*       GT_OUT_OF_RANGE          - on wrong etherType values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmEtherTypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntCfmEtherTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherType));

    rc = internal_cpssDxChVntCfmEtherTypeSet(devNum, etherType);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherType));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntCfmEtherTypeGet
*
* DESCRIPTION:
*       Gets the current EtherType to identify CFM
*       (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       etherTypePtr - pointer to CFM (Connectivity Fault Management) EtherType.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntCfmEtherTypeGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *etherTypePtr
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(etherTypePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).CFMEtherType;
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).CFMEtherType;
        }
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vntReg.vntCfmEtherTypeConfReg;
    }

    /* Get CFM EtherType */
    return prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, etherTypePtr);

}

/*******************************************************************************
* cpssDxChVntCfmEtherTypeGet
*
* DESCRIPTION:
*       Gets the current EtherType to identify CFM
*       (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       etherTypePtr - pointer to CFM (Connectivity Fault Management) EtherType.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmEtherTypeGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *etherTypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntCfmEtherTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherTypePtr));

    rc = internal_cpssDxChVntCfmEtherTypeGet(devNum, etherTypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherTypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntCfmLbrOpcodeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       opcode      - CFM LBR Opcode (APPLICABLE RANGES: 0..0xFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, opcode.
*       GT_OUT_OF_RANGE          - on wrong opcode values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntCfmLbrOpcodeSet
(
    IN GT_U8    devNum,
    IN GT_U32   opcode
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    startBit;          /* start bit of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    if(opcode > 0xFF)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
        startBit = 3;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        startBit = 2;
    }

    /* Set LBR Opcode */
    return prvCpssHwPpSetRegField(devNum, regAddr, startBit, 8, opcode);

}

/*******************************************************************************
* cpssDxChVntCfmLbrOpcodeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       opcode      - CFM LBR Opcode (APPLICABLE RANGES: 0..0xFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, opcode.
*       GT_OUT_OF_RANGE          - on wrong opcode values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmLbrOpcodeSet
(
    IN GT_U8    devNum,
    IN GT_U32   opcode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntCfmLbrOpcodeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, opcode));

    rc = internal_cpssDxChVntCfmLbrOpcodeSet(devNum, opcode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, opcode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntCfmLbrOpcodeGet
*
* DESCRIPTION:
*      Gets the current CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       opcodePtr   - pointer to CFM LBR Opcode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntCfmLbrOpcodeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *opcodePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    startBit;          /* start bit of the field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(opcodePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig;
        startBit = 3;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        startBit = 2;
    }

    /* Get LBR Opcode */
    return prvCpssHwPpGetRegField(devNum, regAddr, startBit, 8, opcodePtr);

}

/*******************************************************************************
* cpssDxChVntCfmLbrOpcodeGet
*
* DESCRIPTION:
*      Gets the current CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       opcodePtr   - pointer to CFM LBR Opcode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmLbrOpcodeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *opcodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntCfmLbrOpcodeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, opcodePtr));

    rc = internal_cpssDxChVntCfmLbrOpcodeGet(devNum, opcodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, opcodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntCfmReplyModeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode change mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       mode    - CFM Opcode change mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, change mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntCfmReplyModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT mode
)
{
    GT_U32  regData;    /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    switch(mode)
    {
        case CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_OPCODE_E:
            regData = 0;
            break;
        case CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_LSBIT_E:
            regData = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Set CFM Opcode change mode */
    return prvCpssHwPpSetRegField(devNum,
                                  PRV_DXCH_REG1_UNIT_HA_MAC(devNum).
                                                                haGlobalConfig1,
                                  10, 1, regData);

}

/*******************************************************************************
* cpssDxChVntCfmReplyModeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode change mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       mode    - CFM Opcode change mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, change mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmReplyModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntCfmReplyModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChVntCfmReplyModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntCfmReplyModeGet
*
* DESCRIPTION:
*      Gets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode change mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - (pointer to) CFM Opcode change mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, change mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - output parameter is NULL pointer.
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntCfmReplyModeGet
(
    IN GT_U8                            devNum,
    OUT CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT *modePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regData;    /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    rc = prvCpssHwPpGetRegField(devNum,
                                  PRV_DXCH_REG1_UNIT_HA_MAC(devNum).
                                                                haGlobalConfig1,
                                  10, 1, &regData);
    if( GT_OK != rc )
    {
        return rc;
    }

    *modePtr = (regData == 0) ?
        CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_OPCODE_E :
        CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_LSBIT_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVntCfmReplyModeGet
*
* DESCRIPTION:
*      Gets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode change mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - (pointer to) CFM Opcode change mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, change mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - output parameter is NULL pointer.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntCfmReplyModeGet
(
    IN GT_U8                            devNum,
    OUT CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntCfmReplyModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChVntCfmReplyModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntPortGroupLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*
* OUTPUTS:
*       timeStampValuePtr - pointer to timestamp of the last read operation.
*                           200  MHz:   granularity - 5 nSec
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntPortGroupLastReadTimeStampGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *timeStampValuePtr
)
{
    GT_U32    regAddr;       /* register address   */
    GT_U32    portGroupId;   /* the port group Id  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(timeStampValuePtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.lastReadTimeStampReg;

    /* Gets the last-read-time-stamp counter value */
    return prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,
                                               regAddr, timeStampValuePtr);

}

/*******************************************************************************
* cpssDxChVntPortGroupLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*
* OUTPUTS:
*       timeStampValuePtr - pointer to timestamp of the last read operation.
*                           200  MHz:   granularity - 5 nSec
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntPortGroupLastReadTimeStampGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *timeStampValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntPortGroupLastReadTimeStampGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, timeStampValuePtr));

    rc = internal_cpssDxChVntPortGroupLastReadTimeStampGet(devNum, portGroupsBmp, timeStampValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, timeStampValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       timeStampValuePtr - pointer to timestamp of the last read operation.
*                           200  MHz:   granularity - 5 nSec
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntLastReadTimeStampGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *timeStampValuePtr
)
{
    /* Gets the last-read-time-stamp counter value */
    return cpssDxChVntPortGroupLastReadTimeStampGet(devNum,
                                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                timeStampValuePtr);
}

/*******************************************************************************
* cpssDxChVntLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       timeStampValuePtr - pointer to timestamp of the last read operation.
*                           200  MHz:   granularity - 5 nSec
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChVntLastReadTimeStampGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *timeStampValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntLastReadTimeStampGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, timeStampValuePtr));

    rc = internal_cpssDxChVntLastReadTimeStampGet(devNum, timeStampValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, timeStampValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntOamPortPduTrapEnableSet
*
* DESCRIPTION:
*      Enable/Disable trap to CPU of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs destined for this port are trapped to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*       enable   - GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                  GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntOamPortPduTrapEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  enable
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    data;             /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    data = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to TTI-Physical-Port-Attribute table */
         rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_OAM_LINK_LAYER_PDU_TRAP_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            data);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
        /* write to port-vlan-qos table  */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         portNum,/* port */
                                         1,/* start at word 1 */
                                         31,/* start at bit 0*/
                                         1, /* 1 bit */
                                         data);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChVntOamPortPduTrapEnableSet
*
* DESCRIPTION:
*      Enable/Disable trap to CPU of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs destined for this port are trapped to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*       enable   - GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                  GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortPduTrapEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntOamPortPduTrapEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChVntOamPortPduTrapEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVntOamPortPduTrapEnableGet
*
* DESCRIPTION:
*      Get trap to CPU status(Enable/Disable) of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs are trapped to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*
*
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                     GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol..
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVntOamPortPduTrapEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS   rc;           /* return status                */
    GT_U32      hwValue;      /* value to read from port-vlan-qos table  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read from TTI-Physical-Port-Attribute table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_OAM_LINK_LAYER_PDU_TRAP_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &hwValue);

    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* read from port-vlan-qos table  */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                         PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                         portNum,/* port */
                                         1,/* start at word 1 */
                                         31,/* start at bit 0*/
                                         1, /* 1 bit */
                                         &hwValue);
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;

}

/*******************************************************************************
* cpssDxChVntOamPortPduTrapEnableGet
*
* DESCRIPTION:
*      Get trap to CPU status(Enable/Disable) of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs are trapped to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number ,CPU port.
*                  In eArch devices portNum is default ePort.
*
*
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                     GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol..
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVntOamPortPduTrapEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVntOamPortPduTrapEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChVntOamPortPduTrapEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}





