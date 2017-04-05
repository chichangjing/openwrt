/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* SerdesRxOptimizer.h
*
* DESCRIPTION:
*       Serdes optimizer API definition
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
******************************************************************************/

#ifndef __serdesRxOpt_H
#define __serdesRxOpt_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gtOs/gtGenTypes.h>

/* PRBS used by system */
typedef enum
{
    PRBS_7,
    PRBS_15,
    PRBS_23,
    PRBS_31
} MV_PRBS_TYPE;

/* test modes definition */
typedef enum
{
    AccurTimeAware = 0, /* Accuracy/Time aware (mode 0) */
    AccurAware,         /* Accuracy aware (mode 1) */
    AccurAwareShort,    /* Accuracy aware short - no sampler tuning(mode 2) */
    TimeAware           /* Time aware (mode 3) */

}MV_SERDES_OPT_MODE;

/* results report modes*/
typedef enum
{
    Silence,    /* don't print anything */
    Short,      /* print short status per lane */
    Verbose     /* print each phase results and status per lane */

}MV_SERDES_OPT_REPORT_TYPE;

typedef struct
{
    GT_U32 dfeBestVal;
    GT_U32 ffeRBestVal;
    GT_U32 ffeCBestVal;
    GT_32  samplerWindow;
    GT_U32 samplerCenter;
    GT_U32 sqlchVal;

    GT_BOOL ffeRFittedAttenuation;
    GT_BOOL ffeCFittedAttenuation;
    GT_BOOL volatgeBerRisk;
    GT_BOOL timeBerRisk;
    GT_BOOL signalDetectFail;
    GT_BOOL prbsNotLocked;

}MV_SERDES_OPT_RESULTS;

/* os wrapper function prototypes */
/*******************************************************************************
* MV_SOPT_OS_MALLOC_FUNC
*
* DESCRIPTION:
*       Allocates memory block of specified size.
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*******************************************************************************/
typedef void * (*MV_SOPT_OS_MALLOC_FUNC)
(
    GT_U32 size
);
/*******************************************************************************
* MV_SOPT_OS_FREE_FUNC
*
* DESCRIPTION:
*       De-allocates or frees a specified memory block.
*
* INPUTS:
*       memblock - previously allocated memory block to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*******************************************************************************/
typedef void (*MV_SOPT_OS_FREE_FUNC)
(
    IN void* const memblock
);

/*******************************************************************************
* MV_SOPT_OS_MEM_SET_FUNC
*
* DESCRIPTION:
*       Stores 'symbol' converted to an unsigned char in each of the elements
*       of the array of unsigned char beginning at 'start', with size 'size'.
*
* INPUTS:
*       start  - start address of memory block for setting
*       symbol - character to store, converted to an unsigned char
*       size   - size of block to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to set memory block
*
*******************************************************************************/
typedef void * (*MV_SOPT_OS_MEM_SET_FUNC)
(
    IN void * start,
    IN int    symbol,
    IN GT_U32 size
);

/*******************************************************************************
* MV_SOPT_OS_MEM_CPY_FUNC
*
* DESCRIPTION:
*       Copies 'size' characters from the object pointed to by 'source' into
*       the object pointed to by 'destination'. If copying takes place between
*       objects that overlap, the behavior is undefined.
*
* INPUTS:
*       destination - destination of copy
*       source      - source of copy
*       size        - size of memory to copy
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Pointer to destination
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_VOID * (*MV_SOPT_OS_MEM_CPY_FUNC)
(
    IN GT_VOID *       destination,
    IN const GT_VOID * source,
    IN GT_U32          size
);

/*******************************************************************************
* MV_SOPT_OS_TIME_WK_AFTER_FUNC
*
* DESCRIPTION:
*       Puts current task to sleep for specified number of millisecond.
*
* INPUTS:
*       mils - time to sleep in milliseconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
*******************************************************************************/
typedef GT_STATUS (*MV_SOPT_OS_TIME_WK_AFTER_FUNC)
(
    IN GT_U32 mils
);

/*******************************************************************************
* MV_OS_EXACT_DELAY_FUNC
*
* DESCRIPTION:
*       Implement exact time delay for specified number of millisecond.
*
* INPUTS:
*       mils - time to delay in milliseconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
*******************************************************************************/
typedef GT_STATUS (*MV_SOPT_OS_EXACT_DELAY_FUNC)
(
    IN GT_U8  devNum,
    IN GT_U32 portGroup,
    IN GT_U32 mils
);

/*******************************************************************************
* MV_SOPT_OS_IO_PRINTF_FUNC
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream.
*
* INPUTS:
*       format  - format string to write
*       ... - parameters of the 'format'
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef int (*MV_SOPT_OS_IO_PRINTF_FUNC)(
    IN  const char* format,
    IN  ...
);

/*******************************************************************************
* MV_SOPT_OS_IO_SPRINTF_FUNC
*
* DESCRIPTION:
*       Write a formatted string to a buffer.
*
* INPUTS:
*       buffer  - buffer to write to
*       format  - format string
*       ... - parameters of the 'format'
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters copied to buffer, not including
*       the NULL terminator.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef int (*MV_SOPT_OS_IO_SPRINTF_FUNC)(
    IN  char * buffer,
    IN  const char* format,
    IN  ...
);

/*******************************************************************************
* MV_SOPT_OS_SIG_SEM_BIN_CREATE_FUNC
*
* DESCRIPTION:
*       Create and initialize a binary semaphore for signaling between tasks or
*       between Interrupt context and tasks.
*
* INPUTS:
*       name   - "signaling" semaphore Name
*       init   - init value of "signaling" semaphore:
*                   GT_FALSE - empty, GT_TRUE - full
*
* OUTPUTS:
*       smidPtr - (pointer to)semaphore "signaling" Id
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       see also :
*       1. CPSS_OS_SIG_SEM_BIN_CREATE_FUNC
*       2. CPSS_OS_SIG_SEM_DELETE_FUNC
*       3. CPSS_OS_SIG_SEM_WAIT_FUNC
*       4. CPSS_OS_SIG_SEM_SIGNAL_FUNC
*
*******************************************************************************/
typedef GT_STATUS (*MV_SOPT_OS_SIG_SEM_BIN_CREATE_FUNC)
(
    IN  GT_8        *name,
    IN  GT_BOOL     init,
    OUT GT_UINTPTR  *smidPtr
);

/*******************************************************************************
* MV_SOPT_OS_SIG_SEM_DELETE_FUNC
*
* DESCRIPTION:
*       Delete "signaling" semaphore.
*
* INPUTS:
*       smid - semaphore Id (as created by
*                 CPSS_OS_SIG_SEM_BIN_CREATE_FUNC)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       see also :
*       1. CPSS_OS_SIG_SEM_BIN_CREATE_FUNC
*       2. CPSS_OS_SIG_SEM_DELETE_FUNC
*       3. CPSS_OS_SIG_SEM_WAIT_FUNC
*       4. CPSS_OS_SIG_SEM_SIGNAL_FUNC
*
*******************************************************************************/
typedef GT_STATUS (*MV_SOPT_OS_SIG_SEM_DELETE_FUNC)
(
    IN GT_UINTPTR smid
);

/*******************************************************************************
* MV_SOPT_OS_SIG_SEM_WAIT_FUNC
*
* DESCRIPTION:
*       Wait on "signaling" semaphore.
*
* INPUTS:
*       smid    - "signaling" semaphore Id (as created by
*                 CPSS_OS_SIG_SEM_BIN_CREATE_FUNC)
*       timeOut - time out in milliseconds or 0 to wait forever
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_TIMEOUT - on time out
*
* COMMENTS:
*       see also :
*       1. CPSS_OS_SIG_SEM_BIN_CREATE_FUNC
*       2. CPSS_OS_SIG_SEM_DELETE_FUNC
*       3. CPSS_OS_SIG_SEM_WAIT_FUNC
*       4. CPSS_OS_SIG_SEM_SIGNAL_FUNC
*
*******************************************************************************/
typedef GT_STATUS (*MV_SOPT_OS_SIG_SEM_WAIT_FUNC)
(
    IN GT_UINTPTR  smid,
    IN GT_U32      timeOut
);

/*******************************************************************************
* CPSS_OS_SIG_SEM_SIGNAL_FUNC
*
* DESCRIPTION:
*       Signal a "signaling" semaphore.
*
* INPUTS:
*       smid    - "signaling" semaphore Id (as created by
*                 CPSS_OS_SIG_SEM_BIN_CREATE_FUNC)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       see also :
*       1. CPSS_OS_SIG_SEM_BIN_CREATE_FUNC
*       2. CPSS_OS_SIG_SEM_DELETE_FUNC
*       3. CPSS_OS_SIG_SEM_WAIT_FUNC
*       4. CPSS_OS_SIG_SEM_SIGNAL_FUNC
*
*******************************************************************************/
typedef GT_STATUS (*MV_SOPT_OS_SIG_SEM_SIGNAL_FUNC)
(
    IN GT_UINTPTR smid
);

/*******************************************************************************
* MV_SOPT_OS_RAND_FUNC
*
* DESCRIPTION:
*       Generates a pseudo-random integer between 0 and RAND_MAX
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       rValue    - pseudo-random integer
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_32 (*MV_SOPT_OS_RAND_FUNC)
(
    void
);

/* structure that hold the "os" functions used by serdes optimizer */
typedef struct {
  MV_SOPT_OS_EXACT_DELAY_FUNC   osExactDelayPtr;
  MV_SOPT_OS_TIME_WK_AFTER_FUNC osTimerWkPtr;
  MV_SOPT_OS_MEM_SET_FUNC       osMemSetPtr;
  MV_SOPT_OS_FREE_FUNC          osFreePtr;
  MV_SOPT_OS_MALLOC_FUNC        osMallocPtr;
  MV_SOPT_OS_IO_PRINTF_FUNC     osPrintfPtr;
  MV_SOPT_OS_MEM_CPY_FUNC       osMemCpyPtr;
  MV_SOPT_OS_SIG_SEM_BIN_CREATE_FUNC  osSemBCreatePtr;
  MV_SOPT_OS_SIG_SEM_DELETE_FUNC    osSemDelPtr;
  MV_SOPT_OS_SIG_SEM_WAIT_FUNC      osSemWaitPtr;
  MV_SOPT_OS_SIG_SEM_SIGNAL_FUNC    osSemSignalPtr;
  MV_SOPT_OS_IO_SPRINTF_FUNC        osSprintfPtr;
  MV_SOPT_OS_RAND_FUNC              osRandPtr;

}MV_SERDES_OPT_OS_FUNC_PTR;

/*******************************************************************************
* mvSerdesTunningSystemInit
*
* DESCRIPTION:
*       Init environment variables for tunning algorithm.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningSystemInit
(
    MV_SERDES_OPT_OS_FUNC_PTR *funcPtr
);

/*******************************************************************************
* mvSerdesTunningSystemClose
*
* DESCRIPTION:
*       Free all resources allocated by tunning algorithm.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningSystemClose
(
    void
);

/*******************************************************************************
* mvSerdesTunningRxLaneRegister
*
* DESCRIPTION:
*       Configure all RX lane related information for tunning
*       (include access functions pointers).
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*       prbsType  - PRBS type (7, 15, 23, 31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningRxLaneRegister
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 laneNum,
    MV_PRBS_TYPE prbsType
);

/*******************************************************************************
* mvSerdesTunningRxLaneUnregister
*
* DESCRIPTION:
*       Remove RX lane related information from tunning algorithm
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningRxLaneUnregister
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 laneNum
);

/*******************************************************************************
* mvSerdesTunningTxLaneRegister
*
* DESCRIPTION:
*       Configure all TX lane related information to run PRBS
*       (include functions pointers).
*
* INPUTS:
*       devNum    - Device Number
*       laneNum   - Lane Number.
*       prbsType  - PRBS type (7, 15, 23, 31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningTxLaneRegister
(
    GT_U8 devNum,
    GT_U32 portGroup,
    GT_U32 laneNum,
    MV_PRBS_TYPE prbsType
);

/*******************************************************************************
* mvSerdesTunningTxLaneUnregister
*
* DESCRIPTION:
*       Remove TX lane related information from tunning algorithm
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
int mvSerdesTunningTxLaneUnregister
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 laneNum
);

/*******************************************************************************
* mvSerdesTunningStartTx
*
* DESCRIPTION:
*       Run PRBS on TX lanes registered in the system.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void mvSerdesTunningStartTx();

/*******************************************************************************
* mvSerdesTunningStopTx
*
* DESCRIPTION:
*       Stop PRBS transmit on TX lanes registered.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void mvSerdesTunningStopTx();

/*******************************************************************************
* mvSerdesTunningStart
*
* DESCRIPTION:
*       Run serdes parameters tunning algorithm (on all RX lanes).
*
* INPUTS:
*       prbsTime - duration of the PRBS test during suggested setup verification [ms].
*       testMode - run different algorithm variations
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - tunning success on all RX lanes
*       1  - tunning failed
*
*******************************************************************************/
int mvSerdesTunningStart
(
    MV_SERDES_OPT_MODE testMode,
    GT_U32  prbsTime
);

/*******************************************************************************
* mvSerdesTunningReport
*
* DESCRIPTION:
*       Print optimal serdes parameters for tested lane for Verbose report mode.
*       Return serdes optimal parameters.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Test result parameters.
*
*******************************************************************************/
MV_SERDES_OPT_RESULTS mvSerdesTunningReport
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 laneNum
);

/*******************************************************************************
* mvSerdesTunningReportTypeSet
*
* DESCRIPTION:
*       Set report type.
*
* INPUTS:
*       devNum    - Device Number
*       portGroup - port group (core) number
*       laneNum   - Lane Number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Test result parameters.
*
*******************************************************************************/
void mvSerdesTunningReportTypeSet
(
    MV_SERDES_OPT_REPORT_TYPE  type
);

#ifdef __cplusplus
}
#endif

#endif /* __serdesRxOpt_H */


