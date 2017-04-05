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
* presteraSmiIrq.c
*
* DESCRIPTION:
*       functions in kernel mode special for presteraSmi IRQ.
*
* DEPENDENCIES:
*
*       $Revision: 2 $
*******************************************************************************/
#include <linux/mm.h>
#include <linux/version.h>
#include "include/presteraSmiIrq.h"
#include "./include/presteraSmiGlob.h"

/*#define PRESTERA_DEBUG*/ /* debug only */
#ifdef PRESTERA_DEBUG
#define DBG_PRINTK(x)   printk x
#else 
#define DBG_PRINTK(x)
#endif 

int rx_DSR = 0; /* rx DSR invocation counter */
int tx_DSR = 0; /* tx DSR invocation counter */

struct semaphore *netIfIntTaskSemPtr;  /*  netIfIntTask Signalling sema */

long ethPortFifoEnQueuePossible(unsigned long elemCnt);
long ethPortFifoEnQueue(unsigned long elem);

/*******************************************************************************
* presteraSmi_eth_port_rx_DSR
*
* DESCRIPTION:
*       This is the PresteraSMI ethernet port Rx Deferred-Service-Routine (DSR),
*       reponsible for inserting the packet segments and segment lengths to the
*       FIFO. The routine wakes the netIfintTask thread.
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
* RETURNS:ppTq
*       0 on success, or
*       1 otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
unsigned long presteraSmi_eth_port_rx_DSR(unsigned char       *segmentList[],
										  unsigned long       segmentLen[],
										  unsigned long       numOfSegments,
										  unsigned long       queueNum)
{
    unsigned long   firstElem;
    int             i;

    /* validate that there is ample space for the packet in FIFO */
    if (ethPortFifoEnQueuePossible(numOfSegments*2 + 1) != 0)
    {
        return 1;
    }

    /* set the first element (packet control) and insert to FIFO */
    firstElem = (queueNum << ETH_PORT_FIFO_QUE_NUM_OFFSET) |
                numOfSegments|
                ETH_PORT_FIFO_TYPE_RX_MASK;

    if (ethPortFifoEnQueue(firstElem) != 0)
    {
        panic("presteraSmi_eth_port_rx_DSR: ethPortFifoEnQueue failed\n");
        return 1;
    }

    /* insert all packet segments and segment lengths to FIFO */
    for (i = 0; i < numOfSegments; i++)
    {
        if (ethPortFifoEnQueue((unsigned long)segmentList[i]) != 0)
        {
            panic("presteraSmi_eth_port_rx_DSR: ethPortFifoEnQueue failed\n");
            return 1;
        }

        if (ethPortFifoEnQueue((unsigned long)segmentLen[i]) != 0)
        {
            panic("presteraSmi_eth_port_rx_DSR: ethPortFifoEnQueue failed\n");
            return 1;
        }
    }
	rx_DSR++;
    
    /* awake reading process */
    up(netIfIntTaskSemPtr);
    
    return 0;
}


/*******************************************************************************
* presteraSmi_eth_port_tx_end_DSR
*
* DESCRIPTION:
*       This is the presteraSmi ethernet port Tx Complete Deferred-Service-Routine (DSR),
*       reponsible for inserting the packet segments to the FIFO. The routine
*       wakes the presteraSmi interrupt thread.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0 on success, or
*       1 otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
unsigned long presteraSmi_eth_port_tx_end_DSR(unsigned char       *segmentList[],
											  unsigned long       numOfSegments)
{
    int             i;
    
    /* validate that there is ample space for the packet in FIFO */
    if (ethPortFifoEnQueuePossible(numOfSegments + 1) != 0)
    {
        panic("presteraSmi_eth_port_tx_end_DSR: ethPortFifoEnQueue failed\n");
        return 1;
    }

    if (ethPortFifoEnQueue(numOfSegments) != 0)
    {
        panic("presteraSmi_eth_port_tx_end_DSR: ethPortFifoEnQueue failed\n");
        return 1;
    }

    /* insert all packet segments to FIFO */
    for (i = 0; i < numOfSegments; i++)
    {
        if (ethPortFifoEnQueue((unsigned long)segmentList[i]) != 0)
        {
            panic("presteraSmi_eth_port_tx_end_DSR: ethPortFifoEnQueue failed\n");
            return 1;
        }
    }

	tx_DSR++;

	/* awake reading process */
  up(netIfIntTaskSemPtr);
    
    return 0;
}
