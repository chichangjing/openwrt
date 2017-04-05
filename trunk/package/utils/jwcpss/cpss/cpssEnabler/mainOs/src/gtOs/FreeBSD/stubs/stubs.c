/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* stubs.c
*
* DESCRIPTION:
*       This is Linux simulation packet receive  and interrupt implementation
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

#include <gtOs/gtGenTypes.h>

#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsSem.h>

#define DO_SUSPEND_TASKS_FOR_INTERRUPT

#define SHOSTP_max_intr_CNS 32

/* win32 names */
typedef GT_U32 UINT_32,HANDLE, DWORD;
#ifndef FALSE
	#define FALSE               0
#endif
#ifndef TRUE
	#define TRUE                1
#endif

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


static UINT_32 SHOSTP_intr_enable = 0 ;

static SHOSTP_intr_STC  SHOSTP_intr_table[SHOSTP_max_intr_CNS] ;
static GT_SEM           SHOSTP_intr_table_mtx = 0;

/* commandLine should belong to this module to make mainOs module workable
 * without CPSS Enabler into another third-party client. */
char  commandLine[128];

extern void* simOsTaskCreate (
    int prio,
    unsigned (__TASKCONV *startaddrPtr)(void*),
    void    *arglistPtr
);
extern void SASICG_exit(
	void
);



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
    SASICG_exit();
    exit(0);
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
void SHOSTC_exit(UINT_32  status)
{
   SHOSTG_abort();
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
    char* msg
)
{
   osPrintf("%s",msg) ;

   SHOSTG_abort() ;
}

/*******************************************************************************
* SHOSTC_intr_table_lock
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
#define SHOSTC_intr_table_lock() osSemWait(SHOSTP_intr_table_mtx,OS_WAIT_FOREVER)

/*******************************************************************************
* SHOSTC_intr_table_unlock
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
#define SHOSTC_intr_table_unlock() osSemSignal(SHOSTP_intr_table_mtx)

#ifdef  DO_SUSPEND_TASKS_FOR_INTERRUPT
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
    for(i=0 ; i < SHOSTP_max_intr_CNS; i++)
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
#endif /* DO_SUSPEND_TASKS_FOR_INTERRUPT */

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
    memset((void *)SHOSTP_intr_table,0,sizeof(SHOSTP_intr_table)) ;
    if (osSemMCreate("ShostpIntTblMtx", &SHOSTP_intr_table_mtx ) != GT_OK)
        exit (0);
}

/*******************************************************************************
* SHOSTG_go_intr_go
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
extern void SHOSTG_go_intr_go (

    void
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/
    SHOSTP_intr_enable = 1;

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
    for(i=0; i < SHOSTP_max_intr_CNS; i++)
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
static unsigned __TASKCONV  SHOSTP_intr_thread(void* pPtr)
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

        osSemWait((GT_SEM)intr_ptr->ev,0) ; /* wait forever */
        fun = intr_ptr->fun;
        fun_param = (SHOSTP_intr_param_FUN *)intr_ptr->fun;

        if(!fun) /* || !SHOSTP_intr_enable) */
            continue ;

        osTaskLock();

        /*
         * Do not suspend other interrupt tasks osTaskLock() is
         * enough
         */
#ifdef  DO_SUSPEND_TASKS_FOR_INTERRUPT
        SHOSTP_do_intr_susp(intr_ptr) ;
#endif

        /* KEEP THE TASKS LOCKED -
           DO NOT LET OTHER TASKS INTERRUPT IN THE MIDDLE OF THE "ISR" */
        if ( intr_ptr->mask == FALSE )
        {
            if(intr_ptr->param_exist == TRUE)
                (*fun_param)(intr_ptr->param);
            else
                (*fun)();
        }
        else
            intr_ptr->was_ev = TRUE;

#ifdef  DO_SUSPEND_TASKS_FOR_INTERRUPT
        SHOSTP_do_intr_resume(intr_ptr) ;
#endif

        osTaskUnLock();
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

 /* take interrupts simulation semaphore   */
   SHOSTC_intr_table_lock();

   tmp_ptr = &SHOSTP_intr_table[intr] ;

   if ( tmp_ptr->mask == FALSE )
   {
       SHOSTC_intr_table_unlock();
       return;
   }

   tmp_ptr->mask = FALSE;

   if( tmp_ptr->susp_flag )
   {
       SHOSTC_intr_table_unlock();
       return;
   }

   if ( osTaskResume(tmp_ptr->thd_id) != 0 )
       SHOSTC_dll_abort("\nResume inter failed.\n");

   if((intr) && (intr <= SHOSTP_max_intr_CNS) && (tmp_ptr->thd_id) && (tmp_ptr->ev) )
   {
       if (tmp_ptr->was_ev)
           osSemSignal((GT_SEM)tmp_ptr->ev) ;

       tmp_ptr->was_ev = FALSE;
   }
   else
   {
       SHOSTC_dll_abort("\nResume spec int failed.\n");
   }

   SHOSTC_intr_table_unlock();

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

    /* take interrupts simulation semaphore   */
    SHOSTC_intr_table_lock();

    /* suspend specific interrupt */
    osTaskGetSelf(&thd_id) ;

    tmp_ptr = &SHOSTP_intr_table[intr];

    if ( tmp_ptr->mask == TRUE )
    {
        SHOSTC_intr_table_unlock();
        return;
    }

    tmp_ptr->mask = TRUE;

    if( tmp_ptr->susp_flag )
    {
        SHOSTC_intr_table_unlock();
        return;
    }

    if ( tmp_ptr->thd_id == thd_id )
    {
        SHOSTC_intr_table_unlock();
        return;
    }

    if ( osTaskSuspend( tmp_ptr->thd_id ) != 0 )
        SHOSTC_dll_abort("\nSuspend inter failed.\n");

    SHOSTC_intr_table_unlock();
}

/*******************************************************************************
* SHOSTG_interrupt_do_enable
*
* DESCRIPTION:
*              
* INPUTS:
*       new_val - 
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
extern  void  SHOSTG_interrupt_do_enable (

    UINT_32     new_val
)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/

/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

    SHOSTC_intr_table_lock();

    SHOSTP_do_intr_resume(0);

    SHOSTC_intr_table_unlock();
}

/*******************************************************************************
* SHOSTG_interrupt_enable
*
* DESCRIPTION:
*              
* INPUTS:
*       new_val - 
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
}

/*******************************************************************************
* SHOSTG_interrupt_disable
*
* DESCRIPTION:
*              
* INPUTS:
*       none 
*
* OUTPUTS:
*      None
* 
* RETURNS:
*      None
*
* COMMENTS:
*    TBD: Implement  
*******************************************************************************/
UINT_32 SHOSTG_interrupt_disable (void)
{
    return 0;
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
    UINT_32  ret_val = 0 ;
    GT_SEM   semId = (GT_SEM)0;
    SHOSTP_intr_STC *intr_ptr ;
    static int cnt = 0;
    char   semName[50];
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/
    SHOSTC_intr_table_lock();

    intr_ptr = &SHOSTP_intr_table[intr] ;

    if(!intr_ptr->hnd)
    {
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
            if (osSemBinCreate(semName, 0, &semId) != 0)
            {
                SHOSTC_intr_table_unlock();

                SHOSTC_dll_abort("\nCan't create interrupt semaphore.\n");
            }
        }

        intr_ptr->ev = (HANDLE)semId;

        intr_ptr->thd_id = (DWORD)simOsTaskCreate(7/*GT_TASK_PRIORITY_TIME_CRITICAL*/,
                                                 (unsigned (*)(void*))SHOSTP_intr_thread,
                                                 intr_ptr);

        intr_ptr->hnd = (HANDLE)intr_ptr->thd_id;

        if(!intr_ptr->thd_id)
        {
            SHOSTC_dll_abort("\nCan't create inter thread.\n");
        }

        ret_val = intr ;
    } /* if(!intr_ptr->hnd) */
    else
    {
        SHOSTC_intr_table_unlock();
        SHOSTC_dll_abort("\nInterrupt index already in use.\n");
    }

    SHOSTC_intr_table_unlock();

    return ret_val;
}

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

    SHOSTC_intr_table_lock();

    if(intr && intr <= SHOSTP_max_intr_CNS &&
       SHOSTP_intr_table[intr].hnd && SHOSTP_intr_table[intr].ev)
    {
        if (SHOSTP_intr_table[intr].mask == TRUE )
        {
            SHOSTP_intr_table[intr].was_ev = TRUE;
        }
        else
        {
            osSemSignal((GT_SEM)SHOSTP_intr_table[intr].ev) ;
        }

       SHOSTC_intr_table_unlock();
       return 1 ;
    }

    SHOSTC_intr_table_unlock();

    return 0 ;
}

char* strlwr (char* str)
{
	char	*p;

	for (p = str; *p; p++)
		if (*p >= 'A' && *p <= 'Z')
			*p += 'a' - 'A';
		/*  *p = tolower((unsigned char)(*p)); */
	
	return str;
}

char *GetCommandLine(void)
{
    return commandLine;
}

void LinuxSim_stubs_init(void)
{
}
