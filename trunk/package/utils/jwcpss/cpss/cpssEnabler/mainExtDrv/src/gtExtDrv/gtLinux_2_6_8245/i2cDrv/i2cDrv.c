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
* gtI2c.c - I2C unit c file.
*
* DESCRIPTION
*       Functions to control the I2C.
* DEPENDENCIES:
*       None.
*
*       $Revision: 1.1.2.2 $
*
*******************************************************************************/

/*includes*/
#include <gtExtDrv/os/extDrvOs.h>
#include "i2cDrv.h"
#include <private/8245/gtCore.h>
#include <cpssCommon/cpssPresteraDefs.h>

#ifdef PRESTERA_DEBUG
#define I2C_DEBUG
#endif

#ifdef I2C_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif


/********* Internal functions *************************************************/
I2C_DATA_STRUCT gtI2cGenerateStartBit(GT_VOID );
GT_VOID         gtI2cGenerateStopBit (GT_VOID);
GT_VOID         gtI2cIntEnable       (GT_VOID);
GT_VOID         gtI2cIntDisable      (GT_VOID);
GT_VOID         gtI2cSetAckBit       (GT_VOID);
GT_STATUS       gtI2cReadStatus      (GT_VOID);
GT_BOOL         gtI2cReadIntFlag     (GT_VOID);
GT_VOID         gtI2cClearIntFlag    (GT_VOID);
GT_VOID         gtI2cReset           (GT_VOID);



/********* Variables **********************************************************/
GT_U32 gtI2cTclock;

/*******************************************************************************
* gtI2cSlaveInit - Initializes the I2C slave address and the general call enable
*                  register.
*
* DESCRIPTION:
*       This function initialize the GT as a slave, with the given address.
*       If the desired device address is a 10 bit address, add (logical OR)
*       to the deviceAddress parameter the value I2C_10BIT_ADDR defined in
*       gtI2c.h. The parameter generalCallStatus (defind in gtI2c.h) indicates
*       if the slave will answer general call or not. If set to 1 the I2C slave
*       interface responds to general call accesses. if set to 0 the I2C slave
*       interface will not responds to general call accesses.The general call,
*       if enabled causes the GT (while acting as a slave) to respond to any
*       access regardless of the targeted address.
*     NOTE:
*       before using this function you must call 'i2cMasterInit' function to
*       initialize the I2C mechanism.
*
* INPUT:
*       deviceAddress - the desired address to assigned to the GT slave address
*                       register.
*       generalCallStatus - inidicates whether the slave will respond to general
*                           call or not (values defind in gtI2c.h).
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
GT_VOID gtI2cSlaveInit
(
    IN GT_U32                   deviceAddress,
    IN I2C_GENERAL_CALL_STATUS  generalCallStatus
)
{
    GT_U32 tempAddress;

    /* 10 Bit deviceAddress */
    if((deviceAddress & I2C_10BIT_ADDR) == I2C_10BIT_ADDR)
    {
        /* writing the 2 most significant bits of the 10 bit address*/
        GT_REG_WRITE(I2C_SLAVE_ADDR,(((deviceAddress & 0X300) >> 7)|0Xf0) |
                      generalCallStatus);
        /* writing the 8 least significant bits of the 10 bit address*/
        tempAddress = (deviceAddress & 0xff);
        GT_REG_WRITE(I2C_EXTENDED_SLAVE_ADDR,tempAddress);
    }
    /*7 bit address*/
    else
    {
       GT_REG_WRITE(I2C_EXTENDED_SLAVE_ADDR,0x0);
       GT_REG_WRITE(I2C_SLAVE_ADDR,(deviceAddress<<1) | generalCallStatus);
    }
}

/*******************************************************************************
* gtI2cMasterInit - Initializes the I2C mechanism integrated in the GT.
*
* DESCRIPTION:
*       This function initialize the I2C mechanism and must be called before
*       any attemp to use the I2C bus ( use this function on both master or
*       slave mode to initialize the I2C mechanism).The function calculates the
*       parameters needed for the I2C's freuency registers, resets the I2C and
*       then enables it.
*
* INPUT:
*       i2cFreq - the desired frequency , values defind in gtI2c.h and can be
*                 either _100KHZ or _400KHZ.
*       tclk    - The system's Tclock.
*
* OUTPUT:
*       I2C mechanism is enabled.
*       gtI2cTclock is set to tclk value.
*
* RETURN:
*       the actual frequancy calculated and assigned to the I2C baude - rate
*       register (for more details please see the I2C section in the GT
*       datasheet).
*
*******************************************************************************/
GT_U32 gtI2cMasterInit
(
    IN I2C_FREQ i2cFreq,
    IN GT_U32   tclk
)
{
    GT_U32    n,m;
    GT_U32    freq;
    GT_U32    margin;
    GT_U32    minMargin = 0xffffffff;
    GT_U32    power;
    GT_U32    actualFreq = 0;
    GT_U32    actualN = 0;
    GT_U32    actualM = 0;

    for(n = 0 ; n < 8 ; n++)
    {
        for(m = 0 ; m < 16 ; m++)
        {
            power = 2<<n; /* power = 2^(n+1) */
            freq = tclk/(10*(m+1)*power);
            margin = ((i2cFreq - freq)>0)?(i2cFreq - freq):(freq- i2cFreq);
            if(margin < minMargin)
            {
                minMargin   = margin;
                actualFreq  = freq;
                actualN     = n;
                actualM     = m;
            }
        }
    }
    /* Reset the I2C logic */
    gtI2cReset();
    /* Set the baud-rate */
    GT_REG_WRITE(I2C_STATUS_BAUDE_RATE,(actualM<<3) | actualN);
    /* Enable the I2C and slave */
    GT_REG_WRITE(I2C_CONTROL,I2C_ENABLE|I2C_ACK);
    gtI2cTclock = tclk;

    return (actualFreq);
}

/*******************************************************************************
* gtI2cIntEnable - Inables the I2C's interrupt generation.
*
* DESCRIPTION:
*      This function enables the GT interrupt mode. After calling  this function
*      an interrupt will be generated each time the interrupt flag is set . If
*      any of the status codes other than 0xf8 are set , the I2C hardware sets
*      the interrupt flag and ( if  interrupt  is enabled ) an interrupt will be
*      generated .When interrupt enable bit is cleared to zero, the interrupt
*      line will always remain low.
*     NOTE:
*      It is highly recommended to use I2C interrupt to interface the I2C
*      module, rather than using register polling method.
*
* INPUT:
*      None.
*
* OUTPUT:
*      Set I2C_INT_ENABLE bit in I2C_CONTROL register.
*
* RETURN:
*      None.
*
*******************************************************************************/
GT_VOID gtI2cIntEnable
(
    GT_VOID
)
{
    GT_U32  temp;

    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp | I2C_INT_ENABLE);
}

/*******************************************************************************
* gtI2cIntDisable - Disables the I2C's interrupt generation.
*
* DESCRIPTION:
*       This function disables the I2C's interrupt generation described in
*       'gtI2cIntEnable'.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Unset I2C_INT_ENABLE bit in I2C_CONTROL register.
*
* RETURN:
*       None.
*
*******************************************************************************/
GT_VOID gtI2cIntDisable
(
    GT_VOID
)
{
    GT_U32 temp;

    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp & ~I2C_INT_ENABLE);
}
/*******************************************************************************
* gtI2cReadStatus - Reads the I2C's status register
*
* DESCRIPTION:
*       This function reads the status register and return the status code
*       generated in the last operation.There are 31 aveliable codes (defind
*       in gtI2c.h ) that can be assigned to the status register (also called
*       baude - rate register) each time one of the statuses occur.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       The status code.
*
*******************************************************************************/
GT_STATUS gtI2cReadStatus
(
    GT_VOID
)
{
    GT_U32 status;

    GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);

    return (status);
}

/*******************************************************************************
* gtI2cReadIntFlag - Reads the I2C's interrupt flag status.
*
* DESCRIPTION:
*       This function reads the I2C's interrupt flag status .The interrupt flag
*       is assigned each time when one of the status codes other than 0xf8 are
*       set.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       1 if the interrupt flag of the I2C is set
*       0 otherwise.
*
*******************************************************************************/
GT_BOOL gtI2cReadIntFlag
(
    GT_VOID
)
{
    GT_U32 temp;

    GT_REG_READ(I2C_CONTROL,&temp);

    if( (temp & I2C_INT_FLAG) != 0)
        return (GT_TRUE);
    else
        return (GT_FALSE);
}

/*******************************************************************************
* gtI2cClearIntFlag - Clears the interrupt flag bit in the control register.
*
* DESCRIPTION:
*       This function clears the interrupt flag bit.The interrupt flag bit is
*       set to '1' automatically when any of 30 of the possible 31 statuses is
*       entered. The only status that does not set IFLG is status F8h. When
*       interrupt flag is set , the low period of the I2C bus clock line (SCL)
*       is stretched and the data transfer is suspended. After calling this
*       function, zero is written to interrupt flag, the interrupt line goes
*       low (in case interrupt is enable, see function 'i2cIntEnable') and the
*       I2C clock line is released . It is necessary to use this function each
*       time one of the possible 31 statuses is entered to release the bus and
*       allow data transfer to continue.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Clear bit I2C_INT_FLAG in the I2C_CONTROL register.
*
* RETURN:
*       None.
*
*******************************************************************************/
GT_VOID gtI2cClearIntFlag
(
    GT_VOID
)
{
    GT_U32 temp;

    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp & (~I2C_INT_FLAG));
}

/*******************************************************************************
* gtI2cReset - Reset the I2C's logic and set its registers values to their
*              defaults.
*
* DESCRIPTION:
*       This function resets the I2C mechanism . After calling this function ,a
*       software reset is performed  by setting the soft reset register to 0,it
*       sets the I2C back to idle and the stop it.The start bit and interrupt
*       bits of the I2C control register will also be set to zero.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Set I2C_SOFT_RESET register to 0x0.
*
* RETURN:
*       None.
*
*******************************************************************************/
GT_VOID gtI2cReset
(
    GT_VOID
)
{
    GT_REG_WRITE(I2C_SOFT_RESET,0x0);
}

/*******************************************************************************
* gtI2cGenerateStartBit - Generate start bit to begin a transaction.
*
* DESCRIPTION:
*       This function (mostly used by other functions) initiates a start
*       condition on the bus(to indicate a master transaction request) when the
*       bus is free or a repeated start condition, if the master already drives
*       the bus. After a start bit has been transmited successfuly the next
*       action should be generating the target slave address with a read/write
*       bit. Note that these action are already handeld by the upcoming
*       functions.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Set I2C_START_BIT in the I2C_CONTROL register.
*
* RETURN:
*       I2c data struct(defined in gtI2c.h) with status and error code.
*
*******************************************************************************/
I2C_DATA_STRUCT gtI2cGenerateStartBit
(
    GT_VOID
)
{
    GT_U32          temp;
    GT_U32          timeOut;
    GT_U32          status;
    I2C_DATA_STRUCT gtI2cData;

    /* Generate the start bit */
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp | I2C_START_BIT);

    /* Wait for it to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
            case I2C_BUS_ERROR:
                gtI2cData.errorCode = I2C_GENERAL_ERROR;
                gtI2cData.status = status;
                gtI2cData.data = 0xffffffff;
                gtI2cGenerateStopBit();
                gtI2cClearIntFlag();

                return (gtI2cData);

            case I2C_START_CONDITION_TRA:
                timeOut = I2C_TIMEOUT_VALUE + 1;
                break;

            default:
                break;
        }
    }

    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        gtI2cData.errorCode = I2C_TIME_OUT_ERROR;
        gtI2cData.status = status;
        gtI2cData.data = 0xffffffff;
        gtI2cReset();
      /*gtI2cGenerateStopBit();
        gtI2cClearIntFlag();
      */

        return (gtI2cData);
    }

    gtI2cData.errorCode = I2C_NO_ERROR;
    gtI2cData.status = status;

    return (gtI2cData);
}

/*******************************************************************************
* gtI2cGenerateStopBit - Generate's stop bit to end a transaction.
*
* DESCRIPTION:
*       This function (mostly used by other function) generate stop bit on the
*       bus to indicate end of  transaction by the master.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Set I2C_STOP_BIT in the I2C_CONTROL register.
*
* RETURN:
*       None.
*
*******************************************************************************/
GT_VOID gtI2cGenerateStopBit
(
    GT_VOID
)
{
    GT_U32    temp;

    /* Generate stop bit */
    GT_REG_READ(I2C_CONTROL,&temp);
    /*GT_REG_WRITE(I2C_CONTROL,(temp | I2C_STOP_BIT)&(~I2C_INT_FLAG));*/
    GT_REG_WRITE(I2C_CONTROL,(temp | I2C_STOP_BIT));
}
/*******************************************************************************
* gtI2cSetAckBit - Sets the acknoledge bit on.
*
* DESCRIPTION:
*       This function ( mostly used by other functions ) sets the acknowledge
*       bit to 1. When set to 1,  the I2C master drives the acknowledge bit in
*       response to received read data from the slave. If working in a slave
*       mode , an acknowledge bit will be driven on the bus in response to
*       received address or write data.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Set I2C_ACK in the I2C_CONTROL register.
*
* RETURN:
*       None.
*
*******************************************************************************/
GT_VOID gtI2cSetAckBit
(
    GT_VOID
)
{
    GT_U32    temp;

    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp | I2C_ACK);
}


/*******************************************************************************
* i2cMasterWriteBytes - write data to an I2c device.
*
* DESCRIPTION:
*       This function writes the data within the data struct 'i2cData' to an i2c
*       slave with 7 or 10 bit address. For example, this function can be used
*       for writing from one GT (master) to another GT (slave ,on the same board
*       or on differant board) or writing to any device (with the exception of
*       an EEPROM) that support I2C .The data struct 'i2cData' deliverd is
*       defind in gtI2c.h and contain the data to be written.
*
* INPUT:
*       deviceAddress - 7 or 10 bit address of the target device . If the device
*                       has a 10 bit address , add to deviceAddress the value
*                       I2C_10BIT_ADDR (defind in gtI2c.h).
*       *pI2cData     - struct containing the data to be written.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/

static GT_STATUS i2cMasterWriteBytes
(
    IN  GT_U16   i2cAddress,
    IN  GT_U8   *i2cData,
    IN  GT_U8   i2cDataLen,
    IN  GT_BOOL enStop,
    OUT GT_U32  *i2cStaus
)

{
    GT_U32          timeOut,status;
    I2C_DATA_STRUCT I2cResult;
    GT_U8           i;          /* counter for the number of bytes read*/
    GT_U32          temp;

    /* Unset  stop bit */
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp & (~I2C_STOP_BIT));

    /*gtI2cSetAckBit();*/
    I2cResult = gtI2cGenerateStartBit();
    if(I2cResult.errorCode != I2C_NO_ERROR)
    {
        *i2cStaus = I2cResult.status;
        DBG_INFO(("GT_FAIL1-start bit condition\n"));
        return GT_FAIL;
    }

     /* Transmit the i2cAddress */
    /* 7 Bit i2cAddress */

    GT_REG_WRITE(I2C_DATA,i2cAddress<<1);

    gtI2cClearIntFlag();
    /*while(!gtI2cReadIntFlag());*/
    /* Wait for the i2cAddress to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
            {
            case I2C_BUS_ERROR:
                /* pI2cData->errorCode = I2C_GENERAL_ERROR;*/
                *i2cStaus = status;
                gtI2cGenerateStopBit();
                gtI2cClearIntFlag();
                DBG_INFO(("GT_FAIL2\n"));
                return GT_FAIL;

            case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
                timeOut = I2C_TIMEOUT_VALUE + 1;
                break;
            case I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
                /* pI2cData->errorCode = I2C_GENERAL_ERROR;*/
                *i2cStaus = status;
                gtI2cGenerateStopBit();
                gtI2cClearIntFlag();
                DBG_INFO(("GT_FAIL3-\n"));
                return GT_FAIL;
        /* case I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC:
                timeOut = I2C_TIMEOUT_VALUE + 1;
                break;
            case I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC:
                pI2cData->errorCode = I2C_GENERAL_ERROR;
                *i2cStaus = status;
                gtI2cGenerateStopBit();
                gtI2cClearIntFlag();
                return;
        */
            default:
                break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
     /* pI2cData->errorCode = I2C_NO_DEVICE_WITH_SUCH_ADDR;*/
        *i2cStaus = status;
        gtI2cGenerateStopBit();
        gtI2cClearIntFlag();
            DBG_INFO(("GT_FAIL2"));
            return GT_FAIL;

    }

    while(!gtI2cReadIntFlag());

    GT_REG_WRITE(I2C_DATA,i2cData[0]);
    gtI2cClearIntFlag();

    for ( i= 1 ; i < i2cDataLen + 1 ; i++)
    {
        /* wait for the data to be transmited */
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {

            if (gtI2cReadIntFlag() != 1) continue;
            GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
            switch(status)
            {
                case I2C_BUS_ERROR:
                    /*pI2cData->errorCode = I2C_GENERAL_ERROR;*/
                    *i2cStaus = status;
                    gtI2cGenerateStopBit();
                    gtI2cClearIntFlag();
                    DBG_INFO(("GT_FAIL10- byte%x\n",i));
                    return GT_FAIL;

                case I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC:
                    /*pI2cData->errorCode = I2C_GENERAL_ERROR;*/
                    *i2cStaus = status;
                    gtI2cGenerateStopBit();
                    gtI2cClearIntFlag();
                    DBG_INFO(("GT_FAIL11- byte%x\n",i));
                    return GT_FAIL;
                case I2C_MAS_TRAN_DATA_BYTE_ACK_REC:
                    timeOut = I2C_TIMEOUT_VALUE + 1;
                    DBG_INFO(("i num=%x\n",i-1));

                    /* Generate stop bit */
                    break;
                default:
                    DBG_INFO(("def status=%x\n, byte=%x",status,i));
                    break;
            }
        }

        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            /*pI2cData->errorCode = I2C_TIME_OUT_ERROR;*/
            *i2cStaus = status;
            gtI2cGenerateStopBit();
            gtI2cClearIntFlag();
            DBG_INFO(("GT_FAIL12"));

            return GT_FAIL;

        }

        if (i< i2cDataLen)
        {
                /* Write the data */
            GT_REG_WRITE(I2C_DATA,i2cData[i]);
            gtI2cClearIntFlag();
        }


        /*gtI2cClearIntFlag();*/
    }
    /*pI2cData->errorCode = I2C_NO_ERROR;*/
    if (enStop == GT_TRUE)
    {
        gtI2cGenerateStopBit();
        gtI2cClearIntFlag();
    }

    *i2cStaus = status;
    DBG_INFO(("GT_OK\n"));

    return GT_OK;

}

/*******************************************************************************
* gtI2cWriteRegister
*
* DESCRIPTION:
*       This function utilizes managment write registers to GT devices
*       via I2c interface.
*
* INPUTS:
*       deviceAdress - The I2c device address ( 7 bit only).
*       address      - The device register address word to be read .
*       data         - Data word to write to device.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS gtI2cWriteRegister
(
    IN GT_U16   deviceAddress,
    IN GT_U32   regAddr,
    IN GT_U32   data
)
{
    GT_U8   i2cData[8];
    GT_U32  i2cStatus;


    i2cData[0] = (regAddr & 0xFF000000)>>24;
    i2cData[1] = (regAddr & 0xFF0000)>>16;
    i2cData[2] = (regAddr & 0xFF00)>>8;
    i2cData[3] = regAddr & 0xFF;
    i2cData[4] = (data & 0xFF000000)>>24;
    i2cData[5] = (data & 0xFF0000)>>16;
    i2cData[6] = (data & 0xFF00)>>8;
    i2cData[7] = data & 0xFF;


    return i2cMasterWriteBytes(deviceAddress,i2cData,8,GT_TRUE,&i2cStatus);
}


/*******************************************************************************
* gtI2cReadRegister
*
* DESCRIPTION:
*       This function utilizes managment register read from GT devices
*       via I2c interface.
*
* INPUTS:
*       deviceAdress - The I2c device address ( 7 bit only).
*       address      - The device register address word to be read .
*
* OUTPUTS:
*       data         - Data word read from device, is valid only
*                      if status == GT_OK.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS gtI2cReadRegister
(
    IN GT_U16   deviceAddress,
    IN GT_U32   regAddr,
    OUT GT_U32  *dataWord
)
{
    GT_U32  temp;
    GT_U32  timeOut;
    GT_U32  status;
    GT_U8 i2cData[4];
    GT_STATUS gtStatus;
    GT_U32 i2cStatus;
    GT_U8   i;

    *dataWord = 0;

    i2cData[0] =  (regAddr >> 24) & 0xff;
    i2cData[1] =  (regAddr >> 16) & 0xff;
    i2cData[2] =  (regAddr >> 8) & 0xff;
    i2cData[3] =  (regAddr) & 0xff;
    gtI2cSetAckBit();
    gtStatus = i2cMasterWriteBytes(deviceAddress,i2cData,4,GT_FALSE,&i2cStatus);
    if (gtStatus == GT_FAIL)
        return GT_FAIL;

    /* generate a repeated start bit */
    GT_REG_READ(I2C_CONTROL,&temp);
    GT_REG_WRITE(I2C_CONTROL,temp | I2C_START_BIT);
    while(!gtI2cReadIntFlag());
    gtI2cClearIntFlag();
    /* Wait for it to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            gtI2cGenerateStopBit();
            gtI2cClearIntFlag();
            return GT_FAIL;
        case I2C_REPEATED_START_CONDITION_TRA:
            timeOut = I2C_TIMEOUT_VALUE + 1;
            DBG_INFO(("Repeat start bit\n"));
            break;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        gtI2cGenerateStopBit();
        gtI2cClearIntFlag();
        return GT_FAIL;
    }
    /* Transmit the deviceAddress */
    GT_REG_WRITE(I2C_DATA,(deviceAddress<<1) | I2C_READ);
    while(!gtI2cReadIntFlag());
    gtI2cClearIntFlag();
    /* Wait for the deviceAddress to be transmited */
    for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
    {
        if (gtI2cReadIntFlag() != 1) continue;
        GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
        switch(status)
        {
        case I2C_BUS_ERROR:
            gtI2cGenerateStopBit();
            gtI2cClearIntFlag();
            return GT_FAIL;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC:
            timeOut = I2C_TIMEOUT_VALUE + 1;
            DBG_INFO(("Addr+read tx, Ack rcv\n"));
            break;
        case I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC:
            gtI2cGenerateStopBit();
            gtI2cClearIntFlag();
            return GT_FAIL;
        default:
            break;
        }
    }
    if(timeOut == I2C_TIMEOUT_VALUE)
    {
        gtI2cGenerateStopBit();
        gtI2cClearIntFlag();
        return GT_FAIL;
    }

    for ( i= 0 ;i<4; i++)
    {
        GT_REG_READ(I2C_CONTROL,&temp);
        if (i== 3 )
        {
            GT_REG_WRITE(I2C_CONTROL,temp & (~(I2C_ACK | I2C_INT_FLAG)));
        }
        else
        {
            GT_REG_WRITE(I2C_CONTROL,temp & (~I2C_INT_FLAG));
        }

     /* Wait for the data */
        for(timeOut = 0 ; timeOut < I2C_TIMEOUT_VALUE ; timeOut++)
        {

            if (gtI2cReadIntFlag() != 1) continue;

            GT_REG_READ(I2C_STATUS_BAUDE_RATE,&status);
            switch(status)
            {
            case I2C_BUS_ERROR:
                gtI2cGenerateStopBit();
                gtI2cClearIntFlag();
                return GT_FAIL;
            case I2C_MAS_REC_READ_DATA_ACK_NOT_TRA:
                GT_REG_READ(I2C_DATA,&temp);
                *dataWord |= ((temp & 0xff) << (8*(3-i)));
                DBG_INFO(("Ack not Tr,data rcv=%x\n",temp));
                timeOut = I2C_TIMEOUT_VALUE + 1;
                gtI2cGenerateStopBit();
                gtI2cClearIntFlag();
                break;
            case I2C_MAS_REC_READ_DATA_ACK_TRA:
                GT_REG_READ(I2C_DATA,&temp);
                *dataWord |= ((temp & 0xff) << (8*(3-i)));
                DBG_INFO(("Ack Tr, data rcv=%x\n",temp));
                /*gtI2cClearIntFlag();*/
                timeOut = I2C_TIMEOUT_VALUE + 1;
                break;
            default:
                break;
            }
        }
        if(timeOut == I2C_TIMEOUT_VALUE)
        {
            gtI2cGenerateStopBit();
            gtI2cClearIntFlag();
            return GT_FAIL;
        }
           /* Clear the interrupt flag and signal no-acknowledge */


    }

    return GT_OK;
}



