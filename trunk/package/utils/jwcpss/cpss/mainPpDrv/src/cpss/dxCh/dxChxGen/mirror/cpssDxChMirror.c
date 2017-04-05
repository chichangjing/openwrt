/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChMirror.c
*
* DESCRIPTION:
*       CPSS DxCh Mirror APIs implementation.
*
*
* FILE REVISION NUMBER:
*       $Revision: 57 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/mirror/private/prvCpssDxChMirrorLog.h>

#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChStc.h>

/* Convert analyzer port and device to null port and device */
#define CORE_MIRR_CONVERT_ANALYZER_FOR_CPU_OR_NULL_PORT_MAC(                       \
                       _analyzDev, _analyzPort, _ownDevice, _devNullPort)      \
{                                                                              \
    if (_analyzPort == CPSS_CPU_PORT_NUM_CNS)                                  \
    {                                                                          \
        _analyzDev = _ownDevice;                                               \
    }                                                                          \
    else                                                                       \
    if (_analyzPort == CPSS_NULL_PORT_NUM_CNS)                                 \
    {                                                                          \
        _analyzDev = _ownDevice;                                               \
        _analyzPort = _devNullPort;                                            \
    }                                                                          \
}

/* maximal index of analyzer */
#define  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS 6

/*******************************************************************************
* internal_cpssDxChMirrorAnalyzerVlanTagEnable
*
* DESCRIPTION:
*       Enable/Disable Analyzer port adding additional VLAN Tag to mirrored
*       packets. The content of this VLAN tag is configured
*       by cpssDxChMirrorTxAnalyzerVlanTagConfig.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number.
*       portNum     - ports number to be set
*       enable      - GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorAnalyzerVlanTagEnable
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  enable
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   rc;             /* function return code */
    GT_U32      fieldValue;     /* register's field value */
    GT_U32      mirrPort;       /* mirror port*/
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    /* Assign field value */
    fieldValue = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                               portNum,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                               LION3_HA_EPORT_TABLE_2_TO_ANALYZER_VLAN_TAG_ADD_EN_E, /* field name */
                                               PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                               fieldValue);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        mirrPort = OFFSET_TO_BIT_MAC(localPort);

        /* TO ANALYZER VLAN Adding Configuration Registers */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
            toAnalyzerVlanAddConfig[OFFSET_TO_WORD_MAC(localPort)];

        /* Bit per port indicating if a Vlan Tag is to be added to TO_ANALYZER Packets */
        rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, mirrPort, 1, fieldValue);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagEnable
*
* DESCRIPTION:
*       Enable/Disable Analyzer port adding additional VLAN Tag to mirrored
*       packets. The content of this VLAN tag is configured
*       by cpssDxChMirrorTxAnalyzerVlanTagConfig.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number.
*       portNum     - ports number to be set
*       enable      - GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagEnable
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorAnalyzerVlanTagEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChMirrorAnalyzerVlanTagEnable(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorAnalyzerVlanTagEnableGet
*
* DESCRIPTION:
*       Get Analyzer port VLAN Tag to mirrored packets mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number.
*       portNum     - ports number to be set
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorAnalyzerVlanTagEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_STATUS   rc;             /* function return code */
    GT_U32      fieldValue;     /* register's field value */
    GT_U32      mirrPort;       /* mirror port*/
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                               portNum,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                               LION3_HA_EPORT_TABLE_2_TO_ANALYZER_VLAN_TAG_ADD_EN_E, /* field name */
                                               PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                               &fieldValue);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        mirrPort = OFFSET_TO_BIT_MAC(localPort);

        /* TO ANALYZER VLAN Adding Configuration Registers */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
            toAnalyzerVlanAddConfig[OFFSET_TO_WORD_MAC(localPort)];

        /* Bit per port indicating if a Vlan Tag is to be added to TO_ANALYZER Packets */
        rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, mirrPort, 1, &fieldValue);
    }
    if(rc != GT_OK)
        return rc;

    /* Assign field value */
    *enablePtr = ((fieldValue != 0) ? GT_TRUE : GT_FALSE);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagEnableGet
*
* DESCRIPTION:
*       Get Analyzer port VLAN Tag to mirrored packets mode.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number.
*       portNum     - ports number to be set
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorAnalyzerVlanTagEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChMirrorAnalyzerVlanTagEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxAnalyzerVlanTagConfig
(
    IN GT_U8                                       devNum,
    IN CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regData;        /* register data */
    GT_STATUS   rc;             /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerVlanTagConfigPtr);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(analyzerVlanTagConfigPtr->vpt);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(analyzerVlanTagConfigPtr->vid);
    if (analyzerVlanTagConfigPtr->cfi > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* VLAN tag data vid+cfi+vpt+ethertype */
    regData = (analyzerVlanTagConfigPtr->vid) |
              (analyzerVlanTagConfigPtr->cfi << 12) |
              (analyzerVlanTagConfigPtr->vpt << 13) |
              (analyzerVlanTagConfigPtr->etherType << 16);

    /* Egress Analyzer VLAN Tag Configuration Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).egrAnalyzerVLANTagConfig;
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.egrAnalyzerVlanTagConfig;
    }

    rc = prvCpssHwPpWriteRegister(devNum, regAddr, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerVlanTagConfig
(
    IN GT_U8                                       devNum,
    IN CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxAnalyzerVlanTagConfig);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerVlanTagConfigPtr));

    rc = internal_cpssDxChMirrorTxAnalyzerVlanTagConfig(devNum, analyzerVlanTagConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerVlanTagConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxAnalyzerVlanTagConfigGet
(
    IN GT_U8                                       devNum,
    OUT CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regData;        /* register data */
    GT_STATUS   rc;             /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerVlanTagConfigPtr);


    /* Egress Analyzer VLAN Tag Configuration Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).egrAnalyzerVLANTagConfig;
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.egrAnalyzerVlanTagConfig;
    }

    rc = prvCpssHwPpReadRegister(devNum, regAddr, &regData);
    if(rc != GT_OK)
        return rc;

    /* VLAN tag data vid+cfi+vpt+ethertype */
    analyzerVlanTagConfigPtr->vid = (GT_U16)(regData & 0xFFF);
    analyzerVlanTagConfigPtr->cfi = (GT_U8)((regData & 0x1000) >> 12);
    analyzerVlanTagConfigPtr->vpt = (GT_U8)((regData & 0xE000) >> 13);
    analyzerVlanTagConfigPtr->etherType = (GT_U16)((regData & 0xFFFF0000) >> 16);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerVlanTagConfigGet
(
    IN GT_U8                                       devNum,
    OUT CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxAnalyzerVlanTagConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerVlanTagConfigPtr));

    rc = internal_cpssDxChMirrorTxAnalyzerVlanTagConfigGet(devNum, analyzerVlanTagConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerVlanTagConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorRxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorRxAnalyzerVlanTagConfig
(
    IN GT_U8                                       devNum,
    IN CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regData;        /* register data */
    GT_STATUS   rc;             /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerVlanTagConfigPtr);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(analyzerVlanTagConfigPtr->vpt);
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(analyzerVlanTagConfigPtr->vid);
    if (analyzerVlanTagConfigPtr->cfi > 1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* VLAN tag data vid+cfi+vpt+ethertype */
    regData = (analyzerVlanTagConfigPtr->vid) |
              (analyzerVlanTagConfigPtr->cfi << 12) |
              (analyzerVlanTagConfigPtr->vpt << 13) |
              (analyzerVlanTagConfigPtr->etherType << 16);

    /* Ingress Analyzer VLAN Tag Configuration Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).ingrAnalyzerVLANTagConfig;
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ingAnalyzerVlanTagConfig;
    }

    rc = prvCpssHwPpWriteRegister(devNum, regAddr, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerVlanTagConfig
(
    IN GT_U8                                       devNum,
    IN CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorRxAnalyzerVlanTagConfig);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerVlanTagConfigPtr));

    rc = internal_cpssDxChMirrorRxAnalyzerVlanTagConfig(devNum, analyzerVlanTagConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerVlanTagConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorRxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorRxAnalyzerVlanTagConfigGet
(
    IN GT_U8                                       devNum,
    OUT CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      regData;        /* register data */
    GT_STATUS   rc;             /* function return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerVlanTagConfigPtr);

    /* Ingress Analyzer VLAN Tag Configuration Register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).ingrAnalyzerVLANTagConfig;
    }
    else
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.ingAnalyzerVlanTagConfig;
    }

    rc = prvCpssHwPpReadRegister(devNum, regAddr, &regData);
    if(rc != GT_OK)
        return rc;

    /* VLAN tag data vid+cfi+vpt+ethertype */
    analyzerVlanTagConfigPtr->vid = (GT_U16)(regData & 0xFFF);
    analyzerVlanTagConfigPtr->cfi = (GT_U8)((regData & 0x1000) >> 12);
    analyzerVlanTagConfigPtr->vpt = (GT_U8)((regData & 0xE000) >> 13);
    analyzerVlanTagConfigPtr->etherType = (GT_U16)((regData & 0xFFFF0000) >> 16);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerVlanTagConfigGet
(
    IN GT_U8                                       devNum,
    OUT CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC *analyzerVlanTagConfigPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorRxAnalyzerVlanTagConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerVlanTagConfigPtr));

    rc = internal_cpssDxChMirrorRxAnalyzerVlanTagConfigGet(devNum, analyzerVlanTagConfigPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerVlanTagConfigPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a port to be an analyzer port of tx mirrored
*       ports on all system pp's.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerHwDev - pp HW Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxAnalyzerPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     analyzerPort,
    IN  GT_HW_DEV_NUM   analyzerHwDev
)
{

    GT_U32      regAddr;     /* hw pp memory address */
    GT_U32      data;        /* data for hw write in pp */
    GT_STATUS   rc;          /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffPortsCfg;

    CORE_MIRR_CONVERT_ANALYZER_FOR_CPU_OR_NULL_PORT_MAC(
            analyzerHwDev, analyzerPort, (PRV_CPSS_HW_DEV_NUM_MAC(devNum)),
            PRV_CPSS_DXCH_NULL_PORT_NUM_CNS);

    data = ((analyzerPort & 0x3f) << 5) |
            (analyzerHwDev & 0x1f);

    /* configure the egress analyzer device and port */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 11, data);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a port to be an analyzer port of tx mirrored
*       ports on all system pp's.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerHwDev - pp HW Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     analyzerPort,
    IN  GT_HW_DEV_NUM   analyzerHwDev
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxAnalyzerPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerPort, analyzerHwDev));

    rc = internal_cpssDxChMirrorTxAnalyzerPortSet(devNum, analyzerPort, analyzerHwDev);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerPort, analyzerHwDev));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of tx mirrored ports on all system pp's.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerHwDevPtr - (pointer to)pp HW Device number of analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxAnalyzerPortGet
(
    IN  GT_U8           devNum,
    OUT GT_PORT_NUM     *analyzerPortPtr,
    OUT GT_HW_DEV_NUM   *analyzerHwDevPtr
)
{
    GT_U32      regAddr;     /* hw pp memory address */
    GT_U32      data;        /* data for hw write in pp */
    GT_STATUS   rc;          /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(analyzerHwDevPtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffPortsCfg;

    /* get configuration of the egress analyzer device and port */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 11, &data);

    if(rc != GT_OK)
        return rc;


    *analyzerPortPtr = ((data & 0x7E0) >> 5);
    *analyzerHwDevPtr = (data & 0x1f);

    return rc;

}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of tx mirrored ports on all system pp's.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerHwDevPtr - (pointer to)pp HW Device number of analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxAnalyzerPortGet
(
    IN  GT_U8           devNum,
    OUT GT_PORT_NUM     *analyzerPortPtr,
    OUT GT_HW_DEV_NUM   *analyzerHwDevPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxAnalyzerPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerPortPtr, analyzerHwDevPtr));

    rc = internal_cpssDxChMirrorTxAnalyzerPortGet(devNum, analyzerPortPtr, analyzerHwDevPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerPortPtr, analyzerHwDevPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorRxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a specific port to be an analyzer port of Rx mirrored
*       ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerHwDev - pp HW Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorRxAnalyzerPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     analyzerPort,
    IN  GT_HW_DEV_NUM   analyzerHwDev
)
{

    GT_U32      regAddr;     /* hw pp memory address */
    GT_U32      data;        /* data for hw write in pp */
    GT_STATUS   rc;          /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffPortsCfg;

    CORE_MIRR_CONVERT_ANALYZER_FOR_CPU_OR_NULL_PORT_MAC(
            analyzerHwDev, analyzerPort, (PRV_CPSS_HW_DEV_NUM_MAC(devNum)),
            PRV_CPSS_DXCH_NULL_PORT_NUM_CNS);

    data = ((analyzerPort & 0x3f) << 5) |
            (analyzerHwDev & 0x1f);


    /* configure the ingress analyzer device and port */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 11, 11, data);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a specific port to be an analyzer port of Rx mirrored
*       ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerHwDev - pp HW Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     analyzerPort,
    IN  GT_HW_DEV_NUM   analyzerHwDev
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorRxAnalyzerPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerPort, analyzerHwDev));

    rc = internal_cpssDxChMirrorRxAnalyzerPortSet(devNum, analyzerPort, analyzerHwDev);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerPort, analyzerHwDev));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorRxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of Rx mirrored ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*
*       devNum  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerHwDevPtr - (pointer to)pp HW Device number of analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorRxAnalyzerPortGet
(
    IN  GT_U8           devNum,
    OUT GT_PORT_NUM     *analyzerPortPtr,
    OUT GT_HW_DEV_NUM   *analyzerHwDevPtr
)
{
    GT_U32      regAddr;     /* hw pp memory address */
    GT_U32      data;        /* data for hw write in pp */
    GT_STATUS   rc;          /* function call return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(analyzerPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(analyzerHwDevPtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffPortsCfg;

    /* get configuration of the ingress analyzer device and port */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 11, 11, &data);

    if(rc != GT_OK)
        return rc;


    *analyzerPortPtr = ((data & 0x7E0) >> 5);
    *analyzerHwDevPtr = (data & 0x1f);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of Rx mirrored ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* NOT APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*
*       devNum  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerHwDevPtr - (pointer to)pp HW Device number of analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxAnalyzerPortGet
(
    IN  GT_U8           devNum,
    OUT GT_PORT_NUM     *analyzerPortPtr,
    OUT GT_HW_DEV_NUM   *analyzerHwDevPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorRxAnalyzerPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerPortPtr, analyzerHwDevPtr));

    rc = internal_cpssDxChMirrorRxAnalyzerPortGet(devNum, analyzerPortPtr, analyzerHwDevPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerPortPtr, analyzerHwDevPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxCascadeMonitorEnable
*
* DESCRIPTION:
*       One global bit that is set to 1 when performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - the device number
*       enable    - enable/disable Egress Monitoring on cascading ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxCascadeMonitorEnable
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{

    GT_U32      regAddr, regData; /* pp memory address and data for hw access*/
    GT_STATUS   rc;               /* function call return value            */
    GT_U32      bitNum;           /* bit number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    regData = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.cscdEgressMonitoringEnableConfReg;
        bitNum = 0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trSrcSniff;
        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* Cheetah 2 and above devices */
            bitNum = 29;
        }
        else
        {
            /* Cheetah and Cheetah+ devices */
            bitNum = 28;
        }

    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, bitNum, 1, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxCascadeMonitorEnable
*
* DESCRIPTION:
*       One global bit that is set to 1 when performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - the device number
*       enable    - enable/disable Egress Monitoring on cascading ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxCascadeMonitorEnable
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxCascadeMonitorEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChMirrorTxCascadeMonitorEnable(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxCascadeMonitorEnableGet
*
* DESCRIPTION:
*       Get One global bit that indicate performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - the device number
*
* OUTPUTS:
*       enablePtr    - enable/disable Egress Monitoring on cascading ports.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxCascadeMonitorEnableGet
(
    IN   GT_U8   devNum,
    OUT  GT_BOOL *enablePtr
)
{

    GT_U32      regAddr, regData; /* pp memory address and data for hw access*/
    GT_STATUS   rc;               /* function call return value            */
    GT_U32      bitNum;           /* bit number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.cscdEgressMonitoringEnableConfReg;
        bitNum = 0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trSrcSniff;
        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* Cheetah 2 and above devices */
            bitNum = 29;
        }
        else
        {
            /* Cheetah and Cheetah+ devices */
            bitNum = 28;
        }
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitNum, 1, &regData);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (regData == 0x1) ? GT_TRUE : GT_FALSE;


    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxCascadeMonitorEnableGet
*
* DESCRIPTION:
*       Get One global bit that indicate performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - the device number
*
* OUTPUTS:
*       enablePtr    - enable/disable Egress Monitoring on cascading ports.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxCascadeMonitorEnableGet
(
    IN   GT_U8   devNum,
    OUT  GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxCascadeMonitorEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChMirrorTxCascadeMonitorEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* prvCpssDxChMirrorPortIndexRegDataCalculate
*
* DESCRIPTION:
*       Calculate  start bit and  Port Index Register index
*       according to the port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum         - the device number
*       portNum        - port number.
*       rxMirrorEnable - GT_TRUE - Calculate  start bit and
*                            Port Index Register index for ingress mirror port.
*                      - GT_FALSE - Calculate  start bit and
*                            Port Index Register index for egress mirror port.
*
* OUTPUTS:
*       startBitPtr  - pointer to start bit.
*       regIndexPtr  - pointer to register index.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvCpssDxChMirrorPortIndexRegDataCalculate
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 rxMirrorEnable,
    OUT GT_U32                  *startBitPtr,
    OUT GT_U32                  *regIndexPtr
)
{
    /* Note: for RX this function expect 'local port' (to port group) */
    /* Note: for TX this function expect 'global port' (to device)    */

    if(((0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum)) ||
       (rxMirrorEnable == GT_TRUE)) && (portNum == CPSS_CPU_PORT_NUM_CNS))
    {
        /* for RX : CPU port - for all devices          */
        /* for TX : CPU port - for ch1,diamond,2,3,xcat */
        portNum = 28;/* lead to register 2 bit 24..26 */
    }

    /*************************************************/
    /* for RX : 3  registers .. 10 ports in register */
    /*************************************************/

    /********************************************************************************/
    /* for TX : 14 registers .. 10 ports in register .. lower 64 in registers 0..6  */
    /*                                               .. next  64 in registers 7..13 */
    /*    ch1,diamond,2,3,xcat : 3 registers --> supported like RX register         */
    /*    Lion , xcat2 : 7 registers --> support lower 64 ports                     */
    /*    Lion2        : 14 registers --> support 128 ports                         */
    /********************************************************************************/

    *startBitPtr = ((portNum & 0x3f) % 10) * 3;
    *regIndexPtr = ((portNum & 0x3f) / 10) + (((portNum & BIT_6) >> 6) * 7);
}

/*******************************************************************************
* prvCpssDxChMirrorToAnalyzerForwardingModeGet
*
* DESCRIPTION:
*       Get Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      modePtr   - pointer to mode of forwarding To Analyzer packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChMirrorToAnalyzerForwardingModeGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   *modePtr
)
{
    GT_U32      regAddr;      /* register address */
    GT_U32      regData;      /* register data */
    GT_STATUS   rc;           /* return status */

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.analyzerPortGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 2, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch(regData)
        {
            case 0:
                *modePtr = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_END_TO_END_E ;
                break;
            case 1:
                *modePtr = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E ;
                break;
            case 2:
                *modePtr = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E ;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        switch(regData)
        {
            case 0:
                *modePtr = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E ;
                break;
            case 3:
                *modePtr = CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific ePort to be Rx mirrored port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       mirrPort  - port number, CPU port supported.
*       enable    - enable/disable Rx mirror on this port
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       index     - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChMirrorRxPortSet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             mirrPort,
    IN  GT_BOOL                 enable,
    IN  GT_U32                  index
)
{
    GT_U32      data;       /* data set to hw */

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum, mirrPort);

    if(index > PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* Set index to Analyzer interface for the ePort */
    if(enable == GT_TRUE)
    {
        data = index + 1;
    }
    else /* No mirroring for the ePort */
    {
        data = 0;
    }

    /* set EQ-ingress-eport table */
    return prvCpssDxChWriteTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_EPORT_E,
                                       mirrPort,
                                       PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                       4, /* start bit */
                                       3, /* 3 bit */
                                       data);
}

/*******************************************************************************
* internal_cpssDxChMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific port to be Rx mirrored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number, CPU port supported.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*       enable         - enable/disable Rx mirror on this port
*                        GT_TRUE - Rx mirroring enabled, packets
*                                  received on a mirrPort are
*                                  mirrored to Rx analyzer.
*                        GT_FALSE - Rx mirroring disabled.
*       index          - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*                        Supported for xCat and above device.
*                        Used only if forwarding mode to analyzer is Source-based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorRxPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     mirrPort,
    IN  GT_BOOL         isPhysicalPort,
    IN  GT_BOOL         enable,
    IN  GT_U32          index
)
{
    GT_U32      data;        /* data from Ports VLAN and QoS table entry */
    GT_U32      offset;      /* offset in VLAN and QoS table entry */
    GT_STATUS   rc = GT_OK;  /* function call return value */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32       localPort;  /* local port - support multi-port-groups device */
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mode; /* forwarding mode */
    GT_U32      regIndex; /* the index of Port Ingress Mirror Index register */
    GT_U32      regAddr;  /* pp memory address for hw access*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        /* device support physical port mirroring only */
        isPhysicalPort = GT_TRUE;
    }

    if (isPhysicalPort == GT_FALSE)
    {
        /* configure ePort mirroring */
        return prvCpssDxChMirrorRxPortSet(devNum, mirrPort, enable, index);
    }

    /* configure physical port mirroring */
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,mirrPort);

    if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Get Analyzer forwarding mode */
        rc = prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Source-based-forwarding mode. */
        if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)||
           (mode != CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E))
        {
            if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            /* Set index to Analyzer interface for the port */
            if(enable == GT_TRUE)
            {
                data = index + 1;
            }
            /* No mirroring for the port */
            else
            {
                data = 0;
            }

            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                offset = (mirrPort % 8) * 4;/* 8 ports in entry , in steps of 4 bits*/

                rc = prvCpssDxChWriteTableEntryField(devNum,
                                                    PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E,
                                                    (mirrPort >> 3), /* global port / 8 */
                                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                    offset,
                                                    3,
                                                    data);
            }
            else
            {
                /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
                portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, mirrPort);
                /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
                localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,mirrPort);

                prvCpssDxChMirrorPortIndexRegDataCalculate(devNum, localPort, GT_TRUE,
                                                           &offset, &regIndex);

                /* getting register address */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    bufferMng.eqBlkCfgRegs.portRxMirrorIndex[regIndex];

                /* Set Analyzer destination interface index  */
                rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, offset,
                                                     3, data);
            }

            if(rc != GT_OK)
            {
                return rc;
            }
        }
        offset = 7;
    }
    else
    {
        offset = 23;
    }

    /* For xCat and above hop-by-hop forwarding mode and other DxCh devices */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        data = BOOL2BIT_MAC(enable);

        /* configure the Ports VLAN and QoS configuration entry,
           enable MirrorToIngressAnalyzerPort field */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            mirrPort,
                                            0,
                                            offset,
                                            1,
                                            data);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific port to be Rx mirrored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number, CPU port supported.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*       enable         - enable/disable Rx mirror on this port
*                        GT_TRUE - Rx mirroring enabled, packets
*                                  received on a mirrPort are
*                                  mirrored to Rx analyzer.
*                        GT_FALSE - Rx mirroring disabled.
*       index          - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*                        Supported for xCat and above device.
*                        Used only if forwarding mode to analyzer is Source-based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxPortSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     mirrPort,
    IN  GT_BOOL         isPhysicalPort,
    IN  GT_BOOL         enable,
    IN  GT_U32          index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorRxPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mirrPort, isPhysicalPort, enable, index));

    rc = internal_cpssDxChMirrorRxPortSet(devNum, mirrPort, isPhysicalPort, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mirrPort, isPhysicalPort, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* prvCpssDxChMirrorRxPortGet
*
* DESCRIPTION:
*       Gets status of Rx mirroring (enabled or disabled) of specific ePort
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - the device number
*       mirrPort     - port number, CPU port supported.
*
* OUTPUTS:
*       enablePtr    - (pointer to) Rx mirror mode
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index.
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChMirrorRxPortGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             mirrPort,
    OUT GT_BOOL                 *enablePtr,
    OUT GT_U32                  *indexPtr
)
{
    GT_STATUS   rc;    /* function return code */
    GT_U32      data;  /* data read */

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,mirrPort);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    /* get EQ-ingress-eport table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                       PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_EPORT_E,
                                       mirrPort,
                                       PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                       4, /* start bit */
                                       3, /* 3 bit */
                                       &data);

    if(rc != GT_OK)
        return rc;

    /* No mirroring for the ePort */
    if(!data)
    {
        *enablePtr = GT_FALSE;
    }
    /* Get index to Analyzer interface for the ePort */
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = data - 1;
    }

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorRxPortGet
*
* DESCRIPTION:
*       Gets status of Rx mirroring (enabled or disabled) of specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number, CPU port supported.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*
* OUTPUTS:
*       enablePtr    - (pointer to) Rx mirror mode
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index.
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorRxPortGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     mirrPort,
    IN  GT_BOOL         isPhysicalPort,
    OUT GT_BOOL         *enablePtr,
    OUT GT_U32          *indexPtr
)
{
    GT_U32    data;     /* HW data */
    GT_U32    offset;   /* field offset */
    GT_STATUS rc;       /* function call return value */
    GT_U32  regIndex;   /* the index of Port Ingress Mirror Index register */
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mode; /* forwarding mode */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;  /* local port - support multi-port-groups device */
    GT_U32  regAddr;    /* pp memory address for hw access*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        /* device support physical port mirroring only */
        isPhysicalPort = GT_TRUE;
    }

    if (isPhysicalPort == GT_FALSE)
    {
        /* get ePort mirroring configuration */
        return prvCpssDxChMirrorRxPortGet(devNum, mirrPort, enablePtr, indexPtr);
    }

    /* get phisical port mirroring configuration */
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, mirrPort);

    if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Get Analyzer forwarding mode */
        rc = prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Source-based-forwarding mode. */
        if((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)||
           (mode != CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E))
        {
            CPSS_NULL_PTR_CHECK_MAC(indexPtr);

            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
            {
                offset = (mirrPort % 8) * 4;/* 8 ports in entry , in steps of 4 bits*/

                rc = prvCpssDxChReadTableEntryField(devNum,
                                                    PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E,
                                                    (mirrPort >> 3), /* global port / 8 */
                                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                    offset,
                                                    3,
                                                    &data);
            }
            else
            {
                /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
                portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, mirrPort);
                /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
                localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,mirrPort);

                prvCpssDxChMirrorPortIndexRegDataCalculate(devNum, localPort, GT_TRUE,
                                                           &offset, &regIndex);

                /* getting register address */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    bufferMng.eqBlkCfgRegs.portRxMirrorIndex[regIndex];

                /* Get Analyzer destination interface index  */
                rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset,
                                               3, &data);
            }

            if(rc != GT_OK)
            {
                return rc;
            }

            /* No mirroring for the port */
            if(!data)
            {
                *enablePtr = GT_FALSE;
            }
            /* Get index to Analyzer interface for the port */
            else
            {
                *enablePtr = GT_TRUE;
                *indexPtr = data - 1;
            }

            return GT_OK;
        }
        /* xCat and above hop-by-hop forwarding mode */
        else
        {
            offset = 7;
        }
    }
    else
    {
        offset = 23;
    }

    /* For xCat and above hop-by-hop forwarding mode and other DxCh devices */
    /* Get configuration MirrorToIngressAnalyzerPort field */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        mirrPort,
                                        0,
                                        offset,
                                        1,
                                        &data);
    if(rc != GT_OK)
        return rc;

    *enablePtr = (data != 0) ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorRxPortGet
*
* DESCRIPTION:
*       Gets status of Rx mirroring (enabled or disabled) of specific port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number, CPU port supported.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*
* OUTPUTS:
*       enablePtr    - (pointer to) Rx mirror mode
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index.
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxPortGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     mirrPort,
    IN  GT_BOOL         isPhysicalPort,
    OUT GT_BOOL         *enablePtr,
    OUT GT_U32          *indexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorRxPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mirrPort, isPhysicalPort, enablePtr, indexPtr));

    rc = internal_cpssDxChMirrorRxPortGet(devNum, mirrPort, isPhysicalPort, enablePtr, indexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mirrPort, isPhysicalPort, enablePtr, indexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChMirrorTxPortSet
*
* DESCRIPTION:
*       Enable or disable Tx mirroring per Eport.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       mirrPort  - port number.
*       enable    - enable/disable Tx mirror on this ePort
*                   GT_TRUE - Tx mirroring enabled, packets
*                             transmitted from a mirrPort are
*                             mirrored to Tx analyzer.
*                   GT_FALSE - Tx mirroring disabled.
*       index     - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChMirrorTxPortSet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             mirrPort,
    IN  GT_BOOL                 enable,
    IN  GT_U32                  index
)
{
    GT_STATUS   rc;
    GT_U32      data;       /* data set to hw */

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum, mirrPort);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* Set index to Analyzer interface for the ePort */
    if(enable == GT_TRUE)
    {
        data = index + 1;
    }
    /* No mirroring for the port */
    else
    {
        data = 0;
    }

    /* NOTE: EQ not need <MDB> issues */


    rc = prvCpssDxChWriteTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    mirrPort,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EGRESS_MIRROR_TO_ANALYZER_INDEX_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   data);
    return rc;
}

/*******************************************************************************
* prvCpssDxChMirrorTxPortGet
*
* DESCRIPTION:
*       Get status (enabled/disabled) of Tx mirroring per ePort .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       mirrPort  - port number.
*
* OUTPUTS:
*       enablePtr - (pointer to) Tx mirror mode on this eport
*                                GT_TRUE - Tx mirroring enabled, packets
*                                          transmitted from a mirrPort are
*                                          mirrored to Tx analyzer.
*                                GT_FALSE - Tx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChMirrorTxPortGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             mirrPort,
    OUT GT_BOOL                 *enablePtr,
    OUT GT_U32                  *indexPtr
)
{

    GT_STATUS   rc;   /* function return code */
    GT_U32      data; /* data read */

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum, mirrPort);

    rc = prvCpssDxChReadTableEntryField(devNum,
                                    PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
                                    mirrPort,
                                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EGRESS_MIRROR_TO_ANALYZER_INDEX_E, /* field name */
                                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                   &data);

    /* No mirroring for the ePort */
    if(!data)
    {
        *enablePtr = GT_FALSE;
    }
    /* Get index to Analyzer interface for the ePort */
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = data - 1;
    }

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxPortSet
*
* DESCRIPTION:
*       Enable or disable Tx mirroring per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*       enable         - enable/disable Tx mirror on this port
*                        GT_TRUE - Tx mirroring enabled, packets
*                                  transmitted from a mirrPort are
*                                  mirrored to Tx analyzer.
*                        GT_FALSE - Tx mirroring disabled.
*       index          - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*                        Supported for xCat and above device.
*                        Used only if forwarding mode to analyzer is Source-based.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxPortSet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             mirrPort,
    IN  GT_BOOL                 isPhysicalPort,
    IN  GT_BOOL                 enable,
    IN  GT_U32                  index
)
{
    GT_STATUS   rc;     /* function call return value              */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  regIndex;   /* the index of Port Egress Mirror Index register */
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mode; /* forwarding mode */
    GT_U32   localPort;/* local port - support multi-port-groups device */
    GT_U32  regAddr1, regAddr2 = 0, regAddr3; /* pp memory address for hw access*/
    GT_U32  startBit1,startBit2 = 0, startBit3; /* bits to start to write data for registers 1 & 2 & 3*/
    GT_U32  regData2 = 0;  /* register2 data */
    GT_U32  stcLimit;   /* Egress STC limit */
    GT_BOOL readBeforeTxQWrite; /* read previous register before TxQ register write */
    GT_U32  readPortGroupId; /* port group id iterator for read after write */
    GT_U32  readRegData;     /* register data */
    GT_U32  analyzerIndex;/* analyzer index - xcat and above */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* check index parameter for E_ARCH devices */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        /* device support physical port mirroring only */
        isPhysicalPort = GT_TRUE;
    }

    if (isPhysicalPort == GT_FALSE)
    {
        /* configure ePort mirroring */
        return prvCpssDxChMirrorTxPortSet(devNum, mirrPort, enable, index);
    }

    if(enable == GT_TRUE)
    {
        analyzerIndex = index + 1;
    }
    /* No mirroring for the port */
    else
    {
        analyzerIndex = 0;
    }

    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, mirrPort);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, mirrPort);
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,mirrPort);

    /* check if need to set 'per port' in the EQ unit */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE &&
       PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Get Analyzer forwarding mode */
        rc = prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Source-based-forwarding mode. */
        if(mode != CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E)
        {
            if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            prvCpssDxChMirrorPortIndexRegDataCalculate(devNum, mirrPort, GT_FALSE,
                                                       &startBit2, &regIndex);

            /* getting register address */
            regAddr2 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bufferMng.eqBlkCfgRegs.portTxMirrorIndex[regIndex];

            regData2 = analyzerIndex;
        }
    }

    readBeforeTxQWrite = GT_FALSE;
    /* configure EgressMirrorToAnalyzerEn field in Port<n> Txq
       Configuration Register */
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,mirrPort,&regAddr1);
        startBit1 = 20;

    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr1 = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.egrAnalyzerEnableIndex[(mirrPort>>3)];
        }
        else
        {
            regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
                egressAnalyzerEnable;
        }

        startBit1 = localPort;

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            startBit1 = (mirrPort % 8) * 3;
        }
        else
        if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            startBit1 &= 0xf;
            readBeforeTxQWrite = GT_TRUE;
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* in eArch this per physical is not used */
        regAddr3 = PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        startBit3 = 0;
    }
    else
    {
        regAddr3 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trSrcSniff;

        startBit3 = mirrPort % 32;

        if(PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            regAddr3 += 0x64 * (mirrPort >> 5);
        }
        else
        {
            if (mirrPort  == CPSS_CPU_PORT_NUM_CNS)
            {
                /* Determine CPU port bit offset */
                /* Cheetah 2,3.. devices */
                if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
                {
                    startBit3 = 28;
                }
                else /* Cheetah and Cheetah+ devices */
                {
                    startBit3 = 27;
                }
            }
        }
    }

    /* For CPU port STC is not relevant, therefore for CPU port STC is considered as */
    /* disabled by forcing the limit indication to 0. */
    if (mirrPort  == CPSS_CPU_PORT_NUM_CNS)
    {
        stcLimit = 0;
    }
    else
    {
        /* the mirrPort is 'physical' port so no problem calling API of physical port */
        rc = cpssDxChStcPortLimitGet(devNum, mirrPort, CPSS_DXCH_STC_EGRESS_E, &stcLimit);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    if(enable == GT_TRUE)
    {

        /* Prior to configuring any non-cascade port configured for egress mirroring,
           for source based mirroring set Port Egress Mirror Index,
           for hop-by-hop mirroring set the corresponding bit
           in the <PortEgress MonitorEn[28:0]> field of the
           Egress Monitoring Enable Configuration Register.
           The Egress Monitoring Enable Configuration Register be set according
           to the global port number and must be set in all the cores.
        */

        /* If Egress STC is disabled on port enable Egress monitoring on port. */
        if( 0 == stcLimit )
        {
            if(regAddr3 != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                rc = prvCpssHwPpSetRegField(devNum, regAddr3, startBit3, 1, 1);
                if (rc != GT_OK)
                {
                    return rc;
                }

                /* In multi port group devices need to guaranty that all registers
                   were updated before TxQ register write. Because EQ registers
                   are per port group but TxQ register is only one in device.
                   This is possible that EQ registers in some port groups are
                   updated after TxQ. Need to avoid such problem by read EQ
                   register after write to TxQ one. */
                if (readBeforeTxQWrite != GT_FALSE)
                {
                    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,readPortGroupId)
                    {
                        rc = prvCpssHwPpPortGroupReadRegister (devNum, readPortGroupId, regAddr3, &readRegData);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,readPortGroupId)
                }
            }
        }

        if( regAddr2 != 0 )
        {
            rc = prvCpssHwPpSetRegField(devNum, regAddr2,
                                           startBit2, 3, regData2);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (readBeforeTxQWrite != GT_FALSE)
            {
                PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,readPortGroupId)
                {
                    rc = prvCpssHwPpPortGroupReadRegister (devNum, readPortGroupId, regAddr2, &readRegData);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
                PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,readPortGroupId)
            }
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr1, startBit1, 3, analyzerIndex);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr1, startBit1, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    else
    {
        /* The order must be opposite from above. */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr1, startBit1, 3, analyzerIndex);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr1, startBit1, 1, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* read TxQ register to guaranty that it was updated before EQ registers change. */
        if (readBeforeTxQWrite != GT_FALSE)
        {
            rc = prvCpssHwPpPortGroupReadRegister (devNum, portGroupId, regAddr1, &readRegData);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if( regAddr2 != 0 )
        {
            rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr2,
                                                    startBit2, 3, regData2);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* If Egress STC is disabled on port disable Egress monitoring on port. */
        if( 0 == stcLimit )
        {
            if(regAddr3 != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
            {
                rc = prvCpssHwPpSetRegField(devNum, regAddr3, startBit3, 1, 0);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortSet
*
* DESCRIPTION:
*       Enable or disable Tx mirroring per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*       enable         - enable/disable Tx mirror on this port
*                        GT_TRUE - Tx mirroring enabled, packets
*                                  transmitted from a mirrPort are
*                                  mirrored to Tx analyzer.
*                        GT_FALSE - Tx mirroring disabled.
*       index          - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*                        Supported for xCat and above device.
*                        Used only if forwarding mode to analyzer is Source-based.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortSet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             mirrPort,
    IN  GT_BOOL                 isPhysicalPort,
    IN  GT_BOOL                 enable,
    IN  GT_U32                  index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mirrPort, isPhysicalPort, enable, index));

    rc = internal_cpssDxChMirrorTxPortSet(devNum, mirrPort, isPhysicalPort, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mirrPort, isPhysicalPort, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChMirrorTxPortGet
*
* DESCRIPTION:
*       Get status (enabled/disabled) of Tx mirroring per port .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*
* OUTPUTS:
*       enablePtr - (pointer to) Tx mirror mode on this port
*                                GT_TRUE - Tx mirroring enabled, packets
*                                          transmitted from a mirrPort are
*                                          mirrored to Tx analyzer.
*                                GT_FALSE - Tx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index.
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxPortGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             mirrPort,
    IN  GT_BOOL                 isPhysicalPort,
    OUT GT_BOOL                 *enablePtr,
    OUT GT_U32                  *indexPtr
)
{

    GT_U32      regAddr;
    GT_STATUS   rc;
    GT_U32      value=0;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  regIndex;   /* the index of Port Egress Mirror Index register */
    GT_U32  regData;    /* register data */
    GT_U32  startBit , startBit1;       /* bit to start to write data */
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mode; /* forwarding mode */
    GT_U32   localPort; /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        /* device support physical port mirroring only */
        isPhysicalPort = GT_TRUE;
    }

    if (isPhysicalPort == GT_FALSE)
    {
        /* get ePort mirroring configuration */
        return prvCpssDxChMirrorTxPortGet(devNum, mirrPort, enablePtr, indexPtr);
    }

    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, mirrPort);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, mirrPort);
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,mirrPort);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,mirrPort,&regAddr);
        startBit1 = 20;
    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.egrAnalyzerEnableIndex[(mirrPort>>3)];
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
                statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.
                egressAnalyzerEnable;
        }

        startBit1 = localPort;

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            startBit1  = (mirrPort % 8) * 3;
        }
        else
        if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            startBit1 &= 0xf;
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, startBit1, 3, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, startBit1, 1, &value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(value != 0)
    {
        *enablePtr = GT_TRUE;
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            *indexPtr = value - 1;
        }
        else
        if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* Get Analyzer destination interface index for xCat only, if
               Forwarding mode is Source-based. */

            /* Get Analyzer forwarding mode */
            rc = prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* Source-based-forwarding mode. */
            if(mode != CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E)
            {
                prvCpssDxChMirrorPortIndexRegDataCalculate(devNum, mirrPort, GT_FALSE,
                                                           &startBit, &regIndex);

                /* getting register address */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    bufferMng.eqBlkCfgRegs.portTxMirrorIndex[regIndex];

                /* Get Analyzer destination interface index  */
                rc = prvCpssHwPpGetRegField(devNum, regAddr, startBit,
                                                 3, &regData);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* No mirroring for the port */
                if(!regData)
                {
                     CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
                /* Get index to Analyzer interface for the port */
                else
                {
                    *indexPtr = regData - 1;
                }
            }
        }
    }
    else
    {
        *enablePtr = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortGet
*
* DESCRIPTION:
*       Get status (enabled/disabled) of Tx mirroring per port .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - the device number
*       mirrPort       - port number.
*       isPhysicalPort - defines type of mirrPort parameter to be either
*                        physical port or ePort.
*                        GT_TRUE  - mirrPort is physical port
*                        GT_FALSE - mirrPort is ePort
*                        APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
*                        Ignored by other devices, mirrPort is always physical one.
*
* OUTPUTS:
*       enablePtr - (pointer to) Tx mirror mode on this port
*                                GT_TRUE - Tx mirroring enabled, packets
*                                          transmitted from a mirrPort are
*                                          mirrored to Tx analyzer.
*                                GT_FALSE - Tx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index.
*                   Supported for xCat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             mirrPort,
    IN  GT_BOOL                 isPhysicalPort,
    OUT GT_BOOL                 *enablePtr,
    OUT GT_U32                  *indexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mirrPort, isPhysicalPort, enablePtr, indexPtr));

    rc = internal_cpssDxChMirrorTxPortGet(devNum, mirrPort, isPhysicalPort, enablePtr, indexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mirrPort, isPhysicalPort, enablePtr, indexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChMirrRxStatMirroringToAnalyzerRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Rx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or ratio.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrRxStatMirroringToAnalyzerRatioSet
(
    IN GT_U8    devNum,
    IN GT_U32   ratio
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if(ratio >= BIT_11)/* 11 bits in hw */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.ingrStatisticMirrToAnalyzerPortConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trapSniffed;
    }

    /* writing <IngressStatMirroringToAnalyzerPortRatio> field */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 11, ratio);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirroringToAnalyzerRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Rx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or ratio.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirroringToAnalyzerRatioSet
(
    IN GT_U8    devNum,
    IN GT_U32   ratio
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrRxStatMirroringToAnalyzerRatioSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ratio));

    rc = internal_cpssDxChMirrRxStatMirroringToAnalyzerRatioSet(devNum, ratio);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ratio));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrRxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Rx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*
* OUTPUTS:
*      ratioPtr - (pointer to) Indicates the ratio of egress mirrored to analyzer port packets
*                 forwarded to the analyzer port. 1 of every 'ratio' packets are
*                 forwarded to the analyzer port.
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrRxStatMirroringToAnalyzerRatioGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *ratioPtr
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;
    GT_U32      value=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ratioPtr);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.ingrStatisticMirrToAnalyzerPortConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trapSniffed;
    }

    /* writing <IngressStatMirroringToAnalyzerPortRatio> field */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 11, &value);
    if(rc != GT_OK)
        return rc;

    *ratioPtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Rx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*
* OUTPUTS:
*      ratioPtr - (pointer to) Indicates the ratio of egress mirrored to analyzer port packets
*                 forwarded to the analyzer port. 1 of every 'ratio' packets are
*                 forwarded to the analyzer port.
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirroringToAnalyzerRatioGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *ratioPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrRxStatMirroringToAnalyzerRatioGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ratioPtr));

    rc = internal_cpssDxChMirrRxStatMirroringToAnalyzerRatioGet(devNum, ratioPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ratioPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrRxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Ingress Statistical Mirroring to the Ingress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*      enable - enable\disable Ingress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrRxStatMirrorToAnalyzerEnable
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
)
{
    GT_U32      regAddr;
    GT_U32      regData;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.ingrStatisticMirrToAnalyzerPortConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trapSniffed;
    }


    /* enable/disable IngressStatMirroringToAnalyzerPortEn
       Ingress Statistic Mirroring to Analyzer Port Configuration Register */
    regData = (enable == 1) ? 1 : 0;

    /* writing <IngressStatMirroringToAnalyzerPortEn> field */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 11, 1, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Ingress Statistical Mirroring to the Ingress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*      enable - enable\disable Ingress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirrorToAnalyzerEnable
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrRxStatMirrorToAnalyzerEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChMirrRxStatMirrorToAnalyzerEnable(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrRxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get Ingress Statistical Mirroring to the Ingress Analyzer Port Mode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      enablePtr - (pointer to) Ingress Statistical Mirroring mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrRxStatMirrorToAnalyzerEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;
    GT_U32      regData;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.ingrStatisticMirrToAnalyzerPortConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.trapSniffed;
    }

    /* getting <IngressStatMirroringToAnalyzerPortEn> field */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 11, 1, &regData);
    if(rc != GT_OK)
        return rc;

    /* enable/disable IngressStatMirroringToAnalyzerPortEn
       Ingress Statistic Mirroring to Analyzer Port Configuration Register */
    *enablePtr = ((regData == 1) ? GT_TRUE : GT_FALSE);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get Ingress Statistical Mirroring to the Ingress Analyzer Port Mode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      enablePtr - (pointer to) Ingress Statistical Mirroring mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxStatMirrorToAnalyzerEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrRxStatMirrorToAnalyzerEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChMirrRxStatMirrorToAnalyzerEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrRxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum       - device number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrRxAnalyzerDpTcSet
(
    IN GT_U8             devNum,
    IN CPSS_DP_LEVEL_ENT analyzerDp,
    IN GT_U8             analyzerTc
)
{
    GT_U32      regData;
    GT_U32      regAddr;
    GT_U32      dp;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, analyzerDp, dp);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(analyzerTc);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.ingrAndEgrMonitoringToAnalyzerQoSConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffQosCfg;
    }

    /* setting data */
    regData = (dp | (analyzerTc << 2));

    /* writing data to register */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 5, 5, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum       - device number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxAnalyzerDpTcSet
(
    IN GT_U8             devNum,
    IN CPSS_DP_LEVEL_ENT analyzerDp,
    IN GT_U8             analyzerTc
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrRxAnalyzerDpTcSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerDp, analyzerTc));

    rc = internal_cpssDxChMirrRxAnalyzerDpTcSet(devNum, analyzerDp, analyzerTc);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerDp, analyzerTc));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrRxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum          - device number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_BAD_PTR               - wrong pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrRxAnalyzerDpTcGet
(
    IN GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT *analyzerDpPtr,
    OUT GT_U8             *analyzerTcPtr
)
{
    GT_U32      regData;
    GT_U32      regAddr;
    GT_U32      dp;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(analyzerDpPtr);
    CPSS_NULL_PTR_CHECK_MAC(analyzerTcPtr);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.ingrAndEgrMonitoringToAnalyzerQoSConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffQosCfg;
    }

    /* reading data from register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 5, 5, &regData);

    /* setting data */
    dp = (regData & 0x3);

    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, dp, (*analyzerDpPtr));

    *analyzerTcPtr = (GT_U8)((regData >> 2 ) & 0x7);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrRxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum          - device number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_BAD_PTR               - wrong pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrRxAnalyzerDpTcGet
(
    IN GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT *analyzerDpPtr,
    OUT GT_U8             *analyzerTcPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrRxAnalyzerDpTcGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerDpPtr, analyzerTcPtr));

    rc = internal_cpssDxChMirrRxAnalyzerDpTcGet(devNum, analyzerDpPtr, analyzerTcPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerDpPtr, analyzerTcPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrTxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to egress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum       - device number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port (APPLICABLE RANGES: 0..7)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrTxAnalyzerDpTcSet
(
    IN GT_U8              devNum,
    IN CPSS_DP_LEVEL_ENT  analyzerDp,
    IN GT_U8              analyzerTc
)
{
    GT_U32      regData;
    GT_U32      regAddr;
    GT_U32      dp;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, analyzerDp, dp);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(analyzerTc);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.ingrAndEgrMonitoringToAnalyzerQoSConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffQosCfg;
    }

    regData = (dp | (analyzerTc << 2));

    /* writing data to register */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 5, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to egress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum       - device number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port (APPLICABLE RANGES: 0..7)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxAnalyzerDpTcSet
(
    IN GT_U8              devNum,
    IN CPSS_DP_LEVEL_ENT  analyzerDp,
    IN GT_U8              analyzerTc
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrTxAnalyzerDpTcSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerDp, analyzerTc));

    rc = internal_cpssDxChMirrTxAnalyzerDpTcSet(devNum, analyzerDp, analyzerTc);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerDp, analyzerTc));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrTxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to egress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum          - device number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_BAD_PTR               - wrong pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrTxAnalyzerDpTcGet
(
    IN GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT *analyzerDpPtr,
    OUT GT_U8             *analyzerTcPtr
)
{
    GT_U32      regData;
    GT_U32      regAddr;
    GT_U32      dp;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(analyzerDpPtr);
    CPSS_NULL_PTR_CHECK_MAC(analyzerTcPtr);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.ingrAndEgrMonitoringToAnalyzerQoSConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.sniffQosCfg;
    }

    /* reading data from register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 5, &regData);

    /* setting data */
    dp = (regData & 0x3);

    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, dp, (*analyzerDpPtr));

    *analyzerTcPtr = (GT_U8)((regData >> 2 ) & 0x7);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to egress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum          - device number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_BAD_PTR               - wrong pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxAnalyzerDpTcGet
(
    IN GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT *analyzerDpPtr,
    OUT GT_U8             *analyzerTcPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrTxAnalyzerDpTcGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, analyzerDpPtr, analyzerTcPtr));

    rc = internal_cpssDxChMirrTxAnalyzerDpTcGet(devNum, analyzerDpPtr, analyzerTcPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, analyzerDpPtr, analyzerTcPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrTxStatMirroringToAnalyzerRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Tx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device or ratio.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrTxStatMirroringToAnalyzerRatioSet
(
    IN GT_U8    devNum,
    IN GT_U32   ratio
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if(ratio > 2047)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCStatisticalTxSniffConfig;
    }
    /* getting register address */
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
    }

    /* writing <EgressStatMirroringToAnalyzerPortRatio> field */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 5, 11, ratio);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirroringToAnalyzerRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Tx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device or ratio.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirroringToAnalyzerRatioSet
(
    IN GT_U8    devNum,
    IN GT_U32   ratio
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrTxStatMirroringToAnalyzerRatioSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ratio));

    rc = internal_cpssDxChMirrTxStatMirroringToAnalyzerRatioSet(devNum, ratio);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ratio));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChMirrTxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Tx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*
* OUTPUTS:
*      ratioPtr - (pointer to)Indicates the ratio of egress mirrored to analyzer
*              port packets forwarded to the analyzer port. 1 of every 'ratio'
*              packets are forwarded to the analyzer port.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrTxStatMirroringToAnalyzerRatioGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *ratioPtr
)
{
    GT_U32      regAddr;
    GT_STATUS   rc;
    GT_U32      value=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ratioPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCStatisticalTxSniffConfig;
    }
    /* getting register address */
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
    }

    /* getting <EgressStatMirroringToAnalyzerPortRatio> field */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 5, 11, &value);
    if(rc != GT_OK)
        return rc;

    *ratioPtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Tx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum   - device number.
*
* OUTPUTS:
*      ratioPtr - (pointer to)Indicates the ratio of egress mirrored to analyzer
*              port packets forwarded to the analyzer port. 1 of every 'ratio'
*              packets are forwarded to the analyzer port.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirroringToAnalyzerRatioGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *ratioPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrTxStatMirroringToAnalyzerRatioGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ratioPtr));

    rc = internal_cpssDxChMirrTxStatMirroringToAnalyzerRatioGet(devNum, ratioPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ratioPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrTxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*      enable - enable\disable Egress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrTxStatMirrorToAnalyzerEnable
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
)
{
    GT_U32      regAddr;
    GT_U32      regData;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCStatisticalTxSniffConfig;
    }
    /* getting register address */
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
    }


    regData = (enable == GT_TRUE) ? 1 : 0;

    /* writing <EgressStatMirrorEn> field */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 4, 1, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number.
*      enable - enable\disable Egress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirrorToAnalyzerEnable
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrTxStatMirrorToAnalyzerEnable);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChMirrTxStatMirrorToAnalyzerEnable(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrTxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get mode of Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number
*
* OUTPUTS:
*      enablePtr - (pointer to)  Egress Statistical Mirroring.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrTxStatMirrorToAnalyzerEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;
    GT_U32      regData;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(devNum).statisticalAndCPUTrigEgrMirrToAnalyzerPort.STCStatisticalTxSniffConfig;
    }
    /* getting register address */
    else if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.trStatSniffAndStcReg;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.
            statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort.stcStatisticalTxSniffConfig;
    }

    /* writing <EgressStatMirrorEn> field */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 4, 1, &regData);
    if(rc != GT_OK)
        return rc;

    *enablePtr = ((regData != 0) ? GT_TRUE : GT_FALSE);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get mode of Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*      devNum    - device number
*
* OUTPUTS:
*      enablePtr - (pointer to)  Egress Statistical Mirroring.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrTxStatMirrorToAnalyzerEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrTxStatMirrorToAnalyzerEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChMirrTxStatMirrorToAnalyzerEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorToAnalyzerForwardingModeSet
*
* DESCRIPTION:
*       Set Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      mode   - mode of forwarding To Analyzer packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change  Forwarding mode, applicaton should
*       disable Rx/Tx mirrorred ports.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorToAnalyzerForwardingModeSet
(
    IN GT_U8     devNum,
    IN CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   mode
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;         /* register address */
    GT_U32      regData;         /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch(mode)
        {
            case CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_END_TO_END_E:
                regData = 0;
                break;
            case CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E:
                regData = 1;
                break;
            case CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E:
                regData = 2;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.analyzerPortGlobalConfig;
    }
    else
    {
        switch(mode)
        {
            case CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E:
                regData = 3;
                break;
            case CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E:
                regData = 0;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 2, regData);
    if(rc != GT_OK)
        return rc;

    if(mode != CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_HOP_BY_HOP_E)
    {
        regData = 1;
    }
    else
    {
        regData = 0;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* no need to set values in the EGF */

    }
    else
    if((PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_LION2_E)&&
       (PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum) != 0))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.egr.filterConfig.globalEnables;

        /* Enable / Disable End to End Sniffer. */
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 9, 1, regData);
        if(rc != GT_OK)
            return rc;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            txqVer1.dist.global.distGlobalControlReg;

        rc = prvCpssHwPpSetRegField(devNum, regAddr, 10, 1, regData);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChMirrorToAnalyzerForwardingModeSet
*
* DESCRIPTION:
*       Set Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      mode   - mode of forwarding To Analyzer packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change  Forwarding mode, applicaton should
*       disable Rx/Tx mirrorred ports.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorToAnalyzerForwardingModeSet
(
    IN GT_U8     devNum,
    IN CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorToAnalyzerForwardingModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChMirrorToAnalyzerForwardingModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorToAnalyzerForwardingModeGet
*
* DESCRIPTION:
*       Get Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      modePtr   - pointer to mode of forwarding To Analyzer packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorToAnalyzerForwardingModeGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   *modePtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    return prvCpssDxChMirrorToAnalyzerForwardingModeGet(devNum, modePtr);
}

/*******************************************************************************
* cpssDxChMirrorToAnalyzerForwardingModeGet
*
* DESCRIPTION:
*       Get Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      modePtr   - pointer to mode of forwarding To Analyzer packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorToAnalyzerForwardingModeGet
(
    IN  GT_U8     devNum,
    OUT CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorToAnalyzerForwardingModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChMirrorToAnalyzerForwardingModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorAnalyzerInterfaceSet
*
* DESCRIPTION:
*       This function sets analyzer interface.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (APPLICABLE RANGES: 0..6)
*      interfacePtr   - Pointer to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index, interface type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on wrong port or device number in interfacePtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorAnalyzerInterfaceSet
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;         /* register address */
    GT_U32      regData = 0;         /* register data */
    GT_U32      hwDev, hwPort;
    GT_U32      dataLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    CPSS_NULL_PTR_CHECK_MAC(interfacePtr);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(interfacePtr->interface.type != CPSS_INTERFACE_PORT_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        hwDev = interfacePtr->interface.devPort.hwDevNum;
        hwPort = interfacePtr->interface.devPort.portNum;

        if(hwPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum) ||
           hwDev  > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        regData = (hwDev | hwPort << 10);

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            dataLength = 25;
        }
        else
        {
            dataLength = 23;
        }

        /* getting register address */
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                mirrToAnalyzerPortConfigs.mirrorInterfaceParameterReg[index];

        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, dataLength, regData);

    }
    else
    {
        PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
            interfacePtr->interface.devPort.hwDevNum,
            interfacePtr->interface.devPort.portNum);
        hwDev =  PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(interfacePtr->interface.devPort.hwDevNum,
                                                     interfacePtr->interface.devPort.portNum);
        hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(interfacePtr->interface.devPort.hwDevNum,
                                                      interfacePtr->interface.devPort.portNum);

        if((hwDev > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum)) ||
           (hwPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* set 1 bit for MonitorType (value = 0 incase of portType)
           5 bits for devNum and 13 bits for portNum */
        U32_SET_FIELD_IN_ENTRY_MAC(&regData,0,2,0);
        U32_SET_FIELD_IN_ENTRY_MAC(&regData,2,5,hwDev);
        U32_SET_FIELD_IN_ENTRY_MAC(&regData,7,13,hwPort);

        /* getting register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[index];
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 20, regData);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerInterfaceSet
*
* DESCRIPTION:
*       This function sets analyzer interface.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (APPLICABLE RANGES: 0..6)
*      interfacePtr   - Pointer to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index, interface type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on wrong port or device number in interfacePtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerInterfaceSet
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorAnalyzerInterfaceSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, interfacePtr));

    rc = internal_cpssDxChMirrorAnalyzerInterfaceSet(devNum, index, interfacePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, interfacePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorAnalyzerInterfaceGet
*
* DESCRIPTION:
*       This function gets analyzer interface.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*      interfacePtr   - Pointer to analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorAnalyzerInterfaceGet
(
    IN  GT_U8     devNum,
    IN GT_U32     index,
    OUT CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regData;    /* register data */
    GT_STATUS   rc;         /* return status */
    GT_U32      dataLength;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(interfacePtr);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* getting register address */
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                mirrToAnalyzerPortConfigs.mirrorInterfaceParameterReg[index];

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            dataLength = 25;
        }
        else
        {
            dataLength = 23;
        }

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, dataLength, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        interfacePtr->interface.type = CPSS_INTERFACE_PORT_E;
        interfacePtr->interface.devPort.hwDevNum = regData & 0x3FF;
        interfacePtr->interface.devPort.portNum = regData >> 10;
    }
    else
    {
        /* getting register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[index];

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 20, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(0 != U32_GET_FIELD_MAC(regData,0,2))/*monitorType*/
        {
            /* this monitorType is not supported */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        interfacePtr->interface.type = CPSS_INTERFACE_PORT_E;
        interfacePtr->interface.devPort.hwDevNum  = U32_GET_FIELD_MAC(regData,2,5);
        interfacePtr->interface.devPort.portNum = U32_GET_FIELD_MAC(regData,7,13);

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerInterfaceGet
*
* DESCRIPTION:
*       This function gets analyzer interface.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*      interfacePtr   - Pointer to analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerInterfaceGet
(
    IN  GT_U8     devNum,
    IN GT_U32     index,
    OUT CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   *interfacePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorAnalyzerInterfaceGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, interfacePtr));

    rc = internal_cpssDxChMirrorAnalyzerInterfaceGet(devNum, index, interfacePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, interfacePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for ingress
*       mirroring from all engines except
*       port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode, Policy-Based,
*                  VLAN-Based, FDB-Based, Router-Based.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regData;     /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.analyzerPortGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;
    }

    /* 0 is used for no mirroring */
    regData = (enable == GT_TRUE) ? (index + 1) : 0;

    return prvCpssHwPpSetRegField(devNum, regAddr, 2, 3, regData);
}

/*******************************************************************************
* cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for ingress
*       mirroring from all engines except
*       port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode, Policy-Based,
*                  VLAN-Based, FDB-Based, Router-Based.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable, index));

    rc = internal_cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet(devNum, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for ingress mirroring
*       from all engines except port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode, Policy-Based,
*                     VLAN-Based, FDB-Based, Router-Based.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
)
{
    GT_U32      regAddr;       /* register address */
    GT_U32      regData;       /* register data */
    GT_STATUS   rc;            /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.analyzerPortGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 2, 3, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* No mirroring */
    if(regData == 0)
    {
        *enablePtr = GT_FALSE;
    }
    /* mirroring is enabled */
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = regData - 1;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for ingress mirroring
*       from all engines except port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode, Policy-Based,
*                     VLAN-Based, FDB-Based, Router-Based.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr, indexPtr));

    rc = internal_cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet(devNum, enablePtr, indexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr, indexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regData;    /* register data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.analyzerPortGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;
    }

    /* 0 is used for no mirroring */
    regData = (enable == GT_TRUE) ? (index + 1) : 0;

    return prvCpssHwPpSetRegField(devNum, regAddr, 5, 3, regData);
}

/*******************************************************************************
* cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   enable,
    IN GT_U32    index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable, index));

    rc = internal_cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(devNum, enable, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
)
{
    GT_U32      regAddr;      /* register address */
    GT_U32      regData;      /* register data */
    GT_STATUS   rc;           /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    /* getting register address */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            mirrToAnalyzerPortConfigs.analyzerPortGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.analyzerPortGlobalConfig;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 5, 3, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* No mirroring */
    if(regData == 0)
    {
        *enablePtr = GT_FALSE;
    }
    /* mirroring is enabled */
    else
    {
        *enablePtr = GT_TRUE;
        *indexPtr = regData - 1;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr,
    OUT GT_U32    *indexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr, indexPtr));

    rc = internal_cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet(devNum, enablePtr, indexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr, indexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorAnalyzerMirrorOnDropEnableSet
*
* DESCRIPTION:
*       Enable / Disable mirroring of dropped packets.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*      enable - GT_TRUE  - dropped packets are mirrored to analyzer interface.
*               GT_FALSE - dropped packets are not mirrored to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorAnalyzerMirrorOnDropEnableSet
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN GT_BOOL   enable
)
{
    GT_U32      regAddr;         /* register address */
    GT_U32      regData;         /* register data */
    GT_U32      fieldOffset;     /* register field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regData = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* getting register address */
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                mirrToAnalyzerPortConfigs.mirrorInterfaceParameterReg[index];

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            fieldOffset = 25;
        }
        else
        {
            fieldOffset = 23;
        }

        return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, regData);
    }
    else
    {
        /* getting register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[index];

        return prvCpssHwPpSetRegField(devNum, regAddr, 20, 1, regData);
    }

}

/*******************************************************************************
* cpssDxChMirrorAnalyzerMirrorOnDropEnableSet
*
* DESCRIPTION:
*       Enable / Disable mirroring of dropped packets.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*      enable - GT_TRUE  - dropped packets are mirrored to analyzer interface.
*               GT_FALSE - dropped packets are not mirrored to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerMirrorOnDropEnableSet
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN GT_BOOL   enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorAnalyzerMirrorOnDropEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, enable));

    rc = internal_cpssDxChMirrorAnalyzerMirrorOnDropEnableSet(devNum, index, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorAnalyzerMirrorOnDropEnableGet
*
* DESCRIPTION:
*       Get mirroring status of dropped packets.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*      enablePtr - Pointer to mirroring status of dropped packets.
*                  - GT_TRUE  - dropped packets are mirrored to
*                               analyzer interface.
*                  - GT_FALSE - dropped packets are not mirrored to
*                               analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorAnalyzerMirrorOnDropEnableGet
(
    IN  GT_U8     devNum,
    IN GT_U32     index,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32      regAddr;      /* register address */
    GT_U32      regData;      /* register data */
    GT_STATUS   rc;           /* return code */
    GT_U32      fieldOffset;  /* register field offset */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(index >  PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* getting register address */
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                mirrToAnalyzerPortConfigs.mirrorInterfaceParameterReg[index];

        if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            fieldOffset = 25;
        }
        else
        {
            fieldOffset = 23;
        }

        rc = prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &regData);
    }
    else
    {
        /* getting register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.mirrorInterfaceParameterReg[index];

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 20, 1, &regData);
    }

    *enablePtr = (regData == 1) ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerMirrorOnDropEnableGet
*
* DESCRIPTION:
*       Get mirroring status of dropped packets.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*      enablePtr - Pointer to mirroring status of dropped packets.
*                  - GT_TRUE  - dropped packets are mirrored to
*                               analyzer interface.
*                  - GT_FALSE - dropped packets are not mirrored to
*                               analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerMirrorOnDropEnableGet
(
    IN  GT_U8     devNum,
    IN GT_U32     index,
    OUT GT_BOOL   *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorAnalyzerMirrorOnDropEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, enablePtr));

    rc = internal_cpssDxChMirrorAnalyzerMirrorOnDropEnableGet(devNum, index, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                     GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_U32  regAddr;            /* register address     */
    GT_U32  data;               /* reg subfield data    */
    GT_U32  portGroupId;        /* the port group Id - support multi-port-groups device */
    GT_U32  fieldOffset;        /* register field offset */
    GT_U32   localPort;          /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    data = (enable == GT_TRUE) ? 0 : 1;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                               portNum,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                               LION3_HA_EPORT_TABLE_2_MIRROR_TO_ANALYZER_KEEP_TAGS_E, /* field name */
                                               PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                               data);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        fieldOffset = OFFSET_TO_BIT_MAC(localPort);

        /* getting register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
            mirrorToAnalyzerHeaderConfReg[OFFSET_TO_WORD_MAC(localPort)];

        /* Enable/Disable VLAN tag removal of mirrored traffic. */
        return  prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                    fieldOffset, 1, data);
    }
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                     GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
*
* DESCRIPTION:
*       Get status of enabling/disabling VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - pointer to status of VLAN tag removal of mirrored traffic.
*                  - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                    GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      regAddr;      /* register address */
    GT_U32      regData;      /* register data */
    GT_U32      portGroupId;  /* the port group Id - support multi-port-groups device */
    GT_U32      fieldOffset;  /* register field offset */
    GT_STATUS   rc;           /* return code */
    GT_U32       localPort;    /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E);
    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                                               PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
                                               portNum,
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                               LION3_HA_EPORT_TABLE_2_MIRROR_TO_ANALYZER_KEEP_TAGS_E,/* field name */
                                               PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                               &regData);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        fieldOffset = OFFSET_TO_BIT_MAC(localPort);

            /* getting register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
            mirrorToAnalyzerHeaderConfReg[OFFSET_TO_WORD_MAC(localPort)];

        /* Get status of enabling/disabling VLAN tag removal of mirrored traffic. */
        rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                    fieldOffset, 1, &regData);
    }
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (regData == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
*
* DESCRIPTION:
*       Get status of enabling/disabling VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - pointer to status of VLAN tag removal of mirrored traffic.
*                  - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                    GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorEnhancedMirroringPriorityModeSet
*
* DESCRIPTION:
*       Setting Enhanced Mirroring Priority mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       mode    - Enhanced Mirroring Priority selected working mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorEnhancedMirroringPriorityModeSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT  mode
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regData;    /* register data    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }

    switch(mode)
    {
        case CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_TDM_E: regData = 0x0;
                                                       break;
        case CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_SP_INGRESS_EGRESS_MIRROR_E:
                                                       regData = 0xE;
                                                       break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;

    /* write bits [19:16] in Pre-Egress Engine Global Configuration Register */
    return prvCpssHwPpSetRegField(devNum, regAddr, 16, 4, regData);
}

/*******************************************************************************
* cpssDxChMirrorEnhancedMirroringPriorityModeSet
*
* DESCRIPTION:
*       Setting Enhanced Mirroring Priority mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       mode    - Enhanced Mirroring Priority selected working mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorEnhancedMirroringPriorityModeSet
(
    IN GT_U8                                        devNum,
    IN CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT  mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorEnhancedMirroringPriorityModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChMirrorEnhancedMirroringPriorityModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorEnhancedMirroringPriorityModeGet
*
* DESCRIPTION:
*       Getting Enhanced Mirroring Priority mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       modePtr    - (pointer to) Enhanced Mirroring Priority selected working mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorEnhancedMirroringPriorityModeGet
(
    IN GT_U8                                         devNum,
    OUT CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT  *modePtr
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address */
    GT_U32      regData;    /* register data */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        PRV_CPSS_DXCH_XCAT_A3_ONLY_DEV_CHECK_MAC(devNum);
    }

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;

    /* read bits [19:16] in Pre-Egress Engine Global Configuration Register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 16, 4, &regData);
    if(GT_OK != rc)
    {
        return rc;
    }

    switch(regData)
    {
        case 0x0: *modePtr = CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_TDM_E;
                  break;
        case 0xE: *modePtr = CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_SP_INGRESS_EGRESS_MIRROR_E;
                  break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChMirrorEnhancedMirroringPriorityModeGet
*
* DESCRIPTION:
*       Getting Enhanced Mirroring Priority mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       modePtr    - (pointer to) Enhanced Mirroring Priority selected working mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorEnhancedMirroringPriorityModeGet
(
    IN GT_U8                                         devNum,
    OUT CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT  *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorEnhancedMirroringPriorityModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChMirrorEnhancedMirroringPriorityModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet
*
* DESCRIPTION:
*       Set the drop code to be used for Hop-By-Hop mirroring mode, when a packet
*       is duplicated to a target analyzer, and the global Ingress Analyzer Index
*       or Egress Analyzer Index is DISABLED.
*       In this case the packet is hard dropped with this drop code.
*
*       NOTE: not relevant to the device that generates the duplication but only
*       to device that will get 'TO_ANALYZER' DSA tag (in HOP-BY_HOP mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       dropCode  - the drop code. (values like 'cpu code')
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device , bad dropCode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT    dropCode
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode;
    GT_U32      regAddr;        /* register address */
    GT_U32  regVal;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    /* convert dropCode to dsaCpuCode */
    rc = prvCpssDxChNetIfCpuToDsaCode(dropCode, &dsaCpuCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    regVal = dsaCpuCode;

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
        mirrToAnalyzerPortConfigs.analyzerPortGlobalConfig;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 8, 8, regVal);
    return rc;
}

/*******************************************************************************
* cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet
*
* DESCRIPTION:
*       Set the drop code to be used for Hop-By-Hop mirroring mode, when a packet
*       is duplicated to a target analyzer, and the global Ingress Analyzer Index
*       or Egress Analyzer Index is DISABLED.
*       In this case the packet is hard dropped with this drop code.
*
*       NOTE: not relevant to the device that generates the duplication but only
*       to device that will get 'TO_ANALYZER' DSA tag (in HOP-BY_HOP mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       dropCode  - the drop code. (values like 'cpu code')
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device , bad dropCode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT    dropCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropCode));

    rc = internal_cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet(devNum, dropCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet
*
* DESCRIPTION:
*       Get the drop code to be used for Hop-By-Hop mirroring mode, when a packet
*       is duplicated to a target analyzer, and the global Ingress Analyzer Index
*       or Egress Analyzer Index is DISABLED.
*       In this case the packet is hard dropped with this drop code.
*
*       NOTE: not relevant to the device that generates the duplication but only
*       to device that will get 'TO_ANALYZER' DSA tag (in HOP-BY_HOP mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       dropCodePtr  - (pointer to) the drop code. (values like 'cpu code')
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *dropCodePtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT  dsaCpuCode;
    GT_U32      regAddr;        /* register address */
    GT_U32  regVal;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(dropCodePtr);

    regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
        mirrToAnalyzerPortConfigs.analyzerPortGlobalConfig;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 8, 8, &regVal);
    if (rc != GT_OK)
    {
        return rc;
    }

    dsaCpuCode = regVal;

    /* convert dsaCpuCode to dropCode */
    rc = prvCpssDxChNetIfDsaToCpuCode(dsaCpuCode,dropCodePtr);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet
*
* DESCRIPTION:
*       Get the drop code to be used for Hop-By-Hop mirroring mode, when a packet
*       is duplicated to a target analyzer, and the global Ingress Analyzer Index
*       or Egress Analyzer Index is DISABLED.
*       In this case the packet is hard dropped with this drop code.
*
*       NOTE: not relevant to the device that generates the duplication but only
*       to device that will get 'TO_ANALYZER' DSA tag (in HOP-BY_HOP mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       dropCodePtr  - (pointer to) the drop code. (values like 'cpu code')
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *dropCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropCodePtr));

    rc = internal_cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet(devNum, dropCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChMirrorTxPortVlanEnableSet
*
* DESCRIPTION:
*       Enable or disable Tx vlan mirroring from specific physical port.
*       (define if the physical port allow/deny egress vlan mirroring)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - physical port number.
*       enable    - enable/disable Tx vlan mirroring from this physical port
*                   GT_TRUE  - Tx vlan mirroring enabled  for this physical port.
*                   GT_FALSE - Tx vlan mirroring disabled for this physical port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device , bad portNum.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxPortVlanEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChWriteTableEntryField(devNum,
                    PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E,
                    portNum,
                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                    SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_ETARGET_PHYSICAL_PORT_EVLAN_MIRRORING_ENABLE_E, /* field name */
                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                    BOOL2BIT_MAC(enable));
    }
    else
    {
        regAddr = PRV_DXCH_REG1_UNIT_EGF_QAG_MAC(devNum).
            distributor.evlanMirrEnable[OFFSET_TO_WORD_MAC(portNum)];

        rc = prvCpssHwPpSetRegField(devNum, regAddr, OFFSET_TO_BIT_MAC(portNum), 1, BOOL2BIT_MAC(enable));
    }

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxPortVlanEnableSet
*
* DESCRIPTION:
*       Enable or disable Tx vlan mirroring from specific physical port.
*       (define if the physical port allow/deny egress vlan mirroring)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - physical port number.
*       enable    - enable/disable Tx vlan mirroring from this physical port
*                   GT_TRUE  - Tx vlan mirroring enabled  for this physical port.
*                   GT_FALSE - Tx vlan mirroring disabled for this physical port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device , bad portNum.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortVlanEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxPortVlanEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChMirrorTxPortVlanEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMirrorTxPortVlanEnableGet
*
* DESCRIPTION:
*       Get is the Tx vlan mirroring from specific physical port enabled/disabled.
*       (define if the physical port allow/deny egress vlan mirroring)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - physical port number.
*
* OUTPUTS:
*       enablePtr - (pointer to)enable/disable Tx vlan mirroring from this physical port
*                   GT_TRUE  - Tx vlan mirroring enabled  for this physical port.
*                   GT_FALSE - Tx vlan mirroring disabled for this physical port.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChMirrorTxPortVlanEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;        /* register address */
    GT_U32      regValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        rc = prvCpssDxChReadTableEntryField(devNum,
                    PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E,
                    portNum,
                    PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                    SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_ETARGET_PHYSICAL_PORT_EVLAN_MIRRORING_ENABLE_E, /* field name */
                    PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                    &regValue);
    }
    else
    {
        regAddr = PRV_DXCH_REG1_UNIT_EGF_QAG_MAC(devNum).
            distributor.evlanMirrEnable[OFFSET_TO_WORD_MAC(portNum)];

        rc = prvCpssHwPpGetRegField(devNum, regAddr, OFFSET_TO_BIT_MAC(portNum), 1, &regValue);
    }

    *enablePtr = BIT2BOOL_MAC(regValue);

    return rc;
}

/*******************************************************************************
* cpssDxChMirrorTxPortVlanEnableGet
*
* DESCRIPTION:
*       Get is the Tx vlan mirroring from specific physical port enabled/disabled.
*       (define if the physical port allow/deny egress vlan mirroring)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - physical port number.
*
* OUTPUTS:
*       enablePtr - (pointer to)enable/disable Tx vlan mirroring from this physical port
*                   GT_TRUE  - Tx vlan mirroring enabled  for this physical port.
*                   GT_FALSE - Tx vlan mirroring disabled for this physical port.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMirrorTxPortVlanEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMirrorTxPortVlanEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChMirrorTxPortVlanEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}




