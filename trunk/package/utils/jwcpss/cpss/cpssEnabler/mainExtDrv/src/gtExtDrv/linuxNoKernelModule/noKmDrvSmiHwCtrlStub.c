/*******************************************************************************
*                Copyright 2015, MARVELL SEMICONDUCTOR, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL ISRAEL LTD. (MSIL).                                          *
********************************************************************************
* noKmDrvSmiHwCtrlStub.c
*
* DESCRIPTION:
*       Stub implementation for SMI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 1 $
*******************************************************************************/

/*Includes*/

#include <gtExtDrv/drivers/gtSmiDrvCtrl.h>

/*******************************************************************************
* hwIfSmiInitDriver
*
* DESCRIPTION:
*       Init the SMI interface 
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfSmiInitDriver
(
    GT_VOID
)
{
  return GT_FAIL;
}


/*******************************************************************************
* hwIfSmiWriteReg
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
GT_STATUS hwIfSmiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)value;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfSmiReadReg
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
GT_STATUS hwIfSmiReadReg
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)dataPtr;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfSmiTaskWriteReg
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
GT_STATUS hwIfSmiTaskWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)value;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfSmiTaskReadReg
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
GT_STATUS hwIfSmiTaskReadReg
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)dataPtr;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfSmiInterruptWriteReg
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
GT_STATUS hwIfSmiInterruptWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)value;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfSmiInterruptReadReg
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
GT_STATUS hwIfSmiInterruptReadReg
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)dataPtr;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* extDrvDirectSmiRead
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
GT_STATUS extDrvDirectSmiReadReg
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *value
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)value;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* extDrvDirectSmiWriteReg
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*       dataPtr    - Data read from register.
*
* OUTPUTS:
*        None,
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvDirectSmiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)value;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfSmiTskRegRamRead
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
GT_STATUS hwIfSmiTskRegRamRead
(
    IN GT_U32       devSlvId,
    IN GT_U32       addr,
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
)
{
    (void)devSlvId;
    (void)addr;
    (void)dataArr;
    (void)arrLen;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfSmiTskRegRamWrite
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
GT_STATUS hwIfSmiTskRegRamWrite
(
    IN GT_U32       devSlvId,
    IN GT_U32       addr,
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
)
{
    (void)devSlvId;
    (void)addr;
    (void)dataArr;
    (void)arrLen;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfSmiTskRegVecRead
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
GT_STATUS hwIfSmiTskRegVecRead
(
    IN GT_U32       devSlvId,
    IN GT_U32       addrArr[],
    OUT GT_U32      dataArr[],
    IN GT_U32       arrLen
)
{
    (void)devSlvId;
    (void)addrArr;
    (void)dataArr;
    (void)arrLen;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfSmiTskRegVecWrite
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
GT_STATUS hwIfSmiTskRegVecWrite
(
    IN GT_U32       devSlvId,
    IN GT_U32       addrArr[],
    IN GT_U32       dataArr[],
    IN GT_U32       arrLen
)
{
    (void)devSlvId;
    (void)addrArr;
    (void)dataArr;
    (void)arrLen;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* extDrvSmiDevVendorIdGet
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
GT_STATUS extDrvSmiDevVendorIdGet
(
    OUT GT_U16  *vendorId,
    OUT GT_U16  *devId,
    IN  GT_U32  instance
)
{
    (void)vendorId;
    (void)devId;
    (void)instance;
    return GT_NOT_SUPPORTED;
}
