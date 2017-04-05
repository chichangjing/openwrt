/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDriverPpHw.h
*
* DESCRIPTION:
*       CPSS PP Driver Hardware Access API
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __cpssDriverPpHwh
#define __cpssDriverPpHwh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


/*******************************************************************************
*   Note: about the 'portGroupId' parameter , in the next APIs.
*   portGroupId - The port group Id. relevant only to 'multi-port-groups' devices.
*       when value is CPSS_PORT_GROUP_UNAWARE_MODE_CNS :
*       1. For write APIs CPSS_PORT_GROUP_UNAWARE_MODE_CNS, meaning function should write
*          all active port groups , to the specified table entry
*          (same as there was no portGroupId parameter)
*       2. For read APIs CPSS_PORT_GROUP_UNAWARE_MODE_CNS, meaning function should read
*          register only from 1st active port group , from the specified table entry
*          (same as there was no portGroupId parameter).
*******************************************************************************/


/*
 * typedef: enum  CPSS_DRV_HW_TRACE_TYPE_ENT
 *
 * Description: HW Trace type
 *
 * Fields:
 *      CPSS_DRV_HW_TRACE_TYPE_READ_E   - HW access is read.
 *      CPSS_DRV_HW_TRACE_TYPE_WRITE_E  - HW access is write.
 *      CPSS_DRV_HW_TRACE_TYPE_BOTH_E   - Both HW access read and write.
 *      CPSS_DRV_HW_TRACE_TYPE_WRITE_DELAY_E - HW access is write and delay.
 *      CPSS_DRV_HW_TRACE_TYPE_ALL_E - HW access is write, read and delay.
 */
typedef enum
{
    CPSS_DRV_HW_TRACE_TYPE_READ_E,
    CPSS_DRV_HW_TRACE_TYPE_WRITE_E,
    CPSS_DRV_HW_TRACE_TYPE_BOTH_E,
    CPSS_DRV_HW_TRACE_TYPE_WRITE_DELAY_E,
    CPSS_DRV_HW_TRACE_TYPE_ALL_E

}CPSS_DRV_HW_TRACE_TYPE_ENT;


/*******************************************************************************
* cpssDrvPpHwRegisterRead
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       devNum  - The PP to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
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
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegisterRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
);

/*******************************************************************************
* cpssDrvPpHwRegisterWrite
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to write to.
*       data    - The value to be written.
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
GT_STATUS cpssDrvPpHwRegisterWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
);

/*******************************************************************************
* cpssDrvPpHwRegFieldGet
*
* DESCRIPTION:
*       Read a selected register field.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
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
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegFieldGet
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
);

/*******************************************************************************
* cpssDrvPpHwRegFieldSet
*
* DESCRIPTION:
*       Write value to selected register field.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to write to.
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
*       this function actually read the register modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegFieldSet
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData

);

/*******************************************************************************
* cpssDrvPpHwRegBitMaskRead
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - Register address to read from.
*       mask    - Mask for selecting the read bits.
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
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegBitMaskRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
);

/*******************************************************************************
* cpssDrvPpHwRegBitMaskWrite
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - Register address to write to.
*       mask    - Mask for selecting the written bits.
*       value   - Data to be written to register.
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
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegBitMaskWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    IN GT_U32   value
);

/*******************************************************************************
* cpssDrvPpHwRamRead
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr    - Address offset to read from.
*       length  - Number of Words (4 byte) to read.
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
*******************************************************************************/
GT_STATUS cpssDrvPpHwRamRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data
);

/*******************************************************************************
* cpssDrvPpHwRamWrite
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       data    - An array containing the data to be written.
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
GT_STATUS cpssDrvPpHwRamWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
);

/*******************************************************************************
* cpssDrvPpHwVectorRead
*
* DESCRIPTION:
*       Read from PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addrArr - Address array to read from.
*       arrLen  - The size of addrArr/dataArr.
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
GT_STATUS cpssDrvPpHwVectorRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen
);

/*******************************************************************************
* cpssDrvPpHwVectorWrite
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addrArr - Address offset to write to.
*       dataArr - An array containing the data to be written.
*       arrLen  - The size of addrArr/dataArr.
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
GT_STATUS cpssDrvPpHwVectorWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
);

/*******************************************************************************
* cpssDrvPpHwRamInReverseWrite
*
* DESCRIPTION:
*       Writes to PP's RAM in reverse.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       data    - An array containing the data to be written.
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
GT_STATUS cpssDrvPpHwRamInReverseWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
);

/*******************************************************************************
* cpssDrvPpHwIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       devNum  - The PP to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
*                 Note: regAddr should be < 0x1000000
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
*******************************************************************************/
GT_INLINE GT_STATUS cpssDrvPpHwIsrRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   *dataPtr
);

/*******************************************************************************
* cpssDrvPpHwIsrWrite
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
*
* INPUTS:
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to write to.
*       data    - The value to be written.
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
GT_STATUS cpssDrvPpHwIsrWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
);

/*******************************************************************************
* cpssDrvPpHwInternalPciRegRead
*
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
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
GT_STATUS cpssDrvPpHwInternalPciRegRead
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);

/*******************************************************************************
* cpssDrvPpHwInternalPciRegWrite
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
*       data    - Data to be written.
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
GT_STATUS cpssDrvPpHwInternalPciRegWrite
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

/*******************************************************************************
* cpssDrvHwPpResetAndInitControllerReadReg
*
* DESCRIPTION:
*       Read a register value from the Reset and Init Controller.
*
* INPUTS:
*       devNum  - The PP to read from.
*       regAddr - The register's address to read from.
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
*******************************************************************************/
GT_STATUS cpssDrvHwPpResetAndInitControllerReadReg
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);

/*******************************************************************************
* cpssDrvHwPpResetAndInitControllerWriteReg
*
* DESCRIPTION:
*       Write to the Reset and Init Controller given register.
*
* INPUTS:
*       devNum  - The PP to write to.
*       regAddr - The register's address to write to.
*       data    - The value to be written.
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
GT_STATUS cpssDrvHwPpResetAndInitControllerWriteReg
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

/*******************************************************************************
* cpssDrvHwPpResetAndInitControllerGetRegField
*
* DESCRIPTION:
*       Read a selected register field from the Reset and Init Controller.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       regAddr - The register's address to read from.
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
*******************************************************************************/
GT_STATUS cpssDrvHwPpResetAndInitControllerGetRegField
 (
    IN  GT_U8   devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
);

/*******************************************************************************
* cpssDrvHwPpResetAndInitControllerSetRegField
*
* DESCRIPTION:
*       Write value to selected register field of the Reset and Init Controller.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to write to.
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
*       this function actually read the register modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
GT_STATUS cpssDrvHwPpResetAndInitControllerSetRegField
 (
    IN  GT_U8   devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
);

/*******************************************************************************
* cpssDrvPpHwTraceEnable
*
* DESCRIPTION:
*       This function enables/disables to trace hw access. 
*
* INPUTS:
*       devNum      - The Pp's device number.
*       traceType   - trace access type.
*       enable      - GT_TRUE: enable hw access info tracing
*                     GT_FALSE: disable hw access info tracing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwTraceEnable
(
    IN GT_U8                        devNum,
    IN CPSS_DRV_HW_TRACE_TYPE_ENT   traceType,
    IN GT_BOOL                      enable
);


/*
 * typedef: enum CPSS_DRV_HW_ACCESS_STAGE_ENT
 *
 * Description: Enumeration of HW access callback function stages 
 *
 * Enumerations:
 *    CPSS_DRV_HW_ACCESS_STAGE_PRE_E - first position, actual logic takes place. 
 *    CPSS_DRV_HW_ACCESS_STAGE_POST_E - second position - after logic takes place 
 */
typedef enum {
    CPSS_DRV_HW_ACCESS_STAGE_PRE_E,
    CPSS_DRV_HW_ACCESS_STAGE_POST_E
}CPSS_DRV_HW_ACCESS_STAGE_ENT;


/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_READ_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines register read callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       regAddr      - The register's address to read from.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       data - Includes the register value.
 *
 * RETURNS:
 *       GT_OK       - callback success to get the data.
 *       GT_FAIL     - callback failed.
 *       GT_ABORTED  - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_READ_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    OUT GT_U32  *data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_WRITE_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines register write callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       regAddr      - The register's address to read from.
 *       data         - The value to be written.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK       - callback success to get the data.
 *       GT_FAIL     - callback failed.
 *       GT_ABORTED  - operation done and aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_WRITE_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    IN  GT_U32   data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);


/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_FIELD_READ_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines register field read callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       regAddr      - The register's address to read from.
 *       fieldOffset  - The start bit number in the register.
 *       fieldLength  - The number of bits to be read.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       data - Includes the register value.
 *
 * RETURNS:
 *       GT_OK        - callback success to get the data.
 *       GT_FAIL      - callback failed.
 *       GT_ABORTED   - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_FIELD_READ_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_FIELD_WRITE_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines register field write callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       regAddr          - The register's address to read from.
 *       fieldOffset  - The start bit number in the register.
 *       fieldLength  - The number of bits to be read.
 *       data         - The value to be written.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK        - callback success to get the data.
 *       GT_FAIL      - callback failed.
 *       GT_ABORTED   - operation done and aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_FIELD_WRITE_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    IN GT_U32    fieldOffset,
    IN GT_U32    fieldLength,
    IN  GT_U32   data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);


/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_BIT_MASK_READ_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines register bit-mask read callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       regAddr      - The register's address to read from.
 *       mask         - Mask for selecting the read bits.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       data - Includes the register value.
 *
 * RETURNS:
 *       GT_OK         - callback success to get the data.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_BIT_MASK_READ_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    IN GT_U32    mask,
    OUT GT_U32  *data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_BIT_MASK_WRITE_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines register bit-mask write callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       regAddr      - The register's address to read from.
 *       mask         - Mask for selecting the read bits.
 *       data         - The value to be written.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK         - callback success to get the data.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation done and aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_BIT_MASK_WRITE_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    IN  GT_U32   mask,
    IN  GT_U32   data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_RAM_READ_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines ram read callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       addr         - Address offset to read from.
 *       length       - Number of Words (4 byte) to read.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       data - Includes the register value.
 *
 * RETURNS:
 *       GT_OK         - callback success.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_RAM_READ_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_RAM_WRITE_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines ram write/write in reverse, callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       addr         - Address offset to read from.
 *       length       - Number of Words (4 byte) to read.
 *       data         - The value to be written.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK         - callback success.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_RAM_WRITE_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN  GT_U32  *data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_VECTOR_READ_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines vector of registers read callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       addrArr      - Address offset to write to.
 *       arrLen       - The size of addrArr/dataArr.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       dataArr - An array containing the read data.
 *
 * RETURNS:
 *       GT_OK         - callback success.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_VECTOR_READ_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_VECTOR_WRITE_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines vector of registers write callback function for Hw access.
 *
 * INPUTS:
 *       devNum      -  The PP to read from.
 *       portGroupId - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       addrArr     - Address offset to write to.
 *       dataArr     - An array containing the data to be written.
 *       arrLen      - The size of addrArr/dataArr.
 *       stage       - stage for callback run.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK         - callback success.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DRV_HW_ACCESS_PORT_GROUP_VECTOR_WRITE_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
);


/*
 *  typedef: struct CPSS_DRV_HW_ACCESS_OBJ_STC
 *
 * Description: the structure holds the regiters/memory access callback routines.
 *
 *  COMMENTS: 
 *  ----------------------------------------------------------------------------------------
 *             CPSS Function                                   Hw access Callback
 *------------------------------------------------------------------------------------------
 *   1    prvCpssDrvHwPpPortGroupReadRegister             hwAccessRegisterReadFunc;       
 *   2    prvCpssDrvHwPpPortGroupWriteRegister             hwAccessRegisterWriteFunc;      
 *   3    prvCpssDrvHwPpPortGroupGetRegField               hwAccessRegisterFieldReadFunc;  
 *   4    prvCpssDrvHwPpPortGroupSetRegField               hwAccessRegisterFieldWriteFunc; 
 *   5    prvCpssDrvHwPpPortGroupReadRegBitMask        hwAccessRegisterBitMaskReadFunc;
 *   6    prvCpssDrvHwPpPortGroupWriteRegBitMask        hwAccessRegisterBitMaskWriteFunc
 *   7    prvCpssDrvHwPpPortGroupReadRam                  hwAccessRamReadFunc;            
 *   8    prvCpssDrvHwPpPortGroupWriteRam                  hwAccessRamWriteFunc;           
 *   9    prvCpssDrvHwPpPortGroupWriteRamInReverse   hwAccessRamWriteInReverseFunc;  
 *  10   prvCpssDrvHwPpPortGroupReadVec                   hwAccessVectorReadFunc;         
 *  11   prvCpssDrvHwPpPortGroupWriteVec                   hwAccessVectorWriteFunc;        
 *  12   prvCpssDrvHwPpPortGroupIsrRead                    hwAccessRegisterIsrReadFunc;    
 *  13   prvCpssDrvHwPpPortGroupIsrWrite                    hwAccessRegisterIsrWriteFunc;   
 *---------------------------------------------------------------------------------------------
 */
typedef struct{
    CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_READ_FUNC           hwAccessRegisterReadFunc;         /* 1 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_WRITE_FUNC          hwAccessRegisterWriteFunc;        /* 2 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_FIELD_READ_FUNC     hwAccessRegisterFieldReadFunc;    /* 3 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_FIELD_WRITE_FUNC    hwAccessRegisterFieldWriteFunc;   /* 4 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_BIT_MASK_READ_FUNC  hwAccessRegisterBitMaskReadFunc;  /* 5 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_BIT_MASK_WRITE_FUNC hwAccessRegisterBitMaskWriteFunc; /* 6 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_RAM_READ_FUNC                hwAccessRamReadFunc;              /* 7 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_RAM_WRITE_FUNC               hwAccessRamWriteFunc;             /* 8 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_RAM_WRITE_FUNC               hwAccessRamWriteInReverseFunc;    /* 9 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_VECTOR_READ_FUNC             hwAccessVectorReadFunc;           /* 10 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_VECTOR_WRITE_FUNC            hwAccessVectorWriteFunc;          /* 11 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_READ_FUNC           hwAccessRegisterIsrReadFunc;      /* 12 */
    CPSS_DRV_HW_ACCESS_PORT_GROUP_REGISTER_WRITE_FUNC          hwAccessRegisterIsrWriteFunc;     /* 13 */
} CPSS_DRV_HW_ACCESS_OBJ_STC;


/*******************************************************************************
* cpssDrvHwAccessObjectBind
*
* DESCRIPTION:
*         The function binds/unbinds a callback routines for HW access.
*
* INPUTS:
*    hwAccessObjPtr - HW access object pointer.
*    bind           -  GT_TRUE - bind callback routines.
*                      GT_FALSE - un bind callback routines.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDrvHwAccessObjectBind
(
    IN CPSS_DRV_HW_ACCESS_OBJ_STC *hwAccessObjPtr,
    IN GT_BOOL                     bind
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDriverPpHwh */

