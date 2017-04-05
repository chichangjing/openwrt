/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortEee.h
*
* DESCRIPTION:
*       CPSS DxCh Port's IEEE 802.3az Energy Efficient Ethernet (EEE) Support.
*       NOTEs:
*       1. The device support relevant when the PHY works in EEE slave mode.
*          When the PHY works in EEE master mode, the device is not involved in
*          EEE processing.
*       2. The feature not relevant when port is not 'tri-speed'
*       3. LPI is short for 'Low Power Idle'
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __cpssDxChPortEee
#define __cpssDxChPortEee

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 * typedef: enum CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT
 *
 * Description: Enumeration of EEE manual modes.
 *
 * Enumerations:
 *   CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_DISABLED_E - manual mode is disabled
 *   CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENABLED_E  - manual mode is enabled (but not 'forced')
 *   CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_FORCED_E   - manual mode is enabled and
 *          force transmitting of LPI signalling while working in manual mode
 *
 */
typedef enum{
    CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_DISABLED_E   ,
    CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENABLED_E    ,
    CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_FORCED_E
}CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT;


/*
 * typedef: CPSS_DXCH_PORT_EEE_LPI_STATUS_STC
 *
 * Description: structure for EEE LPI status info
 *
 * Fields:
 *  macTxPathLpi    - Indicates that the MAC is sending LPI to the Tx PHY.
 *                  GT_FALSE - not sending.
 *                  GT_TRUE  - sending.
 *  macTxPathLpWait - Indicates that the MAC is sending IDLE to the Tx PHY and waiting for Tx timer to end.
 *                  GT_FALSE - not sending.
 *                  GT_TRUE  - sending and wait.
 *  macRxPathLpi    - Indicates that the PHY has detected the assertion / de-assertion of LP_IDLE from link partner (through the PCS).
 *                  GT_FALSE - not detected.
 *                  GT_TRUE  - detected.
 *  pcsTxPathLpi    - PCS Tx path LPI status .
 *                  GT_FALSE - Does not receive: Tx PCS does not receive LP_IDLE.
 *                  GT_TRUE  - Receives: Tx PCS receives LP_IDLE.
 *  pcsRxPathLpi    - PCS Rx path LPI status.
 *                  GT_FALSE - Does not receive: Rx PCS does not receive LP_IDLE.
 *                  GT_TRUE  - Receives: Rx PCS receives LP_IDLE.
 *
*/
typedef struct{
    GT_BOOL macTxPathLpi;
    GT_BOOL macTxPathLpWait;
    GT_BOOL macRxPathLpi;
    GT_BOOL pcsTxPathLpi;
    GT_BOOL pcsRxPathLpi;
}CPSS_DXCH_PORT_EEE_LPI_STATUS_STC;

/*******************************************************************************
* cpssDxChPortEeeLpiRequestEnableSet
*
* DESCRIPTION:
*       Set port enable/disable transmitting of LPI signaling to the PHY and
*       identification of LPI on reception.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiRequestEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChPortEeeLpiRequestEnableGet
*
* DESCRIPTION:
*       Get port's enable/disable transmitting of LPI signaling to the PHY and
*       identification of LPI on reception.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       enablePtr   -  (pointer to)
*                       GT_TRUE  - enabled
*                       GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiRequestEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChPortEeeLpiManualModeSet
*
* DESCRIPTION:
*       Set port manual mode (enabled/disabled/forced) for LPI signaling.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - the manual mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum or mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiManualModeSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChPortEeeLpiManualModeGet
*
* DESCRIPTION:
*       Get port manual mode (enabled/disabled/forced) for LPI signaling.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       modePtr   - (pointer to) the manual mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiManualModeGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_DXCH_PORT_EEE_LPI_MANUAL_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChPortEeeLpiTimeLimitsSet
*
* DESCRIPTION:
*       Set port LPI timers.
*       NOTE: The 'time' granularity is 1 uSec for 1000 Mbps and 10 uSec for 100 Mbps.
*       so the function set value 'as is'.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       twLimit   - The minimum time from LPI de-assertion until valid data can be sent .
*                   (APPLICABLE RANGES: 0..4095)
*       liLimit   - The minimum time since going into LPI and allowing LPI de-assert.
*                   (APPLICABLE RANGES: 0..254)
*       tsLimit   - The minimum time from emptying of Tx FIFO and LPI assert.
*                   (APPLICABLE RANGES: 0..255)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_OUT_OF_RANGE          - on out of range : twLimit , liLimit , tsLimit
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiTimeLimitsSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_U32                  twLimit,
    IN  GT_U32                  liLimit,
    IN  GT_U32                  tsLimit
);

/*******************************************************************************
* cpssDxChPortEeeLpiTimeLimitsGet
*
* DESCRIPTION:
*       Get port LPI timers.
*       NOTE: The 'time' granularity is 1 uSec for 1000 Mbps and 10 uSec for 100 Mbps.
*       so the function get value 'as is'.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
* OUTPUTS:
*       twLimitPtr - (pointer to) The minimum time from LPI de-assertion until valid data can be sent .
*       liLimitPtr - (pointer to) The minimum time since going into LPI and allowing LPI de-assert.
*       tsLimitPtr - (pointer to) The minimum time from emptying of Tx FIFO and LPI assert.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiTimeLimitsGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *twLimitPtr,
    OUT GT_U32                  *liLimitPtr,
    OUT GT_U32                  *tsLimitPtr
);

/*******************************************************************************
* cpssDxChPortEeeLpiStatusGet
*
* DESCRIPTION:
*       Get port LPI status.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
* OUTPUTS:
*       statusPtr - (pointer to) the status info.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEeeLpiStatusGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT CPSS_DXCH_PORT_EEE_LPI_STATUS_STC *statusPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortEee */

