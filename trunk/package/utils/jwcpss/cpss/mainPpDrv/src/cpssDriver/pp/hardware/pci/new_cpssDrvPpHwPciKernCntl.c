/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpHwTEMPLATECntl.c
*
* DESCRIPTION:
*       Prestera driver Hardware read and write functions implementation.
*       TEMPLATE
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#define _GNU_SOURCE

#include <sys/ioctl.h>
#ifdef  PRESTERA_SYSCALLS
#   include <linux/unistd.h>
#endif

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

#include <cpssDriver/pp/hardware/private/presteraPpDriverGlob.h>



typedef struct PRV_CPSS_BUS_DRIVER_PCIKERN_STCT {
    CPSS_BUS_DRIVER_OBJ_STC common;
    GT_U32          driverId;
} PRV_CPSS_BUS_DRIVER_PCIKERN_STC;


/*******************************************************************************
* Intenal Macros and definitions
*******************************************************************************/
#define PP_DRIVER_IO_OP(_op,_reg,_length,_dataPtr) \
    struct mvPpDrvDriverIo_STC io; \
    io.id = drv->driverId; \
    io.op = mvPpDrvDriverIoOps_ ## _op ## _E; \
    io.regAddr = _reg; \
    io.length = _length; \
    io.dataPtr = (mv_kmod_uintptr_t)((uintptr_t)_dataPtr); \
    if (prestera_ctl(PRESTERA_PP_DRIVER_IO, &io)) \
    { \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); \
    } \
    return GT_OK;

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/*******************************************************************************
* Forward functions declarations.
*******************************************************************************/

    
    


/*******************************************************************************
* hwPpPciKernReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to read from.
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
static GT_STATUS hwPpPciKernReadRegister
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    PP_DRIVER_IO_OP(PpRegRead,regAddr,1,data);
}


/*******************************************************************************
* hwPpPciKernWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       drv             - Driver object ptr
*       regAddr - The register's address to write to.
*       data    - The value to be written.
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
static GT_STATUS hwPpPciKernWriteRegister
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    PP_DRIVER_IO_OP(PpRegWrite,regAddr,1,&value);
    return GT_OK;
}


/*******************************************************************************
* hwPpPciKernReadRam
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       drv     - Driver object ptr
*       addr    - Address offset to read from.
*       length  - Number of Words (4 byte) to read.
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
static GT_STATUS hwPpPciKernReadRam
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN GT_U32 addr,
    IN GT_U32 length,
    OUT GT_U32  *data
)
{
    PP_DRIVER_IO_OP(RamRead,addr,length,data);
    return GT_OK;
}



/*******************************************************************************
* hwPpPciKernWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       drv     - Driver object ptr
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
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciKernWriteRam
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    PP_DRIVER_IO_OP(RamWrite,addr,length,data);
}

/*******************************************************************************
* hwPpPciKernIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to read from.
*                 Note: regAddr should be < 0x1000000
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
static GT_STATUS hwPpPciKernIsrRead
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    PP_DRIVER_IO_OP(PpRegRead,regAddr,1,dataPtr);
}

/*******************************************************************************
* hwPpPciKernIsrWrite
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to write to.
*       data    - The value to be written.
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
static GT_STATUS hwPpPciKernIsrWrite
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    PP_DRIVER_IO_OP(PpRegWrite,regAddr,1,&value);
}

/*******************************************************************************
* hwPpPciKernReadInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to read from.
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
static GT_STATUS hwPpPciKernReadInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    PP_DRIVER_IO_OP(PciRegRead,regAddr,1,data);
}


/*******************************************************************************
* hwPpPciKernWriteInternalPciReg
*
* DESCRIPTION:
*       This function writes to an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to write to.
*       data    - Data to be written.
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
static GT_STATUS hwPpPciKernWriteInternalPciReg
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    PP_DRIVER_IO_OP(PciRegWrite,regAddr,1,&data);
}


/*******************************************************************************
* hwPpPciKernResetAndInitCtrlReadRegister
*
* DESCRIPTION:
*       This function reads a register from the Reset and Init Controller.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to read from.
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
static GT_STATUS hwPpPciKernResetAndInitCtrlReadRegister
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    PP_DRIVER_IO_OP(DfxRegRead,regAddr,1,data);
}


/*******************************************************************************
* hwPpPciKernResetAndInitCtrlWriteRegister
*
* DESCRIPTION:
*       This function writes a register to the Reset and Init Controller.
*
* INPUTS:
*       drv     - Driver object ptr
*       regAddr - The register's address to wrire to.
*       data    - Data to be written.
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
static GT_STATUS hwPpPciKernResetAndInitCtrlWriteRegister
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    PP_DRIVER_IO_OP(DfxRegWrite,regAddr,1,&data);
}

/*******************************************************************************
* hwPpPciKernHwInit
*
* DESCRIPTION:
*       This function initializes a given PP.
*
* INPUTS:
*       drv     - Driver object ptr
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpPciKernHwInit
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv
)
{
    PP_DRIVER_IO_OP(Reset,0,0,NULL);
}

/*******************************************************************************
* hwPpPciKernDestroyDrv
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
static GT_VOID hwPpPciKernDestroyDrv
(
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv
)
{
    struct mvPpDrvDriverIo_STC io;
    io.id = drv->driverId;
    io.op = mvPpDrvDriverIoOps_Destroy_E;
    prestera_ctl(PRESTERA_PP_DRIVER_IO, &io);
    cpssOsFree(drv);
}

/*******************************************************************************
* prvCpssDriverPpHwPciKernInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*
* INPUTS:
*       devNum      - The PP's device number to init the structure for.
*       portGroupId - The port group id.
*       hwInfo      - Hardware info (PCI address)
*       driverType  - 
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
static GT_STATUS prvCpssDriverPpHwPciKernInit
(
    IN  PRV_CPSS_DRV_HW_INFO_STC    *hwInfo,
    IN  enum mvPpDrvDriverType_ENT   driverType,
    OUT CPSS_BUS_DRIVER_OBJ_STC     **drvRet
)
{
    PRV_CPSS_BUS_DRIVER_PCIKERN_STC *drv;
    struct mvPpDrvDriverOpen_STC inf;

    inf.busNo = hwInfo->busNo;
    inf.devSel = hwInfo->devSel;
    inf.funcNo = hwInfo->funcNo;
    inf.type = driverType;

    if (prestera_ctl(PRESTERA_PP_DRIVER_OPEN,&inf))
    {
        /* failed */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* create object */
    drv = (PRV_CPSS_BUS_DRIVER_PCIKERN_STC*)cpssOsMalloc(sizeof(*drv));
    if (drv == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    *drvRet = (CPSS_BUS_DRIVER_OBJ_STC*)drv;

    cpssOsMemSet(drv, 0, sizeof(*drv));
    drv->driverId = inf.id;

    drv->common.drvHwPpDestroyDrv = (CPSS_DRV_DESTROY_DRV_FUNC) hwPpPciKernDestroyDrv;
    drv->common.drvHwPpHwInit = (CPSS_DRV_HWINIT_FUNC) hwPpPciKernHwInit;
    drv->common.drvHwPpReadReg = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPciKernReadRegister;
    drv->common.drvHwPpWriteReg = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPciKernWriteRegister;
    drv->common.drvHwPpReadRam = (CPSS_DRV_HW_READ_RAM_FUNC) hwPpPciKernReadRam;
    drv->common.drvHwPpWriteRam = (CPSS_DRV_HW_WRITE_RAM_FUNC) hwPpPciKernWriteRam;
    drv->common.drvHwPpIsrRead = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPciKernIsrRead;
    drv->common.drvHwPpIsrWrite = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPciKernIsrWrite;
    drv->common.drvHwPpReadIntPciReg = (CPSS_DRV_HW_READ_REG_FUNC) hwPpPciKernReadInternalPciReg;
    drv->common.drvHwPpWriteIntPciReg = (CPSS_DRV_HW_WRITE_REG_FUNC) hwPpPciKernWriteInternalPciReg;
    drv->common.drvHwPpResetAndInitControllerReadReg = (CPSS_DRV_HW_READ_REG_FUNC)hwPpPciKernResetAndInitCtrlReadRegister;
    drv->common.drvHwPpResetAndInitControllerWriteReg = (CPSS_DRV_HW_WRITE_REG_FUNC)hwPpPciKernResetAndInitCtrlWriteRegister;


    return GT_OK;
}


/*******************************************************************************
* cpssDriverPpHwBusPciKernInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*       Implemented in kernel module, no memory maping
*       hwInfo->appAddrCompletionRegionsBmp:
*           0x0f  - use 4 regions address completion (64M virtual addresses)
*           0x03  - use 2 regions address completion (32M virtual addresses)
*           0x3c  - use new 8 regions address completion (4M virtual addresses)
*
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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;
    enum mvPpDrvDriverType_ENT drvType;
    switch (hwInfo->appAddrCompletionRegionsBmp)
    {
        case 0x0f:
            drvType = mvPpDrvDriverType_Pci_E;
            break;
        case 0x03:
            drvType = mvPpDrvDriverType_PciHalf_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    rc = prvCpssDriverPpHwPciKernInit(hwInfo, drvType, &drv);
    if (rc == GT_OK)
    {
        drv->devNum = devNum;
        drv->portGroupId = portGroupId;
        *drvRet = drv;
        return rc;
    }

#ifdef DEVELOPER_NEW_DRIVERS
    /* fallback to regular driver (not implemented */
    return cpssDriverPpHwBusPciInit(devNum, portGroupId, hwInfo, drvRet);
#else
    return rc;
#endif
}

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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;
    rc = prvCpssDriverPpHwPciKernInit(hwInfo,
            mvPpDrvDriverType_PexMbus_E, &drv);
    if (rc == GT_OK)
    {
        drv->devNum = devNum;
        drv->portGroupId = portGroupId;
        *drvRet = drv;
        return rc;
    }

    /* fallback to regular driver */
    return cpssDriverPpHwBusPexMbusInit(devNum, portGroupId, hwInfo, drvRet);
}
#endif
