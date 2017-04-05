/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
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
* gtOsExc.h
*
* DESCRIPTION:
*       Operating System wrapper. Exception Handling facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*******************************************************************************/

#ifndef __gtOsExch
#define __gtOsExch

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes ******************************************************/

#include <stdio.h>

#ifdef _VXWORKS

#include <vxWorks.h>
#include <excLib.h>
#include <taskLib.h>
#include <esf.h>

#if (CPU == PPC604) || (CPU == PPC603) || (CPU == PPC860) || (CPU == PPC85XX)
#define CPU_PPC_EXIST
#include <arch/ppc/esfPpc.h>
#endif
#if  ((CPU==ARMARCH7) || (CPU==ARMARCH5) || (CPU==ARMARCH5_T)) && (defined(CPU_946ES) || defined(CPU_926EJ))
#define CPU_ARM_EXIST
#include <arch/arm/esfArm.h>
#endif

#endif /* _VXWORKS */

#include <gtOs/gtGenTypes.h>

/************* Defines *******************************************************/
#define  FUNC_CALL_LENGTH   64
#define  MAX_UINT_8     255

#if defined(_WIN32) || defined(_linux) || defined(_uClinux) || defined(_FreeBSD)
#define  ESFPPC char
#define  ESF    char
#endif


/************* Enumoretion Types **********************************************/
typedef enum {
    OS_FATAL_RESET = 0,
    OS_FATAL_MONITOR,  /* TBD */
    OS_FATAL_WARNING
} OS_FATAL_ERROR_TYPE;

/************* Data structures ************************************************/
typedef struct  {
      unsigned int exc_vector; /* exception vector */
      char exc_name[30];  /* exception name */
} OS_exception_list_struct;

/* ptr to fatal error function  */
typedef void (*FATAL_FUNC_PTR)(OS_FATAL_ERROR_TYPE, char *);


/*******************************************************************************
* osFatalErrorInit
*
* DESCRIPTION:
*       Initalize the Fatal Error mechanism.
* INPUTS:
*       fatalErrFunc - Pointer to function handling the user fatal error calles.
*                      If using the default fatal error hendling - use NULL
*
* OUTPUTS:
*       None
* RETURNS:
*       0 - success, 1 - fail.
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osFatalErrorInit( FATAL_FUNC_PTR funcPtr );

/*******************************************************************************
* osFatalError
*
* DESCRIPTION:
*       Handling fatal error message.
* INPUTS:
*       type - Type of fatal error.
*       message - formated text.
*       ... - arguments related to the formated text.
* OUTPUTS:
*       None
* RETURNS:
*       None
* COMMENTS:
*       None
*
*******************************************************************************/
void osFatalError(
  OS_FATAL_ERROR_TYPE fatalErrorType,
  char * messageAttached
);

/*******************************************************************************
* osExceptionInit
*
* DESCRIPTION:
*       Replacing the OS exception handling.
* INPUTS:
*
* OUTPUTS:
*       None
* RETURNS:
*       None
* COMMENTS:
*       None
*
*******************************************************************************/
void osExceptionInit(void);


#ifdef CPU_PPC_EXIST
#undef CPU_PPC_EXIST
/*******************************************************************************
* osExceptionHandling  ( For PPC architecture )
*
* DESCRIPTION:
*       Handling the OS exceptions.
* INPUTS:
*       task - ID of offending task.
*       vecNum - Vector number.
*       stackPtr - Pointer to the main control architecture registers.
* OUTPUTS:
*       None
* RETURNS:
*       0 - success, 1 - fail.
* COMMENTS:
*       None
*
*******************************************************************************/
void osExceptionHandling(
  int      task,
  int      vecNum,
  ESFPPC  *stackPtr
);
#endif

#ifdef CPU_ARM_EXIST
#undef CPU_ARM_EXIST
/*******************************************************************************
* osExceptionHandling  ( For ARM architecture )
*
* DESCRIPTION:
*       Handling the OS exceptions.
* INPUTS:
*       task - ID of offending task.
*       vecNum - Vector number.
*       stackPtr - Pointer to the main control architecture registers.
* OUTPUTS:
*       None
* RETURNS:
*       0 - success, 1 - fail.
* COMMENTS:
*       None
*
*******************************************************************************/
void osExceptionHandling(
  int      task,
  int      vecNum,
  ESF      *stackPtr
);
#endif


/*******************************************************************************
* osReset
*
* DESCRIPTION:
*       Reset the CPU
* INPUTS:
*       type - reset type (cold, hot ... TBD )
* OUTPUTS:
*       None
* RETURNS:
*       None
* COMMENTS:
*       Calling to specific function from mainExtDrv
*
*******************************************************************************/
void  osReset(int type);

#ifdef __cplusplus
}
#endif

#endif  /* __gtOsExch */
/* Do Not Add Anything Below This Line */

