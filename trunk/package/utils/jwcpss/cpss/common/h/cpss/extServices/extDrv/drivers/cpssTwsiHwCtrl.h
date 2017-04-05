/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTwsiHwCtrl.h
*
* DESCRIPTION:
*       API implementation for TWSI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssTwsiHwCtrlh
#define __cpssTwsiHwCtrlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/extServices/os/gtOs/gtGenTypes.h>


/*******************************************************************************
* CPSS_EXTDRV_HW_IF_TWSI_INIT_DRIVER_FUNC
*
* DESCRIPTION:
*       Init the TWSI interface
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_TWSI_INIT_DRIVER_FUNC)
(
    GT_VOID
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_TWSI_WRITE_REG_FUNC
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_TWSI_WRITE_REG_FUNC)
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_TWSI_READ_REG_FUNC
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_TWSI_READ_REG_FUNC)
(
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
);

/* CPSS_EXT_DRV_HW_IF_TWSI_STC -
    structure that hold the "external driver TWSI interface" functions needed be
    bound to cpss.
*/
typedef struct{
    CPSS_EXTDRV_HW_IF_TWSI_INIT_DRIVER_FUNC       extDrvHwIfTwsiInitDriverFunc;
    CPSS_EXTDRV_HW_IF_TWSI_WRITE_REG_FUNC         extDrvHwIfTwsiWriteRegFunc;
    CPSS_EXTDRV_HW_IF_TWSI_READ_REG_FUNC          extDrvHwIfTwsiReadRegFunc;
}CPSS_EXT_DRV_HW_IF_TWSI_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssTwsiHwCtrlh */

