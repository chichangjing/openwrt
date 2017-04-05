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
* mvHwsServiceCpuInt.h
*
* DESCRIPTION:
*           This file contains HWS Service CPU interrupt definition
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#ifndef __mvHwsServiceCpuInterrupt_H
#define __mvHwsServiceCpuInterrupt_H

#ifdef __cplusplus
extern "C" {
#endif

#define MV_HWS_PORT_INT_MAX_NUM (32)

/*
** Service CPU Global Interrupt Map -MV_HWS_PORT_EVENT_GROUP Event Summary
** +===========================+====+====+====+====+====+====+====+====+
** + Service CPU Event Summary + 31 + 30 +         + 03 + 02 + 01 + 00 +
** +===========================+====+====+====+====+====+====+====+====+ 
** +                           + Reserved          + |  + |  + |  + |  +
** +===========================+====+====+====+====+=|==+=|==+=|==+=|==+ 
**                                                   |    |    |    |
**                        Port Group Event 3 [31:24] +    |    |    |
**                             Port Group Event 2 [23:16] +    |    |
**                                  Port Group Event 1 [15:08] +    |
**                                       Port Group Event 0 [07:00] +
**
** Service CPU Global Interrupt Map
** +===================+====+====+====+====+====+====+====+====+====+====+====+====+====+====+====+====+
** + Service CPU Group |      Group 3      |      Group 2      |      Group 1      |      Group 0      +
** +===================+====+====+====+====+====+====+====+====+====+====+====+====+====+====+====+====+
** + Service CPU Mask  | 31 +         + 24 | 23 +         + 16 | 15 +         + 08 | 07 +         + 00 +
** +===================+====+====+====+====+====+====+====+====+====+====+====+====+====+====+====+====+
** + Service CPU Event | 31 +         + 24 | 23 +         + 16 | 15 +         + 08 | 07 +         + 00 +
** +===================+====+====+====+====+=|==+====+====+====+====+====+====+====+====+====+====+====+
**                                           |
**                                           |    Port Interrupt Event
**                                           |       +====+====+====+====+====+====+====+====+
**                                           +---->  + 07 + 06 + 05 + 04 + 03 + 02 + 01 + 00 +
**                                           |       +====+====+====+====+====+====+====+====+
**                                           |       + |  + Reserved                    + |  +
**                                           |       +=|==+=============================+=|==+
**                                           |         + Link Event                       + HCD Event
**                                           |   
**                                           |    Port Interrupt Status
**                                           |       +====+====+====+====+====+====+====+====+
**                                           +---->  + 07 + 06 + 05 + 04 + 03 + 02 + 01 + 00 +
**                                                   +====+====+====+====+====+====+====+====+
**                                                   + |  + HCD Type                    + |  +
**                                                   +=|==+=============================+=|==+
**                                                     + Link Status                      + HCD Status
*/

#define MV_HWS_GROUP_EVENT_NUM    (4)
#define MV_HWS_PORT_EVENT_NUM     (8)

#define MV_HWS_PORT_EVENT_MASK    (0x81)

#define MV_HWS_PORT_LINK_EVENT    (0x80)
#define MV_HWS_PORT_LINK_MASK     (0x80)
#define MV_HWS_PORT_HCD_EVENT     (0x01)
#define MV_HWS_PORT_HCD_MASK      (0x7F)
#define MV_HWS_PORT_HCD_SHIFT     (1)

typedef struct
{
    GT_U8  portIntEvent;
    GT_U8  portIntStatus;

}MV_HWS_PORT_INT;

typedef struct
{
    GT_U32          portIntSummary;
    GT_U32          portIntMask;
    GT_U32          portIntEvent;
    MV_HWS_PORT_INT portInt[MV_HWS_PORT_INT_MAX_NUM];

}MV_HWS_SERV_CPU_INT;

/*******************************************************************************
* mvHwsServCpuIntrInit
*
* DESCRIPTION:
*       HWS Service CPU interrupt init
*
* INPUTS:
*       shareMemBaseAddr - Base address for interrupt map on shared memory
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
*
*******************************************************************************/
GT_VOID mvHwsServCpuIntrInit(GT_U32 sharedMemBaseAddr);

/*******************************************************************************
* mvHwsServCpuIntrSet
*
* DESCRIPTION:
*       HWS Service CPU interrupt config
*       Update port interrupt status & event, including event summary
*
* INPUTS:
*       port - Port number associated with the raized interrupt
*       intr - interrupt event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_VOID mvHwsServCpuIntrSet(GT_U32 port, GT_U32 intr);

/*******************************************************************************
* mvHwsServCpuIntrGet
*
* DESCRIPTION:
*       HWS Service CPU interrupt retrive
*       Return interrupt map to calling interrupt handler
*       Clear active interrupt events
*
* INPUTS:
*       intrMap - current interrupt map
*
* OUTPUTS:
*       intrMap - current interrupt map
*
* RETURNS:
*       None
*
*******************************************************************************/
GT_VOID mvHwsServCpuIntrGet(MV_HWS_SERV_CPU_INT *intrMap);

/*******************************************************************************
* mvHwsServCpuIntrClear
*
* DESCRIPTION:
*       HWS Service CPU interrupt clear
*       Clear port interrupt event
*       Clear Service CPU interrupt event
*       Clear Service CPU interrupt event summary
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
GT_VOID mvHwsServCpuIntrClear(void);

/*******************************************************************************
* mvHwsServCpuIntrStatusReset
*
* DESCRIPTION:
*       HWS Service CPU interrupt status update 
*       Updates only interrupt status Not event
*
* INPUTS:
*       port - Port number associated with the raized interrupt
*       intr - interrupt event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
*
*******************************************************************************/
GT_VOID mvHwsServCpuIntrStatusReset(GT_U32 port);

/*******************************************************************************
* mvHwsServCpuIntrHandler
*
* DESCRIPTION:
*       HWS Service CPU interrupt handler
*       This API should be called from the interrupt handler after retriving
*       interrupt map via mvHwsServCpuIntrGet
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
GT_VOID mvHwsServCpuIntrHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __mvHwsServiceCpuInterrupt_H */


