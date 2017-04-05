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
*******************************************************************************/

/*includes*/
#include <gtExtDrv/os/extDrvOs.h>
#include "i2cDrv.h"

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

    return (0);
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
    return (GT_OK);
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
    I2C_DATA_STRUCT gtI2cData = {0};
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


    return GT_OK;
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

    return GT_OK;
}



