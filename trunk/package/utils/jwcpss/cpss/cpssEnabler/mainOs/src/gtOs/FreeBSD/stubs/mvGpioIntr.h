/*******************************************************************************
*              Copyright 2003, RADLAN Computer Communications, LTD.
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
* GALILEO TECHNOLOGY, INC. (GTI) AND RADLAN Computer Communications, LTD.
********************************************************************************
* mvGpioIntr.h
*
* DESCRIPTION:
*       Interface for using GPIO port interrupts.
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/


typedef unsigned long GT_BOOL;
typedef unsigned long GT_STATUS;
typedef unsigned char GT_U8;
typedef unsigned short GT_U16;
typedef unsigned long GT_U32;

typedef void (*GT_VOIDFUNCPTR)(void);
typedef void (*GT_VOIDINTFUNCPTR)(GT_U32);

#define IN
#define GT_FAIL -1
#define GT_OK 0
#ifndef NULL
#define NULL 0
#endif

/*******************************************************************************
* gpioIntEnable
*
* DESCRIPTION:
*       Enable corresponding interrupt bits
*
* INPUTS:
*       gpio_pin - pin of GPIO to enable interrupt for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/
void gpioIntEnable
(
    IN GT_U16   gpio_pin
);

/*******************************************************************************
* gpioIntDisable
*
* DESCRIPTION:
*       Disable corresponding interrupt bits.
*
* INPUTS:
*       gpio_pin - pin of GPIO to disable interrupt for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/
void gpioIntDisable
(
    IN GT_U16   gpio_pin
);

/*******************************************************************************
* gpioInt
*
* DESCRIPTION:
*       Connect a specified C routine to a specified interrupt vector.
*
* INPUTS:
*       parameter - parameter to be passed to routine
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
void gpioInt
(
   GT_U32 parameter
);

/*******************************************************************************
* gpioIntConnect
*
* DESCRIPTION:
*       Connect a specified C routine to a specified interrupt vector.
*
* INPUTS:
*       vector    - interrupt vector number to attach to.
*       routine   - routine to be called.
*       parameter - parameter to be passed to routine.
*       gpio_pin  - pin of GPIO to connect interrupt to.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpioIntConnect
(
    IN  GT_U32           vector,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter,
   IN  GT_U16          gpio_pin
);

/*******************************************************************************
* sysGpioIntConnect
*
* DESCRIPTION:
*       Connect a specified C routine to a specified interrupt vector.
*
* INPUTS:
*       vector    - interrupt vector number to attach to
*       routine   - routine to be called
*       parameter - parameter to be passed to routine
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS sysGpioIntConnect
(
    IN  GT_U32           vector,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter
);


