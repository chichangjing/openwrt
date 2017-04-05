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
* gtAppDemoExMxSysConfig.c
*
* DESCRIPTION:
*       System configuration and initialization control for ExMx devices.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:

*       $Revision: 1.1.2.7 $
*
*******************************************************************************/
#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

#include <cpss/generic/init/cpssInit.h>
#include <cpss/extServices/cpssExtServices.h>
#include <cpss/exMx/exMxGen/cpssHwInit/cpssExMxHwInit.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortCtrl.h>

#include <cpss/exMx/exMxGen/config/cpssExMxCfgInit.h>
#include <cpss/exMx/exMxTg/config/cpssExMxTgCfgInit.h>

#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgVlan.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>

#include <cpss/exMx/exMxGen/cos/cpssExMxL2Cos.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortCtrl.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortTx.h>
#include <cpss/generic/port/cpssPortCtrl.h>

#include <cpss/exMx/exMxTg/ip/cpssExMxTgIp.h>



/*******************************************************************************
 * External variables
 ******************************************************************************/

/* appDemoPpConfigList - Holds the Pp's configuration parameters */
extern APP_DEMO_PP_CONFIG appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];


/*******************************************************************************
 * Local usage variables
 ******************************************************************************/

static FUNCP_CPSS_MODULE_INIT cpssInitList[20]; /* Defined at the end of the file. */

static GT_VOID appDemoCoreToCpssPpInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC       *sysConfPtr,
    OUT CPSS_EXMX_PP_CONFIG_INIT_STC  *ppLogicaslInfoPtr
);

static GT_VOID appDemoCoreToCpssPpTgInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC         *sysConfPtr,
    OUT CPSS_EXMXTG_PP_CONFIG_INIT_STC  *tgPpLogicaslInfoPtr
);

static GT_STATUS appDemoConvert2ExMxPhase1Info
(
    IN  GT_U8                              devNum,
    IN  CPSS_PP_PHASE1_INIT_PARAMS         *appCpssPpPhase1ParamsPtr,
    OUT CPSS_EXMX_PP_PHASE1_INIT_INFO_STC  *cpssPpPhase1InfoPtr
);

static GT_STATUS appDemoConvert2ExMxPhase2Info
(
    IN  CPSS_PP_PHASE2_INIT_PARAMS          *appCpssPpPhase2ParamsPtr,
    OUT CPSS_EXMX_PP_PHASE2_INIT_INFO_STC   *cpssPpPhase2InfoPtr
);


/*******************************************************************************
 * Public API implementation
 ******************************************************************************/

/*******************************************************************************
* appDemoExMxHwPpPhase1Init
*
* DESCRIPTION:
*       Implements HW phase 1 initialization.
*
* INPUTS:
*      dev                     -   temp device id, could be changed later
*      coreSysCfgPh1ParamsPtr  -   phase 1 core specific parameters
*      corePpPhase1ParamsPtr   -   PP phase 1 init parameters
*
* OUTPUTS:
*      ppDeviceTypePtr         -   output correct device type
*
* RETURNS:
*       GT_OK       - on success,
*       GT_BAD_PTR  -   on bad pointers
*       GT_FAIL     - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxHwPpPhase1Init
(
    IN  GT_U8                        dev,
    IN  CPSS_PP_PHASE1_INIT_PARAMS  *appCpssPpPhase1ParamsPtr,
    OUT CPSS_PP_DEVICE_TYPE         *ppDevTypePtr
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_EXMX_PP_PHASE1_INIT_INFO_STC   cpssPpPhase1Params;     /* CPSS phase 1 PP params */


    if ((NULL == appCpssPpPhase1ParamsPtr) ||
        (NULL == ppDevTypePtr))
    {
        return GT_BAD_PTR;
    }

    /* fill CPSS Phase1 init parameters */
    rc = appDemoConvert2ExMxPhase1Info(dev,
                                       appCpssPpPhase1ParamsPtr,
                                       &cpssPpPhase1Params);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssExMxHwPpPhase1Init(&cpssPpPhase1Params, ppDevTypePtr);
    return rc;
} /* appDemoExMxHwPpPhase1Init */

/*******************************************************************************
* appDemoExMxHwPpStartInit
*
* DESCRIPTION:
*       Implements PP start initialization.
*
* INPUTS:
*      dev             -   device number
*      fReloadEeprom   -   reload Eeprom flag
*      initDataListPtr -   list of registers to init
*      initDataListLen -   length of registers' list
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
GT_STATUS appDemoExMxHwPpStartInit
(
    IN  GT_U8                       dev,
    IN  GT_BOOL                     fReloadEeprom,
    IN  CPSS_REG_VALUE_INFO_STC     *initDataListPtr,
    IN  GT_U32                      initDataListLen
)
{
    return cpssExMxHwPpStartInit(dev,
                                 fReloadEeprom,
                                 initDataListPtr,
                                 initDataListLen);
} /* appDemoExMxHwPpStartInit */

/*******************************************************************************
* appDemoExMxHwPpPhase2Init
*
* DESCRIPTION:
*       Implements HW phase 2 initialization.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*      oldDev                  -   old device id, used for phase 1
*      coreSysCfgPh2ParamsPtr  -   phase 2 core specific parameters
*      corePpPhase2ParamsPtr   -   PP phase 2 init parameters
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  -   on bad pointer
*       GT_FAIL     - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxHwPpPhase2Init
(
    IN  GT_U8                       oldDev,
    IN  CPSS_PP_PHASE2_INIT_PARAMS  *appCpssPpPhase2ParamsPtr
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_EXMX_PP_PHASE2_INIT_INFO_STC   cpssPpPhase2Info;       /* Cpss phase 2 info */


    if (NULL == appCpssPpPhase2ParamsPtr)
    {
        return GT_BAD_PTR;
    }

    /* Fill CPSS phase2 init structure */
    rc = appDemoConvert2ExMxPhase2Info(appCpssPpPhase2ParamsPtr,
                                       &cpssPpPhase2Info);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssExMxHwPpPhase2Init(oldDev, &cpssPpPhase2Info);
    return rc;
} /* appDemoExMxHwPpPhase2Init */

/*******************************************************************************
* appDemoExMxPpLogicalInit
*
* DESCRIPTION:
*      Implements logical initialization for specific device.
*      Device number is a part of system configuration parameters.
*
* INPUTS:
*      devNum               -   device number
*      logicalConfigParams  -   PP logical configuration parameters
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPpLogicalInit
(
    IN  GT_U8                        devNum,
    IN  CPSS_PP_CONFIG_INIT_STC     *logicalConfigParams
)
{
   GT_STATUS                        rc = GT_OK;
   CPSS_EXMX_PP_CONFIG_INIT_STC     ppLogicaslInfo;     /* CPSS format - ExMx info  */
   CPSS_EXMXTG_PP_CONFIG_INIT_STC   tgPpLogicaslInfo;   /* CPSS format - Tiger info */


   switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
   {
        case CPSS_PP_FAMILY_TIGER_E:
            /* convert core format to specifc CPSS format -- Tiger */
            appDemoCoreToCpssPpTgInfoConvert(logicalConfigParams,
                                             &tgPpLogicaslInfo);

            /* call CPSS to do Pp logical init */
            rc = cpssExMxTgCfgPpLogicalInit(devNum, &tgPpLogicaslInfo);
            break;
        default:
            /* convert core format to specific CPSS format -- ExMx */
            appDemoCoreToCpssPpInfoConvert(logicalConfigParams,
                                           &ppLogicaslInfo);

            /* call CPSS to do Pp logical init */
            rc = cpssExMxCfgPpLogicalInit(devNum, &ppLogicaslInfo);
            break;
    }
    return rc;
} /* appDemoExMxPpLogicalInit */

/*******************************************************************************
* appDemoExMxLibrariesInit
*
* DESCRIPTION:
*      Initialize all CPSS libraries for specific device.
*      It's entry point where all required CPSS libraries are initialized.
*
* INPUTS:
*      dev                 -   device number
*      libInitParamsPtr    -   initialization parameters
*      sysConfigParamPtr   -   system configuration parameters
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
GT_STATUS appDemoExMxLibrariesInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParams,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParams
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       i;          /* Per function loop counter    */


    /* for given device id init all cpss modules */
    i = 0;
    while ((cpssInitList[i] != 0) && (GT_OK == rc))
    {
        /* for given device id init cpss module */
        rc = cpssInitList[i](dev, libInitParams, sysConfigParams);
        i++;
    }
    return rc;
} /* appDemoExMxLibrariesInit */

/*******************************************************************************
* appDemoExMxTrafficEnable
*
* DESCRIPTION:
*      Enable traffic for specific device.
*      Enable ports and device.
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxTrafficEnable
(
    IN  GT_U8   dev
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       port;


    /* for given device id loop on ports */
    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
    {
        rc = cpssExMxPortEnable(dev, port, GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    /* enable device */
    rc = cpssExMxCfgDevEnable(dev, GT_TRUE);
    return rc;
} /* appDemoExMxTrafficEnable */

/*******************************************************************************
* appDemoExMxPpGeneralInit
*
* DESCRIPTION:
*      Implements general initialization for specific device.
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPpGeneralInit
(
    IN  GT_U8   dev
)
{
    GT_STATUS              rc = GT_OK;
    CPSS_PORTS_BMP_STC     portsMembers;
    CPSS_PORTS_BMP_STC     portsTagging;
    CPSS_VLAN_INFO_STC     cpssVlanInfo;   /* cpss vlan info format    */
    GT_U8                  port;           /* current port number      */
    GT_U16                 vid;            /* default VLAN id          */
    CPSS_PORT_MRU_SIZE_ENT mruSize;


    /*********************************************************/
    /* Set FDB learning mode                                 */
    /*********************************************************/
    rc = cpssExMxBrgFdbMacVlanLookupModeSet(dev,CPSS_IVL_E);
    if (rc != GT_OK)
       return rc;


    /*********************************************************/
    /* Port Configuration                                    */
    /*********************************************************/

    /* Jambo frame support */
    mruSize = CPSS_PORT_MRU_1522_E; /* default */


    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfVirtPorts; port++)
    {
        rc = cpssExMxCosPortOverrideTcEnable(dev, port, GT_FALSE);
        if(rc != GT_OK)
            return rc;
    }

    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
    {
        rc = cpssExMxPortDuplexModeSet(dev, port, CPSS_PORT_FULL_DUPLEX_E);
        if(rc != GT_OK)
            return rc;

        if(PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType != PRV_CPSS_PORT_XG_E)
        {
            rc = cpssExMxPortDuplexAutoNegEnable(dev, port, GT_TRUE);
            if(rc != GT_OK)
                return rc;

            rc = cpssExMxPortFlowCntrlAutoNegEnable(dev, port, GT_FALSE, GT_TRUE);
            if(rc != GT_OK)
                return rc;

            rc = cpssExMxPortSpeedAutoNegEnable(dev, port, GT_TRUE);
            if(rc != GT_OK)
                return rc;
        }

        rc = cpssExMxPortFlowControlEnable(dev, port, GT_TRUE);
        if(rc != GT_OK)
            return rc;

        rc = cpssExMxPortPeriodicFcEnable(dev, port, GT_FALSE);
        if(rc != GT_OK)
            return rc;

        rc = cpssExMxPortForceLinkDownEnable(dev, port, GT_FALSE);
        if(rc != GT_OK)
            return rc;

        rc = cpssExMxPortMruSet(dev,port, mruSize);
        if(rc != GT_OK)
            return rc;

        rc = cpssExMxPortCrcCheckEnableSet(dev, port, GT_TRUE);
        if(rc != GT_OK)
            return rc;

        rc = cpssExMxPortTxWatchdogEnable(dev, port, GT_TRUE, 500);
        if(rc != GT_OK)
            return rc;
    }



    /*********************************************************/
    /* Default VLAN configuration: VLAN 1 contains all ports */
    /*********************************************************/

    /* default VLAN id is 1 */
    vid = 1;

    /* Fill Vlan info */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));
    cpssVlanInfo.hasLocals = GT_TRUE;

    /* Fill ports and tagging members */
    osMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));

    /* set all ports as VLAN members */
    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfVirtPorts; port++)
    {
        /* set the port as member of vlan */
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembers, port);


        /* Set port pvid */
        rc = cpssExMxBrgVlanPortPvidSet(dev, port, vid);
        if(rc != GT_OK)
             return rc;

    }

    /* Write default VLAN entry */
    rc = cpssExMxBrgVlanEntryWrite(dev, vid,
                                   &portsMembers,
                                   &portsTagging,
                                   &cpssVlanInfo);

    return rc;
} /* appDemoExMxPpGeneralInit */


/*******************************************************************************
 * Private function implementation
 ******************************************************************************/

/*******************************************************************************
* appDemoCoreToCpssPpInfoConvert
*
* DESCRIPTION:
*       convert application format to CPSS format for Tiger "PP logical init"
*
* INPUTS:
*       ppConfPtr    - The PP logical configuration parameters
*
* OUTPUTS:
*      tgPpLogicaslInfoPtr - CPSS logical info format for Tiger device
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID appDemoCoreToCpssPpTgInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC         *ppConfigPtr,
    OUT CPSS_EXMXTG_PP_CONFIG_INIT_STC  *tgPpLogicaslInfoPtr
)
{

    /* interfaces */
    tgPpLogicaslInfoPtr->maxNumOfInlifs = ppConfigPtr->maxNumOfLifs ;

    /* traffic condition */
    tgPpLogicaslInfoPtr->maxNumOfPolicerEntries = ppConfigPtr->maxNumOfPolicerEntries;
    tgPpLogicaslInfoPtr->policerConformCountEn = ppConfigPtr->policerConformCountEn;

    /* MPLS */
    tgPpLogicaslInfoPtr->maxNumOfNhlfe = ppConfigPtr->maxNumOfNhlfe;
    tgPpLogicaslInfoPtr->maxNumOfMplsIfs = ppConfigPtr->maxNumOfMplsIfs;

    /* PCL */
    tgPpLogicaslInfoPtr->maxNumOfPclAction = ppConfigPtr->maxNumOfPclAction;

    /* PCL */
    tgPpLogicaslInfoPtr->pclActionSize = ppConfigPtr->pclActionSize;

    /* Bridge */
    tgPpLogicaslInfoPtr->maxVid = ppConfigPtr->maxVid;
    tgPpLogicaslInfoPtr->maxMcGroups = ppConfigPtr->maxMcGroups;
    /* tgPpLogicaslInfoPtr->tunnelTableSize  is the remining memory after
       reducing 2 previous tables */

    /* IP Unicast */
    tgPpLogicaslInfoPtr->maxNumOfVirtualRouters = ppConfigPtr->maxNumOfVirtualRouters;
    tgPpLogicaslInfoPtr->maxNumOfIpv4NextHop = ppConfigPtr->maxNumOfIpNextHop;

    /* IPv4 Unicast */
    tgPpLogicaslInfoPtr->maxNumOfIpv4Prefixes = ppConfigPtr->maxNumOfIpv4Prefixes;

    /* IPv4/IPv6 Multicast */
    tgPpLogicaslInfoPtr->maxNumOfIpv4McEntries = ppConfigPtr->maxNumOfIpv4McEntries;
    tgPpLogicaslInfoPtr->maxNumOfMll = ppConfigPtr->maxNumOfMll;

    /* IPv6 Unicast */
    tgPpLogicaslInfoPtr->maxNumOfIpv6Prefixes = ppConfigPtr->maxNumOfIpv6Prefixes;

    /* IPv6 Multicast */
    tgPpLogicaslInfoPtr->maxNumOfIpv6McGroups = ppConfigPtr->maxNumOfIpv6McGroups;

    /* Tunnels */
    tgPpLogicaslInfoPtr->maxNumOfTunnelEntries = ppConfigPtr->maxNumOfTunnelEntries;

    /* ipv4 tunnel termination */
    tgPpLogicaslInfoPtr->maxNumOfIpv4TunnelTerms = ppConfigPtr->maxNumOfIpv4TunnelTerms;

    /* inlifs */
    tgPpLogicaslInfoPtr->vlanInlifEntryType = ppConfigPtr->vlanInlifEntryType;

    /* if GT_FALSE - no division of memory pools for the use of different IP versions */
    tgPpLogicaslInfoPtr->ipMemDivisionOn = ppConfigPtr->ipMemDivisionOn;

    /* devision of memory between IPv4 and IPv6 prefixes */
    tgPpLogicaslInfoPtr->ipv6MemShare = ppConfigPtr->ipv6MemShare;

} /* appDemoCoreToCpssPpTgInfoConvert */

/*******************************************************************************
* appDemoCoreToCpssPpInfoConvert
*
* DESCRIPTION:
*       convert application format to CPSS format for ExMX "PP logical init"
*
* INPUTS:
*       ppConfPtr    - The PP logical configuration parameters
*
* OUTPUTS:
*      ppLogicaslInfoPtr - CPSS format for ExMX device.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID appDemoCoreToCpssPpInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC       *ppConfigPtr,
    OUT CPSS_EXMX_PP_CONFIG_INIT_STC  *ppLogicaslInfoPtr
)
{


    /* interfaces */
    ppLogicaslInfoPtr->maxNumOfInlifs = ppConfigPtr->maxNumOfLifs;


    /* traffic condition */
    ppLogicaslInfoPtr->maxNumOfPolicerEntries = ppConfigPtr->maxNumOfPolicerEntries;
    ppLogicaslInfoPtr->policerConformCountEn = ppConfigPtr->policerConformCountEn;

    /* MPLS */
    ppLogicaslInfoPtr->maxNumOfNhlfe = ppConfigPtr->maxNumOfNhlfe;
    ppLogicaslInfoPtr->maxNumOfMplsIfs = ppConfigPtr->maxNumOfMplsIfs;

    /* PCL */
    ppLogicaslInfoPtr->maxNumOfPclAction = ppConfigPtr->maxNumOfPclAction;


    /* PCL */
    ppLogicaslInfoPtr->pclActionSize = ppConfigPtr->pclActionSize;

    /* Bridge */
    ppLogicaslInfoPtr->maxVid = ppConfigPtr->maxVid;
    ppLogicaslInfoPtr->maxMcGroups = ppConfigPtr->maxMcGroups;

    /* IP Unicast */
    ppLogicaslInfoPtr->maxNumOfVirtualRouters = ppConfigPtr->maxNumOfVirtualRouters;
    ppLogicaslInfoPtr->maxNumOfIpv4NextHop = ppConfigPtr->maxNumOfIpNextHop;

    /* IPv4 Unicast */
    ppLogicaslInfoPtr->maxNumOfIpv4Prefixes = ppConfigPtr->maxNumOfIpv4Prefixes;

    /* IPv4/IPv6 Multicast */
    ppLogicaslInfoPtr->maxNumOfIpv4McEntries = ppConfigPtr->maxNumOfIpv4McEntries;
    ppLogicaslInfoPtr->maxNumOfMll = ppConfigPtr->maxNumOfMll;

    /* IPv6 Unicast */
    ppLogicaslInfoPtr->maxNumOfIpv6Prefixes = ppConfigPtr->maxNumOfIpv6Prefixes;

    /* IPv6 Multicast */
    ppLogicaslInfoPtr->maxNumOfIpv6McGroups = ppConfigPtr->maxNumOfIpv6McGroups;

    /* Tunnels */
    ppLogicaslInfoPtr->maxNumOfTunnelEntries = ppConfigPtr->maxNumOfTunnelEntries;

    /* ipv4 tunnel termination */
    ppLogicaslInfoPtr->maxNumOfIpv4TunnelTerms = ppConfigPtr->maxNumOfIpv4TunnelTerms;

    /* inlifs */
    ppLogicaslInfoPtr->vlanInlifEntryType = ppConfigPtr->vlanInlifEntryType;

    /* if GT_FALSE - no division of memory pools for the use of different IP versions */
    ppLogicaslInfoPtr->ipMemDivisionOn = ppConfigPtr->ipMemDivisionOn;

    /* devision of memory between IPv4 and IPv6 prefixes*/
    ppLogicaslInfoPtr->ipv6MemShare = ppConfigPtr->ipv6MemShare;

} /* appDemoCoreToCpssPpInfoConvert */

/*******************************************************************************
* appDemoConvert2ExMxPhase1Info
*
* DESCRIPTION:
*       Set CPSS info from the core format.
*
* INPUTS:
*       devNum                      -   The PP's device number to be initialized.
*       appCpssPpPhase1ParamsPtr    -   PP phase 1 configuration parameters.
*
* OUTPUTS:
*       cpssPpPhase1InfoPtr - CPSS PP phase 1 configuration parameters.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on bad pointers
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function converts HW data into CPSS phase 1 format.
*
*******************************************************************************/
static GT_STATUS appDemoConvert2ExMxPhase1Info
(
    IN  GT_U8                               devNum,
    IN  CPSS_PP_PHASE1_INIT_PARAMS          *appCpssPpPhase1ParamsPtr,
    OUT CPSS_EXMX_PP_PHASE1_INIT_INFO_STC   *cpssPpPhase1InfoPtr
)
{
    GT_STATUS   rc = GT_OK;

    if ((NULL == appCpssPpPhase1ParamsPtr) ||
        (NULL == cpssPpPhase1InfoPtr))
    {
        return GT_BAD_PTR;
    }

    osMemSet(cpssPpPhase1InfoPtr, 0, sizeof(*cpssPpPhase1InfoPtr));

    cpssPpPhase1InfoPtr->devNum          = devNum;
    cpssPpPhase1InfoPtr->busBaseAddr     = appCpssPpPhase1ParamsPtr->baseAddr;
    cpssPpPhase1InfoPtr->internalPciBase = appCpssPpPhase1ParamsPtr->internalPciBase;
    cpssPpPhase1InfoPtr->intVecNum       = appCpssPpPhase1ParamsPtr->intVecNum;
    cpssPpPhase1InfoPtr->intMask         = appCpssPpPhase1ParamsPtr->intMask;
    cpssPpPhase1InfoPtr->coreClock       = appCpssPpPhase1ParamsPtr->coreClk;

    osMemCpy(&cpssPpPhase1InfoPtr->flowDramCfg,
             &appCpssPpPhase1ParamsPtr->flowDramCfg,
             sizeof(appCpssPpPhase1ParamsPtr->flowDramCfg));

    osMemCpy(&cpssPpPhase1InfoPtr->bufferDramCfg,
             &appCpssPpPhase1ParamsPtr->bufferDramCfg,
             sizeof(appCpssPpPhase1ParamsPtr->bufferDramCfg));

    osMemCpy(&cpssPpPhase1InfoPtr->bufferMgmtCfg,
             &appCpssPpPhase1ParamsPtr->bufferMgmtCfg,
             sizeof(appCpssPpPhase1ParamsPtr->bufferMgmtCfg));

    cpssPpPhase1InfoPtr->uplinkCfg          = appCpssPpPhase1ParamsPtr->uplinkCfg;
    cpssPpPhase1InfoPtr->bufferDram         = appCpssPpPhase1ParamsPtr->bufferDram;
    cpssPpPhase1InfoPtr->flowDram           = appCpssPpPhase1ParamsPtr->flowDram;
    cpssPpPhase1InfoPtr->mngInterfaceType   = appCpssPpPhase1ParamsPtr->mngInterfaceType;
    cpssPpPhase1InfoPtr->routingSramCfgType = appCpssPpPhase1ParamsPtr->routingSramCfgType;
    cpssPpPhase1InfoPtr->numOfSramsCfg      = appCpssPpPhase1ParamsPtr->numOfSramsCfg;

    osMemCpy(cpssPpPhase1InfoPtr->sramsCfg,
             appCpssPpPhase1ParamsPtr->sramsCfg,
             sizeof(appCpssPpPhase1ParamsPtr->sramsCfg));

    cpssPpPhase1InfoPtr->maxBufSizeMode = appCpssPpPhase1ParamsPtr->maxBufSize;
    cpssPpPhase1InfoPtr->ppHAState      = appCpssPpPhase1ParamsPtr->sysHAState;

    return rc;
} /* appDemoConvert2ExMxPhase1Info */

/*******************************************************************************
* appDemoConvert2ExMxPhase2Info
*
* DESCRIPTION:
*       Convert core phase 2 info to CPSS phase 2 info.
*
* INPUTS:
*       appCpssPpPhase2ParamsPtr    -   PP phase 2 configuration parameters.
*
* OUTPUTS:
*       cpssPpPhase2InfoPtr         - CPSS PP phase 2 configuration parameters.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on bad pointers
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function converts HW data into CPSS phase 2 format.
*
*******************************************************************************/
static GT_STATUS appDemoConvert2ExMxPhase2Info
(
    IN  CPSS_PP_PHASE2_INIT_PARAMS          *appCpssPpPhase2ParamsPtr,
    OUT CPSS_EXMX_PP_PHASE2_INIT_INFO_STC   *cpssPpPhase2InfoPtr
)
{
    GT_STATUS               rc = GT_OK;
    CPSS_RX_BUF_INFO_STC    *cpssRxBufInfoPtr;  /* pointer to cpss rx buf format    */
    CPSS_RX_BUF_INFO_STC    *pssRxBufInfoPtr;   /* pointer to cpss rx buf format     */


    if ((NULL == appCpssPpPhase2ParamsPtr) ||
        (NULL == cpssPpPhase2InfoPtr))
    {
        return GT_BAD_PTR;
    }

    osMemSet(cpssPpPhase2InfoPtr, 0, sizeof(*cpssPpPhase2InfoPtr));

    cpssPpPhase2InfoPtr->newDevNum                  = appCpssPpPhase2ParamsPtr->devNum;
    cpssPpPhase2InfoPtr->netIfCfg.txDescBlock       = appCpssPpPhase2ParamsPtr->netIfCfg.txDescBlock;
    cpssPpPhase2InfoPtr->netIfCfg.txDescBlockSize   = appCpssPpPhase2ParamsPtr->netIfCfg.txDescBlockSize;
    cpssPpPhase2InfoPtr->netIfCfg.rxDescBlock       = appCpssPpPhase2ParamsPtr->netIfCfg.rxDescBlock;
    cpssPpPhase2InfoPtr->netIfCfg.rxDescBlockSize   = appCpssPpPhase2ParamsPtr->netIfCfg.rxDescBlockSize;

    cpssRxBufInfoPtr    = &cpssPpPhase2InfoPtr->netIfCfg.rxBufInfo;
    pssRxBufInfoPtr     = &appCpssPpPhase2ParamsPtr->netIfCfg.rxBufInfo;

    cpssRxBufInfoPtr->allocMethod = pssRxBufInfoPtr->allocMethod;
    osMemCpy(cpssRxBufInfoPtr->bufferPercentage,
             pssRxBufInfoPtr->bufferPercentage,
             sizeof(pssRxBufInfoPtr->bufferPercentage));

    cpssRxBufInfoPtr->rxBufSize     = appCpssPpPhase2ParamsPtr->rxBufSize;
    cpssRxBufInfoPtr->headerOffset  = appCpssPpPhase2ParamsPtr->headerOffset;

    if (CPSS_RX_BUFF_DYNAMIC_ALLOC_E == cpssRxBufInfoPtr->allocMethod)
    {
        cpssRxBufInfoPtr->buffData.dynamicAlloc.mallocFunc =
            (GT_VOID*)pssRxBufInfoPtr->buffData.dynamicAlloc.mallocFunc;

        cpssRxBufInfoPtr->buffData.dynamicAlloc.numOfRxBuffers =
            pssRxBufInfoPtr->buffData.dynamicAlloc.numOfRxBuffers;
    }
    else if (CPSS_RX_BUFF_STATIC_ALLOC_E == cpssRxBufInfoPtr->allocMethod)
    {
        cpssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr =
            pssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr;
        cpssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize =
            pssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize;
    }

    cpssPpPhase2InfoPtr->auqCfg.auDescBlock     = appCpssPpPhase2ParamsPtr->auqCfg.auDescBlock;
    cpssPpPhase2InfoPtr->auqCfg.auDescBlockSize = appCpssPpPhase2ParamsPtr->auqCfg.auDescBlockSize;

    return rc;
} /* appDemoConvert2ExMxPhase2Info */

/*******************************************************************************
 * CPSS modules initialization routines
 ******************************************************************************/

#include <cpss/exMx/exMxGen/logicalIf/cpssExMxInlif.h>
static GT_STATUS prvInlifLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS   rc;
    GT_U8       port;
    GT_U32      inlifHwData[4];

    cpssOsMemSet(inlifHwData, 0, sizeof(inlifHwData));

    /* set all port inlif to defualt value (all zero) */
    for (port = 0 ; port < PRV_CPSS_PP_MAC(dev)->numOfVirtPorts ; port++)
    {
        rc = cpssExMxInlifPortEntrySet(dev,port,inlifHwData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

#ifdef IMPL_BRIDGE
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgVlan.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgStp.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgMc.h>

static GT_STATUS prvBridgeLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U8     port;

    if(libInitParamsPtr->initBridge == GT_FALSE)
        return GT_OK;

    /* Init VLAN */
    rc = cpssExMxBrgVlanInit(dev);
    if( GT_OK != rc)
        return rc;

    rc = cpssExMxBrgStpInit(dev);
    if( GT_OK != rc)
        return rc;

    rc = cpssExMxBrgFdbInit(dev);
    if( GT_OK != rc)
        return rc;
    /* Set lookUp mode and lookup length. */
    rc = cpssExMxBrgFdbMacAddrLookupModeSet(dev, CPSS_MAC_SQN_VLAN_SQN_E);
    if( GT_OK != rc)
        return rc;

    rc = cpssExMxBrgFdbMaxLookupLenSet(dev, 4/*lookupLen*/);
    if( GT_OK != rc)
        return rc;


    rc = cpssExMxBrgMcInit(dev);
    if( GT_OK != rc)
        return rc;

    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfVirtPorts; port++)
    {

        rc = cpssExMxBrgFdbPortAuToCpuSet(dev, port, CPSS_MAC_AU_MESSAGE_SA_SECURITY_E, GT_TRUE);
        if( GT_OK != rc)
            return rc;

        /*  Set Port Learn State to CPSS_LOCK_FRWRD_E which is:
            A. Locked - Automatic disabled
            B. LockePortCmd - Do not drop (forward as usual). */
        rc = cpssExMxBrgFdbPortLearnSet(dev, port, GT_FALSE, CPSS_LOCK_FRWRD_E);
        if( GT_OK != rc)
            return rc;
    }

    return rc;
}
#endif


#ifdef IMPL_MIRROR
#include <cpss/exMx/exMxGen/mirror/cpssExMxMirror.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgEgrFlt.h>

static GT_STATUS prvMirrorLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;
    GT_U8     port;

    if(libInitParamsPtr->initMirror == GT_FALSE)
        return GT_OK;

    rc = cpssExMxMirrorInit(dev);
    if( GT_OK != rc)
        return rc;

    /* disable (unknown) unicast egress filtering for every port in device
     * to support mirroring
     */
    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfVirtPorts; port++)
    {
       /* call the disable egress filtering per port  */
       rc = cpssExMxBrgPortEgrFltUnkEnable(dev, port, GT_FALSE);
       if (rc != GT_OK)
           return rc;
    }

    /* Disable VLAN Egress Filtering on bridged known unicast packets on devNum */
    rc = cpssExMxBrgVlanEgressFilteringEnable(dev, GT_FALSE);
    if (rc != GT_OK)
        return rc;



    return GT_OK;
}
#endif


#ifdef IMPL_NETIF
#include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>

static GT_STATUS prvNetIfLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initNetworkIf == GT_FALSE)
        return GT_OK;

    rc = cpssExMxNetIfInit(dev);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif


#ifdef IMPL_PCL
#include <cpss/exMx/exMxGen/pcl/cpssExMxPcl.h>

static GT_STATUS prvPclLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPcl == GT_FALSE)
        return GT_OK;

    rc = cpssExMxPclInit(dev);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_I2C
#include <cpss/exMx/exMxGen/i2c/cpssExMxI2c.h>

static GT_STATUS prvI2cLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initI2c == GT_FALSE)
        return GT_OK;

    rc = cpssExMxI2cInit(dev);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_POLICER
#include <cpss/exMx/exMxGen/policer/cpssExMxPolicer.h>

static GT_STATUS prvPolicerLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPolicer == GT_FALSE)
        return GT_OK;

    rc = cpssExMxPolicerInit(dev);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_PORT
#include <cpss/exMx/exMxGen/port/cpssExMxPortBufMg.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortStat.h>
#include <cpss/exMx/exMxGen/phy/cpssExMxPhySmi.h>

static GT_STATUS prvPortLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPort == GT_FALSE)
        return GT_OK;

    rc = cpssExMxPortStatInit(dev);
    if( GT_OK != rc)
        return rc;

    rc = cpssExMxPhyPortSmiInit(dev);
    if( GT_OK != rc)
        return rc;

    rc = cpssExMxPortBufMgInit(dev);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_IP
#include <cpss/exMx/exMxGen/ip/cpssExMxIp.h>
#include <cpss/exMx/exMxGen/ip/cpssExMxIpTypes.h>
#include <cpss/exMx/exMxGen/ipLpmEngine/cpssExMxIpLpmTypes.h>
#include <cpss/exMx/exMxGen/ipLpmEngine/cpssExMxIpLpm.h>

static GT_STATUS prvIpLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS                               rc;
    CPSS_EXMX_IP_MEM_CFG_STC                *shadowCfgArray;
    CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC         ucRouteEntry;
    CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC         mcRouteEntry;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC    defUcNextHopPointer;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC    defMcRoutePointer;
    GT_U32                                  lpmDbId = 0;
    GT_U32                                  vrId = 0;
    GT_U32                                  i;

    static GT_BOOL lpmDbInitialized = GT_FALSE;     /* keeps if LPM DB was created */
    static GT_BOOL vrInitialized = GT_FALSE;        /* keeps if virtual router was created */

    /*************************************************/
    /* create LPM DB if it wasn't previously created */
    /*************************************************/

    if (lpmDbInitialized == GT_FALSE)
    {
        /* set shadow configuration array */
        shadowCfgArray = osMalloc(sizeof(CPSS_EXMX_IP_MEM_CFG_STC) *
                                  sysConfigParamsPtr->lpmDbNumOfMemCfg);
        if (shadowCfgArray == NULL)
            return GT_OUT_OF_CPU_MEM;
        for (i = 0 ; i < sysConfigParamsPtr->lpmDbNumOfMemCfg ; i++)
        {
            switch (sysConfigParamsPtr->lpmDbMemCfgArray[i].routingSramCfgType)
            {
            case APP_DEMO_CPSS_EXMX_TWIST_INTERNAL_E:
                shadowCfgArray[i].routingSramCfgType = CPSS_TWIST_INTERNAL_E;
                break;
            case APP_DEMO_CPSS_EXMX_TWIST_EXTERNAL_E:
                shadowCfgArray[i].routingSramCfgType = CPSS_TWIST_EXTERNAL_E;
                break;
            case APP_DEMO_CPSS_EXMX_TWO_EXTERNAL_250MHZ_E:
                shadowCfgArray[i].routingSramCfgType = CPSS_TWO_EXTERNAL_250MHZ_E;
                break;
            case APP_DEMO_CPSS_EXMX_TWO_EXTERNAL_300MHZ_E:
                shadowCfgArray[i].routingSramCfgType = CPSS_TWO_EXTERNAL_300MHZ_E;
                break;
            case APP_DEMO_CPSS_EXMX_DROP_IN_CAPACITY_E:
                shadowCfgArray[i].routingSramCfgType = CPSS_DROP_IN_CAPACITY_E;
                break;
            case APP_DEMO_CPSS_EXMX_DROP_IN_SPEED_E:
                shadowCfgArray[i].routingSramCfgType = CPSS_DROP_IN_SPEED_E;
                break;
            case APP_DEMO_CPSS_EXMX_TWO_INTERNAL_E:
                shadowCfgArray[i].routingSramCfgType = CPSS_TWO_INTERNAL_E;
                break;
            case APP_DEMO_CPSS_EXMX_ONE_INTERNAL_E:
                shadowCfgArray[i].routingSramCfgType = CPSS_ONE_INTERNAL_E;
                break;
            case APP_DEMO_CPSS_EXMX_NRAM_NOT_EXISTS_E:
                shadowCfgArray[i].routingSramCfgType = CPSS_NRAM_NOT_EXISTS_E;
                break;
            default:
                return GT_BAD_PARAM;
            }
            shadowCfgArray[i].numOfSramsSizes =
                sysConfigParamsPtr->lpmDbMemCfgArray[i].numOfSramsSizes;
            cpssOsMemCpy(&shadowCfgArray[i].sramsSizeArray,
                         &sysConfigParamsPtr->lpmDbMemCfgArray[i].sramsSizeArray,
                         sizeof(CPSS_SRAM_SIZE_ENT) * shadowCfgArray[i].numOfSramsSizes);
        }

        /* now create the lpm db */
        rc = cpssExMxIpLpmDBCreate(lpmDbId,shadowCfgArray,
                                   sysConfigParamsPtr->lpmDbNumOfMemCfg,
                                   CPSS_IP_PROTOCOL_IPV4V6_E,
                                   sysConfigParamsPtr->ipv6MemShare,
                                   sysConfigParamsPtr->maxNumOfVirtualRouters);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* mark the lpm db as created */
        lpmDbInitialized = GT_TRUE;
    }

    /****************************************/
    /* set route entry 1 as default unicast */
    /****************************************/
    cpssOsMemSet(&ucRouteEntry,0,sizeof(CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC));
    ucRouteEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    rc = cpssExMxIpUcRouteEntriesWrite(dev,16,&ucRouteEntry,1);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /******************************************/
    /* set route entry 0 as default multicast */
    /******************************************/
    cpssOsMemSet(&mcRouteEntry,0,sizeof(CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC));
    mcRouteEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    rc = cpssExMxIpMcRouteEntryWrite(dev,0,&mcRouteEntry,1);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /*******************************/
    /* add active device to LPM DB */
    /*******************************/
    rc = cpssExMxIpLpmDBDevListAdd(lpmDbId,&dev,1);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /*********************************************************/
    /* create virtual router if it wasn't previously created */
    /*********************************************************/
    if (vrInitialized == GT_FALSE)
    {
        defUcNextHopPointer.routeEntryBaseMemAddr = 16;
        defUcNextHopPointer.blockSize = 1;
        defUcNextHopPointer.routeEntryMethod = CPSS_EXMX_IP_REG_ROUTE_ENTRY_E;

        defMcRoutePointer.routeEntryBaseMemAddr = 0;
        defMcRoutePointer.blockSize = 1;
        defMcRoutePointer.routeEntryMethod = CPSS_EXMX_IP_REG_ROUTE_ENTRY_E;

        if(appDemoPpConfigList[dev].devFamily == CPSS_PP_FAMILY_TIGER_E)
        {
            /* set PCL ID for IPV6 Multicast Group lookup key */
            rc = cpssExMxTgIpv6McPclIdSet(dev, sysConfigParamsPtr->vrIpv6McPclId);
            if (rc != GT_OK)
            {
                return  rc;
            }
        }

        rc = cpssExMxIpLpmVirtualRouterSet(lpmDbId,vrId,defUcNextHopPointer,
                                           defMcRoutePointer,
                                           CPSS_IP_PROTOCOL_IPV4V6_E,
                                           sysConfigParamsPtr->defIpv6McPclRuleIndex,
                                           sysConfigParamsPtr->vrIpv6McPclId);
        if (rc != GT_OK)
        {
            return  rc;
        }

        vrInitialized = GT_TRUE;
    }

    return GT_OK;
}
#endif

#ifdef IMPL_TRUNK
#include <cpss/exMx/exMxGen/trunk/cpssExMxTrunk.h>

static GT_STATUS prvTrunkLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initTrunk == GT_FALSE)
        return GT_OK;

    rc = cpssExMxTrunkInit(dev, (GT_U8)sysConfigParamsPtr->numOfTrunks);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif


#ifdef IMPL_TUNNEL
#include <cpss/exMx/exMxTg/tunnel/cpssExMxTgTunnelTerm.h>
#include <cpss/exMx/exMxTg/tunnel/cpssExMxTgTunnelEntry.h>
#include <cpss/exMx/exMxTg/logicalIf/cpssExMxTgInlif.h>

/* number of bytes in Inlif Table HW entry row          */
#define INLIF_ENTRY_SIZE_BYTE   (16)

static GT_STATUS prvTunnelLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;
    GT_U8 counterSetIndex = 0;
    GT_U8 expeditedTunnelNum = 0;
    GT_U32 inlif[INLIF_ENTRY_SIZE_BYTE / sizeof(GT_U32)];

    cpssOsMemSet(inlif,0,INLIF_ENTRY_SIZE_BYTE);
    if(libInitParamsPtr->initIpv4Tunnel == GT_FALSE)
        return GT_OK;

    /* initialize tunnel entry facilities */
     rc = cpssExMxTgTunnelInit(dev);
     if( GT_OK != rc)
         return rc;

     /* initialize tunnel termination parameters */
     rc = cpssExMxTgTunnelTermInterfaceModeSet(dev,
                                         CPSS_EXMXTG_TUNNEL_TERM_INTERFACE_ROUTE_ENTRY_MODE_E);
     if( GT_OK != rc)
         return rc;

     for ( counterSetIndex =0; counterSetIndex < 3; counterSetIndex++)
     {
         rc = cpssExMxTgTunnelTermExpeditedPacketCounterSet (dev,counterSetIndex,0);
         if( GT_OK != rc)
             return rc;
     }

     for ( expeditedTunnelNum =0; expeditedTunnelNum < 8; expeditedTunnelNum++)
     {

         rc = cpssExMxTgInlifExpeditedTunnelEntrySet (dev,expeditedTunnelNum,inlif);
         if( GT_OK != rc)
             return rc;
     }

    return  cpssExMxTgTunnelTermPolicyRedirectModeSet(dev,
                                                   CPSS_EXMXTG_TUNNEL_TERM_PCL_ENABLED_VID_ALTERATION_MODE_E);

}


#endif



/* Definition of lib init functions. */
static FUNCP_CPSS_MODULE_INIT cpssInitList[] = {

prvInlifLibInit,

#ifdef IMPL_BRIDGE
    prvBridgeLibInit,
#endif

#ifdef IMPL_MIRROR
prvMirrorLibInit,
#endif


#ifdef IMPL_NETIF
prvNetIfLibInit,
#endif

#ifdef IMPL_PCL
prvPclLibInit,
#endif

#ifdef IMPL_I2C
prvI2cLibInit,
#endif

#ifdef IMPL_POLICER
prvPolicerLibInit,
#endif

#ifdef IMPL_PORT
prvPortLibInit,
#endif

#ifdef IMPL_IP
prvIpLibInit,
#endif

#ifdef IMPL_TRUNK
prvTrunkLibInit,
#endif

#ifdef IMPL_TUNNEL
prvTunnelLibInit,
#endif

    0
};


