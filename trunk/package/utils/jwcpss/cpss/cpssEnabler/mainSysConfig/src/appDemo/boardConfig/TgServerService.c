/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* TgServerService.c
*
* DESCRIPTION:
*       Includes TG server initialization and services.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
/* includes */
#include <cmdShell/common/cmdCommon.h>

#include <appDemo/boardConfig/ethitems.h>
#include <appDemo/boardConfig/fstitems.h>

#include <common/tgfCommon.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <utf/private/prvUtfHelpers.h>
#include <cpss/generic/port/private/prvCpssPortTypes.h>
#include <common/tgfPortGen.h>
#include <trafficEngine/tgfTrafficTable.h>
#include <cpss/generic/version/cpssGenStream.h>
#include <cpssCommon/private/prvCpssCrc.h>
#include <utf/utfMain.h>

#include <stdio.h>
/*#include <stdlib.h>*/

#ifdef EXMXPM_FAMILY
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortStat.h>
#else
#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/


#ifdef  LINUX
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


#else
#ifdef  _VXWORKS
#include "vxWorks.h"
#include "sockLib.h"
#include "inetLib.h"
#include "taskLib.h"
#include "stdioLib.h"
#include "strLib.h"
#include "ioLib.h"
#include "fioLib.h"
#include <cpss/extServices/private/prvCpssBindFunc.h>

#else
#ifdef  __FreeBSD__


#else
#ifdef WIN32
#include "winsock.h" // library for WSAStartup
#include "windows.h"
#include <time.h>

#else

#error "OS_NOT_SET"

#endif  /* WIN32 */
#endif  /* __FreeBSD__ */
#endif  /* _VXWORKS */
#endif  /* LINUX */


#ifdef LINUX_SIM

#else
#ifdef ASIC_SIMULATION

#endif  /* ASIC_SIMULATION */
#endif  /* LINUX_SIM */

/*******************************************************************************
 * Local usage variables
 ******************************************************************************/

#define TCP_NODELAY_LOCAL  1  /* Turn off Nagle's algorithm. */
#define TG_SERVER_THREAD_PRIO                        6
#define TG_SOCKET_HENDLER_THREAD_PRIO        6

#define NUM_OF_PORTS                                        128
#define NUM_OF_TESTED_PORTS                                4
#define NUM_OF_VFD                                                3
#define SERVER_MAX_CONNECTIONS                        5
#define CAPTURE_BUFFER_SIZE                                0x600
#define TRIGGERS_PACKET_BUFFER_SIZE                0x600   /* 768000 - 750K Bytes = 0x600*500 packets. */
#define JAMBO_PACKET_SIZE                                10240   /*  10K Bytes */
#define READ_BUFFER_SIZE                256

/* flags for TG server task */
static volatile GT_BOOL TgServerTaskInitFlag                        = GT_FALSE;
static volatile GT_BOOL TgServer_Triggers_Mode_Flag                = GT_FALSE;
static volatile GT_BOOL TgServer_Capture_Mode_Flag                = GT_FALSE;
static volatile GT_BOOL TgServer_Capture_Start_Set_Flag        = GT_FALSE;
static volatile GT_BOOL TgServer_WS_Mode_Flag                        = GT_FALSE;

static volatile GT_U32  prvTgfFwModeEthFlag                                = 0x1; /* ETH_PCL_SET == default value. */

/* flag for TG server debug */
static volatile GT_BOOL TgServer_Print_Flag                        = GT_FALSE;
static volatile GT_BOOL TgServer_Print_Flag_DBG_01        = GT_FALSE;
static volatile GT_BOOL TgServer_Print_Flag_DBG_02        = GT_FALSE; /* DBG Triggers. */
static volatile GT_BOOL TgServer_Print_Flag_DBG_03        = GT_FALSE; /* DBG Counters. */
static volatile GT_BOOL TgServer_Print_Flag_DBG_04        = GT_FALSE; /* DBG WS Counters. */
static volatile GT_BOOL TgServer_Print_Flag_DBG_KA        = GT_FALSE; /* DBG KA Client.   */
static volatile GT_BOOL TgServer_Print_Flag_DBG_KA_SocHndl = GT_FALSE; /* DBG KA Client, dbg Socket Hendler.   */

static volatile GT_BOOL Trace_Packet_Print_Flag                = GT_FALSE; /* DBG tgfTrafficTracePacket. */

/* Signal read counters if decrement is needed though to Triggers on port. */
static volatile GT_BOOL TxPortTriggerFlag[NUM_OF_PORTS];

/* captureType shadow. */
GT_U32 captureType[NUM_OF_PORTS]; /* Set to the shadow */

static volatile ETHTransmit pData;

/* TG server task ID */
GT_TASK   TgServerTid;

/* SocketHandler task ID */
GT_TASK   SocketHandlerTid;

/* Ports Packets counter. */
static volatile uint64 ulPortBurstCount[NUM_OF_PORTS];

/* Ports Octets counter. */
static volatile uint64 ulPortOctetsCount[NUM_OF_PORTS];

/* Ports Packets shadow counter. */
static volatile uint64 ulPortShadowBurstCount[NUM_OF_PORTS];

/* Ports Octets shadow counter. */
static volatile uint64 ulPortShadowOctetsCount[NUM_OF_PORTS];

/* Save Extra packets transmited from port (by CPU). */
static volatile uint64 ulPortExtraBurstCount[NUM_OF_PORTS];

/* Save Extra Octets transmited from port (by CPU). */
static volatile uint64 ulPortExtraOctetsCount[NUM_OF_PORTS];

/* Save WS packets transmited from port. */
static volatile uint64 ulPortWsBurstCount[NUM_OF_PORTS];

/* Tx Packets save buffer. */
/* static GT_U8 ulTxPacketSave[JAMBO_PACKET_SIZE]; */

/* Ports Tx Packets save buffer One. */
static GT_U8 ulPortTxPacketSave_One[NUM_OF_TESTED_PORTS + 1][JAMBO_PACKET_SIZE];

/* Ports Tx Packets save buffer Two. */
static GT_U8 ulPortTxPacketSave_Two[NUM_OF_TESTED_PORTS + 1][JAMBO_PACKET_SIZE];

/* ================================================= */
/*            ports save parameters                  */
/* ================================================= */

typedef GT_U8   UL_PACKET_BUFFER_SIZE[5];
typedef GT_U32  UL_PACKET_SIZE[5];

/* struct for packet save parameters */
typedef struct
{
    UL_PACKET_BUFFER_SIZE        PortTxNumberSave;
    UL_PACKET_BUFFER_SIZE        PortTxFlagSave;
        UL_PACKET_SIZE                        PortTxPacketSizeSave_One;
        UL_PACKET_SIZE                        PortTxPacketSizeSave_Two;
} UL_PACKET_SAVE_STC;

/* ports used in the test. (for Tx). */
static UL_PACKET_SAVE_STC ulPortTxFlagSave =  {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},                /* PortTxNumberSave         */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},                /* PortTxFlagSave           */
    {0x00, 0x00, 0x00, 0x00, 0x00},                /* PortTxPacketSizeSave_One        */
    {0x00, 0x00, 0x00, 0x00, 0x00}                /* PortTxPacketSizeSave_Two        */
};

/* ================================================= */

/* Tx Port # save for capture. */
static volatile GT_U32 ulTxPortNumber;

/* Ports Packets counter save for capture. */
static volatile uint64 ulPortBurstCountSave[NUM_OF_PORTS];

/* Ports Packet size counter save for capture. */
static volatile GT_U32 ulPortPacketSizeSave[NUM_OF_PORTS];

/*
 * Typedef: struct TG_SERVER_PARAMS_STC
 *
 * Description:
 *      This structure is used to pass parameters
 *      to newly created TG task
 *
 * Fields:
 *      portNumber
 *      TgTaskName
 *      multipleInstances
 *      isConsole
 */

typedef struct TG_SERVER_PARAMS_STC {
    int portNumber;
    int SocketNumber;
    char *TgTaskName;
    GT_BOOL multipleInstances;
    GT_BOOL isConsole;
} TG_SERVER_PARAMS_STC;


#ifdef  LINUX
    extern void bzero (char *buffer, int nbytes);
        extern int inet_ntoa();
        extern int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#else
#ifdef WIN32

#else
#ifdef  _VXWORKS
        extern int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#else
        extern char * inet_ntoa(struct in_addr);
        extern         unsigned long         inet_addr (char *);
#endif
#endif
#endif

extern unsigned int sleep (unsigned int __seconds);

extern GT_STATUS tgfTrafficTableRxPcktTblClear(GT_VOID);

/******************************************************************************\
 *              Default device\port numbers initialization                    *
\******************************************************************************/
/* ports array */
extern GT_U32   prvTgfPortsArray[PRV_TGF_MAX_PORTS_NUM_CNS];

#if defined(PPC_CPU) || defined(POWERPC_CPU)
        extern int select();
#endif /* PPC_CPU || POWERPC_CPU */

/********************************************/ /* TODO: <--- a.y. 18/01/2012 */
/* extern GT_STATUS tgfTrafficTracePacketPrivateByteSet(IN GT_BOOL trace); */


#ifdef  LINUX
        fd_set read_fd;
        fd_set write_fd;
#else
#ifdef  _VXWORKS
        fd_set read_fd;
        fd_set write_fd;
#else
#ifdef  WIN32
        fd_set read_fd;
        fd_set write_fd;
#else
        int read_fd;
        int write_fd;
#endif        /* _VXWORKS */
#endif        /* WIN32 */
#endif        /* LINUX */

GT_VOID   TgServerServiceStart(int portNumber);
GT_VOID   TgServerTask (TG_SERVER_PARAMS_STC* params);
GT_VOID   SocketHandler(TG_SERVER_PARAMS_STC* params);
GT_STATUS SIM_NSSetPortMappingMode_01(int sockfd);
GT_STATUS SIM_HTCardModels(int sockfd);
GT_STATUS SIM_HTSetCommand(int sockfd);
GT_STATUS SIM_HTSetCommand_02(int sockfd);
GT_STATUS SIM_HTSetCommand_03(int sockfd);
GT_STATUS SIM_HTSetStructure_01(int sockfd);
GT_STATUS SIM_HTSetStructure_02(int sockfd);
GT_STATUS SIM_HTSetStructure_03(int sockfd);
GT_STATUS SIM_HTSetStructure_04(int sockfd);
GT_STATUS SIM_HTGetStructure_01(int sockfd);
GT_STATUS SIM_HTGetStructure_02(int sockfd);
GT_STATUS SIM_HTGetStructure_03(int sockfd);
GT_STATUS SIM_HTGetStructure_04(int sockfd);
GT_STATUS SIM_HTGetStructure_05(int sockfd);
GT_STATUS SIM_HTRun(int sockfd);
GT_STATUS SIM_HTStop(int sockfd);

#ifdef WIN32
        char dostuff(int sock); /* function prototype */
#else
#ifdef  _VXWORKS
        char dostuff(int sock); /* function prototype */
#else /* Linux */
#ifdef  LINUX
        GT_U8 dostuff(int); /* function prototype */
#endif  /* Linux */
#endif  /* _VXWORKS */
#endif  /* WIN32 */

void error(const char *msg)
{
    perror(msg);
    return;
}

/*******************************************************************************
* readSocketData -
* DESCRIPTION:
*       Read data from TCP / UDP socket.
*
* INPUTS:
*       sock         - The socket FD.
*                buffer        - The Input buffer.
*                nLeft        -
* OUTPUTS:

* RETURNS:
*       int                - The number of data bytes read from socket.
*
*******************************************************************************/
int readSocketData(int sock, char *buffer, int nLeft)
{
        int n = 0x0;

        #ifdef _VXWORKS
                n = recv(sock, buffer, nLeft, 0);
        #else
        #ifdef WIN32
                n = recv(sock, buffer, nLeft, 0);
        #else /* Linux */
        #ifdef  LINUX
                n = read(sock, buffer, nLeft);
        #endif  /* Linux */
        #endif /* WIN32 */
        #endif /* _VXWORKS */

        return (n);
}

/*******************************************************************************
* writeSocketData -
* DESCRIPTION:
*       Write data to TCP / UDP socket.
*
* INPUTS:
*       sock         - The socket FD.
*                buffer        - The Output buffer.
*                nLeft        -
* OUTPUTS:

* RETURNS:
*       int                - The number of data bytes written to socket.
*
*******************************************************************************/
int writeSocketData(int sock, char *buffer, int sizeofBuffer)
{
        int n = 0x0;

        #ifdef _VXWORKS
                n = send(sock, buffer, sizeofBuffer, 0);
        #else
        #ifdef WIN32
                n = send(sock, buffer, sizeofBuffer, 0);
        #else /* Linux */
        #ifdef  LINUX
                n = write(sock, buffer, sizeofBuffer);
        #endif  /* Linux */
        #endif /* WIN32 */
        #endif /* _VXWORKS */

        return (n);
}

int bind_socket(int socket, struct sockaddr *serv_addr, int size_of_serv_addr)
{
        int retVal = 0x0;

#ifdef _VXWORKS

        if (bind(socket, serv_addr, size_of_serv_addr) == ERROR)
        {
        perror("listen");
        close (socket);
        return (-1);
        }
#else
#ifdef WIN32

        if (bind((SOCKET)socket, serv_addr, size_of_serv_addr) == SOCKET_ERROR)
        {
        fprintf(stderr,"Server: bind() failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return (-1);
    }
#else /* Linux */
#ifdef  LINUX

        if (bind(socket, serv_addr, size_of_serv_addr) < 0)
        {
                error("ERROR on binding");
        return (-1);
        }
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        return (retVal);
}

/**************************************
// Name: ctoi ( char to int )
// Description:Converts char to int. atoi wont do that.
// By: Rob L Backus
//
// Inputs:Takes a char for the arg.
//
// Returns:Returns 0 through 9 ( if char c == 0 through 9 ) else returns -1.
//
// Side Effects:Too simple to have side effects.
//
//ctoi will return 0 to 9 if char c== 0 to 9
//else ctoi will return -1;
// (c)har (to) (i)nt */
int ctoi(char c)
{
        int x=0;
        for(x=0; x<10; x++)
        {
                if(c==((char)48+x)) return x;
        }
        return -1;
}


#define HVFD_NONE                0
#define HVFD_RANDOM        1
#define HVFD_INCR                2
#define HVFD_DECR                3
#define HVFD_ENABLED        4
#define HVFD_STATIC        5

/* transmit mode: */
#define CONTINUOUS_PACKET_MODE 0
#define SINGLE_BURST_MODE      1
#define MULTI_BURST_MODE       2
#define CONTINUOUS_BURST_MODE  3
#define ECHO_MODE              4

/******************************* Test packet **********************************/

/* default VLAN0 Id */
#define PRV_TGF_VLANID_CNS  0

/* LENGTH of packet headers included */
static GT_U32 PACKET_HEADERS_LEN         = 0x0;
static GT_U32 L2HeaderSize                         = 12;   /* TGF_L2_HEADER_SIZE_CNS;   */
static GT_U32 VlanTagHeaderSize         = 4;    /* TGF_VLAN_TAG_SIZE_CNS;    */
static GT_U32 EthetTypeHeaderSize        = 2;    /* TGF_ETHERTYPE_SIZE_CNS;   */
static GT_U32 IpV4HeaderSize                 = 20;   /* TGF_IPV4_HEADER_SIZE_CNS; */
static GT_U32 IpV6HeaderSize                 = 40;   /* TGF_IPV6_HEADER_SIZE_CNS; */
static GT_U32 MPLSHeaderSize                 = 4;    /* TGF_MPLS_HEADER_SIZE_CNS; */
static GT_U8  CRC_SIZE                     = 4;    /*                           */

/* array of source MACs for the tests */
static TGF_MAC_ADDR prvTgfSaMacArr[] = {{0x00, 0x00, 0x11, 0x11, 0x11, 0x11},
                                        {0x00, 0x00, 0x11, 0x11, 0x11, 0x11},
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x88},
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x88}
                                       };

/* array of destination MACs for the ports */
static TGF_MAC_ADDR prvTgfDaMacArr[] = {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06},
                                        {0x01, 0x02, 0x03, 0x04, 0x05, 0x07},
                                        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                        {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}
                                       };


/* L2 part of packet */
static TGF_PACKET_L2_STC prvTgfPacketL2Part = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},                        /* dstMac */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}                        /* srcMac */
};


GT_U8  SaveDataBuffer[1536];

/* VLAN_TAG part */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart;


/* packet's ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherTypePart = {TGF_ETHERTYPE_0800_IPV4_TAG_CNS};

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4Part;

/* packet's IPv6 */
static TGF_PACKET_IPV6_STC prvTgfPacketIpv6Part;

/* packet's IPv4 over IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4OverIpv4Part;

/* packet's IPv6 over IPv4 */
static TGF_PACKET_IPV6_STC prvTgfPacketIpv6OverIpv4Part;

/* MPLS part */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsMplsPart;

/* packet's VFD */
static TGF_VFD_INFO_STC    prvTgfVfdArray[NUM_OF_VFD]; /* vfd Array -- used to increment the tested bytes. */

GT_U32 numVfd[NUM_OF_TESTED_PORTS]; /* number of VFDs in vfdArray */

GT_U32 vfdArrayStart[NUM_OF_TESTED_PORTS];   /* Array Start location. */


/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfPacketPartArray[NUM_OF_TESTED_PORTS + 1][sizeof(TGF_PACKET_PART_STC)];

/* PAYLOAD part */
#ifdef WIN32
        static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart;
#else
#ifdef  _VXWORKS
        static TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart;
#else /* Linux */
#ifdef  LINUX
        static volatile TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart;
#endif  /* Linux */
#endif  /* _VXWORKS */
#endif  /* WIN32 */


static TGF_PACKET_STC prvTgfPacketInfo[NUM_OF_TESTED_PORTS + 1];
static int PRV_TGF_PACKET_LEN = 0x0;

/* LENGTH of packet with CRC */
static int PRV_TGF_PACKET_CRC_LEN = 0x0;

/*
#ifndef  ASIC_SIMULATION
#define TG_SERVER_QA_TEST_SLEEP(delay) cpssOsTimerWkAfter(delay)
#else
#define TG_SERVER_QA_TEST_SLEEP(delay)
#endif
*/

static GT_U32        txRate; /* actual TX rate on port */
#if 0
                                  wsRate, /* theoretically expected WS rate */
                                  percent;/* difference persentage between expected and real rates */

static GT_BOOL rateOk = 0x0; /* is real rate correct */
#endif

/* Packet buffer for Capture.  */
static GT_U8 CapturePacket[CAPTURE_BUFFER_SIZE];

/******************************* Triggers parameters **********************************/
#ifdef WIN32
        TGF_VFD_INFO_STC        vfdArray[NUM_OF_PORTS][2];
#else
#ifdef  _VXWORKS
        static volatile TGF_VFD_INFO_STC        vfdArray[NUM_OF_PORTS][2];
#else /* Linux */
#ifdef  LINUX
        TGF_VFD_INFO_STC        vfdArray[NUM_OF_PORTS][2];
#endif  /* Linux */
#endif  /* _VXWORKS */
#endif  /* WIN32 */

        static volatile GT_U32 numTriggers[NUM_OF_PORTS], numTriggers_Arry01[NUM_OF_PORTS];
        static volatile GT_U32 TotalNumTriggers[NUM_OF_PORTS];

        GT_U8        *trigPacketBuff = NULL;
/***************************************************************************************/

/* This CRC32 Calculate function is only god for LE machenes, BE support need to be    */
/* implemented. for now, use the CPSS (Linux) function:
GT_U32 prvCpssCalcCrc32(
        IN GT_U32 crc,
    IN GT_U8  *buffer,
    IN GT_U32 byteNum
);  */

/* Calculate a CRC value to be used by CRC calculation functions. */
#define CRC32_POLYNOMIAL 0xEDB88320

GT_U32 CRC32Value(int i)
{
        int j;
        GT_U32 ulCRC;

        ulCRC = i;
        for (j=8; j>0; j--)
        {
                if (ulCRC & 1)
                        ulCRC = (ulCRC >> 1)^CRC32_POLYNOMIAL;
                else ulCRC >>= 1;
        }
        return ulCRC;
}

/* Calculate the 32-Bit CRC of a block of data all at once. */
GT_U32 CalculateBlockCRC32(GT_U32 ulCount, unsigned char *ucBuffer)
{
        GT_U32 ulTemp1;
        GT_U32 ulTemp2;
        GT_U32 ulCRC_Block = 0;

        while (ulCount-- != 0)
        {
                ulTemp1 = (ulCRC_Block >> 8) & 0x00FFFFFFL;
                ulTemp2 = CRC32Value(((int)ulCRC_Block^ * ucBuffer++)&0xff);
                ulCRC_Block = ulTemp1^ulTemp2;
        }
        return(ulCRC_Block);
}

/*******************************************************************************
* TgServerPclMirroringFlag
*
* DESCRIPTION:
*       Set app Demo Server Pcl Mirroring Flag
*
* INPUTS:
*       bool Mirriring flag
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*
 * typedef: enum TGF_CAPTURE_MODE_ENT
 *
 * Description: Enumeration of packet capture mode
 *
 * Enumerations:
 *    TGF_CAPTURE_MODE_PCL_E - packet is captured by (ingress) PCL
 *                             (the egress port is defined 'loopback' and
 *                              after pack looped it is send to CPU by IPCL)
 *
 *    TGF_CAPTURE_MODE_MIRRORING_E - packet is captured by (ingress) mirroring to cpu port
 *                                   (the egress port is defined 'loopback' and
 *                                    after pack looped it is ingress mirrored to the CPU)
 *
*        typedef enum
*        {
*                TGF_CAPTURE_MODE_PCL_E,
*                TGF_CAPTURE_MODE_MIRRORING_E
*        } TGF_CAPTURE_MODE_ENT;
*
*******************************************************************************/
GT_VOID TgServerPclMirroringFlag(GT_BOOL portPclMirrorFlag)
{

        if(portPclMirrorFlag == GT_TRUE) /* ETH_MIRRORING_SET */
                prvTgfFwModeEthFlag = 0x2;
        else if(portPclMirrorFlag == GT_FALSE)  /* ETH_PCL_SET <-- default value. */
                prvTgfFwModeEthFlag = 0x1;

        if(TgServer_Print_Flag_DBG_04)
        {
                if(portPclMirrorFlag)
                        printf("\n prvTgfFwModeEthFlag set to ETH_MIRRORING.\n");
                else {printf("\n prvTgfFwModeEthFlag set to ETH_PCL.\n");}
        }

        return;
}


/*******************************************************************************
* QaTestsDBGPrintSetOnOff
*
* DESCRIPTION:
*       Set QA DBG print to terminal on or off.
*
* INPUTS:
*       status - on / off (true / false)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID
*
*******************************************************************************/
GT_VOID QaTestsDBGPrintSetOnOff(GT_BOOL status, int flagNumber)
{
        if(status)
        {
            switch (flagNumber)
            {
                case 0: TgServer_Print_Flag        = GT_TRUE; break;
                case 1: TgServer_Print_Flag_DBG_01         = GT_TRUE; break;
                case 2: TgServer_Print_Flag_DBG_02         = GT_TRUE; break;
                case 3: TgServer_Print_Flag_DBG_03         = GT_TRUE; break;
                case 4: TgServer_Print_Flag_DBG_04         = GT_TRUE; break;

                case 5: {TgServer_Print_Flag_DBG_01 = GT_TRUE;
                         TgServer_Print_Flag_DBG_02 = GT_TRUE;
                         TgServer_Print_Flag_DBG_03 = GT_TRUE; break;}

                case 6: TgServer_Print_Flag_DBG_KA  = GT_TRUE; break;

                default: break;
            }
        }
        else
        {
            switch (flagNumber)
            {
                case 0: TgServer_Print_Flag                        = GT_FALSE; break;
                case 1: TgServer_Print_Flag_DBG_01         = GT_FALSE; break;
                case 2: TgServer_Print_Flag_DBG_02         = GT_FALSE; break;
                case 3: TgServer_Print_Flag_DBG_03         = GT_FALSE; break;
                case 4: TgServer_Print_Flag_DBG_04         = GT_FALSE; break;

                case 5: {TgServer_Print_Flag_DBG_01 = GT_FALSE;
                         TgServer_Print_Flag_DBG_02 = GT_FALSE;
                         TgServer_Print_Flag_DBG_03 = GT_FALSE; break;}

                case 6: TgServer_Print_Flag_DBG_KA  = GT_FALSE; break;

                default: break;
            }
        }

        return;
}


/*******************************************************************************
* DBGTracePacketPrintSetOnOff
*
* DESCRIPTION:
*       Set QA DBG TracePacket print to terminal on or off.
*
* INPUTS:
*       status - on / off (true / false)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID
*
*******************************************************************************/
GT_VOID DBGTracePacketPrintSetOnOff(GT_BOOL status)
{
        /* tgfTrafficTracePacketPrivateByteSet(status); */

        Trace_Packet_Print_Flag = status;

        return;
}

/******************************************************************************
* TGServiceParseConvertPacket_One
*
* DESCRIPTION:
*       Parse and Convert Packet string to EUT format, save it as a struct of pointers.
*
* INPUTS:
*       GT_U8 *buffer        -
*                int        index                -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS TGServiceParseConvertPacket_One(GT_U8 *DataBuffer, int index)
{
        GT_STATUS status = GT_OK;
        GT_U32 partsCount  = 0x0;
        GT_U32 PayloadLength;
        GT_U32 PayloadHeaderStart;
        int           idx = 0x0;
        int    length = 0x0;

        GT_U8  IPV4_6Type                   = 0x0;
        GT_U8        IPV4_Over_IPV4_Type        = 0x0;
        GT_U8        IPV6_Over_IPV4_Type        = 0x0;
        GT_U16 VlanTagEtherType = 0x0;
        GT_U16 EthernetType     = 0x0;

        /* LENGTH of packet headers included */
        PACKET_HEADERS_LEN = 0x0;

        length = ulPortTxFlagSave.PortTxPacketSizeSave_One[index];

        if(TgServer_Print_Flag_DBG_KA)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n===========================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start TGServiceParseConvertPacket_One ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n===========================================\n");
        }

        prvTgfDaMacArr[0][0] = DataBuffer[0];
        prvTgfDaMacArr[0][1] = DataBuffer[1];
        prvTgfDaMacArr[0][2] = DataBuffer[2];
        prvTgfDaMacArr[0][3] = DataBuffer[3];
        prvTgfDaMacArr[0][4] = DataBuffer[4];
        prvTgfDaMacArr[0][5] = DataBuffer[5];

        /* set destination MAC address in Packet */
        memcpy(prvTgfPacketL2Part.daMac, prvTgfDaMacArr[0], sizeof(prvTgfPacketL2Part.daMac));

        prvTgfSaMacArr[0][0] = DataBuffer[6];
        prvTgfSaMacArr[0][1] = DataBuffer[7];
        prvTgfSaMacArr[0][2] = DataBuffer[8];
        prvTgfSaMacArr[0][3] = DataBuffer[9];
        prvTgfSaMacArr[0][4] = DataBuffer[10];
        prvTgfSaMacArr[0][5] = DataBuffer[11];

        /* set source MAC address in Packet */
        memcpy(prvTgfPacketL2Part.saMac, prvTgfSaMacArr[0], sizeof(prvTgfPacketL2Part.saMac));

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
        {
                PRV_UTF_LOG0_MAC("\n== TGServiceParseConvertPacket_One ==\n");
                for(idx = 0x0 ; idx < 6 ; idx++)
                {
                        printf /*PRV_UTF_LOG1_MAC*/("\n------- Packet L2 array, daMac = 0x%02x", (prvTgfPacketL2Part.daMac[idx]));
                }

                PRV_UTF_LOG0_MAC("\n");
                for(idx = 0x0 ; idx < 6 ; idx++)
                {
                        printf /*PRV_UTF_LOG1_MAC*/("\n------- Packet L2 array, saMac = 0x%02x", (prvTgfPacketL2Part.saMac[idx]));
                }
        }

        /* PARTS of packet [0] */
        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_L2_E;
        prvTgfPacketPartArray[index][partsCount++].partPtr = &prvTgfPacketL2Part;

        /* Add L2 header size. */
        PACKET_HEADERS_LEN += L2HeaderSize;

        VlanTagEtherType = ((DataBuffer[12] << 8) + DataBuffer[13]);

        /* VLAN part in packet. */
        if(VlanTagEtherType == TGF_ETHERTYPE_8100_VLAN_TAG_CNS /*0x8100*/)
        {
                prvTgfPacketVlanTagPart.etherType         = VlanTagEtherType;
                prvTgfPacketVlanTagPart.pri               = (TGF_PRI)((DataBuffer[14] & 0xE0) >> 5);
                prvTgfPacketVlanTagPart.cfi                         = (TGF_CFI)((DataBuffer[14] & 0x10) >> 4);
                prvTgfPacketVlanTagPart.vid                         = (TGF_VLAN_ID)(((DataBuffer[14] & 0x0F) << 8) | DataBuffer[15]);

                if(TgServer_Print_Flag_DBG_KA)
                {
                        printf /*PRV_UTF_LOG1_MAC*/("\n\nprvTgfPacketVlanTagPart_2.Type         = 0x%04x", VlanTagEtherType);
                        printf /*PRV_UTF_LOG1_MAC*/("\nprvTgfPacketVlanTagPart_2.pri         = 0x%x", prvTgfPacketVlanTagPart.pri);
                        printf /*PRV_UTF_LOG1_MAC*/("\nprvTgfPacketVlanTagPart_2.cfi         = 0x%x", prvTgfPacketVlanTagPart.cfi);
                        printf /*PRV_UTF_LOG1_MAC*/("\nprvTgfPacketVlanTagPart_2.vid         = 0x%03x", prvTgfPacketVlanTagPart.vid);
                }

                /* PARTS of packet [1] */
                prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_VLAN_TAG_E;
                prvTgfPacketPartArray[index][partsCount++].partPtr = &prvTgfPacketVlanTagPart;

                /* Add VLAN TAG header size. */
                PACKET_HEADERS_LEN += VlanTagHeaderSize;

                EthernetType = ((DataBuffer[16] << 8) + DataBuffer[17]);

                /* EthernetType part in packet. (MPLS) */
                if(EthernetType == TGF_ETHERTYPE_8847_MPLS_TAG_CNS    /*0x8847*/)
                {
                        prvTgfPacketEtherTypePart.etherType = EthernetType;

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf("\n\nprvTgfPacketEtherTypePart.etherType = 0x%04x", EthernetType);
                        }

                        /* PARTS of packet [2] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_ETHERTYPE_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr        = &prvTgfPacketEtherTypePart;

                        /* Add ETHERTYPE header size. */
                        PACKET_HEADERS_LEN += EthetTypeHeaderSize;

                        prvTgfPacketMplsMplsPart.label                 = ((DataBuffer[18] << 12)|(DataBuffer[19] << 4)|((DataBuffer[20] & 0xF0) >> 4));
                        prvTgfPacketMplsMplsPart.exp            = ((DataBuffer[20] & 0x0E) >> 1);
                        prvTgfPacketMplsMplsPart.stack                = (DataBuffer[20] & 0x01);
                        prvTgfPacketMplsMplsPart.timeToLive        = (DataBuffer[21]);

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf("\n\nprvTgfPacketMplsMplsPart.label                = 0x%05lx", prvTgfPacketMplsMplsPart.label);
                                printf("\nprvTgfPacketMplsMplsPart.exp                        = 0x%01x", prvTgfPacketMplsMplsPart.exp);
                                printf("\nprvTgfPacketMplsMplsPart.stack                = 0x%01x", prvTgfPacketMplsMplsPart.stack);
                                printf("\nprvTgfPacketMplsMplsPart.timeToLive        = 0x%02x", prvTgfPacketMplsMplsPart.timeToLive);
                        }

                        /* PARTS of packet [3] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_MPLS_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr         = &prvTgfPacketMplsMplsPart;

                        /* Add IPV4 header size. */
                        PACKET_HEADERS_LEN += MPLSHeaderSize;

                        /* MPLS has no more packet parts, all is payload. */
                        goto PAYLOAD;
                } /* if(EthernetType == TGF_ETHERTYPE_8847_MPLS_TAG_CNS / *0x8847* /) */

                /* EthernetType part in packet. */
                else if((EthernetType == TGF_ETHERTYPE_0800_IPV4_TAG_CNS    /*0x0800*/)||
                   (EthernetType == TGF_ETHERTYPE_9100_TAG_CNS         /*0x9100*/)||
                   (EthernetType == TGF_ETHERTYPE_86DD_IPV6_TAG_CNS    /*0x86DD*/)||
                   (EthernetType == TGF_ETHERTYPE_0806_ARP_TAG_CNS     /*0x0806*/)||
                   (EthernetType == TGF_ETHERTYPE_88E7_MIM_TAG_CNS     /*0x88E7*/)||
                   (EthernetType == TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS /*0x88A8*/)||
                   (EthernetType == TGF_ETHERTYPE_NON_VALID_TAG_CNS    /*0xFFFF*/))
                {
                        prvTgfPacketEtherTypePart.etherType = EthernetType;

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf /*PRV_UTF_LOG1_MAC*/("\n\nprvTgfPacketEtherTypePart.etherType = 0x%04x", EthernetType);
                        }

                        /* PARTS of packet [2] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_ETHERTYPE_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr        = &prvTgfPacketEtherTypePart;

                        /* Add ETHERTYPE header size. */
                        PACKET_HEADERS_LEN += EthetTypeHeaderSize;
                }

                IPV4_6Type = ((DataBuffer[18] & 0xF0) + (DataBuffer[18] & 0x0F));

                /* IPV4 part in packet. */
                if(IPV4_6Type == 0x45)
                {
                        prvTgfPacketIpv4Part.version                 = ((DataBuffer[18] & 0xF0) >> 4);
                        prvTgfPacketIpv4Part.headerLen            = (DataBuffer[18] & 0x0F);
                        prvTgfPacketIpv4Part.typeOfService        = (DataBuffer[19]);
                        prvTgfPacketIpv4Part.totalLen                 = (((DataBuffer[20]) << 8) | DataBuffer[21]);
                        prvTgfPacketIpv4Part.id                                 = (((DataBuffer[22]) << 8) | DataBuffer[23]);
                        prvTgfPacketIpv4Part.flags                         = (DataBuffer[24] & 0xE0);
                        prvTgfPacketIpv4Part.offset                         = ((((DataBuffer[24]) << 8) & 0x1F) | DataBuffer[25]);
                        prvTgfPacketIpv4Part.timeToLive                = (DataBuffer[26]);
                        prvTgfPacketIpv4Part.protocol                = (DataBuffer[27]);
                        prvTgfPacketIpv4Part.csum                         = (((DataBuffer[28]) << 8) | DataBuffer[29]);

                        prvTgfPacketIpv4Part.srcAddr[0]         = (DataBuffer[30]);
                        prvTgfPacketIpv4Part.srcAddr[1]         = (DataBuffer[31]);
                        prvTgfPacketIpv4Part.srcAddr[2]         = (DataBuffer[32]);
                        prvTgfPacketIpv4Part.srcAddr[3]         = (DataBuffer[33]);

                        prvTgfPacketIpv4Part.dstAddr[0]         = (DataBuffer[34]);
                        prvTgfPacketIpv4Part.dstAddr[1]         = (DataBuffer[35]);
                        prvTgfPacketIpv4Part.dstAddr[2]         = (DataBuffer[36]);
                        prvTgfPacketIpv4Part.dstAddr[3]         = (DataBuffer[37]);

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                PRV_UTF_LOG1_MAC("\n\nprvTgfPacketIpv4Part.version                = 0x%01x", prvTgfPacketIpv4Part.version);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.headerLen                = 0x%01x", prvTgfPacketIpv4Part.headerLen);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.typeOfService        = 0x%02x", prvTgfPacketIpv4Part.typeOfService);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.totalLen                = 0x%04x", prvTgfPacketIpv4Part.totalLen);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.id                                = 0x%04x", prvTgfPacketIpv4Part.id);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.flags                        = 0x%01x", prvTgfPacketIpv4Part.flags);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.offset                        = 0x%02x", prvTgfPacketIpv4Part.offset);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.timeToLive                = 0x%01x", prvTgfPacketIpv4Part.timeToLive);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.protocol                = 0x%01x", prvTgfPacketIpv4Part.protocol);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.csum                        = 0x%02x", prvTgfPacketIpv4Part.csum);

                                for(idx = 0x0 ; idx < 4 ; idx++)
                                {
                                        PRV_UTF_LOG2_MAC("\nprvTgfPacketIpv4Part.IPsrcAddr[%d] = 0x%02x", idx, (prvTgfPacketIpv4Part.srcAddr[idx]));
                                }

                                PRV_UTF_LOG0_MAC("\n");
                                for(idx = 0x0 ; idx < 4 ; idx++)
                                {
                                        PRV_UTF_LOG2_MAC("\nprvTgfPacketIpv4Part.IPdstAddr[%d] = 0x%02x", idx, (prvTgfPacketIpv4Part.dstAddr[idx]));
                                }
                                PRV_UTF_LOG0_MAC("\n");
                        }

                        /* PARTS of packet [3] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_IPV4_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr         = &prvTgfPacketIpv4Part;

                        /* Add IPV4 header size. */
                        PACKET_HEADERS_LEN += IpV4HeaderSize;
                }
                /* IPV4 part in packet. */
                else if((IPV4_6Type & 0xF0) == 0x60)
                {
                        prvTgfPacketIpv6Part.version                = ((DataBuffer[18] & 0xF0) >> 4);
                        prvTgfPacketIpv6Part.trafficClass        = (((DataBuffer[18] & 0x0F) << 4) |((DataBuffer[19] & 0xF0) >> 4));
                        prvTgfPacketIpv6Part.flowLabel                = (((DataBuffer[19] & 0x0F) << 16)|(DataBuffer[20] << 8)|(DataBuffer[21]));
                        prvTgfPacketIpv6Part.payloadLen                = ((DataBuffer[22] << 8)|(DataBuffer[23]));
                        prvTgfPacketIpv6Part.nextHeader                =  (DataBuffer[24]);
                        prvTgfPacketIpv6Part.hopLimit                =  (DataBuffer[25]);

                        prvTgfPacketIpv6Part.srcAddr[0]                = ((DataBuffer[26] << 8)|(DataBuffer[27]));
                        prvTgfPacketIpv6Part.srcAddr[1]         = ((DataBuffer[28] << 8)|(DataBuffer[29]));
                        prvTgfPacketIpv6Part.srcAddr[2]         = ((DataBuffer[30] << 8)|(DataBuffer[31]));
                        prvTgfPacketIpv6Part.srcAddr[3]         = ((DataBuffer[32] << 8)|(DataBuffer[33]));
                        prvTgfPacketIpv6Part.srcAddr[4]         = ((DataBuffer[34] << 8)|(DataBuffer[35]));
                        prvTgfPacketIpv6Part.srcAddr[5]         = ((DataBuffer[36] << 8)|(DataBuffer[37]));
                        prvTgfPacketIpv6Part.srcAddr[6]         = ((DataBuffer[38] << 8)|(DataBuffer[39]));
                        prvTgfPacketIpv6Part.srcAddr[7]         = ((DataBuffer[40] << 8)|(DataBuffer[41]));

                        prvTgfPacketIpv6Part.dstAddr[0]         = ((DataBuffer[42] << 8)|(DataBuffer[43]));
                        prvTgfPacketIpv6Part.dstAddr[1]         = ((DataBuffer[44] << 8)|(DataBuffer[45]));
                        prvTgfPacketIpv6Part.dstAddr[2]         = ((DataBuffer[46] << 8)|(DataBuffer[47]));
                        prvTgfPacketIpv6Part.dstAddr[3]         = ((DataBuffer[48] << 8)|(DataBuffer[49]));
                        prvTgfPacketIpv6Part.dstAddr[4]         = ((DataBuffer[50] << 8)|(DataBuffer[51]));
                        prvTgfPacketIpv6Part.dstAddr[5]         = ((DataBuffer[52] << 8)|(DataBuffer[53]));
                        prvTgfPacketIpv6Part.dstAddr[6]         = ((DataBuffer[54] << 8)|(DataBuffer[55]));
                        prvTgfPacketIpv6Part.dstAddr[7]         = ((DataBuffer[56] << 8)|(DataBuffer[57]));

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                PRV_UTF_LOG1_MAC("\n\nprvTgfPacketIpv6Part.version                = 0x%01x", prvTgfPacketIpv6Part.version);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.trafficClass        = 0x%02x", prvTgfPacketIpv6Part.trafficClass);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.flowLabel                = 0x%05x", (unsigned int)prvTgfPacketIpv6Part.flowLabel);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.payloadLen                = 0x%04x", prvTgfPacketIpv6Part.payloadLen);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.nextHeader                = 0x%02x", prvTgfPacketIpv6Part.nextHeader);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.hopLimit                = 0x%01x", prvTgfPacketIpv6Part.hopLimit);

                                for(idx = 0x0 ; idx < 8 ; idx++)
                                {
                                        PRV_UTF_LOG2_MAC("\nprvTgfPacketIpv6Part.srcIPAddr[%d] = 0x%04x", idx, (prvTgfPacketIpv6Part.srcAddr[idx]));
                                }

                                PRV_UTF_LOG0_MAC("\n");
                                for(idx = 0x0 ; idx < 8 ; idx++)
                                {
                                        PRV_UTF_LOG2_MAC("\nprvTgfPacketIpv6Part.dstIPAddr[%d] = 0x%04x", idx, (prvTgfPacketIpv6Part.dstAddr[idx]));
                                }
                                PRV_UTF_LOG0_MAC("\n");
                        }

                        /* PARTS of packet [4] */
                        prvTgfPacketPartArray[index][partsCount].type          = TGF_PACKET_PART_IPV6_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr = &prvTgfPacketIpv6Part;

                        /* Add IPV6 header size. */
                        PACKET_HEADERS_LEN += IpV6HeaderSize;
                }

                IPV4_Over_IPV4_Type = ((DataBuffer[38] & 0xF0) + (DataBuffer[38] & 0x0F));
                IPV6_Over_IPV4_Type =  (DataBuffer[38] & 0xF0);

                /* packet's IPv4 over IPv4 */
                if(IPV4_Over_IPV4_Type == 0x45)
                {
                        prvTgfPacketIpv4OverIpv4Part.version                 = ((DataBuffer[38] & 0xF0) >> 4);
                        prvTgfPacketIpv4OverIpv4Part.headerLen            = (DataBuffer[38] & 0x0F);
                        prvTgfPacketIpv4OverIpv4Part.typeOfService        = (DataBuffer[39]);
                        prvTgfPacketIpv4OverIpv4Part.totalLen                 = (((DataBuffer[40]) << 8) | DataBuffer[41]);
                        prvTgfPacketIpv4OverIpv4Part.id                                 = (((DataBuffer[42]) << 8) | DataBuffer[43]);
                        prvTgfPacketIpv4OverIpv4Part.flags                         = (DataBuffer[44] & 0xE0);
                        prvTgfPacketIpv4OverIpv4Part.offset                         = ((((DataBuffer[44]) << 8) & 0x1F) | DataBuffer[45]);
                        prvTgfPacketIpv4OverIpv4Part.timeToLive                = (DataBuffer[46]);
                        prvTgfPacketIpv4OverIpv4Part.protocol                = (DataBuffer[47]);
                        prvTgfPacketIpv4OverIpv4Part.csum                         = (((DataBuffer[48]) << 8) | DataBuffer[49]);

                        prvTgfPacketIpv4OverIpv4Part.srcAddr[0]         = (DataBuffer[50]);
                        prvTgfPacketIpv4OverIpv4Part.srcAddr[1]         = (DataBuffer[51]);
                        prvTgfPacketIpv4OverIpv4Part.srcAddr[2]         = (DataBuffer[52]);
                        prvTgfPacketIpv4OverIpv4Part.srcAddr[3]         = (DataBuffer[53]);

                        prvTgfPacketIpv4OverIpv4Part.dstAddr[0]         = (DataBuffer[54]);
                        prvTgfPacketIpv4OverIpv4Part.dstAddr[1]         = (DataBuffer[55]);
                        prvTgfPacketIpv4OverIpv4Part.dstAddr[2]         = (DataBuffer[56]);
                        prvTgfPacketIpv4OverIpv4Part.dstAddr[3]         = (DataBuffer[57]);

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf("\n\nprvTgfPacketIpv4OverIpv4Part.version                = 0x%01x", prvTgfPacketIpv4OverIpv4Part.version);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.headerLen                = 0x%01x", prvTgfPacketIpv4OverIpv4Part.headerLen);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.typeOfService        = 0x%02x", prvTgfPacketIpv4OverIpv4Part.typeOfService);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.totalLen                        = 0x%04x", prvTgfPacketIpv4OverIpv4Part.totalLen);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.id                                = 0x%04x", prvTgfPacketIpv4OverIpv4Part.id);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.flags                        = 0x%01x", prvTgfPacketIpv4OverIpv4Part.flags);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.offset                        = 0x%02x", prvTgfPacketIpv4OverIpv4Part.offset);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.timeToLive                = 0x%01x", prvTgfPacketIpv4OverIpv4Part.timeToLive);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.protocol                        = 0x%01x", prvTgfPacketIpv4OverIpv4Part.protocol);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.csum                                = 0x%02x", prvTgfPacketIpv4OverIpv4Part.csum);

                                for(idx = 0x0 ; idx < 4 ; idx++)
                                {
                                        printf("\nprvTgfPacketIpv4OverIpv4Part.IPsrcAddr[%d] =        0x%02x", idx, (prvTgfPacketIpv4OverIpv4Part.srcAddr[idx]));
                                }

                                printf("\n");
                                for(idx = 0x0 ; idx < 4 ; idx++)
                                {
                                        printf("\nprvTgfPacketIpv4OverIpv4Part.IPdstAddr[%d] =        0x%02x", idx, (prvTgfPacketIpv4OverIpv4Part.dstAddr[idx]));
                                }
                                printf("\n");
                        }

                        /* PARTS of packet [3] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_IPV4_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr = &prvTgfPacketIpv4OverIpv4Part;

                        /* Add IPV4 header size. */
                        PACKET_HEADERS_LEN += IpV4HeaderSize;
                }

                /* IPV6OverIPV4 part in packet. */
                else if(IPV6_Over_IPV4_Type == 0x60)
                {
                        prvTgfPacketIpv6OverIpv4Part.version                = ((DataBuffer[38] & 0xF0) >> 4);
                        prvTgfPacketIpv6OverIpv4Part.trafficClass        = (((DataBuffer[38] & 0x0F) << 8) |((DataBuffer[39] & 0xF0) >> 8));
                        prvTgfPacketIpv6OverIpv4Part.flowLabel                = (((DataBuffer[39] & 0x0F) << 16)|(DataBuffer[40] << 8)|(DataBuffer[41]));
                        prvTgfPacketIpv6OverIpv4Part.payloadLen                = ((DataBuffer[42] << 8)|(DataBuffer[43]));
                        prvTgfPacketIpv6OverIpv4Part.nextHeader                =  (DataBuffer[44]);
                        prvTgfPacketIpv6OverIpv4Part.hopLimit                =  (DataBuffer[45]);

                        prvTgfPacketIpv6OverIpv4Part.srcAddr[0]                = ((DataBuffer[46] << 8)|(DataBuffer[47]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[1]         = ((DataBuffer[48] << 8)|(DataBuffer[49]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[2]         = ((DataBuffer[50] << 8)|(DataBuffer[51]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[3]         = ((DataBuffer[52] << 8)|(DataBuffer[53]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[4]         = ((DataBuffer[54] << 8)|(DataBuffer[55]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[5]         = ((DataBuffer[56] << 8)|(DataBuffer[57]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[6]         = ((DataBuffer[58] << 8)|(DataBuffer[59]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[7]         = ((DataBuffer[60] << 8)|(DataBuffer[61]));

                        prvTgfPacketIpv6OverIpv4Part.dstAddr[0]         = ((DataBuffer[62] << 8)|(DataBuffer[63]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[1]         = ((DataBuffer[64] << 8)|(DataBuffer[65]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[2]         = ((DataBuffer[66] << 8)|(DataBuffer[67]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[3]         = ((DataBuffer[68] << 8)|(DataBuffer[69]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[4]         = ((DataBuffer[70] << 8)|(DataBuffer[71]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[5]         = ((DataBuffer[72] << 8)|(DataBuffer[73]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[6]         = ((DataBuffer[74] << 8)|(DataBuffer[75]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[7]         = ((DataBuffer[76] << 8)|(DataBuffer[77]));

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf("\n\nprvTgfPacketIpv6OverIpv4Part.version        = 0x%01x", prvTgfPacketIpv6OverIpv4Part.version);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.trafficClass = 0x%02x", prvTgfPacketIpv6OverIpv4Part.trafficClass);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.flowLabel        = 0x%05x", (unsigned int)prvTgfPacketIpv6OverIpv4Part.flowLabel);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.payloadLen        = 0x%04x", prvTgfPacketIpv6OverIpv4Part.payloadLen);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.nextHeader        = 0x%02x", prvTgfPacketIpv6OverIpv4Part.nextHeader);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.hopLimit                = 0x%01x", prvTgfPacketIpv6OverIpv4Part.hopLimit);

                                for(idx = 0x0 ; idx < 8 ; idx++)
                                {
                                        printf("\nprvTgfPacketIpv6OverIpv4Part.srcIPAddr[%d] = 0x%04x", idx, (prvTgfPacketIpv6OverIpv4Part.srcAddr[idx]));
                                }

                                printf("\n");
                                for(idx = 0x0 ; idx < 8 ; idx++)
                                {
                                        printf("\nprvTgfPacketIpv6OverIpv4Part.dstIPAddr[%d] = 0x%04x", idx, (prvTgfPacketIpv6OverIpv4Part.dstAddr[idx]));
                                }
                                printf("\n");
                        }

                        /* PARTS of packet [4] */
                        prvTgfPacketPartArray[index][partsCount].type          = TGF_PACKET_PART_IPV6_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr = &prvTgfPacketIpv6OverIpv4Part;

                        /* Add IPV6overIPV4 header size. */
                        PACKET_HEADERS_LEN += IpV6HeaderSize;
                }
        } /* if(VlanTagEtherType == TGF_ETHERTYPE_8100_VLAN_TAG_CNS / *0x8100* /) */
        else /* UnTaged packet. */
        {
                EthernetType = ((DataBuffer[12] << 8) + DataBuffer[13]);

                /* EthernetType part in packet. (MPLS) */
                if(EthernetType == TGF_ETHERTYPE_8847_MPLS_TAG_CNS    /*0x8847*/)
                {
                        prvTgfPacketEtherTypePart.etherType = EthernetType;

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf("\n\nprvTgfPacketEtherTypePart.etherType = 0x%04x", EthernetType);
                        }

                        /* PARTS of packet [2] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_ETHERTYPE_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr        = &prvTgfPacketEtherTypePart;

                        /* Add ETHERTYPE header size. */
                        PACKET_HEADERS_LEN += EthetTypeHeaderSize;

                        prvTgfPacketMplsMplsPart.label                 = ((DataBuffer[14] << 12)|(DataBuffer[15] << 4)|((DataBuffer[16] & 0xF0) >> 4));
                        prvTgfPacketMplsMplsPart.exp            = ((DataBuffer[16] & 0x0E) >> 1);
                        prvTgfPacketMplsMplsPart.stack                = (DataBuffer[16] & 0x01);
                        prvTgfPacketMplsMplsPart.timeToLive        = (DataBuffer[17]);

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf("\n\nprvTgfPacketMplsMplsPart.label                = 0x%05lx", prvTgfPacketMplsMplsPart.label);
                                printf("\nprvTgfPacketMplsMplsPart.exp                        = 0x%01x", prvTgfPacketMplsMplsPart.exp);
                                printf("\nprvTgfPacketMplsMplsPart.stack                = 0x%01x", prvTgfPacketMplsMplsPart.stack);
                                printf("\nprvTgfPacketMplsMplsPart.timeToLive        = 0x%02x", prvTgfPacketMplsMplsPart.timeToLive);
                        }

                        /* PARTS of packet [3] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_MPLS_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr         = &prvTgfPacketMplsMplsPart;

                        /* Add IPV4 header size. */
                        PACKET_HEADERS_LEN += MPLSHeaderSize;

                        /* MPLS has no more packet parts, all is payload. */
                        goto PAYLOAD;
                } /* if(EthernetType == TGF_ETHERTYPE_8847_MPLS_TAG_CNS / *0x8847* /) */

                /* EthernetType part in packet. */
                if((EthernetType == TGF_ETHERTYPE_0800_IPV4_TAG_CNS    /*0x0800*/)||
                   (EthernetType == TGF_ETHERTYPE_9100_TAG_CNS         /*0x9100*/)||
                   (EthernetType == TGF_ETHERTYPE_8847_MPLS_TAG_CNS    /*0x8847*/)||
                   (EthernetType == TGF_ETHERTYPE_86DD_IPV6_TAG_CNS    /*0x86DD*/)||
                   (EthernetType == TGF_ETHERTYPE_0806_ARP_TAG_CNS     /*0x0806*/)||
                   (EthernetType == TGF_ETHERTYPE_88E7_MIM_TAG_CNS     /*0x88E7*/)||
                   (EthernetType == TGF_ETHERTYPE_88A8_SERVICE_TAG_CNS /*0x88A8*/)||
                   (EthernetType == TGF_ETHERTYPE_NON_VALID_TAG_CNS    /*0xFFFF*/))
                {
                        prvTgfPacketEtherTypePart.etherType = EthernetType;

                        if(TgServer_Print_Flag_DBG_KA == GT_TRUE)  /* <--- TODO: */
                        {
                                PRV_UTF_LOG1_MAC("\n\nprvTgfPacketEtherTypePart.etherType = 0x%04x", EthernetType);
                        }

                        /* PARTS of packet [1] */
                        prvTgfPacketPartArray[index][partsCount].type          = TGF_PACKET_PART_ETHERTYPE_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr = &prvTgfPacketEtherTypePart;

                        /* Add ETHERTYPE header size. */
                        PACKET_HEADERS_LEN += EthetTypeHeaderSize;
                }

                IPV4_6Type = ((DataBuffer[14] & 0xF0) + (DataBuffer[14] & 0x0F));

                /* IPV4 part in packet. */
                if(IPV4_6Type == 0x45)
                {
                        prvTgfPacketIpv4Part.version                 = ((DataBuffer[14] & 0xF0) >> 4);
                        prvTgfPacketIpv4Part.headerLen            = (DataBuffer[14] & 0x0F);
                        prvTgfPacketIpv4Part.typeOfService        = (DataBuffer[15]);
                        prvTgfPacketIpv4Part.totalLen                 = (((DataBuffer[16]) << 8) | DataBuffer[17]);
                        prvTgfPacketIpv4Part.id                                 = (((DataBuffer[18]) << 8) | DataBuffer[19]);
                        prvTgfPacketIpv4Part.flags                         = (DataBuffer[20] & 0xE0);
                        prvTgfPacketIpv4Part.offset                         = ((((DataBuffer[20]) << 8) & 0x1F) | DataBuffer[21]);
                        prvTgfPacketIpv4Part.timeToLive                = (DataBuffer[22]);
                        prvTgfPacketIpv4Part.protocol                = (DataBuffer[23]);
                        prvTgfPacketIpv4Part.csum                         = (((DataBuffer[24]) << 8) | DataBuffer[25]);

                        prvTgfPacketIpv4Part.srcAddr[0]         = (DataBuffer[26]);
                        prvTgfPacketIpv4Part.srcAddr[1]         = (DataBuffer[27]);
                        prvTgfPacketIpv4Part.srcAddr[2]         = (DataBuffer[28]);
                        prvTgfPacketIpv4Part.srcAddr[3]         = (DataBuffer[29]);

                        prvTgfPacketIpv4Part.dstAddr[0]         = (DataBuffer[30]);
                        prvTgfPacketIpv4Part.dstAddr[1]         = (DataBuffer[31]);
                        prvTgfPacketIpv4Part.dstAddr[2]         = (DataBuffer[32]);
                        prvTgfPacketIpv4Part.dstAddr[3]         = (DataBuffer[33]);

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                PRV_UTF_LOG1_MAC("\n\nprvTgfPacketIpv4Part.version                = 0x%01x", prvTgfPacketIpv4Part.version);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.headerLen                = 0x%01x", prvTgfPacketIpv4Part.headerLen);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.typeOfService        = 0x%02x", prvTgfPacketIpv4Part.typeOfService);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.totalLen                = 0x%04x", prvTgfPacketIpv4Part.totalLen);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.id                                = 0x%04x", prvTgfPacketIpv4Part.id);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.flags                        = 0x%01x", prvTgfPacketIpv4Part.flags);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.offset                        = 0x%02x", prvTgfPacketIpv4Part.offset);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.timeToLive                = 0x%01x", prvTgfPacketIpv4Part.timeToLive);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.protocol                = 0x%01x", prvTgfPacketIpv4Part.protocol);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv4Part.csum                        = 0x%02x", prvTgfPacketIpv4Part.csum);

                                for(idx = 0x0 ; idx < 4 ; idx++)
                                {
                                        PRV_UTF_LOG2_MAC("\nprvTgfPacketIpv4Part.IPsrcAddr[%d] =        0x%02x", idx, (prvTgfPacketIpv4Part.srcAddr[idx]));
                                }

                                PRV_UTF_LOG0_MAC("\n");
                                for(idx = 0x0 ; idx < 4 ; idx++)
                                {
                                        PRV_UTF_LOG2_MAC("\nprvTgfPacketIpv4Part.IPdstAddr[%d] =        0x%02x", idx, (prvTgfPacketIpv4Part.dstAddr[idx]));
                                }
                                PRV_UTF_LOG0_MAC("\n");
                        }

                        /* PARTS of packet [2] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_IPV4_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr = &prvTgfPacketIpv4Part;

                        /* Add IPV4 header size. */
                        PACKET_HEADERS_LEN += IpV4HeaderSize;
                }
                /* IPV4 part in packet. */
                else if(IPV4_6Type == 0x60)
                {
                        prvTgfPacketIpv6Part.version                = ((DataBuffer[14] & 0xF0) >> 4);
                        prvTgfPacketIpv6Part.trafficClass        = (((DataBuffer[14] & 0x0F) << 4) |((DataBuffer[15] & 0xF0) >> 4));
                        prvTgfPacketIpv6Part.flowLabel                = (((DataBuffer[15] & 0x0F) << 16)|(DataBuffer[16] << 8)|(DataBuffer[17]));
                        prvTgfPacketIpv6Part.payloadLen                = ((DataBuffer[18] << 8)|(DataBuffer[19]));
                        prvTgfPacketIpv6Part.nextHeader                =  (DataBuffer[20]);
                        prvTgfPacketIpv6Part.hopLimit                =  (DataBuffer[21]);

                        prvTgfPacketIpv6Part.srcAddr[0]                = ((DataBuffer[22] << 8)|(DataBuffer[23]));
                        prvTgfPacketIpv6Part.srcAddr[1]         = ((DataBuffer[24] << 8)|(DataBuffer[25]));
                        prvTgfPacketIpv6Part.srcAddr[2]         = ((DataBuffer[26] << 8)|(DataBuffer[27]));
                        prvTgfPacketIpv6Part.srcAddr[3]         = ((DataBuffer[28] << 8)|(DataBuffer[29]));
                        prvTgfPacketIpv6Part.srcAddr[4]         = ((DataBuffer[30] << 8)|(DataBuffer[31]));
                        prvTgfPacketIpv6Part.srcAddr[5]         = ((DataBuffer[32] << 8)|(DataBuffer[33]));
                        prvTgfPacketIpv6Part.srcAddr[6]         = ((DataBuffer[34] << 8)|(DataBuffer[35]));
                        prvTgfPacketIpv6Part.srcAddr[7]         = ((DataBuffer[36] << 8)|(DataBuffer[37]));

                        prvTgfPacketIpv6Part.dstAddr[0]         = ((DataBuffer[38] << 8)|(DataBuffer[39]));
                        prvTgfPacketIpv6Part.dstAddr[1]         = ((DataBuffer[40] << 8)|(DataBuffer[41]));
                        prvTgfPacketIpv6Part.dstAddr[2]         = ((DataBuffer[42] << 8)|(DataBuffer[43]));
                        prvTgfPacketIpv6Part.dstAddr[3]         = ((DataBuffer[44] << 8)|(DataBuffer[45]));
                        prvTgfPacketIpv6Part.dstAddr[4]         = ((DataBuffer[46] << 8)|(DataBuffer[47]));
                        prvTgfPacketIpv6Part.dstAddr[5]         = ((DataBuffer[48] << 8)|(DataBuffer[49]));
                        prvTgfPacketIpv6Part.dstAddr[6]         = ((DataBuffer[50] << 8)|(DataBuffer[51]));
                        prvTgfPacketIpv6Part.dstAddr[7]         = ((DataBuffer[52] << 8)|(DataBuffer[53]));

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                PRV_UTF_LOG1_MAC("\n\nprvTgfPacketIpv6Part.version                = 0x%01x", prvTgfPacketIpv6Part.version);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.trafficClass        = 0x%02x", prvTgfPacketIpv6Part.trafficClass);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.flowLabel                = 0x%05x", (unsigned int)prvTgfPacketIpv6Part.flowLabel);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.payloadLen                = 0x%04x", prvTgfPacketIpv6Part.payloadLen);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.nextHeader                = 0x%02x", prvTgfPacketIpv6Part.nextHeader);
                                PRV_UTF_LOG1_MAC("\nprvTgfPacketIpv6Part.hopLimit                = 0x%01x", prvTgfPacketIpv6Part.hopLimit);

                                for(idx = 0x0 ; idx < 8 ; idx++)
                                {
                                        PRV_UTF_LOG2_MAC("\nprvTgfPacketIpv6Part.srcIPAddr[%d] = 0x%04x", idx, (prvTgfPacketIpv6Part.srcAddr[idx]));
                                }

                                PRV_UTF_LOG0_MAC("\n");
                                for(idx = 0x0 ; idx < 8 ; idx++)
                                {
                                        PRV_UTF_LOG2_MAC("\nprvTgfPacketIpv6Part.dstIPAddr[%d] = 0x%04x", idx, (prvTgfPacketIpv6Part.dstAddr[idx]));
                                }
                                PRV_UTF_LOG0_MAC("\n");
                        }

                        /* PARTS of packet [3] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_IPV6_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr        = &prvTgfPacketIpv6Part;

                        /* Add IPV4 header size. */
                        PACKET_HEADERS_LEN += IpV6HeaderSize;
                }

                IPV4_Over_IPV4_Type = ((DataBuffer[38] & 0xF0) + (DataBuffer[38] & 0x0F));
                IPV6_Over_IPV4_Type =  (DataBuffer[38] & 0xF0);

                /* packet's IPv4 over IPv4 */
                if(IPV4_Over_IPV4_Type == 0x45)
                {
                        prvTgfPacketIpv4OverIpv4Part.version                 = ((DataBuffer[38] & 0xF0) >> 4);
                        prvTgfPacketIpv4OverIpv4Part.headerLen            = (DataBuffer[38] & 0x0F);
                        prvTgfPacketIpv4OverIpv4Part.typeOfService        = (DataBuffer[39]);
                        prvTgfPacketIpv4OverIpv4Part.totalLen                 = (((DataBuffer[40]) << 8) | DataBuffer[41]);
                        prvTgfPacketIpv4OverIpv4Part.id                                 = (((DataBuffer[42]) << 8) | DataBuffer[43]);
                        prvTgfPacketIpv4OverIpv4Part.flags                         = (DataBuffer[44] & 0xE0);
                        prvTgfPacketIpv4OverIpv4Part.offset                         = ((((DataBuffer[44]) << 8) & 0x1F) | DataBuffer[45]);
                        prvTgfPacketIpv4OverIpv4Part.timeToLive                = (DataBuffer[46]);
                        prvTgfPacketIpv4OverIpv4Part.protocol                = (DataBuffer[47]);
                        prvTgfPacketIpv4OverIpv4Part.csum                         = (((DataBuffer[48]) << 8) | DataBuffer[49]);

                        prvTgfPacketIpv4OverIpv4Part.srcAddr[0]         = (DataBuffer[50]);
                        prvTgfPacketIpv4OverIpv4Part.srcAddr[1]         = (DataBuffer[51]);
                        prvTgfPacketIpv4OverIpv4Part.srcAddr[2]         = (DataBuffer[52]);
                        prvTgfPacketIpv4OverIpv4Part.srcAddr[3]         = (DataBuffer[53]);

                        prvTgfPacketIpv4OverIpv4Part.dstAddr[0]         = (DataBuffer[54]);
                        prvTgfPacketIpv4OverIpv4Part.dstAddr[1]         = (DataBuffer[55]);
                        prvTgfPacketIpv4OverIpv4Part.dstAddr[2]         = (DataBuffer[56]);
                        prvTgfPacketIpv4OverIpv4Part.dstAddr[3]         = (DataBuffer[57]);

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf("\n\nprvTgfPacketIpv4OverIpv4Part.version                = 0x%01x", prvTgfPacketIpv4OverIpv4Part.version);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.headerLen                = 0x%01x", prvTgfPacketIpv4OverIpv4Part.headerLen);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.typeOfService        = 0x%02x", prvTgfPacketIpv4OverIpv4Part.typeOfService);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.totalLen                        = 0x%04x", prvTgfPacketIpv4OverIpv4Part.totalLen);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.id                                = 0x%04x", prvTgfPacketIpv4OverIpv4Part.id);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.flags                        = 0x%01x", prvTgfPacketIpv4OverIpv4Part.flags);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.offset                        = 0x%02x", prvTgfPacketIpv4OverIpv4Part.offset);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.timeToLive                = 0x%01x", prvTgfPacketIpv4OverIpv4Part.timeToLive);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.protocol                        = 0x%01x", prvTgfPacketIpv4OverIpv4Part.protocol);
                                printf("\nprvTgfPacketIpv4OverIpv4Part.csum                                = 0x%02x", prvTgfPacketIpv4OverIpv4Part.csum);

                                for(idx = 0x0 ; idx < 4 ; idx++)
                                {
                                        printf("\nprvTgfPacketIpv4OverIpv4Part.IPsrcAddr[%d] =        0x%02x", idx, (prvTgfPacketIpv4OverIpv4Part.srcAddr[idx]));
                                }

                                printf("\n");
                                for(idx = 0x0 ; idx < 4 ; idx++)
                                {
                                        printf("\nprvTgfPacketIpv4OverIpv4Part.IPdstAddr[%d] =        0x%02x", idx, (prvTgfPacketIpv4OverIpv4Part.dstAddr[idx]));
                                }
                                printf("\n");
                        }

                        /* PARTS of packet [3] */
                        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_IPV4_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr = &prvTgfPacketIpv4OverIpv4Part;

                        /* Add IPV4 header size. */
                        PACKET_HEADERS_LEN += IpV4HeaderSize;
                }

                /* IPV6OverIPV4 part in packet. */
                else if(IPV6_Over_IPV4_Type == 0x60)
                {
                        prvTgfPacketIpv6OverIpv4Part.version                = ((DataBuffer[38] & 0xF0) >> 4);
                        prvTgfPacketIpv6OverIpv4Part.trafficClass        = (((DataBuffer[38] & 0x0F) << 8) |((DataBuffer[39] & 0xF0) >> 8));
                        prvTgfPacketIpv6OverIpv4Part.flowLabel                = (((DataBuffer[39] & 0x0F) << 16)|(DataBuffer[40] << 8)|(DataBuffer[41]));
                        prvTgfPacketIpv6OverIpv4Part.payloadLen                = ((DataBuffer[42] << 8)|(DataBuffer[43]));
                        prvTgfPacketIpv6OverIpv4Part.nextHeader                =  (DataBuffer[44]);
                        prvTgfPacketIpv6OverIpv4Part.hopLimit                =  (DataBuffer[45]);

                        prvTgfPacketIpv6OverIpv4Part.srcAddr[0]                = ((DataBuffer[46] << 8)|(DataBuffer[47]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[1]         = ((DataBuffer[48] << 8)|(DataBuffer[49]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[2]         = ((DataBuffer[50] << 8)|(DataBuffer[51]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[3]         = ((DataBuffer[52] << 8)|(DataBuffer[53]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[4]         = ((DataBuffer[54] << 8)|(DataBuffer[55]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[5]         = ((DataBuffer[56] << 8)|(DataBuffer[57]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[6]         = ((DataBuffer[58] << 8)|(DataBuffer[59]));
                        prvTgfPacketIpv6OverIpv4Part.srcAddr[7]         = ((DataBuffer[60] << 8)|(DataBuffer[61]));

                        prvTgfPacketIpv6OverIpv4Part.dstAddr[0]         = ((DataBuffer[62] << 8)|(DataBuffer[63]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[1]         = ((DataBuffer[64] << 8)|(DataBuffer[65]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[2]         = ((DataBuffer[66] << 8)|(DataBuffer[67]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[3]         = ((DataBuffer[68] << 8)|(DataBuffer[69]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[4]         = ((DataBuffer[70] << 8)|(DataBuffer[71]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[5]         = ((DataBuffer[72] << 8)|(DataBuffer[73]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[6]         = ((DataBuffer[74] << 8)|(DataBuffer[75]));
                        prvTgfPacketIpv6OverIpv4Part.dstAddr[7]         = ((DataBuffer[76] << 8)|(DataBuffer[77]));

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf("\n\nprvTgfPacketIpv6OverIpv4Part.version        = 0x%01x", prvTgfPacketIpv6OverIpv4Part.version);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.trafficClass = 0x%02x", prvTgfPacketIpv6OverIpv4Part.trafficClass);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.flowLabel        = 0x%05x", (unsigned int)prvTgfPacketIpv6OverIpv4Part.flowLabel);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.payloadLen        = 0x%04x", prvTgfPacketIpv6OverIpv4Part.payloadLen);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.nextHeader        = 0x%02x", prvTgfPacketIpv6OverIpv4Part.nextHeader);
                                printf("\nprvTgfPacketIpv6OverIpv4Part.hopLimit                = 0x%01x", prvTgfPacketIpv6OverIpv4Part.hopLimit);

                                for(idx = 0x0 ; idx < 8 ; idx++)
                                {
                                        printf("\nprvTgfPacketIpv6OverIpv4Part.srcIPAddr[%d] = 0x%04x", idx, (prvTgfPacketIpv6OverIpv4Part.srcAddr[idx]));
                                }

                                printf("\n");
                                for(idx = 0x0 ; idx < 8 ; idx++)
                                {
                                        printf("\nprvTgfPacketIpv6OverIpv4Part.dstIPAddr[%d] = 0x%04x", idx, (prvTgfPacketIpv6OverIpv4Part.dstAddr[idx]));
                                }
                                printf("\n");
                        }

                        /* PARTS of packet [4] */
                        prvTgfPacketPartArray[index][partsCount].type          = TGF_PACKET_PART_IPV6_E;
                        prvTgfPacketPartArray[index][partsCount++].partPtr = &prvTgfPacketIpv6OverIpv4Part;

                        /* Add IPV6overIPV4 header size. */
                        PACKET_HEADERS_LEN += IpV6HeaderSize;
                }
        }

PAYLOAD:

        PayloadLength = (length - PACKET_HEADERS_LEN);
        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                PRV_UTF_LOG1_MAC("\n== PayloadLength = %ld ==\n", PayloadLength);

        PayloadHeaderStart = (length - PayloadLength);
        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                PRV_UTF_LOG1_MAC("\n== PayloadHeaderStart = %ld ==\n", PayloadHeaderStart);

        memset(SaveDataBuffer, 0, sizeof(SaveDataBuffer));
        memcpy(SaveDataBuffer, DataBuffer, length);

        /* PAYLOAD part */
        prvTgfPacketPayloadPart.dataLength = (GT_U32) PayloadLength;                        /* PayloadLength   */
        prvTgfPacketPayloadPart.dataPtr    = (GT_U8 *)(&SaveDataBuffer[PayloadHeaderStart]);        /* Payload dataPtr */

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                PRV_UTF_LOG1_MAC("\n== prvTgfPacketPayloadPart.dataLength = %ld ==\n", prvTgfPacketPayloadPart.dataLength);

        /* PARTS of packet [4] */
        prvTgfPacketPartArray[index][partsCount].type                  = TGF_PACKET_PART_PAYLOAD_E;
        prvTgfPacketPartArray[index][partsCount++].partPtr        = (TGF_PACKET_PAYLOAD_STC *)&prvTgfPacketPayloadPart;

        /* LENGTH of packet */
        PRV_TGF_PACKET_LEN = length;

        /* Set LENGTH of packet with CRC, for wire-speed rate calculation. */
        PRV_TGF_PACKET_CRC_LEN = PRV_TGF_PACKET_LEN + TGF_CRC_LEN_CNS;

        /* PACKET to send */
        prvTgfPacketInfo[index].totalLen   = PRV_TGF_PACKET_LEN;
        prvTgfPacketInfo[index].numOfParts = partsCount;    /*sizeof(prvTgfPacketPartArray) / sizeof(TGF_PACKET_PART_STC);*/
        prvTgfPacketInfo[index].partsArray = prvTgfPacketPartArray[index];

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
        {
                PRV_UTF_LOG1_MAC("\n== length = %ld ==\n", prvTgfPacketInfo[index].totalLen);
                PRV_UTF_LOG1_MAC("== numOfParts = %ld\n", prvTgfPacketInfo[index].numOfParts);
        }

        printf /*PRV_UTF_LOG0_MAC*/("\n");

        if(TgServer_Print_Flag_DBG_KA)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n==========================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit TGServiceParseConvertPacket_One ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n==========================================\n");
        }

        printf /*PRV_UTF_LOG0_MAC*/("\n");

        return (status);
}

/******************************************************************************
* TGServiceParseConvertPacket_Two
*
* DESCRIPTION:
*       Parse and Convert Packet string to EUT format, save it as a struct of pointers.
*
* INPUTS:
*       GT_U8 *buffer         -
*                int index                -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS TGServiceParseConvertPacket_Two(GT_U8 *DataBuffer, int index /*, GT_U32 *pMumVfd, GT_U32 *pVfdArrayStart*/)
{
        GT_STATUS status = GT_OK;
        int n = 0x0, idx = 0x0;
        GT_U32 vfdArrayStart_L = 0x0;
        GT_U32 numVfd_L = 0x0; /* Local number of VFDs in vfdArray */
        TGF_VFD_INFO_STC  *vrfPtr;

        #if (defined(PPC_CPU) || defined(POWERPC_CPU) || defined(MV_CPU_BE) || defined(CPU_ARM) || defined(INTEL64_CPU) || defined(INTEL_CPU ))
        int counter_aa = 0x0;
        unsigned char *temp = NULL;
        #endif

        if(TgServer_Print_Flag_DBG_KA)
        {
                /*PRV_UTF_LOG0_MAC("\n\n===========================================\n");*/
                printf /*PRV_UTF_LOG0_MAC*/("== Start TGServiceParseConvertPacket_Two ==");
                /*PRV_UTF_LOG0_MAC("\n===========================================\n");*/
        }

        printf /*PRV_UTF_LOG0_MAC*/("\n");

        /* Convert the buffer according to cpu type, for BE / LE modes. */
        #if (defined(PPC_CPU) || defined(POWERPC_CPU) || defined(MV_CPU_BE))

                if(TgServer_Print_Flag_DBG_01)
                        PRV_UTF_LOG0_MAC("\n== (ETHTransmit)pData:\n");

                pData.ucTransmitMode                = (DataBuffer[0]);
                pData.uiDataLength            = (((DataBuffer[3]) << 8) | DataBuffer[2]);
                pData.ucDuplexMode                = (DataBuffer[4]);
                pData.ucSpeed                         = (DataBuffer[5]);

                pData.uiCollisionBackoffAggressiveness = (((DataBuffer[7]) << 8) | DataBuffer[6]);

                pData.ulBurstCount         = (((DataBuffer[11]) << 24) | ((DataBuffer[10]) << 16) |
                                                              ((DataBuffer[ 9]) <<  8) | DataBuffer[8]);

                pData.ulMultiBurstCount = (((DataBuffer[15]) << 24) | ((DataBuffer[14]) << 16) |
                                                              ((DataBuffer[13]) <<  8) | DataBuffer[12]);

                pData.ulInterFrameGap        = (((DataBuffer[19]) << 24) | ((DataBuffer[18]) << 16) |
                                                              ((DataBuffer[17]) << 8) | DataBuffer[16]);

                pData.uiInterFrameGapScale = (((DataBuffer[23]) << 24) | ((DataBuffer[22]) << 16) |
                                                                ((DataBuffer[21]) << 8) | DataBuffer[20]);

                pData.ulInterBurstGap         = (((DataBuffer[27]) << 24) | ((DataBuffer[26]) << 16) |
                                                              ((DataBuffer[25]) << 8) | DataBuffer[24]);

                pData.uiInterBurstGapScale        = (((DataBuffer[29]) << 8) | DataBuffer[28]);
                pData.ucRandomBackground         = (DataBuffer[30]);
                pData.ucRandomLength                 = (DataBuffer[31]);
                pData.ucCRCErrors                        = (DataBuffer[32]);
                pData.ucAlignErrors                = (DataBuffer[33]);
                pData.ucSymbolErrors                 = (((DataBuffer[35]) << 8) | DataBuffer[34]);
                pData.uiDribbleBits                 = (((DataBuffer[37]) << 8) | DataBuffer[36]);
                pData.ucVFD1Mode                        = (((DataBuffer[39]) << 8) | DataBuffer[38]);
                pData.uiVFD1Offset                = (((DataBuffer[41]) << 8) | DataBuffer[40]); /* TG gets adrs * 8. */
                pData.iVFD1Range                        = (((DataBuffer[43]) << 8) | DataBuffer[42]);
                pData.ucVFD1Pattern[0]        = (DataBuffer[44]);
                pData.ucVFD1Pattern[1]        = (DataBuffer[45]);
                pData.ucVFD1Pattern[2]        = (DataBuffer[46]);
                pData.ucVFD1Pattern[3]        = (DataBuffer[47]);
                pData.ucVFD1Pattern[4]        = (DataBuffer[48]);
                pData.ucVFD1Pattern[5]        = (DataBuffer[49]);
                pData.uiVFD1CycleCount         = (((DataBuffer[51]) << 8) | DataBuffer[50]);
                pData.uiVFD1BlockCount         = (((DataBuffer[53]) << 8) | DataBuffer[52]);
                pData.ucVFD2Mode                        = (((DataBuffer[55]) << 8) | DataBuffer[54]);
                pData.uiVFD2Offset                = (((DataBuffer[57]) << 8) | DataBuffer[56]); /* TG gets adrs * 8. */
                pData.iVFD2Range                        = (((DataBuffer[59]) << 8) | DataBuffer[58]);
                pData.ucVFD2Pattern[0]        = (DataBuffer[60]);
                pData.ucVFD2Pattern[1]        = (DataBuffer[61]);
                pData.ucVFD2Pattern[2]        = (DataBuffer[62]);
                pData.ucVFD2Pattern[3]        = (DataBuffer[63]);
                pData.ucVFD2Pattern[4]        = (DataBuffer[64]);
                pData.ucVFD2Pattern[5]        = (DataBuffer[65]);
                pData.uiVFD2CycleCount        = (((DataBuffer[67]) << 8) | DataBuffer[66]);
                pData.uiVFD2BlockCount        = (DataBuffer[68]);

                if(TgServer_Print_Flag_DBG_01)
                {
                   PRV_UTF_LOG0_MAC("\n== pData:\n");
                   temp = (unsigned char *)(DataBuffer);

                   for(counter_aa = 0x0 ; counter_aa < 0x60 ; counter_aa++)
                   {
                                if((counter_aa & 0x0F) == 0)
                                {
                                        PRV_UTF_LOG1_MAC("0x%4.4x :", counter_aa);
                    }

                                PRV_UTF_LOG1_MAC(" %2.2x", (temp[counter_aa]));

                                if((counter_aa & 0x0F) == 0x0F)
                    {
                                        PRV_UTF_LOG0_MAC("\n");
                    }
                  }
                  PRV_UTF_LOG0_MAC("\n\n");
                }

        #elif defined(CPU_ARM)

                memcpy((GT_VOID *)&pData, DataBuffer, sizeof(ETHTransmit));

                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                {
                   PRV_UTF_LOG0_MAC("\n== pData:\n");
                   temp = (unsigned char *)(DataBuffer);

                   for(counter_aa = 0x0 ; counter_aa < 0x60 ; counter_aa++)
                   {
                                if((counter_aa & 0x0F) == 0)
                            {
                          PRV_UTF_LOG1_MAC("0x%4.4x :", counter_aa);
                    }

                                PRV_UTF_LOG1_MAC(" %2.2x", (temp[counter_aa]));

                                if((counter_aa & 0x0F) == 0x0F)
                           {
                                        PRV_UTF_LOG0_MAC("\n");
                           }
                   }
                   PRV_UTF_LOG0_MAC("\n\n");
                }

        #elif (defined(INTEL64_CPU)  || defined(INTEL_CPU ))

                pData.ucTransmitMode                = ((ETHTransmit *)DataBuffer)->ucTransmitMode;
                pData.uiDataLength                = ((ETHTransmit *)DataBuffer)->uiDataLength;
                pData.ucDuplexMode                = ((ETHTransmit *)DataBuffer)->ucDuplexMode;
                pData.ucSpeed                                = ((ETHTransmit *)DataBuffer)->ucSpeed;
                pData.uiCollisionBackoffAggressiveness = ((ETHTransmit *)DataBuffer)->uiCollisionBackoffAggressiveness;

                pData.ulBurstCount                    = (unsigned short)(((DataBuffer[11]) << 24) | ((DataBuffer[10]) << 16) |
                                                                                                    ((DataBuffer[ 9]) << 8) | DataBuffer[8]);
                pData.ulMultiBurstCount         = (unsigned short)(((DataBuffer[15]) << 24) | ((DataBuffer[14]) << 16) |
                                                                                                ((DataBuffer[13]) << 8) | DataBuffer[12]);

                pData.ulInterFrameGap        = (unsigned short)(((DataBuffer[19]) << 24) | ((DataBuffer[18]) << 16) |
                                                                                        ((DataBuffer[17]) << 8) | ((DataBuffer[16] & 0x000000ff)));

                pData.uiInterFrameGapScale        = (((DataBuffer[23]) << 24) | ((DataBuffer[22]) << 16) | ((DataBuffer[21]) << 8) | DataBuffer[20]);

                pData.ulInterBurstGap        = (unsigned short)(((DataBuffer[27]) << 24) | ((DataBuffer[26]) << 16) |
                                                                                        ((DataBuffer[25]) << 8) | ((DataBuffer[24] & 0x000000ff)));

                pData.uiInterBurstGapScale        = (((DataBuffer[29]) << 8) | DataBuffer[28]);
                pData.ucRandomBackground        = (DataBuffer[30]);
                pData.ucRandomLength                = (DataBuffer[31]);
                pData.ucCRCErrors                        = (DataBuffer[32]);
                pData.ucAlignErrors                = (DataBuffer[33]);
                pData.ucSymbolErrors                = (((DataBuffer[35]) << 8) | DataBuffer[34]);
                pData.uiDribbleBits                = (((DataBuffer[37]) << 8) | DataBuffer[36]);
                pData.ucVFD1Mode                        = (((DataBuffer[39]) << 8) | DataBuffer[38]);
                pData.uiVFD1Offset                        = (((DataBuffer[41]) << 8) | DataBuffer[40]);
                pData.iVFD1Range                        = (((DataBuffer[43]) << 8) | DataBuffer[42]);

                for(idx = 0x0; idx < 6; idx++)
                {
                        pData.ucVFD1Pattern[idx] = (DataBuffer[idx + 44]);
                }

                pData.uiVFD1CycleCount        = (((DataBuffer[51]) << 8) | DataBuffer[50]);
                pData.uiVFD1BlockCount        = (((DataBuffer[53]) << 8) | DataBuffer[52]);
                pData.ucVFD2Mode                        = (((DataBuffer[55]) << 8) | DataBuffer[54]);
                pData.uiVFD2Offset                = (((DataBuffer[57]) << 8) | DataBuffer[56]);
                pData.iVFD2Range                        = (((DataBuffer[59]) << 8) | DataBuffer[58]);

                for(idx = 0x0; idx < 6; idx++)
                {
                        pData.ucVFD2Pattern[idx] = (DataBuffer[idx + 60]);
                }

                pData.uiVFD2CycleCount        = (((DataBuffer[67]) << 8) | DataBuffer[66]);
                pData.uiVFD2BlockCount        = (DataBuffer[68]);

                #if 1
                pData.ucVFD3Mode                = (((DataBuffer[70]) << 8) | DataBuffer[69]);
                pData.uiVFD3Offset                = (((DataBuffer[72]) << 8) | DataBuffer[71]);
                pData.uiVFD3Range                = (((DataBuffer[74]) << 8) | DataBuffer[73]);
                pData.uiVFD3DataCount        = (((DataBuffer[76]) << 8) | DataBuffer[75]);
                pData.uiVFD3BlockCount        = (((DataBuffer[78]) << 8) | DataBuffer[77]);

                for(idx = 0x0 ; idx < 20 /*2044*/ ; idx++)
                {
                        pData.ucVFD3Buffer[idx] = (DataBuffer[idx + 79]);
                }
                #endif

                #if 0
                pData.ucAntiPartitioningEnable        = ((ETHTransmit *)DataBuffer)->ucAntiPartitioningEnable;
                pData.ucARPOptimizationEnable          = ((ETHTransmit *)DataBuffer)->ucARPOptimizationEnable;
                pData.ucExtendedPPPoEEnable                = ((ETHTransmit *)DataBuffer)->ucExtendedPPPoEEnable;

                for(idx = 0x0 ; idx < 6 /*60*/ ; idx++)
                {
                        pData.ucReserved[idx] = ((ETHTransmit *)DataBuffer)->ucReserved[idx];
                }
                #endif


                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                {
                        PRV_UTF_LOG0_MAC("\n== pData:\n");
                        temp = (unsigned char *)(DataBuffer);

                        for(counter_aa = 0x0 ; counter_aa < 0x60 /*length*/ ; counter_aa++)
                        {
                                if((counter_aa & 0x0F) == 0)
                            {
                              PRV_UTF_LOG1_MAC("0x%4.4x :", counter_aa);
                    }

                                PRV_UTF_LOG1_MAC(" %2.2x", (temp[counter_aa]));

                                if((counter_aa & 0x0F) == 0x0F)
                    {
                                        PRV_UTF_LOG0_MAC("\n");
                    }
                   }
                }

        #endif

        if(TgServer_Print_Flag_DBG_KA_SocHndl == GT_TRUE)  /* <--- TODO */
        {
                printf /*PRV_UTF_LOG2_MAC*/("== n = %4d, DataBuffer->ucTransmitMode               = %d. ==\n", n, pData.ucTransmitMode);
                printf /*PRV_UTF_LOG1_MAC*/("==           DataBuffer->uiDataLength         = %hd. ==\n", pData.uiDataLength);
                printf /*PRV_UTF_LOG1_MAC*/("==           DataBuffer->ucDuplexMode         = %d.  ==\n", pData.ucDuplexMode);
                printf /*PRV_UTF_LOG1_MAC*/("==           DataBuffer->ucSpeed              = %d.  ==\n", pData.ucSpeed);
                printf /*PRV_UTF_LOG1_MAC*/("==           DataBuffer->uiCollisionBackoffAggressiveness = %hd. ==\n", pData.uiCollisionBackoffAggressiveness);
                printf /*PRV_UTF_LOG1_MAC*/("==           DataBuffer->ulBurstCount         = %ld. ==\n", pData.ulBurstCount);
                printf /*PRV_UTF_LOG1_MAC*/("==           DataBuffer->ulMultiBurstCount    = %ld. ==\n", pData.ulMultiBurstCount);
                PRV_UTF_LOG1_MAC("==           DataBuffer->ulInterFrameGap      = %ld. ==\n", pData.ulInterFrameGap);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiInterFrameGapScale = %hd. ==\n", pData.uiInterFrameGapScale);
                PRV_UTF_LOG1_MAC("==           DataBuffer->ulInterBurstGap      = %ld. ==\n", pData.ulInterBurstGap);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiInterBurstGapScale = %hd. ==\n", pData.uiInterBurstGapScale);
                PRV_UTF_LOG1_MAC("==           DataBuffer->ucRandomBackground   = %d.  ==\n", pData.ucRandomBackground);
                PRV_UTF_LOG1_MAC("==           DataBuffer->ucRandomLength       = %d.  ==\n", pData.ucRandomLength);
                PRV_UTF_LOG1_MAC("==           DataBuffer->ucCRCErrors          = %d.  ==\n", pData.ucCRCErrors);
                PRV_UTF_LOG1_MAC("==           DataBuffer->ucAlignErrors        = %d.  ==\n", pData.ucAlignErrors);
                PRV_UTF_LOG1_MAC("==           DataBuffer->ucSymbolErrors       = %d.  ==\n", pData.ucSymbolErrors);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiDribbleBits        = %hd. ==\n", pData.uiDribbleBits);
                PRV_UTF_LOG1_MAC("==           DataBuffer->ucVFD1Mode           = %d.  ==\n", pData.ucVFD1Mode);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD1Offset         = %hd. ==\n", pData.uiVFD1Offset); /* TG gets adrs * 8. */
                PRV_UTF_LOG1_MAC("==           DataBuffer->iVFD1Range           = %hd. ==\n", pData.iVFD1Range);

                for(idx = 0x0; idx < 6; idx++)
                {
                        PRV_UTF_LOG2_MAC("==           DataBuffer->ucVFD1Pattern[%d]     = 0x%02x. ==\n",  idx, pData.ucVFD1Pattern[idx]);
                }

                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD1CycleCount     = %hd. ==\n", pData.uiVFD1CycleCount);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD1BlockCount     = %hd. ==\n", pData.uiVFD1BlockCount);
                PRV_UTF_LOG1_MAC("==           DataBuffer->ucVFD2Mode           = %d.  ==\n", pData.ucVFD2Mode);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD2Offset         = %hd. ==\n", pData.uiVFD2Offset); /* TG gets adrs * 8. */
                PRV_UTF_LOG1_MAC("==           DataBuffer->iVFD2Range           = %hd. ==\n", pData.iVFD2Range);

                for(idx = 0x0; idx < 6; idx++)
                {
                        PRV_UTF_LOG2_MAC("==           DataBuffer->ucVFD2Pattern[%d]     = 0x%02x. ==\n", idx, pData.ucVFD2Pattern[idx]);
                }

                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD2CycleCount     = %hd. ==\n", pData.uiVFD2CycleCount);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD2BlockCount     = %hd. ==\n", pData.uiVFD2BlockCount);

                PRV_UTF_LOG1_MAC("==           DataBuffer->ucVFD3Mode           = %d.  ==\n", pData.ucVFD3Mode);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD3Offset         = %hd. ==\n", pData.uiVFD3Offset); /* TG gets adrs * 8. */
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD3Range          = %hd. ==\n", pData.uiVFD3Range);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD3DataCount      = %hd. ==\n", pData.uiVFD3DataCount);
                PRV_UTF_LOG1_MAC("==           DataBuffer->uiVFD3BlockCount     = %hd. ==\n", pData.uiVFD3BlockCount);

                PRV_UTF_LOG0_MAC("\n==           ucVFD3Buffer[]:");
                for(idx = 0x0 ; idx <= 20 /*2044*/ ; idx++)
                {
                        PRV_UTF_LOG1_MAC(" %x", pData.ucVFD3Buffer[idx]);
                }


        }

        vrfPtr = &prvTgfVfdArray[0];

        if(pData.ucVFD1Mode != HVFD_NONE)
        {
                if(pData.ucVFD1Mode == HVFD_STATIC)
                {
                        prvTgfDaMacArr[0][0] = pData.ucVFD1Pattern[0];
                        prvTgfDaMacArr[0][1] = pData.ucVFD1Pattern[1];
                        prvTgfDaMacArr[0][2] = pData.ucVFD1Pattern[2];
                        prvTgfDaMacArr[0][3] = pData.ucVFD1Pattern[3];
                        prvTgfDaMacArr[0][4] = pData.ucVFD1Pattern[4];
                        prvTgfDaMacArr[0][5] = pData.ucVFD1Pattern[5];

                        /* set destination MAC address in Packet */
                        memcpy(prvTgfPacketL2Part.daMac, prvTgfDaMacArr[0], sizeof(prvTgfPacketL2Part.daMac));
                }
                else if(pData.ucVFD1Mode == HVFD_INCR)
                {
                        prvTgfDaMacArr[0][0] = pData.ucVFD1Pattern[0];
                        prvTgfDaMacArr[0][1] = pData.ucVFD1Pattern[1];
                        prvTgfDaMacArr[0][2] = pData.ucVFD1Pattern[2];
                        prvTgfDaMacArr[0][3] = pData.ucVFD1Pattern[3];
                        prvTgfDaMacArr[0][4] = pData.ucVFD1Pattern[4];
                        prvTgfDaMacArr[0][5] = pData.ucVFD1Pattern[5];

                        /* set the info for the increment of the mac */
                        vrfPtr->cycleCount                = sizeof(TGF_MAC_ADDR);                /* number of bytes in patternPtr == 6 */
                        vrfPtr->incValue                = pData.uiVFD1CycleCount;        /* it is the value for inc/dec                  */
                        vrfPtr->mode                        = pData.ucVFD1Mode;                        /* TGF_VFD_MODE_INCREMENT_E                          */
                        vrfPtr->modeExtraInfo        = 2;                                                /* 4 inc,dec it is the current step (0,1,2...) */
                        vrfPtr->offset                        = (pData.uiVFD1Offset / 8); /* offset in the packet to override TG need adrs * 8, UT doesn't. */

                        memcpy(vrfPtr->patternPtr, prvTgfDaMacArr[0], sizeof(TGF_MAC_ADDR));

                        numVfd_L += 1;

                        if(TgServer_Print_Flag_DBG_01 == GT_TRUE)  /* <--- TODO */
                        {
                                PRV_UTF_LOG1_MAC("\n== pData.ucVFD1Mode = HVFD_INCR, numVfd = %ld\n", numVfd_L);
                        }
                }
                else if(pData.ucVFD1Mode == HVFD_DECR)
                {
                        PRV_UTF_LOG0_MAC("\n== pData.ucVFD1Mode = HVFD_DECR\n");
                }
        }

        if(pData.ucVFD2Mode != HVFD_NONE)
        {
                if(pData.ucVFD2Mode == HVFD_STATIC)
                {
                        prvTgfSaMacArr[0][0] = pData.ucVFD2Pattern[0];
                        prvTgfSaMacArr[0][1] = pData.ucVFD2Pattern[1];
                        prvTgfSaMacArr[0][2] = pData.ucVFD2Pattern[2];
                        prvTgfSaMacArr[0][3] = pData.ucVFD2Pattern[3];
                        prvTgfSaMacArr[0][4] = pData.ucVFD2Pattern[4];
                        prvTgfSaMacArr[0][5] = pData.ucVFD2Pattern[5];

                        /* set source MAC address in Packet */
                        memcpy(prvTgfPacketL2Part.saMac, prvTgfSaMacArr[0], sizeof(prvTgfPacketL2Part.saMac));
                }
                else if(pData.ucVFD2Mode == HVFD_INCR)
                {
                        prvTgfSaMacArr[0][0] = pData.ucVFD2Pattern[0];
                        prvTgfSaMacArr[0][1] = pData.ucVFD2Pattern[1];
                        prvTgfSaMacArr[0][2] = pData.ucVFD2Pattern[2];
                        prvTgfSaMacArr[0][3] = pData.ucVFD2Pattern[3];
                        prvTgfSaMacArr[0][4] = pData.ucVFD2Pattern[4];
                        prvTgfSaMacArr[0][5] = pData.ucVFD2Pattern[5];

                        vrfPtr = &prvTgfVfdArray[1];

                        /* set the info for the increment of the mac */
                        vrfPtr->cycleCount                = sizeof(TGF_MAC_ADDR);                /* number of bytes in patternPtr == 6 */
                        vrfPtr->incValue                = 0;/*pData.uiVFD2CycleCount;*/        /* it is the value for inc/dec                  */
                        vrfPtr->mode                        = pData.ucVFD2Mode;                        /* TGF_VFD_MODE_INCREMENT_E                          */
                        vrfPtr->modeExtraInfo        = 0;                                                /* 4 inc,dec it is the current step (0,1,2...) */
                        vrfPtr->offset                        = (pData.uiVFD2Offset / 8); /* offset in the packet to override TG need adrs * 8, UT doesn't. */

                        memcpy(vrfPtr->patternPtr, prvTgfSaMacArr[0], sizeof(TGF_MAC_ADDR));

                        numVfd_L += 2;

                        if(TgServer_Print_Flag_DBG_01 == GT_TRUE)  /* <--- TODO */
                        {
                                PRV_UTF_LOG1_MAC("\n== pData.ucVFD2Mode = HVFD_INCR, numVfd = %ld\n", numVfd_L);
                        }
                }
                else if(pData.ucVFD2Mode == HVFD_DECR)
                {
                        PRV_UTF_LOG0_MAC("\n== pData.ucVFD2Mode = HVFD_DECR\n");
                }
        }

        if(pData.ucVFD3Mode != HVFD_NONE)
        {
                if(pData.ucVFD3Mode == HVFD_ENABLED)
                {
                        prvTgfSaMacArr[0][0] = ulPortTxPacketSave_One[index][(prvTgfVfdArray[vfdArrayStart_L].offset) - 5];
                        prvTgfSaMacArr[0][1] = ulPortTxPacketSave_One[index][(prvTgfVfdArray[vfdArrayStart_L].offset) - 4];
                        prvTgfSaMacArr[0][2] = ulPortTxPacketSave_One[index][(prvTgfVfdArray[vfdArrayStart_L].offset) - 3];
                        prvTgfSaMacArr[0][3] = ulPortTxPacketSave_One[index][(prvTgfVfdArray[vfdArrayStart_L].offset) - 2];
                        prvTgfSaMacArr[0][4] = ulPortTxPacketSave_One[index][(prvTgfVfdArray[vfdArrayStart_L].offset) - 1];
                        prvTgfSaMacArr[0][5] = ulPortTxPacketSave_One[index][(prvTgfVfdArray[vfdArrayStart_L].offset)];

                        vrfPtr = &prvTgfVfdArray[1];

                        /* set the info for the increment of the mac */
                        vrfPtr->cycleCount                = sizeof(TGF_MAC_ADDR);
                        vrfPtr->incValue                = pData.uiVFD3Range;        /* it is the value for inc/dec                  */
                        vrfPtr->mode                        = TGF_VFD_MODE_INCREMENT_E;
                        vrfPtr->modeExtraInfo        = 0;
                        vrfPtr->offset                        = (pData.uiVFD3Offset / 8); /* TG need adrs * 8, UT doesn't. */

                        memcpy(vrfPtr->patternPtr, prvTgfSaMacArr[0], sizeof(TGF_MAC_ADDR));

                        numVfd_L += 2;

                        if(TgServer_Print_Flag_DBG_01 == GT_TRUE)  /* <--- TODO */
                        {
                                PRV_UTF_LOG1_MAC("\n== pData.ucVFD3Mode = HVFD_ENABLED, numVfd = %ld\n", numVfd_L);
                        }
                }
        }

        if((pData.ucVFD1Mode == HVFD_NONE)&&(pData.ucVFD2Mode == HVFD_NONE)&&(pData.ucVFD3Mode == HVFD_NONE))
        {
                if(TgServer_Print_Flag_DBG_KA)
                {
                        printf("\n\n== (pData.ucVFD1Mode and pData.ucVFD2Mode and pData.ucVFD3Mode) == HVFD_NONE\n");
                }
        }

        /* PARTS of packet */
        prvTgfPacketPartArray[index][0].type                = TGF_PACKET_PART_L2_E;
        prvTgfPacketPartArray[index][0].partPtr           = &prvTgfPacketL2Part;

        prvTgfPacketInfo[index].partsArray = prvTgfPacketPartArray[index];

        switch(numVfd_L)
        {
                case 0:  vfdArrayStart_L = 0x0; break;
                case 1:  vfdArrayStart_L = 0x0; break;
                case 2:  vfdArrayStart_L = 0x1; numVfd_L = 0x01; break;
                case 3:  vfdArrayStart_L = 0x0; numVfd_L = 0x02; break;
                default: vfdArrayStart_L = 0x0; break;
        }

        numVfd[ulPortTxFlagSave.PortTxNumberSave[index]] = numVfd_L;
        vfdArrayStart[ulPortTxFlagSave.PortTxNumberSave[index]] = vfdArrayStart_L;

        if(TgServer_Print_Flag_DBG_KA_SocHndl)  /*  <---- TODO: */
        {
                PRV_UTF_LOG1_MAC("\n\n== vfdArrayStart = %ld\n", vfdArrayStart_L);
        }

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
        {
                PRV_UTF_LOG1_MAC("\n== length = %ld ==\n", prvTgfPacketInfo[index].totalLen);
                PRV_UTF_LOG1_MAC("== numOfParts = %ld\n", prvTgfPacketInfo[index].numOfParts);
        }

        if(TgServer_Print_Flag_DBG_KA)
        {
                /*PRV_UTF_LOG0_MAC("\n\n==========================================\n");*/
                printf /*PRV_UTF_LOG0_MAC*/("== Exit TGServiceParseConvertPacket_Two ==");
                /*PRV_UTF_LOG0_MAC("\n==========================================\n");*/
        }

        printf /*PRV_UTF_LOG0_MAC*/("\n");

        return (status);
}

/*******************************************************************************
* TGServiceStart
*
* DESCRIPTION:
*       Start app Demo Server
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID TgServerServiceStart(int portNumber)
{
        GT_STATUS rc  = GT_FALSE;
        char taskName[32];

        GT_BOOL multipleInstances = GT_FALSE;
        GT_BOOL isConsole         = GT_FALSE;

        TG_SERVER_PARAMS_STC *params;

        if(GT_FALSE == TgServerTaskInitFlag)
        {
                sprintf(taskName, "TgServer");

                /* Allocate memory for TG server task parameters. */
                params = (TG_SERVER_PARAMS_STC *)cmdOsMalloc(sizeof(*params));
                if (!params)
                    {
                        cmdOsPrintf("\n%s[%d] TgServerServiceStart: Failed to Malloc.\n", __FILE__, __LINE__);
                        return;
            }

            params->portNumber = portNumber;
            params->TgTaskName = taskName;
                params->multipleInstances = multipleInstances;
                params->isConsole = isConsole;

                /* Now create TG server task. */
                rc = cmdOsTaskCreate(taskName,
                                                TG_SERVER_THREAD_PRIO,        /* prio = 6 */
                                                0x8000,                         /* use default stack size (32768). */
                                                (unsigned (__TASKCONV *)(void*))TgServerTask,
                                                (GT_VOID *)params,                        /* parameters. */
                                (GT_TASK *)&TgServerTid);

                /* Free memory. */
                cmdOsFree(params);

                TgServerTaskInitFlag = GT_TRUE;
        }
        else
        {
                cmdOsPrintf("\n%s[%d] TgServerServiceStart: TG server Task already running.\n", __FILE__, __LINE__);
                return;
        }

        if (rc != GT_OK)
    {
                cmdOsPrintf("\n%s[%d] TgServerServiceStart: failed to create TG server.\n", __FILE__, __LINE__);
    }
        else{ cmdOsPrintf("TgServerServiceStart: TG server Task created and running.\n"); }

    return;
}

/******************************************************************************
* TgServerTask
*
* DESCRIPTION:
*       Server implementation - API that will start the server side for Client/
*       Server application.
*
* INPUTS:
*       TG_SERVER_PARAMS_STC* params -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:          portNumber = 16385;
*
******************************************************************************/
GT_VOID TgServerTask(TG_SERVER_PARAMS_STC* params)
{
        int socket_type;
        int rc = GT_OK, socNumber = 0x0;
        /*HANDLE        hThread;
        int nCaptureMode = 0x0;
        CPSS_INTERFACE_INFO_STC portInterface;*/

#ifdef _VXWORKS
        unsigned short portno;
        int clilen;
        int retval = 0x0;
        int optionVal = 1; /* Turn ON the different setsockopt options */
    struct sockaddr_in serv_addr, /* = local */ cli_addr /* = from */;
        GT_U32 portsCounter = 0x0;

    int        sockAddrSize;                            /* size of socket address structure */
    int        listen_socket;                           /* socket file descriptor */
    int newsockfd;                               /* socket descriptor from accept */
#else
#ifdef WIN32
    WSADATA wsaData;
    SOCKET listen_socket, newsockfd;
        struct sockaddr_in serv_addr, cli_addr;
        int retval = 0x0, portno = 0x0, clilen;
        GT_U32 portsCounter = 0x0;
        GT_U8 flag = 1;

#else /* Linux */
#ifdef  LINUX
        int retval = 0x0, flag = 1;
        int listen_socket, newsockfd;
        int portno;
        GT_U32 portsCounter = 0x0;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        char taskName[32];

        GT_BOOL multipleInstances = GT_FALSE;
        GT_BOOL isConsole         = GT_FALSE;

        TG_SERVER_PARAMS_STC         *params_sock_hendler = NULL;

        PRV_UTF_LOG0_MAC("\n\n=========================\n");
        PRV_UTF_LOG0_MAC("== Enter TgServerTask ==");
        PRV_UTF_LOG0_MAC("\n=========================\n");

        if (params->portNumber < 0)
        {
                fprintf(stderr,"ERROR, no port provided\n");
                return;
        }

        /*         The socket() function returns a socket descriptor
                representing an endpoint. The statement also identifies
                that the INET (Internet Protocol)address family with
        the TCP transport (SOCK_STREAM)will be used for this socket. */

        socket_type = SOCK_STREAM;

#ifdef _VXWORKS
    /* set up the local address */
    sockAddrSize = sizeof (struct sockaddr_in);
        memset((char *) &serv_addr,        0, sockAddrSize);
#else
#ifdef WIN32
        /* Request Winsock version 2.2 */
    if ((retval = WSAStartup(0x202, &wsaData)) != 0)
    {
        fprintf(stderr,"Server: WSAStartup() failed with error %d\n", retval);
        WSACleanup();
        return;
    }
    else {printf("Server: WSAStartup() is OK.\n");}

        /* Confirm that the WinSock DLL supports 2.2.                                */
        /* Note that if the DLL supports versions greater                    */
        /* than 2.2 in addition to 2.2, it will still return                 */
        /* 2.2 in wVersion since that is the version we requested.        */

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 )
        {
                /* Tell the user that we could not find a usable WinSock DLL. */
                printf("The dll do not support the Winsock version %u.%u!\n", LOBYTE(wsaData.wVersion),HIBYTE(wsaData.wVersion));
                WSACleanup();
                return;
        }
        else
        {
                printf("The dll supports the Winsock version %u.%u!\n", LOBYTE(wsaData.wVersion),HIBYTE(wsaData.wVersion));
                printf("The highest version this dll can support: %u.%u\n", LOBYTE(wsaData.wHighVersion), HIBYTE(wsaData.wHighVersion));
        }

        memset((char *) &serv_addr,        0, sizeof(serv_addr));

#else /* Linux */
#ifdef  LINUX
        bzero((char *) &serv_addr,        sizeof(serv_addr));
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        portno = params->portNumber;
    if (portno == 0)
    {
        fprintf(stderr,"portno == 0\n");
    }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        /* Port MUST be in Network Byte Order */
#ifdef WIN32
        serv_addr.sin_port = htons((unsigned short)portno);
#else
        serv_addr.sin_port = htons(portno);
#endif

#ifdef _VXWORKS
        serv_addr.sin_len = (u_char) sockAddrSize;
#endif


#ifdef _VXWORKS
        /* TCP socket */
        listen_socket = socket(AF_INET, socket_type, 0);
        if (listen_socket == ERROR)
        {
        perror("socket");
        return;
        }
#else
#ifdef WIN32
        /* TCP socket */
        listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listen_socket == INVALID_SOCKET)
        {
        fprintf(stderr,"Server: socket() failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }
#else /* Linux */
#ifdef  LINUX
        /* TCP socket */
        listen_socket = socket(AF_INET, socket_type, 0);
        if (listen_socket < 0)
        {
                error("ERROR opening socket");
        return;
    }
#endif /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */
    else { printf("Server: socket() is OK.\n"); }

        /* The setsockopt() function is used to allow
           the local address to be reused when the server
           is restarted before the required wait time expires. */
        /* Also Disable the Nagle algorithm (TCP No Delay). */

#ifdef  LINUX
        retval = setsockopt(listen_socket, IPPROTO_TCP,
                 TCP_NODELAY_LOCAL, (char *)&flag, sizeof(flag) );

        if (retval < 0x0)
        {
                close(listen_socket);
                printf("Couldn't setsockopt(TCP_NODELAY_LOCAL)\n");
                perror("Server-setsockopt() error");
                goto FINISH;
        }
#endif  /* Linux */

        /* After the socket descriptor is created, a bind()
           function gets a unique name for the socket.
           Here we set the s_addr to zero, which allows the system to
           connect to any client that used port 3005. */

#ifdef _VXWORKS

    /* Specify the SO_REUSEADDR option  to bind a stream socket to a local
     * port  that may be still bound to another stream socket that may be
     * hanging around with a "zombie" protocol control block whose context
     * is  not yet freed from previous sessions.
     */
    retval = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&optionVal, sizeof(optionVal));

        if (retval == -1)
        {
                printf("Couldn't setsockopt(TCP_NODELAY_LOCAL) retval = %d\n", retval);
                return;
        }

    /* Specify the TCP_NODELAY option for protocols such as X Window System
     * Protocol that require immediate delivery of many small messages.
     *
     * By default VxWorks uses congestion avoidance algorithm
     * for virtual  terminal  protocols and  bulk  data  transfer
     * protocols. When the TCP_NODELAY option is turned on and there are
     * segments to be sent out, TCP  bypasses  the  congestion
     * avoidance algorithm  and sends the segments out when there
     * is enough space in the send window.
     */

    /* Disable the Nagle algorithm (TCP No Delay). */
    retval = setsockopt (listen_socket, IPPROTO_TCP, TCP_NODELAY_LOCAL, (char *)&optionVal, sizeof(optionVal));

    if (retval == -1)
    {
        printf("Couldn't setsockopt(TCP_NODELAY_LOCAL) retval = %d\n", retval);
        return;
    }

        if (bind(listen_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == ERROR)
        {
        perror("listen");
        close (listen_socket);
        return;
        }
#else
#ifdef WIN32
    /* Disable the Nagle algorithm (TCP No Delay). */
    retval = setsockopt( listen_socket, IPPROTO_TCP, ( TCP_NODELAY_LOCAL /*| SO_REUSEADDR */), (char *)&flag, sizeof(flag) );

    if (retval == -1)
    {
        printf("Couldn't setsockopt(TCP_NODELAY_LOCAL) retval = %d\n", retval);
                WSACleanup();
        return;
    }

        if (bind(listen_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
        {
        fprintf(stderr,"Server: bind() failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }
#else /* Linux */
#ifdef  LINUX
        if (bind(listen_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        {
                error("ERROR on binding");
        return;
        }
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */
    else { printf("Server: bind() is OK.\n"); }

        clilen = sizeof(cli_addr);

        /* The listen() function allows the server to accept
           incoming client connections. Here, the backlog is set to 5.
       This means that the system can queue up to 5 connection requests before
           the system starts rejecting incoming requests. */

        if (socket_type != SOCK_DGRAM)
    {
#ifdef _VXWORKS
                if((listen(listen_socket, SERVER_MAX_CONNECTIONS)) == ERROR)
                {
                        perror("listen");
                        goto FINISH;
                }
                else { printf("Server: listen() is OK.\n"); }
#else
#ifdef WIN32
                if((listen(listen_socket, SERVER_MAX_CONNECTIONS)) == SOCKET_ERROR)
                {
                        fprintf(stderr, "Server-listen() error: %d\n", WSAGetLastError());
                        goto FINISH;
                }
#else /* Linux */
#ifdef  LINUX
                if((listen(listen_socket, SERVER_MAX_CONNECTIONS)) < 0x0)
                {
                        error("ERROR on listen");
                        goto FINISH;
                }
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */
        }
    else { printf("Server: listen() ERROR.\n"); }

        /* The server will accept a connection request
           with this accept() function, provided the connection
           request does the following:
           - Is part of the same address family
           - Uses streams sockets (TCP)
           - Attempts to connect to the specified port */

#ifdef _VXWORKS
                newsockfd = ERROR;
#else
#ifdef WIN32
                newsockfd = INVALID_SOCKET;
#else /* Linux */
#ifdef  LINUX
                newsockfd = -1;
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        /* Clear all ports burst counters. */
        /* Clear all ports Octets counters. */
        for(portsCounter = 0x0 ; portsCounter < NUM_OF_PORTS ; portsCounter++)
        {
                ulPortBurstCount [portsCounter] = 0x0;
                ulPortOctetsCount[portsCounter] = 0x0;

                ulPortExtraBurstCount[portsCounter]  = 0x0;
                ulPortExtraOctetsCount[portsCounter] = 0x0;

                ulPortShadowBurstCount [portsCounter] = 0x0;
                ulPortShadowOctetsCount[portsCounter] = 0x0;

                ulTxPortNumber = 0xFF;
                ulPortBurstCountSave[portsCounter] = 0x0;
                ulPortPacketSizeSave[portsCounter] = 0x0;

                ulPortWsBurstCount[portsCounter]   = 0x0;

                TxPortTriggerFlag[portsCounter] = GT_FALSE;
        }

        /* Reset port # used in the test. (for Tx). */
        ulPortTxFlagSave.PortTxNumberSave[0] = 0xFF;
        ulPortTxFlagSave.PortTxNumberSave[1] = 0xFF;
        ulPortTxFlagSave.PortTxNumberSave[2] = 0xFF;
        ulPortTxFlagSave.PortTxNumberSave[3] = 0xFF;
        ulPortTxFlagSave.PortTxNumberSave[4] = 0xFF;

        /* Reset port Tx Packet Size Save. */
        ulPortTxFlagSave.PortTxPacketSizeSave_One[0] = 0x00;
        ulPortTxFlagSave.PortTxPacketSizeSave_One[1] = 0x00;
        ulPortTxFlagSave.PortTxPacketSizeSave_One[2] = 0x00;
        ulPortTxFlagSave.PortTxPacketSizeSave_One[3] = 0x00;
        ulPortTxFlagSave.PortTxPacketSizeSave_One[4] = 0x00;

        ulPortTxFlagSave.PortTxPacketSizeSave_Two[0] = 0x00;
        ulPortTxFlagSave.PortTxPacketSizeSave_Two[1] = 0x00;
        ulPortTxFlagSave.PortTxPacketSizeSave_Two[2] = 0x00;
        ulPortTxFlagSave.PortTxPacketSizeSave_Two[3] = 0x00;
        ulPortTxFlagSave.PortTxPacketSizeSave_Two[4] = 0x00;

        /* Allocate memory for Triggers buffer. */
        trigPacketBuff = (GT_U8 *)cmdOsMalloc(TRIGGERS_PACKET_BUFFER_SIZE);
        if (!trigPacketBuff)
           {
                fprintf(stderr, "Error cmdOsMalloc() - Triggers buffer, %d\n", errno);
                goto FINISH;
        }

        /* Allocate memory for TG server task parameters. */
        params_sock_hendler = (TG_SERVER_PARAMS_STC *)cmdOsMalloc(sizeof(*params_sock_hendler));
        if (!params_sock_hendler)
        {
                fprintf(stderr, "Error cmdOsMalloc() %d\n", errno);
                goto FINISH;
        }

        /* In a continous loop, wait for incoming clients. Once one
           is detected, create a task and pass the handle off to it.
        */
        while(1)
        {
                printf("\nServer (%s, %s, %s)-Ready for client connection...\n",CPSS_STREAM_NAME_CNS, __DATE__, __TIME__);

                newsockfd = accept(listen_socket, (struct sockaddr *) &cli_addr, &clilen);
                printf("\nAccept connection... sockfd = %d\n", (int)newsockfd);
                socNumber++;

        #ifdef _VXWORKS
                if (newsockfd == ERROR)
                {
                        printf("\nServer-accept() FAIL.\n");
                        perror("accept");
                        break;
                }
        #else
        #ifdef WIN32
                if (newsockfd == INVALID_SOCKET)
                {
                        fprintf(stderr,"Server: accept() error %d\n", WSAGetLastError());
            WSACleanup();
                        break;
                }
        #else /* Linux */
        #ifdef  LINUX
                if (newsockfd < 0x0)
                {
                        fprintf(stderr, "Error accepting %d\n", errno);
                        break;
                }
        #endif  /* Linux */
        #endif /* WIN32 */
        #endif /* _VXWORKS */
                else{ printf("Server-accept() is OK\n"); }

                /*client IP*/
                printf("Server-newsockfd, is OK...\n");
        #if defined(INTEL64_CPU)
                printf("Got connection from the following client: %s\n", "1234");
        #else
                printf("Got connection from the following client: %s, on port #: %d\n", inet_ntoa(cli_addr.sin_addr), portno);
        #endif

                sprintf(taskName, "SocketHandler_%02d", socNumber);

                params_sock_hendler->portNumber               = portno;
                params_sock_hendler->SocketNumber             = (int)newsockfd;
                params_sock_hendler->TgTaskName               = taskName;
                params_sock_hendler->multipleInstances        = multipleInstances;
                params_sock_hendler->isConsole                = isConsole;

                /* Now create SocketHandler task. */
                rc = cmdOsTaskCreate(taskName,
                                        TG_SOCKET_HENDLER_THREAD_PRIO,  /* prio = 6 */
                                        0x8000,                             /* use default stack size (32768). */
                                        (unsigned (__TASKCONV *)(void*))SocketHandler,
                                        (GT_VOID *)params_sock_hendler, /* parameters. */
                             (GT_TASK *)&SocketHandlerTid);

                if (rc != GT_OK)
                {
                        cmdOsPrintf("\n%s[%d] TgServerServiceStart: failed to create TG server.\n", __FILE__, __LINE__);
            break;
        }


        } /*  end of while */

FINISH:

#ifdef _VXWORKS
        close(listen_socket);
#else
#ifdef WIN32
        closesocket(listen_socket);
#else /* Linux */
#ifdef  LINUX
        close(listen_socket);
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        PRV_UTF_LOG0_MAC("\n== TG Server socket Deleted successfully. ==");

        PRV_UTF_LOG0_MAC("\n\n=======================\n");
        PRV_UTF_LOG0_MAC("== Exit TgServerTask ==");
        PRV_UTF_LOG0_MAC("\n=======================\n");

        /* Free memory. */
        cmdOsFree(trigPacketBuff);

        /* Free memory. */
        cmdOsFree(params_sock_hendler);

        /* Now Delete the Socket Handler task. */
        cmdOsTaskDelete(SocketHandlerTid);

        PRV_UTF_LOG0_MAC("== Socket Handler Task and memory Deleted successfully. ==\n");

        PRV_UTF_LOG0_MAC("== TgServerTask Deleted successfully. ==\n");

        /* Now Delete TG server task. */
        cmdOsTaskDelete(TgServerTid);

        if(GT_TRUE == TgServerTaskInitFlag)
        {
                TgServerTaskInitFlag = GT_FALSE;
        }
        else {        PRV_UTF_LOG0_MAC("== TgServerTaskInitFlag Deleted successfully. ==\n");}

        return;
}



/******** SocketHandler() ***************/

/****************************************/
GT_VOID SocketHandler(TG_SERVER_PARAMS_STC* params)
{
        GT_STATUS status = GT_OK;
        int n = 0x0, loopCounter = 0x0;
        static volatile int retval_Handler = 0x0;
    unsigned int newsockfd = params->SocketNumber;
        int PortNum   = 0x0;

        char buffer[READ_BUFFER_SIZE];
        int  nLeft = (READ_BUFFER_SIZE - 156);

        /* Wait to let main task to finish printig. - sleep 100 Msec. */
        cpssOsTimerWkAfter(100);

        FD_ZERO(&read_fd);/* sets all associated flags in the socket to 0 */
#ifdef  LINUX
        FD_SET(newsockfd, &read_fd);
#else
#ifdef  _VXWORKS
        FD_SET(newsockfd, &read_fd);
#else
#ifdef  WIN32
   FD_SET(newsockfd, &read_fd);
#endif        /* WIN32*/
#endif        /* _VXWORKS */
#endif  /* LINUX */

        if(TgServer_Print_Flag)
                PRV_UTF_LOG0_MAC("\n Enter SocketHandler() Before while(1)...\n");

        /* Clear vfdArray[][] to disable Un-necessary triggers. */
        for(PortNum = 0x0; PortNum < NUM_OF_PORTS; PortNum++)
        {
                memset((GT_VOID *)(&vfdArray[PortNum][0]), 0, sizeof(TGF_VFD_INFO_STC));
                memset((GT_VOID *)(&vfdArray[PortNum][1]), 0, sizeof(TGF_VFD_INFO_STC));
        }

        /* Clear prvTgfVfdArray[] to disable Un-necessary increments. */
        for(loopCounter = 0x0; loopCounter < NUM_OF_VFD; loopCounter ++)
        {
                 memset(&prvTgfVfdArray[loopCounter], 0, sizeof(TGF_VFD_INFO_STC));
        }

        /* Clear number of VFDs in vfdArray. */
        for(loopCounter = 0x0; loopCounter < NUM_OF_TESTED_PORTS; loopCounter ++)
        {
                 memset(&numVfd[loopCounter], 0, sizeof(GT_U32));
        }

        /* Clear Array Start. */
        for(loopCounter = 0x0; loopCounter < NUM_OF_TESTED_PORTS; loopCounter ++)
        {
                 memset(&vfdArrayStart[loopCounter], 0, sizeof(GT_U32));
        }
        /* Clear numTriggers[]. */
        for(PortNum = 0x0; PortNum < NUM_OF_PORTS; PortNum++)
        {
                numTriggers[PortNum]                = 0x0;
                numTriggers_Arry01[PortNum]        = 0x0;
                TotalNumTriggers[PortNum]        = 0x0;
        }

        loopCounter = 0x0;

        /* To Enable Log print. */
        /* prvUtfLogPrintEnable(0); */

        /* disable packet trace */
        if(Trace_Packet_Print_Flag)
                prvUtfLogPrintEnable(UTF_LOG_OUTPUT_SERIAL_ALL_E);

        while(1)
        {
                /*============================================================*/

                memset(buffer, 0, sizeof(buffer));

                n = readSocketData(newsockfd, &buffer[0], nLeft);

                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        printf("\n n = %d\n", n);

                if (n < 0)
                {
                        if(TgServer_Print_Flag)
                                printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
                }
                else if (n == 0) /* Graceful close */
                {
                        printf("\n%s [Line:%d] Got 0 bytes from socket\n", __FILE__, __LINE__);
                        printf("\nClient: %d was gracefully closed\n\n", newsockfd);
                        break;
                }
        else if (n < 0)
        {
            printf("recv() failed: %d\n", n);
            break;
        }


                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        PRV_UTF_LOG1_MAC("Here is the message: %s\n",buffer);

                if (strncmp(buffer, "exit", 4) == 0)
                        retval_Handler = 99;
                else if (strncmp(buffer, "EXIT", 4) == 0)
                        retval_Handler = 99;
                else if (strncmp(buffer, "SIM_NSSetPortMappingMode_01", 27) == 0)
                        retval_Handler = 100;
                else if (strncmp(buffer, "SIM_HTSetCommand_01", 19) == 0)
                        retval_Handler = 101;
                else if (strncmp(buffer, "SIM_HTSetStructure_01", 21) == 0)
                        retval_Handler = 102;
                else if (strncmp(buffer, "SIM_HTSetStructure_02", 21) == 0)
                        retval_Handler = 103;
                else if (strncmp(buffer, "SIM_HTSetStructure_03", 21) == 0)
                        retval_Handler = 109;
                else if (strncmp(buffer, "SIM_HTSetStructure_04", 21) == 0)
                        retval_Handler = 110;
                else if (strncmp(buffer, "SIM_HTRun_01", 12) == 0)
                        retval_Handler = 104;
                else if (strncmp(buffer, "SIM_HTGetStructure_01", 21) == 0) /* Get the Counters and Rates info. */
                        retval_Handler = 105;
                else if (strncmp(buffer, "SIM_HTGetStructure_02", 21) == 0)
                        retval_Handler = 106;
                else if (strncmp(buffer, "SIM_HTGetStructure_03", 21) == 0) /* Get info about captured frames. */
                        retval_Handler = 111;
                else if (strncmp(buffer, "SIM_HTGetStructure_04", 21) == 0) /* Get captured frame data, (capture_type_info).  */
                        retval_Handler = 112;
                else if (strncmp(buffer, "SIM_HTGetStructure_05", 21) == 0) /* Get the number of captured frames, (capture_count_info). */
                        retval_Handler = 113;
                else if (strncmp(buffer, "SIM_HTStop_01", 13) == 0)
                        retval_Handler = 107;
                else if (strncmp(buffer, "SIM_HTSetCommand_02", 19) == 0)
                        retval_Handler = 108;
                else if (strncmp(buffer, "SIM_HTSetCommand_03", 19) == 0)
                        retval_Handler = 114;
                else if (strncmp(buffer, "isalive", 7) == 0)
                        retval_Handler = 115;
                else {retval_Handler = 0x0;}


                /*============================================================*/
                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        PRV_UTF_LOG1_MAC("\n retval_Handler = %d", retval_Handler);
                if(retval_Handler == 98)
                {
                        continue;
                }
                else if(retval_Handler == 99)
                {
                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(newsockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)newsockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(newsockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(newsockfd,"EXIT", 4);
                        if (n < 0)
                                error("ERROR writing to socket");
                        else {         PRV_UTF_LOG0_MAC("\n Got EXIT command..."); break; }
                }
                else if(retval_Handler == 100)
                {
                        status = SIM_NSSetPortMappingMode_01(newsockfd);  /* SIM_NSSetPortMappingMode_01 */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (100)");
                                continue;
                        }
                }
                else if(retval_Handler == 101)
                {
                        /* Reset all Ethernet port's counters */
                        status = SIM_HTSetCommand(newsockfd);  /* SIM_HTSetCommand */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (101)");
                                continue;
                        }
                        else
                        {
                                /* Check if the socket is prepared to accept data */
                                if (FD_ISSET(newsockfd, &write_fd))
                                {
                                        #ifdef  WIN32
                                                FD_CLR((SOCKET)newsockfd, (struct fd_set*)&write_fd);
                                        #else /* Linux */
                                                FD_CLR(newsockfd, &write_fd);
                                        #endif
                                }

                                n = writeSocketData(newsockfd,"SIM_HTSetCommand_OK", 19);
                                if (n < 0)
                                        error("ERROR writing to socket");
                                else {        continue; }
                        }
                }
                else if(retval_Handler == 102)
                {
                        status = SIM_HTSetStructure_01(newsockfd);  /* SIM_HTSetStructure_01*/
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (102)");
                                continue;
                        }
                }
                else if(retval_Handler == 103)
                {
                        status = SIM_HTSetStructure_02(newsockfd);  /* SIM_HTSetStructure_02*/
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (103)");
                                continue;
                        }
                }
                else if(retval_Handler == 109)
                {
                        status = SIM_HTSetStructure_03(newsockfd);  /* SIM_HTSetStructure_03*/
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (109)");
                                continue;
                        }
                }
                else if(retval_Handler == 110)
                {
                        status = SIM_HTSetStructure_04(newsockfd);  /* SIM_HTSetStructure_04*/
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (110)");
                                continue;
                        }
                }
                else if(retval_Handler == 104)
                {
                        status = SIM_HTRun(newsockfd);  /* SIM_HTRun */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (104)");
                                continue;
                        }
                }
                else if(retval_Handler == 105)
                {
                        status = SIM_HTGetStructure_01(newsockfd);  /* SIM_HTGetStructure_01 */ /* Get the Counters and Rates info. */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (105)");
                                continue;
                        }
                }
                else if(retval_Handler == 106)
                {
                        status = SIM_HTGetStructure_02(newsockfd);  /* SIM_HTGetStructure_02 */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (106)");
                                continue;
                        }
                }
                else if(retval_Handler == 111)
                {
                        status = SIM_HTGetStructure_03(newsockfd);  /* SIM_HTGetStructure_03 */ /* Get info about captured frames. */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (111)");
                                continue;
                        }
                }
                else if(retval_Handler == 112)
                {
                        status = SIM_HTGetStructure_04(newsockfd);  /* SIM_HTGetStructure_04 */ /* Get captured frame data, (capture_type_info).  */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (112)");
                                continue;
                        }
                }
                else if(retval_Handler == 113)
                {
                        status = SIM_HTGetStructure_05(newsockfd);  /* SIM_HTGetStructure_05 */ /* Get the number of captured frames, (capture_count_info). */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (113)");
                                continue;
                        }
                }
                else if(retval_Handler == 107)
                {
                        status = SIM_HTStop(newsockfd);  /* SIM_HTStop */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (107)");
                                continue;
                        }
                }
                else if(retval_Handler == 108)
                {
                        /* Reset TG port counters and set this port in linkup mode */
                        status = SIM_HTSetCommand_02(newsockfd);  /* SIM_HTSetCommand_02 */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (108)");
                                continue;
                        }
                }
                else if(retval_Handler == 114)
                {
                        /* Set forwarding mode: Mirroring / PCL */
                        status = SIM_HTSetCommand_03(newsockfd);  /* SIM_HTSetCommand_03 */
                        if(GT_OK != status)
                        {
                                PRV_UTF_LOG0_MAC("\n Bad status on command... (114)");
                                continue;
                        }
                }
                else if(retval_Handler == 115)
                {
                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(newsockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)newsockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(newsockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(newsockfd,"isalive", 7);
                        if (n < 0)
                                error("ERROR writing to socket");
                }
                else if(retval_Handler == 0x0)
                {
                        /* if(loopCounter > 10)
                        {
                                if(TgServer_Print_Flag)
                                        PRV_UTF_LOG0_MAC("\n loopCounter > 10");

                                break;
                        }
                        else{loopCounter++;} */

                        continue;
                }
                else {PRV_UTF_LOG1_MAC("\n\n== UnKnown retval_Handlerue. == %d\n", retval_Handler);}

   }/* while(1) */

#ifdef _VXWORKS
   close(newsockfd);
#else
#ifdef WIN32
        closesocket(newsockfd);
#else /* Linux */
        close(newsockfd);
#endif /* WIN32 */
#endif /* _VXWORKS */

   return;
}


/******************************************************************************
* SIM_NSSetPortMappingMode_01
*
* DESCRIPTION:
*       InitSystem -
*
* INPUTS:
*       int sockfd -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_NSSetPortMappingMode_01(int sockfd)
{
        GT_STATUS status = GT_OK;
        char buffer[READ_BUFFER_SIZE];
        int n = 0x0;

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n=======================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_NSSetPortMappingMode_01 ==");
                PRV_UTF_LOG0_MAC("\n=======================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"SIM_NSSetPortMappingMode_OK");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n======================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_NSSetPortMappingMode_01 ==");
                PRV_UTF_LOG0_MAC("\n======================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTSetCommand
*
* DESCRIPTION:
*       Run SetCommand - Reset all Ethernet port's counters.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTSetCommand(int sockfd)
{
        GT_STATUS status = GT_OK;

#ifdef _VXWORKS
        char prvTgfDevNum = 0x0;
#else
#ifdef WIN32
        char prvTgfDevNum = 0x0;
#else /* Linux */
#ifdef  LINUX
        int prvTgfDevNum = 0x0;
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

#if 0
        char buffer[READ_BUFFER_SIZE];
        int n = 0x0;
#endif

        GT_U32 portsCounter = 0x0;

        if(TgServer_Print_Flag_DBG_KA)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTSetCommand ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n====================================\n");
        }

#if 0  /*  <-----  */

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Ready_4_Parameters");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        nLeft = 100;
        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        n = readSocketData(sock, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi(buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading prvTgfDevNum");
                else if(prvTgfDevNum > 2)
                        error("ERROR prvTgfDevNum > 2");

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

#endif  /*  <-----  */


        /* Clear all ports burst counters.       */
        /* Clear all ports Octets counters.      */
        /* Clear all ports packet size counters. */
        for(portsCounter = 0x0 ; portsCounter < NUM_OF_PORTS ; portsCounter++)
        {
                ulPortBurstCount [portsCounter] = 0x0;
                ulPortOctetsCount[portsCounter] = 0x0;

                ulPortExtraBurstCount[portsCounter]  = 0x0;
                ulPortExtraOctetsCount[portsCounter] = 0x0;

                ulPortShadowBurstCount [portsCounter] = 0x0;
                ulPortShadowOctetsCount[portsCounter] = 0x0;

                ulTxPortNumber = 0xFF;
                ulPortBurstCountSave[portsCounter] = 0x0;
                ulPortPacketSizeSave[portsCounter] = 0x0;

                ulPortWsBurstCount[portsCounter]   = 0x0;

                TxPortTriggerFlag[portsCounter] = GT_FALSE;
        }

        /* Clear numTriggers[]. */
        for(portsCounter = 0x0; portsCounter < NUM_OF_PORTS; portsCounter++)
        {
                numTriggers[portsCounter]                        = 0x0;
                numTriggers_Arry01[portsCounter]        = 0x0;
                TotalNumTriggers[portsCounter]                = 0x0;
        }

        status = prvTgfEthCountersReset(prvTgfDevNum);


        if(TgServer_Print_Flag_DBG_KA)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTSetCommand ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTSetCommand_02
*
* DESCRIPTION:
*       Run SetCommand - Reset TG port counters and set this port in linkup mode.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTSetCommand_02(int sockfd)
{
        GT_STATUS status = GT_OK;
#ifdef  _VXWORKS
        int prvTgfDevNum = 0x0;
        int m_Hub = 0x0, m_Slot = 0x0, m_Port = 0x0, m_Port_01 = 0x0, m_Port_02 = 0x0;
#else /* Linux */
        int prvTgfDevNum = 0x0;
        GT_PHYSICAL_PORT_NUM m_Hub = 0x0, m_Slot = 0x0, m_Port = 0x0, m_Port_01 = 0x0, m_Port_02 = 0x0;
#endif
        char buffer[READ_BUFFER_SIZE];
           int  nBytes = READ_BUFFER_SIZE;
        int  nLeft, n = 0x0;
        GT_BOOL clearOnRead = GT_TRUE;

        if(TgServer_Print_Flag_DBG_KA)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTSetCommand_02 ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n====================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,20);
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi(buffer[7]);
                if(prvTgfDevNum < 0)
                        error("ERROR reading prvTgfDevNum");
                else if(prvTgfDevNum > 2)
                        error("ERROR prvTgfDevNum > 2");

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,16  /*sizeof(buffer)*/);
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "hub_number", 10) == 0)
        {
                m_Hub = ctoi(buffer[11]);
                if(m_Hub < 0x0)
                        error("ERROR reading m_Hub");

                m_Slot = ctoi(buffer[13]);
                if(m_Slot < 0x0)
                        error("ERROR reading m_Slot");

                m_Port    = ctoi(buffer[17]);
                m_Port_01 = ctoi(buffer[16]);
                m_Port_02 = ctoi(buffer[15]);

                if(m_Port < 0x0)
                        error("ERROR reading m_Port");

                if(m_Port_01 < 0x0)
                        error("ERROR reading m_Port");

                if(m_Port_02 < 0x0)
                        error("ERROR reading m_Port");

                m_Port = ((m_Port_02 * 100) + (m_Port_01 * 10) + m_Port);

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,16 /*sizeof(buffer)*/);
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        /*         Enable / Disable clear on read for MAC counters:
                GT_TRUE - Counters are cleared (After read). */

#ifdef EXMXPM_FAMILY
        status = cpssExMxPmPortMacCountersClearOnReadEnableSet(prvTgfDevNum, m_Port, clearOnRead);
#else
#ifdef CHX_FAMILY
        status = cpssDxChPortMacCountersClearOnReadSet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)m_Port, (GT_BOOL )clearOnRead);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

        if(GT_OK != status)
                printf("\n== cpssDxChPortMacCountersClearOnReadSet - port #%d FAIL ==\n", m_Port);
        else
        {
                if(TgServer_Print_Flag_DBG_KA)
                {
                        printf /*PRV_UTF_LOG0_MAC*/("\n== cpssDxChPortMacCountersClearOnReadSet - to --> %s, port #%d  ==\n",
                                                                                                                                (clearOnRead ? "TRUE" : "FALSE"),m_Port);
                }
        }

        /* Clear MAC counters. */
        status = prvTgfResetCountersEth((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)m_Port);

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        if(GT_OK == status)
        {
                if(TgServer_Print_Flag_DBG_03 == GT_TRUE)
                {
                        PRV_UTF_LOG1_MAC("\n== prvTgfResetCountersEth - port #%d OK ==\n", m_Port);
                }

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetCommand_02_OK");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,22 /*sizeof(buffer)*/);
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetCommand_02_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,24 /*sizeof(buffer)*/);
                if (n < 0)
                        error("ERROR writing to socket");

                PRV_UTF_LOG0_MAC("== SIM_HTSetCommand_02_FAIL ==");
        }

        /*         Disable clear on read for MAC counters:
                GT_FALSE - Counters are not cleared. (After read). */

        clearOnRead = GT_FALSE;

#ifdef EXMXPM_FAMILY
        status = cpssExMxPmPortMacCountersClearOnReadEnableSet(prvTgfDevNum, m_Port, clearOnRead);
#else
#ifdef CHX_FAMILY
        status = cpssDxChPortMacCountersClearOnReadSet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)m_Port, (GT_BOOL )clearOnRead);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

        if(GT_OK != status)
                printf("\n== cpssDxChPortMacCountersClearOnReadSet - port #%d FAIL ==\n", m_Port);
        else
        {
                if(TgServer_Print_Flag_DBG_KA)
                {
                        printf /*PRV_UTF_LOG0_MAC*/("\n== cpssDxChPortMacCountersClearOnReadSet - to --> %s, port #%d  ==\n",
                                                                                                                                (clearOnRead ? "TRUE" : "FALSE"),m_Port);
                }
        }

        /* Clear port burst counters.                           */
        /* Clear port Octets counters.                          */
        /* Clear all ports packet size counters.         */

        ulPortBurstCount [m_Port] = 0x0;
        ulPortOctetsCount[m_Port] = 0x0;

        ulPortExtraBurstCount[m_Port]  = 0x0;
        ulPortExtraOctetsCount[m_Port] = 0x0;

        ulTxPortNumber = 0xFF;
        ulPortBurstCountSave[m_Port] = 0x0;
        ulPortPacketSizeSave[m_Port] = 0x0;

        ulPortWsBurstCount[m_Port] = 0x0;

        if(TgServer_Print_Flag_DBG_KA)
                printf /*PRV_UTF_LOG1_MAC*/("== prvTgfResetCountersEth - Clear port #%d burst counters. ==\n", m_Port);

        /* Clear numTriggers[]. */
        numTriggers[m_Port]                        = 0x0;
        numTriggers_Arry01[m_Port]        = 0x0;
        TotalNumTriggers[m_Port]        = 0x0;

        /* clear Rx Packets table */
        status = tgfTrafficTableRxPcktTblClear();
        if(status != GT_OK)
                printf("\n= FAIL on Error in tgfTrafficTableRxPcktTblClear. !");
        else if(TgServer_Print_Flag_DBG_02)
                printf("== tgfTrafficTableRxPcktTblClear - OK ==\n");

        if(TgServer_Print_Flag_DBG_KA)
                printf /*PRV_UTF_LOG1_MAC*/("== prvTgfResetCountersEth - Clear port #%d numTriggers[]. ==\n", m_Port);

        if(TgServer_Print_Flag_DBG_KA)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTSetCommand_02 ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTSetCommand_03
*
* DESCRIPTION:
*       Run SetCommand - Set forwarding mode: Mirroring / PCL.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTSetCommand_03(int sockfd)
{
        GT_STATUS status = GT_OK;
        if(TgServer_Print_Flag)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTSetCommand_03 ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n====================================\n");
        }
        if(TgServer_Print_Flag)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTSetCommand_03 ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n===================================\n");
        }

        return (status);
}


/******************************************************************************
* SIM_HTSetStructure_01
*
* DESCRIPTION:
*       Run HTSetStructure- Fill data to packet.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTSetStructure_01(int sockfd)
{
        GT_STATUS status = GT_OK;
        int n = 0x0, TxPortNum = 0x0;
        char buffer[READ_BUFFER_SIZE]; /* 256 */
           int  nBytes = READ_BUFFER_SIZE;
        int  nLeft, idx, check = 0x0;
        int counter_aa;
        int length = 0x0, length_01 = 0x0, length_02 = 0x0, length_03 = 0x0, length_04 = 0x0;

        char  DataBuffer[JAMBO_PACKET_SIZE];

#ifdef  _VXWORKS
        int prvTgfDevNum = 0x0;
#else /* Linux */
        int prvTgfDevNum = 0x0;
#endif

        /* LENGTH of packet headers included */
        PACKET_HEADERS_LEN = 0x0;

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTSetStructure_01 ==");
                PRV_UTF_LOG0_MAC("\n====================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi(buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading prvTgfDevNum");
                else if(prvTgfDevNum > 2)
                        error("ERROR prvTgfDevNum > 2");

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "Packet_background_pattern", 25) == 0)
        {
                length = ctoi(buffer[30]);
                if(length < 0x0)
                        error("ERROR reading length");

                length_01 = ctoi(buffer[29]);
                if(length_01 < 0x0)
                        error("ERROR reading length");

                length_02 = ctoi(buffer[28]);
                if(length_02 < 0x0)
                        error("ERROR reading length");

                length_03 = ctoi(buffer[27]);
                if(length_03 < 0x0)
                        error("ERROR reading length");

                length_04 = ctoi(buffer[26]);
                if(length_04 < 0x0)
                        error("ERROR reading length");

                length = (length_04 * 10000) + (length_03 * 1000) + (length_02 * 100) + (length_01 * 10) + length;
                if(length == 0x0)
                        error("ERROR reading length");

                if(length > JAMBO_PACKET_SIZE)
                {
                        if(TgServer_Print_Flag_DBG_KA)
                                PRV_UTF_LOG1_MAC("\n== SIM_HTSetStructure_01 - ERROR length excced JAMBO_PACKET_SIZE (10240 Bytes). %ld==\n", length);
                        error("ERROR length excced JAMBO_PACKET_SIZE");
                        return(GT_FAIL);
                }

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(DataBuffer, 0, sizeof(DataBuffer));
        /* memset(ulPortTxPacketSave_One, 0, sizeof(ulPortTxPacketSave_One)); */ /* <--- TODO */

        nLeft = length;
        idx = 0;

        while (nLeft > 0)
           {
                n = readSocketData(sockfd, &DataBuffer[idx], nLeft);

              if (n < 0) { printf /*PRV_UTF_LOG2_MAC*/("\n%s [line #:%d]ERROR reading from socket", __FILE__, __LINE__); }
                        else if (n == 0) {break;}

              idx += n;
            nLeft -= n;
           }

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        /* Set port # used in the test. (for Tx) */
        if(0xFF == ulPortTxFlagSave.PortTxNumberSave[0])
        {
                ulPortTxFlagSave.PortTxNumberSave[0]                 = 0xBD; /* Indicate this port is set for phase I. */
                ulPortTxFlagSave.PortTxPacketSizeSave_One[0] = length;
                TxPortNum = 0;                        /* SetTx flag on port, to define it's location in the PortTxPacketSave struct. */
        }
        else if(0xFF == ulPortTxFlagSave.PortTxNumberSave[1])
        {
                ulPortTxFlagSave.PortTxNumberSave[1]                  = 0xBD; /* Indicate this port is set for phase I. */
                ulPortTxFlagSave.PortTxPacketSizeSave_One[1] = length;
                TxPortNum = 1;
        }
        else if(0xFF == ulPortTxFlagSave.PortTxNumberSave[2])
        {
                ulPortTxFlagSave.PortTxNumberSave[2]                  = 0xBD; /* Indicate this port is set for phase I. */
                ulPortTxFlagSave.PortTxPacketSizeSave_One[2] = length;
                TxPortNum = 2;
        }
        else if(0xFF == ulPortTxFlagSave.PortTxNumberSave[3])
        {
                ulPortTxFlagSave.PortTxNumberSave[3]                  = 0xBD; /* Indicate this port is set for phase I. */
                ulPortTxFlagSave.PortTxPacketSizeSave_One[3] = length;
                TxPortNum = 3;
        }
        else if(0xFF == ulPortTxFlagSave.PortTxNumberSave[4])
        {
                ulPortTxFlagSave.PortTxNumberSave[4]                  = 0xBD; /* Indicate this port is set for phase I. */
                ulPortTxFlagSave.PortTxPacketSizeSave_One[4] = length;
                TxPortNum = 4;
        }
        else
        {
                if(TgServer_Print_Flag_DBG_KA)
                        PRV_UTF_LOG0_MAC("\n== SIM_HTSetStructure_01 - Set port # used in the test. (for Tx) FAIL. ==\n");

                return(GT_FAIL);
        }

        /* Copy first part of packet to port Tx packets save buffer One. (For multiple Tx after a single SetTxSetup). */
        memcpy(&ulPortTxPacketSave_One[TxPortNum][0], DataBuffer, length);

        if(TgServer_Print_Flag_DBG_04)
        {
                PRV_UTF_LOG1_MAC("\n\n== SIM_HTSetStructure_01, After read_03(). length = %d ==\n", length);

                PRV_UTF_LOG1_MAC("\n== ulPortTxPacketSave_One: (index = %d)\n", TxPortNum);
                for(counter_aa = 0x0 ; counter_aa < length ; counter_aa++)
                {
                        if((counter_aa & 0x0F) == 0)
                   {
                    PRV_UTF_LOG1_MAC("0x%4.4x :", counter_aa);
            }

                        PRV_UTF_LOG1_MAC(" %2.2x", (GT_U8)(ulPortTxPacketSave_One[TxPortNum][counter_aa]));

                        if((counter_aa & 0x0F) == 0x0F)
            {
                                PRV_UTF_LOG0_MAC("\n");
            }
                }
                PRV_UTF_LOG0_MAC("\n");
        }

        if (n >= 0)
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_03");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else
        {
                printf /*PRV_UTF_LOG0_MAC*/("== SIM_HTSetStructure_01 - (n != 0) ==");
                return(GT_FAIL);
        }

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
        {
                printf("\n%s [Line:%d] Got bad reply from socket (!= OK_Continue)\n", __FILE__, __LINE__);
                status = GT_FALSE;
        }

        if(GT_OK == status)
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetStructure_01_OK");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetStructure_01_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");

                PRV_UTF_LOG0_MAC("== SIM_HTSetStructure_01 - return SIM_HTSetStructure_01_FAIL. ==\n");
        }

        if(status == GT_OK)
                check = 0x1;

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n===================================\n");
                printf /*PRV_UTF_LOG1_MAC*/("== Exit SIM_HTSetStructure_01 == (status = %s)", (check ? "OK" : "FAIL"));
                PRV_UTF_LOG0_MAC("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTSetStructure_02
*
* DESCRIPTION:
*       Run HTSetStructure- Fill data to packet.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTSetStructure_02(int sockfd)
{
        GT_STATUS status = GT_OK;
        int n = 0x0;
        int prvTgfPortNum = 0x0, TxPortNum = 0x0;
        char buffer[READ_BUFFER_SIZE];
        char DataBuffer[2560];
           int  nBytes = READ_BUFFER_SIZE, idx = 0;
        int  nLeft, counter_aa = 0x0;
        int  length    = 0x0, length_01 = 0x0;
        int  length_02 = 0x0, length_03 = 0x0, length_04 = 0x0;

        static volatile GT_STATUS saveStatus = 0x0;

#ifdef  _VXWORKS
        int prvTgfDevNum = 0x0;
#else /* Linux */
        int prvTgfDevNum = 0x0;
#endif

        if(TgServer_Print_Flag_DBG_KA == GT_TRUE)
        {
                PRV_UTF_LOG0_MAC("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTSetStructure_02 ==");
                PRV_UTF_LOG0_MAC("\n====================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi(buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading prvTgfDevNum ");
                else if(prvTgfDevNum > 2)
                        error("ERROR prvTgfDevNum > 2");

                prvTgfPortNum = (ctoi(buffer[9]) * 100) + (ctoi(buffer[10]) * 10) + ctoi(buffer[11]);

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "struct_data", 11) == 0)
        {
                length = ctoi(buffer[16]);
                if(length < 0x0)
                        error("ERROR reading length");

                length_01 = ctoi(buffer[15]);
                if(length_01 < 0x0)
                        error("ERROR reading length_01");

                length_02 = ctoi(buffer[14]);
                if(length_02 < 0x0)
                        error("ERROR reading length_02");

                length_03 = ctoi(buffer[13]);
                if(length_03 < 0x0)
                        error("ERROR reading length_03");

                length_04 = ctoi(buffer[12]);
                if(length_04 < 0x0)
                        error("ERROR reading length_03");

                length = (length_04 * 10000) + (length_03 * 1000) + (length_02 * 100) + (length_01 * 10) + length;
                if(length == 0x0)
                        error("ERROR reading length");

                if(length > JAMBO_PACKET_SIZE)
                {
                        if(TgServer_Print_Flag_DBG_KA)
                                PRV_UTF_LOG1_MAC("\n== SIM_HTSetStructure_01 - ERROR length excced JAMBO_PACKET_SIZE (10240 Bytes). %ld==\n", length);
                        error("ERROR length excced JAMBO_PACKET_SIZE");
                        return(GT_FAIL);
                }

            /* Wait to let main task to finish printig. - sleep 100 Msec. */
                /*cpssOsTimerWkAfter(20);*/
                if(TgServer_Print_Flag_DBG_KA)
                        printf /*PRV_UTF_LOG1_MAC*/("\n== length = %d. ==\n", length);

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(DataBuffer, 0, sizeof(DataBuffer));

        nLeft = length;
        idx = 0;

        while (nLeft > 0)
           {
                n = readSocketData(sockfd, &DataBuffer[idx], nLeft);

              if (n < 0) { printf /*PRV_UTF_LOG2_MAC*/("\n%s [line #:%d]ERROR reading from socket", __FILE__, __LINE__); }
                        else if (n == 0) {break;}

              idx += n;
            nLeft -= n;
           }

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n DataBuffer[n] = %d\n", idx);

        /* Set port # used in the test. (for Tx). */
        if(0xBD == ulPortTxFlagSave.PortTxNumberSave[0])
        {
                ulPortTxFlagSave.PortTxNumberSave[0]                  = prvTgfPortNum;
                ulPortTxFlagSave.PortTxPacketSizeSave_Two[0] = length;
                TxPortNum = 0;                /* SetTx flag on port, to define it's location in the PortTxPacketSave struct. */

                /* Now check if have already a set buffer. */
                if(ulPortTxFlagSave.PortTxNumberSave[1] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[1] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[2] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[2] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[3] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[3] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[4] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[4] = 0xFF;       /* Set the buffer as reusable. */
        }
        else if(0xBD == ulPortTxFlagSave.PortTxNumberSave[1])
        {
                ulPortTxFlagSave.PortTxNumberSave[1]                  = prvTgfPortNum;
                ulPortTxFlagSave.PortTxPacketSizeSave_Two[1] = length;
                TxPortNum = 1;

                /* Now check if have already a set buffer. */
                if(ulPortTxFlagSave.PortTxNumberSave[0] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[0] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[2] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[2] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[3] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[3] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[4] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[4] = 0xFF;       /* Set the buffer as reusable. */
        }
        else if(0xBD == ulPortTxFlagSave.PortTxNumberSave[2])
        {
                ulPortTxFlagSave.PortTxNumberSave[2]                  = prvTgfPortNum;
                ulPortTxFlagSave.PortTxPacketSizeSave_Two[2] = length;
                TxPortNum = 2;

                /* Now check if have already a set buffer. */
                if(ulPortTxFlagSave.PortTxNumberSave[0] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[0] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[1] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[1] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[3] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[3] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[4] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[4] = 0xFF;       /* Set the buffer as reusable. */
        }
        else if(0xBD == ulPortTxFlagSave.PortTxNumberSave[3])
        {
                ulPortTxFlagSave.PortTxNumberSave[3]                  = prvTgfPortNum;
                ulPortTxFlagSave.PortTxPacketSizeSave_Two[3] = length;
                TxPortNum = 3;

                /* Now check if have already a set buffer. */
                if(ulPortTxFlagSave.PortTxNumberSave[0] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[0] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[1] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[1] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[2] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[2] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[4] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[4] = 0xFF;       /* Set the buffer as reusable. */
        }
        else if(0xBD == ulPortTxFlagSave.PortTxNumberSave[4])
        {
                ulPortTxFlagSave.PortTxNumberSave[4]                  = prvTgfPortNum;
                ulPortTxFlagSave.PortTxPacketSizeSave_Two[4] = length;
                TxPortNum = 4;

                /* Now check if have already a set buffer. */
                if(ulPortTxFlagSave.PortTxNumberSave[0] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[0] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[1] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[1] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[2] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[2] = 0xFF;       /* Set the buffer as reusable. */
                else if(ulPortTxFlagSave.PortTxNumberSave[3] == prvTgfPortNum)
                        ulPortTxFlagSave.PortTxNumberSave[3] = 0xFF;       /* Set the buffer as reusable. */
        }
        else
        {
                if(TgServer_Print_Flag_DBG_KA)
                        PRV_UTF_LOG0_MAC("\n== SIM_HTSetStructure_02 - Set port # used in the test. (for Tx) FAIL. ==\n");
        }

        if (idx > 0)
        {
                /* Copy second part of packet to port Tx packets save buffer Two. (For multiple Tx after a single SetTxSetup). */
                memcpy(&ulPortTxPacketSave_Two[TxPortNum][0], DataBuffer, length);

                if(TgServer_Print_Flag_DBG_KA)
                        PRV_UTF_LOG0_MAC("\n== SIM_HTSetStructure_02 - Copy second part of packet to port Tx packets save. ==\n");

                if(TgServer_Print_Flag_DBG_KA)
                {
                   printf /*PRV_UTF_LOG1_MAC*/("\n\n== SIM_HTSetStructure_02, length = %d ==\n", length);

                   PRV_UTF_LOG1_MAC("\n== ulPortTxPacketSave_Two: (port = %d)\n", prvTgfPortNum);
                   for(counter_aa = 0x0 ; counter_aa < 0x60 /*length*/ ; counter_aa++)
                   {
                                if((counter_aa & 0x0F) == 0)
                            {
                                        printf /*PRV_UTF_LOG1_MAC*/("0x%4.4x :", counter_aa);
                                }

                                printf /*PRV_UTF_LOG1_MAC*/(" %2.2x", (GT_U8)(ulPortTxPacketSave_Two[TxPortNum][counter_aa]));

                                if((counter_aa & 0x0F) == 0x0F)
                                {
                                        printf /*PRV_UTF_LOG0_MAC*/("\n");
                                }
                   }
                   printf /*PRV_UTF_LOG0_MAC*/("\n");
                }
        }

        if (idx > 0)
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_03");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else
        {
                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n%s [Line:%d] ERROR writing to socket\n", __FILE__, __LINE__);
                return(GT_FAIL);
        }

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        if(GT_OK == status)
        {
                if(TgServer_Print_Flag_DBG_KA)
                {
                        PRV_UTF_LOG0_MAC("\n== SIM_HTSetStructure_02 - return SIM_HTSetStructure_02_OK ==");
                        PRV_UTF_LOG1_MAC("\n== SIM_HTSetStructure_02 - status = %d ==", status);
                }

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetStructure_02_OK");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");

                saveStatus = status;
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetStructure_02_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");

                printf /*PRV_UTF_LOG0_MAC*/("\n== SIM_HTSetStructure_02 - return SIM_HTSetStructure_02_FAIL ==");
        }

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG2_MAC("\n== SIM_HTSetStructure_02 - status = %d, saveStatus = %d ==", status, saveStatus);

                PRV_UTF_LOG0_MAC("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTSetStructure_02 ==");
                PRV_UTF_LOG0_MAC("\n===================================\n");
        }

        if(GT_OK == saveStatus)
                return (GT_OK);
        else{return (GT_FAIL);}
}

/******************************************************************************
* SIM_HTSetStructure_03
*
* DESCRIPTION:
*       Run HTSetStructure- Set Triggers.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTSetStructure_03(int sockfd)
{
        GT_STATUS status = GT_OK;
        int prvTgfDevNum = 0x0, n = 0x0;
        int prvTgfPortNum = 0x0;
        ETHTrigger *pData_Local;
        char buffer[READ_BUFFER_SIZE];
        char DataBuffer[2560];
           int  nBytes = READ_BUFFER_SIZE, idx = 0;
        int  nLeft, nCaptureMode = 0x0;
        int length = 0x0, length_01 = 0x0;
        int counter_aa = 0x0;

        CPSS_INTERFACE_INFO_STC portInterface;

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTSetStructure_03 ==");
                PRV_UTF_LOG0_MAC("\n====================================\n");
        }
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi(buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading prvTgfDevNum ");
                else if(prvTgfDevNum > 2)
                        error("ERROR prvTgfDevNum > 2");

                prvTgfPortNum = (ctoi(buffer[9]) * 100) + (ctoi(buffer[10]) * 10) + ctoi(buffer[11]);

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "struct_data", 11) == 0)
        {
                length = ctoi(buffer[13]);
                if(length < 0x0)
                        error("ERROR reading length_02");

                length_01 = ctoi(buffer[12]);
                if(length_01 < 0x0)
                        error("ERROR reading length_03");

                length = (length_01 * 10) + length;
                if(length == 0x0)
                        error("ERROR reading length");

                if(length > JAMBO_PACKET_SIZE)
                {
                        PRV_UTF_LOG0_MAC("\n== SIM_HTSetStructure_03 - ERROR length excced JAMBO_PACKET_SIZE (10240 Bytes). ==\n");
                        error("ERROR length excced JAMBO_PACKET_SIZE");
                        return(GT_FAIL);
                }

                if(TgServer_Print_Flag_DBG_KA == GT_TRUE)  /* <--- TODO: */
                {
                        printf /*PRV_UTF_LOG2_MAC*/("\n== Triggers struct length = %d, Port #: %d ==\n", length, prvTgfPortNum);
                }

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(DataBuffer, 0, sizeof(DataBuffer));

        nLeft = (nBytes);

        n = readSocketData(sockfd, &DataBuffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n DataBuffer[n] = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (n > 0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                {
                        PRV_UTF_LOG0_MAC("\n== TriggersDataBuffer:\n");
                        for(counter_aa = 0x0 ; counter_aa < length ; counter_aa++)
                        {
                    if((counter_aa & 0x0F) == 0)
                            {
                              PRV_UTF_LOG1_MAC("0x%4.4x :", counter_aa);
                    }

                                PRV_UTF_LOG1_MAC(" %2.2x", DataBuffer[counter_aa]);

                                if((counter_aa & 0x0F) == 0x0F)
                    {
                                        PRV_UTF_LOG0_MAC("\n");
                    }
                        }
                        PRV_UTF_LOG0_MAC("\n");
                }

                pData_Local = (ETHTrigger *)DataBuffer;

                if(TgServer_Print_Flag_DBG_KA)
                {
                        PRV_UTF_LOG2_MAC("== n = %4d, DataBuffer->ucTriggerMode                 = %d. ==\n", n, pData_Local->ucTriggerMode);
                        PRV_UTF_LOG1_MAC("==           DataBuffer->uiTrigger1Offset             = %hd. ==\n", pData_Local->uiTrigger1Offset);
                        PRV_UTF_LOG1_MAC("==           DataBuffer->uiTrigger1Range              = %d.  ==\n", pData_Local->uiTrigger1Range);

                        for(idx = 0x0; idx < 6; idx++)
                        {
                                PRV_UTF_LOG2_MAC("==           DataBuffer->ucTrigger1Pattern[%d]        = 0x%02x. ==\n",  idx, pData_Local->ucTrigger1Pattern[idx]);
                        }

                        PRV_UTF_LOG1_MAC("==           DataBuffer->uiTrigger2Offset             = %d.  ==\n", pData_Local->uiTrigger2Offset);
                        PRV_UTF_LOG1_MAC("==           DataBuffer->uiTrigger2Range              = %hd. ==\n", pData_Local->uiTrigger2Range);

                        for(idx = 0x0; idx < 6; idx++)
                        {
                                PRV_UTF_LOG2_MAC("==           DataBuffer->ucTrigger2Pattern[%d]        = 0x%02x. ==\n", idx, pData_Local->ucTrigger2Pattern[idx]);
                        }
                }

                if(0x9 == pData_Local->ucTriggerMode)
                        pData_Local->ucTriggerMode = ETH_TRIGGERS_OFF;

            /* check captured packet. */
                if(pData_Local->ucTriggerMode == ETH_TRIGGERS_OFF)
                {
                        /* Do nothing. */
                        memset((GT_VOID *)(&vfdArray[prvTgfPortNum][0]), 0, sizeof(TGF_VFD_INFO_STC));
                        memset((GT_VOID *)(&vfdArray[prvTgfPortNum][1]), 0, sizeof(TGF_VFD_INFO_STC));

                        if(TgServer_Print_Flag_DBG_KA == GT_TRUE)
                        {
                                PRV_UTF_LOG0_MAC("\n== TriggerMode = ETH_TRIGGERS_OFF. ==\n");
                        }
                }
                else if(pData_Local->ucTriggerMode == ETH_TRIGGER1_ONLY)
                {
                        /* clear VFD array */
                        memset((GT_VOID *)(&vfdArray[prvTgfPortNum][0]), 0, sizeof(TGF_VFD_INFO_STC));
                        memset((GT_VOID *)(&vfdArray[prvTgfPortNum][1]), 0, sizeof(TGF_VFD_INFO_STC));

                        vfdArray[prvTgfPortNum][0].mode                         = TGF_VFD_MODE_STATIC_E;
                        vfdArray[prvTgfPortNum][0].modeExtraInfo         = 0x0;
                        vfdArray[prvTgfPortNum][0].offset                         = (pData_Local->uiTrigger1Offset / 8); /* TG need adrs * 8, UT doesn't. */
                        vfdArray[prvTgfPortNum][0].cycleCount                 = pData_Local->uiTrigger1Range;

                        memcpy((GT_VOID *)(vfdArray[prvTgfPortNum][0].patternPtr), &pData_Local->ucTrigger1Pattern[0], pData_Local->uiTrigger1Range);

                        vfdArray[prvTgfPortNum][1].mode                         = TGF_VFD_MODE_OFF_E;
                        vfdArray[prvTgfPortNum][1].modeExtraInfo         = 0x0;
                        vfdArray[prvTgfPortNum][1].offset                         = 0x0;
                        vfdArray[prvTgfPortNum][1].cycleCount                 = 1;

                        pData_Local->ucTrigger2Pattern[0] = 0x81;
                        memcpy((GT_VOID *)(vfdArray[prvTgfPortNum][1].patternPtr), &pData_Local->ucTrigger2Pattern[0], pData_Local->uiTrigger2Range);

                        if(TgServer_Print_Flag_DBG_KA == GT_TRUE)  /* <--- TODO: */
                        {
                                PRV_UTF_LOG0_MAC("\n== TriggerMode = ETH_TRIGGER1_ONLY. ==\n");
                        }
                }
                else if(pData_Local->ucTriggerMode == ETH_TRIGGER2_ONLY)
                {
                        /* clear VFD array */
                        memset((GT_VOID *)(&vfdArray[prvTgfPortNum][0]), 0, sizeof(TGF_VFD_INFO_STC));
                        memset((GT_VOID *)(&vfdArray[prvTgfPortNum][1]), 0, sizeof(TGF_VFD_INFO_STC));

                        vfdArray[prvTgfPortNum][1].mode = TGF_VFD_MODE_STATIC_E;
                        vfdArray[prvTgfPortNum][1].modeExtraInfo = 0;
                        vfdArray[prvTgfPortNum][1].offset = (pData_Local->uiTrigger2Offset / 8); /* TG need adrs * 8, UT doesn't. */
                        vfdArray[prvTgfPortNum][1].cycleCount = pData_Local->uiTrigger2Range;

                        memcpy((GT_VOID *)(vfdArray[prvTgfPortNum][1].patternPtr), &pData_Local->ucTrigger2Pattern[0], pData_Local->uiTrigger2Range);

                        if(TgServer_Print_Flag_DBG_KA == GT_TRUE)  /* <--- TODO: */
                        {
                                PRV_UTF_LOG0_MAC("\n== TriggerMode = ETH_TRIGGER2_ONLY. ==\n");
                        }
                }
                else if((pData_Local->ucTriggerMode == ETH_TRIGGER1_OR_TRIGGER2)||(pData_Local->ucTriggerMode == ETH_TRIGGER1_AND_TRIGGER2))
                {
                        memset((GT_VOID *)(&vfdArray[prvTgfPortNum][0]), 0, sizeof(TGF_VFD_INFO_STC));
                        memset((GT_VOID *)(&vfdArray[prvTgfPortNum][1]), 0, sizeof(TGF_VFD_INFO_STC));

                        vfdArray[prvTgfPortNum][0].mode = TGF_VFD_MODE_STATIC_E;
                        vfdArray[prvTgfPortNum][0].modeExtraInfo = 0;
                        vfdArray[prvTgfPortNum][0].offset = (pData_Local->uiTrigger1Offset / 8);
                        vfdArray[prvTgfPortNum][0].cycleCount = pData_Local->uiTrigger1Range;

                        vfdArray[prvTgfPortNum][1].mode = TGF_VFD_MODE_STATIC_E;
                        vfdArray[prvTgfPortNum][1].modeExtraInfo = 0;
                        vfdArray[prvTgfPortNum][1].offset = (pData_Local->uiTrigger2Offset / 8);
                        vfdArray[prvTgfPortNum][1].cycleCount = pData_Local->uiTrigger2Range;

                        memcpy((GT_VOID *)(vfdArray[prvTgfPortNum][0].patternPtr), &pData_Local->ucTrigger1Pattern[0], pData_Local->uiTrigger1Range);
                        memcpy((GT_VOID *)(vfdArray[prvTgfPortNum][1].patternPtr), &pData_Local->ucTrigger2Pattern[0], pData_Local->uiTrigger2Range);

                        if(TgServer_Print_Flag_DBG_KA == GT_TRUE)  /* <--- TODO: */
                        {
                                PRV_UTF_LOG0_MAC("\n== TriggerMode = (ETH_TRIGGER1_OR_TRIGGER2 || ETH_TRIGGER1_AND_TRIGGER2). ==\n");
                        }
                }

                /* Set Trigger Mode flag. */
                TgServer_Triggers_Mode_Flag = pData_Local->ucTriggerMode;

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_03");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else
        {
                return(GT_FAIL);
        }

        if((vfdArray[prvTgfPortNum][0].mode == TGF_VFD_MODE_STATIC_E)||
           (vfdArray[prvTgfPortNum][1].mode == TGF_VFD_MODE_STATIC_E))
        {
                /* setup nexthope portInterface for capturing */
                portInterface.type              = CPSS_INTERFACE_PORT_E;
                portInterface.devPort.hwDevNum  = prvTgfDevNum;
                portInterface.devPort.portNum   = prvTgfPortNum;

                if(prvTgfFwModeEthFlag == 0x1)  /* ETH_PCL_SET */
                        nCaptureMode = TGF_CAPTURE_MODE_PCL_E;
                else if(prvTgfFwModeEthFlag == 0x2)  /* ETH_MIRRORING_SET */
                        nCaptureMode = TGF_CAPTURE_MODE_MIRRORING_E;
                else if(prvTgfFwModeEthFlag == 0x0)
                {
                        PRV_UTF_LOG0_MAC("\n= FAIL on set prvTgfFwModeEthFlag. !");
                        status = GT_FAIL;
                }

                /* New Triggers read method. */
                /* enable packet trace */
                status = tgfTrafficTracePacketByteSet(GT_TRUE);
                if(status != GT_OK)
                        PRV_UTF_LOG0_MAC("\n= FAIL on Error in tgfTrafficTracePacketByteSet. !");

                /* enable capturing Tx packets on port */
                /* =================================== */
                status = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, nCaptureMode, GT_TRUE);
                if(status != GT_OK)
                        PRV_UTF_LOG0_MAC("\n= FAIL on tgfTrafficGeneratorPortTxEthCaptureSet. !");
                else if(TgServer_Print_Flag_DBG_03)
                        printf("\n== SIM_HTSetStructure_03 - tgfTrafficGeneratorPortTxEthCaptureSet = TRUE. portNumber = %d ==\n", prvTgfPortNum);
        }

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        if(GT_OK == status)
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetStructure_03_OK");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetStructure_03_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }

        if(TgServer_Print_Flag_DBG_KA == GT_TRUE)
        {
                PRV_UTF_LOG0_MAC("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTSetStructure_03 ==");
                PRV_UTF_LOG0_MAC("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTSetStructure_04
*
* DESCRIPTION:
*       Run HTSetStructure- Set Triggers.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTSetStructure_04(int sockfd)
{
        GT_STATUS status = GT_OK;
        int prvTgfDevNum = 0x0, n = 0x0;
        int prvTgfPortNum = 0x0;
        FSTCaptureParams *pData_Local;
        char buffer[READ_BUFFER_SIZE];
        char DataBuffer[256];
           GT_U32  nBytes = READ_BUFFER_SIZE;
        GT_U32  nLeft, i = 0x0, nCaptureMode = 0x0;
        GT_U32 tempBurst = 0x0;
        int length = 0x0, length_01 = 0x0;
        int counter_aa = 0x0;

        GT_BOOL                 getFirst = GT_TRUE;
        GT_U8                   queue    = 0;
        GT_U32                  buffLen  = CAPTURE_BUFFER_SIZE;
           static  GT_U8                packetBuff[CAPTURE_BUFFER_SIZE] = {0};
        GT_U32                  packetActualLength  = 0;
        TGF_NET_DSA_STC                rxParam;

        CPSS_INTERFACE_INFO_STC        portInterface = {0};

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTSetStructure_04 ==");
                PRV_UTF_LOG0_MAC("\n====================================\n");
        }
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi(buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading prvTgfDevNum ");
                else if(prvTgfDevNum > 2)
                        error("ERROR prvTgfDevNum > 2");

                prvTgfPortNum = (ctoi(buffer[9]) * 100) + (ctoi(buffer[10]) * 10) + ctoi(buffer[11]);

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "struct_data", 11) == 0)
        {
                length = ctoi(buffer[13]);
                if(length < 0x0)
                        error("ERROR reading length_02");

                length_01 = ctoi(buffer[12]);
                if(length_01 < 0x0)
                        error("ERROR reading length_03");

                length = (length_01 * 10) + length;
                if(length == 0x0)
                        error("ERROR reading length");

                if(TgServer_Print_Flag_DBG_KA == GT_TRUE)  /* <--- TODO: */
                {
                        PRV_UTF_LOG2_MAC("\n== Triggers struct length = %d, Port #: %d ==\n", length, prvTgfPortNum);
                }

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(DataBuffer, 0, sizeof(DataBuffer));

        nLeft = (nBytes);

        n = readSocketData(sockfd, &DataBuffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n DataBuffer[n] = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (n > 0)
        {
                if(TgServer_Print_Flag_DBG_02)
                {
                        PRV_UTF_LOG0_MAC("\n== CaptureDataBuffer:\n");
                        for(counter_aa = 0x0 ; counter_aa < length ; counter_aa++)
                        {
                    if((counter_aa & 0x0F) == 0)
                            {
                              PRV_UTF_LOG1_MAC("0x%4.4x :", counter_aa);
                    }

                                PRV_UTF_LOG1_MAC(" %2.2x", DataBuffer[counter_aa]);

                                if((counter_aa & 0x0F) == 0x0F)
                    {
                                        PRV_UTF_LOG0_MAC("\n");
                    }
                        }
                        PRV_UTF_LOG0_MAC("\n");
                }

                pData_Local = (FSTCaptureParams *)DataBuffer;

                if((pData_Local->ucStartStop == 0)&&(pData_Local->ucFilterMode == 1)&&
                        (pData_Local->ucCRCErrors == 1)&&(pData_Local->ucCollisions == 1))
                {
                        captureType[prvTgfPortNum] = CAPTURE_BAD_TYPE;
                }
                else if((pData_Local->ucStartStop == 0)&&(pData_Local->ucFilterMode == 1)&&(pData_Local->ucOnTrigger == 1))
                {
                        captureType[prvTgfPortNum] = CAPTURE_TRIGGERS_TYPE;
                }
                if((pData_Local->ucStartStop == 0)&&(pData_Local->ucFilterMode == 0))
                {
                        captureType[prvTgfPortNum] = CAPTURE_ALL_TYPE;
                }

                /* Set Start / Stop. */
                if((pData_Local->ucStartStop == 1)) /* START */
                {
                        /* Check Capture Type for port. */
                        if(captureType[prvTgfPortNum] != 0x0)
                        {
                                /* setup portInterface for capturing */
                                portInterface.type              = CPSS_INTERFACE_PORT_E;
                                portInterface.devPort.hwDevNum  = prvTgfDevNum;
                                portInterface.devPort.portNum   = prvTgfPortNum;

                                if(prvTgfFwModeEthFlag == 0x1)  /* ETH_PCL_SET */
                                        nCaptureMode = TGF_CAPTURE_MODE_PCL_E;
                                else if(prvTgfFwModeEthFlag == 0x2)  /* ETH_MIRRORING_SET */
                                        nCaptureMode = TGF_CAPTURE_MODE_MIRRORING_E;
                                else if(prvTgfFwModeEthFlag == 0x0)
                                {
                                        PRV_UTF_LOG0_MAC("\n= FAIL on set prvTgfFwModeEthFlag. !");
                                        status = GT_FAIL;
                                }

                                /* enable capture on port */
                            status = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, nCaptureMode, GT_TRUE);
                                if(status != GT_OK)
                                        PRV_UTF_LOG2_MAC("\n%s[%d] = FAIL on PortTxEthCaptureSet. !", __FILE__, __LINE__);
                                else if(TgServer_Print_Flag)
                                        PRV_UTF_LOG0_MAC("\n= enable capture on port.");

                                /* clear table */
                                status = tgfTrafficTableRxPcktTblClear();
                                if(status != GT_OK)
                                        PRV_UTF_LOG0_MAC("\n= FAIL on clear table. !");

                                /* set capturing mode */
                                status = tgfTrafficTableRxModeSet(TGF_RX_CYCLIC_E, 600);
                                if(status != GT_OK)
                                        PRV_UTF_LOG0_MAC("\n= FAIL on set capturing mode. !");

                                TgServer_Capture_Start_Set_Flag = GT_TRUE;
                        }
                        else
                        {
                                if(TgServer_Print_Flag_DBG_02)
                                        PRV_UTF_LOG1_MAC("\n= Capture Type for port %d Not set. !\n",prvTgfPortNum);
                        }
                }
                else if((pData_Local->ucStartStop == 0)) /* STOP */
                {
                        /* Check Capture Type for port. */
                        if((TgServer_Capture_Start_Set_Flag)&&(captureType[prvTgfPortNum] != 0x0))
                        {
                                /* setup portInterface for capturing */
                                portInterface.type             = CPSS_INTERFACE_PORT_E;
                                portInterface.devPort.hwDevNum = prvTgfDevNum;
                                portInterface.devPort.portNum  = prvTgfPortNum;

                                if(prvTgfFwModeEthFlag == 0x1)  /* ETH_PCL_SET */
                                        nCaptureMode = TGF_CAPTURE_MODE_PCL_E;
                                else if(prvTgfFwModeEthFlag == 0x2)  /* ETH_MIRRORING_SET */
                                        nCaptureMode = TGF_CAPTURE_MODE_MIRRORING_E;
                                else if(prvTgfFwModeEthFlag == 0x0)
                                {
                                        PRV_UTF_LOG0_MAC("\n= FAIL on set prvTgfFwModeEthFlag. !");
                                        status = GT_FAIL;
                                }

                                /* Get entry from captured packet's table */
                                status = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                                                                               getFirst, GT_TRUE, packetBuff,
                                                                           &buffLen, &packetActualLength,
                                                                           (GT_U8 *)&prvTgfDevNum, &queue,
                                                                           &rxParam);

                                if(status != GT_OK)
                                {
                                        PRV_UTF_LOG0_MAC("\n= FAIL on Get First entry from captured packet's table. !");
                                        status = GT_OK;
                                }
                                else
                                {
                                        if(rxParam.portNum == (GT_U32)prvTgfPortNum)
                                        {
                                                TgServer_Capture_Mode_Flag = GT_TRUE;

                                                if(TgServer_Print_Flag_DBG_02)
                                                {
                                                        PRV_UTF_LOG1_MAC("\n= Got First sampled packets. (i=%ld) !", i);
                                                        PRV_UTF_LOG1_MAC("\n= rxParam.portNum = %d, Capture_Mode_Flag = GT_TRUE", prvTgfPortNum);
                                                        PRV_UTF_LOG1_MAC("\n= ulPortBurstCountSave[ulTxPortNumber] = %ld", (GT_U32)ulPortBurstCountSave[ulTxPortNumber]);
                                                        PRV_UTF_LOG1_MAC("\n= ulPortPacketSizeSave[ulTxPortNumber] = %ld", (GT_U32)ulPortPacketSizeSave[ulTxPortNumber]);
                                                }
                                        }
                                }

                                if(TgServer_Capture_Mode_Flag != GT_TRUE)
                                {
                                        getFirst = GT_FALSE;

                                        tempBurst = ((GT_U32)ulPortBurstCountSave[ulTxPortNumber]);
                                        PRV_UTF_LOG1_MAC("\n= tempBurst = %ld", tempBurst);

                                        PRV_UTF_LOG2_MAC("\n= ulPortBurstCountSave[ulTxPortNumber] = %ld, TxPort = %ld",
                                                                 tempBurst, ulTxPortNumber);

                                    /* get next sampled packets. */
                                    for (i = 1 ; i <= tempBurst ; i++ )
                                        {
                                        status = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                                                  getFirst, GT_TRUE, packetBuff,
                                                                  &buffLen, &packetActualLength,
                                                           (GT_U8 *)&prvTgfDevNum, &queue,
                                                           &rxParam);
                                                if(status != GT_OK)
                                                {
                                                        PRV_UTF_LOG1_MAC("\n= FAIL on get next sampled packets. (i=%ld) !", i);
                                                        status = GT_OK;
                                                }
                                else if(rxParam.portNum == (GT_U32)prvTgfPortNum)
                                                {
                                                        TgServer_Capture_Mode_Flag = GT_TRUE;

                                                        if(TgServer_Print_Flag_DBG_02)
                                                        {
                                                                PRV_UTF_LOG1_MAC("\n= Got next sampled packets. (i=%ld) !", i);
                                                                PRV_UTF_LOG1_MAC("\n= rxParam.portNum = %d, Capture_Mode_Flag(in loop) = GT_TRUE", prvTgfPortNum);
                                                                PRV_UTF_LOG1_MAC("\n= ulPortBurstCountSave[ulTxPortNumber] = %Ld", ulPortBurstCountSave[ulTxPortNumber]);
                                                        }

                                                        break;
                                                }
                                        }
                                }

                                if(TgServer_Capture_Mode_Flag == GT_TRUE)
                                {
                                        /* copy the packet to the shadow buffer. */
                                        memset(CapturePacket, 0, buffLen);
                                        memcpy(CapturePacket, packetBuff, buffLen);
                                }

                                {
                                        if(TgServer_Print_Flag_DBG_02)
                                        {
                                        int counter_aa = 0x0, c_length = 256;

                                        PRV_UTF_LOG1_MAC("\n== CapturePacket: (length = %d)\n", c_length);

                                                for(counter_aa = 0x0 ; counter_aa < c_length ; counter_aa++)
                                                        PRV_UTF_LOG1_MAC(" %02x", (CapturePacket[counter_aa]));

                                                PRV_UTF_LOG0_MAC("\n");
                                        }
                                }

                                /* Clear Capture Type for port. */
                                captureType[prvTgfPortNum]                 = 0x0;
                                TgServer_Capture_Mode_Flag                = GT_FALSE;
                                TgServer_Capture_Start_Set_Flag        = GT_FALSE;

                                if(TgServer_Print_Flag_DBG_02)
                                        PRV_UTF_LOG1_MAC("\n= ucStop for port %d has been set. !\n",prvTgfPortNum);
                        }

                        /* clear table */
                        status = tgfTrafficTableRxPcktTblClear();
                        if(status != GT_OK)
                                PRV_UTF_LOG0_MAC("\n= FAIL on clear table. !");

                }/* else if((pData_Local->ucStartStop == 0)) */

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_03");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else
        {
                return(GT_FAIL);
        }

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        if(GT_OK == status)
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetStructure_04_OK");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTSetStructure_04_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTSetStructure_04 ==");
                PRV_UTF_LOG0_MAC("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTRun
*
* DESCRIPTION:
*       Run HTRun - Start transmit.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTRun(int sockfd)
{
        GT_STATUS status = GT_OK;
        char buffer[READ_BUFFER_SIZE];
           int  nBytes = READ_BUFFER_SIZE, idx = 0x0, idx_save = 0x0;
        int  nLeft, n = 0x0;
        int  prvTgfBurstCount = 0x1;
        GT_U32 vfdArrayStart_L = 0x0;
        GT_U32 numVfd_L = 0;/* Local number of VFDs in vfdArray */
        GT_U32 cycleCount         = 0x0;
        GT_U32 ulCRC_Block        = 0x0;
        GT_U32 ulCount                = 0x0;
        GT_U32 ulCRC_ExpectedTrigger = 0x0;

        int nCaptureMode = 0x0;
        CPSS_INTERFACE_INFO_STC portInterface;

    int sleepAfterXCount = 32; /* == 0, meaning NO SLEEP needed during the burst of 'burstCount' */
    int sleepTime        = 5; /* sleep time (in milliseconds) after X packets sent. */

#ifdef  _VXWORKS
        static volatile int prvTgfDevNum = 0x0, portsCount = 0x0;
        static volatile int portNumber = 0x0, portNumber_01 = 0x0, portNumber_02 = 0x0;
#else
#ifdef WIN32
        GT_U8 portsCount = 0x0;
        int prvTgfDevNum = 0x0;
        int portNumber = 0x0, portNumber_01 = 0x0, portNumber_02 = 0x0;
#else /* Linux */
#ifdef LINUX
        GT_U8 portsCount = 0x0;
        int prvTgfDevNum = 0x0;
        int portNumber = 0x0, portNumber_01 = 0x0, portNumber_02 = 0x0;
#endif /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        GT_BOOL                 getFirst = GT_TRUE;
        GT_BOOL             triggered         = GT_FALSE;
        GT_U8                   queue            = 0x0, i = 0x0;
        GT_U32                  buffLen          = TRIGGERS_PACKET_BUFFER_SIZE;
        GT_U32                  packetActualLength  = 0x0;
        TGF_NET_DSA_STC                rxParam;

        rxParam.portNum                = 0x0;

        if(TgServer_Print_Flag_DBG_KA)
        {
                /*PRV_UTF_LOG0_MAC("\n\n====================================\n");*/
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTRun ==");
                /*PRV_UTF_LOG0_MAC("\n====================================\n");*/
        }

        memset((char *)&pData, 0, sizeof(ETHTransmit));

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi(buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading DevNum");
                else if(prvTgfDevNum > 2)
                        error("ERROR prvTgfDevNum > 2");

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else        { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "PortsArray", 10) == 0)
        {
                portNumber    = ctoi(buffer[13]);
                portNumber_01 = ctoi(buffer[12]);
                portNumber_02 = ctoi(buffer[11]);

                if(portNumber < 0x0)
                        error("ERROR reading m_Port");

                if(portNumber_01 < 0x0)
                        error("ERROR reading m_Port");

                if(portNumber_02 < 0x0)
                        error("ERROR reading m_Port");

                portNumber = ((portNumber_02 * 100) + (portNumber_01 * 10) + portNumber);

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else        {return(GT_FAIL);}

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                PRV_UTF_LOG2_MAC("\n== prvTgfDevNum = %d, portNumber = %ld. ==\n", prvTgfDevNum, portNumber);

        for(idx = 0x0 ; idx < 5 ; idx++)
        {
                /* Check port # used in the test. (for Tx). */
                if(portNumber == ulPortTxFlagSave.PortTxNumberSave[idx])
                {
                        /* Send the ulPortTxPacketSave_One[] buffer for this port to the convert function to prepare it for the EUT SetTx. */
                        status = TGServiceParseConvertPacket_One(&ulPortTxPacketSave_One[idx][0], idx);

                        /* Send the ulPortTxPacketSave_Two[] buffer for this port to the convert function to prepare it for the EUT SetTx. */
                        status = TGServiceParseConvertPacket_Two(&ulPortTxPacketSave_Two[idx][0], idx /*, &numVfd_L, &vfdArrayStart*/);

                        idx_save = idx;
                        break;
                }
        }

#if 0  /* TODO:  <-- a.y. 12/09/12 */
        if((idx == 6)&&(0xFF == ulPortTxFlagSave.PortTxNumberSave[idx-1]))
                if(TgServer_Print_Flag_DBG_KA)
                        PRV_UTF_LOG1_MAC("\n == prvTgfSetTxSetupEth() FAIL, ulPortTxFlagSave.PortTxNumberSave[%d] not set. ! ==", (idx_save));
#endif

        /* Set Tx Setup */
        /* ============ */
        status = prvTgfSetTxSetupEth((GT_U8)prvTgfDevNum, &prvTgfPacketInfo[idx_save], prvTgfBurstCount, 0, NULL);

        /*==================================================================================================================*/
        /*                                           End of SetTxSetup phase I                                              */
        /*==================================================================================================================*/

        /* Convert the buffer according to cpu type, for BE / LE modes. */
        #if (defined(PPC_CPU) || defined(POWERPC_CPU) || defined(MV_CPU_BE))

                pData.ucTransmitMode        = (ulPortTxPacketSave_Two[idx_save][0]);
                pData.uiDataLength    = (((ulPortTxPacketSave_Two[idx_save][ 3]) <<  8) | ulPortTxPacketSave_Two[idx_save][2]);
                pData.ulBurstCount         = (((ulPortTxPacketSave_Two[idx_save][11]) << 24) | ((ulPortTxPacketSave_Two[idx_save][10]) << 16) |
                                                              ((ulPortTxPacketSave_Two[idx_save][ 9]) <<  8) | ulPortTxPacketSave_Two[idx_save][8]);
                pData.ulInterFrameGap        = (((ulPortTxPacketSave_Two[idx_save][19]) << 24) | ((ulPortTxPacketSave_Two[idx_save][18]) << 16) |
                                                              ((ulPortTxPacketSave_Two[idx_save][17]) <<  8) | ulPortTxPacketSave_Two[idx_save][16]);
        #elif defined(CPU_ARM)

                #ifdef _VXWORKS
                        memcpy((ETHTransmit *)&pData, (&ulPortTxPacketSave_Two[idx_save][0]), sizeof(ETHTransmit));
                #else
                #ifdef WIN32
                        memcpy((GT_VOID *)&pData, (&ulPortTxPacketSave_Two[idx_save][0]), sizeof(ETHTransmit));
                #else /* Linux */
                #ifdef  LINUX
                        memcpy((GT_VOID *)&pData, (&ulPortTxPacketSave_Two[idx_save][0]), sizeof(ETHTransmit));
                #endif  /* Linux */
                #endif /* WIN32 */
                #endif /* _VXWORKS */

        #elif (defined(INTEL64_CPU) || defined(INTEL_CPU))

                pData.ucTransmitMode        = (unsigned char)(ulPortTxPacketSave_Two[idx_save][0]);
                pData.uiDataLength        = (unsigned short)(ulPortTxPacketSave_Two[idx_save][3] << 8 | ulPortTxPacketSave_Two[idx_save][2]);
                pData.ulBurstCount        = (unsigned short)(((ulPortTxPacketSave_Two[idx_save][11]) << 24) | ((ulPortTxPacketSave_Two[idx_save][10]) << 16) |
                                                                 ((ulPortTxPacketSave_Two[idx_save][ 9]) << 8) | ulPortTxPacketSave_Two[idx_save][8]);
                pData.ulInterFrameGap        = (unsigned short)(((ulPortTxPacketSave_Two[idx_save][19]) << 24) | ((ulPortTxPacketSave_Two[idx_save][18]) << 16) |
                                                                 ((ulPortTxPacketSave_Two[idx_save][17]) << 8) | ((ulPortTxPacketSave_Two[idx_save][16] & 0x000000ff)));
        #endif

        if(TgServer_Print_Flag_DBG_KA == GT_TRUE)
        {
                PRV_UTF_LOG1_MAC("== SIM_HTRun(), DataBuffer->ucTransmitMode    = %d. ==\n",  pData.ucTransmitMode);
                printf /*PRV_UTF_LOG1_MAC*/("==           DataBuffer->uiDataLength         = %hd. ==\n", pData.uiDataLength);
                printf /*PRV_UTF_LOG1_MAC*/("==           DataBuffer->ulBurstCount         = %ld. ==\n", pData.ulBurstCount);
                printf /*PRV_UTF_LOG1_MAC*/("==           DataBuffer->ulInterFrameGap      = %ld. ==\n", pData.ulInterFrameGap);
        }

        /*if((pData.ucTransmitMode == CONTINUOUS_PACKET_MODE)||
             (pData.ucTransmitMode == CONTINUOUS_BURST_MODE))*/
        if((pData.ucTransmitMode != SINGLE_BURST_MODE))
        {
                        /* Set Tx Setup (SIM_HTRun()) */
                        /* =========================================================== */
                        status = prvTgfSetTxSetupEth((GT_U8)prvTgfDevNum, &prvTgfPacketInfo[idx_save], 1, 0, NULL);
                        if(status != GT_OK)
                                PRV_UTF_LOG0_MAC("\n= FAIL on prvTgfSetTxSetupEth. !");

                        if(TgServer_Print_Flag_DBG_KA_SocHndl)  /*  <---- TODO: */
                        {
                                PRV_UTF_LOG0_MAC("\n\n== Set ucTransmitMode = WS\n");
                        }

                        /* Configure WS mode */
                        /* =========================================================== */
                        status = prvTgfTxModeSetupEth((GT_U8)prvTgfDevNum, portNumber,
                                                                                PRV_TGF_TX_MODE_CONTINUOUS_E,
                                                                                PRV_TGF_TX_CONT_MODE_UNIT_WS_E,
                                                                                0);
                        if(status != GT_OK)
                                PRV_UTF_LOG0_MAC("\n= FAIL on prvTgfTxModeSetupEth. !");

                        /* Signal WS Tx from port. */
                        ulPortWsBurstCount[portNumber] = 148800;

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                PRV_UTF_LOG2_MAC("\n== ADD: ulPortBurstCount[port #%d] = %Ld\n",
                                                portNumber, ulPortWsBurstCount[portNumber]);
                        }

                        /* Set WS flag. */
                        TgServer_WS_Mode_Flag = GT_TRUE;

                        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                           PRV_UTF_LOG0_MAC("\n== SIM_HTRun - Set 'TgServer_WS_Mode_Flag' = GT_TRUE. ==\n");
        }
        else
        {
                        numVfd_L = numVfd[portNumber];
                        vfdArrayStart_L = vfdArrayStart[portNumber];

                        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        {
                                PRV_UTF_LOG2_MAC("\n== numVfd_L: = %ld, vfdArrayStart = %ld\n", numVfd_L, vfdArrayStart_L);
                        }

                        /* Set Tx Setup */
                        /* =========================================================== */
                        status = prvTgfSetTxSetupEth((GT_U8)prvTgfDevNum, &prvTgfPacketInfo[idx_save], \
                                pData.ulBurstCount, numVfd_L, (numVfd_L ? (&prvTgfVfdArray[vfdArrayStart_L]) : NULL));
                        if(status != GT_OK)
                                printf /*PRV_UTF_LOG0_MAC*/("\n= FAIL on prvTgfSetTxSetupEth. !");

                        if(pData.ulInterFrameGap >= 20000) /*38000*/
                        {
                                /* Set Tx Setup InterFrameGap by using sleepTime between XCount packets */
                                /* ==================================================================== */
                                status = prvTgfSetTxSetup2Eth(sleepAfterXCount, sleepTime);
                                if(status != GT_OK)
                                        printf /*PRV_UTF_LOG0_MAC*/("\n= FAIL on prvTgfSetTxSetup2Eth. !");

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                {
                                        PRV_UTF_LOG3_MAC("\n\n== pData.ulInterFrameGap = %ld. set Xcount = %d, SleepTime = %d\n", \
                                                                pData.ulInterFrameGap, sleepAfterXCount, sleepTime);
                                }
                        }
                        else if(pData.ulInterFrameGap >= 10000)
                        {
                                /* default traffic Tx delay in mS */
                                /* #define PRV_TGF_TX_DELAY_CNS        100 */

                                sleepAfterXCount        = 1;
                                sleepTime                        = 1;

                                /* Set Tx Setup InterFrameGap by using sleepTime between XCount packets */
                                /* ==================================================================== */
                                status = prvTgfSetTxSetup2Eth(sleepAfterXCount, sleepTime);
                                if(status != GT_OK)
                                        printf("\n= FAIL on prvTgfSetTxSetup2Eth. !");

                                if(TgServer_Print_Flag_DBG_03)
                                {
                                        printf("\n\n== pData.ulInterFrameGap = %ld. set Xcount = %d, SleepTime = %d\n", \
                                                                pData.ulInterFrameGap, sleepAfterXCount, sleepTime);
                                }
                        }

                        /* Configure default mode */
                        /* =========================================================== */
                        status = prvTgfTxModeSetupEth((GT_U8)prvTgfDevNum, portNumber,
                                                PRV_TGF_TX_MODE_SINGLE_BURST_E,
                                                                                PRV_TGF_TX_CONT_MODE_UNIT_WS_E,
                                                                                0);
                        if(status != GT_OK)
                                PRV_UTF_LOG0_MAC("\n= FAIL on prvTgfTxModeSetupEth. !");

                        /* Clear FWS flag. */
                        TgServer_WS_Mode_Flag = GT_FALSE;
                        if(TgServer_Print_Flag_DBG_03)
                                PRV_UTF_LOG0_MAC("\n== SIM_HTRun - Set 'TgServer_WS_Mode_Flag' = GT_FALSE. ==\n");

                        /* Set # of packets Tx from port to the Shadow PortBurst counter. */
                        ulPortShadowBurstCount [portNumber] = pData.ulBurstCount;

                        /* Set # of Octets Tx from port to the Shadow PortOctets counter. */
                        ulPortShadowOctetsCount[portNumber] = ((pData.uiDataLength + CRC_SIZE)*(pData.ulBurstCount));

                        /* Save port number and BurstCount[] for Capture. */
                        ulTxPortNumber = portNumber;
                        ulPortBurstCountSave[portNumber] += pData.ulBurstCount;
                        if(pData.uiDataLength)
                                ulPortPacketSizeSave[portNumber] = pData.uiDataLength;

                        if(TgServer_Print_Flag_DBG_03)
                        {
                        #if (defined(PPC_CPU) || defined(POWERPC_CPU) || defined(MV_CPU_BE))
                                char printBuffer_L[20];

                                GT_U32 cntHi32Bits_L  = 0x0;
                                GT_U32 cntLow32Bits_L = 0x0;

                                /* get Hi and Low 32 bits of the port shadow counter */
                                cntHi32Bits_L  = (((uint64) ulPortShadowBurstCount[portNumber]) << 32);
                                cntLow32Bits_L = ((GT_U32)ulPortShadowBurstCount[portNumber]);

                                sprintf(printBuffer_L, "%08lX%08lX", cntHi32Bits_L, cntLow32Bits_L);
                                PRV_UTF_LOG2_MAC("== ADD: ulPortShadowOctetsCount[port #%d] = 0x%16.16s\n", portNumber, printBuffer_L);

                                /*---------------------------------------------------------------------*/

                                /* get Hi and Low 32 bits of the port shadow octets counter */
                                cntHi32Bits_L  = (((uint64) ulPortShadowOctetsCount[portNumber]) << 32);
                                cntLow32Bits_L = ((GT_U32)ulPortShadowOctetsCount[portNumber]);

                                sprintf(printBuffer_L, "%08lX%08lX", cntHi32Bits_L, cntLow32Bits_L);
                                PRV_UTF_LOG2_MAC("== ADD: ulPortOctetsCount[port #%d] = 0x%16.16s\n", portNumber, printBuffer_L);

                        #elif (defined(CPU_ARM) || defined(INTEL64_CPU) || defined(INTEL_CPU))

                                PRV_UTF_LOG2_MAC("\n== ADD: ulPortShadowBurstCount[port #%ld] = 0x%08llX\n",
                                                portNumber, ulPortShadowBurstCount[portNumber]);

                                PRV_UTF_LOG2_MAC("\n== ADD: ulPortShadowOctetsCount[port #%ld] = 0x%08llX\n",
                                                portNumber, ulPortShadowOctetsCount[portNumber]);

                        #endif
                        }
        } /* else... */

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
        {
                        PRV_UTF_LOG0_MAC("\n== SIM_HTRun ==\n");
                        for(idx = 0x0 ; idx < 6 ; idx++)
                        {
                                PRV_UTF_LOG1_MAC("\n------- Packet L2 array, daMac = 0x%02x", (prvTgfPacketL2Part.daMac[idx]));
                        }

                        PRV_UTF_LOG0_MAC("\n");
                        for(idx = 0x0 ; idx < 6 ; idx++)
                        {
                                PRV_UTF_LOG1_MAC("\n------- Packet L2 array, saMac = 0x%02x", (prvTgfPacketL2Part.saMac[idx]));
                        }
                        PRV_UTF_LOG0_MAC("\n\n");
        }

        /*==================================================================================================================*/
        /*                                           End of SetTxSetup phase II                                             */
        /*==================================================================================================================*/

        /* use different 'etherType for vlan tagging' instead of 0x8100 */
    /* this will make our packet that has '0x8100' to be recognized as 'untagged'
           by the 'traffic generator' and also the CPSS will not override the
           ethertype of tag0 with the DSA tag */

        /* To Enable /Disable [TGF] Log print. */
        if(Trace_Packet_Print_Flag)
                prvUtfLogPrintEnable(UTF_LOG_OUTPUT_SERIAL_ALL_E);
        else{prvUtfLogPrintEnable(UTF_LOG_OUTPUT_SERIAL_FINAL_E);}

        tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);

        /* Check if we have a Trigger on the Tx port, if we do, disable the Trigger
                and after the transmit enable again.
                (# - If we have a trigger on the Tx port, it creates a loop back and the Rx packet
                     is loaded to the CPU and 'Droped' - (saved in Rx buffer) instead of "playing the routing game"). */

        if((vfdArray[portNumber][0].mode == TGF_VFD_MODE_STATIC_E)||
           (vfdArray[portNumber][1].mode == TGF_VFD_MODE_STATIC_E))
        {
                /* setup nexthope portInterface for capturing */
                portInterface.type              = CPSS_INTERFACE_PORT_E;
                portInterface.devPort.hwDevNum  = prvTgfDevNum;
                portInterface.devPort.portNum   = portNumber;

                if(prvTgfFwModeEthFlag == 0x1)  /* ETH_PCL_SET */
                        nCaptureMode = TGF_CAPTURE_MODE_PCL_E;
                else if(prvTgfFwModeEthFlag == 0x2)  /* ETH_MIRRORING_SET */
                        nCaptureMode = TGF_CAPTURE_MODE_MIRRORING_E;
                else if(prvTgfFwModeEthFlag == 0x0)
                {
                        printf("\n= FAIL on set prvTgfFwModeEthFlag. !");
                        status = GT_FAIL;
                }

                /* Disable capturing Tx packets on Transmiting port */
                /* ================================================ */
                status = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, nCaptureMode, GT_FALSE);
                if(status != GT_OK)
                        printf("\n= FAIL on tgfTrafficGeneratorPortTxEthCaptureSet. !");

                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        printf("\n== SIM_HTRun - tgfTrafficGeneratorPortTxEthCaptureSet = FALSE. portNumber = %d ==\n",
                                                        portNumber);

                /* Enable capturing Rx packets on Transmiting port (Set global flag 'ON') */
                status = prvTgfTrafficTableRxStartCapture(GT_TRUE);
                if(status != GT_OK)
                        printf("\n= FAIL on prvTgfTrafficTableRxStartCapture. !");

                TxPortTriggerFlag[portNumber] = GT_TRUE;
        }


        /* send packet */
        /* ############################################################# */
        status = prvTgfStartTransmitingEth((GT_U8)prvTgfDevNum, portNumber);
        /* ############################################################# */

        /* Add # of packets Tx from the Shadow PortBurst counter to PortBurst counter. */
        ulPortBurstCount[portNumber] += ulPortShadowBurstCount[portNumber];

        /* Add # of Octets Tx from the Shadow PortOctets counter PortBurst counter. */
        ulPortOctetsCount[portNumber] += ulPortShadowOctetsCount[portNumber];

        if(TgServer_Print_Flag_DBG_KA)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n== SIM_HTRun - call prvTgfStartTransmitingEth() ->");
                PRV_UTF_LOG2_MAC("\n== ADD: ulPortBurstCount[port #: %ld] = 0x%08LX",
                                portNumber, ulPortBurstCount[portNumber]);

                PRV_UTF_LOG2_MAC("\n== ADD: ulPortOctetsCount[port %ld] = 0x%08llX\n",
                                portNumber, ulPortOctetsCount[portNumber]);
        }

        /* Check if we have a Trigger on the Tx port, if we do, disable the Trigger
                and after the transmit enable again.
                (# - If we have a trigger on the Tx port, it creates a loop back and the Rx packet
                is loaded to the CPU and 'Droped' - (saved in Rx buffer) instead of "playing the routing game"). */

        if(GT_TRUE == TxPortTriggerFlag[portNumber])
        {
                /* Enable capturing Tx packets on port */
                /* =================================== */
                status = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, nCaptureMode, GT_TRUE);
                if(status != GT_OK)
                        printf("\n= FAIL on tgfTrafficGeneratorPortTxEthCaptureSet. !");

                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        printf("\n== SIM_HTRun - tgfTrafficGeneratorPortTxEthCaptureSet = TRUE. portNumber = %d ==\n", portNumber);
        }

        /* Clear Tx Setup InterFrameGap by using sleepTime & XCount to 0x0. */
        /* ================================================================ */
        status &= prvTgfSetTxSetup2Eth(0x0 /*sleepAfterXCount*/, 0x0 /*sleepTime*/);

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
        {
                PRV_UTF_LOG1_MAC("\n\n== Clear Tx Setup InterFrameGap, status = %s\n", (status ? "OK" : "FAIL"));
        }

        if(status != GT_OK)
                status = GT_OK;

        /* If we use FWS mode, then wait to get stable traffic - sleep 20 Msec. */
        if(TgServer_WS_Mode_Flag)
        {
                cpssOsTimerWkAfter(20);
        }

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                PRV_UTF_LOG1_MAC("\n==NUM_OF_PORTS = %d\n", NUM_OF_PORTS);

        /*==================================================================================*/
        /*                              Get Triggers.                                       */
        /*==================================================================================*/
for(portsCount = 0x0 ; portsCount < NUM_OF_PORTS ; portsCount++)
{
        if(TgServer_Print_Flag_DBG_01)
                PRV_UTF_LOG1_MAC("\n== Enter portsCount Loop - %d\n", portsCount);

        if(vfdArray[portsCount][0].mode == TGF_VFD_MODE_STATIC_E)
        {
                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        PRV_UTF_LOG0_MAC("\n== Enter vfdArray[portsCount][0].mode\n");

                /* enable packet trace */
                status = tgfTrafficTracePacketByteSet(GT_TRUE);
                if(status != GT_OK)
                        PRV_UTF_LOG0_MAC("\n= Error in tgfTrafficTracePacketByteSet. !");

                while(1)
                {
                        /* Clear the buffer. */
                        if(NULL != trigPacketBuff)
                                memset(trigPacketBuff, 0, sizeof(trigPacketBuff));

                        /* Get entry from captured packet's table */
                        status = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                              getFirst, GT_TRUE, trigPacketBuff,
                                              &buffLen, &packetActualLength,
                                              (GT_U8 *)&prvTgfDevNum, &queue,
                                              &rxParam);
                        if (GT_OK != status)
                        {
                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        PRV_UTF_LOG1_MAC("\n== status of Get entry from captured packet's table = %d\n", status);
                                break;
                        }
                        else if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        {
                                PRV_UTF_LOG0_MAC("\n== trigPacketBuff -> vfdArray[0]: \n");
                                for(i = 0x0 ; i < buffLen ; i++)
                                {
                                        if((i & 0x0F) == 0)
                                        {
                                                PRV_UTF_LOG1_MAC("0x%4.4x :", i);
                                        }

                                        PRV_UTF_LOG1_MAC(" %2.2x", (GT_U8)(trigPacketBuff[i]));

                                        if((i & 0x0F) == 0x0F)
                                        {
                                                PRV_UTF_LOG0_MAC("\n");
                                        }
                                }
                                PRV_UTF_LOG0_MAC("\n");
                        }

                        /* get the next entry */
                        getFirst = GT_FALSE;

                        /* check the portNumber */
                        if(rxParam.hwDevNum != (GT_U32)prvTgfDevNum || rxParam.portNum != portsCount)
                        {
                                /* go to the next packet */
                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                {
                                        PRV_UTF_LOG2_MAC("\n== go to the next packet. (numTriggers[port #%d][0] = %ld) ==", portsCount, numTriggers[portsCount]);
                                        PRV_UTF_LOG2_MAC("\n== rxParam.hwDevNum = %ld, rxParam.portNum = %ld ==\n", rxParam.hwDevNum, rxParam.portNum);
                                }
                                continue;
                        }

                        cycleCount = (vfdArray[portsCount][0].cycleCount);

                        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        {
                                PRV_UTF_LOG2_MAC("\n== vfdArray[port #%d][0].mode          = %d   ==", portsCount, vfdArray[portsCount][0].mode);
                                PRV_UTF_LOG2_MAC("\n== vfdArray[port #%d][0].modeExtraInfo = %ld  ==", portsCount, vfdArray[portsCount][0].modeExtraInfo);
                                PRV_UTF_LOG2_MAC("\n== vfdArray[port #%d][0].offset        = %ld  ==", portsCount, vfdArray[portsCount][0].offset);
                                PRV_UTF_LOG2_MAC("\n== vfdArray[port #%d][0].cycleCount    = %ld  ==\n", portsCount, vfdArray[portsCount][0].cycleCount);
                                for(i=0x0 ; i < cycleCount ; i++)
                                        PRV_UTF_LOG3_MAC("\n== vfdArray[port #%d][0].patternPtr[%d] = 0x%x ==", portsCount, i, vfdArray[portsCount][0].patternPtr[i]);

                                PRV_UTF_LOG1_MAC("\n== buffLen = %ld =\n", buffLen);
                        }

                        /* If the trigger is looking for CRC we need to calclate it and compare with the requested trigger. */
                        /* CRC - Assia. */
                        if((packetActualLength - cycleCount) == (vfdArray[portsCount][0].offset))
                        {
                                if(cycleCount > 4)
                                {
                                        error("ERROR CRC trigger is > 4. !");
                                        break;
                                }

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        printf("== packetActualLength = %ld\n", packetActualLength);

                                ulCount        = (packetActualLength - cycleCount);

                                /* Calculate the 32-Bit CRC of a block of data all at once. */
                                /* ulCRC_Block = CalculateBlockCRC32(ulCount, (GT_U8 *)trigPacketBuff); - Not for BE. ! */
                                ulCRC_Block = prvCpssCalcCrc32(0xffffffff,  (GT_U8 *)trigPacketBuff, ulCount);

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        printf("== ulCRC_Block = 0x%08lx\n", ulCRC_Block);

                                ulCRC_ExpectedTrigger = ((vfdArray[portsCount][0].patternPtr[0] << 24)|
                                                                                 (vfdArray[portsCount][0].patternPtr[1] << 16)|
                                                                                 (vfdArray[portsCount][0].patternPtr[2] <<  8)|
                                                                                 (vfdArray[portsCount][0].patternPtr[3]));

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        printf("== ulCRC_ExpectedTrigger = 0x%08lx\n", ulCRC_ExpectedTrigger);

                                if(ulCRC_Block == ulCRC_ExpectedTrigger)
                                {
                                        numTriggers_Arry01[portsCount]++;
                                }

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        printf("\n== numTriggers_Arry01[port #%d] = %ld ==\n", portsCount,
                                                                numTriggers_Arry01[portsCount]);
                        }
                        else
                        {
                                /* check triggers */
                                status = tgfTrafficGeneratorTxEthTriggerCheck((GT_U8 *)trigPacketBuff, (GT_U32)buffLen, 1,
                                                                                        (TGF_VFD_INFO_STC *)(&vfdArray[portsCount][0]), (GT_BOOL *)&triggered);
                                if (GT_OK != status)
                                {
                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                                PRV_UTF_LOG1_MAC("\n== status of check triggers = %d\n", status);
                                        break;
                                }

                                if(triggered)
                                        numTriggers[portsCount]++;

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        PRV_UTF_LOG2_MAC("\n== numTriggers[port #%d] = %ld ==\n", portsCount, numTriggers[portsCount]);
                        }
                }

            /* disable packet trace */
                status = tgfTrafficTracePacketByteSet(GT_FALSE);
        }

        /* get the first entry (once more). */
        getFirst = GT_TRUE;

        if(vfdArray[portsCount][1].mode == TGF_VFD_MODE_STATIC_E)
        {
                /* enable packet trace */
                status = tgfTrafficTracePacketByteSet(GT_TRUE);
                if(status != GT_OK)
                        PRV_UTF_LOG0_MAC("\n= Error in tgfTrafficTracePacketByteSet. !");

                while(1)
                {
                        /* Clear the buffer. */
                        if(NULL != trigPacketBuff)
                                memset(trigPacketBuff, 0, sizeof(trigPacketBuff));

                        /* Get entry from captured packet's table */
                        status = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                              getFirst, GT_TRUE, trigPacketBuff,
                                              &buffLen, &packetActualLength,
                                              (GT_U8 *)&prvTgfDevNum, &queue,
                                              &rxParam);
                        if (GT_OK != status)
                        {
                                if(TgServer_Print_Flag_DBG_KA)
                                        PRV_UTF_LOG1_MAC("\n== status of Get entry from captured packet's table vfdArray[1] = %d\n", status);
                                break;
                        }
                        else if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        {
                                PRV_UTF_LOG0_MAC("\n== trigPacketBuff -> vfdArray[1]: \n");
                                for(i = 0x0 ; i < buffLen ; i++)
                                {
                                        if((i & 0x0F) == 0)
                                        {
                                                PRV_UTF_LOG1_MAC("0x%4.4x :", i);
                                        }

                                        PRV_UTF_LOG1_MAC(" %2.2x", (GT_U8)(trigPacketBuff[i]));

                                        if((i & 0x0F) == 0x0F)
                                        {
                                                PRV_UTF_LOG0_MAC("\n");
                                        }
                                }
                                PRV_UTF_LOG0_MAC("\n");
                        }

                        /* get the next entry */
                        getFirst = GT_FALSE;

                        /* check the portNumber */
                        if(rxParam.hwDevNum != (GT_U32)prvTgfDevNum || rxParam.portNum != portsCount)
                        {
                                /* go to the next packet */
                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                {
                                        PRV_UTF_LOG2_MAC("\n== go to the next packet. (numTriggers[port #%d][1] = %ld) =", portsCount, numTriggers[portsCount]);
                                        PRV_UTF_LOG2_MAC("\n== rxParam.hwDevNum = %ld, rxParam.portNum = %ld ==\n", rxParam.hwDevNum, rxParam.portNum);
                                }
                                continue;
                        }

                        cycleCount = (vfdArray[portsCount][1].cycleCount);

                        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                        {
                                PRV_UTF_LOG2_MAC("\n== vfdArray[port #%d][1].mode          = %d   ==", portsCount, vfdArray[portsCount][1].mode);
                                PRV_UTF_LOG2_MAC("\n== vfdArray[port #%d][1].modeExtraInfo = %ld  ==", portsCount, vfdArray[portsCount][1].modeExtraInfo);
                                PRV_UTF_LOG2_MAC("\n== vfdArray[port #%d][1].offset        = %ld  ==", portsCount, vfdArray[portsCount][1].offset);
                                PRV_UTF_LOG2_MAC("\n== vfdArray[port #%d][1].cycleCount    = %ld  ==\n", portsCount, vfdArray[portsCount][1].cycleCount);
                                for(i=0x0 ; i < cycleCount ; i++)
                                        PRV_UTF_LOG3_MAC("\n== vfdArray[port #%d][1].patternPtr[%d] = 0x%x ==",
                                                                 portsCount, i, vfdArray[portsCount][1].patternPtr[i]);

                                PRV_UTF_LOG1_MAC("\n== buffLen = %ld =\n", buffLen);
                        }

                        /* If the trigger is looking for CRC we need to calclate it and compare with the requested trigger. */
                        /* CRC - Assia. */
                        if((packetActualLength - cycleCount) == (vfdArray[portsCount][1].offset))
                        {
                                if(cycleCount > 4)
                                {
                                        error("ERROR CRC trigger is > 4. !");
                                        break;
                                }

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        printf("== packetActualLength = %ld\n", packetActualLength);

                                ulCount        = (packetActualLength - cycleCount);

                                /* Calculate the 32-Bit CRC of a block of data all at once. */
                                /* ulCRC_Block = CalculateBlockCRC32(ulCount, (GT_U8 *)trigPacketBuff); - Not for BE. ! */
                                ulCRC_Block = prvCpssCalcCrc32(0xffffffff,  (GT_U8 *)trigPacketBuff, ulCount);

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        printf("== ulCRC_Block = 0x%08lx\n", ulCRC_Block);

                                ulCRC_ExpectedTrigger = ((vfdArray[portsCount][1].patternPtr[0] << 24)|
                                                                                 (vfdArray[portsCount][1].patternPtr[1] << 16)|
                                                                                 (vfdArray[portsCount][1].patternPtr[2] <<  8)|
                                                                                 (vfdArray[portsCount][1].patternPtr[3]));

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        printf("== ulCRC_ExpectedTrigger = 0x%08lx\n", ulCRC_ExpectedTrigger);

                                if(ulCRC_Block == ulCRC_ExpectedTrigger)
                                {
                                        numTriggers_Arry01[portsCount]++;
                                }

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        printf("\n== numTriggers_Arry01[port #%d] = %ld ==\n", portsCount,
                                                                numTriggers_Arry01[portsCount]);
                        }
                        else
                        {
                                /* check triggers */
                                status = tgfTrafficGeneratorTxEthTriggerCheck((GT_U8 *)trigPacketBuff, (GT_U32)buffLen, 1,
                                                                                        (TGF_VFD_INFO_STC *)&vfdArray[portsCount][1], (GT_BOOL *)&triggered);

                                if (GT_OK != status)
                                {
                                        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        PRV_UTF_LOG1_MAC("\n== status of check triggers = %d\n", status);
                                        break;
                                }

                                if(triggered)
                                        numTriggers_Arry01[portsCount]++;

                                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                                        PRV_UTF_LOG2_MAC("\n== numTriggers_Arry01[port #%d] = %ld ==\n", portsCount,
                                                                numTriggers_Arry01[portsCount]);
                        }
                }

            /* disable packet trace */
                status = tgfTrafficTracePacketByteSet(GT_FALSE);
        } /* if(vfdArray[portsCount][1].mode... */

        if((vfdArray[portsCount][0].mode == TGF_VFD_MODE_STATIC_E)||
           (vfdArray[portsCount][1].mode == TGF_VFD_MODE_STATIC_E))
        {
                if(TgServer_Triggers_Mode_Flag == ETH_TRIGGERS_OFF)
                {
                        TotalNumTriggers[portsCount] = (numTriggers[portsCount] + numTriggers_Arry01[portsCount]);
                }
                else if(TgServer_Triggers_Mode_Flag == ETH_TRIGGER1_ONLY)
                {
                        TotalNumTriggers[portsCount] = (numTriggers[portsCount] + numTriggers_Arry01[portsCount]);
                }
                else if(TgServer_Triggers_Mode_Flag == ETH_TRIGGER2_ONLY)
                {
                        TotalNumTriggers[portsCount] = (numTriggers[portsCount] + numTriggers_Arry01[portsCount]);
                }
                else if(TgServer_Triggers_Mode_Flag == ETH_TRIGGER1_OR_TRIGGER2)
                {
                        TotalNumTriggers[portsCount] = (numTriggers[portsCount] | numTriggers_Arry01[portsCount]);
                }
                else if(TgServer_Triggers_Mode_Flag == ETH_TRIGGER1_AND_TRIGGER2)
                {
                        TotalNumTriggers[portsCount] = (numTriggers[portsCount] & numTriggers_Arry01[portsCount]);
                }

                if(TgServer_Print_Flag_DBG_KA_SocHndl)
                PRV_UTF_LOG2_MAC("\n== TotalNumTriggers[port #%d] = %ld ==", portsCount, TotalNumTriggers[portsCount]);

        } /* if((vfdArray[]... */
}   /* for(portsCount = 0x0 ; portsCount > NUM_OF_PORTS ; portsCount++) */

        /*=======================================================================*/
        /*=======================================================================*/

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA_SocHndl)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        if(GT_OK == status)
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTRun_OK");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else if(GT_BAD_PARAM == status)
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTRun_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
                printf /*PRV_UTF_LOG0_MAC*/("\n== SIM_HTRun - GT_BAD_PARAM ==");
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTRun_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
                printf /*PRV_UTF_LOG0_MAC*/("\n== SIM_HTRun - return SIM_HTRun_FAIL ==");
        }

        if(TgServer_Print_Flag_DBG_KA)
        {
                /*PRV_UTF_LOG0_MAC("\n\n===================================\n");*/
                printf /*PRV_UTF_LOG1_MAC*/("== Exit SIM_HTRun == status = %d", status);
                /*PRV_UTF_LOG0_MAC("\n===================================\n");*/
        }

        return (status);
}

/******************************************************************************
* SIM_HTStop
*
* DESCRIPTION:
*       Run HTStop - Stop transmit.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTStop(int sockfd)
{
        GT_STATUS status = GT_OK;
        char buffer[READ_BUFFER_SIZE];
           int  nBytes = READ_BUFFER_SIZE;
        int  nLeft, n = 0x0;

#ifdef WIN32
        int prvTgfDevNum = 0x0;
        int portNumber = 0x0, portNumber_01 = 0x0, portNumber_02 = 0x0;
#else /* VxWorks */
#ifdef  _VXWORKS
        int prvTgfDevNum = 0x0;
        int portNumber = 0x0, portNumber_01 = 0x0, portNumber_02 = 0x0;
#else /* Linux */
#ifdef LINUX
        int prvTgfDevNum = 0x0;
        int portNumber = 0x0, portNumber_01 = 0x0, portNumber_02 = 0x0;
#endif /* Linux */
#endif /* VxWorks */
#endif /* WIN32 */

        if(TgServer_Print_Flag_DBG_01)
        {
                PRV_UTF_LOG0_MAC("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTStop ==");
                PRV_UTF_LOG0_MAC("\n====================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi(buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading DevNum");

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "PortsArray", 10) == 0)
        {
                portNumber    = ctoi(buffer[13]);
                portNumber_01 = ctoi(buffer[12]);
                portNumber_02 = ctoi(buffer[11]);

                if(portNumber < 0x0)
                        error("ERROR reading m_Port");

                if(portNumber_01 < 0x0)
                        error("ERROR reading m_Port");

                if(portNumber_02 < 0x0)
                        error("ERROR reading m_Port");

                portNumber = ((portNumber_02 * 100) + (portNumber_01 * 10) + portNumber);

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        if(TgServer_Print_Flag_DBG_KA)
                PRV_UTF_LOG2_MAC("\n== prvTgfDevNum = %d, portNumber = %d. ==\n", prvTgfDevNum, portNumber);

        /* Stop sending packets */
        status = prvTgfStopTransmitingEth((GT_U8)prvTgfDevNum, portNumber);

        /* If we use WS mode: Restore "TG" mode to Single burst. */
        if(TgServer_WS_Mode_Flag)
        {
                /* Configure default mode */
                /* =========================================================== */
                status = prvTgfTxModeSetupEth((GT_U8)prvTgfDevNum, portNumber,
                                               PRV_TGF_TX_MODE_SINGLE_BURST_E,
                                                                PRV_TGF_TX_CONT_MODE_UNIT_WS_E,
                                                                0);

                if(TgServer_Print_Flag_DBG_KA)
                   PRV_UTF_LOG0_MAC("\n== SIM_HTStop - Restore \"TG\" mode to Single burst. ==\n");
        }

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        if(GT_OK == status)
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTStop_OK");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTStop_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");

                PRV_UTF_LOG0_MAC("== SIM_HTStop_FAIL ==");
        }

        if(TgServer_Print_Flag_DBG_01)
        {
                PRV_UTF_LOG0_MAC("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTStop ==");
                PRV_UTF_LOG0_MAC("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTGetStructure_01
*
* DESCRIPTION:
*       Run HTGetStructure- Get the Counters and Rates info.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTGetStructure_01(int sockfd)
{
        GT_STATUS status = GT_OK;
        CPSS_PORT_MAC_COUNTER_SET_STC portCountersPtr;
        char buffer[READ_BUFFER_SIZE];
        uint64 LocalBuffer[50];
           int  nBytes = READ_BUFFER_SIZE;
        int nLeft, n = 0x0;
        int counter_01;

        char        printstring[30];
        char         printBuffer[20];

        GT_U32 portIndex         = 0x0;
        GT_U32 cntHi32Bits  = 0x0;
        GT_U32 cntLow32Bits = 0x0;

        CPSS_PORT_MAC_COUNTERS_ENT  cntrName;
    uint64 cntrVal = 0x0;
        GT_BOOL clearOnRead = GT_TRUE;

#ifdef WIN32
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else /* VxWorks */
#ifdef  _VXWORKS
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else /* Linux */
#ifdef LINUX
        int prvTgfDevNum = 0x0;
        GT_PHYSICAL_PORT_NUM portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#endif /* Linux */
#endif /* VxWorks */
#endif /* WIN32 */

#if (defined(PPC_CPU) || defined(POWERPC_CPU) || defined(MV_CPU_BE))
    GT_U32 cntHi32Bits_A  = 0x0;
    GT_U32 cntLow32Bits_A = 0x0;
#endif

        if(TgServer_Print_Flag_DBG_KA)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTGetStructure_01 ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n====================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("== SIM_HTGetStructure_01 - After write: Ready_4_Parameter_01 ==\n");
                PRV_UTF_LOG1_MAC("== SIM_HTGetStructure_01 - sizeof(LocalBuffer) = %ld ==\n", sizeof(LocalBuffer));
        }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi(buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading DevNum");

                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "counters_basic_pattern", 22) == 0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                {
                        PRV_UTF_LOG0_MAC("\n  buffer[] = \n");
                        for(counter_01 = 0x0 ; counter_01 < 30 ; counter_01++)
                        {
                                PRV_UTF_LOG1_MAC("0x%02x ", buffer[counter_01]);
                        }
                        PRV_UTF_LOG0_MAC("\n");
                }

                portNum    = ctoi(buffer[25]);
                portNum_01 = ctoi(buffer[24]);
                portNum_02 = ctoi(buffer[23]);

                if(portNum < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_01 < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_02 < 0x0)
                        error("ERROR reading m_Port");

                portNum = ((portNum_02 * 100) + (portNum_01 * 10) + portNum);

                memset(buffer, 0, sizeof(buffer));

                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        if(TgServer_Print_Flag_DBG_KA)
                PRV_UTF_LOG2_MAC("\n== prvTgfDevNum = %d, portNumber = %d. ==\n", prvTgfDevNum, portNum);

        /*=================================================================================*/
        /*                              Get Triggers was here. !                           */
        /*=================================================================================*/

        /* Clear Tx counters on the Transmitting port. */
        if(ulPortBurstCount[portNum] > 0x0)
        {
                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n== DEC: ulPortBurstCount[port #%d] = %Ld", portNum, ulPortBurstCount[portNum]);

                if(TgServer_Print_Flag_DBG_KA)
                        printf("\n== DEC: ulPortOctetsCount[port #%d] = %Ld\n", portNum, ulPortOctetsCount[portNum]);

                /*         Enable  / Disable clear on read for MAC counters:
                        GT_TRUE - Counters are cleared (After read). */

                clearOnRead = GT_TRUE;

#ifdef EXMXPM_FAMILY
        status = cpssExMxPmPortMacCountersClearOnReadEnableSet(prvTgfDevNum, portNum, clearOnRead);
#else
#ifdef CHX_FAMILY
        status = cpssDxChPortMacCountersClearOnReadSet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)portNum, (GT_BOOL )clearOnRead);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

                if(GT_OK != status)
                        printf("\n== %s [line #:%d] - cpssDxChPortMacCountersClearOnReadSet - port #%d FAIL ==\n", __FILE__, __LINE__, portNum);
                else
                {
                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf /*PRV_UTF_LOG0_MAC*/("\n== cpssDxChPortMacCountersClearOnReadSet - to --> %s, port #%d  ==\n",
                                                                                                                                (clearOnRead ? "TRUE" : "FALSE"),portNum);
                        }
                }

                /* CPSS_GOOD_PKTS_SENT_E = CPSS_GOOD_UC_PKTS_SENT_E + CPSS_MC_PKTS_SENT_E + CPSS_BRDC_PKTS_SENT_E */
                /* So... clear all 3. */
                /* ================== */
                cntrName = CPSS_GOOD_UC_PKTS_SENT_E;

                /* Clear a single MIB counter by reading it. */
#ifdef EXMXPM_FAMILY
                status = cpssExMxPmPortSpecificMacCounterGet(prvTgfDevNum, portNum, cntrName, (GT_U64 *)&cntrVal);
#else
#ifdef CHX_FAMILY
                status = cpssDxChMacCounterGet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)portNum,
                                                                        (CPSS_PORT_MAC_COUNTERS_ENT)cntrName, (GT_U64 *)&cntrVal);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

                if(GT_OK != status)
                        printf("\n== %s [line #:%d] - cpssDxChMacCounterGet - port #%d FAIL ==\n", __FILE__, __LINE__, portNum);

                /* Save Extra packets transmited from port (by CPU). */
                ulPortExtraBurstCount[portNum] += cntrVal;

                cntrName = CPSS_MC_PKTS_SENT_E;

                /* Clear a single MIB counter by reading it. (No use for cntrVal). */

#ifdef EXMXPM_FAMILY
                status = cpssExMxPmPortSpecificMacCounterGet(prvTgfDevNum, portNum, cntrName, (GT_U64 *)&cntrVal);
#else
#ifdef CHX_FAMILY
                status = cpssDxChMacCounterGet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)portNum,
                                                                        (CPSS_PORT_MAC_COUNTERS_ENT)cntrName, (GT_U64 *)&cntrVal);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

                if(GT_OK != status)
                        printf("\n== %s [line #:%d] - cpssDxChMacCounterGet - port #%d FAIL ==\n", __FILE__, __LINE__, portNum);

                /* Save Extra packets transmited from port (by CPU). */
                ulPortExtraBurstCount[portNum] += cntrVal;

                cntrName = CPSS_BRDC_PKTS_SENT_E;

                /* Clear a single MIB counter by reading it. (No use for cntrVal). */

#ifdef EXMXPM_FAMILY
                status = cpssExMxPmPortSpecificMacCounterGet(prvTgfDevNum, portNum, cntrName, (GT_U64 *)&cntrVal);
#else
#ifdef CHX_FAMILY
                status = cpssDxChMacCounterGet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)portNum,
                                                                        (CPSS_PORT_MAC_COUNTERS_ENT)cntrName, (GT_U64 *)&cntrVal);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

                if(GT_OK != status)
                        printf("\n== %s [line #:%d] - cpssDxChMacCounterGet - port #%d FAIL ==\n", __FILE__, __LINE__, portNum);

                /* Save Extra packets transmited from port (by CPU). */
                ulPortExtraBurstCount[portNum] += cntrVal;
                /* ================== */

                if(ulPortExtraBurstCount[portNum] > ulPortBurstCount[portNum])
                {
                        ulPortExtraBurstCount[portNum] -= ulPortBurstCount[portNum];
                        if(TgServer_Print_Flag_DBG_03)
                                printf("\n== SAVE: ulPortExtraBurstCount[port #%d] = %Ld", portNum, ulPortExtraBurstCount[portNum]);
                }
                else{ulPortExtraBurstCount[portNum] = 0x0;}

                cntrName = CPSS_GOOD_OCTETS_SENT_E;

                /* Clear a single MIB counter by reading it. (No use for cntrVal). */

#ifdef EXMXPM_FAMILY
                status = cpssExMxPmPortSpecificMacCounterGet(prvTgfDevNum, portNum, cntrName, (GT_U64 *)&cntrVal);
#else
#ifdef CHX_FAMILY
                status = cpssDxChMacCounterGet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)portNum,
                                                                        (CPSS_PORT_MAC_COUNTERS_ENT)cntrName, (GT_U64 *)&cntrVal);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

                if(GT_OK != status)
                        printf("\n== %s [line #:%d] - cpssDxChMacCounterGet - port #%d FAIL ==\n", __FILE__, __LINE__, portNum);

                /* Save Extra Octets transmited from port (by CPU). */
                ulPortExtraOctetsCount[portNum] += cntrVal;

                if(ulPortExtraOctetsCount[portNum] > ulPortOctetsCount[portNum])
                {
                        ulPortExtraOctetsCount[portNum] -= ulPortOctetsCount[portNum];
                        if(TgServer_Print_Flag_DBG_03)
                                printf("\n== SAVE: ulPortExtraOctetsCount[port #%d] = %Ld", portNum, ulPortExtraOctetsCount[portNum]);
                }
                else{ulPortExtraOctetsCount[portNum] = 0x0;}

                /*         Disable clear on read for MAC counters:
                        GT_FALSE - Counters are not cleared. (After read). */

                clearOnRead = GT_FALSE;

#ifdef EXMXPM_FAMILY
        status = cpssExMxPmPortMacCountersClearOnReadEnableSet(prvTgfDevNum, portNum, clearOnRead);
#else
#ifdef CHX_FAMILY
        status = cpssDxChPortMacCountersClearOnReadSet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)portNum, (GT_BOOL )clearOnRead);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

                if(GT_OK != status)
                        printf("\n== %s [line #:%d] - cpssDxChPortMacCountersClearOnReadSet - port #%d FAIL ==\n", __FILE__, __LINE__, portNum);
                else
                {
                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf /*PRV_UTF_LOG0_MAC*/("\n== cpssDxChPortMacCountersClearOnReadSet - to --> %s, port #%d  ==\n",
                                                                                                                                (clearOnRead ? "TRUE" : "FALSE"), portNum);
                        }
                }

                /* Clear port burst counters.                           */
                /* Clear port Octets counters.                          */
                ulPortBurstCount [portNum] = 0x0;
                ulPortOctetsCount[portNum] = 0x0;

        } /* if(ulPortBurstCount[portNum] > 0x0) */

        /* Get counters */
        /* ============ */                            /* disablePrint */

        status = prvTgfReadPortCountersEth((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)portNum , (GT_BOOL)GT_FALSE,
                                                                        (CPSS_PORT_MAC_COUNTER_SET_STC *)&portCountersPtr);


        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer,"Send_counters_Parameters_01 %03ld", sizeof(CPSS_PORT_MAC_COUNTER_SET_STC));

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(LocalBuffer, 0, sizeof(LocalBuffer));

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG1_MAC("\n== PortNumber: %d\n", portNum);
        if(TgServer_Print_Flag_DBG_KA)
           PRV_UTF_LOG1_MAC("\n== SIM_HTGetStructure_01 - 'TgServer_WS_Mode_Flag' = %s. ==\n", \
                        (TgServer_WS_Mode_Flag ? "GT_TRUE" : "GT_FALSE"));
        }

        /* If we use FWS mode. */
        if(TgServer_WS_Mode_Flag)
        {
                /*         Enable clear on read for MAC counters:
                        GT_TRUE - Counters are cleared (After read). */

                clearOnRead = GT_TRUE;

#ifdef EXMXPM_FAMILY
        status = cpssExMxPmPortMacCountersClearOnReadEnableSet(prvTgfDevNum, portNum, clearOnRead);
#else
#ifdef CHX_FAMILY
        status = cpssDxChPortMacCountersClearOnReadSet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)portNum, (GT_BOOL )clearOnRead);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

                if(GT_OK != status)
                        printf("\n== %s [line #:%d] - cpssDxChPortMacCountersClearOnReadSet - port #%d FAIL ==\n", __FILE__, __LINE__, portNum);
                else
                {
                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf /*PRV_UTF_LOG0_MAC*/("\n== cpssDxChPortMacCountersClearOnReadSet - to --> %s, port #%d  ==\n",
                                                                                                                                (clearOnRead ? "TRUE" : "FALSE"), portNum);
                        }
                }

                portIndex = 0x0;
                for(counter_01 = 0x0; ((portNum != prvTgfPortsArray[portIndex]) && (counter_01 < 4)) ; counter_01++)
                {
                        portIndex++;
                        if(portIndex == 4)
                        {
                                PRV_UTF_LOG2_MAC("\n\n%s [line #:%d]ERROR getting port index from prvTgfPortsArray[]", __FILE__, __LINE__);
                                PRV_UTF_LOG2_MAC("\n== portIndex = %d, portNumber = %d. ==\n", portIndex, portNum);
                                return(GT_FAIL);
                        }
                }

                /* switch(portNum)
                {
                  case 0 : portIndex = 0; break;
                  case 8 : portIndex = 1; break;
                  case 18: portIndex = 2; break;
                  case 23: portIndex = 3; break;

                  default: portIndex = 0;
                } */

                if(TgServer_Print_Flag_DBG_KA)
                        PRV_UTF_LOG3_MAC(" \n\nportNum-%d, PRV_TGF_PACKET_CRC_LEN = %ld, portIndex = %d\n", portNum, PRV_TGF_PACKET_CRC_LEN, portIndex);

                /* if(ulPortBurstCount[portNum] > 0x0) */
                {
                        if(ulPortWsBurstCount[portNum] > 0x0)
                        {
                                /* ---- check that Ingress port get WS traffic --------- */
                                /* get port's actual rate */
                                prvTgfCommonPortsRxRateGet(&portIndex, 1, 1000, &txRate);
                                if (0 == txRate)
                                {
                                        if(TgServer_Print_Flag_DBG_KA)
                                                printf(" \nZero actual TX rate, portNum-%d\n", portNum);
                                }
                                else
                                {
                                        if(TgServer_Print_Flag_DBG_KA)
                                                printf("\nprvTgfPortIfModeSpeedTrafficGenerate: portNum-%d actual Tx rate %ld\n",
                                                                        portNum, txRate);
                                }

                                #if 0
                                /* get port's theoretical wire-speed rate */
                                wsRate = prvTgfCommonPortWsRateGet(portIndex, PRV_TGF_PACKET_CRC_LEN);
                                if (0 == wsRate)
                                {
                                        if(TgServer_Print_Flag_DBG_04)
                                                printf(" Zero expected TX rate, portNum-%d\n", portNum);
                                }
                                else
                                {
                                        if(TgServer_Print_Flag_DBG_04)
                                                printf("prvTgfPortIfModeSpeedTrafficGenerate: portNum-%d expected Tx rate %ld\n",
                                                                        portNum, wsRate);
                                }

                                /* calculate difference between ports WS rate and input one */
                                percent = prvTgfCommonDiffInPercentCalc(wsRate, txRate);
                                rateOk = (percent <= 2) ? GT_TRUE : GT_FALSE;
                                #endif
                        }
                        else
                        {
                                txRate = 0x0;
                                if(TgServer_Print_Flag_DBG_KA)
                                        printf(" Zero actual TX rate, portNum-%d\n", portNum);
                                        /* printf(" Zero expected TX rate, portNum-%d\n", portNum); */
                        }

                        /* ++++++++++++++++++++++++++++++++++++++ */
                        /* ==  portCountersPtr.RxFrameRate        */
                        /* ++++++++++++++++++++++++++++++++++++++ */
                        /* clear printstring[] */
                        memset(printstring, 0x0, 30);

                        /* add a OM counter name to printstring[] */
                        sprintf( printstring, "%s", "RxFrameRate" );

                        /* get Hi and Low 32 bits of the port counter */
                        cntHi32Bits  = 0x0;
                        cntLow32Bits = txRate;

                        LocalBuffer[40] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                PRV_UTF_LOG0_MAC("\n");
                                sprintf(printBuffer, "%08lX%08lX", cntHi32Bits, cntLow32Bits);
                                PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring, printBuffer);
                        }
                }
                /* else */
                {
                        if(ulPortWsBurstCount[portNum] == 0x0)
                        {
                                /* ---- check that Egress port get WS traffic --------- */
                                /* get port's actual rate */
                                prvTgfCommonPortsTxRateGet(&portIndex, 1, 1000, &txRate);
                                if (0 == txRate)
                                {
                                        if(TgServer_Print_Flag_DBG_KA)
                                                printf(" \nZero actual RX rate, portNum-%d\n", portNum);
                                }
                                else
                                {
                                        if(TgServer_Print_Flag_DBG_KA)
                                                printf("prvTgfPortIfModeSpeedTrafficGenerate: portNum-%d actual Rx rate %ld\n",
                                                        portNum, txRate);
                                }

                                #if 0
                                /* get port's theoretical wire-speed rate */
                                wsRate = prvTgfCommonPortWsRateGet(portIndex, PRV_TGF_PACKET_CRC_LEN);
                                if (0 == wsRate)
                                {
                                        PRV_UTF_LOG1_MAC(" Zero expected RX rate, portNum-%d\n", portNum);
                                }
                                else {PRV_UTF_LOG1_MAC("prvTgfPortIfModeSpeedTrafficGenerate: portNum-%d expected Rx rate %ld\n",
                                                portNum, wsRate);}

                                /* calculate difference between ports WS rate and input one */
                                percent = prvTgfCommonDiffInPercentCalc(wsRate, txRate);
                                rateOk = (percent <= 2) ? GT_TRUE : GT_FALSE;
                                #endif
                        }
                        else
                        {
                                txRate = 0x0;
                                if(TgServer_Print_Flag_DBG_KA)
                                        printf(" \nZero actual RX rate, portNum-%d\n", portNum);
                                        /* printf(" Zero expected RX rate, portNum-%d\n", portNum); */
                        }

                        /* ++++++++++++++++++++++++++++++++++++++ */
                        /* ==  portCountersPtr.TxFrameRate        */
                        /* ++++++++++++++++++++++++++++++++++++++ */
                        /* clear printstring[] */
                        memset(printstring, 0x0, 30);

                        /* add a OM counter name to printstring[] */
                        sprintf( printstring, "%s", "TxFrameRate" );

                        /* get Hi and Low 32 bits of the port counter */
                        cntHi32Bits  = 0x0;
                        cntLow32Bits = txRate;

                        LocalBuffer[41] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                PRV_UTF_LOG0_MAC("\n");
                                sprintf(printBuffer, "%08lX%08lX", cntHi32Bits, cntLow32Bits);
                                PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring, printBuffer);
                        }
                } /* else if((ulPortBurstCount[portNum] > 0x0) */

                /*         Disable clear on read for MAC counters:
                        GT_FALSE - Counters are not cleared. (After read). */

                clearOnRead = GT_FALSE;

#ifdef EXMXPM_FAMILY
        status = cpssExMxPmPortMacCountersClearOnReadEnableSet(prvTgfDevNum, portNum, clearOnRead);
#else
#ifdef CHX_FAMILY
        status = cpssDxChPortMacCountersClearOnReadSet((GT_U8)prvTgfDevNum, (GT_PHYSICAL_PORT_NUM)portNum, (GT_BOOL )clearOnRead);
#endif /*CHX_FAMILY*/
#endif /*EXMXPM_FAMILY*/

                if(GT_OK != status)
                        printf("\n== %s [line #:%d] - cpssDxChPortMacCountersClearOnReadSet - port #%d FAIL ==\n", __FILE__, __LINE__, portNum);
                else
                {
                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf /*PRV_UTF_LOG0_MAC*/("\n== cpssDxChPortMacCountersClearOnReadSet - to --> %s, port #%d  ==\n",
                                                                                                                                (clearOnRead ? "TRUE" : "FALSE"), portNum);
                        }
                }

        }
        else /* if(TgServer_WS_Mode_Flag) */
        {

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.goodOctetsSent        */ /* <-- Replace between goodOctetsSent to goodOctetsRcv */
                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* clear printstring[] */
                memset(printstring, 0x0, 30);

                /* add a OM counter name to printstring[] */
                sprintf( printstring, "%s", "goodOctetsSent" );

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.goodOctetsSent.l[1]);
                cntLow32Bits = (portCountersPtr.goodOctetsSent.l[0]);

                LocalBuffer[0] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                if(TgServer_Print_Flag_DBG_KA == GT_TRUE)
                {
                        sprintf(printBuffer, "%08lX%08lX", cntHi32Bits, cntLow32Bits);
                        PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring, printBuffer);
                }

#if 0    /* Decrement ulPortOctetsCount[port_#] is not used anymore, we use Set/UnSet clear on read instead. */
                if(ulPortOctetsCount[portNum] > 0x0)
                {
                        if(TgServer_Print_Flag_DBG_03)
                        {
                                PRV_UTF_LOG2_MAC("\n\n== DEC: ulPortOctetsCount[port #%d] = %Ld\n", portNum, ulPortOctetsCount[portNum]);
                        }

                        LocalBuffer[0] -= ulPortOctetsCount[portNum];
                        ulPortOctetsCount[portNum] -= ulPortOctetsCount[portNum];
                }
#endif

                if(ulPortExtraOctetsCount[portNum] > 0x0)
                {
                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                printf("\n== ADD: ulPortExtraOctetsCount[port #%d] = %Ld\n", portNum, ulPortExtraOctetsCount[portNum]);
                        }

                        LocalBuffer[0] += ulPortExtraOctetsCount[portNum];
                }

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.goodPktsSent       */ /* <-- Replace between goodPktsSent to goodPktsRcv */
                /* ++++++++++++++++++++++++++++++++++++++ */
                /* clear printstring[] */
                memset(printstring, 0x0, 30);

                /* add a OM counter name to printstring[] */
                sprintf( printstring, "%s", "goodPktsSent" );

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.goodPktsSent.l[1]);
                cntLow32Bits = (portCountersPtr.goodPktsSent.l[0]);

                /* Decrement # of packets Tx from port, from goodPktsSent and from counter. */
                LocalBuffer[3] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

#if 0    /* Decrement ulPortBurstCount[port_#] is not used anymore, we use Set/UnSet clear on read instead. */
                if(ulPortBurstCount[portNum] > 0x0)
                {
                        if(TgServer_Print_Flag_DBG_03)
                        {
                                PRV_UTF_LOG2_MAC("\n== DEC: ulPortBurstCount[port #%d] = %Ld\n", portNum, ulPortBurstCount[portNum]);
                        }

                        LocalBuffer[3] -= ulPortBurstCount[portNum];
                        ulPortBurstCount[portNum] -= ulPortBurstCount[portNum];
                }
#endif

                if(ulPortExtraBurstCount[portNum] > 0x0)
                {
                        if(TgServer_Print_Flag_DBG_03)
                        {
                                printf("\n== ADD: ulPortExtraBurstCount[port #%d] = %Ld\n", portNum, ulPortExtraBurstCount[portNum]);
                        }

                        LocalBuffer[3] += ulPortExtraBurstCount[portNum];
                }

                if((LocalBuffer[3] < 0x0)||((uint64)LocalBuffer[3] > (uint64)0xFFFFFFFF))
                {
                        if(TgServer_Print_Flag == GT_FALSE)  /*  <---- TODO: */
                        {
                                char        printstring_L3[30];
                                memset(printstring_L3, 0x0, 30);

                                /* add a OM counter name to printstring[] */
                                sprintf( printstring_L3, "%s", "== ERROR: goodPktsSent" );

                                sprintf(printBuffer, "%08lX%08lX", (GT_U32)(((uint64)LocalBuffer[3]) >> 32), ((GT_U32)LocalBuffer[3]));
                                PRV_UTF_LOG2_MAC("\n== %30s = 0x%16.16s\n", printstring_L3, printBuffer);
                        }
                        LocalBuffer[3] = 0x0;
                }

                if(TgServer_Print_Flag_DBG_01 == GT_TRUE)  /*  <---- TODO: */
                {
                        sprintf(printBuffer, "%08lX%08lX", cntHi32Bits, cntLow32Bits);
                        PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring, printBuffer);
                        /* PRV_UTF_LOG3_MAC("== port %03d, %30s = 0x%16.16s\n", portNum, printstring, printBuffer); */
                }

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.goodOctetsRcv      */ /* <-- Replace between goodOctetsSent to goodOctetsRcv */
                /* ++++++++++++++++++++++++++++++++++++++ */
                /* clear printstring[] */
                memset(printstring, 0x0, 30);

                /* add a OM counter name to printstring[] */
                sprintf( printstring, "%s", "goodOctetsRcv" );

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.goodOctetsRcv.l[1]);
                cntLow32Bits = (portCountersPtr.goodOctetsRcv.l[0]);

                LocalBuffer[13] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                if((vfdArray[portNum][0].mode == TGF_VFD_MODE_STATIC_E)||
                   (vfdArray[portNum][1].mode == TGF_VFD_MODE_STATIC_E))
                {
                        if(GT_TRUE == TxPortTriggerFlag[portNum])
                        {
                                /* TxPortTriggerFlag[portNum] = GT_FALSE; */
                        }
                        else
                        {
                                if(TgServer_Print_Flag_DBG_02 == GT_TRUE)  /*  <---- TODO: */
                                {
                                        sprintf(printBuffer, "%08lX%08lX", (GT_U32)(((uint64)LocalBuffer[0]) >> 32),
                                                                ((GT_U32)LocalBuffer[0]));
                                        if(LocalBuffer[13] != 0x0)
                                PRV_UTF_LOG2_MAC("\n\n== DEC: goodOctetSent from goodOctetRcv[port #%d] = 0x%16.16s\n", portNum, printBuffer);
                                }

                                if(LocalBuffer[13] != 0x0) /* Check if the port got any packets first. */
                                        LocalBuffer[13] -= LocalBuffer[0];
                        }
                }

#ifdef _VXWORKS
                if((LocalBuffer[13] < 0x0)||((uint64)LocalBuffer[13] > (uint64)0xFFFFFFFF))
#else
#ifdef WIN32
                if((LocalBuffer[13] < 0x0)||((uint64)LocalBuffer[13] > 0xFFFFFFFF))
#else /* Linux */
#ifdef  LINUX
                if(((uint64)(LocalBuffer[13]) < 0x0)||((uint64)(LocalBuffer[13]) > (uint64)0xFFFFFFFF))
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */
                {
                        if(TgServer_Print_Flag == GT_FALSE)  /*  <---- TODO: */
                        {
                                char        printstring_L3[30];
                                memset(printstring_L3, 0x0, 30);

                                /* add a OM counter name to printstring[] */
                                sprintf( printstring_L3, "%s", "== ERROR: goodPktsRcv" );

                                sprintf(printBuffer, "%08lX%08lX", (GT_U32)(((uint64)LocalBuffer[13]) >> 32),
                                                        ((GT_U32)LocalBuffer[13]));
                                PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring_L3, printBuffer);
                        }
                        LocalBuffer[13] = 0x0;
                }

                if(TgServer_Print_Flag_DBG_KA == GT_TRUE)
                {
                        sprintf(printBuffer, "%08lX%08lX", (GT_U32)(((uint64)LocalBuffer[13]) >> 32),
                                                        ((GT_U32)LocalBuffer[13]));
                        PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring, printBuffer);
                }

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.badOctetsRcv       */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.badOctetsRcv.l[1]);
                cntLow32Bits = (portCountersPtr.badOctetsRcv.l[0]);

                LocalBuffer[1] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.macTransmitErr     */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.macTransmitErr.l[1]);
                cntLow32Bits = (portCountersPtr.macTransmitErr.l[0]);

                LocalBuffer[2] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.goodPktsRcv        */ /* <-- Replace between goodPktsRcv to goodPktsSent */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.goodPktsRcv.l[1]);
                cntLow32Bits = (portCountersPtr.goodPktsRcv.l[0]);

                LocalBuffer[14] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                if((vfdArray[portNum][0].mode == TGF_VFD_MODE_STATIC_E)||
                        (vfdArray[portNum][1].mode == TGF_VFD_MODE_STATIC_E))
                {
                        if(GT_TRUE == TxPortTriggerFlag[portNum])
                        {
                                TxPortTriggerFlag[portNum] = GT_FALSE;
                        }
                        else
                        {
                                if(TgServer_Print_Flag_DBG_02 == GT_TRUE)  /*  <---- TODO: */
                                {
                                        sprintf(printBuffer, "%08lX%08lX", (GT_U32)(((uint64)LocalBuffer[3]) >> 32),
                                                                ((GT_U32)LocalBuffer[3]));
                                        if(LocalBuffer[14] != 0x0)
                                PRV_UTF_LOG2_MAC("\n\n== DEC: goodPktsSent from goodPktsRcv[port #%d] = 0x%16.16s\n", portNum, printBuffer);
                                }

                                if(LocalBuffer[14] != 0x0) /* Check if the port got any packets first. */
                                        LocalBuffer[14] -= LocalBuffer[3];
                        }
                }

#ifdef _VXWORKS
                if((LocalBuffer[14] < 0x0)||((uint64)LocalBuffer[14] > (uint64)0xFFFFFFFF))
#else
#ifdef WIN32
                if((LocalBuffer[14] < 0x0)||((uint64)LocalBuffer[14] > (uint64)0xFFFFFFFF))
#else /* Linux */
#ifdef  LINUX
                if(((uint64)(LocalBuffer[14]) < 0x0)||((uint64)(LocalBuffer[14]) > (uint64)0xFFFFFFFF))
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */
                {
                        if(TgServer_Print_Flag == GT_FALSE)  /*  <---- TODO: */
                        {
                                char        printstring_L3[30];
                                memset(printstring_L3, 0x0, 30);

                                /* add a OM counter name to printstring[] */
                                sprintf( printstring_L3, "%s", "== ERROR: goodPktsRcv" );

                                sprintf(printBuffer, "%08lX%08lX", (GT_U32)(((uint64)LocalBuffer[14]) >> 32),
                                                        ((GT_U32)LocalBuffer[14]));
                                PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring_L3, printBuffer);
                        }
                        LocalBuffer[14] = 0x0;
                }

                if(TgServer_Print_Flag_DBG_KA == GT_TRUE)
                {
                        char        printstring_L3[30];
                        memset(printstring_L3, 0x0, 30);

                        /* add a OM counter name to printstring[] */
                        sprintf( printstring_L3, "%s", "goodPktsRcv" );

                        sprintf(printBuffer, "%08lX%08lX", (GT_U32)(((uint64)LocalBuffer[14]) >> 32),
                                                ((GT_U32)LocalBuffer[14]));
                        PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring_L3, printBuffer);
                }

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.badPktsRcv         */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.badPktsRcv.l[1]);
                cntLow32Bits = (portCountersPtr.badPktsRcv.l[0]);

                LocalBuffer[4] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.brdcPktsRcv        */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.brdcPktsRcv.l[1]);
                cntLow32Bits = (portCountersPtr.brdcPktsRcv.l[0]);

                LocalBuffer[5] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.mcPktsRcv          */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.mcPktsRcv.l[1]);
                cntLow32Bits = (portCountersPtr.mcPktsRcv.l[0]);

                LocalBuffer[6] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.pkts64Octets       */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.pkts64Octets.l[1]);
                cntLow32Bits = (portCountersPtr.pkts64Octets.l[0]);

                LocalBuffer[7] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.pkts65to127Octets  */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.pkts65to127Octets.l[1]);
                cntLow32Bits = (portCountersPtr.pkts65to127Octets.l[0]);

                LocalBuffer[8] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.pkts128to255Octets */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.pkts128to255Octets.l[1]);
                cntLow32Bits = (portCountersPtr.pkts128to255Octets.l[0]);

                LocalBuffer[9] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.pkts256to511Octets */
                /* ++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.pkts256to511Octets.l[1]);
                cntLow32Bits = (portCountersPtr.pkts256to511Octets.l[0]);

                LocalBuffer[10] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.pkts512to1023Octets  */
                /* ++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.pkts512to1023Octets.l[1]);
                cntLow32Bits = (portCountersPtr.pkts512to1023Octets.l[0]);

                LocalBuffer[11] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.pkts1024tomaxOoctets  */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.pkts1024tomaxOoctets.l[1]);
                cntLow32Bits = (portCountersPtr.pkts1024tomaxOoctets.l[0]);

                LocalBuffer[12] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.excessiveCollisions   */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.excessiveCollisions.l[1]);
                cntLow32Bits = (portCountersPtr.excessiveCollisions.l[0]);

                LocalBuffer[15] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.mcPktsSent            */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.mcPktsSent.l[1]);
                cntLow32Bits = (portCountersPtr.mcPktsSent.l[0]);

                LocalBuffer[16] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.brdcPktsSent          */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.brdcPktsSent.l[1]);
                cntLow32Bits = (portCountersPtr.brdcPktsSent.l[0]);

                LocalBuffer[17] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.unrecogMacCntrRcv     */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.unrecogMacCntrRcv.l[1]);
                cntLow32Bits = (portCountersPtr.unrecogMacCntrRcv.l[0]);

                LocalBuffer[18] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.fcSent                */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.fcSent.l[1]);
                cntLow32Bits = (portCountersPtr.fcSent.l[0]);

                LocalBuffer[19] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.goodFcRcv             */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.goodFcRcv.l[1]);
                cntLow32Bits = (portCountersPtr.goodFcRcv.l[0]);

                LocalBuffer[20] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.dropEvents            */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.dropEvents.l[1]);
                cntLow32Bits = (portCountersPtr.dropEvents.l[0]);

                LocalBuffer[21] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.undersizePkts         */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.undersizePkts.l[1]);
                cntLow32Bits = (portCountersPtr.undersizePkts.l[0]);

                LocalBuffer[22] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.fragmentsPkts         */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.fragmentsPkts.l[1]);
                cntLow32Bits = (portCountersPtr.fragmentsPkts.l[0]);

                LocalBuffer[23] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.oversizePkts          */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.oversizePkts.l[1]);
                cntLow32Bits = (portCountersPtr.oversizePkts.l[0]);

                LocalBuffer[24] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                if(LocalBuffer[24] != 0x0)
                {
                        char        printstring_L24[30];
                        memset(printstring_L24, 0x0, 30);

                        /* add a OM counter name to printstring[] */
                        sprintf( printstring_L24, "%s", "== oversizePkts" );
                        sprintf(printBuffer, "%08lX%08lX", (GT_U32)(((uint64)LocalBuffer[24]) >> 32),
                                                ((GT_U32)LocalBuffer[24]));
                        PRV_UTF_LOG2_MAC("\n== %30s = 0x%16.16s\n", printstring_L24, printBuffer);

                        if(ulPortShadowBurstCount[portNum] > 0x0)
                        {
                                if(TgServer_Print_Flag_DBG_KA)
                                {
                                        printf("\n== ulPortShadowBurstCount[port #%d] = %Ld\n", portNum, ulPortShadowBurstCount[portNum]);
                                }

                                /* New setup, pass Rx to Tx, Tx to Rx, copy OverSize to Tx, clear OverSize */
                                LocalBuffer[14] = LocalBuffer[24];
                        }


                        /* IF: goodPktsRcv == oversizePkts, clear goodPktsRcv, clear goodOctetsRcv. */
                        if((LocalBuffer[14] == LocalBuffer[24])&&(LocalBuffer[24] != 0x0))
                        {
                                LocalBuffer[24] = 0x0;
                                LocalBuffer[13] = 0x0;
                        }
                        /* IF: goodPktsSent == oversizePkts, clear goodPktsSent, clear goodOctetsSent */
                        /* if((LocalBuffer[3] == LocalBuffer[24])&&(LocalBuffer[24] != 0x0)) <-- old */
                        if((LocalBuffer[3] == LocalBuffer[14])&&(LocalBuffer[14] != 0x0))
                        {
                                /* pass Rx to Tx, Tx to Rx, clear Tx. Packet is counted in OverSize. */
                                LocalBuffer[3] = 0x0;
                                LocalBuffer[0] = 0x0;
                        }
                }

                /* Check if Tx to TG OverSized packets from a test port (Not the initial looped port), */
                /* because they are not counted as Oversize. */
                else if((LocalBuffer[0] != 0x0)&&(LocalBuffer[3] != 0x0))
                {
                        if((LocalBuffer[0] / LocalBuffer[3]) >= 1024)   /* <-- goodOctetsRcv <-- goodOctetsSent */
                        {
                                /* Do that only if we are reading counters on a non initial looped port. */
                                if(ulPortShadowBurstCount[portNum] == 0x0)
                                {
                                        /* Move goodPktsSent to oversizePkts, clear goodPktsRcv. */
                                        LocalBuffer[24] = LocalBuffer[3];
                                        LocalBuffer[3] = 0x0;
                                }
                        }
                }
                else if((LocalBuffer[0] == 0x0)||(LocalBuffer[3] == 0x0))
                {
                        LocalBuffer[0]  = 0x0; /* <-- goodOctetsRcv  */
                        LocalBuffer[3]  = 0x0; /* <-- goodOctetsSent */
                        LocalBuffer[24] = 0x0; /* <-- oversizePkts   */
                }

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.jabberPkts            */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.jabberPkts.l[1]);
                cntLow32Bits = (portCountersPtr.jabberPkts.l[0]);

                LocalBuffer[25] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.macRcvError           */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.macRcvError.l[1]);
                cntLow32Bits = (portCountersPtr.macRcvError.l[0]);

                LocalBuffer[26] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.badCrc             */
                /* ++++++++++++++++++++++++++++++++++++++ */
                /* clear printstring[] */
                memset(printstring, 0x0, 30);

                /* add a OM counter name to printstring[] */
                sprintf( printstring, "%s", "badCrc" );

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.badCrc.l[1]);
                cntLow32Bits = (portCountersPtr.badCrc.l[0]);

                LocalBuffer[27] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                if(TgServer_Print_Flag)
                {
                        sprintf(printBuffer, "%08lX%08lX", cntHi32Bits, cntLow32Bits);
                        PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring, printBuffer);
                }

                /* ++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.collisions         */
                /* ++++++++++++++++++++++++++++++++++++++ */
                /* clear printstring[] */
                memset(printstring, 0x0, 30);

                /* add a OM counter name to printstring[] */
                sprintf( printstring, "%s", "collisions" );

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.collisions.l[1]);
                cntLow32Bits = (portCountersPtr.collisions.l[0]);

                LocalBuffer[28] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                if(TgServer_Print_Flag)
                {
                        sprintf(printBuffer, "%08lX%08lX", cntHi32Bits, cntLow32Bits);
                        PRV_UTF_LOG2_MAC("== %30s = 0x%16.16s\n", printstring, printBuffer);
                }

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.lateCollisions        */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.lateCollisions.l[1]);
                cntLow32Bits = (portCountersPtr.lateCollisions.l[0]);

                LocalBuffer[29] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.badFcRcv              */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.badFcRcv.l[1]);
                cntLow32Bits = (portCountersPtr.badFcRcv.l[0]);

                LocalBuffer[30] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.ucPktsRcv             */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.ucPktsRcv.l[1]);
                cntLow32Bits = (portCountersPtr.ucPktsRcv.l[0]);

                LocalBuffer[31] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.ucPktsSent            */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.ucPktsSent.l[1]);
                cntLow32Bits = (portCountersPtr.ucPktsSent.l[0]);

                LocalBuffer[32] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.multiplePktsSent      */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.multiplePktsSent.l[1]);
                cntLow32Bits = (portCountersPtr.multiplePktsSent.l[0]);

                LocalBuffer[33] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* +++++++++++++++++++++++++++++++++++++++++ */
                /* ==  portCountersPtr.deferredPktsSent      */
                /* +++++++++++++++++++++++++++++++++++++++++ */

                /* get Hi and Low 32 bits of the port counter */
                cntHi32Bits  = (portCountersPtr.deferredPktsSent.l[1]);
                cntLow32Bits = (portCountersPtr.deferredPktsSent.l[0]);

                LocalBuffer[34] = (((uint64) cntHi32Bits) << 32) + ((GT_U32)cntLow32Bits);

                /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

        } /* else if(TgServer_WS_Mode_Flag) */
        /*===================================*/

#if (defined(PPC_CPU) || defined(POWERPC_CPU) || defined(MV_CPU_BE))

        /* Convert the relevant counters to Little-Endian for Client,(just 4 PPC, or BE compile). */

        /* == 01 */
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits_A  = (((uint64) LocalBuffer[0]) << 32);
        cntLow32Bits_A = ((GT_U32)LocalBuffer[0]);

        cntHi32Bits_A  = (((cntHi32Bits_A & 0xFF000000) >> 24) | ((cntHi32Bits_A & 0x00FF0000) >>  8) |
                      ((cntHi32Bits_A & 0x0000FF00) <<  8) | ((cntHi32Bits_A & 0x000000FF) << 24));

        cntLow32Bits_A = (((cntLow32Bits_A & 0xFF000000) >> 24) | ((cntLow32Bits_A & 0x00FF0000) >>  8) |
                      ((cntLow32Bits_A & 0x0000FF00) <<  8) | ((cntLow32Bits_A & 0x000000FF) << 24));

        /* ==  portCountersPtr.goodOctetsSent */
        LocalBuffer[0] = (((uint64) cntLow32Bits_A) << 32) + ((GT_U32)cntHi32Bits_A);

        /* == 02 */
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits_A  = (((uint64) LocalBuffer[3]) << 32);
        cntLow32Bits_A = ((GT_U32)LocalBuffer[3]);

        cntHi32Bits_A  = (((cntHi32Bits_A & 0xFF000000) >> 24) | ((cntHi32Bits_A & 0x00FF0000) >>  8) |
                      ((cntHi32Bits_A & 0x0000FF00) <<  8) | ((cntHi32Bits_A & 0x000000FF) << 24));

        cntLow32Bits_A = (((cntLow32Bits_A & 0xFF000000) >> 24) | ((cntLow32Bits_A & 0x00FF0000) >>  8) |
                      ((cntLow32Bits_A & 0x0000FF00) <<  8) | ((cntLow32Bits_A & 0x000000FF) << 24));

        /* ==  portCountersPtr.goodPktsSent */
        LocalBuffer[3] = (((uint64) cntLow32Bits_A) << 32) + ((GT_U32)cntHi32Bits_A);

        /* == 03 */
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits_A  = (((uint64) LocalBuffer[14]) << 32);
        cntLow32Bits_A = ((GT_U32)LocalBuffer[14]);

        cntHi32Bits_A  = (((cntHi32Bits_A & 0xFF000000) >> 24) | ((cntHi32Bits_A & 0x00FF0000) >>  8) |
                      ((cntHi32Bits_A & 0x0000FF00) <<  8) | ((cntHi32Bits_A & 0x000000FF) << 24));

        cntLow32Bits_A = (((cntLow32Bits_A & 0xFF000000) >> 24) | ((cntLow32Bits_A & 0x00FF0000) >>  8) |
                      ((cntLow32Bits_A & 0x0000FF00) <<  8) | ((cntLow32Bits_A & 0x000000FF) << 24));

        /* ==  portCountersPtr.goodPktsRcv */
        LocalBuffer[14] = (((uint64) cntLow32Bits_A) << 32) + ((GT_U32)cntHi32Bits_A);

        /* == 04 */
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits_A  = (((uint64) LocalBuffer[22]) << 32);
        cntLow32Bits_A = ((GT_U32)LocalBuffer[22]);

        cntHi32Bits_A  = (((cntHi32Bits_A & 0xFF000000) >> 24) | ((cntHi32Bits_A & 0x00FF0000) >>  8) |
                      ((cntHi32Bits_A & 0x0000FF00) <<  8) | ((cntHi32Bits_A & 0x000000FF) << 24));

        cntLow32Bits_A = (((cntLow32Bits_A & 0xFF000000) >> 24) | ((cntLow32Bits_A & 0x00FF0000) >>  8) |
                      ((cntLow32Bits_A & 0x0000FF00) <<  8) | ((cntLow32Bits_A & 0x000000FF) << 24));

        /* ==  portCountersPtr.undersizePkts */
        LocalBuffer[22] = (((uint64) cntLow32Bits_A) << 32) + ((GT_U32)cntHi32Bits_A);

        /* == 05 */
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits_A  = (((uint64) LocalBuffer[24]) << 32);
        cntLow32Bits_A = ((GT_U32)LocalBuffer[24]);

        cntHi32Bits_A  = (((cntHi32Bits_A & 0xFF000000) >> 24) | ((cntHi32Bits_A & 0x00FF0000) >>  8) |
                      ((cntHi32Bits_A & 0x0000FF00) <<  8) | ((cntHi32Bits_A & 0x000000FF) << 24));

        cntLow32Bits_A = (((cntLow32Bits_A & 0xFF000000) >> 24) | ((cntLow32Bits_A & 0x00FF0000) >>  8) |
                      ((cntLow32Bits_A & 0x0000FF00) <<  8) | ((cntLow32Bits_A & 0x000000FF) << 24));

        /* ==  portCountersPtr.oversizePkts */
        LocalBuffer[24] = (((uint64) cntLow32Bits_A) << 32) + ((GT_U32)cntHi32Bits_A);

        /* == 06 */
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits_A  = (((uint64) LocalBuffer[27]) << 32);
        cntLow32Bits_A = ((GT_U32)LocalBuffer[27]);

        cntHi32Bits_A  = (((cntHi32Bits_A & 0xFF000000) >> 24) | ((cntHi32Bits_A & 0x00FF0000) >>  8) |
                      ((cntHi32Bits_A & 0x0000FF00) <<  8) | ((cntHi32Bits_A & 0x000000FF) << 24));

        cntLow32Bits_A = (((cntLow32Bits_A & 0xFF000000) >> 24) | ((cntLow32Bits_A & 0x00FF0000) >>  8) |
                      ((cntLow32Bits_A & 0x0000FF00) <<  8) | ((cntLow32Bits_A & 0x000000FF) << 24));

        /* ==  portCountersPtr.badCrc */
        LocalBuffer[27] = (((uint64) cntLow32Bits_A) << 32) + ((GT_U32)cntHi32Bits_A);

        /* == 07 */
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits_A  = (((uint64) LocalBuffer[28]) << 32);
        cntLow32Bits_A = ((GT_U32)LocalBuffer[28]);

        cntHi32Bits_A  = (((cntHi32Bits_A & 0xFF000000) >> 24) | ((cntHi32Bits_A & 0x00FF0000) >>  8) |
                      ((cntHi32Bits_A & 0x0000FF00) <<  8) | ((cntHi32Bits_A & 0x000000FF) << 24));

        cntLow32Bits_A = (((cntLow32Bits_A & 0xFF000000) >> 24) | ((cntLow32Bits_A & 0x00FF0000) >>  8) |
                      ((cntLow32Bits_A & 0x0000FF00) <<  8) | ((cntLow32Bits_A & 0x000000FF) << 24));

        /* ==  portCountersPtr.collisions */
        LocalBuffer[28] = (((uint64) cntLow32Bits_A) << 32) + ((GT_U32)cntHi32Bits_A);


        /* == 08 */
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits_A  = (((uint64) LocalBuffer[40]) << 32);
        cntLow32Bits_A = ((GT_U32)LocalBuffer[40]);

        cntHi32Bits_A  = (((cntHi32Bits_A & 0xFF000000) >> 24) | ((cntHi32Bits_A & 0x00FF0000) >>  8) |
                      ((cntHi32Bits_A & 0x0000FF00) <<  8) | ((cntHi32Bits_A & 0x000000FF) << 24));

        cntLow32Bits_A = (((cntLow32Bits_A & 0xFF000000) >> 24) | ((cntLow32Bits_A & 0x00FF0000) >>  8) |
                      ((cntLow32Bits_A & 0x0000FF00) <<  8) | ((cntLow32Bits_A & 0x000000FF) << 24));

        /* ==  portCountersPtr.RxFrameRate */
        LocalBuffer[40] = (((uint64) cntLow32Bits_A) << 32) + ((GT_U32)cntHi32Bits_A);

        /* == 09 */
        /* get Hi and Low 32 bits of the port counter */
        cntHi32Bits_A  = (((uint64) LocalBuffer[41]) << 32);
        cntLow32Bits_A = ((GT_U32)LocalBuffer[41]);

        cntHi32Bits_A  = (((cntHi32Bits_A & 0xFF000000) >> 24) | ((cntHi32Bits_A & 0x00FF0000) >>  8) |
                      ((cntHi32Bits_A & 0x0000FF00) <<  8) | ((cntHi32Bits_A & 0x000000FF) << 24));

        cntLow32Bits_A = (((cntLow32Bits_A & 0xFF000000) >> 24) | ((cntLow32Bits_A & 0x00FF0000) >>  8) |
                      ((cntLow32Bits_A & 0x0000FF00) <<  8) | ((cntLow32Bits_A & 0x000000FF) << 24));

        /* ==  portCountersPtr.TxFrameRate */
        LocalBuffer[41] = (((uint64) cntLow32Bits_A) << 32) + ((GT_U32)cntHi32Bits_A);

#elif (defined(CPU_ARM) || defined(INTEL64_CPU) || defined(INTEL_CPU))

        /* LocalBuffer[ 0] = LocalBuffer[ 0]; */
        /* LocalBuffer[ 3] = LocalBuffer[ 3]; */
        /* LocalBuffer[14] = LocalBuffer[14]; */
        /* LocalBuffer[22] = LocalBuffer[22]; */
        /* LocalBuffer[24] = LocalBuffer[24]; */
        /* LocalBuffer[27] = LocalBuffer[27]; */
        /* LocalBuffer[28] = LocalBuffer[28]; */

        /* LocalBuffer[40] = LocalBuffer[40]; */
        /* LocalBuffer[41] = LocalBuffer[41]; */

#endif  /* #if defined(PPC_CPU) || defined(POWERPC_CPU) || defined(MV_CPU_BE)) */

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;


        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,(GT_VOID *)(LocalBuffer),sizeof(LocalBuffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "Got_counters_basic_pattern", 26) == 0)
        {
                if(GT_OK == status)
                {
                        if(TgServer_Print_Flag_DBG_KA)
                        {
                                PRV_UTF_LOG0_MAC("\n== After - status = OK.  ==\n");
                        }

                        if(TgServer_Print_Flag_DBG_02)
                        if((vfdArray[portNum][0].mode == TGF_VFD_MODE_STATIC_E)||
                           (vfdArray[portNum][1].mode == TGF_VFD_MODE_STATIC_E))
                           PRV_UTF_LOG2_MAC("\n== TotalNumTriggers[port #%d] = %ld ==", portNum, TotalNumTriggers[portNum]);

                        memset(buffer, 0, sizeof(buffer));

                        sprintf(buffer,"Send_numTriggers %04ld", TotalNumTriggers[portNum]);

                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(sockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(sockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(sockfd,buffer,sizeof(buffer));
                        if (n < 0)
                                error("ERROR writing to socket");

                        memset(buffer, 0, sizeof(buffer));

                        nLeft = (nBytes - 156);

                        n = readSocketData(sockfd, &buffer[0], nLeft);

                        if(TgServer_Print_Flag_DBG_KA)
                                printf("\n n = %d\n", n);

                        if (n < 0)
                        {
                                if(TgServer_Print_Flag)
                                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
                        }
                        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

                        if(TgServer_Print_Flag)
                        {
                                PRV_UTF_LOG0_MAC("\n== Before write - Got_numTriggers.  ==\n");
                        }

                        if (strncmp((char *)buffer, "Got_numTriggers", 15) == 0)
                        {
                                memset(buffer, 0, sizeof(buffer));
                                sprintf(buffer,"SIM_HTGetStructure_01_OK");

                                /* Check if the socket is prepared to accept data */
                                if (FD_ISSET(sockfd, &write_fd))
                                {
                                        #ifdef  WIN32
                                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                        #else /* Linux */
                                                FD_CLR(sockfd, &write_fd);
                                        #endif
                                }

                                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                                if (n < 0)
                                        error("ERROR writing to socket");
                        }
                        else
                        {
                                memset(buffer, 0, sizeof(buffer));
                                sprintf(buffer,"SIM_HTGetStructure_01_FAIL");

                                /* Check if the socket is prepared to accept data */
                                if (FD_ISSET(sockfd, &write_fd))
                                {
                                        #ifdef  WIN32
                                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                        #else /* Linux */
                                                FD_CLR(sockfd, &write_fd);
                                        #endif
                                }

                                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                                if (n < 0)
                                        error("ERROR writing to socket");

                                PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_01_FAIL. ==\n");
                        }
                } /* if(GT_OK == status) */
        }
        else  /* if (strncmp(buffer,...) */
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTGetStructure_01_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");

                PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_01_FAIL. ==\n");
        }

#if 1 /* Clear numTriggers[] only in clear_all. */
        if((vfdArray[portNum][0].mode == TGF_VFD_MODE_STATIC_E)||
           (vfdArray[portNum][1].mode == TGF_VFD_MODE_STATIC_E))
        {
                /* Clear all Triggers[] on this port. */
                numTriggers[portNum]                = 0x0;
                /* numTriggers_Arry01[portNum]        = 0x0;
                TotalNumTriggers[portNum]        = 0x0; */

                if(TgServer_Print_Flag_DBG_02)
                        PRV_UTF_LOG2_MAC("\n== Clear TotalNumTriggers[port #%d] = %ld ==", portNum, TotalNumTriggers[portNum]);
        }
#endif

        /* restore default ethertype */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);

        if(TgServer_Print_Flag_DBG_KA)
        {
                printf /*PRV_UTF_LOG0_MAC*/("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTGetStructure_01 ==");
                printf /*PRV_UTF_LOG0_MAC*/("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTGetStructure_02
*
* DESCRIPTION:
*       Run HTGetStructure- Get data from device.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTGetStructure_02(int sockfd)
{
        GT_STATUS status = GT_OK;
        CPSS_PORT_MAC_COUNTER_SET_STC portCountersPtr;
        char buffer[READ_BUFFER_SIZE];
        GT_U64 LocalBuffer[40];
           int  nBytes = READ_BUFFER_SIZE;
        int  nLeft, n = 0x0;

#ifdef _VXWORKS
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else
#ifdef WIN32
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else /* Linux */
#ifdef  LINUX
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTGetStructure_02 ==");
                PRV_UTF_LOG0_MAC("\n====================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp((char *)buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi((char)buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading DevNum");

                memset(buffer, 0, sizeof(buffer));

                sprintf((char *)buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp((char *)buffer, "counters_basic_pattern", 22) == 0)
        {
                portNum    = ctoi(buffer[25]);
                portNum_01 = ctoi(buffer[24]);
                portNum_02 = ctoi(buffer[23]);

                if(portNum < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_01 < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_02 < 0x0)
                        error("ERROR reading m_Port");

                portNum = ((portNum_02 * 100) + (portNum_01 * 10) + portNum);

                memset(buffer, 0, sizeof(buffer));

                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        if(TgServer_Print_Flag)
                PRV_UTF_LOG2_MAC("\n== prvTgfDevNum = %d, portNumber = %d. ==\n", prvTgfDevNum, portNum);

        /* Get counters */
        /* ============ */                                                /* disablePrint */
        status = prvTgfReadPortCountersEth((GT_U8)prvTgfDevNum, portNum , GT_FALSE, &portCountersPtr);

        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer,"Send_counters_Parameters_01 %ld", sizeof(CPSS_PORT_MAC_COUNTER_SET_STC));

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        memset(LocalBuffer, 0, sizeof(LocalBuffer));

#if (defined(_VXWORKS) || defined(WIN32))

        LocalBuffer[0]  = portCountersPtr.goodOctetsRcv;
        LocalBuffer[1]  = portCountersPtr.badOctetsRcv;
        LocalBuffer[2]  = portCountersPtr.macTransmitErr;
        LocalBuffer[3]  = portCountersPtr.goodPktsRcv;
        LocalBuffer[4]  = portCountersPtr.badPktsRcv;
        LocalBuffer[5]  = portCountersPtr.brdcPktsRcv;
        LocalBuffer[6]  = portCountersPtr.mcPktsRcv;
        LocalBuffer[7]  = portCountersPtr.pkts64Octets;
        LocalBuffer[8]  = portCountersPtr.pkts65to127Octets;
        LocalBuffer[9]  = portCountersPtr.pkts128to255Octets;
        LocalBuffer[10] = portCountersPtr.pkts256to511Octets;
        LocalBuffer[11] = portCountersPtr.pkts512to1023Octets;
        LocalBuffer[12] = portCountersPtr.pkts1024tomaxOoctets;
        LocalBuffer[13] = portCountersPtr.goodOctetsSent;
        LocalBuffer[14] = portCountersPtr.goodPktsSent;
        LocalBuffer[15] = portCountersPtr.excessiveCollisions;
        LocalBuffer[16] = portCountersPtr.mcPktsSent;
        LocalBuffer[17] = portCountersPtr.brdcPktsSent;
        LocalBuffer[18] = portCountersPtr.unrecogMacCntrRcv;
        LocalBuffer[19] = portCountersPtr.fcSent;
        LocalBuffer[10] = portCountersPtr.goodFcRcv;
        LocalBuffer[21] = portCountersPtr.dropEvents;
        LocalBuffer[22] = portCountersPtr.undersizePkts;
        LocalBuffer[23] = portCountersPtr.fragmentsPkts;
        LocalBuffer[24] = portCountersPtr.oversizePkts;
        LocalBuffer[25] = portCountersPtr.jabberPkts;
        LocalBuffer[26] = portCountersPtr.macRcvError;
        LocalBuffer[27] = portCountersPtr.badCrc;
        LocalBuffer[28] = portCountersPtr.collisions;
        LocalBuffer[29] = portCountersPtr.lateCollisions;
        LocalBuffer[30] = portCountersPtr.badFcRcv;
        LocalBuffer[31] = portCountersPtr.ucPktsRcv;
        LocalBuffer[32] = portCountersPtr.ucPktsSent;
        LocalBuffer[33] = portCountersPtr.multiplePktsSent;
        LocalBuffer[34] = portCountersPtr.deferredPktsSent;

#else /* Linux */
#ifdef  LINUX

        LocalBuffer[0]  = (GT_U64)portCountersPtr.goodOctetsRcv;
        LocalBuffer[1]  = (GT_U64)portCountersPtr.badOctetsRcv;
        LocalBuffer[2]  = (GT_U64)portCountersPtr.macTransmitErr;
        LocalBuffer[3]  = (GT_U64)portCountersPtr.goodPktsRcv;
        LocalBuffer[4]  = (GT_U64)portCountersPtr.badPktsRcv;
        LocalBuffer[5]  = (GT_U64)portCountersPtr.brdcPktsRcv;
        LocalBuffer[6]  = (GT_U64)portCountersPtr.mcPktsRcv;
        LocalBuffer[7]  = (GT_U64)portCountersPtr.pkts64Octets;
        LocalBuffer[8]  = (GT_U64)portCountersPtr.pkts65to127Octets;
        LocalBuffer[9]  = (GT_U64)portCountersPtr.pkts128to255Octets;
        LocalBuffer[10] = (GT_U64)portCountersPtr.pkts256to511Octets;
        LocalBuffer[11] = (GT_U64)portCountersPtr.pkts512to1023Octets;
        LocalBuffer[12] = (GT_U64)portCountersPtr.pkts1024tomaxOoctets;
        LocalBuffer[13] = (GT_U64)portCountersPtr.goodOctetsSent;
        LocalBuffer[14] = (GT_U64)portCountersPtr.goodPktsSent;
        LocalBuffer[15] = (GT_U64)portCountersPtr.excessiveCollisions;
        LocalBuffer[16] = (GT_U64)portCountersPtr.mcPktsSent;
        LocalBuffer[17] = (GT_U64)portCountersPtr.brdcPktsSent;
        LocalBuffer[18] = (GT_U64)portCountersPtr.unrecogMacCntrRcv;
        LocalBuffer[19] = (GT_U64)portCountersPtr.fcSent;
        LocalBuffer[10] = (GT_U64)portCountersPtr.goodFcRcv;
        LocalBuffer[21] = (GT_U64)portCountersPtr.dropEvents;
        LocalBuffer[22] = (GT_U64)portCountersPtr.undersizePkts;
        LocalBuffer[23] = (GT_U64)portCountersPtr.fragmentsPkts;
        LocalBuffer[24] = (GT_U64)portCountersPtr.oversizePkts;
        LocalBuffer[25] = (GT_U64)portCountersPtr.jabberPkts;
        LocalBuffer[26] = (GT_U64)portCountersPtr.macRcvError;
        LocalBuffer[27] = (GT_U64)portCountersPtr.badCrc;
        LocalBuffer[28] = (GT_U64)portCountersPtr.collisions;
        LocalBuffer[29] = (GT_U64)portCountersPtr.lateCollisions;
        LocalBuffer[30] = (GT_U64)portCountersPtr.badFcRcv;
        LocalBuffer[31] = (GT_U64)portCountersPtr.ucPktsRcv;
        LocalBuffer[32] = (GT_U64)portCountersPtr.ucPktsSent;
        LocalBuffer[33] = (GT_U64)portCountersPtr.multiplePktsSent;
        LocalBuffer[34] = (GT_U64)portCountersPtr.deferredPktsSent;

        /* LocalBuffer[40] = (GT_U64)portCountersPtr.RxFrameRate;
           LocalBuffer[41] = (GT_U64)portCountersPtr.TxFrameRate;
        */

#endif  /* Linux */
#endif /* _VXWORKS */

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,(GT_VOID *)(LocalBuffer),sizeof(LocalBuffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "Got_counters_basic_pattern", 26) == 0)
        {
                if(GT_OK == status)
                {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer,"SIM_HTGetStructure_02_OK");

                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(sockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(sockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(sockfd,buffer,sizeof(buffer));
                        if (n < 0)
                                error("ERROR writing to socket");
                }
                else
                {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer,"SIM_HTGetStructure_02_FAIL");

                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(sockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(sockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(sockfd,buffer,sizeof(buffer));
                        if (n < 0)
                                error("ERROR writing to socket");

                        PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_02_FAIL. ==\n");
                }
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTGetStructure_02_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");

                PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_02_FAIL. ==\n");
        }

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTGetStructure_02 ==");
                PRV_UTF_LOG0_MAC("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTGetStructure_03
*
* DESCRIPTION:
*       Run HTGetStructure- Get info about captured frames.
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTGetStructure_03(int sockfd)
{
        GT_STATUS status = GT_OK;
        char buffer[READ_BUFFER_SIZE];
        GT_U32 LocalBuffer[6];
           int  nBytes = READ_BUFFER_SIZE;
        int  nLeft, n = 0x0;

        FSTCaptureInfo                FSTInfo;

#ifdef _VXWORKS
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else
#ifdef WIN32
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else /* Linux */
#ifdef  LINUX
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTGetStructure_03 ==");
                PRV_UTF_LOG0_MAC("\n====================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp((char *)buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi((char)buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading DevNum");

                memset(buffer, 0, sizeof(buffer));

                sprintf((char *)buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp((char *)buffer, "capture_type_info", 17) == 0)
        {
                portNum    = ctoi(buffer[20]);
                portNum_01 = ctoi(buffer[19]);
                portNum_02 = ctoi(buffer[18]);

                if(portNum < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_01 < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_02 < 0x0)
                        error("ERROR reading m_Port");

                portNum = ((portNum_02 * 100) + (portNum_01 * 10) + portNum);

                memset(buffer, 0, sizeof(buffer));

                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        if(TgServer_Print_Flag)
                PRV_UTF_LOG2_MAC("\n== prvTgfDevNum = %d, portNumber = %d. ==\n", prvTgfDevNum, portNum);

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;


        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer,"Send_capture_type_info_01 %03ld", sizeof(LocalBuffer));

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;


        /* Send captute type to client. */
        FSTInfo.FrameInfo[0].ulIndex                         = 1;
        FSTInfo.FrameInfo[0].ucStatus                        = FST_CAPTURE_STATUS_TRIGGER;
        FSTInfo.FrameInfo[0].ucPreambleCount        = 0;
        FSTInfo.FrameInfo[0].uiLength                        = (unsigned short)(ulPortPacketSizeSave[ulTxPortNumber] + CRC_SIZE);
        FSTInfo.FrameInfo[0].ulTimestamp                = 0;

        memset(LocalBuffer, 0, sizeof(LocalBuffer));

        LocalBuffer[0] = (GT_U32)(FSTInfo.FrameInfo[0].ulIndex);
        LocalBuffer[1] = (GT_U32)(FSTInfo.FrameInfo[0].ucStatus);
        LocalBuffer[2] = (GT_U32)(FSTInfo.FrameInfo[0].ucPreambleCount);
        LocalBuffer[3] = (GT_U32)(FSTInfo.FrameInfo[0].uiLength);
        LocalBuffer[4] = (GT_U32)(FSTInfo.FrameInfo[0].ulTimestamp);

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,(GT_VOID *)(LocalBuffer),sizeof(LocalBuffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "Got_capture_type_info", 21) == 0)
        {
                if(GT_OK == status)
                {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer,"SIM_HTGetStructure_03_OK");

                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(sockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(sockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(sockfd,buffer,sizeof(buffer));
                        if (n < 0)
                                error("ERROR writing to socket");
                }
                else
                {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer,"SIM_HTGetStructure_03_FAIL");

                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(sockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(sockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(sockfd,buffer,sizeof(buffer));
                        if (n < 0)
                                error("ERROR writing to socket");

                        PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_03_FAIL. ==\n");
                }
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTGetStructure_03_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");

                PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_03_FAIL. ==\n");
        }

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTGetStructure_03 ==");
                PRV_UTF_LOG0_MAC("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTGetStructure_04
*
* DESCRIPTION:
*       Run HTGetStructure - Get captured frame data, (capture_type_info).
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTGetStructure_04(int sockfd)
{
        GT_STATUS status = GT_OK;
        char buffer[READ_BUFFER_SIZE];
        /* GT_U32 DataSize        = CAPTURE_BUFFER_SIZE; *//*1536*/
        GT_U8 maxFrames = 96;
        GT_U8 FrameNum = 0x1;
        GT_U8 LocalBuffer[0x600];
           int  nBytes = READ_BUFFER_SIZE;
        int  nLeft, n = 0x0;

#ifdef _VXWORKS
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else
#ifdef WIN32
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else /* Linux */
#ifdef  LINUX
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTGetStructure_04 ==");
                PRV_UTF_LOG0_MAC("\n====================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp((char *)buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi((char)buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading DevNum");

                memset(buffer, 0, sizeof(buffer));

                sprintf((char *)buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp((char *)buffer, "capture_data_info", 17) == 0)
        {
                portNum    = ctoi(buffer[20]);
                portNum_01 = ctoi(buffer[19]);
                portNum_02 = ctoi(buffer[18]);

                if(portNum < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_01 < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_02 < 0x0)
                        error("ERROR reading m_Port");

                portNum = ((portNum_02 * 100) + (portNum_01 * 10) + portNum);

                memset(buffer, 0, sizeof(buffer));

                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;

        if(TgServer_Print_Flag)
                PRV_UTF_LOG3_MAC("\n== prvTgfDevNum = %d, portNumber = %d, CAPTURE_BUFFER_SIZE = %d ==\n",
                                                        prvTgfDevNum, portNum, CAPTURE_BUFFER_SIZE);

        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer,"Send_capture_data_info_01 %04d", CAPTURE_BUFFER_SIZE);

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        if(TgServer_Print_Flag)
                PRV_UTF_LOG1_MAC("\n== After 'Send_capture_data_info_01' n = %d, ==\n", n);

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;


        if(FrameNum > maxFrames)
                error("ERROR FrameNum > 96");

        memset(LocalBuffer, 0, sizeof(LocalBuffer));

        LocalBuffer[0] = FrameNum;

        /* copy the packet from the shadow buffer. */
        memcpy(&LocalBuffer[1], CapturePacket, (sizeof(LocalBuffer)-1) /*1536-1*/);

        {
                if(TgServer_Print_Flag)
                {
                int counter_aa = 0x0, c_length = CAPTURE_BUFFER_SIZE;

                PRV_UTF_LOG1_MAC("\n== LocalBuffer: (length = %d)\n", c_length);
                        for(counter_aa = 0x0 ; counter_aa < c_length ; counter_aa++)
                                PRV_UTF_LOG1_MAC(" %02x", (LocalBuffer[counter_aa]));

                        PRV_UTF_LOG0_MAC("\n\n");
                }
        }

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd, (GT_VOID *)LocalBuffer, sizeof(LocalBuffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "Got_capture_data_info", 21) == 0)
        {
                if(GT_OK == status)
                {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer,"SIM_HTGetStructure_04_OK");

                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(sockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(sockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(sockfd,buffer,sizeof(buffer));
                        if (n < 0)
                                error("ERROR writing to socket");
                }
                else
                {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer,"SIM_HTGetStructure_04_FAIL");

                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(sockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(sockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(sockfd,buffer,sizeof(buffer));
                        if (n < 0)
                                error("ERROR writing to socket");

                        PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_04_FAIL. ==\n");
                }
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTGetStructure_04_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");

                PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_04_FAIL. ==\n");
        }

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTGetStructure_04 ==");
                PRV_UTF_LOG0_MAC("\n===================================\n");
        }

        return (status);
}

/******************************************************************************
* SIM_HTGetStructure_05
*
* DESCRIPTION:
*       Run HTGetStructure- Get the number of captured frames, (capture_count_info).
*
* INPUTS:
*       int -
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
******************************************************************************/
GT_STATUS SIM_HTGetStructure_05(int sockfd)
{
        GT_STATUS status = GT_OK;
        char buffer[READ_BUFFER_SIZE];
           int  nBytes = READ_BUFFER_SIZE;
        int  nLeft, n = 0x0;

#ifdef _VXWORKS
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else
#ifdef WIN32
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#else /* Linux */
#ifdef  LINUX
        int prvTgfDevNum = 0x0;
        int portNum = 0x0, portNum_01 = 0x0, portNum_02 = 0x0;
#endif  /* Linux */
#endif /* WIN32 */
#endif /* _VXWORKS */

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n====================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Start SIM_HTGetStructure_05 ==");
                PRV_UTF_LOG0_MAC("\n====================================\n");
        }

        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer,"Ready_4_Parameter_01");

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp((char *)buffer, "DevNum", 6) == 0)
        {
                prvTgfDevNum = ctoi((char)buffer[7]);
                if(prvTgfDevNum < 0x0)
                        error("ERROR reading DevNum");

                memset(buffer, 0, sizeof(buffer));

                sprintf((char *)buffer,"Got_Parameter_01");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp((char *)buffer, "capture_count_info", 18) == 0)
        {
                portNum    = ctoi(buffer[21]);
                portNum_01 = ctoi(buffer[20]);
                portNum_02 = ctoi(buffer[19]);

                if(portNum < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_01 < 0x0)
                        error("ERROR reading m_Port");

                if(portNum_02 < 0x0)
                        error("ERROR reading m_Port");

                portNum = ((portNum_02 * 100) + (portNum_01 * 10) + portNum);

                memset(buffer, 0, sizeof(buffer));

                sprintf(buffer,"Got_Parameter_02");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");
        }
        else { return(GT_FAIL); }

        if(TgServer_Print_Flag)
                PRV_UTF_LOG2_MAC("\n== prvTgfDevNum = %d, portNumber = %d. ==\n", prvTgfDevNum, portNum);

        memset(buffer, 0, sizeof(buffer));
        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "OK_Continue", 11) != 0)
                status = GT_FALSE;


        /* Send the number of packets that were captured. */
        memset(buffer, 0, sizeof(buffer));
        sprintf((char *)buffer,"Send_capture_count_info_01 %04d", 1);

        /* Check if the socket is prepared to accept data */
        if (FD_ISSET(sockfd, &write_fd))
        {
                #ifdef  WIN32
                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                #else /* Linux */
                        FD_CLR(sockfd, &write_fd);
                #endif
        }

        n = writeSocketData(sockfd,buffer,sizeof(buffer));
        if (n < 0)
                error("ERROR writing to socket");

        memset(buffer, 0, sizeof(buffer));

        nLeft = (nBytes - 156);

        n = readSocketData(sockfd, &buffer[0], nLeft);

        if(TgServer_Print_Flag_DBG_KA)
                printf("\n n = %d\n", n);

        if (n < 0)
        {
                if(TgServer_Print_Flag)
                        printf("\n%s [Line:%d] ERROR reading from socket", __FILE__, __LINE__);
        }
        else if (n == 0) {printf("\n%s [Line:%d] Got 0 bytes from socket", __FILE__, __LINE__); }

        if (strncmp(buffer, "Got_capture_count_info", 22) == 0)
        {
                if(GT_OK == status)
                {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer,"SIM_HTGetStructure_05_OK");

                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(sockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(sockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(sockfd,buffer,sizeof(buffer));
                        if (n < 0)
                                error("ERROR writing to socket");
                }
                else
                {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer,"SIM_HTGetStructure_05_FAIL");

                        /* Check if the socket is prepared to accept data */
                        if (FD_ISSET(sockfd, &write_fd))
                        {
                                #ifdef  WIN32
                                        FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                                #else /* Linux */
                                        FD_CLR(sockfd, &write_fd);
                                #endif
                        }

                        n = writeSocketData(sockfd,buffer,sizeof(buffer));
                        if (n < 0)
                                error("ERROR writing to socket");

                        PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_05_FAIL. ==\n");
                }
        }
        else
        {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer,"SIM_HTGetStructure_05_FAIL");

                /* Check if the socket is prepared to accept data */
                if (FD_ISSET(sockfd, &write_fd))
                {
                        #ifdef  WIN32
                                FD_CLR((SOCKET)sockfd, (struct fd_set*)&write_fd);
                        #else /* Linux */
                                FD_CLR(sockfd, &write_fd);
                        #endif
                }

                n = writeSocketData(sockfd,buffer,sizeof(buffer));
                if (n < 0)
                        error("ERROR writing to socket");

                PRV_UTF_LOG0_MAC("\n== SIM_HTGetStructure_05_FAIL. ==\n");
        }

        if(TgServer_Print_Flag_DBG_KA)
        {
                PRV_UTF_LOG0_MAC("\n\n===================================\n");
                printf /*PRV_UTF_LOG0_MAC*/("== Exit SIM_HTGetStructure_05 ==");
                PRV_UTF_LOG0_MAC("\n===================================\n");
        }

        return (status);
}
