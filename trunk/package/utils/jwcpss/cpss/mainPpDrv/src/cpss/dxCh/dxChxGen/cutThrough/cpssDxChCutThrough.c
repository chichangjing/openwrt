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
* cpssDxChCutThrough.c
*
* DESCRIPTION:
*     CPSS DXCH Cut Through facility facility implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 28$
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/cutThrough/cpssDxChCutThrough.h>
#include <cpss/dxCh/dxChxGen/cutThrough/private/prvCpssDxChCutThroughLog.h>

/*******************************************************************************
* internal_cpssDxChCutThroughPortEnableSet
*
* DESCRIPTION:
*      Enable/Disable Cut Through forwarding for packets received on the port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable the port for Cut Through.
*                  GT_FALSE: Disable the port for Cut Through.
*       untaggedEnable -  GT_TRUE:  Enable Cut Through forwarding for
*                                   untagged packets received on the port.
*                         GT_FALSE: Disable Cut Through forwarding for
*                                    untagged packets received on the port.
*                         Used only if enable == GT_TRUE.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       Cut Through forwarding for tagged packets is enabled
*       per source port and UP. See cpssDxChCutThroughUpEnableSet.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughPortEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL  enable,
    IN GT_BOOL  untaggedEnable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    fieldOffset1;       /* The start bit number in the register - for enable*/
    GT_U32    fieldOffset2;       /* The start bit number in the register - for untaggedEnable*/
    GT_U32    data;              /* reg sub field data */
    GT_U32    mask;              /* Mask for selecting the written bits */
    GT_U32    portGroupId;       /* the port group Id - support multi port group device */
    GT_U32     localPort;         /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* each RXDMA port hold it's own registers , so bit index is not parameter of port number */
        fieldOffset1 = 7; /*enable*/
        fieldOffset2 = 8; /*untaggedEnable*/

        regAddr = PRV_DXCH_REG1_UNIT_RXDMA_MAC(devNum).
            singleChannelDMAConfigs.SCDMAConfig0[localPort];
    }
    else
    {
        if(CPSS_CPU_PORT_NUM_CNS == localPort)
        {
            fieldOffset1 = 15;    /*enable*/
        }
        else
        {
            fieldOffset1 = localPort;   /*enable*/
        }

        fieldOffset2 = fieldOffset1 + 16; /*untaggedEnable*/

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                           cutThroughRegs.ctEnablePerPortReg;
    }

    data = (enable == GT_TRUE) ? (1 << fieldOffset1) : 0;
    mask = 1 << fieldOffset1;

    /* Set Cut Through forwarding data for untagged packets received on the port */
    data |= (untaggedEnable == GT_TRUE) ? (1 << fieldOffset2) : 0;
    mask |= 1 << fieldOffset2;

    /* Enable/Disable Cut Through forwarding */
    return prvCpssHwPpPortGroupWriteRegBitMask(devNum, portGroupId,
                                             regAddr, mask, data);
}

/*******************************************************************************
* cpssDxChCutThroughPortEnableSet
*
* DESCRIPTION:
*      Enable/Disable Cut Through forwarding for packets received on the port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable the port for Cut Through.
*                  GT_FALSE: Disable the port for Cut Through.
*       untaggedEnable -  GT_TRUE:  Enable Cut Through forwarding for
*                                   untagged packets received on the port.
*                         GT_FALSE: Disable Cut Through forwarding for
*                                    untagged packets received on the port.
*                         Used only if enable == GT_TRUE.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       Cut Through forwarding for tagged packets is enabled
*       per source port and UP. See cpssDxChCutThroughUpEnableSet.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL  enable,
    IN GT_BOOL  untaggedEnable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughPortEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable, untaggedEnable));

    rc = internal_cpssDxChCutThroughPortEnableSet(devNum, portNum, enable, untaggedEnable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable, untaggedEnable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughPortEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode on the specified port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status:
*                    - GT_TRUE:  Enable the port for Cut Through.
*                      GT_FALSE: Disable the port for Cut Through.
*       untaggedEnablePtr - pointer to Cut Through forwarding mode status
*                            for untagged packets.
*                            Used only if *enablePtr == GT_TRUE.
*                            GT_TRUE:  Enable Cut Through forwarding for
*                                      untagged packets received on the port.
*                            GT_FALSE: Disable Cut Through forwarding for
*                                      untagged packets received on the port.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughPortEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_BOOL  *untaggedEnablePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    fieldOffset1;       /* The start bit number in the register - for enable*/
    GT_U32    fieldOffset2;       /* The start bit number in the register - for untaggedEnable*/
    GT_U32    data;              /* reg data */
    GT_U32    portGroupId;       /* the port group Id - support multi port group device */
    GT_U32     localPort;         /* local port - support multi-port-groups device */
    GT_STATUS rc;              /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(untaggedEnablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);


    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* each RXDMA port hold it's own registers , so bit index is not parameter of port number */
        fieldOffset1 = 7; /*enable*/
        fieldOffset2 = 8; /*untaggedEnable*/

        regAddr = PRV_DXCH_REG1_UNIT_RXDMA_MAC(devNum).
            singleChannelDMAConfigs.SCDMAConfig0[localPort];
    }
    else
    {
        if(CPSS_CPU_PORT_NUM_CNS == localPort)
        {
            fieldOffset1 = 15;    /*enable*/
        }
        else
        {
            fieldOffset1 = localPort;   /*enable*/
        }

        fieldOffset2 = fieldOffset1 + 16; /*untaggedEnable*/

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                           cutThroughRegs.ctEnablePerPortReg;
    }

    /* Get Enable/Disable Cut Through forwarding status */
    rc = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (((data >> fieldOffset1) & 0x1) == 0) ? GT_FALSE : GT_TRUE;

    /* Get Cut Through forwarding status for untagged packets received on the port */
    *untaggedEnablePtr = (((data >> fieldOffset2)  & 0x1) == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughPortEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode on the specified port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status:
*                    - GT_TRUE:  Enable the port for Cut Through.
*                      GT_FALSE: Disable the port for Cut Through.
*       untaggedEnablePtr - pointer to Cut Through forwarding mode status
*                            for untagged packets.
*                            Used only if *enablePtr == GT_TRUE.
*                            GT_TRUE:  Enable Cut Through forwarding for
*                                      untagged packets received on the port.
*                            GT_FALSE: Disable Cut Through forwarding for
*                                      untagged packets received on the port.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL  *enablePtr,
    OUT GT_BOOL  *untaggedEnablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughPortEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr, untaggedEnablePtr));

    rc = internal_cpssDxChCutThroughPortEnableGet(devNum, portNum, enablePtr, untaggedEnablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr, untaggedEnablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughUpEnableSet
*
* DESCRIPTION:
*       Enable / Disable tagged packets with the specified UP
*       to be Cut Through.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       up              - user priority of a VLAN or DSA tagged
*                         packet (APPLICABLE RANGES: 0..7).
*       enable   - GT_TRUE:  tagged packets, with the specified UP
*                            may be subject to Cut Through forwarding.
*                  GT_FALSE: tagged packets, with the specified UP
*                            aren't subject to Cut Through forwarding.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or up.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The ingress port should be enabled for Cut Through forwarding.
*       Use cpssDxChCutThroughPortEnableSet for it.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughUpEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    up,
    IN GT_BOOL  enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if(up > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    data = (enable == GT_TRUE) ? 1 : 0;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        regAddr = PRV_DXCH_REG1_UNIT_RXDMA_MAC(devNum).globalRxDMAConfigs.globalConfigCutThrough.CTUPEnable;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctUpEnableReg;
    }

    /* Enable / Disable tagged packets, with the specified UP to be Cut Through. */
    return prvCpssHwPpSetRegField(devNum, regAddr, up, 1, data);

}

/*******************************************************************************
* cpssDxChCutThroughUpEnableSet
*
* DESCRIPTION:
*       Enable / Disable tagged packets with the specified UP
*       to be Cut Through.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       up              - user priority of a VLAN or DSA tagged
*                         packet (APPLICABLE RANGES: 0..7).
*       enable   - GT_TRUE:  tagged packets, with the specified UP
*                            may be subject to Cut Through forwarding.
*                  GT_FALSE: tagged packets, with the specified UP
*                            aren't subject to Cut Through forwarding.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or up.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The ingress port should be enabled for Cut Through forwarding.
*       Use cpssDxChCutThroughPortEnableSet for it.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughUpEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    up,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughUpEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, up, enable));

    rc = internal_cpssDxChCutThroughUpEnableSet(devNum, up, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, up, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughUpEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode for tagged packets
*      with the specified UP.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       up              - user priority of a VLAN or DSA tagged
*                         packet (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status
*                     for tagged packets, with the specified UP.:
*                     - GT_TRUE:  tagged packets, with the specified UP
*                                 may be subject to Cut Through forwarding.
*                       GT_FALSE: tagged packets, with the specified UP
*                                 aren't subject to Cut Through forwarding.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or up
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughUpEnableGet
(
    IN  GT_U8    devNum,
    IN GT_U8     up,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    data;        /* reg sub field data */
    GT_STATUS rc;          /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(up > 7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        regAddr = PRV_DXCH_REG1_UNIT_RXDMA_MAC(devNum).globalRxDMAConfigs.globalConfigCutThrough.CTUPEnable;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctUpEnableReg;
    }

    /* Enable / Disable tagged packets, with the specified UP to be Cut Through. */
    rc =  prvCpssHwPpGetRegField(devNum, regAddr, up, 1, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughUpEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode for tagged packets
*      with the specified UP.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       up              - user priority of a VLAN or DSA tagged
*                         packet (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status
*                     for tagged packets, with the specified UP.:
*                     - GT_TRUE:  tagged packets, with the specified UP
*                                 may be subject to Cut Through forwarding.
*                       GT_FALSE: tagged packets, with the specified UP
*                                 aren't subject to Cut Through forwarding.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or up
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughUpEnableGet
(
    IN  GT_U8    devNum,
    IN GT_U8     up,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughUpEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, up, enablePtr));

    rc = internal_cpssDxChCutThroughUpEnableGet(devNum, up, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, up, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughVlanEthertypeSet
*
* DESCRIPTION:
*       Set VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum           - device number.
*       etherType0       - VLAN EtherType0 (APPLICABLE RANGES: 0..0xFFFF)
*       etherType1       - VLAN EtherType1 (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong etherType0 or etherType1
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughVlanEthertypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType0,
    IN GT_U32   etherType1
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if((etherType0 > 0xFFFF) || (etherType1 > 0xFFFF))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    data = etherType0 | etherType1 << 16;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        regAddr = PRV_DXCH_REG1_UNIT_RXDMA_MAC(devNum).globalRxDMAConfigs.globalConfigCutThrough.CTEtherTypeIdentification;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctEthertypeReg;
    }

    /* Set VLAN Ethertype in order to identify tagged packets. */
    return prvCpssHwPpWriteRegister(devNum, regAddr, data);
}

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeSet
*
* DESCRIPTION:
*       Set VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum           - device number.
*       etherType0       - VLAN EtherType0 (APPLICABLE RANGES: 0..0xFFFF)
*       etherType1       - VLAN EtherType1 (APPLICABLE RANGES: 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong etherType0 or etherType1
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughVlanEthertypeSet
(
    IN GT_U8    devNum,
    IN GT_U32   etherType0,
    IN GT_U32   etherType1
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughVlanEthertypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherType0, etherType1));

    rc = internal_cpssDxChCutThroughVlanEthertypeSet(devNum, etherType0, etherType1);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherType0, etherType1));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughVlanEthertypeGet
*
* DESCRIPTION:
*       Get VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum           - device number.
*
* OUTPUTS:
*       etherType0Ptr    - Pointer to VLAN EtherType0
*       etherType1Ptr    - Pointer to VLAN EtherType1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughVlanEthertypeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *etherType0Ptr,
    OUT GT_U32   *etherType1Ptr
)
{
    GT_U32    regAddr;     /* register address   */
    GT_U32    data;        /* reg data */
    GT_STATUS rc;          /* return status      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(etherType0Ptr);
    CPSS_NULL_PTR_CHECK_MAC(etherType1Ptr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        regAddr = PRV_DXCH_REG1_UNIT_RXDMA_MAC(devNum).globalRxDMAConfigs.globalConfigCutThrough.CTEtherTypeIdentification;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctEthertypeReg;
    }

    /* Get VLAN Ethertype in order to identify tagged packets. */
    rc = prvCpssHwPpReadRegister(devNum, regAddr, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *etherType0Ptr = data & 0xFFFF;
    *etherType1Ptr = (data >> 16) & 0xFFFF;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeGet
*
* DESCRIPTION:
*       Get VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum           - device number.
*
* OUTPUTS:
*       etherType0Ptr    - Pointer to VLAN EtherType0
*       etherType1Ptr    - Pointer to VLAN EtherType1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughVlanEthertypeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *etherType0Ptr,
    OUT GT_U32   *etherType1Ptr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughVlanEthertypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, etherType0Ptr, etherType1Ptr));

    rc = internal_cpssDxChCutThroughVlanEthertypeGet(devNum, etherType0Ptr, etherType1Ptr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, etherType0Ptr, etherType1Ptr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughMinimalPacketSizeSet
*
* DESCRIPTION:
*       Set minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       size        - minimal packet size in bytes for Cut Through (APPLICABLE RANGES: 129..16376).
*                     Granularity - 8 bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When using cut-through to 1G port, and bypassing
*       of Router And Ingress Policer engines is disabled,
*       the minimal packet size should be 512 bytes.
*       When bypassing of Router And Ingress Policer engines is enabled,
*       the minimal cut-through packet size should be:
*       - for 10G or faster ports - at least 257 bytes.
*       - for ports slower than 10 G  - at least 513 bytes
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughMinimalPacketSizeSet
(
    IN GT_U8    devNum,
    IN GT_U32   size
)
{
    GT_U32    regAddr;           /* register address   */
    GT_U32    data;              /* reg sub field data */
    GT_U32    hwValue;           /* HW value           */
    GT_U32    hwMask;            /* HW mask            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    if((size < 0x81) || (size > 0x3FF8))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        cutThroughRegs.ctPacketIndentificationReg;

    /* The field is defined in "Number of transactions" in resolution of 8 bytes.
       For example: 17 transactions (default value) means a packet whose
       byte count is 129B up to 136B */
    data = size / 8;
    if((size % 8) != 0)
    {
        data++;
    }

    hwValue = data | (data << 11);
    hwMask  = 0x003FFFFF;

    /* Set minimal packet size that is enabled for Cut Through. */
    return prvCpssHwPpWriteRegBitMask(devNum, regAddr, hwMask, hwValue);
}

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeSet
*
* DESCRIPTION:
*       Set minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       size        - minimal packet size in bytes for Cut Through (APPLICABLE RANGES: 129..16376).
*                     Granularity - 8 bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When using cut-through to 1G port, and bypassing
*       of Router And Ingress Policer engines is disabled,
*       the minimal packet size should be 512 bytes.
*       When bypassing of Router And Ingress Policer engines is enabled,
*       the minimal cut-through packet size should be:
*       - for 10G or faster ports - at least 257 bytes.
*       - for ports slower than 10 G  - at least 513 bytes
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMinimalPacketSizeSet
(
    IN GT_U8    devNum,
    IN GT_U32   size
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughMinimalPacketSizeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, size));

    rc = internal_cpssDxChCutThroughMinimalPacketSizeSet(devNum, size);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, size));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughMinimalPacketSizeGet
*
* DESCRIPTION:
*       Get minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       sizePtr     - pointer to minimal packet size in bytes for Cut Through.
*                     Granularity - 8 bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughMinimalPacketSizeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *sizePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    data;        /* reg sub field data */
    GT_STATUS rc;          /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(sizePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        cutThroughRegs.ctPacketIndentificationReg;

    /* Get minimal packet size that is enabled for Cut Through. */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 11, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *sizePtr = data * 8;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeGet
*
* DESCRIPTION:
*       Get minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       sizePtr     - pointer to minimal packet size in bytes for Cut Through.
*                     Granularity - 8 bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMinimalPacketSizeGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *sizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughMinimalPacketSizeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, sizePtr));

    rc = internal_cpssDxChCutThroughMinimalPacketSizeGet(devNum, sizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, sizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughMemoryRateLimitSet
*
* DESCRIPTION:
*       Set rate limiting of read operations from the memory
*       per target port in Cut Through mode according to the port speed.
*       To prevent congestion in egress pipe, buffer memory read operations
*       are rate limited according to the target port speed.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number including CPU port.
*       enable       - GT_TRUE - rate limiting is enabled.
*                    - GT_FALSE - rate limiting is disabled.
*       portSpeed    - port speed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portNum or portSpeed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Rate limit is recommended be enabled
*       on all egress ports for cut-through traffic.
*       Rate limit to the CPU port should be configured as a 1G port.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughMemoryRateLimitSet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL enable,
    IN CPSS_PORT_SPEED_ENT  portSpeed
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32    fieldOffset;       /* The start bit number in the register */
    GT_U32    portGroupId;       /* the port group Id - support multi port group device */
    GT_U32     localPort;         /* local port - support multi-port-groups device */
    GT_U32    coreClock;         /* core clock frequency */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    coreClock = PRV_CPSS_PP_MAC(devNum)->coreClock;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    if(enable == GT_FALSE)
    {
        data = 0;
    }
    else
    {
        switch(portSpeed)
        {
            case CPSS_PORT_SPEED_1000_E:
                data = coreClock;
                break;
            case CPSS_PORT_SPEED_10000_E:
                data = coreClock / 10;
                break;
            case CPSS_PORT_SPEED_2500_E:
                data = coreClock * 10 / 25;
                break;
           case CPSS_PORT_SPEED_5000_E:
               data = coreClock  / 5;
               break;
            case CPSS_PORT_SPEED_12000_E:
                data = coreClock / 12;
                break;
            case CPSS_PORT_SPEED_20000_E:
                data = coreClock / 20;
                break;
            case CPSS_PORT_SPEED_40000_E:
                data = coreClock / 40;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if (data > 0)
    {
        /* the value should be decremented to avoid losses under full wire speed */
        data -= 1;

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
        {
            if (coreClock == 360)
            {
                switch (portSpeed)
                {
                    case CPSS_PORT_SPEED_1000_E:  data = 359; break;
                    case CPSS_PORT_SPEED_10000_E: data = 35; break;
                    case CPSS_PORT_SPEED_20000_E: data = 17; break;
                    case CPSS_PORT_SPEED_40000_E: data = 4;  break;
                    default:
                    {
                        break;
                    }
                }
            }
        }
    }

    if(CPSS_CPU_PORT_NUM_CNS == localPort)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            cutThroughRegs.ctCpuPortMemoryRateLimitThresholdReg;
        fieldOffset = 0;
    }
    else
    {
        /* fix Coverity warning OVERRUN_STATIC for array:
         PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctPortMemoryRateLimitThresholdReg
        */
        if (localPort >= 12)
        {
            /* it's should never happen because
            PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC does not allows
            such ports to be accepted  */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            cutThroughRegs.ctPortMemoryRateLimitThresholdReg[localPort / 2];
        fieldOffset = (localPort % 2) * 10;
    }

    /* Set rate limiting. */
    return prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                         fieldOffset, 10, data);

}

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitSet
*
* DESCRIPTION:
*       Set rate limiting of read operations from the memory
*       per target port in Cut Through mode according to the port speed.
*       To prevent congestion in egress pipe, buffer memory read operations
*       are rate limited according to the target port speed.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number including CPU port.
*       enable       - GT_TRUE - rate limiting is enabled.
*                    - GT_FALSE - rate limiting is disabled.
*       portSpeed    - port speed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portNum or portSpeed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Rate limit is recommended be enabled
*       on all egress ports for cut-through traffic.
*       Rate limit to the CPU port should be configured as a 1G port.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMemoryRateLimitSet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL enable,
    IN CPSS_PORT_SPEED_ENT  portSpeed
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughMemoryRateLimitSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable, portSpeed));

    rc = internal_cpssDxChCutThroughMemoryRateLimitSet(devNum, portNum, enable, portSpeed);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable, portSpeed));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughMemoryRateLimitGet
*
* DESCRIPTION:
*       Get rate limiting of read operations from the memory
*       per target port in Cut Through mode.
*       To prevent congestion in egress pipe, buffer memory read operations
*       are rate limited according to the target port speed.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number.
*       portNum       - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr     - pointer to rate limiting mode status.
*                        - GT_TRUE - rate limiting is enabled.
*                        - GT_FALSE - rate limiting is disabled.
*       portSpeedPtr  - pointer to port speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or portNum
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughMemoryRateLimitGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL *enablePtr,
    OUT CPSS_PORT_SPEED_ENT  *portSpeedPtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32    fieldOffset;       /* The start bit number in the register */
    GT_U32    portGroupId;       /* the port group Id - support multi port group device */
    GT_U32     localPort;         /* local port - support multi-port-groups device */
    GT_STATUS rc;                /* return status                */
    GT_U32    coreClock;         /* core clock frequency */
    GT_U32    speedCoef;         /* speed coefficient */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(portSpeedPtr);

    coreClock = PRV_CPSS_PP_MAC(devNum)->coreClock;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    if(CPSS_CPU_PORT_NUM_CNS == localPort)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            cutThroughRegs.ctCpuPortMemoryRateLimitThresholdReg;
        fieldOffset = 0;
    }
    else
    {
        /* fix Coverity warning OVERRUN_STATIC for array:
         PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->cutThroughRegs.ctPortMemoryRateLimitThresholdReg
        */
        if (localPort >= 12)
        {
            /* it's should never happen because
            PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC does not allows
            such ports to be accepted  */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            cutThroughRegs.ctPortMemoryRateLimitThresholdReg[localPort / 2];
        fieldOffset = (localPort % 2) * 10;
    }

    /* Get rate limiting. */
    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                       fieldOffset, 10, &data);
    if(rc != GT_OK)
    {
        return rc;
    }
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E && coreClock == 360)
    {
        switch (data)
        {
            case 359: *portSpeedPtr = CPSS_PORT_SPEED_1000_E;  break;
            case 35:  *portSpeedPtr = CPSS_PORT_SPEED_10000_E; break;
            case 17:  *portSpeedPtr = CPSS_PORT_SPEED_20000_E; break;
            case 4:   *portSpeedPtr = CPSS_PORT_SPEED_40000_E; break;
            default:
            {
                goto regular_processing;
            }
        }
        *enablePtr = GT_TRUE;
        return GT_OK;
    }
regular_processing:
    if (data > 0)
    {
        /* restoring the value that was decremented to avoid losses under full wire speed */
        data += 1;
    }
    if(data == 0)
    {
        *enablePtr = GT_FALSE;
    }
    else
    {
        speedCoef = coreClock * 10 / data;

        switch(speedCoef)
        {
            case 10:
                *portSpeedPtr = CPSS_PORT_SPEED_1000_E;
                break;
            case 100:
                *portSpeedPtr = CPSS_PORT_SPEED_10000_E;
                break;
            case 25:
                *portSpeedPtr = CPSS_PORT_SPEED_2500_E;
                break;
            case 50:
                *portSpeedPtr = CPSS_PORT_SPEED_5000_E;
                break;
            case 120:
                *portSpeedPtr = CPSS_PORT_SPEED_12000_E;
                break;
            case 200:
                *portSpeedPtr = CPSS_PORT_SPEED_20000_E;
                break;
            case 400:
                *portSpeedPtr = CPSS_PORT_SPEED_40000_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        *enablePtr = GT_TRUE;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitGet
*
* DESCRIPTION:
*       Get rate limiting of read operations from the memory
*       per target port in Cut Through mode.
*       To prevent congestion in egress pipe, buffer memory read operations
*       are rate limited according to the target port speed.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number.
*       portNum       - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr     - pointer to rate limiting mode status.
*                        - GT_TRUE - rate limiting is enabled.
*                        - GT_FALSE - rate limiting is disabled.
*       portSpeedPtr  - pointer to port speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or portNum
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughMemoryRateLimitGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL *enablePtr,
    OUT CPSS_PORT_SPEED_ENT  *portSpeedPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughMemoryRateLimitGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr, portSpeedPtr));

    rc = internal_cpssDxChCutThroughMemoryRateLimitGet(devNum, portNum, enablePtr, portSpeedPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr, portSpeedPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughBypassRouterAndPolicerEnableSet
*
* DESCRIPTION:
*       Enable / Disable bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       enable       - GT_TRUE -  bypassing of Router and Ingress Policer
*                                 engines is enabled.
*                    - GT_FALSE - bypassing of Router and Ingress Policer
*                                 engines is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughBypassRouterAndPolicerEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */
    GT_U32    fieldOffset;       /* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    data = (enable == GT_TRUE) ? 1 : 0;
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig1;
        fieldOffset = 7;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bridgeRegs.bridgeGlobalConfigRegArray[2];
        fieldOffset = 20;
    }

    /* Enable / Disable bypassing the Router and Ingress Policer engines.  */
    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, 1, data);

}

/*******************************************************************************
* cpssDxChCutThroughBypassRouterAndPolicerEnableSet
*
* DESCRIPTION:
*       Enable / Disable bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       enable       - GT_TRUE -  bypassing of Router and Ingress Policer
*                                 engines is enabled.
*                    - GT_FALSE - bypassing of Router and Ingress Policer
*                                 engines is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassRouterAndPolicerEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughBypassRouterAndPolicerEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChCutThroughBypassRouterAndPolicerEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughBypassRouterAndPolicerEnableGet
*
* DESCRIPTION:
*       Get mode of bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       enablePtr     - pointer to bypassing the Router and Ingress
*                       Policer engines status.
*                      - GT_TRUE  -  bypassing of Router and Ingress Policer
*                                    engines is enabled.
*                      - GT_FALSE -  bypassing of Router and Ingress Policer
*                                  engines is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughBypassRouterAndPolicerEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32    regAddr;     /* register address */
    GT_U32    data;        /* reg sub field data */
    GT_STATUS rc;          /* return status */
    GT_U32    fieldOffset; /* The start bit number in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig1;
        fieldOffset = 7;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bridgeRegs.bridgeGlobalConfigRegArray[2];
        fieldOffset = 20;
    }

    /* Get mode of bypassing the Router and Ingress Policer engines. */
    rc =  prvCpssHwPpGetRegField(devNum, regAddr, fieldOffset, 1, &data);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (data == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChCutThroughBypassRouterAndPolicerEnableGet
*
* DESCRIPTION:
*       Get mode of bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       enablePtr     - pointer to bypassing the Router and Ingress
*                       Policer engines status.
*                      - GT_TRUE  -  bypassing of Router and Ingress Policer
*                                    engines is enabled.
*                      - GT_FALSE -  bypassing of Router and Ingress Policer
*                                  engines is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassRouterAndPolicerEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughBypassRouterAndPolicerEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChCutThroughBypassRouterAndPolicerEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughBypassModeSet
*
* DESCRIPTION:
*       Set Bypass mode.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number.
*       bypassModePtr - pointer to Bypass mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, Bypass Ingress PCL mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughBypassModeSet
(
    IN GT_U8                             devNum,
    IN CPSS_DXCH_CUT_THROUGH_BYPASS_STC  *bypassModePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    value;             /* register field value */
    GT_U32    startBit;          /* start bit of the field */
    GT_U32    i;                 /* iterator */
    GT_STATUS rc;
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;/* pointer to the module
                                                configure of the PP's database*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(bypassModePtr);

    /* Configure the module configruation struct.   */
    moduleCfgPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg);

    if ((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE) ||
        (bypassModePtr->bypassRouter == GT_TRUE))
    {
        /* Enable the router glue engine when IPCL is used for routing. */
        value = 0;
    }
    else
    {
        switch(moduleCfgPtr->ip.routingMode)
        {
            case CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E:
                value = 0;
                break;
            case CPSS_DXCH_TCAM_ROUTER_BASED_E:
                value = 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        value |= (BOOL2BIT_MAC(bypassModePtr->bypassRouter)) << 27;
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->IPvX.routerGlobalCtrl.routerGlobalCtrl1;
        rc = prvCpssHwPpWriteRegBitMask(devNum, regAddr, 0x08000000, value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        value |= (BOOL2BIT_MAC(bypassModePtr->bypassRouter)) << 30;
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerAdditionalCtrlReg;
        rc = prvCpssHwPpWriteRegBitMask(devNum, regAddr, 0x40000001, value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    for (i = 0; i <= CPSS_DXCH_POLICER_STAGE_EGRESS_E; i++)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_PLR_MAC(devNum,i).policerCtrl1;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                PLR[i].policerControl1Reg;
        }

        switch((CPSS_DXCH_POLICER_STAGE_TYPE_ENT)i)
        {
            case CPSS_DXCH_POLICER_STAGE_INGRESS_0_E:
                value = BOOL2BIT_MAC(bypassModePtr->bypassIngressPolicerStage0);
                break;

            case CPSS_DXCH_POLICER_STAGE_INGRESS_1_E:
                value = BOOL2BIT_MAC(bypassModePtr->bypassIngressPolicerStage1);
                break;
            case CPSS_DXCH_POLICER_STAGE_EGRESS_E:
                value = BOOL2BIT_MAC(bypassModePtr->bypassEgressPolicer);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, value);
        if(rc != GT_OK)
        {
            return rc;
        }

    }

    value = BOOL2BIT_MAC(bypassModePtr->bypassEgressPcl);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* in Bobcat2 and above moved from HA to EPCL */
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
        startBit = 12;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        startBit = 29;
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, startBit, 1, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(bypassModePtr->bypassIngressPcl)
    {
        case CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_NO_BYPASS_E:
            value = 0;
            break;
        case CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ONE_LOOKUP_E:
            value = 1;
            break;
        case CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_TWO_LOOKUP_E:
            value = 2;
            break;
        case CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_FULL_BYPASS_E:
            value = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).ingrPolicyGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 15, 2, value);

}

/*******************************************************************************
* cpssDxChCutThroughBypassModeSet
*
* DESCRIPTION:
*       Set Bypass mode.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number.
*       bypassModePtr - pointer to Bypass mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, Bypass Ingress PCL mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassModeSet
(
    IN GT_U8                             devNum,
    IN CPSS_DXCH_CUT_THROUGH_BYPASS_STC  *bypassModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughBypassModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, bypassModePtr));

    rc = internal_cpssDxChCutThroughBypassModeSet(devNum, bypassModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, bypassModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughBypassModeGet
*
* DESCRIPTION:
*       Get Bypass mode.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number.
*
* OUTPUTS:
*       bypassModePtr - pointer to Bypass mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughBypassModeGet
(
    IN  GT_U8                              devNum,
    OUT CPSS_DXCH_CUT_THROUGH_BYPASS_STC   *bypassModePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    value;             /* register field value */
    GT_U32    startBit;          /* start bit of the field */
    GT_U32    i;                 /* iterator */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_XCAT2_E | CPSS_LION_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    CPSS_NULL_PTR_CHECK_MAC(bypassModePtr);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(devNum)->IPvX.routerGlobalCtrl.routerGlobalCtrl1;
        startBit = 27;
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerAdditionalCtrlReg;
        startBit = 30;
    }
    rc = prvCpssHwPpGetRegField(devNum, regAddr, startBit, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    bypassModePtr->bypassRouter = BIT2BOOL_MAC(value);

    for (i = 0; i <= CPSS_DXCH_POLICER_STAGE_EGRESS_E; i++)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_PLR_MAC(devNum,i).policerCtrl1;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                PLR[i].policerControl1Reg;
        }

        rc = prvCpssHwPpGetRegField(devNum, regAddr, 6, 1, &value);
        if(rc != GT_OK)
        {
            return rc;
        }

        switch((CPSS_DXCH_POLICER_STAGE_TYPE_ENT)i)
        {
            case CPSS_DXCH_POLICER_STAGE_INGRESS_0_E:
                bypassModePtr->bypassIngressPolicerStage0 = BIT2BOOL_MAC(value);
                break;

            case CPSS_DXCH_POLICER_STAGE_INGRESS_1_E:
                bypassModePtr->bypassIngressPolicerStage1 = BIT2BOOL_MAC(value);
                break;
            case CPSS_DXCH_POLICER_STAGE_EGRESS_E:
                bypassModePtr->bypassEgressPolicer = BIT2BOOL_MAC(value);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* in Bobcat2 abd above moved from HA to EPCL */
        regAddr = PRV_DXCH_REG1_UNIT_EPCL_MAC(devNum).egrPolicyGlobalConfig;
        startBit = 12;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        startBit = 29;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, startBit, 1, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    bypassModePtr->bypassEgressPcl = BIT2BOOL_MAC(value);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_PCL_MAC(devNum).ingrPolicyGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 15, 2, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(value)
    {
        case 0:
            bypassModePtr->bypassIngressPcl = CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_NO_BYPASS_E;
            break;
        case 1:
            bypassModePtr->bypassIngressPcl = CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ONE_LOOKUP_E;
            break;
        case 2:
            bypassModePtr->bypassIngressPcl = CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_TWO_LOOKUP_E;
            break;
        case 3:
            bypassModePtr->bypassIngressPcl = CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_FULL_BYPASS_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    }

    return rc;
}

/*******************************************************************************
* cpssDxChCutThroughBypassModeGet
*
* DESCRIPTION:
*       Get Bypass mode.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number.
*
* OUTPUTS:
*       bypassModePtr - pointer to Bypass mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughBypassModeGet
(
    IN  GT_U8                              devNum,
    OUT CPSS_DXCH_CUT_THROUGH_BYPASS_STC   *bypassModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughBypassModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, bypassModePtr));

    rc = internal_cpssDxChCutThroughBypassModeGet(devNum, bypassModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, bypassModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughPortGroupMaxBuffersLimitSet
*
* DESCRIPTION:
*        Set max buffers limit for Cut-Through packets.
*        Packets are handled as non-Cut-Through when number of allocated buffers more then limit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - device number.
*       portGroupsBmp             - bitmap of Port Groups.
*                                   NOTEs:
*                                    1. for non multi-port groups device this parameter is IGNORED.
*                                    2. for multi-port groups device :
*                                       bitmap must be set with at least one bit representing
*                                       valid port group(s). If a bit of non valid port group
*                                       is set then function returns GT_BAD_PARAM.
*                                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       buffersLimitEnable        - Buffers limit enable.
*                                     - GT_TRUE  - enable.
*                                     - GT_FALSE - disable.
*       buffersLimit              - Max buffers limit for Cut-Trough traffic.
*                                     The resolution is one buffer.
*                                     (APPLICABLE RANGES: 0..0x3FFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong maxSharedBuffersLimit
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*       buffersLimit = (buffers number per port group) – (guard band).
*       where guard band is (MTU) * (number of cut-through ports in port group)
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughPortGroupMaxBuffersLimitSet
(
    IN GT_U8                            devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN GT_BOOL                          buffersLimitEnable,
    IN GT_U32                           buffersLimit
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    portGroupId;       /* the port group Id */
    GT_U32    value;             /* register value */
    GT_U32    mask;              /* register mask */
    GT_STATUS rc;                /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    if (PRV_CPSS_DXCH_LION2_B1_AND_ABOVE_CHECK_MAC(devNum) == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }
    if (buffersLimit > BIT_14)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    value = ((BOOL2BIT_MAC(buffersLimitEnable)) << 31) | buffersLimit;
    mask  = 0x80003FFF;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngSharedBufConfigReg;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        rc = prvCpssHwPpPortGroupWriteRegBitMask(
            devNum, portGroupId, regAddr, mask, value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughPortGroupMaxBuffersLimitSet
*
* DESCRIPTION:
*        Set max buffers limit for Cut-Through packets.
*        Packets are handled as non-Cut-Through when number of allocated buffers more then limit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - device number.
*       portGroupsBmp             - bitmap of Port Groups.
*                                   NOTEs:
*                                    1. for non multi-port groups device this parameter is IGNORED.
*                                    2. for multi-port groups device :
*                                       bitmap must be set with at least one bit representing
*                                       valid port group(s). If a bit of non valid port group
*                                       is set then function returns GT_BAD_PARAM.
*                                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       buffersLimitEnable        - Buffers limit enable.
*                                     - GT_TRUE  - enable.
*                                     - GT_FALSE - disable.
*       buffersLimit              - Max buffers limit for Cut-Trough traffic.
*                                     The resolution is one buffer.
*                                     (APPLICABLE RANGES: 0..0x3FFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong maxSharedBuffersLimit
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*       buffersLimit = (buffers number per port group) – (guard band).
*       where guard band is (MTU) * (number of cut-through ports in port group)
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortGroupMaxBuffersLimitSet
(
    IN GT_U8                            devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN GT_BOOL                          buffersLimitEnable,
    IN GT_U32                           buffersLimit
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughPortGroupMaxBuffersLimitSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, buffersLimitEnable, buffersLimit));

    rc = internal_cpssDxChCutThroughPortGroupMaxBuffersLimitSet(devNum, portGroupsBmp, buffersLimitEnable, buffersLimit);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, buffersLimitEnable, buffersLimit));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChCutThroughPortGroupMaxBuffersLimitGet
*
* DESCRIPTION:
*        Get max buffers limit for Cut-Through packets.
*        Packets are handled as non-Cut-Through when number of allocated buffers more then limit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - device number.
*       portGroupsBmp             - bitmap of Port Groups.
*                                   NOTEs:
*                                    1. for non multi-port groups device this parameter is IGNORED.
*                                    2. for multi-port groups device :
*                                       bitmap must be set with at least one bit representing
*                                       valid port group(s). If a bit of non valid port group
*                                       is set then function returns GT_BAD_PARAM.
*                                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       buffersLimitEnablePtr        - (pointer to)Buffers limit enable.
*                                        - GT_TRUE  - enable.
*                                        - GT_FALSE - disable.
*       buffersLimitPtr              - (pointer to)Max buffers limit for Cut-Trough traffic.
*                                        The resolution is one buffer.
*                                        (APPLICABLE RANGES: 0..0x3FFF)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on Null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*       buffersLimit = (buffers number per port group) – (guard band).
*       where guard band is (MTU) * (number of cut-through ports in port group)
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChCutThroughPortGroupMaxBuffersLimitGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    OUT GT_BOOL                         *buffersLimitEnablePtr,
    OUT GT_U32                          *buffersLimitPtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    portGroupId;       /* the port group Id */
    GT_U32    value;             /* register value */
    GT_STATUS rc;                /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);
    if (PRV_CPSS_DXCH_LION2_B1_AND_ABOVE_CHECK_MAC(devNum) == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);
    CPSS_NULL_PTR_CHECK_MAC(buffersLimitEnablePtr);
    CPSS_NULL_PTR_CHECK_MAC(buffersLimitPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngSharedBufConfigReg;

    rc = prvCpssHwPpPortGroupReadRegister(
        devNum, portGroupId, regAddr, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *buffersLimitEnablePtr  = BIT2BOOL_MAC((value >> 31));
    *buffersLimitPtr        = (value & 0x3FFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCutThroughPortGroupMaxBuffersLimitGet
*
* DESCRIPTION:
*        Get max buffers limit for Cut-Through packets.
*        Packets are handled as non-Cut-Through when number of allocated buffers more then limit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                    - device number.
*       portGroupsBmp             - bitmap of Port Groups.
*                                   NOTEs:
*                                    1. for non multi-port groups device this parameter is IGNORED.
*                                    2. for multi-port groups device :
*                                       bitmap must be set with at least one bit representing
*                                       valid port group(s). If a bit of non valid port group
*                                       is set then function returns GT_BAD_PARAM.
*                                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       buffersLimitEnablePtr        - (pointer to)Buffers limit enable.
*                                        - GT_TRUE  - enable.
*                                        - GT_FALSE - disable.
*       buffersLimitPtr              - (pointer to)Max buffers limit for Cut-Trough traffic.
*                                        The resolution is one buffer.
*                                        (APPLICABLE RANGES: 0..0x3FFF)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on Null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*       buffersLimit = (buffers number per port group) – (guard band).
*       where guard band is (MTU) * (number of cut-through ports in port group)
*
*******************************************************************************/
GT_STATUS cpssDxChCutThroughPortGroupMaxBuffersLimitGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    OUT GT_BOOL                         *buffersLimitEnablePtr,
    OUT GT_U32                          *buffersLimitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChCutThroughPortGroupMaxBuffersLimitGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, buffersLimitEnablePtr, buffersLimitPtr));

    rc = internal_cpssDxChCutThroughPortGroupMaxBuffersLimitGet(devNum, portGroupsBmp, buffersLimitEnablePtr, buffersLimitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, buffersLimitEnablePtr, buffersLimitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

