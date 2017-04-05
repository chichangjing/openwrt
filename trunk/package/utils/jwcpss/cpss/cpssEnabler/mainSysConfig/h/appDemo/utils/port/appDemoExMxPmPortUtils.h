/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* appDemoExMxPmPortUtils.h
*
* DESCRIPTION:
*       App demo ExMxPm port API headers.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#ifndef __appDemoExMxPmPortUtilsh
#define __appDemoExMxPmPortUtilsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/port/cpssPortCtrl.h>

/*******************************************************************************
* gtAppDemoPuma3PortModeSpeedSet
*
* DESCRIPTION:
*       Example of configuration sequence of different ports interface modes
*       and speeds
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       ifMode   - Interface mode.
*       speed    - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtAppDemoPuma3PortModeSpeedSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT  speed
);

/*******************************************************************************
* appDemoPuma3BoardTypeSet
*
* DESCRIPTION:
*       A pre-init API defining whether the board is a modular or a 
*		standalone board.
*
* INPUTS:
*       devNum    - device number
*       isModular - GT_TRUE  - The board is a modular board.
*				  - GT_FALSE - The board is a standalone board.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoPuma3BoardTypeSet
(
    GT_U8   devNum,
    GT_BOOL isModular
);

/*******************************************************************************
* appDemoPuma3LoopbackEnableSet
*
* DESCRIPTION:
*       A pre-init API defining whether the fabric ports are initialized with a
* 		loopback or not.
*
* INPUTS:
*       devNum          - device number
*       enableLoopBack  - GT_TRUE  - initialize fabric ports with loopback.
*						- GT_FALSE - initialize fabric ports without loopback.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoPuma3LoopbackEnableSet
(
    GT_U8   devNum,
    GT_BOOL enableLoopBack
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoExMxPmPortUtilsh */
