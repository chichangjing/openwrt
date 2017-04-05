/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvObj.h
*
* DESCRIPTION:
*       Includes structs definition for the PCI/SMI/TWSI CPSS Driver Object.
*
*       private file to be used only in the cpssDriver
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/

#ifndef __prvCpssDrvObjh
#define __prvCpssDrvObjh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/events/cpssGenEventCtrl.h>

/*******************************************************************************
* PRV_CPSS_DRV_HW_CNTL_FUNC
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*
* INPUTS:
*       devNum          - The PP's device number to init the structure for.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
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
*
* COMMENTS:
*       1.  In case isDiag == GT_TRUE, no semaphores are initialized.
*
* Galtis:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (* PRV_CPSS_DRV_HW_CNTL_FUNC)
(
 IN GT_U8       devNum,
 IN GT_U32      portGroupId,
 IN GT_UINTPTR  baseAddr,
 IN GT_UINTPTR  internalPciBase,
 IN GT_BOOL     isDiag
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_READ_REG_FUNC
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       devNum          - The PP to read from.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_READ_REG_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_WRITE_REG_FUNC
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       devNum          - The PP to write to.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - The register's address to write to.
*       data            - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_WRITE_REG_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_GET_REG_FIELD_FUNC
*
* DESCRIPTION:
*       Read a selected register field.
*
* INPUTS:
*       devNum          - The PP device number to read from.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - The register's address to read from.
*       fieldOffset     - The start bit number in the register.
*       fieldLength     - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_GET_REG_FIELD_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_SET_SEG_FIELD_FUNC
*
* DESCRIPTION:
*       Write value to selected register field.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - The register's address to write to.
*       fieldOffset     - The start bit number in the register.
*       fieldLength     - The number of bits to be written to register.
*       fieldData       - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       this function actually read the register modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_SET_SEG_FIELD_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData

);

/*******************************************************************************
* PRV_CPSS_DRV_HW_READ_REG_BITMASK_FUNC
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*
* INPUTS:
*       devNum          - PP device number to read from.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - Register address to read from.
*       mask            - Mask for selecting the read bits.
*
* OUTPUTS:
*       dataPtr         - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_READ_REG_BITMASK_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_WRITE_REG_BITMASK_FUNC
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*
* INPUTS:
*       devNum          - PP device number to write to.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - Register address to write to.
*       mask            - Mask for selecting the written bits.
*       value           - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_WRITE_REG_BITMASK_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    IN GT_U32   value
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_READ_RAM_FUNC
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       devNum          - The PP device number to read from.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       addr            - Address offset to read from.
*       length          - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       data    - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_READ_RAM_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data
);



/*******************************************************************************
* PRV_CPSS_DRV_HW_WRITE_RAM_FUNC
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       addr            - Address offset to write to.
*       length          - Number of Words (4 byte) to write.
*       data            - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_WRITE_RAM_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_READ_VEC_FUNC
*
* DESCRIPTION:
*       Read from PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum          - The PP device number to read from.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       addrArr         - Address array to read from.
*       arrLen          - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_READ_VEC_FUNC)
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   addrArr[],
    OUT GT_U32   dataArr[],
    IN  GT_U32   arrLen
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_WRITE_VEC_FUNC
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       addrArr         - Address offset to write to.
*       dataArr         - An array containing the data to be written.
*       arrLen          - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_WRITE_VEC_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_WRITE_RAM_REV_FUNC
*
* DESCRIPTION:
*       Writes to PP's RAM in reverse.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       addr            - Address offset to write to.
*       length          - Number of Words (4 byte) to write.
*       data            - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_WRITE_RAM_REV_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_ISR_READ_FUNC
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       devNum          - The PP to read from.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - The register's address to read from.
*                         Note: regAddr should be < 0x1000000
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_ISR_READ_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   *dataPtr
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_ISR_WRITE_FUNC
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
*
* INPUTS:
*       devNum          - The PP to write to.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - The register's address to write to.
*       data            - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_ISR_WRITE_FUNC)
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_READ_INT_PCI_REG_FUNC
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum          - The Pp's device numbers.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - The register's address to read from.
*
* OUTPUTS:
*       data    - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_READ_INT_PCI_REG_FUNC)
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);


/*******************************************************************************
* PRV_CPSS_DRV_HW_WRITE_INT_PCI_REG_FUNC
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum          - The Pp's device numbers.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       regAddr         - The register's address to read from.
*       data            - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_WRITE_INT_PCI_REG_FUNC)
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);


/*******************************************************************************
* PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_READ_REG_FUNC
*
* DESCRIPTION:
*       Read a Reset and Init Controller register value.
*
* INPUTS:
*       devNum          - The PP device number to read from.
*       regAddr         - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_READ_REG_FUNC)
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);


/*******************************************************************************
* PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_GET_REG_FIELD_FUNC
*
* DESCRIPTION:
*       Read a Reset and Init Controller selected register field.
*
* INPUTS:
*       devNum          - The PP device number to read from.
*       regAddr         - The register's address to read from.
*       fieldOffset     - The start bit number in the register.
*       fieldLength     - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_GET_REG_FIELD_FUNC)
(
    IN  GT_U8   devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
);


/*******************************************************************************
* PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_WRITE_REG_FUNC
*
* DESCRIPTION:
*       Write to a Reset and Init Controller given register.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       regAddr         - The register's address to write to.
*       data            - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_WRITE_REG_FUNC)
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);


/*******************************************************************************
* PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_SET_REG_FIELD_FUNC
*
* DESCRIPTION:
*       Write value to a Reset and Init Controller selected register field.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       regAddr         - The register's address to write to.
*       fieldOffset     - The start bit number in the register.
*       fieldLength     - The number of bits to be written to register.
*       fieldData       - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       this function actually read the register modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_SET_REG_FIELD_FUNC)
(
    IN  GT_U8   devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
);


/*******************************************************************************
* PRV_CPSS_DRV_HW_CFG_PHASE1_INIT_FUNC
*
* DESCRIPTION:
*       This function is called by cpssExMxHwPpPhase1Init() or other
*       cpss "phase 1" family functions, in order to enable PP Hw access,
*       the device number provided to this function may
*       be changed when calling prvCpssDrvPpHwPhase2Init().
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       ppOutInfoPtr  - (pointer to)the info that driver return to caller.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_CFG_PHASE1_INIT_FUNC)
(
    IN  GT_U8                                   devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr,
    OUT PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC *ppOutInfoPtr
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_INTERRUPT_INIT_FUNC
*
* DESCRIPTION:
*       Initialize the interrupts mechanism for a given device.
*
* INPUTS:
*       devNum          - The device number to initialize the interrupts
*                         mechanism for.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       intVecNum       - The interrupt vector number this device is connected
*                         to.
*       intMask         - The interrupt mask to enable/disable interrupts on
*                         this device.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_INTERRUPT_INIT_FUNC)
(
    IN  GT_U8           devNum,
    IN  GT_U32          portGroupId,
    IN  GT_U32          intVecNum,
    IN  GT_U32          intMask
);
/*******************************************************************************
* PRV_CPSS_DRV_HW_EVENTS_MASK_FUNC
*
* DESCRIPTION:
*       enables / disables a given event from reaching the CPU.
*
* INPUTS:
*       devNum      - The PP's device number to mask / unmask the interrupt for.
*       intIndex    - The interrupt cause to enable/disable.
*       enable      - GT_TRUE enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_EVENTS_MASK_FUNC)
(
    IN  GT_U8   devNum,
    IN  GT_U32  intIndex,
    IN  GT_BOOL enable
);
/*******************************************************************************
* PRV_CPSS_DRV_HW_EVENTS_MASK_GET_FUNC
*
* DESCRIPTION:
*       Gets enable/disable status of a given event reaching the CPU.
*
* INPUTS:
*       devNum      - The PP's device number to mask / unmask the interrupt for.
*       intIndex    - The interrupt cause to get enable/disable status.
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE enable, GT_FALSE disable.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_VOID (*PRV_CPSS_DRV_HW_EVENTS_MASK_GET_FUNC)
(
    IN  GT_U8   devNum,
    IN  GT_U32  intIndex,
    OUT  GT_BOOL *enablePtr
);
/*******************************************************************************
* PRV_CPSS_DRV_HW_SET_RAM_BURST_CONFIG_FUNC

*
* DESCRIPTION:
*       Sets the Ram burst information for a given device.
*
* INPUTS:
*       devNum          - The Pp's device number.
*       ramBurstInfoPtr - A list of Ram burst information for this device.
*       burstInfoLen    - Number of valid entries in ramBurstInfo.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_SET_RAM_BURST_CONFIG_FUNC)
(
    IN  GT_U8                             devNum,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfoPtr,
    IN  GT_U8                             burstInfoLen
);
/*******************************************************************************
* PRV_CPSS_DRV_HW_SET_RAM_BURST_CONFIG_FUNC

*
* DESCRIPTION:
*      Set the interrupt mask for a given device.
*
* INPUTS:
*       devNum          - The Pp's device number.
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*       b4StartInit     - Is the call to this function is done before / after start
*                         Init.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*      None.
*
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_INIT_INT_MASKS_FUNC)
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId,
    IN  GT_BOOL b4StartInit
);


/*******************************************************************************
* PRV_CPSS_DRV_HW_PP_HA_MODE_SET_FUNC
*
* DESCRIPTION:
*       function to set CPU High Availability mode of operation.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum       - the device number.
*       mode - active or standby
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_HW_PP_HA_MODE_SET_FUNC)
(
    IN  GT_U8   devNum,
    IN  CPSS_SYS_HA_MODE_ENT mode
);

/*******************************************************************************
* PRV_CPSS_DRV_HW_INTERRUPTS_TREE_GET_FUNC
*
* DESCRIPTION:
*       function return :
*       1. the root to the interrupts tree info of the specific device
*       2. the interrupt registers that can't be accesses before 'Start Init'
*
* APPLICABLE DEVICES: All devices --> Packet Processors (not Fa/Xbar)
*
* INPUTS:
*       devNum          - the device number
*       portGroupId     - The port group Id.
*                         Relevant only for multi port groups devices.
*
* OUTPUTS:
*       numOfElementsPtr - (pointer to) number of elements in the tree.
*       treeRootPtrPtr - (pointer to) pointer to root of the interrupts tree info.
*       numOfInterruptRegistersNotAccessibleBeforeStartInitPtr - (pointer to)
*                           number of interrupt registers that can't be accessed
*                           before 'Start init'
*       notAccessibleBeforeStartInitPtrPtr (pointer to)pointer to the interrupt
*                           registers that can't be accessed before 'Start init'
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - the driver was not initialized for the device
*
* COMMENTS:
*
*
*******************************************************************************/
typedef GT_STATUS   (*PRV_CPSS_DRV_HW_INTERRUPTS_TREE_GET_FUNC)
(
    IN GT_U8                                 devNum,
    IN GT_U32                                portGroupId,
    OUT GT_U32                               *numElementsPtr,
    OUT const CPSS_INTERRUPT_SCAN_STC        **treeRootPtrPtr,
    OUT GT_U32  *numOfInterruptRegistersNotAccessibleBeforeStartInitPtr,
    OUT GT_U32  **notAccessibleBeforeStartInitPtrPtr
);



/*
 * Typedef: struct PRV_CPSS_DRV_MNG_INF_OBJ_STC
 *
 * Description: The structure defines the PCI/SMI/TWSI driver object
 *
 *
 * Fields:
 *      drvHwCntlInit                    - init of data driver structures
 *      drvHwPpReadReg                   - read register value
 *      drvHwPpWriteReg                  - write register value
 *      drvHwPpGetRegField               - get register value field
 *      drvHwPpSetRegField               - set register value field
 *      drvHwPpReadRegBitMask            - read register bit mask
 *      drvHwPpWriteRegBitMask           - write register bit mask
 *      drvHwPpReadRam                   - read RAM data
 *      drvHwPpWriteRam                  - write RAM data
 *      drvHwPpReadVec                   - read Vector of addresses (not consecutive) data
 *      drvHwPpWriteVec                  - write Vector of addresses (not consecutive) data
 *      drvHwPpWriteRamRev               - write RAM reversed data
 *      drvHwPpIsrRead                   - read register value using interrupt
 *                                           address completion region
 *      drvHwPpIsrWrite                  - write register value using interrupt
 *                                           address completion region
 *      drvHwPpReadIntPciReg             - read internal PCI register
 *      drvHwPpWriteIntPciReg            - write internal PCI register
 *      drvHwPpResetAndInitControllerReadReg     - read Reset and Init 
 *                                                    Controller register value
 *      drvHwPpResetAndInitControllerWriteReg    - write Reset and Init 
 *                                                    Controller register value
 *      drvHwPpResetAndInitControllerGetRegField - get Reset and Init 
 *                                              Controller register field value
 *      drvHwPpResetAndInitControllerSetRegField - set Reset and Init 
 *                                              Controller register field value 
 */
typedef struct {
    PRV_CPSS_DRV_HW_CNTL_FUNC                      drvHwCntlInit;
    PRV_CPSS_DRV_HW_READ_REG_FUNC                  drvHwPpReadReg;
    PRV_CPSS_DRV_HW_WRITE_REG_FUNC                 drvHwPpWriteReg;
    PRV_CPSS_DRV_HW_GET_REG_FIELD_FUNC             drvHwPpGetRegField;
    PRV_CPSS_DRV_HW_SET_SEG_FIELD_FUNC             drvHwPpSetRegField;
    PRV_CPSS_DRV_HW_READ_REG_BITMASK_FUNC          drvHwPpReadRegBitMask;
    PRV_CPSS_DRV_HW_WRITE_REG_BITMASK_FUNC         drvHwPpWriteRegBitMask;
    PRV_CPSS_DRV_HW_READ_RAM_FUNC                  drvHwPpReadRam;
    PRV_CPSS_DRV_HW_WRITE_RAM_FUNC                 drvHwPpWriteRam;
    PRV_CPSS_DRV_HW_READ_VEC_FUNC                  drvHwPpReadVec;
    PRV_CPSS_DRV_HW_WRITE_VEC_FUNC                 drvHwPpWriteVec;
    PRV_CPSS_DRV_HW_WRITE_RAM_REV_FUNC             drvHwPpWriteRamRev;
    PRV_CPSS_DRV_HW_ISR_READ_FUNC                  drvHwPpIsrRead;
    PRV_CPSS_DRV_HW_ISR_WRITE_FUNC                 drvHwPpIsrWrite;
    PRV_CPSS_DRV_HW_READ_INT_PCI_REG_FUNC          drvHwPpReadIntPciReg;
    PRV_CPSS_DRV_HW_WRITE_INT_PCI_REG_FUNC         drvHwPpWriteIntPciReg;
    PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_READ_REG_FUNC          
                                       drvHwPpResetAndInitControllerReadReg;
    PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_WRITE_REG_FUNC          
                                       drvHwPpResetAndInitControllerWriteReg;
    PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_GET_REG_FIELD_FUNC          
                                       drvHwPpResetAndInitControllerGetRegField;
    PRV_CPSS_DRV_HW_RESET_AND_INIT_CTRL_SET_REG_FIELD_FUNC          
                                       drvHwPpResetAndInitControllerSetRegField;
}PRV_CPSS_DRV_MNG_INF_OBJ_STC;


#ifdef INCL_EXDXMX_DRIVER
/* pointer to the object that control PCI */
extern PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPciPtr;

/* pointer to the object that control PEX with 8 address completion regions */
extern PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPexMbusPtr;

/* pointer to the object that control PCI -- for HA standby */
extern PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPciHaStandbyPtr;

/* pointer to the object that control SMI */
extern PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfSmiPtr;

/* pointer to the object that control TWSI */
extern PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfTwsiPtr;

/* pointer to the object that control PEX */
/*extern PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPexPtr;*/

#endif /* INCL_SOHO_DRIVER */

/*
 * Typedef: struct PRV_CPSS_DRV_GEN_OBJ_STC
 *
 * Description: The structure defines generic driver object functions that not
 *              relate to PCI/SMI/TWSI
 *
 *
 * Fields:
 *      drvCfgPpHwPhase1Init             - initialization of "Phase1"
 *      drvPpHwInterruptInit             - Initialize the interrupts mechanism
 *                                         for a given device
 *      drvHwPpSetRamBurstConfig         - Set the interrupt mask for a given
 *                                         device
 *      driverPpHwEventsMask             - enables / disables a given event from
 *                                         reaching the CPU
 *      drvHwPpHwEventsMaskGet           - Gets enable/disable status of a given
 *                                         event reaching to the CPU
 *      drvHwPpInitInMasks               - Set the interrupt mask for a given
 *                                         device
 *      drvHwPpInterruptsTreeGet         - get the interrupt tree info
 *                                         and interrupt registers that can't be
 *                                         accesses before 'Start Init'
 *
 *      busPtr - pointer to the functions that are bus (pci/smi/twsi) oriented
 *              should be one of:
 *                     prvCpssDrvMngInfPciPtr   - PCI
 *                     prvCpssDrvMngInfPciHaStandbyPtr - PCI standby
 *                     prvCpssDrvMngInfSmiPtr   - SMI
 *                     prvCpssDrvMngInfTwsiPtr  - TWSI
 *                     NULL -- for SOHO devices
 *
 */
typedef struct {
    PRV_CPSS_DRV_HW_CFG_PHASE1_INIT_FUNC           drvHwPpCfgPhase1Init;
    PRV_CPSS_DRV_HW_INTERRUPT_INIT_FUNC            drvHwPpInterruptInit;
    PRV_CPSS_DRV_HW_EVENTS_MASK_FUNC               drvHwPpEventsMask;
    PRV_CPSS_DRV_HW_EVENTS_MASK_GET_FUNC           drvHwPpEventsMaskGet;
    PRV_CPSS_DRV_HW_SET_RAM_BURST_CONFIG_FUNC      drvHwPpSetRamBurstConfig;
    PRV_CPSS_DRV_HW_INIT_INT_MASKS_FUNC            drvHwPpInitInMasks;
    PRV_CPSS_DRV_HW_PP_HA_MODE_SET_FUNC            drvHwPpHaModeSet;
    PRV_CPSS_DRV_HW_INTERRUPTS_TREE_GET_FUNC       drvHwPpInterruptsTreeGet;
}PRV_CPSS_DRV_GEN_OBJ_STC;

#ifdef INCL_SOHO_DRIVER
/* pointer to the object that control SOHO */
extern PRV_CPSS_DRV_GEN_OBJ_STC *prvCpssDrvGenSohoObjPtr;
#endif /* INCL_SOHO_DRIVER */

#ifdef INCL_EXDXMX_DRIVER
/* pointer to the object that control ExMxDx */
extern PRV_CPSS_DRV_GEN_OBJ_STC *prvCpssDrvGenExMxDxObjPtr;
#endif /* INCL_SOHO_DRIVER */

/*
 * Typedef: struct PRV_CPSS_DRV_OBJ_STC
 *
 * Description: The structure defines driver object
 *
 *
 * Fields:
 *      genPtr - pointer to the function that are not bus oriented .
 *              should be one of:
 *                  prvCpssDrvGenSohoObjPtr  - for SOHO device
 *                  prvCpssDrvGenExMxDxObjPtr - for other devices
 *
 *      busPtr - pointer to the functions that are bus (pci/smi/twsi) oriented
 *              should be one of:
 *                  prvCpssDrvMngInfPciPtr   - PCI
 *                  prvCpssDrvMngInfPciHaStandbyPtr - PCI standby
 *                  prvCpssDrvMngInfSmiPtr   - SMI
 *                  prvCpssDrvMngInfTwsiPtr  - TWSI
 *                  NULL -- for SOHO devices
 *
 */
typedef struct {
    PRV_CPSS_DRV_GEN_OBJ_STC                      *genPtr;
    PRV_CPSS_DRV_MNG_INF_OBJ_STC                   *busPtr;
    CPSS_SYS_HA_MODE_ENT                          HAState;
}PRV_CPSS_DRV_OBJ_STC;


/* macro to check if the driver hold the generic functions object */
#define PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum)     \
    if(prvCpssDrvPpObjConfig[devNum] == NULL ||         \
       prvCpssDrvPpObjConfig[devNum]->genPtr == NULL)   \
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG)

/* macro to check if the driver hold the BUS oriented functions object */
#define PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum)     \
    if(prvCpssDrvPpObjConfig[devNum] == NULL)           \
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG)

/* declare the driver object functions */
extern PRV_CPSS_DRV_OBJ_STC* prvCpssDrvPpObjConfig[PRV_CPSS_MAX_PP_DEVICES_CNS];


/*******************************************************************************
* prvCpssDrvHwPciDriverObjectInit
*
* DESCRIPTION:
*      This function creates and initializes PCI device driver object
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPciDriverObjectInit
(
     void
);

/*******************************************************************************
* prvCpssDrvHwPexMbusDriverObjectInit
*
* DESCRIPTION:
*      This function creates and initializes PEX device with 8 address complete regions driver object
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPexMbusDriverObjectInit
(
     void
);

/*******************************************************************************
* prvCpssDrvHwSmiDriverObjectInit
*
* DESCRIPTION:
*      This function creates and initializes SMI device driver object
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwSmiDriverObjectInit
(
     void
);

/*******************************************************************************
* prvCpssDrvHwTwsiDriverObjectInit
*
* DESCRIPTION:
*      This function creates and initializes Twsi device driver object
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwTwsiDriverObjectInit
(
     void
);

/*******************************************************************************
* prvCpssDrvHwPciStandByDriverObjectInit
*
* DESCRIPTION:
*      This function creates and initializes PCI device driver object for
*      a standby CPU
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPciStandByDriverObjectInit
(
     void
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvObjh */


