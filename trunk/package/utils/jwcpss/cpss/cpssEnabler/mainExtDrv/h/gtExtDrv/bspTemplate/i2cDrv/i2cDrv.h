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
* gt64260I2cDrv.h - I2C unit header file
*
* DESCRIPTION:
*       Function to control the I2C unit.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/


#ifndef __i2cDrvh
#define __i2cDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*#include <prestera/common/gtCommonDefs.h> */

  
/********* Defines ************************************************************/
#define I2C_TIMEOUT_VALUE               0x2000000 /* 1 Second at 500 Mhz */
#define I2C_ENABLE                      BIT_6
#define I2C_INT_ENABLE                  BIT_7
#define I2C_ACK                         BIT_2
#define I2C_INT_FLAG                    BIT_3
#define I2C_STOP_BIT                    BIT_4
#define I2C_START_BIT                   BIT_5
#define I2C_READ                        BIT_0
#define I2C_EEPROM_DELAY                10 /* Mili sec */
#define I2C_10BIT_ADDR                  BIT_31

/* Error codes */
#define I2C_TIME_OUT_ERROR              0xFF
#define I2C_NO_DEVICE_WITH_SUCH_ADDR    0x01
#define I2C_GENERAL_ERROR               0x02
#define I2C_NO_ERROR                    0x03
#define I2C_INT_FLAG_STUCK_AT_0         0x04

/* I2C status codes */
/*  ShogtRtcuts-
    RECEIVED    -> REC
    TRANSMITED  -> TRA
    MASTER      -> MAS
    SLAVE       -> SLA
    ACKNOWLEDGE -> ACK
    ARBITRATION -> ARB
    ADDR        -> ADDR
*/

#define I2C_BUS_ERROR                                                       0X00
#define I2C_START_CONDITION_TRA                                             0X08
#define I2C_REPEATED_START_CONDITION_TRA                                    0X10
#define I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC                                 0X18
#define I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC                             0X20
#define I2C_MAS_TRAN_DATA_BYTE_ACK_REC                                      0X28
#define I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC                                  0X30
#define I2C_MAS_LOST_ARB_DURING_ADDR_OR_DATA_TRA                            0X38
#define I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC                                  0X40
#define I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC                              0X48
#define I2C_MAS_REC_READ_DATA_ACK_TRA                                       0X50
#define I2C_MAS_REC_READ_DATA_ACK_NOT_TRA                                   0X58
#define I2C_SLA_REC_ADDR_PLUS_WRITE_BIT_ACK_TRA                             0X60
#define I2C_MAS_LOST_ARB_DURING_ADDR_TRA_ADDR_IS_TARGETED_TO_SLA_ACK_TRA_W  0X68
#define I2C_GENERAL_CALL_REC_ACK_TRA                                        0X70
#define I2C_MAS_LOST_ARB_DURING_ADDR_TRA_GENERAL_CALL_ADDR_REC_ACK_TRA      0X78
#define I2C_SLA_REC_WRITE_DATA_AFTER_REC_SLA_ADDR_ACK_TRAN                  0X80
#define I2C_SLA_REC_WRITE_DATA_AFTER_REC_SLA_ADDR_ACK_NOT_TRAN              0X88
#define I2C_SLA_REC_WRITE_DATA_AFTER_REC_GENERAL_CALL_ACK_TRAN              0X90
#define I2C_SLA_REC_WRITE_DATA_AFTER_REC_GENERAL_CALL_ACK_NOT_TRAN          0X98
#define I2C_SLA_REC_STOP_OR_REPEATED_START_CONDITION                        0XA0
#define I2C_SLA_REC_ADDR_PLUS_READ_BIT_ACK_TRA                              0XA8
#define I2C_MAS_LOST_ARB_DURING_ADDR_TRA_ADDR_IS_TARGETED_TO_SLA_ACK_TRA_R  0XB0
#define I2C_SLA_TRA_READ_DATA_ACK_REC                                       0XB8
#define I2C_SLA_TRA_READ_DATA_ACK_NOT_REC                                   0XC0
#define I2C_SLA_TRA_LAST_READ_DATA_ACK_REC                                  0XC8
#define I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC                          0XD0
#define I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC                      0XD8
#define I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_REC                           0XE0
#define I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC                       0XE8
#define I2C_NO_RELEVANT_STATUS_INTERRUPT_FLAG_IS_KEPT_0                     0XF8


 
/********* Typedefs ***********************************************************/

/* typedefs */
typedef enum
{
    I2C_FREQ_100KHZ = 100000,
    I2C_FREQ_400KHZ = 400000
} I2C_FREQ;


typedef enum
{
    I2C_GENERAL_CALL_DISABLE,
    I2C_GENERAL_CALL_ENABLE

} I2C_GENERAL_CALL_STATUS;


typedef struct
{
    unsigned int data; /* For read transaction, data is valid only if errorCode
                          = I2C_NO_ERROR, For write transaction, it is a
                          guarantee for a successful operation */
    unsigned int errorCode;
    unsigned int status; /*I2C's status register when updating the error code*/
} I2C_DATA_STRUCT;

     
/********* Public functions ***************************************************/
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
);

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
);

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
);

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
);









#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __i2cDrvh */

