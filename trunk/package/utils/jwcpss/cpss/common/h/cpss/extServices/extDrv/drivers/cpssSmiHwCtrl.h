/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssSmiHwCtrl.h
*
* DESCRIPTION:
*       Public definitions and function prototype for SMI HW-IF
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssSmiHwCtrlH
#define __cpssSmiHwCtrlH

#ifdef __cplusplus
extern "C" {
#endif


/************ Includes ********************************************************/
#include <cpss/extServices/os/gtOs/gtGenTypes.h>


/************* Functions Prototype ********************************************/
/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_INIT_DRIVER_FUNC
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
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_INIT_DRIVER_FUNC)
(
    GT_VOID
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_WRITE_REG_FUNC
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
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
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_WRITE_REG_FUNC)
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_READ_REG_FUNC
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using SMI.
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
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_READ_REG_FUNC)
(
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_READ_FUNC
*
* DESCRIPTION:
*       Reads a memory map (contiguous memory) using SMI from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addr     - Register address to start write the writing.
*       arrLen   - The length of dataArr (the number of registers to read)
*
* OUTPUTS:
*       dataArr  - An array containing the data to be written.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*       - register address is incremented in 4 byte per register
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_READ_FUNC)
(
    IN GT_U32       devSlvId,
    IN GT_U32       addr,
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_WRITE_FUNC
*
* DESCRIPTION:
*       Writes a memory map (contiguous memory) using SMI from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addr     - Register address to start write the reading.
*       arrLen   - The size of addrArr/dataArr.
*       dataArr  - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*       - register address is incremented in 4 byte per register
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_WRITE_FUNC)
(
    IN GT_U32       devSlvId,
    IN GT_U32       addr,
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_READ_FUNC
*
* DESCRIPTION:
*       Reads SMI register vector from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addrArr  - Array of addresses to write to.
*       arrLen   - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr  - An array containing the data to be written.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_READ_FUNC)
(
    IN GT_U32       devSlvId,
    IN GT_U32       addrArr[],
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_WRITE_FUNC
*
* DESCRIPTION:
*       Writes SMI register vector from task context.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       addrArr  - Array of addresses to write to.
*       dataArr  - An array containing the data to be written.
*       arrLen   - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - on success
*       GT_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_WRITE_FUNC)
(
    IN GT_U32       devSlvId,
    IN GT_U32       addrArr[],
    IN GT_U32       dataArr[],
    IN GT_U32       arrLen
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FUNC
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
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
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FUNC)
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_TASK_READ_REG_FUNC
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using SMI.
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
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_TASK_READ_REG_FUNC)
(
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
);


/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_WRITE_REG_FUNC
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
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
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_WRITE_REG_FUNC)
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_READ_REG_FUNC
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using SMI.
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
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_READ_REG_FUNC)
(
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_DEV_VENDOR_ID_GET_FUNC
*
* DESCRIPTION:
*       This routine returns vendor Id of the given device.
*
* INPUTS:
*       instance - The requested device instance.
*
* OUTPUTS:
*       vendorId <- The device vendor Id.
*       devId    <- The device Id.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_DEV_VENDOR_ID_GET_FUNC)
(
    OUT GT_U16  *vendorId,
    OUT GT_U16  *devId,
    IN  GT_U32  instance
);

/*******************************************************************************
* CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FIELD_FUNC
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       mask - write only the unmask bits.
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
typedef GT_STATUS (*CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FIELD_FUNC)
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 value
);

/* CPSS_EXT_DRV_HW_IF_SMI_STC -
    structure that hold the "external driver SMI interface" functions needed be
    bound to cpss.
*/
typedef struct{
    CPSS_EXTDRV_HW_IF_SMI_INIT_DRIVER_FUNC            extDrvHwIfSmiInitDriverFunc;
    CPSS_EXTDRV_HW_IF_SMI_WRITE_REG_FUNC              extDrvHwIfSmiWriteRegFunc;
    CPSS_EXTDRV_HW_IF_SMI_READ_REG_FUNC               extDrvHwIfSmiReadRegFunc;
    CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_READ_FUNC      extDrvHwIfSmiTaskRegRamReadFunc;
    CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_WRITE_FUNC     extDrvHwIfSmiTaskRegRamWriteFunc;
    CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_READ_FUNC      extDrvHwIfSmiTaskRegVecReadFunc;
    CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_WRITE_FUNC     extDrvHwIfSmiTaskRegVecWriteFunc;
    CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FUNC         extDrvHwIfSmiTaskWriteRegFunc;
    CPSS_EXTDRV_HW_IF_SMI_TASK_READ_REG_FUNC          extDrvHwIfSmiTaskReadRegFunc;
    CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_READ_REG_FUNC     extDrvHwIfSmiIntReadRegFunc;
    CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_WRITE_REG_FUNC    extDrvHwIfSmiIntWriteRegFunc;
    CPSS_EXTDRV_HW_IF_SMI_DEV_VENDOR_ID_GET_FUNC      extDrvHwIfSmiDevVendorIdGetFunc;
    CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FIELD_FUNC   extDrvHwIfSmiTaskWriteFieldFunc;

}CPSS_EXT_DRV_HW_IF_SMI_STC;


#ifdef __cplusplus
}
#endif


#endif  /* __cpssSmiHwCtrlH */


