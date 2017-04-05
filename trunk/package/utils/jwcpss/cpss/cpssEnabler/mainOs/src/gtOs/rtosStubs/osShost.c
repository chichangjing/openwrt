/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osShost.c
*
* DESCRIPTION:
*       functions that implement the SHOST for RTOS on simulation.
*       SHOST - is the simulation of HOST.
*       RTOS on simulation - is real time operating system that instead of
*           accessing the HW it access a remote simulation via sockets.
*
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*
*******************************************************************************/

#include <gtOs/gtGenTypes.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsTimer.h>
#include <gtExtDrv/drivers/gtResetDrv.h>

#include <asicSimulation/SCIB/scib.h>

#define PROTECT_TASK_DOING_READ_WRITE_START_MAC         SCIB_SEM_TAKE
#define PROTECT_TASK_DOING_READ_WRITE_END_MAC           SCIB_SEM_SIGNAL

/********************************************/
/* next types taken from Simulation/Windows */
/********************************************/
typedef void *PVOID;
#define __stdcall


extern void exit(int);/* should be OS function */

GT_TASK_HANDLE simOsTaskCreate
(
    IN  GT_TASK_PRIORITY_ENT prio,
    IN  unsigned (__TASKCONV *startaddrPtr)(void*),
    IN  void    *arglistPtr
);

GT_STATUS osTaskManagerFlagsSet(
    IN GT_TASK      tid,
    IN GT_U32       vip,
    IN GT_U32       interrupt
);

GT_U8   * osTaskManagerNameGet(
    IN GT_TASK  tid
);


/**************************************/
/* end of types taken from Simulation */
/**************************************/


#define SHOSTP_max_intr_CNS 32


typedef UINT_32  SHOSTP_intr_FUN(void);
typedef UINT_32  SHOSTP_intr_param_FUN(UINT_32 param);

typedef struct SHOSTP_intr_STCT
{
    HANDLE hnd ;
    DWORD  thd_id ;
    UINT_32 id ;
    UINT_32 mask ;
    UINT_32 priority ;
    UINT_32 susp_flag ;
    HANDLE  ev ;
    UINT_32 was_ev ;
    SHOSTP_intr_FUN *fun ;
    int param_exist;
    UINT_32 param;
} SHOSTP_intr_STC ;


/* index 0 in the array of SHOSTP_intr_table is not used !!!! */
#define SHOSTP_INTR_TABLE_START_INDEX   1
static SHOSTP_intr_STC  SHOSTP_intr_table[SHOSTP_max_intr_CNS] ;
static GT_U32           SHOSTP_intr_table_next_index_to_use = SHOSTP_INTR_TABLE_START_INDEX;/* next index to use in array of SHOSTP_intr_table */

/* convert interrupt index to index in array of SHOSTP_intr_table --> for better performance
   index in array of SHOSTP_intr_convert_table is the interrupt number.
   the data inside SHOSTP_intr_convert_table[x] is the index to array of SHOSTP_intr_table
*/
static GT_U32           SHOSTP_intr_convert_table[SHOSTP_max_intr_CNS];

#define INVALID_INDEX   0xFFFFFFFF

#define CONVERT_INTERRUPT_TO_INDEX(intr) \
    (intr >= SHOSTP_INTR_TABLE_START_INDEX && \
     (intr < SHOSTP_max_intr_CNS)) ? SHOSTP_intr_convert_table[intr] :\
                                                     INVALID_INDEX

/* semaphore to protect the DB of SHOSTP_intr_table */
static GT_SEM   interruptManagerDbMutex=NULL;

#define LOCK_DB     PROTECT_TASK_DOING_READ_WRITE_START_MAC
#define UNLOCK_DB   PROTECT_TASK_DOING_READ_WRITE_END_MAC

/* when interrupt is wake up ,
   it need to lock all task and other interrupts(with less priority..)

   but it must first lock the DB , so no task will be inside it when we will
   lock the tasks !! to avoid dead lock
*/
#define LOCK_DB_AND_TASKS_AND_OTHER_INTERRUPTS(intr_ptr)\
        LOCK_DB;                         \
        osTaskLock();                    \
        SHOSTP_do_intr_susp(intr_ptr)

/* when interrupt goto sleep , it need to unlock what it locked... */
#define UNLOCK_DB_AND_TASKS_AND_OTHER_INTERRUPTS(intr_ptr)\
        SHOSTP_do_intr_resume(intr_ptr) ;          \
        osTaskUnLock();                            \
        UNLOCK_DB

/*******************************************************************************
* SHOSTP_is_intrrupt
*
* DESCRIPTION:
*      checks if task is registered as interrupt in DB
*
* INPUTS:
*      thd_id - task id to be checked if "interrupt" task
*
* OUTPUTS:
*       None
*
* RETURNS:
*      1 - task is "interrupt"
*      0 - task is not interrupt
*
* COMMENTS:
*       NO NEED TO LOCK DB
*
*******************************************************************************/
static GT_U32 SHOSTP_is_intrrupt (
    GT_TASK     thd_id
)
{
    GT_U32  i;
    /*loop on all DB */
    for(i=SHOSTP_INTR_TABLE_START_INDEX ; i < SHOSTP_intr_table_next_index_to_use; i++)
    {
        if(SHOSTP_intr_table[i].hnd)
        {
            if(SHOSTP_intr_table[i].thd_id == thd_id)
            {
                return 1;
            }
        }
    }

    return 0;
}

/*******************************************************************************
* SHOSTG_abort
*
* DESCRIPTION:
*       Called in ordrer to terminate the program.
*
* INPUTS:
*      None
*
* OUTPUTS:
*       None
*
* RETURNS:
*      None
*
* COMMENTS:
*       None
*
*******************************************************************************/
void SHOSTG_abort(void)
{
    GT_U8   *myTaskName;
    GT_TASK tid;
    GT_U32  ii = 20;
    osTaskGetSelf(&tid);

    myTaskName = osTaskManagerNameGet(tid);

    /* since the "exit" will kill the task , we need other way to be able to
       know what happened */

    osPrintf("\n SHOSTG_abort \n");
    osPrintf("\n SHOSTG_abort \n");
    osPrintf(" task[%s][%d] aborting (FATAL ERROR) \n",
            myTaskName ? (char*)myTaskName : "unknown",tid);
    osPrintf("\n SHOSTG_abort \n");
    osPrintf("\n SHOSTG_abort \n");

    while(ii--)
    {
        osTimerWkAfter(1000);
        osPrintf(".");
    }

    osPrintf("\n SHOSTG_abort : calling extDrvReset  \n");
    osTimerWkAfter(500);

    /* call to reset the board , so system can start fresh */
    extDrvReset();

    exit(0);/* kill process if the extDrvReset() failed to reset the board */
}

/*******************************************************************************
* SHOSTG_reset
*
* DESCRIPTION:
*
* INPUTS:
*      status
*
* OUTPUTS:
*       None
*
* RETURNS:
*      None
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern void SHOSTG_reset (

    /*!     INPUTS:             */

    unsigned int status

)
{
    SHOSTG_abort();
}

/*******************************************************************************
* SHOSTC_exit
*
* DESCRIPTION:
*       Called in ordrer to terminate the program.
*
* INPUTS:
*      None
*
* OUTPUTS:
*       None
*
* RETURNS:
*      None
*
* COMMENTS:
*       None
*
*******************************************************************************/
void SHOSTC_exit(unsigned int  status)
{
   SHOSTG_abort();
}


/*******************************************************************************
* SHOSTC_dll_abort
*
* DESCRIPTION:
*       Called in ordrer to terminate the program with a printed message.
*
* INPUTS:
*      msg - the message to be print out,
*
* OUTPUTS:
*       None
*
* RETURNS:
*      None
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern void SHOSTC_dll_abort(
    /* INPUT  */
    UINT_8* msg
)
{
   osPrintf(msg) ;

   SHOSTG_abort() ;
}

/*******************************************************************************
* SHOSTP_do_intr_susp
*
* DESCRIPTION:
*        Suspend all interrupt tasks that have lower priority than the specified
*        interrupt.
*
* INPUTS:
*       intr_ptr - pointer to interrupt structure.
*
* OUTPUTS:
*      None
*
* RETURNS:
*      None
*
* COMMENTS:
*    The name source is historical.
*
*       !!!! must be called under DB locked !!!!
*******************************************************************************/
static void SHOSTP_do_intr_susp (

    /*!     INPUTS:             */

    SHOSTP_intr_STC *intr_ptr
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/

    SHOSTP_intr_STC *tmp_ptr ;
    UINT_32 i;
    DWORD thd_id;

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/

    osTaskGetSelf(&thd_id);

    /* suspend all interrupts with less or equal priority */
    for(i=SHOSTP_INTR_TABLE_START_INDEX ; i < SHOSTP_intr_table_next_index_to_use; i++)
    {
      if(SHOSTP_intr_table[i].hnd)
      {
        tmp_ptr = &SHOSTP_intr_table[i];
        if(tmp_ptr->thd_id == thd_id ||
           (intr_ptr && tmp_ptr->priority > intr_ptr->priority))
            continue ;
        if ( tmp_ptr->mask == FALSE ){
            if ( osTaskSuspend( tmp_ptr->thd_id ) != 0 )
                SHOSTC_dll_abort("\nSuspend inter failed.\n");

        }
        tmp_ptr->susp_flag++ ;
      }
    }

}

/*******************************************************************************
* SHOSTC_init_intr_table
*
* DESCRIPTION:
*
* INPUTS:
*       None
*
* OUTPUTS:
*      None
*
* RETURNS:
*      None
*
* COMMENTS:
*    The name source is historical.
*******************************************************************************/
void SHOSTC_init_intr_table (

    void
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/
    osMemSet((void *)SHOSTP_intr_table,0,sizeof(SHOSTP_intr_table)) ;
    osMemSet((void *)SHOSTP_intr_convert_table,0,sizeof(SHOSTP_intr_convert_table)) ;

    if(interruptManagerDbMutex == NULL)
    {
        /* create the mutex that protect the DB */
        if(GT_OK != osSemBinCreate("interruptManagerDbMutex",OS_SEMB_FULL,&interruptManagerDbMutex))
        {
            SHOSTC_dll_abort("SHOSTC_init_intr_table : create semaphore failed.\n");
        }
    }
}

/*******************************************************************************
* SHOSTP_do_intr_resume
*
* DESCRIPTION:
*       Resumes all interrupt tasks.
*
* INPUTS:
*       intr_ptr - pointer to interrupt structure.
*
* OUTPUTS:
*      None
*
* RETURNS:
*      None
*
* COMMENTS:
*    The name source is historical.
*
*       !!!! must be called under DB locked !!!!
*******************************************************************************/
static void SHOSTP_do_intr_resume (

    /*!     INPUTS:             */

    SHOSTP_intr_STC *intr_ptr
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/

    SHOSTP_intr_STC *tmp_ptr ;
    UINT_32 i;
    DWORD thd_id;

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/

    osTaskGetSelf(&thd_id) ;

    /* resume all interrupts with less or equal priority */
    for(i=SHOSTP_INTR_TABLE_START_INDEX; i < SHOSTP_intr_table_next_index_to_use; i++)
    {
        if(SHOSTP_intr_table[i].hnd)
        {
            tmp_ptr = &SHOSTP_intr_table[i] ;
            if(tmp_ptr->thd_id == thd_id ||  !tmp_ptr->susp_flag ||
              (intr_ptr && tmp_ptr->priority > intr_ptr->priority))
                continue ;
            if ( tmp_ptr->mask == FALSE )
            {
                if (( tmp_ptr->susp_flag == 0 )||( osTaskResume((GT_TASK)tmp_ptr->hnd) != 0 ))
                    SHOSTC_dll_abort("\nResume inter failed.\n");
            }

            tmp_ptr->susp_flag-- ;
        }
    }
}

/*******************************************************************************
* SHOSTP_intr_thread
*
* DESCRIPTION:
*       This tasks is the ISR simulation.
*
* INPUTS:
*       intr_ptr - pointer to interrupt structure.
*
* OUTPUTS:
*      None
*
* RETURNS:
*      None
*
* COMMENTS:
*    The name source is historical.
*******************************************************************************/
static DWORD __stdcall  SHOSTP_intr_thread(PVOID pPtr)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/

  SHOSTP_intr_STC *intr_ptr = (SHOSTP_intr_STC *) pPtr ;
  SHOSTP_intr_FUN *fun ;
  SHOSTP_intr_param_FUN *fun_param;

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/


    for( ;intr_ptr && intr_ptr->ev ; )
    {
        osSemWait(intr_ptr->ev,0) ; /* wait forever */

        fun = intr_ptr->fun;
        fun_param = (SHOSTP_intr_param_FUN *)intr_ptr->fun;

        if(!fun)
        {
            continue ;
        }

        LOCK_DB_AND_TASKS_AND_OTHER_INTERRUPTS(intr_ptr);

        if ( intr_ptr->mask == FALSE )
        {
            if(intr_ptr->param_exist == TRUE)
                (*fun_param)(intr_ptr->param);
            else
                (*fun)();
        }
        else
            intr_ptr->was_ev = TRUE;

        UNLOCK_DB_AND_TASKS_AND_OTHER_INTERRUPTS(intr_ptr);
    }
    return 0 ;
}


/*******************************************************************************
* SHOSTG_interrupt_enable_one
*
* DESCRIPTION:
*       Enables one interrupt.
*
* INPUTS:
*       intr - interrupt to be enabled.
*
* OUTPUTS:
*      None
*
* RETURNS:
*      None
*
* COMMENTS:
*    The name source is historical.
*******************************************************************************/
extern void SHOSTG_interrupt_enable_one (

    /*!     INPUTS:             */

    UINT_32    intr
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/

    SHOSTP_intr_STC *tmp_ptr ;

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/

   LOCK_DB;
   intr = CONVERT_INTERRUPT_TO_INDEX(intr);
   if(intr >= SHOSTP_max_intr_CNS)
   {
       UNLOCK_DB;
       return;
   }

   tmp_ptr = &SHOSTP_intr_table[intr] ;

   if ( tmp_ptr->mask == FALSE )
   {
       UNLOCK_DB;
       return;
   }

   tmp_ptr->mask = FALSE;

   if( tmp_ptr->susp_flag )
   {
       UNLOCK_DB;
       return;
   }

   if ( osTaskResume(tmp_ptr->thd_id) != 0 )
       SHOSTC_dll_abort("\nResume inter failed.\n");

   if((tmp_ptr->thd_id) && (tmp_ptr->ev))
   {
       if (tmp_ptr->was_ev)
           osSemSignal((GT_SEM)tmp_ptr->ev) ;

       tmp_ptr->was_ev = FALSE;
   }
   else
   {
       SHOSTC_dll_abort("\nResume spec int failed.\n");
   }

   UNLOCK_DB;
   return;
}

/*******************************************************************************
* SHOSTG_interrupt_disable_one
*
* DESCRIPTION:
*       disables one interrupt.
*
* INPUTS:
*       intr - interrupt to be disabled.
*
* OUTPUTS:
*      None
*
* RETURNS:
*      None
*
* COMMENTS:
*    The name source is historical.
*******************************************************************************/
extern void SHOSTG_interrupt_disable_one (

    /*!     INPUTS:             */

    UINT_32    intr
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/
    SHOSTP_intr_STC *tmp_ptr ;
    DWORD thd_id;

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/

    /* suspend specific interrupt */
    osTaskGetSelf(&thd_id) ;

    LOCK_DB;
    intr = CONVERT_INTERRUPT_TO_INDEX(intr);
    if(intr >= SHOSTP_max_intr_CNS)
    {
        UNLOCK_DB;
        return;
    }

    tmp_ptr = &SHOSTP_intr_table[intr];

    if ( tmp_ptr->mask == TRUE )
    {
        UNLOCK_DB;
        return;
    }

    tmp_ptr->mask = TRUE;

    if( tmp_ptr->susp_flag )
    {
        UNLOCK_DB;
        return;
    }

    if ( tmp_ptr->thd_id == thd_id )
    {
        UNLOCK_DB;
        return;
    }

    if ( osTaskSuspend( tmp_ptr->thd_id ) != 0 )
        SHOSTC_dll_abort("\nSuspend inter failed.\n");

    UNLOCK_DB;
    return;
}

/*******************************************************************************
* SHOSTG_interrupt_enable
*
* DESCRIPTION:
*
* INPUTS:
*       new_val - new calling task priority
*
* OUTPUTS:
*      None
*
* RETURNS:
*      None
*
* COMMENTS:
*    The name source is historical.
*******************************************************************************/
extern  void  SHOSTG_interrupt_enable (

    UINT_32     new_val
)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/

    UINT_32  thd_id = 0, oldprio;
    GT_U32  lockedDb = 0;

/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

    osTaskGetSelf(&thd_id);

    if(0 == SHOSTP_is_intrrupt(thd_id))
    {/* don't lock for interrupts , since they lock the DB at their context */
        lockedDb = 1;
        LOCK_DB;
    }

    SHOSTP_do_intr_resume(0);

    if(lockedDb)
    {
        UNLOCK_DB;
    }

    osSetTaskPrior(thd_id, new_val, &oldprio);

}

/*******************************************************************************
* SHOSTG_interrupt_disable
*
* DESCRIPTION:
*       disables All interrupts.
*
* INPUTS:
*
* OUTPUTS:
*      None
*
* RETURNS:
*      the new priority of the calling task
*
* COMMENTS:
*
*******************************************************************************/
extern GT_U32 SHOSTG_interrupt_disable (

    /*!     INPUTS:             */

)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/

    UINT_32  thd_id = 0, oldprio,new_val;
    GT_U32  lockedDb = 0;

/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

    osTaskGetSelf(&thd_id);

    if(0 == SHOSTP_is_intrrupt(thd_id))
    {/* don't lock for interrupts , since they lock the DB at their context */
        lockedDb = 1;
        LOCK_DB;
    }

    SHOSTP_do_intr_susp(0);

    if(lockedDb)
    {
        UNLOCK_DB;
    }

    osGetTaskPrior(thd_id, &oldprio);
    new_val = oldprio;/* dont change priority */

/*    osSetTaskPrior(thd_id, new_val, &oldprio);*/

    return new_val;
}


/*******************************************************************************
* SHOSTG_bind_interrupt_with_param
*
* DESCRIPTION:
*
* INPUTS:
*       priority - the priority to be given to the interrupt task.
*       intr - interrupt number.
*       fun - interrupt handler.
*       ev - pointer to a semaphore ID (to be allocated) for the interrupt task.
*       param - any parameter to be transffered to the interrupt task.
*
* OUTPUTS:
*      None
*
* RETURNS:
*      None
*
* COMMENTS:
*    The name source is historical.
*******************************************************************************/
extern UINT_32 SHOSTG_bind_interrupt_with_param (

    /*!     INPUTS:             */

    UINT_32 priority,

    UINT_32 intr,

    void *fun,

    HANDLE ev,

    UINT_32 param
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/
    UINT_32  semId = 0, ret_val = 0 ;
    SHOSTP_intr_STC *intr_ptr ;
    static int cnt = 0;
    char   semName[50];
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/

    LOCK_DB;
    if(SHOSTP_intr_convert_table[intr] != 0)
    {
        SHOSTC_dll_abort("SHOSTG_bind_interrupt_with_param:Interrupt index already in use. \n");
        UNLOCK_DB;
        return 0;
    }

    if(SHOSTP_intr_table_next_index_to_use >= SHOSTP_max_intr_CNS)
    {
        SHOSTC_dll_abort("SHOSTG_bind_interrupt_with_param:DB overflow. \n");
        UNLOCK_DB;
        return 0;
    }

    SHOSTP_intr_convert_table[intr] = SHOSTP_intr_table_next_index_to_use++;

    intr = CONVERT_INTERRUPT_TO_INDEX(intr);

    intr_ptr = &SHOSTP_intr_table[intr] ;

    intr_ptr->id = intr ;
    intr_ptr->mask = FALSE ;
    intr_ptr->priority = priority ;
    intr_ptr->susp_flag = 0 ;
    intr_ptr->fun = (SHOSTP_intr_FUN *) fun ;
    intr_ptr->param_exist = TRUE;
    intr_ptr->param = param;

    /* We use semaphore instead of event */
    if(!ev)
    {
        osSprintf(semName, "intSem_%d", cnt);
        if (osSemBinCreate(semName, 0, (GT_SEM)&semId) != 0)
        {
            SHOSTC_dll_abort("\nCan't create interrupt semaphore.\n");
        }
    }

    intr_ptr->ev = (HANDLE)semId;

    intr_ptr->thd_id = (DWORD)simOsTaskCreate(7/*GT_TASK_PRIORITY_TIME_CRITICAL*/,
                                             (unsigned (*)(void*))SHOSTP_intr_thread,
                                             intr_ptr);

    osTaskManagerFlagsSet(intr_ptr->thd_id,1,1);/* vip task , and interrupt */

    intr_ptr->hnd = (HANDLE)intr_ptr->thd_id;

    if(!intr_ptr->thd_id)
    {
        SHOSTC_dll_abort("\nCan't create inter thread.\n");
    }

    ret_val = intr ;

    UNLOCK_DB;

    return ret_val;
}

/*******************************************************************************
* SHOSTG_set_interrupt
*
* DESCRIPTION:
*       signal an interrupt to start working
*
* INPUTS:
*       intr - interrupt number.
*
* OUTPUTS:
*      None
*
* RETURNS:
*      1 - the interrupt was found in DB
*      0 - the interrupt was NOT found in DB
*
* COMMENTS:
*    The name source is historical.
*******************************************************************************/
extern UINT_32  SHOSTG_set_interrupt
(
    /*!     INPUTS:             */
    UINT_32 intr
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/

    LOCK_DB;

    intr = CONVERT_INTERRUPT_TO_INDEX(intr);
    if(intr >= SHOSTP_max_intr_CNS)
    {
        UNLOCK_DB;
        return 0;
    }

    if(SHOSTP_intr_table[intr].hnd && SHOSTP_intr_table[intr].ev)
    {
        if (SHOSTP_intr_table[intr].mask == TRUE )
        {
            SHOSTP_intr_table[intr].was_ev = TRUE;
        }
        else
        {
            osSemSignal(SHOSTP_intr_table[intr].ev) ;
        }

        UNLOCK_DB;

       return 1 ;
    }

    UNLOCK_DB;

    return 0 ;
}


