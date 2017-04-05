/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvPpHwApi.c
*
* DESCRIPTION:
*       Prestera driver Hardware API
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/
/* LOG in this module may be enabled only after adding CPSS_LOCK protection in it. */
/*#define CPSS_LOG_IN_MODULE_ENABLE*/ 
#include <cpssDriver/log/private/prvCpssDriverLog.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwPpPortGroupCntl.h>
#include <cpss/generic/log/prvCpssLog.h>

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

GT_BOOL memoryAccessTraceOn = GT_FALSE;

/* Object that holds callback functions to HW access */
CPSS_DRV_HW_ACCESS_OBJ_STC prvCpssHwAccessObj = {NULL, NULL,NULL, NULL,
                                                 NULL, NULL,NULL, NULL,
                                                 NULL, NULL,NULL, NULL,
                                                 NULL};

#define PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum)   \
    if (prvCpssDrvPpConfig[devNum] == NULL ) \
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, \
           prvCpssLogErrorMsgDeviceNotInitialized, devNum);\
    if (prvCpssDrvPpObjConfig[devNum]->HAState == CPSS_SYS_HA_MODE_STANDBY_E) \
        return GT_OK

#define PRV_CPSS_DRV_GET_PP_DRV(devNum,portGroupId,drv) \
    drv = prvCpssDrvPpConfig[devNum]->drivers[portGroupId]

#define PRV_CPSS_DRV_CHECK_GET_PP_DRV(devNum,portGroupId,drv) \
    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum); \
    PRV_CPSS_DRV_GET_PP_DRV(devNum,portGroupId,drv)


/* macro to start a loop on active port group ids */
#define PRV_CPSS_DRV_START_HANDLE_PORT_GROUPS_MAC(_devNum, _portGroupId)     \
{                                                                            \
    GT_U32 firstPg,lastPg,activePortGroupsBmp;                               \
    if (_portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)                    \
    {                                                                        \
        firstPg =                                                            \
            prvCpssDrvPpConfig[_devNum]->portGroupsInfo.firstActivePortGroup;\
        lastPg =                                                             \
            prvCpssDrvPpConfig[_devNum]->portGroupsInfo.lastActivePortGroup; \
        activePortGroupsBmp =                                                \
            prvCpssDrvPpConfig[_devNum]->portGroupsInfo.activePortGroupsBmp; \
    }                                                                        \
    else                                                                     \
    {                                                                        \
        firstPg = (_portGroupId);                                            \
        lastPg = (_portGroupId);                                             \
        activePortGroupsBmp = (1<<(_portGroupId));                           \
    }                                                                        \
    for((_portGroupId) = firstPg; (_portGroupId) <= lastPg; (_portGroupId)++)\
    {                                                                        \
        if(0 == (activePortGroupsBmp & (1<<(_portGroupId))))                 \
        {                                                                    \
            continue;                                                        \
        }

/* macro to end a loop on active port groups */
#define PRV_CPSS_DRV_END_HANDLE_PORT_GROUPS_MAC(_devNum, _portGroupId)       \
    }                                                                        \
}

extern GT_BOOL  prvCpssDrvTraceHwWrite[PRV_CPSS_MAX_PP_DEVICES_CNS];
extern GT_BOOL  prvCpssDrvTraceHwRead[PRV_CPSS_MAX_PP_DEVICES_CNS];

/*******************************************************************************
* prvCpssDrvHwPpDoReadOpDrv
*
* DESCRIPTION:
*       Read a selected register 
*
* INPUTS:
*       drv         - Driver object ptr
*       readFunc    - Method to read register
*       isrContext  -
*       addrSpace   - 
*       regAddr     - The register's address to read from.
*
* OUTPUTS:
*       dataPtr     - Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success, otherwise error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDrvHwPpDoReadOpDrv
(
    IN CPSS_BUS_DRIVER_OBJ_STC      *drv,
    IN CPSS_DRV_HW_READ_REG_FUNC    readFunc,
    IN GT_BOOL                      isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT addrSpace,
    IN GT_U32                       regAddr,
    OUT GT_U32                      *dataPtr
)
{
    GT_STATUS rc;
    rc = readFunc(drv, regAddr, dataPtr);
    if (rc != GT_OK)
        return rc;

    /* Check if trace hw read is enabled */
    if (prvCpssDrvTraceHwRead[drv->devNum] == GT_TRUE)
    {
        rc = cpssTraceHwAccessRead(drv->devNum,
                                     drv->portGroupId,
                                     isrContext,
                                     addrSpace,
                                     regAddr,
                                     1, /* length to trace the data in words */
                                     dataPtr);
    }
    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpDoWriteOpDrv
*
* DESCRIPTION:
*       write a selected register bitmasked value.
*
* INPUTS:
*       drv         - Driver object ptr
*       writeFunc   - Method to write register
*       isrContext  -
*       addrSpace   - 
*       regAddr     - The register's address to read from.
*       data        - Data to be written to register.
*
*
* RETURNS:
*       GT_OK   - on success, otherwise error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDrvHwPpDoWriteOpDrv
(
    IN CPSS_BUS_DRIVER_OBJ_STC      *drv,
    IN CPSS_DRV_HW_WRITE_REG_FUNC   writeFunc,
    IN GT_BOOL                      isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT addrSpace,
    IN GT_U32                       regAddr,
    IN GT_U32                       data
)
{
    GT_STATUS rc;
    rc = writeFunc(drv, regAddr, data);
    if (rc != GT_OK)
        return rc;
    /* Check if trace hw write is enabled */
    if (prvCpssDrvTraceHwWrite[drv->devNum] == GT_TRUE)
    {
        rc = cpssTraceHwAccessWrite(drv->devNum,
                                      drv->portGroupId,
                                      isrContext,
                                      addrSpace,
                                      regAddr,
                                      1, /* length to trace the data in words */
                                      &data);
    }
    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpGetRegFieldDrv
*
* DESCRIPTION:
*       Read a selected register field.
*
* INPUTS:
*       drv         - Driver object ptr
*       readFunc    - Method to read register
*       isrContext  -
*       addrSpace   - 
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
static GT_STATUS prvCpssDrvHwPpGetRegFieldDrv
(
    IN CPSS_BUS_DRIVER_OBJ_STC      *drv,
    IN CPSS_DRV_HW_READ_REG_FUNC     readFunc,
    IN GT_BOOL                      isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT addrSpace,
    IN GT_U32                       regAddr,
    IN GT_U32                       fieldOffset,
    IN GT_U32                       fieldLength,
    OUT GT_U32                      *fieldData
)
{
    GT_U32 data, mask;
    GT_STATUS rc;

    rc = prvCpssDrvHwPpDoReadOpDrv(drv, readFunc, isrContext, addrSpace,
            regAddr, &data);

    if (rc != 0)
        return rc;

    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    *fieldData = (GT_U32)((data & mask) >> fieldOffset);
    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpWriteRegBitMaskDrv
*
* DESCRIPTION:
*       write a selected register bitmasked value.
*
* INPUTS:
*       drv         - Driver object ptr
*       readFunc    - Method to read register
*       writeFunc   - Method to write register
*       isrContext  -
*       addrSpace   - 
*       regAddr     - The register's address to read from.
*       mask    - Mask for selecting the written bits.
*       value   - Data to be written to register.
*
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
static GT_STATUS prvCpssDrvHwPpWriteRegBitMaskDrv
(
    IN CPSS_BUS_DRIVER_OBJ_STC      *drv,
    IN CPSS_DRV_HW_READ_REG_FUNC    readFunc,
    IN CPSS_DRV_HW_WRITE_REG_FUNC   writeFunc,
    IN GT_BOOL                      isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT addrSpace,
    IN GT_U32                       regAddr,
    IN GT_U32                       mask,
    IN GT_U32                       value
)
{
    GT_U32 data;
    GT_STATUS rc;

    if(mask == 0xffffffff)
    {
        return prvCpssDrvHwPpDoWriteOpDrv(drv, writeFunc, isrContext, addrSpace,
                regAddr, value);
    }
    rc = prvCpssDrvHwPpDoReadOpDrv(drv, readFunc, isrContext, addrSpace,
            regAddr, &data);
    if (rc != GT_OK)
        return rc;
    data &= ~mask;
    data |= (value & mask);
    rc = prvCpssDrvHwPpDoWriteOpDrv(drv, writeFunc, isrContext, addrSpace,
            regAddr, data);
    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpSetRegFieldDrv
*
* DESCRIPTION:
*       write a selected register field.
*
* INPUTS:
*       drv         - Driver object ptr
*       readFunc    - Method to read register
*       writeFunc   - Method to write register
*       isrContext  -
*       addrSpace   - 
*       regAddr     - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*       fieldData   - Data to read from the register.
*
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
static GT_STATUS prvCpssDrvHwPpSetRegFieldDrv
(
    IN CPSS_BUS_DRIVER_OBJ_STC      *drv,
    IN CPSS_DRV_HW_READ_REG_FUNC    readFunc,
    IN CPSS_DRV_HW_WRITE_REG_FUNC   writeFunc,
    IN GT_BOOL                      isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT addrSpace,
    IN GT_U32                       regAddr,
    IN GT_U32                       fieldOffset,
    IN GT_U32                       fieldLength,
    IN GT_U32                       fieldData
)
{
    GT_U32 mask, value;

    CALC_MASK_MAC(fieldLength, fieldOffset, mask);
    value = ((fieldData << fieldOffset) & mask);

    return prvCpssDrvHwPpWriteRegBitMaskDrv(drv,
            readFunc, writeFunc, isrContext, addrSpace,
            regAddr, mask, value);
}

/*******************************************************************************
* prvCpssDrvHwPpReadRegBitMaskDrv
*
* DESCRIPTION:
*       Read a selected register bit masked value.
*
* INPUTS:
*       drv         - Driver object ptr
*       readFunc    - Method to read register
*       isrContext  -
*       addrSpace   - 
*       regAddr     - The register's address to read from.
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
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDrvHwPpReadRegBitMaskDrv
(
    IN CPSS_BUS_DRIVER_OBJ_STC      *drv,
    IN CPSS_DRV_HW_READ_REG_FUNC    readFunc,
    IN GT_BOOL                      isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT addrSpace,
    IN GT_U32                       regAddr,
    IN GT_U32                       mask,
    OUT GT_U32                      *dataPtr
)
{
    GT_STATUS rc;

    rc = prvCpssDrvHwPpDoReadOpDrv(drv, readFunc, isrContext, addrSpace,
            regAddr, dataPtr);
    if (rc != 0)
        return rc;
    *dataPtr &= mask;
    return rc;
}




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
)
{
    GT_STATUS rc;
#if (!defined(ASIC_SIMULATION) && !defined(NO_KERN_PP_DRV) && defined(LINUX)) || defined(DEVELOPER_NEW_DRIVERS)
    PRV_CPSS_DRV_HW_INFO_STC hwInfo;
#endif
    CPSS_BUS_DRIVER_OBJ_STC *drv = NULL;

    /* check if driver object is ready */
    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.isMultiPortGroupDevice == GT_FALSE)
    {
        portGroupId = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.numOfPortGroups        = 1;
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.activePortGroupsBmp    = BIT_0;
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.firstActivePortGroup   = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.lastActivePortGroup    = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;

        prvCpssDrvPpConfig[devNum]->intCtrl.
            portGroupInfo[CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS].
                isrCookieInfo.devNum = devNum;
        prvCpssDrvPpConfig[devNum]->intCtrl.
            portGroupInfo[CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS].
                isrCookieInfo.portGroupId = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;
    }
    else if(0 == PRV_CPSS_DRV_IS_IN_RANGE_PORT_GROUP_ID_MAC(devNum,portGroupId))
    {
        /* out of range parameter */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else /* multi port groups device */
    {
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.numOfPortGroups++;
        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.numOfPortGroups >= 32)
        {
            /* limit to 32 due to current GT_U32 as bitmap */
            /* for active port groupss                     */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
        }

        prvCpssDrvPpConfig[devNum]->portGroupsInfo.activePortGroupsBmp    |= (1 << portGroupId);

        /* check if need to update the firstActivePortGroup */
        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.firstActivePortGroup > portGroupId)
        {
            prvCpssDrvPpConfig[devNum]->portGroupsInfo.firstActivePortGroup = portGroupId;
        }

        /* check if need to update the lastActivePortGroup */
        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.lastActivePortGroup < portGroupId)
        {
            prvCpssDrvPpConfig[devNum]->portGroupsInfo.lastActivePortGroup = portGroupId;
        }

        prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].isrCookieInfo.devNum = devNum;
        prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].isrCookieInfo.portGroupId = portGroupId;

        /* initializer the 'force' portGroupId to 'Not forcing' */
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.debugForcedPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }



#if (!defined(ASIC_SIMULATION) && !defined(NO_KERN_PP_DRV) && defined(LINUX)) || defined(DEVELOPER_NEW_DRIVERS)
    hwInfo.baseAddr = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].baseAddr;
    hwInfo.internalPciBase = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].internalPciBase;
    hwInfo.resetAndInitControllerBase = prvCpssDrvPpConfig[devNum]->resetAndInitControllerBase;
    hwInfo.busNo  = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwAddr.busNo;
    hwInfo.devSel = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwAddr.devSel;
    hwInfo.funcNo = prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwAddr.funcNo;
    hwInfo.isrAddrCompletionRegionsBmp =
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].isrAddrCompletionRegionsBmp;
    hwInfo.appAddrCompletionRegionsBmp =
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].appAddrCompletionRegionsBmp;
#endif

    rc = GT_OK;
    switch (prvCpssDrvPpConfig[devNum]->mngInterfaceType)
    {
        case CPSS_CHANNEL_PEX_KERNEL_E:
#if !defined(ASIC_SIMULATION) && !defined(NO_KERN_PP_DRV) && defined(LINUX)
            rc = cpssDriverPpHwBusPciKernInit(devNum, portGroupId, &hwInfo, &drv);
#else /* defined(ASIC_SIMULATION) || !defined(LINUX) */
#  ifdef DEVELOPER_NEW_DRIVERS
            /* fallback to CPSS_PP_DRIVER_TYPE_PEX_E */
            prvCpssDrvPpConfig[devNum]->mngInterfaceType = CPSS_CHANNEL_PEX_NEWDRV_E;
#  else /* !DEVELOPER_NEW_DRIVERS */
            /* fallback to legacy driver */
            prvCpssDrvPpConfig[devNum]->mngInterfaceType = CPSS_CHANNEL_PEX_E;
            break;
#  endif /* !DEVELOPER_NEW_DRIVERS */
#endif /* defined(ASIC_SIMULATION) || !defined(LINUX) */
#ifdef DEVELOPER_NEW_DRIVERS
        case CPSS_CHANNEL_PEX_NEWDRV_E:
#  if defined(LINUX) || defined(ASIC_SIMULATION)
            rc = cpssDriverPpHwBusPciInit(devNum, portGroupId, &hwInfo, &drv);
#  else
            prvCpssDrvPpConfig[devNum]->mngInterfaceType = CPSS_CHANNEL_PEX_E;
#  endif
            break;
        case CPSS_CHANNEL_PEX_MBUS_KERNEL_E:
#  if !defined(ASIC_SIMULATION) && defined(LINUX)
            rc = cpssDriverPpHwBusPexMbusKernInit(devNum, portGroupId, &hwInfo, &drv);
            break;
#  else
            /* fallback to CPSS_PP_DRIVER_TYPE_PEX_MBUS_E */
            prvCpssDrvPpConfig[devNum]->mngInterfaceType = CPSS_CHANNEL_PEX_MBUS_NEWDRV_E;
#  endif
        case CPSS_CHANNEL_PEX_MBUS_NEWDRV_E:
#  if defined(LINUX) || defined(ASIC_SIMULATION)
            rc = cpssDriverPpHwBusPexMbusInit(devNum, portGroupId, &hwInfo, &drv);
#  else
            prvCpssDrvPpConfig[devNum]->mngInterfaceType = CPSS_CHANNEL_PEX_MBUS_E;
#  endif
            break;
#endif /* DEVELOPER_NEW_DRIVERS */
        default:
            break;
    }

    if (rc != GT_OK)
        return rc;

    if (drv != NULL)
    {
        prvCpssDrvPpConfig[devNum]->drivers[portGroupId] = drv;
        rc = drv->drvHwPpHwInit(drv);
        return rc;
    }
    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwCntlInit(devNum,portGroupId,
                                  baseAddr,internalPciBase,isDiag);
    return rc;
}



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
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpReadRegister);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, data));

    rc = prvCpssDrvHwPpPortGroupReadRegister(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRegister
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpWriteRegister);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, value));

    rc = prvCpssDrvHwPpPortGroupWriteRegister(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpGetRegField);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, fieldOffset, fieldLength, fieldData));

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr,fieldOffset,fieldLength,fieldData);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
*       this function actually read the register modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpSetRegField
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData

)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpSetRegField);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, fieldOffset, fieldLength, fieldData));

    rc = prvCpssDrvHwPpPortGroupSetRegField(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr,fieldOffset,fieldLength,fieldData);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpReadRegBitMask);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, mask, dataPtr));

    rc = prvCpssDrvHwPpPortGroupReadRegBitMask(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr,mask,dataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRegBitMask
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 value
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpWriteRegBitMask);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, mask, value));

    rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr,mask,value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadRam
(
    IN GT_U8    devNum,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpReadRam);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, addr, length, data));

    rc = prvCpssDrvHwPpPortGroupReadRam(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr,length,data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;

}


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
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRam
(
    IN GT_U8  devNum,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpWriteRam);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, addr, length, data));

    rc = prvCpssDrvHwPpPortGroupWriteRam(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr,length,data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpReadVec);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, addrArr, dataArr, arrLen));

    rc = prvCpssDrvHwPpPortGroupReadVec(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addrArr,dataArr,arrLen);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpWriteVec);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, addrArr, dataArr, arrLen));

    rc = prvCpssDrvHwPpPortGroupWriteVec(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addrArr,dataArr,arrLen);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpWriteRamInReverse);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, addr, length, data));

    rc =  prvCpssDrvHwPpPortGroupWriteRamInReverse(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr,length,data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       devNum  - The PP to read from.
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
GT_INLINE GT_STATUS prvCpssDrvHwPpIsrRead
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    return prvCpssDrvHwPpPortGroupIsrRead(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,dataPtr);
}
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
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpIsrWrite
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    return prvCpssDrvHwPpPortGroupIsrWrite(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,value);
}
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
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpReadInternalPciReg);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, data));

    rc = prvCpssDrvHwPpPortGroupReadInternalPciReg(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
)
{
    GT_STATUS rc;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpWriteInternalPciReg);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, data));

    rc = prvCpssDrvHwPpPortGroupWriteInternalPciReg(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpResetAndInitControllerReadReg);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, data));

    PRV_CPSS_DRV_CHECK_GET_PP_DRV(devNum,CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS,drv);
    
    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->
                    drvHwPpResetAndInitControllerReadReg(devNum, regAddr, data);

        CPSS_LOG_API_EXIT_MAC(funcId, rc);
        return rc;
    }

    if (drv->drvHwPpResetAndInitControllerReadReg == NULL)
    {
        CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, GT_NOT_SUPPORTED);
    }

    rc = prvCpssDrvHwPpDoReadOpDrv(drv, drv->drvHwPpResetAndInitControllerReadReg,
        GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_RESET_AND_INIT_CTRL_E,
        regAddr, data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpResetAndInitControllerGetRegField);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum,  regAddr, fieldOffset, fieldLength, fieldData));

    PRV_CPSS_DRV_CHECK_GET_PP_DRV(devNum,CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS,drv);

    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->
            drvHwPpResetAndInitControllerGetRegField(
                                                devNum, regAddr,
                                                fieldOffset,fieldLength,fieldData);
        CPSS_LOG_API_EXIT_MAC(funcId, rc);
        return rc;
    }
    if (drv->drvHwPpResetAndInitControllerReadReg == NULL)
    {
        CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, GT_NOT_SUPPORTED);
    }

    rc = prvCpssDrvHwPpGetRegFieldDrv(drv, drv->drvHwPpResetAndInitControllerReadReg,
            GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_RESET_AND_INIT_CTRL_E,
            regAddr,fieldOffset,fieldLength,fieldData);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpResetAndInitControllerWriteReg);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, data));


    PRV_CPSS_DRV_CHECK_GET_PP_DRV(devNum,CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS,drv);
    
    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->
                       drvHwPpResetAndInitControllerWriteReg(devNum, regAddr, data);
        CPSS_LOG_API_EXIT_MAC(funcId, rc);
        return rc;
    }
    if (drv->drvHwPpResetAndInitControllerWriteReg == NULL)
    {
        CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, GT_NOT_SUPPORTED);
    }

    rc = prvCpssDrvHwPpDoWriteOpDrv(drv, drv->drvHwPpResetAndInitControllerWriteReg,
            GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_RESET_AND_INIT_CTRL_E,
            regAddr, data);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpResetAndInitControllerSetRegField);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, regAddr, fieldOffset, fieldLength, fieldData));

    PRV_CPSS_DRV_CHECK_GET_PP_DRV(devNum,CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS,drv);
    
    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->
            drvHwPpResetAndInitControllerSetRegField(
                                                devNum, regAddr,
                                                fieldOffset,fieldLength,fieldData);
        CPSS_LOG_API_EXIT_MAC(funcId, rc);
        return rc;
    }
    if (    drv->drvHwPpResetAndInitControllerReadReg == NULL ||
            drv->drvHwPpResetAndInitControllerWriteReg == NULL)
    {
        CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, GT_NOT_SUPPORTED);
    }

    rc = prvCpssDrvHwPpSetRegFieldDrv(drv,
            drv->drvHwPpResetAndInitControllerReadReg,
            drv->drvHwPpResetAndInitControllerWriteReg,
            GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_RESET_AND_INIT_CTRL_E,
            regAddr, fieldOffset,fieldLength,fieldData);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    PRV_CPSS_DRV_PORT_GROUPS_INFO_STC *pgInfo;

    if (prvCpssDrvPpConfig[devNum] == NULL )
       return 0;
    pgInfo = &(prvCpssDrvPpConfig[devNum]->portGroupsInfo);
    if(pgInfo->isMultiPortGroupDevice == GT_TRUE)
    {
        if(pgInfo->debugForcedPortGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
            portGroupId = pgInfo->debugForcedPortGroupId;

        if((portGroupId) == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
        {   /*use the first active port group */
            portGroupId = pgInfo->firstActivePortGroup;
        }
        else if(0 == PRV_CPSS_DRV_IS_IN_RANGE_PORT_GROUP_ID_MAC(devNum,portGroupId) ||
               (0 == (pgInfo->activePortGroupsBmp & (1<<(portGroupId)))))
        {   /* non active port group */
            return NULL;
        }
    }
    else  /*use the only 'active port group' */
    {
        portGroupId = pgInfo->firstActivePortGroup;
    }
    return prvCpssDrvPpConfig[devNum]->drivers[portGroupId];
}

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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupReadRegister);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, regAddr, data));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);
    PRV_CPSS_DRV_GET_PP_DRV(devNum,portGroupId,drv);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterReadFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRegisterReadFunc( 
                                                  devNum, portGroupId, 
                                                  regAddr, data,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }
    
    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadReg(
            devNum, portGroupId, regAddr, data);
    }
    else
    {
        rc = prvCpssDrvHwPpDoReadOpDrv(drv, drv->drvHwPpReadReg,
                GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                regAddr, data);
    }
    
    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterReadFunc != NULL)
    {
       prvCpssHwAccessObj.hwAccessRegisterReadFunc( 
                                                  devNum, portGroupId, 
                                                  regAddr, data,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_STATUS rc = GT_OK;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupWriteRegister);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, regAddr, value));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum,portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterWriteFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRegisterWriteFunc( 
                                                  devNum, portGroupId, 
                                                  regAddr, value,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }

    PRV_CPSS_DRV_START_HANDLE_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        drv = prvCpssDrvPpConfig[devNum]->drivers[portGroupId];
        if (drv == NULL)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteReg(
                devNum,portGroupId,regAddr,value);
        }
        else
        {
            rc = prvCpssDrvHwPpDoWriteOpDrv(drv, drv->drvHwPpWriteReg,
                    GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                    regAddr, value);
        }
        if(rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }
    PRV_CPSS_DRV_END_HANDLE_PORT_GROUPS_MAC(devNum,portGroupId)

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterWriteFunc != NULL)
    {
       prvCpssHwAccessObj.hwAccessRegisterWriteFunc( 
                                                  devNum, portGroupId, 
                                                  regAddr, value,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}



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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;
   

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupGetRegField);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, regAddr, fieldOffset, fieldLength, fieldData));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);
    PRV_CPSS_DRV_GET_PP_DRV(devNum,portGroupId,drv);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterFieldReadFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRegisterFieldReadFunc( 
                                                  devNum, portGroupId, regAddr, 
                                                  fieldOffset, fieldLength, fieldData,                                                
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
        if(rc!= GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpGetRegField(
            devNum, portGroupId,
            regAddr,fieldOffset,fieldLength,fieldData);
    }
    else
    {
        rc = prvCpssDrvHwPpGetRegFieldDrv(drv, drv->drvHwPpReadReg,
            GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
            regAddr,fieldOffset,fieldLength,fieldData);
    }


     /* Check if HW access required. */
     if(prvCpssHwAccessObj.hwAccessRegisterFieldReadFunc != NULL)
     {
         rc = prvCpssHwAccessObj.hwAccessRegisterFieldReadFunc( 
                                                    devNum, portGroupId, regAddr, 
                                                    fieldOffset, fieldLength, fieldData,                                                
                                                    CPSS_DRV_HW_ACCESS_STAGE_POST_E);
     }
     
     CPSS_LOG_API_EXIT_MAC(funcId, rc);

     return rc;
}

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
)
{
    GT_STATUS rc = GT_OK;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupSetRegField);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, regAddr, fieldOffset, fieldLength, fieldData));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum,portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterFieldWriteFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRegisterFieldWriteFunc( 
                                                  devNum, portGroupId, regAddr, 
                                                  fieldOffset, fieldLength, fieldData,                                                
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }

    PRV_CPSS_DRV_START_HANDLE_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        drv = prvCpssDrvPpConfig[devNum]->drivers[portGroupId];
        if (drv == NULL)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpSetRegField(
                devNum, portGroupId,
                regAddr,fieldOffset,fieldLength,fieldData);
        }
        else
        {
            rc = prvCpssDrvHwPpSetRegFieldDrv(drv,
                drv->drvHwPpReadReg, drv->drvHwPpWriteReg,
                GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                regAddr,fieldOffset,fieldLength,fieldData);
        }
        if(rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }
    PRV_CPSS_DRV_END_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterFieldWriteFunc != NULL)
    {
        prvCpssHwAccessObj.hwAccessRegisterFieldWriteFunc( 
                                                   devNum, portGroupId, regAddr, 
                                                   fieldOffset, fieldLength, fieldData,
                                                   CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupReadRegBitMask);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, regAddr, mask, dataPtr));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);
    PRV_CPSS_DRV_GET_PP_DRV(devNum,portGroupId,drv);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterBitMaskReadFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRegisterBitMaskReadFunc( 
                                                  devNum, portGroupId, regAddr, 
                                                  mask, dataPtr,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }

    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadRegBitMask(
            devNum, portGroupId, regAddr, mask, dataPtr);
    }
    else
    {
        rc = prvCpssDrvHwPpReadRegBitMaskDrv(drv, drv->drvHwPpReadReg,
            GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
            regAddr, mask, dataPtr);
    }
    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterBitMaskReadFunc != NULL)
    {
       prvCpssHwAccessObj.hwAccessRegisterBitMaskReadFunc( 
                                                  devNum, portGroupId, regAddr, 
                                                  mask, dataPtr,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }


    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_STATUS rc = GT_OK;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupWriteRegBitMask);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, regAddr, mask, value));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterBitMaskWriteFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRegisterBitMaskWriteFunc( 
                                                  devNum, portGroupId, regAddr, 
                                                  mask, value,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }

    PRV_CPSS_DRV_START_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)
    {
        drv = prvCpssDrvPpConfig[devNum]->drivers[portGroupId];
        if (drv == NULL)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteRegBitMask(
                devNum, portGroupId, regAddr, mask, value);
        }
        else
        {
            rc = prvCpssDrvHwPpWriteRegBitMaskDrv(drv,
                drv->drvHwPpReadReg, drv->drvHwPpWriteReg,
                GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                regAddr, mask, value);
        }
        if(rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }
    PRV_CPSS_DRV_END_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterBitMaskWriteFunc != NULL)
    {
       prvCpssHwAccessObj.hwAccessRegisterBitMaskWriteFunc( 
                                                  devNum, portGroupId, regAddr, 
                                                  mask, value,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupReadRam);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, addr, length, data));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);
    PRV_CPSS_DRV_CHECK_GET_PP_DRV(devNum,portGroupId,drv);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address range*/
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 addr,
                                                                 length);
        if (rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRamReadFunc!= NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRamReadFunc( 
                                        devNum, portGroupId,
                                        addr, length, data,
                                        CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }

    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadRam(
            devNum, portGroupId, addr, length, data);
    }
    else
    {
        rc = drv->drvHwPpReadRam(drv, addr, length, data);
        if (rc == GT_OK && prvCpssDrvTraceHwRead[devNum] == GT_TRUE)
        {
            rc = cpssTraceHwAccessRead(devNum, portGroupId,
                    GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                    addr, length, data);
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRamReadFunc!= NULL)
    {
       prvCpssHwAccessObj.hwAccessRamReadFunc( 
                                        devNum, portGroupId,
                                        addr, length, data,
                                        CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }
    
    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}


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
)
{
    GT_STATUS rc = GT_OK;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupWriteRam);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, addr, length, data));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address range*/
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 addr,
                                                                 length);
        if (rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRamWriteFunc!= NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRamWriteFunc( 
                                        devNum, portGroupId,
                                        addr, length, data,
                                        CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }

    PRV_CPSS_DRV_START_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)
    {
        drv = prvCpssDrvPpConfig[devNum]->drivers[portGroupId];
        if (drv == NULL)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteRam(
                devNum, portGroupId, addr, length, data);
        }
        else
        {
            rc = drv->drvHwPpWriteRam(drv, addr, length, data);
            if (rc == GT_OK && prvCpssDrvTraceHwWrite[drv->devNum] == GT_TRUE)
            {
                rc = cpssTraceHwAccessWrite(devNum, portGroupId, GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                        addr, length, data);
            }
        }
        if(rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }
    PRV_CPSS_DRV_END_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRamWriteFunc!= NULL)
    {
       prvCpssHwAccessObj.hwAccessRamWriteFunc( 
                                        devNum, portGroupId,
                                        addr, length, data,
                                        CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_STATUS rc = GT_OK;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupWriteRamInReverse);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, addr, length, data));

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address range*/
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 addr,
                                                                 length);
        if (rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRamWriteFunc!= NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRamWriteInReverseFunc( 
                                        devNum, portGroupId,
                                        addr, length, data,
                                        CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }

    PRV_CPSS_DRV_START_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)
    {
        drv = prvCpssDrvPpConfig[devNum]->drivers[portGroupId];
        if (drv != NULL)
        {
            rc = GT_NOT_IMPLEMENTED;
        }
        else
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteRamRev(
                devNum, portGroupId, addr, length, data);
        }
        if(rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }
    PRV_CPSS_DRV_END_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRamWriteFunc!= NULL)
    {
       prvCpssHwAccessObj.hwAccessRamWriteInReverseFunc( 
                                        devNum, portGroupId,
                                        addr, length, data,
                                        CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }
    
    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_U32      i;
    GT_STATUS   rc = GT_OK;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupReadVec);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, addrArr, dataArr, arrLen));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);
    PRV_CPSS_DRV_CHECK_GET_PP_DRV(devNum, portGroupId, drv);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address ranges */
        for (i = 0; i < arrLen; i++)
        {
            rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                     portGroupId,
                                                                     addrArr[i],
                                                                     1);
            if (rc != GT_OK)
            {
                CPSS_LOG_API_EXIT_MAC(funcId, rc);
                return rc;
            }
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessVectorReadFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessVectorReadFunc( 
                                                  devNum, portGroupId,
                                                  addrArr, dataArr, arrLen,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }

    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadVec(
            devNum, portGroupId, addrArr, dataArr, arrLen);
    }
    else if (drv->drvHwPpReadVec != NULL)
    {
        rc = drv->drvHwPpReadVec(drv, addrArr, dataArr, arrLen);
        /* TODO: Trace */
    }
    else
    {
        for (i = 0; i < arrLen; i++)
        {
            rc = prvCpssDrvHwPpDoReadOpDrv(drv, drv->drvHwPpReadReg,
                    GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                    addrArr[i], &dataArr[i]);
            if (GT_OK != rc)
            {
                break;
            }
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessVectorReadFunc != NULL)
    {
       prvCpssHwAccessObj.hwAccessVectorReadFunc( 
                                                  devNum, portGroupId,
                                                  addrArr, dataArr, arrLen,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_U32      i;
    GT_STATUS rc = GT_OK;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupWriteVec);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, addrArr, dataArr, arrLen));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address ranges */
        for (i = 0; i < arrLen; i++)
        {
            rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                     portGroupId,
                                                                     addrArr[i],
                                                                     1);
            if (rc != GT_OK)
            {
                CPSS_LOG_API_EXIT_MAC(funcId, rc);
                return rc;
            }
        }
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessVectorWriteFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessVectorWriteFunc( 
                                                  devNum, portGroupId,
                                                  addrArr, dataArr, arrLen,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           CPSS_LOG_API_EXIT_MAC(funcId, GT_OK);
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, rc);
       }
    }

    PRV_CPSS_DRV_START_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)
    {
        drv = prvCpssDrvPpConfig[devNum]->drivers[portGroupId];
        if (drv == NULL)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteVec(
                devNum, portGroupId, addrArr, dataArr, arrLen);
        }
        else if (drv->drvHwPpWriteVec != NULL)
        {
            rc = drv->drvHwPpWriteVec(drv, addrArr, dataArr, arrLen);
            /* TODO: handle trace */
        }
        else
        {
            for (i = 0; i < arrLen; i++)
            {
                rc = prvCpssDrvHwPpDoWriteOpDrv(drv, drv->drvHwPpWriteReg,
                        GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                        addrArr[i],dataArr[i]);
                if (rc != GT_OK)
                    break;
            }
        }
        if(rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }
    PRV_CPSS_DRV_END_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessVectorWriteFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessVectorWriteFunc( 
                                                  devNum, portGroupId,
                                                  addrArr, dataArr, arrLen,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);
    PRV_CPSS_DRV_CHECK_GET_PP_DRV(devNum,portGroupId,drv);

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterIsrReadFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRegisterIsrReadFunc( 
                                                  devNum, portGroupId, 
                                                  regAddr, dataPtr,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           return rc;
       }
    }

    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpIsrRead(
            devNum, portGroupId, regAddr, dataPtr);
    }
    else if (drv->drvHwPpIsrRead == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    else
    {
        rc = prvCpssDrvHwPpDoReadOpDrv(drv, drv->drvHwPpIsrRead,
                GT_TRUE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                regAddr, dataPtr);
    }

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterIsrReadFunc != NULL)
    {
       prvCpssHwAccessObj.hwAccessRegisterIsrReadFunc( 
                                                  devNum, portGroupId, 
                                                  regAddr, dataPtr,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }


    return rc;
}

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
)
{
    GT_STATUS rc = GT_OK;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterIsrWriteFunc != NULL)
    {
       rc = prvCpssHwAccessObj.hwAccessRegisterIsrWriteFunc( 
                                                  devNum, portGroupId, 
                                                  regAddr, value,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E);
       if(rc == GT_ABORTED)
       {
           return GT_OK;
       }

       if(rc!= GT_OK)
       {
           return rc;
       }
    }

    PRV_CPSS_DRV_START_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)
    {
        drv = prvCpssDrvPpConfig[devNum]->drivers[portGroupId];
        if (drv == NULL)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpIsrWrite(
                devNum, portGroupId, regAddr, value);
        }
        else if (drv->drvHwPpIsrWrite == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }
        else
        {
            rc = prvCpssDrvHwPpDoWriteOpDrv(drv, drv->drvHwPpIsrWrite,
                    GT_TRUE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E,
                    regAddr, value);
        }
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DRV_END_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)

    /* Check if HW access required. */
    if(prvCpssHwAccessObj.hwAccessRegisterIsrWriteFunc != NULL)
    {
       prvCpssHwAccessObj.hwAccessRegisterIsrWriteFunc( 
                                                  devNum, portGroupId, 
                                                  regAddr, value,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E);
    }

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupReadInternalPciReg);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, regAddr, data));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);
    PRV_CPSS_DRV_GET_PP_DRV(devNum,portGroupId,drv);

    if (drv == NULL)
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadIntPciReg(
            devNum, portGroupId, regAddr, data);
    }
    else if (drv->drvHwPpReadIntPciReg == NULL)
    {
        CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, GT_NOT_SUPPORTED);
    }
    else
    {
        rc = prvCpssDrvHwPpDoReadOpDrv(drv, drv->drvHwPpReadIntPciReg,
                GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E,
                regAddr, data);
    }

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

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
)
{
    GT_STATUS rc = GT_OK;
    CPSS_BUS_DRIVER_OBJ_STC *drv;

    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, prvCpssDrvHwPpPortGroupWriteInternalPciReg);

    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupId, regAddr, data));

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    PRV_CPSS_DRV_START_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)
    {
        drv = prvCpssDrvPpConfig[devNum]->drivers[portGroupId];
        if (drv == NULL)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteIntPciReg(
                devNum, portGroupId, regAddr, data);
        }
        else if (drv->drvHwPpWriteIntPciReg == NULL)
        {
            CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(funcId, GT_NOT_SUPPORTED);
        }
        else
        {
            rc = prvCpssDrvHwPpDoWriteOpDrv(drv, drv->drvHwPpWriteIntPciReg,
                    GT_FALSE, CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E,
                    regAddr, data);
        }
        if(rc != GT_OK)
        {
            CPSS_LOG_API_EXIT_MAC(funcId, rc);
            return rc;
        }
    }
    PRV_CPSS_DRV_END_HANDLE_PORT_GROUPS_MAC(devNum, portGroupId)

    CPSS_LOG_API_EXIT_MAC(funcId, rc);

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupDebugForcePortGroupId
*
* DESCRIPTION:
*       debug tool --> force port group Id for all operations in the cpssDriver level
* INPUTS:
*       devNum             - The PP to read from.
*       forcedPortGroupId  - The port group Id to force.
*                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*                            --> remove the forcing.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - non-multi port groups device
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupDebugForcePortGroupId
(
    IN GT_U8    devNum,
    IN GT_U32   forcedPortGroupId
)
{
    /* must save the value before call PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(...) */
    GT_U32  copyOfPortGroupId = forcedPortGroupId;

    PRV_CPSS_DRV_CHECK_PP_DEV_MAC(devNum);
    /* check the forcedPortGroupId value */
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, copyOfPortGroupId);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    /* set the new 'debugForcedPortGroupId' */
    prvCpssDrvPpConfig[devNum]->portGroupsInfo.debugForcedPortGroupId =
        forcedPortGroupId;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvHwPpMemoryAccessTraceEnableSet
*
* DESCRIPTION:
*       debug tool --> make debug prints for all memory access in the cpssDriver level
* INPUTS:
*       enable      - GT_TRUE  - enable memory access debug prints
*                     GT_FALSE - disable memory access debug prints is off
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpMemoryAccessTraceEnableSet
(
    IN GT_BOOL    enable
)
{
    memoryAccessTraceOn = enable;
    return GT_OK;
}
