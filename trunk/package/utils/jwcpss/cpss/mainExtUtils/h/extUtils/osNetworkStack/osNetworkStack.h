/*******************************************************************************
*                Copyright 2014, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* osNetworkStack.h
*
* DESCRIPTION:
*       CPSS <=> OS TCP/IP stack
*
* DEPENDENCIES:
*
* COMMENTS:
*       Please note: only one management interface can be configured
*
*       Example:
*           GT_ETHERADDR addr = {{0x00,0x00,0x00,0x11,0x22,0x33}};
*           GT_U8 devId=0;
*           GT_U16 vlanId=1;
*           osNetworkStackInit();
*           osNetworkStackConnect(devId,vlanId,&addr);
*           osNetworkStackIfconfig("up inet 10.1.2.3 netmask 255.255.255.0");
*
*           This will configure linux network interface tap0 with
*             HWaddr=00:00:00:11:22:33 inet addr:10.1.2.3 Mask:255.255.255.0
*           Also packets will be routed between this interface and network port
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __osNetworkStack_h__
#define __osNetworkStack_h__

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#define SWITCH_MAX_PORT 2 /*FIXME, temp use, must set by other method*/

/*******************************************************************************
* osNetworkStackInit
*
* DESCRIPTION:
*       Initialize library
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS osNetworkStackInit(void);

/*******************************************************************************
* osNetworkStackConnect
*
* DESCRIPTION:
*       Connect device/vlan to OS network stack (unicast and broadcast)
*       Only one OS network interface can be confugured
*
* INPUTS:
*       devId       - device ID
*       vlanId      - Vlan ID (already configured)
*       etherAddr   - Mac address of interface
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS osNetworkStackConnect(
  IN  GT_U8         devId,
  IN  GT_U16        vlanId,
  IN  GT_ETHERADDR  *etherAddr
);

/*******************************************************************************
* osNetworkStackDisconnect
*
* DESCRIPTION:
*       Disconnect PP from OS network stack
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS osNetworkStackDisconnect(void);

/*******************************************************************************
* osNetworkStackMacSet
*
* DESCRIPTION:
*       Change MAC address of OS network interface, update FDB
*
* INPUTS:
*       etherAddr   - Mac address of interface
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS osNetworkStackMacSet(
  IN  GT_ETHERADDR *etherPtr
);

/*******************************************************************************
* osNetworkStackMacGet
*
* DESCRIPTION:
*       Query for MAC address of OS network interface
*
* INPUTS:
*       None
*
* OUTPUTS:
*       etherAddr   - Mac address of interface
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS osNetworkStackMacGet(
  OUT GT_ETHERADDR *etherPtr
);

/*******************************************************************************
* osNetworkStackIfconfig
*
* DESCRIPTION:
*       Configure OS network interface
*
* INPUTS:
*       config  - Configuration string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       Example:
*       osNetworkStackIfconfig("up inet 10.1.2.3 netmask 255.255.255.0")
*
*******************************************************************************/
GT_STATUS osNetworkStackIfconfig(
  IN const char *config
);

#endif /* __osNetworkStack_h__ */
