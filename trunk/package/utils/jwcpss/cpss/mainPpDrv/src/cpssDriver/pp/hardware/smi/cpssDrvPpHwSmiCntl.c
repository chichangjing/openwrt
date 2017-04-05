/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtHwCntlSmi.c
*
* DESCRIPTION:
*       Prestera SMI driver Hardware read and write functions implementation.
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

extern GT_BOOL  prvCpssDrvTraceHwWrite[PRV_CPSS_MAX_PP_DEVICES_CNS];
extern GT_BOOL  prvCpssDrvTraceHwRead[PRV_CPSS_MAX_PP_DEVICES_CNS];

#ifdef HW_DEBUG
#define debugPrint(_x)      cpssOsPrintf _x
#else
#define debugPrint(_x)
#endif  /* VLAN_DEBUG */

/* in SMI -- no swap needed */
#define HW_BYTE_SWAP(devNum,portGroupId,data)   data

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
* driverHwSmiCntlInit
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
static GT_STATUS driverHwSmiCntlInit
(
    IN GT_U8        devNum,
    IN GT_U32       portGroupId,
    IN GT_UINTPTR   baseAddr,
    IN GT_UINTPTR   internalPciBase,
    IN GT_BOOL      isDiag
)
{
    /* fix warnings */
    internalPciBase = internalPciBase;
    isDiag = isDiag;

    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr           = baseAddr;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].compIdx            = 0;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase    = 0;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[0]   = 0;
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].lastWriteAddr[1]   = 0;

    cpssExtDrvHwIfSmiInitDriver();

    return GT_OK;
}
/*******************************************************************************
* hwPpSmiReadRegister
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
static GT_STATUS hwPpSmiReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    GT_STATUS retVal;
    GT_U32    temp;

    debugPrint(("Read from dev %d, reg 0x%x,",devNum, regAddr));
    retVal = cpssExtDrvHwIfSmiTaskReadReg(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr,
                                regAddr, &temp);
    *data = HW_BYTE_SWAP(devNum, portGroupId,temp);
    debugPrint((" data 0x%x.\n", *data));
    if (retVal != GT_OK) 
    {
        return retVal;
    }

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE) 
    {
        /* trace HW read access */
        retVal = cpssTraceHwAccessRead(devNum, 
                                       portGroupId, 
                                       GT_FALSE, /* ISR context */
                                       CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                       regAddr, 
                                       1, /* length to trace the data in words */
                                       data);
    }

    return retVal;
}




/*******************************************************************************
* hwPpSmiWriteRegister
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
static GT_STATUS hwPpSmiWriteRegister
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_STATUS retVal;
    GT_U32    temp;

    debugPrint(("Write to device %d, reg 0x%x, data 0x%x.\n",devNum,regAddr,
                value));

    temp = HW_BYTE_SWAP(devNum, portGroupId,value);
    retVal = cpssExtDrvHwIfSmiTaskWriteReg(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr,
                                 regAddr, temp);
    if (retVal != GT_OK) 
    {
        return retVal;
    }

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE) 
    {
        /* trace HW write access */
        retVal = cpssTraceHwAccessWrite(devNum, 
                                       portGroupId, 
                                       GT_FALSE, /* ISR context */
                                       CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                       regAddr, 
                                       1, /* length to trace the data in words */
                                       &value);
    }

    return retVal;
}

/*******************************************************************************
* hwPpSmiGetRegField
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
static GT_STATUS hwPpSmiGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
)

{
    GT_STATUS retVal;
    GT_U32 data, mask;

    debugPrint(("GetField from dev %d, reg 0x%x, offset %d, length %d,",
                devNum, regAddr, fieldOffset, fieldLength));

    retVal = hwPpSmiReadRegister(devNum, portGroupId,regAddr,&data);

    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    *fieldData = (GT_U32)((data & mask) >> fieldOffset);

    debugPrint(("data 0x%x.\n", *fieldData));

    return retVal;
}



/*******************************************************************************
* hwPpSmiSetRegField
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
*       this function actually reads the register value modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
static GT_STATUS hwPpSmiSetRegField
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData

)
{
    GT_STATUS retVal;
    GT_U32 data, mask;

    debugPrint(("SetField to dev %d, reg 0x%x, offset %d, length %d, \
        data 0x%x.\n",devNum, regAddr, fieldOffset, fieldLength, fieldData));

    retVal = hwPpSmiReadRegister( devNum,portGroupId, regAddr,&data);

    if (GT_OK != retVal)
    {
        return retVal;
    }
    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    data &= ~mask;   /* turn the field off */
    /* insert the new value of field in its place */
    data |= ((fieldData << fieldOffset) & mask);

    retVal = hwPpSmiWriteRegister( devNum, portGroupId,regAddr, data);

    return retVal;

}




/*******************************************************************************
* hwPpSmiReadRegBitMask
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
static GT_STATUS hwPpSmiReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
)
{
    GT_STATUS retVal;
    GT_U32 data;

    debugPrint(("\nhwPpReadRegBitMask on device %d, from register 0x%x, \
                mask 0x%x,", devNum, regAddr, mask));

    retVal = hwPpSmiReadRegister(devNum,portGroupId, regAddr,&data);
    *dataPtr = (data) & mask;

    debugPrint((" data is 0x%x.\n", *dataPtr));
    return retVal;


}

/*******************************************************************************
* hwPpSmiWriteRegBitMask
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
static GT_STATUS hwPpSmiWriteRegBitMask
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 value
)
{
    GT_STATUS retVal;
    GT_U32 data;

    debugPrint(("\nhwPpWriteRegBitMask on device %d, from register 0x%x, \
                mask 0x%x, data is 0x%x.\n",
                devNum, regAddr, mask, value));

    /* in case mask = 0xffffffff call hwWrite only */
    if(mask == 0xffffffff)
    {
        retVal = hwPpSmiWriteRegister(devNum,portGroupId, regAddr, value);
        return retVal;
    }

    /* else read modify write */
    retVal = hwPpSmiReadRegister(devNum, portGroupId,regAddr, &data);
    if (GT_OK != retVal)
    {
        return retVal;
    }
    data &= (~mask);            /* Turn the field off.                        */
    data |= (value & mask);     /* Insert the new value of field in its place.*/

    retVal = hwPpSmiWriteRegister(devNum, portGroupId,regAddr, data);

    return retVal;

}


/*******************************************************************************
* hwPpSmiReadRam
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
static GT_STATUS hwPpSmiReadRam
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    OUT GT_U32  *data
)
{
    GT_STATUS retVal;
    GT_U32    i;
    GT_U32    baseAddress;

    baseAddress = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr;

    retVal = cpssExtDrvHwIfSmiTskRegRamRead(baseAddress, addr, data, length);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    for (i = 0; i < length; i++)
    {
        data[i] = HW_BYTE_SWAP(devNum, portGroupId,data[i]);
    }

#ifdef HW_DEBUG
    for( i = 0; i < length; i++)
    {
        debugPrint(("\hwPpReadRam on device %d, from address 0x%x, \
                    data is 0x%x.\n",
                    devNum, addr + 4 * i, data[i]));
    }
#endif


    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE) 
    {
        /* trace HW read access */
        retVal = cpssTraceHwAccessRead(devNum, 
                                       portGroupId, 
                                       GT_FALSE, /* ISR context */
                                       CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                       addr, 
                                       length, /* length to trace the data in words */
                                       data);
    }

    return retVal;
}


/*******************************************************************************
* hwPpSmiWriteRam
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
static GT_STATUS hwPpSmiWriteRam
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    GT_U32    i;
    GT_U32    baseAddress;
    GT_STATUS retVal;

#ifdef HW_DEBUG
    for( i = 0; i < length; i++)
    {
        debugPrint(("\hwPpWriteRam on device %d, from address 0x%x, \
                    data is 0x%x.\n",
                    devNum, addr + i * 4, data[i]));
    }
#endif

    for (i = 0; i < length; i++)
    {
        data[i] = HW_BYTE_SWAP(devNum, portGroupId,data[i]);
    }

    baseAddress = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr;

    retVal = cpssExtDrvHwIfSmiTskRegRamWrite(baseAddress, addr, data, length);
    if (retVal != GT_OK) 
    {
        return retVal;
    }

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE) 
    {
        /* trace HW write access */
        retVal = cpssTraceHwAccessWrite(devNum, 
                                       portGroupId, 
                                       GT_FALSE, /* ISR context */
                                       CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                       addr, 
                                       length, /* length to trace the data in words */
                                       data);
    }

    return retVal;
}


/*******************************************************************************
* hwPpSmiReadVec
*
* DESCRIPTION:
*       Read from PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id.
*       addrArr - Address array to read from.
*       arrLen  - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpSmiReadVec
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen
)
{
    GT_STATUS   retVal;
    GT_U32      i;
    GT_U32      baseAddress;

    baseAddress = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr;

    retVal = cpssExtDrvHwIfSmiTskRegVecRead(baseAddress, addrArr, dataArr, arrLen);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    for (i = 0; i < arrLen; i++)
    {
        dataArr[i] = HW_BYTE_SWAP(devNum, portGroupId,dataArr[i]);
    }

#ifdef HW_DEBUG
    for( i = 0; i < arrLen; i++)
    {
        debugPrint(("\hwPpReadRam on device %d, from address 0x%x, \
                    data is 0x%x.\n",
                    devNum, addrArr[i], dataArr[i]));
    }
#endif

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE) 
    {
        for( i = 0; i < arrLen; i++)
        {
            /* trace HW read access */
            retVal = cpssTraceHwAccessRead(devNum, 
                                       portGroupId, 
                                       GT_FALSE, /* ISR context */
                                       CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                       addrArr[i], 
                                       1, /* length to trace the data in words */
                                       &dataArr[i]);
            if (retVal != GT_OK) 
            {
                return retVal;
            }
        }
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpSmiWriteVec
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id.
*       addrArr - Address offset to write to.
*       dataArr - An array containing the data to be written.
*       arrLen  - The size of addrArr/dataArr.
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
static GT_STATUS hwPpSmiWriteVec
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
)
{
    GT_U32      i;
    GT_U32      baseAddress;
    GT_STATUS   retVal;

#ifdef HW_DEBUG
    for( i = 0; i < arrLen; i++)
    {
        debugPrint(("\hwPpWriteRam on device %d, from address 0x%x, \
                    data is 0x%x.\n",
                    devNum, addrArr[i], dataArr[i]));
    }
#endif

    for (i = 0; i < arrLen; i++)
    {
        dataArr[i] = HW_BYTE_SWAP(devNum, portGroupId,dataArr[i]);
    }

    baseAddress = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr;

    retVal = cpssExtDrvHwIfSmiTskRegVecWrite(baseAddress, addrArr, dataArr, arrLen);
    if (retVal != GT_OK) 
    {
        return retVal;
    }

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE) 
    {
        for( i = 0; i < arrLen; i++)
        {
            /* trace HW write access */
            retVal = cpssTraceHwAccessWrite(devNum, 
                                            portGroupId, 
                                            GT_FALSE, /* ISR context */
                                            CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                            addrArr[i], 
                                            1, /* length to trace the data in words */
                                            &dataArr[i]);
            if (retVal != GT_OK) 
            {
                return retVal;
            }
        }
    }

    return GT_OK;
}


/*******************************************************************************
* hwPpSmiIsrRead
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
static GT_INLINE GT_STATUS hwPpSmiIsrRead
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    GT_STATUS retVal;
    GT_U32    temp;

    retVal = cpssExtDrvHwIfSmiInterruptReadReg(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr,
                                     regAddr, &temp);
    *dataPtr = HW_BYTE_SWAP(devNum, portGroupId,temp);
    if (retVal != GT_OK) 
    {
        return retVal;
    }

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE) 
    {
        /* trace HW read access */
        retVal = cpssTraceHwAccessRead(devNum, 
                                       portGroupId, 
                                       GT_TRUE, /* ISR context */
                                       CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                       regAddr, 
                                       1, /* length to trace the data in words */
                                       dataPtr);
    }

    return retVal;
}


/*******************************************************************************
* hwPpSmiIsrWrite
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
static GT_STATUS hwPpSmiIsrWrite
(
    IN GT_U8 devNum,
    IN GT_U32   portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_STATUS retVal;
    GT_U32    temp;

    debugPrint(("Write to device %d, reg 0x%x, data 0x%x.\n",devNum,regAddr,
                value));

    temp = HW_BYTE_SWAP(devNum, portGroupId,value);
    retVal = cpssExtDrvHwIfSmiInterruptWriteReg(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr,
                                      regAddr, temp);
    if (retVal != GT_OK) 
    {
        return retVal;
    }

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE) 
    {
        /* trace HW write access */
        retVal = cpssTraceHwAccessWrite(devNum, 
                                        portGroupId, 
                                        GT_TRUE, /* ISR context */
                                        CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E, /* Address space */
                                        regAddr, 
                                        1, /* length to trace the data in words */
                                        &value);
    }

    return retVal;
}

/*******************************************************************************
* hwPpSmiReadInternalPciReg
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
static GT_STATUS hwPpSmiReadInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_STATUS retVal;

    portGroupId = portGroupId;
    /* here we go directly to the "BUS" */
    retVal = cpssExtDrvHwIfSmiTaskReadReg(devNum, regAddr, data);
    if (retVal != GT_OK) 
    {
        return retVal;
    }

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[devNum] == GT_TRUE) 
    {
        /* trace HW read access */
        retVal = cpssTraceHwAccessRead(devNum, 
                                       portGroupId, 
                                       GT_FALSE, /* ISR context */
                                       CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E, /* Address space */
                                       regAddr, 
                                       1, /* length to trace the data in words */
                                       data);
    }

    return retVal;
}


/*******************************************************************************
* hwPpSmiWriteInternalPciReg
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
static GT_STATUS hwPpSmiWriteInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    GT_STATUS retVal;

    portGroupId = portGroupId;
    /* here we go directly to the "BUS" */
    retVal = cpssExtDrvHwIfSmiTaskWriteReg(devNum, regAddr, data);
    if (retVal != GT_OK) 
    {
        return retVal;
    }

    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[devNum] == GT_TRUE) 
    {
        /* trace HW write access */
        retVal = cpssTraceHwAccessWrite(devNum, 
                                        portGroupId, 
                                        GT_FALSE, /* ISR context */
                                        CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E, /* Address space */
                                        regAddr, 
                                        1, /* length to trace the data in words */
                                        &data);
    }

    return retVal;
}

/*******************************************************************************
* dummyWriteRamReverse
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
*       GT_NOT_SUPPORTED - always
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS dummyWriteRamReverse
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

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
}


/*******************************************************************************
* prvCpssDrvHwSmiDriverObjectInit
*
* DESCRIPTION:
*      This function creates and initializes SMI device driver object
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
GT_STATUS prvCpssDrvHwSmiDriverObjectInit
(
     void
)
{
#ifdef INCL_EXDXMX_DRIVER
     /* driver object initialization */
    prvCpssDrvMngInfSmiPtr->drvHwCntlInit          = driverHwSmiCntlInit;
    prvCpssDrvMngInfSmiPtr->drvHwPpReadReg         = hwPpSmiReadRegister;
    prvCpssDrvMngInfSmiPtr->drvHwPpWriteReg        = hwPpSmiWriteRegister;
    prvCpssDrvMngInfSmiPtr->drvHwPpGetRegField     = hwPpSmiGetRegField;
    prvCpssDrvMngInfSmiPtr->drvHwPpSetRegField     = hwPpSmiSetRegField;
    prvCpssDrvMngInfSmiPtr->drvHwPpReadRegBitMask  = hwPpSmiReadRegBitMask;
    prvCpssDrvMngInfSmiPtr->drvHwPpWriteRegBitMask = hwPpSmiWriteRegBitMask;
    prvCpssDrvMngInfSmiPtr->drvHwPpReadRam         = hwPpSmiReadRam;
    prvCpssDrvMngInfSmiPtr->drvHwPpWriteRam        = hwPpSmiWriteRam;
    prvCpssDrvMngInfSmiPtr->drvHwPpReadVec         = hwPpSmiReadVec;
    prvCpssDrvMngInfSmiPtr->drvHwPpWriteVec        = hwPpSmiWriteVec;
    prvCpssDrvMngInfSmiPtr->drvHwPpWriteRamRev     = dummyWriteRamReverse;
    prvCpssDrvMngInfSmiPtr->drvHwPpIsrRead         = hwPpSmiIsrRead;
    prvCpssDrvMngInfSmiPtr->drvHwPpIsrWrite        = hwPpSmiIsrWrite;
    prvCpssDrvMngInfSmiPtr->drvHwPpReadIntPciReg   = hwPpSmiReadInternalPciReg;
    prvCpssDrvMngInfSmiPtr->drvHwPpWriteIntPciReg  = hwPpSmiWriteInternalPciReg;
#endif /* INCL_SOHO_DRIVER */

    return GT_OK;
}



