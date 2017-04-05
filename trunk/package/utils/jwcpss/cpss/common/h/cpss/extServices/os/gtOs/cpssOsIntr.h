/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssOsIntr.h
*
* DESCRIPTION:
*       Operating System wrapper. Interrupt facility.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssOsIntrh
#define __cpssOsIntrh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>


/************* Typedefs *******************************************************/
/*
 * Typedef: enumeration CPSS_OS_INTR_MODE_ENT
 *
 * Description: An enumeration for interrupts mode
 *
 * Fields:
 *       CPSS_OS_INTR_MODE_LOCK_E   - lock interrupts
 *       CPSS_OS_INTR_MODE_UNLOCK_E - unlock interrupts
 *
 */
typedef enum
{
    CPSS_OS_INTR_MODE_LOCK_E = 0,
    CPSS_OS_INTR_MODE_UNLOCK_E
}CPSS_OS_INTR_MODE_ENT;


/************* Functions ******************************************************/
/*******************************************************************************
* CPSS_OS_INT_ENABLE_FUNC
*
* DESCRIPTION:
*       Enable corresponding interrupt bits
*
* INPUTS:
*       intMask - new interrupt bits
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_OS_INT_ENABLE_FUNC)
(
    IN GT_U32   intMask
);

/*******************************************************************************
* CPSS_OS_INT_DISABLE_FUNC
*
* DESCRIPTION:
*       Disable corresponding interrupt bits
*
* INPUTS:
*       intMask - new interrupt bits
*       enable - to enable or disable the interrupt
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_OS_INT_DISABLE_FUNC)
(
    IN GT_U32   intMask
);

/*******************************************************************************
* CPSS_OS_INT_MODE_SET_FUNC
*
* DESCRIPTION:
*       set interrupt mode -- Lock/unlock
*
* INPUTS:
*       mode   - interrupt state lock/unlock
*       keyPtr - (pointer to)if mode is INTR_MODE_UNLOCK, lock key returned by
*                preceding interrupt disable call
*
* OUTPUTS:
*       keyPtr -(pointer to)if mode is INTR_MODE_LOCK lock key for the interrupt
*                level
*
* RETURNS:
*       Lock key for the interrupt level
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_32 (*CPSS_OS_INT_MODE_SET_FUNC)
(
    IN      CPSS_OS_INTR_MODE_ENT    mode,
    INOUT   GT_32                   *keyPtr
);

/*******************************************************************************
* CPSS_OS_INT_CONNECT_FUNC
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
typedef GT_STATUS (*CPSS_OS_INT_CONNECT_FUNC)
(
    IN  GT_U32           vector,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter
);

/* CPSS_OS_INT_BIND_STC -
    structure that hold the "os interrupts" functions needed be bound to cpss.

*/
typedef struct{
    CPSS_OS_INT_ENABLE_FUNC       osIntEnableFunc;
    CPSS_OS_INT_DISABLE_FUNC      osIntDisableFunc;
    CPSS_OS_INT_MODE_SET_FUNC     osIntModeSetFunc;
    CPSS_OS_INT_CONNECT_FUNC      osIntConnectFunc;

}CPSS_OS_INT_BIND_STC;


#ifdef __cplusplus
}
#endif

#endif  /* __cpssOsIntrh */


