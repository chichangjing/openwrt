/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osVxDebug.c
*
* DESCRIPTION:
*       VxWorks Operating System wrapper. Debug facility.
*
* DEPENDENCIES:
*       VxWorks, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#include <vxWorks.h>
#include <time.h>
#include <tickLib.h>
#include <intLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <stdlib.h>
#include <string.h>
#include <taskHookLib.h>
#include <dbgLib.h>
#include <sysSymTbl.h>

#if defined(PPC_CPU) || defined(POWERPC_CPU)
#include <cacheLib.h>
#endif

#include <gtOs/gtOsDebug.h>
#include <gtOs/gtOsTimer.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsIntr.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsExc.h>

/* constants */
#define PRV_OS_DEBUG_UTILIZATION_ENTRIES_CNS 10000
#define PRV_OS_DEBUG_MAX_TASKS_NUM_CNS 255

#define PRV_OS_DEBUG_200_MILISEC_IN_5_MIN_CNS (60 * 5)*5
#define PRV_OS_DEBUG_200_MILISEC_IN_1_MIN_CNS (60)*5
#define PRV_OS_DEBUG_200_MILISEC_IN_SEC_CNS 5
#define PRV_OS_DEBUG_200_MILISEC_IN_5_SEC_CNS 25

#define INT_START_BIT 0x80000000

#define  PRV_OS_DEBUG_prev_func_CNS            64

#ifdef DEBUG_OS_DEBUG
#define DBG(x)     osPrintf x
#else
#define DBG(x)
#endif

#if defined(PPC_CPU) || defined(POWERPC_CPU)

  #define EPIC_OK                          0

  #define SYSTEM_RESET                     0x0100
  #define MACHINE_CHECK                    0x0200
  #define DATA_STORAGE                     0x0300
  #define INSTRUCTION_STORAGE              0x0400
  #define EXTERNAL                         0x0500
  #define ALIGNMENT                        0x0600
  #define PROGRAM                          0x0700
  #define FLOATING_POINT_UNAVAILABLE       0x0800
  #define DECREMENTER                      0x0900
  #define SYSTEM_CALL                      0x0C00
  #define TRACE                            0x0D00
  #define FLOATING_POINT_ASSIST            0x0E00
  #define INSTRUCTION_TRANSLATION_MISS     0x1000
  #define DATA_LOAD_TRANSLATION_MISS       0x1100
  #define DATA_STORE_TRANSLATION_MISS      0x1200
  #define INSTRUCTION_ADDR_BREAKPOINT      0x1300
  #define SYSTEM_MANAGEMENT_INTERRUPT      0x1400
  #define NULL_INT_INDEX                   0x1500
  #define NULL_INT_LAST_INDEX              0x2F00

  #define  PRV_OS_DEBUG_intr_rfi_opcode_CNS           0x4C000064
  #define  PRV_OS_DEBUG_intr_blr_opcode_CNS           0x4E800020
  #define  PRV_OS_DEBUG_intr_srr0_set_opcode_CNS      0x7C7A03A6
  #define  PRV_OS_DEBUG_intr_srr0_get_opcode_CNS      0x7C7A02A6
  #define  PRV_OS_DEBUG_intr_srr1_get_opcode_CNS      0x7C7B02A6
  #define  PRV_OS_DEBUG_intr_msr_set_opcode_CNS       0x7C600124
  #define  PRV_OS_DEBUG_intr_r3_set_opcode_CNS        0x38600002
  #define  PRV_OS_DEBUG_intr_copy_r3_to_r1_opcode_CNS 0x38230000
  #define  PRV_OS_DEBUG_intr_copy_r1_to_r3_opcode_CNS 0x38610000

#elif defined(CPU_ARM)
  
  #define SYSTEM_RESET                     0x00
  #define UNDEFINED_INSTRUCTION            0x04
  #define SOFTWARE_INTERRUPT               0x08
  #define ABORT_PREFETCH                   0x0C
  #define ABORT_DATA                       0x10
  #define IRQ                              0x18
  #define FIQ                              0x1C

  /*!
      OpCode and masks for SUB,STM,STR and LDM assembler commands.
  */
  #define SUB_SP_CMD 0x4DD000
  #define SUB_SP_MASK 0xDFFF000
  #define MOV_CMD 0xE3A00000
  #define MOV_CMD_MASK 0xFFFFF000
  #define STM_CMD 0xE82D0000
  #define STM_CMD_MASK 0xFE7F0000
  #define STM_CMD_NO 0xE92D0000
  #define STR_CMD 0xE52DE004
  #define LDR_CMD 0xE5900000
  #define LDR_CMD_MASK 0xFFF0F000
  #define LDM_CMD 0xE83D0000
  #define LDM_CMD_NO 0xE89D0000
  #define LDM_CMD_MASK 0xFE7F0000

typedef struct PRV_OS_DEBUG_services_call_info_STCT
{
    GT_U32 * startAddress;  /* Base address of called function */
    GT_U32 * endAddress;    /* Address of function */
    GT_U32 * returnAddress; /* Return address of function */
    GT_U32   ip;             /* Content of IP register readed from stack */
    GT_U32   fp;             /* Content of FP register readed from stack */
    GT_U32   frameSize;      /* Frame size */
    GT_U32 * stack;         /* Current value of stack pointer */
}PRV_OS_DEBUG_services_call_info_STC;

#endif


/* definitions */
typedef struct prvOsDebug_monitor_trace_STCT
{
  GT_U32     sys_tick;            /* 0x0 - number of system ticks per second */
  GT_U32     sys_count;           /* 0x4 - count within the tick */
  GT_U32     event_id;            /* 0x8 - Code for type of event:
                                            At initialization reset to 0,
                                            At Interrupt event: Bit 31 sets
                                               to '1' and OR with Cause
                                               register. Bit 30 indicates end of interrupt
                                            At Task Switching Sets for the
                                               next task to be run.*/
} prvOsDebug_monitor_trace_STC;

typedef struct prvOsDebug_task_tbl_STCT
{
  GT_U32 task_id;
  GT_U8  sec_utilization[PRV_OS_DEBUG_200_MILISEC_IN_5_MIN_CNS];
} prvOsDebug_task_tbl_STC;

typedef struct  {
      GT_U32 exc_vector; /* exception vector */
      char exc_name[30];  /* exception name */
}  PRV_OS_DEBUG_exception_STC;


typedef void prvOsDebug_void_int_FNC (GT_U32 val);  /* void fnc, int */


/* locals */
#if defined(PPC_CPU) || defined(POWERPC_CPU)

#define PRV_OS_DEBUG_intr_number_of_exceptions_CNS   11

    static ESFPPC   *PRV_OS_DEBUG_stackPtr;

    static PRV_OS_DEBUG_exception_STC PRV_OS_DEBUG_exc_struct[] = {
                    {MACHINE_CHECK,"MACHINE_CHECK"},
                    {DATA_STORAGE,"DATA_STORAGE"},
                    {INSTRUCTION_STORAGE,"INSTRUCTION_STORAGE"},
                    {EXTERNAL,"EXTERNAL"},
                    {ALIGNMENT,"ALIGNMENT"},
                    {PROGRAM,"PROGRAM"},
                    {FLOATING_POINT_UNAVAILABLE,"FLOATING_POINT_UNAVAILABLE"},
                    {DECREMENTER,"DECREMENTER"},
                    {SYSTEM_CALL,"SYSTEM_CALL"},
                    {TRACE,"TRACE"},
                    {FLOATING_POINT_ASSIST,"FLOATING_POINT_ASSIST"}
                    };

#elif defined(CPU_ARM)
#define PRV_OS_DEBUG_intr_number_of_exceptions_CNS   7
    static PRV_OS_DEBUG_exception_STC PRV_OS_DEBUG_exc_struct[] = {
                    {SYSTEM_RESET,"SYSTEM RESET"},
                    {UNDEFINED_INSTRUCTION,"UNDEFINED INSTRUCTION"},
                    {SOFTWARE_INTERRUPT,"SOFTWARE INTERRUPT"},
                    {ABORT_PREFETCH,"ABORT PREFETCH"},
                    {ABORT_DATA,"ABORT DATA"},
                    {IRQ,"IRQ"},
                    {FIQ,"FIQ"}
                    };

PRV_OS_DEBUG_services_call_info_STC PRV_OS_DEBUG_function_calls[PRV_OS_DEBUG_prev_func_CNS];
GT_U32 PRV_OS_DEBUG_services_previous_functions[PRV_OS_DEBUG_prev_func_CNS];
#endif

static GT_BOOL dbgServicesInitialized = GT_FALSE;
static GT_BOOL prvOsDebug_utilization_five_sec_passed_LCL;
static GT_BOOL prvOsDebug_utilization_one_min_passed_LCL;
static GT_BOOL prvOsDebug_utilization_five_min_passed_LCL;
static GT_BOOL prvOsDebug_hook_installed_LCL = GT_FALSE;
static GT_BOOL prvOsDebug_cpu_utilization_activated_LCL = GT_FALSE;

/* number of dropped events counter - increment each time an event occures
   but the events table is full */
static GT_U32 prvOsDebug_tasks_tbl_full_LCL = 0;

/* utilization task Id */
static GT_U32 prvOsDebug_utilize_task_id_LCL;
static GT_U32 prvOsDebug_idle_task_id_LCL;

/* This is the index of the tasks table. */
static GT_U32 prvOsDebug_tasks_tbl_idx_LCL;
static GT_U32 prvOsDebug_num_of_tasks_LCL;

/* the tasks table pointer */
static prvOsDebug_task_tbl_STC *prvOsDebug_task_tbl_LCL;

/* the utilization task time stamp */
static prvOsDebug_monitor_trace_STC  prvOsDebug_utilization_second_start_LCL;



/* This is a declaration for function returning pointer to VOID*/
static prvOsDebug_void_int_FNC *prvOsDebug_debug_task_switch_func_PTR = NULL;


/* This is the index of the event table. */
static GT_U32  prvOsDebug_cpu_utilization_tbl_idx_LCL = 0;
static GT_U32  prvOsDebug_cpu_utilization_num_of_items_LCL =
                                    PRV_OS_DEBUG_UTILIZATION_ENTRIES_CNS;

/* the events table pointer */
static prvOsDebug_monitor_trace_STC *prvOsDebug_utilization_tbl_LCL;

/* private functions */


/*******************************************************************************
* prvDebugMonitorFillTraceTimeEntry
*
* DESCRIPTION:
*       Fills trace time entry
*
* INPUTS:
*       None
*
* OUTPUTS:
*       entryPtr - (pointer to) trace time entry
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static void prvDebugMonitorFillTraceTimeEntry
(
    OUT prvOsDebug_monitor_trace_STC * entryPtr
)
{
    entryPtr->sys_tick = tickGet();
    entryPtr->sys_count = sysTimestamp();
}


/*******************************************************************************
* prvDebugCpuUtilizationFromInterrupt
*
* DESCRIPTION:
*       CPU Utilization from ISR
*
* INPUTS:
*       endOfInt
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
GT_VOID prvDebugCpuUtilizationFromInterrupt
(
    GT_BOOL endOfInt
)
{
    static GT_BOOL interrupt_started = GT_FALSE;

    if (prvOsDebug_cpu_utilization_activated_LCL != GT_TRUE)
        return;


    if ( !endOfInt )
    {
        if(interrupt_started)
            return;

        interrupt_started = TRUE;

        if(prvOsDebug_cpu_utilization_tbl_idx_LCL < prvOsDebug_cpu_utilization_num_of_items_LCL)
        {
            /* Sets the first event entry with this "set event" function */
            prvDebugMonitorFillTraceTimeEntry(
                &prvOsDebug_utilization_tbl_LCL[prvOsDebug_cpu_utilization_tbl_idx_LCL]);

            /* Setting Start of interrupt flag (bit 31) */
            prvOsDebug_utilization_tbl_LCL
                [prvOsDebug_cpu_utilization_tbl_idx_LCL].event_id = INT_START_BIT;

            /* Incrementing the index */
            prvOsDebug_cpu_utilization_tbl_idx_LCL++;
        }
        else
            /* the events table is full so drop the event and add error counter */
            prvOsDebug_tasks_tbl_full_LCL++;

    }
    else
    {
        if(!interrupt_started)
            return;

        interrupt_started = FALSE;

        /* this is the end of the interrupt, the control is back to the active task */
        if(prvOsDebug_cpu_utilization_tbl_idx_LCL <
                                    prvOsDebug_cpu_utilization_num_of_items_LCL)
        {
            prvDebugMonitorFillTraceTimeEntry(
                &(prvOsDebug_utilization_tbl_LCL[prvOsDebug_cpu_utilization_tbl_idx_LCL]));

            prvOsDebug_utilization_tbl_LCL
                [prvOsDebug_cpu_utilization_tbl_idx_LCL].event_id = (GT_U32)taskIdCurrent;

            /* Incrementing the index */
            prvOsDebug_cpu_utilization_tbl_idx_LCL++;
        }
        else
            /* the events table is full so drop the event and add error counter */
            prvOsDebug_tasks_tbl_full_LCL++;
    }
}

/*******************************************************************************
* prvDebugEntryFromTaskSwitch
*
* DESCRIPTION:
*       Hook to Task Switch
*
* INPUTS:
*       pOldTcb - pointer to old task's WIND_TCB
*       pNewTcb - pointer to new task's WIND_TCB
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
static void prvDebugEntryFromTaskSwitch
(
    IN WIND_TCB *pOldTcb,
    IN WIND_TCB *pNewTcb
)
{
    GT_U32 intVal;

    /* avoid conflict between services from task switch to those from interrupt */
    intVal = intLock();

    /* call CPU utilization handling */
    if(prvOsDebug_cpu_utilization_activated_LCL)
    {
        if(prvOsDebug_cpu_utilization_tbl_idx_LCL <
                                   prvOsDebug_cpu_utilization_num_of_items_LCL)
        {
            /* Sets the first event entry with this "set event" function */
            prvDebugMonitorFillTraceTimeEntry(
                &(prvOsDebug_utilization_tbl_LCL[prvOsDebug_cpu_utilization_tbl_idx_LCL]));

            prvOsDebug_utilization_tbl_LCL
                [prvOsDebug_cpu_utilization_tbl_idx_LCL].event_id = (GT_U32)pNewTcb;

            /* Incrementing the index */
            prvOsDebug_cpu_utilization_tbl_idx_LCL++;
        }
        else
            /* the events table is full so drop the event and add error counter */
            prvOsDebug_tasks_tbl_full_LCL++;
    }

    intUnlock(intVal);
}

/*******************************************************************************
* prvDebugHookAdd
*
* DESCRIPTION:
*       Adds debug hook to task switch
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvDebugHookAdd (void)
{

    /* if trace & CPU utilization disabled, we can delete the entry from */
    /* interrupt entry, and the hook to task switch */
    if(!prvOsDebug_hook_installed_LCL)
    {
        /* Setting the task switching function pointer */
        prvOsDebug_debug_task_switch_func_PTR =
                       (prvOsDebug_void_int_FNC *)prvDebugEntryFromTaskSwitch;

        /* bind routine to be called at every task switch */
        if ( taskSwitchHookAdd ((FUNCPTR)prvOsDebug_debug_task_switch_func_PTR) == ERROR )
        {
            prvOsDebug_debug_task_switch_func_PTR = NULL;
            return GT_FAIL;
        }
        else
        {
            prvOsDebug_hook_installed_LCL = TRUE;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvDebugCheckHookDelete
*
* DESCRIPTION:
*       Deletes debug hook from task switch
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
*       None
*
*******************************************************************************/
static void prvDebugCheckHookDelete (void)
{


    /* if CPU utilization disabled, we can delete the entry from */
    /* interrupt entry, and the hook to task switch */
    if(!prvOsDebug_cpu_utilization_activated_LCL)
    {
        taskSwitchHookDelete ((FUNCPTR)prvDebugEntryFromTaskSwitch);

        prvOsDebug_debug_task_switch_func_PTR = NULL;

        prvOsDebug_hook_installed_LCL   = FALSE;
    }
}

/*******************************************************************************
* prvDebugTicksUnite
*
* DESCRIPTION:
*       Calculates delta 
*
* INPUTS:
*       sys_tick1  - old system tick
*       sys_tick2  - new system tick
*       time_tick1 - old timer tick
*       time_tick2 - new timer tick
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Delta in timer ticks
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_U32 prvDebugTicksUnite
(
    GT_U32 sys_tick1,
    GT_U32 sys_tick2,
    GT_U32 time_tick1,
    GT_U32 time_tick2
)
{
    GT_U32 timer_ticks_per_sys_tick;
    GT_U32 result;     /* result of timer ticks passed */
    unsigned long long result1,result2;

    if (sys_tick1 > sys_tick2)
    {
        return 0;
    }

    timer_ticks_per_sys_tick = sysTimestampPeriod();

    result1 = sys_tick1 * timer_ticks_per_sys_tick + time_tick1;
    result2 = sys_tick2 * timer_ticks_per_sys_tick + time_tick2;

    result = (GT_U32)(result2 - result1); 

    return result;
}

#if defined(PPC_CPU) || defined(POWERPC_CPU)
/*******************************************************************************
* prvDebugPrintStack
*
* DESCRIPTION:
*       Prints stack (PPC)
*
* INPUTS:
*       pc - (pointer to) Program counter
*       sp - (pointer to) Stack pointer
*
* OUTPUTS:
*       None
*
* RETURNS:
*       
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int prvDebugPrintStack
(
    GT_U32 *pc,
    GT_U32 *sp
)
{
    volatile GT_U32 tmp;
    GT_U32 i,j;
    /* Number of previous functions call */
    GT_U32 PRV_OS_DEBUG_prev_func = 16;
    GT_U32 previous_functions[16];
    GT_U32 local_pc, local_sp;

    local_pc = (GT_U32)pc;
    local_sp = (GT_U32)sp;

    /* pc points to last function */
    previous_functions[0] = (GT_U32)local_pc;

    j = 1;

    /* skip first function call - now it's garbage here */
    tmp = local_sp;
    local_sp = *(GT_U32 *)tmp;

    /* reading stack while sp not zero */
    while (((*(GT_U32 *)local_sp) != 0) &&  (j < PRV_OS_DEBUG_prev_func))
    {
      previous_functions[j++] = *(GT_U32 *)(local_sp + 4);

      tmp = local_sp;

      local_sp = *(GT_U32 *)tmp;
    }

    /* Print Last calls stack */
    osPrintSync("\nLast calls stack\n");

    for (i = 0; i < j; i++)
    {
      osPrintSync("%08lx\n", previous_functions[i]);
    }

    return 0;
}

/*******************************************************************************
* prvDebugInterruptExpHndlrPrint
*
* DESCRIPTION:
*       Prints Exception information (PPC)
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
*       None
*
*******************************************************************************/
static int prvDebugInterruptExpHndlrPrint(void)
{
    int i;
  #if 0 /* RazA - 85xx has different exception register implementation.
                  Need to be revised */
    GT_U32 dar,dsisr;
  #endif
    GT_U32 pc,r0,r1,r2,r3,r4,r5,msr,cr;
    char *exc_ptr, *task_name;
    char string[] = "Unknown";

    task_name = taskName(taskIdSelf());

  #if 0 /* RazA - 85xx has different exception register implementation.
                  Need to be revised */
    dar   = PRV_OS_DEBUG_stackPtr->dar;         /* data access register */
    dsisr = PRV_OS_DEBUG_stackPtr->dsisr;       /* data storage interrupt status register */
  #endif

    pc  = PRV_OS_DEBUG_stackPtr->regSet.pc;
    r0  = PRV_OS_DEBUG_stackPtr->regSet.gpr[0]; /* r0 register */
    r1  = PRV_OS_DEBUG_stackPtr->regSet.gpr[1]; /* r1 register */
    r2  = PRV_OS_DEBUG_stackPtr->regSet.gpr[2]; /* r2 register */
    r3  = PRV_OS_DEBUG_stackPtr->regSet.gpr[3]; /* r3 register */
    r4  = PRV_OS_DEBUG_stackPtr->regSet.gpr[4]; /* r4 register */
    r5  = PRV_OS_DEBUG_stackPtr->regSet.gpr[5]; /* r5 register */
    msr = PRV_OS_DEBUG_stackPtr->regSet.msr;    /* machine state register */
    cr  = PRV_OS_DEBUG_stackPtr->regSet.cr;     /* condition register */

    /* if exception type not found set it to "unknown" */
    exc_ptr = string;

    /* find type of exception by it's vector */
    for (i = 0; i< PRV_OS_DEBUG_intr_number_of_exceptions_CNS; i ++)
       if ( PRV_OS_DEBUG_exc_struct[i].exc_vector == PRV_OS_DEBUG_stackPtr->vecOffset)
       {
         exc_ptr =  PRV_OS_DEBUG_exc_struct[i].exc_name;
         break;
       }

    osPrintSync("\nEXCEPTION OCCURRED in task %s: %s exception \n",
                                                        task_name, exc_ptr);
    osPrintSync("Instruction            0x%X\n", pc);

  #if 0 /* RazA - 85xx has different exception register implementation.
                  Need to be revised */
    osPrintSync("Accessed data          0x%X\n", dar);
  #endif

    osPrintSync("Stack pointer          0x%X\n", r1);
    osPrintSync("Machine state register 0x%X\n", msr);
    osPrintSync("Condition register     0x%X\n", cr);
    osPrintSync("General purpose R0     0x%X\n", r0);
    osPrintSync("General purpose R1     0x%X\n", r1);
    osPrintSync("General purpose R2     0x%X\n", r2);
    osPrintSync("General purpose R3     0x%X\n", r3);
    osPrintSync("General purpose R4     0x%X\n", r4);
    osPrintSync("General purpose R5     0x%X\n", r5);

    prvDebugPrintStack ((GT_U32 *)pc, (GT_U32 *)r1);
    
    return 0;
}

/*******************************************************************************
* prvDebugInterruptExcepHndlr
*
* DESCRIPTION:
*       Exception handler hook for PPC,
*
* INPUTS:
*       
*
* OUTPUTS:
*       None
*
* RETURNS:
*       
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int prvDebugInterruptExcepHndlr
(
          int vec,
          ESFPPC *stackPtr,
          void *pRegs
)
{
    volatile int i ;
    void (*IntrFunc) (GT_U32 *);
    GT_U32  vecOffsetLcl;

    /* copy pointer to ESFPPC structure - registers of current task
                                               prior to exception */
    PRV_OS_DEBUG_stackPtr = stackPtr;

    vecOffsetLcl = (GT_U32)stackPtr->vecOffset;
    /* copy new exception handler function in assembler to vector offset */

    /* copy data from R3 to SRR0 */
    *(volatile GT_U32 *)(vecOffsetLcl + 0) = (GT_U32)PRV_OS_DEBUG_intr_srr0_set_opcode_CNS;

    /* copy 0x2(exception is recoverable) to R3 */
    *(volatile GT_U32 *)(vecOffsetLcl + 4) = (GT_U32)PRV_OS_DEBUG_intr_r3_set_opcode_CNS;

    /* copy data from R3 to MSR */
    *(volatile GT_U32 *)(vecOffsetLcl + 8) = (GT_U32)PRV_OS_DEBUG_intr_msr_set_opcode_CNS;

    /* return from interrupt */
    *(volatile GT_U32 *)(vecOffsetLcl + 0xC) = (GT_U32)PRV_OS_DEBUG_intr_rfi_opcode_CNS;

    cacheFlush ( _DATA_CACHE, (void *)(vecOffsetLcl), 0x10);
    cacheInvalidate ( _INSTRUCTION_CACHE, (void *)(vecOffsetLcl), 0x10);

    IntrFunc =  (void *)(vecOffsetLcl);
    IntrFunc((GT_U32 *)prvDebugInterruptExpHndlrPrint);

    for (i=0;i<10000;i++);

    return 0;
}

#elif defined(CPU_ARM)
/*******************************************************************************
* prvDebugPrintStack
*
* DESCRIPTION:
*       Prints stack (ARM)
*
* INPUTS:
*       pc     - (pointer to) Program counter
*       taskId - (pointer to) Task Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int    prvDebugPrintStack
(
    GT_U32 *pc,
    int taskId
)
{
    volatile GT_U32 *sp =0, *lr= 0, *currentIP = 0, frameSize = 0;
    volatile GT_U32 callsCount = 0, i, *spIp = 0,shValue = 0;
    volatile GT_U32 ldr_cmd, mov_cmd, beginOffset, cntSavedRegs;
    TASK_DESC taskDesc;
    volatile WIND_TCB *pTCB;

    char * nameP;
    int    Value;
    SYM_TYPE  Type; 
    /* get information about current task */
    taskInfoGet(taskId, &taskDesc);

    pTCB = taskTcb(taskId);
    if(!pTCB)
        return 0;

    for(i=0 ; i<PRV_OS_DEBUG_prev_func_CNS; i++)
    {
      PRV_OS_DEBUG_function_calls[i].startAddress = 0;
      PRV_OS_DEBUG_function_calls[i].returnAddress = 0;
      PRV_OS_DEBUG_function_calls[i].ip = 0;
      PRV_OS_DEBUG_function_calls[i].fp = 0;
      PRV_OS_DEBUG_function_calls[i].frameSize = 0;
      PRV_OS_DEBUG_function_calls[i].stack = 0;
      PRV_OS_DEBUG_services_previous_functions[i] = 0;
    }

    sp = (GT_U32*)(pTCB->pExcRegSet->r[13]); /*td_sp*/
    lr = (GT_U32*)(pTCB->pExcRegSet->pc);

    while((sp < (GT_U32*)taskDesc.td_pStackBase) &&
          (sp > (GT_U32*)taskDesc.td_pStackEnd))
    {
      frameSize    = 0;
      beginOffset  = 0;
      cntSavedRegs = 0;

      /* It finds function start. Function start is str or stm command. */
      for(currentIP = lr;;currentIP--)
      {
          if(STR_CMD == *currentIP)
          {
              cntSavedRegs = 1;
              PRV_OS_DEBUG_function_calls[callsCount].startAddress = (GT_U32 *)currentIP;
              break;
          }
          if(STM_CMD == ((*currentIP) & STM_CMD_MASK))
          {
              if(((*currentIP) & 0x4000))
              {
                for(i=0; i<16; i++)
                {
                    cntSavedRegs += (((*currentIP) & 0xFFFF) >> i) & 1;
                }
                PRV_OS_DEBUG_function_calls[callsCount].startAddress = (GT_U32 *)currentIP;
                break;
              }
          }
      }
   /*!
       * It finds end of fuction. The sign of fuction end is command ldm.
       * But if function is cycled, it can have no end defined by str command.
       * In this case end of function is unreachable.
       * Formal end of finding is next fucntion.
       */
      for(currentIP = lr;;currentIP++)
      {
          if(LDM_CMD == ((*currentIP) & LDM_CMD_MASK))
          {
              PRV_OS_DEBUG_function_calls[callsCount].endAddress = (GT_U32 *)currentIP;
              break;
          }
          if((STM_CMD == ((*currentIP) & STM_CMD_MASK)) ||
             (STR_CMD == *currentIP))
          {
              PRV_OS_DEBUG_function_calls[callsCount].endAddress = 0;
              break;
          }
      }
      /* Look for "sub sp, sp, CNS" in current function. */
      for(currentIP = (PRV_OS_DEBUG_function_calls[callsCount].startAddress-1); currentIP < lr;
          currentIP ++)
      {
          if(SUB_SP_CMD == ((*currentIP) & SUB_SP_MASK))
          {
              if(0 != ((*currentIP) & 0x2000000)) /*Check operand 2 type*/
              {/*In case of constant - occures when number of local variables is low */
                  i = (*currentIP) & 0xFFF;
                  shValue = 32-(2 * (i >> 8));
                  if(32 != shValue )
                  {
                      frameSize += ((i & 0xFF) << shValue) >> 2 ;
                  }
                  else
                  {
                      frameSize += i >> 2;
                  }
                  if(SUB_SP_CMD == (*(currentIP+1) & 0xFFFFF000) )
                  {
                      frameSize += (*(currentIP + 1) & 0xFF) >> 2;
                  }
              }
              else
              {/* In case of register - MOV or LDR commands can be used */
                  mov_cmd = (MOV_CMD | (((*currentIP) & 0xF) << 12));
                  ldr_cmd = (LDR_CMD | (((*currentIP) & 0xF) << 12));
                  for(spIp = currentIP;
                      spIp > PRV_OS_DEBUG_function_calls[callsCount].startAddress; spIp--)
                  {
                      if( mov_cmd == ((*spIp) & MOV_CMD_MASK))
                      {
                          i = (*spIp) & 0xFFF;
                          shValue = 32-(2 * (i >> 8));
                          if(32 != shValue )
                          {
                              frameSize += ((i & 0xFF) << shValue) >> 2 ;
                          }
                          else
                          {
                              frameSize += i >> 2;
                          }
                          break;
                      }
                      if( ldr_cmd == ((*spIp) & LDR_CMD_MASK))
                      {
                          i = (*spIp) & 0xFFF;
                          frameSize += *(spIp + ((i + 8) >> 2)) >> 2;
                          break;
                      }
                  }
              }
              if(currentIP == (PRV_OS_DEBUG_function_calls[callsCount].startAddress-1))
              {
                  beginOffset = frameSize;
              }
              else
              {
                  break;
              }
          }
      }

      frameSize += cntSavedRegs;
      if(beginOffset !=0) PRV_OS_DEBUG_function_calls[callsCount].startAddress--;

      PRV_OS_DEBUG_function_calls[callsCount].frameSize = frameSize;
      PRV_OS_DEBUG_function_calls[callsCount].returnAddress = (GT_U32 *)lr;
      sp += frameSize;
      lr =(GT_U32*)(*(sp-1-beginOffset));
      callsCount += 1;

      if( ((GT_U32)lr < 0x00100000) ||
          ((GT_U32)lr > (GT_U32)sysPhysMemTop()) )
      {
          break;
      }

      if(0 == *lr )
      {
          break;
      }

    }

    for(i=0; i<callsCount; i++)
    {
      symByValueFind(sysSymTbl,(GT_U32)PRV_OS_DEBUG_function_calls[i].startAddress,
                   &nameP,&Value,&Type);
      osPrintSync("0x%08lx - %s\n", (GT_U32)PRV_OS_DEBUG_function_calls[i].startAddress,nameP);
    }

    return 0;
}

/*******************************************************************************
* prvDebugInterruptExcepHndlr
*
* DESCRIPTION:
*       Exception handler hook for ARM. 
*
* INPUTS:
*       
*
* OUTPUTS:
*       None
*
* RETURNS:
*       
*
* COMMENTS:
*       None
*
*******************************************************************************/
static int prvDebugInterruptExcepHndlr
(
    int  task,
    int  vecNum,
    ESF *stackPtr
)
{
    int i;
    GT_U32 *pc,psr,exp;
    char *task_name;
    char string[] = "Unknown";
    char *exc_ptr = string;

    intLock();    /* disable interrupts */
    taskLock();   /* disable task switching */

    task_name = (char *)taskName(task);

    pc  = stackPtr->pc;
    psr = stackPtr->cpsr;    /* machine state register */
    exp = stackPtr->vecAddr;

    /* find type of exception by it's vector */
    for (i = 0; i< PRV_OS_DEBUG_intr_number_of_exceptions_CNS; i ++)
       if ( PRV_OS_DEBUG_exc_struct[i].exc_vector == stackPtr->vecAddr)
       {
         exc_ptr =  PRV_OS_DEBUG_exc_struct[i].exc_name;
         break;
       }

    /* if exception type not found set it to "unknown" */
    if (i == PRV_OS_DEBUG_intr_number_of_exceptions_CNS)
           exc_ptr = string;

    osPrintSync("\n\n ===================================================\n");
    osPrintSync("FATAL ERROR: %s: %s exception \n",task_name, exc_ptr);
    osPrintSync("Instruction            0x%X\n", pc);
    osPrintSync("Exception vector       0x%X\n", exp);
    osPrintSync("Program state register 0x%X\n", psr);
    prvDebugPrintStack(pc,task);

    osPrintSync(" ===================================================\n\n\n");
    return 0;
}
#endif

/*******************************************************************************
* prvDebugCalcTask
*
* DESCRIPTION:
*       Debug calculation task
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
*       None
*
*******************************************************************************/
unsigned __TASKCONV prvDebugCalcTask(GT_VOID *paramsPtr)
{
    GT_U32 intVal, i, j;
    GT_U32 *accomulator = NULL, *task_id = NULL;
    GT_U32 utilize_idx, accomulator_idx;
    prvOsDebug_monitor_trace_STC  utilization_last_sample;
    GT_U32 overall_ticks;
    GT_BOOL first_time = GT_TRUE, allocate_ok1 = GT_FALSE, allocate_ok2 = GT_FALSE;
    GT_U32 temp =0;

    /* this task should be suspended immediately after creation & start */
    if(first_time)
    {
        first_time = FALSE;
        if (taskSuspend(0) == ERROR)
        {
            DBG(("\prvDebugCalcTask error: suspend utilization task failed .\n\n "));
        }
    }

    /* allocate space for internals arrays */
    do
    {
        if(!allocate_ok1)
        {
            /* this array is used so store all tasks current utilization */
            accomulator = malloc(prvOsDebug_num_of_tasks_LCL * sizeof(GT_U32));

            if(accomulator != NULL)
                allocate_ok1 = GT_TRUE;
            else
                osTimerWkAfter(200);
        }

        if(!allocate_ok2)
        {
            task_id = malloc(prvOsDebug_num_of_tasks_LCL * sizeof(GT_U32));

            if(task_id != NULL)
                allocate_ok2 = GT_TRUE;
            else
                osTimerWkAfter(200);
        }

    }while((!allocate_ok1) || (!allocate_ok2)); /* make sure allocation is good */

    /* run always */
    for( ; ; )
    {

        /* if utilization service is not used, suspend the task */
        if(prvOsDebug_cpu_utilization_activated_LCL == GT_FALSE)
        {
            if (taskSuspend(0) == ERROR)
            {
                DBG(("\prvDebugCalcTask error: suspend utilization task failed .\n\n "));
            }
        }

        /* make sure the events table isn't empty */
        if(prvOsDebug_cpu_utilization_tbl_idx_LCL >= 2)
        {
            memset(accomulator, 0, prvOsDebug_num_of_tasks_LCL * sizeof(GT_U32));
            memset(task_id, 0, prvOsDebug_num_of_tasks_LCL * sizeof(GT_U32));

            /* lock interrupts */
            intVal = intLock();

            /* sample the current time */
            prvDebugMonitorFillTraceTimeEntry(&utilization_last_sample);

            accomulator_idx = 0;

            /* ignore last sample since it is the current task */
            utilize_idx = prvOsDebug_cpu_utilization_tbl_idx_LCL - 2;

            /* step over the last second time table and accumulate time for */
            /* all active tasks in it */
            for(i=0; (i<prvOsDebug_cpu_utilization_tbl_idx_LCL-1) &&
                                                 (utilize_idx != 0xFFFFFFFF); i++)
            {
                /* accumulate only tasks, skip interrupts events */
                if(!(prvOsDebug_utilization_tbl_LCL[utilize_idx].event_id & INT_START_BIT))
                {
                    for(j=0; j<accomulator_idx; j++)
                    {
                        /* search if this entry's task was listed on current tasks array */
                        if(prvOsDebug_utilization_tbl_LCL[utilize_idx].event_id ==
                                                                       task_id[j])
                            break;
                    }

                    /* calculate event time and store it in array */
                    accomulator[j] += prvDebugTicksUnite(
                          prvOsDebug_utilization_tbl_LCL[utilize_idx].sys_tick,
                          prvOsDebug_utilization_tbl_LCL[utilize_idx+1].sys_tick,
                          prvOsDebug_utilization_tbl_LCL[utilize_idx].sys_count,
                          prvOsDebug_utilization_tbl_LCL[utilize_idx+1].sys_count);

                    if(j == accomulator_idx)
                    {
                        /* the entry's task is not listed yet in task array - put it in */
                        task_id[accomulator_idx] =
                              prvOsDebug_utilization_tbl_LCL[utilize_idx].event_id;

                        /* prepare to a new nu-listed task */
                        accomulator_idx++;
                    }
                }

                /* step to next event entry */
                utilize_idx--;
            }

            /* calculate overall timer ticks from last utilization task operation */
            overall_ticks = prvDebugTicksUnite(
                              prvOsDebug_utilization_second_start_LCL.sys_tick,
                              utilization_last_sample.sys_tick,
                              prvOsDebug_utilization_second_start_LCL.sys_count,
                              utilization_last_sample.sys_count);


            /* update all tasks's last second utilizations */
            for(i=0; i<prvOsDebug_num_of_tasks_LCL; i++)
            {
                /* search if task was listed above */
                for(j=0; j<accomulator_idx; j++)
                {
                    if(prvOsDebug_task_tbl_LCL[i].task_id == task_id[j])
                        break;
                }
                
                if(j == accomulator_idx)
                {
                    /* this task wasn't sample during last second */
                    prvOsDebug_task_tbl_LCL[i].sec_utilization
                                [prvOsDebug_tasks_tbl_idx_LCL] = (GT_U8)0;
                }
                else
                {
                    /* calculate utilization and put it in task's table */
                    prvOsDebug_task_tbl_LCL[i].sec_utilization
                                [prvOsDebug_tasks_tbl_idx_LCL] =
                                   (GT_U8)((accomulator[j] * 100) / overall_ticks);
                }
            }
            temp = 0;
            /* update all tasks's last second utilizations */
            for(i=0; i<prvOsDebug_num_of_tasks_LCL; i++)
            {
                temp += prvOsDebug_task_tbl_LCL[i].sec_utilization
                                [prvOsDebug_tasks_tbl_idx_LCL];
            }

            /* increament 200-mili seconds index */
            prvOsDebug_tasks_tbl_idx_LCL++;

            /* check if we reached the end of ring */
            if(prvOsDebug_tasks_tbl_idx_LCL == PRV_OS_DEBUG_200_MILISEC_IN_5_MIN_CNS)
            {
                prvOsDebug_tasks_tbl_idx_LCL = 0;
                prvOsDebug_utilization_five_min_passed_LCL = TRUE;
            }

            if(prvOsDebug_tasks_tbl_idx_LCL > PRV_OS_DEBUG_200_MILISEC_IN_1_MIN_CNS)
                prvOsDebug_utilization_one_min_passed_LCL = TRUE;
          
            if(prvOsDebug_tasks_tbl_idx_LCL > PRV_OS_DEBUG_200_MILISEC_IN_5_SEC_CNS)
                prvOsDebug_utilization_five_sec_passed_LCL = TRUE;
    

            /* clear events table */
            memset(prvOsDebug_utilization_tbl_LCL, 0,
                   sizeof(prvOsDebug_monitor_trace_STC) * prvOsDebug_cpu_utilization_num_of_items_LCL);

            /* reset events index */
            prvOsDebug_cpu_utilization_tbl_idx_LCL = 0;

            /* update time of new second */
            prvOsDebug_utilization_second_start_LCL.sys_tick =
                                                 utilization_last_sample.sys_tick;

            prvOsDebug_utilization_second_start_LCL.sys_count =
                                                 utilization_last_sample.sys_count;

            /* restore interrupts */
            intUnlock(intVal);
        }

        /* sleep 200 milliseconds until next sample */
        osTimerWkAfter(200);
    }
}

/*******************************************************************************
* prvDebugIdleTask
*
* DESCRIPTION:
*       Idle task
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
*       None
*
*******************************************************************************/
unsigned __TASKCONV prvDebugIdleTask(GT_VOID *paramsPtr)
{
    GT_U32 tid;
    for ( ; ; )
    {
        tid = taskIdSelf();
    }
}

/*******************************************************************************
* osDebugServicesInit
*
* DESCRIPTION:
*       Initializes the Debug Services.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugServicesInit(GT_VOID)
{
    GT_STATUS rc;

    if (dbgServicesInitialized == GT_FALSE)
    {    
#if defined(PPC_CPU) || defined(POWERPC_CPU)
        /*the following hook, hooks our function to be  */
        /*invoked BEFORE vxWorks handles the exception. */
        _func_excBaseHook = prvDebugInterruptExcepHndlr;
#elif defined(CPU_ARM)
        excHookAdd ((FUNCPTR)prvDebugInterruptExcepHndlr);
#endif

        rc = osTaskCreate("HDEB",100,0x10000,prvDebugCalcTask,NULL,&prvOsDebug_utilize_task_id_LCL);
        if (rc != GT_OK)
        {
            return rc;
        }
    
        rc = osTaskCreate("IDLE",255,0x1000,prvDebugIdleTask,NULL,&prvOsDebug_idle_task_id_LCL);
        if (rc != GT_OK)
        {
            return rc;
        }
        dbgServicesInitialized = GT_TRUE;
    }

    return GT_OK;
}


/*******************************************************************************
* osDebugTaskUtilStart
*
* DESCRIPTION:
*       Starts task utilization measurement.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugTaskUtilStart(GT_VOID)
{

    GT_U32 intVal;
    int id_list[PRV_OS_DEBUG_MAX_TASKS_NUM_CNS];
    int *id_list_ptr = id_list;
    GT_U32 task_num = 0, i;
    GT_STATUS rc;

    /* check that Debug Services initialized*/
    if (dbgServicesInitialized == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }

    if(!prvOsDebug_cpu_utilization_activated_LCL)
        prvOsDebug_cpu_utilization_activated_LCL = TRUE;
    else
    {
        return GT_OK;
    }

    /* initiate values to the events table */
    prvOsDebug_cpu_utilization_num_of_items_LCL =
                                      PRV_OS_DEBUG_UTILIZATION_ENTRIES_CNS;
    prvOsDebug_cpu_utilization_tbl_idx_LCL = 0;
    prvOsDebug_utilization_five_sec_passed_LCL = FALSE;
    prvOsDebug_utilization_one_min_passed_LCL = FALSE;
    prvOsDebug_utilization_five_min_passed_LCL = FALSE;

    /* allocate space for events table */
    prvOsDebug_utilization_tbl_LCL = malloc(sizeof(prvOsDebug_monitor_trace_STC) *
                                    prvOsDebug_cpu_utilization_num_of_items_LCL);

    if(prvOsDebug_utilization_tbl_LCL == NULL)
    {
        DBG(("\osDebugTaskUtilStart error: could not allocate memory for utilization table.\n"));
        return GT_FAIL;
    }

    memset(prvOsDebug_utilization_tbl_LCL, 0, sizeof(prvOsDebug_monitor_trace_STC) *
                                    prvOsDebug_cpu_utilization_num_of_items_LCL);

    /* get task list */
    task_num = taskIdListGet(id_list_ptr, PRV_OS_DEBUG_MAX_TASKS_NUM_CNS);

    prvOsDebug_num_of_tasks_LCL = task_num;

    /* allocate space for the tasks table */
    prvOsDebug_task_tbl_LCL = malloc(sizeof(prvOsDebug_task_tbl_STC) * task_num);

    if(prvOsDebug_task_tbl_LCL == NULL)
    {
        DBG(("\osDebugTaskUtilStart error: could not allocate memory for tasks table.\n"));
        return GT_FAIL;
    }

    /* initiate all tasks entries in table    */
    for(i=0; i<task_num; i++)
    {
        prvOsDebug_task_tbl_LCL[i].task_id = id_list[i];
        memset(prvOsDebug_task_tbl_LCL[i].sec_utilization, 0,
                                               PRV_OS_DEBUG_200_MILISEC_IN_5_MIN_CNS);
    }

    /* initiate tasks table params */
    prvOsDebug_tasks_tbl_idx_LCL = 0;
    prvOsDebug_tasks_tbl_full_LCL = 0;

    /* Disable interrupts */
    intVal = intLock();

    rc = prvDebugHookAdd();
    if (rc != GT_OK)
    {
        return rc;
    }

    prvDebugMonitorFillTraceTimeEntry(&prvOsDebug_utilization_second_start_LCL);

    /* resume utilization task operation */
    taskResume(prvOsDebug_utilize_task_id_LCL);

    /* Enable interrupts */
    intUnlock(intVal);

    return GT_OK;
}

/*******************************************************************************
* osDebugTaskUtilStop
*
* DESCRIPTION:
*       Stops task utilization measurement.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugTaskUtilStop(GT_VOID)
{
    GT_U32 intVal;

    /* check that Debug Services initialized*/
    if (dbgServicesInitialized == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }

    intVal = intLock();
  
    if(prvOsDebug_cpu_utilization_activated_LCL)
        prvOsDebug_cpu_utilization_activated_LCL = FALSE;
    else
    {
        DBG(("\osDebugTaskUtilStop error: CPU utilization was not activated.\n\n "));
        intUnlock(intVal);
        return GT_FAIL;
    }
  
    /* free dynamic tables */
    free(prvOsDebug_utilization_tbl_LCL);
    free(prvOsDebug_task_tbl_LCL);
  
    /* suspend utilization task */
    if (taskSuspend(prvOsDebug_utilize_task_id_LCL) == ERROR)
    {
        DBG(("\osDebugTaskUtilStop error: suspend utilization task failed .\n\n "));
        intUnlock(intVal);
        return GT_FAIL;
    }
  
    prvDebugCheckHookDelete();

    intUnlock(intVal);
  
    return GT_OK;
}

/*******************************************************************************
* osDebugTaskUtilGet
*
* DESCRIPTION:
*       Gets task utilization measurement for given task.
*
* INPUTS:
*       taskId - ID of task whose measurement is to be given.
*
* OUTPUTS:
*       utilizationPtr - (pointer to) utilization measurement.
*
* RETURNS:
*       GT_OK    - on success
*       GT_NOT_INITIALIZED - task utilization isn't started
*       GT_NOT_FOUND - given task ID is not found
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugTaskUtilGet
(
    IN GT_TASK  taskId,
    OUT OS_DEBUG_UTIL_MEAS_STC *utilizationPtr
)
{
    GT_U32   utilization;
    GT_U32   i, sec_idx, Task_idx;
    GT_U32   accomulator;
    GT_U32   intVal;
    GT_U32   num_of_items, num_of_items_per_min;

    /* check that Debug Services initialized*/
    if (dbgServicesInitialized == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }

    if (utilizationPtr == NULL)
    {
        return GT_BAD_PTR;
    }

    if(!prvOsDebug_cpu_utilization_activated_LCL)
        return GT_NOT_INITIALIZED;

    /* find task's index in tasks table */
    for(i=0; i<prvOsDebug_num_of_tasks_LCL; i++)
    {
        if(prvOsDebug_task_tbl_LCL[i].task_id == taskId)
            break;
    }

    if(i == prvOsDebug_num_of_tasks_LCL)
        return GT_NOT_FOUND;
    else
        Task_idx = i;

    /* Disable interrupts */
    intVal = intLock();

    /* calculate last five minutes utilization from the average of the task's */
    /* seconds utilizations entries */

    /* sum all seconds utilizations */
    if(prvOsDebug_utilization_five_min_passed_LCL)
        num_of_items = PRV_OS_DEBUG_200_MILISEC_IN_5_MIN_CNS;
    else
    {
        /* seems wrong if entry = 1 sec */
        if(prvOsDebug_tasks_tbl_idx_LCL > PRV_OS_DEBUG_200_MILISEC_IN_SEC_CNS)
            num_of_items = prvOsDebug_tasks_tbl_idx_LCL -1;
        else
        {
            /* the utilization is active for less then a second */
            utilizationPtr->utilize_5_sec = 0;
            utilizationPtr->utilize_min = 0;

            intUnlock(intVal);
            return GT_OK;
        }
    }

    /* Check 5 mins have passed. Otherwise we get N/A */
    if(prvOsDebug_utilization_five_min_passed_LCL)
    {
        accomulator = 0;
        sec_idx = prvOsDebug_tasks_tbl_idx_LCL - 1;

        for(i=0; i<num_of_items; i++)
        {
            if(sec_idx == 0xFFFFFFFF)
                sec_idx = PRV_OS_DEBUG_200_MILISEC_IN_5_MIN_CNS - 1;

            accomulator += (GT_U32)prvOsDebug_task_tbl_LCL[Task_idx].
                                                          sec_utilization[sec_idx];
            sec_idx--;
        }

        /* average all seconds */
        utilization = accomulator / num_of_items;

        /* can't be over 100 precent */
        if((GT_U32)utilization > 100)
        {
            intUnlock(intVal);
            return GT_FAIL;
        }
        utilizationPtr->utilize_5_min = (GT_U32)utilization;
    }
    else
        utilizationPtr->utilize_5_min = OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS;

    /* calculate last one minutes utilization from the avarage of */
    /* the task's seconds utilizations entries */
    if(prvOsDebug_utilization_one_min_passed_LCL)
    {
        accomulator = 0;

        sec_idx = prvOsDebug_tasks_tbl_idx_LCL - 1;

        if(num_of_items > PRV_OS_DEBUG_200_MILISEC_IN_1_MIN_CNS)
            num_of_items_per_min = PRV_OS_DEBUG_200_MILISEC_IN_1_MIN_CNS;
        else
            num_of_items_per_min = num_of_items;

        for(i=0; i<num_of_items_per_min; i++)
        {
            if(sec_idx == 0xFFFFFFFF)
                sec_idx = PRV_OS_DEBUG_200_MILISEC_IN_5_MIN_CNS - 1;

            accomulator += (GT_U32)prvOsDebug_task_tbl_LCL[Task_idx].
                                                        sec_utilization[sec_idx];
            sec_idx--;
        }

        /* avarage all seconds */
        utilization = accomulator / num_of_items_per_min;

        /* can't be over 100 precent */
        if((GT_U32)utilization > 100)
        {
            intUnlock(intVal);
            return GT_FAIL;
        }

        utilizationPtr->utilize_min = (GT_U32)utilization;
    }
    else
      utilizationPtr->utilize_min = OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS;

    /* calculate last 5 second utilization from the average of the task's */
    /*    200-mili seconds utilizations entries */
    if(prvOsDebug_utilization_five_sec_passed_LCL)
    {
        accomulator = 0;

        sec_idx = prvOsDebug_tasks_tbl_idx_LCL - 1;

        for(i=0; i<PRV_OS_DEBUG_200_MILISEC_IN_5_SEC_CNS; i++)
        {
            if(sec_idx == 0xFFFFFFFF)
                sec_idx = PRV_OS_DEBUG_200_MILISEC_IN_5_MIN_CNS - 1;

            accomulator += (GT_U32)prvOsDebug_task_tbl_LCL[Task_idx].
                                                        sec_utilization[sec_idx];
            sec_idx--;
        }

        /* average all seconds */
        utilization = accomulator / PRV_OS_DEBUG_200_MILISEC_IN_5_SEC_CNS;

        /* can't be over 100 precent */
        if((GT_U32)utilization > 100)
        {
            intUnlock(intVal);
            return GT_FAIL;
        }

        utilizationPtr->utilize_5_sec = (GT_U32)utilization;

    } /* if (prvOsDebug_utilization_five_sec_passed_LCL) */
    else
        utilizationPtr->utilize_5_sec = OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS;      

    intUnlock(intVal);

    return GT_OK;
}

/*******************************************************************************
* prvDebugUtilPrint
*
* DESCRIPTION:
*       Prints task utilization
*
* INPUTS:
*       namePtr
*       infoPtr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static void prvDebugUtilPrint
(
    IN GT_U8 * namePtr,
    IN OS_DEBUG_UTIL_MEAS_STC *infoPtr
)
{
    if((infoPtr->utilize_5_min == OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS) &&
       (infoPtr->utilize_min == OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS)   &&
       (infoPtr->utilize_5_sec == OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS) )
    {
        osPrintf(" %-20s%-10d%-10s%-10s\n",
                 namePtr, "N/A", "N/A", "N/A");
    }

    else if((infoPtr->utilize_5_min == OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS) &&
            (infoPtr->utilize_min == OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS) )
    {
        osPrintf(" %-20s%-10d%-10s%-10s\n",
         namePtr, infoPtr->utilize_5_sec, "N/A", "N/A");
    }
    else if(infoPtr->utilize_5_min == OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS)
    {
        osPrintf(" %-20s%-10d%-10d%-10s\n",
         namePtr, infoPtr->utilize_5_sec, infoPtr->utilize_min, "N/A");
    }
    else
    {
        osPrintf(" %-20s%-10d%-10d%-10d\n",
         namePtr, infoPtr->utilize_5_sec, infoPtr->utilize_min, infoPtr->utilize_5_min);
    }
}



/*******************************************************************************
* osDebugTaskUtilPrint
*
* DESCRIPTION:
*       Prints tasks utilization measurements.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS  osDebugTaskUtilPrint(void)
{

    GT_U32 task_num = 0, i;
    int id_list[PRV_OS_DEBUG_MAX_TASKS_NUM_CNS];
    int *id_list_ptr = id_list;
    GT_STATUS status;
    OS_DEBUG_UTIL_MEAS_STC info, isrInfo;
    GT_BOOL printIsrUtil = GT_TRUE;

    /* check that Debug Services initialized*/
    if (dbgServicesInitialized == GT_FALSE)
    {
        osPrintf("\nDebug Services not initialized");
        return GT_FAIL;
    }

    if(!prvOsDebug_cpu_utilization_activated_LCL)
    {
        osPrintf("\nCPU utilization service was not started yet");
        return GT_FAIL;
    }

    task_num = taskIdListGet(id_list_ptr, PRV_OS_DEBUG_MAX_TASKS_NUM_CNS);

    osPrintf("\n\n %-20s%-10s%-10s%-10s\n", "TASK NAME",
             "5-seconds", "minute", "5-minutes");
    osPrintf(" ---------------------------------------------\n");
    /* get CPU utilization */
    status = osDebugCpuUtilGet(&info);
    if (status == GT_OK)
    {
        prvDebugUtilPrint("CPU utilization",&info);
    }
    else
    {
        osPrintf("CPU Utilization read error\n");
    }

    osPrintf(" ---------------------------------------------\n");

    isrInfo.utilize_5_min = isrInfo.utilize_min = isrInfo.utilize_5_sec = 100;

    for(i=0;i<task_num;i++)
    {
        status = osDebugTaskUtilGet(id_list[i], &info);

        if(status == GT_OK)
        {
            prvDebugUtilPrint(taskName(id_list[i]),&info);

            if (info.utilize_5_sec != OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS && 
                isrInfo.utilize_5_sec != OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS)
            {
                isrInfo.utilize_5_sec -= info.utilize_5_sec;
            }
            else
            {
                isrInfo.utilize_5_sec = OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS;
            }

            if (info.utilize_min != OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS && 
                isrInfo.utilize_min != OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS)
            {
                isrInfo.utilize_min -= info.utilize_min;
            }
            else
            {
                isrInfo.utilize_min = OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS;
            }

            if (info.utilize_5_min != OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS && 
                isrInfo.utilize_5_min != OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS)
            {
                isrInfo.utilize_5_min -= info.utilize_5_min;
            }
            else
            {
                isrInfo.utilize_5_min = OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS;
            }
            
        }
        else
        {
            printIsrUtil = GT_FALSE;
            osPrintf(" %-20s Utilization read error\n",taskName(id_list[i]));
        }
            
    }
    if (printIsrUtil == GT_TRUE)
    {
        prvDebugUtilPrint("ISR",&isrInfo);
    }

    return GT_OK;
}

/*******************************************************************************
* osDebugCpuUtilGet
*
* DESCRIPTION:
*       Gets CPU utilization measurement.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       utilizationPtr - (pointer to) utilization measurement.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugCpuUtilGet
(
    OUT OS_DEBUG_UTIL_MEAS_STC *utilizationPtr
)
{
    OS_DEBUG_UTIL_MEAS_STC idleUtil;
    GT_STATUS rc;

    /* check that Debug Services initialized*/
    if (dbgServicesInitialized == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }

    rc = osDebugTaskUtilGet(prvOsDebug_idle_task_id_LCL,&idleUtil);
    if (rc != GT_OK)
    {
        return rc;
    }

    utilizationPtr->utilize_5_sec = (idleUtil.utilize_5_sec == OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS) ?
        OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS : 100 - idleUtil.utilize_5_sec ;
    utilizationPtr->utilize_min = (idleUtil.utilize_min == OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS) ?
        OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS : 100 - idleUtil.utilize_min ;
    utilizationPtr->utilize_5_min = (idleUtil.utilize_5_min == OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS) ?
        OS_DEBUG_UTILIZATION_NOT_AVAILABLE_CNS : 100 - idleUtil.utilize_5_min ;

    return GT_OK;
}

/*******************************************************************************
* osDebugFreeMemGet
*
* DESCRIPTION:
*       Gets the free memory in the system.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       freeMemPtr - (pointer to) free memory in Kbytes.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osDebugFreeMemGet
(
    OUT GT_U32 *freeMemPtr
)
{
    GT_U32 size, size2, *memptr;

    size = memFindMax();

    /* Check the sum of the 2 biggest block in town */
    memptr = malloc(size);

    if (memptr)
    {
        size2 = memFindMax();
        size += size2;
        free(memptr);
    }

    *freeMemPtr = size;

    return GT_OK;
}

