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
* gtSmiHwCtrl.c
*
* DESCRIPTION:
*       API implementation for SMI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 1.1.2.1 $
*******************************************************************************/

/*Includes*/

#include <gtExtDrv/drivers/gtSmiDrvCtrl.h>
#include <gtExtDrv/drivers/gtPciDrv.h>
#include <gtExtDrv/drivers/gtSmiDrv.h>
#include <gtExtDrv/drivers/gtSmiHwCtrl.h>
#include <asicSimulation/SCIB/scib.h>

static GT_U32 simulDeviceIds[32]; /* array with correspond indexes in scibDb */
/*****************************************************************************
* hwIfSmiIndirectRead
*
* DESCRIPTION:
*       This function reads data from a device in the secondary MII bus.
*
* INPUTS:
*       devSmiAddr  - The SMI Device id for PP.
*       regSmiAddr  - The SMI Device id for register of PP.
*       regAddr     - The SMI register address
*       valuePtr    - The storage where register date to be saved.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_TIMEOUT  - on timeout
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwIfSmiIndirectRead (
    GT_U32  devSmiAddr,
    GT_U32  regSmiAddr,
    GT_U32  regAddr,
    GT_U32* valuePtr
)
{
    GT_U32     simData;
    GT_U32     outData;

    scibSmiRegRead(simulDeviceIds[devSmiAddr], regSmiAddr, regAddr,&simData);
    outData = simData;
    * valuePtr = outData;

    return GT_OK;
}
/*****************************************************************************
* hwIfSmiIndirectWrite
*
* DESCRIPTION:
*       This function writes data from a device in the secondary MII bus.
*
* INPUTS:
*       devSmiAddr  - The SMI Device id for PP.
*       regSmiAddr  - The SMI Device id for register of PP.
*       regAddr     - The SMI register address
*       value       - The register's value.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_TIMEOUT  - on timeout
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwIfSmiIndirectWrite (
    GT_U32  devSmiAddr,
    GT_U32  regSmiAddr,
    GT_U32  regAddr,
    GT_U32  value
)
{
    scibSmiRegWrite(simulDeviceIds[devSmiAddr],  regSmiAddr, regAddr, value);
    return GT_OK;
}

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
*       GT_OK               - on success
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
    GT_U32 hwId;

    for (hwId = 0; hwId < 32; hwId++)
    {
        simulDeviceIds[hwId] = scibGetDeviceId(hwId);
    }
    return GT_OK;
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
*      For SOHO PPs the regAddr consist of 2 parts:
*      - 16 MSB is SMI device ID for register
*      - 16 LSB is register address within SMI device ID for register
*
*******************************************************************************/
GT_STATUS hwIfSmiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_STATUS rc;
    GT_U32 regSmiDevId;
    GT_U32 regSmiRegAddr;

    /* get internal info for register */
    regSmiRegAddr = regAddr & 0xffff;
    regSmiDevId   = regAddr >> 16;

    /* The SOHO may work in two addressing modes:
       - Single mode. In this mode only 1 SMI Slave PP resides on the SMI bus.
         And register may be accessed directly by it regSmiDevId.
       - Multiple mode. In this mode several SMI Slave PPs reside on the SMI
         bus. Particular PP may be accesed only by its devSlvId. To
         access to registers inderect method must be used.
       The value of devSlvId defines addressing mode:
          0 < devSlvId <= 31             - Multiple mode and indirect access
          devSlvId == 0 or devSlvId > 31 - Single mode and direct access
    */

    /* devSlvId == 0 - is also valid SMI dev Id */
    if (devSlvId > 31)
    {
        scibSmiRegWrite(0, regSmiDevId, regSmiRegAddr, value);
        rc = GT_OK;
    }
    else
    {
        rc  = hwIfSmiIndirectWrite(devSlvId, regSmiDevId,regSmiRegAddr,value);
    }

    return rc;
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
    GT_STATUS rc;
    GT_U32 regSmiDevId;
    GT_U32 regSmiRegAddr;

    /* get internal info for register */
    regSmiRegAddr = regAddr & 0xffff;
    regSmiDevId   = regAddr >> 16;

    /* The SOHO may work in two addressing modes:
       - Single mode. In this mode only 1 SMI Slave PP resides on the SMI bus.
         And register may be accessed directly by it regSmiDevId.
       - Multiple mode. In this mode several SMI Slave PPs reside on the SMI
         bus. Particular PP may be accesed only by its devSlvId. To
         access to registers inderect method must be used.
       The value of devSlvId defines addressing mode:
          0 < devSlvId <= 31             - Multiple mode and indirect access
          devSlvId == 0 or devSlvId > 31 - Single mode and direct access
    */

    /* devSlvId == 0 - is also valid SMI dev Id */
    if (devSlvId > 31)
    {

        GT_U32     simData;
        GT_U32     outData;

        scibSmiRegRead(0, regSmiDevId, regSmiRegAddr, &simData);
        outData = simData;
        *dataPtr = outData;
        rc = GT_OK;
    }
    else
    {
        rc  = hwIfSmiIndirectRead(devSlvId, regSmiDevId,regSmiRegAddr,dataPtr);
    }

    return rc;
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
    GT_STATUS retVal;

#ifdef DEBUG
    if (checkaddr == regAddr)
    {
        logMsg("hwIfSmiTaskWriteReg address 0x%08x, 0x%08x\n",
               (int)regAddr, (int)value,
               0,0,0,0);
        taskSuspend(0);
    }

    if (logHwReadWriteFlag)
    {
        postIt(devSlvId, "W", regAddr, value);
    }
#endif
    retVal = hwIfSmiWriteReg(devSlvId, regAddr, value);

    return retVal;
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
    GT_STATUS retVal;


    retVal = hwIfSmiReadReg(devSlvId, regAddr, dataPtr);

#ifdef DEBUG
    if (checkaddr == regAddr)
    {
        logMsg("hwIfSmiTaskReadReg address 0x%08x, 0x%08x\n",
               (int)regAddr, (int)*dataPtr,
               0,0,0,0);
        taskSuspend(0);
    }

    if (logHwReadWriteFlag)
    {
        postIt(devSlvId, "R", regAddr, *dataPtr);
    }
#endif
    return retVal;
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
    GT_U32          i;
    GT_STATUS       rc;

    for (i = 0; i < arrLen; i++, addr += 4)
    {
        rc = hwIfSmiReadReg(devSlvId, addr, &dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
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
    GT_U32          i;
    GT_STATUS       rc;

    for (i = 0; i < arrLen; i++, addr += 4)
    {
        rc = hwIfSmiWriteReg(devSlvId, addr, dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
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
    GT_U32          i;
    GT_STATUS       rc;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiReadReg(devSlvId, addrArr[i], &dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
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
    GT_U32          i;
    GT_STATUS       rc;

    for (i = 0; i < arrLen; i++)
    {
        rc = hwIfSmiWriteReg(devSlvId, addrArr[i], dataArr[i]);

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
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
    GT_STATUS retVal;

#ifdef DEBUG
    if (checkaddr == regAddr)
    {
        logMsg("hwIfSmiInterruptWriteReg address 0x%08x, 0x%08x\n",
               (int)regAddr, (int)value,
               0,0,0,0);
        if (! intContext())
        {
            taskSuspend(0);
        }
    }

    if (logHwReadWriteFlag)
    {
        postIt(devSlvId, "W", regAddr, value);
    }
#endif

    retVal = hwIfSmiWriteReg(devSlvId, regAddr, value);

    return retVal;
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
    GT_STATUS retVal;

    retVal = hwIfSmiReadReg(devSlvId, regAddr, dataPtr);

#ifdef DEBUG
    if (checkaddr == regAddr)
    {
        logMsg("hwIfSmiInterruptReadReg address 0x%08x, 0x%08x\n",
               (int)regAddr, (int)*dataPtr,
               0,0,0,0);
        if (!intContext())
        {
            taskSuspend(0);
        }
    }

    if (logHwReadWriteFlag)
    {
        postIt(devSlvId, "W", regAddr, *dataPtr);
    }
#endif
    return retVal;
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
    GT_U32      vendor;
    GT_U32      device;

    /* Prestera is little endian */
    scibReadMemory(instance, 0x00000050, 1, &vendor);

    scibReadMemory(instance, 0x0000004c, 1, &device);

    *vendorId   = (GT_U16)(vendor & 0xffff);
    *devId      = (GT_U16)((device>>4) & 0xffff);

    return GT_OK;
}

#ifdef DEBUG
int traceAddress(GT_U32 add)
{
    checkaddr = add;
    return 0;
}

int untraceAddress(GT_U32 add)
{
    checkaddr = 0xBAD0ADD0;
    return 0;
}
#endif
/*******************************************************************************
* hwPpSetRegField
*
* DESCRIPTION:
*       Write value to selected register field.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       this function actually read the register value modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
GT_STATUS hwIfSmiTaskWriteRegField
(
    IN GT_U32 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 fieldData

)
{
    GT_STATUS retVal;
    GT_U32    data;

    retVal = hwIfSmiTaskReadReg( devNum, regAddr,&data);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    data &= ~mask;   /* turn the field off */
    /* insert the new value of field in its place */
    data |= (fieldData  & mask);

    retVal = hwIfSmiTaskWriteReg( devNum, regAddr, data);

    return retVal;
}



