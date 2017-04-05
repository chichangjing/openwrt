/*******************************************************************************
*              Copyright 2002, GALILEO TECHNOLOGY, LTD.
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
* gtDbDx285-24G4XGConfig.c
*
* DESCRIPTION:
*       Initialization functions for the Cheetah 2 DB-285-24G-4XG boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
/* cascading support */
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgNestVlan.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/
/* Init the SMI interface */
extern GT_STATUS hwIfSmiInitDriver
(
    GT_VOID
);
/* memory allocation */
extern void *malloc(size_t);

/*******************************************************************************
* getUserPorts
*
* DESCRIPTION:
*       get number and list of ports that can be added by user to VLAN
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - The Pp device number
*       unitNum         - unit number
* OUTPUTS:
*       portList          - list of ports numbers
*       num_of_user_ports - number of ports and length of ports list
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#if 0
static GT_STATUS getUserPorts
(
    IN     GT_U8       boardRevId,
    IN     GT_U8       devNum,
    IN     GT_U8       unitNum,
    INOUT  GT_LPORT    *portList,
    INOUT  GT_U8       *num_of_user_ports
)
{
    GT_U8   ii , jj;
    GT_U32   ppCounter;    /* Number of Packet processors. */

    /* check the number of devices */
    ppCounter = 0;
    for(ii = 0; ii < PRV_CPSS_MAX_PP_DEVICES_CNS; ii++)
    {
        if(appDemoPpConfigList[ii].valid == GT_TRUE)
        {
            ppCounter++;
        }
    }

    if (ppCounter > 2)
    {
        return GT_FAIL;
    }
    else if (ppCounter > 1)
    {
        ii = 0;
        jj = 0;

        /* skip cascade ports */
        while(ii < *num_of_user_ports)
        {
            if((portList[ii] == (GT_LPORT)GT_PORT_DEV_2_LPORT(devNum, 24, unitNum)) ||
               (portList[ii] == (GT_LPORT)GT_PORT_DEV_2_LPORT(devNum, 25, unitNum)))
            {
                ii++;
                continue;
            }

            portList[jj] = portList[ii];
            ii++;
            jj++;
        }
        *num_of_user_ports = jj;
    }
    return GT_OK;
}
#endif

/*******************************************************************************
* getBoardInfo
*
* DESCRIPTION:
*       Return the board configuration info.
*
* INPUTS:
*       boardRevId  - The board revision Id.
*
* OUTPUTS:
*       isB2bSystem - GT_TRUE, the system is a B2B system.
*       numOfPp     - Number of Packet processors in system.
*       numOfFa     - Number of Fabric Adaptors in system.
*       numOfXbar   - Number of Crossbar devices in system.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getBoardInfo
(
    IN  GT_U8   boardRevId,
    OUT GT_U8   *numOfPp,
    OUT GT_U8   *numOfFa,
    OUT GT_U8   *numOfXbar,
    OUT GT_BOOL *isB2bSystem
)
{
    GT_STATUS   retVal;                 /* Function return value.       */
    GT_U8       ppCounter;              /* Number of Packet processors. */
    GT_U8       i;                      /* Loop index.                  */
    GT_U8       devNum;                 /* Device number                */
    GT_STATUS   smiSupported;


#if !defined (ASIC_SIMULATION)
#if CH_SMI_SUPPORT
    smiSupported = hwIfSmiInitDriver();
#else
    smiSupported = GT_NOT_SUPPORTED;
#endif
#else
    smiSupported = GT_NOT_SUPPORTED;
#endif

    /* PCI devices must be probed first
     * because there is no way to list all devices on SMI bus
     */
    retVal = appDemoSysGetPciInfo();
    if(retVal != GT_OK && smiSupported == GT_NOT_SUPPORTED)
    {
        return retVal;
    }
    ppCounter = 0;
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            ppCounter++;
        }
    }

    /* If no PCI devices was found then SMI device will be tried */
    if (ppCounter == 0 && smiSupported != GT_NOT_SUPPORTED)
    {
        /* no PCI device found, try SMI */
        devNum  = SYSTEM_DEV_NUM_MAC(0);
        appDemoPpConfigList[devNum].channel = CPSS_CHANNEL_SMI_E;
        appDemoPpConfigList[devNum].devNum  = devNum;
        appDemoPpConfigList[devNum].smiInfo.smiDevVendorId.devId       = 0xD804;
        appDemoPpConfigList[devNum].smiInfo.smiDevVendorId.vendorId    = 0x11ab;
        appDemoPpConfigList[devNum].smiInfo.smiIdSel                   = 16 ;
        appDemoPpConfigList[devNum].valid = GT_TRUE;
        ppCounter = 1;
    }

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    *isB2bSystem = ((boardRevId == 2) ? GT_TRUE : GT_FALSE);


    return GT_OK;
}


/*******************************************************************************
* getPpPhase1Config
*
* DESCRIPTION:
*       Returns the configuration parameters for corePpHwPhase1Init().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devIdx          - The Pp Index to get the parameters for.
*
* OUTPUTS:
*       ppPhase1Params  - Phase1 parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getPpPhase1Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devIdx,
    OUT CPSS_PP_PHASE1_INIT_PARAMS    *phase1Params
)
{
    CPSS_PP_PHASE1_INIT_PARAMS    localPpPh1Config = CPSS_PP_PHASE1_DEFAULT;
    void    *intVecNum;

    localPpPh1Config.devNum = devIdx;

    if (appDemoPpConfigList[devIdx].channel == CPSS_CHANNEL_PCI_E)
    {
       localPpPh1Config.baseAddr =
                                appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
       localPpPh1Config.internalPciBase =
                            appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
       localPpPh1Config.deviceId =
            ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
             (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

#if defined(_linux) && defined(CPU_ARM) && !defined(ASIC_SIMULATION)
       /* the Socrates PCI bus irq number is not correct, so we set the */
       /* intVecNum hard coded.                                         */
       intVecNum = (void*)39;
       localPpPh1Config.intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
       localPpPh1Config.intMask   = (GT_U32)((GT_UINTPTR)intVecNum);
#else
        if(boardRevId == 2)
        {/* RD-DX285-48G-4XG */
            extDrvGetPciIntVec((devIdx == 0) ? GT_PCI_INT_A : GT_PCI_INT_B,
                                &intVecNum);
            localPpPh1Config.intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
            extDrvGetIntMask((devIdx == 0) ? GT_PCI_INT_A : GT_PCI_INT_B,
                                &(localPpPh1Config.intMask));
        }
        else
        {
            extDrvGetPciIntVec(GT_PCI_INT_D,&intVecNum);
            localPpPh1Config.intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
            extDrvGetIntMask(GT_PCI_INT_D,&(localPpPh1Config.intMask));
        }
#endif
       localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PCI_E;

    }
    else
    {
       localPpPh1Config.baseAddr         =
                                   appDemoPpConfigList[devIdx].smiInfo.smiIdSel;
       localPpPh1Config.internalPciBase  = 0x0;
       localPpPh1Config.deviceId         =
        ((appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.devId << 16) |
         (appDemoPpConfigList[devIdx].smiInfo.smiDevVendorId.vendorId));

       localPpPh1Config.intVecNum        = 10; /* GPIO */
       localPpPh1Config.intMask          = 10; /* GPIO */

       localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_SMI_E;
    }

    localPpPh1Config.uplinkCfg = CPSS_PP_NO_UPLINK_E;

    /* retrieve PP Core Clock from HW */
    localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

    osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));

    /* Disable Flow Control for this revision */
    if(boardRevId == 3)
    {
        appDemoPpConfigList[devIdx].flowControlDisable = GT_TRUE;
    }

    return GT_OK;
}


/*******************************************************************************
* configBoardAfterPhase1
*
* DESCRIPTION:
*       This function performs all needed configurations that should be done
*       after phase1.
*
* INPUTS:
*       boardRevId      - The board revision Id.
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
static GT_STATUS configBoardAfterPhase1
(
    IN  GT_U8   boardRevId
)
{
    GT_U8    i;                      /* Loop index.                  */

    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            cpssDxChBrgFdbAAandTAToCpuSet(appDemoPpConfigList[i].devNum,GT_TRUE);
        }
    }
    return GT_OK;
}


/*******************************************************************************
* getPpPhase2Config
*
* DESCRIPTION:
*       Returns the configuration parameters for corePpHwPhase2Init().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       oldDevNum       - The old Pp device number to get the parameters for.
*
* OUTPUTS:
*       ppPhase2Params  - Phase2 parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getPpPhase2Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       oldDevNum,
    OUT CPSS_PP_PHASE2_INIT_PARAMS    *phase2Params
)
{
    CPSS_PP_PHASE2_INIT_PARAMS    localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    CPSS_RX_BUF_INFO_STC            * pRxBufInfo;
    GT_U32                      bufferSize;
    GT_U32                      txHdrBufSize;
    GT_STATUS                    retVal;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = CPSS_98DX285_CNS;
    if(appDemoPpConfigList[oldDevNum].channel == CPSS_CHANNEL_PCI_E)
    {
        localPpPh2Config.baseAddr =
                            appDemoPpConfigList[oldDevNum].pciInfo.pciBaseAddr;
        localPpPh2Config.fuqUseSeparate = GT_TRUE;

    }
    else
    {
        localPpPh2Config.baseAddr =
                            appDemoPpConfigList[oldDevNum].smiInfo.smiIdSel;
        /* Cheetah utilizes the Tx desc for Tx header usage (MarvellTag+Cookie).
           There are 100 Tx buffers in HW.                                   */
        cpssDxChHwTxDescSizeGet(localPpPh2Config.deviceId, &txHdrBufSize);
        bufferSize = txHdrBufSize * 100;
        localPpPh2Config.netIfCfg.txDescBlock     = malloc(bufferSize);
        localPpPh2Config.netIfCfg.txDescBlockSize = bufferSize;
        localPpPh2Config.netIfCfg.rxDescBlock     = 0;
        localPpPh2Config.netIfCfg.rxDescBlockSize = 0;

        pRxBufInfo = &localPpPh2Config.netIfCfg.rxBufInfo;
        pRxBufInfo->allocMethod = CPSS_RX_BUFF_STATIC_ALLOC_E;

        /* There are 100 Rx buffers in HW per queue. */
        bufferSize = RX_BUFF_SIZE_DEF * 100 * 4;
        pRxBufInfo->buffData.staticAlloc.rxBufBlockPtr  = malloc(bufferSize);
        pRxBufInfo->buffData.staticAlloc.rxBufBlockSize = bufferSize;

        localPpPh2Config.auqCfg.auDescBlock       = NULL;
        localPpPh2Config.auqCfg.auDescBlockSize   = 0;

        osMemCpy(phase2Params,&localPpPh2Config,
                                              sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

        return GT_OK;
    }
    retVal = appDemoAllocateDmaMem(localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                                   RX_BUFF_SIZE_DEF,RX_BUFF_ALLIGN_DEF,
                                   TX_DESC_NUM_DEF,AU_DESC_NUM_DEF,
                                   &localPpPh2Config);

    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

    return retVal;
}

/*
    functions :
        cpuMemSpacePrint ,
        cpuMemSpaceByteWrite ,
        cpuMemSpaceWrite

    moved to file:
        <appDemo\sysHwConfig\gtAppDemoPciConfig.c>
*/

/*******************************************************************************
* internal10GPortConfig
*
* DESCRIPTION:
*       Internal function performs all needed configurations of 10G ports.
*
* INPUTS:
*       devNum      - Device Id.
*       portNum     - Port Number.
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
GT_STATUS internal10GPortConfig
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM  port_num
)
{
    GT_STATUS     rc;
/*  next values are from coreCpssDxChPhyPort10GSmiRegisterBasicWrite
    therefore, they are general for all DxCh
*/
/*  IN  GT_U8     phyId          = 0;
    IN  GT_BOOL   useExternalPhy = GT_FALSE;
    IN  GT_U8     phyDev         = 5;
*/

    /* Ports 24, 25 - XENPAK/XFP
       Ports 26, 27 - 1-5m cable
       SETUP Amp = 7+20% / PreEmp = xC Interpolator input 6 bits
       control/Ampl=4/PreEmp=0 */

        /* Amp = 7+20% / PreEmp = XC */
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff28, 5, 0xf030) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff28, 5, 0xf030) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff29, 5, 0xf030) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff29, 5, 0xf030) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff2A, 5, 0xf030) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff2A, 5, 0xf030) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff2B, 5, 0xf030) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff2B, 5, 0xf030) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff44, 5, 0x000c) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }

        /*  Loop Gain=0x0 (Cheetah+/CH2) */
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff2c, 5, 0x2c01) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff2c, 5, 0x2c01) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }

        /*  RX EQUALIZER (CABLE (5m) SETUP - OPTIONAL) */
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff40, 5, 0x003d) ;/*LANE 0*/
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff41, 5, 0x003d) ;/*LANE 1*/
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff42, 5, 0x003d) ;/*LANE 2*/
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff43, 5, 0x003d) ;/*LANE 3*/
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }

        /* XG RX PCS reset (Cheetah/Cheetah+/CH2 ) */
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff34, 5, 0xff00) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff34, 5, 0xff00) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }
        rc = cpssDxChPhyPort10GSmiRegisterWrite(devNum, port_num, 0, GT_FALSE,
                                                0xff34, 5, 0xe000) ;
        if (GT_OK != rc)
        {
            return GT_FAIL;
        }


    return GT_OK;
}

/*******************************************************************************
* cascadePortConfig
*
* DESCRIPTION:
*       Additional configuration for cascading ports
*
* INPUTS:
*       devNum  - Device number.
*       portNum - Cascade port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success,
*       GT_HW_ERROR - HW failure
*       GT_FAIL     - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cascadePortConfig
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum
)
{
    GT_STATUS       rc;

    /* set the MRU of the cascade port to be big enough for DSA tag */
    if (cpssDxChPortMruSet(devNum, portNum, 1536) != GT_OK)
    {
        return GT_FAIL;
    }

    /* configure cascade ports to 12G speed */
    rc = cpssDxChCscdHyperGInternalPortModeSet(devNum,portNum,
                                               CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E);

    return rc;

}

/*******************************************************************************
* configBoardAfterPhase2
*
* DESCRIPTION:
*       This function performs all needed configurations that should be done
*       after phase2.
*
* INPUTS:
*       boardRevId      - The board revision Id.
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
static GT_STATUS configBoardAfterPhase2
(
    IN  GT_U8   boardRevId
)
{
    GT_U32  i;
    GT_U8   ppCounter;

    /* check the number of devices */
    ppCounter = 0;
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; ++i)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            ppCounter++;
        }
    }

    if (ppCounter > 2)
    {
        return GT_FAIL;

    }

    for(i = SYSTEM_DEV_NUM_MAC(0); i < SYSTEM_DEV_NUM_MAC(ppCounter); ++i)
    {
        appDemoPpConfigList[i].internal10GPortConfigFuncPtr = internal10GPortConfig;
        appDemoPpConfigList[i].internalCscdPortConfigFuncPtr = cascadePortConfig;
    }

    if (ppCounter > 1)
    {/* ppCounter == 2 */

        for(i = SYSTEM_DEV_NUM_MAC(0); i < SYSTEM_DEV_NUM_MAC(ppCounter); ++i)
        {
            /* XG ports */
            appDemoPpConfigList[i].numberOf10GPortsToConfigure = 2;
            appDemoPpConfigList[i].ports10GToConfigureArr[0] = 26;
            appDemoPpConfigList[i].ports10GToConfigureArr[1] = 27;

            /* Cascading */
            appDemoPpConfigList[i].numberOfCscdTrunks = 1;

            appDemoPpConfigList[i].numberOfCscdPorts = 2;

            appDemoPpConfigList[i].cscdPortsArr[0].portNum = 24;
            appDemoPpConfigList[i].cscdPortsArr[0].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[i].cscdPortsArr[0].trunkId = APP_DEMO_127_TRUNKS_CNS;

            appDemoPpConfigList[i].cscdPortsArr[1].portNum = 25;
            appDemoPpConfigList[i].cscdPortsArr[1].cscdPortType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;
            appDemoPpConfigList[i].cscdPortsArr[1].trunkId = APP_DEMO_127_TRUNKS_CNS;
            /* CPSS_PORTS_BMP_PORT_SET_MAC(&appDemoPpConfigList[i].cscdPortsBmp, cscdPortNum); */


            appDemoPpConfigList[i].numberOfCscdTargetDevs = 1;

            appDemoPpConfigList[i].cscdTargetDevsArr[0].targetDevNum = (0 == i) ? 1 : 0;
            appDemoPpConfigList[i].cscdTargetDevsArr[0].linkToTargetDev.linkNum = APP_DEMO_127_TRUNKS_CNS;
            appDemoPpConfigList[i].cscdTargetDevsArr[0].linkToTargetDev.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;
        }
    }
    else
    {
        /* Standalone Cheetah 2 device */
        /* Configure XG ports 24/25. Fixes taken from CHEETAH2 EEPROM (DB-DX-24GE-4XG (SLM-609), Dev0) */
        appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].numberOf10GPortsToConfigure = 2;
        appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[0] = 24;
        appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].ports10GToConfigureArr[1] = 25;
    }

    return GT_OK;
}

/*******************************************************************************
* getPpLogicalInitParams
*
* DESCRIPTION:
*       Returns the parameters needed for sysPpLogicalInit() function.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - The Pp device number to get the parameters for.
*
* OUTPUTS:
*       ppLogInitParams - Pp logical init parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getPpLogicalInitParams
(
    IN  GT_U8           boardRevId,
    IN  GT_U8           devNum,
    OUT CPSS_PP_CONFIG_INIT_STC    *ppLogInitParams
)
{
    CPSS_PP_CONFIG_INIT_STC  localPpCfgParams = PP_LOGICAL_CONFIG_DEFAULT;

    localPpCfgParams.maxNumOfIpv4Prefixes = 3920;
    localPpCfgParams.numOfTrunks = APP_DEMO_127_TRUNKS_CNS;

    if (boardRevId == 2)
    {
        /* Ch2 with Vr support case */
        localPpCfgParams.maxNumOfVirtualRouters = 10;
        localPpCfgParams.maxNumOfPceForIpPrefixes = 500;
    }
    else
    {
        /* Ch2 without Vr support case */
        localPpCfgParams.maxNumOfVirtualRouters = 1;
    }

    osMemCpy(ppLogInitParams,&localPpCfgParams,sizeof(CPSS_PP_CONFIG_INIT_STC));
    return GT_OK;
}


/*******************************************************************************
* getTapiLibInitParams
*
* DESCRIPTION:
*       Returns Tapi library initialization parameters.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       libInitParams   - Library initialization parameters.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getTapiLibInitParams
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devNum,
    OUT APP_DEMO_LIB_INIT_PARAMS    *libInitParams
)
{
    APP_DEMO_LIB_INIT_PARAMS  localLibInitParams = LIB_INIT_PARAMS_DEFAULT;

    localLibInitParams.initClassifier            = GT_FALSE;
    localLibInitParams.initIpv6                  = GT_TRUE;

    localLibInitParams.initIpv4Filter            = GT_FALSE;
    localLibInitParams.initIpv4Tunnel            = GT_FALSE;
    localLibInitParams.initMpls                  = GT_FALSE;
    localLibInitParams.initMplsTunnel            = GT_FALSE;
    localLibInitParams.initPcl                   = GT_TRUE;

    osMemCpy(libInitParams,&localLibInitParams,
             sizeof(APP_DEMO_LIB_INIT_PARAMS));
    return GT_OK;
}


/*******************************************************************************
* afterInitBoardConfig
*
* DESCRIPTION:
*       Board specific configurations that should be done after board
*       initialization.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS afterInitBoardConfig
(
    IN  GT_U8       boardRevId
)
{
/* The file is in the SOHO build list, but hwPpSetRegField is not existing
   in the SOHO only build. It's should be commented out code
   for SOHO only builds. */
#if 0
    GT_U8       i;
    GT_U32      regAddr;
#endif
    GT_STATUS   retVal;
    GT_U8       devNum;
    CPSS_DXCH_BRG_VLAN_INFO_STC      vlanInfo;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    CPSS_PORTS_BMP_STC                   portsMembers;
    CPSS_PORTS_BMP_STC                   portsTagging;
    GT_U32 vlanEgressPortNum;         /* vlan or egress port number */
    GT_U32 mruIndex;            /* mru Index */
    GT_U32 ingressPortNum;      /* Ingress port number */

    devNum = appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devNum;

#if 0
    /* change the GIG_PORTS_TYPE reset value in the Port MAC control register */
    /* 0,2. This is a WA added for the Socrates PrPmc CPU. Ports 0,1 should   */
    /* not be applied with this fix (combo ports)                             */
    regAddr = 0xA800200;
    for (i = 2; i < 24; i++)
    {
        /* set the portType field of the Port MAC control register 0 to SGMII */
        retVal = cpssDrvPpHwRegFieldSet(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr, 1, 1, 0);
        if (GT_OK != retVal)
        {
            return GT_FAIL;
        }

        /* set the inBandAnEn field of the Port MAC control register 3 to SGMII */
        retVal = cpssDrvPpHwRegFieldSet(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr + 0xC, 2, 1, 0);
        if (GT_OK != retVal)
        {
            return GT_FAIL;
        }

        regAddr += 0x100;
    }
#endif

    /* Multiplexor settings */
    if(boardRevId == 4)
    {
        cpssOsMemSet(&vlanInfo, 0, sizeof(CPSS_DXCH_BRG_VLAN_INFO_STC));
        cpssOsMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));
        cpssOsMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
        cpssOsMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));

        retVal = cpssDxChBrgVlanTableInvalidate(devNum);
        if(retVal != GT_OK)
        {
            return retVal;
        }

        /* Sets vlan Ether Type mode and vlan range. */
        retVal = cpssDxChBrgVlanEtherTypeSet(devNum, CPSS_VLAN_ETHERTYPE1_E,
                                           0x8900, 0x64);
        if(retVal != GT_OK)
        {
            return retVal;
        }

        for(vlanEgressPortNum = 24; vlanEgressPortNum < 28; vlanEgressPortNum++)
        {
            ingressPortNum = vlanEgressPortNum - 24;
            portsMembers.ports[0] = 1 << vlanEgressPortNum | 1 << ingressPortNum;

            /* Set Vlan 24 with ports 0, 24; Vlan 25 with ports 1, 25;
                   Vlan 26 with ports 2, 26; Vlan 27 with ports 3, 27; */
            retVal = cpssDxChBrgVlanEntryWrite(devNum, 24, &portsMembers,
                                               &portsTagging,
                                               &vlanInfo, &portsTaggingCmd);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            retVal = cpssDxChBrgVlanPortVidSet(devNum, (GT_U8)ingressPortNum,CPSS_DIRECTION_INGRESS_E,
                                              (GT_U16)vlanEgressPortNum);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            retVal = cpssDxChBrgVlanPortVidSet(devNum, (GT_U8)vlanEgressPortNum,CPSS_DIRECTION_INGRESS_E,
                                              (GT_U16)vlanEgressPortNum);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            /* Set VLAN MRU to 10240 bytes.*/
            mruIndex = ingressPortNum + 1;
            retVal = cpssDxChBrgVlanMruProfileIdxSet(devNum,
                                                    (GT_U16)vlanEgressPortNum,
                                                     mruIndex);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            retVal = cpssDxChBrgVlanMruProfileValueSet(devNum, mruIndex, 10240);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            /* Change the MRU size on Ingress ports to 10240 bytes. */
            retVal = cpssDxChPortMruSet(devNum, (GT_U8)ingressPortNum, 10240);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            /* Set Customer port */
            retVal = cpssDxChBrgNestVlanEtherTypeSelectSet(devNum,
                                                        (GT_U8)ingressPortNum,
                                                        CPSS_VLAN_ETHERTYPE1_E);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            retVal = cpssDxChBrgNestVlanEtherTypeSelectSet(devNum,
                                                       (GT_U8)vlanEgressPortNum,
                                                        CPSS_VLAN_ETHERTYPE1_E);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            retVal = cpssDxChBrgNestVlanAccessPortSet(devNum,
                                                     (GT_U8)ingressPortNum,
                                                      GT_TRUE);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            retVal = cpssDxChBrgNestVlanAccessPortSet(devNum,
                                                     (GT_U8)vlanEgressPortNum,
                                                      GT_TRUE);
            if(retVal != GT_OK)
            {
                return retVal;
            }
        }
    }

    return GT_OK;
}


/*******************************************************************************
* gtDbDx28524G4XGBoardReg
*
* DESCRIPTION:
*       Registeration function for the DB-285-24G-4XG board (Cheetah 2 devices).
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       boardCfgFuncs   - Board configuration functions.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtDbDx28524G4XGBoardReg
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
    if(boardCfgFuncs == NULL)
    {
        return GT_FAIL;
    }

    boardCfgFuncs->boardGetInfo                 = getBoardInfo;
    /*boardCfgFuncs->boardGetCoreCfgPh1Params     = getCoreSysCfgPhase1;*/
    boardCfgFuncs->boardGetPpPh1Params          = getPpPhase1Config;
    boardCfgFuncs->boardAfterPhase1Config       = configBoardAfterPhase1;
    /*boardCfgFuncs->boardGetCoreCfgPh2Params     = getCoreSysCfgPhase2;*/
    boardCfgFuncs->boardGetPpPh2Params          = getPpPhase2Config;
    boardCfgFuncs->boardAfterPhase2Config       = configBoardAfterPhase2;
    /*boardCfgFuncs->boardGetSysCfgParams         = getSysConfigParams;*/
    boardCfgFuncs->boardGetPpLogInitParams      = getPpLogicalInitParams;
    boardCfgFuncs->boardGetLibInitParams        = getTapiLibInitParams;
    boardCfgFuncs->boardAfterInitConfig         = afterInitBoardConfig;
    /*boardCfgFuncs->boardGetUserPorts            = getUserPorts;*/

    return GT_OK;
}

