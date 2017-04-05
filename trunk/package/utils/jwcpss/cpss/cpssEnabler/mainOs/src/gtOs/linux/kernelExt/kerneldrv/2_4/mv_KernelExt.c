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
* mvKernelExt.c
*
* DESCRIPTION:
*       functions in kernel mode special for mainOs.
*
* DEPENDENCIES:
*       mvKernelExt.h
*       mvKernelExt_Sem.c
*
*       $Revision: 5.*******************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/threads.h>

#ifdef  MVKERNELEXT_SYSCALLS
#  include <asm/unistd.h>
#endif

#include "mv_KernelExt.h"

/* local variables and variables */
static int                  mvKernelExt_major = MVKERNELEXT_MAJOR;
static int                  mvKernelExt_initialized = 0;
static int                  mvKernelExt_opened = 0;

MODULE_PARM(mvKernelExt_major,"i");
MODULE_AUTHOR("Marvell Semi.");
MODULE_LICENSE("GPL");

/************************************************************************
 * mvKernelExt_read: this should be the read device function, for now in
 * current mvKernelExt driver implemention it does nothing
 ************************************************************************/
static ssize_t mvKernelExt_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    return -ERESTARTSYS;
}

/************************************************************************
 *
 * mvKernelExt_write: this should be the write device function, for now in
 * current mvKernelExt driver implemention it does nothing
 *
 ************************************************************************/
static ssize_t mvKernelExt_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    return -ERESTARTSYS;
}

static loff_t mvKernelExt_lseek(struct file *filp, loff_t off, int whence)
{
    return -ERESTARTSYS;
}


#include "../common/mv_KernelExt.c"
#include "../common/mv_KernelExtSem.c"
#include "../common/mv_KernelExtMsgQ.c"


/************************************************************************
*
* waitqueue support functions
*
* These functions required to suspend thread till some event occurs
************************************************************************/

/*******************************************************************************
* mv_waitqueue_init
*
* DESCRIPTION:
*       Initialize wait queue structure
*
* INPUTS:
*       queue  - pointer to wait queue structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static void mv_waitqueue_init(
        mv_waitqueue_t* queue
)
{
    memset(queue, 0, sizeof(*queue));
}

/*******************************************************************************
* mv_waitqueue_cleanup
*
* DESCRIPTION:
*       Cleanup wait queue structure
*
* INPUTS:
*       queue  - pointer to wait queue structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static void mv_waitqueue_cleanup(
        mv_waitqueue_t* queue
)
{
    memset(queue, 0, sizeof(*queue));
}

/*******************************************************************************
* mv_waitqueue_add
*
* DESCRIPTION:
*       add task to wait queue
*
* INPUTS:
*       queue  - pointer to wait queue structure
*       tsk    - pointer to task structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Interrupts must be disabled when this function called
*
*******************************************************************************/
static void mv_waitqueue_add(
        mv_waitqueue_t* queue,
        struct mv_task* tsk
)
{
    tsk->waitqueue = queue;
    tsk->wait_next = NULL;
    if (queue->first == NULL)
    {
        queue->first = tsk;
    } else {
        queue->last->wait_next = tsk;
    }
    queue->last = tsk;
}

/*******************************************************************************
* mv_waitqueue_wake_first
*
* DESCRIPTION:
*       wakeup first task waiting in queue
*
* INPUTS:
*       queue  - pointer to wait queue structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Interrupts must be disabled when this function called
*
*******************************************************************************/
static void mv_waitqueue_wake_first(
        mv_waitqueue_t* queue
)
{
    struct mv_task *p;
    
    p = queue->first;

    if (!p)
        return;

    if (p->tasklockflag == 1)
        p->task->state &= ~TASK_INTERRUPTIBLE;
    else if (p->tasklockflag == 2)
        wake_up_process(p->task);
    p->tasklockflag = 0;


    p->waitqueue = NULL;
    queue->first = p->wait_next;
    if (queue->first == NULL)
        queue->last = NULL;
}

/*******************************************************************************
* mv_waitqueue_wake_all
*
* DESCRIPTION:
*       wakeup all tasks waiting in queue
*
* INPUTS:
*       queue  - pointer to wait queue structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Interrupts must be disabled when this function called
*
*******************************************************************************/
static void mv_waitqueue_wake_all(
        mv_waitqueue_t* queue
)
{
    struct mv_task *p;
    
    p = queue->first;

    if (!p)
        return;

    while (p)
    {
        if (p->tasklockflag == 1)
            p->task->state &= ~TASK_INTERRUPTIBLE;
        else if (p->tasklockflag == 2)
            wake_up_process(p->task);
        p->tasklockflag = 0;

        p->waitqueue = NULL;
        p = p->wait_next;
    }

    queue->first = queue->last = NULL;
}

/*******************************************************************************
* mv_delete_from_waitqueue
*
* DESCRIPTION:
*       remove task from wait queue
*
* INPUTS:
*       tsk    - pointer to task structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Interrupts must be disabled when this function called
*
*******************************************************************************/
static void mv_delete_from_waitqueue(
        struct mv_task* tsk
)
{
    mv_waitqueue_t* queue;
    struct mv_task* p;

    queue = tsk->waitqueue;
    if (!queue)
        return;

    tsk->waitqueue = NULL;

    if (queue->first == tsk)
    {
        queue->first = tsk->wait_next;
        if (queue->first == NULL)
            queue->last = NULL;
        return;
    }

    for (p = queue->first; p; p = p->wait_next)
    {
        if (p->wait_next == tsk)
        {
            p->wait_next = tsk->wait_next;
            if (p->wait_next == NULL)
                queue->last = p;
            return;
        }
    }

    if (p->tasklockflag == 1)
        p->task->state &= ~TASK_INTERRUPTIBLE;
    else if (p->tasklockflag == 2)
        wake_up_process(p->task);
    p->tasklockflag = 0;

}

/*******************************************************************************
* mv_do_short_wait_on_queue
*
* DESCRIPTION:
*       Suspend a task on wait queue for a short period.
*       Function has a best performance in a cost of CPU usage
*       This is useful for mutual exclusion semaphores
*
* INPUTS:
*       queue  - pointer to wait queue structure
*       tsk    - pointer to task structure
*       owner  - resourse owner
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Zero if wait successful
*       Non zero if wait interrupted (signal caught)
*
* COMMENTS:
*       Interrupts must be disabled when this function called
*
*******************************************************************************/
static int mv_do_short_wait_on_queue(
        mv_waitqueue_t* queue,
        struct mv_task* tsk,
        struct task_struct** owner
)
{
    mv_waitqueue_add(queue, tsk);

    tsk->tasklockflag = 1;
    while (tsk->waitqueue)
    {
        if (unlikely(signal_pending(tsk->task)))
        {
            tsk->tasklockflag = 0;
            mv_delete_from_waitqueue(tsk);
            return -1;
        }
        tsk->task->state |= TASK_INTERRUPTIBLE;
        local_irq_enable();
        yield();
        local_irq_disable();
    }
    return 0;
}

/*******************************************************************************
* mv_do_wait_on_queue
*
* DESCRIPTION:
*       Suspend a task on wait queue.
*       Function has the same performance as mv_do_short_wait_on_queue when
*       task suspended for short period. After that task state changed to
*       suspended
*       This is useful for binary and counting semaphores
*
* INPUTS:
*       queue  - pointer to wait queue structure
*       tsk    - pointer to task structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Zero if wait successful
*       Non zero if wait interrupted (signal caught)
*
* COMMENTS:
*       Interrupts must be disabled when this function called
*
*******************************************************************************/
static int mv_do_wait_on_queue(
        mv_waitqueue_t* queue,
        struct mv_task* tsk
)
{
    int cnt = 200;

    mv_waitqueue_add(queue, tsk);

    tsk->tasklockflag = 1;
    while (tsk->waitqueue && cnt--)
    {
        if (unlikely(signal_pending(tsk->task)))
        {
            tsk->tasklockflag = 0;
            mv_delete_from_waitqueue(tsk);
            return -1;
        }
        tsk->task->state |= TASK_INTERRUPTIBLE;
        local_irq_enable();
        yield();
        local_irq_disable();
        if (!tsk->waitqueue)
            return 0;
    }

    tsk->tasklockflag = 2;
    while (tsk->waitqueue)
    {
        if (unlikely(signal_pending(tsk->task)))
        {
            tsk->tasklockflag = 0;
            mv_delete_from_waitqueue(tsk);
            return -1;
        }
        set_task_state(tsk->task, TASK_INTERRUPTIBLE);
        local_irq_enable();
        schedule();
        local_irq_disable();
    }
    return 0;
}

/*******************************************************************************
* mv_do_wait_on_queue_timeout
*
* DESCRIPTION:
*       Suspend a task on wait queue.
*       Return if timer expited.
*
* INPUTS:
*       queue   - pointer to wait queue structure
*       tsk     - pointer to task structure
*       timeout - timeout in scheduller ticks
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Non zero if wait successful
*       Zero if timeout occured
*       -1 if wait interrupted (signal caught)
*
* COMMENTS:
*       Interrupts must be disabled when this function called
*
*******************************************************************************/
static unsigned long mv_do_wait_on_queue_timeout(
        mv_waitqueue_t* queue,
        struct mv_task* tsk,
        unsigned long timeout
)
{
    mv_waitqueue_add(queue, tsk);

    tsk->tasklockflag = 2;
    while (tsk->waitqueue && timeout)
    {
        if (unlikely(signal_pending(tsk->task)))
        {
            tsk->tasklockflag = 0;
            mv_delete_from_waitqueue(tsk);
            return -1;
        }
        set_task_state(tsk->task, TASK_INTERRUPTIBLE);
        local_irq_enable();
        timeout = schedule_timeout(timeout);
        local_irq_disable();
    }
    if (tsk->waitqueue) /* timeout, delete from waitqueue */
    {
        tsk->tasklockflag = 0;
        mv_delete_from_waitqueue(tsk);
    }
    return timeout;
}



/************************************************************************
*
* Task lookup functions
*
* These functions required to lookup tasks in task array
************************************************************************/

/*******************************************************************************
* mv_check_tasks
*
* DESCRIPTION:
*       Walk through task array and check if task still alive
*       Perform cleanup actions for dead tasks
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
* COMMENTS:
*
*******************************************************************************/
static void mv_check_tasks(void)
{
    int k;

    local_irq_disable();
    for (k = 0; k < mv_num_tasks; )
    {
        /* search task */
        struct task_struct *p;
        int found = 0;
        for_each_task(p) {
            if (p == mv_tasks[k]->task)
                found = 1;
        }
        if (!found)
        {
            /* task not found */
            mv_unregistertask(mv_tasks[k]->task);
            continue;
        }
        k++;
    }
    local_irq_enable();
}

/*******************************************************************************
* translate_priority
*
* DESCRIPTION:
*       Translates a v2pthread priority into kernel priority
*
* INPUTS:
*       policy    - scheduler policy
*       priority  - vxWorks task priority
*
* OUTPUTS:
*       None
*
* RETURNS:
*       kernel priority
*
* COMMENTS:
*
*******************************************************************************/
#if 0
static int translate_priority(
        int policy,
        int priority
)
{
    if (policy == SCHED_OTHER)
        return 0;

    /*
    **  Validate the range of the user's task priority.
    */
    if (priority < 0 || priority > 255)
        return 99;
 
#ifdef  CPU_ARM
    if (priority <= 10)
        return 0;
#endif

    /* translate 0..255 to MAX_RT_PRIO..MAX_PRIO */
    priority *= (99-1);
    priority >>= 8;
    priority += 1;
    if (priority >= 99)
        priority = 99-1;

    return( priority );
}
#endif


/*******************************************************************************
* mv_set_prio
*
* DESCRIPTION:
*       Set task priority
*
* INPUTS:
*       param->taskid       - task ID
*       param->vxw_priority - vxWorks task priority
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Zero if successful
*       -MVKERNELEXT_EINVAL  - if task is not registered
*
* COMMENTS:
*
*******************************************************************************/
static int mv_set_prio(mv_priority_stc *param)
{
    struct mv_task* p;
#if 0
    struct sched_param prio;
    int policy;
#endif

    local_irq_disable();
    p = get_task_by_id(param->taskid);
    local_irq_enable();

    if (p == NULL)
        return -MVKERNELEXT_EINVAL;

    p->vxw_priority = param->vxw_priority;

#if 0
#ifdef ENABLE_REALTIME_SCHEDULING_POLICY
    policy = SCHED_RR;
#else
    policy = SCHED_OTHER;
#endif
    prio.sched_priority = translate_priority(policy, param->vxw_priority);
    sched_setscheduler(p->task, policy, &prio);
#endif

    return 0;
}


/*******************************************************************************
* mvKernelExt_read_proc_mem
*
* DESCRIPTION:
*       proc read data rooutine.
*       Use cat /proc/mvKernelExt to show task list and tasklock state
*
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Data length
*
* COMMENTS:
*
*******************************************************************************/
static int mvKernelExt_read_proc_mem(
        char    *page,
        char    **start,
        off_t   offset,
        int     count,
        int     *eof,
        void    *data
)
{
    int len;
    int k;
    
    len = 0;
    len += sprintf(page+len,"mvKernelExt major=%d\n", mvKernelExt_major);
#ifdef MV_TASKLOCK_STAT
    len += sprintf(page+len,"tasklock count=%d wait=%d\n",
            mv_tasklock_lcount,
            mv_tasklock_wcount);
#endif

    len += sprintf(page+len,"lock owner=%d\n", mv_tasklock_owner?mv_tasklock_owner->pid:0);
    {
        struct mv_task* p;
        for (p = mv_tasklock_waitqueue.first; p; p = p->wait_next)
            len += sprintf(page+len," wq: %d\n", p->task->pid);
    }
    /* list registered tasks */
    for (k = 0; k < mv_num_tasks; k++)
    {
        len += sprintf(page+len,
                "  task id=%d state=0x%x flag=%d"
#ifdef  MV_TASKLOCK_STAT
                " tlcount=%d tlwait=%d"
#endif
                " name=\"%s\"\n",
                mv_tasks[k]->task->pid,
                (unsigned int)mv_tasks[k]->task->state,
                mv_tasks[k]->tasklockflag,
#ifdef  MV_TASKLOCK_STAT
                mv_tasks[k]->tasklock_lcount,
                mv_tasks[k]->tasklock_wcount,
#endif
                mv_tasks[k]->name);
    }

    *eof = 1;

    return len;
}


/************************************************************************
* mvKernelExt_cleanup 
*
* DESCRIPTION:
*       Perform cleanup actions while module unloading
*       Unregister /proc entry, remove device entry
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
* COMMENTS:
*
*******************************************************************************/
static void mvKernelExt_cleanup(void)
{
    printk("mvKernelExt Says: Bye world from kernel\n");

    mvKernelExt_cleanup_common();

    mvKernelExt_initialized = 0;

    remove_proc_entry("mvKernelExt", NULL);

    unregister_chrdev(mvKernelExt_major, "mvKernelExt");

}

/************************************************************************
* mvKernelExt_init 
*
* DESCRIPTION:
*       Module initialization
*       Register device entry, /proc entry
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Zero if successful
*       Non zero if failed
*
* COMMENTS:
*
*******************************************************************************/
static int mvKernelExt_init(void)
{
    SET_MODULE_OWNER(&mvKernelExt_fops);
    int         result = 0;

    printk(KERN_DEBUG "mvKernelExt_init\n");

    /* first thing register the device at OS */
    
    /* Register your major. */
    result = register_chrdev(
            mvKernelExt_major,
            "mvKernelExt",
            &mvKernelExt_fops);

    if (result < 0)
    {
        printk("mvKernelExt_init: register_chrdev err= %d\n", result);
        return result;
    }

    printk(KERN_DEBUG "mvKernelExt_major = %d\n",
            mvKernelExt_major);

    result = mvKernelExt_init_common();
    if (result)
    {
        unregister_chrdev(mvKernelExt_major, "mvKernelExt");
        return result;
    }

    /* create proc entry */
    create_proc_read_entry("mvKernelExt", 0, NULL, mvKernelExt_read_proc_mem, NULL);

    mvKernelExt_initialized = 1;

    printk(KERN_DEBUG "mvKernelExt_init finished\n");

    return 0;
}


module_init(mvKernelExt_init);
module_exit(mvKernelExt_cleanup);
