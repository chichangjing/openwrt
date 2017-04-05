/*===============================================================================================*/
/*  THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF                                               */
/*  MICROSEMI COP.- ANALOG MIXED SIGNAL GROUP LTD. ("AMSG")                                      */
/*  AND IS SUBJECT TO A NON-DISCLOSURE AGREEMENT                                                 */
/*  NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT,                                               */
/*  MASK WORK RIGHT OR COPYRIGHT OF AMSG OR ANY THIRD PARTY.                                     */
/*  AMSG RESERVES THE RIGHT AT ITS SOLE                                                          */
/*  DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO AMSG.                        */
/*  THIS CODE IS PROVIDED "AS IS". AMSG MAKES NO WARRANTIES, EXPRESSED,                          */
/*  IMPLIEDOR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.                    */
/*  THIS SOURCE CODE, MAY NOT BE DISCLOSED TO ANY THIRD PARTY OR USED IN ANY OTHER MANNER,       */
/*  AND KNOWLEDGE DERIVED THEREFROM OR CANNOT BE USED TO WRITE ANY PROGRAM OR CODE.              */
/*  USE IS PERMITTED ONLY PURSUANT TO WRITTEN AGREEMENT SIGNED BY AMSG.                          */
/*  KNOWLEDGE OF THIS FILE MAY UNDER NO CIRCUMSTANCES BE USED TO WRITE ANY PROGRAM.              */
/*===============================================================================================*/

///////////////////////////////////////////////
// This file holds definitions that should be
// part of OS services.
///////////////////////////////////////////////

#ifndef _OS_DEF1
    #define _OS_DEF1

#include <string.h> //memset, memcpy

//-----------------------------------------------------
//-----C O M P I L E R    I S S U E S -----------------
//-----------------------------------------------------

#define __ARM __arm // Compiler specific: Speed up specific funtion excenution by comiling them to ARM mode when the entire project is compiled in THUMB mode

typedef signed char        S8   ;
typedef unsigned char      U8   ;
typedef signed short       S16  ;
typedef unsigned short     U16  ;
typedef signed long        S32  ;
typedef unsigned long      U32  ;
#ifndef DRAGONITE_TYPE_A1
/* causes error illigal long after long */
typedef signed long long   S64  ;
typedef unsigned long long U64  ;
#endif 
typedef volatile S8        VS8  ;
typedef volatile U8        VU8  ;
typedef volatile S16       VS16 ;
typedef volatile U16       VU16 ;
typedef volatile S32       VS32 ;
typedef volatile U32       VU32 ;


//---------------------------------------------------------------------
//----------- OS REPLACEMENT ------------------------------------------
//---------------------------------------------------------------------
extern U8 NumTaskRunning;

#define OSAL_TASK_RUNNING_FINISH_INIT_STAGE() NumTaskRunning++;
//simulate watchdog action
#define WDG_Report_OK(eFUNC_COMM_TASK);

//os should provide a timer counter. here we will just count to 1000
#define OSAL_DELAY_mSec(Xms)  { U32 i;  for(i = 0; i < Xms; i++); }

//should be meaningful when there is another task accessing same memory
#define _ENTER_CRITICAL_SECTION_
#define _EXIT_CRITICAL_SECTION_

#endif
