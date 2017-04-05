/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvHwPpPortGroupCntl.h
*
* DESCRIPTION:
*       CPSS Prestera driver structs definitions and functions declarations for
*       hardware access, Per port group (supports'multi port groups' device)
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#ifndef __prvCpssDrvHwPpPortGroupCntlh
#define __prvCpssDrvHwPpPortGroupCntlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/os/gtOs/cpssOsSem.h>
#include <cpss/generic/events/cpssGenEventCtrl.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpssDriver/pp/hardware/cpssDrvBusDriverObj.h>


/*******************************************************************************
*   Note: about the 'portGroupId' parameter , in the next APIs.
*   portGroupId - The port group id. relevant only to 'multi-port-groups' devices.
*       when value is CPSS_PORT_GROUP_UNAWARE_MODE_CNS :
*       1. For write APIs CPSS_PORT_GROUP_UNAWARE_MODE_CNS, meaning function should write
*          all active  port groups, to the specified table entry
*          (same as there was no portGroupId parameter)
*       2. For read APIs CPSS_PORT_GROUP_UNAWARE_MODE_CNS, meaning function should read
*          register only from 1st active port group, from the specified table entry
*          (same as there was no portGroupId parameter).
*******************************************************************************/

/*******************************************************************************
* prvCpssDrvHwPpPortGroupGetDrv
*
* DESCRIPTION:
*       Read a register value from the given PP.
*       in the specific port group in the device
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer to device driver or NULL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CPSS_BUS_DRIVER_OBJ_STC* prvCpssDrvHwPpPortGroupGetDrv
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId
);
 

/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP.
*       in the specific port group in the device
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
);



/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       data        - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
);



/*******************************************************************************
* prvCpssDrvHwPpPortGroupGetRegField
*
* DESCRIPTION:
*       Read a selected register field.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
);



/*******************************************************************************
* prvCpssDrvHwPpPortGroupSetRegField
*
* DESCRIPTION:
*       Write value to selected register field.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
);



/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - Register address to read from.
*       mask        - Mask for selecting the read bits.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
* GalTis:
*       Command - hwPpReadRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
);



/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - Register address to write to.
*       mask        - Mask for selecting the written bits.
*       value       - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
* GalTis:
*       Command - hwPpWriteRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    IN GT_U32   value
);



/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadRam
*
* DESCRIPTION:
*       Read from PP's RAM.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr        - Address offset to read from.
*       length      - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       data    - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* GalTis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadRam
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data
);


/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteRamInReverse
*
* DESCRIPTION:
*       Writes to PP's RAM in reverse.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr        - Address offset to write to.
*       length      - Number of Words (4 byte) to write.
*       data        - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteRamInReverse
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
);

/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM.
*       in the specific port group in the device
*
* INPUTS:
*       devNum     - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr       - Address offset to write to.
*       length     - Number of Words (4 byte) to write.
*       data       - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* GalTis:
*       Table - PPWrite
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteRam
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
);


/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadVec
*
* DESCRIPTION:
*       Read from PP's RAM a vector of addresses.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addrArr     - Address array to read from.
*       arrLen      - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadVec
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen
);

/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteVec
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addrArr     - Address offset to write to.
*       dataArr     - An array containing the data to be written.
*       arrLen      - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteVec
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
);

/*******************************************************************************
* prvCpssDrvHwPpPortGroupIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*                     Note - regAddr should be < 0x1000000
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* GalTis:
*       None.
*
*******************************************************************************/
GT_INLINE GT_STATUS prvCpssDrvHwPpPortGroupIsrRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   *dataPtr
);



/*******************************************************************************
* prvCpssDrvHwPpPortGroupIsrWrite
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       data        - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* GalTis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupIsrWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
);

/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*       in the specific port group in the device
*
* INPUTS:
*       devNum     - The Pp's device numbers.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr    - The register's address to read from.
*
* OUTPUTS:
*       data    - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);


/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The Pp's device numbers.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*       data        - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvHwPpPortGroupCntlh */


