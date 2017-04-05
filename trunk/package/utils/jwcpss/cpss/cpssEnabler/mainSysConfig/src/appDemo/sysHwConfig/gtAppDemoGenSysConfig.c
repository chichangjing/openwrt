/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtAppDemoGenSysConfig.c
*
* DESCRIPTION:
*       General System configuration and initialization control.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:

*       $Revision: 22 $
*
*******************************************************************************/
#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>


/*******************************************************************************
 * Global variables
 ******************************************************************************/

/* becomes TRUE after first initialization of the system, stays TRUE after
 the shutdown */
GT_BOOL systemInitialized = GT_FALSE;

/* appDemoPpConfigList - Holds the Pp's configuration parameters.
   allocate additional entries for multi portgroup devices. Because the PCI/PEX
   scan fill in DB starting from first SW devNum and for all port groups */
APP_DEMO_PP_CONFIG appDemoPpConfigList[APP_DEMO_PP_CONFIG_SIZE_CNS];
/* hold the "system" generic info */
APP_DEMO_SYS_CONFIG_STC appDemoSysConfig = {
    GT_FALSE  /*cpuEtherPortUsed*/
};

/* offset used during HW device ID calculation formula */
GT_U8 appDemoHwDevNumOffset = 0x10;

/*****************************************************************************
* Local variables
******************************************************************************/
#define END_OF_TABLE    0xFFFFFFFF

/* DB to hold the device types that the CPSS support */
extern const struct {
    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    GT_U8                       numOfPorts;
    const CPSS_PP_DEVICE_TYPE   *devTypeArray;
    const CPSS_PORTS_BMP_STC    *defaultPortsBmpPtr;
    const /*CORES_INFO_STC*/void *coresInfoPtr;
    const GT_U32                *activeCoresBmpPtr;
}cpssSupportedTypes[];


/* PEX devices */
PRV_PCI_DEVICE_QUIRKS_ARRAY_MAC


/* Forward declaration of ExMx configuration functions */
#if (defined EX_FAMILY) || (defined MX_FAMILY)
    extern GT_STATUS appDemoExMxLibrariesInit();
    extern GT_STATUS appDemoExMxPpLogicalInit();
    extern GT_STATUS appDemoExMxTrafficEnable();
    extern GT_STATUS appDemoExMxHwPpPhase1Init();
    extern GT_STATUS appDemoExMxHwPpStartInit();
    extern GT_STATUS appDemoExMxHwPpPhase2Init();
    extern GT_STATUS appDemoExMxPpGeneralInit();
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY)*/

/* Forward declaration of ExMxPm configuration functions */
#if (defined EXMXPM_FAMILY)
    extern GT_STATUS appDemoExMxPmLibrariesInit();
    extern GT_STATUS appDemoExMxPmPpLogicalInit();
    extern GT_STATUS appDemoExMxPmTrafficEnable();
    extern GT_STATUS appDemoExMxPmHwPpPhase1Init();
    extern GT_STATUS appDemoExMxPmHwPpStartInit();
    extern GT_STATUS appDemoExMxPmHwPpPhase2Init();
    extern GT_STATUS appDemoExMxPmPpGeneralInit();
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY)*/

#if (defined CHX_FAMILY) || (defined SAL_FAMILY)
/* Forward declaration of Dx configuration functions */
extern GT_STATUS appDemoDxLibrariesInit();
extern GT_STATUS appDemoDxPpLogicalInit();
extern GT_STATUS appDemoDxTrafficEnable();
extern GT_STATUS appDemoDxHwPpPhase1Init();
extern GT_STATUS appDemoDxHwPpStartInit();
extern GT_STATUS appDemoDxHwPpPhase2Init();
extern GT_STATUS appDemoDxPpGeneralInit();
#endif /*(defined CHX_FAMILY) || (defined SAL_FAMILY)*/
/* macro to force casting between 2 functions prototypes */
#define FORCE_FUNC_CAST (void*)


/*****************************************************************************
* Public API implementation
******************************************************************************/

/*******************************************************************************
* getDevFamily
*
* DESCRIPTION:
*       Gets the device family from vendor Id and device Id (read from PCI bus)
*
* INPUTS:
*       pciDevVendorIdPtr - pointer to PCI/PEX device identification data.
*
* OUTPUTS:
*       devFamilyPtr - pointer to CPSS PP device family.
*       isPexPtr     - pointer to is PEX flag:
*                        GT_TRUE - device has PEX interface
*                        GT_FALSE - device has not PEX interface
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS getDevFamily(
    IN GT_PCI_DEV_VENDOR_ID    *pciDevVendorIdPtr,
    OUT CPSS_PP_FAMILY_TYPE_ENT *devFamilyPtr,
    OUT GT_BOOL     *isPexPtr
)
{
    CPSS_PP_DEVICE_TYPE deviceType = pciDevVendorIdPtr->devId << 16 | pciDevVendorIdPtr->vendorId;
    GT_U32                  ii;
    GT_U32                  jj;
    GT_BOOL                 found = GT_FALSE;

    /* get the info about our device */
    ii = 0;
    while (cpssSupportedTypes[ii].devFamily != END_OF_TABLE)
    {
        jj = 0;
        while (cpssSupportedTypes[ii].devTypeArray[jj] != END_OF_TABLE)
        {
            if (deviceType == cpssSupportedTypes[ii].devTypeArray[jj])
            {
                found = GT_TRUE;
                break;
            }
            jj++;
        }

        if (GT_TRUE == found)
        {
            break;
        }
        ii++;
    }

    /* check if deviceType was found */
    if (GT_TRUE == found)
    {
        /* get family type from CPSS DB */
        *devFamilyPtr = cpssSupportedTypes[ii].devFamily;
    }
    else  /* device not managed by CPSS , so check the devId */
    {
        switch (deviceType)
        {
            case CPSS_98FX950_CNS:
                *devFamilyPtr = APPDEMO_FX950_FAMILY_TYPE;
                break;

            default:
                return GT_NOT_SUPPORTED;
        }
    }

    *isPexPtr = GT_FALSE;

    /* loop on the PEX devices */
    for (ii = 0; prvPciDeviceQuirks[ii].pciId != 0xffffffff; ii++)
    {
        if (prvPciDeviceQuirks[ii].pciId == deviceType)
        {
            *isPexPtr = GT_TRUE;
            break;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoSysGetPciInfo
*
* DESCRIPTION:
*       Gets the Pci info for the mini-application usage.
*
* INPUTS:
*       None.
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
GT_STATUS appDemoSysGetPciInfo
(
    GT_VOID
)
{
    GT_PCI_INFO pciInfo;
    GT_U8       devIdx;
    GT_BOOL     isFirst;
    GT_STATUS   status;
    GT_BOOL     isPex;/* is it PEX device */


    devIdx = SYSTEM_DEV_NUM_MAC(0);
    isFirst = GT_TRUE;

    status = gtPresteraGetPciDev(isFirst, &pciInfo);
    if (GT_OK != status)
    {
        return status;
    }

    while (GT_OK == status)
    {
        appDemoPpConfigList[devIdx].channel = CPSS_CHANNEL_PCI_E;
        appDemoPpConfigList[devIdx].devNum  = devIdx;

        osMemCpy(&(appDemoPpConfigList[devIdx].pciInfo),
                 &pciInfo,
                 sizeof(GT_PCI_INFO));

        /* get family */
        status = getDevFamily(&appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId,
                              &appDemoPpConfigList[devIdx].devFamily,
                              &isPex);
        if (GT_OK != status)
        {
            return GT_FAIL;
        }
        appDemoPpConfigList[devIdx].valid = GT_TRUE;

#if (defined LINUX_SIM) || ((!defined _linux) && (!defined _FreeBSD))
        /* Correct Base Addresses for PEX devices */
        if (isPex == GT_TRUE)
        {
            /* Internal registers BAR is in the 0x18 reg and only 16 MSB are used */
            appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr =
                appDemoPpConfigList[devIdx].pciInfo.pciHeaderInfo[6] & 0xFFFF0000;

#ifdef ASIC_SIMULATION
            /* correct Internal PCI base to compensate CPSS Driver +
                0x70000 / 0xf0000 PEX offset  */
            appDemoPpConfigList[devIdx].pciInfo.internalPciBase &= 0xFFFF;
#else
            /* PEX BAR need to be masked because only 12 MSB are used */
            appDemoPpConfigList[devIdx].pciInfo.internalPciBase &= 0xFFF00000;
#endif
        }
#endif

        /* For xCat2 devices the PCI controller is identify by using the device ID,  */
        /* thus the PCI scan recognize the controller as another xCat2 device.       */
        /* Since the PCI base address which is attached to the PCI controller is 0x0 */
        /* we are using this indication to know how to distinguish between the       */
        /* device and the controller, and skip the latter one.                       */
        if( (appDemoPpConfigList[devIdx].devFamily >= CPSS_PP_FAMILY_DXCH_XCAT2_E)  &&
            (appDemoPpConfigList[devIdx].devFamily < CPSS_PP_FAMILY_END_DXCH_E)     &&
            (0x0 == appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr) )
        {
            appDemoPpConfigList[devIdx].valid = GT_FALSE;
        }
        else
        {
            devIdx++;
        }

        isFirst = GT_FALSE;

        status = gtPresteraGetPciDev(isFirst, &pciInfo);
    }
    return GT_OK;
}


/*******************************************************************************
* appDemoSysGetSmiInfo
*
* DESCRIPTION:
*       Gets the SMI info for the mini-application usage.
*
* INPUTS:
*       None.
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
GT_STATUS appDemoSysGetSmiInfo
(
    GT_VOID
)
{
    GT_STATUS   status;
    GT_SMI_INFO smiInfo;
    GT_U8       devIdx;
    GT_BOOL     isFirst;
    GT_PCI_DEV_VENDOR_ID devVendorId;
    GT_BOOL     isPex;/* is it PEX device */

    isFirst = GT_TRUE;

    /* Find first not valid device */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < PRV_CPSS_MAX_PP_DEVICES_CNS; devIdx++)
    {
        if (GT_FALSE == appDemoPpConfigList[devIdx].valid)
        {
            break;
        }
    }

    if (devIdx == PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_FAIL;
    }

    while (GT_OK == gtPresteraGetSmiDev(isFirst, &smiInfo))
    {
        appDemoPpConfigList[devIdx].channel = CPSS_CHANNEL_SMI_E;
        appDemoPpConfigList[devIdx].devNum  = devIdx;

        osMemCpy(&(appDemoPpConfigList[devIdx].smiInfo),
                 &smiInfo,
                 sizeof(GT_SMI_INFO));

        devVendorId.vendorId = smiInfo.smiDevVendorId.vendorId;
        devVendorId.devId = smiInfo.smiDevVendorId.devId;

        /* get family */
        status = getDevFamily(&devVendorId,
                    &appDemoPpConfigList[devIdx].devFamily,
                    &isPex);/* dummy parameter for SMI devices */
        if (GT_OK != status)
        {
            return GT_FAIL;
        }

        appDemoPpConfigList[devIdx].valid = GT_TRUE;
        devIdx++;
        isFirst = GT_FALSE;
    }
    return GT_OK;
}


/*******************************************************************************
* appDemoSysGetTwsiInfo
*
* DESCRIPTION:
*       Gets the SMI info for the mini-application usage.
*
* INPUTS:
*       None.
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
GT_STATUS appDemoSysGetTwsiInfo
(
    GT_VOID
)
{
    return GT_NOT_IMPLEMENTED;
}

/*******************************************************************************
* appDemoSysConfigFuncsGet
*
* DESCRIPTION:
*       Gets the system configuration functions accordingly to given device type.
*
* INPUTS:
*       deviceType  - device type.
*
* OUTPUTS:
*       sysConfigFuncsPtr  - system configuration functions.
*       apiSupportedBmpPtr - (pointer to)bmp of supported type of API to call
*
* RETURNS:
*       GT_OK           -   on success
*       GT_BAD_PTR      -   bad pointer
*       GT_NOT_FOUND    -   device type is unknown
*       GT_FAIL         -   on failure
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoSysConfigFuncsGet
(
    IN  GT_U32                      deviceType,
    OUT APP_DEMO_SYS_CONFIG_FUNCS   *sysConfigFuncsPtr,
    OUT GT_U32                      *apiSupportedBmpPtr
)
{
    CPSS_PP_FAMILY_TYPE_ENT devFamily;
    GT_U32                  ii;
    GT_U32                  jj;
    GT_BOOL                 found = GT_FALSE;


    if (NULL == sysConfigFuncsPtr || apiSupportedBmpPtr == NULL)
    {
        return GT_BAD_PTR;
    }

    /* get the info about our device */
    ii = 0;
    while (cpssSupportedTypes[ii].devFamily != END_OF_TABLE)
    {
        jj = 0;
        while (cpssSupportedTypes[ii].devTypeArray[jj] != END_OF_TABLE)
        {
            if (deviceType == cpssSupportedTypes[ii].devTypeArray[jj])
            {
                found = GT_TRUE;
                break;
            }
            jj++;
        }

        if (GT_TRUE == found)
        {
            break;
        }
        ii++;
    }

    /* check if deviceType was found */
    if (GT_FALSE == found)
    {
        return GT_NOT_FOUND;
    }

    /* get family type */
    devFamily = cpssSupportedTypes[ii].devFamily;
    *apiSupportedBmpPtr = 0;

    /* fill API accordingly to family type */
    if(CPSS_IS_EXMX_FAMILY_MAC(devFamily))
    {
#if (defined EX_FAMILY) || (defined MX_FAMILY)
        /* ExMx API should be used */
        *apiSupportedBmpPtr = APP_DEMO_EXMX_FUNCTIONS_SUPPORT_CNS;
        if(devFamily == CPSS_PP_FAMILY_TIGER_E)
        {
            *apiSupportedBmpPtr |= APP_DEMO_EXMXTG_FUNCTIONS_SUPPORT_CNS;
        }

        sysConfigFuncsPtr->cpssHwPpPhase1Init   = FORCE_FUNC_CAST appDemoExMxHwPpPhase1Init;
        sysConfigFuncsPtr->cpssHwPpStartInit    = FORCE_FUNC_CAST appDemoExMxHwPpStartInit;
        sysConfigFuncsPtr->cpssHwPpPhase2Init   = FORCE_FUNC_CAST appDemoExMxHwPpPhase2Init;
        sysConfigFuncsPtr->cpssLibrariesInit    = FORCE_FUNC_CAST appDemoExMxLibrariesInit;
        sysConfigFuncsPtr->cpssPpLogicalInit    = FORCE_FUNC_CAST appDemoExMxPpLogicalInit;
        sysConfigFuncsPtr->cpssTrafficEnable    = FORCE_FUNC_CAST appDemoExMxTrafficEnable;
        sysConfigFuncsPtr->cpssPpGeneralInit    = FORCE_FUNC_CAST appDemoExMxPpGeneralInit;
#endif /* (defined EX_FAMILY) || (defined MX_FAMILY) */
    }
    else if(CPSS_IS_EXMXPM_FAMILY_MAC(devFamily))
    {
#if (defined EXMXPM_FAMILY)
        /* ExMx API should be used */
        *apiSupportedBmpPtr = APP_DEMO_EXMXPM_FUNCTIONS_SUPPORT_CNS;

        sysConfigFuncsPtr->cpssHwPpPhase1Init   = FORCE_FUNC_CAST appDemoExMxPmHwPpPhase1Init;
        sysConfigFuncsPtr->cpssHwPpStartInit    = FORCE_FUNC_CAST appDemoExMxPmHwPpStartInit;
        sysConfigFuncsPtr->cpssHwPpPhase2Init   = FORCE_FUNC_CAST appDemoExMxPmHwPpPhase2Init;
        sysConfigFuncsPtr->cpssLibrariesInit    = FORCE_FUNC_CAST appDemoExMxPmLibrariesInit;
        sysConfigFuncsPtr->cpssPpLogicalInit    = FORCE_FUNC_CAST appDemoExMxPmPpLogicalInit;
        sysConfigFuncsPtr->cpssTrafficEnable    = FORCE_FUNC_CAST appDemoExMxPmTrafficEnable;
        sysConfigFuncsPtr->cpssPpGeneralInit    = FORCE_FUNC_CAST appDemoExMxPmPpGeneralInit;
#endif /* (defined EXMXPM_FAMILY) */
    }
    else if(CPSS_IS_DXSAL_FAMILY_MAC(devFamily))
    {
#if (defined SAL_FAMILY)
        *apiSupportedBmpPtr = APP_DEMO_DXSAL_FUNCTIONS_SUPPORT_CNS;

        sysConfigFuncsPtr->cpssHwPpPhase1Init   = FORCE_FUNC_CAST appDemoDxHwPpPhase1Init;
        sysConfigFuncsPtr->cpssHwPpStartInit    = FORCE_FUNC_CAST appDemoDxHwPpStartInit;
        sysConfigFuncsPtr->cpssHwPpPhase2Init   = FORCE_FUNC_CAST appDemoDxHwPpPhase2Init;
        sysConfigFuncsPtr->cpssLibrariesInit    = FORCE_FUNC_CAST appDemoDxLibrariesInit;
        sysConfigFuncsPtr->cpssPpLogicalInit    = FORCE_FUNC_CAST appDemoDxPpLogicalInit;
        sysConfigFuncsPtr->cpssTrafficEnable    = FORCE_FUNC_CAST appDemoDxTrafficEnable;
        sysConfigFuncsPtr->cpssPpGeneralInit    = FORCE_FUNC_CAST appDemoDxPpGeneralInit;
#endif /* (defined SAL_FAMILY) */
    }
    else if(CPSS_IS_DXCH_FAMILY_MAC(devFamily))
    {
#if (defined CHX_FAMILY)
        *apiSupportedBmpPtr = APP_DEMO_DXCH_FUNCTIONS_SUPPORT_CNS;

        if(devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            /*ch2,3 ... */
            *apiSupportedBmpPtr |= APP_DEMO_DXCH2_FUNCTIONS_SUPPORT_CNS;
        }

        if(devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
        {
            /*ch3 ...*/
            *apiSupportedBmpPtr |= APP_DEMO_DXCH3_FUNCTIONS_SUPPORT_CNS;
        }

        if(devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            /*xCat ...*/
            *apiSupportedBmpPtr |= APP_DEMO_DXCH_XCAT_FUNCTIONS_SUPPORT_CNS;
        }

        if(devFamily >= CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /*Lion ...*/
            *apiSupportedBmpPtr |= APP_DEMO_DXCH_LION_FUNCTIONS_SUPPORT_CNS;
        }

        sysConfigFuncsPtr->cpssHwPpPhase1Init   = FORCE_FUNC_CAST appDemoDxHwPpPhase1Init;
        sysConfigFuncsPtr->cpssHwPpStartInit    = FORCE_FUNC_CAST appDemoDxHwPpStartInit;
        sysConfigFuncsPtr->cpssHwPpPhase2Init   = FORCE_FUNC_CAST appDemoDxHwPpPhase2Init;
        sysConfigFuncsPtr->cpssLibrariesInit    = FORCE_FUNC_CAST appDemoDxLibrariesInit;
        sysConfigFuncsPtr->cpssPpLogicalInit    = FORCE_FUNC_CAST appDemoDxPpLogicalInit;
        sysConfigFuncsPtr->cpssTrafficEnable    = FORCE_FUNC_CAST appDemoDxTrafficEnable;
        sysConfigFuncsPtr->cpssPpGeneralInit    = FORCE_FUNC_CAST appDemoDxPpGeneralInit;
#endif /* (defined CHX_FAMILY) */
    }

    if((*apiSupportedBmpPtr) == 0)
    {
        /* Unsupported family type */
        return GT_NOT_IMPLEMENTED;
    }

    if(appDemoOnDistributedSimAsicSide != 0)
    {
        sysConfigFuncsPtr->cpssHwPpStartInit = NULL;
        sysConfigFuncsPtr->cpssTrafficEnable = NULL;
        sysConfigFuncsPtr->cpssPpGeneralInit = NULL;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoHwDevNumOffsetSet
*
* DESCRIPTION:
*       Sets value of appDemoHwDevNumOffset used during device HW ID calculation
*       based on device SW ID..
*
* INPUTS:
*       hwDevNumOffset - value to save into appDemoHwDevNumOffset.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoHwDevNumOffsetSet
(
    IN GT_U8 hwDevNumOffset
)
{
    appDemoHwDevNumOffset = hwDevNumOffset;

    return GT_OK;
}

/*******************************************************************************
* appDemoDbByDevNum
*
* DESCRIPTION:
*       get the AppDemo DB entry for the 'devNum'.
*
* INPUTS:
*       devNum - the 'cpss devNum'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the appDemoDb
*
* COMMENTS:
*       None.
*
*******************************************************************************/
APP_DEMO_PP_CONFIG* appDemoDbByDevNum(IN GT_U8 devNum)
{
    static APP_DEMO_PP_CONFIG dummyDbEntry;
    GT_U32  ii;

    APP_DEMO_PP_CONFIG* currentEntryInAppDemoDbPtr = &appDemoPpConfigList[0];
    for(ii = 0 ; ii < APP_DEMO_PP_CONFIG_SIZE_CNS ; ii++,currentEntryInAppDemoDbPtr++)
    {
        if(currentEntryInAppDemoDbPtr->valid == GT_TRUE &&
           currentEntryInAppDemoDbPtr->devNum == devNum)
        {
            return currentEntryInAppDemoDbPtr;
        }
    }

    cpssOsPrintf("appDemoDbByDevNum: Error not found device [%d] \n",devNum);
    /* should not happen */
    cpssOsMemSet(&dummyDbEntry,0,sizeof(dummyDbEntry));
    return &dummyDbEntry;
}

