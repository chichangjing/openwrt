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
* genDrvTwsiHwCtrl.c
*
* DESCRIPTION:
*       API implementation for TWSI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 1 $
*******************************************************************************/

/*Includes*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>  /* for NAME_MAX */
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include <gtExtDrv/drivers/gtTwsiHwCtrl.h>
#include <gtExtDrv/drivers/pssBspApis.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsTimer.h>

#include "kerneldrv/include/presteraGlob.h"

/* file descriptor returnd by openning the PP *nix device driver */
static GT_32 gtI2cFd = -1;
static GT_U32 i2cCurrentSlaveId;

/* Macroes definitions */
#define TWSI_SLAVE_ADDR(data)     (data)

#define MV_ON_READ_SET_BITS31(data)    (data[0] |= 0x80)
#define MV_ON_WRITE_RESET_BIT31(data)  (data[0] &= 0x7F)
#define MV_RESET_BIT_30(data)          (data[0] &= 0xBF)

static GT_STATUS mvLongToChar (IN GT_U32 src, 
                               OUT GT_U8 dst[4]);

static GT_STATUS mvCharToLong (IN GT_U8 src[4], 
                               OUT GT_U32 *dst);

static GT_STATUS mvConcatCharArray (IN GT_U8 src0[4], 
                                    IN GT_U8 src1[4], 
                                    OUT GT_U8 dst[8]);

#define I2C_SLAVE       0x0703  /* Use this slave address */
#define I2C_SMBUS       0x0720  /* SMBus transfer */
#define I2C_SMBUS_BLOCK_MAX 32  /* As specified in SMBus standard */

#define I2C_SMBUS_READ      1       /* smbus_access read or write markers */
#define I2C_SMBUS_WRITE     0       /* smbus_access read or write markers */

#define I2C_SMBUS_QUICK         0
#define I2C_SMBUS_BYTE          1
#define I2C_SMBUS_BYTE_DATA     2
#define I2C_SMBUS_WORD_DATA     3
#define I2C_SMBUS_PROC_CALL     4
#define I2C_SMBUS_BLOCK_DATA        5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7       /* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

/* This is the structure as used in the I2C_SMBUS ioctl call */
struct i2c_smbus_ioctl_data {
    unsigned char read_write;
    unsigned char command;
    unsigned long size;
    union i2c_smbus_data *data;
};


union i2c_smbus_data {
    unsigned char byte;
    unsigned short word;
    unsigned char block[I2C_SMBUS_BLOCK_MAX + 2]; /* block[0] is used for length */
                                                /* and one more for PEC */
};

GT_STATUS hwIfTwsiSetSlaveId
(
    IN GT_U32 devSlvId
)
{
    if(devSlvId == i2cCurrentSlaveId)
    {
        return GT_OK;
    }

    if (ioctl(gtI2cFd, I2C_SLAVE, devSlvId) < 0)
    {
        fprintf(stderr, "Error: Could not set slave address: %s\n", strerror(errno));
        return GT_FAIL;
    }

    i2cCurrentSlaveId = devSlvId;

    osTimerWkAfter(10);

    return GT_OK;
}

/*******************************************************************************
* hwIfTwsiWriteReg
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
*       GT_OK    - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_U8           regCharAddr[4];
    GT_U8           regCharData[4];
    GT_U8           regCharAddrData[8];
    struct GT_TwsiReadWrite_STC twsiWritePrm;

    /*Phase 1: Master Drives Address and Data over TWSI*/
    mvLongToChar (regAddr, regCharAddr);
    MV_ON_WRITE_RESET_BIT31 (regCharAddr);
    MV_RESET_BIT_30 (regCharAddr);

    mvLongToChar (value, regCharData);
    mvConcatCharArray(regCharAddr,regCharData,regCharAddrData);

    if (ioctl (gtI2cFd, PRESTERA_IOC_TWSIWAITNOBUSY) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWAITNOBUSY) fail errno(%s)\n",strerror(errno));
        return GT_FAIL;
    }

    twsiWritePrm.devId  = TWSI_SLAVE_ADDR(devSlvId);
    twsiWritePrm.len    = 8;
    twsiWritePrm.pData  = (mv_kmod_uintptr_t)((uintptr_t)regCharAddrData);
    twsiWritePrm.stop   = GT_TRUE;

    if (ioctl (gtI2cFd, PRESTERA_IOC_TWSIWRITE, &twsiWritePrm) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWRITE) fail errno(%s)\n",strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}


/*******************************************************************************
* hwIfTwsiReadReg
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
GT_STATUS hwIfTwsiReadReg
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    GT_U8           twsiRdDataBuff[4];
    GT_U8           regCharAddr[4];       
    struct GT_TwsiReadWrite_STC twsiWritePrm;

    /*PHASE 1: Write Structure, Master drives regAddr over TWSI*/
    mvLongToChar(regAddr, regCharAddr);
    MV_ON_READ_SET_BITS31 (regCharAddr);
    MV_RESET_BIT_30 (regCharAddr);
    
    if (ioctl (gtI2cFd, PRESTERA_IOC_TWSIWAITNOBUSY) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWAITNOBUSY) fail errno(%s)\n",strerror(errno));
        return GT_FAIL;
    }

    twsiWritePrm.devId  = TWSI_SLAVE_ADDR(devSlvId);
    twsiWritePrm.len    = 4;
    twsiWritePrm.pData  = (mv_kmod_uintptr_t)((uintptr_t)regCharAddr);
    twsiWritePrm.stop   = GT_FALSE;

    if (ioctl (gtI2cFd, PRESTERA_IOC_TWSIWRITE, &twsiWritePrm) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIWRITE) fail errno(%s)\n",strerror(errno));
        return GT_FAIL;
    }

    twsiWritePrm.devId  = TWSI_SLAVE_ADDR(devSlvId);
    twsiWritePrm.len    = 4;
    twsiWritePrm.pData  = (mv_kmod_uintptr_t)((uintptr_t)twsiRdDataBuff);
    twsiWritePrm.stop   = GT_TRUE;

    if (ioctl (gtI2cFd, PRESTERA_IOC_TWSIREAD, &twsiWritePrm) < 0)
    {
        fprintf(stderr, "hwIfTwsiWriteReg(PRESTERA_IOC_TWSIREAD) fail errno(%s)\n",strerror(errno));
        return GT_FAIL;
    }
    
    mvCharToLong(twsiRdDataBuff, dataPtr);

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
* hwIfTwsiInitDriver
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
*       GT_OK     - on success
*       GT_FAIL   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiInitDriver
(
    GT_VOID
)
{
    if (gtI2cFd >= 0)
        return GT_OK;

#if (defined CPU_MSYS) || (defined CPU_ARMADA_385)
    /* Open i2c-0 file for MSYS handling */
    gtI2cFd = open("/dev/i2c-0", O_RDWR);

#else
    /* Open i2c-1 file for ARMADA_XP handling */
    gtI2cFd = open("/dev/i2c-1", O_RDWR);

#endif

    if (gtI2cFd < 0)
    {
        fprintf(stderr, "failed to open /dev/i2c-*: %s\n", strerror(errno));
        return GT_FAIL;
    }

    i2cCurrentSlaveId = 0xFFFFFFFF;

    return GT_OK;
}


/*******************************************************************************
* hwIfTwsiWriteData
*
* DESCRIPTION:
*       Generic TWSI Write operation.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       dataPtr  - (pointer to) data to be send on the TWSI.
*                   dataPtr[0] - should include start offset 
*       dataLen  - number of bytes to send on the TWSI (from dataPtr).
*                  range: 1-9
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on hardware error
*       GT_BAD_PARAM    - on wrong parameters
*       GT_BAD_PTR      - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiWriteData
(
    IN GT_U32 devSlvId,
    IN GT_U8  *dataPtr,
    IN GT_U8  dataLen
)
{
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;
    GT_U32 regAddr;
    GT_U32 i;

    if( dataPtr == NULL )
    {
        return GT_BAD_PTR;
    }

    if( dataLen < 1 || dataLen > 9 )
    {
        return GT_BAD_PARAM;
    }

    if(GT_OK != hwIfTwsiSetSlaveId(devSlvId))
    {
        return GT_FAIL;
    }

    regAddr = dataPtr[0];

    /* Write data */
    args.read_write = I2C_SMBUS_WRITE;
    args.command = regAddr; /* Byte Address */
    args.size = I2C_SMBUS_BLOCK_DATA;

    data.block[0] = dataLen;
    for (i = 1; i <= dataLen; i++)
    {
        data.block[i] = dataPtr[i];
    }

    args.data = &data;

    if (ioctl(gtI2cFd, I2C_SMBUS, &args) < 0)
    {
        fprintf(stderr, "Error: Failed to to perform SMBUS IOCTL READ transaction: %s\n", strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* hwIfTwsiReadData
*
* DESCRIPTION:
*       Generic TWSI Read operation.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       dataLen  - number of bytes to recieve on the TWSI (into dataPtr).
*                  range: 1-8.
*       dataPtr  - dataPtr[0] should include start offset
*
* OUTPUTS:
*       dataPtr  - (pointer to) data to be send on the TWSI.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on hardware error
*       GT_BAD_PARAM    - on wrong parameters
*       GT_BAD_PTR      - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiReadData
(               
    IN GT_U32 devSlvId,
    IN GT_U8  dataLen,
    INOUT GT_U8 *dataPtr
)
{
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;
    GT_U32 regAddr;
    GT_U32 i;

    if( dataPtr == NULL )
    {
        return GT_BAD_PTR;
    }

    if( dataLen < 1 || dataLen > 9 )
    {
        return GT_BAD_PARAM;
    }

    if(GT_OK != hwIfTwsiSetSlaveId(devSlvId))
    {
        return GT_FAIL;
    }

    regAddr = dataPtr[0];

    /* Write command */
    args.read_write = I2C_SMBUS_WRITE;
    args.command = regAddr; /* Byte Address */
    args.size = I2C_SMBUS_BYTE;
    args.data = NULL;
    if (ioctl(gtI2cFd, I2C_SMBUS, &args) < 0)
    {
        fprintf(stderr, "Error: Failed to to perform SMBUS IOCTL WRITE transaction: %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Read data block */
    args.read_write = I2C_SMBUS_READ;
    args.command = regAddr; /* Byte Address */
    args.size = I2C_SMBUS_BLOCK_DATA;

    data.block[0] = dataLen;
    args.data = &data;

    if (ioctl(gtI2cFd, I2C_SMBUS, &args) < 0)
    {
        fprintf(stderr, "Error: Failed to to perform SMBUS IOCTL READ transaction: %s\n", strerror(errno));
        return GT_FAIL;
    }

    for (i = 1; i <= data.block[0]; i++)
    {
        dataPtr[i] = data.block[i];
    }

    return GT_OK;
}

/*******************************************************************************
* hwIfTwsiWriteByte
*
* DESCRIPTION:
*       Writes the 8 bits of a register using TWSI.
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
*       GT_OK    - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiWriteByte
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;

    if(GT_OK != hwIfTwsiSetSlaveId(devSlvId))
    {
        return GT_FAIL;
    }

    /* Write data */
    args.read_write = I2C_SMBUS_WRITE;
    args.command = regAddr; /* Byte Address */
    args.size = I2C_SMBUS_BYTE_DATA;

    data.byte = (unsigned char) value;

    args.data = &data;
    if (ioctl(gtI2cFd, I2C_SMBUS, &args) < 0)
    {
        fprintf(stderr, "Error: Failed to to perform SMBUS IOCTL READ transaction: %s\n", strerror(errno));
        return GT_FAIL;
    }

    return GT_OK;
}


/*******************************************************************************
* hwIfTwsiReadByte
*
* DESCRIPTION:
*       Reads the 8 bits of a register using TWSI.
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
GT_STATUS hwIfTwsiReadByte
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;

    if(GT_OK != hwIfTwsiSetSlaveId(devSlvId))
    {
        return GT_FAIL;
    }

    /* Write command */
    args.read_write = I2C_SMBUS_WRITE;
    args.command = regAddr; /* Byte Address */
    args.size = I2C_SMBUS_BYTE;
    args.data = NULL;
    if (ioctl(gtI2cFd, I2C_SMBUS, &args) < 0)
    {
        fprintf(stderr, "Error: Failed to to perform SMBUS IOCTL WRITE transaction: %s\n", strerror(errno));
        return GT_FAIL;
    }

    /* Read data */
    args.read_write = I2C_SMBUS_READ;
    args.command = regAddr; /* Byte Address */
    args.size = I2C_SMBUS_BYTE_DATA;
    args.data = &data;
    if (ioctl(gtI2cFd, I2C_SMBUS, &args) < 0)
    {
        fprintf(stderr, "Error: Failed to to perform SMBUS IOCTL READ transaction: %s\n", strerror(errno));
        return GT_FAIL;
    }

    *dataPtr = data.byte;

    return GT_OK;
}

