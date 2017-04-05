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
* presteraSmiGlob.h
*
* DESCRIPTION:
*       This file includes the declaration of the struct we want to send to kernel mode,
*       from user mode.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __PRESTERA_SMI_GLOB__
#define __PRESTERA_SMI_GLOB__

/************************ ethrnet port definitions ****************************/
/* first 32 bit in Ethrnet Port FIFO (Rx and Tx) bits:                        */
/*  0..6    - number of elements    (values of: 0..127)                       */
/*  7..9    - queue number          (0..7)                                    */
/*  10..30  - reserved                                                        */
/*  31      - TxEnd or Rx flag      (0..1)                                    */
#define ETH_PORT_FIFO_ELEM_CNT_MASK         0x0000007F
#define ETH_PORT_FIFO_QUE_NUM_OFFSET        7
#define ETH_PORT_FIFO_QUE_NUM_MASK          0x00000380
#define ETH_PORT_FIFO_TYPE_RX_MASK          (1<<31)
#define MAX_SEG                             100

#define GT_MAX_RX_QUEUE_CNS     8   /* maximum number of RX queues */

typedef struct SMI_REG_STCT
{
    uint32_t    slvId;
    uint32_t    regAddr;
    uint32_t    value;
} SMI_REG;

typedef struct SMI_REG_RAM_STCT
{
    uint32_t        devSlvId;
    uint32_t        addr;
    uint32_t        *dataArr;
    uint32_t        arrLen;
} SMI_REG_RAM_STC;

typedef struct SMI_REG_VEC_STCT
{
    unsigned long   devSlvId;
    unsigned long   *addrArr;
    unsigned long   *dataArr;
    unsigned long   arrLen;
} SMI_REG_VEC_STC;

typedef struct write_smi_field_STCT
{
    unsigned long slvId;
    unsigned long regAddr;
    unsigned long mask;
    unsigned long value;
} write_smi_reg_field_STC;

typedef struct rx_init
{
    uint32_t            rxBufPoolSize;
    unsigned char       *rxBufPoolPtr;
    uint32_t            rxBufSize;
    uint32_t            *numOfRxBufsPtr;
    uint32_t            headerOffset;
	uint32_t            rxQNum;
	uint32_t            rxQbufPercentage[GT_MAX_RX_QUEUE_CNS];	
} RX_INIT_PARAM;
    

typedef struct rx_buff_free
{
    unsigned long       numOfSegments;
    unsigned long       queueNum;
	unsigned char*      segmentList[MAX_SEG];
} RX_FREE_BUF_PARAM;

typedef struct cpu_code_to_queue_param
{
  unsigned long cpuCode;
  unsigned char queue;
} CPU_CODE_TO_QUEUE_PARAM;

/*
 * Typedef: enum bspEthNetPortType_ENT
 *
 * Description:
 *      This type defines types of switch ports for BSP ETH driver.
 *
 * Fields:
 *      bspEthNetPortType_cpss_E   - packets forwarded to CPSS
 *      bspEthNetPortType_raw_E    - packets forwarded to OS (without dsa removal)
 *      bspEthNetPortType_linux_E  - packets forwarded to OS (with dsa removal)
 *
 * Note:
 *      The enum has to be compatible with MV_NET_OWN and ap_packet.c
 *
 */
typedef enum
{

    /* cpss = the packet is sent directly to cpss */
    bspEthNetPortType_cpss_E    = 0,

    /* raw = the packet is sent to the network stack WITHOUT removing the dsa */
    bspEthNetPortType_raw_E    = 1,

    /* linux = the packet is sent to the network stack AFTER removing the dsa */
    bspEthNetPortType_linux_E    = 2,

    bspEthNetPortType_numOfTypes
} bspEthNetPortType_ENT;


typedef struct mux_param
{
  unsigned long portNum;
  bspEthNetPortType_ENT portType;
} MUX_PARAM;



/************************ IOCTLs ****************************/
#define PRESTERA_SMI_IOC_MAGIC 's'
#define PRESTERA_SMI_IOC_WRITEREG          _IOWR(PRESTERA_SMI_IOC_MAGIC, 0, SMI_REG)
#define PRESTERA_SMI_IOC_READREG           _IOWR(PRESTERA_SMI_IOC_MAGIC, 1, SMI_REG)
#define PRESTERA_SMI_IOC_WRITEREGDIRECT    _IOWR(PRESTERA_SMI_IOC_MAGIC, 2, SMI_REG)
#define PRESTERA_SMI_IOC_READREGDIRECT     _IOWR(PRESTERA_SMI_IOC_MAGIC, 3, SMI_REG)
#define PRESTERA_SMI_IOC_WRITEREGFIELD     _IOWR(PRESTERA_SMI_IOC_MAGIC, 7, write_smi_reg_field_STC)
#define PRESTERA_SMI_IOC_READREGRAM        _IOWR(PRESTERA_SMI_IOC_MAGIC, 8, SMI_REG_RAM_STC)
#define PRESTERA_SMI_IOC_WRITEREGRAM       _IOWR(PRESTERA_SMI_IOC_MAGIC, 9, SMI_REG_RAM_STC)
#define PRESTERA_SMI_IOC_READREGVEC        _IOWR(PRESTERA_SMI_IOC_MAGIC, 10, SMI_REG_VEC_STC)
#define PRESTERA_SMI_IOC_WRITEREGVEC       _IOWR(PRESTERA_SMI_IOC_MAGIC, 11, SMI_REG_VEC_STC)
#define PRESTERA_SMI_IOC_ETHPORTENABLE     _IO(PRESTERA_SMI_IOC_MAGIC,   13)
#define PRESTERA_SMI_IOC_ETHPORTDISABLE    _IO(PRESTERA_SMI_IOC_MAGIC,   14)
#define PRESTERA_SMI_IOC_ETHPORTRXINIT     _IOWR(PRESTERA_SMI_IOC_MAGIC, 15, RX_INIT_PARAM)
#define PRESTERA_SMI_IOC_ETHPORTTXINIT     _IOW(PRESTERA_SMI_IOC_MAGIC,  16, long)
#define PRESTERA_SMI_IOC_ETHPORTFREEBUF    _IOW(PRESTERA_SMI_IOC_MAGIC,  17, RX_FREE_BUF_PARAM)
#define PRESTERA_SMI_IOC_ETHPORTRXBIND     _IO(PRESTERA_SMI_IOC_MAGIC,   18)
#define PRESTERA_SMI_IOC_ETHPORTTXBIND     _IO(PRESTERA_SMI_IOC_MAGIC,   19)
#define PRESTERA_SMI_IOC_NETIF_WAIT		     _IO(PRESTERA_SMI_IOC_MAGIC,   20)   
#define PRESTERA_SMI_IOC_TXMODE_SET		     _IOW(PRESTERA_SMI_IOC_MAGIC,  21, long) 
#define PRESTERA_SMI_IOC_CPUCODE_TO_QUEUE  _IOW(PRESTERA_SMI_IOC_MAGIC,  22, long) 
#define PRESTERA_SMI_IOC_MUXSET            _IOW(PRESTERA_SMI_IOC_MAGIC,  23, long) 
#define PRESTERA_SMI_IOC_MUXGET            _IOW(PRESTERA_SMI_IOC_MAGIC,  24, long) 


#endif /* __PRESTERA_SMI_GLOB__ */
