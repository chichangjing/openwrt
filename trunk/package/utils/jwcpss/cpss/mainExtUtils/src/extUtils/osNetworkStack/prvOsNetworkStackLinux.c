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
* prvOsNetworkStackLinux.c
*
* DESCRIPTION:
*       OS TCP/IP stack API
*       Linux BM implementation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#ifndef LINUX
# error "Linux only"
#endif
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_tun.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <extUtils/osNetworkStack/osNetworkStack.h>

/* interface file descriptor */
int tnFd = -1;
int tnFd_port[SWITCH_MAX_PORT];
/* interface name */
static char tnName[IFNAMSIZ];
static char tnNamePort[SWITCH_MAX_PORT][IFNAMSIZ];
/* forward declaration */
GT_STATUS prvOsNetworkStackIfconfig(
  IN const char *config
);

GT_STATUS prvOsNetworkStackIfconfigByName(
  IN const char *config,
  IN        char *name
);
/*******************************************************************************
* prvOsNetworkStackDeviceOpen
*
* DESCRIPTION:
*       Open and initialize OS network device (usually tap0)
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
GT_STATUS prvOsNetworkStackDevicePortOpen(int portIndex)
{
    struct ifreq ifr;
    if (tnFd_port[portIndex] >= 0)
        return GT_OK;
    printf("%s.%d ============== portIndex %d \n",__FILE__,__LINE__,portIndex);
    /* open the clone device */
    if ( (tnFd_port[portIndex] = open("/dev/net/tun", O_RDWR)) < 0 )
    {
        cpssOsPrintf("prvOsNetworkStackDeviceOpen(): "
                    "Failed to open /dev/net/tun: %s\r\n",
                    strerror(errno));
        printf("%s.%d ============== portIndex %d failed \n",__FILE__,__LINE__,portIndex);
        return GT_FAIL;
    }
    printf("%s.%d ============== %d \n",__FILE__,__LINE__,tnFd_port[portIndex] );
    /* preparation of the struct ifr, of type "struct ifreq" */
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    /* the kernel will try to allocate the "next" device of the TAP type */

    /* try to create the device */
    if (ioctl(tnFd_port[portIndex], TUNSETIFF, (void *) &ifr) < 0)
    {
        cpssOsPrintf("prvOsNetworkStackDeviceOpen(): "
                    "Failed to clone TAP device (TUNSETIFF): %s\r\n",
                    strerror(errno));
        close(tnFd_port[portIndex]);
        tnFd_port[portIndex] = -1;
        printf("%s.%d ============== portIndex %d failed \n",__FILE__,__LINE__,portIndex);
        return GT_FAIL;
    }

#if 0
    if(ioctl(tnFd_port[portIndex], TUNSETPERSIST, 1) < 0){
    	 cpssOsPrintf("set TUNSETPERSIST failed: %s\n",
    	                    strerror(errno));
      return GT_FAIL;
    }
#endif
    /* if the operation was successful, write back the name of the
     * interface to the variable tnName, so we will know it
     */
    strcpy(tnNamePort[portIndex], ifr.ifr_name);
    printf("%s.%d ==============  ifr.ifr_name %s, portIndex %d \n",__FILE__,__LINE__,ifr.ifr_name,portIndex);
    prvOsNetworkStackIfconfigByName("mtu 1500", ifr.ifr_name);

    return GT_OK;
}

GT_STATUS prvOsNetworkStackDeviceOpen(void)
{
#if 0
    struct ifreq ifr;
    if (tnFd >= 0)
        return GT_OK;
    printf("%s.%d ============== \n",__FILE__,__LINE__);
    /* open the clone device */
    if ( (tnFd = open("/dev/net/tun", O_RDWR)) < 0 )
    {
        cpssOsPrintf("prvOsNetworkStackDeviceOpen(): "
                    "Failed to open /dev/net/tun: %s\r\n",
                    strerror(errno));
        return GT_FAIL;
    }
    printf("%s.%d ============== \n",__FILE__,__LINE__);
    /* preparation of the struct ifr, of type "struct ifreq" */
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    /* the kernel will try to allocate the "next" device of the TAP type */

    /* try to create the device */
    if (ioctl(tnFd, TUNSETIFF, (void *) &ifr) < 0)
    {
        cpssOsPrintf("prvOsNetworkStackDeviceOpen(): "
                    "Failed to clone TAP device (TUNSETIFF): %s\r\n",
                    strerror(errno));
        close(tnFd);
        tnFd = -1;
        return GT_FAIL;
    }

    /* if the operation was successful, write back the name of the
     * interface to the variable tnName, so we will know it
     */
    strcpy(tnName, ifr.ifr_name);
    printf("%s.%d ==============  ifr.ifr_name %s \n",__FILE__,__LINE__,ifr.ifr_name);

    prvOsNetworkStackIfconfig("mtu 1500");
#endif
    {
    	int i = 0;
    	for(i = 0; i < SWITCH_MAX_PORT; i++)
    	{
    		tnFd_port[i] = -1;
    		prvOsNetworkStackDevicePortOpen(i);
    	}
    }
    return GT_OK;
}



/*******************************************************************************
* prvOsNetworkStackDeviceClose
*
* DESCRIPTION:
*       Close OS network device (usually tap0)
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
GT_STATUS prvOsNetworkStackDeviceClose(void)
{
    if (tnFd < 0)
        return GT_OK;
    prvOsNetworkStackIfconfig("down");
    close(tnFd);
    tnFd = -1;
    return GT_OK;
}

/*******************************************************************************
* prvOsNetworkStackDeviceRead
*
* DESCRIPTION:
*       Read packet from OS netwrok stack
*
* INPUTS:
*       pktLen      - length of pktBuf
*
* OUTPUTS:
*       pktBuf      - pointer for packet data
*       pktLen      - actual length of packet data
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvOsNetworkStackDeviceRead(
   OUT   GT_U8  *pktBuf,
   INOUT GT_U32 *pktLen
)
{
    int nread;
    if (tnFd < 0)
        return GT_NOT_INITIALIZED;
    while(1)
    {
        nread = read(tnFd, pktBuf, (size_t)(*pktLen));
        if (nread > 0)
            break;
        if (errno != EINTR)
            return GT_FAIL;
    }
    *pktLen = (GT_U32)nread;
    return GT_OK;
}

GT_STATUS prvOsNetworkStackDevicePortRead(
   OUT   GT_U8  *pktBuf,
   INOUT GT_U32 *pktLen,
   IN    GT_U32  portIndex
)
{
    int nread;
    if (tnFd_port[portIndex] < 0)
        return GT_NOT_INITIALIZED;
    while(1)
    {
        nread = read(tnFd_port[portIndex], pktBuf, (size_t)(*pktLen));
        if (nread > 0)
            break;
        if (errno != EINTR)
            return GT_FAIL;
    }
    *pktLen = (GT_U32)nread;
    return GT_OK;
}

/*******************************************************************************
* prvOsNetworkStackDeviceWrite
*
* DESCRIPTION:
*       Write packet to OS netwrok stack
*
* INPUTS:
*       pktBuf      - pointer for packet data
*       pktLen      - length of pktBuf
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
GT_STATUS prvOsNetworkStackDeviceWrite(
  IN    GT_U8   *pktBuf,
  IN    GT_U32  pktLen
)
{
    int nwrite;
    if (tnFd < 0)
        return GT_NOT_INITIALIZED;
    nwrite = write(tnFd, pktBuf, (size_t)pktLen);
    return (nwrite < 0) ? GT_FAIL : GT_OK;
}


GT_STATUS prvOsNetworkStackDevicePortWrite(
  IN    GT_U8   *pktBuf,
  IN    GT_U32  pktLen,
  IN    GT_U32  portIndex
)
{
    int nwrite;
    if (tnFd_port[portIndex] < 0)
        return GT_NOT_INITIALIZED;

    nwrite = write(tnFd_port[portIndex], pktBuf, (size_t)pktLen);
    return (nwrite < 0) ? GT_FAIL : GT_OK;
}

/*******************************************************************************
* prvOsNetworkStackMacGet
*
* DESCRIPTION:
*       Get mac address of OS network device (usually tap0)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       etherPtr    - pointer to store mac address
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvOsNetworkStackMacGet(
  OUT GT_ETHERADDR *etherPtr
)
{
    struct ifreq ifr;

    if (tnFd < 0)
        return GT_NOT_INITIALIZED;

    memset(&ifr, 0, sizeof(ifr));
    if (ioctl(tnFd, SIOCGIFHWADDR, (void *) &ifr) < 0)
    {
        return GT_FAIL;
    }
    memcpy(etherPtr->arEther, ifr.ifr_hwaddr.sa_data, 6);
    return GT_OK;
}

GT_STATUS prvOsNetworkStackPortMacGet(
  OUT GT_ETHERADDR *etherPtr,
  IN  int portIndex
)
{
    struct ifreq ifr;

    if (tnFd_port[portIndex] < 0)
        return GT_NOT_INITIALIZED;

    memset(&ifr, 0, sizeof(ifr));
    if (ioctl(tnFd_port[portIndex], SIOCGIFHWADDR, (void *) &ifr) < 0)
    {
        return GT_FAIL;
    }
    memcpy(etherPtr->arEther, ifr.ifr_hwaddr.sa_data, 6);
    return GT_OK;
}

/*******************************************************************************
* prvOsNetworkStackMacSet
*
* DESCRIPTION:
*       Get mac address of OS network device (usually tap0)
*
* INPUTS:
*       etherPtr    - mac address to set
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
GT_STATUS prvOsNetworkStackMacSet(
  IN  GT_ETHERADDR *etherPtr
)
{
    struct ifreq ifr;

    if (tnFd < 0)
        return GT_NOT_INITIALIZED;
    
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, tnName);
    memcpy(ifr.ifr_hwaddr.sa_data, etherPtr->arEther, 6);
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

    if (ioctl(tnFd, SIOCSIFHWADDR, (void *) &ifr) < 0)
    {
        return GT_FAIL;
    }
    return GT_OK;

}

GT_STATUS prvOsNetworkStackPortMacSet(
  IN  GT_ETHERADDR *etherPtr
)
{
	int i;
    struct ifreq ifr;
#if 0
    if (tnFd < 0)
        return GT_NOT_INITIALIZED;
#endif
    for(i = 0 ; i < SWITCH_MAX_PORT; i++)
    {
    	if(tnFd_port[i] < 0)
    		continue;
		memset(&ifr, 0, sizeof(ifr));
		strcpy(ifr.ifr_name, tnNamePort[i]);
		memcpy(ifr.ifr_hwaddr.sa_data, etherPtr->arEther, 6);
		ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
		/*etherPtr->arEther[5]++;*/

		if (ioctl(tnFd_port[i], SIOCSIFHWADDR, (void *) &ifr) < 0)
		{
			return GT_FAIL;
		}

    }
    return GT_OK;

}

/*******************************************************************************
* prvOsNetworkStackIfconfig
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
*       prvOsNetworkStackIfconfig("up inet 10.1.2.3 netmask 255.255.255.0")
*
*******************************************************************************/
GT_STATUS prvOsNetworkStackIfconfig(
  IN const char *config
)
{
    char cmd[BUFSIZ];

    if (BUFSIZ <= (strlen("ifconfig ") + strlen(tnName) + strlen(config)))
    {
        return GT_BAD_PARAM;
    }
    cpssOsSprintf(cmd, "ifconfig %s %s", tnName, config); 
#define ignore_unused_result(_cmd) { int rc = _cmd; (void)sizeof rc; }
    ignore_unused_result(system(cmd));
    return GT_OK;
}

GT_STATUS prvOsNetworkStackIfconfigByName(
  IN const char *config,
  IN        char *name
)
{
    char cmd[BUFSIZ];

    if (BUFSIZ <= (strlen("ifconfig ") + strlen(name) + strlen(config)))
    {
        return GT_BAD_PARAM;
    }
    cpssOsSprintf(cmd, "ifconfig %s %s", name, config);
#define ignore_unused_result(_cmd) { int rc = _cmd; (void)sizeof rc; }
    ignore_unused_result(system(cmd));
    return GT_OK;
}
