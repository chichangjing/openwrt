/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsBobKCpll.h
*
* DESCRIPTION:
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#ifndef __mvHwServicesBobKCpll_H
#define __mvHwServicesBobKCpll_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gtOs/gtGenTypes.h>

#define  DEVICE_GENERAL_CONTROL_16  0x000F8290
#define  DEVICE_GENERAL_CONTROL_17  0x000F8294
#define  DEVICE_GENERAL_CONTROL_20  0x000F82A0
#define  DEVICE_GENERAL_CONTROL_21  0x000F82A4

#define MAX_CPLL_DATA_INDEX 36

typedef enum
{
    CPLL0,
    CPLL1,
    CPLL2,
    MV_HWS_MAX_CPLL_NUMBER

}MV_HWS_CPLL_NUMBER;


typedef enum
{
    MV_HWS_25MHzIN,
    MV_HWS_156MHz_IN,
    MV_HWS_MAX_INPUT_FREQUENCY

}MV_HWS_CPLL_INPUT_FREQUENCY;

typedef enum
{
    MV_HWS_156MHz_OUT,
    MV_HWS_78MHz_OUT,
    MV_HWS_200MHz_OUT,
    MV_HWS_MAX_OUTPUT_FREQUENCY

}MV_HWS_CPLL_OUTPUT_FREQUENCY;

typedef enum
{
    MV_HWS_ANA_GRP0,
    MV_HWS_ANA_GRP1,
    MV_HWS_CPLL,
    MV_HWS_RCVR_CLK_IN,
    MV_HWS_MAX_INPUT_SRC_CLOCK

}MV_HWS_INPUT_SRC_CLOCK;

/*******************************************************************************
* mvHwsCpllControl
*
* DESCRIPTION:
*       Configure the requested CPLL (0, 1, 2) or bypass on specific CPLL
*
* INPUTS:
*       devNum    - system device number
*       cpllNum   - cpll number
*       bypassEn  - bypass enable/disable
*       inFreq    - input frequency
*       outFreq   - output frequency
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCpllControl
(
    GT_U8                           devNum,
    MV_HWS_CPLL_NUMBER              cpllNum,
    GT_BOOL                         bypass_en,
    MV_HWS_CPLL_INPUT_FREQUENCY     inFreq,
    MV_HWS_CPLL_OUTPUT_FREQUENCY    outFreq
);

/*******************************************************************************
* mvHwsSerdesClockGlobalControl
*
* DESCRIPTION:
*       Global configuration per Serdes for referance clock control source.
*       Need to be done before any Serdes in the system is powered up.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - SERDES number
*       srcClock  - Input clock source
*       inFreq    - input frequency
*       outFreq   - output frequency
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesClockGlobalControl
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          serdesNum,
    MV_HWS_INPUT_SRC_CLOCK          srcClock,
    MV_HWS_CPLL_INPUT_FREQUENCY     inFreq,
    MV_HWS_CPLL_OUTPUT_FREQUENCY    outFreq
);

/*******************************************************************************
* mvHwsSerdesClockGlobalControlGet
*
* DESCRIPTION:
*       Getting Input clock source after
*       Global configuration per Serdes for referance clock control source.
*
* INPUTS:
*       devNum             - system device number
*       serdesNum          - SERDES number
*
* OUTPUTS:
*       srcClockPtr        - Input clock source
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - srcClockPtr is NULL
*       GT_BAD_STATE       - on bad state
*
*******************************************************************************/
GT_STATUS mvHwsSerdesClockGlobalControlGet
(
    GT_U8                           devNum,
    GT_U32                          serdesNum,
    MV_HWS_INPUT_SRC_CLOCK         *srcClockPtr
);


#ifdef __cplusplus
}
#endif

#endif /* __mvHwServicesBobKPortIf_H */


