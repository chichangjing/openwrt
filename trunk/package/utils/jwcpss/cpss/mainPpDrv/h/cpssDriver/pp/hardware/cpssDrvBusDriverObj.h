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

#ifndef __prvCpssDrvBusDriverObjh
#define __prvCpssDrvBusDriverObjh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 * typedef: struct PRV_CPSS_DRV_HW_INFO_STC
 *
 * Description:
 *
 * Hardware access info to manage PP
 *
 * Fields:
 *
 *   baseAddr        - Base address to which PP is mapped on PCI.
 *                     in case of SMI it would be smiIdSel.
 *   internalPciBase - Base address to which the internal PCI registers
 *                     are mapped to.
 *   resetAndInitControllerBase - Base address to which Reset and Init
 *                     Controller (a.k.a. DFX server) are mapped to.
 *                     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3; xCat3, etc)
 *   busNo           - PCI/PEX busNo or SMI Id
 *   devSel          - PCI/PEX deviceNo
 *   funcNo          - PCI/PEX func
 *   isrAddrCompletionRegionsBmp  - Bitmap of configurable address completion regions
 *                            used by interrupt handling routine.
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                            If zero then initialized from
 *                            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId]
 *   appAddrCompletionRegionsBmp  - Bitmap of configurable address completion regions
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
 *                            If zero then initialized from
 *                            prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId]
 *
 * Comments:
 *
 */
typedef struct
{
    GT_UINTPTR  baseAddr;
    GT_UINTPTR  internalPciBase;
    GT_UINTPTR  resetAndInitControllerBase;
    GT_U32      busNo;
    GT_U32      devSel;
    GT_U32      funcNo;
    GT_U32      isrAddrCompletionRegionsBmp;
    GT_U32      appAddrCompletionRegionsBmp;
} PRV_CPSS_DRV_HW_INFO_STC;

struct CPSS_BUS_DRIVER_OBJ_STCT;

/*******************************************************************************
* CPSS_DRV_DESTROY_DRV_FUNC
*
* DESCRIPTION:
*       Destroy driver object
*
* INPUTS:
*       drv             - Driver object ptr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_VOID (*CPSS_DRV_DESTROY_DRV_FUNC)
(
    IN struct CPSS_BUS_DRIVER_OBJ_STCT *drv
);

/*******************************************************************************
* CPSS_DRV_HWINIT_FUNC
*
* DESCRIPTION:
*       This function initializes a given PP.
*
* INPUTS:
*       drv             - Driver object ptr
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_DRV_HWINIT_FUNC)
(
    IN struct CPSS_BUS_DRIVER_OBJ_STCT *drv
);

/*******************************************************************************
* CPSS_DRV_HW_READ_REG_FUNC
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       drv             - Driver object ptr
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
typedef GT_STATUS (*CPSS_DRV_HW_READ_REG_FUNC)
(
    IN struct CPSS_BUS_DRIVER_OBJ_STCT *drv,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
);

/*******************************************************************************
* CPSS_DRV_HW_WRITE_REG_FUNC
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       drv             - Driver object ptr
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
typedef GT_STATUS (*CPSS_DRV_HW_WRITE_REG_FUNC)
(
    IN struct CPSS_BUS_DRIVER_OBJ_STCT *drv,
    IN GT_U32   regAddr,
    IN GT_U32   value
);

/*******************************************************************************
* CPSS_DRV_HW_READ_RAM_FUNC
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       drv             - Driver object ptr
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
typedef GT_STATUS (*CPSS_DRV_HW_READ_RAM_FUNC)
(
    IN struct CPSS_BUS_DRIVER_OBJ_STCT *drv,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data
);



/*******************************************************************************
* CPSS_DRV_HW_WRITE_RAM_FUNC
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       drv             - Driver object ptr
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
typedef GT_STATUS (*CPSS_DRV_HW_WRITE_RAM_FUNC)
(
    IN struct CPSS_BUS_DRIVER_OBJ_STCT *drv,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
);

/*******************************************************************************
* CPSS_DRV_HW_READ_VEC_FUNC
*
* DESCRIPTION:
*       Read from PP's RAM a vector of addresses.
*
* INPUTS:
*       drv             - Driver object ptr
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
typedef GT_STATUS (*CPSS_DRV_HW_READ_VEC_FUNC)
(
    IN struct CPSS_BUS_DRIVER_OBJ_STCT *drv,
    IN  GT_U32   addrArr[],
    OUT GT_U32   dataArr[],
    IN  GT_U32   arrLen
);

/*******************************************************************************
* CPSS_DRV_HW_WRITE_VEC_FUNC
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*
* INPUTS:
*       drv             - Driver object ptr
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
typedef GT_STATUS (*CPSS_DRV_HW_WRITE_VEC_FUNC)
(
    IN struct CPSS_BUS_DRIVER_OBJ_STCT *drv,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
);

/*
 * Typedef: struct CPSS_BUS_DRIVER_OBJ_STC
 *
 * Description: The structure defines the PCI/SMI/TWSI driver object
 *
 *
 * Fields:
 *      drvHwPpReadReg                   - read register value
 *      drvHwPpWriteReg                  - write register value
 *      drvHwPpReadRam                   - read RAM data
 *      drvHwPpWriteRam                  - write RAM data
 *      drvHwPpReadVec                   - (optional) read Vector of addresses (not consecutive) data
 *      drvHwPpWriteVec                  - (optional) write Vector of addresses (not consecutive) data
 *      drvHwPpIsrRead                   - read register value using interrupt
 *                                           address completion region
 *      drvHwPpIsrWrite                  - write register value using interrupt
 *                                           address completion region
 *      drvHwPpReadIntPciReg             - (optional) read internal PCI register
 *      drvHwPpWriteIntPciReg            - (optional) write internal PCI register
 *      drvHwPpResetAndInitControllerReadReg         - (optional) read Reset and Init 
 *                                                     Controller register value
 *      drvHwPpResetAndInitControllerWriteReg        - (optional) write Reset and Init 
 *                                                     Controller register value
 *      drvHwPpGetBaseAddr               - (optional) Returns base address for register access
 *      drvHwPpRamBurstConfigSet         - (optional) Set RamBurst config
 */
typedef struct CPSS_BUS_DRIVER_OBJ_STCT {
    GT_U8                                      devNum;
    GT_U32                                     portGroupId;
    CPSS_DRV_DESTROY_DRV_FUNC                  drvHwPpDestroyDrv;
    CPSS_DRV_HWINIT_FUNC                       drvHwPpHwInit;
    CPSS_DRV_HW_READ_REG_FUNC                  drvHwPpReadReg;
    CPSS_DRV_HW_WRITE_REG_FUNC                 drvHwPpWriteReg;
    CPSS_DRV_HW_READ_RAM_FUNC                  drvHwPpReadRam;
    CPSS_DRV_HW_WRITE_RAM_FUNC                 drvHwPpWriteRam;
    CPSS_DRV_HW_READ_VEC_FUNC                  drvHwPpReadVec;
    CPSS_DRV_HW_WRITE_VEC_FUNC                 drvHwPpWriteVec;
    CPSS_DRV_HW_READ_REG_FUNC                  drvHwPpIsrRead;
    CPSS_DRV_HW_WRITE_REG_FUNC                 drvHwPpIsrWrite;
    CPSS_DRV_HW_READ_REG_FUNC                  drvHwPpReadIntPciReg;
    CPSS_DRV_HW_WRITE_REG_FUNC                 drvHwPpWriteIntPciReg;
    CPSS_DRV_HW_READ_REG_FUNC                  drvHwPpResetAndInitControllerReadReg;
    CPSS_DRV_HW_WRITE_REG_FUNC                 drvHwPpResetAndInitControllerWriteReg;
}CPSS_BUS_DRIVER_OBJ_STC;


#ifdef DEVELOPER_NEW_DRIVERS
/*TODO: ifdef */
/*******************************************************************************
* cpssDriverPpHwBusPciInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*
* INPUTS:
*       devNum      - The PP's device number to init the structure for.
*       portGroupId - The port group id.
*       hwInfo      - Hardware info (PCI address)
*
* OUTPUTS:
*       drvPtr   - pointer
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDriverPpHwBusPciInit
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  PRV_CPSS_DRV_HW_INFO_STC    *hwInfo,
    OUT CPSS_BUS_DRIVER_OBJ_STC     **drvRet
);

/*******************************************************************************
* cpssDriverPpHwBusPexMbusInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*
* INPUTS:
*       devNum      - The PP's device number to init the structure for.
*       portGroupId - The port group id.
*       hwInfo      - Hardware info (PCI address)
*
* OUTPUTS:
*       drvPtr   - pointer
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDriverPpHwBusPexMbusInit
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  PRV_CPSS_DRV_HW_INFO_STC    *hwInfo,
    OUT CPSS_BUS_DRIVER_OBJ_STC     **drvRet
);
#endif /* DEVELOPER_NEW_DRIVERS */

#ifdef LINUX
#ifndef ASIC_SIMULATION
/*******************************************************************************
* cpssDriverPpHwBusPciKernInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*       Implemented in kernel module, no memory maping
*       hwInfo->appAddrCompletionRegionsBmp:
*           0x0f  - use 4 regions address completion (64M virtual addresses)
*           0x03  - use 2 regions address completion (32M virtual addresses)
*
* INPUTS:
*       devNum      - The PP's device number to init the structure for.
*       portGroupId - The port group id.
*       hwInfo      - Hardware info (PCI address)
*
* OUTPUTS:
*       drvPtr   - pointer
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDriverPpHwBusPciKernInit
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  PRV_CPSS_DRV_HW_INFO_STC    *hwInfo,
    OUT CPSS_BUS_DRIVER_OBJ_STC     **drvRet
);

#ifdef DEVELOPER_NEW_DRIVERS
/*******************************************************************************
* cpssDriverPpHwBusPexMbusKernInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*       Implemented in kernel module, no memory maping
*
* INPUTS:
*       devNum      - The PP's device number to init the structure for.
*       portGroupId - The port group id.
*       hwInfo      - Hardware info (PCI address)
*
* OUTPUTS:
*       drvPtr   - pointer
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDriverPpHwBusPexMbusKernInit
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  PRV_CPSS_DRV_HW_INFO_STC    *hwInfo,
    OUT CPSS_BUS_DRIVER_OBJ_STC     **drvRet
);
#endif /* DEVELOPER_NEW_DRIVERS */

#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvBusDriverObjh */


