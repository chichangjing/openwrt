/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvPpCfgInit.h
*
* DESCRIPTION:
*       Includes initialization functions for the driver level of the CPSS Sw.
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*
*******************************************************************************/
#ifndef __prvCpssDrvPpCfgInith
#define __prvCpssDrvPpCfgInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpssDriver/pp/prvCpssDrvErrataMng.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>


/*
 * typedef: enum PRV_CPSS_DRV_DEV_ID_LOCATION_ENT
 *
 * Description: the enumeration indicate the location of the DEV_ID for the
 *              device family (the specific device is unknown at this stage)
 *
 *
 * Enumerations:
 *    PRV_CPSS_DRV_DEV_ID_LOCATION_PCI_CONFIG_CYCLE_ADDR_0x00000000_E - take info from
 *                  address 0x00000000 on the PCI config cycle space
 *                  (relevant only to PCI devices (not PEX))
 *   PRV_CPSS_DRV_DEV_ID_LOCATION_PEX_CONFIGURATION_HEADER_ADDR_0X00070000_E  - take info from
 *                  address 0x00070000 on the PEX config header space
 *                  (relevant only to PCI devices (not PCI))
 *    PRV_CPSS_DRV_DEV_ID_LOCATION_DEVICE_ADDR_0x0000004C_E - take DEV_ID from
 *                  register 0x0000004c in the device address space (PCI/PEX/SMI...)
 *                  NOTE: the vendorId in register 0x00000050 (0x11AB)
 *
 */
typedef enum
{
    PRV_CPSS_DRV_DEV_ID_LOCATION_PCI_CONFIG_CYCLE_ADDR_0x00000000_E,
    PRV_CPSS_DRV_DEV_ID_LOCATION_PEX_CONFIGURATION_HEADER_ADDR_0X00070000_E,
    PRV_CPSS_DRV_DEV_ID_LOCATION_DEVICE_ADDR_0x0000004C_E
}PRV_CPSS_DRV_DEV_ID_LOCATION_ENT;

/*
 * Typedef: structure PRV_CPSS_DRV_PP_PHASE_1_INFO_STC
 *
 * Description: hold info about the device received by the cpssDriver, when
 *              calling prvCpssDrvPpHwPhase1Init(...)
 *
 * Fields:
 *      mngInterfaceType    - the management interface SMI/PCI/TWSI/PEX
 *      busBaseAddr[]       - the unique bus Base address (PEX/PCI base address/
 *                            SMI id select/ WSI id select...) that the
 *                            device connected to on the management interface
 *                            bus(PEX/PCI/SMI/TWSI).
 *                            NOTEs:
 *                            1. Index in the array is the portGroupId
 *                            2. Use PRV_CPSS_DRV_PARAM_NOT_USED_CNS for non active portGroup.
 *      internalPciBase[]   - Base address to which the internal PCI registers
 *                            are mapped to. Relevant only when mngInterfaceType = PCI/PEX.
 *                            1. Index in the array is the portGroupId
 *                            2. ignored when busBaseAddr[index] == PRV_CPSS_DRV_PARAM_NOT_USED_CNS
 *      hwAddr[]            - HW (bus) address info. Used for CPSS_CHANNEL_PEX_KERNEL_E only
 *                            1. Index in the array is the portGroupId
 *    resetAndInitControllerBase - Base address to which Reset and Init
 *                                 Controller (a.k.a. DFX server) are mapped to.
 *                                 (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      ppHAState           - PP's High availability mode : Active / Standby.
 *      devIdLocationType   - the location of the DEV_ID for those device family
 *      numOfPortGroups     - Number of port groups.
 *                            For 'non multi-port-groups' device --> use 1
 *                            For Lion (regardless to active port-groups) should be 4
 *      isrAddrCompletionRegionsBmp  - Bitmap of configurable address completion regions
 *                            used by interrupt hanling routine.
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *      appAddrCompletionRegionsBmp  - Bitmap of configurable address completion regions
 *                            used by CPSS API.
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                            Bobcat2, Caelum, Bobcat3 PPs that have 8 address completion regions.
 *                            Region 0 provide access to the lower addresses that contain also
 *                            configuration of address completion regions. It must never be configured.
 *                            Lists (coded as ranges) of other regions can be assigned for several CPUs
 *                            for interrupt handling (for such CPU that processes the PP interrupts)
 *                            and for all other CPSS API using.
 *                            These assigned ranges of regions must not have common members.
 *                            These assigned ranges of regions must not contain zero range.
 *
*/
typedef struct
{
    CPSS_PP_INTERFACE_CHANNEL_ENT   mngInterfaceType;
    GT_UINTPTR                      busBaseAddr[CPSS_MAX_PORT_GROUPS_CNS];
    GT_UINTPTR                      internalPciBase[CPSS_MAX_PORT_GROUPS_CNS];
    GT_UINTPTR                      resetAndInitControllerBase;
    CPSS_HW_ADDR_STC                hwAddr[CPSS_MAX_PORT_GROUPS_CNS];
    CPSS_SYS_HA_MODE_ENT            ppHAState;
    PRV_CPSS_DRV_DEV_ID_LOCATION_ENT devIdLocationType;
    GT_U32                          numOfPortGroups;
    GT_U32                          isrAddrCompletionRegionsBmp;
    GT_U32                          appAddrCompletionRegionsBmp;
}PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC;

/*
 * Typedef: structure PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC
 *
 * Description: hold info about the device returned by the cpssDriver, when
 *              calling prvCpssDrvPpHwPhase1Init(...)
 *
 * Fields:
 *      devType     - device type of the PP.
 *      revision    - The device's revision number.
 *      devFamily   - CPSS's device family that current device belongs to.
 *      devSubFamily - CPSS's device sub-family that current device belongs to.
 *      numOfPorts  - number of ports in this device
 *                    For 'multi-port-groups' device , the number of 'global ports'.
 *                    For example : Lion --> 60 ports (0..59)
 *      existingPorts - bmp of actual exiting ports
 *                      needed for devices that their physical ports not
 *                      continues from first port num to last port num.
 *                    For 'multi-port-groups' device , the bmp of 'global ports'
 *                    For example : Lion --> 0..11 , 16..27 , 32..43 , 48..59
 *
*/
typedef struct{
    CPSS_PP_DEVICE_TYPE     devType;
    GT_U8                   revision;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;
    CPSS_PP_SUB_FAMILY_TYPE_ENT devSubFamily;
    GT_U8                   numOfPorts;
    CPSS_PORTS_BMP_STC      existingPorts;
}PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC;


/*
 * Typedef: structure PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC
 *
 * Description: hold info about the device returned by the cpssDriver, when
 *              calling prvCpssDrvPpHwPhase1Init(...)
 *
 * Fields:
 *      prvCpssDrvAfterDbRelease  - flag that state the DB of cpssDriver was
 *                                  released and now we are after that stage
 *                                  NOTE: the flag start with GT_FALSE , but once
 *                                  changed to GT_TRUE it will stay that way forever.
 *      performReadAfterWrite     - GT_TRUE - perform read after write
 *                                  GT_FALSE - otherwise
*/
typedef struct
{
    GT_BOOL  prvCpssDrvAfterDbRelease;
    GT_BOOL  performReadAfterWrite;
} PRV_CPSS_DRV_GLOBAL_DATA_STC;

extern PRV_CPSS_DRV_GLOBAL_DATA_STC drvGlobalInfo;
/*******************************************************************************
* prvCpssDrvSysConfigPhase1
*
* DESCRIPTION:
*       This function sets cpssDriver system level system configuration
*       parameters before any of the device's phase1 initialization .
*
*
*  APPLICABLE DEVICES: ALL systems must call this function
*
* INPUTS:
*       none
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvSysConfigPhase1
(
    void
);

/*******************************************************************************
* prvCpssDrvHwPpPhase1Init
*
* DESCRIPTION:
*       This function is called by cpssExMxHwPpPhase1Init() or other
*       cpss "phase 1" family functions, in order to enable PP Hw access,
*       the device number provided to this function may
*       be changed when calling prvCpssDrvPpHwPhase2Init().
*
*  APPLICABLE DEVICES: ALL devices
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
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPhase1Init
(
    IN  GT_U8                devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr,
    OUT PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC *ppOutInfoPtr
);


/*******************************************************************************
* prvCpssDrvHwPpPhase2Init
*
* DESCRIPTION:
*       the function set parameters for the driver for the "init Phase 2".
*       the function "renumber" the current device number of the device to a
*       new device number.
*
*  APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       devNum     - The PP's current device number .
*       newDevNum  - The PP's "new" device number to register.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL     - on error
*       GT_ALREADY_EXIST - the new device number is already used
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPhase2Init
(
    IN GT_U8    devNum,
    IN GT_U8    newDevNum
);

/*******************************************************************************
* prvCpssDrvHwPpRamBurstConfigSet
*
* DESCRIPTION:
*       Sets the Ram burst information for a given device.
*       info need to be set during "phase 1" (before "phase 2")
*
*  APPLICABLE DEVICES: ExMx devices
*
* INPUTS:
*       devNum          - The Pp's device number.
*       ramBurstInfoList - A list of Ram burst information for this device.
*       burstInfoLen    - Number of valid entries in ramBurstInfo.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       Narrow SRAM burst is not supported under Tiger devices.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpRamBurstConfigSet
(
    IN  GT_U8               devNum,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfoList,
    IN  GT_U8               burstInfoLen
);

/*******************************************************************************
* prvCpssDrvHwPpInitInMasks

*
* DESCRIPTION:
*      Set the interrupt mask for a given device.
*
*  APPLICABLE DEVICES: ExMx devices
*
* INPUTS:
*       devNum      - The Pp's device number.
*       b4StartInit - Is the call to this function is done before / after start
*                     Init.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*      None.
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpInitInMasks
(
    IN  GT_U8   devNum,
    IN  GT_BOOL b4StartInit
);


/*******************************************************************************
* prvCpssDrvHwPpDevRemove
*
* DESCRIPTION:
*       remove the device from the CPSS driver.
*       the function clean all the internal DB relate to the device , disconnect
*       from interrupts of the device
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       devNum - device number to remove.
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
GT_STATUS prvCpssDrvHwPpDevRemove
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDrvHwPpHaModeSet
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
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpHaModeSet
(
    IN  GT_U8   devNum,
    IN  CPSS_SYS_HA_MODE_ENT mode
);

/*******************************************************************************
* prvCpssDrvHwPpRenumber
*
* DESCRIPTION:
*       the function set parameters for the driver to renumber it's DB.
*       the function "renumber" the current device number of the device to a
*       new device number.
*       NOTE:
*       this function MUST be called under 'Interrupts are locked'
*
*  APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       oldDevNum  - The PP's "old" device number .
*       newDevNum  - The PP's "new" device number swap the DB to.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL     - on error
*       GT_ALREADY_EXIST - the new device number is already used
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpRenumber
(
    IN GT_U8    oldDevNum,
    IN GT_U8    newDevNum
);

/*******************************************************************************
* prvCpssDrvDebugDeviceIdSet
*
* DESCRIPTION:
*       This is debug function.
*       The function overrides device ID by given value.
*       The function should be called before cpssDxChHwPpPhase1Init().
*
*  APPLICABLE DEVICES: All devices
*
* INPUTS:
*       devNum  - device number .
*       devType - device type to store.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL     - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvDebugDeviceIdSet
(
    IN GT_U8                    devNum,
    IN CPSS_PP_DEVICE_TYPE      devType
);

/*******************************************************************************
* prvCpssDrvAddrCheckWaBind
*
* DESCRIPTION:
*       Binds errata db with address check callback
*
* APPLICABLE DEVICES: All devices
*
* INPUTS:
*       devNum           - the device number
*       addrCheckFuncPtr - pointer to callback function that checks addresses.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_INITIALIZED       - the driver was not initialized for the device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvAddrCheckWaBind
(
    IN  GT_U8                                   devNum,
    IN  PRV_CPSS_DRV_ERRATA_ADDR_CHECK_FUNC     addrCheckFuncPtr
);

/*******************************************************************************
* Function: prvCpssDrvPortsFullMaskGet
*
* DESCRIPTION:
*       get bmp of all phy ports of the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       numOfPorts - number of physical ports in the device
*
* OUTPUTS:
*       portsBmpPtr - (pointer to) bmp of ports
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvCpssDrvPortsFullMaskGet
(
    IN GT_U32                   numOfPorts,
    OUT CPSS_PORTS_BMP_STC       *portsBmpPtr
);

/*******************************************************************************
* Function: prvCpssDrvPortsBmpMaskWithMaxPorts
*
* DESCRIPTION:
*       mask bmp ports with the max number of ports of the of the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       portsBmpPtr - (pointer to) bmp of ports
*
* OUTPUTS:
*       portsBmpPtr - (pointer to) bmp of ports , after the mask
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvCpssDrvPortsBmpMaskWithMaxPorts
(
    IN GT_U8                        devNum,
    INOUT CPSS_PORTS_BMP_STC       *portsBmpPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvPpCfgInith */


