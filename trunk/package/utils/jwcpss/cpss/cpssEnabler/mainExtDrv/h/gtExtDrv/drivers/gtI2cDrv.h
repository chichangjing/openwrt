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
* gtI2cDrv.h - I2C unit header file
*
* DESCRIPTION:
*       Function to control the I2C unit.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/


#ifndef __gtI2cDrvh
#define __gtI2cDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

 
/********* Include files ******************************************************/
#include <gtExtDrv/os/extDrvOs.h>

 
/********* Typedefs ***********************************************************/

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
*       None
* OUTPUT:
*       None.
*
* RETURN:
*       GT_OK.
*       GT_FAIL.
*
*******************************************************************************/
GT_STATUS gtI2cMgmtMasterInit
(
    GT_VOID
);

/*******************************************************************************
* gtI2cMgmtSlaveInit - Initializes the I2C slave.
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
*       None
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
);

/*******************************************************************************
* gtI2cMgmtReadRegister
*
* DESCRIPTION:
*       Managment read from GT devices register via I2c interface.
*
* INPUTS:
*       i2cBaseAddr     - Base address to which GT device is mapped in the I2c
*                         interface.
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
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS gtI2cMgmtReadRegister
(
    IN  GT_U16  i2cBaseAddr,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);


/*******************************************************************************
* gtI2cMgmtWriteRegister
*
* DESCRIPTION:
*       Managment write to GT devices registers via specified hardware
*       interface.
*
* INPUTS:
*       i2cBaseAddr     - Base address to which GT device is mapped in the I2c
*                         interface.
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
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS gtI2cMgmtWriteRegister
(
    IN  GT_U16  i2cBaseAddr,
    IN  GT_U32  regAddr,
    IN GT_U32   data
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

