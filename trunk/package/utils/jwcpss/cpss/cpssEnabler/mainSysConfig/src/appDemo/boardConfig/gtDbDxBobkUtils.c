#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChRegsVer1.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBobKResource.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortInterlaken.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPizzaArbiterWorkConserving.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPAUnitBW.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/gtDbDxBobcat2Mappings.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern GT_STATUS cpssInitSystem
(
    IN  GT_U32  boardIdx,
    IN  GT_U32  boardRevId,
    IN  GT_U32  reloadEeprom
);

extern GT_STATUS appDemoTraceHwAccessEnable
(
    IN GT_U8                                devNum,
    IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT    accessType,
    IN GT_BOOL                              enable
);


extern GT_STATUS appDemoDbEntryAdd
(
    IN  GT_CHAR *namePtr,
    IN  GT_U32  value
);

extern GT_STATUS appDemoDebugDeviceIdSet
(
    IN GT_U8    devNum,
    IN GT_U32   deviceId
);

extern GT_STATUS appDemoBc2PortListInit
(
    IN  GT_U8 dev,
    PortInitList_STC * portInitList
);

extern GT_STATUS gtBobcat2PortPizzaArbiterIfStateDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId
);

extern GT_STATUS gtBobcat2PortPizzaArbiterIfUnitStateDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  CPSS_DXCH_PA_UNIT_ENT unit
);

extern GT_STATUS gtBobcat2PortMappingDump
(
    IN  GT_U8  dev
);

extern GT_STATUS prvCpssDxChCaelumPortResourseConfigGet
(
    IN  GT_U8                                        devNum,
    IN  GT_PHYSICAL_PORT_NUM                         portNum,
    OUT PRV_CPSS_DXCH_CAELUM_PORT_RESOURCE_STC    *resourcePtr
);

extern void appDemoBc2SetCustomPortsMapping
(
    IN GT_BOOL enable
);

extern GT_STATUS appDemoBc2TmPortsInit
(
    IN GT_U8 dev
);

extern GT_CHAR * CPSS_SPEED_2_STR
(
    CPSS_PORT_SPEED_ENT speed
);


extern GT_CHAR * CPSS_IF_2_STR
(
    CPSS_PORT_INTERFACE_MODE_ENT ifEnm
);




extern GT_CHAR * CPSS_MAPPING_2_STR
(
    CPSS_DXCH_PORT_MAPPING_TYPE_ENT mapEnm
);


extern GT_CHAR * RXDMA_IfWidth_2_STR
(
    IN GT_U32 RXDMA_IfWidth
);


extern GT_CHAR * TX_FIFO_IfWidth_2_STR
(
    IN GT_U32 TX_FIFO_IfWidth
);

extern GT_CHAR * CPSS_SPEED_2_STR
(
    CPSS_PORT_SPEED_ENT speed
);


extern GT_STATUS appDemoBobKSimCoreClockSet
(
    GT_U8  devNum,
    GT_U32 coreClock
);



/* 
    API to configure TmPorts, with non default ports mapping
*/
extern void appDemoBc2SetCustomPortsMapping(GT_BOOL enable);
extern GT_STATUS appDemoBc2TmPortsInit(GT_U8 dev);



#ifdef __cplusplus
}
#endif /* __cplusplus */


GT_STATUS appDemoBobk2PortListMacPcsStatePrint
(
    IN GT_U8 dev
)
{
    GT_STATUS rc;
    GT_U32 portIdx;
    GT_PHYSICAL_PORT_NUM portNum;
    static CPSS_DXCH_DETAILED_PORT_MAP_STC portMap;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapPtr = &portMap;
    CPSS_PORT_SPEED_ENT speed;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
    GT_BOOL isPortEnabled;
    GT_BOOL isForceLinkDownUp;
    GT_BOOL isLinkDown;
    GT_BOOL isMacReset;
    /* 
    GT_BOOL isPcsRxReset;
    GT_BOOL isPcsTxReset;
    */


    cpssOsPrintf("\n+---------------------------------------------------------------------------------------+");
    cpssOsPrintf("\n|                             Port status                                               |");
    cpssOsPrintf("\n+----+------+----------+-------+--------------+-----+-----+-----------+-----+-----------+");
    cpssOsPrintf("\n|    |      |          |       |              |     | Port|    Link   | Mac |   Pcs     |");
    cpssOsPrintf("\n|    |      |          |       |              |     |-----|-----+-----|-----|-----+-----+");
    cpssOsPrintf("\n| #  | Port | map type | Speed |    IF        | mac | En  |  Up |Force| Rst |  Rx |  Tx |");
    cpssOsPrintf("\n|    |      |          |       |              |     |     |     | Up  |     | Rst | Rst |");
    cpssOsPrintf("\n|    |      |          |       |              |     |     |     |     |     |     |     |");
    cpssOsPrintf("\n+----+------+----------+-------+--------------+-----+-----+-----+-----+-----+-----+-----+");  

    portIdx = 0;
    for (portNum = 0 ; portNum < 256; portNum++)
    {
        rc = cpssDxChPortPhysicalPortDetailedMapGet(dev,portNum,/*OUT*/portMapPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (portMapPtr->valid == GT_FALSE)
        {
            continue;
        }
        if (portMapPtr->portMap.mappingType != CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E)
        {
            rc = cpssDxChPortSpeedGet(dev,portNum,/*OUT*/&speed);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = cpssDxChPortInterfaceModeGet(dev,portNum,/*OUT*/&ifMode);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else /* CPU */
        {
            speed  = CPSS_PORT_SPEED_1000_E;
            ifMode = CPSS_PORT_INTERFACE_MODE_NA_E;
        }
        if (speed != CPSS_PORT_SPEED_NA_E)
        {
            rc = cpssDxChPortEnableGet(dev,portNum,&isPortEnabled);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = cpssDxChPortForceLinkDownEnableGet(dev,portNum,&isForceLinkDownUp);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = cpssDxChPortLinkStatusGet(dev,portNum,&isLinkDown);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDxChPortMacResetStateGet(dev,portNum,&isMacReset);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* 
            rc = cpssDxChPortPcsResetGet(dev,portNum,CPSS_PORT_PCS_RESET_MODE_RX_E,&isPcsRxReset);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = cpssDxChPortPcsResetGet(dev,portNum,CPSS_PORT_PCS_RESET_MODE_TX_E,&isPcsTxReset);
            if (rc != GT_OK)
            {
                return rc;
            }
            */

            cpssOsPrintf("\n| %2d | %4d | %-8s | %s | %s |",portIdx
                                                    ,portNum
                                                    ,CPSS_MAPPING_2_STR(portMapPtr->portMap.mappingType)
                                                    ,CPSS_SPEED_2_STR(speed)
                                                    ,CPSS_IF_2_STR(ifMode));
            cpssOsPrintf(" %5d |",portMapPtr->portMap.macNum);
            cpssOsPrintf(" %3d |",isPortEnabled);
            cpssOsPrintf(" %3d |",isForceLinkDownUp);
            cpssOsPrintf(" %3d |",isLinkDown);
            cpssOsPrintf(" %3d |",isMacReset);
            /* 
            cpssOsPrintf(" %3d |",isPcsRxReset);
            cpssOsPrintf(" %3d |",isPcsTxReset);
            */

            portIdx++;
        }
    }
    return GT_OK;
}


GT_STATUS appDemoBobk2PortGobalResourcesPrint
(
    IN GT_U8 dev
)
{
    struct groupResorcesStatus_STC *groupResorcesStatusPtr;
    GT_U32 dpIndex;
    GT_U32 maxDp;

    groupResorcesStatusPtr = (struct groupResorcesStatus_STC *)&PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.groupResorcesStatus;

    maxDp = 1;
    if(PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.multiDataPath.supportMultiDataPath)
    {
        maxDp = PRV_CPSS_DXCH_PP_MAC(dev)->hwInfo.multiDataPath.maxDp;
    }

    cpssOsPrintf("\n+-----+---------------+---------------+---------------+----------+----------+");
    cpssOsPrintf("\n|     |  TxQ Descr's  |   Header Cnt  |   PayLoad Cnt |    TM    |  Core    |");
    cpssOsPrintf("\n| DP# |---------------|---------------|---------------|  cum BW  | Overall  |");
    cpssOsPrintf("\n|     |  used   total |  used   total |  used   total |  (Mbps)  |  Speed   |");
    cpssOsPrintf("\n+-----+---------------+---------------+---------------+----------+----------+");
    for (dpIndex = 0 ; dpIndex < maxDp; dpIndex++)
    {
        cpssOsPrintf("\n| %3d |",dpIndex);
        cpssOsPrintf("  %3d     %3d  |",groupResorcesStatusPtr->usedDescCredits   [dpIndex], groupResorcesStatusPtr->maxDescCredits   [dpIndex]);
        cpssOsPrintf("  %3d     %3d  |",groupResorcesStatusPtr->usedHeaderCredits [dpIndex], groupResorcesStatusPtr->maxHeaderCredits [dpIndex]);
        cpssOsPrintf("  %3d     %3d  |",groupResorcesStatusPtr->usedPayloadCredits[dpIndex], groupResorcesStatusPtr->maxPayloadCredits[dpIndex]);
        cpssOsPrintf("  %6d  |",groupResorcesStatusPtr->trafficManagerCumBWMbps);
        cpssOsPrintf("  %6d  |",groupResorcesStatusPtr->coreOverallSpeedSummary[dpIndex]);
    }
    cpssOsPrintf("\n+-----+---------------+---------------+---------------+----------+----------+");
    cpssOsPrintf("\n");
    return GT_OK;
}

GT_STATUS appDemoBobk2PortListResourcesPrint
(
    IN GT_U8 dev
)
{
    GT_STATUS rc;
    GT_U32 portIdx;
    GT_PHYSICAL_PORT_NUM portNum;
    static CPSS_DXCH_DETAILED_PORT_MAP_STC portMap;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapPtr = &portMap;
    CPSS_PORT_SPEED_ENT speed;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
    PRV_CPSS_DXCH_CAELUM_PORT_RESOURCE_STC    resource;

    cpssOsPrintf("\n+------------------------------------------------------------------------------------------------------------------+");
    cpssOsPrintf("\n|                             Port resources                                                                       |");
    cpssOsPrintf("\n+----+------+----------+-------+--------------+--------------------+-----+-----------------+-----------+-----------+");
    cpssOsPrintf("\n|    |      |          |       |              |                    |RXDMA|   TXDMA SCDMA   |   TXFIFO  | Eth_TXFIFO|");
    cpssOsPrintf("\n|    |      |          |       |              |                    |-----|-----+-----------|-----+-----|-----+-----|");
    cpssOsPrintf("\n| #  | Port | map type | Speed |    IF        | rxdma txq txdma tm |  IF | TxQ |  TX-FIFO  | Out | Pay | Out | Pay |");
    cpssOsPrintf("\n|    |      |          |       |              |                    |Width|Descr|-----+-----|Going| Load|Going| Load|");
    cpssOsPrintf("\n|    |      |          |       |              |                    |     |     | Hdr | Pay | Bus |Thrsh| Bus |Thrsh|");
    cpssOsPrintf("\n|    |      |          |       |              |                    |     |     |Thrsh|Load |Width|     |Width|     |");
    cpssOsPrintf("\n|    |      |          |       |              |                    |     |     |Thrsh|Thrsh|     |     |     |     |");
    cpssOsPrintf("\n+----+------+----------+-------+--------------+--------------------+-----+-----+-----+-----+-----+-----+-----+-----+");  

    portIdx = 0;
    for (portNum = 0 ; portNum < 256; portNum++)
    {
        rc = cpssDxChPortPhysicalPortDetailedMapGet(dev,portNum,/*OUT*/portMapPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (portMapPtr->valid == GT_FALSE)
        {
            continue;
        }
        if (portMapPtr->portMap.mappingType != CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E)
        {
            rc = cpssDxChPortSpeedGet(dev,portNum,/*OUT*/&speed);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = cpssDxChPortInterfaceModeGet(dev,portNum,/*OUT*/&ifMode);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else /* CPU */
        {
            speed  = CPSS_PORT_SPEED_1000_E;
            ifMode = CPSS_PORT_INTERFACE_MODE_NA_E;
        }
        if (speed != CPSS_PORT_SPEED_NA_E)
        {
            cpssOsPrintf("\n| %2d | %4d | %-8s | %s | %s |",portIdx
                                                    ,portNum
                                                    ,CPSS_MAPPING_2_STR(portMapPtr->portMap.mappingType)
                                                    ,CPSS_SPEED_2_STR(speed)
                                                    ,CPSS_IF_2_STR(ifMode));
            cpssOsPrintf(" %5d %3d %5d %2d |"  ,portMapPtr->portMap.rxDmaNum
                                               ,portMapPtr->portMap.txqNum
                                               ,portMapPtr->portMap.txDmaNum
                                               ,portMapPtr->portMap.tmPortIdx);

            rc = prvCpssDxChCaelumPortResourseConfigGet(dev, portNum,/*OUT*/ &resource);
            if(rc != GT_OK)
            {
                return rc;
            }
            cpssOsPrintf("%4s |",RXDMA_IfWidth_2_STR(resource.rxdmaScdmaIncomingBusWidth));
            cpssOsPrintf("%4d |",resource.txdmaCreditValue);
            cpssOsPrintf("%4d |",resource.txfifoHeaderCounterThresholdScdma);
            cpssOsPrintf("%4d |",resource.txfifoPayloadCounterThresholdScdma);
            cpssOsPrintf("%4s |",TX_FIFO_IfWidth_2_STR(resource.txfifoScdmaShiftersOutgoingBusWidth));
            cpssOsPrintf("%4d |",resource.txfifoScdmaPayloadThreshold);
            if (portMapPtr->portMap.trafficManagerEn == GT_FALSE)
            {
                cpssOsPrintf("%4s |%4s |","--","--");
            }
            else
            {
                cpssOsPrintf("%4s |",TX_FIFO_IfWidth_2_STR(resource.ethTxfifoOutgoingBusWidth));
                cpssOsPrintf("%4d |",resource.ethTxfifoScdmaPayloadThreshold);
            }
        
            portIdx++;
        }
    }
    cpssOsPrintf("\n+----+------+----------+-------+--------------+--------------------+-----+-----+-----+-----+-----+-----+-----+-----+");  
    cpssOsPrintf("\n");
    return GT_OK;
}


