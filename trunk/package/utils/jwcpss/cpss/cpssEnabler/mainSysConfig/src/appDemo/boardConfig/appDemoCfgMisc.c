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
* appDemoCfgMisc.c
*
* DESCRIPTION:
*       Includes misc functions implementations to be used by the application
*       demo configuration functions.
*
* FILE REVISION NUMBER:
*       $Revision: 26 $
*
*******************************************************************************/

#ifdef IMPL_FA
#include <appDemo/sysHwConfig/gtAppDemoFaConfig.h>
#endif

#include <appDemo/sysHwConfig/gtAppDemoSmiConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

#if (defined EX_FAMILY) || (defined MX_FAMILY)
    #include <cpss/exMx/exMxGen/cpssHwInit/cpssExMxHwInit.h>
    #include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
    #include <cpss/exMx/exMxGen/phy/cpssExMxPhySmi.h>
#endif

#if defined EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/cpssHwInit/cpssExMxPmHwInit.h>
    #include <cpss/exMxPm/exMxPmGen/phy/cpssExMxPmPhySmi.h>
#endif /*EXMXPM_FAMILY*/

#if defined CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
    #include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#endif/*CHX_FAMILY*/

#if (defined SAL_FAMILY)
    #include <cpss/dxSal/phy/cpssSalPhySmi.h>
#endif/*SAL_FAMILY*/


/*******************************************************************************
 * Internal variables
 ******************************************************************************/

/* This variable will be set to GT_TRUE  by function appDemoRxBuffsCacheableSet in case of cacheable RX buffers */
static GT_BOOL rxBuffersInCachedMem = GT_FALSE;

static GT_STATUS coreGetRxDescSize
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSize
);

GT_STATUS appDemoGetTxDescSize
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSize
);

static GT_STATUS coreGetAuDescSize
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSize
);


/*PP Registers */
static CPSS_REG_VALUE_INFO_STC   twistCRegValInfoList[]      = GT_TWIST_C_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   twistDRegValInfoList[]      = GT_TWIST_D_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   twistCB2BRegValInfoList[]   = GT_TWIST_C_B2B_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   twistDB2BRegValInfoList[]   = GT_TWIST_D_B2B_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   twistDXGRegValInfoList[]    = GT_TWIST_D_XG_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   sambaRegValInfoList[]       = GT_SAMBA_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   samba6x852RegValInfoList[]  = GT_SAMBA_6X8_52_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   samba6x812RegValInfoList[]  = GT_SAMBA_6X8_12_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   pumaEX240RegValInfoList[]   = GT_PUMA_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   dummyRegValInfoList[]       = GT_DUMMY_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   sambaXGRegValInfoList[]     = GT_SAMBA_XG_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   samba6x8XGRegValInfoList[]  = GT_SAMBA_6X8_XG_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   phoenixRegValInfoList[]     = GT_PHOENIX_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   tiger12RegValInfoList[]     = GT_TIGER_12_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   tiger52RegValInfoList[]     = GT_TIGER_52_REG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   tigerXGRegValInfoList[]     = GT_TIGER_XG_REG_VAL_INFO_LIST;

/* FA Registers*/
#ifdef IMPL_FA
static CPSS_REG_VALUE_INFO_STC   foxRegValXgInfoList[]      = GT_FOX_REG_XG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   foxRegValTigerXgInfoList[] = GT_FOX_REG_TIGER_XG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   foxRegValSambaXGInfoList[] = GT_FOX_REG_SAMBA_XG_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   foxRegValTiger12InfoList[] = GT_FOX_REG_TIGER_12_VAL_INFO_LIST;
static CPSS_REG_VALUE_INFO_STC   foxRegValTiger52InfoList[] = GT_FOX_REG_TIGER_52_VAL_INFO_LIST;
/* Should be used for FPGA board, currently Tiger's being used
static CPSS_REG_VALUE_INFO_STC   dummyFaRegValInfoList[]    = GT_DUMMY_FA_REG_VAL_INFO_LIST;*/

#endif

/*******************************************************************************
* appDemoRxBuffsCacheableSet
*
* DESCRIPTION:
*       This function set status of RX buffers - cacheable or not.
*       Should be called one time BEFORE cpssInitSystem
*
* INPUTS:
*       buffersCacheable - Should be GT_TRUE if RX buffers are in cacheable memory.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - always,
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoRxBuffsCacheableSet
(
    IN GT_BOOL buffersCacheable
)
{
    rxBuffersInCachedMem = buffersCacheable;

    return GT_OK;
}

/*******************************************************************************
* appDemoRawSocketModeSet
*
* DESCRIPTION:
*       Instruct the system to use raw packets in MII mode.
*
* INPUTS:
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - always,
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoRawSocketModeSet
(
  IN GT_VOID
)
{
  cpssExtDrvEthRawSocketModeSet(GT_TRUE);
  return GT_OK;
}

/*******************************************************************************
* appDemoLinuxModeSet
*
* DESCRIPTION:
*       Set port <portNum> to Linux Mode (applicable for Linux Only)
*
* INPUTS:
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
*       ip1, ip2, ip3, ip4 - The ip address to assign to the port, 4 numbers
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - always,
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS appDemoLinuxModeSet
(
 IN GT_U32  portNum,
 IN GT_U32 ip1,
 IN GT_U32 ip2,
 IN GT_U32 ip3,
 IN GT_U32 ip4
)
{
  return cpssExtDrvLinuxModeSet(portNum, ip1, ip2, ip3, ip4);
}

/*******************************************************************************
* appDemoLinuxModeGet
*
* DESCRIPTION:
*       Get port <portNum> Linux Mode indication (Linux Only)
*
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
* OUTPUTS:
*
* RETURNS:
*       GT_TRUE if Linux mode, or
*       GT_FALSE otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_BOOL appDemoLinuxModeGet
(
 IN GT_U32  portNum
)
{
  return cpssExtDrvLinuxModeGet(portNum);
}

/*******************************************************************************
* appDemoAllocateDmaMem
*
* DESCRIPTION:
*       This function allocates memory for phase2 initialization stage, the
*       allocations include: Rx Descriptors / Buffer, Tx descriptors, Address
*       update descriptors.
*
* INPUTS:
*       devType         - The Pp device type to allocate the memory for.
*       rxDescNum       - Number of Rx descriptors (and buffers) to allocate.
*       rxBufSize       - Size of each Rx Buffer to allocate.
*       rxBufAllign     - Ammount of allignment required on the Rx buffers.
*       txDescNum       - Number of Tx descriptors to allocate.
*       auDescNum       - Number of address update descriptors to allocate.
*       ppPhase2Params  - The device's Phase2 parameters.
*
* OUTPUTS:
*       ppPhase2Params  - The device's Phase2 parameters including the required
*                         allocations.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoAllocateDmaMem
(
    IN      CPSS_PP_DEVICE_TYPE         devType,
    IN      GT_U32                      rxDescNum,
    IN      GT_U32                      rxBufSize,
    IN      GT_U32                      rxBufAllign,
    IN      GT_U32                      txDescNum,
    IN      GT_U32                      auDescNum,
    INOUT   CPSS_PP_PHASE2_INIT_PARAMS  *ppPhase2Params
)
{
#ifdef IMPL_PP

    GT_U32                      tmpData;
    GT_U32                      *tmpPtr;
    GT_U32                      fuDescNum; /* number of FU descriptors ... allow to be diff then auDescNum */
    GT_CHAR                     txGenQueueParamStr[20];
    GT_BOOL                     txGenQueue[8];      /* Enable Tx queue to work in generator mode */
    GT_U32                      txGenNumOfDescBuff; /* Number of descriptors and buffers per Tx queue */
    GT_U32                      txGenBuffSize;      /* Size of buffer per Tx queue */
    GT_U32                      txGenDescSize;      /* Size of descriptor per Tx queue */
    GT_U32                      txQue;              /* Tx queue number */

    if(appDemoDbEntryGet("auDescNum", &tmpData) == GT_OK)
    {
        /* allow to update the default of the auDescNum */
        auDescNum = tmpData;
    }

    /* default and possible override */
    fuDescNum = auDescNum;
    if(appDemoDbEntryGet("fuDescNum", &tmpData) == GT_OK)
    {
        /* allow to update the default of the auDescNum */
        fuDescNum = tmpData;
    }

    if(appDemoDbEntryGet("fuqUseSeparate", &tmpData) == GT_OK)
    {
        ppPhase2Params->fuqUseSeparate = (GT_BOOL)tmpData;
    }

    if(appDemoDbEntryGet("rxDescNum", &tmpData) == GT_OK)
    {
        /* allow to update the default of the rxDescNum */
        rxDescNum = tmpData;
    }

    if(appDemoDbEntryGet("txDescNum", &tmpData) == GT_OK)
    {
        /* allow to update the default of the txDescNum */
        txDescNum = tmpData;
    }

    if(appDemoDbEntryGet("useDoubleAuq", &tmpData) == GT_OK)
    {
        /* allow to work with double AUQ */
        ppPhase2Params->useDoubleAuq = tmpData;
    }

    if(appDemoDbEntryGet("useSecondaryAuq", &tmpData) == GT_OK)
    {
        /* allow to work with secondary AUQ */
        ppPhase2Params->useSecondaryAuq = tmpData;
    }

    if(appDemoDbEntryGet("useMultiNetIfSdma", &tmpData) == GT_OK)
    {
        /* allow to work in Tx queue generator mode */
        ppPhase2Params->useMultiNetIfSdma = tmpData;
    }
    for(txQue = 0; txQue < 8; txQue++)
    {
        txGenQueue[txQue] = GT_FALSE;
        osSprintf(txGenQueueParamStr, "txGenQueue_%d", txQue);
        if(appDemoDbEntryGet(txGenQueueParamStr, &tmpData) == GT_OK)
        {
            /* Enable Tx queue to work in Tx queue generator mode */
            txGenQueue[txQue] = GT_TRUE;
        }
    }

    if(appDemoDbEntryGet("txGenNumOfDescBuff", &tmpData) == GT_OK)
    {
        /* Number of descriptors and buffers */
        txGenNumOfDescBuff = tmpData;
    }
    else
    {
        /* Default number of descriptors and buffers */
        txGenNumOfDescBuff = 512;
    }

    if(appDemoDbEntryGet("txGenBuffSize", &tmpData) == GT_OK)
    {
        /* Number of descriptors and buffers */
        txGenBuffSize = tmpData;
    }
    else
    {
        /* Default number of descriptors and buffers */
        txGenBuffSize = 128 + 16; /* 16 bytes of eDsa tag */
    }

#if defined EXMXPM_FAMILY
    /* we need the extra number of messages for the secondary AUQ */
    auDescNum += CPSS_EXMXPM_PP_FDB_AUQ_DEADLOCK_EXTRA_MESSAGES_NUM_WA_CNS;
#endif /*EXMXPM_FAMILY*/

    /* Au block size calc & malloc  */
    coreGetAuDescSize(devType,&tmpData);

    if (auDescNum == 0)
    {
        ppPhase2Params->auqCfg.auDescBlock = 0;
        ppPhase2Params->auqCfg.auDescBlockSize = 0;
    }
    else
    {
        ppPhase2Params->auqCfg.auDescBlockSize = tmpData * auDescNum;
        ppPhase2Params->auqCfg.auDescBlock =
            osCacheDmaMalloc(ppPhase2Params->auqCfg.auDescBlockSize +
                            tmpData);/*allocate space for one message more for alignment purposes
                                      NOTE: we not add it to the size , only to the buffer ! */
        if(ppPhase2Params->auqCfg.auDescBlock == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }
        if(((GT_UINTPTR)ppPhase2Params->auqCfg.auDescBlock) % tmpData)
        {
            /* add to the size the extra value for alignment , to the actual size
               will be as needed , after the reduction in the cpss code */
            ppPhase2Params->auqCfg.auDescBlockSize += tmpData;
        }
    }

    if (fuDescNum == 0)
    {
        ppPhase2Params->fuqCfg.auDescBlock = 0;
        ppPhase2Params->fuqCfg.auDescBlockSize = 0;
    }
    else
    {
        /* Fu block size calc & malloc  */
        coreGetAuDescSize(devType,&tmpData);
        ppPhase2Params->fuqCfg.auDescBlockSize = tmpData * fuDescNum;
        ppPhase2Params->fuqCfg.auDescBlock =
            osCacheDmaMalloc(ppPhase2Params->fuqCfg.auDescBlockSize +
                            tmpData);/*allocate space for one message more for alignment purposes
                                      NOTE: we not add it to the size , only to the buffer ! */
        if(ppPhase2Params->fuqCfg.auDescBlock == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        if(((GT_UINTPTR)ppPhase2Params->fuqCfg.auDescBlock) % tmpData)
        {
            /* add to the size the extra value for alignment , to the actual size
               will be as needed , after the reduction in the cpss code */
            ppPhase2Params->fuqCfg.auDescBlockSize += tmpData;
        }
    }

    /* Tx block size calc & malloc  */
    appDemoGetTxDescSize(devType,&tmpData);
    ppPhase2Params->netIfCfg.txDescBlockSize = tmpData * txDescNum;
    if (ppPhase2Params->netIfCfg.txDescBlockSize != 0)
    {
        ppPhase2Params->netIfCfg.txDescBlock =
            osCacheDmaMalloc(ppPhase2Params->netIfCfg.txDescBlockSize);
        if(ppPhase2Params->netIfCfg.txDescBlock == NULL)
        {
            osCacheDmaFree(ppPhase2Params->auqCfg.auDescBlock);
            return GT_OUT_OF_CPU_MEM;
        }
    }
    if((systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E)||
       ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
          (systemRecoveryInfo.systemRecoveryMode.continuousTx == GT_FALSE)) )
    {
        osMemSet(ppPhase2Params->netIfCfg.txDescBlock,0,
                 ppPhase2Params->netIfCfg.txDescBlockSize);
    }

    /* Rx block size calc & malloc  */
    coreGetRxDescSize(devType,&tmpData);
    ppPhase2Params->netIfCfg.rxDescBlockSize = tmpData * rxDescNum;
    if (ppPhase2Params->netIfCfg.rxDescBlockSize != 0)
    {
        ppPhase2Params->netIfCfg.rxDescBlock =
            osCacheDmaMalloc(ppPhase2Params->netIfCfg.rxDescBlockSize);
        if(ppPhase2Params->netIfCfg.rxDescBlock == NULL)
        {
            osCacheDmaFree(ppPhase2Params->auqCfg.auDescBlock);
            osCacheDmaFree(ppPhase2Params->netIfCfg.txDescBlock);
            return GT_OUT_OF_CPU_MEM;
        }
    }
    if((systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E)||
       ( (systemRecoveryInfo.systemRecoveryState == CPSS_SYSTEM_RECOVERY_INIT_STATE_E) &&
          (systemRecoveryInfo.systemRecoveryMode.continuousRx == GT_FALSE)) )
    {
        osMemSet(ppPhase2Params->netIfCfg.rxDescBlock,0,
                 ppPhase2Params->netIfCfg.rxDescBlockSize);
    }

    /* init the Rx buffer allocation method */
    /* Set the system's Rx buffer size.     */
    if((rxBufSize % rxBufAllign) != 0)
    {
        rxBufSize = (rxBufSize + (rxBufAllign - (rxBufSize % rxBufAllign)));
    }

    if (ppPhase2Params->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E)
    {
        ppPhase2Params->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockSize =
        rxBufSize * rxDescNum;

        /* set status of RX buffers - cacheable or not */
        ppPhase2Params->netIfCfg.rxBufInfo.buffersInCachedMem = rxBuffersInCachedMem;

        if ((rxBufSize * rxDescNum) != 0)
        {
            /* If RX buffers should be cachable - allocate it from regular memory */
            if (GT_TRUE == rxBuffersInCachedMem)
            {
                tmpPtr = osMalloc(((rxBufSize * rxDescNum) + rxBufAllign));
            }
            else
            {
                tmpPtr = osCacheDmaMalloc(((rxBufSize * rxDescNum) + rxBufAllign));
            }

            if(tmpPtr == NULL)
            {
                osCacheDmaFree(ppPhase2Params->auqCfg.auDescBlock);
                osCacheDmaFree(ppPhase2Params->netIfCfg.txDescBlock);
                osCacheDmaFree(ppPhase2Params->netIfCfg.rxDescBlock);
                return GT_OUT_OF_CPU_MEM;
            }
        }
        else
        {
            tmpPtr = NULL;
        }

        if((((GT_UINTPTR)tmpPtr) % rxBufAllign) != 0)
        {
            tmpPtr = (GT_U32*)(((GT_UINTPTR)tmpPtr) +
                               (rxBufAllign - (((GT_UINTPTR)tmpPtr) % rxBufAllign)));
        }
        ppPhase2Params->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockPtr = tmpPtr;
    }
    else if (ppPhase2Params->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_NO_ALLOC_E)
    {
        /* do not allocate rx buffers*/
    }
    else
    {
        /* dynamic RX buffer allocation currently is not supported by appDemo*/
        osCacheDmaFree(ppPhase2Params->auqCfg.auDescBlock);
        osCacheDmaFree(ppPhase2Params->netIfCfg.txDescBlock);
        osCacheDmaFree(ppPhase2Params->netIfCfg.rxDescBlock);
        return GT_NOT_SUPPORTED;
    }

    if(ppPhase2Params->useMultiNetIfSdma)
    {
        CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC  * sdmaQueuesConfigPtr;

        /* Tx block size calc & malloc  */
        appDemoGetTxDescSize(devType,&txGenDescSize);

        for(txQue = 0; txQue < 8; txQue++)
        {
            if(txGenQueue[txQue] == GT_FALSE)
            {
                /* Regular mode is not supported yet */
                continue;
            }

            /* Generator mode */
            sdmaQueuesConfigPtr =
                &ppPhase2Params->multiNetIfCfg.txSdmaQueuesConfig[0][txQue];
            sdmaQueuesConfigPtr->buffAndDescAllocMethod = CPSS_TX_BUFF_STATIC_ALLOC_E;
            sdmaQueuesConfigPtr->buffersInCachedMem = GT_FALSE;
            sdmaQueuesConfigPtr->numOfTxDesc = txGenNumOfDescBuff / 2;

            sdmaQueuesConfigPtr->queueMode =
                CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E;
            sdmaQueuesConfigPtr->numOfTxBuff = sdmaQueuesConfigPtr->numOfTxDesc;

            sdmaQueuesConfigPtr->memData.staticAlloc.buffAndDescMemSize =
                (sdmaQueuesConfigPtr->numOfTxDesc + 1) * (txGenDescSize + txGenBuffSize);
            sdmaQueuesConfigPtr->memData.staticAlloc.buffAndDescMemPtr =
                osCacheDmaMalloc(sdmaQueuesConfigPtr->memData.staticAlloc.buffAndDescMemSize);
            sdmaQueuesConfigPtr->buffSize = txGenBuffSize;
        }
    }

#endif

    return GT_OK;
}

/*******************************************************************************
* appDemoGetPpRegCfgList
*
* DESCRIPTION:
*       This function returns the list of registers to be configured to a given
*       device before and after the startInit operation. (Passed to
*       corePpHwStartInit() ).
*
* INPUTS:
*       devNum      - The Pp device type.
*       isB2bSystem - CPSS_TRUE, it's a B2B system, CPSS_FALSE otherwise.
*
* OUTPUTS:
*       regCfgList      - A pointer to the register list.
*       regCfgListSize  - Number of elements in regListPtr.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoGetPpRegCfgList
(
    IN  CPSS_PP_DEVICE_TYPE         devType,
    IN  GT_BOOL                     isB2bSystem,
    OUT CPSS_REG_VALUE_INFO_STC     **regCfgList,
    OUT GT_U32                      *regCfgListSize
)
{
#ifdef IMPL_PP
    switch(devType)
    {
        /* Twist devices    */
        case(CPSS_98MX610B_CNS):
        case(CPSS_98MX610BS_CNS):
        case(CPSS_98EX110BS_CNS):
        case(CPSS_98EX111BS_CNS):
        case(CPSS_98EX112BS_CNS):
        case(CPSS_98EX110B_CNS):
        case(CPSS_98EX111B_CNS):
        case(CPSS_98EX112B_CNS):
        case(CPSS_98MX620B_CNS):
        case(CPSS_98EX120B_CNS):
        case(CPSS_98EX120B__CNS):
        case(CPSS_98EX121B_CNS):
        case(CPSS_98EX122B_CNS):
        case(CPSS_98EX128B_CNS):
        case(CPSS_98EX129B_CNS):
            if(isB2bSystem == GT_FALSE)
            {
                *regCfgList     = twistCRegValInfoList;
                *regCfgListSize = (sizeof(twistCRegValInfoList) /
                                   sizeof(CPSS_REG_VALUE_INFO_STC));
            }
            else
            {
                *regCfgList     = twistCB2BRegValInfoList;
                *regCfgListSize = (sizeof(twistCB2BRegValInfoList) /
                                   sizeof(CPSS_REG_VALUE_INFO_STC));
            }
            break;
        /* Twist-D device */
        case(CPSS_98EX110D_CNS):
        case(CPSS_98EX110DS_CNS):
        case(CPSS_98EX120D_CNS):
        case(CPSS_98EX125D_CNS):
        case(CPSS_98EX115D_CNS):
        case(CPSS_98EX115DS_CNS):
            /* init regCfgList with dummy values. these values must be correct */
            if(isB2bSystem == GT_FALSE)
            {
                *regCfgList     = twistDRegValInfoList;
                *regCfgListSize = (sizeof(twistDRegValInfoList) /
                                   sizeof(CPSS_REG_VALUE_INFO_STC));
            }
            else
            {
                *regCfgList     = twistDB2BRegValInfoList;
                *regCfgListSize = (sizeof(twistDB2BRegValInfoList) /
                                   sizeof(CPSS_REG_VALUE_INFO_STC));
            }
            break;

        case(CPSS_98EX100D_CNS):
                *regCfgList     = phoenixRegValInfoList;
                *regCfgListSize = (sizeof(phoenixRegValInfoList) /
                                   sizeof(CPSS_REG_VALUE_INFO_STC));
                break;

        /* Twist-D-XG */
        case(CPSS_98EX130D_CNS):
        case(CPSS_98EX135D_CNS):
            *regCfgList     = twistDXGRegValInfoList;
            *regCfgListSize = (sizeof(twistDXGRegValInfoList) /
                               sizeof(CPSS_REG_VALUE_INFO_STC));
            break;

        /* Tiger 52 */
        case(CPSS_98EX106_CNS):
        case(CPSS_98EX108_CNS):
        case(CPSS_98EX116_CNS):
        case(CPSS_98EX116DI_CNS):
            *regCfgList     = tiger52RegValInfoList;
            *regCfgListSize = (sizeof(tiger52RegValInfoList) /
                               sizeof(CPSS_REG_VALUE_INFO_STC));
            break;
        /* Tiger 12 */
        case(CPSS_98EX126_CNS):
        case(CPSS_98EX126DI_CNS):
            *regCfgList     = tiger12RegValInfoList;
            *regCfgListSize = (sizeof(tiger12RegValInfoList) /
                               sizeof(CPSS_REG_VALUE_INFO_STC));
            break;

        /* Tiger XG */
        case(CPSS_98EX136_CNS):
        case(CPSS_98EX136DI_CNS):
            *regCfgList     = tigerXGRegValInfoList;
            *regCfgListSize = (sizeof(tigerXGRegValInfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;

        /* Samba */
        case(CPSS_98MX615_CNS):
        case(CPSS_98MX625_CNS):

            *regCfgList     = sambaRegValInfoList;
            *regCfgListSize = (sizeof(sambaRegValInfoList) /
                               sizeof(CPSS_REG_VALUE_INFO_STC));
            break;

        /* Samba 98MX6x8 */
        case(CPSS_98MX618_CNS):

            *regCfgList     = samba6x852RegValInfoList;
            *regCfgListSize = (sizeof(samba6x852RegValInfoList) /
                               sizeof(CPSS_REG_VALUE_INFO_STC));
            break;

        case(CPSS_98MX628_CNS):

            *regCfgList     = samba6x812RegValInfoList;
            *regCfgListSize = (sizeof(samba6x812RegValInfoList) /
                               sizeof(CPSS_REG_VALUE_INFO_STC));
        break;


        /* Samba XG */
        case(CPSS_98MX635_CNS):

            *regCfgList     = sambaXGRegValInfoList;
            *regCfgListSize = (sizeof(sambaXGRegValInfoList) /
                               sizeof(CPSS_REG_VALUE_INFO_STC));
        break;

        case(CPSS_98MX638_CNS):

            *regCfgList     = samba6x8XGRegValInfoList;
            *regCfgListSize = (sizeof(samba6x8XGRegValInfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;

        /* Puma */
        case(CPSS_98EX240_CNS):
        case(CPSS_98EX240_1_CNS):
        case(CPSS_98EX260_CNS):
        case (CPSS_98EX2210_CNS):
        case (CPSS_98EX8262_CNS):
        case (CPSS_98EX2110_CNS):
        case (CPSS_98MX2310_CNS) :
        case (CPSS_98EX8302_CNS):
        case (CPSS_98EX8305_CNS):
        case (CPSS_98EX8502_CNS):
        case (CPSS_98EX2106_CNS):
        case (CPSS_98EX2206_CNS):
        case (CPSS_98MX2306_CNS):
        case (CPSS_98EX8261_CNS):
        case (CPSS_98EX8301_CNS):
        case (CPSS_98EX8303_CNS):
        case (CPSS_98EX8501_CNS):
        case (CPSS_98EX8306_CNS):
        case (CPSS_98EX8307_CNS):

            *regCfgList     = pumaEX240RegValInfoList;
            *regCfgListSize = (sizeof(pumaEX240RegValInfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;

        /* dummy list for devices that not need it */
        default:
            *regCfgList     = dummyRegValInfoList;
            *regCfgListSize = (sizeof(dummyRegValInfoList) /
                               sizeof(CPSS_REG_VALUE_INFO_STC));
        break;
    }
#endif
    return GT_OK;
}


#ifdef IMPL_FA
/*******************************************************************************
* appDemoGetFaRegCfgList
*
* DESCRIPTION:
*       This function returns the list of registers to be configured to a given
*       FA device before and after the startInit operation. (Passed to
*       coreFaHwStartInit() ).
*
* INPUTS:
*       faDevType   - The Fa device type.
*       devType     - The PP device type. See comment.
*
* OUTPUTS:
*       regCfgList      - A pointer to the register list.
*       regCfgListSize  - Number of elements in regListPtr.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       The input of PP device type is needed as an indication to the BOARD
*       type. XG Boards frequency is different then other boards so FA has
*       a different configuration.
*
*******************************************************************************/
GT_STATUS appDemoGetFaRegCfgList
(
    IN  GT_FA_DEVICE                faDevType,
    IN  CPSS_PP_DEVICE_TYPE         devType,  /* See comment */
    OUT CPSS_REG_VALUE_INFO_STC     **regCfgList,
    OUT GT_U32                      *regCfgListSize
)
{
    switch (devType)
    {
        /* Twist devices    */
    case(CPSS_98MX610B_CNS):
    case(CPSS_98MX610BS_CNS):
    case(CPSS_98EX110BS_CNS):
    case(CPSS_98EX111BS_CNS):
    case(CPSS_98EX112BS_CNS):
    case(CPSS_98EX110B_CNS):
    case(CPSS_98EX111B_CNS):
    case(CPSS_98EX112B_CNS):
    case(CPSS_98MX620B_CNS):
    case(CPSS_98EX120B_CNS):
    case(CPSS_98EX120B__CNS):
    case(CPSS_98EX121B_CNS):
    case(CPSS_98EX122B_CNS):
    case(CPSS_98EX128B_CNS):
    case(CPSS_98EX129B_CNS):
        /* Twist-D device */
    case(CPSS_98EX110D_CNS):
    case(CPSS_98EX110DS_CNS):
    case(CPSS_98EX120D_CNS):
    case(CPSS_98EX125D_CNS):
    case(CPSS_98EX115D_CNS):
    case(CPSS_98EX115DS_CNS):
        /* Samba devices*/
    case(CPSS_98MX615_CNS):
    case(CPSS_98MX625_CNS):
    case(CPSS_98MX618_CNS):
    case(CPSS_98MX628_CNS):

        /* Tiger 12 */
    case(CPSS_98EX126_CNS):
    case(CPSS_98EX126DI_CNS):
        *regCfgList     = foxRegValTiger12InfoList;
        *regCfgListSize = (sizeof(foxRegValTiger12InfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;
        /* Tiger 52 */
    case(CPSS_98EX116_CNS):
    case(CPSS_98EX116DI_CNS):
        *regCfgList     = foxRegValTiger52InfoList;
        *regCfgListSize = (sizeof(foxRegValTiger52InfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;

        /* Twist-D-XG */
    case(CPSS_98EX130D_CNS):
    case(CPSS_98EX135D_CNS):
        *regCfgList     = foxRegValXgInfoList;
        *regCfgListSize = (sizeof(foxRegValXgInfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;

    case(CPSS_98EX136_CNS):
    case(CPSS_98EX136DI_CNS):
        *regCfgList     = foxRegValTigerXgInfoList;
        *regCfgListSize = (sizeof(foxRegValTigerXgInfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;

        /* Samba XG*/
    case(CPSS_98MX635_CNS):
    case(CPSS_98MX638_CNS):
        *regCfgList     = foxRegValSambaXGInfoList;
        *regCfgListSize = (sizeof(foxRegValSambaXGInfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
        break;

    case APP_DEMO_CPSS_NO_PP_CONNECTED_CNS:
        *regCfgList     = /*dummyFaRegValInfoList*/foxRegValTigerXgInfoList;
        *regCfgListSize = (sizeof(/*dummyFaRegValInfoList*/foxRegValTigerXgInfoList) /
                           sizeof(CPSS_REG_VALUE_INFO_STC));
break;


    default:
        return GT_BAD_PARAM;
    }

    return GT_OK;
}
#endif

/*******************************************************************************
* coreGetRxDescSize
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Rx descriptor,
*       for a given device.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSize- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS coreGetRxDescSize
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSize
)
{
#if (defined EX_FAMILY) || (defined MX_FAMILY)
    return cpssExMxHwRxDescSizeGet(devType, descSize);
#elif defined EXMXPM_FAMILY
    return cpssExMxPmHwRxDescSizeGet(devType, descSize);
#elif defined CHX_FAMILY
    return cpssDxChHwRxDescSizeGet(devType, descSize);
#else
    *descSize = 0;
    return GT_NOT_IMPLEMENTED;
#endif /* CHX_FAMILY */
}


/*******************************************************************************
* appDemoGetTxDescSize
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Tx descriptor,
*       for a given device.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSize- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern GT_STATUS appDemoGetTxDescSize
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSize
)
{
#if (defined EX_FAMILY) || (defined MX_FAMILY)
    return cpssExMxHwTxDescSizeGet(devType, descSize);
#elif defined EXMXPM_FAMILY
    return cpssExMxPmHwTxDescSizeGet(devType, descSize);
#elif defined CHX_FAMILY
    return cpssDxChHwTxDescSizeGet(devType, descSize);
#else
    *descSize = 0;
    return GT_NOT_IMPLEMENTED;
#endif /* CHX_FAMILY */
}

/*******************************************************************************
* coreGetAuDescSize
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Address Update
*       descriptor, for a given device.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSize- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS coreGetAuDescSize
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSize
)
{
#if (defined EX_FAMILY) || (defined MX_FAMILY)
    return cpssExMxHwAuDescSizeGet(devType, descSize);
#elif defined EXMXPM_FAMILY
    return cpssExMxPmHwAuDescSizeGet(devType, descSize);
#elif defined CHX_FAMILY
    return cpssDxChHwAuDescSizeGet(devType, descSize);
#else
    *descSize = 0;
    return GT_NOT_IMPLEMENTED;
#endif /* CHX_FAMILY */
}

/*******************************************************************************
* appDemoSetPortPhyAddr
*
* DESCRIPTION:
*       Configure the port's default phy address, this function should be used
*       to change the default port's phy address.
*
* INPUTS:
*       devNum  - The Pp device number.
*       portNum - The port number to update the phy address for.
*       phyAddr - The new phy address, only the lower 5 bits of this param are
*                 relevant.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       This function should be called after corePpHwPhase1Init(), and before
*       gtPortSmiInit() (in Tapi level).
*
*******************************************************************************/
GT_STATUS appDemoSetPortPhyAddr
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U8   phyAddr
)
{
#ifndef IMPL_PP
    return GT_OK;
#else /*IMPL_PP*/
    GT_STATUS rc = GT_NOT_IMPLEMENTED;

    /* check if device is active */
    if(appDemoPpConfigList[devNum].valid == GT_FALSE)
    {
        return GT_FAIL;
    }

    if(CPSS_IS_DXCH_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
    {
#if (defined CHX_FAMILY)
        rc = cpssDxChPhyPortAddrSet(devNum,portNum,phyAddr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortAddrSet", rc);
        return rc;
#endif /*(defined CHX_FAMILY)*/
    }
    else if(CPSS_IS_EXMX_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
    {
#if (defined EX_FAMILY) || (defined MX_FAMILY)
        return cpssExMxPhyPortAddrSet(devNum,portNum,phyAddr);
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY) */
    }
    else if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
    {
#if (defined EXMXPM_FAMILY)
        rc = cpssExMxPmPhyPortAddrSet(devNum,portNum,phyAddr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPortAddrSet", rc);
        return rc;
#endif /*(defined EXMXPM_FAMILY)*/
    }
    else if(CPSS_IS_DXSAL_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
    {
#if (defined SAL_FAMILY)
        return cpssDxSalPhyPortAddrSet(devNum,portNum,phyAddr);
#endif /*(defined SAL_FAMILY)*/
    }

    return rc;

#endif /*IMPL_PP*/
}

/*******************************************************************************
* appDemoGetRegisterWriteTime
*
* DESCRIPTION: Get the time needed for register write consecutive operations
*              as stated in the request.
*
* INPUTS:
*       devNum      - the device number to write to.
*       regAddr1    - first register address to write to.
*       regAddr2    - second register address to write to.
*       regAddr3    - third register address to write to.
*       regData     - the data to write to the register.
*       numOfWrites - the number of times to perform the register write operation.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoGetRegisterWriteTime
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr1,
    IN GT_U32 regAddr2,
    IN GT_U32 regAddr3,
    IN GT_U32 regData,
    IN GT_U32 numOfWrites
)
{
    GT_STATUS rc;
    GT_U32 writeTime;
    GT_U32 i;

    writeTime = osTime();
    for( i = 0 ; i< numOfWrites ; i++)
    {
        rc = cpssDrvPpHwRegisterWrite(devNum,
                                      CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      regAddr1,
                                      regData);
        if( GT_OK != rc )
            return rc;

        rc = cpssDrvPpHwRegisterWrite(devNum,
                                      CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      regAddr2,
                                      regData);
        if( GT_OK != rc )
            return rc;

        rc = cpssDrvPpHwRegisterWrite(devNum,
                                      CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      regAddr3,
                                      regData);
        if( GT_OK != rc )
            return rc;
    }
    writeTime = osTime() - writeTime;

    osPrintf("%d register write operations took %d Sec.\n", 3*numOfWrites, writeTime);

    return GT_OK;
}

/*******************************************************************************
* appDemoGetRegisterReadTime
*
* DESCRIPTION: Get the time needed for register read consecutive operations
*              as stated in the request.
*
* INPUTS:
*       devNum      - the device number to read from.
*       regAddr1    - first register address to read.
*       regAddr2    - second register address to read.
*       regAddr3    - third register address to read.
*       numOfReads  - the number of times to perform the register read operation.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoGetRegisterReadTime
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr1,
    IN GT_U32 regAddr2,
    IN GT_U32 regAddr3,
    IN GT_U32 numOfReads
)
{
    GT_STATUS rc;
    GT_U32 regData;
    GT_U32 readTime;
    GT_U32 i;

    readTime = osTime();
    for( i = 0 ; i< numOfReads ; i++)
    {
        rc = cpssDrvPpHwRegisterRead(devNum,
                                     CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                     regAddr1,
                                     &regData);
        if( GT_OK != rc )
            return rc;

        rc = cpssDrvPpHwRegisterRead(devNum,
                                     CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                     regAddr2,
                                     &regData);
        if( GT_OK != rc )
            return rc;

        rc = cpssDrvPpHwRegisterRead(devNum,
                                     CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                     regAddr3,
                                     &regData);
        if( GT_OK != rc )
            return rc;
    }
    readTime = osTime() - readTime;

    osPrintf("%d register read operations took %d Sec.\n", 3*numOfReads, readTime);

    return GT_OK;
}

