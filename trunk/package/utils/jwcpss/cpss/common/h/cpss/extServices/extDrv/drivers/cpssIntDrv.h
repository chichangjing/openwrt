/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssIntDrv.h
*
* DESCRIPTION:
*       Includes interrupt management routines.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __cpssIntDrvh
#define __cpssIntDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/extServices/os/gtOs/cpssOsIntr.h>

/*******************************************************************************
* CPSS_EXTDRV_INT_CONNECT_FUNC
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
typedef GT_STATUS (*CPSS_EXTDRV_INT_CONNECT_FUNC)
(
    IN  GT_U32           vector,
    IN  GT_VOIDFUNCPTR   routine,
    IN  GT_U32           parameter
);

/*******************************************************************************
* CPSS_EXTDRV_INT_ENABLE_FUNC
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
typedef GT_STATUS (*CPSS_EXTDRV_INT_ENABLE_FUNC)
(
    IN GT_U32   intMask
);

/*******************************************************************************
* CPSS_EXTDRV_INT_DISABLE_FUNC
*
* DESCRIPTION:
*       Disable corresponding interrupt bits.
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
typedef GT_STATUS (*CPSS_EXTDRV_INT_DISABLE_FUNC)
(
    IN GT_U32   intMask
);


/*******************************************************************************
* CPSS_EXTDRV_INT_LOCK_MODE_SET_FUNC
*
* DESCRIPTION:
*       Lock/unlock interrupts
*
* INPUTS:
*       mode   - interrupt state lock/unlock
*       key    - if mode is INTR_MODE_UNLOCK, lock key returned by
*                preceding interrupt disable call
*
* OUTPUTS:
*       key    - if mode is INTR_MODE_LOCK lock key for the interrupt
*                level
*
* RETURNS:
*       Lock key for the interrupt level
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_32 (*CPSS_EXTDRV_INT_LOCK_MODE_SET_FUNC)
(
    IN      CPSS_OS_INTR_MODE_ENT mode,
    INOUT   GT_32          *key
);

/* CPSS_EXT_DRV_INT_STC -
    structure that hold the "external driver interrupts" functions needed be
    bound to cpss.
*/
typedef struct{
    CPSS_EXTDRV_INT_CONNECT_FUNC          extDrvIntConnectFunc;
    CPSS_EXTDRV_INT_ENABLE_FUNC           extDrvIntEnableFunc;
    CPSS_EXTDRV_INT_DISABLE_FUNC          extDrvIntDisableFunc;
    CPSS_EXTDRV_INT_LOCK_MODE_SET_FUNC    extDrvIntLockModeSetFunc;

}CPSS_EXT_DRV_INT_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssIntDrvh */


