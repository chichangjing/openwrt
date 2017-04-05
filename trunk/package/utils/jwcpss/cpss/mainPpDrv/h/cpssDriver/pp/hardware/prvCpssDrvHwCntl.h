/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvHwCntl.h
*
* DESCRIPTION:
*       CPSS Prestera driver structs definitions and functions declarations for
*       hardware access.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/

#ifndef __prvCpssDrvHwCntlh
#define __prvCpssDrvHwCntlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/os/gtOs/cpssOsSem.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwPpPortGroupCntl.h>

/* offset of the address completion from the PCI base address*/
#define ADDR_COMP_REG 0

/* offset of the address completion for PEX 8 completion regions */
#define PEX_MBUS_ADDR_COMP_REG_MAC(_index) (0x120 + (4 * _index))

/* address of the device Id and the revision id register address */
#define PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS    0x4c

/*
 * Typedef: struct PRV_CPSS_DRV_RAM_BURST_INFO_STC
 *
 * Description: Holds information regarding the bursts that should be performed
 *              when access the different Pp's Rams.
 *
 * Fields:
 *      ramBase         - Base address of the Ram.
 *      ramBaseMask     - Mask to be used to check if a given address falls into
 *                        this Ram.
 *      burstSize       - Number of words to be written in each burst.
 *      addrResolution  - Resolution of the addresses to be used when addressing
 *                        this Ram (see comment).
 *
 * Comment:
 *       1.  addrResolution: This is the parameter by which the Ram offset is
 *                           multiplied when accessing it by burst, if this
 *                           is different than 1, then a block write / read from
 *                           Ram should be divided into several bursts in size
 *                           'burstSize'.
 *
 */
typedef struct
{
    GT_U32  ramBase;
    GT_U32  ramBaseMask;
    GT_U8   burstSize;
    GT_U8   addrResolution;
}PRV_CPSS_DRV_RAM_BURST_INFO_STC;


/* Maximal size of a memory block to be read / written in   */
/* burst (in 4-bytes words).                                */
#define PRV_CPSS_DRV_MAX_BURST_BLOCK_SIZE_CNS    256
#define PRV_CPSS_DRV_MAX_BURST_EXTRA_ALLIGN_CNS  16

#ifdef ASIC_SIMULATION
/*
 * typedef: struct PRV_SIM_DRV_STRUCT_PP_HW_STC
 *
 * Description:
 *
 * Simulation related data - For performing Read/Write accesses to PP's .
 *
 * Fields:
 *
 *      devId               - simulation device ID
 *      addrCompletEnable   - enable/disable address completion
 *                            for specific device.
 *
 * Comments:
 *
 */
typedef struct
{
    GT_U8               devId;
    GT_BOOL             addrCompletEnable;
}PRV_SIM_DRV_STRUCT_PP_HW_STC;

#endif /* ASIC_SIMULATION */

/*
 * typedef: struct PRV_CPSS_DRV_ADDR_COMPL_REGION_INFO_STC
 *
 * Description:
 *
 * Holds per device information, required for HW access.
 *
 * Fields:
 *    value - 6 MSB address bits to be used
 *    regAddr - register address(out of 3 existing registers in HW.
 *    Only second register holding region 5, and third one, holding regions 6
 *    and 7 are relevant
 * Comments:
 *
 */
typedef struct
{
    GT_U32  value;
    GT_U32  regAddr;
} PRV_CPSS_DRV_ADDR_COMPL_REGION_INFO_STC;

/*
 * typedef: struct PRV_CPSS_DRV_PEX_ADDR_COMPL_LRU_STC
 *
 * Description:
 *
 * Hardware access related data - For performing Read/Write accesses to PP's .
 *
 * Fields:
 *
 *      curPermArrRowIndex     - Permutations array's current row index
 *      numOfDynamicRegions - Number of dynamic regions out of 8 available
 *      regionInfo - shadow, holding information regarding all 8 regions+1 extra
 *      permArrayPtr - array to hold all available permutations
 * Comments:
 *
 */
typedef struct
{
    GT_U32              curPermArrRowIndex;
    GT_U32              numOfDynamicRegions;
    PRV_CPSS_DRV_ADDR_COMPL_REGION_INFO_STC regionInfo[9];
    GT_U32              *permArrayPtr; /* one double array */
} PRV_CPSS_DRV_PEX_ADDR_COMPL_LRU_STC;

/*
 * typedef: struct PRV_CPSS_DRV_STRUCT_PP_PORT_GROUP_HW_STC
 *
 * Description:
 *
 * Hardware access related data - For performing Read/Write accesses to PP's .
 *                                This info is 'per port-group'
 *
 * Fields:
 *
 *      baseAddr            - Base address to which PP is mapped on PCI.
 *                            in case of SMI it would be smiIdSel.
 *      hwAddr              - HW (bus) address.
 *                            Used for CPSS_CHANNEL_PEX_KERNEL_E only
 *      addrCompletShadow[8]- Shadow for the address completion register
 *                            index 0 if fixed 0, index 1 - used by ISR
 *                            functions
 *                            For Bobcat2, Caelum, Bobcat3 used 8 regions.
 *                            Region 0 also must never be configured.
 *                            Two ranges of regions used - one for ISR
 *                            and the second for all other CPSS API.
 *      isrAddrCompletionRegionsBmp  - Bitmap of configurable address completion regions
 *                            used by interrupt hanling routine.
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      isrAddrRegionUsed   - last address completion region used for ISR (work variable))
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      isrAddrRegionMin   -  minimal address completion region used for ISR (work variable))
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      isrAddrRegionMax   -  maximal address completion region used for ISR (work variable))
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      appAddrCompletionRegionsBmp  - Bitmap of configurable address completion regions
 *                            used by CPSS API.
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      appAddrRegionUsed   - last address completion region used for CPSS API (work variable))
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      appAddrRegionMin   -  minimal address completion region used for CPSS (work variable))
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      appAddrRegionMax   -  maximal address completion region used for CPSS (work variable))
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      hwComplSem          - binary semaphore to enable mutual exclusion use of
 *                            address Completion register use.
 *      compIdx             - last Completion Index used (index 0,1 are
 *                            reserved)
 *      internalPciBase     - Base address to which the internal PCI registers
 *                            are mapped to.
 *      ramBurstInfo        - Holds the ram burst parameters for this device.
 *      ramBurstInfoLen     - Number of valid entries in ramBurstInfo[].
 *      lastWriteAddr       - Address of the last 2 addresses that a write
 *                            operation was performed on.
 *
 *      doReadAfterWrite    - WA for Errata of PPs that require to SYNC their
 *                            write actions before doing a read action.
 *                            relevant only for PCI
 *      dummyReadAfterWriteRegAddr - address of"dummy" register to be used for
 *                            WA of PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E
 *      simInfo.devId               - simulation device ID
 *      simInfo.addrCompletEnable   - enable/disable address completion
 *                            for specific device.
 *      pexInfoPtr          - PEX related info, relevant for PEX dependent
 *                            devices
 * Comments:
 *
 */
typedef struct
{
    GT_UINTPTR          baseAddr;
    CPSS_HW_ADDR_STC    hwAddr;
    GT_U32              addrCompletShadow[8];
    GT_U32              isrAddrCompletionRegionsBmp;
    GT_U32              isrAddrRegionUsed;
    GT_U32              isrAddrRegionMin;
    GT_U32              isrAddrRegionMax;
    GT_U32              appAddrCompletionRegionsBmp;
    GT_U32              appAddrRegionUsed;
    GT_U32              appAddrRegionMin;
    GT_U32              appAddrRegionMax;
    CPSS_OS_MUTEX       hwComplSem;
    GT_U8               compIdx;
    GT_UINTPTR          internalPciBase;
    PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfo;
    GT_U8               ramBurstInfoLen;
    GT_U32              lastWriteAddr[2];

    GT_BOOL             doReadAfterWrite;
    GT_U32              dummyReadAfterWriteRegAddr;

#ifdef ASIC_SIMULATION
    PRV_SIM_DRV_STRUCT_PP_HW_STC simInfo;
#endif /* ASIC_SIMULATION */

} PRV_CPSS_DRV_STRUCT_PP_PORT_GROUP_HW_STC;



/*******************************************************************************
* prvCpssDrvHwCntlInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*
* INPUTS:
*       devNum          - The PP's device number to init the structure for.
*       portGroupId     - The port group id.
*                          relevant only to 'multi-port-group' devices.
*                          supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       baseAddr        - The PP base address on the host interface bus.
*       internalPciBase - Base address to which the internal pci registers
*                         are mapped to.
*       isDiag          - Is this initialization is for diagnostics purposes
*                         (GT_TRUE), or is it a final initialization of the Hw
*                         Cntl unit (GT_FALSE)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       1.  In case isDiag == GT_TRUE, no semaphores are initialized.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwCntlInit
(
    IN GT_U8        devNum,
    IN GT_U32       portGroupId,
    IN GT_UINTPTR   baseAddr,
    IN GT_UINTPTR   internalPciBase,
    IN GT_BOOL      isDiag
);

/*******************************************************************************
* prvCpssDrvHwPpReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP.
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
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
);



/*******************************************************************************
* prvCpssDrvHwPpWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register.
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
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRegister
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 value
);



/*******************************************************************************
* prvCpssDrvHwPpGetRegField
*
* DESCRIPTION:
*       Read a selected register field.
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
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
);



/*******************************************************************************
* prvCpssDrvHwPpSetRegField
*
* DESCRIPTION:
*       Write value to selected register field.
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
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpSetRegField
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData
);



/*******************************************************************************
* prvCpssDrvHwPpReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to read from.
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
* GalTis:
*       Command - hwPpReadRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
);



/*******************************************************************************
* prvCpssDrvHwPpWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to write to.
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
* GalTis:
*       Command - hwPpWriteRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRegBitMask
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 value
);



/*******************************************************************************
* prvCpssDrvHwPpReadRam
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to read from.
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
* GalTis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadRam
(
    IN GT_U8 devNum,
    IN GT_U32 addr,
    IN GT_U32 length,
    OUT GT_U32  *data
);


/*******************************************************************************
* prvCpssDrvHwPpWriteRamInReverse
*
* DESCRIPTION:
*       Writes to PP's RAM in reverse.
*
* INPUTS:
*       devNum  - The PP device number to write to.
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
GT_STATUS prvCpssDrvHwPpWriteRamInReverse
(
    IN GT_U8 devNum,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
);

/*******************************************************************************
* prvCpssDrvHwPpWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to write to.
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
* GalTis:
*       Table - PPWrite
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRam
(
    IN GT_U8 devNum,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
);


/*******************************************************************************
* prvCpssDrvHwPpReadVec
*
* DESCRIPTION:
*       Read from PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to read from.
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
GT_STATUS prvCpssDrvHwPpReadVec
(
    IN GT_U8    devNum,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen
);

/*******************************************************************************
* prvCpssDrvHwPpWriteVec
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to write to.
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
GT_STATUS prvCpssDrvHwPpWriteVec
(
    IN GT_U8    devNum,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
);

/*******************************************************************************
* prvCpssDrvHwPpIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       devNum  - The PP to read from.
*       regAddr - The register's address to read from.
*                 Note - regAddr should be < 0x1000000
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
GT_INLINE GT_STATUS prvCpssDrvHwPpIsrRead
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
);



/*******************************************************************************
* prvCpssDrvHwPpIsrWrite
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
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
* GalTis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpIsrWrite
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 value
);

/*******************************************************************************
* prvCpssDrvHwPpReadInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
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
GT_STATUS prvCpssDrvHwPpReadInternalPciReg
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);


/*******************************************************************************
* prvCpssDrvHwPpWriteInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
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
GT_STATUS prvCpssDrvHwPpWriteInternalPciReg
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

/*******************************************************************************
* prvCpssDrvHwPpResetAndInitControllerReadReg
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
GT_STATUS prvCpssDrvHwPpResetAndInitControllerReadReg
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);

/*******************************************************************************
* prvCpssDrvHwPpResetAndInitControllerGetRegField
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
GT_STATUS prvCpssDrvHwPpResetAndInitControllerGetRegField
 (
    IN  GT_U8   devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
);

/*******************************************************************************
* prvCpssDrvHwPpResetAndInitControllerWriteReg
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
GT_STATUS prvCpssDrvHwPpResetAndInitControllerWriteReg
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

/*******************************************************************************
* prvCpssDrvHwPpResetAndInitControllerSetRegField
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
GT_STATUS prvCpssDrvHwPpResetAndInitControllerSetRegField
 (
    IN  GT_U8   devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvHwCntlh */


