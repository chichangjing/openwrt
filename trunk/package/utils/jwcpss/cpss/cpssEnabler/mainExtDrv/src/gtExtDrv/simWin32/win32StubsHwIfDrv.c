/*******************************************************************************
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
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
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
* gtHwIfDrv.h - Managment hardware interface driver.
*
* DESCRIPTION:
*       Enable managment hw access to GT devices via selected hardware
*       interfaces.
*
* DEPENDENCIES:
*       None.
* $Revision: 1.1.2.3 $
*******************************************************************************/



#include <gtExtDrv/drivers/gtHwIfDrv.h>
#include <gtExtDrv/drivers/gtI2cDrv.h>

#ifdef IMPL_FPGA
#include <private/USBToI2C/USBtoI2Cpro.h>
#endif

#if !(defined (ASIC_SIMULATION))
/*#warning "simulation should integrate into the PSS code"*/
#else
#include <asicSimulation/SCIB/scib.h>
#endif


/*On FPGA board all devices on the I2C bus are greater/equal to 0xB0 */
#define RHODES_STARTING_DEV (176)

#ifdef IMPL_FPGA

#define MV_ON_READ_SET_BITS31(data)    (data[0] |= 0x80)
#define MV_ON_WRITE_RESET_BIT31(data)  (data[0] &= 0x7F)
#define MV_RESET_BIT_30(data)          (data[0] &= 0xBF)

/*******************************************************************************
* mvConcatCharArrays
*
* DESCRIPTION:
*       Concatinate 2 Arrays of Chars to one Array of chars
*
* INPUTS:
*       src0 - Source Array of 4 chars long
*       src1 - Source Array of 4 chars long
*
* OUTPUTS:
*       dst - Conacatinated Array of 8 chars long {src1,src0}
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL   - on failure
*
* COMMENTS:
*           None
*
*******************************************************************************/
static GT_STATUS mvConcatCharArray (IN GT_U8 src0[4], 
                                    IN GT_U8 src1[4], 
                                    OUT GT_U8 dst[8])
{
    GT_U32 i=0,j; /*Source and Dest Counters*/

    for (j=0; j < 8; j++)
    {
        if (j < 4)
            dst[j] = src0[i++];
        else if (j == 4)
        {
            dst[j] = src1[0];
            i=1;
        } else
            dst[j] = src1[i++];
    }
    return GT_OK;
}

/*******************************************************************************
* mvLongToChar
*
* DESCRIPTION:
*       Transforms unsigned long int type to 4 separate chars.
*
* INPUTS:
*       src - source unsigned long integer.
*
* OUTPUTS:
*       dst - Array of 4 chars
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL   - on failure
*
* COMMENTS:
*           MSB is copied to dst[0]!!
*
*******************************************************************************/
static GT_STATUS mvLongToChar (IN GT_U32 src, 
                               OUT GT_U8 dst[4])
{
    GT_U32 i;

    for (i = 4 ; i > 0 ; i--)
    {
        dst[i-1] = (GT_U8) src & 0xFF;
        src>>=8;
    }

  return GT_OK;
}

/*******************************************************************************
* mvCharToLong
*
* DESCRIPTION:
*       Transforms an array of 4 separate chars to unsigned long integer type
*
* INPUTS:
*       src - Source Array of 4 chars
*
* OUTPUTS:
*       dst - Unsigned long integer number
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL   - on failure
*
* COMMENTS:
*           MSB resides in src[0]!!
*
*******************************************************************************/
static GT_STATUS mvCharToLong (IN GT_U8 src[4], 
                               OUT GT_U32 *dst)
{
    GT_U32  i;
    GT_U32  tempU32 = 0x00000000;    

    for (i = 4 ; i > 0 ; i--)
    {
        tempU32 += ((GT_U32)src[i-1]) << (8*(4-i));
    }

  *dst = tempU32;
  return GT_OK;
}
#endif
/*******************************************************************************
* extDrvMgmtReadRegister
*
* DESCRIPTION:
*       Managment read from GT devices register via specified hardware
*       interface.
*
* INPUTS:
*       mgmtIf      - Managmnet hardware interface to device.
*       devId       - Device id defined by interface for accessing device.
*                     union depends on type of hardware interface.
*       regAddr     - The device register address to read.
*
* OUTPUTS:
*       data        - Data word read from device, is valid only
*                     if status == GT_OK.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       For the white mode application the function calls for the scib service.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvMgmtReadRegister
(
    IN GT_MGMT_IF_TYPE      mgmtIf,
    IN GT_DEV_ID            devId,
    IN GT_U32               regAddr,
    OUT GT_U32              *dataPtr
)
{

#ifdef IMPL_FPGA
    
        GT_STATUS       rc;
        GT_U8           regCharAddr[4];
        GT_U8           regCharData[4];
        GT_U8           readValue[4];
        GT_U8           regCharAddrData[8];
        GT_U8           size;

        /*Phase 1: Master Drives Address and Data over TWSI*/
        mvLongToChar(regAddr, regCharAddr);
        MV_ON_READ_SET_BITS31(regCharAddr);
        MV_RESET_BIT_30(regCharAddr);
    
        size = (sizeof(regCharAddr) / sizeof(regCharAddr[0]));

        rc = I2CWrite(devId.i2cBaseAddr, size, &regCharAddr, 1);
        
        rc = I2CRead(devId.i2cBaseAddr, size, &readValue, 1);

        mvCharToLong(readValue, dataPtr);

        return rc;
    

#endif 

    scibReadMemory(devId.devNum, regAddr, 1, dataPtr);

    return (GT_OK);
}

/*******************************************************************************
* extDrvMgmtWriteRegister
*
* DESCRIPTION:
*       Managment write to GT devices registers via specified hardware
*       interface.
*
* INPUTS:
*       mgmtIf      - Managmnet hardware interface to device.
*       devId       - Device id defined by interface for accessing device.
*                     union depends on type of hardware interface.
*       regAddr     - The device register address to write.
*       data        - Data word to write to device.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       For the white mode application the function calls for the scib service.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvMgmtWriteRegister
(
    IN GT_MGMT_IF_TYPE      mgmtIf,
    IN GT_DEV_ID            devId,
    IN GT_U32               regAddr,
    IN GT_U32               data
)
{
   
#ifdef IMPL_FPGA
        
        
        GT_STATUS rc;
        GT_U8           regCharAddr[4];
        GT_U8           regCharData[4];
        GT_U8           regCharAddrData[8];
        GT_U8           size; 

        /*Phase 1: Master Drives Address and Data over TWSI*/
        mvLongToChar(regAddr, regCharAddr);
        MV_ON_WRITE_RESET_BIT31(regCharAddr);
        MV_RESET_BIT_30(regCharAddr);
    
        mvLongToChar(data, regCharData);
        mvConcatCharArray(regCharAddr,regCharData,regCharAddrData);

        size = (sizeof(regCharAddrData) / sizeof(regCharAddrData[0]));
        rc = I2CWrite(devId.i2cBaseAddr, size, &regCharAddrData, 1);
        

        return rc;
    
#endif 
   
    
    scibWriteMemory(devId.devNum, regAddr, 1, &data);

    return (GT_OK);
}

/*******************************************************************************
* extDrvMgmtIsrReadRegister
*
* DESCRIPTION:
*       Managment read from GT devices register, guaranteed to be non blocked
*       access via specified hardware interface.
*
* INPUTS:
*       mgmtIf      - Managmnet hardware interface to device.
*       devId       - Device id defined by interface for accessing device.
*                     union depends on type of hardware interface.
*       regAddr     - The device register address to read.
*
* OUTPUTS:
*       data        - Data word read from device, is valid only
*                     if status == GT_OK.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       For the white mode application the function calls for the scib service.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvMgmtIsrReadRegister
(
    IN GT_MGMT_IF_TYPE      mgmtIf,
    IN GT_DEV_ID            devId,
    IN GT_U32               regAddr,
    OUT GT_U32              *data
)
{
    scibReadMemory(devId.devNum, regAddr, 1, data);

    return (GT_OK);
}


/*******************************************************************************
* extDrvMgmtIsrWriteRegister
*
* DESCRIPTION:
*       Managment write to GT devices register, guaranteed to be non blocked
*       access via specified hardware interface.
*
* INPUTS:
*       mgmtIf      - Managmnet hardware interface to device.
*       devId       - Device id defined by interface for accessing device.
*                     union depends on type of hardware interface.
*       regAddr     - The device register address to write.
*       data        - Data word to write to device.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       For the white mode application the function calls for the scib service.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvMgmtIsrWriteRegister
(
    IN GT_MGMT_IF_TYPE      mgmtIf,
    IN GT_DEV_ID            devId,
    IN GT_U32               regAddr,
    IN GT_U32               data
)
{
    scibWriteMemory(devId.devNum, regAddr, 1, &data);

    return (GT_OK);
}



