/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssLedCtrl.h
*
* DESCRIPTION:
*       Includes Leds control data structures and enumerations definitions
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __cpssLedCtrlh
#define __cpssLedCtrlh

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * typedef: enum CPSS_LED_ORDER_MODE_ENT
 *
 * Description: LED stream ordering mode
 *
 * Fields:
 *      CPSS_LED_ORDER_MODE_BY_PORT_E    - the indication order is arranged by port
 *      CPSS_LED_ORDER_MODE_BY_CLASS_E   - the indication order is arranged by class
 *
 */
typedef enum
{
    CPSS_LED_ORDER_MODE_BY_PORT_E,
    CPSS_LED_ORDER_MODE_BY_CLASS_E
} CPSS_LED_ORDER_MODE_ENT;

/*
 * typedef: enum CPSS_LED_BLINK_DUTY_CYCLE_ENT
 *
 * Description: Duty cycle of LED blink signal
 *
 * Fields:
 *      CPSS_LED_BLINK_DUTY_CYCLE_0_E - 25% on, 75% off
 *      CPSS_LED_BLINK_DUTY_CYCLE_1_E - 50% on, 50% off
 *      CPSS_LED_BLINK_DUTY_CYCLE_2_E - 50% on, 50% off
 *      CPSS_LED_BLINK_DUTY_CYCLE_3_E - 75% on, 25% off
 *
 */
typedef enum
{
    CPSS_LED_BLINK_DUTY_CYCLE_0_E,
    CPSS_LED_BLINK_DUTY_CYCLE_1_E,
    CPSS_LED_BLINK_DUTY_CYCLE_2_E,
    CPSS_LED_BLINK_DUTY_CYCLE_3_E
} CPSS_LED_BLINK_DUTY_CYCLE_ENT;

/*
 * typedef: enum CPSS_LED_BLINK_DURATION_ENT
 *
 * Description: The period of the LED Blink signal 
 *              (see HW spec doc for meaning of values).
 *
 *
 * Fields:
 *      CPSS_LED_BLINK_DURATION_0_E
 *      CPSS_LED_BLINK_DURATION_1_E
 *      CPSS_LED_BLINK_DURATION_2_E
 *      CPSS_LED_BLINK_DURATION_3_E
 *      CPSS_LED_BLINK_DURATION_4_E
 *      CPSS_LED_BLINK_DURATION_5_E
 *      CPSS_LED_BLINK_DURATION_6_E
 *      CPSS_LED_BLINK_DURATION_7_E
 *          (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 */
typedef enum
{
    CPSS_LED_BLINK_DURATION_0_E,
    CPSS_LED_BLINK_DURATION_1_E,
    CPSS_LED_BLINK_DURATION_2_E,
    CPSS_LED_BLINK_DURATION_3_E,
    CPSS_LED_BLINK_DURATION_4_E,
    CPSS_LED_BLINK_DURATION_5_E,
    CPSS_LED_BLINK_DURATION_6_E,
    CPSS_LED_BLINK_DURATION_7_E
} CPSS_LED_BLINK_DURATION_ENT;

/*
 * typedef: enum CPSS_LED_PULSE_STRETCH_ENT
 *
 * Description: The length of stretching for dynamic signals 
 *              (see HW spec doc for meaning of values).
 *
 * Fields:
 *      CPSS_LED_PULSE_STRETCH_0_NO_E  - no stretching  
 *      CPSS_LED_PULSE_STRETCH_1_E
 *      CPSS_LED_PULSE_STRETCH_2_E
 *      CPSS_LED_PULSE_STRETCH_3_E
 *      CPSS_LED_PULSE_STRETCH_4_E
 *      CPSS_LED_PULSE_STRETCH_5_E
 *      CPSS_LED_PULSE_STRETCH_6_E
 *          (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      CPSS_LED_PULSE_STRETCH_7_E
 *          (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 */
typedef enum
{
    CPSS_LED_PULSE_STRETCH_0_NO_E,
    CPSS_LED_PULSE_STRETCH_1_E,
    CPSS_LED_PULSE_STRETCH_2_E,
    CPSS_LED_PULSE_STRETCH_3_E,
    CPSS_LED_PULSE_STRETCH_4_E,
    CPSS_LED_PULSE_STRETCH_5_E,
    CPSS_LED_PULSE_STRETCH_6_E,
    CPSS_LED_PULSE_STRETCH_7_E
} CPSS_LED_PULSE_STRETCH_ENT;

/*
 * typedef: enum CPSS_LED_CLOCK_OUT_FREQUENCY_ENT
 *
 * Description: The LED clock out frequency
 *
 * Fields:
 *      CPSS_LED_CLOCK_OUT_FREQUENCY_500_E   - 500 KHz LED clock frequency. 
 *      CPSS_LED_CLOCK_OUT_FREQUENCY_1000_E  - 1 MHz LED clock frequency.
 *      CPSS_LED_CLOCK_OUT_FREQUENCY_2000_E  - 2 MHz LED clock frequency.
 *      CPSS_LED_CLOCK_OUT_FREQUENCY_3000_E  - 3 MHz LED clock frequency.
 */
typedef enum
{
    CPSS_LED_CLOCK_OUT_FREQUENCY_500_E,
    CPSS_LED_CLOCK_OUT_FREQUENCY_1000_E,
    CPSS_LED_CLOCK_OUT_FREQUENCY_2000_E,
    CPSS_LED_CLOCK_OUT_FREQUENCY_3000_E
} CPSS_LED_CLOCK_OUT_FREQUENCY_ENT;

/*
 * typedef: enum CPSS_LED_CLASS_5_SELECT_ENT
 *
 * Description: The indication displayed on class5 (for dual-media port\phy).
 *
 * Fields:
 *      CPSS_LED_CLASS_5_SELECT_HALF_DUPLEX_E   - Half Duplex is displayed on class5.
 *      CPSS_LED_CLASS_5_SELECT_FIBER_LINK_UP_E - If port is a dual media port, 
 *                                                Fiber Link Up is displayed on class5.
 */
typedef enum
{
    CPSS_LED_CLASS_5_SELECT_HALF_DUPLEX_E,
    CPSS_LED_CLASS_5_SELECT_FIBER_LINK_UP_E
} CPSS_LED_CLASS_5_SELECT_ENT;

/*
 * typedef: enum CPSS_LED_CLASS_13_SELECT_ENT
 *
 * Description: The indication displayed on class13 (for dual-media port\phy).
 *
 * Fields:
 *      CPSS_LED_CLASS_13_SELECT_LINK_DOWN_E      - Link Down is displayed on class13.
 *      CPSS_LED_CLASS_13_SELECT_COPPER_LINK_UP_E - If port is a dual media port, 
 *                                                  Copper Link Up is displayed on class13. 
 */
typedef enum
{
    CPSS_LED_CLASS_13_SELECT_LINK_DOWN_E,
    CPSS_LED_CLASS_13_SELECT_COPPER_LINK_UP_E
} CPSS_LED_CLASS_13_SELECT_ENT;

/*
 * typedef: enum CPSS_LED_BLINK_SELECT_ENT
 *
 * Description: Blink types to select
 *
 * Fields:
 *      CPSS_LED_BLINK_SELECT_0_E - Blink 0 signal
 *      CPSS_LED_BLINK_SELECT_1_E - Blink 1 signal
 */
typedef enum
{
    CPSS_LED_BLINK_SELECT_0_E,
    CPSS_LED_BLINK_SELECT_1_E
} CPSS_LED_BLINK_SELECT_ENT;

/*
 * typedef: enum CPSS_LED_DATA_ENT
 *
 * Description: Types of data reflected by the XG-PCS lane LEDs
 *
 * Fields:
 *      CPSS_LED_DATA_LOW_E              - LED low
 *      CPSS_LED_DATA_HIGH1_E            - LED high
 *      CPSS_LED_DATA_HIGH2_E            - LED high
 *      CPSS_LED_DATA_SYNC_E             - Sync indication for the lane
 *      CPSS_LED_DATA_OVERRUN_E          - PPM FIFO overrun
 *      CPSS_LED_DATA_UNDERRUN_E         - PPM FIFO underrun
 *      CPSS_LED_DATA_BYTE_ERROR_E       - /E/ code or invalid code
 *      CPSS_LED_DATA_DISPARITY_ERROR_E  - Disparity error, invalid code
 *      CPSS_LED_DATA_RX_E               - Link and data on RX XGMII
 *      CPSS_LED_DATA_TX_E               - Link and data on RX XGMII
 *      CPSS_LED_DATA_ACTIVITY_E         - RX or TX
 *      CPSS_LED_DATA_RX_OR_LINK_E       - RX or Link
 *      CPSS_LED_DATA_LINK_E             - Lane's link is up
 *      CPSS_LED_DATA_LOCAL_FAULT_E      - Local fault indication on the lane
 */

typedef enum
{
    CPSS_LED_DATA_LOW_E,
    CPSS_LED_DATA_HIGH1_E,
    CPSS_LED_DATA_HIGH2_E,
    CPSS_LED_DATA_SYNC_E,
    CPSS_LED_DATA_OVERRUN_E,
    CPSS_LED_DATA_UNDERRUN_E,
    CPSS_LED_DATA_BYTE_ERROR_E,
    CPSS_LED_DATA_DISPARITY_ERROR_E,
    CPSS_LED_DATA_RX_E,
    CPSS_LED_DATA_TX_E,
    CPSS_LED_DATA_ACTIVITY_E,
    CPSS_LED_DATA_RX_OR_LINK_E,
    CPSS_LED_DATA_LINK_E,
    CPSS_LED_DATA_LOCAL_FAULT_E
} CPSS_LED_DATA_ENT;

/*
 * typedef: struct CPSS_LED_CONF_STC
 *
 * Description: 
 *      LED stream configuration parameters.
 *
 * Fields:
 *      ledOrganize         - The order of the signals driven in the LED stream.
 *      disableOnLinkDown   - How to display indications when the link is down:
 *                            GT_FALSE - No effect, Link status has no effect 
 *                                       on other indications.
 *                            GT_TRUE - When link is down, the following indications: 
 *                                      speed, duplex, activity are forced down.
 *                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
 *      blink0DutyCycle     - The duty cycle of the Blink0 signal.
 *      blink0Duration      - The period of the Blink0 signal.
 *      blink1DutyCycle     - The duty cycle of the Blink1 signal.
 *      blink1Duration      - The period of the Blink1 signal.
 *      pulseStretch        - The length of stretching for dynamic signals.
 *      ledStart            - The first bit in the LED stream to be driven, range 0..255
 *      ledEnd              - The last bit in the LED stream to be driven, range 0..255
 *      clkInvert           - Inverts the LEDClk pin, GT_FALSE - disable, GT_TRUE - enable.
 *                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
 *      class5select        - Selects the indication displayed on class5 (relevant only for 
 *                            GE ports LED stream).
 *                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
 *      class13select       - Selects the indication displayed on class13 (relevant only for 
 *                            GE ports LED stream).
 *                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
 *      invertEnable        - LED data polarity
 *                            GT_TRUE - active low: LED light is in low indication.
 *                            GT_FALSE - active high: LED light is in high indication.
 *                              (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      ledClockFrequency   - LED output clock frequency
 *                              (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 * 
 */
typedef struct CPSS_LED_CONF_STCT
{
    CPSS_LED_ORDER_MODE_ENT         ledOrganize;
    GT_BOOL                         disableOnLinkDown;
    CPSS_LED_BLINK_DUTY_CYCLE_ENT   blink0DutyCycle;
    CPSS_LED_BLINK_DURATION_ENT     blink0Duration;
    CPSS_LED_BLINK_DUTY_CYCLE_ENT   blink1DutyCycle;
    CPSS_LED_BLINK_DURATION_ENT     blink1Duration;
    CPSS_LED_PULSE_STRETCH_ENT      pulseStretch;
    GT_U32                          ledStart;
    GT_U32                          ledEnd;
    GT_BOOL                         clkInvert;
    CPSS_LED_CLASS_5_SELECT_ENT     class5select;
    CPSS_LED_CLASS_13_SELECT_ENT    class13select;
    GT_BOOL                         invertEnable;
    CPSS_LED_CLOCK_OUT_FREQUENCY_ENT ledClockFrequency;
} CPSS_LED_CONF_STC;

/*
 * typedef: struct CPSS_LED_CLASS_MANIPULATION_STC
 *
 * Description:
 *      LED class manipulation configuration.
 *
 * Fields:
 *      invertEnable    - Enables a NOT operation on Class data.
 *                        GT_TRUE - data inversion enabled.
 *                        GT_FALSE - data is not inverted.
 *                          (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
 *      blinkEnable     - Enables an AND operation on Class with selected Blink signals.
 *                        GT_TRUE - blinking enabled: High data displayed as blinking.
 *                        GT_FALSE - blinking feature not enabled.
 *      blinkSelect     - Selects between Blink 0 or Blink 1 signals.
 *      forceEnable     - Enables forcing Class data.
 *                        GT_TRUE - forceData is the data displayed.
 *                        GT_FALSE - no forcing on dispalyed data.
 *      forceData       - The data to be forced on Class data.
 *                        For GE devices: range 0x0..0xFFF (12 bits length)
 *                        For XG devices: range 0x0..0x3   (2 bits length)
 *      pulseStretchEnable
 *                      - enable the stretch affect on a specific class
 *                          (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      disableOnLinkDown
 *                      - disable the LED indication classes when the link is down
 *                          (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 */
typedef struct CPSS_LED_CLASS_MANIPULATION_STCT
{
    GT_BOOL                     invertEnable;
    GT_BOOL                     blinkEnable;
    CPSS_LED_BLINK_SELECT_ENT   blinkSelect;
    GT_BOOL                     forceEnable;
    GT_U32                      forceData;
    GT_BOOL                     pulseStretchEnable;
    GT_BOOL                     disableOnLinkDown;
} CPSS_LED_CLASS_MANIPULATION_STC;

/*
 * typedef: struct CPSS_LED_GROUP_CONF_STC
 *
 * Description:
 *      LED group classes used for the combinational logic.
 *      The combinational logic = (Class A AND Class B) OR (Class C AND Class D).
 *
 * Fields:
 *      classA  - Selects which class is used as A in the group combinational logic.
 *      classB  - Selects which class is used as B in the group combinational logic. 
 *      classC  - Selects which class is used as C in the group combinational logic. 
 *      classD  - Selects which class is used as D in the group combinational logic.
 */
typedef struct CPSS_LED_GROUP_CONF_STCT
{
    GT_U32  classA;
    GT_U32  classB;
    GT_U32  classC;
    GT_U32  classD;
} CPSS_LED_GROUP_CONF_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssLedCtrlh */

