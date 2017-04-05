/*******************************************************************************
*              (c), Copyright 2013, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoTmConfig.c
*
* DESCRIPTION:
*       Initialization functions for the TM.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/boardConfig/gtDbDxBobcat2PhyConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/gtDbDxBobcat2Mappings.h>

#include <cpss/generic/tm/cpssTmPublicDefs.h>
#include <cpss/generic/tm/cpssTmCtl.h>
#include <cpss/generic/tm/cpssTmSched.h>
#include <cpss/generic/tm/cpssTmDrop.h>
#include <cpss/generic/tm/cpssTmNodesCreate.h>
#include <cpss/generic/tm/cpssTmNodesCtl.h>
#include <cpss/generic/tm/cpssTmShaping.h>
#include <cpss/generic/tm/cpssTmNodesUpdate.h>
#include <cpss/generic/tm/cpssTmNodesStatus.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgE2Phy.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueDrop.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>

extern GT_BOOL appDemoIsTmEnabled(void); /* SW */
extern GT_STATUS getTMQCounters(GT_BOOL includeBytes);
typedef struct _xNodes
{
    GT_U32 aNode;
    GT_U32 bNode;
    GT_U32 cNode;
    GT_U32 pNode;
    GT_BOOL exists;
}xNodes;

#define QUEUE_POOL_SIZE_CNS 16384

static  GT_U32 queue_id_pool[QUEUE_POOL_SIZE_CNS];
static  GT_U32 queue_id_pool_size = 0;
static  xNodes qNodeTopology[QUEUE_POOL_SIZE_CNS];
static  GT_U32 totalQueuesPerPort = 0;  

GT_STATUS getQNodeT(GT_U32 qNodeIndex)
{
    if (qNodeTopology[qNodeIndex].exists != GT_TRUE)
    {
        osPrintf("Node does not exist\n");
        return GT_OK;
    }

    if (qNodeIndex < QUEUE_POOL_SIZE_CNS)
        osPrintf("aNode %d\nbNode %d\ncNode %d\npNode %d\n mapped by ePort %d, TC %d\n",
                 qNodeTopology[qNodeIndex].aNode,
                 qNodeTopology[qNodeIndex].bNode,
                 qNodeTopology[qNodeIndex].cNode,
                 qNodeTopology[qNodeIndex].pNode,
                 qNodeIndex/8, qNodeIndex%8);
    return GT_OK;
}

/* find out how much memory is currently in use in the DRAM */
GT_STATUS getTMMemUse()
{
    GT_STATUS rc = GT_OK;
    GT_U32 totalUse[2] = {0,0}, qNodeIndex, qLength[2] = {0,0}, maxQLenght[2] = {0,0}, maxIndex[2] = {0,0};

    for (qNodeIndex = 0;  qNodeIndex < QUEUE_POOL_SIZE_CNS; qNodeIndex++)
    {
        if (qNodeTopology[qNodeIndex].exists == GT_FALSE)
            continue;

        rc = cpssTmDropQueueLengthGet(0,
                                      CPSS_TM_LEVEL_Q_E,
                                      qNodeIndex,
                                      &(qLength[0]),
                                      &(qLength[1]));
        if (rc != GT_OK)
            return rc;

        totalUse[0] += qLength[0];
        totalUse[1] += qLength[1];
        if (maxQLenght[0] < qLength[0])
        {
            maxQLenght[0]   = qLength[0];
            maxIndex[0]     = qNodeIndex;
        }
        if (maxQLenght[1] < qLength[1])
        {
            maxQLenght[1]   = qLength[1];
            maxIndex[1]     = qNodeIndex;
        }

    }
        osPrintf("total (average) instant use is %u (%u) bytes\n max use %u (%u) on queue %d (%d)",
                 totalUse[0]*16,
                 totalUse[1]*16,
                 maxQLenght[0]*16,
                 maxQLenght[1]*16,
                 maxIndex[0],
                 maxIndex[1]
                 );
    return GT_OK;
}

/* get TM non empty Qs */
GT_STATUS getTMNonEmptyQueues(void)
{
    GT_STATUS rc = GT_OK;
    GT_U32  i;
    GT_U32  qNodeIndex,
            qLength[2] = {0,0};

    for (i = 0; i < queue_id_pool_size;  i++)
    {
        qNodeIndex = queue_id_pool[i] ;

        rc = cpssTmDropQueueLengthGet(0,
                                      CPSS_TM_LEVEL_Q_E,
                                      qNodeIndex,
                                      &(qLength[0]),
                                      &(qLength[1]));
        if (rc != GT_OK)
            return rc;

        if (qLength[0])
            osPrintf("queue: %u length is %u bytes\n", qNodeIndex, qLength[0] * 16);
    }

    return GT_OK;
}

/* collect tm Ports indxs */
GT_STATUS gtAppDemoTmGetTmPortsIds
(
    IN      GT_U8  dev,                /* device id */                     
    INOUT   GT_U32 *tmPortsArrSizePtr, /* in  - max size of tmPortsArr, 
                                          out - actual size of tmPortsArr */ 
    OUT     GT_U32 *tmPortsArr         /* tmPortsArr to fill */  
)
{
    GT_STATUS               rc;    
    CPSS_DXCH_PORT_MAP_STC  *portsMappingPtr = NULL;
    GT_U32                  i,tmPorts = 0;
    GT_U32                  dumpTmPorts = 0;
    GT_BOOL                 isTmSwEnabled;
    GT_U32                  portsMappingLen = 0;
    /*
    APP_DEMO_PP_CONFIG      *appDemoPpConfigPtr;  
    appDemoPpConfigPtr = &appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(dev)]; 
    portsMappingPtr = (CPSS_DXCH_PORT_MAP_STC*)appDemoPpConfigPtr->portsMapArrPtr; 
     
    if (portsMappingPtr == NULL)
    {
        return GT_FAIL;
    }
                                                                                                                                                                  
    */ 

    rc = appDemoDxChMaxMappedPortGet(dev, &portsMappingLen, &portsMappingPtr);

    if (rc != GT_OK)
    {
        return GT_FAIL;
    }

    rc = appDemoDbEntryGet("tmDumpPorts", &dumpTmPorts);
    if(rc != GT_OK)
    {
        dumpTmPorts = 0;
    }
    
    isTmSwEnabled = appDemoIsTmEnabled();

    /* on system which is not appDemoIsTmEnabled (cpssInitSystem 29,1)
       tm Port Index is taken from phisicalPortNumber, unless its CPU port */

    for (i = 0; i < portsMappingLen && tmPorts < *tmPortsArrSizePtr; i++) 
    {
        if (!portsMappingPtr[i].tmEnable && isTmSwEnabled)
        {
            if (dumpTmPorts)
            {
                cpssOsPrintf("physical Port: %d, non Ethernet Tm Port\n", portsMappingPtr[i].physicalPortNumber);
            }
            continue;
        }

        if (portsMappingPtr[i].mappingType !=  CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E)
        {
            if (dumpTmPorts)
            {
                cpssOsPrintf("physical Port: %d, non Ethernet Tm Port\n", portsMappingPtr[i].physicalPortNumber);
            }
            continue;
        }

        if (portsMappingPtr[i].physicalPortNumber > 71 && portsMappingPtr[i].physicalPortNumber < 128)
        {
            if (dumpTmPorts)
            {
                cpssOsPrintf("physical Port: %d, not valid number for Tm Port\n", portsMappingPtr[i].physicalPortNumber);
            }
            continue;
        }

        if (isTmSwEnabled)
        {
            if (dumpTmPorts)
            {
                cpssOsPrintf("physical Port: %d, Tm Port: %d\n", 
                             portsMappingPtr[i].physicalPortNumber,
                             portsMappingPtr[i].tmPortInd);
            }

            tmPortsArr[tmPorts++] = portsMappingPtr[i].tmPortInd;
        }
        else 
        {
            if (dumpTmPorts)
            {
                cpssOsPrintf("physical Port: %d, Tm Port: %d\n", 
                             portsMappingPtr[i].physicalPortNumber,
                             portsMappingPtr[i].physicalPortNumber);
            }

            tmPortsArr[tmPorts++] = portsMappingPtr[i].physicalPortNumber;
        }
    }

    *tmPortsArrSizePtr = tmPorts;

    return GT_OK;
}

static GT_BOOL appDemoTmGetValidTmPorts
(
    IN    GT_U32 tmPortsArrSize,        /* size of tmPortsArr */
    IN    GT_U32 *tmPortsArr,           /* tmPortsArr retrived from gtAppDemoTmGetTmPortsIds */
    IN    GT_U32 fromTmPort,            /* start tm port index */
    INOUT GT_U32 *numOfTmPortsPtr,      /* in  - requiered number of tm ports  
                                           out - actual number of tm ports */
    OUT   GT_U32 *outTmPortsArr         /* out - tmPortsArr to fill with requiered numOfTmPortsPtr
                                           starting from fromTmPort*/     
)
{
    GT_U32  i, j;
    GT_U32  tmPort = fromTmPort;
     
    for (i = 0, j = 0; i < tmPortsArrSize && j < *numOfTmPortsPtr; i++)
    {
        if (tmPortsArr[i] >= tmPort)
        {
            outTmPortsArr[j++] = tmPort;
            tmPort = tmPortsArr[i] + 1;
        }
    }

    *numOfTmPortsPtr = j;

    return GT_OK;
}


/* get total Queues per port for TM scenarios 1-9 */
GT_U32 appDemoTmGetTotalQueuesPerPort(void)
{
    return totalQueuesPerPort;
}


/*******************************************************************************
* appDemoTmGeneral48PortsInit
*
* DESCRIPTION:
*      Implements general UpTo 48 Ports initialization for specific device.
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
GT_STATUS appDemoTmGeneral48PortsInit
(
    IN  GT_U8   dev
)
{
    GT_STATUS                         rc = GT_OK;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC params[5];
    CPSS_TM_PORT_PARAMS_STC           portParams;
    CPSS_TM_PORT_DROP_PER_COS_STC     portCosParams;
    CPSS_TM_QUEUE_PARAMS_STC          queueParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC   profile;
    GT_U32                            profileId;
    GT_U32                            profileCosId;
    GT_U32                            queueId=0;
    GT_U32                            portsNum;
    GT_U32                            queuesNum;
    GT_U32                            cos;
    GT_U32                            portMode;
    GT_U32                            i, j;
    CPSS_TM_LIB_INIT_PARAMS_STC       tmLibinitParams = {0};
    GT_U32                            tmPortsArr[64];
    GT_U32                            tmPortsArrSize;
    GT_U32                            tmPort;
    GT_U32                            dumpTmPorts = 0;
    CPSS_TM_QUANTUM_LIMITS_STC        cpssQuantumLimits;
    CPSS_TM_PORT_BW_ENT                 tmPortBw;
    
    tmLibinitParams.tmMtu = 8192;

    queue_id_pool_size = 0;

    cpssOsPrintf("\n ***********CPSS_TM_0************* \n ");
    rc = cpssTmInitExt(dev, &tmLibinitParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/

    for (i = CPSS_TM_LEVEL_Q_E; i < CPSS_TM_LEVEL_P_E; i++) 
    {
        params[i].periodicState = GT_FALSE;
        params[i].shaperDecoupling = GT_FALSE;
    }
    params[CPSS_TM_LEVEL_P_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_P_E].shaperDecoupling = GT_FALSE;

    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;

    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(1000000); /* 1Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &profileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* Optional for CoS Port mode: create the same profile for Cos */
    cos = 2;

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &profileCosId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/
    portParams.cirBw = 1000000;  /*1G */
    portParams.eirBw = 0;
    portParams.cbs = 8; /*KBytes*/
    portParams.ebs = 0x1FFFF;

    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = profileId;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_E;

    /* Optional for CoS Port mode: Port CoS parameters configuration */
    portCosParams.dropCosMap = 1 << cos;
    for (i = 0; i < 8; i++)
        portCosParams.dropProfileIndArr[i] = CPSS_TM_NO_DROP_PROFILE_CNS;
    portCosParams.dropProfileIndArr[cos] = profileCosId;

    /* get nodes quantum limits */
    rc = cpssTmNodeQuantumLimitsGet(dev, &cpssQuantumLimits);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodeQuantumLimitsGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    /* verify quantum setings with cpssQuantumLimits */
    queueParams.quantum = (cpssQuantumLimits.minQuantum / cpssQuantumLimits.resolution);
    queueParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/
    portsNum = 59;
    queuesNum = 8; /* per port */


    tmPortsArrSize = portsNum;
    rc = gtAppDemoTmGetTmPortsIds(dev, &tmPortsArrSize, tmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portsNum = MIN(portsNum, tmPortsArrSize);

    rc = appDemoDbEntryGet("tmDumpPorts", &dumpTmPorts);
    if(rc != GT_OK)
    {
        dumpTmPorts = 0;
    }

    totalQueuesPerPort = queuesNum;

    portParams.eirBw = 0;        
    portParams.ebs = 0x1FFFF;

    for (i = 0; i < portsNum; i++) 
    {
        tmPort = tmPortsArr[i];

        if (tmPort >= 48)
        {
            tmPortBw = CPSS_TM_PORT_BW_10G_E; 
            portParams.cirBw = 10000000;  /* 10G */
            portParams.cbs = 80;          /* KBytes */
        }
        else
        {
            tmPortBw = CPSS_TM_PORT_BW_1G_E;
            portParams.cirBw = 1000000;  /* 1G */
            portParams.cbs = 8;          /* KBytes */
        }
       
        if (dumpTmPorts)
        {
            cpssOsPrintf("cpssTmPortCreate: port: %d, tmPort: %d, tmPortBw\n", 
                         i, 
                         tmPort, 
                         (tmPortBw == CPSS_TM_PORT_BW_1G_E) ? "1G" : "10G");
        }

        rc = cpssTmPortCreate(dev, tmPort, tmPortBw, &portParams, 1, 1, 1, queuesNum);
        
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Optional for CoS Port mode */
        rc = cpssTmPortDropPerCosConfig(dev, tmPort, &portCosParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortDropPerCosConfig", rc);
        if (rc != GT_OK)
        {
            return rc;
        }


        for (j = 0; j < queuesNum; j++) 
        {
            switch(j) 
            {
            case 0:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                break;
            case 1:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO1_E;
                break;
            case 2:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO2_E;
                break;
            case 3:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO3_E;
                break;
            case 4:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO4_E;
                break;
            case 5:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO5_E;
                break;
            case 6:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO6_E;
                break;
            case 7:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO7_E;
                break;
            default:
                return GT_FAIL;
            }
            rc = cpssTmTransQueueToPortCreate(dev, tmPort, &queueParams, &queueId);
            queue_id_pool[queue_id_pool_size++]=queueId;
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTransQueueToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    for (i = 0; i < queue_id_pool_size; i++) 
    {
        /*cpssOsTimerWkAfter(1000);*/
        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
        {
            return rc;
        }


        /* Optional for CoS Port mode: choose CoS for each Queue */
        rc = cpssTmDropQueueCosSet(dev, queue_id_pool[i], cos);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropQueueCosSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }


    /* Optional for CoS Port mode: choose Port mode */

    portMode = 0; /* 0 - Global Mode, 1 - CoS Mode */
    rc = cpssTmDropQueryResponceSet(dev, portMode, CPSS_TM_LEVEL_Q_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropQueryResponceSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("TmPorts (Transparent Ports To Queues): %d, cNodes: %d, bNodes:%d, aNodes: %d, queues: %d\n",
                 portsNum, 1, 1, 1, queuesNum); 

    return rc;
} /* appDemoTmGeneral48PortsInit */

/*******************************************************************************
* appDemoTmScenario2Init
*
* DESCRIPTION:
*      Implements scenario 2 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p)
*      Tail Drop on port and queue level
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
GT_STATUS appDemoTmScenario2Init
(
    IN  GT_U8   dev
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC   params[5];
    CPSS_TM_PORT_PARAMS_STC             portParams;
    CPSS_TM_QUEUE_PARAMS_STC            queueParams;
    CPSS_TM_C_NODE_PARAMS_STC           cParams;
    CPSS_TM_B_NODE_PARAMS_STC           bParams;
    CPSS_TM_A_NODE_PARAMS_STC           aParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC     profile;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC  profileShaping;
    CPSS_TM_LEVEL_ENT                   level;
    GT_U32                              cProfileShapingIndPtr;
    GT_U32                              bProfileShapingIndPtr;
    GT_U32                              aProfileShapingIndPtr;
    GT_U32                              cNodeInd;
    GT_U32                              bNodeInd;
    GT_U32                              aNodeInd;
    GT_U32                              portDropProfileId;
    GT_U32                              queueDropProfileId2; /*0.5 M*/
    GT_U32                              queueDropProfileId1; /*1 M*/
    GT_U32                              queueId;
    GT_U32                              portsNum;
    GT_U32                              queuesNum;
    GT_U32                              aNodesNum;
    GT_U32                              bNodesNum;
    GT_U32                              cNodesNum;
    GT_U32                              cos;
    GT_U32                              i, j, ii, c, b, a;
    GT_U32                              tmPortsArr[64];
    GT_U32                              tmPortsArrSize;
    GT_U32                              tmPort;
    CPSS_TM_TREE_PARAMS_STC             tmTreeParams; 
    GT_U32                              totalQueues = 0;
    GT_U32                              portSpeed = 1000000; /* 1Gbs */

    cpssOsPrintf("\n ***********CPSS_TM_2*********** \n ");
    osMemSet(qNodeTopology,0,QUEUE_POOL_SIZE_CNS*sizeof(xNodes));

    queue_id_pool_size = 0;
    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* first, get tree nodes levels number as its affected our shaping calculations */
    portsNum = 10;
    cNodesNum = 4; /* per port */
    bNodesNum = 5; /* per c node */
    aNodesNum = 10; /* per b node */
    queuesNum = 8; /* per a node */

    rc = cpssTmTreeParamsGet(dev, &tmTreeParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTreeParamsGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    tmPortsArrSize = portsNum;
    rc = gtAppDemoTmGetTmPortsIds(dev, &tmPortsArrSize, tmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portsNum = MIN(portsNum, tmPortsArrSize);

    totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;

    /* verify maxQueues */  
    if (portsNum * totalQueuesPerPort > tmTreeParams.maxQueues)
    {
        cNodesNum = 2; /* per port */
        bNodesNum = 4; /* per c node */
        aNodesNum = 5; /* per b node */
        queuesNum = 8; /* per a node */

        totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;
    }

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/

        /*port level*/
    params[CPSS_TM_LEVEL_P_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_P_E].shaperDecoupling = GT_FALSE;
    /* c level*/
    params[CPSS_TM_LEVEL_C_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_C_E].shaperDecoupling = GT_FALSE;
    /* b level*/
    params[CPSS_TM_LEVEL_B_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_B_E].shaperDecoupling = GT_FALSE;
    /* a level*/
    params[CPSS_TM_LEVEL_A_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_A_E].shaperDecoupling = GT_FALSE;
    /* q level*/
    params[CPSS_TM_LEVEL_Q_E].periodicState = GT_FALSE;
    params[CPSS_TM_LEVEL_Q_E].shaperDecoupling = GT_FALSE;

    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(portSpeed); /* 1 Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &portDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure C level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_C_E;
    profileShaping.cirBw = (portSpeed) / cNodesNum; /* 250MB or  500MB per cnode */ 
    profileShaping.cbs = 80;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &cProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure B level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_B_E;
    profileShaping.cirBw = (portSpeed) / (cNodesNum * bNodesNum); /* 50MB or 125MB per bnode */
    profileShaping.cbs = 80;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &bProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure A level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_A_E;
    profileShaping.cirBw = (portSpeed) / (aNodesNum * cNodesNum * bNodesNum); /* 5M or 25M per anode */
    profileShaping.cbs = 80;    /*KBytes*/
    profileShaping.eirBw = (portSpeed) / (aNodesNum * cNodesNum * bNodesNum); /* 5M or 25M per anode */
    profileShaping.ebs = 80; /*KBytes*/

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &aProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/
    portParams.cirBw = portSpeed;
    portParams.eirBw = 0;
    portParams.cbs = 80; /*KBytes*/
    portParams.ebs = 0;
    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = portDropProfileId;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_FP7_E;

    /*****************************************************/
    /* Configure C parameters                        */
    /*****************************************************/
    cParams.shapingProfilePtr = cProfileShapingIndPtr;
    cParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap = 1;
    cParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_E;
    for (ii = 0; ii < 8; ii++)
    {
        cParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E;
        cParams.dropProfileIndArr[ii] = 0;
    }

    /*****************************************************/
    /* Configure B parameters                        */
    /*****************************************************/
    bParams.shapingProfilePtr= bProfileShapingIndPtr;
    bParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        bParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    bParams.dropProfileInd =0;
    bParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_E;

    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr= aProfileShapingIndPtr;
    aParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        aParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    aParams.dropProfileInd =0;
    aParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_SHP_E;

    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS; /* in CPSS_TM_NODE_QUANTUM_UNIT_CNS */

    /*****************************************************/
    /* Create Queue Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(500); /* 0.5M */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId2);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(1000); /* 1 M */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/

    /* 
        portsNum, cNodesNum, bNodesNum, aNodesNum, queuesNum have calculated above
    */

    for (i = 0; i < portsNum; i++) 
    {
        tmPort = tmPortsArr[i];

        /* queue 0 is excluded from the pool */
        totalQueues += MIN(cNodesNum*bNodesNum*aNodesNum*queuesNum, tmTreeParams.maxQueues-1);

        if (totalQueues > tmTreeParams.maxQueues-1)
        {
            cpssOsPrintf("total queues: %d is exceeded max queues after %d tm ports\n", totalQueues, i);
            break;
        }

        /*create port*/
        rc = cpssTmPortCreate(dev, 
                              tmPort, 
                              CPSS_TM_PORT_BW_1G_E, 
                              &portParams, 
                              cNodesNum,
                              cNodesNum * bNodesNum, 
                              cNodesNum * bNodesNum * aNodesNum, 
                              cNodesNum * bNodesNum * aNodesNum * queuesNum);

        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (c = 0; c < cNodesNum; c++) 
        {
            /*create c node*/
            rc = cpssTmCnodeToPortCreate(dev, tmPort, &cParams, &cNodeInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            for (b = 0; b < bNodesNum; b++) 
            {
                /*create b node*/
                rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                for (a = 0; a < aNodesNum; a++) 
                {
                    /*create a node*/
                    rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    for (j = 0; j < queuesNum; j++) 
                    {
                        switch(j) 
                        {
                        case 0:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                            break;
                        case 1:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO1_E;
                            break;
                        case 2:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO2_E;
                            break;
                        case 3:
                            queueParams.dropProfileInd = queueDropProfileId1;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO3_E;
                            break;
                        case 4:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO4_E;
                            break;
                        case 5:
                            queueParams.dropProfileInd = queueDropProfileId1;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO5_E;
                            break;
                        case 6:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO6_E;
                            break;
                        case 7:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO7_E;
                            break;
                        default:
                            return GT_FAIL;
                        }

                        /*create queue node */
                        rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
                        queue_id_pool[queue_id_pool_size++]=queueId;
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }

                        /* add entry to Q-Node topology DB*/
                        qNodeTopology[queueId].aNode = aNodeInd;
                        qNodeTopology[queueId].bNode = bNodeInd;
                        qNodeTopology[queueId].cNode = cNodeInd;
                        qNodeTopology[queueId].pNode = tmPort;
                        qNodeTopology[queueId].exists = GT_TRUE;
                    }
                }
            }
        }
    }

    /*install all queues*/
    for (i = 0; i < queue_id_pool_size; i++) 
    {
        rc = cpssDxChTmGlueDropQueueProfileIdSet(dev, queue_id_pool[i], 0);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTmGlueDropQueueProfileIdSet", rc);
        if (rc != GT_OK)
            return rc;

        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    osPrintf("Shaping profiles ID's A %d, B %d, C %d\n", aProfileShapingIndPtr, bProfileShapingIndPtr, cProfileShapingIndPtr);

    cpssOsPrintf("TmPorts: %d, cNodes: %d, bNodes:%d, aNodes: %d, queues: %d\n",
                 portsNum, cNodesNum, bNodesNum, aNodesNum, queuesNum); 

    return rc;
} /* appDemoTmScenario2Init */



/*******************************************************************************
* appDemoTmScenario3Init
*
* DESCRIPTION:
*      Implements scenario 3 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p), propagated priority.
*      Tail Drop on port and queue level and color aware on  Q1 in each A node.
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
GT_STATUS appDemoTmScenario3Init
(
    IN  GT_U8   dev
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC   params[5];
    CPSS_TM_PORT_PARAMS_STC             portParams;
    CPSS_TM_QUEUE_PARAMS_STC            queueParams;
    CPSS_TM_C_NODE_PARAMS_STC           cParams;
    CPSS_TM_B_NODE_PARAMS_STC           bParams;
    CPSS_TM_A_NODE_PARAMS_STC           aParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC     profile;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC  profileShaping;
    CPSS_TM_LEVEL_ENT                   level;
    GT_U32                              cProfileShapingIndPtr;
    GT_U32                              bProfileShapingIndPtr;
    GT_U32                              aProfileShapingIndPtr;
    GT_U32                              cNodeInd;
    GT_U32                              bNodeInd;
    GT_U32                              aNodeInd;
    GT_U32                              portDropProfileId;
    GT_U32                              queueDropProfileId;
    GT_U32                              queueDropProfileId05;
    GT_U32                              queueDropProfileId1;
    GT_U32                              queueId;
    GT_U32                              portsNum;
    GT_U32                              queuesNum;
    GT_U32                              aNodesNum;
    GT_U32                              bNodesNum;
    GT_U32                              cNodesNum;
    GT_U32                              cos;
    GT_U32                              i, j, ii, c, b, a;
    GT_U32                              tmPortsArr[64];
    GT_U32                              tmPortsArrSize;
    GT_U32                              tmPort;
    CPSS_TM_TREE_PARAMS_STC             tmTreeParams; 
    GT_U32                              totalQueues = 0;
    GT_U32                              portSpeed = 1000000; /* 1Gbps */

    cpssOsPrintf("\n ***********CPSS_TM_3*********** \n ");

    queue_id_pool_size = 0;

    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* first, get tree nodes levels number as its affected our shaping calculations */
    portsNum = 10;
    cNodesNum = 4; /* per port */
    bNodesNum = 5; /* per c node */
    aNodesNum = 10; /* per b node */
    queuesNum = 8; /* per a node */

    rc = cpssTmTreeParamsGet(dev, &tmTreeParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTreeParamsGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    tmPortsArrSize = portsNum;
    rc = gtAppDemoTmGetTmPortsIds(dev, &tmPortsArrSize, tmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portsNum = MIN(portsNum, tmPortsArrSize);

    totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;

    /* verify maxQueues */  
    if (portsNum * totalQueuesPerPort > tmTreeParams.maxQueues)
    {
        cNodesNum = 2; /* per port */
        bNodesNum = 4; /* per c node */
        aNodesNum = 5; /* per b node */
        queuesNum = 8; /* per a node */

        totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;
    }

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/

        /*port level*/
    params[CPSS_TM_LEVEL_P_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_P_E].shaperDecoupling = GT_FALSE;
    /* c level*/
    params[CPSS_TM_LEVEL_C_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_C_E].shaperDecoupling = GT_FALSE;
    /* b level*/
    params[CPSS_TM_LEVEL_B_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_B_E].shaperDecoupling = GT_FALSE;
    /* a level*/
    params[CPSS_TM_LEVEL_A_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_A_E].shaperDecoupling = GT_FALSE;
    /* q level*/
    params[CPSS_TM_LEVEL_Q_E].periodicState = GT_FALSE;
    params[CPSS_TM_LEVEL_Q_E].shaperDecoupling = GT_FALSE;

    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(portSpeed); /* 1 Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &portDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************/
    /* Configure C level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_C_E;
    profileShaping.cirBw = (portSpeed) / cNodesNum; /* 250MB or  500MB per cnode */ 
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    /* validate (and update if necessary)  cbs & ebs values*/
    rc = cpssTmShapingProfileValidate(dev, level, &profileShaping);

    if ((rc != GT_OK) && (rc != GT_BAD_SIZE))
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortShapingValidate", rc);
        return rc;
    }

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &cProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure B level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_B_E;
    profileShaping.cirBw = (portSpeed) / (cNodesNum * bNodesNum); /* 50MB or 125MB per bnode */
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &bProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure A level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_A_E;
    profileShaping.cirBw = (portSpeed) / (aNodesNum * cNodesNum * bNodesNum); /* 5M or 25M per anode */
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = (portSpeed) / (aNodesNum * cNodesNum * bNodesNum); /* 5M or 25M per anode */
    profileShaping.ebs = 8; /*KBytes*/

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &aProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/
    portParams.cirBw = portSpeed; /* 1G */
    portParams.eirBw = 0;
    portParams.cbs = 8; /*KBytes*/
    portParams.ebs = 0;
    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = portDropProfileId;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E; /* CPSS_TM_ELIG_N_SHP_PP_TB_E;*/

    /*****************************************************/
    /* Configure C parameters                        */
    /*****************************************************/
    cParams.shapingProfilePtr= cProfileShapingIndPtr;
    cParams.quantum =CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap =1;
    cParams.eligiblePrioFuncId =CPSS_TM_ELIG_N_MIN_SHP_PP_E;
    for (ii = 0; ii < 8; ii++)
    {
        cParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E;
        cParams.dropProfileIndArr[ii] = 0;
    }

    /*****************************************************/
    /* Configure B parameters                        */
    /*****************************************************/
    bParams.shapingProfilePtr= bProfileShapingIndPtr;
    bParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        bParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    bParams.dropProfileInd = 0;
    bParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E;

    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr= aProfileShapingIndPtr;
    aParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        aParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    aParams.dropProfileInd = 0;
    aParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_SHP_PP_MAX_TB_0;

    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;

    /*****************************************************/
    /* Create Queue Drop profile CA                         */
    /*****************************************************/

/*
 * By default system is configured for use 2 colors
 * but here we will set color number explicitly
*/
    cpssTmDropColorNumSet(dev, CPSS_TM_LEVEL_Q_E,CPSS_TM_COLOR_NUM_2_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropColorNumSet", rc);

    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_CA_TD_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(500000); /* make it big - to have no influence*/
    profile.caTdDp.caTdThreshold[0] = cpssTmDropConvertBWToBytes(2000);
    profile.caTdDp.caTdThreshold[1] = cpssTmDropConvertBWToBytes(500);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Queue Drop profile  CB                       */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(500);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId05);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(1000);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/

    /* 
        portsNum, cNodesNum, bNodesNum, aNodesNum, queuesNum have calculated above
    */

    for (i = 0; i < portsNum; i++) 
    {
        tmPort = tmPortsArr[i];

        /* queue 0 is excluded from the pool */
        totalQueues += MIN(cNodesNum*bNodesNum*aNodesNum*queuesNum, tmTreeParams.maxQueues-1);

        if (totalQueues > tmTreeParams.maxQueues-1)
        {
            cpssOsPrintf("total queues: %d is exceeded max queues after %d tm ports\n", totalQueues, i);
            break;
        }

        /*create port*/
        rc = cpssTmPortCreate(dev, 
                              tmPort, 
                              CPSS_TM_PORT_BW_1G_E, 
                              &portParams, 
                              cNodesNum,
                              cNodesNum * bNodesNum, 
                              cNodesNum * bNodesNum * aNodesNum, 
                              cNodesNum * bNodesNum * aNodesNum * queuesNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (c = 0; c < cNodesNum; c++) 
        {
            /*create c node*/
            rc = cpssTmCnodeToPortCreate(dev, tmPort, &cParams, &cNodeInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            for (b = 0; b < bNodesNum; b++) 
            {
                /*create b node*/
                rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                for (a = 0; a < aNodesNum; a++) 
                {
                    /*create a node*/
                    rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    for (j = 0; j < queuesNum; j++) 
                    {
                        switch(j) 
                        {
                        case 0:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                            queueParams.dropProfileInd = queueDropProfileId1;
                            break;
                        case 1:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO1_E;
                            queueParams.dropProfileInd = queueDropProfileId;
                            break;
                        case 2:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO2_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        case 3:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO3_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        case 4:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO4_E;
                            queueParams.dropProfileInd = queueDropProfileId1;
                            break;
                        case 5:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO5_E;
                            queueParams.dropProfileInd = queueDropProfileId1;
                            break;
                        case 6:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO6_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        case 7:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO7_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        default:
                            return GT_FAIL;
                        }

                        /*create queue node */
                        rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
                        queue_id_pool[queue_id_pool_size++] = queueId;
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
            }
        }
    }

    /*install all queues*/
    for (i = 0; i < queue_id_pool_size; i++) 
    {
        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    cpssOsPrintf("TmPorts: %d, cNodes: %d, bNodes:%d, aNodes: %d, queues: %d\n",
                 portsNum, cNodesNum, bNodesNum, aNodesNum, queuesNum); 

    return rc;
} /* appDemoTmScenario3Init */



/*******************************************************************************
* appDemoTmScenario4Init
*
* DESCRIPTION:
*      Implements scenario 4 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p), propagated priority.
*      Tail Drop on port and queue level and color aware on Q1 in each A node and wred mode on queue0 in each A node
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
GT_STATUS appDemoTmScenario4Init
(
    IN  GT_U8   dev
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC   params[5];
    CPSS_TM_PORT_PARAMS_STC             portParams;
    CPSS_TM_QUEUE_PARAMS_STC            queueParams;
    CPSS_TM_C_NODE_PARAMS_STC           cParams;
    CPSS_TM_B_NODE_PARAMS_STC           bParams;
    CPSS_TM_A_NODE_PARAMS_STC           aParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC     profile;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC  profileShaping;
    CPSS_TM_LEVEL_ENT                   level;
    GT_U32                              cProfileShapingIndPtr;
    GT_U32                              bProfileShapingIndPtr;
    GT_U32                              aProfileShapingIndPtr;
    GT_U32                              cNodeInd;
    GT_U32                              bNodeInd;
    GT_U32                              aNodeInd;
    GT_U32                              portDropProfileId;
    GT_U32                              queueDropProfileIdWred;
    GT_U32                              queueDropProfileIdCa;
    GT_U32                              queueDropProfileId1;
    GT_U32                              queueDropProfileId05;
    GT_U32                              queueId;
    GT_U32                              portsNum;
    GT_U32                              queuesNum;
    GT_U32                              aNodesNum;
    GT_U32                              bNodesNum;
    GT_U32                              cNodesNum;
    GT_U32                              cos;
    GT_U32                              i, j, ii, c, b, a;
    GT_U32                              queue_id_pool_size=0;
    GT_U32                              maxProbability=50;
    GT_U32                              curvePtr;
    GT_U32                              tmPortsArr[64];
    GT_U32                              tmPortsArrSize;
    GT_U32                              tmPort;
    CPSS_TM_TREE_PARAMS_STC             tmTreeParams; 
    GT_U32                              totalQueues = 0;
    GT_U32                              portSpeed = 1000000; /* 1Gbps */

    cpssOsPrintf("\n ***********CPSS_TM_4*********** \n ");

    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* first get, tree nodes levels number as its affected our shaping calculations */
    portsNum = 10;
    cNodesNum = 4; /* per port */
    bNodesNum = 5; /* per c node */
    aNodesNum = 10; /* per b node */
    queuesNum = 8; /* per a node */

    rc = cpssTmTreeParamsGet(dev, &tmTreeParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTreeParamsGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    tmPortsArrSize = portsNum;
    rc = gtAppDemoTmGetTmPortsIds(dev, &tmPortsArrSize, tmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portsNum = MIN(portsNum, tmPortsArrSize);

    totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;

    /* verify maxQueues */  
    if (portsNum * totalQueuesPerPort > tmTreeParams.maxQueues)
    {
        cNodesNum = 2; /* per port */
        bNodesNum = 4; /* per c node */
        aNodesNum = 5; /* per b node */
        queuesNum = 8; /* per a node */

        totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;
    }

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/

        /*port level*/
    params[CPSS_TM_LEVEL_P_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_P_E].shaperDecoupling = GT_FALSE;
    /* c level*/
    params[CPSS_TM_LEVEL_C_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_C_E].shaperDecoupling = GT_FALSE;
    /* b level*/
    params[CPSS_TM_LEVEL_B_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_B_E].shaperDecoupling = GT_FALSE;
    /* a level*/
    params[CPSS_TM_LEVEL_A_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_A_E].shaperDecoupling = GT_FALSE;
    /* q level*/
    params[CPSS_TM_LEVEL_Q_E].periodicState = GT_FALSE;
    params[CPSS_TM_LEVEL_Q_E].shaperDecoupling = GT_FALSE;

    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(portSpeed); /* 1Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &portDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************/
    /* Configure C level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_C_E;
    profileShaping.cirBw = (portSpeed) / cNodesNum; /* 250MB or  500MB per cnode */ 
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    /* validate (and update if necessary)  cbs & ebs values*/
    rc = cpssTmShapingProfileValidate(dev, level, &profileShaping);

    if ((rc != GT_OK) && (rc != GT_BAD_SIZE))
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortShapingValidate", rc);
        return rc;
    }

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &cProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure B level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_B_E;
    profileShaping.cirBw = (portSpeed) / (cNodesNum * bNodesNum); /* 50MB or 125MB per bnode */
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &bProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure A level shaping parameters              */
    /*****************************************************/

    /* 
       the eligible function for a nodes is CPSS_TM_ELIG_N_SHP_PP_MAX_TB_0
       actual bw per one a node is cirBw.
    */    
    level= CPSS_TM_LEVEL_A_E;
    profileShaping.cirBw = (portSpeed) / (aNodesNum * cNodesNum * bNodesNum); /* 5M or 25M per anode */
    profileShaping.cbs = 8; /*KBytes*/
    profileShaping.eirBw = (portSpeed) / (aNodesNum * cNodesNum * bNodesNum); /* 5M or 25M per anode */
    profileShaping.ebs = 8; /*KBytes*/

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &aProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/
    portParams.cirBw = portSpeed; /* 1G */
    portParams.eirBw = 0;
    portParams.cbs = 8; /*KBytes*/
    portParams.ebs = 0;
    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = portDropProfileId;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E; /* CPSS_TM_ELIG_N_SHP_PP_TB_E;*/

    /*****************************************************/
    /* Configure C parameters                        */
    /*****************************************************/
    cParams.shapingProfilePtr = cProfileShapingIndPtr;
    cParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap = 1;
    cParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E;
    for (ii = 0; ii < 8; ii++)
    {
        cParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E;
        cParams.dropProfileIndArr[ii] = 0;
    }

    /*****************************************************/
    /* Configure B parameters                        */
    /*****************************************************/
    bParams.shapingProfilePtr= bProfileShapingIndPtr;
    bParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        bParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    bParams.dropProfileInd = 0;
    bParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E;

    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr= aProfileShapingIndPtr;
    aParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        aParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    aParams.dropProfileInd = 0;
    aParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_SHP_PP_MAX_TB_0;

    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;

    /*****************************************************/
    /* Configure Queue Treditional curve                        */
    /*****************************************************/
    rc = cpssTmDropWredTraditionalCurveCreate(dev,CPSS_TM_LEVEL_Q_E, 0, maxProbability, &curvePtr);
    if (rc != GT_OK) {
        return rc;
    }

    /*****************************************************/
    /* Create Queue Drop profile  Wred                        */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
/*
 * By default system is configured for use 2 colors
 * but here we will set color number explicitly
*/
    cpssTmDropColorNumSet(dev, CPSS_TM_LEVEL_Q_E,CPSS_TM_COLOR_NUM_2_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropColorNumSet", rc);

    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E;
    profile.cbTdThresholdBytes = 0;
    profile.caWredDp.aqlExponent = 0;
    profile.caWredDp.curveIndex[0] = (GT_U8)curvePtr;
    profile.caWredDp.curveIndex[1] = (GT_U8)curvePtr;
    profile.caWredDp.dpCurveScale[0] = 0;
    profile.caWredDp.dpCurveScale[1] = 0;

    profile.caWredDp.caWredTdMinThreshold[0] = 0;
    profile.caWredDp.caWredTdMinThreshold[1] = 0;
    profile.caWredDp.caWredTdMaxThreshold[0] = cpssTmDropConvertBWToBytes(2000);
    profile.caWredDp.caWredTdMaxThreshold[1] = cpssTmDropConvertBWToBytes(500);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileIdWred);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Queue Drop profile CA                    */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
/*
 * By default system is configured for use 2 colors
 * but here we will set color number explicitly
*/
    cpssTmDropColorNumSet(dev, CPSS_TM_LEVEL_Q_E,CPSS_TM_COLOR_NUM_2_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropColorNumSet", rc);

    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_CA_TD_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(500);
    profile.caTdDp.caTdThreshold[0] = cpssTmDropConvertBWToBytes(1000);
    profile.caTdDp.caTdThreshold[1] = cpssTmDropConvertBWToBytes(250);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileIdCa);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************/
    /* Create Queue Drop profile CB                    */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(500);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId05);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(1000);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/

    /* 
        portsNum, cNodesNum, bNodesNum, aNodesNum, queuesNum have calculated above
    */

    for (i = 0; i < portsNum; i++) 
    {
        tmPort = tmPortsArr[i];

        /* queue 0 is excluded from the pool */
        totalQueues += MIN(cNodesNum*bNodesNum*aNodesNum*queuesNum, tmTreeParams.maxQueues-1);

        if (totalQueues > tmTreeParams.maxQueues-1)
        {
            cpssOsPrintf("total queues: %d is exceeded max queues after %d tm ports\n", totalQueues, i);
            break;
        }

        /*create port*/
        rc = cpssTmPortCreate(dev, tmPort, 
                              CPSS_TM_PORT_BW_1G_E, 
                              &portParams, 
                              cNodesNum,
                              cNodesNum*bNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum*queuesNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (c = 0; c < cNodesNum; c++) 
        {
            /*create c node*/
            rc = cpssTmCnodeToPortCreate(dev, tmPort, &cParams, &cNodeInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            for (b = 0; b < bNodesNum; b++) 
            {
                /*create b node*/
                rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                for (a = 0; a < aNodesNum; a++) 
                {
                    /*create a node*/
                    rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    for (j = 0; j < queuesNum; j++) 
                    {
                        switch(j) 
                        {
                        case 0:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                            queueParams.dropProfileInd = queueDropProfileIdWred;
                            break;
                        case 1:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO1_E;
                            queueParams.dropProfileInd = queueDropProfileIdCa;
                            break;
                        case 2:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO2_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        case 3:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO3_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        case 4:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO4_E;
                            queueParams.dropProfileInd = queueDropProfileId1;
                            break;
                        case 5:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO5_E;
                            queueParams.dropProfileInd = queueDropProfileId1;
                            break;
                        case 6:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO6_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        case 7:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO7_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        default:
                            return GT_FAIL;
                        }

                        /*create queue node */
                        rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
                        queue_id_pool[queue_id_pool_size++] = queueId;
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
            }
        }
    }

    /*install all queues*/
    for (i = 0; i < queue_id_pool_size; i++) 
    {
        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    cpssOsPrintf("TmPorts: %d, cNodes: %d, bNodes:%d, aNodes: %d, queues: %d\n",
                 portsNum, cNodesNum, bNodesNum, aNodesNum, queuesNum); 

    return rc;
} /* appDemoTmScenario4Init */



/*******************************************************************************
* appDemoTmScenario5Init
*
* DESCRIPTION:
*      Implements scenario 5 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 4 queues per a node
*      shaping on all levels (a,b,c,p), propagated priority.
*      Tail Drop on port wred drop all queues
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
GT_STATUS appDemoTmScenario5Init
(
    IN  GT_U8   dev
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC   params[5];
    CPSS_TM_PORT_PARAMS_STC             portParams;
    CPSS_TM_QUEUE_PARAMS_STC            queueParams;
    CPSS_TM_C_NODE_PARAMS_STC           cParams;
    CPSS_TM_B_NODE_PARAMS_STC           bParams;
    CPSS_TM_A_NODE_PARAMS_STC           aParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC     profile;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC  profileShaping;
    CPSS_TM_LEVEL_ENT                   level;
    GT_U32                              cProfileShapingIndPtr;
    GT_U32                              bProfileShapingIndPtr;
    GT_U32                              aProfileShapingIndPtr;
    GT_U32                              cNodeInd;
    GT_U32                              bNodeInd;
    GT_U32                              aNodeInd;
    GT_U32                              portDropProfileId;
    GT_U32                              queueDropProfileId2; /*0.5 M*/
    GT_U32                              queueDropProfileIdWred1;
    GT_U32                              queueDropProfileIdWred2;
    GT_U32                              queueDropProfileIdWred05;
    GT_U32                              queueDropProfileIdWred15;
    GT_U32                              queueId;
    GT_U32                              portsNum;
    GT_U32                              queuesNum;
    GT_U32                              aNodesNum;
    GT_U32                              bNodesNum;
    GT_U32                              cNodesNum;
    GT_U32                              cos;
    GT_U32                              i, j, ii, c, b, a;
    GT_U32                              maxProbability=50;
    GT_U32                              curvePtr;
    GT_U32                              tmPortsArr[64];
    GT_U32                              tmPortsArrSize;
    GT_U32                              tmPort;
    CPSS_TM_TREE_PARAMS_STC             tmTreeParams; 
    GT_U32                              totalQueues = 0;
    GT_U32                              portSpeed = 1000000; /* 1Gbps */

    cpssOsPrintf("\n ***********CPSS_TM_5*********** Q drop: 0.5/1/1.5/2\n ");
    osMemSet(qNodeTopology,0,QUEUE_POOL_SIZE_CNS*sizeof(xNodes));

    queue_id_pool_size = 0;

    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* first get, tree nodes levels number as its affected our shaping calculations */
    portsNum = 10;
    cNodesNum = 4; /* per port */
    bNodesNum = 5; /* per c node */
    aNodesNum = 10; /* per b node */
    queuesNum = 4; /* per a node */

    rc = cpssTmTreeParamsGet(dev, &tmTreeParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTreeParamsGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    tmPortsArrSize = portsNum;
    rc = gtAppDemoTmGetTmPortsIds(dev, &tmPortsArrSize, tmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portsNum = MIN(portsNum, tmPortsArrSize);

    totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;

    /* verify maxQueues */  
    if (portsNum * totalQueuesPerPort > tmTreeParams.maxQueues)
    {
        cNodesNum = 2; /* per port */
        bNodesNum = 4; /* per c node */
        aNodesNum = 5; /* per b node */
        queuesNum = 4; /* per a node */

        totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;

    }

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/

        /*port level*/
    params[CPSS_TM_LEVEL_P_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_P_E].shaperDecoupling = GT_FALSE;
    /* c level*/
    params[CPSS_TM_LEVEL_C_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_C_E].shaperDecoupling = GT_FALSE;
    /* b level*/
    params[CPSS_TM_LEVEL_B_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_B_E].shaperDecoupling = GT_FALSE;
    /* a level*/
    params[CPSS_TM_LEVEL_A_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_A_E].shaperDecoupling = GT_FALSE;
    /* q level*/
    params[CPSS_TM_LEVEL_Q_E].periodicState = GT_FALSE;
    params[CPSS_TM_LEVEL_Q_E].shaperDecoupling = GT_FALSE;

    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(portSpeed); /* 1Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &portDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************/
    /* Configure C level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_C_E;
    profileShaping.cirBw = (portSpeed) / cNodesNum; /* 250MB or  500MB per cnode */ 
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    /* validate (and update if necessary)  cbs & ebs values*/
    rc = cpssTmShapingProfileValidate(dev, level, &profileShaping);

    if ((rc != GT_OK) && (rc != GT_BAD_SIZE))
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortShapingValidate", rc);
        return rc;
    }

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &cProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure B level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_B_E;
    profileShaping.cirBw = (portSpeed) / (cNodesNum * bNodesNum); /* 50MB or 125MB per bnode */
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &bProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure A level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_A_E;
    profileShaping.cirBw = (portSpeed) / (aNodesNum * cNodesNum * bNodesNum); /* 5M or 25M per anode */
    profileShaping.cbs = 8; /*KBytes*/
    profileShaping.eirBw = (portSpeed) / (aNodesNum * cNodesNum * bNodesNum); /* 5M or 25M per anode */
    profileShaping.ebs = 8; /*KBytes*/

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &aProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/
    portParams.cirBw = portSpeed; /* 1G */
    portParams.eirBw = 0;
    portParams.cbs = 8; /*KBytes*/
    portParams.ebs = 0;
    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = portDropProfileId;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_E;

    /*****************************************************/
    /* Configure C parameters                        */
    /*****************************************************/
    cParams.shapingProfilePtr= cProfileShapingIndPtr;
    cParams.quantum =CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap = 1;
    cParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E;
    for (ii = 0; ii < 8; ii++)
    {
        cParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E;
        cParams.dropProfileIndArr[ii] = 0;
    }

    /*****************************************************/
    /* Configure B parameters                        */
    /*****************************************************/
    bParams.shapingProfilePtr = bProfileShapingIndPtr;
    bParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 1; ii < 8; ii++)
        bParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    bParams.schdModeArr[0] = CPSS_TM_SCHD_MODE_DWRR_E;
    bParams.dropProfileInd = 0;
    bParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E;

    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr = aProfileShapingIndPtr;
    aParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    aParams.schdModeArr[0] = CPSS_TM_SCHD_MODE_DWRR_E;
    for (ii = 1; ii < 8; ii++)
        aParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    aParams.dropProfileInd = 0;
    aParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_SHP_PP_MAX_TB_0;


    /*****************************************************/
    /* Configure Queue Traditional curve                        */
    /*****************************************************/
    rc = cpssTmDropWredTraditionalCurveCreate(dev,CPSS_TM_LEVEL_Q_E, 0, maxProbability, &curvePtr);
    if (rc != GT_OK)
        return rc;

    /*****************************************************/
    /* Create Queue Drop profile  WRED                       */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
/*
 * By default system is configured for use 2 colors
 * but here we will set color number explicitly
*/
    cpssTmDropColorNumSet(dev, CPSS_TM_LEVEL_Q_E,CPSS_TM_COLOR_NUM_2_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropColorNumSet", rc);

    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(500000);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId2);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E;
    profile.cbTdThresholdBytes = 0;
    profile.caWredDp.aqlExponent = 0;
    profile.caWredDp.curveIndex[0] = (GT_U8)curvePtr;
    profile.caWredDp.curveIndex[1] = (GT_U8)curvePtr;
    profile.caWredDp.dpCurveScale[0] = 0;
    profile.caWredDp.dpCurveScale[1] = 0;
    profile.caWredDp.caWredTdMinThreshold[0] = 0;
    profile.caWredDp.caWredTdMinThreshold[1] = 0;
    profile.caWredDp.caWredTdMaxThreshold[0] = cpssTmDropConvertBWToBytes(500);
    profile.caWredDp.caWredTdMaxThreshold[1] = cpssTmDropConvertBWToBytes(125);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileIdWred05);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    profile.caWredDp.caWredTdMaxThreshold[0] = cpssTmDropConvertBWToBytes(1000);
    profile.caWredDp.caWredTdMaxThreshold[1] = cpssTmDropConvertBWToBytes(250);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileIdWred1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    profile.caWredDp.caWredTdMaxThreshold[0] = cpssTmDropConvertBWToBytes(2000);
    profile.caWredDp.caWredTdMaxThreshold[1] = cpssTmDropConvertBWToBytes(500);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileIdWred2);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    profile.caWredDp.caWredTdMaxThreshold[0] = cpssTmDropConvertBWToBytes(1500);
    profile.caWredDp.caWredTdMaxThreshold[1] = cpssTmDropConvertBWToBytes(375);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileIdWred15);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/

    /* 
        portsNum, cNodesNum, bNodesNum, aNodesNum, queuesNum have calculated above
    */

    for (i = 0; i < portsNum; i++) 
    {
        tmPort = tmPortsArr[i];

        /* queue 0 is excluded from the pool */
        totalQueues += MIN(cNodesNum*bNodesNum*aNodesNum*queuesNum, tmTreeParams.maxQueues-1);

        if (totalQueues > tmTreeParams.maxQueues-1)
        {
            cpssOsPrintf("total queues: %d is exceeded max queues after %d tm ports\n", totalQueues, i);
            break;
        }

        /*create port*/
        rc = cpssTmPortCreate(dev, 
                              tmPort, 
                              CPSS_TM_PORT_BW_1G_E, 
                              &portParams, 
                              cNodesNum,
                              cNodesNum*bNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum*queuesNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (c = 0; c < cNodesNum; c++) 
        {
            /*create c node*/
            rc = cpssTmCnodeToPortCreate(dev, tmPort, &cParams, &cNodeInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            for (b = 0; b < bNodesNum ; b++) 
            {
                /*create b node*/
                rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                for (a = 0; a < aNodesNum; a++) 
                {
                    /*create a node*/
                    rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    for (j = 0; j < queuesNum; j++) 
                    {
                        switch(j) 
                        {
                        case 0:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                            queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
                            queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
                            queueParams.dropProfileInd = queueDropProfileIdWred05;
                            break;
                        case 1:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                            queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
                            queueParams.quantum = 2*CPSS_TM_NODE_QUANTUM_UNIT_CNS;
                            queueParams.dropProfileInd = queueDropProfileIdWred1;
                            break;
                        case 2:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                            queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
                            queueParams.quantum = 3*CPSS_TM_NODE_QUANTUM_UNIT_CNS;
                            queueParams.dropProfileInd = queueDropProfileIdWred15;
                            break;
                        case 3:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                            queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
                            queueParams.quantum = 4*CPSS_TM_NODE_QUANTUM_UNIT_CNS;
                            queueParams.dropProfileInd = queueDropProfileIdWred2;
                            break;
                        default:
                            return GT_FAIL;
                        }

                        /*create queue node */
                        rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
                        queue_id_pool[queue_id_pool_size++]=queueId;
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                        qNodeTopology[queueId].aNode = aNodeInd;
                        qNodeTopology[queueId].bNode = bNodeInd;
                        qNodeTopology[queueId].cNode = cNodeInd;
                        qNodeTopology[queueId].pNode = tmPort;
                        qNodeTopology[queueId].exists = GT_TRUE;
                    }
                }
            }
        }
    }

    /*install all queues*/
    for (i = 0; i < queue_id_pool_size; i++) 
    {
        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    cpssOsPrintf("TmPorts: %d, cNodes: %d, bNodes:%d, aNodes: %d, queues: %d\n",
                 portsNum, cNodesNum, bNodesNum, aNodesNum, queuesNum); 

    return rc;
} /* appDemoTmScenario5Init */


/*******************************************************************************
* tmTreeGrow
*
* DESCRIPTION:
*      Builds a basic symmetric TM tree with default attributes.
*
* INPUTS:
*      dev       - device number
*      portsNum  - number of consequitive TM ports
*      cNodesNum - per port node
*      bNodesNum - per C node
*      aNodesNum - per B node
*      queuesNum - per A node
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*   Should be moved to TM utils.  Restriction: TM ports are numbered from 0.
*
*******************************************************************************/
static GT_STATUS tmTreeGrow
(
    IN  GT_U8   dev,
    IN GT_U32   portsNum, /* number of TM ports */
    IN GT_U32   cNodesNum, /* per port */
    IN GT_U32   bNodesNum, /* per c node */
    IN GT_U32   aNodesNum, /* per b node */
    IN GT_U32   queuesNum  /* per a node */

)
{
    GT_STATUS                   rc = GT_OK;
    GT_U32                      i, j, c, b, a;
    GT_U32                      cNodeInd, bNodeInd, aNodeInd, queueId, profileShapingInd;
    CPSS_TM_LEVEL_ENT           tmLevel;
    CPSS_TM_PORT_PARAMS_STC     portParams;
    CPSS_TM_C_NODE_PARAMS_STC   cParams;
    CPSS_TM_B_NODE_PARAMS_STC   bParams;
    CPSS_TM_A_NODE_PARAMS_STC   aParams;
    CPSS_TM_QUEUE_PARAMS_STC    queueParams;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC   params[5];
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC  profileShaping;
    GT_U32                              tmPortsArr[64];
    GT_U32                              tmPortsArrSize;
    GT_U32                              tmPort;
    GT_U32                              totalQueues = 0;
    CPSS_TM_TREE_PARAMS_STC             tmTreeParams; 
    /*GT_U32                              bc2BoardType = APP_DEMO_BC2_BOARD_DB_CNS;*/
    CPSS_TM_PORT_BW_ENT                 tmPortBw;

    osMemSet(qNodeTopology,0,QUEUE_POOL_SIZE_CNS*sizeof(xNodes));
    osMemSet(&portParams,0,sizeof(portParams));
    osMemSet(&cParams,0,sizeof(cParams));
    osMemSet(&bParams,0,sizeof(bParams));
    osMemSet(&aParams,0,sizeof(aParams));
    osMemSet(&queueParams,0,sizeof(queueParams));

    queue_id_pool_size = 0;

    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
        return rc;

    rc = cpssTmAgingChangeStatus(dev, 0);
    if (rc != GT_OK)
        return rc;


    /*****************************************************/
     /* Configure Periodic Scheme                         */
     /*****************************************************/


     for (tmLevel = CPSS_TM_LEVEL_Q_E; tmLevel <= CPSS_TM_LEVEL_P_E; tmLevel++)
     {
         params[tmLevel].periodicState = GT_TRUE;
         params[tmLevel].shaperDecoupling = GT_FALSE; /* should be parametrized ?*/
     }
     rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
     CPSS_ENABLER_DBG_TRACE_RC_MAC("tmTreeGrow", rc);
     if (rc != GT_OK)
         return rc;

     /*****************************************************/
     /* Configure Port parameters, they can be changed later with TM utility functions: updateShaper, nodeTDSet, updateNodePrioWeight  */
     /*****************************************************/
     portParams.cirBw = 1000000; /* 1G */
     portParams.eirBw = 0;
     portParams.cbs = 8;
     portParams.ebs = 0;

     portParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;
     portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_FP0_E;
     for (i = 0; i < 8; i++) /* these parameters cannot be changed with utility functions */
     {
         portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
         portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
     }


    /*****************************************************/
    /* Configure C parameters                       */
    /*****************************************************/
    cParams.shapingProfilePtr= CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    cParams.quantum =CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap =1;
    cParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_FP0_E;

    for (i = 0; i < 8; i++)
    {
       cParams.schdModeArr[i]   = CPSS_TM_SCHD_MODE_DWRR_E;
       cParams.dropProfileIndArr[i] = CPSS_TM_NO_DROP_PROFILE_CNS;
    }


    /*****************************************************/
    /* Configure B parameters                       */
    /*****************************************************/
    bParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    bParams.quantum           = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    bParams.dropProfileInd    = CPSS_TM_NO_DROP_PROFILE_CNS;
    bParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_FP0_E;
    for (i = 0; i < 8; i++)
       bParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_DWRR_E;


    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr  = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    aParams.quantum            = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    aParams.dropProfileInd     = CPSS_TM_NO_DROP_PROFILE_CNS;
    aParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_FP0_E;
    for (i = 0; i < 8; i++)
        aParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_DWRR_E;

    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
    queueParams.shapingProfilePtr  = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum            = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    queueParams.dropProfileInd     = CPSS_TM_NO_DROP_PROFILE_CNS;

    /* create 10 "dummy" shaping profiles per level, in actual scenario they will be changed with updateShaper*/
    for (tmLevel = CPSS_TM_LEVEL_Q_E; tmLevel < CPSS_TM_LEVEL_P_E; tmLevel++)
    {
        profileShaping.cirBw = 200000; /* 200M */
        profileShaping.cbs = 80;  /* Kbytes */
        profileShaping.eirBw = 0;
        profileShaping.ebs = 0;

        osPrintf("\n%d level shaping profile indexes: ",tmLevel);
        for (i = 0; i < 10; i++)
        {
            rc = cpssTmShapingProfileCreate(dev, tmLevel, &profileShaping, &profileShapingInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
            if (rc != GT_OK)
                return rc;
            osPrintf(" %d,", profileShapingInd);
        }
    }

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/

    tmPortsArrSize = MIN(portsNum, 64);
    rc = gtAppDemoTmGetTmPortsIds(dev, &tmPortsArrSize, tmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portsNum = MIN(portsNum, tmPortsArrSize);

    rc = cpssTmTreeParamsGet(dev, &tmTreeParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTreeParamsGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0; i < portsNum; i++) 
    {
        tmPort = tmPortsArr[i];

        /* queue 0 is excluded from the pool */
        totalQueues += MIN(cNodesNum*bNodesNum*aNodesNum*queuesNum, tmTreeParams.maxQueues-1);

        if (totalQueues > tmTreeParams.maxQueues-1)
        {
            cpssOsPrintf("tmTreeGrow: total queues: %d is exceeded max queues after %d tm ports\n", totalQueues, i);
            break;
        }

        if (portsNum >= 48 )     
        {
            /*
            rc = bobcat2BoardTypeGet(dev, 2, &bc2BoardType);
            if (rc == GT_OK &&
                (bc2BoardType == APP_DEMO_BC2_BOARD_DB_CNS ||
                 bc2BoardType == APP_DEMO_BC2_BOARD_RD_MSI_CNS ||
                 bc2BoardType == APP_DEMO_BC2_BOARD_RD_MTL_CNS))
            {
                tmPortBw = CPSS_TM_PORT_BW_10G_E;
            } 
            */
            tmPortBw = CPSS_TM_PORT_BW_10G_E;
            portParams.cirBw = 10000000; /* 10G */
            portParams.eirBw = 0;
            portParams.cbs = 80;
            portParams.ebs = 0;
        }
        else
        {
            tmPortBw = CPSS_TM_PORT_BW_1G_E;
            portParams.cirBw = 1000000; /* 1G */
            portParams.eirBw = 0;
            portParams.cbs = 8;
            portParams.ebs = 0;
        }

        /*create port*/
        rc = cpssTmPortCreate(dev, 
                              tmPort, 
                              tmPortBw, 
                              &portParams, 
                              cNodesNum,
                              cNodesNum*bNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum, 
                              MIN(cNodesNum*bNodesNum*aNodesNum*queuesNum, tmTreeParams.maxQueues-1));                

        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
            return rc;

        for (c = 0; c < cNodesNum; c++) 
        {
            /*create c node*/
            rc = cpssTmCnodeToPortCreate(dev, tmPort, &cParams, &cNodeInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
            if (rc != GT_OK)
                return rc;

            for (b = 0; b < bNodesNum; b++) 
            {
                /*create b node*/
                rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
                if (rc != GT_OK)
                    return rc;

                for (a = 0; a < aNodesNum; a++) 
                {
                    /*create a node*/
                    rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
                    if (rc != GT_OK)
                        return rc;

                    for (j = 0; j < queuesNum; j++) 
                    {
                        /*create queue node */
                        rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
                        queue_id_pool[queue_id_pool_size++] = queueId;
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
                        if (rc != GT_OK)
                            return rc;
                        qNodeTopology[queueId].aNode = aNodeInd;
                        qNodeTopology[queueId].bNode = bNodeInd;
                        qNodeTopology[queueId].cNode = cNodeInd;
                        qNodeTopology[queueId].pNode = tmPort;
                        qNodeTopology[queueId].exists = GT_TRUE;

                        /* patch till fix allocation of last queue */
                        if (queue_id_pool_size == tmTreeParams.maxQueues-2)
                        {                            
                            cpssOsPrintf("tmTreeGrow: max queues: %d has reached\n", queue_id_pool_size);
                            /* break all loops */
                            j = queuesNum;
                            a = aNodesNum; 
                            b = bNodesNum;
                            c = cNodesNum;
                            i = portsNum;
                        }
                    }
                }
            }
        }
    }

    /*install all queues*/
    for (i = 0; i < queue_id_pool_size; i++)
    {
        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
            return rc;
    }
    return GT_OK;
}


/*******************************************************************************
* appDemoTmScenario6Init
*
* DESCRIPTION:
*      Implements scenario 6 initialization for specific device.
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
GT_STATUS appDemoTmScenario6Init
(
    IN  GT_U8   dev
)
{
    GT_STATUS           rc = GT_OK;

    cpssOsPrintf("\n ***********CPSS_TM_6*********** \n ");

    rc = tmTreeGrow(dev, 4, 1, 5, 2, 4);

    totalQueuesPerPort = 1 * 5 * 2 * 4;

    return rc;
}

/*******************************************************************************
* appDemoTmScenario7Init
*
* DESCRIPTION:
*      Implements scenario 5 initialization for specific device.
*      4 1G ports, 1 c node per port, 2 b node per c node, 2 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p), propagated priority.
*      Tail Drop on port wred drop all queues
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
GT_STATUS appDemoTmScenario7Init
(
    IN  GT_U8   dev
)
{
    GT_STATUS           rc = GT_OK;

    cpssOsPrintf("\n ***********CPSS_TM_7*********** \n ");

    rc = tmTreeGrow(dev, 1, 2, 2, 2, 8);

    totalQueuesPerPort = 2 * 2 * 2 * 8;

    getTMQCounters(0);

    return rc;


} /* appDemoTmScenario7Init */


/*******************************************************************************
* appDemoTmScenario8Init
*
* DESCRIPTION:
*      Implements scenario 8 initialization for specific device.
*  In this scenario there are shapers for every (VLAN,TC) pair - on init
*  TC 0 gets shaper 1Mb, TC1 - 2Mb and so on
* In this scenario we also test "mixed mode", where port 0 is enabled for TM
* an the rest of ports are not.
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
GT_STATUS appDemoTmScenario8Init
(
    IN  GT_U8   dev
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC  profileShaping;
    GT_U32                              i, qProfileShapingInd;
    GT_U32                              aNodes;
    GT_U32                              queues;
    CPSS_TM_TREE_PARAMS_STC             tmTreeParams; 

    cpssOsPrintf("\n ***********CPSS_TM_8*********** \n ");

    rc = cpssTmTreeParamsGet(dev, &tmTreeParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTreeParamsGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
     
    queues = 8;
    aNodes = tmTreeParams.maxQueues / queues;
    aNodes = MIN(tmTreeParams.maxAnodes, aNodes);

    rc = tmTreeGrow(dev, 1, 1, 1, aNodes, queues);
    if (rc != GT_OK)
        return rc;

    totalQueuesPerPort = 1 * 1 * aNodes * queues;

    /* we will need to redefine all shapers */
    for (i = 1; i < 41; i++)
    {
        rc = cpssTmShapingProfileDelete(dev, i);
        if (rc != GT_OK)
            return rc;
    }

    for (i = 1; i < (aNodes * 8); i++)
    {
        /* create shaping profile per node */
        profileShaping.cirBw = 10000; /* 10 M    */
        profileShaping.cbs = 20;
        profileShaping.eirBw = 0; /* 0 */
        profileShaping.ebs = 0;

        rc = cpssTmShapingProfileCreate(dev, CPSS_TM_LEVEL_Q_E, &profileShaping, &qProfileShapingInd);
        if (rc != GT_OK)
            return rc;
    }
    /* the rest of configuration will not be in commands script but in scenario8Set */

    return rc;

} /* appDemoTmScenario8Init */

/*******************************************************************************
* appDemoTmScenario9Init
*
* DESCRIPTION:
*      Scenario for demonstration of PFC. There is one C-node for one Q-node, including nodes
*      in between
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
GT_STATUS appDemoTmScenario9Init
(
    IN  GT_U8   dev
)
{
    GT_STATUS           rc = GT_OK;

    cpssOsPrintf("\n ***********CPSS_TM_9*********** \n ");

    rc = tmTreeGrow(dev, 1, 8, 1, 1, 1);
    if (rc != GT_OK)
        return rc;

    totalQueuesPerPort = 8 * 1 * 1 * 1;

    getTMQCounters(0);

    /* the rest of configuration will not be in commands script but in scenario9Set */
    return rc;

} /* appDemoTmScenario9Init */

/*******************************************************************************
* appDemoTmRev4ScenarioInit
*
* DESCRIPTION:
*      Scenario for demonstration of PFC. There is one C-node for one Q-node, including nodes
*      in between
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
GT_STATUS appDemoTmRev4ScenarioInit
(
    IN  GT_U8   dev
)
{
    GT_STATUS           rc = GT_OK;

    cpssOsPrintf("\n ***********CPSS_TM_REV4*********** \n ");

    rc = tmTreeGrow(dev, 1, 1, 1, 1, 128);
    if (rc != GT_OK)
        return rc;

    totalQueuesPerPort = 128 * 1 * 1 * 1;

    getTMQCounters(0);

    /* the rest of configuration will not be in commands script but in scenario9Set */
    return rc;

} /* appDemoTmRev4ScenarioInit */

/*******************************************************************************
* appDemoTmStressScenarioInit
*
* DESCRIPTION:
*      Implements 8 ports with 10G BW. each
*       8 ports, 512 C nodes, 2K B nodes, 4K A nodes, 16K queues
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
GT_STATUS appDemoTmStressScenarioInit
(
    IN  GT_U8   dev,
    IN  GT_U8   mode
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC   params[5];
    CPSS_TM_PORT_PARAMS_STC             portParams;
    CPSS_TM_QUEUE_PARAMS_STC            queueParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC     profile;
    CPSS_TM_DROP_PROFILE_PARAMS_STC     profile_cos;
    GT_U32                              portDropProfileId;
    GT_U32                              queueDropProfileId;
    GT_U32                              queueId=0;
    GT_U32                              cos;
    GT_U32                              cNodeInd;
    GT_U32                              bNodeInd;
    GT_U32                              aNodeInd;
    CPSS_TM_C_NODE_PARAMS_STC           cParams;
    CPSS_TM_B_NODE_PARAMS_STC           bParams;
    CPSS_TM_A_NODE_PARAMS_STC           aParams;
    GT_U32                              i, j, ii, c, b, a;
    GT_U32                              a_node_counter=0;
    GT_U32                              portDropProfileCos0Id;
    GT_U32                              portDropProfileCos1Id;
    GT_U32                              portDropProfileCos2Id;
    GT_U32                              portDropProfileCos3Id;

    GT_U8                               port_elig_fun;
    GT_U8                               c_node_elig_fun;
    GT_U8                               b_node_elig_fun;
    GT_U8                               a_node_elig_fun;
    GT_U8                               queue_elig_fun_flag;
    GT_U8                               port_drop_flag;
    GT_U8                               port_shaping_flag;
    CPSS_TM_TREE_PARAMS_STC             tmTreeParams; 

    GT_U32                              allTmPortsArr[80];
    GT_U32                              allTmPortsArrSize = 80;
    GT_U32                              tmPortsArr[12];
    GT_U32                              tmPortsArrSize = 12;
    GT_U32                              tmPort = 56;

    GT_U32                              portsNum;
    GT_U32                              cNodesNum = 0; 
    GT_U32                              bNodesNum = 0; 
    GT_U32                              aNodesNum = 0; 
    GT_U32                              queuesNum = 0;
    GT_U32                              totalQueuesPerPort;
    CPSS_TM_ELIG_FUNC_NODE_ENT          node_elig_func;

    rc = gtAppDemoTmGetTmPortsIds(dev, &allTmPortsArrSize, allTmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    appDemoTmGetValidTmPorts(allTmPortsArrSize, 
                             allTmPortsArr,                              
                             tmPort, 
                             &tmPortsArrSize,
                             tmPortsArr);
    
    /*    
    if (tmPortsArrSize == 0) - possible on initSystem 29,1
    {
        rc = GT_FAIL;   
        CPSS_ENABLER_DBG_TRACE_RC_MAC("Tm Ports Mapping is Missing", rc);
        return rc;
    }
    */


    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Todo: adjust scenario modes by tmTree Params */
    rc = cpssTmTreeParamsGet(dev, &tmTreeParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTreeParamsGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    portsNum = tmPortsArrSize;

    /* Prepare test configuration according to mode*/

    if (mode < 4)
    {
        cpssOsPrintf("\n***********CPSS_TM_STRESS_%d_PORTS***********\n", portsNum); 
    }

    appDemoDbEntryGet("tmStressCnodes", &cNodesNum);
    appDemoDbEntryGet("tmStressBnodes", &bNodesNum);
    appDemoDbEntryGet("tmStressAnodes", &aNodesNum);
    appDemoDbEntryGet("tmStressQueues", &queuesNum);

    /* for all cases */
    if (tmTreeParams.maxQueues > (4 * 1024)) /* BC2 */
    {
        if (!cNodesNum) 
            cNodesNum = 31; 
        if (!bNodesNum) 
            bNodesNum = 1; 
        if (!aNodesNum) 
            aNodesNum = 11; 
        if (!queuesNum) 
            queuesNum = 4;
    }
    else                                /* Bobk */
    {
        if (!cNodesNum) 
            cNodesNum = 14; 
        if (!bNodesNum) 
            bNodesNum = 1; 
        if (!aNodesNum) 
            aNodesNum = 6; 
        if (!queuesNum) 
            queuesNum = 4;
    }

    switch (mode)
    {
        case 0:
        case 2:
        case 3:
        {
            cpssOsPrintf("* %d * %d * %d * %d, Q no drop, Port drop 10G global only *\n",
                         cNodesNum,
                         bNodesNum,
                         aNodesNum,
                         queuesNum); 

            port_drop_flag = 1;
            port_shaping_flag = GT_TRUE;
            queue_elig_fun_flag = 1;

            if (mode == 0)
            {
                node_elig_func = CPSS_TM_ELIG_N_PP_E;
                cpssOsPrintf("\tQueue priority = q_index % 4,  A, B, C, Port have propagated priority\n"); 
            }
            else if (mode == 2)
            {
                node_elig_func = CPSS_TM_ELIG_N_PRIO1_E;
                cpssOsPrintf("\tQueue priority = q_index % 4,  A, B, C, Port have equal priority 1\n");
            }
            else
            {
                node_elig_func = CPSS_TM_ELIG_N_FP0_E;
                cpssOsPrintf("\tQueues, A, B, C ,Ports have equal priority 0\n");
                cpssOsPrintf("\tNO DROP ON PORT LEVEL, NO SHAPING ON PORT LEVEL!!!\n");
                port_drop_flag = 0;
                port_shaping_flag = GT_FALSE;
                queue_elig_fun_flag = 0;
            }
            
            totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;

            cpssOsPrintf("\n* Total Queues: %d *\n", (totalQueuesPerPort * portsNum));
            cpssOsPrintf("* Total Queues Per Port: %d *\n", totalQueuesPerPort);

            portParams.numOfChildren = cNodesNum;
            cParams.numOfChildren    = bNodesNum;
            bParams.numOfChildren    = aNodesNum;
            aParams.numOfChildren    = queuesNum;

            port_elig_fun   = node_elig_func;
            c_node_elig_fun = node_elig_func;
            b_node_elig_fun = node_elig_func;
            a_node_elig_fun = node_elig_func;            
            break;
        }
        case 1:
        case 4:
        {
            queuesNum = (cNodesNum * bNodesNum * aNodesNum * queuesNum);
            cNodesNum = 1; 
            bNodesNum = 1; 
            aNodesNum = 1; 

            port_drop_flag = 1;
            port_shaping_flag = GT_TRUE;
            queue_elig_fun_flag = 1;

            cpssOsPrintf("* %d * %d * %d * %d, Q no drop, Port drop 10G global only\n",
                         cNodesNum,
                         bNodesNum,
                         aNodesNum,
                         queuesNum); 

            if (mode == 1)
            {
                node_elig_func = CPSS_TM_ELIG_N_PP_E;
                cpssOsPrintf("\tQueue priority = q_index % 4,  A, B, C, Port have propagated priority\n");
            }
            else 
            {
                node_elig_func = CPSS_TM_ELIG_N_FP0_E;
                cpssOsPrintf("\tQueues, A, B, C ,Ports have equal priority 0\n");
                cpssOsPrintf("\tNO DROP ON PORT LEVEL, NO SHAPING ON PORT LEVEL!!!\n");
                port_drop_flag = 0;
                port_shaping_flag = GT_FALSE;
                queue_elig_fun_flag = 0;
            }

            totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;

            cpssOsPrintf("\n* Total Queues: %d *\n", (totalQueuesPerPort * portsNum));
            cpssOsPrintf("* Total Queues Per Port: %d *\n", totalQueuesPerPort);

            portParams.numOfChildren = cNodesNum;
            cParams.numOfChildren    = bNodesNum;
            bParams.numOfChildren    = aNodesNum;
            aParams.numOfChildren    = queuesNum;
            
            port_elig_fun   = node_elig_func;
            c_node_elig_fun = node_elig_func;
            b_node_elig_fun = node_elig_func;
            a_node_elig_fun = node_elig_func;

            break;
        }
#if 0
        case 2:
        {
            if (tmTreeParams.maxQueues > 4*1024) /* BC2 */
            {
                cNodesNum = 31; 
                bNodesNum = 1; 
                aNodesNum = 11; 
                queuesNum = 4;
            }
            else                                /* Bobk */
            {
                cNodesNum = 8; 
                aNodesNum = 12;
            }

            cpssOsPrintf("* %d * %d * %d * %d, Q no drop, Port drop 10G global only\n",
                         cNodesNum,
                         bNodesNum,
                         aNodesNum,
                         queuesNum); 
            cpssOsPrintf("\tQueue priority = q_index % 4,  A, B, C, Port have equal priority 1\n");

            totalQueuesPerPort = cNodesNum * bNodesNum * aNodesNum * queuesNum;

            cpssOsPrintf("\n*Total Queues Per Port: %d*\n", totalQueuesPerPort);

            portParams.numOfChildren = cNodesNum;
            cParams.numOfChildren    = bNodesNum;
            bParams.numOfChildren    = aNodesNum;
            aParams.numOfChildren    = queuesNum;

            port_elig_fun=   CPSS_TM_ELIG_N_PRIO1_E;
            c_node_elig_fun= CPSS_TM_ELIG_N_PRIO1_E;
            b_node_elig_fun= CPSS_TM_ELIG_N_PRIO1_E;
            a_node_elig_fun= CPSS_TM_ELIG_N_PRIO1_E;
            queue_elig_fun_flag = 1;
            port_drop_flag=1;
            port_shaping_flag=GT_TRUE;

            break;
        }
        case 3:
        {
            cpssOsPrintf("* 31 * 1 * 11 * 4 , Q no drop\n ");
            cpssOsPrintf("\tall Queues , A, B, C ,Ports  have  equal priority 0\n");
            cpssOsPrintf("\tNO DROP ON PORT LEVEL, NO SHAPING ON PORT LEVEL !!!\n");
            portParams.numOfChildren=31;
            cParams.numOfChildren=1;
            bParams.numOfChildren=11;
            aParams.numOfChildren=4;

            port_elig_fun=   CPSS_TM_ELIG_N_FP0_E;
            c_node_elig_fun= CPSS_TM_ELIG_N_FP0_E;
            b_node_elig_fun= CPSS_TM_ELIG_N_FP0_E;
            a_node_elig_fun= CPSS_TM_ELIG_N_FP0_E;
            queue_elig_fun_flag = 0;
            port_drop_flag=0;
            port_shaping_flag=GT_FALSE;
            break;
        }
        case 4:
        {
            cpssOsPrintf("* 1 * 1 * 1 * (31*11*4) , Q no drop \n");
            cpssOsPrintf("\tall Queues , A, B, C ,Ports  have  equal priority 0\n");
            cpssOsPrintf("\tNO DROP ON PORT LEVEL, NO SHAPING ON PORT LEVEL !!!\n");
            portParams.numOfChildren=1;
            cParams.numOfChildren=1;
            bParams.numOfChildren=1;
            aParams.numOfChildren=4*11*31;

            port_elig_fun=   CPSS_TM_ELIG_N_FP0_E;
            c_node_elig_fun= CPSS_TM_ELIG_N_FP0_E;
            b_node_elig_fun= CPSS_TM_ELIG_N_FP0_E;
            a_node_elig_fun= CPSS_TM_ELIG_N_FP0_E;
            queue_elig_fun_flag = 0;
            port_drop_flag=0;
            port_shaping_flag=GT_TRUE;
            break;
        }
#endif
        default:
        {
            cpssOsPrintf("\n ***CPSS_TM_STRESS_TEST  -  unexpected mode number:%d, exiting ...\n ", mode);
            cpssTmClose(dev);
            return GT_FAIL;
        }
    }

    i = 0;
    cpssOsPrintf("tmPorts: %02d, ", tmPortsArr[i++]);
    for (;i < tmPortsArrSize; i++)
    {    
        cpssOsPrintf("%02d, ", tmPortsArr[i]);
    }

    cpssOsPrintf("\n");

    queue_id_pool_size = 0;

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/
    /* no shaping on all levels*/
    for (i = CPSS_TM_LEVEL_Q_E; i < CPSS_TM_LEVEL_P_E; i++) 
    {
        params[i].periodicState = GT_FALSE;
        params[i].shaperDecoupling = GT_FALSE;
    }
    params[CPSS_TM_LEVEL_P_E].periodicState = port_shaping_flag;
    params[CPSS_TM_LEVEL_P_E].shaperDecoupling = GT_FALSE;

    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes( 1000000); /* 1Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &portDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    profile_cos.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile_cos.cbTdThresholdBytes = cpssTmDropConvertBWToBytes( 10000000 / 4 ); /* 10 Gbps / 4 */

    cos = 0;
    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile_cos, &portDropProfileCos0Id);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    cos = 1;
    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile_cos, &portDropProfileCos1Id);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    cos = 2;
    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile_cos, &portDropProfileCos2Id);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    cos = 3;
    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile_cos, &portDropProfileCos3Id);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/
    portParams.cirBw = 10000000;  /*10 G */
    portParams.cbs = 8 ; /*KBytes*/
    portParams.eirBw = 0;
    portParams.ebs = 0x1FFFF;
    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }

    if (port_drop_flag)     
        portParams.dropProfileInd = portDropProfileId;
    else                    
        portParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;
    portParams.eligiblePrioFuncId = port_elig_fun;

    /*****************************************************/
    /* Configure C parameters                        */
    /*****************************************************/
    cParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    cParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap = 0;
    cParams.eligiblePrioFuncId = c_node_elig_fun;

    for (ii = 0; ii < 8; ii++)
    {
        cParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E;
        cParams.dropProfileIndArr[ii] = 0;
    }

    /*****************************************************/
    /* Configure B parameters                        */
    /*****************************************************/
    bParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    bParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        bParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    bParams.dropProfileInd = 0;
    bParams.eligiblePrioFuncId = b_node_elig_fun;

    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    aParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        aParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    aParams.dropProfileInd = 0;
    aParams.eligiblePrioFuncId = a_node_elig_fun;

    /*****************************************************/
    /* Create Queue Drop profile                         */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(5000);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
 /*   queueParams.dropProfileInd = queueDropProfileId; */
    queueParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;

    /* by default all queues have the same priority,  if    queue_elig_fun_flag = 1 it will be changed during creation */
    queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/

    /**************************************************************************/
    /* validate (and update if necessary)  cbs & ebs values                   */
    /**************************************************************************/
    rc = cpssTmPortShapingValidate(dev, CPSS_TM_PORT_BW_10G_E, portParams.cirBw, portParams.eirBw , &(portParams.cbs), &(portParams.ebs));
    /* 
      if rc =GT_BAD_SIZE - configuration error is fixed inside function(this error is like warning here) 
     */
    if ((rc != GT_OK) && (rc != GT_BAD_SIZE))
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortShapingValidate", rc);
        return rc;
    }


    for (i = 0; i < tmPortsArrSize; i++) 
    {
        tmPort = tmPortsArr[i];
        /*create port*/
        rc = cpssTmAsymPortCreate(dev, tmPort, CPSS_TM_PORT_BW_10G_E, &portParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAsymPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (c = 0; c < portParams.numOfChildren; c++) 
        {
            /*create c node*/
            rc = cpssTmCnodeToPortCreate(dev, tmPort, &cParams, &cNodeInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            for (b = 0; b < cParams.numOfChildren; b++) 
            {
                /*create b node*/
                rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                for (a = 0; a < bParams.numOfChildren; a++) 
                {
                    /*create a node*/
                    /* temporary  queue 0 is excluded from the pool - patch for H/W bug*/
                    /* so for the last A node we have only 3 children here*/
                    if (/*4095*/ tmTreeParams.maxAnodes-1 == a_node_counter++) 
                        aParams.numOfChildren=3;

                    rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    for (j = 0; j < aParams.numOfChildren; j++)
                    {
                        if (queue_elig_fun_flag)
                        {
                            switch(j % 4 )
                            {
                                case 0:
                                    queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                                    break;
                                case 1:
                                    queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO1_E;
                                    break;
                                case 2:
                                    queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO2_E;
                                    break;
                                case 3:
                                    queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO3_E;
                                    break;
                                default:
                                    return GT_FAIL;
                            }
                        }
                        /*create queue node */
                        /* temporary  queue 0 is excluded from the pool - patch for H/W bug*/
                        /* here workaround foor this - only in order to keep test running*/
                        if (queue_id_pool_size < QUEUE_POOL_SIZE_CNS)
                        {
                            rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
                            queue_id_pool[queue_id_pool_size++]=queueId;
                            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }
/*                          rc = cpssTmDropQueueCosSet(dev,queueId,j);
                            CPSS_ENABLER_DBG_TRACE_RC_MAC("ccpssTmDropQueueCosSet", rc);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }
*/
                        }
                        /* */
                    }
                }
            }
        }
    }
    /* currently no port drop per cos
    for(i=56; i<64; i++)
    {
        rc=cpssTmPortDropPerCosConfig(dev, i, &portDropPerCosParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortDropPerCosConfig", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    */
    /**/
    /*install all queues*/
    for (i = 0; i < queue_id_pool_size; i++)
    {
        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }


    return rc;
} /* appDemoTmStressScenarioInit */

/*******************************************************************************
* appDemoTmGeneral4PortsInit
*
* DESCRIPTION:
*      Implements general 4 Ports initialization for specific device.
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
*       For Cos Port mode the next APi should be run!!!! AA TBD
*
*******************************************************************************/
GT_STATUS appDemoTmGeneral4PortsInit
(
    IN  GT_U8   dev
)
{
    GT_STATUS           rc = GT_OK;

    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC params[5];
    CPSS_TM_PORT_PARAMS_STC           portParams;
    CPSS_TM_PORT_DROP_PER_COS_STC     portCosParams;
    CPSS_TM_QUEUE_PARAMS_STC          queueParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC   profile;
    GT_U32                            profileId;
    GT_U32                            profileCosId;
    GT_U32                            queueId;
    GT_U32                            portsNum;
    GT_U32                            queuesNum;
    GT_U32                            portMode;
    GT_U32                            cos;
    GT_U32                            i, j;
    GT_U32                            tmPortsArr[64];
    GT_U32                            tmPortsArrSize;
    GT_U32                            tmPort;

    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/
    for (i = CPSS_TM_LEVEL_Q_E; i < CPSS_TM_LEVEL_P_E; i++) 
    {
        params[i].periodicState = GT_FALSE;
        params[i].shaperDecoupling = GT_FALSE;
    }

    params[CPSS_TM_LEVEL_P_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_P_E].shaperDecoupling = GT_FALSE;

    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;

    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(10000000); /* 10Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &profileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Optional for CoS Port mode: create  profile for Cos */
    cos = 2;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(1000000); /* 1 Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &profileCosId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/
    portParams.cirBw = 10000000; /* 10G */
    portParams.eirBw = 0;
    portParams.cbs = 10;  /*KBytes*/
    portParams.ebs = 0x1FFFF;
    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = profileId;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_E;

    /* Optional for CoS Port mode: Port CoS parameters configuration */
    portCosParams.dropCosMap = 1 << cos;
    for (i = 0; i < 8; i++)
        portCosParams.dropProfileIndArr[i] = CPSS_TM_NO_DROP_PROFILE_CNS;
    portCosParams.dropProfileIndArr[cos] = profileCosId;

    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    queueParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;

    /**************************************************************************/
    /* validate (and update if necessary)  cbs & ebs values                   */
    /**************************************************************************/
    rc = cpssTmPortShapingValidate(dev, CPSS_TM_PORT_BW_10G_E, portParams.cirBw, portParams.eirBw , &(portParams.cbs), &(portParams.ebs));

    if ((rc != GT_OK) && (rc != GT_BAD_SIZE))
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortShapingValidate", rc);
        return rc;
    }

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/
    portsNum = 4;
    queuesNum = 8; /* per port */

    tmPortsArrSize = portsNum;
    rc = gtAppDemoTmGetTmPortsIds(dev, &tmPortsArrSize, tmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portsNum = MIN(portsNum, tmPortsArrSize);
    queuesNum = 8; /* per port */

    for (i = 0; i < portsNum; i++) 
    {
        tmPort = tmPortsArr[i];

        rc = cpssTmPortCreate(dev, tmPort, CPSS_TM_PORT_BW_10G_E, &portParams, 1, 1, 1, queuesNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Optional for CoS Port mode */
        rc = cpssTmPortDropPerCosConfig(dev, tmPort, &portCosParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortDropPerCosConfig", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (j = 0; j < queuesNum; j++) 
        {
            switch(j) 
            {
            case 0:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                break;
            case 1:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO1_E;
                break;
            case 2:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO2_E;
                break;
            case 3:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO3_E;
                break;
            case 4:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO4_E;
                break;
            case 5:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO5_E;
                break;
            case 6:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO6_E;
                break;
            case 7:
                queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO7_E;
                break;
            default:
                return GT_FAIL;
            }
            rc = cpssTmTransQueueToPortCreate(dev, tmPort, &queueParams, &queueId);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTransQueueToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssTmNodesCtlQueueInstall(dev, queueId);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Optional: for Cos Port configuration */
            rc = cpssTmDropQueueCosSet(dev, queueId, cos);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropQueueCosSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* Optional for CoS Port mode: choose Port mode */
    portMode = 0; /* 0 - Global Mode, 1 - CoS Mode */
    rc = cpssTmDropQueryResponceSet(dev, portMode, CPSS_TM_LEVEL_Q_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropQueryResponceSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    return rc;
} /* appDemoTmGeneral4PortsInit */

GT_STATUS appDemoTmInitStressConfiguration
(
    IN  GT_U8   dev,
    IN  GT_U8   param
)
{
    GT_STATUS   rc;         /* return code */
    cpssOsPrintf("appDemoTmInitStressConfiguration: dev: %d, mode: %d\n", dev, param);
    rc = appDemoTmStressScenarioInit(dev, param);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoTmInitStressConfiguration", rc);
    if (rc == GT_OK)
    {
        cpssOsPrintf("appDemoTmInitStressConfiguration: dev: %d, mode: %d, successful\n", dev, param);
    }
    else
    {
        cpssOsPrintf("appDemoTmInitStressConfiguration: dev: %d, mode: %d, failed rc: %d\n", dev, param, rc);
    }
    return rc;
}

/*******************************************************************************
* appDemoTmScenarioSimpleShaping
*
* DESCRIPTION:
*      Implements scenario for  testing shaping.
*      1 10M port, 1 c node per port, 1 b node per c node, 1 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p), propagated priority.
*      Tail Drop on port and queue level and color aware on Q1 in each A node and wred mode on queue0 in each A node
*
* INPUTS:
*      dev     -   device number
*      param     -   parameter for customization configuration
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
GT_STATUS appDemoTmScenarioSimpleShaping
(
    IN  GT_U8   dev,
    IN  GT_U8   param
)
{
    GT_STATUS                               rc = GT_OK;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC       params[5];
    CPSS_TM_PORT_PARAMS_STC                 portParams;
    CPSS_TM_QUEUE_PARAMS_STC                queueParams;
    CPSS_TM_C_NODE_PARAMS_STC               cParams;
    CPSS_TM_B_NODE_PARAMS_STC               bParams;
    CPSS_TM_A_NODE_PARAMS_STC               aParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC         profile;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC      profileShaping;
    CPSS_TM_LEVEL_ENT                       level;
    GT_U32                                  cProfileShapingIndPtr;
    GT_U32                                  bProfileShapingIndPtr;
    GT_U32                                  aProfileShapingIndPtr;
    GT_U32                                  qProfileShapingIndPtr;
    GT_U32                                  cNodeInd;
    GT_U32                                  bNodeInd;
    GT_U32                                  aNodeInd;
    GT_U32                                  portDropProfileId;
    GT_U32                                  queueDropProfileIdWred;
    GT_U32                                  queueDropProfileIdCa;
    GT_U32                                  queueDropProfileId1; 
    GT_U32                                  queueDropProfileId05;
    GT_U32                                  queueId;             
    GT_U32                                  portsNum;            
    GT_U32                                  queuesNum;           
    GT_U32                                  aNodesNum;           
    GT_U32                                  bNodesNum;           
    GT_U32                                  cNodesNum;           
    GT_U32                                  cos;                 
    GT_U32                                  i, j, ii, c, b, a;   
    GT_U32                                  maxProbability=50;   
    GT_U32                                  curvePtr;            
    GT_U32                                  tmPortsArr[64];
    GT_U32                                  tmPortsArrSize;
    GT_U32                                  tmPort;

    cpssOsPrintf("\n ***********CPSS_TM_4 Simple shaping , param=%d*********** \n ",param);

    queue_id_pool_size = 0;

    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/

        /*port level*/
    params[CPSS_TM_LEVEL_P_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_P_E].shaperDecoupling = GT_FALSE;
    /* c level*/
    params[CPSS_TM_LEVEL_C_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_C_E].shaperDecoupling = GT_FALSE;
    /* b level*/
    params[CPSS_TM_LEVEL_B_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_B_E].shaperDecoupling = GT_FALSE;
    /* a level*/
    params[CPSS_TM_LEVEL_A_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_A_E].shaperDecoupling = GT_FALSE;
    /* q level*/
    params[CPSS_TM_LEVEL_Q_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_Q_E].shaperDecoupling = GT_FALSE;

    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(1000000); /* 1Gbps */
 
    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &portDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************/
    /* Configure C level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_C_E;
    profileShaping.cirBw = 10000; /* 10M */
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &cProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure B level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_B_E;
    profileShaping.cirBw = 10000; /* 10M */
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &bProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure A level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_A_E;
    profileShaping.cirBw = 10000; /* 10M */
    profileShaping.cbs = 8;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &aProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Queue shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_Q_E;
    profileShaping.cirBw = 5000; /* 5M */
    profileShaping.cbs = 8; /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &qProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/
    portParams.cirBw = 10000000; /* 10G */
    portParams.eirBw = 0;
    portParams.cbs = 8; /*KBytes*/
    portParams.ebs = 0;
    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = portDropProfileId;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E; /* CPSS_TM_ELIG_N_SHP_PP_TB_E;*/

    /*****************************************************/
    /* Configure C parameters                        */
    /*****************************************************/
    cParams.shapingProfilePtr = cProfileShapingIndPtr;
    cParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap = 1;
    cParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E;
    for (ii = 0; ii < 8; ii++)
    {
        cParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E;
        cParams.dropProfileIndArr[ii] = 0;
    }

    /*****************************************************/
    /* Configure B parameters                        */
    /*****************************************************/
    bParams.shapingProfilePtr = bProfileShapingIndPtr;
    bParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        bParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    bParams.dropProfileInd = 0;
    bParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E;

    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr= aProfileShapingIndPtr;
    aParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        aParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    aParams.dropProfileInd = 0;
    aParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_PP_E;

    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
/*    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS; */
    queueParams.shapingProfilePtr = qProfileShapingIndPtr;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;

    /*****************************************************/
    /* Configure Queue Traditional curve                        */
    /*****************************************************/
    rc = cpssTmDropWredTraditionalCurveCreate(dev,CPSS_TM_LEVEL_Q_E, 0, maxProbability, &curvePtr);
    if (rc != GT_OK) {
        return rc;
    }

    /*****************************************************/
    /* Create Queue Drop profile  Wred                        */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
/*
 * By default system is configured for use 2 colors
 * but here we will set color number explicitly
*/
    cpssTmDropColorNumSet(dev, CPSS_TM_LEVEL_Q_E,CPSS_TM_COLOR_NUM_2_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropColorNumSet", rc);

    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_CA_WRED_E;
    profile.cbTdThresholdBytes = 0;
    profile.caWredDp.aqlExponent = 0;
    profile.caWredDp.curveIndex[0] = (GT_U8)curvePtr;
    profile.caWredDp.curveIndex[1] = (GT_U8)curvePtr;
    profile.caWredDp.dpCurveScale[0] = 0;
    profile.caWredDp.dpCurveScale[1] = 0;
    profile.caWredDp.caWredTdMinThreshold[0] = 0;
    profile.caWredDp.caWredTdMinThreshold[1] = 0;
    profile.caWredDp.caWredTdMaxThreshold[0] = cpssTmDropConvertBWToBytes(2000);
    profile.caWredDp.caWredTdMaxThreshold[1] = cpssTmDropConvertBWToBytes(500);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileIdWred);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Queue Drop profile CA                    */
    /*****************************************************/
/*
 * By default system is configured for use 2 colors
 * but here we will set color number explicitly
*/
    cpssTmDropColorNumSet(dev, CPSS_TM_LEVEL_Q_E,CPSS_TM_COLOR_NUM_2_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropColorNumSet", rc);

    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_CA_TD_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(500);
    profile.caTdDp.caTdThreshold[0] = cpssTmDropConvertBWToBytes(1000);
    profile.caTdDp.caTdThreshold[1] = cpssTmDropConvertBWToBytes(250);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileIdCa);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************/
    /* Create Queue Drop profile CB                    */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(500);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId05);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(1000);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/
    portsNum = 1;
    cNodesNum = 1; /* per port */
    bNodesNum = 1; /* per c node */
    aNodesNum = 1; /* per b node */
    queuesNum = 8; /* per a node */

    /**************************************************************************/
    /* validate (and update if necessary)  cbs & ebs values                   */
    /**************************************************************************/
    rc = cpssTmPortShapingValidate(dev, CPSS_TM_PORT_BW_10G_E, portParams.cirBw, portParams.eirBw , &(portParams.cbs), &(portParams.ebs));

    tmPortsArrSize = portsNum;
    rc = gtAppDemoTmGetTmPortsIds(dev, &tmPortsArrSize, tmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portsNum = MIN(portsNum, tmPortsArrSize);

    for (i = 0; i < portsNum; i++) 
    {
        tmPort = tmPortsArr[i];

        /*create port*/
        rc = cpssTmPortCreate(dev, 
                              tmPort, 
                              CPSS_TM_PORT_BW_10G_E, 
                              &portParams, 
                              cNodesNum,
                              cNodesNum*bNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum*queuesNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (c = 0; c < cNodesNum; c++) 
        {
            /*create c node*/
            rc = cpssTmCnodeToPortCreate(dev, tmPort, &cParams, &cNodeInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            for (b = 0; b < bNodesNum; b++) 
            {
                /*create b node*/
                rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                for (a = 0; a < aNodesNum; a++) 
                {
                    /*create a node*/
                    rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    for (j = 0; j < queuesNum; j++) 
                    {
                        switch(j) 
                        {
                        case 0:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_MIN_SHP_PRIO0_E;
                            queueParams.dropProfileInd = queueDropProfileIdWred;
                            break;
                        case 1:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_MIN_SHP_PRIO1_E;
                            queueParams.dropProfileInd = queueDropProfileIdCa;
                            break;
                        case 2:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_MIN_SHP_PRIO2_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        case 3:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_MIN_SHP_PRIO3_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        case 4:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_MIN_SHP_PRIO4_E;
                            queueParams.dropProfileInd = queueDropProfileId1;
                            break;
                        case 5:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_MIN_SHP_PRIO5_E;
                            queueParams.dropProfileInd = queueDropProfileId1;
                            break;
                        case 6:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_MIN_SHP_PRIO6_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        case 7:
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_MIN_SHP_PRIO7_E;
                            queueParams.dropProfileInd = queueDropProfileId05;
                            break;
                        default:
                            return GT_FAIL;
                        }

                        /*create queue node */
                        rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
                        queue_id_pool[queue_id_pool_size++]=queueId;
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
            }
        }
    }

    /*install all queues*/
    for (i = 0; i < queue_id_pool_size; i++) 
    {
        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return rc;
} /* appDemoTmScenarioSimpleShaping */


GT_STATUS appDemoSimpleShapingInit
(
    IN  GT_U8   dev,
    IN  GT_U8   param
)
{
    GT_STATUS   rc;         /* return code */
   rc = appDemoTmScenarioSimpleShaping(dev,param);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoTmScenarioSimpleShaping", rc);
    if (rc == GT_OK)
    {
        cpssOsPrintf("\n appDemoTmScenarioSimpleShaping dev=%d , param=%d: successful  \n",dev, param);
    }
    else
    {
        cpssOsPrintf("\n appDemoTmScenarioSimpleShaping dev=%d, param=%d : failed  rc=%d \n",dev,param, rc);
    }
    return rc;
}

/*******************************************************************************
* appDemoTmScenario20Init
*
* DESCRIPTION:
*      Implements scenario 2 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p)
*      Tail Drop on port and queue level
*      port 64 - 40G   -> C -> B -> A -> 252 Queues
*      no shaping , no drop
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
GT_STATUS appDemoTmScenario20Init
(
    IN  GT_U8   dev
)
{
    GT_STATUS                               rc = GT_OK;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC       params[5];
    CPSS_TM_PORT_PARAMS_STC                 portParams;
    CPSS_TM_QUEUE_PARAMS_STC                queueParams;
    CPSS_TM_C_NODE_PARAMS_STC               cParams;
    CPSS_TM_B_NODE_PARAMS_STC               bParams;
    CPSS_TM_A_NODE_PARAMS_STC               aParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC         profile;
    CPSS_TM_SHAPING_PROFILE_PARAMS_STC      profileShaping;
    CPSS_TM_LEVEL_ENT                       level;
    GT_U32                                  cProfileShapingIndPtr;
    GT_U32                                  bProfileShapingIndPtr;
    GT_U32                                  aProfileShapingIndPtr;
    GT_U32                                  cNodeInd;
    GT_U32                                  bNodeInd;
    GT_U32                                  aNodeInd;
    GT_U32                                  portDropProfileId;
    GT_U32                                  queueDropProfileId2; /*0.5 M*/
    GT_U32                                  queueDropProfileId1; /*1 M*/
    GT_U32                                  queueId;
    GT_U32                                  portsNum;
    GT_U32                                  queuesNum;
    GT_U32                                  aNodesNum;
    GT_U32                                  bNodesNum;
    GT_U32                                  cNodesNum;
    GT_U32                                  cos;
    GT_U32                                  i, j, ii, c, b, a;
    GT_U32                                  portNo;
    GT_U32                                  tmPortsArr[64];
    GT_U32                                  tmPortsArrSize;
    GT_U32                                  tmPort;
    CPSS_TM_TREE_PARAMS_STC                 tmTreeParams; 
    GT_U32                                  totalQueues = 0;

    cpssOsPrintf("\n ***********CPSS_TM_2, 4 queues per port transparent***********\n ");

    queue_id_pool_size = 0;

    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/

        /*port level*/
    params[CPSS_TM_LEVEL_P_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_P_E].shaperDecoupling = GT_FALSE;
    /* c level*/
    params[CPSS_TM_LEVEL_C_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_C_E].shaperDecoupling = GT_FALSE;
    /* b level*/
    params[CPSS_TM_LEVEL_B_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_B_E].shaperDecoupling = GT_FALSE;
    /* a level*/
    params[CPSS_TM_LEVEL_A_E].periodicState = GT_TRUE;
    params[CPSS_TM_LEVEL_A_E].shaperDecoupling = GT_FALSE;
    /* q level*/
    params[CPSS_TM_LEVEL_Q_E].periodicState = GT_FALSE;
    params[CPSS_TM_LEVEL_Q_E].shaperDecoupling = GT_FALSE;

    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;

    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(1000000); /* 1Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, cos, &profile, &portDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************/
    /* Configure C level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_C_E;
    profileShaping.cirBw = 250000; /* 250M */
    profileShaping.cbs = 80;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &cProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure B level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_B_E;
    profileShaping.cirBw = 50000; /* 50M */
    profileShaping.cbs = 80;    /*KBytes*/
    profileShaping.eirBw = 0;
    profileShaping.ebs = 0;

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &bProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure A level shaping parameters              */
    /*****************************************************/

    level= CPSS_TM_LEVEL_A_E;
    profileShaping.cirBw = 5000; /* 5M */
    profileShaping.cbs = 80;    /*KBytes*/
    profileShaping.eirBw = 5000; /* 5M */
    profileShaping.ebs = 80; /*KBytes*/

    rc = cpssTmShapingProfileCreate(dev, level, &profileShaping, &aProfileShapingIndPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmShapingProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/
    portParams.cirBw = 1000000;
    portParams.eirBw = 0;
    portParams.cbs = 80; /*KBytes*/
    portParams.ebs = 0;
    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = portDropProfileId;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_FP7_E;

    /*****************************************************/
    /* Configure C parameters                        */
    /*****************************************************/
    cParams.shapingProfilePtr = cProfileShapingIndPtr;
    cParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap = 1;
    cParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_E;
    for (ii = 0; ii < 8; ii++)
    {
        cParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E;
        cParams.dropProfileIndArr[ii] = 0;
    }

    /*****************************************************/
    /* Configure B parameters                        */
    /*****************************************************/
    bParams.shapingProfilePtr = bProfileShapingIndPtr;
    bParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        bParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    bParams.dropProfileInd = 0;
    bParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_MIN_SHP_E;

    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr = aProfileShapingIndPtr;
    aParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        aParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    aParams.dropProfileInd = 0;
    aParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_SHP_E;

    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS; /* in CPSS_TM_NODE_QUANTUM_UNIT_CNS */

    /*****************************************************/
    /* Create Queue Drop profile                          */
    /*****************************************************/
    cos = CPSS_TM_INVAL_CNS;

    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(500);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId2);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(1000);

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_Q_E, cos, &profile, &queueDropProfileId1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/
    portsNum = 10;
    cNodesNum = 4; /* per port */
    bNodesNum = 5; /* per c node */
    aNodesNum = 10; /* per b node */
    queuesNum = 8; /* per a node */

    rc = cpssTmTreeParamsGet(dev, &tmTreeParams);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmTreeParamsGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    tmPortsArrSize = portsNum;
    rc = gtAppDemoTmGetTmPortsIds(dev, &tmPortsArrSize, tmPortsArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portsNum = MIN(portsNum, tmPortsArrSize);

    /* verify maxQueues */
    if (portsNum * cNodesNum * bNodesNum * aNodesNum * queuesNum > tmTreeParams.maxQueues)
    {
        cNodesNum = 2; /* per port */
        bNodesNum = 4; /* per c node */
        aNodesNum = 5; /* per b node */
        queuesNum = 8; /* per a node */
    }

    for (i = 0; i < portsNum; i++) 
    {
        tmPort = tmPortsArr[i];

        /* queue 0 is excluded from the pool */
        totalQueues += MIN(cNodesNum*bNodesNum*aNodesNum*queuesNum, tmTreeParams.maxQueues-1);

        if (totalQueues > tmTreeParams.maxQueues-1)
        {
            cpssOsPrintf("total queues: %d is exceeded max queues after %d tm ports\n", totalQueues, i);
            break;
        }

        /*create port*/
        rc = cpssTmPortCreate(dev, 
                              tmPort, 
                              CPSS_TM_PORT_BW_1G_E, 
                              &portParams, 
                              cNodesNum,
                              cNodesNum*bNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum*queuesNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (c = 0; c < cNodesNum; c++) 
        {
            /*create c node*/
            rc = cpssTmCnodeToPortCreate(dev, tmPort, &cParams, &cNodeInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            for (b = 0; b < bNodesNum; b++) 
            {
                /*create b node*/
                rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                for (a = 0; a < aNodesNum; a++) 
                {
                    /*create a node*/
                    rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    for (j = 0; j < queuesNum; j++) 
                    {
                        switch(j) 
                        {
                        case 0:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;
                            break;
                        case 1:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO1_E;
                            break;
                        case 2:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO2_E;
                            break;
                        case 3:
                            queueParams.dropProfileInd = queueDropProfileId1;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO3_E;
                            break;
                        case 4:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO4_E;
                            break;
                        case 5:
                            queueParams.dropProfileInd = queueDropProfileId1;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO5_E;
                            break;
                        case 6:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO6_E;
                            break;
                        case 7:
                            queueParams.dropProfileInd = queueDropProfileId2;
                            queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO7_E;
                            break;
                        default:
                            return GT_FAIL;
                        }

                        /*create queue node */
                        rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
                        queue_id_pool[queue_id_pool_size++] = queueId;
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
                        if (rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
            }
        }
    }


    /* configure tree for ports 128-191 */
    /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/

    portParams.cirBw = CPSS_TM_SHAPING_INFINITE_BW_CNS;
    portParams.eirBw = 0;
    portParams.cbs = 0; /*KBytes*/
    portParams.ebs = 0;

    /**************************************************************************/
    /* validate (and update if necessary)  cbs & ebs values                   */
    /**************************************************************************/
    rc = cpssTmPortShapingValidate(dev, 
                                   CPSS_TM_PORT_BW_10G_E, 
                                   portParams.cirBw, 
                                   portParams.eirBw , 
                                   &(portParams.cbs), 
                                   &(portParams.ebs));

    if ((rc != GT_OK) && (rc != GT_BAD_SIZE))
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortShapingValidate", rc);
        return rc;
    }

    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_FP0_E;
    /*****************************************************/
    /* Configure C parameters                        */
    /*****************************************************/
    cParams.shapingProfilePtr= CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    cParams.quantum =CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap =1;
    cParams.eligiblePrioFuncId =CPSS_TM_ELIG_N_FP0_E;
    for (ii = 0; ii < 8; ii++)
    {
        cParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E;
        cParams.dropProfileIndArr[ii]=CPSS_TM_NO_DROP_PROFILE_CNS;
    }
    /*****************************************************/
    /* Configure B parameters                        */
    /*****************************************************/
    bParams.shapingProfilePtr= CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    bParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii=0; ii<8; ii++)
        bParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    bParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;
    bParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_FP0_E;
    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr= CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    aParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (ii = 0; ii < 8; ii++)
        aParams.schdModeArr[ii] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    aParams.dropProfileInd =CPSS_TM_NO_DROP_PROFILE_CNS;
    aParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_FP0_E;
    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    queueParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;
    queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;

    /* create ports */
    for (portNo = 128 ; portNo < 192 ; portNo++)
    {
        rc = cpssTmPortCreate(dev, 
                              portNo , 
                              CPSS_TM_PORT_BW_40G_E, 
                              &portParams, 
                              1, 
                              1, 
                              1, 
                              4);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /*create c node*/
        rc = cpssTmCnodeToPortCreate(dev, portNo , &cParams, &cNodeInd);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        /*create b node*/
        rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        /*create a node*/
        rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (j = 0; j < 4; j++)
        {
            /*create queue node */
            rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
            queue_id_pool[queue_id_pool_size++] = queueId;
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    /*install all queues*/
    for (i = 0; i < queue_id_pool_size; i++) 
    {
        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return rc;
} /* appDemoTmScenario20Init */

/*******************************************************************************
* appDemoTmTxFifoScenarioInit
*
* DESCRIPTION:
*      Implements 8 ports with 10G BW. each
*       8 ports, 512 C nodes, 2K B nodes, 4K A nodes, 16K queues
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
GT_STATUS appDemoTmTxFifoScenarioInit
(
    IN  GT_U8   dev
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_TM_LEVEL_PERIODIC_PARAMS_STC   params[5];
    CPSS_TM_PORT_PARAMS_STC             portParams;
    CPSS_TM_QUEUE_PARAMS_STC            queueParams;
    CPSS_TM_DROP_PROFILE_PARAMS_STC     profile;
    GT_U32                              portDropProfileId;
    GT_U32                              queueId=0;
    GT_U32                              cNodeInd;
    GT_U32                              bNodeInd;
    GT_U32                              aNodeInd;
    CPSS_TM_C_NODE_PARAMS_STC           cParams;
    CPSS_TM_B_NODE_PARAMS_STC           bParams;
    CPSS_TM_A_NODE_PARAMS_STC           aParams;
    GT_U32                              i;
    GT_U32                              c;
    GT_U32                              b;
    GT_U32                              a;
    GT_U32                              q;
    GT_U32                              queuesNum;
    GT_U32                              aNodesNum;
    GT_U32                              bNodesNum;
    GT_U32                              cNodesNum;
    GT_U32                              portNo;   

    cpssOsPrintf("\n ***CPSS_TM_TX_FIFO***  12Px10G , 10G CB drop global/port   1000 Q /port symmetrically ...\n ");

    queue_id_pool_size = 0;

    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Configure Periodic Scheme                         */
    /*****************************************************/
    /* no shaping on all levels*/
    for (i = CPSS_TM_LEVEL_Q_E; i <= CPSS_TM_LEVEL_P_E; i++) 
    {
        params[i].periodicState = GT_FALSE;
        params[i].shaperDecoupling = GT_FALSE;
    }
    rc = cpssTmSchedPeriodicSchemeConfig(dev, params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmSchedPeriodicSchemeConfig", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*****************************************************/
    /* Create Port Drop profile                          */
    /*****************************************************/
    profile.dropMode = CPSS_TM_DROP_MODE_CB_TD_ONLY_E;
    profile.cbTdThresholdBytes = cpssTmDropConvertBWToBytes(10000000); /* 10Gbps */

    rc = cpssTmDropProfileCreate(dev, CPSS_TM_LEVEL_P_E, CPSS_TM_INVAL_CNS, &profile, &portDropProfileId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmDropProfileCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

     /*****************************************************/
    /* Configure Port parameters                         */
    /*****************************************************/


    portParams.cirBw = CPSS_TM_INVAL_CNS;
    portParams.eirBw = CPSS_TM_INVAL_CNS;
    portParams.cbs = CPSS_TM_INVAL_CNS;
    portParams.ebs = CPSS_TM_INVAL_CNS;
    for (i = 0; i < 8; i++) 
    {
        portParams.quantumArr[i] = CPSS_TM_PORT_QUANTUM_UNIT_CNS;
        portParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
    }
    portParams.dropProfileInd = portDropProfileId;
    portParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_PP_E;

    /*****************************************************/
    /* Configure C parameters                        */
    /*****************************************************/
    cParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    cParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    cParams.dropCosMap = 0;
    cParams.eligiblePrioFuncId =CPSS_TM_ELIG_N_PP_E;
    for (i = 0; i < 8; i++)
    {
        cParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E;
        cParams.dropProfileIndArr[i] = 0;
    }

    /*****************************************************/
    /* Configure B parameters                        */
    /*****************************************************/
    bParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    bParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (i = 0; i < 8; i++)
        bParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    bParams.dropProfileInd =CPSS_TM_NO_DROP_PROFILE_CNS;
    bParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_PP_E;

    /*****************************************************/
    /* Configure A parameters                        */
    /*****************************************************/
    aParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    aParams.quantum= CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    for (i = 0; i < 8; i++)
        aParams.schdModeArr[i] = CPSS_TM_SCHD_MODE_RR_E; /* No DWRR configured*/
    aParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;
    aParams.eligiblePrioFuncId = CPSS_TM_ELIG_N_PP_E;

    /*****************************************************/
    /* Configure Queue parameters                        */
    /*****************************************************/
    queueParams.shapingProfilePtr = CPSS_TM_SHAPING_INFINITE_PROFILE_INDEX_CNS;
    queueParams.quantum = CPSS_TM_NODE_QUANTUM_UNIT_CNS;
    queueParams.dropProfileInd = CPSS_TM_NO_DROP_PROFILE_CNS;
    queueParams.eligiblePrioFuncId = CPSS_TM_ELIG_Q_PRIO0_E;

    /*****************************************************/
    /* Create Tree                                       */
    /*****************************************************/
    cNodesNum=5;
    bNodesNum=5;
    aNodesNum=5;
    queuesNum=8;

    portParams.numOfChildren = cNodesNum;
    cParams.numOfChildren = bNodesNum;
    bParams.numOfChildren = aNodesNum;
    aParams.numOfChildren = queuesNum;

    for (portNo = 56; portNo <= 67; portNo++) 
    {
        /*create port*/
        rc = cpssTmPortCreate(dev, 
                              portNo, 
                              CPSS_TM_PORT_BW_10G_E, 
                              &portParams, 
                              cNodesNum,
                              cNodesNum*bNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum, 
                              cNodesNum*bNodesNum*aNodesNum*queuesNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmPortCreate", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (c = 0; c < portParams.numOfChildren; c++) 
        {
            /*create c node*/
            rc = cpssTmCnodeToPortCreate(dev, portNo, &cParams, &cNodeInd);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmCnodeToPortCreate", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            for (b = 0; b < cParams.numOfChildren; b++) 
            {
                /*create b node*/
                rc = cpssTmBnodeToCnodeCreate(dev, cNodeInd, &bParams, &bNodeInd);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmBnodeToCnodeCreate", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                for (a = 0; a < bParams.numOfChildren; a++) 
                {
                    /*create a node*/
                    rc = cpssTmAnodeToBnodeCreate(dev, bNodeInd, &aParams, &aNodeInd);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmAnodeToBnodeCreate", rc);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    for (q = 0; q < aParams.numOfChildren; q++) 
                    {
                        /*create queue node */
                        /* temporary  queue 0 is excluded from the pool - patch for H/W bug*/
                        /* here workaround for this - only in order to keep test running*/
                        if (queue_id_pool_size < QUEUE_POOL_SIZE_CNS)
                        {
                            rc = cpssTmQueueToAnodeCreate(dev, aNodeInd, &queueParams, &queueId);
                            queue_id_pool[queue_id_pool_size++]=queueId;
                            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmQueueToAnodeCreate", rc);
                            if (rc != GT_OK)
                            {
                                return rc;
                            }
                        }
                        /* */
                    }
                }
            }
        }
    }
    /**/
    /*install all queues*/
    for (i = 0; i < queue_id_pool_size; i++)
    {
        rc = cpssTmNodesCtlQueueInstall(dev, queue_id_pool[i]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmNodesCtlQueueInstall", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }


    return rc;
} /* appDemoTmTxFifoScenarioInit */


GT_STATUS appDemoTmTxFifo
(
    IN  GT_U8   dev
)
{
    GT_STATUS   rc;         /* return code */
    rc = appDemoTmTxFifoScenarioInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoTmTxFifo", rc);
    if (rc == GT_OK)
    {
        cpssOsPrintf("\n appDemoTmTxFifo dev=%d : successful  \n",dev);
    }
    else
    {
        cpssOsPrintf("\n appDemoTmTxFifo dev=%d : failed  rc=%d \n",dev,rc);
    }
    return rc;
}

/*******************************************************************************
* appDemoTmLibInitOnly
*
* DESCRIPTION:
*      Implements only cpssTmInit() - default init configuration for TM*
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
GT_STATUS appDemoTmLibInitOnly
(
    IN  GT_U8   dev
)
{
    GT_STATUS           rc = GT_OK;

    cpssOsPrintf("\n ***CPSS_TM_LIB_INIT  -  Default TM configuration ***\n ");
    rc = cpssTmInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTmInit", rc);
    return rc;
} /* appDemoTmLibInitOnly */
