/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* cmdSystem.h
*
* DESCRIPTION:
*       commander compile-time configuration header
*
*
* DEPENDENCIES:
*       none
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*******************************************************************************/

#ifndef __cmdSystem_h__
#define __cmdSystem_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Include files ***************************************************/

/***** Global Definitions **********************************************/

#define CMD_THREAD_PRIO 5      /* default thread priority      */

/* enable streaming components */
#define CMD_INCLUDE_SERIAL          /* serial port shell server */


/* enable sockets connection*/
#define CMD_INCLUDE_TCPIP           /* tcpip shell server       */
#define CMD_ENABLE_DUAL_CONSOLES    /* Whether the application use more than*/
#define CMD_INCLUDE_REMOTEIP        /* Whether to allow detection of remote IP
                                       Connection */


/* default streaming values */
#define CMD_SERVER_PORT     31337   /* default IP server port   */
#define CMD_KA_SERVER_PORT  31338   /* default Keep Alive IP server port   */
#define CMD_REMOTEIP_PORT	31341   /* default remote IP detection port	*/
#define CMD_SERVER_INST     2       /* number of max. instances */
#define CMD_SERIAL_PORT     0       /* serial port device       */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdSystem_H__ */

