/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortSyncEther.h
*
* DESCRIPTION:
*       CPSS definition for Sync-E (Synchronous Ethernet)
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*******************************************************************************/

#ifndef __cpssDxChPortSyncEther
#define __cpssDxChPortSyncEther

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
* typedef: enum CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT
*
* Description: Enumeration of PLL reference clock.
*
* Enumerations:
*  CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_OSCILLATOR_E - Oscillator clock
*  CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_RECOVERED_E  - Recovered clock
*
* Comments:
*         None.
*/
typedef enum
{
    CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_OSCILLATOR_E,
    CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_RECOVERED_E
}CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT;

/*
* typedef: enum CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT
*
* Description: Enumeration of physical recovery clock output pins.
*
* Enumerations:
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E - clock output pin0 or
*                                              clock output for port group 0.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E - clock output pin1 or
*                                              clock output for port group 1.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E - clock output for port group 2.
*                                              (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3).
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E - clock output for port group 3.
*                                              (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3).
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK4_E - clock output for port group 4.
*                                              (APPLICABLE DEVICES: Lion2; Bobcat2; Caelum; Bobcat3).
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK5_E - clock output for port group 5.
*                                              (APPLICABLE DEVICES: Lion2; Bobcat2; Caelum; Bobcat3).
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK6_E - clock output for port group 6.
*                                              (APPLICABLE DEVICES: Lion2; Bobcat2; Caelum; Bobcat3).
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK7_E - clock output for port group 7.
*                                              (APPLICABLE DEVICES: Lion2; Bobcat2; Caelum; Bobcat3).
*
* Comments:
*         None.
*/
typedef enum
{
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK0_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK1_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK2_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK3_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK4_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK5_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK6_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK7_E
}CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT;

/*
* typedef: enum CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT
*
* Description: Enumeration of recovery clock divider values.
*
* Enumerations:
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_1_E - clock divided by 1.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_E - clock divided by 2.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_3_E - clock divided by 3.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_4_E - clock divided by 4.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_5_E - clock divided by 5.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_8_E - clock divided by 8.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_E - clock divided by 16.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_5_E - clock divided by 2.5.
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_5_E - clock divided by 16.5.
*                                              (APPLICABLE DEVICES: Lion2; Bobcat2; Caelum; Bobcat3).
*
* Comments:
*         None.
*/
typedef enum
{
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_1_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_3_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_4_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_5_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_8_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_2_5_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_16_5_E
}CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT;

/*
* typedef: enum CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT
*
* Description: Enumeration of recovered clock select bus.
*
* Enumerations:
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E - clock0 bus 
*  CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E - clock1 bus 
*
* Comments:
*         None.
*/
typedef enum
{
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_0_E,
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_1_E
}CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLOCK_SELECT_ENT;

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortSyncEther */

