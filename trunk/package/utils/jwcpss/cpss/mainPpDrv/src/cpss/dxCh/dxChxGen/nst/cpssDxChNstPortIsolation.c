/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgPortIsolation.c
*
* DESCRIPTION:
*       CPSS DxCh NST Port Isolation Mechanism.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/nst/private/prvCpssDxChNstLog.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

/* width of port isolation table -- number of words */
#define PORT_ISOLATION_NUM_WORDS_CNS    8

/*******************************************************************************
* portIsolationEntryIndexCalc
*
* DESCRIPTION:
*       Function calculates port isolation table index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum           - device number
*       srcInterfacePtr  - (pointer to) source interface information
*
* OUTPUTS:
*       indexPtr    - (pointer to) port isolation table index
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on error.
*       GT_BAD_PARAM  - wrong devNum, portNum, trandId
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS portIsolationEntryIndexCalc
(
    IN GT_U8                                          devNum,
    IN CPSS_INTERFACE_INFO_STC                        *srcInterfacePtr,
    OUT GT_U32                                        *indexPtr
)
{
    GT_U32 hwDev, hwPort;

    devNum = devNum;

    /* Port isolation table index calculation */
    switch (srcInterfacePtr->type)
    {
        case CPSS_INTERFACE_PORT_E:

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(srcInterfacePtr->devPort.hwDevNum,srcInterfacePtr->devPort.portNum);
            hwDev =  PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(srcInterfacePtr->devPort.hwDevNum,srcInterfacePtr->devPort.portNum);
            hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(srcInterfacePtr->devPort.hwDevNum,srcInterfacePtr->devPort.portNum);

            if ((hwDev > BIT_MASK_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupDeviceBits)) ||
                (hwPort > BIT_MASK_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupPortBits)))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
                    "hwDev[%d] more than supported[%d] or hwPort[%d] more than supported[%d]",
                    hwDev,BIT_MASK_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupDeviceBits),
                    hwPort,BIT_MASK_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupPortBits));
            }

            *indexPtr = (hwDev << PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupPortBits) | (hwPort);
            break;

        case CPSS_INTERFACE_TRUNK_E:
            if (srcInterfacePtr->trunkId > BIT_MASK_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupTrunkBits))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
                    "trunkId[%d] more than supported[%d] ",
                    srcInterfacePtr->trunkId,BIT_MASK_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupTrunkBits));
            }

            *indexPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupTrunkIndexBase+
                        srcInterfacePtr->trunkId;

            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
                "not supported interface type[%d]",
                srcInterfacePtr->type);
    }

    CPSS_LOG_INFORMATION_MAC("Calculated port isolation index [%d] \n",
        (*indexPtr ));

    return GT_OK;
}

/*******************************************************************************
* portIsolationEntryAddrCalc_txqVer0
*
* DESCRIPTION:
*       Function calculates port isolation table's entry address.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterfacePtr  - source interface
*
* OUTPUTS:
*       regAddrPtr    - (pointer to) table register
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on error.
*       GT_BAD_PARAM  - wrong devNum, portNum, trandId
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS portIsolationEntryAddrCalc_txqVer0
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        *srcInterfacePtr,
    OUT GT_U32                                        *regAddrPtr
)
{
    GT_STATUS   rc;
    GT_U32  tblIndex;  /* port isolation table index */

    /* calculate index */
    rc = portIsolationEntryIndexCalc(devNum,srcInterfacePtr,&tblIndex);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Address of L2/L3 Port Isolation Register */
    switch (trafficType)
    {
        case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E:
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.l2PortIsolationTableBase + tblIndex * 0x10;
            break;

        case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E:
            *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.l3PortIsolationTableBase + tblIndex * 0x10;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationEnableSet
*
* DESCRIPTION:
*       Function enables/disables the port isolation feature.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       enable    - port isolation feature enable/disable
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;    /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).egrFilterConfigs.egrFiltersEnable;
        return prvCpssHwPpSetRegField(devNum, regAddr, 6, 1, BOOL2BIT_MAC(enable));
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Address of Transmit Queue Resource Sharing Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;
        return prvCpssDrvHwPpSetRegField(devNum, regAddr, 28, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
                filterConfig.globalEnables;
        return prvCpssHwPpSetRegField(devNum, regAddr, 7, 1, BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChNstPortIsolationEnableSet
*
* DESCRIPTION:
*       Function enables/disables the port isolation feature.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       enable    - port isolation feature enable/disable
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChNstPortIsolationEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled state of the port isolation feature.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - (pointer to) port isolation feature state
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      hwValue;    /* register value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).egrFilterConfigs.egrFiltersEnable;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 6, 1, &hwValue);
    }
    else
    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Address of Transmit Queue Resource Sharing Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueResSharingAndTunnelEgrFltr;

        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 28, 1, &hwValue);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.
                filterConfig.globalEnables;
        rc = prvCpssHwPpGetRegField(devNum, regAddr, 7, 1, &hwValue);
    }

    /* Convert return value to BOOLEAN type */
    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;

}

/*******************************************************************************
* cpssDxChNstPortIsolationEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled state of the port isolation feature.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - (pointer to) port isolation feature state
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChNstPortIsolationEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function sets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking. If it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interface.
*                              Only portDev and Trunk are supported.
*       cpuPortMember        - port isolation for CPU Port
*                               GT_TRUE - member
*                               GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written to the
*                              L2/L3 PI table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface or
*                                  localPortsMembersPtr
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationTableEntrySet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_BOOL                                        cpuPortMember,
    IN CPSS_PORTS_BMP_STC                             *localPortsMembersPtr
)
{
    GT_U32      hwValue;    /* register value */
    GT_U32      regAddr;    /* register address */
    GT_U32      maxPortNum;
    GT_STATUS   rc;         /* return status */
    GT_U32      tblIndex;/*table index*/
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(localPortsMembersPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* check port bitmap */
        maxPortNum = PRV_CPSS_PP_MAC(devNum)->numOfPorts;

        if ((localPortsMembersPtr->ports[0] >= ((GT_U32)(1 << maxPortNum))) || (localPortsMembersPtr->ports[1]))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* get register address */
        rc = portIsolationEntryAddrCalc_txqVer0(devNum, trafficType, &srcInterface, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* combine register value from port bitmap and cpu port */
        hwValue = (localPortsMembersPtr->ports[0]) | (BOOL2BIT_MAC(cpuPortMember) << 28);

        return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 29, hwValue);
    }
    else
    {
        /* calculate index */
        rc = portIsolationEntryIndexCalc(devNum,&srcInterface,&tblIndex);
        if(rc != GT_OK)
        {
            return rc;
        }

        switch(trafficType)
        {
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E;
                break;
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            /* check that the device supports all the physical ports that defined in the BMP */
            rc = prvCpssDxChTablePortsBmpLimitedNumCheck(devNum ,
                256/* limited to 256 ports!*/,
                localPortsMembersPtr);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* call the generic function that handle BMP of ports in the EGF_SHT */
        rc = prvCpssDxChHwEgfShtPortsBmpTableEntryWrite(devNum,
                tableType,tblIndex,localPortsMembersPtr,
                GT_TRUE,cpuPortMember);

        return rc;
    }
}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function sets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking. If it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interface.
*                              Only portDev and Trunk are supported.
*       cpuPortMember        - port isolation for CPU Port
*                               GT_TRUE - member
*                               GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written to the
*                              L2/L3 PI table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface or
*                                  localPortsMembersPtr
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationTableEntrySet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_BOOL                                        cpuPortMember,
    IN CPSS_PORTS_BMP_STC                             *localPortsMembersPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationTableEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, trafficType, srcInterface, cpuPortMember, localPortsMembersPtr));

    rc = internal_cpssDxChNstPortIsolationTableEntrySet(devNum, trafficType, srcInterface, cpuPortMember, localPortsMembersPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, trafficType, srcInterface, cpuPortMember, localPortsMembersPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationTableEntryGet
*
* DESCRIPTION:
*       Function gets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking if it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interfaces
*                              Only portDev and Trunk are supported.
*
* OUTPUTS:
*       cpuPortMemberPtr     - (pointer to) port isolation for CPU Port
*                                GT_TRUE - member
*                                GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written
*                              to the L2/L3 PI table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationTableEntryGet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    OUT GT_BOOL                                       *cpuPortMemberPtr,
    OUT CPSS_PORTS_BMP_STC                            *localPortsMembersPtr
)
{
    GT_U32      hwValue;    /* register value */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return status */
    GT_U32      tblIndex;/*table index*/
    PRV_CPSS_DXCH_TABLE_ENT tableType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(localPortsMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(cpuPortMemberPtr);

    cpssOsMemSet(localPortsMembersPtr, 0,sizeof(*localPortsMembersPtr));

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* get register address */
        rc = portIsolationEntryAddrCalc_txqVer0(devNum, trafficType, &srcInterface, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 29, &hwValue);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }

        localPortsMembersPtr->ports[0] = hwValue & 0x0FFFFFFF;

        *cpuPortMemberPtr = BIT2BOOL_MAC((hwValue >> 28) & 1);
    }
    else
    {
        /* calculate index */
        rc = portIsolationEntryIndexCalc(devNum,&srcInterface,&tblIndex);
        if(rc != GT_OK)
        {
            return rc;
        }

        switch(trafficType)
        {
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E;
                break;
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* call the generic function that handle BMP of ports in the EGF_SHT */
        rc = prvCpssDxChHwEgfShtPortsBmpTableEntryRead(devNum,
                tableType,tblIndex,localPortsMembersPtr,
                GT_TRUE,cpuPortMemberPtr);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            /* adjusted physical ports of BMP to hold only bmp of existing ports*/
            rc = prvCpssDxChTablePortsBmpAdjustToDeviceLimitedNum(devNum ,
                256/* limited to 256 ports!*/,
                localPortsMembersPtr , localPortsMembersPtr);
        }

        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntryGet
*
* DESCRIPTION:
*       Function gets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking if it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interfaces
*                              Only portDev and Trunk are supported.
*
* OUTPUTS:
*       cpuPortMemberPtr     - (pointer to) port isolation for CPU Port
*                                GT_TRUE - member
*                                GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written
*                              to the L2/L3 PI table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationTableEntryGet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    OUT GT_BOOL                                       *cpuPortMemberPtr,
    OUT CPSS_PORTS_BMP_STC                            *localPortsMembersPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationTableEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, trafficType, srcInterface, cpuPortMemberPtr, localPortsMembersPtr));

    rc = internal_cpssDxChNstPortIsolationTableEntryGet(devNum, trafficType, srcInterface, cpuPortMemberPtr, localPortsMembersPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, trafficType, srcInterface, cpuPortMemberPtr, localPortsMembersPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* portIsolationPortSet
*
* DESCRIPTION:
*       Function set (add/remove) single local port in port isolation table entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be added to bitmap
*       enable        - GT_TRUE - add the port
*                       GT_FALSE - remove the port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS portIsolationPortSet
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        *srcInterfacePtr,
    IN GT_PHYSICAL_PORT_NUM                           portNum,
    IN GT_BOOL                                        enable
)
{
    GT_U32      offset;     /* offset */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */
    GT_U32  tblIndex;  /* port isolation table index */
    PRV_CPSS_DXCH_TABLE_ENT tableType;
    GT_U32  data;/* data to write to the HW */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    data = BOOL2BIT_MAC(enable);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* check port */
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* get register address */
        rc = portIsolationEntryAddrCalc_txqVer0(devNum, trafficType, srcInterfacePtr, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* calculate bit's offset */
        if (localPort == CPSS_CPU_PORT_NUM_CNS)
        {
            offset =  28;
        }
        else
        {
            offset =  localPort;
        }

        /* set relevant bit in the bitmap */
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, offset , 1, data);
    }
    else
    {
        /* calculate index */
        rc = portIsolationEntryIndexCalc(devNum,srcInterfacePtr,&tblIndex);
        if(rc != GT_OK)
        {
            return rc;
        }

        switch(trafficType)
        {
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E;
                break;
            case CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E:
                tableType = PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* call the generic function that handle port in tables of BMP of ports in the EGF_SHT */
        rc = prvCpssDxChHwEgfShtPortsBmpTableEntrySetPort(devNum,
                    tableType,tblIndex,portNum,enable);

        return rc;
    }
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationPortAdd
*
* DESCRIPTION:
*       Function adds single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Adding local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port isn't blocked.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be added to bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationPortAdd
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_PHYSICAL_PORT_NUM                           portNum
)
{
    return portIsolationPortSet(devNum,trafficType,&srcInterface,portNum,
        GT_TRUE);
}

/*******************************************************************************
* cpssDxChNstPortIsolationPortAdd
*
* DESCRIPTION:
*       Function adds single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Adding local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port isn't blocked.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be added to bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationPortAdd
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_PHYSICAL_PORT_NUM                           portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationPortAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, trafficType, srcInterface, portNum));

    rc = internal_cpssDxChNstPortIsolationPortAdd(devNum, trafficType, srcInterface, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, trafficType, srcInterface, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationPortDelete
*
* DESCRIPTION:
*       Function deletes single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Deleting local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port is blocked.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be deleted from bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In srcInterface parameter only portDev and Trunk are supported.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationPortDelete
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_PHYSICAL_PORT_NUM                           portNum
)
{
    return portIsolationPortSet(devNum,trafficType,&srcInterface,portNum,
        GT_FALSE);
}

/*******************************************************************************
* cpssDxChNstPortIsolationPortDelete
*
* DESCRIPTION:
*       Function deletes single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Deleting local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port is blocked.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be deleted from bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In srcInterface parameter only portDev and Trunk are supported.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationPortDelete
(
    IN GT_U8                                          devNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                        srcInterface,
    IN GT_PHYSICAL_PORT_NUM                           portNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationPortDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, trafficType, srcInterface, portNum));

    rc = internal_cpssDxChNstPortIsolationPortDelete(devNum, trafficType, srcInterface, portNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, trafficType, srcInterface, portNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationModeSet
*
* DESCRIPTION:
*       Set Port Isolation feature mode in the TxQ per egress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       mode        - Port Isolation Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationModeSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_NUM                              portNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT    mode
)
{
    GT_U32      convMode;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_DXCH_NST_PORT_ISOLATION_DISABLE_E:
            convMode = 0;
            break;
        case CPSS_DXCH_NST_PORT_ISOLATION_L2_ENABLE_E:
            convMode = 1;
            break;
        case CPSS_DXCH_NST_PORT_ISOLATION_L3_ENABLE_E:
            convMode = 2;
            break;
        case CPSS_DXCH_NST_PORT_ISOLATION_ALL_ENABLE_E:
            convMode = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssDxChHwEgfShtFieldSet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_PORT_ISOLATION_MODE_E,
            GT_TRUE, /*accessPhysicalPort*/
            GT_TRUE, /*accessEPort*/
            convMode);
}

/*******************************************************************************
* cpssDxChNstPortIsolationModeSet
*
* DESCRIPTION:
*       Set Port Isolation feature mode in the TxQ per egress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       mode        - Port Isolation Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationModeSet
(
    IN GT_U8                                    devNum,
    IN GT_PORT_NUM                              portNum,
    IN CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT    mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mode));

    rc = internal_cpssDxChNstPortIsolationModeSet(devNum, portNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationModeGet
*
* DESCRIPTION:
*       Get Port Isolation feature mode in the TxQ per egress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       modePtr     - (pointer to) Port Isolation Mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_NUM                             portNum,
    OUT CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT   *modePtr
)
{
    GT_STATUS   rc;
    GT_U32      convMode;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = prvCpssDxChHwEgfShtFieldGet(devNum,portNum,
        PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_PORT_ISOLATION_MODE_E,
            GT_FALSE, /*accessPhysicalPort*/
            &convMode);

     switch(convMode)
     {
        case 0:
            *modePtr = CPSS_DXCH_NST_PORT_ISOLATION_DISABLE_E;
            break;
        case 1:
            *modePtr = CPSS_DXCH_NST_PORT_ISOLATION_L2_ENABLE_E;
            break;
        case 2:
            *modePtr = CPSS_DXCH_NST_PORT_ISOLATION_L3_ENABLE_E;
            break;
        case 3:
            *modePtr = CPSS_DXCH_NST_PORT_ISOLATION_ALL_ENABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
     }

    return rc;
}

/*******************************************************************************
* cpssDxChNstPortIsolationModeGet
*
* DESCRIPTION:
*       Get Port Isolation feature mode in the TxQ per egress port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       modePtr     - (pointer to) Port Isolation Mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PORT_NUM                             portNum,
    OUT CPSS_DXCH_NST_PORT_ISOLATION_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modePtr));

    rc = internal_cpssDxChNstPortIsolationModeGet(devNum, portNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationLookupBitsSet
*
* DESCRIPTION:
*       Set the number of bits from the source Interface that are used to
*       index the port isolation table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       numberOfPortBits    - number of least significant bits of src port (APPLICABLE RANGES: 0..15)
*       numberOfDeviceBits  - number of least significant bits of src device (APPLICABLE RANGES: 0..15)
*       numberOfTrunkBits   - number of least significant bits of src trunk  (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - on value out of range for one of:
*                                   numberOfPortBits > 15 , numberOfDeviceBits > 15 ,
*                                   numberOfTrunkBits > 15
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For example, if the value of this field is 7, and <Port Isolation
*       Lookup Device Bits> is 5, then the index to the port isolation table is
*       {SrcDev[4:0], OrigSRCePort[6:0]}.
*       Note:
*           If <Port Isolation Lookup Port Bits> is 0x0, no bits from the source
*               port are used.
*           If <Port Isolation Lookup Device Bits> is 0x0, no bits from the
*               SrcDev are used.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationLookupBitsSet
(
    IN GT_U8    devNum,
    IN GT_U32   numberOfPortBits,
    IN GT_U32   numberOfDeviceBits,
    IN GT_U32   numberOfTrunkBits
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;
    GT_U32      regValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(numberOfPortBits   >= BIT_5 ||
       numberOfDeviceBits >= BIT_4 ||
       numberOfTrunkBits  >= BIT_4 )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regValue = numberOfTrunkBits | (numberOfDeviceBits << 4);

    regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.portIsolationLookup0;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 21, 8, regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    regValue = numberOfPortBits;

    regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.portIsolationLookup1;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 1, 5, regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* save to database */
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupPortBits = numberOfPortBits;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupDeviceBits = numberOfDeviceBits;
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupTrunkBits = numberOfTrunkBits;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationLookupBitsSet
*
* DESCRIPTION:
*       Set the number of bits from the source Interface that are used to
*       index the port isolation table.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       numberOfPortBits    - number of least significant bits of src port (APPLICABLE RANGES: 0..15)
*       numberOfDeviceBits  - number of least significant bits of src device (APPLICABLE RANGES: 0..15)
*       numberOfTrunkBits   - number of least significant bits of src trunk  (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - on value out of range for one of:
*                                   numberOfPortBits > 15 , numberOfDeviceBits > 15 ,
*                                   numberOfTrunkBits > 15
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For example, if the value of this field is 7, and <Port Isolation
*       Lookup Device Bits> is 5, then the index to the port isolation table is
*       {SrcDev[4:0], OrigSRCePort[6:0]}.
*       Note:
*           If <Port Isolation Lookup Port Bits> is 0x0, no bits from the source
*               port are used.
*           If <Port Isolation Lookup Device Bits> is 0x0, no bits from the
*               SrcDev are used.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationLookupBitsSet
(
    IN GT_U8    devNum,
    IN GT_U32   numberOfPortBits,
    IN GT_U32   numberOfDeviceBits,
    IN GT_U32   numberOfTrunkBits
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationLookupBitsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, numberOfPortBits, numberOfDeviceBits, numberOfTrunkBits));

    rc = internal_cpssDxChNstPortIsolationLookupBitsSet(devNum, numberOfPortBits, numberOfDeviceBits, numberOfTrunkBits);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, numberOfPortBits, numberOfDeviceBits, numberOfTrunkBits));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationLookupBitsGet
*
* DESCRIPTION:
*       Get the number of bits from the source Interface that are used to
*       index the port isolation table.
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
*       numberOfPortBitsPtr    - (pointer to)number of least significant bits of src port (APPLICABLE RANGES: 0..15)
*       numberOfDeviceBitsPtr  - (pointer to)number of least significant bits of src device (APPLICABLE RANGES: 0..15)
*       numberOfTrunkBitsPtr   - (pointer to)number of least significant bits of src trunk  (APPLICABLE RANGES: 0..15)
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
static GT_STATUS internal_cpssDxChNstPortIsolationLookupBitsGet
(
    IN GT_U8    devNum,
    OUT GT_U32   *numberOfPortBitsPtr,
    OUT GT_U32   *numberOfDeviceBitsPtr,
    OUT GT_U32   *numberOfTrunkBitsPtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;
    GT_U32      regValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(numberOfPortBitsPtr);
    CPSS_NULL_PTR_CHECK_MAC(numberOfDeviceBitsPtr);
    CPSS_NULL_PTR_CHECK_MAC(numberOfTrunkBitsPtr);

    regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.portIsolationLookup0;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 21, 8, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    *numberOfTrunkBitsPtr  = U32_GET_FIELD_MAC(regValue,0,4);
    *numberOfDeviceBitsPtr = U32_GET_FIELD_MAC(regValue,4,4);

    regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.portIsolationLookup1;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 1, 5, &regValue);

    *numberOfPortBitsPtr    = regValue;

    return rc;
}

/*******************************************************************************
* cpssDxChNstPortIsolationLookupBitsGet
*
* DESCRIPTION:
*       Get the number of bits from the source Interface that are used to
*       index the port isolation table.
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
*       numberOfPortBitsPtr    - (pointer to)number of least significant bits of src port (APPLICABLE RANGES: 0..15)
*       numberOfDeviceBitsPtr  - (pointer to)number of least significant bits of src device (APPLICABLE RANGES: 0..15)
*       numberOfTrunkBitsPtr   - (pointer to)number of least significant bits of src trunk  (APPLICABLE RANGES: 0..15)
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
GT_STATUS cpssDxChNstPortIsolationLookupBitsGet
(
    IN GT_U8    devNum,
    OUT GT_U32   *numberOfPortBitsPtr,
    OUT GT_U32   *numberOfDeviceBitsPtr,
    OUT GT_U32   *numberOfTrunkBitsPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationLookupBitsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, numberOfPortBitsPtr, numberOfDeviceBitsPtr, numberOfTrunkBitsPtr));

    rc = internal_cpssDxChNstPortIsolationLookupBitsGet(devNum, numberOfPortBitsPtr, numberOfDeviceBitsPtr, numberOfTrunkBitsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, numberOfPortBitsPtr, numberOfDeviceBitsPtr, numberOfTrunkBitsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationOnEportsEnableSet
*
* DESCRIPTION:
*       Determines how the port isolation is performed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE: port isolation is performed based on the source ePort.
*                GT_FALSE: port isolation is performed based on the source physical port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationOnEportsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.portIsolationLookup1;

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChNstPortIsolationOnEportsEnableSet
*
* DESCRIPTION:
*       Determines how the port isolation is performed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE: port isolation is performed based on the source ePort.
*                GT_FALSE: port isolation is performed based on the source physical port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationOnEportsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationOnEportsEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChNstPortIsolationOnEportsEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationOnEportsEnableGet
*
* DESCRIPTION:
*       Returns how the port isolation is performed.
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
*       enablePtr - (pointer to)
*               GT_TRUE: port isolation is performed based on the source ePort.
*               GT_FALSE: port isolation is performed based on the source physical port.

*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationOnEportsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      value;
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.portIsolationLookup1;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChNstPortIsolationOnEportsEnableGet
*
* DESCRIPTION:
*       Returns how the port isolation is performed.
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
*       enablePtr - (pointer to)
*               GT_TRUE: port isolation is performed based on the source ePort.
*               GT_FALSE: port isolation is performed based on the source physical port.

*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationOnEportsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationOnEportsEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChNstPortIsolationOnEportsEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationLookupTrunkIndexBaseSet
*
* DESCRIPTION:
*       Determines the first index of the trunk ID based lookup.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       trunkIndexBase - the first index of the trunk ID based lookup.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_OUT_OF_RANGE          - on trunkIndexBase out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The default value is 2048 (0x800) for backward compatibility.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationLookupTrunkIndexBaseSet
(
    IN GT_U8    devNum,
    IN GT_U32   trunkIndexBase
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (trunkIndexBase >= BIT_21)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.portIsolationLookup0;

    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 21, trunkIndexBase);
    if(rc != GT_OK)
        return rc;

     /* set Port data base */
    PRV_CPSS_DXCH_PP_MAC(devNum)->port.portIsolationLookupTrunkIndexBase = trunkIndexBase;

    return rc;
}

/*******************************************************************************
* cpssDxChNstPortIsolationLookupTrunkIndexBaseSet
*
* DESCRIPTION:
*       Determines the first index of the trunk ID based lookup.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       trunkIndexBase - the first index of the trunk ID based lookup.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_OUT_OF_RANGE          - on trunkIndexBase out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The default value is 2048 (0x800) for backward compatibility.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationLookupTrunkIndexBaseSet
(
    IN GT_U8    devNum,
    IN GT_U32   trunkIndexBase
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationLookupTrunkIndexBaseSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, trunkIndexBase));

    rc = internal_cpssDxChNstPortIsolationLookupTrunkIndexBaseSet(devNum, trunkIndexBase);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, trunkIndexBase));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIsolationLookupTrunkIndexBaseGet
*
* DESCRIPTION:
*       Returns the first index of the trunk ID based lookup.
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
*       trunkIndexBasePtr - (pointer to) the first index of the trunk ID based lookup.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIsolationLookupTrunkIndexBaseGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *trunkIndexBasePtr
)
{
    GT_STATUS   rc;
    GT_U32      value;
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(trunkIndexBasePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(devNum).global.portIsolationLookup0;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 21, &value);

    *trunkIndexBasePtr = value;

    return rc;
}

/*******************************************************************************
* cpssDxChNstPortIsolationLookupTrunkIndexBaseGet
*
* DESCRIPTION:
*       Returns the first index of the trunk ID based lookup.
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
*       trunkIndexBasePtr - (pointer to) the first index of the trunk ID based lookup.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIsolationLookupTrunkIndexBaseGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *trunkIndexBasePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIsolationLookupTrunkIndexBaseGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, trunkIndexBasePtr));

    rc = internal_cpssDxChNstPortIsolationLookupTrunkIndexBaseGet(devNum, trunkIndexBasePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, trunkIndexBasePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
