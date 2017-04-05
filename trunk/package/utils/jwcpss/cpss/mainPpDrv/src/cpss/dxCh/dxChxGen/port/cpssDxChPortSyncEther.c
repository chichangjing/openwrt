/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortSyncEther.c
*
* DESCRIPTION:
*       CPSS implementation for Sync-E (Synchronous Ethernet)
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 36 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortSyncEther.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

/*******************************************************************************
* prvCpssDxChPortSyncEtherPortLaneCheckAndInterfaceModeGet
*
* DESCRIPTION:
*       Function check if the lane number is within the allowable range for
*       the port interface mode.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       laneNum     - SERDES lane number within port.
*                     Allowable ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*
* OUTPUTS:
*       ifModePtr   - (pointer to) port interface mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum or lane number
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSyncEtherPortLaneCheck
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_U32                          laneNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT    *ifModePtr
)
{
    GT_STATUS                       rc;     /* return code */
    GT_U32                          maxLaneNum; /* lanes number according to */
                                                /* port interface mode       */
    CPSS_PORT_SPEED_ENT             speed; /* current port speed */
    GT_U32                          startSerdes;

    rc = cpssDxChPortInterfaceModeGet(devNum, portNum, ifModePtr);
    if( GT_OK != rc )
    {
        return rc;
    }

    rc = cpssDxChPortSpeedGet(devNum, portNum, &speed);
    if( GT_OK != rc )
    {
        return rc;
    }

    rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, *ifModePtr, &startSerdes, &maxLaneNum);
    if( GT_OK != rc )
    {
        return rc;
    }

    if( laneNum >= maxLaneNum )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
*
* DESCRIPTION:
*       Function enables/disables second recovery clock output pin.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number.
*       enable  - enable/disable second clock output pin
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Function performs all SERDES power down and restore original SERDES
*       power state.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regValue1;  /* register value */
    GT_STATUS rc;       /* returned status */
    GT_U8   i;          /* loop iterator */
    GT_PHYSICAL_PORT_NUM   portNum;    /* port number */
    GT_BOOL powerStateArr[10]; /* There are 6(Giga) + 4(Stacking) SERDES groups */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode[10]; /* port interface mode */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    switch (enable)
    {
        case GT_FALSE:
            regValue = 0;   /* <2nd_rcvr_clk_out_sel> field value */
            regValue1 = 0;  /* <ref_clk_125_sel> field value */
            break;
        case GT_TRUE:
            regValue = 1;   /* <2nd_rcvr_clk_out_sel> field value */
            regValue1 = 1;  /* <ref_clk_125_sel> field value */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* power down SERDESes and save the current
       SERDESes power state of ports */
    for (i = 0; i < 10; i++)
    {
        if (i < 6)
        {
            /* Giga ports */
            portNum = (GT_U8)(i * 4);
        }
        else
        {   /* Stacking ports */
            portNum = (GT_U8)(18 + i);
        }

        rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode[i]);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( CPSS_PORT_INTERFACE_MODE_MII_E == ifMode[i] )
        {
            continue;
        }

        /* store current SERDES power state */
        rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum, portNum, &powerStateArr[i]);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* power down SERDES */
        rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum, i, GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

    /* set <2nd_rcvr_clk_out_sel> in extended global register2 */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 18, 1, regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

    /* set <ref_clk_125_sel> in extended global register */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 10, 1, regValue1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* restore previous SERDES power state */
    for (i = 0; i < 10; i++)
    {
        if ( (powerStateArr[i] == GT_FALSE) ||
             (CPSS_PORT_INTERFACE_MODE_MII_E == ifMode[i]) )
        {
            continue;
        }

        rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum, i, powerStateArr[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
*
* DESCRIPTION:
*       Function enables/disables second recovery clock output pin.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number.
*       enable  - enable/disable second clock output pin
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Function performs all SERDES power down and restore original SERDES
*       power state.
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherSecondRecoveryClkEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPortSyncEtherSecondRecoveryClkEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherSecondRecoveryClkEnableGet
*
* DESCRIPTION:
*       Function gets status (enabled/disabled) of
*       second recovery clock output pin.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       enablePtr - (pointer to) second clock output pin state
*                    GT_TRUE  - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on bad state
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherSecondRecoveryClkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regValue1;  /* register value */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

    /* get <2nd_rcvr_clk_out_sel> in extended global register2 */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 18, 1, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

    /* get <ref_clk_125_sel> in extended global register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 10, 1, &regValue1);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((regValue == 0) && (regValue1 == 0))
    {
        *enablePtr = GT_FALSE;
    }
    else if ((regValue == 1) && (regValue1 == 1))
    {
        *enablePtr = GT_TRUE;
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherSecondRecoveryClkEnableGet
*
* DESCRIPTION:
*       Function gets status (enabled/disabled) of
*       second recovery clock output pin.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       enablePtr - (pointer to) second clock output pin state
*                    GT_TRUE  - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on bad state
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherSecondRecoveryClkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherSecondRecoveryClkEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPortSyncEtherSecondRecoveryClkEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherPllRefClkSelectSet
*
* DESCRIPTION:
*       Function sets PLL reference clock select:
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*                      For xCat & xCat2 valid ports for configuration:
*                      Giga Ports: 0,4,8,12,16,20
*                      Stacking Ports: 24,25,26,27
*       pllClkSelect - clock source: recovered or oscillator
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or pllClkSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Function performs SERDES power down and restore
*       original SERDES power state.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherPllRefClkSelectSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT pllClkSelect
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  portGroup;  /* port group */
    GT_U32   localPort;  /* local port - support multi-port-groups device */
    GT_STATUS rc;       /* returned status */
    GT_BOOL prevPowerState; /* SERDES previous power state */
    GT_U32  portSerdesGroup;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT3_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

        portSerdesGroup = portNum;
    }
    else
    {
        /* Check port number */
        switch (portNum)
        {
            case 0:     /* Group index for Giga ports = 4-9 */
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                portGroup = (4 + portNum / 4);
                portSerdesGroup = portNum / 4;
                break;
            case 24:    /* Group index for Stacking ports = 0-3 */
            case 25:
            case 26:
            case 27:
                portGroup = (portNum - 24);
                portSerdesGroup = (portNum - 18);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* Check clock source */
    switch(pllClkSelect)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_OSCILLATOR_E:
            regValue = 0;
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_RECOVERED_E:
            regValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* store current SERDES power state */
    rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum, portNum, &prevPowerState);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* power down SERDES */
    rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum, portSerdesGroup, GT_FALSE);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* Extended Global Control1 register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* set <sd_pll_ref_clk<n>_sel> in extended global register1 */
        rc = prvCpssHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, localPort+20, 1, regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* Extended Global Control2 register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

        /* set <sd_pll_ref_clk<n>_sel> in extended global register2 */
        rc = prvCpssHwPpSetRegField(devNum, regAddr, portGroup, 1, regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if (prevPowerState == GT_TRUE)
    {
        /* restore previous SERDES power state */
        rc = cpssDxChPortGroupSerdesPowerStatusSet(devNum, portSerdesGroup, prevPowerState);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherPllRefClkSelectSet
*
* DESCRIPTION:
*       Function sets PLL reference clock select:
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*                      For xCat & xCat2 valid ports for configuration:
*                      Giga Ports: 0,4,8,12,16,20
*                      Stacking Ports: 24,25,26,27
*       pllClkSelect - clock source: recovered or oscillator
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or pllClkSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Function performs SERDES power down and restore
*       original SERDES power state.
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherPllRefClkSelectSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT pllClkSelect
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherPllRefClkSelectSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, pllClkSelect));

    rc = internal_cpssDxChPortSyncEtherPllRefClkSelectSet(devNum, portNum, pllClkSelect);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, pllClkSelect));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherPllRefClkSelectGet
*
* DESCRIPTION:
*       Function gets PLL reference clock select:
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - device number.
*       portNum   - port number.
*                   For xCat & xCat2 valid ports for configuration:
*                   Giga Ports: 0,4,8,12,16,20
*                   Stacking Ports: 24,25,26,27
*
* OUTPUTS:
*       pllClkSelectPtr - (pointer to) clock source: recovered or oscillator
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherPllRefClkSelectGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT *pllClkSelectPtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  portGroup;  /* port group */
    GT_U32   localPort;  /* local port - support multi-port-groups device */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT3_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(pllClkSelectPtr);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    }
    else
    {
        /* Check port group */
        switch (portNum)
        {
            case 0:     /* Group index for Giga ports = 4-9 */
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                portGroup = (4 + portNum / 4);
                break;
            case 24:    /* Group index for Stacking ports = 0-3 */
            case 25:
            case 26:
            case 27:
                portGroup = (portNum - 24);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* Extended Global Control1 register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* get <sd_pll_ref_clk<n>_sel> in extended global register1 */
        rc = prvCpssHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, localPort+20, 1, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* Extended Global Control2 register address */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

        /* get <sd_pll_ref_clk<n>_sel> in extended global register2 */
        rc = prvCpssHwPpGetRegField(devNum, regAddr, portGroup, 1, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* Check clock source */
    *pllClkSelectPtr = (regValue == 1) ? CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_RECOVERED_E
                                       : CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_OSCILLATOR_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherPllRefClkSelectGet
*
* DESCRIPTION:
*       Function gets PLL reference clock select:
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - device number.
*       portNum   - port number.
*                   For xCat & xCat2 valid ports for configuration:
*                   Giga Ports: 0,4,8,12,16,20
*                   Stacking Ports: 24,25,26,27
*
* OUTPUTS:
*       pllClkSelectPtr - (pointer to) clock source: recovered or oscillator
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherPllRefClkSelectGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT *pllClkSelectPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherPllRefClkSelectGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, pllClkSelectPtr));

    rc = internal_cpssDxChPortSyncEtherPllRefClkSelectGet(devNum, portNum, pllClkSelectPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, pllClkSelectPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortSyncEtherRecoveryClkConfigSetForLion
*
* DESCRIPTION:
*       Function configures the recovery clock enable/disable state and sets
*       its source portNum.
*
* APPLICABLE DEVICES:
*        Lion;
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*       enable          - enable/disable recovered clock1/clock2
*                         GT_TRUE - enable
*                         GT_FALSE - disable
*       portNum         - port number.
*       laneNum         - selected SERDES lane number within port.
*                         (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3).
*                         Ranges according to port interface:
*                         SGMII/1000BaseX - 0
*                         RXAUI/HX        - 0..1
*                         XAUII           - 0..3
*                         XLG             - 0..7
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       If SERDES enabling is requesed and another SERDES in the port group
*       is already enabled, GT_BAD_STATE is returned.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSyncEtherRecoveryClkConfigSetForLion
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    IN  GT_BOOL enable,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U32  laneNum
)
{
    GT_STATUS rc;       /* returned status */
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  portGroup;  /* port group */
    GT_U32  serdes;     /* loop index */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */
    GT_U32      startSerdes, /* first serdes used by port */
                maxLaneNum;  /* number of serdeses used by port now */

    rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
    if( GT_OK != rc )
    {
        return rc;
    }

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E: if( 0 != portGroup ) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E: if( 1 != portGroup ) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E: if( 2 != portGroup ) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E: if( 3 != portGroup ) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                                                        break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode,
                                           &startSerdes, &maxLaneNum);
    if( GT_OK != rc )
    {
        return rc;
    }

    if( GT_FALSE == enable )
    {
        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes+laneNum].serdesExternalReg2;

        /* <rcvrd_clk0_sel> */
        rc = prvCpssHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, 11, 1, 1);
    }
    else /* GT_TRUE == enable */
    {
        for( serdes = 0 ; serdes < PRV_CPSS_LION_SERDES_NUM_CNS ; serdes++ )
        {
            if(serdes == startSerdes + laneNum)
            {
                continue;
            }

            /* Serdes External Configuration 1 Register */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdes].serdesExternalReg2;

            /* <rcvrd_clk0_sel> */
            rc = prvCpssHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, 11, 1, &regValue);
            if( GT_OK != rc )
            {
                return rc;
            }

            if( 0 == regValue )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }

        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes+laneNum].serdesExternalReg2;

        /* <rcvrd_clk0_sel> */
        rc = prvCpssHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, 11, 1, 0);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortSyncEtherRecoveryClkConfigSetForLion2
*
* DESCRIPTION:
*       Function configures the recovery clock enable/disable state and sets
*       its source portNum.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*       enable          - enable/disable recovered clock1/clock2
*                         GT_TRUE - enable
*                         GT_FALSE - disable
*       portNum         - port number.
*       laneNum         - selected SERDES lane number within port.
*                         (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3).
*                         Ranges according to port interface:
*                         SGMII/1000BaseX - 0
*                         RXAUI/HX        - 0..1
*                         XAUII           - 0..3
*                         XLG             - 0..7
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       If SERDES enabling is requesed and another SERDES in the port group
*       is already enabled, GT_BAD_STATE is returned.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSyncEtherRecoveryClkConfigSetForLion2
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    IN  GT_BOOL enable,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U32  laneNum
)
{
    GT_STATUS rc;           /* returned status */
    GT_U32  regAddr;        /* register address */
    GT_U32  regValue = 0;   /* register value */
    GT_U32  regOffset;      /* register offset */
    GT_U32  portGroup;      /* port group */
    GT_U32  serdes;         /* loop index */
    GT_U32  startSerdes;    /* start serdes index in port */
    GT_U32  serdesIdx;      /* current serdes index */
    GT_U32  maxSerdesNumber;/* maximal serdes number */
    GT_U32  maxLaneNum;     /* max serdes number per port */
    GT_U32  recoveryClkTypeU32  = recoveryClkType; /* auxiliary variable to avoid casting */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */

    /*
            Port Group <i>                    Port Group <i + 1>
         __________________                   __________________
        |                  |                 |                  |
        | port(SERDES) <x> |________     ____| port(SERDES) <z> |
        |______CLK0________|        |   |    |_______CLK0_______|
                                    |   |
         __________________         |   |     __________________
        |                  |        |   |    |                  |
        | port(SERDES) <y> |  ______|___|____| port(SERDES) <w> |
        |______CLK1________| |      |   |    |_______CLK1_______|
                    |        |      |   |
                    |        |      |   |
                    |        |      |   |
                   _|________|_    _|___|_____
                   \MUX <i+1>/     \ MUX <i> /
                    \_______/       \_______/
                        |               |
                Recovered Clock    Recovered Clock
                 Output <i+1>        Output <i>

    Note: i = 2N, where N = 0-3
    */

    rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* HGL port not supported for clock recovery */
    if(CPSS_PORT_INTERFACE_MODE_HGL_E == ifMode)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode, &startSerdes, &maxLaneNum);
    if( GT_OK != rc )
    {
        return rc;
    }

    if( laneNum >= maxLaneNum )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    serdesIdx = startSerdes + laneNum;

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E:
            if(( 0 != portGroup ) && ( 1 != portGroup ))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E:
            if(( 2 != portGroup ) && ( 3 != portGroup ))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK4_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK5_E:
            if(( 4 != portGroup ) && ( 5 != portGroup ))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK6_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK7_E:
            if(( 6 != portGroup ) && ( 7 != portGroup ))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Find the right offset for Clock0 select or Clock1 select */
    if(((portGroup == recoveryClkTypeU32) && (portGroup % 2 == 0)) ||
       ((portGroup != recoveryClkTypeU32) && (portGroup % 2 != 0)))
    {
        /* Clock0 Select */
        regOffset = 11;
    }
    else
    {
        /* Clock1 Select */
        regOffset = 15;
    }

    if( GT_FALSE == enable )
    {
        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIdx].serdesExternalReg2;

        /* <rcvrd_clk0_sel> or <rcvrd_clk1_sel> */
        rc = prvCpssHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, regOffset, 1, 1);
    }
    else /* GT_TRUE == enable */
    {
        switch (PRV_CPSS_PP_MAC(devNum)->devType)
        {

            case CPSS_LION_2_3_THREE_MINI_GOPS_DEVICES_CASES_MAC:
                maxSerdesNumber = PRV_CPSS_LION2_SERDES_NUM_CNS - 6;
                break;
            case CPSS_LION_2_3_FOUR_MINI_GOPS_DEVICES_CASES_MAC:
                maxSerdesNumber = PRV_CPSS_LION2_SERDES_NUM_CNS;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        for( serdes = 0 ; serdes < maxSerdesNumber ; serdes++ )
        {
            if ( serdes == serdesIdx )
            {
                continue;
            }

            /* Serdes External Configuration 1 Register */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdes].serdesExternalReg2;

            /* <rcvrd_clk0_sel> or <rcvrd_clk1_sel> */
            rc = prvCpssHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, regOffset, 1, &regValue);
            if( GT_OK != rc )
            {
                return rc;
            }

            if( 0 == regValue )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }

        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIdx].serdesExternalReg2;

        /* <rcvrd_clk0_sel> or <rcvrd_clk1_sel> */
        rc = prvCpssHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, regOffset, 1, 0);

        /******************************************/
        /* Clock0/Clock1 Output mux configuration */
        /******************************************/

        /* The configuration for cores 2N and 2N+1 resides in the XLG unit
           of port 0 in core 2N. */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[0].xlgExternalControlRegister;


        /* Set Recovered Clock Output <i> / <i+1> */
        if((portGroup == recoveryClkTypeU32) && (portGroup % 2 == 0))
        {
            /* Port Group Recovered Clock 0 Select*/
            regOffset = 0;
            /* Use core 2N */
            regValue = 0;
        }
        else if((portGroup != recoveryClkTypeU32) && (portGroup % 2 != 0))
        {
            /* Port Group Recovered Clock 0 Select*/
            regOffset = 0;
            /* Use core 2N+1 */
            regValue = 1;
        }
        else if((portGroup == recoveryClkTypeU32) && (portGroup % 2 != 0))
        {
            /* Port Group Recovered Clock 1 Select*/
            regOffset = 1;
            /* Use core 2N+1 */
            regValue = 1;
        }
        else
        {   /*  portGroup != recoveryClkType && portGroup % 2 == 0 */

            /* Port Group Recovered Clock 1 Select*/
            regOffset = 1;
            /* Use core 2N */
            regValue = 0;
        }

        /* The configuration for cores 2N and 2N+1 resides in the XLG unit
           of port 0 in core 2N. */

        portGroup &= ~1;

        rc = prvCpssHwPpPortGroupSetRegField(devNum, portGroup, regAddr, regOffset, 1, regValue);
    }

    return rc;
}


/*******************************************************************************
* prvCpssDxChPortSyncEtherRecoveryClkConfigSetForBobcat2
*
* DESCRIPTION:
*       Function configures the recovery clock enable/disable state and sets
*       its source portNum.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*       enable          - enable/disable recovered clock0/clock1
*                         GT_TRUE - enable
*                         GT_FALSE - disable
*       portNum         - port number.
*       laneNum         - selected SERDES lane number within port.
*                         Ranges according to port interface:
*                         SGMII/1000BaseX - 0
*                         RXAUI/HX        - 0..1
*                         XAUII           - 0..3
*                         XLG             - 0..7
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       If SERDES enabling is requesed and another SERDES in the port group
*       is already enabled, GT_BAD_STATE is returned.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSyncEtherRecoveryClkConfigSetForBobcat2
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    IN  GT_BOOL enable,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U32  laneNum
)
{
    GT_STATUS rc;           /* returned status */
    GT_U32  regAddr;        /* register address */
    GT_U32  regValue = 0;   /* register value */
    GT_U32  regOffset;      /* register offset */
    GT_U32  serdes;         /* loop index */
    GT_U32  startSerdes;    /* start serdes index in port */
    GT_U32  serdesIdx;      /* current serdes index */
    GT_U32  maxSerdesNumber;/* maximal serdes number */
    GT_U32  maxLaneNum;     /* max serdes number per port */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */

    rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
    if( GT_OK != rc )
    {
        return rc;
    }

    rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode, &startSerdes, &maxLaneNum);
    if( GT_OK != rc )
    {
        return rc;
    }

    if( laneNum >= maxLaneNum )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    serdesIdx = startSerdes + laneNum;

    /* Find the right offset for Clock0 select or Clock1 select */
    switch (recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E:
            regOffset = 15;
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E:
            regOffset = 11;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if( GT_FALSE == enable )
    {
        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIdx].serdesExternalReg2;

        /* <rcvrd_clk0_sel> or <rcvrd_clk1_sel> */
        rc = prvCpssHwPpSetRegField(devNum,  regAddr, regOffset, 1, 1);
    }
    else /* GT_TRUE == enable */
    {
        maxSerdesNumber = prvCpssDxChHwInitNumOfSerdesGet(devNum);

        for( serdes = 0 ; serdes < maxSerdesNumber ; serdes++ )
        {
            if ( serdes == serdesIdx )
            {
                continue;
            }

            /* skip not valid SERDESes in the loop */
            PRV_CPSS_DXCH_SKIP_NON_VALID_SERDES_IN_LOOP_MAC(devNum,serdes);

            /* Serdes External Configuration 1 Register */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdes].serdesExternalReg2;

            /* <rcvrd_clk0_sel> or <rcvrd_clk1_sel> */
            rc = prvCpssHwPpGetRegField(devNum,  regAddr, regOffset, 1, &regValue);
            if( GT_OK != rc )
            {
                return rc;
            }

            if( 0 == regValue )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }

        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIdx].serdesExternalReg2;

        /* <rcvrd_clk0_sel> or <rcvrd_clk1_sel> */
        rc = prvCpssHwPpSetRegField(devNum,  regAddr, regOffset, 1, 0);
    }

    return rc;
}


/*******************************************************************************
* internal_cpssDxChPortSyncEtherRecoveryClkConfigSet
*
* DESCRIPTION:
*       Function configures the recovery clock enable/disable state and sets
*       its source portNum.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*       enable          - enable/disable recovered clock1/clock2
*                         GT_TRUE - enable
*                         GT_FALSE - disable
*       portNum         - port number.
*                         For xCat,xCat3 & xCat2 valid ports for configuration:
*                         Giga Ports: 0,4,8,12,16,20
*                         Stacking Ports: 24,25,26,27
*       laneNum         - selected SERDES lane number within port.
*                         (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                         Ranges according to port interface:
*                         SGMII/1000BaseX - 0
*                         RXAUI/HX        - 0..1
*                         XAUII           - 0..3
*                         XLG             - 0..7
*                         10GBASE-R       - 0
*                         20GBASE-R2      - 0..1
*                         XLAUI           - 0..3
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherRecoveryClkConfigSet
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    IN  GT_BOOL enable,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U32  laneNum
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regOffset;  /* register offset */
    GT_U32  portGroup;  /* port group */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        return prvCpssDxChPortSyncEtherRecoveryClkConfigSetForLion(devNum,
                                                                   recoveryClkType,
                                                                   enable,
                                                                   portNum,
                                                                   laneNum);
    }

    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        return prvCpssDxChPortSyncEtherRecoveryClkConfigSetForLion2(devNum,
                                                                    recoveryClkType,
                                                                    enable,
                                                                    portNum,
                                                                    laneNum);
    }

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
       (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        return prvCpssDxChPortSyncEtherRecoveryClkConfigSetForBobcat2(devNum,
                                                                      recoveryClkType,
                                                                      enable,
                                                                      portNum,
                                                                      laneNum);
    }

    /* Check port number */
    switch (portNum)
    {
        case 0:     /* Group index for Giga ports = 4-9 */
        case 4:
        case 8:
        case 12:
        case 16:
        case 20:
            portGroup = (4 + portNum / 4);
            break;
        case 24:    /* Group index for Stacking ports = 0-3 */
        case 25:
        case 26:
        case 27:
            portGroup = (portNum - 24);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* calculate register offset */
    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E:
            regOffset = 10;
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E:
            regOffset = 14;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* set register value */
    regValue = (enable == GT_TRUE) ? portGroup : 0xF;

    /* Extended Global Control2 register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

    /* set <rcvr_clk0_ctrl> or <rcvr_clk1_ctrl> in extended global register2 */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, regOffset, 4, regValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkConfigSet
*
* DESCRIPTION:
*       Function configures the recovery clock enable/disable state and sets
*       its source portNum.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*       enable          - enable/disable recovered clock1/clock2
*                         GT_TRUE - enable
*                         GT_FALSE - disable
*       portNum         - port number.
*                         For xCat,xCat3 & xCat2 valid ports for configuration:
*                         Giga Ports: 0,4,8,12,16,20
*                         Stacking Ports: 24,25,26,27
*       laneNum         - selected SERDES lane number within port.
*                         (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                         Ranges according to port interface:
*                         SGMII/1000BaseX - 0
*                         RXAUI/HX        - 0..1
*                         XAUII           - 0..3
*                         XLG             - 0..7
*                         10GBASE-R       - 0
*                         20GBASE-R2      - 0..1
*                         XLAUI           - 0..3
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkConfigSet
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    IN  GT_BOOL enable,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U32  laneNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherRecoveryClkConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, recoveryClkType, enable, portNum, laneNum));

    rc = internal_cpssDxChPortSyncEtherRecoveryClkConfigSet(devNum, recoveryClkType, enable, portNum, laneNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, recoveryClkType, enable, portNum, laneNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortSyncEtherRecoveryClkConfigGetForLion
*
* DESCRIPTION:
*       Function gets the recovery clock enable/disable state and its source
*       portNum.
*
* APPLICABLE DEVICES:
*        Lion;
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                         GT_TRUE -  enabled
*                         GT_FALSE - disbled
*       portNumPtr      - (pointer to) port number.
*       laneNumPtr      - (pointer to) selected SERDES lane number within port.
*                         (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad port interface state
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSyncEtherRecoveryClkConfigGetForLion
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    OUT GT_BOOL  *enablePtr,
    OUT GT_PHYSICAL_PORT_NUM    *portNumPtr,
    OUT GT_U32   *laneNumPtr
)
{
    GT_STATUS rc;       /* returned status */
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  portGroup;  /* port group */
    GT_U32   localPort;  /* local port - support multi-port-groups device */
    GT_U32  serdes;     /* loop index */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* port interface mode */

    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E: portGroup = 0;
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E: portGroup = 1;
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E: portGroup = 2;
                                                        break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E: portGroup = 3;
                                                        break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    for( serdes = 0 ; serdes < PRV_CPSS_LION_SERDES_NUM_CNS ; serdes++ )
    {
        /* Serdes External Configuration 1 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdes].serdesExternalReg2;

        /* <rcvrd_clk0_sel> */
        rc = prvCpssHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, 11, 1, &regValue);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( 0 == regValue )
        {
            break;
        }
    }

    if( PRV_CPSS_LION_SERDES_NUM_CNS == serdes )
    {
        *enablePtr = GT_FALSE;
    }
    else /* SERDES which was configured for recovery clock was found */
    {
        /* find port interface to calculate lane number */

        /* check is SGMII port */
        localPort = (GT_U8)(serdes/2);
        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
        rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
        if( (GT_OK == rc) &&  ((CPSS_PORT_INTERFACE_MODE_SGMII_E == ifMode)
                                || (CPSS_PORT_INTERFACE_MODE_1000BASE_X_E == ifMode)))
        {
            if( localPort != serdes*2 )
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            *enablePtr = GT_TRUE;
            *laneNumPtr = 0 ;
            return GT_OK;
        }

        /* check is RXAUI port */
        localPort = (GT_U8)(serdes/2);
        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
        rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
        if( (GT_OK == rc) &&  ((CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode)
                                || (CPSS_PORT_INTERFACE_MODE_HX_E == ifMode)))
        {
            *enablePtr = GT_TRUE;
            *laneNumPtr = serdes - localPort*2;
            return GT_OK;
        }

        /* check is XAUI port */
        localPort = (GT_U8)((serdes/4)*2);
        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
        rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
        if( (GT_OK == rc) &&  (CPSS_PORT_INTERFACE_MODE_XGMII_E == ifMode) )
        {
            *enablePtr = GT_TRUE;
            *laneNumPtr = serdes - localPort*2;
            return GT_OK;
        }

        /* check is XLG port */
        if( serdes >= 16 && serdes < 24 )
        {
            localPort = 10;
            *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
            rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
            if( (GT_OK == rc) &&  (CPSS_PORT_INTERFACE_MODE_XLG_E == ifMode) )
            {
                *enablePtr = GT_TRUE;
                *laneNumPtr = serdes - 16;
                return GT_OK;
            }
        }

        /* no port interface to lane number match found */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortSyncEtherRecoveryClkConfigGetForLion2
*
* DESCRIPTION:
*       Function gets the recovery clock enable/disable state and its source
*       portNum.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                         GT_TRUE -  enabled
*                         GT_FALSE - disbled
*       portNumPtr      - (pointer to) port number.
*       laneNumPtr      - (pointer to) selected SERDES lane number within port.
*                         (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad port interface state
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSyncEtherRecoveryClkConfigGetForLion2
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    OUT GT_BOOL  *enablePtr,
    OUT GT_PHYSICAL_PORT_NUM    *portNumPtr,
    OUT GT_U32   *laneNumPtr
)
{
    GT_STATUS rc;       /* returned status */
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  portGroup;  /* port group */
    GT_U32  regOffset;  /* register offset */
    GT_U32  regOffsetMux;  /* register offset for clock mux*/
    GT_U32   localPort;  /* local port - support multi-port-groups device */
    GT_U32  serdes;     /* loop index */
    GT_U32  maxSerdesNumber;/* maximal serdes number */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* port interface mode */
    CPSS_PORT_SPEED_ENT             speed; /* current port speed */


    /*
            Port Group <i>                    Port Group <i + 1>
         __________________                   __________________
        |                  |                 |                  |
        | port(SERDES) <x> |________     ____| port(SERDES) <z> |
        |______CLK0________|        |   |    |_______CLK0_______|
                                    |   |
         __________________         |   |     __________________
        |                  |        |   |    |                  |
        | port(SERDES) <y> |  ______|___|____| port(SERDES) <w> |
        |______CLK1________| |      |   |    |_______CLK1_______|
                    |        |      |   |
                    |        |      |   |
                    |        |      |   |
                   _|________|_    _|___|_____
                   \MUX <i+1>/     \ MUX <i> /
                    \_______/       \_______/
                        |               |
                Recovered Clock    Recovered Clock
                 Output <i+1>        Output <i>

    Note: i = 2N, where N = 0-3
    */

    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK4_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK6_E:
            regOffset = 11;
            regOffsetMux = 0;
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK5_E:
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK7_E:
            regOffset = 15;
            regOffsetMux = 1;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* The configuration for cores 2N and 2N+1 resides in the XLG unit
       of port 0 in core 2N. */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[0].xlgExternalControlRegister;

    /* portGroup should be even (2N) */
    portGroup = (GT_U32)recoveryClkType & ~1;

    /* read XLG external control register */
    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroup, regAddr, regOffsetMux, 1, &regValue);

    /* check current MUX configuration in order to choose appropriate portGroup */
    if(regValue != 0) /* 2N+1 case */
    {
        ++portGroup;
    }

    /* get actual number of SERDESes */
    switch (PRV_CPSS_PP_MAC(devNum)->devType)
    {

        case CPSS_LION_2_3_THREE_MINI_GOPS_DEVICES_CASES_MAC:
            maxSerdesNumber = PRV_CPSS_LION2_SERDES_NUM_CNS - 6;
            break;
        case CPSS_LION_2_3_FOUR_MINI_GOPS_DEVICES_CASES_MAC:
            maxSerdesNumber = PRV_CPSS_LION2_SERDES_NUM_CNS;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    for( serdes = 0 ; serdes < maxSerdesNumber ; serdes++ )
    {
        /* Serdes External Configuration 1 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdes].serdesExternalReg2;

        /* <rcvrd_clk0_sel> or <rcvrd_clk1_sel> */
        rc = prvCpssHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, regOffset, 1, &regValue);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( 0 == regValue )
        {
            break;
        }
    }

    if( maxSerdesNumber == serdes )
    {
        *enablePtr = GT_FALSE;
    }
    else /* SERDES which was configured for recovery clock was found */
    {
        /* find port interface to calculate lane number */

        /* check is SGMII / 10G-KR port */
        localPort = (GT_U8)(serdes - 2*(serdes/6));
        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
        rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
        rc |= cpssDxChPortSpeedGet(devNum, *portNumPtr, &speed);

        if( (GT_OK == rc) &&  ((CPSS_PORT_INTERFACE_MODE_SGMII_E == ifMode)
                                || (CPSS_PORT_INTERFACE_MODE_1000BASE_X_E == ifMode)
                                || ((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) && (CPSS_PORT_SPEED_10000_E == speed))
                                || ((CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode) && (CPSS_PORT_SPEED_10000_E == speed))
                                || ((CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode) && (CPSS_PORT_SPEED_11800_E == speed))
                                || ((CPSS_PORT_INTERFACE_MODE_XHGS_SR_E == ifMode) && (CPSS_PORT_SPEED_11800_E == speed))
                               ))
        {
            *enablePtr = GT_TRUE;
            *laneNumPtr = 0 ;
            return GT_OK;
        }

        /* check is RXAUI / 20G-KR2 port */
        /*  SERDES  |   PORT
             0-1    |    0
             2-3    |    2
             6-7    |    4
             8-9    |    6
             12-13  |    8
             14-15  |    10
             18-19  |    9
             20-21  |    11
        */
        if(serdes < 18)
        {
            localPort = (GT_U8)((serdes&~1) - 2*(serdes/6));
        }
        else if((serdes == 18) || (serdes == 19))
        {
            localPort = 9;
        }
        else
        {   /* serdes == 20 || serdes == 21*/
            localPort = 11;
        }

        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
        rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
        rc |= cpssDxChPortSpeedGet(devNum, *portNumPtr, &speed);

        if( (GT_OK == rc) &&  ((CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode)
                                || (CPSS_PORT_INTERFACE_MODE_HX_E == ifMode)
                                || ((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) && (CPSS_PORT_SPEED_20000_E == speed))
                                || ((CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode) && (CPSS_PORT_SPEED_20000_E == speed))))
        {
            *enablePtr = GT_TRUE;
            *laneNumPtr = serdes & 1;
            return GT_OK;
        }

        /* check is XAUI / 40G-KR4 port */
        /*  SERDES  |   PORT
             0-3    |    0
             6-9    |    4
             12-15  |    8
             18-21  |    9
        */
        if(serdes < 18)
        {
            localPort = (GT_U8)((serdes/6)*4);
        }
        else
        {
            localPort = 9;
        }
        *portNumPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum, portGroup, localPort);
        rc = cpssDxChPortInterfaceModeGet(devNum, *portNumPtr, &ifMode);
        rc |= cpssDxChPortSpeedGet(devNum, *portNumPtr, &speed);

        if( (GT_OK == rc) && ((CPSS_PORT_INTERFACE_MODE_XGMII_E == ifMode)
                             ||((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) && (CPSS_PORT_SPEED_40000_E == speed))
                             ||((CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode) && (CPSS_PORT_SPEED_40000_E == speed))
                             || ((CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode) && (CPSS_PORT_SPEED_47200_E == speed))
                             || ((CPSS_PORT_INTERFACE_MODE_XHGS_SR_E == ifMode) && (CPSS_PORT_SPEED_47200_E == speed))
                              ))
        {
            *enablePtr = GT_TRUE;
            *laneNumPtr = (serdes - 2*(serdes/6)) & 3;
            return GT_OK;
        }

        /* no port interface to lane number match found */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortSyncEtherRecoveryClkConfigGetForBobcat2
*
* DESCRIPTION:
*       Function gets the recovery clock enable/disable state and its source
*       portNum.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                         GT_TRUE -  enabled
*                         GT_FALSE - disbled
*       portNumPtr      - (pointer to) port number.
*       laneNumPtr      - (pointer to) selected SERDES lane number within port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       If SERDES enabling is requesed and another SERDES in the port group
*       is already enabled, GT_BAD_STATE is returned.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSyncEtherRecoveryClkConfigGetForBobcat2
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    OUT GT_BOOL  *enablePtr,
    OUT GT_PHYSICAL_PORT_NUM    *portNumPtr,
    OUT GT_U32   *laneNumPtr
)
{
    GT_STATUS rc;           /* returned status */
    GT_U32  regAddr;        /* register address */
    GT_U32  regValue = 0;   /* register value */
    GT_U32  regOffset;      /* register offset */
    GT_U32  serdes;         /* loop index */
    GT_U32  startSerdes;    /* start serdes index in port */
    GT_U32  maxLaneNum;     /* max serdes number per port */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */
    GT_PHYSICAL_PORT_NUM portNum; /* loop iterator */
    GT_BOOL isValid;        /* port mapping status */

    /* Find the right offset for Clock0 select or Clock1 select */
    switch (recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E:
            regOffset = 15;
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E:
            regOffset = 11;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    for (portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = cpssDxChPortPhysicalPortMapIsValidGet(devNum, portNum, &isValid);
            if((rc != GT_OK) || (isValid != GT_TRUE))
                continue;
        }
        else
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
        }

        rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
        if((ifMode == CPSS_PORT_INTERFACE_MODE_NA_E) || ( GT_OK != rc ))
        {
            continue;
        }

        rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode, &startSerdes, &maxLaneNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        for( serdes = startSerdes; serdes < (startSerdes + maxLaneNum); serdes++ )
        {
            /* Serdes External Configuration 1 Register */
            regAddr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdes].serdesExternalReg2;

            /* <rcvrd_clk0_sel> or <rcvrd_clk1_sel> */
            rc = prvCpssHwPpGetRegField(devNum,  regAddr, regOffset, 1, &regValue);
            if( GT_OK != rc )
            {
                return rc;
            }

            if( 0 == regValue )
            {
                *enablePtr = GT_TRUE;
                *laneNumPtr = serdes - startSerdes;
                *portNumPtr = portNum;

                return GT_OK;
            }
        }
    }

    *enablePtr = GT_FALSE;
    *laneNumPtr = 0;
    *portNumPtr = CPSS_MAX_PORTS_NUM_CNS;

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChPortSyncEtherRecoveryClkConfigGet
*
* DESCRIPTION:
*       Function gets the recovery clock enable/disable state and its source
*       portNum.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                         GT_TRUE -  enabled
*                         GT_FALSE - disbled
*       portNumPtr      - (pointer to) port number.
*       laneNumPtr      - (pointer to) selected SERDES lane number within port.
*                         (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on bad state of register
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherRecoveryClkConfigGet
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    OUT GT_BOOL  *enablePtr,
    OUT GT_PHYSICAL_PORT_NUM    *portNumPtr,
    OUT GT_U32   *laneNumPtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_U32  regOffset;  /* register offset */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(portNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(laneNumPtr);

    if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        return prvCpssDxChPortSyncEtherRecoveryClkConfigGetForLion(
                            devNum,recoveryClkType,enablePtr,portNumPtr,laneNumPtr);
    }

    if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        return prvCpssDxChPortSyncEtherRecoveryClkConfigGetForLion2(
                            devNum,recoveryClkType,enablePtr,portNumPtr,laneNumPtr);
    }

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
       (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        return prvCpssDxChPortSyncEtherRecoveryClkConfigGetForBobcat2(
                            devNum,recoveryClkType,enablePtr,portNumPtr,laneNumPtr);
    }

    /* calculate register offset */
    switch(recoveryClkType)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E:
            regOffset = 10;
            break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E:
            regOffset = 14;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Extended Global Control2 register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;

    /* get <rcvr_clk0_ctrl> or <rcvr_clk1_ctrl> in extended global register2 */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, regOffset, 4, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (regValue < 10)
    {
        *enablePtr = GT_TRUE;
    }
    else if (regValue == 0xF)
    {
        *enablePtr = GT_FALSE;
        return GT_OK;
    }
    else
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

    /* recovery clock output pin port group number */
    switch (regValue)
    {
        /* Stacking ports */
        case 0:
        case 1:
        case 2:
        case 3:
            *portNumPtr = (GT_U8)(24 + regValue);
            break;
        /* Giga ports */
        default:
            *portNumPtr = (GT_U8)((regValue - 4) * 4);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkConfigGet
*
* DESCRIPTION:
*       Function gets the recovery clock enable/disable state and its source
*       portNum.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                         GT_TRUE -  enabled
*                         GT_FALSE - disbled
*       portNumPtr      - (pointer to) port number.
*       laneNumPtr      - (pointer to) selected SERDES lane number within port.
*                         (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on bad state of register
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkConfigGet
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType,
    OUT GT_BOOL  *enablePtr,
    OUT GT_PHYSICAL_PORT_NUM    *portNumPtr,
    OUT GT_U32   *laneNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherRecoveryClkConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, recoveryClkType, enablePtr, portNumPtr, laneNumPtr));

    rc = internal_cpssDxChPortSyncEtherRecoveryClkConfigGet(devNum, recoveryClkType, enablePtr, portNumPtr, laneNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, recoveryClkType, enablePtr, portNumPtr, laneNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
*
* DESCRIPTION:
*       Function sets recovery clock divider bypass enable.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       enable      - enable/disable recovery clock divider bypass enable
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, enable
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
(
    IN  GT_U8  devNum,
    IN  GT_BOOL enable
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    /* set register value */
    regValue = BOOL2BIT_MAC(enable);

    /* Extended Global Control2 register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

    /* set <sd_clkdiv_bypass_en> in extended global register */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 26, 1, regValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
*
* DESCRIPTION:
*       Function sets recovery clock divider bypass enable.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*       enable      - enable/disable recovery clock divider bypass enable
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, enable
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
(
    IN  GT_U8  devNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
*
* DESCRIPTION:
*       Function gets recovery clock divider bypass status.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) recovery clock divider bypass state
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
(
    IN  GT_U8  devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  regValue;   /* register value */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Extended Global Control2 register address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;

    /* set <sd_clkdiv_bypass_en> in extended global register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 26, 1, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set return value */
    *enablePtr = BIT2BOOL_MAC(regValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
*
* DESCRIPTION:
*       Function gets recovery clock divider bypass status.
*
* APPLICABLE DEVICES:
*        xCat.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) recovery clock divider bypass state
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
(
    IN  GT_U8  devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherRecoveryClkDividerValueSet
*
* DESCRIPTION:
*       Function sets recovery clock divider value.
*
* APPLICABLE DEVICES:
*        xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*                     10GBASE-R       - 0
*                     20GBASE-R2      - 0..1
*                     XLAUI           - 0..3
*       clockSelect - clock select - each SERDES may be connected.
*                     (APPLICABLE DEVICES: Lion2, Bobcat2, Caelum, Bobcat3)
*       value       - recovery clock divider value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum, lane number or clockSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherRecoveryClkDividerValueSet
(
    IN  GT_U8                                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                                portNum,
    IN  GT_U32                                              laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT clockSelect,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  value
)
{
    GT_STATUS   rc;         /* return code      */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register data    */
    GT_U32      regOffset;  /* offset in register */
    GT_U32      portGroup;  /* port group */
    GT_U32      serdesNum;  /* SERDES number */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */
    GT_U32      startSerdes, /* first serdes used by port */
                maxLaneNum;  /* number of serdeses used by port now */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E );

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);


    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_FE_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    portGroup = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
       (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode,
                                               &startSerdes, &maxLaneNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( laneNum >= maxLaneNum )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Serdes External Configuration 2 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes +
                                                        laneNum].serdesExternalReg2;

        switch (clockSelect)
        {
            case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E:
                regOffset = 12;
                break;
            case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E:
                regOffset = 8;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else if((CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)||
       (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode,
                                               &startSerdes, &maxLaneNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes+laneNum].
                                                                serdesExternalReg2;

        if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            regOffset = 8;
        }
        else /* Lion2 */
        {
            switch(clockSelect)
            {
                case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E:
                    regOffset = 12;
                    break;
                case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E:
                    regOffset = 8;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }
    else /* xCat2 */
    {
        /* get SERDES number */
        switch (portNum)
        {
            case 0:
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                serdesNum = portNum/4;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
                serdesNum = (portNum - 18);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Serdes External Configuration 3 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg3;

        regOffset = 1;
    }

    switch(value)
    {
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_1_E:    regValue = 0;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_E:    regValue = 1;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_3_E:    regValue = 2;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_4_E:    regValue = 3;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_5_E:    regValue = 4;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_8_E:    regValue = 5;
                                                                    break;
        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_E:

            if((CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily) ||
               (PRV_CPSS_SIP_5_CHECK_MAC(devNum)) ||
               (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            else
            {
                regValue = 6;
            }
            break;

        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_5_E:

            if((CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily) ||
               (PRV_CPSS_SIP_5_CHECK_MAC(devNum)) ||
               (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
            {
                regValue = 6;
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;

        case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_5_E:  regValue = 7;
                                                                    break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    }

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    /* <rcvrd_clk0_div> or <rcvrd_clk1_div> */
    rc = prvCpssHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, regOffset, 3, regValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerValueSet
*
* DESCRIPTION:
*       Function sets recovery clock divider value.
*
* APPLICABLE DEVICES:
*        xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*                     10GBASE-R       - 0
*                     20GBASE-R2      - 0..1
*                     XLAUI           - 0..3
*       clockSelect - clock select - each SERDES may be connected.
*                     (APPLICABLE DEVICES: Lion2, Bobcat2, Caelum, Bobcat3)
*       value       - recovery clock divider value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum, lane number or clockSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerValueSet
(
    IN  GT_U8                                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                                portNum,
    IN  GT_U32                                              laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT clockSelect,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  value
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherRecoveryClkDividerValueSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, laneNum, clockSelect, value));

    rc = internal_cpssDxChPortSyncEtherRecoveryClkDividerValueSet(devNum, portNum, laneNum, clockSelect, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, laneNum, clockSelect, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherRecoveryClkDividerValueGet
*
* DESCRIPTION:
*       Function gets recovery clock divider value.
*
* APPLICABLE DEVICES:
*        xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*                     10GBASE-R       - 0
*                     20GBASE-R2      - 0..1
*                     XLAUI           - 0..3
*       clockSelect - clock select - each SERDES may be connected.
*                     (APPLICABLE DEVICES: Lion2, Bobcat2, Caelum, Bobcat3)
*
* OUTPUTS:
*       valuePtr   - (pointer to) recovery clock divider value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum, lane number or clockSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherRecoveryClkDividerValueGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                                portNum,
    IN  GT_U32                                              laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT clockSelect,
    OUT CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  *valuePtr
)
{
    GT_STATUS   rc;         /* return code      */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register data    */
    GT_U32      regOffset;  /* offset in register */
    GT_U32      portGroup;  /* port group */
    GT_U32      serdesNum;  /* SERDES number */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */
    GT_U32      startSerdes, /* first serdes used by port */
                maxLaneNum;  /* number of serdeses used by port now */
    GT_U32 portMacNum; /* MAC number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E );

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);


    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_FE_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    portGroup = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
       (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode,
                                               &startSerdes, &maxLaneNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( laneNum >= maxLaneNum )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Serdes External Configuration 2 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes +
                                                        laneNum].serdesExternalReg2;

        switch (clockSelect)
        {
            case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E:
                regOffset = 12;
                break;
            case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E:
                regOffset = 8;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else if((CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)||
       (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode,
                                               &startSerdes, &maxLaneNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        /* Serdes External Configuration 1 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes+laneNum].
                                                                serdesExternalReg2;

        if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            regOffset = 8;
        }
        else /* Lion2 */
        {
            switch(clockSelect)
            {
                case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E:
                    regOffset = 12;
                    break;
                case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E:
                    regOffset = 8;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }
    else /* xCat2 */
    {
        /* get SERDES number */
        switch (portNum)
        {
            case 0:
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                serdesNum = portNum/4;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
                serdesNum = (portNum - 18);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Serdes External Configuration 3 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg3;

        regOffset = 1;
    }

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    /* <rcvrd_clk0_div> or <rcvrd_clk1_div> */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, regOffset,
                                            3, &regValue);
    if( GT_OK != rc )
    {
        return rc;
    }

    switch(regValue)
    {
        case 0: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_1_E;
                break;
        case 1: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_E;
                break;
        case 2: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_3_E;
                break;
        case 3: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_4_E;
                break;
        case 4: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_5_E;
                break;
        case 5: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_8_E;
                break;
        case 6: if((CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily) ||
                   (PRV_CPSS_SIP_5_CHECK_MAC(devNum)) ||
                   (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
                {
                    *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_5_E;
                }
                else
                {
                    *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_E;
                }
                break;
        case 7: *valuePtr = CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_5_E;
                break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerValueGet
*
* DESCRIPTION:
*       Function gets recovery clock divider value.
*
* APPLICABLE DEVICES:
*        xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*                     10GBASE-R       - 0
*                     20GBASE-R2      - 0..1
*                     XLAUI           - 0..3
*       clockSelect - clock select - each SERDES may be connected.
*                     (APPLICABLE DEVICES: Lion2, Bobcat2, Caelum, Bobcat3)
*
* OUTPUTS:
*       valuePtr   - (pointer to) recovery clock divider value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum, lane number or clockSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveryClkDividerValueGet
(
    IN  GT_U8                                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                                portNum,
    IN  GT_U32                                              laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT clockSelect,
    OUT CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherRecoveryClkDividerValueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, laneNum, clockSelect, valuePtr));

    rc = internal_cpssDxChPortSyncEtherRecoveryClkDividerValueGet(devNum, portNum, laneNum, clockSelect, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, laneNum, clockSelect, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
*
* DESCRIPTION:
*       Function sets Recovered Clock Automatic Masking enabling.
*
* APPLICABLE DEVICES:
*        xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*                     10GBASE-R       - 0
*                     20GBASE-R2      - 0..1
*                     XLAUI           - 0..3
*       clockSelect - clock select - each SERDES may be connected.
*                     (APPLICABLE DEVICES: Lion2, Bobcat2, Caelum, Bobcat3).
*       enable      - enable/disable Recovered Clock Automatic Masking
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum, lane number or clockSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U32  laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT clockSelect,
    IN  GT_BOOL enable
)
{
    GT_STATUS   rc;         /* return code      */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register data    */
    GT_U32      regOffset = 0;  /* register offset  */
    GT_U32      portGroup;  /* port group */
    GT_U32      serdesNum;  /* SERDES number */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */
    GT_U32      portMacNum;  /* MAC number */
    GT_U32      startSerdes, /* first serdes used by port */
                maxLaneNum;  /* number of serdeses used by port now */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E );

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_FE_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    portGroup = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
       (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode,
                                               &startSerdes, &maxLaneNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( laneNum >= maxLaneNum )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Serdes External Configuration 2 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes +
                                                        laneNum].serdesExternalReg3;

        switch (clockSelect)
        {
            case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E:
                regOffset = 1;
                break;
            case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E:
                regOffset = 0;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else if((CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)||
       (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode,
                                               &startSerdes, &maxLaneNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        /* Serdes External Configuration 2 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes+laneNum].
                                                                serdesExternalReg3;

        if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            regOffset = 0;
        }
        else /* Lion2 */
        {
            switch(clockSelect)
            {
                case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E:
                    regOffset = 1;
                    break;
                case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E:
                    regOffset = 0;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }
    else /* xCat2 */
    {
        /* get SERDES number */
        switch (portNum)
        {
            case 0:
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                serdesNum = portNum/4;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
                serdesNum = (portNum - 18);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Serdes External Configuration 3 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg3;
    }

    /* set register value */
    regValue = BOOL2BIT_MAC(enable);

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    /* <rcvrd_clk0_auto_mask> or <rcvrd_clk1_auto_mask> */
    rc = prvCpssHwPpPortGroupSetRegField(devNum,  portGroup, regAddr, regOffset, 1, regValue);

    return rc;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
*
* DESCRIPTION:
*       Function sets Recovered Clock Automatic Masking enabling.
*
* APPLICABLE DEVICES:
*        xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*                     10GBASE-R       - 0
*                     20GBASE-R2      - 0..1
*                     XLAUI           - 0..3
*       clockSelect - clock select - each SERDES may be connected.
*                     (APPLICABLE DEVICES: Lion2, Bobcat2, Caelum, Bobcat3).
*       enable      - enable/disable Recovered Clock Automatic Masking
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum, lane number or clockSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U32  laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT clockSelect,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, laneNum, clockSelect, enable));

    rc = internal_cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet(devNum, portNum, laneNum, clockSelect, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, laneNum, clockSelect, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
*
* DESCRIPTION:
*       Function gets Recovered Clock Automatic Masking status.
*
* APPLICABLE DEVICES:
*        xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*                     10GBASE-R       - 0
*                     20GBASE-R2      - 0..1
*                     XLAUI           - 0..3
*       clockSelect - clock select - each SERDES may be connected.
*                     (APPLICABLE DEVICES: Lion2, Bobcat2, Caelum, Bobcat3)
*
* OUTPUTS:
*       enablePtr   - (pointer to) Recovered Clock Automatic Masking state
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum, lane number or clockSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U32  laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT clockSelect,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS   rc;         /* return code      */
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register data    */
    GT_U32      regOffset = 0;  /* register offset  */
    GT_U32      portGroup;  /* port group */
    GT_U32      serdesNum;  /* SERDES number */
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* interface mode */
    GT_U32      portMacNum;  /* MAC number */
    GT_U32      startSerdes, /* first serdes used by port */
                maxLaneNum;  /* number of serdeses used by port now */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E );

    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portMacNum) == PRV_CPSS_PORT_FE_E)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);

    portGroup = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum) ||
       (CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode,
                                               &startSerdes, &maxLaneNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( laneNum >= maxLaneNum )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Serdes External Configuration 2 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes +
                                                        laneNum].serdesExternalReg3;

        switch (clockSelect)
        {
            case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E:
                regOffset = 1;
                break;
            case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E:
                regOffset = 0;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else if((CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)||
       (CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily))
    {
        rc = prvCpssDxChPortSyncEtherPortLaneCheck(devNum, portNum, laneNum, &ifMode);
        if( GT_OK != rc )
        {
            return rc;
        }

        rc = prvCpssDxChPortIfModeSerdesNumGet(devNum, portNum, ifMode,
                                               &startSerdes, &maxLaneNum);
        if( GT_OK != rc )
        {
            return rc;
        }

        /* Serdes External Configuration 2 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes +
                                                        laneNum].serdesExternalReg3;

        if(CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            regOffset = 0;
        }
        else /* Lion2 */
        {
            switch(clockSelect)
            {
                case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E:
                    regOffset = 1;
                    break;
                case CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E:
                    regOffset = 0;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }
    else /* xCat2 */
    {
        /* get SERDES number */
        switch (portNum)
        {
            case 0:
            case 4:
            case 8:
            case 12:
            case 16:
            case 20:
                serdesNum = portNum/4;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
                serdesNum = (portNum - 18);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* Serdes External Configuration 3 Register */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg3;
    }

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    /* <rcvrd_clk0_auto_mask> or <rcvrd_clk1_auto_mask>*/
    rc = prvCpssHwPpPortGroupGetRegField(devNum,  portGroup, regAddr, regOffset, 1, &regValue);
    if( GT_OK != rc )
    {
        return rc;
    }

    /* set return value */
    *enablePtr = BIT2BOOL_MAC(regValue);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
*
* DESCRIPTION:
*       Function gets Recovered Clock Automatic Masking status.
*
* APPLICABLE DEVICES:
*        xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*                     For xCat2 valid ports for configuration:
*                     Giga Ports: 0,4,8,12,16,20
*                     Stacking Ports: 24,25,26,27
*       laneNum     - selected SERDES lane number within port.
*                     (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*                     Ranges according to port interface:
*                     SGMII/1000BaseX - 0
*                     RXAUI/HX        - 0..1
*                     XAUII           - 0..3
*                     XLG             - 0..7
*                     10GBASE-R       - 0
*                     20GBASE-R2      - 0..1
*                     XLAUI           - 0..3
*       clockSelect - clock select - each SERDES may be connected.
*                     (APPLICABLE DEVICES: Lion2, Bobcat2, Caelum, Bobcat3)
*
* OUTPUTS:
*       enablePtr   - (pointer to) Recovered Clock Automatic Masking state
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum, lane number or clockSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    IN  GT_U32  laneNum,
    IN  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT clockSelect,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, laneNum, clockSelect, enablePtr));

    rc = internal_cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet(devNum, portNum, laneNum, clockSelect, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, laneNum, clockSelect, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
