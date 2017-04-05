/*******************************************************************************
*              Copyright 2010, Marvell Israel, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* kernelExt.h
*
* DESCRIPTION:
*       defines the interface to kernel module
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#ifndef _kernelExt_h
#define _kernelExt_h

#include <sys/ioctl.h>
#ifdef  MVKERNELEXT_SYSCALLS
#   include <linux/unistd.h>
#endif

#include "mv_KernelExt.h"

extern int mvKernelExtFd;

#ifdef  MVKERNELEXT_SYSCALLS

#   if defined(_syscall0)

        int mv_ctl(unsigned int cmd, unsigned long arg);
#       define mv_ctrl(cmd,arg)  mv_ctl((cmd),(unsigned long)(arg))

#   else /* !defined(_syscall0) */

#       include <unistd.h>
#       include <sys/syscall.h>
#       define mv_ctrl(cmd,arg)         syscall(__NR_mv_ctl, (cmd), (arg))

#   endif /* KERNEL_SYSCALLS_INDIRECT */



#else /* !defined MVKERNELEXT_SYSCALLS */
#   include <sys/ioctl.h>
#   define mv_ctrl(cmd,arg)         ioctl(mvKernelExtFd, (cmd), (arg))
#endif /* !defined MVKERNELEXT_SYSCALLS */

#define mv_noop()                mv_ctrl(MVKERNELEXT_IOC_NOOP,0)
#define mv_tasklock()            mv_ctrl(MVKERNELEXT_IOC_TASKLOCK,0)
#define mv_taskunlock()          mv_ctrl(MVKERNELEXT_IOC_TASKUNLOCK,0)
#define mv_taskunlockforce()     mv_ctrl(MVKERNELEXT_IOC_TASKUNLOCKFORCE,0)
#define mv_sem_signal(semid)     mv_ctrl(MVKERNELEXT_IOC_SEMSIGNAL, semid)
#define mv_sem_wait(semid)       mv_ctrl(MVKERNELEXT_IOC_SEMWAIT, semid)
#define mv_sem_trywait(semid)    mv_ctrl(MVKERNELEXT_IOC_SEMTRYWAIT, semid)

#endif /* _kernelExt_h */
