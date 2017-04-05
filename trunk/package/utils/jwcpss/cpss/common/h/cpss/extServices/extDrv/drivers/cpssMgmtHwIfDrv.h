/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssMgmtHwIfDrv.h
*
* DESCRIPTION:
*       Managment hardware interface driver.
*       Enable managment hw access to GT devices via selected hardware
*       interfaces.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __cpssMgmtHwIfDrvh
#define __cpssMgmtHwIfDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*
 * typedef: enum CPSS_EXTDRV_MGMT_IF_TYPE_ENT
 *
 * Description: Managmnet hardware interface of GT device.
 *
 * Enumerations:
 *    CPSS_EXTDRV_MGMT_VIA_PP_E - GT device is managed through the packet processor
 *                                uplink interface (i.e. packet processor PCI).
 *    CPSS_EXTDRV_MGMT_I2C_E    - GT device is managed through I2C managment
 *                                interface.
 *
 */
typedef enum
{
    CPSS_EXTDRV_MGMT_VIA_PP_E = 0,
    CPSS_EXTDRV_MGMT_I2C_E
}CPSS_EXTDRV_MGMT_IF_TYPE_ENT;


/*
 * typedef: struct CPSS_EXTDRV_DEV_ID_UNT
 *
 * Description:         Device id defined by interface for accessing device.
 *                      union depends on type of hardware interface.
 * Fields:
 *
 *      devNum          - Device number of connected packet processor through
 *                        uplink bus, for PCI interface access.
 *      i2cBaseAddr     - Base address to which GT device is mapped in the I2c
 *                        interface.
 * Comments:
 *
 */
typedef union
{
    GT_U8   devNum;
    GT_U16  i2cBaseAddr;
} CPSS_EXTDRV_DEV_ID_UNT;


/*******************************************************************************
* CPSS_EXTDRV_I2C_MGMT_MASTER_INIT_FUNC
*
* DESCRIPTION:
*       This function initialize the I2C mechanism and must be called before
*       any attemp to use the I2C bus ( use this function on both master or
*       slave mode to initialize the I2C mechanism).The function calculates the
*       parameters needed for the I2C's freuency registers, resets the I2C and
*       then enables it.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURN:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Inputs for the function values are:
*       i2cFreq - the desired frequency , values can be either _100KHZ or
*                 _400KHZ.
*       tclk    - The system's Tclock.
*       The function results are:
*       - I2C mechanism is enabled.
*       - gtI2cTclock is set to tclk value.
*       
*       The function returns the actual frequancy calculated and assigned to
*       the I2C baude - rate register (for more details please see the I2C
*       section in the GT datasheet).
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_I2C_MGMT_MASTER_INIT_FUNC)
(
    GT_VOID
);

/*******************************************************************************
* CPSS_EXTDRV_MGMT_READ_REGISTER_FUNC
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
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_MGMT_READ_REGISTER_FUNC)
(
    IN CPSS_EXTDRV_MGMT_IF_TYPE_ENT   mgmtIf,
    IN CPSS_EXTDRV_DEV_ID_UNT         devId,
    IN GT_U32                         regAddr,
    OUT GT_U32                        *data
);

/*******************************************************************************
* CPSS_EXTDRV_MGMT_WRITE_REGISTER_FUNC
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
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_MGMT_WRITE_REGISTER_FUNC)
(
    IN CPSS_EXTDRV_MGMT_IF_TYPE_ENT   mgmtIf,
    IN CPSS_EXTDRV_DEV_ID_UNT         devId,
    IN GT_U32                         regAddr,
    IN GT_U32                         data
);


/*******************************************************************************
* CPSS_EXTDRV_MGMT_ISR_READ_REGISTER_FUNC
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
*       None
*
*******************************************************************************/
typedef GT_32 (*CPSS_EXTDRV_MGMT_ISR_READ_REGISTER_FUNC)
(
    IN CPSS_EXTDRV_MGMT_IF_TYPE_ENT   mgmtIf,
    IN CPSS_EXTDRV_DEV_ID_UNT         devId,
    IN GT_U32                         regAddr,
    OUT GT_U32                        *data
);


/*******************************************************************************
* CPSS_EXTDRV_MGMT_ISR_WRITE_REGISTER_FUNC
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
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_32 (*CPSS_EXTDRV_MGMT_ISR_WRITE_REGISTER_FUNC)
(
    IN CPSS_EXTDRV_MGMT_IF_TYPE_ENT   mgmtIf,
    IN CPSS_EXTDRV_DEV_ID_UNT         devId,
    IN GT_U32                         regAddr,
    IN GT_U32                         data
);

/* CPSS_EXT_DRV_MGMT_HW_IF_STC -
    structure that hold the "external driver interrupts" functions needed be
    bound to cpss.
*/
typedef struct{
    CPSS_EXTDRV_I2C_MGMT_MASTER_INIT_FUNC      extDrvI2cMgmtMasterInitFunc;
    CPSS_EXTDRV_MGMT_READ_REGISTER_FUNC        extDrvMgmtReadRegisterFunc;
    CPSS_EXTDRV_MGMT_WRITE_REGISTER_FUNC       extDrvMgmtWriteRegisterFunc;
    CPSS_EXTDRV_MGMT_ISR_READ_REGISTER_FUNC    extDrvMgmtIsrReadRegisterFunc;
    CPSS_EXTDRV_MGMT_ISR_WRITE_REGISTER_FUNC   extDrvMgmtIsrWriteRegisterFunc;

}CPSS_EXT_DRV_MGMT_HW_IF_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssMgmtHwIfDrvh */


