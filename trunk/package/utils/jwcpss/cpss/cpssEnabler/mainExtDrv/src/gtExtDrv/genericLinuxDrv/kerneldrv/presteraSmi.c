/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************
* presteraSmi.c
*
* DESCRIPTION:
*       functions in kernel mode special for presteraSmi.
*
* DEPENDENCIES:
*
*       $Revision: 7 $
*******************************************************************************/
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/version.h>

#include "./include/prestera.h"
#include "./include/presteraSmiGlob.h"
#include "./include/presteraSmiIrq.h"

#include <cpss/generic/cpssTypes.h>
#include "gtExtDrv/drivers/pssBspApis.h"

/*#define PRESTERA_DEBUG*/ /* debug only */
#include "./include/presteraDebug.h"

/* local variables and variables */
static int                      presteraSmi_initialized = 0;

/* Forward function/params declaratios */
extern struct semaphore *netIfIntTaskSemPtr;
struct semaphore netIfIntTaskSem;

extern unsigned long presteraSmi_eth_port_rx_DSR(unsigned char*[], unsigned long[],
                                           unsigned long, unsigned long);
extern unsigned long presteraSmi_eth_port_tx_end_DSR(unsigned char*[], unsigned long);


ssize_t presteraSmi_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t presteraSmi_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
int presteraSmi_ioctl(unsigned int, unsigned long);

/******************************************************************************/
/*********************** ethernet port FIFO section ***************************/
/******************************************************************************/
static unsigned long    *fifoPtr    = NULL;    /* the FIFO pointer               */
static unsigned long    occupied = 0;          /* occupied segments counter      */
static unsigned long    fifoSize = 0;          /* FIFO size                      */
static unsigned long    *frontPtr   = NULL;    /* FIFO front pointer             */
static unsigned long    *rearPtr    = NULL;    /* FIFO rear pointer              */
static unsigned long    *firstPtr   = NULL;    /* first element in FIFO          */
static unsigned long    *lastPtr    = NULL;    /* last element in FIFO           */

/*******************************************************************************
* ethPortFifoInit
*
* DESCRIPTION:  This routine allocates kernel memory for the FIFO. It is called
*               twice: once for the Rx and once for the Tx complete. The second
*               invocation allocates the memory and sets the FIFO pointers.
*
* INPUTS:
*       numOfElem - the number of elements (buffers and control data) needed
*                   for Rx/TxEnd
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       -ENOMEM - if there is no sufficiant memory
*       0 - on success.
*
*******************************************************************************/
long ethPortFifoInit(unsigned long numOfElem)
{

    if(numOfElem == 0)
    {
        printk("ethPortFifoInit:Err numOfElem is 0 \n");
        return -EPERM;
    }

    if (fifoSize == 0)
    {
        fifoSize = numOfElem;
    }
    else
    {
        if (fifoPtr != NULL)
        {
            printk("ethPortFifoInit: FIFIO allready initialized\n");
            return -EPERM;
        }

        fifoSize += numOfElem;

        fifoPtr = kmalloc((1 + fifoSize) * sizeof(unsigned long), GFP_KERNEL);

        if (!fifoPtr)
        {
            printk("ethPortFifoInit: Failed allocating memory for FIFO\n");
            return -ENOMEM;
        }

        frontPtr = &fifoPtr[0];
        rearPtr  = &fifoPtr[0];
        firstPtr = &fifoPtr[0];
        lastPtr  = &fifoPtr[1 + fifoSize];
    }
    
    return 0;
}


/*******************************************************************************
* ethPortFifoEnQueue
*
* DESCRIPTION:  This routine adds the new data in the FIFO front.
*
* INPUTS:
*       elem - the element data to insert in the FIFO front
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       1 - FIFO is full, insertion failed!
*       0 - on success.
*
*******************************************************************************/
long ethPortFifoEnQueue(unsigned long elem)
{
    unsigned long    *frontTmpPtr;

    frontTmpPtr = frontPtr;

    frontTmpPtr++;

    /* need to wrap ? */
    if (frontTmpPtr >= lastPtr)
    {
        frontTmpPtr = firstPtr;
    }

    if (frontTmpPtr == rearPtr)
    {
        /* fifo was full, insertion failed */
        printk("ethPortFifoInsert: fifo full err\n");
        return 1;
    }
    else
    {
        /* success, put in the data and update fifo control front ptr */
        *frontPtr = elem;
        frontPtr = frontTmpPtr;
        occupied++;
    }
    return 0;
}


/*******************************************************************************
* ethPortFifoDeQueue
*
* DESCRIPTION:  This routine gets the first data element from the FIFO rear.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       elemPtr <- pointer to update with the element got from FIFO rear
*
* RETURNS:
*       1 - FIFO is empty.
*       0 - on success.
*
*******************************************************************************/
long ethPortFifoDeQueue(unsigned long *elemPtr)
{
    /* empty FIFO */
    if (rearPtr == frontPtr)
    {
    _printk("ethPortFifoDeQueue: fifo is EMPTY\n");
        return 1;
    }

    /* get the data */
    *elemPtr = *rearPtr;
    
    rearPtr++;
    
    occupied--;

    /* need to wrap ? */
    if (rearPtr == lastPtr)
    {
        rearPtr = firstPtr;
    }

    return 0;
}


/*******************************************************************************
* ethPortFifoEnQueuePossible
*
* DESCRIPTION:  This routine returns a status if the number of elements can be
*               inserted to FIFO.
*
* INPUTS:
*       itemsCnt - the number of elements the user wants to insert
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       1 - there in`t enough space for the elements.
*       0 - there is enough space for the elements.
*
*******************************************************************************/
long ethPortFifoEnQueuePossible(unsigned long elemCnt)
{
    if (fifoSize-occupied > elemCnt)
        return 0;
    else
        return 1;
}
/******************************************************************************/
/*********************** ethernet port FIFO section end ***********************/
/******************************************************************************/
/*******************************************************************************
* dropThePacket
*
* DESCRIPTION:  This routine drops the packet.
*                                         
*
* INPUTS:
*       ctrlSeg - packet segment control
*
* OUTPUTS:
*       None
*
* RETURNS:
*       <0 - the errno to pass to user app.
*
*******************************************************************************/
static int dropThePacket(unsigned long ctrlSeg)
{
    unsigned long    readCnt;
    unsigned long    sink;
    unsigned long    *segmentPtr;
    unsigned long    queueNum;
    unsigned long    segNumber;
    
    /* get the number of segments of the packet */
    segNumber = (ETH_PORT_FIFO_ELEM_CNT_MASK & ctrlSeg);
    
    /* allocate mem to copy the segments to */
    segmentPtr = kmalloc(segNumber * sizeof(unsigned long), GFP_KERNEL);
        
    if (!segmentPtr)
    {
        printk("dropThePacket: Failed allocating memory\n");
        return -ENOMEM;
    }
    readCnt = 0;
    
    /* copy the segments from FIFO to allocated memory */
    while (readCnt < segNumber)
    {
        /* read the segment pointer */
        if (ethPortFifoDeQueue(&segmentPtr[readCnt]) != 0)
        {
            panic("dropThePacket: expecting more segments in fifo");
            return -EIO;
        }
        readCnt++;
            
        /* read the segment length, not needed by free routine */
        if (ethPortFifoDeQueue(&sink) != 0)
        {
            panic("dropThePacket: expecting more segments in fifo");
            return -EIO;
        }
    }
    /* get the queue number from control segment */
    queueNum = (ctrlSeg & ETH_PORT_FIFO_QUE_NUM_MASK) >>
                ETH_PORT_FIFO_QUE_NUM_OFFSET;

  _bspEthRxPacketFree((unsigned char**)segmentPtr, readCnt, queueNum);
    kfree(segmentPtr);
    return -ENOBUFS;
}

/*******************************************************************************
* presteraSmi_read
*
* DESCRIPTION:  This routine reads a packet from the network interface FIFO. The
*               first segment in FIFO is the control, which includes some
*               additional information regarding the packet. The next segments
*               are the packet data and for Rx packets, the segment lengths.
*
* INPUTS:
*       filp    - device descriptor
*       count   - the number of segments in the buffer
*       f_pos   - position in the file (not used)
*
* OUTPUTS:
*       buf     <- the buffer to put the packet segments in
*
* RETURNS:
*       >0 - the number of segments in the read packet.
*       0 - there are no more packets to read.
*
*******************************************************************************/
ssize_t presteraSmi_read(struct file       *filp,
                         char              *buf,
                         size_t            count,
                         loff_t            *f_pos)
{
    ssize_t                 readCnt;
    unsigned long           segNumber;
    static unsigned long    fifoData[MAX_SEG*2 + 1];
    unsigned long           wordNumber;

    readCnt = 0;
    
    /* read the first segment (control segment) from  FIFO */
    if (ethPortFifoDeQueue(&fifoData[readCnt++]) != 0)
    {
        /* fifo is empty, no more data */
        return 0;
    }
    
    /* extract from control segment needed data */
    segNumber = (ETH_PORT_FIFO_ELEM_CNT_MASK & fifoData[0]);
  _printk("presteraSmi_read, segNumber=%ld\n", segNumber);

    /* extract from the fifo the data segments */
    if (ETH_PORT_FIFO_TYPE_RX_MASK & fifoData[0])
    {
        wordNumber = (segNumber*2) + 1;

        /* validate the number of words is not too big to copy to user */
        if ((wordNumber * sizeof(unsigned long)) > count)
        {
            /* The control segments indicates too many segments for the packet, */
            /* we can not pass it to user, it is dropped and freed!             */
            return dropThePacket(fifoData[0]);
        }
        /* RX packet segments */
        while (readCnt < wordNumber)
        {
            if (ethPortFifoDeQueue(&fifoData[readCnt]) != 0)
            {
                panic("presteraSmi_read: expecting more segments in fifo");
                return -EIO;
            }
            readCnt++;
        
            if (ethPortFifoDeQueue(&fifoData[readCnt]) != 0)
            {
                panic("presteraSmi_read: expecting more segments in fifo");
                return -EIO;
            }
            readCnt++;
        }
    }
    else /* TX Complete packet segments */
    {
        wordNumber = segNumber + 1;

        /* validate the number of words is not too big to copy to user */
        if ((wordNumber * sizeof(unsigned long)) > count)
        {
            /* return the first segment back to the fifo */
            if (ethPortFifoEnQueue(fifoData[0]) != 0)
            {
                panic("presteraSmi_read: expecting that fifo is not full");
                return -EIO;
            }
            /* put the rest of the segments back to the fifo*/
            while (readCnt < wordNumber)
            {
                if (ethPortFifoDeQueue(&fifoData[readCnt]) != 0)
                {
                    panic("presteraSmi_read: expecting more segments in fifo");
                    return -EIO;
                }
                if (ethPortFifoEnQueue(fifoData[readCnt]) != 0)
                {
                    panic("presteraSmi_read: expecting that fifo is not full");
                    return -EIO;
                }
                readCnt++;
            }
            return -ENOBUFS;
        }

        while (readCnt < wordNumber)
        {
            if (ethPortFifoDeQueue(&fifoData[readCnt]) != 0)
            {
                panic("presteraSmi_read: expecting more segments in fifo");
                return -EIO;
            }
            readCnt++;
        }
    }
    
    readCnt *= sizeof(unsigned long);

    if (copy_to_user((char*)buf, (unsigned long*)fifoData, readCnt))
    {
        printk("presteraSmi_read: copy_to_user FAULT\n");
        return -EFAULT;
    }
    
    return readCnt;
}

/*******************************************************************************
* presteraSmi_write
*
* DESCRIPTION:  This routine sends the packet pointed by the segments in the buf
*               poiner over the network interface.
*
* INPUTS:
*       filp    - device descriptor
*       buf     - the buffer to be written
*       count   - the number of segments in the buffer
*       f_pos   - position in the file (not used)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       -1 - FIFO is empty.
*       <0 - on success.
*
*******************************************************************************/
ssize_t presteraSmi_write(struct file      *filp,
                          const char       *buf,
                          size_t           count,
                          loff_t           *f_pos)
{
    GT_U8       *segmentListPtr[MAX_SEG];
    GT_U32       segmentLen[MAX_SEG];
    const GT_U32       *bufPtr;
    GT_U32        txQueue;

  _printk("presteraSmi_write: START\n");
    bufPtr = (const GT_U32*)buf;

    if (count > MAX_SEG)
    {
        printk("presteraSmi_write: count too big\n");
        return -1;
    }

    /* the segment list is first in the bufPtr array */
    if (copy_from_user(segmentListPtr, &bufPtr[0], sizeof(unsigned long)*count))
    {
        printk("presteraSmi_write: copy_from_user FAULT\n");
        return -EFAULT;
    }

    /* the segment length is second in the bufPtr array */
    if (copy_from_user(segmentLen, &bufPtr[count], sizeof(unsigned long)*count))
    {
        printk("presteraSmi_write: copy_from_user FAULT\n");
        return -EFAULT;
    }
    
    /* The txQueue is in the 8 leftmost bits of segmentLen[0].
     segmentLen[0] is very small and will never get to 2^24 size. gc
     Read about it in 
     cpssEnabler/mainExtDrv/src/gtExtDrv/gtLinuxXcat/gtXcatEthPortControl.c
    */

    txQueue = (segmentLen[0] & 0xff000000) >> 24;
    segmentLen[0] &= 0x00ffffff;

#ifdef PRESTERA_DEBUG
    {
      int i;
    printk("txQueue = %ld\n", txQueue);
      printk("in presteraSmi_write, Tx ");
      for (i=0; i<count; i++)
      {
        printk("seg[%d]=0x%X (%d) ",i, 
               (int)segmentListPtr[i],(int)segmentLen[i]);
      }
      printk("\n");
    }
#endif
    
  if (_bspEthPortTxQueue(segmentListPtr, segmentLen, count, txQueue))
    {
        printk("presteraSmi_write:FAILED-BSP\n");
        return -1;
    }
  _printk("presteraSmi_write: EXIT\n");
    return 1;
}


/************************************************************************
*
*                   presteraSmi_cleanup 
*
************************************************************************/
void presteraSmi_cleanup(void)
{
    presteraSmi_initialized = 0;
}


/************************************************************************
*
*           presteraSmi_ioctl: ioctl() implementation
*
************************************************************************/
int presteraSmi_ioctl (unsigned int cmd, unsigned long arg)
{
  SMI_REG                 smiReg;
  SMI_REG_RAM_STC         smiRegRam;
  unsigned long           i;
  GT_U32                  smiRegVal;
  unsigned int            numOfTxBufs;
    unsigned int            txMode;
    CPU_CODE_TO_QUEUE_PARAM cpuCodeToQueueParam;
  MUX_PARAM               muxParam;
    RX_INIT_PARAM           rxParam;
    RX_FREE_BUF_PARAM       bufFreeParam;
    GT_STATUS               retStatus;

    if (!presteraSmi_initialized)
        return -ENODEV;
    
    /* GETTING DATA */
    switch(cmd)
    {
        case PRESTERA_SMI_IOC_ETHPORTRXBIND:
        case PRESTERA_SMI_IOC_ETHPORTTXBIND:
        case PRESTERA_SMI_IOC_ETHPORTENABLE:
        case PRESTERA_SMI_IOC_ETHPORTDISABLE:
            break;

  case PRESTERA_SMI_IOC_READREG:
  case PRESTERA_SMI_IOC_WRITEREG:
    /* read and parse user data structurr */
    if (copy_from_user(&smiReg,(SMI_REG*) arg, sizeof(SMI_REG)))
    {
      goto ioctlFault;
    }
    break;
  case PRESTERA_SMI_IOC_MUXSET:
    /* read and parse user data structurr */
    if (copy_from_user(&muxParam, (MUX_PARAM*) arg, sizeof(MUX_PARAM)))
    {
      goto ioctlFault;
    }
    break;
    
  case PRESTERA_SMI_IOC_READREGRAM:
  case PRESTERA_SMI_IOC_WRITEREGRAM:
    if (copy_from_user(&smiRegRam, (SMI_REG_RAM_STC*)arg, 
                       sizeof(SMI_REG_RAM_STC)))
    {
      goto ioctlFault;
    }
        
        case PRESTERA_SMI_IOC_TXMODE_SET:
          if (copy_from_user(&txMode,(unsigned int*) arg, sizeof(unsigned int)))
            {
                goto ioctlFault;
            }
            break;          
      
        case PRESTERA_SMI_IOC_CPUCODE_TO_QUEUE:
            /* read and parse user data structure */
            if (copy_from_user(&cpuCodeToQueueParam,
                               (CPU_CODE_TO_QUEUE_PARAM*)arg,
                               sizeof(CPU_CODE_TO_QUEUE_PARAM)))
            {
                goto ioctlFault;
            }
            break;

        case PRESTERA_SMI_IOC_ETHPORTTXINIT:
            /* read and parse user data structure */
            if (copy_from_user(&numOfTxBufs,(unsigned int*) arg, sizeof(unsigned int)))
            {
                goto ioctlFault;
            }
            break;
        case PRESTERA_SMI_IOC_ETHPORTRXINIT:
            /* read and parse user data structure */
            if (copy_from_user(&rxParam,(RX_INIT_PARAM*) arg, sizeof(RX_INIT_PARAM)))
            {
                goto ioctlFault;
            }
            break;

        case PRESTERA_SMI_IOC_ETHPORTFREEBUF:
            /* read and parse user data structure */
            if (copy_from_user(&bufFreeParam, (RX_FREE_BUF_PARAM*)arg,2*sizeof(long)))
            {
                goto ioctlFault;
            }
            if (copy_from_user(&bufFreeParam.segmentList,
                               (RX_FREE_BUF_PARAM*)(arg + (2 * sizeof(long))),
                                 bufFreeParam.numOfSegments * sizeof(char*)))
            {
                goto ioctlFault;
            }
            break;

        case PRESTERA_SMI_IOC_NETIF_WAIT:
            break;

        default:
            printk (KERN_WARNING "Unknown ioctl (%x).\n", cmd);
            break;
    }
    /* DOING SOMETHING */
    switch(cmd)
    {

    /* Note. Both bspSmiReadReg and bspSmiWriteReg perform indirect operation */

  case PRESTERA_SMI_IOC_READREG:
    /* Read the user params */
    _bspSmiReadReg(smiReg.slvId, 0, smiReg.regAddr, &smiReg.value);
    break;   
    
  case PRESTERA_SMI_IOC_WRITEREG:
    /* Write the user params */
    _bspSmiWriteReg(smiReg.slvId, 0, smiReg.regAddr, smiReg.value);
    break;
  case PRESTERA_SMI_IOC_MUXSET:
#ifdef XCAT
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31)
    /* Write the user params */
    _bspEthMuxSet(muxParam.portNum, muxParam.portType);
#endif
#endif
    break;

  case PRESTERA_SMI_IOC_MUXGET:
#ifdef XCAT
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31)
    /* Write the user params */
    _bspEthMuxGet(muxParam.portNum, (GT_U32 *)&muxParam.portType);
#endif
#endif
    break;

  case PRESTERA_SMI_IOC_READREGRAM:
    for (i = 0; i < smiRegRam.arrLen; i++, smiRegRam.addr += 4)
    {
      _bspSmiReadReg(smiRegRam.devSlvId, 0, smiRegRam.addr, &smiRegVal);
      if (copy_to_user((unsigned long*)(smiRegRam.dataArr + i),
                       &smiRegVal,
                       sizeof(smiRegVal)))
      {
        goto ioctlFault;
      }
    }
    break;
    
  case PRESTERA_SMI_IOC_WRITEREGRAM:
    for (i = 0; i < smiRegRam.arrLen; i++, smiRegRam.addr += 4)
    {
      if (copy_from_user(&smiRegVal, 
                         (unsigned long*)(smiRegRam.dataArr + i), 
                         sizeof(smiRegVal)))
      {
        goto ioctlFault;
      }
      
      _bspSmiWriteReg(smiRegRam.devSlvId, 0, smiRegRam.addr, smiRegVal);
    }
    break;
    
        case PRESTERA_SMI_IOC_ETHPORTENABLE:
    _bspEthPortEnable();
            break;

        case PRESTERA_SMI_IOC_ETHPORTDISABLE:
    _bspEthPortDisable();
            break;

        case PRESTERA_SMI_IOC_TXMODE_SET:
    _bspEthPortTxModeSet(txMode);
            break;

        case PRESTERA_SMI_IOC_CPUCODE_TO_QUEUE:
    _bspEthCpuCodeToQueue(cpuCodeToQueueParam.cpuCode, 
                                 cpuCodeToQueueParam.queue);
            break;
      
        case PRESTERA_SMI_IOC_ETHPORTTXINIT:
    _bspEthInit(1); /*?*/
    _printk("tx:_bspEthInit - done\n");
    _bspEthPortTxInit(numOfTxBufs);
            ethPortFifoInit(numOfTxBufs*2);
            break;

        case PRESTERA_SMI_IOC_ETHPORTRXINIT:
    _bspEthInit(1); /*?*/
    _printk("rx:_bspEthInit - done\n");
            
    _printk("IOCTL_ETHPORTRXINIT:rxParam.rxBufPoolPtr 0x%x\n",
            (unsigned int)rxParam.rxBufPoolPtr);
    retStatus = _bspEthPortRxInit(rxParam.rxBufPoolSize,
CPSS_TBD_BOOKMARK /* Should be defined and unified */
#ifdef XCAT
                                  (unsigned char *)_bspPhys2Virt((unsigned long)rxParam.rxBufPoolPtr),
#else
                                         rxParam.rxBufPoolPtr,
#endif
                                         rxParam.rxBufSize,
                                         rxParam.numOfRxBufsPtr, 
                                         rxParam.headerOffset,
                                         rxParam.rxQNum,
                                         rxParam.rxQbufPercentage);

            if(retStatus != GT_OK)
            {
                printk("PRESTERA_SMI_IOC_ETHPORTRXINIT,retStatus = %d \n",retStatus);
                goto ioctlFault;
            }
             ethPortFifoInit((*(rxParam.numOfRxBufsPtr)) * 3);

             break;

        case PRESTERA_SMI_IOC_ETHPORTFREEBUF:
    _bspEthRxPacketFree(bufFreeParam.segmentList,
                               bufFreeParam.numOfSegments,
                               bufFreeParam.queueNum);

            break;

        case PRESTERA_SMI_IOC_ETHPORTRXBIND:
    _bspEthInputHookAdd((BSP_RX_CALLBACK_FUNCPTR)presteraSmi_eth_port_rx_DSR);

            break;

        case PRESTERA_SMI_IOC_ETHPORTTXBIND:
    _bspEthTxCompleteHookAdd((BSP_TX_COMPLETE_CALLBACK_FUNCPTR)presteraSmi_eth_port_tx_end_DSR);
            break;

        case PRESTERA_SMI_IOC_NETIF_WAIT: 
    _printk("netIfIntTaskSemPtr:%p\n",netIfIntTaskSemPtr);

            if (down_interruptible(netIfIntTaskSemPtr))
            {
                return -ERESTARTSYS;
            }
            break;

        default:
            printk (KERN_WARNING "Unknown ioctl (%x).\n", cmd);
            break;
    }

    /* Write back to user */
    switch(cmd)
    {
        case PRESTERA_SMI_IOC_READREG:
        case PRESTERA_SMI_IOC_READREGDIRECT:
            if (copy_to_user((SMI_REG*)arg, &smiReg, sizeof(SMI_REG)))
            {
                goto ioctlFault;
            }
            break;
  case PRESTERA_SMI_IOC_MUXGET:
    if (copy_to_user((MUX_PARAM*)arg, &muxParam, sizeof(MUX_PARAM)))
    {
      goto ioctlFault;
    }
    break;      
        case PRESTERA_SMI_IOC_NETIF_WAIT:
        case PRESTERA_SMI_IOC_WRITEREG:
  case PRESTERA_SMI_IOC_MUXSET:
  case PRESTERA_SMI_IOC_READREGRAM:
  case PRESTERA_SMI_IOC_WRITEREGRAM:
        case PRESTERA_SMI_IOC_WRITEREGDIRECT:
        case PRESTERA_SMI_IOC_WRITEREGFIELD:
        case PRESTERA_SMI_IOC_ETHPORTENABLE:
        case PRESTERA_SMI_IOC_ETHPORTDISABLE:
        case PRESTERA_SMI_IOC_TXMODE_SET:
        case PRESTERA_SMI_IOC_CPUCODE_TO_QUEUE:
        case PRESTERA_SMI_IOC_ETHPORTTXINIT:
        case PRESTERA_SMI_IOC_ETHPORTFREEBUF:
        case PRESTERA_SMI_IOC_ETHPORTRXBIND:
        case PRESTERA_SMI_IOC_ETHPORTTXBIND:
        case PRESTERA_SMI_IOC_ETHPORTRXINIT:
            break;

        default:
            printk (KERN_WARNING "Unknown ioctl (%x).\n", cmd);
            break;
    }
    return 0;

ioctlFault:
    printk("IOCTL: FAULT\n");
    return -EFAULT;
}



/************************************************************************
*
*                   presteraSmi_init 
*
************************************************************************/
int presteraSmi_init(void)
{
    printk(KERN_DEBUG "presteraSmi_init %s\n", __DATE__);

    netIfIntTaskSemPtr = &netIfIntTaskSem;

    /* The netIf user process will wait on it */
    sema_init(netIfIntTaskSemPtr, 0);

    presteraSmi_initialized = 1;
    printk ("\npresteraSmi_init: Init OK!\n");

    return 0;
}
