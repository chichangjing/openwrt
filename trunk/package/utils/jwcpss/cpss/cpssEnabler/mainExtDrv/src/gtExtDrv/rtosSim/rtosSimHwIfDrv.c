/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
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
#include <asicSimulation/SCIB/scib.h>


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



