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
* osVxExc.c
*
* DESCRIPTION:
*       VxWorks Operating System wrapper. Exception handling and Fatal Error facilities.
*
* DEPENDENCIES:
*       VxWorks, CPU independed.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsExc.h>

#include <gtOs/gtOsIntr.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsTask.h>

#include <time.h>
#include <intLib.h>
#include <string.h>
#include <taskLib.h>
#include <esf.h>

/************* Includes ******************************************************/
#if (CPU == PPC604) || (CPU == PPC603) || (CPU == PPC860) || (CPU == PPC85XX)
#include <arch/ppc/regsPpc.h>
#endif

#if  ((CPU==ARMARCH7) || (CPU==ARMARCH5) || (CPU==ARMARCH5_T)) && (defined(CPU_946ES) || defined(CPU_926EJ))
#include <arch/arm/regsArm.h>
#endif



/* Binding the default function handling the fatal error */
FATAL_FUNC_PTR currentFatalErrorFunc = NULL;
/* In case the user would like to define his own fatal Error handling function */
FATAL_FUNC_PTR usrDefinedFatalErrorFunc = NULL;


/* External function definitions */
extern GT_STATUS extDrvReset();

/*******************************************************************************
* osLoadFuctionCalls
*
* DESCRIPTION:
*       Loading the last function calls to a given buffer
* INPUTS:
*       pc - program counter pointer
*       sp - stack pointer
*       funcCallList - given buffer to load in the call stack
* OUTPUTS:
*       None
* RETURNS:
*       None
* COMMENTS:
*       None
*
*******************************************************************************/
void  osLoadFuctionCalls(
  unsigned int * pc,
  unsigned int * sp,
  unsigned int * funcCallList
)
{
  int i;
  unsigned int * spLcl = sp;
  /* Set the first address as the program counter */
  funcCallList[0] = (unsigned int)pc;
  for (i=1; (spLcl != 0) && (i < FUNC_CALL_LENGTH) ; i++ )
  {
    funcCallList[i] = *((unsigned int *)((unsigned int)spLcl + 4));
    spLcl = (unsigned int *)(*spLcl);
    if (spLcl == (unsigned int *)0xeeeeeeee)
      break;
  }
}

/************ Public Functions ************************************************/

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
void  osReset(int type)
{
#ifndef DUNE_FE_IMAGE
  extDrvReset();
  while (1);
#endif
}

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
GT_STATUS osFatalErrorInit( FATAL_FUNC_PTR funcPtr )
{

  /* Binding the default function handling the fatal error */
  currentFatalErrorFunc = (FATAL_FUNC_PTR)osFatalError;

  /* Binding the user defined function handling the fatal error - if declerd */
  if ( funcPtr != (FATAL_FUNC_PTR)NULL )
    usrDefinedFatalErrorFunc = funcPtr;

  return(GT_OK);
}

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
)
{
    /* screan print */
    printf("------------ Fatal Error ------------\n");
    printf("%s", messageAttached);

    /* abort application */
    exit(0);
}

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

void osExceptionInit(void)
{
    return;
}


