/*******************************************************************************
 *                Copyright 2004, MARVELL SEMICONDUCTOR, LTD.                   *
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
 * genDrvEthPortCtrl.c
 *
 * DESCRIPTION:
 *       This file includes all needed functions for receiving packet buffers
 *       from upper layer, and sending them trough the PP's NIC.
 *
 * DEPENDENCIES:
 *       None.
 *
 * FILE REVISION NUMBER:
 *       $Revision: 12 $
 *
 *******************************************************************************/
#define _GNU_SOURCE
#include <sys/ioctl.h>              /* ioctl                */
#include <errno.h>                  /* errno                */
#include <string.h>                 /* strerror             */
#include <unistd.h>                 /* write                */
#include <stdio.h>                  /* fprintf(stderr, ...) */

#ifdef XCAT_DRV
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#endif

#include <gtOs/gtOsGen.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>
#include <gtExtDrv/drivers/gtIntDrv.h>
#include "kerneldrv/include/presteraGlob.h"
#include "kerneldrv/include/presteraSmiGlob.h"

#ifdef XCAT_DRV
#define MV_HIGH_MEMORY_PREFIX 0xe0000000
#else
#define MV_HIGH_MEMORY_PREFIX 0xd0000000
#endif

static GT_STATUS _osPhy2Virt
(
    IN  unsigned long phyAddr,
    OUT unsigned char **virtAddr
)
{
  GT_UINTPTR virtAddr_temp;
  GT_UINTPTR phyAddr_temp = phyAddr;
  GT_STATUS ret;
#ifndef ARM5181_DRV
  if ((phyAddr_temp >> 24) == 0xdf)
    phyAddr_temp = (phyAddr_temp & 0x00ffffff) | 0x1f000000;

  else
#endif
    if (phyAddr_temp & MV_HIGH_MEMORY_PREFIX)
      phyAddr_temp &= ~MV_HIGH_MEMORY_PREFIX;

  ret = osPhy2Virt(phyAddr_temp, &virtAddr_temp);
  if (ret == GT_OK)
      *virtAddr = (unsigned char *)virtAddr_temp;
  return ret;
}

/* debug flag for this module, must be commented in release! */
/* #define PRESTERA_SMI_DEBUG */

#ifdef PRESTERA_SMI_DEBUG
# define DBG_INFO(x)     fprintf x
#else
# define DBG_INFO(x)
#endif

extern GT_32 gtPpFd;                /* pp file descriptor           */
#define MAX_SEG     100             /* maximum segments per packet  */

static GT_U32 netIfTid;
static GT_BOOL rawSocketMode = GT_FALSE;
GT_RawSocketRx_FUNCPTR userRawSocketRxCbFunc = NULL;
extDrvEthTxMode_ENT gTxMode = extDrvEthTxMode_asynch_E;

#ifdef GT_SMI
#if defined(XCAT_DRV) || defined(RAW_SOCKET_MV_MUX)
static GT_BOOL txModeSetDone = GT_FALSE;
#endif
#endif
GT_STATUS prvExtDrvRawSocketInit(GT_VOID);
GT_STATUS prvExtDrvRawSocketEnable(GT_VOID);
GT_STATUS prvExtDrvSendRawSocket
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
);


void gc_dump ( char *buf, int len )
{
#define isprint(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))

  int offs, i;
  int j;

  for ( offs = 0; offs < len; offs += 16 )
  {
    j = 1;
    osPrintf ( "%08x   ", offs );
    for ( i = 0; i < 16 && offs + i < len; i++ )
    {
      osPrintf ( "%02x", ( unsigned char ) buf[offs + i] );
      if ( ! ( ( j++ ) %4 ) && ( j < 16 ) )
        osPrintf ( "," );
    }
    for ( ; i < 16; i++ )
      osPrintf ( "   " );
    osPrintf ( "  " );
    for ( i = 0; i < 16 && offs + i < len; i++ )
      osPrintf ( "%c",
                   isprint ( buf[offs + i] ) ? buf[offs + i] : '.' );
    osPrintf ( "\n" );
  }
  osPrintf ( "\n" );
}
static GT_Rx_FUNCPTR userRxCbFunc = NULL;
GT_Tx_COMPLETE_FUNCPTR userTxCbFunc = NULL;

/*******************************************************************************
 * intTask
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
unsigned __TASKCONV netIfintTask(GT_VOID* unused)
{
  static unsigned long   fifoSeg[MAX_SEG*2+1]; /* FIFO segment array, filled by NetIf  */
  static unsigned char   *segmentListPtr[MAX_SEG];/* segment array used for Rx/TxEnd   */
  static unsigned long   segmentLen[MAX_SEG];  /* segment array length used for Rx     */
  unsigned long   numOfSegments;          /* number of segments in segment array       */
  unsigned long   queueNum;               /* Rx packet queue number                    */
  unsigned long   rc;                     /* return code                               */
  unsigned short  fromRxFifo;
  unsigned long   fifoCtrl;
  unsigned long   fifoIdx;
  unsigned long   i;                      /* iterator                                  */
  size_t          count;

  DBG_INFO((stderr,"netIfintTask-Start %s\n", strerror(errno)));
  /* No need to wait because not interrupts in the driver until intEnable */
  while (1)
  {
    /* Wait for interrupt */
    while (prestera_ctl (PRESTERA_SMI_IOC_NETIF_WAIT, NULL) != 0)
    {
      if (errno != EINTR)
      {
        fprintf(stderr, "netIfintTask: Interrupt wait failed: errno(%s)\n",
                strerror(errno));
        return (int)GT_FAIL;
      }
      fprintf(stderr, "netIfintTask: Interrupt wait restart: errno(%s)\n",strerror(errno));
    }
    DBG_INFO((stderr, "netIfintTask:After IOCTL_WAIT: errno(%s)\n",strerror(errno)));

    /* read all Rx packets and Tx Complete segments from the fifo */
    while ((count = read(gtPpFd, fifoSeg, sizeof(fifoSeg))) > 0)
    {
      /* set the number of segments and type of data */
      fifoCtrl = fifoSeg[0];
      numOfSegments = fifoCtrl & ETH_PORT_FIFO_ELEM_CNT_MASK;
      fromRxFifo = (fifoCtrl & ETH_PORT_FIFO_TYPE_RX_MASK) ? 1 : 0;

      if (fromRxFifo)
      {
        DBG_INFO((stderr,"Rx "));

        queueNum = (fifoCtrl & ETH_PORT_FIFO_QUE_NUM_MASK) >>
          ETH_PORT_FIFO_QUE_NUM_OFFSET;

        /* set the segment list and length from fifo data */
        for (fifoIdx = 1, i = 0; i < numOfSegments; i++)
        {
          /* update the segment list for Rx */
          _osPhy2Virt(fifoSeg[fifoIdx++],&segmentListPtr[i]);

          DBG_INFO((stderr,"netIfintTask:segmentListPtr[i]=0x%X\n",segmentListPtr[i]));

          /* and the segment length for Rx */
          segmentLen[i] = fifoSeg[fifoIdx++];

          DBG_INFO((stderr," seg[%d]= 0x%x (%d);", (int)i,
                    (int)segmentListPtr[i], (int)segmentLen[i]));
#ifdef PRESTERA_SMI_DEBUG
          gc_dump((char *)segmentListPtr[i], segmentLen[i]);
#endif
        }
        if (userRxCbFunc != NULL)
        {
            rc = userRxCbFunc(segmentListPtr, (GT_U32*)segmentLen, numOfSegments, queueNum);
            if (rc != GT_OK)
            {
                fprintf(stderr,"interruptEthPortRxSR err= %d\n", (int)rc);
            }
        }

      }
      else
      {
        DBG_INFO((stderr,"TxEnd "));

        /* set the segment list from fifo data */
        for (fifoIdx = 1, i = 0; i < numOfSegments; i++)
        {
          /* update the segment list for TxEnd */
          _osPhy2Virt(fifoSeg[fifoIdx++],&segmentListPtr[i]);
          DBG_INFO((stderr,"netIfintTask:segmentListPtr[i]=0x%X\n",segmentListPtr[i]));

          DBG_INFO((stderr," seg[%d]= 0x%x;", (int)i, (int)segmentListPtr[i]));
        }
        if (userTxCbFunc != NULL)
        {
            rc = userTxCbFunc(segmentListPtr, numOfSegments);
            if (rc != GT_OK)
            {
                fprintf(stderr,"interruptEthPortTxEndSR err= %d\n", (int)rc);
            }
        }
      }
    } /* while (read()) */
    if (count < 0)
    {
      fprintf(stderr, "read error: %s\n", strerror(errno));
    }

  }/* while (1)*/
}

/*******************************************************************************
 * extDrvNetIfIntConnect
 *
 * DESCRIPTION:
 *       Connect network interface rx/tx events.
 *
 * INPUTS:
 *       None
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_OK   - on success
 *       GT_FAIL - on error
 *
 * COMMENTS:
 *       None
 *
 *******************************************************************************/
GT_U32 extDrvNetIfIntConnect
(
 GT_VOID
 )
{
  static GT_U32 intialized = 0;

  if (rawSocketMode == GT_TRUE)
  {
    if (prvExtDrvRawSocketInit() != GT_OK)
    {
        return GT_FAIL;
    }
  }

  if (intialized)
  {
    fprintf(stderr, "extDrvNetIfIntConnect: intialized: %s\n", strerror(errno));
    return GT_FAIL;
  }
  intialized = 1;

  osTaskCreate("netIfIntTask", 0, 0xa000, netIfintTask, NULL, &netIfTid);

  if (0 == netIfTid)
  {
    fprintf(stderr, "extDrvNetIfIntConnect: taskSpawn(\"netIfIntTask\"): %s\n", strerror(errno));
    return GT_FAIL;
  }

  return  GT_OK;
}

/*******************************************************************************
 * extDrvEthPortRxInit
 *
 * DESCRIPTION: Init the ethernet port Rx interface
 *
 * INPUTS:
 *       rxBufPoolSize   - buffer pool size
 *       rxBufPool_PTR   - the address of the pool
 *       rxQNum          - the number of RX queues
 *       rxQbufPercentage- the buffer percentage dispersal for all queues
 *       rxBufSize       - the buffer requested size
 *       numOfRxBufs_PTR - number of requested buffers, and actual buffers created
 *       headerOffset    - packet header offset size
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
GT_STATUS extDrvEthPortRxInit
(
 IN GT_U32           rxBufPoolSize,
 IN GT_U8_PTR        rxBufPool_PTR,
 IN GT_U32           rxQNum,
 IN GT_U32           rxQbufPercentage[],
 IN GT_U32           rxBufSize,
 INOUT GT_U32        *numOfRxBufs_PTR,
 IN GT_U32           headerOffset
 )
{

  RX_INIT_PARAM       initParam;
  GT_U32        i;

  if (rawSocketMode == GT_TRUE)
  {
    return prvExtDrvRawSocketInit();
  }


  osVirt2Phy((GT_UINTPTR)rxBufPool_PTR, (GT_UINTPTR*)(&initParam.rxBufPoolPtr));
  initParam.rxBufPoolSize     = rxBufPoolSize;
  initParam.rxBufSize         = rxBufSize;
  initParam.numOfRxBufsPtr    = numOfRxBufs_PTR;
  initParam.headerOffset      = headerOffset;
  initParam.rxQNum            = rxQNum = 1; /* LSP support 4 RX Queues */

  for(i = 0; i < GT_MAX_RX_QUEUE_CNS;i++)
  {
    initParam.rxQbufPercentage[i] = rxQbufPercentage[i];
  }


  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTRXINIT, &initParam) != 0)
  {
    fprintf(stderr, "Fail to init ethernet port Rx, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  extDrvNetIfIntConnect();

  return GT_OK;
}

/*******************************************************************************
 * extDrvEthRawSocketModeSet
 *
 * DESCRIPTION: Sets the raw packet mode
 *
 * INPUTS:
 *       flag      - GT_TRUE  - rawSocketMode is set to GT_TRUE
 *                 - GT_FALSE - rawSocketMode is set to GT_FALSE
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
GT_STATUS extDrvEthRawSocketModeSet
(
 IN GT_BOOL          flag
 )
{
  rawSocketMode = flag;
  return GT_OK;
}

/*******************************************************************************
 * extDrvEthRawSocketModeGet
 *
 * DESCRIPTION: returns the raw packet mode
 *
 * INPUTS:
 *       None:
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_BOOL indicating raw mode is true or false.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_BOOL extDrvEthRawSocketModeGet
(
 IN GT_VOID
 )
{
  return rawSocketMode;
}

/*******************************************************************************
* extDrvLinuxModeGet
*
* DESCRIPTION:
*       Get port <portNum> Linux Mode indication (Linux Only)
*
* INPUTS:
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
* OUTPUTS:
*
* RETURNS:
*       GT_TRUE if Linux mode, or
*       GT_FALSE otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_BOOL extDrvLinuxModeGet
(
 IN GT_U32  portNum
)
{
  return GT_FALSE;
}

/*******************************************************************************
 * extDrvEthPortRxInitWithoutInterruptConnection
 *
 * DESCRIPTION: this function initialises  ethernet port Rx interface without interrupt
 *              connection
 *              The function used in multiprocess appDemo for eth port init in appDemo
 *              process. In RxTxProcess used extDrvEthPortRxInit that run
 *              RxTxInterrupt thread
 *
 *
 * INPUTS:
 *  rxBufPoolSize - buffer pool size
 *  rxBufPool_PTR - the address of the pool
 *  rxQNum      - the number of RX queues
 *  rxQbufPercentage  - the buffer percentage dispersal for all queues
 *  rxBufSize - the buffer requested size
 *  numOfRxBufs_PTR - number of requested buffers, and actual buffers created
 *  headerOffset  - packet header offset size
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
GT_STATUS extDrvEthPortRxInitWithoutInterruptConnection
(
 IN GT_U32           rxBufPoolSize,
 IN GT_U8_PTR        rxBufPool_PTR,
 IN GT_U32           rxQNum,
 IN GT_U32           rxQbufPercentage[],
 IN GT_U32           rxBufSize,
 INOUT GT_U32        *numOfRxBufs_PTR,
 IN GT_U32           headerOffset
 )
{

  RX_INIT_PARAM       initParam;
  GT_U32        i;

  osVirt2Phy((GT_UINTPTR)rxBufPool_PTR, (GT_UINTPTR*)(&initParam.rxBufPoolPtr));

  initParam.rxBufPoolSize     = rxBufPoolSize;
  initParam.rxBufSize         = rxBufSize;
  initParam.numOfRxBufsPtr    = numOfRxBufs_PTR;
  initParam.headerOffset      = headerOffset;
  initParam.rxQNum            = rxQNum = 4; /* LSP support 4 RX Queues */

  for(i = 0; i < GT_MAX_RX_QUEUE_CNS;i++)
  {
    initParam.rxQbufPercentage[i] = rxQbufPercentage[i];
  }


  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTRXINIT, &initParam) != 0)
  {
    fprintf(stderr, "Fail to init ethernet port Rx, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  /*
     extDrvNetIfIntConnect();
  */

  return GT_OK;
}


/*******************************************************************************
 * extDrvEthPortTxInit
 *
 * DESCRIPTION: Init the ethernet port Tx interface
 *
 * INPUTS:
 *       numOfTxBufs - number of requested buffers
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
GT_STATUS extDrvEthPortTxInit
(
 IN GT_U32           numOfTxBufs
 )
{

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTTXINIT, &numOfTxBufs) != 0)
  {
    fprintf(stderr, "Fail to init ethernet port Tx, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  return GT_OK;
}

/*******************************************************************************
 * extDrvEthPortEnable
 *
 * DESCRIPTION: Enable the ethernet port interface
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
GT_STATUS extDrvEthPortEnable
(
 GT_VOID
 )
{
  if (rawSocketMode == GT_TRUE)
  {
    return prvExtDrvRawSocketEnable();
  }

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTENABLE, NULL) != 0)
  {
    fprintf(stderr, "Fail to enable ethernet port, errno(%s)\n",
            strerror(errno));
    if (rawSocketMode == GT_TRUE)
        return GT_OK;
    return GT_FAIL;
  }

  return GT_OK;
}


/*******************************************************************************
 * extDrvEthPortDisable
 *
 * DESCRIPTION: Disable the ethernet port interface
 *
 * INPUTS:
 *       None
 *
 * OUTPUTS:fprintf(stderr, "extDrvEthPortEnable:start, errno(%s)\n",strerror(errno));
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
GT_STATUS extDrvEthPortDisable
(
 GT_VOID
 )
{

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTDISABLE, NULL) != 0)
  {
    fprintf(stderr, "Fail to disable ethernet port, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  return GT_OK;
}


/*******************************************************************************
 * extDrvEthPortRxCb
 *
 * DESCRIPTION:
 *       This function receives a packet from the ethernet port.
 *
 * INPUTS:
 *       segmentList     - A list of pointers to the packets segments.
 *       segmentLen      - A list of segement length.
 *       numOfSegments   - The number of segment in segment list.
 *       queueNum        - the received queue number
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_TRUE if successful, or
 *       GT_FALSE otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthPortRxCb
(
 IN GT_U8_PTR        segmentList[],
 IN GT_U32           segmentLen[],
 IN GT_U32           numOfSegments,
 IN GT_U32           queueNum
 )
{
  /* this routine is NOT used as ISR for Linux */
  fprintf(stderr, "extDrvEthPortRxCb, errno(%s)\n",strerror(errno));
  return GT_FAIL;
}


/******************************************************************************
 * extDrvEthPortTxEndCb
 *
 * DESCRIPTION:
 *       This function receives a packet from the ethernet port.
 *
 * INPUTS:
 *       segmentList     - A list of pointers to the packets segments.
 *       numOfSegments   - The number of segment in segment list.
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
GT_STATUS extDrvEthPortTxEndCb
(
 IN GT_U8_PTR        segmentList[],
 IN GT_U32           numOfSegments
 )
{
  /* this routine is NOT used as ISR for Linux */
  fprintf(stderr, "extDrvEthPortTxEndCb, errno(%s)\n",strerror(errno));
  return GT_FAIL;
}


/*******************************************************************************
* extDrvEthPortTx
*
* DESCRIPTION:
*       This function transmits a packet through the ethernet port in salsa.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segement length.
*       numOfSegments   - The number of segment in segment list.
*       txQueue         - The TX queue.
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
GT_STATUS extDrvEthPortTx
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
)
{

  unsigned long  segArr[MAX_SEG*2];
  GT_UINTPTR     phyAddr;
  GT_U32       segLenOffset;
  GT_U32       i;

  if (numOfSegments > MAX_SEG)
    return GT_FAIL;

  if (rawSocketMode == GT_TRUE)
    return prvExtDrvSendRawSocket(segmentList, segmentLen, numOfSegments, txQueue);

  /* copy the segments and lengths to the segArr */
  for (segLenOffset = numOfSegments, i = 0 ;
       i < numOfSegments; i++, segLenOffset++)
  {
    osVirt2Phy((GT_UINTPTR)segmentList[i],&phyAddr);
    segArr[i] = phyAddr;
    segArr[segLenOffset] = segmentLen[i];

    /*
      We need a way to trasfer the txQueue number through the "write" system
      call to "presteraSmi_write" function.
      Since we cannot add another parameter to the "write" system call,
      and we don't want to waste ioctl, we embed the txQueue in the 8
      leftmost bits of segmentLen[0]. segmentLen[0] is always very
      small and will never get to 2^24 size. gc
    */

    if (i == 0)
      segArr[segLenOffset] |= (txQueue << 24);
  }

  /* and send the packet */
  if (write(gtPpFd, segArr, numOfSegments) > 0)
    return GT_OK;
  else
  {
    fprintf(stderr, "extDrvEthPortTx:FAIL to send packet, errno(%s)\n",strerror(errno));
    return GT_FAIL;
  }

  return GT_OK;
}

/*******************************************************************************
* extDrvEthPortTxModeSet
*
* DESCRIPTION: Set the ethernet port tx mode
*
* INPUTS:
*       txMode - extDrvEthTxMode_asynch_E - don't wait for TX done - free packet
*                when interrupt received
*                extDrvEthTxMode_asynch_E - wait to TX done and free packet
*                immediately
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise
*       GT_NOT_SUPPORTED
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortTxModeSet
(
    extDrvEthTxMode_ENT    txMode
)
{
#ifdef GT_SMI
#if defined(XCAT_DRV) || defined(RAW_SOCKET_MV_MUX)
  unsigned long txMode_temp = (unsigned long)txMode;

  gTxMode = txMode;

  if (rawSocketMode == GT_TRUE)
  {
    if (txModeSetDone == GT_TRUE)
      return GT_OK; /* allow just one call to lsp in raw sockets */

    txModeSetDone = GT_TRUE;
  }

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_TXMODE_SET, &txMode_temp) != 0)
  {
    fprintf(stderr, "Fail to set txMode, errno(%s)\n",
            strerror(errno));
    if (rawSocketMode == GT_TRUE)
        return GT_OK;
    return GT_FAIL;
  }
  return GT_OK;

#endif
    return (txMode == extDrvEthTxMode_asynch_E) ? GT_OK : GT_NOT_SUPPORTED;
#else
    return GT_NOT_SUPPORTED;
#endif
}

/*******************************************************************************
 * extDrvEthInputHookAdd
 *
 * DESCRIPTION:
 *       This bind the user Rx callback
 *
 * INPUTS:
 *       userRxFunc - the user Rx callbak function
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
GT_STATUS extDrvEthInputHookAdd
(
 IN GT_Rx_FUNCPTR    userRxFunc
 )
{
  if (userRxFunc == NULL)
  {
      return GT_FAIL;
  }
  userRxCbFunc = userRxFunc;
  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTRXBIND, NULL) != 0)
  {
    fprintf(stderr, "Fail to bind ethernet port Rx callback, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  return GT_OK;
}

/*******************************************************************************
 * extDrvEthRawSocketRxHookAdd
 *
 * DESCRIPTION:
 *       bind the raw packet Rx callback
 *
 * INPUTS:
 *       userRawRxFunc - the user ra packet Rx callbak function
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
GT_STATUS extDrvEthRawSocketRxHookAdd
(
 IN GT_RawSocketRx_FUNCPTR    userRxFunc
 )
{
  if (userRxFunc == NULL)
  {
      return GT_FAIL;
  }
  userRawSocketRxCbFunc = userRxFunc;
  return GT_OK;
}


/*******************************************************************************
 * extDrvEthTxCompleteHookAdd
 *
 * DESCRIPTION:
 *       This bind the user Tx complete callback
 *
 * INPUTS:
 *       userTxFunc - the user Tx callback function
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
GT_STATUS extDrvEthTxCompleteHookAdd
(
 IN GT_Tx_COMPLETE_FUNCPTR userTxFunc
 )
{
  if (userTxFunc == NULL)
  {
      return GT_FAIL;
  }
  userTxCbFunc = userTxFunc;

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_ETHPORTTXBIND, NULL) != 0)
  {
    fprintf(stderr, "Fail to bind ethernet port Tx callback, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  return GT_OK;
}


extern GT_UINTPTR baseVirtAddr;
extern GT_U32 dmaSize;
/*******************************************************************************
 * extDrvEthRxPacketFree
 *
 * DESCRIPTION:
 *       This routine frees the recievd Rx buffer.
 *
 * INPUTS:
 *       segmentList     - A list of pointers to the packets segments.
 *       numOfSegments   - The number of segment in segment list.
 *       queueNum        - receive queue number
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
GT_STATUS extDrvEthRxPacketFree
(
 IN GT_U8_PTR        segmentList[],
 IN GT_U32           numOfSegments,
 IN GT_U32           queueNum
 )
{

  GT_U32   i;
  RX_FREE_BUF_PARAM   buffFreeParam;

  if ( ((GT_UINTPTR)segmentList[0] < baseVirtAddr) ||
    ((GT_UINTPTR)segmentList[0] >= baseVirtAddr + dmaSize) )
  {
    /* it's a malloc'ed raw packet */
    osFree(segmentList[0] - 4);  /* back to osMalloc address */
    return GT_OK;
  }

  for(i = 0; i < numOfSegments; i++)
  {
    osVirt2Phy((GT_UINTPTR)segmentList[i], (GT_UINTPTR*)&buffFreeParam.segmentList[i]);
    /*
    DBG_INFO((stderr, "extDrvEthRxPacketFree:buffFreeParam.segmentListPtr[i]=%p\n",
              buffFreeParam.segmentList[i]));
    */
  }
  buffFreeParam.numOfSegments  = numOfSegments;
  buffFreeParam.queueNum       = queueNum;

  if (prestera_ctl(PRESTERA_SMI_IOC_ETHPORTFREEBUF, &buffFreeParam) != 0)
  {
    fprintf(stderr, "Fail to free ethernet port buff, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  return GT_OK;
}

/*******************************************************************************
* extDrvEthCpuCodeToQueue
*
* DESCRIPTION:
*       Binds DSA CPU code to RX queue.
*
* INPUTS:
*       dsaCpuCode - DSA CPU code
*       rxQueue    -  rx queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on fail
*       GT_NOT_SUPPORTED    - the API is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthCpuCodeToQueue
(
    IN GT_U32  dsaCpuCode,
    IN GT_U8   rxQueue
)
{
  CPU_CODE_TO_QUEUE_PARAM cpuCodeToQueueParam;

  cpuCodeToQueueParam.cpuCode = dsaCpuCode;
  cpuCodeToQueueParam.queue   = rxQueue;

  if (ioctl(gtPpFd, PRESTERA_SMI_IOC_CPUCODE_TO_QUEUE, &cpuCodeToQueueParam) != 0)
  {
    fprintf(stderr, "Fail to set CpuCode to Queue, errno(%s)\n",
            strerror(errno));
    return GT_FAIL;
  }

  return GT_OK;
}

/*******************************************************************************
* extDrvEthPrePendTwoBytesHeaderSet
*
* DESCRIPTION:
*       Enables/Disable pre-pending a two-byte header to all packets arriving
*       to the CPU.
*
* INPUTS:
*        enable    - GT_TRUE  - Two-byte header is pre-pended to packets
*                               arriving to the CPU.
*                    GT_FALSE - Two-byte header is not pre-pended to packets
*                               arriving to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on fail
*       GT_NOT_SUPPORTED    - the API is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPrePendTwoBytesHeaderSet
(
    IN GT_BOOL enable
)
{
    /*CPSS_TBD_BOOKMARK*/
    /* add support */
    return GT_OK;
}
