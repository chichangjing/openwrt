/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtHwCntlStandBy.c
*
* DESCRIPTION:
*       Prestera driver Hardware read and write functions implementation for Stand
*       By mode of work of CPU in context of High Availability feature.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>

/*******************************************************************************
* Global Hw configuration params.
*******************************************************************************/


/*******************************************************************************
* Intenal Macros and definitions
*******************************************************************************/

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/*******************************************************************************
* Forward functions declarations.
*******************************************************************************/


/*******************************************************************************
* Forward declarations for driver object bound functions
*******************************************************************************/

/*******************************************************************************
* driverHwPciCntlInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*
* INPUTS:
*       devNum          - The PP's device number to init the structure for.
*       portGroupId  - The port group Id.
*       baseAddr        - The PP base address on the host interface bus.
*       internalPciBase - Base address to which the internal pci registers
*                         are mapped to.
*       isDiag          - Is this initialization is for diagnostics purposes
*                         (GT_TRUE), or is it a final initialization of the Hw
*                         Cntl unit (GT_FALSE)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       1.  In case isDiag == GT_TRUE, no semaphores are initialized.
*
* Galtis:
*       None.
*
*******************************************************************************/
static GT_STATUS driverHwPciCntlInit
(
    IN GT_U8        devNum,
    IN GT_U32       portGroupId,
    IN GT_UINTPTR   baseAddr,
    IN GT_UINTPTR   internalPciBase,
    IN GT_BOOL      isDiag
)
{
    GT_U8   i;
    char   name[20];
    GT_UINTPTR  address;     /* Physical address for PCI transaction */

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr           = baseAddr;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx            = 2;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase    = internalPciBase;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[0]   = 0;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[1]   = 0;

    /* Reset Hw Address Completion          */
    address = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr + ADDR_COMP_REG;
    *(volatile GT_U32 *)(address) = 0;
    *(volatile GT_U32 *)(address) = 0;
    *(volatile GT_U32 *)(address) = 0;
    GT_SYNC;                                                      \

    /* Update Address Completion shadow     */
    for (i = 0; i < 4; i++)
    {
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].addrCompletShadow[i] = 0;
    }

    if ((isDiag == GT_FALSE) && (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem == (CPSS_OS_MUTEX)0))
    {
        cpssOsSprintf (name,"hwComplSem-%d_%ld",devNum,portGroupId);
        cpssOsMutexCreate (name,&(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem));
    }

    return GT_OK;
}



/*******************************************************************************
* hwPpPciReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       devNum  - The PP to read from.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;

    *data = 0;
    return GT_OK;
}




/*******************************************************************************
* hwPpPciWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciWriteRegister
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;
    value = value;

    return GT_OK;
}


/*******************************************************************************
* hwPpPciGetRegField
*
* DESCRIPTION:
*       Read a selected register field.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;
    fieldOffset = fieldOffset;
    fieldLength = fieldLength;

    *fieldData = 0;
    return GT_OK;
}


/*******************************************************************************
* hwPpPciSetRegField
*
* DESCRIPTION:
*       Write value to selected register field.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id.
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
*       this function actually read the register modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
static GT_STATUS hwPpPciSetRegField
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;
    fieldOffset = fieldOffset;
    fieldLength = fieldLength;
    fieldData = fieldData;

    return GT_OK;
}


/*******************************************************************************
* hwPpPciReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to read from.
*       portGroupId  - The port group Id.
*       regAddr - Register address to read from.
*       mask    - Mask for selecting the read bits.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
*******************************************************************************/
static GT_STATUS hwPpPciReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;
    mask = mask;

    *dataPtr = 0;
    return GT_OK;
}




/*******************************************************************************
* hwPpPciWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to write to.
*       portGroupId  - The port group Id.
*       regAddr - Register address to write to.
*       mask    - Mask for selecting the written bits.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
*******************************************************************************/
static GT_STATUS hwPpPciWriteRegBitMask
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 value
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;
    mask = mask;
    value = value;

    return GT_OK;
}


/*******************************************************************************
* hwPpPciReadRam
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id.
*       addr    - Address offset to read from.
*       length  - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       data    - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciReadRam
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    OUT GT_U32  *data
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    addr = addr;
    length = length;

    *data = 0;
    return GT_OK;
}



/*******************************************************************************
* hwPpPciWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id.
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       data    - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciWriteRam
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    addr = addr;
    length = length;
    data = data;

    return GT_OK;
}


/*******************************************************************************
* hwPpPciWriteRamInReverse
*
* DESCRIPTION:
*       Writes to PP's RAM in reverse.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id.
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       data    - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciWriteRamInReverse
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    addr = addr;
    length = length;
    data = data;

    return GT_OK;
}

/*******************************************************************************
* hwPpPciIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       devNum  - The PP to read from.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to read from.
*                 Note: regAddr should be < 0x1000000
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciIsrRead
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;
    dataPtr = dataPtr;

    return GT_OK;
}
/*******************************************************************************
* hwPpPciIsrWrite
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
*
* INPUTS:
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciIsrWrite
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;
    value = value;

    return GT_OK;
}
/*******************************************************************************
* hwPpPciReadInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data    - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciReadInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;

    *data = 0;
    return GT_OK;
}


/*******************************************************************************
* hwPpPciWriteInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       portGroupId  - The port group Id.
*       regAddr - The register's address to read from.
*       data    - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciWriteInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    /* fix warnings */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;
    data = data;

    return GT_OK;
}


/*******************************************************************************
* hwPpPciResetAndInitCtrlReadRegister
*
* DESCRIPTION:
*       This function reads a register from the Reset and Init Controller.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data    - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciResetAndInitCtrlReadRegister
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    /* fix warnings */
    devNum = devNum;
    regAddr = regAddr;

    *data = 0;

    return GT_OK;
}


/*******************************************************************************
* hwPpPciResetAndInitCtrlWriteRegister
*
* DESCRIPTION:
*       This function writes a register to the Reset and Init Controller.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       regAddr - The register's address to wrire to.
*       data    - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciResetAndInitCtrlWriteRegister
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    /* fix warnings */
    devNum = devNum;
    regAddr = regAddr;
    data = data;

    return GT_OK;
}


/*******************************************************************************
* hwPpPciResetAndInitCtrlGetRegField
*
* DESCRIPTION:
*       This function reads a selected register field from the Reset and Init 
*       Controller.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       regAddr - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - the read field data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciResetAndInitCtrlGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
)
{
    /* fix warnings */
    devNum = devNum;
    regAddr = regAddr;
    fieldOffset = fieldOffset;
    fieldLength = fieldLength;

    *fieldData = 0;

    return GT_OK;
}


/*******************************************************************************
* hwPpPciResetAndInitCtrlSetRegField
*
* DESCRIPTION:
*       This function writes a selected register field to the Reset and Init 
*       Controller.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciResetAndInitCtrlSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
)
{
    /* fix warnings */
    devNum = devNum;
    regAddr = regAddr;
    fieldOffset = fieldOffset;
    fieldLength = fieldLength;
    fieldData = fieldData;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvHwPciStandByDriverObjectInit
*
* DESCRIPTION:
*      This function creates and initializes PCI device driver object for
*      a standby CPU
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPciStandByDriverObjectInit
(
     void
)
{
     /* driver object initialization */
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwCntlInit          = driverHwPciCntlInit;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpReadReg         = hwPpPciReadRegister;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpWriteReg        = hwPpPciWriteRegister;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpGetRegField     = hwPpPciGetRegField;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpSetRegField     = hwPpPciSetRegField;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpReadRegBitMask  = hwPpPciReadRegBitMask;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpWriteRegBitMask = hwPpPciWriteRegBitMask;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpReadRam         = hwPpPciReadRam;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpWriteRam        = hwPpPciWriteRam;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpWriteRamRev     = hwPpPciWriteRamInReverse;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpIsrRead         = hwPpPciIsrRead;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpIsrWrite        = hwPpPciIsrWrite;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpReadIntPciReg   = hwPpPciReadInternalPciReg;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpWriteIntPciReg  = hwPpPciWriteInternalPciReg;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpResetAndInitControllerReadReg = 
                                           hwPpPciResetAndInitCtrlReadRegister;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpResetAndInitControllerWriteReg = 
                                           hwPpPciResetAndInitCtrlWriteRegister;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpResetAndInitControllerGetRegField = 
                                           hwPpPciResetAndInitCtrlGetRegField;
    prvCpssDrvMngInfPciHaStandbyPtr->drvHwPpResetAndInitControllerSetRegField = 
                                           hwPpPciResetAndInitCtrlSetRegField;

    return GT_OK;
}



/********************************************************************************
* !!!!!!!!!!!!!!!!!!!!! FOR DEBUG PURPOSES ONLY !!!!!!!!!!!!!!!!!!!!!!!!!!
********************************************************************************/

