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
*
*******************************************************************************/


#ifndef __gtHwIfDrvh
#define __gtHwIfDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtExtDrv/os/extDrvOs.h>

/*
 * typedef: enum GT_MGMT_IF_TYPE
 *
 * Description: Managmnet hardware interface of GT device.
 *
 * Enumerations:
 *    GT_MGMT_VIA_PP - GT device is managed through the packet processor
 *                          uplink interface (i.e. packet processor PCI).
 *    GT_MGMT_I2C    - GT device is managed through I2C managment
 *                          interface.
 *
 */
typedef enum
{
    GT_MGMT_VIA_PP = 0,
    GT_MGMT_I2C
}GT_MGMT_IF_TYPE;


/*
 * typedef: struct GT_DEV_ID
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
} GT_DEV_ID;


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
    OUT GT_U32              *data
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

