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
#include <gtExtDrv/drivers/gtI2cDrv.h>

#ifdef PRESTERA_DEBUG
#define I2C_DEBUG
#endif

#ifdef I2C_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

           
/********* Internal functions *************************************************/



/********* Variables **********************************************************/

/*******************************************************************************
* gtI2cMgmtSlaveInit - Initializes the I2C slave .
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
*       None.
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
GT_VOID gtI2cMgmtSlaveInit
(
    GT_VOID
)
{

}

/*******************************************************************************
* gtI2cMgmtMasterInit - Initializes the I2C mechanism integrated in the GT.
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
GT_STATUS gtI2cMgmtMasterInit
(
    GT_VOID
)
{

    return (GT_OK);
}


/*******************************************************************************
* gtI2cMgmtWriteRegister
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
GT_STATUS gtI2cMgmtWriteRegister
(
    IN GT_U16   deviceAddress,
    IN GT_U32   regAddr,
    IN GT_U32   data
)
{
    return (GT_OK);
}


/*******************************************************************************
* gtI2cMgmtReadRegister
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
GT_STATUS gtI2cMgmtReadRegister
(
    IN GT_U16   deviceAddress,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
	return (GT_OK);
}



