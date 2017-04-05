/*******************************************************************************
*              Copyright 2002, MARVELL TECHNOLOGY, LTD.
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
* appDemoCommonConfig.c
*
* DESCRIPTION:
*       Initialization common functions for boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

/* Cpss includes */
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <appDemo/sandConfig/appDemoFap20mConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

#include <SAND/Utils/include/sand_os_interface.h>
#include <SAND/Utils/include/sand_extServices.h>
#include <gtOs/gtOsMsgQ.h>
#include <gtOs/gtOsStdLib.h>

#include <SAND/Management/include/sand_module_management.h>

/*******************************************************************************
* appDemoSandSetDefaultOsFuncs
*
* DESCRIPTION:
*       Sets default sand bindings to OS
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoSandSetDefaultOsFuncs
(
    void
)
{
    SAND_OS_BIND_STC    osFuncBind;

    osMemSet(&osFuncBind, 0,sizeof(osFuncBind));

#define SET_BINDING(btype,bfunc,bindto) \
    osFuncBind.bfunc = (btype)bindto

    SET_BINDING(SAND_OS_TASK_CREATE_FUNC, sandOsTaskCreate, osTaskCreate);
    SET_BINDING(SAND_OS_TASK_DELETE_FUNC, sandOsTaskDelete, osTaskDelete);
    SET_BINDING(SAND_OS_TASK_GET_SELF_FUNC, sandOsTaskGetSelf, osTaskGetSelf);
    SET_BINDING(SAND_OS_TASK_LOCK_FUNC, sandOsTaskLock, osTaskLock);
    SET_BINDING(SAND_OS_TASK_UNLOCK_FUNC, sandOsTaskUnLock, osTaskUnLock);
    SET_BINDING(SAND_OS_MALLOC_FUNC, sandOsMalloc, osMalloc);
    SET_BINDING(SAND_OS_FREE_FUNC, sandOsFree, osFree);
    SET_BINDING(SAND_OS_MEMCPY_FUNC, sandOsMemCpy, osMemCpy);
    SET_BINDING(SAND_OS_MEMCMP_FUNC, sandOsMemCmp, osMemCmp);
    SET_BINDING(SAND_OS_MEMSET_FUNC, sandOsMemSet, osMemSet);
    SET_BINDING(SAND_OS_STRLEN_FUNC, sandOsStrLen, osStrlen);
#ifdef  LINUX
    SET_BINDING(SAND_OS_STRNCPY_FUNC, sandOsStrNCpy, osStrNCpy);
    SET_BINDING(SAND_OS_STRNCMP_FUNC, sandOsStrNCmp, osStrNCmp);
#endif
    SET_BINDING(SAND_OS_TIMER_WK_AFTER_FUNC, sandOsTimerWkAfter, osTimerWkAfter);
    SET_BINDING(SAND_OS_TICK_GET_FUNC, sandOsTickGet, osTickGet);
#ifdef  LINUX
    SET_BINDING(SAND_OS_TIME_RT_FUNC, sandOsTimeRT, osTimeRT);
    SET_BINDING(SAND_OS_GET_SYS_CLOCK_RATE_FUNC, sandOsGetSysClockRate, osGetSysClockRate);
#endif
    SET_BINDING(SAND_OS_DELAY_FUNC, sandOsDelay, osDelay);
    SET_BINDING(SAND_OS_MUTEX_CREATE_FUNC, sandOsMutexCreate, osMutexCreate);
    SET_BINDING(SAND_OS_MUTEX_DELETE_FUNC, sandOsMutexDelete, osMutexDelete);
    SET_BINDING(SAND_OS_MUTEX_LOCK_FUNC, sandOsMutexLock, osMutexLock);
    SET_BINDING(SAND_OS_MUTEX_UNLOCK_FUNC, sandOsMutexUnlock, osMutexUnlock);
#ifdef  LINUX
    SET_BINDING(SAND_OS_MSGQ_CREATE_FUNC, sandOsMsgQCreate, osMsgQCreate);
    SET_BINDING(SAND_OS_MSGQ_DELETE_FUNC, sandOsMsgQDelete, osMsgQDelete);
    SET_BINDING(SAND_OS_MSGQ_SEND_FUNC, sandOsMsgQSend, osMsgQSend);
    SET_BINDING(SAND_OS_MSGQ_RECV_FUNC, sandOsMsgQRecv, osMsgQRecv);
    SET_BINDING(SAND_OS_MSGQ_NUM_MSGS_FUNC, sandOsMsgQNumMsgs, osMsgQNumMsgs);
#endif
    SET_BINDING(SAND_OS_PRINTF_FUNC, sandOsPrintf, osPrintf);
    SET_BINDING(SAND_OS_SPRINTF_FUNC, sandOsSprintf, osSprintf);
#ifdef  LINUX
    SET_BINDING(SAND_OS_VPRINTF_FUNC, sandOsVprintf, osVprintf);
    SET_BINDING(SAND_OS_VSPRINTF_FUNC, sandOsVsprintf, osVsprintf);
#endif
#if (!defined(FREEBSD) && !defined(UCLINUX)) || defined(ASIC_SIMULATION)
    SET_BINDING(SAND_OS_SET_INT_LOCK_UNLOCK_FUNC, sandOsSetIntLockUnlock, osSetIntLockUnlock);
#endif
#ifdef  LINUX
    SET_BINDING(SAND_OS_QSORT_FUNC, sandOsQsort, osQsort);
    SET_BINDING(SAND_OS_BSEARCH_FUNC, sandOsBsearch, osBsearch);
#endif

    sandOsBindFunctions(&osFuncBind);

    return GT_OK;
}

/*******************************************************************************
* appDemoSandInit
*
* DESCRIPTION:
*       Opens and initializes SAND parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoSandInit
(
    void
)
{
    unsigned int        is_already_opened;
    unsigned long       ret;                    /* FAP20M return code */
    SAND_RET            sand_ret;               /* SAND return code */

    appDemoSandSetDefaultOsFuncs();

    /******************************************************************
    * Start SAND driver module.
    *****************************************************************/
    ret = sand_module_open(
                   SAND_MAX_DEVICE, /* max devices */
                   16, /* system_tick_in_ms */
                   70, /* tcm task priority */
                   1,  /* min_time_between_tcm_activation */
                   FALSE, /* TCM mockup interrupts */
                   NULL,
                   NULL,
                   &is_already_opened
                 );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    sand_tcm_set_enable_flag(FALSE);

    return GT_OK;
}



int send_string_to_screen(
    char *out_string,
    int  add_cr_lf
)
{
  printf("%s", out_string);
  if(add_cr_lf)
  {
    printf("\r\n");
  }
  return 0;
}


