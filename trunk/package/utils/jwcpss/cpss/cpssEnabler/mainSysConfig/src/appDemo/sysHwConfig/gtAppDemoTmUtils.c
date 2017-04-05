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
* gtAppDemoTmUtils.c
*
* DESCRIPTION:
*       Basic TM configurations.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*       $Revision: 9 $
*
*******************************************************************************/

#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/l2mll/cpssDxChL2Mll.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>

#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>

#include <cpss/generic/port/cpssPortStat.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgEgrFlt.h>
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgE2Phy.h>
#include <cpss/dxCh/dxChxGen/protection/cpssDxChProtection.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgPrvEdgeVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSrcId.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>

#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueDram.h>
/*#include <cpss/dxCh/dxChxGen/tmGlue/private/prvCpssDxChTmGlue.h>*/
#include <cpss/generic/tm/cpssTmPublicDefs.h>
#include <cpss/generic/tm/cpssTmCtl.h>
#include <cpss/generic/tm/cpssTmSched.h>
#include <cpss/generic/tm/cpssTmDrop.h>
#include <cpss/generic/tm/cpssTmNodesCreate.h>
#include <cpss/generic/tm/cpssTmNodesCtl.h>
#include <cpss/generic/tm/cpssTmShaping.h>
#include <cpss/generic/tm/cpssTmNodesUpdate.h>
#include <cpss/generic/tm/cpssTmNodesRead.h>
#include <cpss/generic/tm/cpssTmNodesStatus.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPfc.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGluePfc.h>

#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueAgingAndDelay.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueQueueMap.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueDrop.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueFlowControl.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>

#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>

#include <mvDdr3TrainingIp.h>
#include <mvDdr3TrainingIpBist.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <cpssCommon/private/prvCpssMath.h>
#include <cpss/generic/tm/prvCpssTmCtl.h>
#include <cpss/generic/tm/prvCpssTmDrop.h>

#define TM_QUEUE_POOL_SIZE 16384

static char *tmTreelevelNamesArr[] =
{
    "Q_LEVEL",
    "A_LEVEL",
    "B_LEVEL",
    "C_LEVEL",
    "P_LEVEL"
};

static char *tmDropModeNamesArr[] =
{
    "CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E",
    "CPSS_TM_DROP_MODE_CB_TD_CA_TD_E",
    "CPSS_TM_DROP_MODE_CB_TD_ONLY_E"
};


/* ePort is port for protected path, ePort-1 is working path port to do on it protection,
traffic should be forwarded to (ePort -1), protPort physical port used for protected path as egress physical port*/
/* tempPhyPort is auxilary physical port, this port is assigned to protected path for enableing TM, it is not
connected to any cable, it's used for TM tree for protected path.*/
/* flowProtection as opposed to port protection, only certain ePort is protected */
GT_STATUS doProtection(GT_U32 ePort, GT_U32 protPort, GT_U32 locIndex, GT_U32 tempPhyPort, GT_BOOL flowProtection)
{
    GT_STATUS                                           rc;
    CPSS_INTERFACE_INFO_STC                             physicalInfo;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC    entry;
    GT_U32                                              queueId, bitIndex, n=1;
    CPSS_DXCH_PORT_MAP_STC     portMap;
    GT_U32 workPort;

    if((ePort % 2) == 0)
    {
        osPrintf("ePort must be odd\n");
        return 1;
    }

    if(protPort > 9)
    {
        osPrintf("protPort must < 10\n");
        return 2;
    }

    physicalInfo.devPort.hwDevNum= 16;
    physicalInfo.devPort.portNum = tempPhyPort;
    physicalInfo.type = CPSS_INTERFACE_PORT_E;

    rc= cpssDxChBrgEportToPhysicalPortTargetMappingTableSet(0 , ePort, &physicalInfo);
    if(rc != GT_OK)
        return rc;

    rc= cpssDxChProtectionEnableSet(0, GT_TRUE);
    if(rc != GT_OK)
        return rc;

    rc= cpssDxChProtectionTxEnableSet(0, ePort-1, GT_TRUE);
    if(rc != GT_OK)
        return rc;
    rc= cpssDxChProtectionPortToLocMappingSet(0 , ePort-1, locIndex);
    if(rc != GT_OK)
        return rc;

    if(flowProtection == GT_TRUE) /* protection just for one flow*/
    {
        /* queue should be such as to be descendant of the protPort*/
        queueId = protPort * 1600 + (ePort%200)*8;
        osPrintf("TM queue %d for protection path\n",queueId);

        entry.queueIdBase = 0; /* will use it for full Q-remapping to a different physical port*/

        for (bitIndex = 0; bitIndex < 14; bitIndex++)
        {
            if( (queueId/n)%2 == 1)
            {
                osPrintf("bit %d 1\n",bitIndex);
                entry.bitSelectArr[bitIndex].selectType = CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ONE_E;
            }
            else
            {
                osPrintf("bit %d 0\n",bitIndex);
                entry.bitSelectArr[bitIndex].selectType = CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ZERO_E;
            }
            /* not sure it is needed when selectType = CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ONE_E*/
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex;
            n=n*2;
        }

        /* whatever goes to protPort is mapped to the same TM queue, this queue is tied to P-node of protPort,
        but it is not a must in conifguration, we could have taken a unconneted physical port just for
        queue mapping */
        rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, tempPhyPort, &entry);
        if(rc != GT_OK)
            return rc;

    }
    else /* protection for all ePorts mapped to the same physical port as this ePort, as long
    as they are remapped to auxilary tempPhyPort - the relations in TM tree are mantained to a large extent -
    there are two exceptions:
    1. ePort-1 is the original WP port, ePort+TC maped queue may be significantly different from (ePort-1)+TC maped queue */
    {
        rc= cpssDxChBrgEportToPhysicalPortTargetMappingTableGet(0 , ePort-1, &physicalInfo);
        if(rc != GT_OK)
            return rc;

        workPort = physicalInfo.devPort.portNum;
        if(protPort < workPort)
        {
            osPrintf("in base adjustment protection, protection path physical port must be greater than working path port\n");
            return 3;
        }
        rc = cpssDxChTmGlueQueueMapBitSelectTableEntryGet(0, workPort, &entry);
        entry.queueIdBase = (protPort - workPort)*1600;
        osPrintf(" working path port %d, protection port %d queueIdBase %d\n",workPort, protPort,
                 entry.queueIdBase);

        if(rc != GT_OK)
            return rc;
        rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, tempPhyPort, &entry);
        if(rc != GT_OK)
            return rc;
    }

    /* and the last thing - make the auxilary physical port TM mapped to protection port for egress*/

    osMemSet(&portMap, 0, sizeof(portMap));

    portMap.tmEnable = GT_TRUE;
    portMap.physicalPortNumber = tempPhyPort;
    portMap.mappingType     = CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E;
    portMap.tmPortInd       = tempPhyPort; /* do we care? */
    portMap.interfaceNum    = protPort;/* it represents the mapping of TM port to physical port for egress*/
    portMap.txqPortNumber   = 64;

    osPrintf("write TM mapping for port %d\n", tempPhyPort);
    rc = cpssDxChPortPhysicalPortMapSet(0,1,&portMap);
    if(rc != GT_OK)
        return rc;

    return GT_OK;

}

GT_STATUS protStatusSet(GT_U32 index, CPSS_DXCH_PROTECTION_LOC_STATUS_ENT status)
{
    GT_STATUS rc;

    rc = cpssDxChProtectionLocStatusSet(0, index, status);
    return rc;
}

/* delete a Q-node by index and then add it to an A-node aNodeInd, not nessaserely its former father,
serves to show how Q-node indexes are allocated */
GT_STATUS pveSet(GT_PORT_NUM portIn, GT_PORT_NUM portOut, GT_BOOL enable)
{
    GT_STATUS rc;

    rc = cpssDxChBrgPrvEdgeVlanEnable(0, GT_TRUE);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChBrgPrvEdgeVlanPortEnable(0, portIn, enable, portOut, 16, GT_FALSE);
    if(rc != GT_OK)
        return rc;

    return GT_OK;

}

/* verify with which indexes nodes are created */
GT_STATUS nodeDeleteCreate(GT_U32 index, GT_U32 aNodeInd /* should be below 10*/)
{
    GT_STATUS rc;
    GT_U32 queueId;
    CPSS_TM_QUEUE_PARAMS_STC                queueParams;

    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS; /* in CPSS_TM_NODE_QUANTUM_UNIT_CNS */
    queueParams.dropProfileInd = 1;
    queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;


    rc = cpssTmNodesCtlNodeDelete(0, CPSS_TM_LEVEL_Q_E, index);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssTmQueueToAnodeCreate(0, aNodeInd, &queueParams, &queueId);
    osPrintf("queueId = %d ",queueId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}

/* not in use */
GT_STATUS qMap()
{
    GT_STATUS rc;

    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC   mapEntry;

    rc = cpssDxChTmGlueQueueMapSelectorTableAccessModeSet(0, CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_LOCAL_TARGET_PORT_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    mapEntry.bitSelectArr[0].bitSelector = 0;
    mapEntry.bitSelectArr[0].selectType = CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_EPORT_E;

    rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, 1, &mapEntry);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}

/* get TM aging and CNC TM counters*/
GT_STATUS getTMQCounters(GT_BOOL includeBytes)
{
    GT_STATUS rc;
    GT_U32 i,j,queueId;
    CPSS_DXCH_TM_AGING_AND_DELAY_STATISTICS_STC agingCounters;
    CPSS_DXCH_CNC_COUNTER_STC         cncCounter;
    GT_BOOL Enable;

    osPrintf("AgingAndDelay counters:\n");
    for (i=0; i<100; i++)
    {
        rc = cpssDxChTmGlueAgingAndDelayCountersGet(0, i, &agingCounters);
        if (rc != GT_OK)
            return rc;
        if(agingCounters.packetCounter > 0)
        {
            rc = cpssDxChTmGlueAgingAndDelayCounterQueueIdGet(0, i, &queueId);
            if (rc != GT_OK)
                return rc;
            osPrintf("counter %d: %d packets passed through queue %d on TM exit.  Letency min %d, max %d\n",i,
                     agingCounters.packetCounter, queueId, agingCounters.minLatencyTime, agingCounters.maxLatencyTime);
        }
    }
    osPrintf("TM CNC counters:\n");
    for (i=0; i<32; i++)
    {
        rc = cpssDxChCncBlockClientEnableGet(0, i, CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E, &Enable);
        if( GT_OK != rc)
           return rc;

        if (Enable == GT_FALSE)
        {
            continue;
        }

        for (j=0; j<1024; j++)
        {
            rc = cpssDxChCncCounterGet(0,i,j,CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E,&cncCounter);
            if (rc != GT_OK)
                return rc;
            if( (cncCounter.packetCount.l[1]+cncCounter.packetCount.l[0]) > 0)
            {
                if( (j%2) == 0)
                    osPrintf("0x%X%X ( %d )packets entered TM through queue %d\n",cncCounter.packetCount.l[1], cncCounter.packetCount.l[0],
                             (cncCounter.packetCount.l[1]*0xFFFFFFFF + cncCounter.packetCount.l[0]), i*512+j/2);
                else
                    osPrintf("0x%X%X ( %d )packets dropped on queue %d\n",cncCounter.packetCount.l[1], cncCounter.packetCount.l[0],
                             (cncCounter.packetCount.l[1]*0xFFFFFFFF + cncCounter.packetCount.l[0]), i*512+j/2);
                if(includeBytes == GT_TRUE)
                {
                    if( (j%2) == 0)
                        osPrintf("0x%X%X ( %d )bytes entered TM through queue %d\n",cncCounter.byteCount.l[1], cncCounter.byteCount.l[0],
                                 (cncCounter.byteCount.l[1]*0xFFFFFFFF + cncCounter.byteCount.l[0]), i*512+j/2);
                    else
                        osPrintf("0x%X%X ( %d )bytes dropped on queue %d\n",cncCounter.byteCount.l[1], cncCounter.byteCount.l[0],
                                 (cncCounter.byteCount.l[1]*0xFFFFFFFF + cncCounter.byteCount.l[0]), i*512+j/2);
                }
            }

        }
    }
    return GT_OK;
}

/* get Summary of TM Q length counters */
GT_STATUS getTMQLenCountersSum(GT_U32 from_q, GT_U32 to_q)
{
    GT_STATUS rc = GT_OK;
    GT_U32 totalUse[2] = {0,0},
           qNodeIndex,
           qLength[2] = {0,0},
           maxQLenght[2] = {0,0},
           maxIndex[2] = {0,0};

    if (to_q == 0)
    {
        to_q = TM_QUEUE_POOL_SIZE - 1;
    }
    else
    {
        to_q = MIN(to_q, TM_QUEUE_POOL_SIZE - 1);
    }


    for(qNodeIndex = from_q;  qNodeIndex <= to_q; qNodeIndex++)
    {
        rc = cpssTmDropQueueLengthGet(0,
                                      CPSS_TM_LEVEL_Q_E,
                                      qNodeIndex,
                                      &(qLength[0]),
                                      &(qLength[1]));
        if(rc!=GT_OK)
            return rc;

        totalUse[0] += qLength[0];
        totalUse[1] += qLength[1];
        if(maxQLenght[0] > qLength[0])
        {
            maxQLenght[0]   = qLength[0];
            maxIndex[0]     = qNodeIndex;
        }
        if(maxQLenght[1] > qLength[1])
        {
            maxQLenght[1]   = qLength[1];
            maxIndex[1]     = qNodeIndex;
        }

    }

    osPrintf("Total queues length is %d bytes, max queue length is %d on queue %d\n",
             totalUse[0]*16,
             maxQLenght[0]*16,
             maxIndex[0]);

    osPrintf("Total average queues length is %d bytes, max queue average length is %d on queue %d\n",
             totalUse[1]*16,
             maxQLenght[1]*16,
             maxIndex[1]);

    return GT_OK;
}



/* get Summary of TM drop counters*/
GT_STATUS getTMQCountersSum(GT_BOOL includeBytes)
{
    GT_STATUS rc;
    GT_U32 i,j;
    CPSS_DXCH_CNC_COUNTER_STC cncCounter;
    CPSS_DXCH_CNC_COUNTER_STC in_sum, drop_sum;
    GT_BOOL Enable;

    cpssOsMemSet(&in_sum, 0, sizeof(in_sum));
    cpssOsMemSet(&drop_sum, 0, sizeof(drop_sum));


    osPrintf("TM CNC counters Sum:\n");
    for (i=0; i<32; i++)
    {
        rc = cpssDxChCncBlockClientEnableGet(0, i, CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E, &Enable);
        if( GT_OK != rc)
           return rc;

        if (Enable == GT_FALSE)
        {
            continue;
        }

        for (j=0; j<1024; j++)
        {
            rc = cpssDxChCncCounterGet(0,i,j,CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E,&cncCounter);
            if (rc != GT_OK)
                return rc;
            if( (cncCounter.packetCount.l[1]+cncCounter.packetCount.l[0]) > 0)
            {
                if( (j%2) == 0)
                    in_sum.packetCount = prvCpssMathAdd64(in_sum.packetCount, cncCounter.packetCount);
                else
                    drop_sum.packetCount = prvCpssMathAdd64(drop_sum.packetCount, cncCounter.packetCount);

                if(includeBytes == GT_TRUE)
                {
                    if( (j%2) == 0)
                        in_sum.byteCount = prvCpssMathAdd64(in_sum.byteCount, cncCounter.byteCount);
                    else
                        drop_sum.byteCount = prvCpssMathAdd64(drop_sum.byteCount, cncCounter.byteCount);
                }
            }
        }
    }

    if (in_sum.packetCount.l[1] + in_sum.packetCount.l[0] > 0)
    {
        osPrintf("0x%X%X ( %u )packets entered TM\n", in_sum.packetCount.l[1], in_sum.packetCount.l[0],
                 (in_sum.packetCount.l[1]*0xFFFFFFFF + in_sum.packetCount.l[0]));
    }

    if (in_sum.byteCount.l[1] + in_sum.byteCount.l[0] > 0)
    {
        osPrintf("0x%X%X ( %u )bytes entered TM\n", in_sum.byteCount.l[1], in_sum.byteCount.l[0],
                 (in_sum.byteCount.l[1]*0xFFFFFFFF + in_sum.byteCount.l[0]));

    }

    if (drop_sum.packetCount.l[1] + drop_sum.packetCount.l[0] > 0)
    {
        osPrintf("0x%X%X ( %u )packets dropped\n", drop_sum.packetCount.l[1], drop_sum.packetCount.l[0],
                 (drop_sum.packetCount.l[1]*0xFFFFFFFF + drop_sum.packetCount.l[0]));
    }

    if (drop_sum.byteCount.l[1] + drop_sum.byteCount.l[0] > 0)
    {
        osPrintf("0x%X%X ( %u )bytes dropped\n", drop_sum.byteCount.l[1], drop_sum.byteCount.l[0],
                 (drop_sum.byteCount.l[1]*0xFFFFFFFF + drop_sum.byteCount.l[0]));

    }

    return GT_OK;
}

GT_STATUS configTMQCounters()
{
    GT_STATUS rc=GT_OK;
    GT_U32 i;
    GT_U32                            agedPacketCouterQueueMask=0;
    GT_U32                            agedPacketCouterQueue=0;
    GT_U64                          indexRangesBmp;

    /* enable counting for packets entering TM */

    rc = cpssDxChCncTmClientIndexModeSet(0, CPSS_DXCH_CNC_TM_INDEX_MODE_0_E);
    if( GT_OK != rc)
       return rc;
    for(i=0;i<32;i++)
    {
        rc = cpssDxChCncBlockClientEnableSet(0,i,CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E,GT_TRUE);
        if( GT_OK != rc)
           return rc;
        indexRangesBmp.l[0] = 1 << i;
        indexRangesBmp.l[1] = 0;

        rc = cpssDxChCncBlockClientRangesSet(0,i,
                                             CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E,
                                             indexRangesBmp);
        if( GT_OK != rc)
           return rc;
    }

        /* enable counting for packets exiting TM */

    rc = cpssDxChTmGlueAgingAndDelayAgedPacketsCountingEnableSet(0, GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* count all queues for aged (dropped) packets*/
    agedPacketCouterQueueMask   = 0x3FFF;
    agedPacketCouterQueue       = 0x3FFF;

    rc = cpssDxChTmGlueAgingAndDelayAgedPacketCounterConfigSet(0,
                   agedPacketCouterQueueMask,
                   agedPacketCouterQueue);
    if (rc != GT_OK)
    {
        return rc;
    }

    return rc;

}

/* we have only 100 aging counters for 16K queues, we need to allocate them deligently */
GT_STATUS countNQueues(GT_U32 q0, GT_U32 counter, GT_U32 numOfQueues)
{

    GT_STATUS rc;
    GT_U32 i;

    for(i=0;i<numOfQueues;i++)
    {
        rc = cpssDxChTmGlueAgingAndDelayCounterQueueIdSet(0, counter+i, q0+i);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTmGlueAgingAndDelayCounterQueueIdSet", rc);
        if (rc != GT_OK)
            return rc;
    }
    return GT_OK;
}

/* block all TM quesues up to qnum, - check if traffic is forwarded by TM*/
GT_STATUS blockTMQueues(GT_U32 qnum)
{
    GT_STATUS rc;
    GT_U32 i;
    for(i=0; i<qnum; i++)
    {
        rc = cpssTmNodesCtlQueueUninstall(0,i);
        if (rc != GT_OK)
            return rc;
    }
    return GT_OK;
}

GT_32 queueDropProfileGet(GT_U32 qIndex)
{

    GT_U32      profileIndex = 0;
    GT_U64      regAddr;
    GT_U64      data;
    GT_STATUS   rc = GT_OK;

    regAddr.l[0] = (0x2800000+8*(qIndex/4));
    regAddr.l[1] = 0x10;
    rc = cpssTmCtlReadRegister(0,regAddr,&data);
    if (rc != GT_OK)
        return -1;


    switch (qIndex%4)
    {
    case 0:
                    profileIndex = (data.l[0] & 0x7ff);
                    break;
    case 1:
                    profileIndex = ((data.l[0] >> 16)  & 0x7ff);
                    break;
    case 2:
                    profileIndex = (data.l[1] & 0x7ff);
                    break;
    case 3:
                    profileIndex = ((data.l[1] >> 16)  & 0x7ff);
                    break;
    }

    return profileIndex;
}

/* get node parameters, CoS relevant for C and P levels only

use: nodeConfigGet 0, 16, 0
*/
GT_STATUS nodeConfigGet(CPSS_TM_LEVEL_ENT level,
                       GT_U32 index, GT_U32 CoS)
{
    GT_STATUS rc=GT_OK;
    CPSS_TM_QUEUE_PARAMS_STC  paramsQ;
    CPSS_TM_A_NODE_PARAMS_STC paramsA;
    CPSS_TM_B_NODE_PARAMS_STC paramsB;
    CPSS_TM_C_NODE_PARAMS_STC paramsC;
    CPSS_TM_PORT_PARAMS_STC   paramsP;
    CPSS_TM_PORT_DROP_PER_COS_STC   cosParams;
    GT_32                     dpIndex;

    switch(level)
    {
    case CPSS_TM_LEVEL_Q_E:
        rc = cpssTmQueueConfigurationRead(0, index, &paramsQ);
        if (rc != GT_OK)
            return rc;

        dpIndex = queueDropProfileGet(index);
        if (rc != GT_OK)
            return rc;

        osPrintf("Eligible Priority Function = %d, weight = %d\n Shaping Profile = %d\n TD profile = %d (in HW %u)\n",
                 paramsQ.eligiblePrioFuncId,
                 paramsQ.quantum,
                 paramsQ.shapingProfilePtr,
                 paramsQ.dropProfileInd,
                 dpIndex);
        break;

    case CPSS_TM_LEVEL_A_E:
        rc = cpssTmAnodeConfigurationRead(0,index,&paramsA);
        if (rc != GT_OK)
            return rc;
        osPrintf("Eligible Priority Function = %d, weight = %d\n Shaping Profile = %d\n TD profile = %d\n NumOfChildren = %d\n",
                 paramsA.eligiblePrioFuncId,
                 paramsA.quantum,
                 paramsA.shapingProfilePtr,
                 paramsA.dropProfileInd,
                 paramsA.numOfChildren);
        break;

    case CPSS_TM_LEVEL_B_E:
        rc = cpssTmBnodeConfigurationRead(0,index,&paramsB);
        if (rc != GT_OK)
            return rc;
        osPrintf("Eligible Priority Function = %d, weight = %d\n Shaping Profile = %d\n TD profile = %d\n NumOfChildren = %d\n",
                 paramsB.eligiblePrioFuncId,
                 paramsB.quantum,
                 paramsB.shapingProfilePtr,
                 paramsB.dropProfileInd,
                 paramsB.numOfChildren);
        break;

    case CPSS_TM_LEVEL_C_E:
        rc = cpssTmCnodeConfigurationRead(0,index,&paramsC);
        if (rc != GT_OK)
            return rc;
        osPrintf("Eligible Priority Function = %d, weight = %d\n Shaping Profile = %d\n TD profile = %d\n NumOfChildren = %d\n",
                 paramsC.eligiblePrioFuncId,
                 paramsC.quantum,
                 paramsC.shapingProfilePtr,
                 paramsC.dropProfileIndArr[CoS],
                 paramsC.numOfChildren);
        break;

    case CPSS_TM_LEVEL_P_E:
        rc = cpssTmPortConfigurationRead(0,index,&paramsP, &cosParams);
        if (rc != GT_OK)
            return rc;
        osPrintf("Eligible Priority Function = %d, weight = %d\n Shaping: CIR %d KBit/Sec EIR %d KBit/Sec CBS %d Kbytes EBS %d Kbytes\n TD profile = %d\n NumOfChildren = %d\n",
                 paramsP.eligiblePrioFuncId,
                 paramsP.quantumArr[CoS],
                 paramsP.cirBw,
                 paramsP.eirBw,
                 paramsP.cbs,
                 paramsP.ebs,
                 paramsP.dropProfileInd,
                 paramsP.numOfChildren);
        break;

    default:
        osPrintf("Bad level\n");
        return rc;
    }

    return rc;

}


GT_STATUS updateNodePrioWeight(CPSS_TM_LEVEL_ENT level, GT_U32 index,
                               CPSS_TM_ELIG_FUNC_NODE_ENT funcNum,
                               GT_U32 quantum)
{
    CPSS_TM_PORT_PARAMS_STC       paramsP;
    CPSS_TM_A_NODE_PARAMS_STC     paramsA;
    CPSS_TM_B_NODE_PARAMS_STC     paramsB;
    CPSS_TM_C_NODE_PARAMS_STC     paramsC;
    CPSS_TM_QUEUE_PARAMS_STC      paramsQ;
    CPSS_TM_PORT_DROP_PER_COS_STC cosParams;
    CPSS_TM_ELIG_FUNC_NODE_ENT funcNumOld;
    GT_STATUS                  rc = GT_OK;
    GT_U32                     quantumArr[8];
    GT_U32                     i;

    osMemSet(&paramsP, 0, sizeof(paramsP));
    osMemSet(&paramsA, 0, sizeof(paramsA));
    osMemSet(&paramsB, 0, sizeof(paramsB));
    osMemSet(&paramsC, 0, sizeof(paramsC));
    osMemSet(&paramsQ, 0, sizeof(paramsQ));
    switch(level)
    {
    case CPSS_TM_LEVEL_Q_E:
        rc = cpssTmQueueConfigurationRead(0,index,&paramsQ);
        funcNumOld = paramsQ.eligiblePrioFuncId;
        paramsQ.eligiblePrioFuncId = funcNum;
        paramsQ.quantum = quantum*CPSS_TM_NODE_QUANTUM_UNIT_CNS;
        rc = cpssTmQueueUpdate(0,index,&paramsQ);
        break;

    case CPSS_TM_LEVEL_A_E:
        rc = cpssTmAnodeConfigurationRead(0,index,&paramsA);
        funcNumOld = paramsA.eligiblePrioFuncId;
        paramsA.eligiblePrioFuncId = funcNum;
        paramsA.quantum = quantum*CPSS_TM_NODE_QUANTUM_UNIT_CNS;
        rc = cpssTmAnodeUpdate(0,index,&paramsA);
        break;

    case CPSS_TM_LEVEL_B_E:
        rc = cpssTmBnodeConfigurationRead(0,index,&paramsB);
        funcNumOld = paramsB.eligiblePrioFuncId;
        paramsB.eligiblePrioFuncId = funcNum;
        paramsB.quantum = quantum*CPSS_TM_NODE_QUANTUM_UNIT_CNS;
        rc = cpssTmBnodeUpdate(0,index,&paramsB);
        break;

    case CPSS_TM_LEVEL_C_E:
        rc = cpssTmCnodeConfigurationRead(0,index,&paramsC);
        funcNumOld = paramsC.eligiblePrioFuncId;
        paramsC.eligiblePrioFuncId = funcNum;
        paramsC.quantum = quantum*CPSS_TM_NODE_QUANTUM_UNIT_CNS;
        rc = cpssTmCnodeUpdate(0,index,&paramsC);
        break;

    case CPSS_TM_LEVEL_P_E:
        rc = cpssTmPortConfigurationRead(0,index,&paramsP, &cosParams);
        funcNumOld = paramsP.eligiblePrioFuncId;
        paramsP.eligiblePrioFuncId = funcNum;
        for(i=0;i<8;i++)
            quantumArr[i] = (GT_U16)quantum*CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        rc = cpssTmPortSchedulingUpdate(0,index, funcNum, quantumArr, paramsP.schdModeArr);
        break;

    default:
        osPrintf("Bad level\n");
        return rc;
    }
    if (rc != GT_OK)
        return rc;
    osPrintf("Changed from eligible priority function %d\n",funcNumOld);

    return rc;
}

/* for non-P levels update shaping profile identified by index parameter,
for P-level update the shaping on the P-node index*/
GT_STATUS updateShaper(CPSS_TM_LEVEL_ENT level,
                       GT_U32 index,
                       GT_U32  cirBw,
                       GT_U32  cbs,
                       GT_U32  eirBw,
                       GT_U32  ebs
                       )
{
    GT_STATUS rc=GT_OK;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC  profileStr;

    if(level != CPSS_TM_LEVEL_P_E )
    {
        profileStr.cirBw = cirBw;
        profileStr.cbs = cbs;
        profileStr.eirBw = eirBw;
        profileStr.ebs = ebs;

        rc = cpssTmShapingProfileUpdate(0,level, index, &profileStr);
        if(rc != GT_OK)
            return rc;
    }
    else
    {
        rc = cpssTmPortShapingUpdate(0, index, cirBw, eirBw, cbs, ebs);
    }
/*  rc = cpssTmShapingProfileRead(0,level, index, &profileStr);
    if(rc != GT_OK)
        return rc;
    osPrintf("wrote shaper profile: CIR = %d, CBS = %d, EIR = %d, EBS = %d\n", profileStr.cirBw,
             profileStr.cbs, profileStr.eirBw, profileStr.ebs);*/
    return rc;
}


GT_STATUS nodeShaperEnable(CPSS_TM_LEVEL_ENT level, GT_U32 index, GT_BOOL enable, GT_U32 shaperIndex)
{
    GT_STATUS rc=GT_OK;
    CPSS_TM_A_NODE_PARAMS_STC paramsA;
    CPSS_TM_B_NODE_PARAMS_STC paramsB;
    CPSS_TM_C_NODE_PARAMS_STC paramsC;
    CPSS_TM_QUEUE_PARAMS_STC  paramsQ;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC  profileStr;

    if(enable == GT_TRUE)
    {
        rc = cpssTmShapingProfileRead(0,level, shaperIndex, &profileStr);
        if(rc != GT_OK)
        {
            osPrintf("Cannot set such a shaper\n");
            return rc;
        }
        osPrintf("Set shaper profile: CIR = %d, CBS = %d, EIR = %d, EBS = %d\n", profileStr.cirBw,
                 profileStr.cbs, profileStr.eirBw, profileStr.ebs);

    }

    switch(level)
    {
    case CPSS_TM_LEVEL_Q_E:
        rc = cpssTmQueueConfigurationRead(0,index,&paramsQ);
        if (rc != GT_OK)
            return rc;
        if(enable == GT_FALSE)
            paramsQ.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
        else
            paramsQ.shapingProfilePtr = shaperIndex;

        rc = cpssTmQueueUpdate(0,index,&paramsQ);
        break;

    case CPSS_TM_LEVEL_A_E:
        rc = cpssTmAnodeConfigurationRead(0,index,&paramsA);
        if (rc != GT_OK)
            return rc;
        if(enable == GT_FALSE)
            paramsA.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
        else
            paramsA.shapingProfilePtr = shaperIndex;
        rc = cpssTmAnodeUpdate(0,index,&paramsA);
        break;

    case CPSS_TM_LEVEL_B_E:
        rc = cpssTmBnodeConfigurationRead(0,index,&paramsB);
        if (rc != GT_OK)
            return rc;
        if(enable == GT_FALSE)
            paramsB.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
        else
            paramsB.shapingProfilePtr = shaperIndex;
        rc = cpssTmBnodeUpdate(0,index,&paramsB);
        break;

    case CPSS_TM_LEVEL_C_E:
        rc = cpssTmCnodeConfigurationRead(0,index,&paramsC);
        if (rc != GT_OK)
            return rc;
        if(enable == GT_FALSE)
            paramsC.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
        else
            paramsC.shapingProfilePtr = shaperIndex;
        rc = cpssTmCnodeUpdate(0,index,&paramsC);
        break;

    default:
        osPrintf("Bad level\n");
        return rc;
    }
    if (rc != GT_OK)
        return rc;

    return rc;

}

GT_STATUS allShaperDisable(void)
{
    GT_U32  i;
    GT_STATUS rc = GT_OK;


    for (i=0;i<16000;i++)
    {
        rc = nodeShaperEnable(0, i, GT_FALSE, 0);
    }

    for (i=0;i<2000;i++)
    {
        rc = nodeShaperEnable(1, i, GT_FALSE, 0);
    }

    for (i=0;i<200;i++)
    {
        rc = nodeShaperEnable(2, i, GT_FALSE, 0);
    }

    for (i=0;i<40;i++)
    {
        rc = nodeShaperEnable(3, i, GT_FALSE, 0);
    }

    return rc;
}


/* Create a WRED curve to serve in TD profile, for now - traditional curves only, maxProbability in percents from 1 to 100 */
/* Returns index of created curve or GT_ERROR, NOT REGULAR ERROR CODE */
GT_32 WREDCurveCreate(GT_U32  maxProbability)
{
    GT_STATUS rc=GT_OK;
    GT_U32    curveInd;

    rc = cpssTmDropWredTraditionalCurveCreate(0,CPSS_TM_LEVEL_Q_E, 0, maxProbability, &curveInd);
    if(rc != GT_OK)
        return GT_ERROR;
    osPrintf("Max probability %d, curve number %d\n", maxProbability, curveInd);

    return curveInd;
}


/* set TD profile with bytesThreshold in MB, thresholdRatio same for all colors and modes */
/* typical use: TDProfileCreate 0, 2, 5, 5, 0, 0,100 or TDProfileCreate 0, 0, 7, 7, 1, 0,100 */
/* Returns index of created profile or GT_ERROR, NOT REGULAR ERROR CODE */
GT_32 TDProfileCreate(CPSS_TM_LEVEL_ENT level,
                      CPSS_TM_DROP_MODE_ENT dropMode,
                      GT_U32 cbMBytesThreshold,
                      GT_U32 caMBytesThreshold,
                      GT_U32 curveInd,
                      GT_U32 aqlExponent)
{
    GT_STATUS                       rc=GT_OK;
    CPSS_TM_DROP_PROFILE_PARAMS_STC profile;
    GT_U32 profileInd;
    GT_U32                          i;

    osMemSet(&profile, 0, sizeof(profile));

    profile.dropMode = dropMode;
    switch (dropMode)
    {
        case CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E:
            profile.cbTdThresholdBytes = cbMBytesThreshold * 1024 * 1024;  /* make it big - to have no influence*/
            profile.caWredDp.aqlExponent  = (GT_U8)aqlExponent;
            for (i=0; i < 3 ; i++)
            {
                profile.caWredDp.curveIndex[i] = (GT_U8)curveInd;
                profile.caWredDp.dpCurveScale[i] = 0;
                profile.caWredDp.caWredTdMinThreshold[i] = 0;
                profile.caWredDp.caWredTdMaxThreshold[i] = caMBytesThreshold * 1024 * 1024;
            }
            break;
        case CPSS_TM_DROP_MODE_CB_TD_CA_TD_E:
            profile.cbTdThresholdBytes = cbMBytesThreshold * 1024 * 1024 ;  /* make it big - to have no influence*/
            for (i=0; i < 3 ; i++)
            {
                profile.caTdDp.caTdThreshold[i]= caMBytesThreshold * 1024 * 1024;
            }
            break;
        case CPSS_TM_DROP_MODE_CB_TD_ONLY_E:
            profile.cbTdThresholdBytes = cbMBytesThreshold * 1024 * 1024 ;
            break;
        default:break;
    }

    rc = cpssTmDropProfileCreate(0, level, CPSS_TM_INVAL_CNS, &profile, &profileInd);
    if (rc != GT_OK)
        return GT_ERROR;

    osPrintf("created TD profile ID %d\n", profileInd);
    return profileInd;
}


/* set TD profile ID to a node on a level, also may disable TD*/
GT_STATUS nodeTDSet(CPSS_TM_LEVEL_ENT level, GT_U32 index, GT_BOOL enable, GT_U32 profileId)
{
    GT_STATUS                 rc=GT_OK;
    CPSS_TM_A_NODE_PARAMS_STC paramsA;
    CPSS_TM_B_NODE_PARAMS_STC paramsB;
    CPSS_TM_C_NODE_PARAMS_STC paramsC;
    CPSS_TM_QUEUE_PARAMS_STC  paramsQ;
    GT_U32 i;

    if(enable == GT_FALSE)
        profileId = CPSS_TM_NO_DROP_PROFILE_CNS;

    switch(level)
    {
    case CPSS_TM_LEVEL_Q_E:
        rc = cpssTmQueueConfigurationRead(0,index,&paramsQ);
        if (rc != GT_OK)
            return rc;

        paramsQ.dropProfileInd = profileId;

        rc = cpssTmQueueUpdate(0,index,&paramsQ);
        break;

    case CPSS_TM_LEVEL_A_E:
        rc = cpssTmAnodeConfigurationRead(0,index,&paramsA);
        if (rc != GT_OK)
            return rc;

        paramsA.dropProfileInd = profileId;

        rc = cpssTmAnodeUpdate(0,index,&paramsA);
        break;

    case CPSS_TM_LEVEL_B_E:
        rc = cpssTmBnodeConfigurationRead(0,index,&paramsB);
        if (rc != GT_OK)
            return rc;

        paramsB.dropProfileInd = profileId;

        rc = cpssTmBnodeUpdate(0,index,&paramsB);
        break;

    case CPSS_TM_LEVEL_C_E:
        rc = cpssTmCnodeConfigurationRead(0,index,&paramsC);
        if (rc != GT_OK)
            return rc;

        for (i=0; i<8; i++)
            paramsC.dropProfileIndArr[i] = profileId;

        rc = cpssTmCnodeUpdate(0,index,&paramsC);
        break;

    case CPSS_TM_LEVEL_P_E:

        rc = cpssTmPortDropUpdate(0, index, profileId);
        break;


    default:
        osPrintf("Bad level\n");
        return rc;
    }

    if (rc != GT_OK)
        return rc;
    return GT_OK;
}



/* update an existing TD profile */
GT_STATUS TDProfileUpdate(CPSS_TM_LEVEL_ENT level,
                      GT_U32 profileIndex,
                      GT_U32 cbMBytesThreshold,
                      GT_U32 caMBytesThreshold,
                      GT_U32 curveInd,
                      GT_U32 aqlExponent)
{
    GT_STATUS             rc=GT_OK;
    CPSS_TM_DROP_PROFILE_PARAMS_STC profile;
    GT_U32 i;

    
    rc = cpssTmDropProfileRead(0, level, 0, profileIndex, &profile);
    if (rc != GT_OK)
        return rc;
    
    switch (profile.dropMode)
    {
        case CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E:
            profile.cbTdThresholdBytes = cbMBytesThreshold * 1024 * 1024 ;  /* make it big - to have no influence*/
            profile.caWredDp.aqlExponent  = (GT_U8)aqlExponent;
            for (i=0; i < 3 ; i++)
            {
                profile.caWredDp.curveIndex[i] = (GT_U8)curveInd;
                profile.caWredDp.dpCurveScale[i] = 0;
                profile.caWredDp.caWredTdMinThreshold[i] = 0;
                profile.caWredDp.caWredTdMaxThreshold[i] = caMBytesThreshold * 1024 * 1024 ;
            }
            break;
        case CPSS_TM_DROP_MODE_CB_TD_CA_TD_E:
            profile.cbTdThresholdBytes = cbMBytesThreshold * 1024 ;  /* make it big - to have no influence*/
            for (i=0; i < 3 ; i++)
            {
                profile.caTdDp.caTdThreshold[i]= caMBytesThreshold * 1024 * 1024 ;
            }
            break;
        case CPSS_TM_DROP_MODE_CB_TD_ONLY_E:
            profile.cbTdThresholdBytes = cbMBytesThreshold * 1024 * 1024 ;
            break;
        default:break;
    }

    rc = cpssTmDropProfileUpdate(0, level, CPSS_TM_INVAL_CNS, profileIndex, &profile);
    if (rc != GT_OK)
        return GT_ERROR;

    return GT_OK;
}


/* dump all Drop Profiles Indexes per device  */
GT_STATUS appDemoDropProfilesIdsDump(GT_U8 devNum)
{
    GT_STATUS                       rc = GT_OK;
    CPSS_TM_DROP_PROFILE_PARAMS_STC profileParams;
    GT_U32                          profIndex;
    GT_32                           cosArr[] = {0,1,2,3,4,5,6,7,-1};
    GT_32                           fromCos, toCos, cosArrInd, cos;
    CPSS_TM_LEVEL_ENT               level;

    for (level = CPSS_TM_LEVEL_Q_E; level <= CPSS_TM_LEVEL_P_E; level++)
    {
        if (level == CPSS_TM_LEVEL_P_E)
        {
            fromCos = 0;
            toCos = 8;
        }
        else if (level == CPSS_TM_LEVEL_C_E)
        {
            fromCos = 0;
            toCos = 7;
        }
        else
        {
            fromCos = 8;
            toCos = 8;
        }

        for (cosArrInd = fromCos; cosArrInd <= toCos; cosArrInd++)
        {
            profIndex = 0,
            cos = cosArr[cosArrInd];
            rc = prvCpssTmDropProfileGetNext(devNum, level, cos, &profIndex);

            if (rc != GT_OK)
                continue;

            cpssOsPrintf("Level: %s Cos %d\n", tmTreelevelNamesArr[level], cos);

            while(rc == GT_OK)
            {
                /* verify profiles exist */
                rc = cpssTmDropProfileRead(devNum,
                                           level,
                                           cos,
                                           profIndex,
                                           &profileParams);

                if (rc != GT_OK)
                {
                    cpssOsPrintf("tm_dump_drop_profiles: level: %d, profileIndex: %d, error: %d\n", level, profIndex, rc);
                    return rc;
                }

                cpssOsPrintf("\tprofileIndex: %d, dropMode: %s\n", profIndex, tmDropModeNamesArr[profileParams.dropMode]);

                rc = prvCpssTmDropProfileGetNext(devNum, level, cos, &profIndex);
            }
        }
    }

    return GT_OK;
}

/* Print Drop Profiles parameters */
static GT_STATUS appDemoDropProfilePrint(CPSS_TM_LEVEL_ENT level, GT_U32 cos, GT_U32 profileInd, CPSS_TM_DROP_PROFILE_PARAMS_STC *profilePtr)
{
    char modeStr[64];
    GT_STATUS rc = GT_OK;

    switch(profilePtr->dropMode)
    {
    case CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E:
        osSprintf(modeStr,"ColorBlind TD + WRED Color Aware");
        osPrintf("\nDrop Profile: index: %d, level: %s, cos: %d, mode: %s\n",
                profileInd,
                tmTreelevelNamesArr[level],
                cos,
                modeStr);

        osPrintf("  CB Threshold Bytes (BW) %u (%u)\n",
                profilePtr->cbTdThresholdBytes, cpssTmDropConvertBytesToBW(profilePtr->cbTdThresholdBytes));

        osPrintf("  aqlExponent %d\n",
                profilePtr->caWredDp.aqlExponent);

        osPrintf("  WRED Curves %d, %d, %d\n",
                 profilePtr->caWredDp.curveIndex[0],
                 profilePtr->caWredDp.curveIndex[1],
                 profilePtr->caWredDp.curveIndex[2]);

        osPrintf("  MinThreshold Bytes (BW) %u (%u), %u (%u), %u (%u)\n",
                 profilePtr->caWredDp.caWredTdMinThreshold[0], cpssTmDropConvertBytesToBW(profilePtr->caWredDp.caWredTdMinThreshold[0]),
                 profilePtr->caWredDp.caWredTdMinThreshold[1], cpssTmDropConvertBytesToBW(profilePtr->caWredDp.caWredTdMinThreshold[1]),
                 profilePtr->caWredDp.caWredTdMinThreshold[2], cpssTmDropConvertBytesToBW(profilePtr->caWredDp.caWredTdMinThreshold[2]));

        osPrintf("  MaxThreshold Bytes (BW) %u (%u), %u (%u), %u (%u)\n",
                 profilePtr->caWredDp.caWredTdMaxThreshold[0], cpssTmDropConvertBytesToBW(profilePtr->caWredDp.caWredTdMaxThreshold[0]),
                 profilePtr->caWredDp.caWredTdMaxThreshold[1], cpssTmDropConvertBytesToBW(profilePtr->caWredDp.caWredTdMaxThreshold[1]),
                 profilePtr->caWredDp.caWredTdMaxThreshold[2], cpssTmDropConvertBytesToBW(profilePtr->caWredDp.caWredTdMaxThreshold[2]));

        break;

    case CPSS_TM_DROP_MODE_CB_TD_CA_TD_E:
        osSprintf(modeStr,"ColorBlind TD + Color Aware TD");
        osPrintf("\nDrop Profile: index: %d, level: %s, cos: %d, mode: %s\n",
                profileInd,
                tmTreelevelNamesArr[level],
                cos,
                modeStr);

        osPrintf("  CB Threshold Bytes (BW) %u (%u)\n",
                profilePtr->cbTdThresholdBytes, cpssTmDropConvertBytesToBW(profilePtr->cbTdThresholdBytes));

        osPrintf("  CA Threshold Bytes (BW) %u (%u), %d (%u), %u (%u),\n",
                profilePtr->caTdDp.caTdThreshold[0], cpssTmDropConvertBytesToBW(profilePtr->caTdDp.caTdThreshold[0]),
                profilePtr->caTdDp.caTdThreshold[1], cpssTmDropConvertBytesToBW(profilePtr->caTdDp.caTdThreshold[1]),
                profilePtr->caTdDp.caTdThreshold[2], cpssTmDropConvertBytesToBW(profilePtr->caTdDp.caTdThreshold[2]));

        break;

    case CPSS_TM_DROP_MODE_CB_TD_ONLY_E:
        osSprintf(modeStr,"ColorBlind TD");
        osPrintf("\nDrop Profile: index: %d, level: %s, cos: %d, mode: %s\n",
                profileInd,
                tmTreelevelNamesArr[level],
                cos,
                modeStr);

        osPrintf("  CB Threshold Bytes (BW) %u(%u)\n",
                profilePtr->cbTdThresholdBytes, cpssTmDropConvertBytesToBW(profilePtr->cbTdThresholdBytes));
        break;
    default:
        osPrintf("Unknown drop mode %d\n", profilePtr->dropMode);
        rc = GT_BAD_VALUE;
    }

    return rc;
}

/* Dump Drop Profiles parameters per device for level, cos and range of Drop Profile Indexes */
GT_STATUS appDemoDropProfilesLevelDump(GT_U8 devNum, CPSS_TM_LEVEL_ENT level, GT_U32 cos, GT_U32 from, GT_U32 to)
{
    GT_STATUS                       rc = GT_OK;
    CPSS_TM_DROP_PROFILE_PARAMS_STC profile;
    GT_U32                          profileInd;

    if (cos > 7)
    {
        cos = -1;
    }

    for (profileInd = from; profileInd <= to; profileInd++)
    {
        rc = cpssTmDropProfileRead(devNum, level, cos, profileInd, &profile);

        if (rc != GT_OK)
            return rc;

        rc = appDemoDropProfilePrint(level, cos, profileInd, &profile);
        if (rc != GT_OK)
            break;
    }

    return rc;
}

/* Dump Drop Profiles parameters per device for all Cos and All Levels for range of Drop Profile Indexes */
GT_STATUS appDemoDropProfilesDump(GT_U8 devNum, GT_U32 fromInd, GT_U32 toInd)
{
    GT_STATUS                       rc = GT_OK;
    CPSS_TM_DROP_PROFILE_PARAMS_STC profile;
    GT_U32                          profileInd;
    GT_32                           cosArr[] = {0,1,2,3,4,5,6,7,-1};
    GT_32                           fromCos, toCos, cosArrInd, cos;
    CPSS_TM_LEVEL_ENT               level;

    for (level = CPSS_TM_LEVEL_Q_E; level <= CPSS_TM_LEVEL_P_E; level++)
    {
        if (level == CPSS_TM_LEVEL_P_E)
        {
            fromCos = 0;
            toCos = 8;
        }
        else if (level == CPSS_TM_LEVEL_C_E)
        {
            fromCos = 0;
            toCos = 7;
        }
        else
        {
            fromCos = 8;
            toCos = 8;
        }

        for (cosArrInd = fromCos; cosArrInd <= toCos; cosArrInd++)
        {
            cos = cosArr[cosArrInd];
            for (profileInd = fromInd; profileInd <= toInd; profileInd++)
            {
                rc = cpssTmDropProfileRead(devNum, level, (GT_U32)cos, profileInd, &profile);

                if (rc != GT_OK)
                    continue;

                rc = appDemoDropProfilePrint(level, cos, profileInd, &profile);
                if (rc != GT_OK)
                    return rc;
            }
        }
    }
    return GT_OK;
}


GT_STATUS setDropMask(GT_U32 profileId, GT_U32 dropBitMask, GT_BOOL redDropByTM, GT_BOOL redPaint)
{

    GT_STATUS rc;
    GT_U32 i;
    CPSS_DXCH_TM_GLUE_DROP_MASK_STC dropMaskCfg;
    CPSS_DXCH_COS_PROFILE_STC       cosProfile;

    osPrintf("profileId %d, dropMask 0x%X, redDropByTM %d redPaint %d\n",profileId, dropBitMask, redDropByTM, redPaint);
    /* Drop – profile 0 – consider for drop P level Tail Drop only */
    osMemSet(&dropMaskCfg, 0, sizeof(dropMaskCfg));
    /* Set port tail drop recomenadtion for drop desision */
    dropMaskCfg.qTailDropUnmask = dropBitMask & 0x1;
    dropMaskCfg.qWredDropUnmask = (dropBitMask>>1) & 0x1;
    dropMaskCfg.aTailDropUnmask  = (dropBitMask>>2) & 0x1;
    dropMaskCfg.aWredDropUnmask = (dropBitMask>>3) & 0x1;
    dropMaskCfg.bTailDropUnmask = (dropBitMask>>4) & 0x1;
    dropMaskCfg.bWredDropUnmask = (dropBitMask>>5) & 0x1;
    dropMaskCfg.cTailDropUnmask = (dropBitMask>>6) & 0x1;
    dropMaskCfg.cWredDropUnmask = (dropBitMask>>7) & 0x1;
    dropMaskCfg.portTailDropUnmask  = (dropBitMask>>8) & 0x1;
    dropMaskCfg.portWredDropUnmask = (dropBitMask>>9) & 0x1;
    dropMaskCfg.outOfResourceDropUnmask = GT_TRUE;

    if(redDropByTM)
        dropMaskCfg.redPacketsDropMode = CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_ASK_TM_RESP_E;
    else
        dropMaskCfg.redPacketsDropMode = CPSS_DXCH_TM_GLUE_DROP_RED_PACKET_DROP_MODE_DROP_ALL_RED_E;

    /* Call CPSS API function */
    for (i=0 ; i<16; i++)
    {
        rc = cpssDxChTmGlueDropProfileDropMaskSet(0, profileId, i, &dropMaskCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTmGlueDropProfileDropMaskSet", rc);
        if(rc != GT_OK)
            return rc;
    }

    for (i=0 ; i<16383; i++)
    {
        rc = cpssDxChTmGlueDropQueueProfileIdSet(0, i, profileId);
        if(rc != GT_OK)
            osPrintf(" %d",i);
    }
    osPrintf(" \n");

    osMemSet(&cosProfile, 0, sizeof(cosProfile));

    for (i=0 ; i<8; i++)
    {
        if(redPaint)
            cosProfile.dropPrecedence = CPSS_DP_RED_E;
        else
            cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
        cosProfile.userPriority = i;
        cosProfile.trafficClass = i;
        rc = cpssDxChCosProfileEntrySet(0, i+20, &cosProfile);
        if( GT_OK != rc)
           return rc;
    }


    return GT_OK;
}


/* this is for protected path demo.  Loop prevention can be done whether through sourceID egress filtering
or through port isolation*/
GT_STATUS noLoopSet(GT_BOOL portIsolation)
{
    GT_STATUS rc;

    CPSS_INTERFACE_INFO_STC                        srcInterface;

    if(portIsolation)
    {
        rc = cpssDxChBrgVlanPortIsolationCmdSet(0, 1, CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E );
        if( GT_OK != rc)
           return rc;

        srcInterface.type = CPSS_INTERFACE_PORT_E;
        srcInterface.devPort.hwDevNum = 16;
        srcInterface.devPort.portNum = 0;

        rc = cpssDxChNstPortIsolationPortDelete(0, CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E, srcInterface, 1);
        if( GT_OK != rc)
           return rc;
        srcInterface.devPort.portNum = 1;

        rc = cpssDxChNstPortIsolationPortDelete(0, CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E, srcInterface, 0);
        if( GT_OK != rc)
           return rc;

        rc = cpssDxChNstPortIsolationEnableSet(0, GT_TRUE);
        if( GT_OK != rc)
           return rc;

    }
    else
    {
        rc = cpssDxChBrgSrcIdGroupPortDelete(0, 10, 1 );
        if( GT_OK != rc)
           return rc;

        rc = cpssDxChBrgSrcIdGroupPortDelete(0, 11, 0 );
        if( GT_OK != rc)
           return rc;

        rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(0, 0, 10);
        if( GT_OK != rc)
           return rc;

        rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(0, 1, 11);
        if( GT_OK != rc)
           return rc;
    }
    return GT_OK;
}


GT_STATUS mllCounterGet(void)
{
    GT_U32 acount, i;
    GT_STATUS rc;
    CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC expcount;

    rc = cpssDxChL2MllExceptionCountersGet(0,&expcount);
    if( GT_OK != rc)
      return rc;

    osPrintf("Exception Counter ttl: %d skip: %d\n", expcount.ttl, expcount.skip);
    for(i=0; i<2; i++)
    {
        rc = cpssDxChL2MllCounterGet(0,i,&acount);
        if( GT_OK != rc)
          return rc;
        osPrintf("Counter %d: %d\n", i, acount);
    }

    return GT_OK;
}



/* if application relies on TCAM configuration -done in cpssInit, then tcamIndex should start with 18432
   in this example we deal with traffic arriving on port1 (in case of CPU TX, it is CPU port
   and not src port in DS tag), if it is VLAN = 1, it is assigned source ePort eport2,
   if it is untagged or from another VLAN, it is dropped */

GT_STATUS TTIRuleSet(GT_U32 tcamIndex1, GT_PORT_NUM port1, GT_PORT_NUM ingressEport1, GT_U32 step,
                      GT_BOOL drop, GT_U32 scenario)
{
    GT_STATUS rc = GT_OK;
    CPSS_DXCH_TTI_RULE_UNT              pattern;
    CPSS_DXCH_TTI_RULE_UNT              mask;
    CPSS_DXCH_TTI_ACTION_2_STC          action;
    CPSS_DXCH_TTI_ACTION_UNT            actionU;
    CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType;
    CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType;
    GT_U8                               i,j;
    GT_U32                  startVlan = 1, startCVlan = 100;
    GT_U32                              startSVlan = 200;

    osMemSet(&(mask), 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    osMemSet(&(pattern), 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    osMemSet(&action, 0, sizeof(CPSS_DXCH_TTI_ACTION_2_STC));

    if(scenario == 7)/* we temprarily use legacy keys for scenario 7 untill inner VLAN mach based on meta-data will be fixed */
    {
        keyType = CPSS_DXCH_TTI_KEY_ETH_E;
        ruleType = CPSS_DXCH_TTI_RULE_ETH_E;

        pattern.eth.isVlan1Exists = GT_TRUE;
        mask.eth.isVlan1Exists = 0x1;

/* this is to mark match in TTI */
        action.command = CPSS_PACKET_CMD_FORWARD_E ;
        action.sourceEPortAssignmentEnable = GT_TRUE;
        action.sourceEPort = 9;
        action.userDefinedCpuCode = CPSS_NET_FIRST_USER_DEFINED_E + 1;
        action.keepPreviousQoS = GT_TRUE;

        action.tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;


        for(i=0;i<8;i++)
        {
            pattern.eth.vid1 = (GT_U8)(startCVlan+i);
            mask.eth.vid1 = 0xFF;
            osPrintf("Match VLAN1 %d\n", pattern.eth.vid1);
            action.flowId = i;
            actionU.type2 = action;

            rc = cpssDxChTtiRuleSet(0, tcamIndex1+i*3, ruleType, &pattern, &mask,
                                CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionU);
            if(rc != GT_OK)
                return rc;

            rc = cpssDxChTtiPortLookupEnableSet(0, port1+i, keyType, GT_TRUE);
            if(rc != GT_OK)
                return rc;
        }
        return 0;
    }

    keyType = CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E;
    ruleType = CPSS_DXCH_TTI_RULE_UDB_10_E;
    rc = cpssDxChTtiPacketTypeKeySizeSet(0, CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E, CPSS_DXCH_TTI_KEY_SIZE_10_B_E);

    if(rc != GT_OK)
        return rc;

    /* UDB for source ePort*/
    rc = cpssDxChTtiUserDefinedByteSet(0, CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E, 0,
                                       CPSS_DXCH_TTI_OFFSET_METADATA_E, 26);

    if(rc != GT_OK)
        return rc;
    /* UDB for Tag0 VLAN ID*/
    rc = cpssDxChTtiUserDefinedByteSet(0, CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E, 1,
                                       CPSS_DXCH_TTI_OFFSET_METADATA_E, 24);

    if(rc != GT_OK)
        return rc;

    /* UDB for Tag0[9..12] VLAN ID*/
    rc = cpssDxChTtiUserDefinedByteSet(0, CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E, 2,
                                       CPSS_DXCH_TTI_OFFSET_METADATA_E, 25);

    if(rc != GT_OK)
        return rc;
    /* UDB for Tag1 VLAN ID*/
    rc = cpssDxChTtiUserDefinedByteSet(0, CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E, 3,
                                       CPSS_DXCH_TTI_OFFSET_METADATA_E, 14);

    if(rc != GT_OK)
        return rc;

    if(drop == GT_TRUE)
        action.command = CPSS_PACKET_CMD_DROP_HARD_E;
    else
        action.command = CPSS_PACKET_CMD_FORWARD_E /*CPSS_PACKET_CMD_DROP_HARD_E*/;
    action.tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
    action.userDefinedCpuCode = CPSS_NET_FIRST_USER_DEFINED_E + 1;
    action.keepPreviousQoS = GT_TRUE;
    /*action.trustUp = GT_TRUE; check how it works when keepPreviousQoS == false*/

    for(i=0;i<10;i++)
    {
        rc = cpssDxChTtiPortLookupEnableSet(0, port1+i, keyType, GT_TRUE);
        if(rc != GT_OK)
            return rc;

        switch(scenario)
        {
        case 8:
            for(j=0;j<100;j++)
            {
                pattern.udbArray[1] = (GT_U8)(startVlan + i*100 + j);
                mask.udbArray[1] = 0xFF;
                action.flowId = i*100 + j + 1;
                actionU.type2 = action;
                rc = cpssDxChTtiRuleSet(0, tcamIndex1+i*100 + j, ruleType, &pattern, &mask,
                                        CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionU);
            }
            break;

        case 7:

            pattern.udbArray[1] = (startSVlan+(i/2)*200) & 0xFF;
            mask.udbArray[1] = 0xFF;
            pattern.udbArray[2] = ((startSVlan+(i/2)*200)>>8) & 0xFF;
            mask.udbArray[2] = 0x0F;

            /* inner VLAN match */
            /*
            pattern.udbArray[3] = (GT_U8)(startCVlan+i);
            mask.udbArray[3] = 0xFF;*/

            osPrintf("Match VLAN 0 %d, VLAN 1 %d\n",pattern.udbArray[2]*256 + pattern.udbArray[1], pattern.udbArray[3]);
            action.sourceEPortAssignmentEnable = GT_TRUE;
            action.sourceEPort = 9;

            action.tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;

            action.tag1VlanId = 99;

            action.flowId = i;
            actionU.type2 = action;
            rc = cpssDxChTtiRuleSet(0, tcamIndex1+i, ruleType, &pattern, &mask,
                                    CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionU);
            break;

        case 6:

            pattern.udbArray[0] = (GT_U8)((port1+i)&0x00ff);
            mask.udbArray[0] = 0xFF;
            pattern.udbArray[1] = (GT_U8)((startVlan+i)&0x00ff);
            mask.udbArray[1] = 0xFF;
            action.flowId = i+1;
            osPrintf("port %d, vlan %d, flowID = %d\n", port1+i, startVlan+i, action.flowId);
            action.keepPreviousQoS = GT_TRUE;
            action.sourceEPortAssignmentEnable = GT_TRUE;
            action.sourceEPort = (i+1)+200;

            actionU.type2 = action;

            
            rc = cpssDxChTtiRuleSet(0, tcamIndex1+i, ruleType, &pattern, &mask,
                                    CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionU);
        /* in order to see FDB learning on assigned port, make additional settings */
            rc = cpssDxChBrgFdbPortLearnStatusSet(0,(i+1)+200, GT_FALSE, CPSS_LOCK_FRWRD_E);
            if(rc != GT_OK)
                return rc;

            rc = cpssDxChBrgFdbNaToCpuPerPortSet(0, (i+1)+200, GT_TRUE);
            if(rc != GT_OK)
                return rc;
#if 0
            /*In addition create a case for flow ID that maps into Q-node, connected to port 1, then we will
                move that Q-node to tree rooted in port 0 and verify that such operation is working */

            if(i==0)
            {
                pattern.udbArray[0] = 0;
                mask.udbArray[0] = 0;

                pattern.udbArray[1] = 1;
                mask.udbArray[1] = 0xFF;
                action.command = CPSS_PACKET_CMD_FORWARD_E;
                action.flowId = ingressEport1;
                action.sourceEPortAssignmentEnable = GT_TRUE;
                action.sourceEPort = 111;
                osPrintf("Set flow ID %d\n",action.flowId);
                actionU.type2 = action;
                rc = cpssDxChTtiRuleSet(0, tcamIndex1+20, ruleType, &pattern, &mask,
                                        CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionU);
            /* in order to see FDB learning on assigned port, make additional settings */
                rc = cpssDxChBrgFdbPortLearnStatusSet(0, 111, GT_FALSE, CPSS_LOCK_FRWRD_E);
                if(rc != GT_OK)
                    return rc;

                rc = cpssDxChBrgFdbNaToCpuPerPortSet(0, 111, GT_TRUE);
                if(rc != GT_OK)
                    return rc;
            }
#endif
            break;

        case 2:

            pattern.udbArray[0] = (GT_U8)(port1+i);
            mask.udbArray[0] = 0xFF;
            action.sourceEPortAssignmentEnable = GT_TRUE;
            action.sourceEPort = ingressEport1 + i*step;
            actionU.type2 = action;
            rc = cpssDxChTtiRuleSet(0, tcamIndex1+i, ruleType, &pattern, &mask,
                                    CPSS_DXCH_TTI_ACTION_TYPE2_ENT, &actionU);
            break;

        default:
            osPrintf("unsuported TM scenario\n");
            return 1;
        }


        if(rc != GT_OK)
            return rc;

    /* in order to see FDB learning on assigned port, make additional settings */
        rc = cpssDxChBrgFdbPortLearnStatusSet(0, ingressEport1 + i*step, GT_FALSE, CPSS_LOCK_FRWRD_E);
        if(rc != GT_OK)
            return rc;

        rc = cpssDxChBrgFdbNaToCpuPerPortSet(0, ingressEport1 + i*step, GT_TRUE);
        if(rc != GT_OK)
            return rc;

        /* make these ePorts enabled for policy */
        rc = cpssDxChPclPortIngressPolicyEnable(0, ingressEport1 + i*step, GT_TRUE);
        if(rc != GT_OK)
            return rc;

    }
    return rc;

}


/* use vlanSet 200,100 for TM tree*/
GT_STATUS vlanMacSet(GT_U32 base, GT_U32 offset, GT_BOOL setFDB, GT_U32 mllPairs)
{
    GT_STATUS rc;
        /*
    CPSS_PORTS_BMP_STC              portsMembers;
    CPSS_PORTS_BMP_STC              portsTagging;
        CPSS_DXCH_BRG_VLAN_INFO_STC     cpssVlanInfo;    cpss vlan info format
        CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
        */
    CPSS_DXCH_L2_MLL_LTT_ENTRY_STC  lttEntry;
    CPSS_DXCH_L2_MLL_ENTRY_STC      mllEntry;
    CPSS_DXCH_L2_MLL_PAIR_STC       mllPairEntry;
    GT_U8                           i,j;
    CPSS_MAC_ENTRY_EXT_STC          macEntry;
    CPSS_QOS_ENTRY_STC          macQosCfg;
    CPSS_DXCH_COS_PROFILE_STC       cosProfile;
    GT_PORT_NUM         defEPort = 50;


    rc = cpssDxChL2MllLookupForAllEvidxEnableSet(0, GT_FALSE);
    if( GT_OK != rc)
      return rc;

    rc = cpssDxChBrgVlanFloodVidxModeSet(0,1,5000,CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E);
    if( GT_OK != rc)
      return rc;

    rc = cpssDxChBrgVlanUnregisteredIpmEVidxSet(0,1,CPSS_DXCH_BRG_VLAN_UNREG_IPM_EVIDX_MODE_BOTH_E, 5000);
    if( GT_OK != rc)
      return rc;

#if 0
    /* create VLAN, VLAN 1 serves both as VLAN and as eVLAN */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));
    osMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));

    cpssVlanInfo.unknownMacSaCmd = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.fidValue       = 1; /* it's a MUST!!!*/
    cpssVlanInfo.floodVidx      = 5000;
    cpssVlanInfo.floodVidxMode  = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;
    cpssVlanInfo.naMsgToCpuEn   = GT_TRUE;
    cpssVlanInfo.unregIpmEVidx = 0xFFF;

    rc = cpssDxChBrgVlanEntryWrite(0, 1, &portsMembers, &portsTagging, &cpssVlanInfo, &portsTaggingCmd);
    if( GT_OK != rc)
      return rc;

    /* make all egress ports on VLAN1 untagged - should not be tagged!!!*/
    for(i=0;i<10;i++)
    {
        rc = cpssDxChBrgVlanMemberAdd(0, 1, i, GT_TRUE, CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);
        if( GT_OK != rc)
           return rc;
    }
#endif

    /* create L2MLL groups*/
    osMemSet(&lttEntry, 0, sizeof(lttEntry));
    lttEntry.mllPointer = 2;
    lttEntry.mllMaskProfileEnable = GT_FALSE;
    rc = cpssDxChL2MllLttEntrySet(0, 5000 - 4096, &lttEntry);
    if( GT_OK != rc)
      return rc;

    osMemSet(&mllEntry, 0, sizeof(mllEntry));
    osMemSet(&mllPairEntry, 0, sizeof(mllPairEntry));
    mllPairEntry.nextPointer = 0;
    mllPairEntry.entrySelector = CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_FIRST_MLL_E;

    mllEntry.unknownUcFilterEnable = GT_FALSE;
    mllEntry.egressInterface.type = CPSS_INTERFACE_PORT_E;
    mllEntry.egressInterface.devPort.hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(0); /* the eport is on local device*/;
    mllEntry.egressInterface.devPort.portNum = 1;
    mllEntry.ttlThreshold = 0;/* must be 0*/
    mllEntry.bindToMllCounterEnable = GT_TRUE;
    mllEntry.mllCounterIndex = 0;
    mllEntry.maskBitmap = 0;
    mllEntry.last = GT_FALSE;

    for(i=0;i<mllPairs;i++)
    {
        mllPairEntry.nextPointer = 2*(2+i);

        mllEntry.egressInterface.devPort.portNum = 2*i*base + offset;
        mllEntry.mllCounterIndex = 0;
        mllPairEntry.firstMllNode = mllEntry;
        osPrintf("mll pair %d interface %d", i, 2*i*base + offset);

        mllEntry.egressInterface.devPort.portNum = (2*i+1)*base + offset;
        osPrintf("and interface %d\n", (2*i+1)*base + offset);
        mllEntry.mllCounterIndex = 1;
        if(i== (mllPairs-1) )
        {
            mllPairEntry.nextPointer = 0;
            mllEntry.last = GT_TRUE;
        }
        mllPairEntry.secondMllNode = mllEntry;
        rc = cpssDxChL2MllPairWrite(0,2*(1+i),CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E,&mllPairEntry);
        if( GT_OK != rc)
          return rc;
    }

/* create default ePort for each TM port on ingress, so that MAC learning will be possible */
    rc = TTIRuleSet(28000, 0, defEPort, 200, GT_FALSE, 1);
    if( GT_OK != rc)
      return rc;

    /*need to do source filtering */
    /* i is egress port*/

    for(i=0;i<10;i++)
    {

    /*  rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(0, i, i+1);
        if( GT_OK != rc)
           return rc;
*/
        rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(0, defEPort + i*200, i+1);
        if( GT_OK != rc)
           return rc;

        /* here we talk distination physical port*/
        rc = cpssDxChBrgSrcIdGroupPortDelete(0, i+1, i );
        if( GT_OK != rc)
           return rc;
    }

    /*
    We create configuration for 16 flows, each mapped to a different dedicated Q-node.
    We fill FDB table with 16 000 MAC entries, all on VLAN 1.
    Each entry is constructed according to the following rule:
    MAC address:
        00:00:00:01:<TC>:<ePort>
    Destination interface:
        macEntry.dstInterface.devPort.portNum = 200+ePort
    MAC QoS index:
        macEntry.daQosIndex = <TC>;

    */
    /* set MAC entries for different ePorts, all ePorts here are mapped to port 1*/
    if(setFDB == GT_TRUE)
    {
        osMemSet(&macEntry, 0, sizeof(CPSS_MAC_ENTRY_EXT_STC));
        macEntry.daRoute = GT_FALSE;
        macEntry.isStatic = GT_TRUE;
        macEntry.key.key.macVlan.macAddr.arEther[0] = 0;
        macEntry.key.key.macVlan.macAddr.arEther[1] = 0;
        macEntry.key.key.macVlan.macAddr.arEther[2] = 0;
        macEntry.key.key.macVlan.macAddr.arEther[3] = 0x01;
        macEntry.key.key.macVlan.macAddr.arEther[4] = 0x00;
        macEntry.key.key.macVlan.macAddr.arEther[5] = 0x00;
        macEntry.key.key.macVlan.vlanId = 1;
        macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
        macEntry.dstInterface.devPort.hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(0);
        macEntry.dstInterface.devPort.portNum = 200;

        /* add 1600 MAC entries - 8 for each of 200 ePorts and one for each Q-node connected to port 1*/
        for(i=0;i<200;i++)
        {
            macEntry.key.key.macVlan.macAddr.arEther[5] = i;
            macEntry.dstInterface.devPort.portNum = 200+i; /* ePorts 200 to 399 are mapped to physical port 1*/

            for(j=0;j<8;j++)
            {
                macEntry.daQosIndex = j;
                macEntry.key.key.macVlan.macAddr.arEther[4] = j;
                do
                {
                    rc = cpssDxChBrgFdbMacEntrySet(0, &macEntry);
                    if(rc != GT_OK)
                    {
                        osPrintf("MAC creaton failed rc = %d\n",rc);
                        if(rc != GT_BAD_STATE)
                            return rc;
                        osTimerWkAfter(10);
                    }
                }while(rc != GT_OK);
    /*
    #ifdef _WIN32
                osTimerWkAfter(40);
    #endif
    */
            }

        }
/*
    We create entries to CoS profiles table and MAC CoS table:
        cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
        cosProfile.userPriority = j;
        cosProfile.trafficClass = j;
        rc = cpssDxChCosProfileEntrySet(0, j+20, &cosProfile);

        macQosCfg.qosProfileId = j+20;
        macQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
        macQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        rc = cpssDxChCosMacQosEntrySet(0, j, &macQosCfg);

    This way we can send traffic changing MAC address and direct it to any queue in the tree.
*/
        osMemSet(&macQosCfg, 0, sizeof(macQosCfg));
        osMemSet(&cosProfile, 0, sizeof(cosProfile));
        for(j=1;j<8;j++)
        {
            macQosCfg.qosProfileId = j+20;
            macQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            macQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            macQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            rc = cpssDxChCosMacQosEntrySet(0, j, &macQosCfg);
            if( GT_OK != rc)
               return rc;

            cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
            cosProfile.userPriority = j;
            cosProfile.trafficClass = j;
            rc = cpssDxChCosProfileEntrySet(0, j+20, &cosProfile);
            if( GT_OK != rc)
               return rc;
        }
    }

    /* at the end - configure counters*/
    rc = configTMQCounters();
    if( GT_OK != rc)
       return rc;
    return GT_OK;
}


/* use scenario5Set 0*/
GT_STATUS scenario5Set(GT_PHYSICAL_PORT_NUM portNum, GT_PORT_NUM ePort1, GT_BOOL doDrop, GT_U32 marker)
{
    GT_STATUS rc;
    GT_U32                          i,bitIndex;
    CPSS_MAC_ENTRY_EXT_STC          macEntry;
    /*CPSS_DXCH_COS_PROFILE_STC     cosProfile;*/
        CPSS_INTERFACE_INFO_STC         physicalInfoPtr;
        CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC    entry;

    if(ePort1%4 != 1)
    {
        osPrintf ("ePort1 must be equal to 4*n+1\n");
        return GT_BAD_PARAM;
    }

    if(marker >25 )
    {
        osPrintf ("restrict marker to 24 \n");
        return GT_BAD_PARAM;
    }


    /*
    We create configuration for 4 flows, each mapped to a different dedicated Q-node.
    We fill FDB table with 4 MAC entries, all on VLAN 1.
    */
    /* set MAC entries for different ePorts, all ePorts here are mapped to port 1*/
    osMemSet(&macEntry, 0, sizeof(CPSS_MAC_ENTRY_EXT_STC));
    macEntry.daRoute = GT_FALSE;
    macEntry.isStatic = GT_TRUE;
    macEntry.key.key.macVlan.macAddr.arEther[0] = 0;
    macEntry.key.key.macVlan.macAddr.arEther[1] = 0;
    macEntry.key.key.macVlan.macAddr.arEther[2] = 0;
    macEntry.key.key.macVlan.macAddr.arEther[3] = 0x00;
    macEntry.key.key.macVlan.macAddr.arEther[4] = (GT_U8)marker;
    macEntry.key.key.macVlan.macAddr.arEther[5] = 0x11;
    macEntry.key.key.macVlan.vlanId = 1;
    macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    macEntry.dstInterface.devPort.hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(0);
    macEntry.dstInterface.devPort.portNum = ePort1;

    /* add 4 MAC entries */
    for(i=0;i<4;i++)
    {
        macEntry.key.key.macVlan.macAddr.arEther[5] = (GT_U8)(0x11+i);
        macEntry.dstInterface.devPort.portNum = ePort1+i; /* ePorts 200 to 399 are mapped to physical port 1*/

        do
        {
            rc = cpssDxChBrgFdbMacEntrySet(0, &macEntry);
            if(rc != GT_OK)
            {
                osPrintf("MAC creaton failed rc = %d\n",rc);
                if(rc != GT_BAD_STATE)
                    return rc;
                osTimerWkAfter(10);
            }
        }while(rc != GT_OK);
    }

    for (i=0; i<4; i++)
    {
        physicalInfoPtr.devPort.hwDevNum= PRV_CPSS_HW_DEV_NUM_MAC(0);
        physicalInfoPtr.devPort.portNum = portNum;
        physicalInfoPtr.type = CPSS_INTERFACE_PORT_E;
        rc= cpssDxChBrgEportToPhysicalPortTargetMappingTableSet(0 ,ePort1+i ,&physicalInfoPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    entry.queueIdBase = 0; /* will use it for full Q-remapping to a different physical port*/

    for (bitIndex = 0; bitIndex < 14; bitIndex++)
    {
        entry.bitSelectArr[bitIndex].selectType = CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TARGET_EPORT_E;
        entry.bitSelectArr[bitIndex].bitSelector = bitIndex;
    }

    rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, portNum, &entry);
    if(rc != GT_OK)
        return rc;

    if(doDrop == GT_TRUE)
        rc = setDropMask(0, 0x3FF, GT_FALSE, GT_FALSE);
    else
        rc = setDropMask(0, 0x3, GT_FALSE, GT_FALSE);
    if(rc != GT_OK)
        return rc;

    if(rc != GT_OK)
        return rc;

    /* at the end - configure counters*/
    rc = configTMQCounters();
    if( GT_OK != rc)
       return rc;

    rc = countNQueues(ePort1,1+marker*4,4);
    if( GT_OK != rc)
       return rc;

    return GT_OK;
}


/* use scenario6Set 0,3*/
/*
to set all 12 queues according to Scenario 6 requirements

    updateNodePrioWeight 0,9,0,2
    updateNodePrioWeight 0,10,0,3
    updateNodePrioWeight 0,11,0,5
    updateNodePrioWeight 0,12,5,2

    updateNodePrioWeight 0,13,0,1
    updateNodePrioWeight 0,14,0,2
    updateNodePrioWeight 0,15,0,3
    updateNodePrioWeight 0,16,5,2

    updateNodePrioWeight 0,17,0,5
    updateNodePrioWeight 0,18,0,3
    updateNodePrioWeight 0,19,0,2
    updateNodePrioWeight 0,20,5,2

to set all 3 A-nodes according to Scenario 6 requirements

    updateNodePrioWeight 1,2,18,1
    updateNodePrioWeight 1,3,10,2
    updateNodePrioWeight 1,4,11,2

to set all 2 B-nodes according to Scenario 6 requirements

    updateNodePrioWeight 2,1,10,1
    updateNodePrioWeight 2,2,19,2

set shapers
    nodeShaperEnable 0,12,1,1
    nodeShaperEnable 0,16,1,1
    nodeShaperEnable 0,20,1,1
    updateShaper 0,1,100000,8,0,0

    nodeShaperEnable 1,2,1,11
    updateShaper 1,11,100000,16,0,0

    nodeShaperEnable 2,2,1,21
    updateShaper 2,21,200000,26,0,0

    updateShaper 4,0,800000,46,0,0

*/
GT_STATUS scenario6Set(GT_PHYSICAL_PORT_NUM portNum, GT_32 dropMask)
{
    GT_STATUS rc;
    CPSS_DXCH_BRG_VLAN_INFO_STC     cpssVlanInfo;   /* cpss vlan info format    */
    CPSS_PORTS_BMP_STC              portsMembers;
    CPSS_PORTS_BMP_STC              portsTagging;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
    GT_U16                          i, bitIndex;
    GT_U16                          vlanId;
    CPSS_DXCH_COS_PROFILE_STC       cosProfile;
    /*CPSS_INTERFACE_INFO_STC         physicalInfoPtr;*/
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC    entry;

    /* Fill Vlan info for additonal VLAN */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));
    osMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));

    cpssVlanInfo.naMsgToCpuEn           = GT_TRUE;
    cpssVlanInfo.autoLearnDisable       = GT_TRUE; /* Disable auto learn on VLAN */
    cpssVlanInfo.ucastLocalSwitchingEn  = GT_TRUE;
    cpssVlanInfo.mcastLocalSwitchingEn  = GT_TRUE;
    cpssVlanInfo.floodVidx              = 0xfff;
    cpssVlanInfo.unregIpmEVidx          = 0xfff;

    for(vlanId=1; vlanId<=10; vlanId++)
    {
        cpssVlanInfo.fidValue               = vlanId;
        rc = cpssDxChBrgVlanEntryWrite(0,vlanId,&portsMembers,&portsTagging,
                                       &cpssVlanInfo,
                                       &portsTaggingCmd);
        if (rc != GT_OK)
            return rc;

/* for simplicity we add just one port to the VLAN, so we won;t get multiple copies of packet to this port */
        rc = cpssDxChBrgVlanMemberAdd(0, vlanId, portNum, GT_TRUE,CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E);
        if (rc != GT_OK)
            return rc;
    }

/* create default FlowId for each vlan and ingress port */
    rc = TTIRuleSet(18432, 0, dropMask/*temporary!!!!*/, 1, GT_FALSE, 6);
    if( GT_OK != rc)
      return rc;


    /* TC assignment will be based on UP*/
    for(i=1; i<10; i++)
    {
        rc = cpssDxChCosPortQosTrustModeSet(0, i, CPSS_QOS_PORT_TRUST_L2_E);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosPortTrustQosMappingTableIndexSet(0, i, GT_FALSE, 6);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosL2TrustModeVlanTagSelectSet(0, i, GT_TRUE, CPSS_VLAN_TAG0_E);
        if( GT_OK != rc)
           return rc;
    }

    osMemSet(&cosProfile, 0, sizeof(cosProfile));
    for(i=0; i<8; i++)
    {
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 0, 40+i);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 1, 40+i);
        if(rc != GT_OK)
            return rc;

        cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
        cosProfile.userPriority = 5;
        cosProfile.trafficClass = i;
        rc = cpssDxChCosProfileEntrySet(0, 40+i, &cosProfile);
        if( GT_OK != rc)
           return rc;
/* just to be sure ...*/
/*      rc = cpssDxChCosProfileEntrySet(0, i, &cosProfile);
        if( GT_OK != rc)
           return rc;*/
    }

    entry.queueIdBase = 1;
    for (bitIndex = 0; bitIndex < 14; bitIndex++)
    {
        GT_U8 bitOffset = 2;
        if(bitIndex < bitOffset)
        {
            /* Bits[0...1] - TM TC select type */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex;
        }
        else
        {
            /* Bits[2...13] - Flow ID */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_FLOW_ID_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex - bitOffset;
        }
    }

    rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, portNum, &entry);
    if(rc != GT_OK)
        return rc;
/*
    rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, portNum+1, &entry);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, portNum+2, &entry);
    if(rc != GT_OK)
        return rc;
*/
    rc = setDropMask(0, dropMask, GT_FALSE, GT_FALSE);
    if(rc != GT_OK)
        return rc;

    /* at the end - configure counters*/
    rc = configTMQCounters();
    if( GT_OK != rc)
       return rc;

    rc = countNQueues(0,0,80);
    if( GT_OK != rc)
       return rc;

    return GT_OK;
}

/* use scenario7Set 0,3*/
/* use the following set of commands to do the rest configuration of scenario 7

set TD profiles and WRED curves

    WREDCurveCreate 20
    WREDCurveCreate 40
    WREDCurveCreate 60
    WREDCurveCreate 80

    TDProfileCreate 0,2,500,500,0,0
    TDProfileCreate 0,0,500,500,1,0
    TDProfileCreate 0,0,500,500,1,7
    TDProfileCreate 0,0,500,500,2,0
    TDProfileCreate 0,0,500,500,3,0
    TDProfileCreate 0,0,500,500,4,0
    TDProfileCreate 0,0,500,500,4,7
    TDProfileCreate 0,0,500,500,4,14

check that setting worked as expected

    TDProfileGet 0,0,7,0

assign TD profiles

    nodeTDSet 0,24,1,8
    nodeTDSet 0,23,1,7
    nodeTDSet 0,22,1,6
    nodeTDSet 0,21,1,5
    nodeTDSet 0,20,1,4
    nodeTDSet 0,19,1,3
    nodeTDSet 0,18,1,2
    nodeTDSet 0,17,1,1

demonstrate how wred works - disable a queue node, send packets, read node counters
see how many packets are dropped, you may return on this several times, read DRAM
enable queue, see that all packets passed, read mode counter again

    updateNodePrioWeight 0,24,63,1

here send traffic burst - around 500 packets - to Q 24

    getTMQCounters 0
    getTMMemUse

    updateNodePrioWeight 0,24,0,1
*/
GT_STATUS scenario7Set(GT_PHYSICAL_PORT_NUM portNum, GT_32 dropMask)
{
    GT_STATUS rc;
    CPSS_DXCH_BRG_VLAN_INFO_STC     cpssVlanInfo;   /* cpss vlan info format    */
    CPSS_PORTS_BMP_STC              portsMembers;
    CPSS_PORTS_BMP_STC              portsTagging;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
    GT_U32                          i, bitIndex;
    GT_U16                          vlanId;
    CPSS_DXCH_COS_PROFILE_STC       cosProfile;
    /*CPSS_INTERFACE_INFO_STC       physicalInfoPtr;*/
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC    entry;

    /* Fill Vlan info for additonal VLAN */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));
    osMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));

    cpssVlanInfo.naMsgToCpuEn           = GT_TRUE;
    cpssVlanInfo.autoLearnDisable       = GT_TRUE; /* Disable auto learn on VLAN */
    cpssVlanInfo.ucastLocalSwitchingEn  = GT_TRUE;
    cpssVlanInfo.mcastLocalSwitchingEn  = GT_TRUE;
    cpssVlanInfo.floodVidx              = 0xfff;
    cpssVlanInfo.unregIpmEVidx          = 0xfff;

    /* configure S-vlans*/
    for(vlanId=200; vlanId<=800; vlanId += 200)
    {
        cpssVlanInfo.fidValue               = vlanId;
        rc = cpssDxChBrgVlanEntryWrite(0,vlanId,&portsMembers,&portsTagging,
                                       &cpssVlanInfo,
                                       &portsTaggingCmd);
        if (rc != GT_OK)
            return rc;

/* for simplicity we add just one port to the VLAN, so we won't get multiple copies of packet to this port */
        rc = cpssDxChBrgVlanMemberAdd(0, vlanId, portNum, GT_TRUE,CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E);
        if (rc != GT_OK)
            return rc;

    }

/* create default FlowId for each vlan and ingress port */
    rc = TTIRuleSet(24000, 0, 0, 1, GT_FALSE, 7);
    if( GT_OK != rc)
      return rc;

    /* TC assignment will be based on UP*/
    for(i=1; i<10; i++)
    {
/*      rc = cpssDxChBrgVlanPortVidSet(0, i, CPSS_DIRECTION_INGRESS_E, i+1);
        if (rc != GT_OK)
            return rc;
        rc = cpssDxChBrgVlanMemberAdd(0, i+1, i, GT_TRUE,CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E);
        if (rc != GT_OK)
            return rc;
*/
        rc = cpssDxChCosPortQosTrustModeSet(0, i, CPSS_QOS_PORT_TRUST_L2_E);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosPortTrustQosMappingTableIndexSet(0, i, GT_FALSE, 6);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosL2TrustModeVlanTagSelectSet(0, i, GT_TRUE, CPSS_VLAN_TAG0_E);
        if( GT_OK != rc)
           return rc;
    }

    osMemSet(&cosProfile, 0, sizeof(cosProfile));
    for(i=0; i<8; i++)
    {
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 0, 40+i);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 1, 40+i);
        if(rc != GT_OK)
            return rc;

        cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
        cosProfile.userPriority = 5;
        cosProfile.trafficClass = i;
        rc = cpssDxChCosProfileEntrySet(0, 40+i, &cosProfile);
        if( GT_OK != rc)
           return rc;
/* just to be sure ...*/
/*      rc = cpssDxChCosProfileEntrySet(0, i, &cosProfile);
        if( GT_OK != rc)
           return rc;*/
    }

    entry.queueIdBase = 1;
    for (bitIndex = 0; bitIndex < 14; bitIndex++)
    {
        GT_U8 bitOffset = 3;
        if(bitIndex < bitOffset)
        {
            /* Bits[0...2] - TM TC select type */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex;
        }
        else
        {
            /* Bits[3...13] - Flow ID */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_FLOW_ID_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex - bitOffset;
        }
    }

    rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, portNum, &entry);
    if(rc != GT_OK)
        return rc;

    rc = setDropMask(0, dropMask, GT_FALSE, GT_FALSE);
    if(rc != GT_OK)
        return rc;

    /* at the end - configure counters*/
    rc = configTMQCounters();
    if( GT_OK != rc)
       return rc;

    rc = countNQueues(0,0,80);
    if( GT_OK != rc)
       return rc;

    return GT_OK;
}


/* use scenario8Set 0,3*/
GT_STATUS scenario8Set(GT_PHYSICAL_PORT_NUM portNum, GT_32 dropMask)
{
    GT_STATUS rc;
    CPSS_DXCH_BRG_VLAN_INFO_STC     cpssVlanInfo;   /* cpss vlan info format    */
    CPSS_PORTS_BMP_STC              portsMembers;
    CPSS_PORTS_BMP_STC              portsTagging;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
    GT_U32                          i, bitIndex;
    GT_U16                          vlanId;
    CPSS_DXCH_COS_PROFILE_STC       cosProfile;
    /*CPSS_INTERFACE_INFO_STC       physicalInfoPtr;*/
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC    entry;
    CPSS_DXCH_PORT_MAP_STC     portMap[3];
    GT_U32                      queueId;

    /* Fill Vlan info for additonal VLAN */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));
    osMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));

    cpssVlanInfo.naMsgToCpuEn           = GT_TRUE;
    cpssVlanInfo.autoLearnDisable       = GT_TRUE; /* Disable auto learn on VLAN */
    cpssVlanInfo.ucastLocalSwitchingEn  = GT_TRUE;
    cpssVlanInfo.mcastLocalSwitchingEn  = GT_TRUE;
    cpssVlanInfo.floodVidx              = 0xfff;
    cpssVlanInfo.unregIpmEVidx          = 0xfff;

    /* configure vlans*/
    for(vlanId=2; vlanId<=1000; vlanId++)
    {
        cpssVlanInfo.fidValue               = vlanId;
        rc = cpssDxChBrgVlanEntryWrite(0,vlanId,&portsMembers,&portsTagging,
                                       &cpssVlanInfo,
                                       &portsTaggingCmd);
        if (rc != GT_OK)
            return rc;

/* for simplicity we add just one port to the VLAN, so we won;t get multiple copies of packet to this port */
        rc = cpssDxChBrgVlanMemberAdd(0, vlanId, portNum, GT_TRUE,CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E);
        if (rc != GT_OK)
            return rc;

    }

/* create default FlowId for each vlan */
    rc = TTIRuleSet(20000, 0, 0, 1, GT_FALSE, 8);
    if( GT_OK != rc)
      return rc;

    /* TC assignment will be based on UP*/
    for(i=1; i<10; i++)
    {
        rc = cpssDxChCosPortQosTrustModeSet(0, i, CPSS_QOS_PORT_TRUST_L2_E);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosPortTrustQosMappingTableIndexSet(0, i, GT_FALSE, 6);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosL2TrustModeVlanTagSelectSet(0, i, GT_TRUE, CPSS_VLAN_TAG0_E);
        if( GT_OK != rc)
           return rc;
    }

    osMemSet(&cosProfile, 0, sizeof(cosProfile));
    for(i=0; i<8; i++)
    {
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 0, 40+i);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 1, 40+i);
        if(rc != GT_OK)
            return rc;

        cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
        cosProfile.userPriority = 5;
        cosProfile.trafficClass = i;
        rc = cpssDxChCosProfileEntrySet(0, 40+i, &cosProfile);
        if( GT_OK != rc)
           return rc;
    }

    entry.queueIdBase = 1;
    for (bitIndex = 0; bitIndex < 14; bitIndex++)
    {
        if(bitIndex < 3)
        {
            /* Bits[0...2] - TM TC select type */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex;
        }
        else
        {
            /* Bits[3...13] - Flow ID */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_FLOW_ID_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex - 3;
        }
    }

    rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, portNum, &entry);
    if(rc != GT_OK)
        return rc;

    /* make 3 ports above portNum TM-disabled */
    osMemSet(portMap, 0, sizeof(portMap));
    for(i=0;i<3;i++)
    {

        portMap[i].tmEnable = GT_FALSE;
        portMap[i].physicalPortNumber = portNum + 1 + i;
        portMap[i].mappingType  = CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E;
        portMap[i].tmPortInd        = 0; /* do we care? */
        portMap[i].interfaceNum     = portNum + 1 + i;/* it represents the mapping of TM port to physical port for egress*/
        portMap[i].txqPortNumber    = portNum + 1 + i;
        portMap[i].portGroup    = 0;

        /* check if the table still affect behavior */
        entry.queueIdBase = 1;
        for (bitIndex = 0; bitIndex < 14; bitIndex++)
        {
            if(bitIndex < 14)
            {
                entry.bitSelectArr[bitIndex].selectType =
                    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ZERO_E;
                entry.bitSelectArr[bitIndex].bitSelector = bitIndex;
            }

        }

        rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, portNum + 1 + i, &entry);
        if(rc != GT_OK)
            return rc;
    }
/*  rc = cpssDxChPortPhysicalPortMapSet(0,3,portMap);
    if(rc != GT_OK)
        return rc;
*/

    rc = setDropMask(0, dropMask, GT_FALSE, GT_FALSE);
    if(rc != GT_OK)
        return rc;

/* configure attributes of TM tree - here not in script because of ammount of A-nodes */

    for(vlanId=1; vlanId<=1000; vlanId++)
    {
        for(i=0; i<8; i++)
        {
            queueId = 1+(vlanId-1)*8+i;

            rc = updateShaper(CPSS_TM_LEVEL_Q_E, queueId, (i+1)*10000,  20 + vlanId, 0, 0);

            if (rc != GT_OK)
                return rc;

/* queueId is equal to shaping profile ID */
            rc = nodeShaperEnable(CPSS_TM_LEVEL_Q_E, queueId, GT_TRUE, queueId);
            if (rc != GT_OK)
                return rc;

            rc = updateNodePrioWeight(CPSS_TM_LEVEL_Q_E, queueId,CPSS_TM_ELIG_Q_SHP_SCHED00_PROP00,1);
            if (rc != GT_OK)
                return rc;
        }
    }

    /* at the end - configure counters*/
    rc = configTMQCounters();
    if( GT_OK != rc)
       return rc;

    rc = countNQueues(0,0,80);
    if( GT_OK != rc)
       return rc;

    return GT_OK;
}

/*
demonstrate the aging and delay feature with simple configuration, use in conjunction with scenarios above, such as  scenario6Set 0,3
use as agingScenarioSet 16, 1000
*/
GT_STATUS agingScenarioSet(GT_U32 qIndex, GT_U32 highestBW )
{
    GT_STATUS rc=GT_OK;
    GT_U32                                              resolution = 1000, i;
    CPSS_TM_DROP_PROFILE_PARAMS_STC                     profileArr[CPSS_TM_AGING_PROFILES_CNS];
    GT_U32                                              agingBlockIndex;
    GT_U32                                              agingProfileIndex = 1;
    CPSS_DXCH_TM_AGING_AND_DELAY_PROFILE_THRESHOLDS_STC thresholds;

    for(i = 0; i<CPSS_TM_AGING_PROFILES_CNS; i++)
    {
        osMemSet(&(profileArr[i]), 0, sizeof(CPSS_TM_DROP_PROFILE_PARAMS_STC));
        profileArr[i].dropMode =  CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
        profileArr[i].cbTdThresholdBytes = cpssTmDropConvertBWToBytes(highestBW/(i+1));

    }
    rc = cpssTmDropProfileAgingBlockCreate(0, profileArr, &agingBlockIndex);
    if( GT_OK != rc)
       return rc;
    osPrintf("Aging Block created with index %d \n", agingBlockIndex);

    rc = nodeTDSet(CPSS_TM_LEVEL_Q_E, qIndex, GT_TRUE, agingBlockIndex);
    if( GT_OK != rc)
       return rc;

    rc = cpssDxChTmGlueAgingAndDelayQueueAgingProfileIdSet(0, qIndex, agingProfileIndex);
    if( GT_OK != rc)
       return rc;


    rc = cpssTmAgingChangeStatus(0, 1);
    if( GT_OK != rc)
       return rc;

    rc = cpssDxChTmGlueAgingAndDelayTimerResolutionSet(0, &resolution);
    if( GT_OK != rc)
       return rc;

    osPrintf("Aging Timer resolution set to %d nanoseconds\n", resolution);

    thresholds.threshold0 = 1000;
    thresholds.threshold1 = 2000;
    thresholds.threshold2 = 3000;

    rc = cpssDxChTmGlueAgingAndDelayAgingProfileThresholdsSet(0, agingProfileIndex, &thresholds);
    if( GT_OK != rc)
       return rc;

    rc = cpssDxChTmGlueAgingAndDelayDropAgedPacketEnableSet(0, GT_TRUE);
    if( GT_OK != rc)
       return rc;


    rc = cpssDxChTmGlueAgingAndDelayAgedPacketsCountingEnableSet(0, GT_TRUE);
    if( GT_OK != rc)
       return rc;


    return GT_OK;
}

/* use scenario9Set 0,3 - scenario for PFC demo, pfcIngressPort=0 is where PFC packet enters the board, portNum is where we send TM traffic, that
    should stop egressing on port 0, when Xoff is received on port 0
    PFC send rate 100 packets/second
    */
GT_STATUS scenario9Set(GT_PHYSICAL_PORT_NUM portNum, GT_32 dropMask)
{
    GT_STATUS rc;
    GT_U32                          i, bitIndex;
    CPSS_DXCH_COS_PROFILE_STC       cosProfile;
    /*CPSS_INTERFACE_INFO_STC       physicalInfoPtr;*/
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC    entry;
    GT_PHYSICAL_PORT_NUM            pfcPortNum = 1, pfcIngressPort = 0;


    /* TC assignment will be based on UP*/
    for(i=1; i<10; i++)
    {
        rc = cpssDxChCosPortQosTrustModeSet(0, i, CPSS_QOS_PORT_TRUST_L2_E);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosPortTrustQosMappingTableIndexSet(0, i, GT_FALSE, 6);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosL2TrustModeVlanTagSelectSet(0, i, GT_TRUE, CPSS_VLAN_TAG0_E);
        if( GT_OK != rc)
           return rc;
    }

    osMemSet(&cosProfile, 0, sizeof(cosProfile));
    for(i=0; i<8; i++)
    {
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 0, 40+i);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 1, 40+i);
        if(rc != GT_OK)
            return rc;

        cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
        cosProfile.userPriority = 5;
        cosProfile.trafficClass = i;
        rc = cpssDxChCosProfileEntrySet(0, 40+i, &cosProfile);
        if( GT_OK != rc)
           return rc;
    }

    entry.queueIdBase = 1;
    for (bitIndex = 0; bitIndex < 14; bitIndex++)
    {
        if(bitIndex < 3)
        {
            /* Bits[0...2] - TM TC select type */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex;
        }
        else
        {
            /* Bits[3...13] - Flow ID */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_LOCAL_TARGET_PHY_PORT_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex - 3;
        }
    }

    rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, portNum, &entry);
    if(rc != GT_OK)
        return rc;

    rc = setDropMask(0, dropMask, GT_FALSE, GT_FALSE);
    if(rc != GT_OK)
        return rc;

/* configure PFC TM glue layer */
    rc = cpssDxChTmGluePfcResponseModeSet(0, 0, CPSS_DXCH_TM_GLUE_PFC_RESPONSE_MODE_TM_E);
    if(rc != GT_OK)
        return rc;

/* we use just one port for demo */
    rc = cpssDxChTmGluePfcPortMappingSet(0, pfcIngressPort, pfcPortNum);
    if(rc != GT_OK)
        return rc;
/*  PFC port + tc   mapping to C-node should be in agreement with packet mapping to Q-node, where
    C-node is Q-node ancestor */
    /* C-nodes start from 0 */
    for(i=0;i<8;i++)
    {
        rc = cpssDxChTmGluePfcTmTcPort2CNodeSet(0,pfcPortNum,i,i);
        if(rc != GT_OK)
            return rc;
    }

/* configure PFC */
    rc = cpssDxChPortPfcEnableSet(0,CPSS_DXCH_PORT_PFC_ENABLE_TRIGGERING_AND_RESPONSE_E);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChPortPfcForwardEnableSet(0, pfcIngressPort, GT_TRUE);
    if(rc != GT_OK)
        return rc;
    rc = cpssDxChPortFlowControlModeSet(0, pfcIngressPort, CPSS_DXCH_PORT_FC_MODE_PFC_E);
    if(rc != GT_OK)
        return rc;
    
/* maybe should get the speed of the physical port, but that's just an example */
    rc = cpssDxChTmGlueFlowControlPortSpeedSet(0, pfcPortNum, CPSS_PORT_SPEED_1000_E);
    if(rc != GT_OK)
        return rc;

    /* at the end - configure counters*/
    rc = configTMQCounters();
    if( GT_OK != rc)
       return rc;

    rc = countNQueues(0,0,80);
    if( GT_OK != rc)
       return rc;

    return GT_OK;
}


GT_STATUS scenarioRev4Set(GT_32 dropMask)
{
    GT_STATUS rc;
    GT_U32                          i, bitIndex;
    GT_U16                          vlanId; 
    CPSS_DXCH_COS_PROFILE_STC       cosProfile;
    /*CPSS_INTERFACE_INFO_STC       physicalInfoPtr;*/
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC    entry;
    CPSS_PORTS_BMP_STC              portsMembers;
    CPSS_PORTS_BMP_STC              portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC     cpssVlanInfo;   
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;


    /* create 3 VLANs, such that only two ports - 64 and another are in each VLAN */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));
    osMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));

    cpssVlanInfo.unknownMacSaCmd = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.floodVidx      = 0xFFF;
    cpssVlanInfo.floodVidxMode  = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;
    cpssVlanInfo.naMsgToCpuEn   = GT_TRUE;
    cpssVlanInfo.unregIpmEVidx = 0xFFF;

    
    for(i = 0; i < 4; i++)
    {
        vlanId = (GT_U16)(2+i);
        cpssVlanInfo.fidValue = vlanId; /* it's a MUST!!!*/
        

        rc = cpssDxChBrgVlanEntryWrite(0, vlanId, &portsMembers, &portsTagging, &cpssVlanInfo, &portsTaggingCmd);
        if( GT_OK != rc)
          return rc;
        
        rc = cpssDxChBrgVlanMemberAdd(0, vlanId, 64, GT_TRUE, CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E);
        if( GT_OK != rc)
           return rc;
        
        rc = cpssDxChBrgVlanMemberAdd(0, vlanId, 64+i, GT_TRUE, CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E);
        if( GT_OK != rc)
           return rc;
        
        rc = cpssDxChBrgVlanPortVidSet(0, 64+i, CPSS_DIRECTION_INGRESS_E, vlanId);
        if( GT_OK != rc)
           return rc;
    }

    /* TC assignment will be based on UP*/
    for(i=0; i<4; i++)
    {
        rc = cpssDxChCosPortQosTrustModeSet(0, 64+i, CPSS_QOS_PORT_TRUST_L2_E);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosPortTrustQosMappingTableIndexSet(0, 64+i, GT_FALSE, 6);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosL2TrustModeVlanTagSelectSet(0, 64+i, GT_TRUE, CPSS_VLAN_TAG0_E);
        if( GT_OK != rc)
           return rc;
    }

    osMemSet(&cosProfile, 0, sizeof(cosProfile));
    for(i=0; i<8; i++)
    {
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 0, 40+i);
        if(rc != GT_OK)
            return rc;
        rc = cpssDxChCosUpCfiDeiToProfileMapSet(0, 6, 0, (GT_U8)i, 1, 40+i);
        if(rc != GT_OK)
            return rc;

        cosProfile.dropPrecedence = CPSS_DP_GREEN_E;
        cosProfile.userPriority = 5;
        cosProfile.trafficClass = i;
        rc = cpssDxChCosProfileEntrySet(0, 40+i, &cosProfile);
        if( GT_OK != rc)
           return rc;
    }

    entry.queueIdBase = 1;
    for (bitIndex = 0; bitIndex < 14; bitIndex++)
    {
        if(bitIndex < 3)
        {
            /* Bits[0...2] - TM TC select type */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_TM_TC_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex;
        }
        else
        {
            /* Bits[3...13] - 0 */
            entry.bitSelectArr[bitIndex].selectType =
                CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ZERO_E;
            entry.bitSelectArr[bitIndex].bitSelector = bitIndex - 3;
        }
    }

    for(i=0;i<4;i++)
    {
        rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(0, 64+i, &entry);
        if(rc != GT_OK)
            return rc;
    }

    rc = setDropMask(0, dropMask, GT_FALSE, GT_FALSE);
    if(rc != GT_OK)
        return rc;


    /* at the end - configure counters*/
    rc = configTMQCounters();
    if( GT_OK != rc)
       return rc;

    rc = countNQueues(0,0,80);
    if( GT_OK != rc)
       return rc;

    return GT_OK;
}


/* enable RX and TX mirror for debug purposes
Mirror port is assigned on ingress ePort, analyzerPort is a physical port in simulation only (bug)*/
GT_STATUS setRxTxMirror(GT_PORT_NUM mirrorRxPort, GT_PORT_NUM analyzerRxPort, GT_U32 analyzerRxIndex,
                        GT_PORT_NUM mirrorTxPort, GT_PORT_NUM analyzerTxPort, GT_U32 analyzerTxIndex)
{
    GT_STATUS rc;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC interfaceMirror;

    osPrintf("analyzerRxIndex %d analyzerTxIndex %d\n", analyzerRxIndex, analyzerTxIndex);

    rc = cpssDxChMirrorAnalyzerMirrorOnDropEnableSet(0,analyzerRxIndex,GT_TRUE);
    if( GT_OK != rc)
       return rc;
    rc = cpssDxChMirrorAnalyzerMirrorOnDropEnableSet(0,analyzerTxIndex,GT_TRUE);
    if( GT_OK != rc)
       return rc;

    rc = cpssDxChMirrorToAnalyzerForwardingModeSet(0,
            CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);
    if( GT_OK != rc)
       return rc;

    rc = cpssDxChMirrorRxPortSet(0, mirrorRxPort, GT_TRUE, GT_TRUE, analyzerRxIndex);
    if( GT_OK != rc)
       return rc;

/* possibly does not work with ePort? */
    rc = cpssDxChMirrorTxPortSet(0, mirrorTxPort, GT_TRUE, GT_TRUE, analyzerTxIndex);
    if( GT_OK != rc)
       return rc;

    interfaceMirror.interface.type = CPSS_INTERFACE_PORT_E;
    interfaceMirror.interface.devPort.hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(0);
    interfaceMirror.interface.devPort.portNum = analyzerRxPort;

    rc = cpssDxChMirrorAnalyzerInterfaceSet(0, analyzerRxIndex, &interfaceMirror);
    if( GT_OK != rc)
       return rc;

    interfaceMirror.interface.devPort.portNum = analyzerTxPort;

    rc = cpssDxChMirrorAnalyzerInterfaceSet(0, analyzerTxIndex, &interfaceMirror);
    if( GT_OK != rc)
       return rc;
/*
    rc = cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet(0, GT_TRUE, analyzerRxIndex);
    if( GT_OK != rc)
       return rc;

    rc = cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(0, GT_TRUE, analyzerTxIndex);
    if( GT_OK != rc)
       return rc;
*/
    return rc;
}


/* change LAD params settings (relevant before cpssInitSystem)
   and change number of active LAD Params. (relevant after cpssInitSystem) */

extern GT_STATUS setDRAMIntNumber(GT_U32 intNum);

GT_STATUS tmSetLadParams(GT_U8 numOfLads, GT_U32 minPkgSize, GT_U32 pagesPerBank, GT_U32 pkgesPerBank)
{
    GT_STATUS rc = GT_OK;
    CPSS_TM_CTL_LAD_INF_PARAM_STC ladParams;

    osMemSet(&ladParams, 0, sizeof(ladParams));

    ladParams.minPkgSize = minPkgSize;
    ladParams.pagesPerBank = pagesPerBank;
    ladParams.pkgesPerBank = pkgesPerBank;

    rc = cpssTmCtlLadParamsSet(0, numOfLads, &ladParams);
    if( GT_OK != rc)
       return rc;

    setDRAMIntNumber(numOfLads);
   
    return rc;
}

GT_STATUS tmDumpLadParams(GT_U8 numOfLads)
{
    GT_STATUS rc = GT_OK;
    CPSS_TM_CTL_LAD_INF_PARAM_STC ladParams;

    osMemSet(&ladParams, 0, sizeof(ladParams));

    rc = cpssTmCtlLadParamsGet(0, numOfLads, &ladParams);

    if( GT_OK != rc)
       return rc;

    osPrintf("numOfLads:            0x%0x\n", numOfLads);
    osPrintf("minPkgSize:           0x%0x\n", ladParams.minPkgSize);
    osPrintf("pagesPerBank:         0x%0x\n", ladParams.pagesPerBank);
    osPrintf("pkgesPerBank:         0x%0x\n", ladParams.pkgesPerBank);
    osPrintf("portChunksEmitPerSel: 0x%0x\n", ladParams.portChunksEmitPerSel);

    return rc;
}

GT_STATUS tmDumpLadParamsHW(void)
{
    GT_STATUS rc = GT_OK;
    CPSS_TM_CTL_LAD_INF_PARAM_STC ladParams;
    GT_U32 numOfLads;

    osMemSet(&ladParams, 0, sizeof(ladParams));

    rc = prvCpssTmCtlLadParamsGetHW(0, &numOfLads, &ladParams);

    if( GT_OK != rc)
        return rc;

    osPrintf("numOfLads:            0x%0x\n", numOfLads);
    osPrintf("minPkgSize:           0x%0x\n", ladParams.minPkgSize);
    osPrintf("pagesPerBank:         0x%0x\n", ladParams.pagesPerBank);
    osPrintf("pkgesPerBank:         0x%0x\n", ladParams.pkgesPerBank);
    osPrintf("portChunksEmitPerSel: 0x%0x\n", ladParams.portChunksEmitPerSel);

    return rc;
}

GT_STATUS tmGetTmUnitsErrors(GT_U8                              devNum, 
                             CPSS_TM_UNITS_ERROR_STATUS_STC     *tmUnitsErrorPtr, 
                             CPSS_TM_BAP_UNIT_ERROR_STATUS_STC  *tmBapErrorPtr,
                             GT_BOOL                            *errorOccuredPtr)
{
    GT_STATUS   rc = GT_OK;
    int         i;

    rc = cpssTmUnitsErrorStatusGet(devNum, tmUnitsErrorPtr);
    if (rc != GT_OK)
    {
        osPrintf("cpssTmUnitsErrorStatusGet: error: %d\n", rc); 
        return rc;
    }

    if (tmUnitsErrorPtr->qmngrStatus)
        *errorOccuredPtr = GT_TRUE;
    if (tmUnitsErrorPtr->dropStatus)
        *errorOccuredPtr = GT_TRUE;
    if (tmUnitsErrorPtr->schedStatus)
        *errorOccuredPtr = GT_TRUE;
    if (tmUnitsErrorPtr->rcbStatus)
        *errorOccuredPtr = GT_TRUE;

    for (i = 1; i < CPSS_TM_CTL_MAX_NUM_OF_BAPS_CNS; i++)
    {
        rc = cpssTmBapUnitErrorStatusGet(devNum, i, tmBapErrorPtr); 
        if (rc != GT_OK)
        {
            if (rc == GT_NO_RESOURCE)
                continue;

            osPrintf("cpssTmBapUnitErrorStatusGet: bap: %d, error: %d\n", i, rc); 
            return rc;
        }

        if (tmBapErrorPtr->bapStatus)
        {
            *errorOccuredPtr = GT_TRUE;
            break;
        }
    }

    return rc;
}

static void tmPrintTmUnitsErrors
(
    CPSS_TM_UNITS_ERROR_STATUS_STC      *tmUnitsErrorPtr,   
    CPSS_TM_BAP_UNIT_ERROR_STATUS_STC   *tmBapErrorPtr
)
{
    if (tmUnitsErrorPtr->qmngrStatus)
        osPrintf("qmngrStatus: 0x%0x\n", tmUnitsErrorPtr->qmngrStatus); 
    if (tmUnitsErrorPtr->dropStatus)
        osPrintf("dropStatus:  0x%0x\n", tmUnitsErrorPtr->dropStatus); 
    if (tmUnitsErrorPtr->schedStatus)
        osPrintf("schedStatus: 0x%0x\n", tmUnitsErrorPtr->schedStatus); 
    if (tmUnitsErrorPtr->rcbStatus)
        osPrintf("rcbStatus:   0x%0x\n", tmUnitsErrorPtr->rcbStatus); 

    if (tmBapErrorPtr->bapStatus)
        osPrintf("bap: %d bapStatus: 0x%0x\n", tmBapErrorPtr->bapNum, tmBapErrorPtr->bapStatus);

}

GT_STATUS tmDumpTmUnitsErrors(GT_U8 devNum)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_TM_UNITS_ERROR_STATUS_STC      tmUnitsError;
    CPSS_TM_BAP_UNIT_ERROR_STATUS_STC   tmBapError;
    GT_BOOL                             errorOccured;

    rc = tmGetTmUnitsErrors(devNum, 
                            &tmUnitsError,
                            &tmBapError,
                            &errorOccured);

    if (rc != GT_OK || errorOccured == GT_FALSE)
    {
        return rc;
    }

    tmPrintTmUnitsErrors(&tmUnitsError, &tmBapError);

    return rc;
}

/* dump tm Units Errors
   only if error has occured or by dumpAlways - conditional flag */
GT_STATUS tmConditionalDumpTmUnitsErrors(GT_U8 devNum, char *fileNamePtr, GT_32 line, GT_BOOL dumpAlways)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_TM_UNITS_ERROR_STATUS_STC      tmUnitsError;
    CPSS_TM_BAP_UNIT_ERROR_STATUS_STC   tmBapError;
    GT_BOOL                             errorOccured;

    rc = tmGetTmUnitsErrors(devNum, 
                            &tmUnitsError,
                            &tmBapError,
                            &errorOccured);

    if (rc != GT_OK)
    {
        return rc;
    }

    if (!errorOccured && !dumpAlways)
    {
        return rc;
    }

    osPrintf("%s: %d\n", 
             (fileNamePtr != NULL) ? fileNamePtr : "",
             line);

    tmPrintTmUnitsErrors(&tmUnitsError, &tmBapError);

    return rc;
}



GT_STATUS tmDumpTmDramFreq(GT_U8 devNum)
{
    /* Read TM frequency */
    char    *freqNamePTR = 0;
    GT_U32  freqNum = 0;

    GT_STATUS rc = prvCpssDxChTMFreqGet(devNum, &freqNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch(freqNum)
    {
        case 2:
            freqNamePTR = "CPSS_DRAM_FREQ_933_MHZ_E";
            break;
        case 3:
            freqNamePTR = "CPSS_DRAM_FREQ_667_MHZ_E";
            break;
        case 1:
            freqNamePTR = "CPSS_DRAM_FREQ_800_MHZ_E";
            break;
        case 0:
        default:
            freqNamePTR = "Unknown";
    }

    cpssOsPrintf("DRAM frequency: %d-%s\n", freqNum, freqNamePTR);

    return rc;
}

