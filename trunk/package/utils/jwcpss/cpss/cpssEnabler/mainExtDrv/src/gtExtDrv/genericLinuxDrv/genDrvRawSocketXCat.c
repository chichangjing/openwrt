/*******************************************************************************
 *                Copyright 2015, MARVELL SEMICONDUCTOR, LTD.                   *
 * THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
 * NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
 * OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
 * DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
 * THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
 * IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
 *                                                                              *
 * MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
 * MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
 * SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
 * (MJKK), MARVELL ISRAEL LTD. (MSIL).                                          *
 ********************************************************************************
 * genDrvRawSocketXCat.c
 *
 * DESCRIPTION:
 *       This file includes all needed functions for raw socket support on
 *       xCat/xCat2 (xCat driver)
 *
 * DEPENDENCIES:
 *       None.
 *
 * FILE REVISION NUMBER:
 *       $Revision: 1 $
 *
 *******************************************************************************/
#define  _SVID_SOURCE
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>              /* ioctl        */
#include <sys/select.h>

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>

#include <gtOs/gtOs.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>
#include "kerneldrv/include/presteraGlob.h"
#include "kerneldrv/include/presteraSmiGlob.h"


extern GT_32 gtPpFd;                /* pp file descriptor           */

extern extDrvEthTxMode_ENT gTxMode;

#ifdef PRESTERA_SMI_DEBUG
# define DBG_INFO(x)     fprintf x
#else
# define DBG_INFO(x)
#endif

#define SOCK_RAW_MV SOCK_PACKET
#define MAX_DEVICES 256
#define MAXPACKET 2048

static GT_TASK rawSocketTid;
static int activeDevice[MAX_DEVICES];
static int sockFD[MAX_DEVICES];
static GT_BOOL openAllNicsDone = GT_FALSE;

extern GT_RawSocketRx_FUNCPTR userRawSocketRxCbFunc;
extern GT_Tx_COMPLETE_FUNCPTR userTxCbFunc;

static void rem_crlf(char *buf)
{
  char *cp;
  int len = strlen(buf);

  buf[len] = 0;

  if ((cp = strchr(buf, '\n')) != NULL)
    *cp = 0;
  if ((cp = strchr(buf, '\r')) != NULL)
    *cp = 0;
}

static int makeSocket(int device)
{
  int fd;
  struct ifreq ifr;
  struct sockaddr_ll sa;

  fd = socket(PF_PACKET, SOCK_RAW_MV, htons(ETH_P_ALL));

  if (fd < 0)
  {
    perror("rx socket error");
    exit (1);
  }

  sprintf(ifr.ifr_name, "p%d", device);

  /* get the index */
  if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
  {
    perror("ioctl: SIOCGIFINDEX");
    exit (1);
  }

  /* Bind to the interface name */
  memset(&sa, 0, sizeof(sa));
  sa.sll_family = AF_PACKET;
  sa.sll_ifindex = ifr.ifr_ifindex;
  sa.sll_protocol = htons(ETH_P_ALL);
  sa.sll_hatype = ARPHRD_ETHER;

  if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)))
  {
    perror("bind error");
    exit(1);
  }

  /* Base the buffer size on the interface MTU */
  if (ioctl(fd, SIOCGIFMTU, &ifr) < 0 )
  {
    perror("ioctl SIOCGIFMTU");
    exit(1);
  }
  return fd;
}

static GT_STATUS openAllNics(int *sockFD)
{
  int *fd = sockFD;
  int i, j;
  FILE *fp;
  char buf[128];
  char dev_name[16];
  int num;
  GT_BOOL activeDeviceFound = GT_FALSE;

  memset(activeDevice, 0, sizeof(activeDevice));
  fp = popen("/sbin/ifconfig -a", "r");
  while (fgets(buf, sizeof(buf), fp))
  {
    if(strstr(buf, "Link encap:"))
    {
      if (buf[0] != 'p')
        continue;
      sscanf(&buf[1], "%d", &num);
      sprintf(dev_name, "p%d", num);
      for (j = 0; j < 2; j++) /* try the next 2  lines */
      {
        fgets(buf, sizeof(buf), fp);
        rem_crlf(buf);
        if (strstr(buf, "UP ") && strstr(buf, "RUNNING "))
        {
          activeDeviceFound = GT_TRUE;
          activeDevice[num] = 1;
          continue;
        }
      }
    }
  }
  fclose(fp);

  if (activeDeviceFound == GT_FALSE)
    return GT_FAIL;

  for (i = 0; i < MAX_DEVICES; i++)
  {
    if (activeDevice[i])
      fd[i] = makeSocket(i);
  }
  return GT_OK;
}

/*******************************************************************************
* prvExtDrvSendRawSocket
*
* DESCRIPTION:
*       This function transmits a packet through the linux raw socket
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segement length.
*       numOfSegments   - The number of segment in segment list.
*       txQueue         - The TX queue.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvExtDrvSendRawSocket
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
)
{
  /* this function can be called in synch mode only! */
  int i;
  int len = 0;
  char *packet;
  char *p;
  int dsa_tag, port_num;

  for (i = 0; i < numOfSegments; i++)
    len += segmentLen[i];

  if (len == 0)
    return GT_OK;

  p = packet = (char *)malloc(len);

  for (i = 0; i < numOfSegments; i++)
  {
    osMemCpy(p, segmentList[i], segmentLen[i]);
    p += segmentLen[i];
  }

  dsa_tag = *(int *)&packet[12];
  port_num = (dsa_tag >> 11) & 0x1f;
  if(sendto(sockFD[port_num], packet, len, 0,NULL, 0) < 0)
  {
    osPrintf("sendto error. packet data:\n");
  }

  if ((gTxMode == extDrvEthTxMode_synch_E) && (userTxCbFunc != NULL))
    userTxCbFunc(segmentList, numOfSegments);

  free(packet);
  return GT_OK;
}


/*******************************************************************************
 * rawSocketTask
 *
 * DESCRIPTION:
 *       Network Interface Interrupt handler task.
 *
 * INPUTS:
 *       param1  - device number
 *       param2  - ISR cookie
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_FAIL - on error
 *
 * COMMENTS:
 *       None
 *
 *******************************************************************************/
static unsigned __TASKCONV rawSocketTask(GT_VOID* unused)
{
  fd_set rfds;
  int maxfd = 0;
  int clilen;
  int n;
  unsigned long rc;
  int i;
  char *packet;

  DBG_INFO((stderr,"rawSocketTask-Start %s\n", strerror(errno)));


  /* open all active nics */
  rc = openAllNics(sockFD);

  openAllNicsDone = GT_TRUE;
  if (rc != GT_OK)
  {
    /* no pnn nics exist. get out */
    return 0;
  }

  fprintf(stderr, "rawSocketTask reading raw packets from devices: ");

  for (i = 0; i < MAX_DEVICES; i++)
  {
    if (activeDevice[i])
      fprintf(stderr, "p%d ", i);
  }
  fprintf(stderr, "\n");

  for ( ; ; )
  {
    FD_ZERO(&rfds);
    for (i = 0; i < MAX_DEVICES; i++)
    {
      if (activeDevice[i])
      {
        FD_SET(sockFD[i], &rfds);
        maxfd = (maxfd < sockFD[i]) ? sockFD[i] : maxfd;
      }
    }

    maxfd++;
    if (select(maxfd, &rfds, NULL, NULL, NULL) < 0)
    {
      perror("select()");
      exit(1);
    }

    for (i = 0; i < MAX_DEVICES; i++)
      if (FD_ISSET(sockFD[i], &rfds))
      {
        clilen = MAXPACKET;
        packet = (char *)osMalloc(MAXPACKET);

        packet +=4;

        /* printf(">>>> malloced packet=0x%08x\n", packet); */

        n = recvfrom(sockFD[i], packet, MAXPACKET, 0, NULL, (socklen_t *)&clilen);
        if (n < 0)
        {
          perror("read_raw_packets: recvfrom error");
          printf(">>>> packet=0x%08x\n", packet);
          exit(1);
        }

        /* all packets arriving here have the full dsa tag.
           "i" is the port number */

        if (userRawSocketRxCbFunc != NULL)
          userRawSocketRxCbFunc((GT_U8_PTR)packet, n, 0);
      }
  }
  /* no return */
}

/*******************************************************************************
 * extDrvEthMuxSet
 *
 * DESCRIPTION: Sets the mux mode to one of cpss, raw, linux
 *
 * INPUTS:
 *       portNum   - The port number for the action
 *       portType  - one of the modes: cpss, raw, Linux
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK always
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthMuxSet
(
 IN unsigned long portNum,
 IN extDrvEthNetPortType_ENT portType
 )
{
  MUX_PARAM muxParam;

  muxParam.portNum = portNum;
  muxParam.portType = portType;

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_MUXSET, &muxParam) != 0)
  {
    fprintf(stderr, "Fail to set port %d mux to port-type %d, errno(%s)\n",
            portNum, (int)portType, strerror(errno));
    return GT_FAIL;
  }

  return GT_OK;
}

/*******************************************************************************
 * extDrvEthMuxGet
 *
 * DESCRIPTION: Get the mux mosde of the port
 *
 * INPUTS:
 *       portNum   - The port number for the action
 *
 * OUTPUTS:
 *       portType  - port type information
 *
 * RETURNS:
 *       GT_OK always
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthMuxGet
(
 IN unsigned long portNum,
 OUT extDrvEthNetPortType_ENT *portTypeP
 )
{
  MUX_PARAM muxParam;

  muxParam.portNum = portNum;

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_MUXGET, &muxParam) != 0)
  {
    fprintf(stderr, "Fail to get mux info for port %d, errno(%s)\n",
            portNum, strerror(errno));
    return GT_FAIL;
  }

  *portTypeP = muxParam.portType;
  return GT_OK;
}

/*******************************************************************************
 * prvExtDrvRawSocketEnable
 *
 * DESCRIPTION: Enable raw socket interface
 *
 * INPUTS:
 *       None
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS prvExtDrvRawSocketEnable(GT_VOID)
{
  MUX_PARAM muxParam;
  int i;

  /* wait for openAllNics to finish */
  while(openAllNicsDone == GT_FALSE);

  for (i = 0; i < MAX_DEVICES; i++)
  {
    if (!activeDevice[i])
      continue;

    muxParam.portNum = i;
    muxParam.portType = bspEthNetPortType_raw_E;

    if (ioctl(gtPpFd, PRESTERA_SMI_IOC_MUXSET, &muxParam) != 0)
    {
      fprintf(stderr, "Fail to set port %d mux to raw, errno(%s)\n",
              i, strerror(errno));
      return GT_FAIL;
    }
  }

  return GT_OK;
}

/*******************************************************************************
* extDrvLinuxModeSet
*
* DESCRIPTION:
*       Set port <portNum> to Linux Mode (Linux Only)
*
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
*       ip1, ip2, ip3, ip4 - The ip address to assign to the port, 4 numbers
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - always,
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS extDrvLinuxModeSet
(
 IN GT_U32 portNum,
 IN GT_U32 ip1,
 IN GT_U32 ip2,
 IN GT_U32 ip3,
 IN GT_U32 ip4
)
{
  char buf[128];
  GT_U8   dev;
  GT_U16 vid;
  CPSS_PORTS_BMP_STC  portsMembers;
  CPSS_PORTS_BMP_STC  portsTagging;
  CPSS_DXCH_BRG_VLAN_INFO_STC  cpssVlanInfo;
  CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;

  sprintf(buf, "ifconfig p%d up %d.%d.%d.%d", portNum, ip1, ip2, ip3, ip4);
  extDrvEthMuxSet(portNum, extDrvEthNetPortType_linux_E);

  system(buf);

  /* write vlan entry 4000 to direct <portNum> packets to cpu */

  dev = 0;
  vid = 4000;
  osMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
  osMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));
  osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));
  osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));

  CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembers, portNum);
  portsTaggingCmd.portsCmd[portNum] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;

  cpssVlanInfo.unkSrcAddrSecBreach = GT_FALSE;
  cpssVlanInfo.unregNonIpMcastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
  cpssVlanInfo.unregIpv4McastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
  cpssVlanInfo.unregIpv6McastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
  cpssVlanInfo.unkUcastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
  cpssVlanInfo.unregIpv4BcastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
  cpssVlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
  cpssVlanInfo.ipv4IgmpToCpuEn = GT_FALSE;
  cpssVlanInfo.mirrToRxAnalyzerEn = GT_FALSE;
  cpssVlanInfo.ipv6IcmpToCpuEn = GT_FALSE;
  cpssVlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
  cpssVlanInfo.ipv4IpmBrgMode = CPSS_BRG_IPM_SGV_E;
  cpssVlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_SGV_E;
  cpssVlanInfo.ipv4IpmBrgEn = GT_FALSE;
  cpssVlanInfo.ipv6IpmBrgEn = GT_FALSE;
  cpssVlanInfo.ipv6SiteIdMode = CPSS_IP_SITE_ID_INTERNAL_E;
  cpssVlanInfo.ipv4UcastRouteEn = GT_FALSE;
  cpssVlanInfo.ipv4McastRouteEn = GT_FALSE;
  cpssVlanInfo.ipv6UcastRouteEn = GT_FALSE;
  cpssVlanInfo.ipv6McastRouteEn = GT_FALSE;
  cpssVlanInfo.stgId = 0;
  cpssVlanInfo.autoLearnDisable = GT_TRUE;
  cpssVlanInfo.naMsgToCpuEn = GT_FALSE;
  cpssVlanInfo.mruIdx = 0;
  cpssVlanInfo.bcastUdpTrapMirrEn = GT_FALSE;
  cpssVlanInfo.vrfId = 0;
  cpssVlanInfo.floodVidx = 4095;
  cpssVlanInfo.floodVidxMode = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E;
  cpssVlanInfo.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
  cpssVlanInfo.ucastLocalSwitchingEn = GT_TRUE;
  cpssVlanInfo.mcastLocalSwitchingEn = GT_TRUE;

  cpssVlanInfo.mirrToRxAnalyzerIndex = 0;
  cpssVlanInfo.mirrToTxAnalyzerEn = GT_FALSE;
  cpssVlanInfo.mirrToTxAnalyzerIndex = 0;
  cpssVlanInfo.fidValue = vid;
  cpssVlanInfo.unknownMacSaCmd = CPSS_PACKET_CMD_FORWARD_E;
  cpssVlanInfo.ipv4McBcMirrToAnalyzerEn = GT_FALSE;
  cpssVlanInfo.ipv4McBcMirrToAnalyzerIndex = 0;
  cpssVlanInfo.ipv6McMirrToAnalyzerEn = GT_FALSE;
  cpssVlanInfo.ipv6McMirrToAnalyzerIndex = 0;

  cpssDxChBrgVlanEntryWrite(dev,
                            vid,
                            &portsMembers,
                            &portsTagging,
                            &cpssVlanInfo,
                            &portsTaggingCmd);


  /* associate <portNum> with vlanid 4000 */

  cpssDxChBrgVlanPortVidSet(dev, portNum, 0/*CPSS_DIRECTION_INGRESS_E*/,vid);

  sprintf(buf, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
  printf("\nextDrvLinuxModeSet, setting port %d mode=linux, ip=%s, vlanid=%d\n\n",
         portNum, buf, vid);

  return GT_OK;
}


/*******************************************************************************
* prvExtDrvRawSocketInit
*
* DESCRIPTION:
*       Initialize raw socket operations
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - if success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvExtDrvRawSocketInit(GT_VOID)
{
    GT_STATUS rc;

    rc = osTaskCreate("rawSocketTask", 0, 0xa000, rawSocketTask , NULL, &rawSocketTid);

    if (rc != GT_OK)
    {
      fprintf(stderr, "prvExtDrvRawSocketInit: osTaskCreate(): %d\n", rc);
      return GT_FAIL;
    }
    return GT_OK;
}
