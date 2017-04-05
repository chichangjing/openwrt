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

********************************************************************************
* presteraIrq.c
*
* DESCRIPTION:
*       functions in kernel mode special for prestera IRQ.
*
* DEPENDENCIES:
*
*       $Revision: 1.1.2.2 $
*******************************************************************************/
#include <linux/mm.h>
#include <linux/version.h>
#include "include/presteraIrq.h"

#define PRESTERA_MAX_INTERRUPTS 4
static struct intData          *allInterrupts[PRESTERA_MAX_INTERRUPTS];
static int numAssignedInterrupts = 0;

int short_bh_count = 0; /* BH invocation counter */

/*******************************************************************************
* prestera_tl_ISR
*
* DESCRIPTION:
*       This is the Prestera ISR reponsible for only scheduling the BH (Tasklet).
*
* INPUTS:
*       irq     - the Interrupt ReQuest number
*       dev_id  - the client data used as argument to the handler
*       regs    - holds a snapshot of the CPU context before interrupt
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       IRQ_HANDLED allways
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static irqreturn_t prestera_tl_ISR
(
    int             irq,
    void            *dev_id,
    struct pt_regs  *regs
)
{
    /* disable the interrupt vector */
    disable_irq(irq);    
    
    /* enqueue the PP task BH in the tasklet */
    tasklet_hi_schedule((struct tasklet_struct *)dev_id);
    
    short_bh_count++;

    return IRQ_HANDLED;
}


/*******************************************************************************
* mvPresteraBh
*
* DESCRIPTION:
*       This is the Prestera DSR, reponsible for only signaling of the occurence
*       of an event, any procecing will be done in the intTask (user space thread)
*       it self.
*
* INPUTS:
*       data    - the interrupt control data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void mvPresteraBh (unsigned long data)
{
    /* awake any reading process */
    up(&((struct intData *)data)->sem);
}


/*******************************************************************************
* intConnect
*
* DESCRIPTION:
*       connect and interrupt via register it at the kernel.
*
* INPUTS:
*       intVec  - the interrupt vector number to connect
*       routine - the bound routine for this interrupt vector
*
* OUTPUTS:
*       cookie  - the interrupt control data
*
* RETURNS:
*       0 on success, -1 otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
int intConnect
(
    unsigned int        intVec, 
    void                *routine, 
    struct intData      **cookie
)
{
    unsigned int            status;
    struct tasklet_struct   *tasklet;
    struct intData          *intData;
    
    tasklet = (struct tasklet_struct *)kmalloc(sizeof(struct tasklet_struct),
                                               GFP_KERNEL);
    if (NULL == tasklet)
    {
        printk("kmalloc failed\n");
        return -ENOMEM;
    }
    
    intData = (struct intData *)kmalloc(sizeof(struct intData), GFP_KERNEL);
    if (NULL == intData)
    {
        printk("kmalloc failed\n");
        return -ENOMEM;
    }
    *cookie = intData;
    
    /* The user process will wait on it */
    sema_init(&intData->sem, 0);
    intData->intVec = intVec;
    
    /* For cleanup we will need the tasklet */
    intData->tasklet = tasklet;
    
    tasklet_init(tasklet, mvPresteraBh, (unsigned long)intData);

    status = request_irq(intVec, (irq_handler_t)prestera_tl_ISR, 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
                         IRQF_DISABLED,
#else
                         SA_INTERRUPT,
#endif
                         "mvPP",
                         (void *)tasklet);
    if (status)
    {
        panic("Can not assign IRQ to PresteraDev\n");
        return (-1);
    }
    else
    {
        printk(KERN_DEBUG "PresteraOpen: connected Prestera IRQ - %d\n", intVec);
        disable_irq(intVec);
        local_irq_disable();
        if (numAssignedInterrupts < PRESTERA_MAX_INTERRUPTS)
        {
            allInterrupts[numAssignedInterrupts++] = intData;
            local_irq_enable();
        }
        else
        {
            local_irq_enable();
            printk(KERN_DEBUG "intConnect(): too many irqs assigned\n");
        }

        return (0);
    }
}

/*******************************************************************************
* initInterrupts
*
* DESCRIPTION:
*       Initialize interrupts
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 on success, -1 otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
int initInterrupts(void)
{
    return 0;
}

/*******************************************************************************
* cleanupInterrupts
*
* DESCRIPTION:
*       unbind all interrupts
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 on success, -1 otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
int cleanupInterrupts(void)
{
    while (numAssignedInterrupts > 0)
    {
        struct intData   *intData;
        intData = allInterrupts[--numAssignedInterrupts];

        disable_irq(intData->intVec);
        free_irq(intData->intVec, (void*)intData->tasklet);
        tasklet_kill(intData->tasklet);
        kfree(intData->tasklet);
        kfree(intData);
    }
    return 0;
}

