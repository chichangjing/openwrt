/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* appDemoGenEventHandle.c
*
* DESCRIPTION:
*       this library contains the implementation of the event handling functions
*       for the Gen device  , and the redirection to the GalTis Agent for
*       extra actions .
*
*
* FILE REVISION NUMBER:
*       $Revision: 26 $
******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <extUtils/auEventHandler/auFdbUpdateLock.h>
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <extUtils/auEventHandler/auEventHandler.h>

#if defined CHX_FAMILY
    #include <extUtils/cpssEnabler/appDemoFdbIpv6UcLinkedList.h>

    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfMii.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbHash.h>

    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChErrataMng.h>
    #include <cpss/extServices/private/prvCpssBindFunc.h>
#endif

#if defined EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>
#endif

/****************************************************************************
* Debug                                                                     *
****************************************************************************/
#ifdef ASIC_SIMULATION
static GT_U32 appDemoMaxRetryOnSleep0 = 128;
#endif /*ASIC_SIMULATION*/

/* number of cases that the movement of old entry to other place allowed the new
    entry to take it's place */
static GT_U32 numOfSuccessesInReorder = 0;
static GT_U32 numOfFailesInReorder = 0;
/* indication to trace the first fail of adding an entry into the FDB */
/* when this flag is set to 1 , the first time that the engine can't add entry
   the engine will check how much entries in the FDB. and print in % the capacity.
*/
static GT_U32   traceFirstMiss = 0;
static GT_BOOL  isFirstMissDone = 0;
/* indication that the Cuckoo algorithm succeeded to do it's first reorder of
   entries in order to insert NA that first failed to be inserted but after the
   re order succeeded */
static GT_BOOL  isFirstReorderDone = 0;

#define BUFF_LEN    5
#define AU_SIZE      10


#ifdef IMPL_GALTIS
extern GT_STATUS cmdGenRxPktReceive
(
    IN GT_U8      devNum,
    IN GT_U8      queueIdx,
    IN GT_U32     numOfBuff,
    IN GT_U8     *packetBuffs[],
    IN GT_U32     buffLen[],
    IN void      *rxParamsPtr
);
#endif /* IMPL_GALTIS */

#if (defined CHX_FAMILY)
    #define DXCH_CODE
#endif /* (defined CHX_FAMILY) */

#if (defined EXMXPM_FAMILY)
    #define PM_CODE
#endif /* (defined EXMXPM_FAMILY) */

#if (defined EX_FAMILY)
    #define EX_CODE
#endif /* (defined EX_FAMILY) */



/* trace info for AUQ messages */
/* trace flag */
static GT_U32   traceAuq = 0;
/* trace indications */
static char* traceAuqArr[CPSS_NA_MOVED_E + 1] = {
    "+",/*CPSS_NA_E*/
    "qa",/*CPSS_QA_E*//* should not be */
    "qr",/*CPSS_QR_E*/
    "-",/*CPSS_AA_E*/
    "t",/*CPSS_TA_E*/
    "sa",/*CPSS_SA_E*//* should not be */
    "qi",/*CPSS_QI_E*//* should not be */
    "f",/*CPSS_FU_E*/
    "hr",/*CPSS_HR_E*//* should not be */
    "M"/*MOVED*/ /*CPSS_NA_MOVED_E*/ /* not getting from the AUQ but used as 'Cuckoo algorithm success'*/
};

/* The number of AU messages per type*/
GT_U32 appDemoAuMessageNum[CPSS_NA_MOVED_E + 1] = {0};

/* The number of wrong type messages*/
GT_U32 appDemoAuWrongMessageNum = 0;

#if defined CHX_FAMILY
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>

static  CPSS_DXCH_NET_RX_PARAMS_STC     chRxParams;

#ifdef IMPL_GALTIS
GT_STATUS cpssDxChBrgFdbPortGroupCount_debug
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId
);
#endif /* IMPL_GALTIS */

#define PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS 0xFFFFFFFF

/*******************************************************************************
* appDemoGenNetRxPktHandle
*
* DESCRIPTION:
*       application routine to receive frames . -- Gen function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
static GT_STATUS appDemoNetRxPktHandleChPCI
(
    IN    GT_U8      devNum,
    IN    GT_U8      queueIdx,
    INOUT GT_U32    *numOfBuffPtr,
    OUT   GT_U8     *packetBuffs[],
    OUT   GT_U32     buffLen[],
    OUT   void     **rxParams
)
{
    GT_STATUS                       rc;


    /* get the packet from the device */
    rc = cpssDxChNetIfSdmaRxPacketGet(devNum, queueIdx, numOfBuffPtr,
                        packetBuffs, buffLen, &chRxParams);

    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
    {
        /* no need to free buffers because not got any */
        return rc;
    }
    *rxParams = &chRxParams;


    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdGenRxPktReceive(devNum, queueIdx, *numOfBuffPtr,
                              packetBuffs, buffLen, *rxParams);
#endif /* IMPL_GALTIS */


    /* now you need to free the buffers */
    rc = cpssDxChNetIfRxBufFree(devNum, queueIdx, packetBuffs, *numOfBuffPtr);

    return rc;
}
#endif

#if defined EXMXPM_FAMILY
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>

static  CPSS_EXMXPM_NET_RX_PARAMS_STC     pmRxParams;

/*******************************************************************************
* appDemoNetRxPktHandlePmPCI
*
* DESCRIPTION:
*       application routine to receive frames . -- Gen function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
static GT_STATUS appDemoNetRxPktHandlePmPCI
(
    IN    GT_U8      devNum,
    IN    GT_U8      queueIdx,
    INOUT GT_U32    *numOfBuffPtr,
    OUT   GT_U8     *packetBuffs[],
    OUT   GT_U32     buffLen[],
    OUT   void     **rxParams
)
{
    GT_STATUS                       rc;

    /* get the packet from the device */
    rc = cpssExMxPmNetIfSdmaRxPacketGet(devNum, queueIdx, numOfBuffPtr,
                        packetBuffs, buffLen, &pmRxParams);

    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
    {
        /* no need to free buffers because not got any */
        return rc;
    }
    *rxParams = &pmRxParams;


    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdGenRxPktReceive(devNum,queueIdx, *numOfBuffPtr,
                              packetBuffs, buffLen, *rxParams);
#endif /* IMPL_GALTIS */


    /* now you need to free the buffers */
    rc = cpssExMxPmNetIfSdmaRxBufFree(devNum, queueIdx, packetBuffs, *numOfBuffPtr);

    return rc;
}
#endif

#if defined EX_CODE
#include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>

static CPSS_EXMX_NET_RX_PARAMS_STC     exRxParams;


/*******************************************************************************
* appDemoNetRxPktHandleExMxPCI
*
* DESCRIPTION:
*       application routine to receive frames . -- Gen function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
GT_STATUS appDemoNetRxPktHandleExPCI
(
    IN    GT_U8      devNum,
    IN    GT_U8      queueIdx,
    INOUT GT_U32    *numOfBuffPtr,
    OUT   GT_U8     *packetBuffs[],
    OUT   GT_U32     buffLen[],
    OUT   void     **rxParams
)
{
    GT_STATUS                       rc;

    /* get the packet from the device */
    rc = cpssExMxNetIfRxPacketGet(devNum, queueIdx, numOfBuffPtr,
                        packetBuffs, buffLen, &exRxParams);

    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
    {
        /* no need to free buffers because not got any */
        return rc;
    }
    *rxParams = &exRxParams;


    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdGenRxPktReceive(devNum,queueIdx, *numOfBuffPtr,
                              packetBuffs, buffLen, *rxParams);
#endif /* IMPL_GALTIS */


    /* now you need to free the buffers */
    rc = cpssExMxNetIfRxBufFree(devNum, queueIdx, packetBuffs, *numOfBuffPtr);

    return rc;
}
#endif


/*******************************************************************************
* appDemoGenNetRxPktHandle
*
* DESCRIPTION:
*       application routine to receive frames . -- Gen function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
GT_STATUS appDemoGenNetRxPktHandle
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx
)
{
    GT_STATUS                           rc = GT_OK;
    GT_U32                              numOfBuff = BUFF_LEN;
    GT_U8*                              packetBuffs[BUFF_LEN];
    GT_U32                              buffLenArr[BUFF_LEN];
    void                               *rxParams;

    /* get the packet from the device */
    if(appDemoPpConfigList[devNum].cpuPortMode == CPSS_NET_CPU_PORT_MODE_SDMA_E)
    {
        if(CPSS_IS_EXMX_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
        {
#if defined EX_CODE
            rc = appDemoNetRxPktHandleExPCI(devNum, queueIdx, &numOfBuff,
                                packetBuffs, buffLenArr, &rxParams);
#endif /*EX_CODE*/
        }
        else if (CPSS_IS_DXCH_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
        {
#if defined DXCH_CODE
            rc = appDemoNetRxPktHandleChPCI(devNum, queueIdx, &numOfBuff,
                                            packetBuffs, buffLenArr, &rxParams);
#endif /*DXCH_CODE*/
        }
        else if (CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
        {
#if defined PM_CODE
            rc = appDemoNetRxPktHandlePmPCI(devNum, queueIdx, &numOfBuff,
                                            packetBuffs, buffLenArr, &rxParams);
#endif /*PM_CODE*/
        }
        else
        {
            numOfBuff = 1;
            packetBuffs[0]=NULL;
            buffLenArr[0]=1;
            rxParams = NULL;
        }

        return rc;
    }
    else if(appDemoPpConfigList[devNum].cpuPortMode == CPSS_NET_CPU_PORT_MODE_MII_E)
    {
#if defined DXCH_CODE
        rc = cpssDxChNetIfMiiRxPacketGet(devNum,queueIdx,&numOfBuff,packetBuffs,buffLenArr,&chRxParams);
        if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
        {
            /* no need to free buffers because not got any */
            return rc;
        }
        rxParams = (void*)(&chRxParams);

    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
        /* ignore the return code from this function because we want to keep on
           getting RX buffers , regardless to the CMD manager */
        (void) cmdGenRxPktReceive(devNum,queueIdx,numOfBuff,
                                  packetBuffs,buffLenArr,rxParams);
#endif /* IMPL_GALTIS */

        rc = cpssDxChNetIfMiiRxBufFree(devNum,queueIdx,packetBuffs,numOfBuff);
#else /* if not dx */
        return GT_NOT_IMPLEMENTED;
#endif /*DXCH_CODE*/
    }
    else
    {
        rc = GT_NOT_IMPLEMENTED;
    }

    return rc;
}




/*******************************************************************************/
/*******************************************************************************/
/********************************* AU MESSAGES HANDLE **************************/
/*******************************************************************************/
/*******************************************************************************/


#if defined PM_CODE
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>
#endif

#if defined DXCH_CODE
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#endif

typedef GT_STATUS fdbAuMsgBlockGetFunc(IN    GT_U8 devNum,
                                       INOUT GT_U32                      *numOfAuPtr,
                                       OUT   CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr);

typedef GT_STATUS fdbEntrySetFunc(IN GT_U8                   devNum,
                                  IN GT_U32                  messagePortGroupId,
                                  IN CPSS_MAC_ENTRY_EXT_STC *fdbEntryPtr);

typedef GT_STATUS fdbEntryDeleteFunc(IN GT_U8                        devNum,
                                     IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr);

typedef GT_STATUS fdbEntryInvalidateFunc(IN GT_U8                       devNum,
                                         IN CPSS_MAC_ENTRY_EXT_TYPE_ENT entryType,
                                         IN GT_U32                      index);

typedef GT_STATUS fdbFromCpuAuMsgStatusGetFunc(IN  GT_U8    devNum,
                                               OUT GT_BOOL  *completedPtr,
                                               OUT GT_BOOL  *succeededPtr);

static fdbAuMsgBlockGetFunc          *cpssEnFdbBrgFdbAuMsgBlockGet = NULL;
static fdbEntrySetFunc               *cpssEnFdbBrgFdbEntrySetFunc;
static fdbEntryDeleteFunc            *cpssEnFdbBrgFdbEntryDeleteFunc;
static fdbEntryInvalidateFunc        *cpssEnFdbBrgFdbEntryInvalidateFunc;
static fdbFromCpuAuMsgStatusGetFunc  *cpssEnFdbFromCpuAuMsgStatusGetFunc;

#if defined PM_CODE

extern void         wrapCpssExMxPmBrgAuqMutexLock(void);
extern void         wrapCpssExMxPmBrgAuqMutexUnLock(void);

typedef enum
{
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E,
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_QUERY_FROM_CPU_E,
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_TRIGGERED_ACTION_E
}WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_ENT;

extern GT_STATUS wrapCpssExMxPmBrgFdbResolveDeviceDeadLock
(
    IN  GT_U8                                     devNum,
    IN  WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_ENT  action,
    OUT GT_BOOL                                   *repeatActionPtr
);

/*******************************************************************************
* prvFdbEntryConvertToExMxPmFormat
*
* DESCRIPTION:
*       The function converts FDB entry from general format to EXMXPM format.
*
*  APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       macEntryPtr - pointer to FDB entry with general format
*
* OUTPUTS:
*       fdbEntryPtr  - pointer to FDB entry with EXMXPM format
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID prvFdbEntryConvertToExMxPmFormat
(
    IN  CPSS_MAC_ENTRY_EXT_STC      *macEntryPtr,
    OUT CPSS_EXMXPM_FDB_ENTRY_STC   *fdbEntryPtr
)
{

    switch(macEntryPtr->key.entryType)
    {
       case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
           fdbEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E;
           break;
       case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
           fdbEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;
           break;
       case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
           fdbEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E;
           break;
       default:
           break;
    }

    if((macEntryPtr->key.entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E) ||
       (macEntryPtr->key.entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E))
    {
        cpssOsMemCpy(fdbEntryPtr->key.key.ipMcast.sip,
                     macEntryPtr->key.key.ipMcast.sip, 4);
        cpssOsMemCpy(fdbEntryPtr->key.key.ipMcast.dip,
                     macEntryPtr->key.key.ipMcast.dip, 4);
        fdbEntryPtr->key.key.ipMcast.fId = macEntryPtr->key.key.ipMcast.vlanId;
    }
    else
    {
        cpssOsMemCpy(fdbEntryPtr->key.key.macFid.macAddr.arEther,
                     macEntryPtr->key.key.macVlan.macAddr.arEther, 6);

        fdbEntryPtr->key.key.macFid.fId = macEntryPtr->key.key.macVlan.vlanId;
    }

    osMemCpy(&(fdbEntryPtr->dstOutlif.interfaceInfo),
            &(macEntryPtr->dstInterface),
            sizeof(macEntryPtr->dstInterface));

    fdbEntryPtr->isStatic = macEntryPtr->isStatic;
    fdbEntryPtr->dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;

    switch(macEntryPtr->daCommand)
    {
       case CPSS_MAC_TABLE_FRWRD_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_FORWARD_E;
           break;
       case CPSS_MAC_TABLE_DROP_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_DROP_HARD_E;
           break;
       case CPSS_MAC_TABLE_INTERV_E:
       case CPSS_MAC_TABLE_CNTL_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
           break;
       case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
           break;
       case CPSS_MAC_TABLE_SOFT_DROP_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
           break;
       default:
           break;
    }

    switch(macEntryPtr->saCommand)
    {
       case CPSS_MAC_TABLE_FRWRD_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_FORWARD_E;
           break;
       case CPSS_MAC_TABLE_DROP_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_DROP_HARD_E;
           break;
       case CPSS_MAC_TABLE_INTERV_E:
       case CPSS_MAC_TABLE_CNTL_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
           break;
       case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
           break;
       case CPSS_MAC_TABLE_SOFT_DROP_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
           break;
       default:
           break;
    }

    fdbEntryPtr->daRoute = macEntryPtr->daRoute;
    fdbEntryPtr->mirrorToRxAnalyzerPortEn = macEntryPtr->mirrorToRxAnalyzerPortEn;
    fdbEntryPtr->sourceId = macEntryPtr->sourceID;
    fdbEntryPtr->userDefined = macEntryPtr->userDefined;
    fdbEntryPtr->daQosIndex = macEntryPtr->daQosIndex;
    fdbEntryPtr->saQosIndex = macEntryPtr->saQosIndex;
    fdbEntryPtr->daSecurityLevel = macEntryPtr->daSecurityLevel;
    fdbEntryPtr->saSecurityLevel = macEntryPtr->saSecurityLevel;
    fdbEntryPtr->appSpecificCpuCode = macEntryPtr->appSpecificCpuCode;

    /* The correct support of new field in generic structure is: */
    /* fdbEntryPtr->spUnknown = macEntryPtr->spUnknown;          */
    /* But the code kept backward compatible.                    */
    fdbEntryPtr->spUnknown = GT_FALSE;

}

/*******************************************************************************
* prvExMxPmConvertToAuGenFormat
*
* DESCRIPTION:
*       The function converts EXMXPM AU message to general AU message format.
*
*  APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       auMessagesPtr    - pointer to AU message in EXMXPM format
*
* OUTPUTS:
*       auMessagesGenPtr - pointer to AU message in general format
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID prvExMxPmConvertToAuGenFormat
(
    IN  CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC  *auMessagesPtr,
    OUT CPSS_MAC_UPDATE_MSG_EXT_STC  *auMessagesGenPtr
)
{


    auMessagesGenPtr->updType         = auMessagesPtr->updType;
    auMessagesGenPtr->entryWasFound   = auMessagesPtr->entryWasFound;
    auMessagesGenPtr->macEntryIndex   = auMessagesPtr->fdbEntryIndex;

    switch(auMessagesPtr->fdbEntry.key.entryType)
    {
       case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
           auMessagesGenPtr->macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
           break;
       case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
           auMessagesGenPtr->macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
           break;
       case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:
           auMessagesGenPtr->macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
           break;
       default:
           break;
    }

    if((auMessagesPtr->fdbEntry.key.entryType ==
        CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E) ||
       (auMessagesPtr->fdbEntry.key.entryType ==
         CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E))
    {
        cpssOsMemCpy(auMessagesGenPtr->macEntry.key.key.ipMcast.sip,
                     auMessagesPtr->fdbEntry.key.key.ipMcast.sip, 4);
        cpssOsMemCpy(auMessagesGenPtr->macEntry.key.key.ipMcast.dip,
                     auMessagesPtr->fdbEntry.key.key.ipMcast.dip, 4);
        auMessagesGenPtr->macEntry.key.key.ipMcast.vlanId =
             (GT_U16)auMessagesPtr->fdbEntry.key.key.ipMcast.fId;
    }
    else
    {
        cpssOsMemCpy(auMessagesGenPtr->macEntry.key.key.macVlan.macAddr.arEther,
                     auMessagesPtr->fdbEntry.key.key.macFid.macAddr.arEther, 6);

        auMessagesGenPtr->macEntry.key.key.macVlan.vlanId =
            (GT_U16)auMessagesPtr->fdbEntry.key.key.macFid.fId;
    }



    osMemCpy(&(auMessagesGenPtr->macEntry.dstInterface),
             &(auMessagesPtr->fdbEntry.dstOutlif.interfaceInfo),
            sizeof(auMessagesPtr->fdbEntry.dstOutlif.interfaceInfo));



    auMessagesGenPtr->macEntry.isStatic = auMessagesPtr->fdbEntry.isStatic;

    switch(auMessagesPtr->fdbEntry.daCommand)
    {
       case CPSS_PACKET_CMD_FORWARD_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
           break;
       case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
           break;
       case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_CNTL_E;
           break;
       case CPSS_PACKET_CMD_DROP_HARD_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_DROP_E;
           break;
       case CPSS_PACKET_CMD_DROP_SOFT_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_SOFT_DROP_E;
           break;
       default:
           break;
    }


    switch(auMessagesPtr->fdbEntry.saCommand)
    {
       case CPSS_PACKET_CMD_FORWARD_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
           break;
       case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
           break;
       case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_CNTL_E;
           break;
       case CPSS_PACKET_CMD_DROP_HARD_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_DROP_E;
           break;
       case CPSS_PACKET_CMD_DROP_SOFT_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_SOFT_DROP_E;
           break;
       default:
           break;
    }


    auMessagesGenPtr->macEntry.daRoute = auMessagesPtr->fdbEntry.daRoute;
    auMessagesGenPtr->macEntry.mirrorToRxAnalyzerPortEn =
        auMessagesPtr->fdbEntry.mirrorToRxAnalyzerPortEn;
    auMessagesGenPtr->macEntry.sourceID = auMessagesPtr->fdbEntry.sourceId;
    auMessagesGenPtr->macEntry.userDefined = auMessagesPtr->fdbEntry.userDefined;
    auMessagesGenPtr->macEntry.daQosIndex = auMessagesPtr->fdbEntry.daQosIndex;
    auMessagesGenPtr->macEntry.saQosIndex = auMessagesPtr->fdbEntry.saQosIndex;
    auMessagesGenPtr->macEntry.daSecurityLevel =
        auMessagesPtr->fdbEntry.daSecurityLevel;
    auMessagesGenPtr->macEntry.saSecurityLevel =
        auMessagesPtr->fdbEntry.saSecurityLevel;
    auMessagesGenPtr->macEntry.appSpecificCpuCode =
        auMessagesPtr->fdbEntry.appSpecificCpuCode;

    auMessagesGenPtr->skip = auMessagesPtr->skip;
    auMessagesGenPtr->aging = auMessagesPtr->aging;
    auMessagesGenPtr->associatedHwDevNum = auMessagesPtr->associatedDevNum;
    auMessagesGenPtr->entryOffset = auMessagesPtr->entryOffset;
    auMessagesGenPtr->portGroupId = auMessagesPtr->portGroupId;


}

/*******************************************************************************
* prvExMxPmBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller.
*       The PP may place AU messages in common FDB Upload (FU) messages
*       queue or in separate queue only for AU messages. The function
*       cpssExMxPmHwPpPhase2Init configures queue for AU messages by the
*       fuqUseSeparate parameter. If common FU queue is used then function
*       returns all AU messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
*  APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvExMxPmBrgFdbAuMsgBlockGet
(
    IN      GT_U8                                    devNum,
    INOUT   GT_U32                                   *numOfAuPtr,
    OUT     CPSS_MAC_UPDATE_MSG_EXT_STC              *auMessagesPtr
)
{
    GT_U32 ii;
    CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC  auPmMessagesArr[AU_SIZE];
    GT_STATUS rc;

    wrapCpssExMxPmBrgAuqMutexLock();
    rc = cpssExMxPmBrgFdbAuMsgBlockGet(devNum, numOfAuPtr, auPmMessagesArr);
    wrapCpssExMxPmBrgAuqMutexUnLock();
    if ((rc != GT_OK) && (rc != GT_NO_MORE))
    {
        return rc;
    }

    for (ii = 0; ii < *numOfAuPtr; ii++)
    {
       prvExMxPmConvertToAuGenFormat(&auPmMessagesArr[ii], &auMessagesPtr[ii]);

    }

    return rc;

}

/*******************************************************************************
* prvExMxPmBrgFdbEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware FDB table through
*       Address Update message.(AU message to the PP is non direct access to FDB
*       table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryPtr     - pointer to FDB table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvExMxPmBrgFdbEntrySet
(
    IN GT_U8                            devNum,
    IN GT_U32                           messagePortGroupId,
    IN CPSS_MAC_ENTRY_EXT_STC           *fdbEntryPtr
)
{
    GT_STATUS       rc;
    CPSS_EXMXPM_FDB_ENTRY_STC   pmFdbEntry;
    GT_BOOL repeatAction;
    GT_U32  numRetries=10;

    messagePortGroupId = messagePortGroupId;/* ignored parameter */

    /* not all fields in the FDB entry comes from the 'common structure' */
    osMemSet(&pmFdbEntry,0,sizeof(pmFdbEntry));

    prvFdbEntryConvertToExMxPmFormat(fdbEntryPtr, &pmFdbEntry);

    wrapCpssExMxPmBrgAuqMutexLock();
    do{
        rc = cpssExMxPmBrgFdbEntrySet(devNum, &pmFdbEntry);
        wrapCpssExMxPmBrgFdbResolveDeviceDeadLock(devNum,
                    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E,
                    &repeatAction);
        numRetries --;
    }while(repeatAction == GT_TRUE && numRetries);
    wrapCpssExMxPmBrgAuqMutexUnLock();

    return rc;
}


/*******************************************************************************
* prvExMxPmBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware FDB table through Address Update
*       message.(AU message to the PP is non direct access to FDB table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryKeyPtr  - pointer to key parameters of the FDB entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssExMxPmBrgFdbEntrySet,
*       cpssExMxPmBrgFdbQaSend and cpssExMxPmBrgFdbEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssExMxPmBrgFdbFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static GT_STATUS prvExMxPmBrgFdbMacEntryDelete
(
    IN GT_U8                            devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC       *fdbEntryKeyPtr
)
{
    GT_STATUS       rc;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC   pumaFdbEntryKey;
    GT_BOOL repeatAction;
    GT_U32  numRetries=10;

    pumaFdbEntryKey.entryType = fdbEntryKeyPtr->entryType;
    switch(fdbEntryKeyPtr->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            pumaFdbEntryKey.key.macFid.macAddr = fdbEntryKeyPtr->key.macVlan.macAddr;
            pumaFdbEntryKey.key.macFid.fId = fdbEntryKeyPtr->key.macVlan.vlanId;
            break;
        default:
            osMemCpy(fdbEntryKeyPtr->key.ipMcast.sip,pumaFdbEntryKey.key.ipMcast.sip,4);
            osMemCpy(fdbEntryKeyPtr->key.ipMcast.dip,pumaFdbEntryKey.key.ipMcast.dip,4);
            pumaFdbEntryKey.key.ipMcast.fId = fdbEntryKeyPtr->key.ipMcast.vlanId;
            break;
    }

    wrapCpssExMxPmBrgAuqMutexLock();
    do{
        rc = cpssExMxPmBrgFdbEntryDelete(devNum, &pumaFdbEntryKey);
        wrapCpssExMxPmBrgFdbResolveDeviceDeadLock(devNum,
                    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E,
                    &repeatAction);
        numRetries --;
    }while(repeatAction == GT_TRUE && numRetries);
    wrapCpssExMxPmBrgAuqMutexUnLock();

    return rc;
}

/*******************************************************************************
* prvExMxPmBrgFdbFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function checks the status of last AU message process:
*       completion and success.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       completedPtr - pointer to AU message processing completion.
*                      GT_TRUE - AU message was processed by PP.
*                      GT_FALSE - AU message processing is not completed yet by PP.
*       succeededPtr - pointer to a success of AU operation.
*                      GT_TRUE  - the AU action succeeded.
*                      GT_FALSE - the AU action has failed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or msgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvExMxPmBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8                   devNum,
    OUT GT_BOOL                 *completedPtr,
    OUT GT_BOOL                 *succeededPtr
)
{
    return cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(devNum, CPSS_NA_E,
                                                 completedPtr, succeededPtr);

}

#endif

#if defined DXCH_CODE
GT_STATUS prvDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN GT_U8    devNum,
    IN GT_BOOL  usePortGroupsBmp,
    IN GT_U32   currPortGroupsBmp
)
{
    GT_STATUS   st;
    GT_BOOL completed;
    GT_BOOL succeeded;
    GT_PORT_GROUPS_BMP completedBmp;
    GT_PORT_GROUPS_BMP succeededBmp;
    GT_PORT_GROUPS_BMP completedBmpSummary;
    GT_PORT_GROUPS_BMP succeededBmpSummary;
    #ifdef ASIC_SIMULATION
    GT_U32  sleepTime = 0;
    GT_U32  maxRetry = appDemoMaxRetryOnSleep0;
    #endif/*ASIC_SIMULATION*/

    if(usePortGroupsBmp == GT_FALSE)
    {
        completed = GT_FALSE;
        succeeded = GT_FALSE;

        do{
            #ifdef ASIC_SIMULATION
            if ((sleepTime == 0) && maxRetry)
            {
                /* do max retries with 'busy wait' but with option to other tasks
                   to also run --> reduce run time */
                maxRetry--;
            }
            else if (maxRetry == 0)
            {
                sleepTime = 1;
            }
            osTimerWkAfter(sleepTime);
            #endif /*ASIC_SIMULATION*/
            st = cpssDxChBrgFdbFromCpuAuMsgStatusGet(devNum, &completed, &succeeded,NULL);
            if(st != GT_OK)
            {
                goto exit_cleanly_lbl;
            }
        } while (completed == GT_FALSE);

        if(succeeded == GT_FALSE)
        {
            /* operation finished but not succeeded */
            st = GT_FAIL;
            goto exit_cleanly_lbl;
        }
    }
    else
    {
        completedBmpSummary = 0;
        succeededBmpSummary = 0;
        do{
            completedBmp = 0;
            succeededBmp = 0;
            #ifdef ASIC_SIMULATION
            if ((sleepTime == 0) && maxRetry)
            {
                /* do max retries with 'busy wait' but with option to other tasks
                   to also run --> reduce run time */
                maxRetry--;
            }
            else if (maxRetry == 0)
            {
                sleepTime = 1;
            }
            osTimerWkAfter(sleepTime);
            #endif /*ASIC_SIMULATION*/
            st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(devNum, currPortGroupsBmp, &completedBmp, &succeededBmp);
            if(st != GT_OK)
            {
                goto exit_cleanly_lbl;
            }

            completedBmpSummary |= completedBmp;
            succeededBmpSummary |= succeededBmp;
        }while (
            (completedBmpSummary & PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp) !=
            (currPortGroupsBmp & PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp));

        if(completedBmpSummary != succeededBmpSummary)
        {
            /* operation finished but not succeeded on all port groups */
            st = GT_FAIL;
            goto exit_cleanly_lbl;
        }
    }

exit_cleanly_lbl:

    return st;
}

#define SIP5_FDB_NUM_OF_BANKS_CNS   16
/* inserting the NA failed but we may be able to move one of the
   already existing FDB entries 'FDB Cuckoo algorithm'
   NOTE: this is relevant only to Sip5 devices.

    Calculate the 16 optional FDB indexes : cpssDxChBrgFdbHashCrcMultiResultsCalc ( optionalIndexesArr[16])
    i.        'move' one of the 'colliding' entries (new 'NA move index' message)
            cpssDxChBrgFdbMacEntryMove (optionalIndexesArr[ii]) , ii = 0..15
        1.        If failed , try with other 'colliding' entries (retry up to 16 time)
        2.        If fail on all 16 'colliding' entries ' FAILED
    ii.        Insert the new entry in the 'free place' , cpssDxChBrgFdbMacEntrySet (newEntry)
    iii.        Should succeed , cpssDxChBrgFdbFromCpuAuMsgStatusGet , succeeded  = GT_TRUE
*/
static GT_STATUS prvDxChBrgFdbMacEntryTryMoveToFindFreeEntry
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS rc;
    GT_U32      crcMultiHashArr[SIP5_FDB_NUM_OF_BANKS_CNS];
    GT_U32      numOfBanks = SIP5_FDB_NUM_OF_BANKS_CNS;
    GT_U32      ii;
    GT_U32      fdbIndex;/* the FDB index of the entry that we try to move to
                            new place to allow the new entry to used instead of it. */

    /* calculate the 16 entries that may occupy the places for this new entry */
    rc = cpssDxChBrgFdbHashCrcMultiResultsCalc(devNum,
        &macEntryPtr->key,/*entryKeyPtr*/
        0,/*multiHashStartBankIndex*/
        numOfBanks,
        crcMultiHashArr);
    if(rc != GT_OK)
    {
        return rc;
    }

    for(ii = 0 ; ii < numOfBanks; ii++)
    {
        fdbIndex = crcMultiHashArr[ii];

        /* send request to move this entry */
        rc = cpssDxChBrgFdbMacEntryMove(devNum,fdbIndex);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if the movement succeeded */
        rc = prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_FALSE,0);
        if(rc == GT_OK)
        {
            /* we managed to move the entry to new place */
            break;
        }
    }

    if(ii == numOfBanks)
    {
        return GT_FULL;
    }

    /* retry to add the entry */
    rc = cpssDxChBrgFdbMacEntrySet(devNum,macEntryPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* the operation should succeed */
    rc =  prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_FALSE,0);
    return rc;
}

/* function to be called when adding new entry failed */
GT_STATUS   prvDxChBrgFdbMacEntryNewFailed
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS rc;
    GT_U32    reorder = 0;
    GT_U32    needStatistics = 0;
    GT_U32    devIdx; /* index to appDemoPpConfigList */

    rc = appDemoDevIdxGet(devNum, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    if(appDemoPpConfigList[devIdx].doFdbCuckoo == GT_TRUE)
    {
        /* inserting the NA failed but we may be able to move one of the
           already existing FDB entries 'FDB Cuckoo algorithm'

           NOTE: this is relevant only to Sip5 devices.
        */
        rc = prvDxChBrgFdbMacEntryTryMoveToFindFreeEntry(devNum,macEntryPtr);
        if(rc == GT_OK)
        {
            reorder = 1;
            /* the movement of old entry to other place allowed the new entry
               to take it's place */
            numOfSuccessesInReorder++;
        }
        else
        {
            numOfFailesInReorder++;
        }
    }
    else
    {
        /* indicate that the bucket is FULL */
        rc = GT_FULL;
        numOfFailesInReorder++;
    }

    if(reorder == 0)
    {
        if(traceAuq)
        {
            /* put visual indication that NA failed */
            osPrintf("F"/*FAILED*/);/* should be followed by "+" due to traceAuqArr[CPSS_NA_E]*/
        }

        if(isFirstMissDone == 0)
        {
            isFirstMissDone = 1;
            if (traceFirstMiss)
            {
                osPrintf("NA processing: First Miss detected \n");
                needStatistics = 1;
            }
        }
    }
    else /*reorder==1*/
    {
        if(traceAuq)
        {
            /* put visual that reorder succeeded */
            osPrintf("%s",traceAuqArr[CPSS_NA_MOVED_E]); /* should be followed by "+" due to traceAuqArr[CPSS_NA_E]*/
        }

        if(isFirstReorderDone == 0)
        {
            isFirstReorderDone = 1;
            if (traceFirstMiss)
            {
                osPrintf("NA processing: First Reorder detected \n");
                needStatistics = 1;
            }
        }
    }

    if(needStatistics)
    {
#ifdef IMPL_GALTIS
        /* call the debug function to print capacity of FDB */
        /* this actually print info from 'first port group' */
        (void) cpssDxChBrgFdbPortGroupCount_debug(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS);
#endif /* IMPL_GALTIS */
    }

    return rc;
}


/* 'mac set' entry for multi port groups device */
GT_STATUS prvDxChBrgFdbPortGroupMacEntrySet
(
    IN GT_U8                        devNum,
    IN GT_U32                       messagePortGroupId,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS   rc;
    GT_PORT_GROUPS_BMP  deletedPortGroupsBmp;  /* bitmap of Port Groups - to delete the entry from */
    GT_PORT_GROUPS_BMP  portGroupsBmp;  /* bitmap of Port Groups */
    GT_U32              devIdx; /* index to appDemoPpConfigList */

    rc = appDemoDevIdxGet(devNum, &devIdx);
    if ( GT_OK != rc )
    {
        return rc;
    }

    if(appDemoPpConfigList[devIdx].numOfPortGroups == 0)
    {
        rc = cpssDxChBrgFdbMacEntrySet(devNum,macEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc =  prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_FALSE,0);

        if(rc == GT_FAIL)
        {
            /*adding new entry failed - check to see if we can do something about it.
              or just to 'trace' this fail */
            rc = prvDxChBrgFdbMacEntryNewFailed(devNum,macEntryPtr);
        }

        return rc;
    }

    if(appDemoPpConfigList[devIdx].fdbMode ==
        APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E)
    {
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }
    else
    {
        /* do NOT care if entry is :
            1. port or trunk
            2. or relate to local/remote device
           because this is ' controlled learn' that actually emulates 'auto learn'.
           so when message come from one of the port groups we add the entry
           to all port groups that associated with this src port group (unifiedFdbPortGroupsBmp)
        */
        if(messagePortGroupId < appDemoPpConfigList[devIdx].numOfPortGroups)
        {
            portGroupsBmp =
                appDemoPpConfigList[devIdx].portGroupsInfo[messagePortGroupId].unifiedFdbPortGroupsBmp;
        }
        else
        {
            /* error -- should not happen */
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
    }

    rc =  cpssDxChBrgFdbPortGroupMacEntrySet(devNum,portGroupsBmp,macEntryPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portGroupsBmp == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        rc = prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_FALSE,0);
        if(rc == GT_FAIL)
        {
            /*adding new entry failed - check to see if we can do something about it.
              or just to 'trace' this fail */
            /*adding new entry failed*/
            rc = prvDxChBrgFdbMacEntryNewFailed(devNum,macEntryPtr);
        }

        return rc;
    }

    /* remove from the active port groups the port groups that we added the entry
       to them */
    deletedPortGroupsBmp =
        PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp & (~portGroupsBmp);

    /* we need to delete the mac entry from other port groups that it may be in
       (station movement) */
    rc = cpssDxChBrgFdbPortGroupMacEntryDelete(devNum,deletedPortGroupsBmp,&macEntryPtr->key);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* wait for finish on deleted port groups , but not require success ,
       because mac not necessarily exists there */
    rc =  prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_TRUE,deletedPortGroupsBmp);
    if(rc != GT_OK)
    {
        /* this is not error !!!! */
        rc = GT_OK;
    }

    /* wait for finish and success on added port groups to see that that added
       entry succeeded */
    rc = prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_TRUE,portGroupsBmp);

    return rc;
}

GT_STATUS prvDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    GT_STATUS   rc;
    GT_U32      devIdx; /* index to appDemoPpConfigList */

    rc = cpssDxChBrgFdbMacEntryDelete(devNum,macEntryKeyPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc =  prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_FALSE,0);

    if(rc != GT_OK)
    {
        rc = appDemoDevIdxGet(devNum, &devIdx);
        if ( GT_OK != rc )
        {
            return rc;
        }

        if(appDemoPpConfigList[devIdx].numOfPortGroups == 0 ||
           (appDemoPpConfigList[devIdx].fdbMode ==
            APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E))
        {
            return rc;
        }

        /* wait for finish on deleted port groups , but not require success ,
           because mac not necessarily exists there */

        /* this is not error !!!! */
        rc = GT_OK;
    }

    return rc;
}

GT_STATUS prvDxChBrgFdbMacEntryInvalidate
(
    IN  GT_U8                           devNum,
    IN  CPSS_MAC_ENTRY_EXT_TYPE_ENT     entryType,
    IN  GT_U32                          index
)
{
    GT_STATUS rc=GT_OK;
    GT_BOOL   valid;
    GT_BOOL   skip;
    GT_U32    pairIndex=0;

    /* get index pair incase of ipv6 key or data */
    if((entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E)||
       (entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E))
    {
        /* need to get second index to delete */
        rc = prvAppDemoBrgFdbIpv6UcFindPairIndexToDelete(devNum, entryType, index, GT_FALSE, &pairIndex);
        if((rc != GT_OK)&&(rc!=GT_NOT_FOUND))
        {
            return rc;
        }
    }

    rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,index,&valid,&skip);
    if(rc != GT_OK)
        return rc;

    /* If the entry is valid need to invalidate and to decrement the bank counter
       If the entry is invalid - do nothing */
    if(valid)
    {
        rc = cpssDxChBrgFdbMacEntryInvalidate(devNum,index);
        if(rc!=GT_OK)
            return rc;

        /* decrement bank counter */
        rc =  cpssDxChBrgFdbBankCounterUpdate(devNum, index%16, GT_FALSE);
        if(rc!=GT_OK)
            return rc;

    }

    if(pairIndex != PRV_APPDEMO_INVALID_FDB_INDEX_VALUE_CNS)
    {
        rc = cpssDxChBrgFdbMacEntryStatusGet(devNum,pairIndex,&valid,&skip);
        if(rc != GT_OK)
            return rc;

        /* If the entry is valid need to invalidate and to decrement the bank counter
           If the entry is invalid - do nothing */
        if(valid)
        {
            rc = cpssDxChBrgFdbMacEntryInvalidate(devNum,pairIndex);
            if(rc!=GT_OK)
                return rc;

            /* decrement bank counter */
            rc =  cpssDxChBrgFdbBankCounterUpdate(devNum, pairIndex%16, GT_FALSE);
            if(rc!=GT_OK)
                return rc;
        }
    }

    /* if we succeed in deleting some or both entries (ipv6 UC addr and data) from HW
       we need to delete the address/data index from the Link List data base */
    rc = prvAppDemoBrgFdbIpv6UcFindPairIndexToDelete(devNum, entryType, index, GT_TRUE, &pairIndex);
    if(rc != GT_OK)
    {
        return rc;
    }

    return rc;
}
#endif /*DXCH_CODE*/

/*******************************************************************************
* prvAppDemoAaMsgHandleFdbUcRouteEntries
*
* DESCRIPTION:
*       This handle AA message for FDB UC Route Entries
*
* INPUTS:
*       entryType       - UC Entry type: ipv4/ipv6 Key/ipv6 data
*       macEntryIndex   - UC Entry index to invalidate
*
* OUTPUTS:
*       errorCounterPtr - (pointer to) incremented on every error.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvAppDemoAaMsgHandleFdbUcRouteEntries
(
    IN  CPSS_MAC_ENTRY_EXT_TYPE_ENT entryType,
    IN  GT_U32                      macEntryIndex,
    OUT GT_U32                      *errorCounterPtr
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       dev;

    if(cpssEnFdbBrgFdbEntryInvalidateFunc)
    {
        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
        {
            if(appDemoPpConfigList[dev].valid == GT_FALSE)
            {
                continue;
            }

            AU_FDB_UPDATE_LOCK();
            /* delete data from cpss */

            rc = cpssEnFdbBrgFdbEntryInvalidateFunc(appDemoPpConfigList[dev].devNum,
                                                    entryType,macEntryIndex);

            if (rc != GT_OK)
            {
                /* go to next message to handle */
                (*errorCounterPtr)++;
                AU_FDB_UPDATE_UNLOCK();
                continue;
            }

            AU_FDB_UPDATE_UNLOCK();
        }
    }

    return rc;
}

/*******************************************************************************
* appDemoAuMsgHandle
*
* DESCRIPTION:
*       This handle batch of AU messages.
*
* INPUTS:
*       devNum      - the device number.
*       bufferArr[] - array of AU messages
*       numOfAu     - number of AU messages in bufferArr[]
*
* OUTPUTS:
*       errorCounterPtr - (pointer to) incremented on every error.
*       isFirstMissDonePtr  - (pointer to) indication to trace the first fail of adding an entry into the FDB
*                   when this flag is set to 1 , the first time that the engine can't add entry
*                   the engine will check how much entries in the FDB. and print in % the capacity.
*       isFirstReorderDonePtr -  indication that the Cuckoo algorithm succeeded to do it's first reorder of
*                   entries in order to insert NA that first failed to be inserted but after the
*                   are order succeeded
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoAuMsgHandle
(
    IN  GT_UINTPTR           auEvHandler,
    IN  GT_U8                devNum,
    IN  CPSS_MAC_UPDATE_MSG_EXT_STC bufferArr[],
    IN  GT_U32               numOfAu,
    OUT GT_U32              *errorCounterPtr,
    OUT GT_BOOL             *isFirstMissDonePtr,
    OUT GT_BOOL             *isFirstReorderDonePtr
)
{
    GT_STATUS                   rc;
    GT_U32                      auIndex;
    GT_BOOL                     completed;
    GT_BOOL                     succeeded;
    CPSS_MAC_ENTRY_EXT_STC      cpssMacEntry;
    GT_U8                       dev;
    CPSS_MAC_ENTRY_EXT_TYPE_ENT entryType;

    for (auIndex = 0; auIndex < numOfAu; auIndex++)
    {
        if(bufferArr[auIndex].updType <= CPSS_FU_E )
        {
            appDemoAuMessageNum[bufferArr[auIndex].updType]++;
        }
        else
        {
            appDemoAuWrongMessageNum++;
        }

        switch (bufferArr[auIndex].updType)
        {
            case CPSS_AA_E:
                if(appDemoSysConfig.supportAaMessage == GT_FALSE)
                {
                    break;
                }

                entryType = bufferArr[auIndex].macEntry.key.entryType;
                if (((entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E)||
                    (entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E)||
                    (entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E))
#if defined DXCH_CODE
                    && (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_BOBCAT2_FDB_ROUTE_UC_DELETE_BY_MESSAGE_WA_E))
#endif
                    )
                {
                    rc = prvAppDemoAaMsgHandleFdbUcRouteEntries(entryType,bufferArr[auIndex].macEntryIndex,errorCounterPtr);
                    if(rc != GT_OK)
                        return rc;

                    break;
                }
                /* fall through ... */
            case CPSS_NA_E:
            {
                osMemCpy(&cpssMacEntry, &bufferArr[auIndex].macEntry,
                             sizeof(CPSS_MAC_ENTRY_EXT_STC));
                cpssMacEntry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
                cpssMacEntry.age = GT_TRUE;
                cpssMacEntry.spUnknown = GT_FALSE;/*even if SP came from device , we need to set entry with 'SP false' */

                for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
                {
                    if(appDemoPpConfigList[dev].valid == GT_FALSE)
                    {
                        continue;
                    }

                    AU_FDB_UPDATE_LOCK();
                    if(bufferArr[auIndex].updType == CPSS_NA_E)
                    {
                        /* write data to cpss */
                        rc = cpssEnFdbBrgFdbEntrySetFunc(appDemoPpConfigList[dev].devNum,
                                                         bufferArr[auIndex].portGroupId,&cpssMacEntry);
                    }
                    else /*AA*/
                    {
                        /* delete data from cpss */
                        rc = cpssEnFdbBrgFdbEntryDeleteFunc(appDemoPpConfigList[dev].devNum,
                                                            &cpssMacEntry.key);
                    }

                    if (rc != GT_OK)
                    {
                        /* go to next message to handle */
                        (*errorCounterPtr)++;
                        AU_FDB_UPDATE_UNLOCK();
                        continue;
                    }

                    if(cpssEnFdbFromCpuAuMsgStatusGetFunc)
                    {
                       completed = GT_FALSE;
                       succeeded = GT_FALSE;/*fix warning: local variable 'succeeded' may be used without having been initialized*/
                       /* wait for the PP to process the action */
                       while(completed == GT_FALSE)
                       {
                            /* simulation task needs CPU time in order
                               to execute the process */
                            #ifdef ASIC_SIMULATION
                            osTimerWkAfter(1);
                            #endif

                            /* Check that the AU message processing has completed */
                            rc = cpssEnFdbFromCpuAuMsgStatusGetFunc(appDemoPpConfigList[dev].devNum,
                                                                    &completed,
                                                                    &succeeded);
                            if (rc != GT_OK)
                            {
                                /* go to next message to handle */
                                (*errorCounterPtr)++;
                                succeeded = GT_FALSE;
                                break;
                            }
                       }
                    }
                    else
                    {
                        /* was checked in other place */
                        succeeded = GT_TRUE;
                    }

                    AU_FDB_UPDATE_UNLOCK();

                    if (succeeded == GT_FALSE)
                    {
                        /* the action competed but without success !? */
                        /* go to next message to handle */
                        (*errorCounterPtr)++;
                        continue;
                    }
                }
                break;

            }
            case CPSS_QA_E:
            case CPSS_QR_E:
            case CPSS_SA_E:
            case CPSS_QI_E:
            case CPSS_FU_E:
            case CPSS_TA_E:
                break;
            default:
                /* the action competed but without success !? */
                /* go to next message to handle */
                (*errorCounterPtr)++;
                continue;
        }

        if(traceAuq)
        {
            osPrintf("%s",traceAuqArr[bufferArr[auIndex].updType]);
        }

        /* check if we have callback function for this type of message */
        auEventHandlerDoCallbacks(auEvHandler,
                (AU_EV_HANDLER_EV_TYPE_ENT)(bufferArr[auIndex].updType),
                devNum, &bufferArr[auIndex]);

    }

    *isFirstMissDonePtr = isFirstMissDone;
    *isFirstReorderDonePtr = isFirstReorderDone;

    return GT_OK;
}

/*******************************************************************************
* appDemoDxChAuMsgHandle
*
* DESCRIPTION:
*       DXCH devices : This handle batch of AU messages.
*
* INPUTS:
*       devNum      - the device number.
*       bufferArr[] - array of AU messages
*       numOfAu     - number of AU messages in bufferArr[]
*
* OUTPUTS:
*       errorCounterPtr - (pointer to) incremented on every error.
*       isFirstMissDonePtr  - (pointer to) indication to trace the first fail of adding an entry into the FDB
*                   when this flag is set to 1 , the first time that the engine can't add entry
*                   the engine will check how much entries in the FDB. and print in % the capacity.
*       isFirstReorderDonePtr -  indication that the Cuckoo algorithm succeeded to do it's first reorder of
*                   entries in order to insert NA that first failed to be inserted but after the
*                   are order succeeded
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxChAuMsgHandle
(
    IN  GT_UINTPTR           auEvHandler,
    IN  GT_U8                devNum,
    IN  CPSS_MAC_UPDATE_MSG_EXT_STC bufferArr[],
    IN  GT_U32               numOfAu,
    OUT GT_U32              *errorCounterPtr,
    OUT GT_BOOL             *isFirstMissDonePtr,
    OUT GT_BOOL             *isFirstReorderDonePtr
)
{
#if defined DXCH_CODE
    cpssEnFdbBrgFdbAuMsgBlockGet       = cpssDxChBrgFdbAuMsgBlockGet;
    cpssEnFdbBrgFdbEntrySetFunc        = prvDxChBrgFdbPortGroupMacEntrySet;
    cpssEnFdbBrgFdbEntryDeleteFunc     = prvDxChBrgFdbMacEntryDelete;
    cpssEnFdbBrgFdbEntryInvalidateFunc = prvDxChBrgFdbMacEntryInvalidate;
    cpssEnFdbFromCpuAuMsgStatusGetFunc = NULL;/*the set and delete functions will handle it*/
#endif /*DXCH_CODE*/

    return appDemoAuMsgHandle(auEvHandler,devNum,bufferArr,numOfAu,errorCounterPtr,isFirstMissDonePtr,isFirstReorderDonePtr);
}



/*******************************************************************************
* cpssEnChDxAuMsgHandle
*
* DESCRIPTION:
*       This routine gets and handles the ChDx Address Update messages.
*
* INPUTS:
*       devNum      - the device number.
*       evExtData   - Unified event additional information
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
GT_STATUS cpssEnGenAuMsgHandle
(
    IN GT_UINTPTR           auEvHandler,
    IN GT_U8                devNum,
    GT_U32                  evExtData
)
{

    GT_STATUS                   rc = GT_OK;
    GT_STATUS                   rc1;
    GT_U32                      numOfAu;
    CPSS_MAC_UPDATE_MSG_EXT_STC bufferArr[AU_SIZE];
    GT_U32                      error = 0;/* number of errors */
    GT_BOOL   dummy1,dummy2;

    evExtData = evExtData;

    if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
    {
#if defined DXCH_CODE
        cpssEnFdbBrgFdbAuMsgBlockGet       = cpssDxChBrgFdbAuMsgBlockGet;
        cpssEnFdbBrgFdbEntrySetFunc        = prvDxChBrgFdbPortGroupMacEntrySet;
        cpssEnFdbBrgFdbEntryDeleteFunc     = prvDxChBrgFdbMacEntryDelete;
        cpssEnFdbBrgFdbEntryInvalidateFunc = prvDxChBrgFdbMacEntryInvalidate;
        cpssEnFdbFromCpuAuMsgStatusGetFunc = NULL;/*the set and delete functions will handle it*/
#endif /*DXCH_CODE*/
    }

    if(IS_API_EXMXPM_DEV_MAC(devNum))/* is device support EXMXPM API ? */
    {
#if defined PM_CODE
        cpssEnFdbBrgFdbAuMsgBlockGet       = prvExMxPmBrgFdbAuMsgBlockGet;
        cpssEnFdbBrgFdbEntrySetFunc        = prvExMxPmBrgFdbEntrySet;
        cpssEnFdbBrgFdbEntryDeleteFunc     = prvExMxPmBrgFdbMacEntryDelete;
        cpssEnFdbBrgFdbEntryInvalidateFunc = NULL;
        cpssEnFdbFromCpuAuMsgStatusGetFunc = prvExMxPmBrgFdbFromCpuAuMsgStatusGet;
#endif
    }

    if(NULL == cpssEnFdbBrgFdbAuMsgBlockGet)
    {
        /* the AU messages processing is not implemented yet for the device */
        return GT_OK;
    }

    /* reset the full buffer ... as CPSS not set all fields and it will cause 'memcmp' to fail. */
    cpssOsMemSet(bufferArr,0,sizeof(bufferArr));

    while(rc != GT_NO_MORE)
    {
        /*************************************/
        /* get the AU message buffer from HW */
        /*************************************/
        numOfAu = AU_SIZE;

        AU_FDB_UPDATE_LOCK();
        rc = cpssEnFdbBrgFdbAuMsgBlockGet(devNum, &numOfAu, bufferArr);
        AU_FDB_UPDATE_UNLOCK();

        if((rc != GT_NO_MORE) && (rc != GT_OK))
        {
            return rc;
        }

        /*********************************/
        /* handle all of the AU messages */
        /*********************************/
        rc1 = appDemoAuMsgHandle(auEvHandler,devNum,bufferArr,numOfAu,&error,&dummy1,&dummy2);
        if(rc1 != GT_OK)
        {
            return rc1;
        }

        /* reset the entries that we got */
        cpssOsMemSet(bufferArr,0,numOfAu * sizeof(bufferArr[0]));
    }

    if(error)
    {
        return GT_FAIL;
    }

    return rc;
}

/*******************************************************************************
* appDemoTraceAuqFlagSet
*
* DESCRIPTION:
*       function to allow set the flag of : traceAuq
*
* INPUTS:
*       enable - enable/disable the printings of indication of AUQ/FUQ messages:
*           "+",    CPSS_NA_E
*           "qa",   CPSS_QA_E should not be
*           "qr",   CPSS_QR_E
*           "-",    CPSS_AA_E
*           "t",    CPSS_TA_E
*           "sa",   CPSS_SA_E should not be
*           "qi",   CPSS_QI_E should not be
*           "f"     CPSS_FU_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoTraceAuqFlagSet
(
    IN GT_U32   enable
)
{
    traceAuq = enable;
    return GT_OK;
}

#ifdef ASIC_SIMULATION
GT_STATUS appDemoMaxRetryOnSleep0Set
(
    IN GT_U32   maxRetry
)
{
    appDemoMaxRetryOnSleep0 = maxRetry;
    return GT_OK;
}
#endif /*ASIC_SIMULATION*/

/*******************************************************************************
* appDemoAuMessageNumberDump
*
* DESCRIPTION:
*       Get number of Au messages.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoAuMessageNumberDump
(
    GT_VOID
)
{
    cpssOsPrintf("The number NA messages is %d\n", appDemoAuMessageNum[CPSS_NA_E]);
    cpssOsPrintf("The number QA messages is %d\n", appDemoAuMessageNum[CPSS_QA_E]);
    cpssOsPrintf("The number QR messages is %d\n", appDemoAuMessageNum[CPSS_QR_E]);
    cpssOsPrintf("The number AA messages is %d\n", appDemoAuMessageNum[CPSS_AA_E]);
    cpssOsPrintf("The number TA messages is %d\n", appDemoAuMessageNum[CPSS_TA_E]);
    cpssOsPrintf("The number SA messages is %d\n", appDemoAuMessageNum[CPSS_SA_E]);
    cpssOsPrintf("The number QI messages is %d\n", appDemoAuMessageNum[CPSS_QI_E]);
    cpssOsPrintf("The number FU messages is %d\n", appDemoAuMessageNum[CPSS_FU_E]);
    if(appDemoAuWrongMessageNum != 0)
    {
        cpssOsPrintf("The number AU wrong messages is %d\n", appDemoAuWrongMessageNum);
    }

    if(numOfSuccessesInReorder)
    {
        cpssOsPrintf("number of cases that the movement of old entry to other place allowed the new \n"
                     "entry to take it's place is [%d] \n",
                     numOfSuccessesInReorder);
    }

    if(numOfFailesInReorder)
    {
        cpssOsPrintf("number of cases that NA not succeeded [%d] \n",
                    numOfFailesInReorder);
    }




    return GT_OK;
}

/*******************************************************************************
* appDemoAuProcessingTraceFirstMiss
*
* DESCRIPTION:
*       function to allow set the flag of : traceFirstMiss
*
*       NOTE: this trace will happen only ONCE.
*             in order to get this print again , call appDemoAuProcessingFirstMissIndicationReset()
*             to state that the FDB did 'flush' so we can again look for 'first miss'
*
*       NOTE: another trace that relate to this flag is 'first reorder'
*             in order to get this print again , call appDemoAuProcessingFirstReorderIndicationReset()
*             to state that the FDB did 'flush' so we can again look for 'first reorder'
*
*
* INPUTS:
*       enable - enable/disable the printings during AU processing of indication
*               of 'first miss'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoAuProcessingTraceFirstMiss
(
    IN GT_U32   enable
)
{
    traceFirstMiss = enable;
#if defined EXMXPM_FAMILY
    /* the trace is currently implemented only for DXCH devices ,
       because we not have debug function like cpssDxChBrgFdbPortGroupCount_debug()*/
    osPrintf("the trace is currently implemented only for DXCH devices \n");
#endif /*EXMXPM_FAMILY*/
    return GT_OK;
}
/*******************************************************************************
* appDemoAuProcessingFirstMissIndicationReset
*
* DESCRIPTION:
*       function state the 'first miss' indication should be reset.
*       meaning that the next fail to insert new FDB entry would be considered
*       'first miss'
*
*       use this function to state that the FDB did 'flush' so we can again look
*       for 'first mniss'
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoAuProcessingFirstMissIndicationReset
(
    GT_VOID
)
{
    isFirstMissDone = 0;
    return GT_OK;
}

/*******************************************************************************
* appDemoAuProcessingFirstReorderIndicationReset
*
* DESCRIPTION:
*       function state that the Cuckoo algorithm succeeded to do it's first reorder of
*       entries in order to insert NA that first failed to be inserted but after the
*       re order succeeded
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoAuProcessingFirstReorderIndicationReset
(
    GT_VOID
)
{
    isFirstReorderDone = 0;
    return GT_OK;
}
