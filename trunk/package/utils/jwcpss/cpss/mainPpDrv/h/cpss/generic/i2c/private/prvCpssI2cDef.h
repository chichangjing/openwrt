/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssExMxI2cDef.h
*
* DESCRIPTION:
*       Includes private definitions for I2C module.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssI2cDefh
#define __prvCpssI2cDefh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


#define ABS_VAL(x)  ((GT_32)(x) > 0) ? (x) : (-(GT_32)(x))

#define PRV_CPSS_I2C_TIMEOUT_VALUE_CNS      (0x2000000) /* 1 Second at 500 Mhz */
#define PRV_CPSS_I2C_ENABLE_BIT_CNS         (0x40)
#define PRV_CPSS_I2C_INT_ENABLE_BIT_CNS     (0x80)
#define PRV_CPSS_I2C_ACK_BIT_CNS            (0x4)
#define PRV_CPSS_I2C_INT_FLAG_BIT_CNS       (0x8)
#define PRV_CPSS_I2C_STOP_BIT_CNS           (0x10)
#define PRV_CPSS_I2C_START_BIT_CNS          (0x20)
#define PRV_CPSS_I2C_READ_CNS               (0x1)
#define PRV_CPSS_I2C_EEPROM_DELAY_CNS       (10)  /* Mili sec */
#define PRV_CPSS_10BIT_ADDR_CNS             (0x80000000)
/* Error codes */
#define PRV_CPSS_I2C_TIME_OUT_ERROR_CNS              (0xFF)
#define PRV_CPSS_I2C_NO_DEVICE_WITH_SUCH_ADDR_CNS    (0x01)
#define PRV_CPSS_I2C_GENERAL_ERROR_CNS               (0x02)
#define PRV_CPSS_I2C_NO_ERROR_CNS                    (0x03)
#define PRV_CPSS_I2C_INT_FLAG_STUCK_AT_0_CNS         (0x04)

/* I2C status codes */
/*  Shortcuts-
    RECEIVED    -> REC
    TRANSMITED  -> TRA
    MASTER      -> MAS
    SLAVE       -> SLA
    ACKNOWLEDGE -> ACK
    ARBITRATION -> ARB
    ADDRESS     -> ADDR
*/

#define PRV_CPSS_I2C_BUS_ERROR_CNS                                                       (0X00)
#define PRV_CPSS_I2C_START_CONDITION_TRA_CNS                                             (0X08)
#define PRV_CPSS_I2C_REPEATED_START_CONDITION_TRA_CNS                                    (0X10)
#define PRV_CPSS_I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC_CNS                                 (0X18)
#define PRV_CPSS_I2C_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC_CNS                             (0X20)
#define PRV_CPSS_I2C_MAS_TRAN_DATA_BYTE_ACK_REC_CNS                                      (0X28)
#define PRV_CPSS_I2C_MAS_TRAN_DATA_BYTE_ACK_NOT_REC_CNS                                  (0X30)
#define PRV_CPSS_I2C_MAS_LOST_ARB_DURING_ADDR_OR_DATA_TRA_CNS                            (0X38)
#define PRV_CPSS_I2C_ADDR_PLUS_READ_BIT_TRA_ACK_REC_CNS                                  (0X40)
#define PRV_CPSS_I2C_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC_CNS                              (0X48)
#define PRV_CPSS_I2C_MAS_REC_READ_DATA_ACK_TRA_CNS                                       (0X50)
#define PRV_CPSS_I2C_MAS_REC_READ_DATA_ACK_NOT_TRA_CNS                                   (0X58)
#define PRV_CPSS_I2C_SLA_REC_ADDR_PLUS_WRITE_BIT_ACK_TRA_CNS                             (0X60)
#define PRV_CPSS_I2C_MAS_LOST_ARB_ON_ADDR_TRA_ADDR_IS_TRG_TO_SLA_ACK_TRA_W_CNS           (0X68)
#define PRV_CPSS_I2C_GENERAL_CALL_REC_ACK_TRA_CNS                                        (0X70)
#define PRV_CPSS_I2C_MAS_LOST_ARB_DURING_ADDR_TRA_GNE_CALL_ADDR_REC_ACK_TRA_CNS          (0X78)
#define PRV_CPSS_I2C_SLA_REC_WRITE_DATA_AFTER_REC_SLA_ADDR_ACK_TRAN_CNS                  (0X80)
#define PRV_CPSS_I2C_SLA_REC_WRITE_DATA_AFTER_REC_SLA_ADDR_ACK_NOT_TRAN_CNS              (0X88)
#define PRV_CPSS_I2C_SLA_REC_WRITE_DATA_AFTER_REC_GENERAL_CALL_ACK_TRAN_CNS              (0X90)
#define PRV_CPSS_I2C_SLA_REC_WRITE_DATA_AFTER_REC_GENERAL_CALL_ACK_NOT_TRAN_CNS          (0X98)
#define PRV_CPSS_I2C_SLA_REC_STOP_OR_REPEATED_START_CONDITION_CNS                        (0XA0)
#define PRV_CPSS_I2C_SLA_REC_ADDR_PLUS_READ_BIT_ACK_TRA_CNS                              (0XA8)
/*#define PRV_CPSS_I2C_MAS_LOST_ARB_ON_ADDR_TRA_ADDR_IS_TRG_TO_SLA_ACK_TRA_R_CNS           (0XB0)*/
#define PRV_CPSS_I2C_SLA_TRA_READ_DATA_ACK_REC_CNS                                       (0XB8)
#define PRV_CPSS_I2C_SLA_TRA_READ_DATA_ACK_NOT_REC_CNS                                   (0XC0)
#define PRV_CPSS_I2C_SLA_TRA_LAST_READ_DATA_ACK_REC_CNS                                  (0XC8)
#define PRV_CPSS_I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC_CNS                          (0XD0)
#define PRV_CPSS_I2C_SECOND_ADDR_PLUS_WRITE_BIT_TRA_ACK_NOT_REC_CNS                      (0XD8)
#define PRV_CPSS_I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_REC_CNS                           (0XE0)
#define PRV_CPSS_I2C_SECOND_ADDR_PLUS_READ_BIT_TRA_ACK_NOT_REC_CNS                       (0XE8)
#define PRV_CPSS_I2C_NO_RELEVANT_STATUS_INTERRUPT_FLAG_IS_KEPT_0_CNS                     (0XF8)



/* typedefs */
typedef enum _i2cfreq 
{   
    _100KHZ_E = 100000,
    _400KHZ_E = 400000
} PRV_CPSS_I2C_FREQ_ENT;

typedef enum _i2cAddrRange
{
    _256BYTES_E,
    MORE_THAN_256BYTES_E
} PRV_CPSS_I2C_ADDR_RANGE_ENT;

typedef enum _i2cGenaralCallStatus
{
    I2C_GENERAL_CALL_DISABLE_E,
    I2C_GENERAL_CALL_ENABLE_E
}PRV_CPSS_EXMX_I2C_GENERAL_CALL_STATUS_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssI2cDefh */

