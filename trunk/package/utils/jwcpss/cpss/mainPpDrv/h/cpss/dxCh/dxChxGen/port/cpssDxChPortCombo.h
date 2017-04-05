/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortCombo.h
*
* DESCRIPTION:
*       CPSS implementation for Combo Port configuration and control facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssDxChPortComboh
#define __cpssDxChPortComboh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/*
 * typedef: enum CPSS_DXCH_PORT_SERDES_MODE_ENT
 *
 * Description: Enumeration of Serdes modes
 *
 * Enumerations:
 *    CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E - Regular Serdes are active
 *    CPSS_DXCH_PORT_SERDES_MODE_COMBO_E - Combo Serdes are active
 */
typedef enum
{
    CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E,
    CPSS_DXCH_PORT_SERDES_MODE_COMBO_E

}CPSS_DXCH_PORT_SERDES_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_PORT_MAC_PARAMS_STC
 *
 * Description: Structure describes specific physical MAC
 * 
 * APPLICABLE DEVICES: Lion2
 * 
 * Fields:
 *      macNum - physical MAC number
 *      macPortGroupNum - number of core where MAC resides, for multi-core devices,
 *                          for single core leave it 0. 
 *      connection - connection type (fiber/copper) of this MAC (dependent on 
 *                      specific board architecture)
 * 
 */
typedef struct {
    GT_U32              macNum;
    GT_U32              macPortGroupNum;
} CPSS_DXCH_PORT_MAC_PARAMS_STC;

/* max number of MAC's partisipating in combo port */
#define CPSS_DXCH_PORT_MAX_MACS_IN_COMBO_CNS   2

/* 1) don't use preferred MAC while configure combo port;
   2) place in MAC's array free */
#define CPSS_DXCH_PORT_COMBO_NA_MAC_CNS   0x7FFFFFFF

/*
* typedef: struct CPSS_DXCH_PORT_COMBO_PARAMS_STC
*
* Description: Parameters describing combo port
* 
 * APPLICABLE DEVICES: Lion2
* 
 * Fields:
*      macArray - list of MAC's used by combo port
*      preferredMacIdx - (optional) index of MAC in macArray, which should be 
 *                          activated by default, during port interface
*                          configuration and which should be chosen if port
*                          connected by both exits.
*                          If application not interested to use this option
*                              must set to CPSS_DXCH_PORT_COMBO_NA_MAC_CNS
* 
 */
typedef struct {
    CPSS_DXCH_PORT_MAC_PARAMS_STC   macArray[CPSS_DXCH_PORT_MAX_MACS_IN_COMBO_CNS];
    GT_U32              preferredMacIdx;
} CPSS_DXCH_PORT_COMBO_PARAMS_STC;

/*******************************************************************************
* cpssDxChPortSerdesComboModeSet
*
* DESCRIPTION:
*       Set Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum  - physical port number, values 20..23.
*       mode     - serdes mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum, mode
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesComboModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_DXCH_PORT_SERDES_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChPortSerdesComboModeGet
*
* DESCRIPTION:
*       Get Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number, values 20..23.
*
* OUTPUTS:
*       modePtr   -  pointer to Serdes mode.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesComboModeGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_DXCH_PORT_SERDES_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChPortComboModeEnableSet
*
* DESCRIPTION:
*       Register/unregister port as combo in CPSS SW DB and set some specific
*       parameters. This configuration will be used by cpssDxChPortModeSpeedSet,
*       when port's physical interface will be actually configured and for
*       other system needs.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*       enable  -   GT_TRUE - port is combo;
*                   GT_FALSE - port is regular.
*       paramsPtr - (ptr to) parameters describing specific combo port options,
*                   doesn't matter if enable == GT_FALSE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - paramsPtr == NULL
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortComboModeEnableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  GT_BOOL                                 enable,
    IN  const   CPSS_DXCH_PORT_COMBO_PARAMS_STC *paramsPtr
);

/*******************************************************************************
* cpssDxChPortComboModeEnableGet
*
* DESCRIPTION:
*       Get if port registered as combo and its parameters.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*
* OUTPUTS:
*       enablePtr       -   GT_TRUE - port is combo;
*                           GT_FALSE - port is regular.
*       paramsPtr       - parameters describing specific combo port options
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - enablePtr or paramsPtr == NULL
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortComboModeEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr,
    OUT CPSS_DXCH_PORT_COMBO_PARAMS_STC *paramsPtr
);

/*******************************************************************************
* cpssDxChPortComboPortActiveMacSet
*
* DESCRIPTION:
*       Activate one of MAC's serving combo port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*       macPtr  - (ptr to) parameters describing one of MAC of given combo port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - macPtr == NULL
*       GT_FAIL         - on error
*
* COMMENTS:
*   
*
*******************************************************************************/
GT_STATUS cpssDxChPortComboPortActiveMacSet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  const   CPSS_DXCH_PORT_MAC_PARAMS_STC  *macPtr
);

/*******************************************************************************
* cpssDxChPortComboPortActiveMacGet
*
* DESCRIPTION:
*       Get current activate MAC of combo port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number.
*
* OUTPUTS:
*       macPtr  - (ptr to) parameters describing active MAC
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - macPtr == NULL
*       GT_FAIL         - on error
*
* COMMENTS:
*   
*
*******************************************************************************/
GT_STATUS cpssDxChPortComboPortActiveMacGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT CPSS_DXCH_PORT_MAC_PARAMS_STC   *macPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortComboh */

