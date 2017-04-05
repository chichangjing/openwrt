/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrafficGenerator.c
*
* DESCRIPTION:
*     emulate traffic generator capabilities
*     reset counters
*     read counters
*     send traffic
*     capture received traffic
*     set trigger on received traffic
*
* FILE REVISION NUMBER:
*       $Revision: 143 $
*
*******************************************************************************/

#ifdef COW_INTERNAL_COMPILE
    /* those 3 are defined , so remove them */
    #undef EXMXPM_FAMILY
    #undef EX_FAMILY
    #undef CHX_FAMILY

    /* define only the tested one */
    #define CHX_FAMILY

    #undef IMPL_GALTIS
    #define IMPL_GALTIS
#endif /*COW_INTERNAL_COMPILE*/

/* for VC as usually used compiler - check mismatch of sizeof().  */
#ifdef _VISUALC
    #define CHECK_SIZE_MISMATCH
#endif


/* the define of UT_FIXED_PORT_TO_SUPPORT_U32_TYPE_MAC must come from C files that
   already fixed the types of ports from GT_U8 !

   NOTE: must come before ANY include to H files !!!!
*/
#define UT_FIXED_PORT_TO_SUPPORT_U32_TYPE_MAC

#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <extUtils/trafficEngine/tgfTrafficGenerator.h>
#include <extUtils/trafficEngine/tgfTrafficTable.h>
#include <extUtils/iterators/extUtilDevPort.h>
#include <gtOs/gtOsTimer.h>
#include <gtUtil/gtBmPool.h>
#include <extUtils/trafficEngine/prvTgfLog.h>
#include <extUtils/trafficEngine/private/prvTgfInternalDefs.h>

#if 0
#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfPortGen.h>
#include <common/tgfTrunkGen.h>
#include <common/tgfCscdGen.h>
#endif

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
    #include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
    #include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
    #include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
    #include <cpss/dxCh/dxChxGen/tcam/cpssDxChTcam.h>
    #include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>
    #include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
    #include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
    #include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>
    #include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
    #include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>

typedef GT_U32 (*APP_DEMO_DXCH_LION3_TCAM_PCL_CONVERTED_INDEX_GET_FUN)
(
    IN     GT_U8                                devNum,
    IN     GT_U32                               index,
    IN     CPSS_DXCH_TCAM_RULE_SIZE_ENT         ruleSize
);
APP_DEMO_DXCH_LION3_TCAM_PCL_CONVERTED_INDEX_GET_FUN appDemoDxChLion3TcamPclConvertedIndexGetPtr = NULL;
#define TGF_DXCH_LION3_TCAM_PCL_CONVRT_IDX_GET_MAC(devNum,index,ruleSize) \
    ((appDemoDxChLion3TcamPclConvertedIndexGetPtr == NULL) \
        ? 0 \
        : appDemoDxChLion3TcamPclConvertedIndexGetPtr(devNum,index,ruleSize))

#if 0
extern GT_STATUS appDemoDxChLion3NewPclTcamSupportSet
(
    IN  GT_BOOL             enable
);
#endif
#endif /* CHX_FAMILY */


#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortStat.h>
    #include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>
    #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>
    #include <cpss/exMxPm/exMxPmGen/pcl/cpssExMxPmPcl.h>
    #include <cpss/exMxPm/exMxPmGen/monitor/cpssExMxPmMirror.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgVlan.h>
    #include <cpss/exMxPm/exMxPmGen/trunk/cpssExMxPmTrunk.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgNestVlan.h>

#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    #include <cpss/exMx/exMxGen/port/cpssExMxPortStat.h>
    #include <cpss/exMx/exMxGen/port/cpssExMxPortCtrl.h>
    #include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>
    #include <cpss/exMx/exMxGen/pcl/cpssExMxPcl.h>
#endif /* EX_FAMILY */

#ifdef ASIC_SIMULATION
extern GT_U32 skernelNumOfPacketsInTheSystemGet(GT_VOID);

extern void skernelFatalError
(
    IN char * format, ...
);

extern GT_U32 simLogIsOpen(void);
extern GT_U32 skernelStatusGet(void);
#endif /*ASIC_SIMULATION*/

typedef GT_STATUS (*appDemoDbEntryGet_TYPE)
(
    IN  GT_CHAR *namePtr,
    OUT GT_U32  *valuePtr
);

appDemoDbEntryGet_TYPE appDemoDbEntryGet_func = NULL;

GT_U32 *tgfCmdCpssNetEnhUtUseCaptureToCpuPtr = NULL;

/* the GM not supports mapping of 'Target physical port' to 'MAC number'
   therefore any 'Physical port' with 'MAC number' > 71 is declined , so GM will
   not crash on MIB counters of such port
*/
#define BC2_GM_NOT_SUPPORT_TXQ_PORT_MAPPING_CNS     1

#define PRV_TGF_STC_S2D_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        (dstStcPtr)->field = (srcStcPtr)->field
#define PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        cpssOsMemCpy((&(dstStcPtr)->field),&((srcStcPtr)->field),sizeof((dstStcPtr)->field))
#define PRV_TGF_STC_D2S_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        (srcStcPtr)->field = (dstStcPtr)->field
#define PRV_TGF_STC_D2S_UNION_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(srcStcPtr,dstStcPtr,field)


/* flag to use contineouse FWS */
GT_U32 prvTgfFwsUse = 0;

/* indication for function tgfTrafficGeneratorPortTxEthCaptureSet(...)
       to allow loopback on the 'captured port' or not. */
static GT_BOOL txEthCaptureDoLoopback = GT_FALSE;/*GT_TRUE;*/

/* mark DB entry as free */
#define  PRV_TGF_TX_MODE_FREE_DB_ENTRY(_index) U32_SET_FIELD_MAC(txDevPortContUsedBmp,(_index), 1, 0)

/* mark DB entry as allocated */
#define  PRV_TGF_TX_MODE_ALLOC_DB_ENTRY(_index) U32_SET_FIELD_MAC(txDevPortContUsedBmp,(_index), 1, 1)

/* is DB not empty */
#define PRV_TRG_TX_MODE_DB_NOT_EMPTY (txDevPortContUsedBmp)

/* the DB */
static PRV_TGF_TX_DEV_PORT_STC txDevPortArr[PRV_TGF_TX_PORTS_MAX_NUM_CNS];
static GT_U32 txDevPortContUsedBmp = 0;

/* store mirroring mode and descriptors limits */
static CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT mirrModeStore;
static CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT      mirrPriorityMode;
static GT_U32 mirrRxDescStore;
static GT_U32 mirrTxDescStore;

#ifndef ASIC_SIMULATION
/* number of packets to get WS traffic. */
GT_U32 prvTgfWsBurst = 30;
/* flag to use VNT time stamps for rate calculation */
GT_U32 prvTgfCommonVntTimeStampsInUse = 1;
#else /*ASIC_SIMULATION*/
/* number of packets to get 'WS' traffic. */
GT_U32 prvTgfWsBurst = 1;
/*simulation not supports the feature*/
GT_U32 prvTgfCommonVntTimeStampsInUse = 0;
#endif/*ASIC_SIMULATION*/

/* debug flag to check rate by VNT Time Stamp */
GT_U32 prvTgfCommonVntTimeStampsDebug = 0;
/* check if the device supports prvTgfCommonVntTimeStampsInUse */
/* CH+ device not support it */
#define DEV_SUPPORT_VNT_TIME_STAMPS_MAC(devNum) \
    ((PRV_CPSS_PP_MAC(devNum)->appDevFamily & (CPSS_CH1_E | CPSS_CH1_DIAMOND_E)) ? 0:1)
/******************************************************************************\
 *                              Define section                                *
\******************************************************************************/
/* number of packets thae we set as max 'burst' when working with SDMA in 'abort' mode */
/* set this value , so traffic that go to the CPU will not be dropped do to lack
   of 'RX to CPU' resources

   the problem occurs to me only in linux simulation (and not in win32)


*/
#if (defined _WIN32)
    /* win 32 sim */
    #define SDMA_ABORT_MAX_CNS  32
#else
        #if (defined LINUX_SIM)
            /* linux sim */
            #define SDMA_ABORT_MAX_CNS  4 /* 16 was not got enough */
        #else
            /* vxWorks HW (not checked on linux HW) */
            #define SDMA_ABORT_MAX_CNS  16 /*was set to 16 because on Lion BOARD value 32 wan not good enough */
        #endif
#endif

#define MAX_RETRY_CNS   5

#if 0
/* the GM of Puma3 supports send of the 'from cpu' and the loopback
   traffic in the context of the sending task (this current task)
   so AFTER the cpss called for sending the packet , the GM already finished
   processing all the traffic.
   */
    #ifdef GM_USED
        #define SUPPORT_ALL_IN_SINGLE_CONTEXT_MAC   \
            ((PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->devFamily == CPSS_PP_FAMILY_PUMA3_E) ? 1 : 0)
    #else
        #define SUPPORT_ALL_IN_SINGLE_CONTEXT_MAC   0
    #endif
#else /*1*/
    /* although the puma3 GM supports mac loopback on the ports.
       there is problem on the fabric ports that set as 'mac internal loopback' .
       so the appdemo not set those ports with internal mac loopback but as 'external mac loopbac'
       (with the help of the simulation that covers the GM) */
    #define SUPPORT_ALL_IN_SINGLE_CONTEXT_MAC   0
#endif/*1*/

#define DEV_0_CNS    0
/* default device number */
GT_U8   prvTgfDevNum       = DEV_0_CNS;

/* flag where to configure loopback on port */
PRV_TGF_LOOPBACK_UNIT_ENT prvTgfLoopbackUnit = PRV_TGF_LOOPBACK_UNIT_MAC_E;


/* list of ports in devices that must be added to all vlans and MC (vidx) groups */
PRV_TGF_MEMBER_FORCE_INFO_STC  prvTgfDevPortForceArray[PRV_TGF_FORCE_MEMBERS_COUNT_CNS];
/* number of ports/devices in arrays of :prvTgfDevPortForceArray[] */
/* by default there are no members that need to forced to all vlans */
GT_U32    prvTgfDevPortForceNum = 0;


/* do we sent traffic to the CPU ? */
GT_U32 tgfTrafficGeneratorRxCaptureNum = 0;
/* indication that a test set traffic to be sent to the CPU , but not relate to 'capture' */
GT_BOOL tgfTrafficGeneratorExpectTraficToCpuEnabled = GT_FALSE;
/* number of packets that returned 'not GT_OK' on send */
GT_U32  prvTgfTrafficGeneratorNumberPacketsSendFailed = 0;
/* number of packets that returned 'GT_OK' on send */
GT_U32  prvTgfTrafficGeneratorNumberPacketsSendOk = 0;

/* enable print of TX Packet events in log file */
GT_BOOL prvTgfTrafficPrintPacketTxEnable = GT_TRUE;

/* the number of 'HW errors' that we got from trying to send packets.

    once we get those error in consecutive , there will be no recovery !

    we need to end the running ... to stop wasting time .
*/
static GT_U32   consecutiveHwErrors = 0;


/* default PCL Ingress Policy state */
static GT_BOOL  prvTgfDefIngressPolicyState = GT_FALSE;

static GT_U32   miiCounter = 10;
/* the device number that is connected to the CPU via it we transmit traffic
   to the 'loopback' port */
GT_U8   prvTgfCpuDevNum = 0;

/* default number of time loops to send packet */
#define TGF_DEFAULT_TIME_LOOP_NUM_CNS   5

/* number of time loops to send packet */
static GT_U32   prvTgfTimeLoop = TGF_DEFAULT_TIME_LOOP_NUM_CNS;

/* maximal time in 10 milliseconds for burst Tx.
   default is 300 milliseconds

    changed to 300 because : Lion2 in WIN32 simulation with ports 0..127 needed more then 100 milliseconds
*/
#ifdef  EMBEDDED
    /* run on hardware - two tick sleep is enough */
#define COUNTERS_POOLING_MAX_COUNT_CNS 2
#else
    /* run on simulation */
#define COUNTERS_POOLING_MAX_COUNT_CNS 300
#endif

GT_U32 prvTgfTrafficRxCheckMaxCount = COUNTERS_POOLING_MAX_COUNT_CNS;

/* maximal number of simulation idle state pooling in milliseconds */
#ifdef LINUX_SIM
/* minimal sleep time */
#define MINIMAL_SLEEP_CNS 40
/* mandatory sleep time */
#define MANDATORY_SLEEP_CNS 10
#else
/* minimal sleep time */
#define MINIMAL_SLEEP_CNS 10
/* mandatory sleep time */
#define MANDATORY_SLEEP_CNS 0
#endif

/* the Linux and Windows parallel UT run required minimum 5000 milliseconds */
#define IDLE_POOLING_MAX_COUNT_CNS 5000
/*
state that the test doing storming on the ingress port (ingress traffic
will be send back to ingress port that is in 'loopback mode').
this indication needed for WM to know to break the loop regardless to
indications from skernelStatusGet()
*/
static GT_BOOL  tgfStormingExpected = GT_FALSE;

/* default number for time factor for GM devices */
#define TGF_DEFAULT_GM_TIME_FACTOR_CNS   3
/* time factor for GM devices , due to very slow processing of packets (2 per second) */
/* for 'fine tuning' / debug use function tgfTrafficGeneratorGmTimeFactorSet(...) */
static GT_U32   prvTgfGmTimeFactor = TGF_DEFAULT_GM_TIME_FACTOR_CNS;/* used when GM_USED */

/* default number of loops to get captured packet */
#define TGF_DEFAULT_CAPTURE_LOOP_NUM_CNS 7

/* number of time loops to get captured packet */
GT_U32   prvTgfCaptureTimeLoop = TGF_DEFAULT_CAPTURE_LOOP_NUM_CNS;

/*flag to force link on huge sending*/
static GT_U32   debug_forcePrint = 0;

#ifdef GM_USED
/* do we wait for traffic that need to trap to cpu */
static GT_U32   retryRxToCpuAndWait = 1;
#else
/* do we wait for traffic that need to trap to cpu */
static GT_U32   retryRxToCpuAndWait = 0;
#endif


/* ports array */
GT_U32   prvTgfPortsArray[PRV_TGF_MAX_PORTS_NUM_CNS] =
{
    0, 8, 18, 23, /* 4 ports of 'default' tests */
    0, 0,  0,  0,
    0, 0,  0,  0,
    0, 0,  0,  0,
    0, 0,  0,  0
};


#define  DEV_CHECK_MAC(dev) if(!PRV_CPSS_IS_DEV_EXISTS_MAC(dev)) return GT_BAD_PARAM
#ifdef CHX_FAMILY
    #ifndef TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
        #define TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  sizeof(CPSS_DXCH_NET_RX_PARAMS_STC)
    #endif /* TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS */
    /* E ARCH supported by the device */
    #define TGF_CPSS_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(_devNum)            \
        (((_devNum) < PRV_CPSS_MAX_PP_DEVICES_CNS) ?                     \
           (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(_devNum)) :    \
           GT_FALSE)
    #define TGF_CPSS_PP_HW_INFO_E_ARCH_ENABLED_MAC(_devNum)              \
        (((_devNum) < PRV_CPSS_MAX_PP_DEVICES_CNS) ?                     \
           (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(_devNum)) :      \
           GT_FALSE)
#else
    #define TGF_CPSS_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(_devNum) \
        /* dummy check to allow 'macro' to optionally return 'GT_TRUE' so code that */ \
        /* uses it will never state 'statement not reached' */      \
        (((_devNum) == 255 ) ? GT_TRUE : GT_FALSE)
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #ifndef TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
        #define TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  sizeof(CPSS_EXMXPM_NET_RX_PARAMS_STC)
    #endif /* TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS */
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    #ifndef TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
        #define TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  sizeof(CPSS_EXMX_NET_RX_PARAMS_STC)
    #endif /* TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS */
#endif /* EX_FAMILY */

#ifndef TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS
    #define TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS  1
#endif /* TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS */

/*number of buffer at the pool*/
#define   TGF_MAX_NUM_OF_BUFFERS_AT_POOL    4

static GT_POOL_ID prvTgfTxBuffersPoolId = 0;

/* set PCL id for the capture */
static GT_U16   prvTgfTrafficCapturePclId = (BIT_7 - 1);/* use 7 bits to support ExMx */

/* PCL id that will represent the  capture */
#define   TGF_CAPTURE_PCL_ID_CNS (prvTgfTrafficCapturePclId)
#define   TGF_CAPTURE_PCL_ID_CNS_1 (prvTgfTrafficCapturePclId+1)
/* set index for the PCL to use for capture*/
static GT_U32   prvTgfTrafficCapturePclRuleIndex = 255;

/* rule index that will represent the capture */
#define TGF_PCL_RULE_INDEX_CNS (prvTgfTrafficCapturePclRuleIndex)
#define TGF_PCL_RULE_INDEX_CNS_1 (prvTgfTrafficCapturePclRuleIndex+1)
/* use this etherType to recognize 'Tagged packes' that send
  from the traffic generator to the PP */
static GT_U16 etherTypeForVlanTag = TGF_ETHERTYPE_8100_VLAN_TAG_CNS;

    #define MAX_MIRROR_PORTS_CNS    32

    /* PVID per port struct typedef */
    typedef struct
    {
        GT_BOOL         isValid;
        GT_U8           devNum;
        GT_PORT_NUM     portNum;
        GT_U16          pvid;
} PRV_TGF_PVID_PER_PORT_STC;

    /* global PVID DB per device, per port */
static PRV_TGF_PVID_PER_PORT_STC prvTgfGlobalPvidDatabaseArr[
    MAX_MIRROR_PORTS_CNS] = { {0} };

/* do we need to capture by PCL all packets , exclude those of specific vlan ? */
static GT_BOOL  pclCaptureExcludeVidEnabled = GT_FALSE;
/* the specific vlan to exclude from PCL capture .
  relevant only when  pclCaptureExcludeVidEnabled == GT_TRUE */
static GT_BOOL  pclCaptureExcludeVid = 0;


/******************************************************************************\
 *                              API implementation                            *
\******************************************************************************/
static GT_STATUS autoChecksumField
(
    IN TGF_AUTO_CHECKSUM_FULL_INFO_STC  *checksumInfoPtr,
    IN TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
);

#ifdef GM_USED
    /* convert CPSS devNum, coreId to simulation deviceId */
    extern GT_STATUS prvCpssDrvDevNumToSimDevIdConvert
    (
        IN  GT_U8       devNum,
        IN  GT_U32      portGroupId,
        OUT GT_U32      *simDeviceIdPtr
    );

    /* set the 'loopback force mode' on a port of device */
    extern GT_STATUS scibPortLoopbackForceModeSet
    (
        IN  GT_U32      deviceId,
        IN  GT_U32      portNum,
        IN  GT_U32      mode
    );

    /* set the 'link state' on a port of device */
    extern GT_STATUS scibPortLinkStateSet
    (
        IN  GT_U32      deviceId,
        IN  GT_U32      portNum,
        IN GT_BOOL      linkState
    );
#endif /* GM_USED */

/*******************************************************************************
* internalIsNumInList
*
* DESCRIPTION:
*       Check is number in list
*
* INPUTS:
*       checkNum     - number for check
*       numListArr[] - array of numbers (can be NULL)
*       numListLen   - members number in numListArr[] (can be 0)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_TRUE      - num was found in numListArr.
*       GT_FALSE     - in otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL internalIsNumInList
(
    IN GT_U32   checkNum,
    IN GT_U32   numListArr[],
    IN GT_U32   numListLen
)
{
    GT_U32      i = 0;

    if ((numListLen == 0) || (NULL == numListArr))
    {
        return GT_FALSE;
    }

    for (i=0; i<numListLen; i++)
    {
        if (numListArr[i] == checkNum)
        {
            return GT_TRUE;
        }
    }
    return GT_FALSE;
}

/*******************************************************************************
* tgfTrafficGeneratorPacketChecksumUpdate
*
* DESCRIPTION:
*       Update checksums in packet built by tgfTrafficEnginePacketBuild
*
* INPUTS:
*       checksumInfoArr -       (array of) info about instance of fields that need auto checksum build
*       checksumExtraInfoPtr -  (array of) extra info for auto checksum build
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*       Input parameters are arrays returened by tgfTrafficEnginePacketBuild.
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPacketChecksumUpdate
(
    OUT TGF_AUTO_CHECKSUM_FULL_INFO_STC *checksumInfoPtr,
    OUT TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
)
{
    return autoChecksumField(checksumInfoPtr, checksumExtraInfoPtr);
}

/*******************************************************************************
* tgfTrafficGeneratorCpuDeviceSet
*
* DESCRIPTION:
*       Set the CPU device though it we send traffic to the 'Loop back port'
*
* INPUTS:
*       cpuDevice - the CPU device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorCpuDeviceSet
(
    IN GT_U8    cpuDevice
)
{
    if (!PRV_CPSS_IS_DEV_EXISTS_MAC(cpuDevice))
    {
        return GT_BAD_PARAM;
    }

    prvTgfCpuDevNum = cpuDevice;

    /* avoid compiler warnings */
    TGF_PARAM_NOT_USED(prvTgfTrafficCapturePclId);
    TGF_PARAM_NOT_USED(prvTgfTrafficCapturePclRuleIndex);

    return GT_OK;
}

#ifdef GM_USED
/*******************************************************************************
* simulateExternalLoopbackPortSet
*
* DESCRIPTION:
*       WA for GM devices that do not know to 'loopback'
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*       enable           - enable / disable (loopback/no loopback)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS simulateExternalLoopbackPortSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL                   enable
)
{
    /* WA for GM devices that do not know to 'loopback' */
    GT_STATUS   rc;
    GT_U32  portGroupId;
    GT_U32  localPortNum;
    GT_U32  simDeviceId;
    GT_U32 portMacNum;      /* MAC number */

#ifdef CHX_FAMILY
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        #if BC2_GM_NOT_SUPPORT_TXQ_PORT_MAPPING_CNS == 1
        if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
        {
            if(portNum >= PRV_CPSS_PP_MAC(devNum)->numOfPorts)
            {
                /* the GM of BC2,Bobk not access the 'MIB' of the port according
                   to phy port and not according to 'MAC port' , so allowing such
                   high port into the configuration CRASH the GM ! */
                return GT_FAIL;
            }
        }
        #endif
    }
#else
    portMacNum = portNum;
#endif
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);
    if((portGroupId == 2 || portGroupId == 3) &&
       PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_PUMA3_E)
    {
        /* the Puma3 device has 4 pipes that handle packets but there
           are only 2 MG units . so we have only devObjPtr->portGroupId = 0 and 1 */
        /* but still we need to get from the GM packet need to be sent to
            pipe 0 or pipe 1 or pipe 2 or pipe 3 with local port 0..11 */

        portGroupId  -=  2;
        localPortNum += 32;
    }

    rc = prvCpssDrvDevNumToSimDevIdConvert(devNum,portGroupId,&simDeviceId);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = scibPortLoopbackForceModeSet(simDeviceId,localPortNum,enable);
    return rc;
}
/*******************************************************************************
* simulatePortForceLinkUpSet
*
* DESCRIPTION:
*       WA for GM devices that not force link up according to 'mac control'
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*       enable  - enable / disable (force 'link up')
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS simulatePortForceLinkUpSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL                   enable
)
{
    /* WA for GM devices that do not know to 'force link UP' */
    GT_STATUS   rc;
    GT_U32  portGroupId;
    GT_U32  localPortNum;
    GT_U32  simDeviceId;
    GT_U32 portMacNum;      /* MAC number */
    GT_BOOL linkUp;

#ifdef CHX_FAMILY
    PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(devNum, portNum, portMacNum);
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        #if BC2_GM_NOT_SUPPORT_TXQ_PORT_MAPPING_CNS == 1
        if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
        {
            if(portNum >= PRV_CPSS_PP_MAC(devNum)->numOfPorts)
            {
                /* the GM of BC2,Bobk not access the 'MIB' of the port according
                   to phy port and not according to 'MAC port' , so allowing such
                   high port into the configuration CRASH the GM ! */
                return GT_FAIL;
            }
        }
        #endif
    }
#else
    portMacNum = portNum;
#endif
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portMacNum);
    localPortNum = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portMacNum);
    if((portGroupId == 2 || portGroupId == 3) &&
       PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_PUMA3_E)
    {
        /* the Puma3 device has 4 pipes that handle packets but there
           are only 2 MG units . so we have only devObjPtr->portGroupId = 0 and 1 */
        /* but still we need to get from the GM packet need to be sent to
            pipe 0 or pipe 1 or pipe 2 or pipe 3 with local port 0..11 */

        portGroupId  -=  2;
        localPortNum += 32;
    }


    rc = prvCpssDrvDevNumToSimDevIdConvert(devNum,portGroupId,&simDeviceId);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* get link status */
    rc = cpssDxChPortLinkStatusGet(devNum, portNum, &linkUp);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = scibPortLinkStateSet(simDeviceId,localPortNum,enable);

    if(linkUp != enable)
    {
        cpssOsTimerWkAfter(300);/* allow the port fully 'get UP' before we send traffic to it */
    }

    return rc;
}

#endif /*GM_USED*/

#ifdef CHX_FAMILY

/*                                                "
NOTE: the emulation of DB and functions that start with prefix : lion3Emulate_
are COPIED TO : <galtisAgent\wrapCpss\dxCh\bridge\wrapCpssDxChBridgeVlan.c> --> 'Galtis wrapper' --> named wrLion3Emulate_
to remove the dependencies between the 2 libraries.
*/

static GT_U32 lion3Emulate_tpidBmpTable[2][8] = {{0},{0}};
static GT_U32 lion3Emulate_tpidBmpTable_used[2][8] = {{0},{0}};


GT_STATUS lion3Emulate_IngressTpidProfileClear
(
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               profileIndex
)
{
    GT_U32  tag = (ethMode ==  CPSS_VLAN_ETHERTYPE0_E) ? 0 : 1;

    if(profileIndex >= 8)
    {
        return GT_BAD_PARAM;
    }

    lion3Emulate_tpidBmpTable_used[tag][profileIndex] = 0;
    lion3Emulate_tpidBmpTable[tag][profileIndex] = 0xFFFFFFFF;

    return GT_OK;
}

GT_STATUS lion3Emulate_IngressTpidProfileGet
(
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp,
    OUT GT_U32               *profilePtr
)
{
    GT_U32  ii;
    GT_U32  iiFirstFree = 0xFF;
    GT_U32  tag = (ethMode ==  CPSS_VLAN_ETHERTYPE0_E) ? 0 : 1;

    /* look for the tpidBmp in the table */
    for(ii = 0 ; ii < 8; ii++)
    {
        if(lion3Emulate_tpidBmpTable_used[tag][ii] == 0)
        {
            if(iiFirstFree == 0xFF)
            {
                iiFirstFree = ii;
            }

            continue;
        }

        if(lion3Emulate_tpidBmpTable[tag][ii] == tpidBmp)
        {
            break;
        }
    }

    if(ii == 8) /* not found in the table */
    {
        if(iiFirstFree == 0xFF)/*not found empty entry*/
        {
            return GT_FULL;
        }

        ii = iiFirstFree;
    }

    if(ii >= 8)
    {
        return GT_BAD_STATE;
    }

    *profilePtr = ii;

    lion3Emulate_tpidBmpTable_used[tag][ii] = 1;
    lion3Emulate_tpidBmpTable[tag][ii] = tpidBmp;

    return GT_OK;
}

static GT_STATUS lion3Emulate_cpssDxChBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U32                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
)
{
    GT_STATUS   rc;
    GT_U32  profile;

    if(TGF_CPSS_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* look for the tpidBmp in the table */
    rc = lion3Emulate_IngressTpidProfileGet(ethMode,tpidBmp,&profile);
    if(rc != GT_OK)
    {
       return rc;
    }

    rc = cpssDxChBrgVlanIngressTpidProfileSet(devNum,profile,ethMode,tpidBmp);
    if(rc != GT_OK)
    {
       return rc;
    }

    /* write to pre-tti-lookup-ingress-eport table */
    rc = cpssDxChBrgVlanPortIngressTpidProfileSet(devNum,
                portNum,
                ethMode,
                GT_TRUE,
                profile);
    if(rc != GT_OK)
    {
       return rc;
    }

   /* write to post-tti-lookup-ingress-eport table */
    rc = cpssDxChBrgVlanPortIngressTpidProfileSet(devNum,
                portNum,
                ethMode,
                GT_FALSE,
                profile);

    return rc;
}

static GT_STATUS lion3Emulate_cpssDxChBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U32                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidBmpPtr
)
{
    GT_STATUS   rc;
    GT_U32  profile;

    if(TGF_CPSS_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* read from pre-tti-lookup-ingress-eport table */
    rc = cpssDxChBrgVlanPortIngressTpidProfileGet(devNum,
                portNum,
                ethMode,
                GT_TRUE,/* default value */
                &profile);
    if(rc != GT_OK)
    {
       return rc;
    }


    rc = cpssDxChBrgVlanIngressTpidProfileGet(devNum,profile,ethMode,tpidBmpPtr);

    return rc;
}
#endif /*CHX_FAMILY*/


/*******************************************************************************
* prvTgfBrgVlanPortIngressTpidSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*       tpidBmp    - bitmap represent entries in the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U32                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc;
    GT_U32  ii;
    PRV_TGF_MEMBER_FORCE_INFO_STC *currMemPtr;

    PRV_TGF_LOG4_MAC("prvTgfBrgVlanPortIngressTpidSet(%d,%d,%d,%d) \n",
        devNum,portNum,ethMode,tpidBmp);


    /* add the classification for all cascade/ring ports , with full bmp of ethertype 0,1 */

    /* look for this member in the DB */
    currMemPtr = &prvTgfDevPortForceArray[0];
    for(ii = 0 ; ii < prvTgfDevPortForceNum; ii++, currMemPtr++)
    {
        if(devNum != currMemPtr->member.devNum)
        {
            continue;
        }

        if(currMemPtr->forceToVlan == GT_FALSE)
        {
            /* member not need to be forced to any vlan */
            continue;
        }

        if(currMemPtr->member.portNum >= CPSS_MAX_PORTS_NUM_CNS)
        {
            return GT_BAD_PARAM;
        }

        /* set the member with full bmp for the ethertype */
        rc = cpssDxChBrgVlanPortIngressTpidSet(currMemPtr->member.devNum,
                        currMemPtr->member.portNum,
                        ethMode, 0x000000FF);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(TGF_CPSS_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* emulate for Lion3*/
        return lion3Emulate_cpssDxChBrgVlanPortIngressTpidSet(devNum, portNum, ethMode, tpidBmp);
    }


    /* call device specific API */
    return cpssDxChBrgVlanPortIngressTpidSet(devNum, portNum, ethMode, tpidBmp);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY

    CPSS_ETHER_MODE_ENT     ingressVlanSel;

    /* avoid warnings */
    TGF_PARAM_NOT_USED(ethMode);

    /* configure VLAN ethertype */
    ingressVlanSel = 0x1 == tpidBmp ? CPSS_VLAN_ETHERTYPE0_E : CPSS_VLAN_ETHERTYPE1_E;

    /* call device specific API */
    return cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(devNum, portNum, ingressVlanSel);
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanPortIngressTpidGet
*
* DESCRIPTION:
*       Function gets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidBmpPtr - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_BAD_PTR               - tpidBmpPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U32                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32              *tpidBmpPtr
)
{
#ifdef CHX_FAMILY
    if(TGF_CPSS_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        /* emulate for Lion3*/
        return lion3Emulate_cpssDxChBrgVlanPortIngressTpidGet(devNum, portNum, ethMode, tpidBmpPtr);
    }
    /* call device specific API */
    return cpssDxChBrgVlanPortIngressTpidGet(devNum, portNum, ethMode, tpidBmpPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY

    GT_STATUS                rc = GT_OK;
    CPSS_ETHER_MODE_ENT      ingressVlanSel;

    /* avoid warnings */
    TGF_PARAM_NOT_USED(ethMode);

    /* call device specific API */
    rc = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(devNum, portNum, &ingressVlanSel);

    *tpidBmpPtr = CPSS_VLAN_ETHERTYPE0_E == ingressVlanSel ? 0x1 : 0x2;

    return rc;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* tgfTrafficGeneratorPortLoopbackModeEnableSet
*
* DESCRIPTION:
*       Set port in 'loopback' mode
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (loopback/no loopback)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortLoopbackModeEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                   enable
)
{
    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    TGF_PARAM_NOT_USED(enable);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    {
        GT_STATUS   rc;
        GT_U32      tpidBmp = (enable == GT_TRUE) ? 0x3f : 0xff;
        GT_U8       devNum;

        devNum = (GT_U8)portInterfacePtr->devPort.hwDevNum;

#ifdef GM_USED
        /* for GM - BOBCAT2 - external loopback is supported by simulation help
           since there is no GOP unit in GM */
        if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            /* WA for the GM devices that not support internal mac loopback port.     */
            simulateExternalLoopbackPortSet(devNum,
                                        portInterfacePtr->devPort.portNum,
                                        enable);
            return GT_OK;
        }
#endif
        /* Set ingress TPID select for TAG0 --> set to use only 6 TPID,
           to not recognize TPIDs that used only for egress between cascade/ring ports.
           this setting needed when :
           1. we capture traffic to the CPU.
           2. the port send traffic 'in loopback' and at the ingress it should
                not be recognize ethertypes that relate to 'egress'
        */

         /* need to fix test to fit to portNum of type GT_PORT_NUM and devNum
               of type GT_HW_DEV_NUM
               Need to fix call to prvTgfCommonIsDeviceForce that gets GT_U8 for devNum
               instead of GT_HW_DEV_NUM
               Need to remove all casting when accomodating test to lion2*/
            CPSS_TBD_BOOKMARK_EARCH

        if(GT_TRUE == prvTgfCommonIsDeviceForce(devNum))
        {
            rc = prvTgfBrgVlanPortIngressTpidSet(devNum,
                                                 portInterfacePtr->devPort.portNum,
                                                 CPSS_VLAN_ETHERTYPE0_E,
                                                 tpidBmp);
            if(rc != GT_OK)
            {
                return rc;
            }

            rc = prvTgfBrgVlanPortIngressTpidSet(devNum,
                                                 portInterfacePtr->devPort.portNum,
                                                 CPSS_VLAN_ETHERTYPE1_E,
                                                 tpidBmp);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        switch(prvTgfLoopbackUnit)
        {
            case PRV_TGF_LOOPBACK_UNIT_MAC_E:
                /* Lion2 and Hooper has erratum FE-2102944 - The unwanted packet
                   is received and transmitted by the device after MAC reset.
                   cpssDxChPortInternalLoopbackEnableSet use MAC reset. So tests
                   should use cpssDxChPortPcsLoopbackModeSet to avoid redundand packets. */
                if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
                {
                    return cpssDxChPortPcsLoopbackModeSet(devNum,
                                                           portInterfacePtr->devPort.portNum,
                                                           enable ? CPSS_DXCH_PORT_PCS_LOOPBACK_TX2RX_E :
                                                                    CPSS_DXCH_PORT_PCS_LOOPBACK_DISABLE_E);
                }
                else
                {
                    return cpssDxChPortInternalLoopbackEnableSet(devNum,
                                                                 portInterfacePtr->devPort.portNum,
                                                                 enable);
                }

            case PRV_TGF_LOOPBACK_UNIT_SERDES_E:
                return cpssDxChPortSerdesLoopbackModeSet(devNum,
                                                         portInterfacePtr->devPort.portNum,
                                                         0xFF, /* exact bitmap not important here */
                                                         CPSS_DXCH_PORT_SERDES_LOOPBACK_ANALOG_TX2RX_E);
            default:
                return GT_NOT_SUPPORTED;
        }
    }
#else

    /* for now other families support just loopback in MAC */
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[unreachable] */
    if(prvTgfLoopbackUnit != PRV_TGF_LOOPBACK_UNIT_MAC_E)
        return GT_NOT_SUPPORTED;
#endif
#ifdef EXMXPM_FAMILY
#ifdef GM_USED
    /* WA for the GM devices that not support internal mac loopback port.

       we do it anyway also for the Puma3 GM that support MAC internal loopback port.
       (currently the Puma3 GM must not be set to internal mac loopback on fabric port ,
       because this damage the 'upstream' ability to duplicate descriptors (like for Rx Mirroring) )
    */
    simulateExternalLoopbackPortSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                    portInterfacePtr->devPort.portNum,
                                    enable);

    if(PRV_CPSS_PP_MAC(portInterfacePtr->devPort.hwDevNum)->devFamily == CPSS_PP_FAMILY_PUMA3_E)
    {
        /* the GM bug cause setting loopback of port 0 to impact also port 32 (fabric port) */
        /* the GM bug cause packets to GM buffer manager errors that may cause 'old'
           packets to be treated when 'new' packets arrive */
        return GT_OK;
    }

#endif /*GM_USED*/

    return cpssExMxPmPortInternalLoopbackEnableSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                                   (GT_U8)portInterfacePtr->devPort.portNum,
                                                   enable);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    return cpssExMxPortInternalLoopbackEnableSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                                 (GT_U8)portInterfacePtr->devPort.portNum,
                                                 enable);
#endif /* EX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}


/*******************************************************************************
* prvTgfCommonMemberForceInfoSet
*
* DESCRIPTION:
*       add member info to DB of 'members to force to vlan/mc groups'
*       in case that the member already exists {dev,port} the other parameters
*       are 'updated' according to last setting.
*
* INPUTS:
*       memberInfoPtr - (pointer to) member info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FULL - the DB is full and no more members can be added -->
*                 need to make DB larger.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfCommonMemberForceInfoSet(
    IN PRV_TGF_MEMBER_FORCE_INFO_STC    *memberInfoPtr
)
{
    GT_U32  ii;
    PRV_TGF_MEMBER_FORCE_INFO_STC *currMemPtr;

    /* look for this member in the DB */
    currMemPtr = &prvTgfDevPortForceArray[0];
    for(ii = 0 ; ii < prvTgfDevPortForceNum; ii++,currMemPtr++)
    {
        if(memberInfoPtr->member.devNum == currMemPtr->member.devNum &&
           memberInfoPtr->member.portNum == currMemPtr->member.portNum )
        {
            break;
        }
    }

    if(ii == prvTgfDevPortForceNum)
    {
        /* new member */

        if(prvTgfDevPortForceNum >= PRV_TGF_FORCE_MEMBERS_COUNT_CNS)
        {
            /* the table already full , can't add new member */
            return GT_FULL;
        }

        prvTgfDevPortForceNum++;
    }

    /* copy the member into the DB */
    *currMemPtr = *memberInfoPtr;


    return GT_OK;
}

/*******************************************************************************
* prvTgfCommonIsDeviceForce
*
* DESCRIPTION:
*       check if the device has ports that used with force configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE  - the device force configuration
*       GT_FALSE - the device NOT force configuration
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvTgfCommonIsDeviceForce(
    IN GT_U8    devNum
)
{
    GT_U32  ii;
    PRV_TGF_MEMBER_FORCE_INFO_STC *currMemPtr;

    /* look for this member in the DB */
    currMemPtr = &prvTgfDevPortForceArray[0];
    for(ii = 0 ; ii < prvTgfDevPortForceNum; ii++,currMemPtr++)
    {
        if(devNum == currMemPtr->member.devNum)
        {
            return GT_TRUE;
        }
    }

    return GT_FALSE;
}

/*******************************************************************************
* tgfTrafficGeneratorPortForceLinkUpEnableSet
*
* DESCRIPTION:
*       Set port in 'force linkup' mode
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       enable           - enable / disable (force/not force)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortForceLinkUpEnableSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN GT_BOOL                   enable
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc;
    GT_BOOL linkUp = GT_FALSE;
    CPSS_PORT_SPEED_ENT  forceSpeed,newSpeed;
    GT_U8                   devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    CPSS_PORT_INTERFACE_MODE_ENT ifMode,newIfMode;
#endif

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    TGF_PARAM_NOT_USED(enable);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* need to remove casting and to check code after lion2 development is done */
    CPSS_TBD_BOOKMARK_EARCH

    devNum = (GT_U8)portInterfacePtr->devPort.hwDevNum;
    portNum = (GT_PHYSICAL_PORT_NUM)portInterfacePtr->devPort.portNum;
    if(enable == GT_TRUE)
    {
        if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
        {
            /* currently it seems that the BC2 not need any extra config beside the 'force link up' */
#ifdef GM_USED
            /* the GM not hold logic to set linkup from the 'MAC register' so we explicitly advice it */
            simulatePortForceLinkUpSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                       portInterfacePtr->devPort.portNum,
                                        enable);
#endif
        }
        else
        {
            /* set MAC in NORMAL state (not in RESET). Bobcat2 does need this
               configuration because it's already done during init process. */
            rc = cpssDxChPortMacResetStateSet(devNum, portNum, GT_FALSE);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* get link status */
        rc = cpssDxChPortLinkStatusGet(devNum, portNum, &linkUp);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(linkUp == GT_FALSE)
        {
            /* link is down. Set port speed to be maximal
               according to interface mode.
               there is no 'other side' that the port can be 'auto negotiation'
               with. The speed and duplex should be forced too. */
            if(enable == GT_TRUE)
            {
                /* get interface mode */
                rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* do not change speed for other then in following switch case
                  interfaces.  port is already configured for proper speed. */
                forceSpeed = CPSS_PORT_SPEED_NA_E;

                switch (ifMode)
                {
                    case CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E:
                    case CPSS_PORT_INTERFACE_MODE_MII_E:
                    case CPSS_PORT_INTERFACE_MODE_SGMII_E:
                    case CPSS_PORT_INTERFACE_MODE_GMII_E:
                    case CPSS_PORT_INTERFACE_MODE_MII_PHY_E:
                    case CPSS_PORT_INTERFACE_MODE_100BASE_FX_E:
                    case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
                            /* force speed to be common acceptable 100M */
                            forceSpeed = CPSS_PORT_SPEED_100_E;
                        break;
                    case CPSS_PORT_INTERFACE_MODE_XGMII_E:
                        /* NOTE: this section was added to support board of xcat-FE , with 4 XG ports.
                            the trunk tests uses port 27 , and without next configuration the
                            BM (buffer management) in the device was stuck */
                        if (0 == PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                        {
                            rc = cpssDxChPortInterfaceModeSet(devNum, portNum,ifMode);
                            if(rc != GT_OK)
                            {
                                PRV_TGF_LOG2_MAC("tgfTrafficGeneratorPortForceLinkUpEnableSet:\
                                                 FAIL : cpssDxChPortInterfaceModeSet port[%d],ifMode[%d]\n",
                                                 portNum, ifMode);
                                return rc;
                            }
                            /* set speed to be 10G */
                            rc = cpssDxChPortSpeedSet(devNum, portNum, CPSS_PORT_SPEED_10000_E);
                            if(rc != GT_OK)
                            {
                                PRV_TGF_LOG1_MAC("FAIL : cpssDxChPortSpeedSet port[%d]\n",
                                                 portNum);
                                return rc;
                            }
                            /* SERDES, XPCS, MAC configuration */
                            rc = cpssDxChPortSerdesPowerStatusSet(devNum, portNum,
                                                                  CPSS_PORT_DIRECTION_BOTH_E,
                                                                  0xF, GT_TRUE);
                            if(rc != GT_OK)
                            {
                                PRV_TGF_LOG1_MAC("FAIL : cpssDxChPortSerdesPowerStatusSet port[%d]\n",
                                                 portNum);
                                return rc;
                            }
                        }
                        else
                        {
                            /* the device with such interface mode can keep it with out change */
                        }

                        break;

                    case CPSS_PORT_INTERFACE_MODE_NA_E:

                        newIfMode = CPSS_PORT_INTERFACE_MODE_NA_E;/* 'non init' value */
                        newSpeed  = CPSS_PORT_SPEED_10000_E;

                        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E ||
                           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
                        {
                            if (portNum & 1)
                            {
                                /* on Lion/Lion2 only odd ports supposed to be used as loopback */
                                newIfMode = CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E;
                                newSpeed  = CPSS_PORT_SPEED_10000_E;
                            }
                        }
                        else
                        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E ||
                           PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E)
                        {
                            /* all ports should support 10G */
                            newIfMode = CPSS_PORT_INTERFACE_MODE_KR_E;
                            newSpeed  = CPSS_PORT_SPEED_10000_E;
                        }
                        else
                        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
                        {
#ifndef ASIC_SIMULATION
                            /* ensure at least two ports in 10G mode.
                               Ports prvTgfPortsArray[3] and 50 are used in NetIf WS test */
                            if ((portNum == prvTgfPortsArray[3]) || (portNum == 80) || (portNum == 50) || (portNum == 58))
                            {
                                /* port #3 should be at least 10G */
                                newIfMode = CPSS_PORT_INTERFACE_MODE_KR_E;
                                newSpeed  = CPSS_PORT_SPEED_10000_E;
                            }
                            else
#endif
                            {
                                /* all ports should support SGMII Giga mode .. so try to set it */
                                newIfMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;
                                newSpeed  = CPSS_PORT_SPEED_1000_E;

                                switch(PRV_CPSS_PP_MAC(devNum)->devType)
                                {
                                    case CPSS_BOBK_CETUS_DEVICES_CASES_MAC:
                                        /* Cetus use 1G ports. */
                                        forceSpeed = CPSS_PORT_SPEED_1000_E; break;
                                    default:
                                        break;
                                }
                            }
                        }

                        if (newIfMode != CPSS_PORT_INTERFACE_MODE_NA_E)
                        {
                            CPSS_PORTS_BMP_STC portsBmp;

                            CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
                            CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp, portNum);
                            rc = cpssDxChPortModeSpeedSet(devNum, portsBmp, GT_TRUE,
                                                          newIfMode,
                                                          newSpeed);
                            if(rc != GT_OK)
                            {
                                PRV_TGF_LOG3_MAC("tgfTrafficGeneratorPortForceLinkUpEnableSet FAIL : cpssDxChPortModeSpeedSet port[%d],newIfMode[%d],newSpeed[%d]\n",
                                                 portNum, newIfMode, newSpeed);
                                return rc;
                            }
                        }
                        break;
                    default:
                        break;
                }

                if (forceSpeed != CPSS_PORT_SPEED_NA_E)
                {
                    /* set port speed */
                    rc = cpssDxChPortSpeedSet(devNum, portNum, forceSpeed);
                    if(rc != GT_OK)
                    {
                        PRV_TGF_LOG1_MAC("FAIL : cpssDxChPortSpeedSet - port[%d]\n",
                                         portNum);
                        return rc;
                    }

                    /* set the port to not be 'auto negotiation' */
                    rc = cpssDxChPortSpeedAutoNegEnableSet(devNum, portNum, GT_FALSE);
                    if(rc != GT_OK)
                    {
                        PRV_TGF_LOG1_MAC("FAIL : cpssDxChPortSpeedAutoNegEnableSet - port[%d]\n",
                                         portNum);
                        return rc;
                    }

                    /* set the port to not be 'auto negotiation' */
                    rc = cpssDxChPortDuplexAutoNegEnableSet(devNum, portNum, GT_FALSE);
                    if(rc != GT_OK)
                    {
                        PRV_TGF_LOG1_MAC("FAIL : cpssDxChPortDuplexAutoNegEnableSet - port[%d]\n",
                                         portNum);
                        return rc;
                    }

                    /* set port to be full duplex */
                    rc = cpssDxChPortDuplexModeSet(devNum, portNum, CPSS_PORT_FULL_DUPLEX_E);
                    if(rc != GT_OK)
                    {
                        PRV_TGF_LOG1_MAC("FAIL : cpssDxChPortDuplexModeSet - port[%d]\n",
                                         portNum);
                        return rc;
                    }
                }
            }
        }
    } /*if force link up enabled */

    rc = cpssDxChPortForceLinkPassEnableSet(devNum, portNum, enable);
#ifdef  ASIC_SIMULATION
    if(enable == GT_TRUE && linkUp == GT_FALSE)
    {
        cpssOsTimerWkAfter(10);/* allow the port fully 'get UP' before we send traffic to it */
    }
#endif /*ASIC_SIMULATION*/
    return rc;


#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #ifdef GM_USED
    if(PRV_CPSS_PP_MAC(portInterfacePtr->devPort.hwDevNum)->devFamily == CPSS_PP_FAMILY_PUMA3_E)
    {
        /* the GM should support force link UP via the 'control register'
           so no need to call this simulation API */
    }
    else
    {
        simulatePortForceLinkUpSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                    portInterfacePtr->devPort.portNum,
                                    enable);
    }
    #else
    if(PRV_CPSS_PP_MAC(portInterfacePtr->devPort.hwDevNum)->devFamily >= CPSS_PP_FAMILY_PUMA3_E)
    {
        GT_U8 portNum;
        GT_U8 devNum;
        CPSS_PORT_INTERFACE_MODE_ENT   ifMode, port0IfMode;
        CPSS_PORT_SPEED_ENT            port0Speed;
        CPSS_PORTS_BMP_STC             portsBmp;
        GT_STATUS                      rc;
        GT_BOOL                        linkUp;

        /* check port interface and configure it to be 10 KR if not configured yet */
        if(enable == GT_TRUE)
        {
            devNum = (GT_U8)portInterfacePtr->devPort.hwDevNum;
            portNum = (GT_U8)portInterfacePtr->devPort.portNum;

            /* get link status */
            rc = cpssExMxPmPortLinkStatusGet(devNum, portNum, &linkUp);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(linkUp == GT_FALSE)
            {
                /* link is down. Check port's mode and if mode is not set then
                   set port mode to be like port 0. */

                /* get interface mode */
                rc = cpssExMxPmPortInterfaceModeGet(devNum, portNum, &ifMode);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* set port configuration like Port0 */
                if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
                {
                    /* get mode of port#0 */
                    rc = cpssExMxPmPortInterfaceModeGet(devNum, 0, &port0IfMode);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    /* get speed of port#0 */
                    rc = cpssExMxPmPortSpeedGet(devNum, 0, &port0Speed);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp, portNum);
                    /* set mode and power UP */
                    rc = cpssExMxPmPortModeSpeedSet(devNum, portsBmp, GT_TRUE,
                                                    port0IfMode, port0Speed);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }
        }
    }
    #endif /*GM_USED*/

    return cpssExMxPmPortForceLinkUpSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                        (GT_U8)portInterfacePtr->devPort.portNum,
                                        enable);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    return cpssExMxPortForceLinkPassEnable((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                           (GT_U8)portInterfacePtr->devPort.portNum,
                                           enable);
#endif /* EX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

/*******************************************************************************
* tgfTrafficGeneratorPortCountersEthReset
*
* DESCRIPTION:
*       Reset the traffic generator counters on the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortCountersEthReset
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr
)
{
    CPSS_PORT_MAC_COUNTER_SET_STC   dummyCounters;

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    /* avoid compiler warning */
    dummyCounters.goodOctetsRcv.c[0] = 0;

#ifdef CHX_FAMILY
    return cpssDxChPortMacCountersOnPortGet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                            portInterfacePtr->devPort.portNum,
                                            &dummyCounters);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /*cpssExMxPmPortCapturedMacCountersGet*/
    return  cpssExMxPmPortAllMacCountersGet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                            (GT_U8)portInterfacePtr->devPort.portNum,
                                                 &dummyCounters);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    return cpssExMxPortMacCountersOnPortGet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                            (GT_U8)portInterfacePtr->devPort.portNum,
                                            &dummyCounters);
#endif /* EX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

/*******************************************************************************
* tgfTrafficGeneratorPortCountersEthRead
*
* DESCRIPTION:
*       Read the traffic generator counters on the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*
* OUTPUTS:
*       countersPtr - (pointer to) the counters of port
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortCountersEthRead
(
    IN  CPSS_INTERFACE_INFO_STC        *portInterfacePtr,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC  *countersPtr
)
{
    GT_STATUS   rc = GT_OK;

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(countersPtr);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    rc = cpssDxChPortMacCountersOnPortGet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                          portInterfacePtr->devPort.portNum,
                                          countersPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /*cpssExMxPmPortCapturedMacCountersGet*/
    rc = cpssExMxPmPortAllMacCountersGet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                         (GT_U8)portInterfacePtr->devPort.portNum,
                                              countersPtr);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    rc = cpssExMxPortMacCountersOnPortGet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                          (GT_U8)portInterfacePtr->devPort.portNum,
                                          countersPtr);
#endif /* EX_FAMILY */

    if(countersPtr->goodPktsRcv.l[0] == 0)
    {
        countersPtr->goodPktsRcv.l[0] = countersPtr->ucPktsRcv.l[0] + countersPtr->mcPktsRcv.l[0] + countersPtr->brdcPktsRcv.l[0];
        countersPtr->goodPktsRcv.l[1] = countersPtr->ucPktsRcv.l[1] + countersPtr->mcPktsRcv.l[1] + countersPtr->brdcPktsRcv.l[1];
    }

    if(countersPtr->goodPktsSent.l[0] == 0)
    {
        countersPtr->goodPktsSent.l[0] = countersPtr->ucPktsSent.l[0] + countersPtr->mcPktsSent.l[0] + countersPtr->brdcPktsSent.l[0];
        countersPtr->goodPktsSent.l[1] = countersPtr->ucPktsSent.l[1] + countersPtr->mcPktsSent.l[1] + countersPtr->brdcPktsSent.l[1];
    }

    return rc;
}

GT_U32      trgEPortInsteadPhyPort = 0;
GT_BOOL     use_trgEPortInsteadPhyPort = GT_FALSE;

/*******************************************************************************
* tgfStateTrgEPortInsteadPhyPort
*
* DESCRIPTION:
*       Option to state that the cpu will send packets to trgEport that is not
*       the 'physical port' (portNum) that used by prvLuaTgfTransmitPackets(..portNum..)
*       prvLuaTgfTransmitPacketsWithCapture(..inPortNum..)
*
*       use enable = GT_TRUE to state to start using trgEPort
*       use enable = GT_FALSE to state to start NOT using trgEPort (trgEPort is ignored)
*
* INPUTS:
*       trgEPort     - egress device number
*       enable       - (GT_BOOL) do we use eport or not
*
* OUTPUTS:
*       None
*
* RETURNS:
*       return_code
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfStateTrgEPortInsteadPhyPort(
    IN GT_U32                trgEPort,
    IN GT_BOOL               enable
)
{
    use_trgEPortInsteadPhyPort  =  enable;
    trgEPortInsteadPhyPort      =  trgEPort;

    return GT_OK;
}


#ifdef CHX_FAMILY

/*******************************************************************************
* prvTgfDxChNetIfParamSet
*
* DESCRIPTION:
*       Fill in DxCh structure for send packet info
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       vpt              - VPT of the packet
*       cfiBit           - CFI bit
*       vid              - VID of the packet
*       packetIsTagged   - packet is tagged flag
*       doForceDsa       - GT_FALSE - choose DSA tag automatically
*                          GT_TRUE - use DSA tag defined by forcedDsa
*       forcedDsa        - DSA tag type, used when doForceDsa == GT_TRUE
* OUTPUTS:
*       dxChPcktParamsPtr - pointer to DxCh structure for send packet info.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
 GT_STATUS prvTgfDxChNetIfParamSet
(
    IN CPSS_INTERFACE_INFO_STC            *portInterfacePtr,
    IN GT_U32                              vpt,
    IN GT_U32                              cfiBit,
    IN GT_U32                              vid,
    IN GT_BOOL                             packetIsTagged,
    IN GT_BOOL                             doForceDsa,
    IN TGF_DSA_TYPE_ENT                    forcedDsa,
    OUT CPSS_DXCH_NET_TX_PARAMS_STC       *dxChPcktParamsPtr
)
{
    GT_U8                               devNum;
    GT_STATUS                           rc;
    CPSS_DXCH_NET_DSA_FROM_CPU_STC    *dxChfromCpuPtr    = NULL;
    CPSS_DXCH_NET_DSA_TYPE_ENT         dxChDsaTagType;

    devNum = (GT_U8)portInterfacePtr->devPort.hwDevNum;

    cpssOsMemSet(dxChPcktParamsPtr, 0, sizeof(*dxChPcktParamsPtr));

    switch (forcedDsa)
    {
        case TGF_DSA_1_WORD_TYPE_E: dxChDsaTagType = CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT; break;
        case TGF_DSA_2_WORD_TYPE_E: dxChDsaTagType = CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT; break;
        case TGF_DSA_3_WORD_TYPE_E: dxChDsaTagType = CPSS_DXCH_NET_DSA_3_WORD_TYPE_ENT; break;
        case TGF_DSA_4_WORD_TYPE_E: dxChDsaTagType = CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT; break;
        default: dxChDsaTagType = forcedDsa; break;
    }

    dxChPcktParamsPtr->cookie = NULL;
    dxChPcktParamsPtr->packetIsTagged = packetIsTagged;

    dxChPcktParamsPtr->sdmaInfo.recalcCrc = GT_TRUE;
    dxChPcktParamsPtr->sdmaInfo.txQueue   = 7;
    dxChPcktParamsPtr->sdmaInfo.evReqHndl = 0;
    dxChPcktParamsPtr->sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

    if (doForceDsa == GT_FALSE)
    {
        dxChPcktParamsPtr->dsaParam.commonParams.dsaTagType     = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
    }
    else
    {
        dxChPcktParamsPtr->dsaParam.commonParams.dsaTagType     = dxChDsaTagType;
    }

    dxChPcktParamsPtr->dsaParam.commonParams.vpt            = (GT_U8) vpt;
    dxChPcktParamsPtr->dsaParam.commonParams.cfiBit         = (GT_U8) cfiBit;
    dxChPcktParamsPtr->dsaParam.commonParams.vid            = (GT_U16) vid;
    dxChPcktParamsPtr->dsaParam.commonParams.dropOnSource   = GT_FALSE;
    dxChPcktParamsPtr->dsaParam.commonParams.packetIsLooped = GT_FALSE;

    dxChPcktParamsPtr->dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;

    dxChfromCpuPtr = &dxChPcktParamsPtr->dsaParam.dsaInfo.fromCpu;

    if(TGF_CPSS_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {

        /* support ability to send to phy port > 63 */
        if (doForceDsa == GT_FALSE)
        {
            dxChPcktParamsPtr->dsaParam.commonParams.dsaTagType     = CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT;
        }

        dxChfromCpuPtr->isTrgPhyPortValid = GT_TRUE;

        if( CPSS_INTERFACE_PORT_E == portInterfacePtr->type)
        {
            dxChfromCpuPtr->dstEport = portInterfacePtr->devPort.portNum;

            if(use_trgEPortInsteadPhyPort == GT_TRUE)
            {
                dxChfromCpuPtr->dstEport = trgEPortInsteadPhyPort;
            }

        }
    }


    dxChfromCpuPtr->dstInterface        = *portInterfacePtr;
    if( CPSS_INTERFACE_PORT_E == portInterfacePtr->type )
    {

        rc = extUtilHwDeviceNumberGet((portInterfacePtr->devPort.hwDevNum),
                                     &dxChfromCpuPtr->dstInterface.devPort.hwDevNum);

        if(GT_OK != rc)
            return rc;

        if(dxChfromCpuPtr->dstInterface.devPort.portNum >= 256)
        {
            GT_U32  value;
            if(PRV_CPSS_SIP_5_20_CHECK_MAC(prvTgfCpuDevNum) && appDemoDbEntryGet_func &&
                (appDemoDbEntryGet_func("doublePhysicalPorts", &value) == GT_OK) &&
                (value != 0))
            {
                dxChfromCpuPtr->isTrgPhyPortValid = GT_FALSE;
            }

            dxChfromCpuPtr->dstInterface.devPort.portNum &= 0xFF;
        }
    }

    dxChfromCpuPtr->tc                  = 7;
    dxChfromCpuPtr->dp                  = CPSS_DP_GREEN_E;
    dxChfromCpuPtr->egrFilterEn         = GT_FALSE;
    dxChfromCpuPtr->cascadeControl      = GT_TRUE;
    dxChfromCpuPtr->egrFilterRegistered = GT_FALSE;
    dxChfromCpuPtr->srcId               = 0;

    if(TGF_CPSS_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* value that in bypass bridge mode <Bypass Forwarding Decision only> */
        dxChfromCpuPtr->srcHwDev = 0x3f0;
    }
    else
    {

        rc = extUtilHwDeviceNumberGet(devNum, &dxChfromCpuPtr->srcHwDev);
        if(GT_OK != rc)
            return rc;
    }

    dxChfromCpuPtr->extDestInfo.devPort.dstIsTagged = dxChPcktParamsPtr->packetIsTagged;
    dxChfromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU = GT_FALSE;

    return GT_OK;
}
#endif
/*******************************************************************************
* prvTgfTrafficGeneratorNetIfSyncTxPacketSend
*
* DESCRIPTION:
*       Transmit the traffic to the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       bufferPtr        - (pointer to) the buffer to send
*       bufferLength     - buffer length (include CRC bytes)
*       useMii           - Use MII flag
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The packet will ingress to the PP after 'loopback' and will act as
*       it was received from a traffic generator connected to the port
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficGeneratorNetIfSyncTxPacketSend
(
    IN CPSS_INTERFACE_INFO_STC          *portInterfacePtr,
    IN GT_U8                            *bufferPtr,
    IN GT_U32                            bufferLength,
    IN GT_BOOL                           useMii
)
{
    GT_U8   devNum    = 0;
    GT_U32  numOfBufs = 0;
    GT_U32  vpt       = 0;
    GT_U32  cfiBit    = 0;
    GT_U32  vid       = 0;
    GT_BOOL packetIsTagged = GT_FALSE;

#ifdef CHX_FAMILY
    GT_STATUS   rc;
    CPSS_DXCH_NET_TX_PARAMS_STC       dxChPcktParams;
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_NET_TX_PARAMS_STC     exMxPmPcktParams = {0};
    CPSS_EXMXPM_NET_DSA_FROM_CPU_STC  *exMxPmfromCpuPtr      = NULL;
    GT_U8                             dummyCookie      = 0;
#endif /* EXMXPM_FAMILY */
#ifdef EX_FAMILY
    CPSS_EXMX_NET_TX_PARAMS_STC       exMxPcktParams = {0};
#endif /* EX_FAMILY */

    GT_U8   *buffList[1];
    GT_U32  buffLenList[1];


    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    buffList[0]    = bufferPtr;
    buffLenList[0] = bufferLength;

    numOfBufs = 1;
    devNum    = prvTgfCpuDevNum;

    if ((bufferPtr[12] == (GT_U8)(etherTypeForVlanTag >> 8)) &&
        (bufferPtr[13] == (GT_U8)(etherTypeForVlanTag & 0xFF)))
    {
        packetIsTagged = GT_TRUE;
        vpt    = (GT_U8) (bufferPtr[14] >> 5);
        cfiBit = (GT_U8) ((bufferPtr[14] >> 4) & 1);
        vid    = ((bufferPtr[14] & 0xF) << 8) | bufferPtr[15];
    }
    else
    {
        packetIsTagged = GT_FALSE;
        vpt    = 0;
        cfiBit = 0;
        vid    = 4094;
    }

#ifdef CHX_FAMILY

    /* fill in DXCH info */
    rc = prvTgfDxChNetIfParamSet(portInterfacePtr, vpt, cfiBit, vid,
                                 packetIsTagged, GT_FALSE, 0, &dxChPcktParams);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((useMii == GT_FALSE) || PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        rc = cpssDxChNetIfSdmaSyncTxPacketSend(devNum, &dxChPcktParams, buffList, buffLenList, numOfBufs);
    }
    else
    {
        rc = cpssDxChNetIfMiiSyncTxPacketSend(devNum, &dxChPcktParams, buffList, buffLenList, numOfBufs);
    }
    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    exMxPmPcktParams.cookie = (GT_PTR) &dummyCookie;
    exMxPmPcktParams.packetIsTagged = packetIsTagged;

    exMxPmPcktParams.sdmaInfo.recalcCrc = GT_TRUE;
    exMxPmPcktParams.sdmaInfo.txQueue   = 7;
    exMxPmPcktParams.sdmaInfo.evReqHndl = 0;
    exMxPmPcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_TRUE;

    exMxPmPcktParams.dsaParam.commonParams.dsaTagType = CPSS_EXMXPM_NET_DSA_TYPE_EXTENDED_E;
    exMxPmPcktParams.dsaParam.commonParams.vpt        = (GT_U8) vpt;
    exMxPmPcktParams.dsaParam.commonParams.cfiBit     = (GT_U8) cfiBit;
    exMxPmPcktParams.dsaParam.commonParams.vid        = (GT_U16) vid;

    exMxPmPcktParams.dsaParam.dsaCmd = CPSS_EXMXPM_NET_DSA_CMD_FROM_CPU_E;
    exMxPmfromCpuPtr = &exMxPmPcktParams.dsaParam.dsaInfo.fromCpu;

    exMxPmfromCpuPtr->dstInterface        = *portInterfacePtr;
    exMxPmfromCpuPtr->tc                  = 7;
    exMxPmfromCpuPtr->dp                  = CPSS_DP_GREEN_E;
    exMxPmfromCpuPtr->egrFilterEn         = GT_FALSE;
    exMxPmfromCpuPtr->egrFilterRegistered = GT_FALSE;
    exMxPmfromCpuPtr->srcId               = 0;
    exMxPmfromCpuPtr->isDit               = GT_FALSE;

    exMxPmfromCpuPtr->dstIsTagged = exMxPmPcktParams.packetIsTagged;
    exMxPmfromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU = GT_FALSE;

    return cpssExMxPmNetIfSdmaTxSyncPacketSend(devNum, &exMxPmPcktParams, buffList, buffLenList, numOfBufs);
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    exMxPcktParams.txParams.userPrioTag = 0;
    exMxPcktParams.txParams.tagged      = packetIsTagged;
    exMxPcktParams.txParams.packetEncap = CPSS_EXMX_NET_CONTROL_PCKT_E;
    exMxPcktParams.txParams.recalcCrc   = GT_TRUE;
    exMxPcktParams.txParams.vid         = (GT_U16) vid;
    exMxPcktParams.txParams.macDaType   = CPSS_EXMX_NET_UNICAST_MAC_E;
    exMxPcktParams.txParams.txQueue     = 7;
    exMxPcktParams.txParams.evReqHndl   = 0;
    exMxPcktParams.txParams.cookie      = NULL;
    exMxPcktParams.txParams.dropPrecedence = CPSS_DP_GREEN_E;

    exMxPcktParams.useVidx = GT_FALSE;

    exMxPcktParams.dest.devPort.tgtDev  = portInterfacePtr->devPort.hwDevNum;
    exMxPcktParams.dest.devPort.tgtPort = portInterfacePtr->devPort.portNum;

    exMxPcktParams.addDsa = GT_FALSE;

    exMxPcktParams.invokeTxBufferQueueEvent = GT_TRUE;

    return cpssExMxNetIfSyncTxPacketSend(devNum, &exMxPcktParams, buffList, buffLenList, numOfBufs);
#endif /* EX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

static GT_BOOL  generatorPoolInit = GT_FALSE;
/*******************************************************************************
* prvTgfTrafficGeneratorNetIfSyncTxBurstSend
*
* DESCRIPTION:
*       Transmit the traffic to the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       bufferPtr        - (pointer to) the buffer to send
*       bufferLength     - buffer length (include CRC bytes)
*       packetsNumber    - number of packets to send
*       doForceDsa       - GT_FALSE - choose DSA tag automatically
*                          GT_TRUE - use DSA tag defined by forcedDsa
*       forcedDsa        - DSA tag type, used when doForceDsa == GT_TRUE
*       useMii           - use MII
* OUTPUTS:
*       timeMilliPtr      - pointer to duration of TX operation in milliseconds.
*       sendFailPtr       - number of failed TX operations
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrafficGeneratorNetIfSyncTxBurstSend
(
    IN CPSS_INTERFACE_INFO_STC          *portInterfacePtr,
    IN GT_U8                            *bufferPtr,
    IN GT_U32                            bufferLength,
    IN GT_U32                            packetsNumber,
    IN GT_BOOL                           doForceDsa,
    IN TGF_DSA_TYPE_ENT                  forcedDsa,
    IN GT_BOOL                           useMii,
    OUT double                          *timeMilliPtr,
    OUT GT_U32                          *sendFailPtr
)
{
    GT_U8   devNum    = 0;
    GT_U32  numOfBufs = 0;
    GT_U32  vpt       = 0;
    GT_U32  cfiBit    = 0;
    GT_U32  vid       = 0;
    GT_BOOL packetIsTagged = GT_FALSE;

#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_NET_TX_PARAMS_STC       dxChPcktParams;
    GT_U32                             ii;
    GT_U32                             sendFail = 0;
    GT_U32                             sendOk = 0;
    GT_STATUS                          sendFailRC;
    GT_U32 startSec, endSec;     /* start and end time seconds part */
    GT_U32 startNano, endNano;   /* start and end time nanoseconds part */
    double startMilli, endMilli; /* start and end time in milliseconds */
#endif /* CHX_FAMILY */

    GT_U8   *buffList[1];
    GT_U32  buffLenList[1];

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    buffList[0]    = bufferPtr;
    buffLenList[0] = bufferLength;

    numOfBufs = 1;
    devNum    = prvTgfCpuDevNum;

    if ((bufferPtr[12] == (GT_U8)(etherTypeForVlanTag >> 8)) &&
        (bufferPtr[13] == (GT_U8)(etherTypeForVlanTag & 0xFF)))
    {
        packetIsTagged = GT_TRUE;
        vpt    = (GT_U8) (bufferPtr[14] >> 5);
        cfiBit = (GT_U8) ((bufferPtr[14] >> 4) & 1);
        vid    = ((bufferPtr[14] & 0xF) << 8) | bufferPtr[15];
    }
    else
    {
        packetIsTagged = GT_FALSE;
        vpt    = 0;
        cfiBit = 0;
        vid    = 4094;
    }

#ifdef CHX_FAMILY
    /* fill in DXCH info */
    rc = prvTgfDxChNetIfParamSet(portInterfacePtr, vpt, cfiBit, vid,
                                 packetIsTagged, doForceDsa, forcedDsa,
                                 &dxChPcktParams);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* store start time */
    cpssOsTimeRT(&startSec,&startNano);

    sendFailRC = GT_OK;
    for (ii = 0; ii < packetsNumber; ii++)
    {
        rc = cpssDxChNetIfSdmaSyncTxPacketSend(devNum, &dxChPcktParams, buffList, buffLenList, numOfBufs);
        if(GT_OK != rc)
        {
            sendFail++;
            sendFailRC = rc;
        }
        else
        {
            sendOk++;
        }
    }

    /* get end time */
    cpssOsTimeRT(&endSec,&endNano);

    startMilli = startSec * 1000 + startNano / 1000000;
    endMilli = endSec * 1000 + endNano / 1000000;

    *timeMilliPtr = (endMilli - startMilli);
    *sendFailPtr  = sendFail;

    return  sendFailRC;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    return GT_BAD_STATE;
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    return GT_BAD_STATE;
#endif /* EX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

#ifdef CHX_FAMILY
    CPSS_DXCH_NET_TX_PARAMS_STC       prvTgfRxToTxdxChPcktParams;
#endif /* CHX_FAMILY */

/*******************************************************************************
* prvTgfTrafficGeneratorRxToTxInfoSet
*
* DESCRIPTION:
*       Fill in structure for Rx To Tx send packet info
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       vpt              - VPT of the packet
*       cfiBit           - CFI bit
*       vid              - VID of the packet
*       packetIsTagged   - packet is tagged flag
*       doForceDsa       - GT_FALSE - choose DSA tag automatically
*                          GT_TRUE - use DSA tag defined by forcedDsa
*       forcedDsa        - DSA tag type, used when doForceDsa == GT_TRUE
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrafficGeneratorRxToTxInfoSet
(
    IN CPSS_INTERFACE_INFO_STC            *portInterfacePtr,
    IN GT_U32                              vpt,
    IN GT_U32                              cfiBit,
    IN GT_U32                              vid,
    IN GT_BOOL                             packetIsTagged,
    IN GT_BOOL                             doForceDsa,
    IN TGF_DSA_TYPE_ENT                    forcedDsa
)
{
#ifdef CHX_FAMILY
    return prvTgfDxChNetIfParamSet(portInterfacePtr, vpt, cfiBit, vid,
                                 packetIsTagged, doForceDsa, forcedDsa,
                                 &prvTgfRxToTxdxChPcktParams);
#else
    return GT_NOT_IMPLEMENTED;
#endif
}

/*******************************************************************************
* prvTgfNetIfRxToTxInfoSet
*
* DESCRIPTION:
*       The function prepares TGF insrastructure for automatic Tx packets from CPU
*       for any Rx packet.
*       All packets came to CPU will be TXed to port defined by input parameter.
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*       packetIsTagged   - packet is tagged flag
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfNetIfRxToTxInfoSet
(
    IN GT_U8                               devNum,
    IN GT_U32                              portNum,
    IN GT_BOOL                             packetIsTagged
)
{
    GT_STATUS                   rc;
    CPSS_INTERFACE_INFO_STC     portInterface;

    /* assign target port */
    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.hwDevNum = devNum;
    portInterface.devPort.portNum  = portNum;

    /* store Rx to Tx info */
    rc = prvTgfTrafficGeneratorRxToTxInfoSet(&portInterface, 0, 0, 4094, packetIsTagged,
                                             GT_FALSE, /* ignored*/ TGF_DSA_1_WORD_TYPE_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* enable Rx to Tx */
    tgfTrafficTableRxToTxSet(GT_TRUE);

    /* disable Do Counting only mode to get Rx to Tx processing */
    tgfTrafficTableRxDoCountOnlySet(GT_FALSE);

    /* Disable TX DMA and other interrups to avoid interrupts influence on test results.
       These interrupts are not used for TX from CPU in synch mode. */
    cpssEventDeviceMaskSet(devNum, CPSS_PP_TX_END_E, CPSS_EVENT_MASK_E);
    cpssEventDeviceMaskSet(devNum, CPSS_PP_TX_ERR_QUEUE_E, CPSS_EVENT_MASK_E);
    cpssEventDeviceMaskSet(devNum, CPSS_PP_RX_CNTR_OVERFLOW_E, CPSS_EVENT_MASK_E);

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficGeneratorRxToTxSend
*
* DESCRIPTION:
*       Transmit the traffic to predefined port by prvTgfTrafficGeneratorRxToTxInfoSet
*
* INPUTS:
*       devNum           - device number
*       numOfBufs        - number of buffers for packet
*       buffList         - list of buffers
*       buffLenList      - buffer length list (include CRC bytes)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrafficGeneratorRxToTxSend
(
    IN GT_U8            devNum,
    IN GT_U32           numOfBufs,
    IN GT_U8            *buffList[],
    IN GT_U32           buffLenList[]
)
{
#ifdef CHX_FAMILY
    GT_BOOL useMii;
    useMii =(PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_MII_E) ? GT_TRUE : GT_FALSE;
    if (useMii == GT_TRUE)
    {
        return cpssDxChNetIfMiiSyncTxPacketSend(devNum, &prvTgfRxToTxdxChPcktParams, buffList, buffLenList, numOfBufs);
    }
    return cpssDxChNetIfSdmaSyncTxPacketSend(devNum, &prvTgfRxToTxdxChPcktParams, buffList, buffLenList, numOfBufs);
#else
    return GT_NOT_IMPLEMENTED;
#endif /* CHX_FAMILY */

}

/*******************************************************************************
* prvTgfTrafficGeneratorPoolInit
*
* DESCRIPTION:
*       Init the pool for sending packets
*
* INPUTS:
*       None
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficGeneratorPoolInit
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;

    if(GT_FALSE == generatorPoolInit)
    {
        GT_BOOL useDMA = GT_TRUE;
#if defined CHX_FAMILY
        if (PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode != CPSS_NET_CPU_PORT_MODE_SDMA_E)
            useDMA = GT_FALSE;
#endif
        /* create pool of buffers from cache */
        if (useDMA == GT_TRUE)
        {
            rc = gtPoolCreateDmaPool(TGF_TX_BUFFER_MAX_SIZE_CNS,
                                     GT_4_BYTE_ALIGNMENT,
                                     TGF_MAX_NUM_OF_BUFFERS_AT_POOL,
                                     GT_TRUE,
                                     &prvTgfTxBuffersPoolId);
        }
        else
        {
            rc = gtPoolCreatePool(TGF_TX_BUFFER_MAX_SIZE_CNS,
                                     GT_4_BYTE_ALIGNMENT,
                                     TGF_MAX_NUM_OF_BUFFERS_AT_POOL,
                                     GT_TRUE,
                                     &prvTgfTxBuffersPoolId);
        }
        if (GT_OK != rc)
        {
            return rc;
        }

        generatorPoolInit = GT_TRUE;
    }



    return rc;
}

/* statistic DB */
#define TGF_STAT_LOOPS_CNS     (IDLE_POOLING_MAX_COUNT_CNS + 1)

/* number of Simulation Idle state pooling loops */
GT_U32 tgfStatLoops[TGF_STAT_LOOPS_CNS];
/* number of Rx MAC Counters pooling loops */
GT_U32 tgfStatCntrLoops[TGF_STAT_LOOPS_CNS];

/*******************************************************************************
* tgfStatLoopsDump
*
* DESCRIPTION:
*       Dump statistic DB.
*
* INPUTS:
*       None.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
int tgfStatLoopsDump(void)
{
    int ii;

    PRV_TGF_LOG0_MAC("\n Idle state Loops statistic\n");
    for (ii = 0; ii < TGF_STAT_LOOPS_CNS; ii++)
    {
        if (tgfStatLoops[ii])
        {
            PRV_TGF_LOG2_MAC("loop %2d count %d\n", ii, tgfStatLoops[ii]);
        }
    }

    PRV_TGF_LOG0_MAC("\n Counters Loops statistic\n");
    for (ii = 0; ii < TGF_STAT_LOOPS_CNS; ii++)
    {
        if (tgfStatCntrLoops[ii])
        {
            PRV_TGF_LOG2_MAC("loop %2d count %d\n", ii, tgfStatCntrLoops[ii]);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorCheckProcessignDone
*
* DESCRIPTION:
*       Check that all packets were processed by device.
*       The function checks Idle state of device - works on simulation only.
*
* INPUTS:
*       sleepAfterCheck - GT_TRUE - sleep some time after Idle check
*                         GT_FALSE - do not sleep after Idle check
*       loopCount       - number of 1 milisecond sleeps done before call of
*                         the function. Used only if sleepAfterCheck == GT_TRUE
* OUTPUTS:
*       None.
*
* RETURNS:
*       0 - pooling detected that system is still busy
*       other value - number of pooling loops till detection of busy.
* COMMENTS:
*
*******************************************************************************/
GT_U32 tgfTrafficGeneratorCheckProcessignDone
(
    IN GT_BOOL  sleepAfterCheck,
    IN GT_U32   loopCount
)
{
#if defined(ASIC_SIMULATION) && !defined(GM_USED)

    GT_U32 status; /* simulation Idle or Busy status */
    GT_U32 overallSleep; /* overall sleep time */
    GT_U32 loops; /* loops counter */

startLoops_lbl:
    if(tgfStormingExpected == GT_FALSE)
    {
        loops = IDLE_POOLING_MAX_COUNT_CNS;
    }
    else
    {
        loops = 100;
    }
    overallSleep = loopCount;

    while(loops)
    {
        status = skernelStatusGet();
        if (status == 0)
        {
            /* the simulation is Idle. Simulation finished processing
               of packets. */
            tgfStatLoops[IDLE_POOLING_MAX_COUNT_CNS - loops]++;
            break;
        }

        /* simulation is Busy. Check it again after sleep. */
        cpssOsTimerWkAfter(1);
        overallSleep++;
        loops--;
    }

    if (loops == 0)
    {
        if(simLogIsOpen())
        {
            /* we need to wait more for the end of processing */
            goto startLoops_lbl;
        }

        tgfStatLoops[IDLE_POOLING_MAX_COUNT_CNS]++;
    }

    if (sleepAfterCheck == GT_TRUE)
    {
        if (overallSleep < MINIMAL_SLEEP_CNS)
        {
            /* the test need some minimal time for CPSS Event handler tasks */
            cpssOsTimerWkAfter(MINIMAL_SLEEP_CNS);
        }
        else
        {
            cpssOsTimerWkAfter(MANDATORY_SLEEP_CNS);
        }
    }

    return loops;
#else
    return IDLE_POOLING_MAX_COUNT_CNS;
#endif
}

/*******************************************************************************
* tgfTrafficGeneratorStormingExpected
*
* DESCRIPTION:
*       state that the test doing storming on the ingress port (ingress traffic
*       will be send back to ingress port that is in 'loopback mode').
*       this indication needed for WM to know to break the loop regardless to
*       indications from skernelStatusGet()
*
* INPUTS:
*       stormingExpected - GT_TRUE - storming is expected
*                          GT_FALSE - storming is NOT expected
* OUTPUTS:
*       None.
*
* RETURNS:
*       nothing
* COMMENTS:
*
*******************************************************************************/
GT_VOID tgfTrafficGeneratorStormingExpected(IN GT_BOOL stormingExpected)
{
    tgfStormingExpected = stormingExpected;
}

/*******************************************************************************
* prvTgfDevPortMacCountersClearOnReadSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* INPUTS:
*       devNum
*       portNum     - physical port number (or CPU port)
*       enable      - enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfDevPortMacCountersClearOnReadSet
(
    IN GT_U8    devNum,
    IN GT_U32   portNum,
    IN GT_BOOL  enable
)
{
#if (defined CHX_FAMILY)
    /* call device specific API */
    return cpssDxChPortMacCountersClearOnReadSet(devNum, portNum, enable);

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmPortMacCountersClearOnReadEnableSet(devNum, portNum, enable);
#endif


#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfDevPortMacCountersClearOnReadGet
*
* DESCRIPTION:
*       Get status (Enable or disable) of  MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr      - pointer to enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvTgfDevPortMacCountersClearOnReadGet
(
    IN GT_U8    devNum,
    IN GT_U32    portNum,
    OUT GT_BOOL  *enablePtr
)
{
#if (defined CHX_FAMILY)
    /* call device specific API */
    return cpssDxChPortMacCountersClearOnReadGet(devNum, portNum, enablePtr);

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmPortMacCountersClearOnReadEnableGet(devNum, portNum, enablePtr);
#endif

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enablePtr);


    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfPortMacCounterGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a specified port on specified device.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*                  when using the SDMA interface refer to the API
*                  cpssDxChNetIfSdmaRxCountersGet(...)
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number, device or counter name
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvTgfPortMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_STATUS                        rc;
#endif
#if (defined CHX_FAMILY)
   /* Get Port MAC MIB counters */
   rc = cpssDxChMacCounterGet(devNum, portNum, cntrName, cntrValuePtr);
   if (GT_OK != rc)
   {
       PRV_TGF_LOG1_MAC(
           "[TGF]: cpssDxChPortMacCountersOnPortGet FAILED, rc = [%d]", rc);
   }
   return rc;
#endif /* CHX_FAMILY */

#if (defined EXMXPM_FAMILY)
   /* Get Port MAC MIB counters */
   rc = cpssExMxPmPortSpecificMacCounterGet(devNum, portNum, cntrName, cntrValuePtr);
   if (GT_OK != rc)
   {
       PRV_TGF_LOG1_MAC(
           "[TGF]: cpssExMxPmPortSpecificMacCounterGet FAILED, rc = [%d]", rc);
   }
   return rc;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(cntrName);
    TGF_PARAM_NOT_USED(cntrValuePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}


/*******************************************************************************
* tgfTrafficGeneratorCheckTxDone
*
* DESCRIPTION:
*       Check that all packets were entered to port.
*       The function checks Rx MAC MIB counter to check that all packets
*       entered to ingress test port.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       burstCount       - number of frames (non-zero value)
*       packetSize       - size of single packet
*       cntrName         - specific counter name
*       counterPreTxPtr  - pointer to counter value before Tx
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorCheckTxDone
(
    IN CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN GT_U32                           burstCount,
    IN GT_U32                           packetSize,
    IN CPSS_PORT_MAC_COUNTERS_ENT       cntrName,
    IN GT_U64                           *counterPreTxPtr
)
{
    GT_STATUS   rc;            /* return status */
    GT_U32      portNum = 0;   /* port number */
    GT_BOOL     clearOnRead;   /* clear on read status */
    GT_U64      portRxCntr;    /* port RX MAC MIB counter value */
    GT_U32      burstSize = burstCount * packetSize; /* size of burst in bytes */
    GT_U32      loopCount, loops; /* loop counters */
    GT_U8       devNum = 0;    /* device number */
    CPSS_PP_FAMILY_TYPE_ENT  devFamily; /* device family */
    GT_U32      prevCount;/* previous packets count*/
    GT_U32      countFail; /* MAC counters pooling failure indication:
                             0 - no failure, other - failure and last loop number */

    /* reset MAC counters read loop */
    loopCount = 0;
    countFail = 0;

    /* skip counters check for FWS traffic */
    if (prvTgfFwsUse == 0)
    {
        portNum = portInterfacePtr->devPort.portNum;

/* TODO */

        /* get device number */
#if 0
        if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
        {
            return GT_BAD_PARAM;
        }
#endif
        devNum = (GT_U8)(portInterfacePtr->devPort.hwDevNum);

        /* get device family */
        DEV_CHECK_MAC(devNum);
        devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;

        /* check counters for DX device only.
          Puma2 GM has Clear-On-Read not working for ports 18, 23 */
        if (CPSS_IS_DXCH_FAMILY_MAC(devFamily))
        {
            /* get status of clear on read */
            rc = prvTgfDevPortMacCountersClearOnReadGet(devNum, portNum, &clearOnRead);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* disable clear counters on read */
            if (clearOnRead != GT_FALSE)
            {
                rc = prvTgfDevPortMacCountersClearOnReadSet(devNum, portNum, GT_FALSE);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            portRxCntr.l[0] = 0;
            portRxCntr.l[1] = 0;
            prevCount = 0;
            do
            {
                /* NOTE :
                   use counter of bytes and not packets because for packets we will need to accumulate 3 counters of : uc,mc,bc
                   so working with bytes saves total runtime
                */

                /* get Rx counter value */
                rc = prvTgfPortMacCounterGet(devNum, portNum, cntrName, &portRxCntr);
                if (rc != GT_OK)
                {
                    return rc;
                }

                if ((portRxCntr.l[0] - counterPreTxPtr->l[0]) >= burstSize)
                {
                    /* all packets were come to port's Rx MAC */
                    break;
                }

                loopCount++;
                cpssOsTimerWkAfter(1);

                if((burstSize > 1) && (loopCount > 200))
                {
                    /* after minimum of 200 iterations (200 milliseconds)
                        lets examine that the device still process packets.
                        do that by checking if the number of receives packets increase.
                    */

                    if(prevCount == portRxCntr.l[0])
                    {
                        /* no processing done in the last period .. no need to wait till end of iterations */
                        countFail = loopCount;
                        break;
                    }

                    if(prevCount > portRxCntr.l[0])
                    {
                        /* should not happen -- ERROR */
                        return (GT_STATUS)GT_ERROR; /*casting needed for DIAB compiler*/
                    }
                }

                prevCount = portRxCntr.l[0];

            }while(loopCount < prvTgfTrafficRxCheckMaxCount);

            tgfStatCntrLoops[loopCount]++;

            if (loopCount >= prvTgfTrafficRxCheckMaxCount)
            {
                /* probably packets were lost */
                countFail = loopCount;
            }

            if (clearOnRead != GT_FALSE)
            {
                /* restore enable clear counters on read */
                rc = prvTgfDevPortMacCountersClearOnReadSet(devNum, portNum, GT_TRUE);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        } /* if(CPSS_IS_DXCH_FAMILY_MAC(devFamily)) */
    }

    /* let the packets be sent */
#ifdef  EMBEDDED
    /* run on hardware - one 10 millisecond sleep is enough */
    loops = 1;
#else
    /* run on simulation */
    loops = prvTgfTimeLoop;
#endif

#if defined(ASIC_SIMULATION) && !defined(GM_USED)
    if (tgfTrafficGeneratorCheckProcessignDone(GT_TRUE, loopCount) == 0)
    {
        /* probably packets were lost */
        PRV_TGF_LOG1_MAC("\nWv: Counters count[%d]\n", loopCount);
    }
#else
#ifdef GM_USED
    if(SUPPORT_ALL_IN_SINGLE_CONTEXT_MAC)
    {
        loops = 0;
    }
#endif /*GM_USED*/

    /* adapt MAC counters loopCount 1 millisecond sleep for units of
       10 millisecond sleep loops */
    loopCount = loopCount / 10;

    if (loopCount < loops)
    {
        loops -= loopCount;
        while(loops--)/* allow debugging by set this value = 1000 */
        {
            cpssOsTimerWkAfter(10);
        }
    }
#endif
    if(countFail)
    {
        PRV_TGF_LOG2_MAC("\nWw: Counters count[%d] Idle count[%d]\n", countFail, loops);
#ifdef  EMBEDDED
        /* ON HW this is real indication that packet got lost .... maybe this is intended
           but on most cases this is indication of unexpected filter by the device !!!

           make it  cpssOsPrintf and not PRV_TGF_LOG0_MAC because we need those
           errors regardless to LOG print mode !!!
        */
        cpssOsPrintf("ERROR : The 'FROM_CPU' packet of the TGF engine did not reach the target port %d/%d ('src port' of the test !!!) \n",devNum,portNum);
        cpssOsPrintf("ERROR : Check what caused the Device to filter the packet \n");
#endif
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorPreTxCountGet
*
* DESCRIPTION:
*       Get counter value for Tx Done check
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       cntrName - specific counter name
*
* OUTPUTS:
*       counterPtr  - pointer to counter.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPreTxCountGet
(
    IN  CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN  CPSS_PORT_MAC_COUNTERS_ENT      cntrName,
    OUT GT_U64                           *counterPtr
)
{
    GT_STATUS   rc;            /* return status */
    GT_U32      portNum;       /* port number */
    GT_BOOL     clearOnRead;   /* clear on read status */
    GT_U8       devNum;        /* device number */
    CPSS_PP_FAMILY_TYPE_ENT  devFamily; /* device family */

    counterPtr->l[0] = 0;
    counterPtr->l[1] = 0;

    /* skip counters check for FWS traffic */
    if (prvTgfFwsUse == 0)
    {
        portNum = portInterfacePtr->devPort.portNum;

/*TODO*/
        /* get device number */
        devNum = (GT_U8)portInterfacePtr->devPort.hwDevNum;
#if 0
        if (GT_OK != prvTgfPortDeviceNumByPortGet(portNum, &devNum))
        {
            return GT_BAD_PARAM;
        }
#endif

        /* get device family */
        DEV_CHECK_MAC(devNum);
        devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;

        /* check counters for DX device only.
          Puma2 GM has Clear-On-Read not working for ports 18, 23 */
        if (CPSS_IS_DXCH_FAMILY_MAC(devFamily))
        {
            /* get status of clear on read */
            rc = prvTgfDevPortMacCountersClearOnReadGet(devNum, portNum, &clearOnRead);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* disable clear counters on read */
            if (clearOnRead != GT_FALSE)
            {
                rc = prvTgfDevPortMacCountersClearOnReadSet(devNum, portNum, GT_FALSE);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            /* get Rx counter value */
            rc = prvTgfPortMacCounterGet(devNum, portNum, cntrName, counterPtr);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (clearOnRead != GT_FALSE)
            {
                /* restore enable clear counters on read */
                rc = prvTgfDevPortMacCountersClearOnReadSet(devNum, portNum, GT_TRUE);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

        } /* if (CPSS_IS_DXCH_FAMILY_MAC(devFamily)) */
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthTransmit
*
* DESCRIPTION:
*       Transmit the traffic to the port
*       NOTE:
*           1. allow to transmit with chunks of burst:
*              after x sent frames --> do sleep of y millisecond
*              see parameters
*
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       packetInfoPtr    - (pointer to) the packet info
*       burstCount       - number of frames (non-zero value)
*       numVfd           - number of VFDs
*       vfdArray         - pointer to array of VFDs (may be NULL if numVfd = 0)
*       sleepAfterXCount - do 'sleep' after X packets sent
*                          when = 0 , meaning NO SLEEP needed during the burst
*                          of 'burstCount'
*       sleepTime        - sleep time (in milliseconds) after X packets sent , see
*                          parameter sleepAfterXCount
*       traceBurstCount  - number of packets in burst count that will be printed
*       loopbackEnabled  - Check RCV counters when TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The packet will ingress to the PP after 'loopback' and will act as
*       it was received from a traffic generator connected to the port
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthTransmit
(
    IN CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN TGF_PACKET_STC                  *packetInfoPtr,
    IN GT_U32                           burstCount,
    IN GT_U32                           numVfd,
    IN TGF_VFD_INFO_STC                 vfdArray[],
    IN GT_U32                           sleepAfterXCount,
    IN GT_U32                           sleepTime,
    IN GT_U32                           traceBurstCount,
    IN GT_BOOL                          loopbackEnabled
)
{
    GT_STATUS   rc, rc1    = GT_OK;
    GT_U32      i, j;
    GT_U8       *bufferPtr = NULL;
    GT_U32      xCounter   ;/* counter of already sent packets ,
                            for sleep when xCounter = 0 ,
                            relevant only when sleepAfterXCount != 0*/
    GT_BOOL     hugeSend = GT_FALSE;/* is the burst 'huge'*/
    GT_U32      maxRetry = MAX_RETRY_CNS;  /* max retry number on error */
    GT_BOOL     doProgressIndication = GT_FALSE;
    GT_BOOL     avoidKeepAlive = GT_FALSE;/* avoid keep alive print */
    TGF_AUTO_CHECKSUM_FULL_INFO_STC checksumInfoArr[TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS];/* auto checksum info */
    TGF_AUTO_CHECKSUM_EXTRA_INFO_STC extraChecksumInfoArr[TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS];/* extra checksum info */
    GT_BOOL     useMii;/* do we use MII or SDMA */
    GT_U32      packetTotalLen = 0;/* packet total length (not include CRC) */
    GT_U32      numOfBytesInPacketBuff;/* number of bytes in packetBuff*/ /* save space of CRC*/
    GT_BOOL     keepAliveDone = GT_FALSE;
    GT_U64      counter;
    CPSS_PORT_MAC_COUNTERS_ENT cntrName; /* counter name */
#ifdef GM_USED
    GT_U32      loops; /* iterator */
#endif
    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);

    cntrName = (loopbackEnabled == GT_TRUE)
            ? CPSS_GOOD_OCTETS_RCV_E
            : CPSS_GOOD_OCTETS_SENT_E;

    if(numVfd)
    {
        CPSS_NULL_PTR_CHECK_MAC(vfdArray);
    }

    if(burstCount == 0)
    {
        return GT_BAD_PARAM;
    }

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(keepAliveDone); /* prevent warning: not used variable */

    useMii = GT_FALSE;
#else
        #if defined CHX_FAMILY
            useMii =((PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_MII_E) ? GT_TRUE:
                     (PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_NONE_E) ? GT_FALSE:
                     PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(prvTgfCpuDevNum) ? GT_TRUE : GT_FALSE);
        #else
            useMii = GT_FALSE;
        #endif
#endif

    if(useMii == GT_TRUE)
    {
        if(burstCount > 128 && sleepAfterXCount == 0)
        {
            keepAliveDone = GT_TRUE;
            tgfPrintKeepAlive();
        }

#ifdef _WIN32
        if(burstCount >= 16 && sleepAfterXCount == 0)
        {
            sleepAfterXCount = miiCounter;
            sleepTime = 10;
            avoidKeepAlive = GT_TRUE;
        }
#endif /*_WIN32*/

    }
    else  /* SDMA */
    {
        /* the simulation in win32 need to process the 'loopback packets'.
           otherwise all the simulation buffers may be over because the SDMA
           processing is sending to the loopbacked port frames in new buffers but
           the smain task is still busy with new SDAM frame , so no one process
           the looped frames , so no one free those buffers ... no more buffers ...
           tests fail */

        /* NOTE: this code was also needed in XCAT board --> see test tgfTablesFillingLttCheckIpv4 */
        if(burstCount > 128 && sleepAfterXCount == 0)
        {
            keepAliveDone = GT_TRUE;
            tgfPrintKeepAlive();
            sleepAfterXCount = 64;
            sleepTime = 10;
            avoidKeepAlive = GT_TRUE;
        }

        if(burstCount > SDMA_ABORT_MAX_CNS &&
            (tgfTrafficGeneratorRxCaptureNum ||
            tgfTrafficGeneratorExpectTraficToCpuEnabled))
        {
#ifdef CHX_FAMILY
            if (PRV_CPSS_DXCH_ERRATA_GET_MAC(prvTgfCpuDevNum,PRV_CPSS_DXCH3_SDMA_WA_E) == GT_TRUE)
            {
                if(keepAliveDone == GT_FALSE)
                {
                    keepAliveDone = GT_TRUE;
                    tgfPrintKeepAlive();
                }
                /* the device not support 'SDMA retry' for traffic trap to the CPU */
                /* and we don't want to loose traffic trapped to the CPU */
                sleepAfterXCount = SDMA_ABORT_MAX_CNS;
                sleepTime = 10;
                avoidKeepAlive = GT_TRUE;
            }
#endif /*CHX_FAMILY*/
        }
    }

#ifdef GM_USED
    if(SUPPORT_ALL_IN_SINGLE_CONTEXT_MAC)
    {
        sleepTime = 0;
    }

#endif /*GM_USED*/


    xCounter = sleepAfterXCount;

    cpssOsMemSet(checksumInfoArr,0,sizeof(checksumInfoArr));

    /* init the pool */
    rc = prvTgfTrafficGeneratorPoolInit();
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    /* get buffer from the pool */
    bufferPtr = gtPoolGetBuf(prvTgfTxBuffersPoolId);
    if(NULL == bufferPtr)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    numOfBytesInPacketBuff = TGF_RX_BUFFER_MAX_SIZE_CNS - 4;/* save space of CRC*/
    /* build the basic frame info */                            /* save 4 bytes space for CRC */
    rc = tgfTrafficEnginePacketBuild(packetInfoPtr, bufferPtr ,&numOfBytesInPacketBuff, checksumInfoArr,extraChecksumInfoArr);
    if(rc != GT_OK)
    {
        PRV_TGF_LOG1_MAC("[TGF]: tgfTrafficEnginePacketBuild FAILED, rc = [%d]", rc);
        goto exit_cleanly_lbl;
    }

    packetTotalLen = numOfBytesInPacketBuff;

    if((packetTotalLen + TGF_CRC_LEN_CNS) > TGF_TX_BUFFER_MAX_SIZE_CNS)
    {
        return GT_BAD_PARAM;
    }

    if((packetTotalLen + TGF_CRC_LEN_CNS) < TGF_TX_BUFFER_MIN_SIZE_CNS)
    {
        PRV_TGF_LOG1_MAC("utf Transmit: WARNING !!! packet size is too small [%d] \n",
                         packetTotalLen + TGF_CRC_LEN_CNS);
    }

    hugeSend = (burstCount > traceBurstCount) ? GT_TRUE : GT_FALSE;

    if((hugeSend == GT_TRUE) && (debug_forcePrint == 0))
    {
        /* we can't allow printings for huge number of frames */
        rc = tgfTrafficTracePacketByteSet(GT_FALSE);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }

    if (prvTgfTrafficPrintPacketTxEnable)
    {
        PRV_TGF_LOG2_MAC("utf : transmit [%d] packets into port[%d] \n",
            burstCount,
            portInterfacePtr->devPort.portNum);
    }

    if(hugeSend == GT_TRUE)
    {
#ifdef ASIC_SIMULATION
        if(burstCount > 1000)
        {
            PRV_TGF_LOG3_MAC("utf Transmit: Huge burst[%d] to dev [%d] port[%d] \n",
                             burstCount,
                             portInterfacePtr->devPort.hwDevNum,
                             portInterfacePtr->devPort.portNum);
        }
#endif /*ASIC_SIMULATION*/
        doProgressIndication = GT_TRUE;
    }

    /* get mac counter before Tx */
    rc = tgfTrafficGeneratorPreTxCountGet(portInterfacePtr, cntrName, &counter);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    for (i = 0; i < burstCount; i++)
    {
        for(j = 0; j < numVfd; j++)
        {
            vfdArray[j].modeExtraInfo = i;

            /* over ride the buffer with VFD info */
            rc = tgfTrafficEnginePacketVfdApply(&vfdArray[j], bufferPtr, packetTotalLen);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }
        }

        /* apply auto checksum fields after applying the VFD*/
        rc = autoChecksumField(checksumInfoArr,extraChecksumInfoArr);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

        /* add the ability to trace the packet content */
        rc = tgfTrafficTracePacket(bufferPtr,
                                   packetTotalLen,
                                   GT_FALSE);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }

        if(sleepAfterXCount && ((--xCounter) == 0))
        {
            /* allow the 'sleep' after X packets sent , to let AppDemo task to
               process AUQ */
            if(avoidKeepAlive == GT_FALSE)
            {
                tgfPrintKeepAlive();
            }

            cpssOsTimerWkAfter(sleepTime);
            xCounter   = sleepAfterXCount;
        }

        if(doProgressIndication == GT_TRUE)
        {
            if(4999 == (i % 5000))
            {
#ifdef ASIC_SIMULATION
                PRV_TGF_LOG1_MAC("sent [%d]",i+1);
#endif /*ASIC_SIMULATION*/
            }
            else if(1999 == (i % 2000))
            {
                tgfPrintKeepAlive();
            }
        }
retry_send_lbl:
        /* send the buffer to the device,port */
        rc = prvTgfTrafficGeneratorNetIfSyncTxPacketSend(portInterfacePtr,
                                                             bufferPtr,
                                                             packetTotalLen + TGF_CRC_LEN_CNS,
                                                             useMii);
        if(rc != GT_OK)
        {
            prvTgfTrafficGeneratorNumberPacketsSendFailed++;

            if(sleepTime)
            {
                cpssOsTimerWkAfter(sleepTime);
            }
            else
            {
                cpssOsTimerWkAfter(10);
            }

            /* retry to send again ! */
            if(--maxRetry)
            {
                PRV_TGF_LOG0_MAC("^");
                goto retry_send_lbl;
            }


            if(rc == GT_HW_ERROR)
            {
                consecutiveHwErrors++;

#ifdef ASIC_SIMULATION
                if(consecutiveHwErrors == 5)
                {
                    skernelFatalError("tgfTrafficGeneratorPortTxEthTransmit : unable to send packets , too many errors ! \n");
                }
#endif/*ASIC_SIMULATION*/
            }

            goto exit_cleanly_lbl;
        }
        else
        {
            prvTgfTrafficGeneratorNumberPacketsSendOk++;
        }
#ifdef GM_USED
        /* time factor for GM devices , due to very slow processing of packets (2 per second) */
        /* for 'fine tuning' / debug use function tgfTrafficGeneratorGmTimeFactorSet(...) */

        loops = prvTgfTimeLoop * prvTgfGmTimeFactor;

        if(SUPPORT_ALL_IN_SINGLE_CONTEXT_MAC)
        {
            loops = 0;
        }

        while(loops--)/* allow debugging by set this value = 1000 */
        {
            cpssOsTimerWkAfter(10);
        }
#endif /*GM_USED*/
        maxRetry = MAX_RETRY_CNS;/* restore the 5 retries per frame */
    }

    /* we managed to send the packets */
    consecutiveHwErrors = 0;


    if(sleepAfterXCount)
    {
        /* allow appDemo process the last section of the burst */
        cpssOsTimerWkAfter(sleepTime);
    }


#ifdef ASIC_SIMULATION
    if(hugeSend == GT_TRUE && burstCount > 1000)
    {
        PRV_TGF_LOG0_MAC(" : Huge burst Ended \n");
    }
#endif /*ASIC_SIMULATION*/

    rc1 = tgfTrafficGeneratorCheckTxDone(portInterfacePtr,
            burstCount, packetTotalLen  + TGF_CRC_LEN_CNS,
            cntrName, &counter);

exit_cleanly_lbl:
    if(hugeSend == GT_TRUE)
    {
        /* restore value */
        rc1 = tgfTrafficTracePacketByteSet(GT_TRUE);
    }

    if(bufferPtr)
    {
        /* release the buffer back to the pool */
        rc1 = gtPoolFreeBuf(prvTgfTxBuffersPoolId, bufferPtr);
    }

    rc = rc1 != GT_OK ? rc1 : rc;

    return rc;
}

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthBurstTransmit
*
* DESCRIPTION:
*       Transmit the traffic burst to the port.
*       Used for TX SDMA performance measurement.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       packetInfoPtr    - (pointer to) the packet info
*       burstCount       - number of frames (non-zero value)
*       doForceDsa       - GT_FALSE - choose DSA tag automatically
*                          GT_TRUE - use DSA tag defined by forcedDsa
*       forcedDsa        - DSA tag type, used when doForceDsa == GT_TRUE
* OUTPUTS:
*       timeMilliPtr      - pointer to duration of TX operation in milliseconds.
*       sendFailPtr       - number of failed TX operations
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthBurstTransmit
(
    IN CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN TGF_PACKET_STC                  *packetInfoPtr,
    IN GT_U32                           burstCount,
    IN GT_BOOL                          doForceDsa,
    IN TGF_DSA_TYPE_ENT                 forcedDsa,
    OUT double                         *timeMilliPtr,
    OUT GT_U32                         *sendFailPtr
)
{
    GT_STATUS   rc, rc1    = GT_OK;
    GT_U8       *bufferPtr = NULL;
    TGF_AUTO_CHECKSUM_FULL_INFO_STC checksumInfoArr[TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS];/* auto checksum info */
    TGF_AUTO_CHECKSUM_EXTRA_INFO_STC extraChecksumInfoArr[TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS];/* extra checksum info */
    GT_U32      packetTotalLen = 0;/* packet total length (not include CRC) */
    GT_U32      numOfBytesInPacketBuff;/* number of bytes in packetBuff*/ /* save space of CRC*/
    GT_BOOL     useMii = GT_FALSE;

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);

    if(burstCount == 0)
    {
        return GT_BAD_PARAM;
    }

    cpssOsMemSet(checksumInfoArr,0,sizeof(checksumInfoArr));

    /* init the pool */
    rc = prvTgfTrafficGeneratorPoolInit();
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    /* get buffer from the pool */
    bufferPtr = gtPoolGetBuf(prvTgfTxBuffersPoolId);
    if(NULL == bufferPtr)
    {
        return GT_OUT_OF_CPU_MEM;
    }

#if defined CHX_FAMILY
    useMii =((PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_MII_E) ? GT_TRUE:
             (PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_NONE_E) ? GT_FALSE:
             PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(prvTgfCpuDevNum) ? GT_TRUE : GT_FALSE);
#endif
    numOfBytesInPacketBuff = TGF_RX_BUFFER_MAX_SIZE_CNS - 4;/* save space of CRC*/
    /* build the basic frame info */                            /* save 4 bytes space for CRC */
    rc = tgfTrafficEnginePacketBuild(packetInfoPtr, bufferPtr ,&numOfBytesInPacketBuff, checksumInfoArr,extraChecksumInfoArr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    packetTotalLen = numOfBytesInPacketBuff;

    if((packetTotalLen + TGF_CRC_LEN_CNS) > TGF_TX_BUFFER_MAX_SIZE_CNS)
    {
        return GT_BAD_PARAM;
    }

    if((packetTotalLen + TGF_CRC_LEN_CNS) < TGF_TX_BUFFER_MIN_SIZE_CNS)
    {
        PRV_TGF_LOG1_MAC("utf Transmit: WARNING !!! packet size is too small [%d] \n",
                         packetTotalLen + TGF_CRC_LEN_CNS);
    }

    if (prvTgfTrafficPrintPacketTxEnable)
    {
        PRV_TGF_LOG2_MAC("utf : transmit [%d] packets into port[%d] \n",
            burstCount,
            portInterfacePtr->devPort.portNum);
    }

    /* apply auto checksum fields after applying the VFD*/
    rc = autoChecksumField(checksumInfoArr,extraChecksumInfoArr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    /* add the ability to trace the packet content */
    rc = tgfTrafficTracePacket(bufferPtr,
                               packetTotalLen,
                               GT_FALSE);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }


    /* send the buffer to the device,port */
    rc = prvTgfTrafficGeneratorNetIfSyncTxBurstSend(portInterfacePtr,
                                                         bufferPtr,
                                                         packetTotalLen + TGF_CRC_LEN_CNS,
                                                         burstCount, doForceDsa, forcedDsa,
                                                         useMii,
                                                         timeMilliPtr, sendFailPtr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

exit_cleanly_lbl:
    if(bufferPtr)
    {
        /* release the buffer back to the pool */
        rc1 = gtPoolFreeBuf(prvTgfTxBuffersPoolId, bufferPtr);
    }

    rc = rc1 != GT_OK ? rc1 : rc;

    return rc;
}

/*******************************************************************************
* tgfTrafficGeneratorRxInCpuGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table
*
* INPUTS:
*       packetType      - the packet type to get
*       getFirst        - get first/next entry
*       trace           - enable\disable packet tracing
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - length of the copied packet to gtBuf
*       packetLenPtr    - Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr        - Rx queue in which the packet was received.
*       rxParamsPtr     - specific device Rx info format.
*
* RETURNS:
*       GT_OK      - on success
*       GT_NO_MORE - on more entries
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       For 'captured' see API tgfTrafficGeneratorPortTxEthCaptureSet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorRxInCpuGet
(
    IN    TGF_PACKET_TYPE_ENT   packetType,
    IN    GT_BOOL               getFirst,
    IN    GT_BOOL               trace,
    OUT   GT_U8                *packetBufPtr,
    INOUT GT_U32               *packetBufLenPtr,
    OUT   GT_U32               *packetLenPtr,
    OUT   GT_U8                *devNumPtr,
    OUT   GT_U8                *queuePtr,
    OUT   TGF_NET_DSA_STC      *rxParamsPtr
)
{
    TGF_PACKET_TYPE_ENT currPacketType = TGF_PACKET_TYPE_REGULAR_E;
    static GT_U32       currentIndex   = 0;
    GT_STATUS           rc             = GT_OK;
    GT_BOOL             newGetFirst    = getFirst;
    GT_8                specificDeviceFormat[TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS] = {0};
#ifdef CHX_FAMILY
    CPSS_DXCH_NET_RX_PARAMS_STC   *dxChPcktParamsPtr   = NULL;
#endif
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_NET_RX_PARAMS_STC *exMxPmPcktParamsPtr = NULL;
#endif
#ifdef EX_FAMILY
    CPSS_EXMX_NET_RX_PARAMS_STC   *exMxPcktParamsPtr   = NULL;
#endif

    CPSS_NULL_PTR_CHECK_MAC(packetBufPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetBufLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(devNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(queuePtr);
    CPSS_NULL_PTR_CHECK_MAC(rxParamsPtr);

start_lbl:
    currPacketType = TGF_PACKET_TYPE_REGULAR_E;

    if(newGetFirst == GT_TRUE)
    {
        currentIndex = 0;
        if(retryRxToCpuAndWait)
        {
            /* wait only once , to allow context switching for all the needed packets */
            tgfTrafficGeneratorRxInCpuNumWait(currentIndex+1 , 0 , NULL);
        }
    }
    else
    {
        currentIndex++;
    }

    newGetFirst = GT_FALSE;

    rc = tgfTrafficTableRxPcktGet(currentIndex, packetBufPtr, packetBufLenPtr, packetLenPtr,
                                  devNumPtr, queuePtr, specificDeviceFormat);

    if(GT_OUT_OF_RANGE == rc)
    {
        /* table is done */
        return GT_NO_MORE;
    }
    else if(rc == GT_NOT_FOUND)
    {
        /* current index has not valid entry, but table is not done yet */
        goto start_lbl;
    }
    else if(rc != GT_OK)
    {
        return rc;
    }

#ifdef CHX_FAMILY
    dxChPcktParamsPtr = (CPSS_DXCH_NET_RX_PARAMS_STC*) specificDeviceFormat;

    if(CPSS_DXCH_NET_DSA_CMD_TO_CPU_E == dxChPcktParamsPtr->dsaParam.dsaType)
    {
        rxParamsPtr->dsaCmdIsToCpu = GT_TRUE;
        rxParamsPtr->cpuCode       = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode;
        rxParamsPtr->srcIsTrunk    = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.srcIsTrunk;

        rc = extUtilSwDeviceNumberGet(dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.hwDevNum,
                                           &rxParamsPtr->hwDevNum);
        if(GT_OK != rc)
            return rc;
        rxParamsPtr->portNum       = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.interface.portNum;
        rxParamsPtr->originByteCount = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.originByteCount;

        if((dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode == CPSS_NET_LAST_USER_DEFINED_E)||
           (dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode == CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E))
        {
            /* 'Captured' packet */
            currPacketType = TGF_PACKET_TYPE_CAPTURE_E;
        }
        /* those 2 parameters actually relevant to eDsa*/
        rxParamsPtr->trunkId       = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.interface.srcTrunkId;
        rxParamsPtr->ePortNum      = dxChPcktParamsPtr->dsaParam.dsaInfo.toCpu.interface.ePort;

    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    exMxPmPcktParamsPtr = (CPSS_EXMXPM_NET_RX_PARAMS_STC*) specificDeviceFormat;

    if(CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E == exMxPmPcktParamsPtr->dsaParam.dsaCmd)
    {
        rxParamsPtr->dsaCmdIsToCpu = GT_TRUE;
        rxParamsPtr->cpuCode       = exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode;
        rxParamsPtr->srcIsTrunk    = 0;/* trap to cpu always with port info --> never trunk */
        rc = extUtilSwDeviceNumberGet(exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.devNum,
                                           &rxParamsPtr->hwDevNum);
        if(GT_OK != rc)
            return rc;

        rxParamsPtr->portNum       = exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.portNum;

        if ((exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode ==
             CPSS_NET_LAST_USER_DEFINED_E)
            || (exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode ==
                CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E)
            || (exMxPmPcktParamsPtr->dsaParam.dsaInfo.toCpu.cpuCode ==
                CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E))
        {
            /* 'Captured' packet */
            currPacketType = TGF_PACKET_TYPE_CAPTURE_E;
        }

        rxParamsPtr->trunkId       = 0;
        rxParamsPtr->ePortNum      = 0;
    }
#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    exMxPcktParamsPtr = (CPSS_EXMX_NET_RX_PARAMS_STC*) specificDeviceFormat;

    rxParamsPtr->dsaCmdIsToCpu = GT_TRUE;
    rxParamsPtr->cpuCode       = exMxPcktParamsPtr->cpuCode;
    rxParamsPtr->srcIsTrunk    = GT_FALSE;
    rxParamsPtr->hwDevNum        = exMxPcktParamsPtr->srcDevNum;
    rxParamsPtr->portNum       = exMxPcktParamsPtr->srcPortNum;

    if(exMxPcktParamsPtr->cpuCode == CPSS_NET_LAST_USER_DEFINED_E)
    {
        /* 'Captured' packet */
        currPacketType = TGF_PACKET_TYPE_CAPTURE_E;
    }
#endif /* EX_FAMILY */

    if((currPacketType != packetType) && (packetType != TGF_PACKET_TYPE_ANY_E))
    {
        /* not matching packet type, so try to get next packet */
        goto start_lbl;
    }

    /* add the ability to trace the packet content */
    if (GT_TRUE == trace)
    {
        PRV_TGF_LOG1_MAC(
            "[TGF]: Packet received by CPU from port[%d]:\n", rxParamsPtr->portNum);
        rc = tgfTrafficTracePacket(packetBufPtr, (*packetLenPtr), GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    /* add ability to trace the packet parts fields */
    CPSS_TBD_BOOKMARK

    /* add ability to trace the Rx specific device parameters */
    CPSS_TBD_BOOKMARK

    return rc;
}
/* indication to use rxParamPtr->ePortNum or rxParamPtr->portNum :
    if(comparePortAsEPort)
    {
        portNum = rxParamPtr->ePortNum;
    }
    else
    {
        portNum = rxParamPtr->portNum;
    }
*/
static GT_U32   comparePortAsEPort = 0;
static GT_U32   forceCompareAsPhysicalPort = 0;


/*******************************************************************************
* tgfTrafficGeneratorExpectedInterfaceEport
*
* DESCRIPTION:
*       set the expected mode of 'port' to be : ePort / phyPort
*       because the CPSS_INTERFACE_INFO_STC not hold difference between eport and
*       physical port , this setting will give the functions of 'expected' packets
*       to know if expected from the eport or from the physical port.
*
*       NOTEs:
*           1. by defaults : physical port
*           2. after compares done , the default value restored automatically by the engine
* INPUTS:
*       interfaceType - the expected mode of 'port' to be : ePort / phyPort
*                   see enum for more explanations
*                  PRV_TGF_EXPECTED_INTERFACE_TYPE_EPORT_NUM_E  - 'ePort'
*                  PRV_TGF_EXPECTED_INTERFACE_TYPE_PHYSICAL_PORT_NUM_E - 'phyPort'
*                  PRV_TGF_EXPECTED_INTERFACE_TYPE_FORCE_PHYSICAL_PORT_NUM_E - force 'phyPort'
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_TRUE  - packet came from the expected interface
*       GT_FALSE - packet came from unexpected interface
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorExpectedInterfaceEport
(
    IN PRV_TGF_EXPECTED_INTERFACE_TYPE_ENT portInterfaceType
)
{
    if(TGF_CPSS_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(prvTgfCpuDevNum) == GT_TRUE)
    {
        comparePortAsEPort = (portInterfaceType == PRV_TGF_EXPECTED_INTERFACE_TYPE_EPORT_NUM_E) ? 1 : 0;
        forceCompareAsPhysicalPort = (portInterfaceType == PRV_TGF_EXPECTED_INTERFACE_TYPE_FORCE_PHYSICAL_PORT_NUM_E) ? 1 : 0;
    }
    else
    {
        /* do not modify value for devices that not support eport */
    }

    return GT_OK;
}



/*******************************************************************************
* sprvTgfTrunkDbIsMemberOfTrunk
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Checks if a member {device,port} is a trunk member.
*       if it is trunk member the function retrieve the trunkId of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       memberPtr - (pointer to) the member to check if is trunk member
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) trunk id of the port .
*                    this pointer allocated by the caller.
*                    this can be NULL pointer if the caller not require the
*                    trunkId(only wanted to know that the member belongs to a
*                    trunk)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_FOUND             - the pair {devNum,port} not a trunk member
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS sprvTgfTrunkDbIsMemberOfTrunk
(
    IN  GT_U8                   devNum,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr,
    OUT GT_TRUNK_ID             *trunkIdPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
   GT_STATUS   rc;
   CPSS_TRUNK_MEMBER_STC localMember, *localMemberPtr;

   if( NULL != memberPtr)
   {
       localMember = *memberPtr;
       rc = extUtilHwDeviceNumberGet((GT_U8)memberPtr->hwDevice, &(localMember.hwDevice));
       if (GT_OK != rc)
       {
           PRV_TGF_LOG1_MAC(
               "[TGF]: prvUtfHwDeviceNumberGet FAILED, rc = [%d]", rc);

           return rc;
       }
       localMemberPtr = &localMember;
   }
   else
       localMemberPtr = NULL;

   /* call device specific API */
#ifdef EXMXPM_FAMILY
    rc = cpssExMxPmTrunkDbIsMemberOfTrunk(devNum, localMemberPtr , trunkIdPtr);
    #else
    rc = cpssDxChTrunkDbIsMemberOfTrunk(devNum, localMemberPtr , trunkIdPtr);
#endif
    if (GT_OK != rc && rc != GT_NOT_FOUND)
    {
#ifdef EXMXPM_FAMILY
       PRV_TGF_LOG1_MAC(
           "[TGF]: cpssExMxPmTrunkDbIsMemberOfTrunk FAILED, rc = [%d]", rc);
#else
       PRV_TGF_LOG1_MAC(
           "[TGF]: cpssDxChTrunkDbIsMemberOfTrunk FAILED, rc = [%d]", rc);
#endif
    }

    return rc;

#else
    devNum = devNum;
    trunkId = trunkId;
    enablePtr = enablePtr;
    memberPtr = memberPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* checkPacketFromExpectedInterface
*
* DESCRIPTION:
*       check if packet came from the expected interface (port/eport/trunk)
*
* INPUTS:
*       portInterfacePtr - the expected interface info
*       rxParamPtr      - the info about origin of the packet
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_TRUE  - packet came from the expected interface
*       GT_FALSE - packet came from unexpected interface
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_BOOL checkPacketFromExpectedInterface
(
    IN  CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN  TGF_NET_DSA_STC                 *rxParamPtr
)
{
    GT_STATUS   rc;
    GT_TRUNK_ID     trunkId;/* trunk Id for the packet came from port that is member of */
    CPSS_TRUNK_MEMBER_STC   trunkMember;/* temporary trunk member */
    GT_U32  portNum;

    /* check the DSA tag */
    if (GT_FALSE == rxParamPtr->dsaCmdIsToCpu)
    {
        /* the packet NOT came from where I expected it to be */
        return GT_FALSE;
    }

    if(GT_TRUE == rxParamPtr->srcIsTrunk)
    {
        if(TGF_CPSS_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(prvTgfCpuDevNum) == GT_TRUE)
        {
            /* the eDSA tag supports the 'Physical port' regardless to the trunk indication */
            /* so continue ... */
        }
        else
        {
            /* the packet NOT came from where I expected it to be */
            return GT_FALSE;
        }
    }


    if(portInterfacePtr == NULL)
    {
        /* interface is not a filter issue */
    }
    else
    if(portInterfacePtr->type == CPSS_INTERFACE_PORT_E)
    {
        GT_BOOL usePhyPort;
        GT_U32 sip5_numBitsPhysicalPortInToCpuDsa_mask = 0xFF;/* 8 bits */

        if (rxParamPtr->hwDevNum != portInterfacePtr->devPort.hwDevNum)
        {
            return GT_FALSE;
        }

        if(PRV_CPSS_SIP_5_20_CHECK_MAC(prvTgfCpuDevNum))
        {
            sip5_numBitsPhysicalPortInToCpuDsa_mask = 0x1FF;/* 9 bits */
        }

        if(forceCompareAsPhysicalPort)
        {
            /* force compare as physical port */
            usePhyPort = GT_TRUE;
        }
        else
        if(comparePortAsEPort)
        {
            usePhyPort = GT_FALSE;
        }
        else
        {
            usePhyPort = GT_TRUE;
        }

        if(usePhyPort == GT_TRUE)
        {
            portNum = rxParamPtr->portNum;

            if(portNum != (portInterfacePtr->devPort.portNum/*phyPort*/ & sip5_numBitsPhysicalPortInToCpuDsa_mask))
            {
                /* the packet NOT came from where I expected it to be */
                return GT_FALSE;
            }

        }
        else
        {
            portNum = rxParamPtr->ePortNum;

            if(portNum != (portInterfacePtr->devPort.portNum/*eport*/))
            {
                /* the packet NOT came from where I expected it to be */
                return GT_FALSE;
            }
        }

    }
    else /* interface is trunk */
    {
        if(GT_TRUE == rxParamPtr->srcIsTrunk)/* this can be only in eDSA tag */
        {
            /* the eDSA tag supports the 'Physical port' regardless to the trunk indication */
            if (portInterfacePtr->trunkId != rxParamPtr->trunkId)
            {
                /* packet not from the needed trunk */
                /* the packet NOT came from where I expected it to be */
                return GT_FALSE;
            }
        }
        else
        {
            /* the trunk members still send traffic to CPU with the portNum ! */
            trunkMember.port = rxParamPtr->portNum;
            trunkMember.hwDevice = rxParamPtr->hwDevNum;

            rc = sprvTgfTrunkDbIsMemberOfTrunk(prvTgfDevNum,&trunkMember,&trunkId);
            if(rc == GT_OK)
            {
                /* the packet came from port that is member of trunk */

                if(trunkId != portInterfacePtr->trunkId)
                {
                    /* packet not from the needed trunk */
                    /* the packet NOT came from where I expected it to be */
                    return GT_FALSE;
                }
            }
            else
            {
                /* packet not from a trunk */
                /* the packet NOT came from where I expected it to be */
                return GT_FALSE;
            }
        }
    }

    return GT_TRUE;
}

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthTriggerCountersGet
*
* DESCRIPTION:
*       Get number of triggers on port/trunk
*       when 'trunk' implementation actually loops on all enabled members.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port/trunk interface
*       numVfd           - number of VFDs
*       vfdArray         - pointer to array of VFDs
*
* OUTPUTS:
*       numTriggersBmpPtr - (pointer to) bitmap of triggers
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The 'captured' (emulation of capture) must be set on this port/trunk
*       see tgfTrafficGeneratorPortTxEthCaptureSet
*
*       Triggers bitmap has the following structure:
*           N - number of captured packets on this port
*           K - number of VFDs (equal to numVfd)
*           |      Packet0     |      Packet1     |     |       PacketN    |
*           +----+----+---+----+----+----+---+----+ ... +----+----+---+----+
*           |Bit0|Bit1|...|BitK|Bit0|Bit1|...|BitK|     |Bit0|Bit1|...|BitK|
*           +----+----+---+----+----+----+---+----+     +----+----+---+----+
*       Bit[i] for Packet[j] set to 1 means that VFD[i] is matched for captured
*       packet number j.
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthTriggerCountersGet
(
    IN  CPSS_INTERFACE_INFO_STC         *portInterfacePtr,
    IN  GT_U32                           numVfd,
    IN  TGF_VFD_INFO_STC                 vfdArray[],
    OUT GT_U32                          *numTriggersBmpPtr
)
{
    GT_U8           devNum   = 0;
    GT_U8           queue    = 0;
    GT_BOOL         getFirst = GT_TRUE;
    GT_U32          i        = 0;
    GT_BOOL         match    = GT_FALSE;
    GT_STATUS       rc       = GT_OK;
    GT_STATUS       rc1      = GT_OK;
    GT_U32          buffLen  = TGF_RX_BUFFER_MAX_SIZE_CNS;
    GT_U32          packetActualLength = 0;
    GT_U32          packetCount = 0;
    GT_U8           packetBuff[TGF_RX_BUFFER_MAX_SIZE_CNS] = {0};
    TGF_NET_DSA_STC rxParam = {0};
#ifdef GM_USED
    GT_U32          maxWaitTime     = 1200; /* first packet wait time */
    GT_U32          nextWaitTime    = 200;
    GT_U32          delayTime       = 100;
#endif /*GM_USED*/

#ifdef GM_USED
    if(PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->devFamily == CPSS_PP_FAMILY_PUMA3_E)
    {
        maxWaitTime     = prvTgfGmTimeFactor*100; /* first packet wait time */
        nextWaitTime    = prvTgfGmTimeFactor*50;
        delayTime       = prvTgfGmTimeFactor*20;
    }
#endif /*GM_USED*/


    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(vfdArray);
    CPSS_NULL_PTR_CHECK_MAC(numTriggersBmpPtr);

    if (numVfd == 0)
    {
        return GT_BAD_PARAM;
    }

    if (portInterfacePtr->type != CPSS_INTERFACE_PORT_E &&
        portInterfacePtr->type != CPSS_INTERFACE_TRUNK_E)

    {
        return GT_BAD_PARAM;
    }

    (*numTriggersBmpPtr) = 0;

    while(1)
    {
        buffLen  = TGF_RX_BUFFER_MAX_SIZE_CNS;
        /* get entry from rxNetworkIf table */
        rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                           getFirst, GT_TRUE, packetBuff,
                                           &buffLen, &packetActualLength,
                                           &devNum, &queue, &rxParam);
#ifdef GM_USED
        /* wait for yet not reached packets               */
        if ((rc == GT_NO_MORE) && (maxWaitTime >= delayTime))
        {
            maxWaitTime -= delayTime;
            cpssOsTimerWkAfter(delayTime);
            continue;
        }
        /* for the fist packet we are ready to wait       */
        /* sevelal delays, for the nexts no more then one */
        maxWaitTime = nextWaitTime;
#endif /*GM_USED*/

        if (rc != GT_OK)
        {
            break;
        }

        getFirst = GT_FALSE; /* now we get the next */

        if(GT_FALSE ==
            checkPacketFromExpectedInterface(portInterfacePtr,&rxParam))
        {
            /* packet not came from expected interface */
            continue;
        }

        /* calculate number of captured packets */
        packetCount++;

        /* check the VFD parameters */
        for (i = 0; i < numVfd ;i++)
        {
            rc = tgfTrafficEnginePacketVfdCheck(
                &vfdArray[i], packetBuff, buffLen, &match);

            /* set appropriate bit to 1 if VFD is matched */
            *numTriggersBmpPtr |= ((GT_U32) match) << (i + (packetCount - 1) * numVfd);
        }
    }

    /* restore default */
    tgfTrafficGeneratorExpectedInterfaceEport(PRV_TGF_EXPECTED_INTERFACE_TYPE_PHYSICAL_PORT_NUM_E);

    return (GT_OK == rc1) ? rc : rc1;
}

/*******************************************************************************
* tgfTrafficGeneratorTxEthTriggerCheck
*
* DESCRIPTION:
*       Check if the buffer triggered by the trigger parameters
*
* INPUTS:
*       bufferPtr    - (pointer to) the buffer
*       bufferLength - length of the buffer
*       numVfd       - number of VFDs
*       vfdArray     - array of VFDs
* OUTPUTS:
*       triggeredPtr - (pointer to) triggered / not triggered
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The 'captured' (emulation of capture) must be set on this port see
*       tgfTrafficGeneratorPortTxEthCaptureSet, tgfTrafficGeneratorRxInCpuGet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorTxEthTriggerCheck
(
    IN  GT_U8                           *bufferPtr,
    IN  GT_U32                           bufferLength,
    IN  GT_U32                           numVfd,
    IN  TGF_VFD_INFO_STC                 vfdArray[],
    OUT GT_BOOL                         *triggeredPtr
)
{
    GT_BOOL     match = GT_FALSE;
    GT_U32      i     = 0;
    GT_STATUS   rc    = GT_OK;

    CPSS_NULL_PTR_CHECK_MAC(vfdArray);
    CPSS_NULL_PTR_CHECK_MAC(triggeredPtr);

    if(numVfd == 0)
    {
        return GT_BAD_PARAM;
    }

    *triggeredPtr = GT_FALSE;

    /* check the VFD parameters */
    for(i = 0; i < numVfd ;i++)
    {
        rc = tgfTrafficEnginePacketVfdCheck(&vfdArray[i], bufferPtr, bufferLength, &match);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(match != GT_TRUE)
        {
            break;
        }
    }

    if(i == numVfd)
    {
        *triggeredPtr = GT_TRUE;
    }

    return rc;
}

#ifdef CHX_FAMILY
typedef struct{
    GT_U8           devNum;
    GT_U32          portNum;
    GT_U32          bmpLookupEnabled;
}MY_CAPTURE_TTI_PORT_INFO_STC;
#define MAX_CAPTURE_PORTS_CNS   8
#define NOT_VALID_CNS   0xFFFFFFFF
static MY_CAPTURE_TTI_PORT_INFO_STC captureTtiPortInfoArr[MAX_CAPTURE_PORTS_CNS];
static GT_BOOL initDone_captureDevPortArr = GT_FALSE;

static GT_U32   ttiLookupsArr_xcat[] = {
     CPSS_DXCH_TTI_KEY_IPV4_E
    ,CPSS_DXCH_TTI_KEY_MPLS_E
    ,CPSS_DXCH_TTI_KEY_ETH_E
    ,CPSS_DXCH_TTI_KEY_MIM_E
    ,NOT_VALID_CNS
};
static GT_U32   ttiLookupsArr_eArch[] = {
     CPSS_DXCH_TTI_KEY_IPV4_E
    ,CPSS_DXCH_TTI_KEY_MPLS_E
    ,CPSS_DXCH_TTI_KEY_ETH_E
    ,CPSS_DXCH_TTI_KEY_MIM_E
    ,CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
    ,CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
    ,CPSS_DXCH_TTI_KEY_UDB_MPLS_E
    ,CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
    ,CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
    ,CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
    ,CPSS_DXCH_TTI_KEY_UDB_IPV6_E
    ,CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
    ,CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
    ,CPSS_DXCH_TTI_KEY_UDB_UDE_E
    ,CPSS_DXCH_TTI_KEY_UDB_UDE1_E
    ,CPSS_DXCH_TTI_KEY_UDB_UDE2_E
    ,CPSS_DXCH_TTI_KEY_UDB_UDE3_E
    ,CPSS_DXCH_TTI_KEY_UDB_UDE4_E
    ,CPSS_DXCH_TTI_KEY_UDB_UDE5_E
    ,CPSS_DXCH_TTI_KEY_UDB_UDE6_E
    ,NOT_VALID_CNS
};

/*******************************************************************************
* prvTgfCaptureSet_ttiLookupInfo
*
* DESCRIPTION:
*       disable any TTI lookups on this physical port ... to remove option that
*          it will ignore the 'mirror'/'ipcl' configurations
*       or
*          restore the TTI lookups on this physical port
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfCaptureSet_ttiLookupInfo
(
    IN GT_U8    devNum,
    IN GT_PORT_NUM   portNum,
    IN GT_BOOL  start
)
{
    GT_STATUS rc;
    GT_U32  ii,jj;
    GT_U32  *arrayPtr = NULL;
    GT_U32  bmpLookupEnabled = 0;
    GT_BOOL enabled;
    GT_U32  firstFreeIndex = NOT_VALID_CNS;

    if(initDone_captureDevPortArr == GT_FALSE)
    {
        for(ii = 0 ; ii < MAX_CAPTURE_PORTS_CNS ; ii++)
        {
            captureTtiPortInfoArr[ii].devNum = 0;
            captureTtiPortInfoArr[ii].portNum  = NOT_VALID_CNS;
            captureTtiPortInfoArr[ii].bmpLookupEnabled   = 0;
        }
        initDone_captureDevPortArr = GT_TRUE;
    }

    for(ii = 0 ; ii < MAX_CAPTURE_PORTS_CNS ; ii++)
    {
        if (start == GT_TRUE)
        {
            if(captureTtiPortInfoArr[ii].portNum == NOT_VALID_CNS && firstFreeIndex == NOT_VALID_CNS)
            {
                firstFreeIndex = ii;
            }

            if(captureTtiPortInfoArr[ii].portNum == portNum &&
               captureTtiPortInfoArr[ii].devNum == devNum)
            {
                /* the port already exists ?! --> ERROR in the test !!!
                   ... but it seems that there are several of those tests already !!! */
                PRV_TGF_LOG1_MAC("TGF: WARNING (potential Error) : The 'capture' on port [%d] is 'start' again without 'stop' after 'start' of previous time \n",
                    portNum);

                /* assume that we already disabled the tti lookups ... and we
                   will 'restore' it to proper values at the 'stop' */
                return GT_FAIL;
            }
        }
        else
        if (start == GT_FALSE &&
            captureTtiPortInfoArr[ii].portNum == portNum &&
            captureTtiPortInfoArr[ii].devNum == devNum)
        {
            break;
        }
    }

    if (ii == MAX_CAPTURE_PORTS_CNS)
    {
        if(start == GT_TRUE)
        {
            if (firstFreeIndex != NOT_VALID_CNS)
            {
                ii = firstFreeIndex;
            }
            else
            {
                cpssOsPrintf("TGF: The capture tti support table is full \n");
                return GT_FULL;
            }
        }
        else
        {
            cpssOsPrintf("TGF: WARNING (potential Error) : The capture tti support table not found port [%d] \n",
                portNum);

            /* assume that someone try to disabled port that already disabled*/
            return GT_FAIL;
        }
    }

    if(TGF_CPSS_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        arrayPtr = ttiLookupsArr_xcat;
    }
    else
    {
        arrayPtr = ttiLookupsArr_eArch;
    }

    if(start == GT_TRUE)
    {
        for( jj = 0 ; arrayPtr[jj] != NOT_VALID_CNS ; jj++)
        {
            /* save the lookup value */
            rc = cpssDxChTtiPortLookupEnableGet(devNum,portNum,
                (CPSS_DXCH_TTI_KEY_TYPE_ENT)arrayPtr[jj],
                &enabled);
            if (rc == GT_NOT_APPLICABLE_DEVICE)
            {
                /* for devices that does not support TTI lookup */
                enabled = GT_FALSE;
            }
            else if (rc != GT_OK)
            {
                return rc;
            }

            if(enabled == GT_TRUE)
            {
                bmpLookupEnabled |= 1 << arrayPtr[jj];

                /* disable the lookup value */
                rc = cpssDxChTtiPortLookupEnableSet(devNum,portNum,
                    (CPSS_DXCH_TTI_KEY_TYPE_ENT)arrayPtr[jj],
                    GT_FALSE);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        /* save to DB */
        captureTtiPortInfoArr[ii].devNum = devNum;
        captureTtiPortInfoArr[ii].portNum  = portNum;
        captureTtiPortInfoArr[ii].bmpLookupEnabled   = bmpLookupEnabled;
    }
    else
    {
        bmpLookupEnabled = captureTtiPortInfoArr[ii].bmpLookupEnabled;

        /* restore values */
        for( jj = 0 ; arrayPtr[jj] != NOT_VALID_CNS ; jj++)
        {
            if(bmpLookupEnabled & (1 << arrayPtr[jj]))
            {
                /* re-enable the lookup value */
                rc = cpssDxChTtiPortLookupEnableSet(devNum,portNum,
                    (CPSS_DXCH_TTI_KEY_TYPE_ENT)arrayPtr[jj],
                    GT_TRUE);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        captureTtiPortInfoArr[ii].devNum = 0;
        captureTtiPortInfoArr[ii].portNum  = NOT_VALID_CNS;
        captureTtiPortInfoArr[ii].bmpLookupEnabled   = 0;
    }


    return GT_OK;
}

#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <linux/if.h>

int g_mac_get_flag = 0;
unsigned char g_mac[6]={0};
#define APP_FAIL -1
#define APP_SUCCESS 0
static int getMac(char *interface, unsigned char *arp)
{
	  int fd;
	  struct ifreq ifr;
	  int ret = APP_FAIL;

	  memset(&ifr, 0, sizeof(struct ifreq));
	  if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
	    ifr.ifr_addr.sa_family = AF_INET;
	    strcpy(ifr.ifr_name, interface);

	    /*mac*/
	    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
	      memcpy(arp, ifr.ifr_hwaddr.sa_data, 6);
	      printf("adapter hardware address %02x:%02x:%02x:%02x:%02x:%02x \n",
	          arp[0], arp[1], arp[2], arp[3], arp[4], arp[5]);
	      ret = APP_SUCCESS;
	    } else {
	    	printf("SIOCGIFHWADDR failed, is the interface up and configured?: %s\n",
	          strerror(errno));
	      ret = APP_FAIL;
	      goto EXIT1;
	    }
	  }
	  else {
	    printf("socket failed!: %s \n", strerror(errno));
	    ret = APP_FAIL;
	    goto EXIT0;
	  }

	g_mac_get_flag = 1;

	EXIT1:
	  close(fd);
	EXIT0:
	  return ret;
}
#undef APP_FAIL
#undef APP_SUCCESS


static GT_STATUS prvTgfDxChPclCaptureSet1
(
    IN GT_U8    devNum,
    IN GT_PORT_NUM   portNum,
    IN GT_BOOL  start
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    GT_U32                             ruleIndex  = 0;
    GT_STATUS                          rc         = GT_OK;
    GT_U32                      first      = 1;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern;
    CPSS_DXCH_PCL_ACTION_STC           action;
    CPSS_INTERFACE_INFO_STC            portInterface;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC       lookupCfg;
    CPSS_DXCH_TCAM_RULE_SIZE_ENT       tcamRuleSize;

    PRV_CPSS_DXCH_PCL_CONVERT_RULE_FORMAT_TO_TCAM_RULE_SIZE_VAL_MAC(tcamRuleSize,ruleFormat);

    if(GT_TRUE == start)
    {
        if(first)
        {
            /* get default Ingress Policy state */
            rc = cpssDxChPclIngressPolicyEnableGet(devNum, &prvTgfDefIngressPolicyState);
            if (GT_OK != rc)
            {
                return rc;
            }

            /* enable Ingress Policy */
            rc = cpssDxChPclIngressPolicyEnable(devNum, GT_TRUE);
            if (GT_OK != rc)
            {
                return rc;
            }

            TGF_RESET_PARAM_MAC(mask);
            TGF_RESET_PARAM_MAC(pattern);
            TGF_RESET_PARAM_MAC(action);

            /* prepare rule index */

            ruleIndex = TGF_DXCH_LION3_TCAM_PCL_CONVRT_IDX_GET_MAC(devNum,
                                                                   TGF_PCL_RULE_INDEX_CNS_1,
                                                                   tcamRuleSize);

            /* need to match Only the PCL id */
            mask.ruleStdNotIp.common.pclId    = 0x3FF;/*10 bits exact match on this field */
            pattern.ruleStdNotIp.common.pclId = TGF_CAPTURE_PCL_ID_CNS_1;

            {
#if 0
            	pattern.ruleStdNotIp.macDa.arEther[0] = 0x01;
            	pattern.ruleStdNotIp.macDa.arEther[1] = 0x00;
            	pattern.ruleStdNotIp.macDa.arEther[2] = 0x5e;
            	pattern.ruleStdNotIp.macDa.arEther[3] = 0x64;
            	pattern.ruleStdNotIp.macDa.arEther[4] = 0xe4;
            	pattern.ruleStdNotIp.macDa.arEther[5] = 0xe4;
#else
            	pattern.ruleStdNotIp.macDa.arEther[0] = 0x00;
            	pattern.ruleStdNotIp.macDa.arEther[1] = 0x90;
            	pattern.ruleStdNotIp.macDa.arEther[2] = 0x9a;
            	pattern.ruleStdNotIp.macDa.arEther[3] = 0x9f;
            	pattern.ruleStdNotIp.macDa.arEther[4] = 0xa9;
            	pattern.ruleStdNotIp.macDa.arEther[5] = 0x3a;
#endif
            	mask.ruleStdNotIp.macDa.arEther[0]=0xff;
            	mask.ruleStdNotIp.macDa.arEther[1]=0xff;
            	mask.ruleStdNotIp.macDa.arEther[2]=0xff;
            	mask.ruleStdNotIp.macDa.arEther[3]=0xff;
            	mask.ruleStdNotIp.macDa.arEther[4]=0xff;
            	mask.ruleStdNotIp.macDa.arEther[5]=0xff;
            }

            /* mirror to CPU */
            action.pktCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

            /* set specific CPU code for those 'captured' packets */
            action.mirror.cpuCode = CPSS_NET_LAST_USER_DEFINED_E;

            /* add the rule to the PCL */
            rc = cpssDxChPclRuleSet(
                devNum, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(pclCaptureExcludeVidEnabled == GT_TRUE)
            {
                /* we have single VLAN that we not want capture to the CPU*/
                mask.ruleStdNotIp.common.vid    = 0xFFF;/*exact match on this field */
                pattern.ruleStdNotIp.common.vid = pclCaptureExcludeVid;

                cpssOsMemSet(&action,0,sizeof(action));
                action.pktCmd = CPSS_PACKET_CMD_FORWARD_E;
                action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;

                /* prepare rule index - 1*/
                ruleIndex = TGF_DXCH_LION3_TCAM_PCL_CONVRT_IDX_GET_MAC(devNum,
                                                                       TGF_PCL_RULE_INDEX_CNS - 1,
                                                                       tcamRuleSize);

                /* add the rule to the PCL -- in order prior to  the 'trap' rule ! */
                rc = cpssDxChPclRuleSet(
                    devNum, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                    &mask, &pattern, &action);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            rc = cpssDxChPclPortLookupCfgTabAccessModeSet(
                    devNum, portNum,
                    CPSS_PCL_DIRECTION_INGRESS_E,
                    CPSS_PCL_LOOKUP_1_E,
                    0,
                    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);
            if(rc != GT_OK)
            {
                return rc;
            }

            first = 0;
        }
    }
    else
    {
        /* restore default Ingress Policy state */
        rc = cpssDxChPclIngressPolicyEnable(devNum, prvTgfDefIngressPolicyState);
        if (GT_OK != rc)
        {
            return rc;
        }

        ruleIndex = TGF_DXCH_LION3_TCAM_PCL_CONVRT_IDX_GET_MAC(devNum,
                                                               TGF_PCL_RULE_INDEX_CNS,
                                                               tcamRuleSize);

        /* invalidate PCL rule */
        rc = cpssDxChPclRuleValidStatusSet(devNum, CPSS_PCL_RULE_SIZE_STD_E,
                                           ruleIndex, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(pclCaptureExcludeVidEnabled == GT_TRUE)
        {
            /* prepare rule index - 1*/
            ruleIndex = TGF_DXCH_LION3_TCAM_PCL_CONVRT_IDX_GET_MAC(devNum,
                                                                   TGF_PCL_RULE_INDEX_CNS - 1,
                                                                   tcamRuleSize);

            /* invalidate PCL rule */
            rc = cpssDxChPclRuleValidStatusSet(devNum, CPSS_PCL_RULE_SIZE_STD_E,
                                               ruleIndex, GT_FALSE);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        first = 1;
    }

    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.portNum = portNum;
    portInterface.devPort.hwDevNum  = devNum;

    TGF_RESET_PARAM_MAC(lookupCfg);

    lookupCfg.pclId        = TGF_CAPTURE_PCL_ID_CNS_1;
    lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    lookupCfg.groupKeyTypes.ipv4Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    lookupCfg.groupKeyTypes.ipv6Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    /* start - enable lookup, stop - disable lookup */
    lookupCfg.enableLookup = start;

    rc = extUtilHwDeviceNumberGet(portInterface.devPort.hwDevNum,
                                 &portInterface.devPort.hwDevNum);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssDxChPclCfgTblSet(devNum, &portInterface,
                              CPSS_PCL_DIRECTION_INGRESS_E,
                              CPSS_PCL_LOOKUP_1_E,
                              &lookupCfg);
    if (GT_OK != rc)
    {
        return rc;
    }

#if 1
    rc = cpssDxChPclPortIngressPolicyEnable(devNum, portNum, start);
#endif
    return rc;
}

/*******************************************************************************
* prvTgfDxChPclCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*           1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop 'Capture' is emulated by setting next for the port:
*           1. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
static GT_STATUS prvTgfDxChPclCaptureSet
(
    IN GT_U8    devNum,
    IN GT_PORT_NUM   portNum,
    IN GT_BOOL  start
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    GT_U32                             ruleIndex  = 0;
    GT_STATUS                          rc         = GT_OK;
    GT_U32                      first      = 1;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT      pattern;
    CPSS_DXCH_PCL_ACTION_STC           action;
    CPSS_INTERFACE_INFO_STC            portInterface;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC       lookupCfg;
    CPSS_DXCH_TCAM_RULE_SIZE_ENT       tcamRuleSize;

    rc = prvTgfDxChPclCaptureSet1(devNum, portNum, start);
    if(rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_PCL_CONVERT_RULE_FORMAT_TO_TCAM_RULE_SIZE_VAL_MAC(tcamRuleSize,ruleFormat);
    if(g_mac_get_flag == 0)
    {
  	    memset(g_mac, 0, sizeof(g_mac));
    	getMac("eth0",g_mac);
    }

    if(GT_TRUE == start)
    {
        if(first)
        {
            /* get default Ingress Policy state */
            rc = cpssDxChPclIngressPolicyEnableGet(devNum, &prvTgfDefIngressPolicyState);
            if (GT_OK != rc)
            {
                return rc;
            }

            /* enable Ingress Policy */
            rc = cpssDxChPclIngressPolicyEnable(devNum, GT_TRUE);
            if (GT_OK != rc)
            {
                return rc;
            }

            TGF_RESET_PARAM_MAC(mask);
            TGF_RESET_PARAM_MAC(pattern);
            TGF_RESET_PARAM_MAC(action);

            /* prepare rule index */

            ruleIndex = TGF_DXCH_LION3_TCAM_PCL_CONVRT_IDX_GET_MAC(devNum,
                                                                   TGF_PCL_RULE_INDEX_CNS,
                                                                   tcamRuleSize);

            /* need to match Only the PCL id */
            mask.ruleStdNotIp.common.pclId    = 0x3FF;/*10 bits exact match on this field */
            pattern.ruleStdNotIp.common.pclId = TGF_CAPTURE_PCL_ID_CNS;

            /*if(g_mac_get_flag == 1)*/
            {
            	/*memcpy(pattern.ruleStdNotIp.macDa.arEther, g_mac, sizeof(g_mac));*/
            	pattern.ruleStdNotIp.macDa.arEther[0]=0x00;
            	pattern.ruleStdNotIp.macDa.arEther[1]=0x90;
            	pattern.ruleStdNotIp.macDa.arEther[2]=0x9A;
            	pattern.ruleStdNotIp.macDa.arEther[3]=0x9F;
            	pattern.ruleStdNotIp.macDa.arEther[4]=0xA9;
            	pattern.ruleStdNotIp.macDa.arEther[5]=0x3A;

            }

            /* mirror to CPU */
            action.pktCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

            /* set specific CPU code for those 'captured' packets */
            action.mirror.cpuCode = CPSS_NET_LAST_USER_DEFINED_E;

            /* add the rule to the PCL */
            rc = cpssDxChPclRuleSet(
                devNum, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                &mask, &pattern, &action);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(pclCaptureExcludeVidEnabled == GT_TRUE)
            {
                /* we have single VLAN that we not want capture to the CPU*/
                mask.ruleStdNotIp.common.vid    = 0xFFF;/*exact match on this field */
                pattern.ruleStdNotIp.common.vid = pclCaptureExcludeVid;

                cpssOsMemSet(&action,0,sizeof(action));
                action.pktCmd = CPSS_PACKET_CMD_FORWARD_E;
                action.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;

                /* prepare rule index - 1*/
                ruleIndex = TGF_DXCH_LION3_TCAM_PCL_CONVRT_IDX_GET_MAC(devNum,
                                                                       TGF_PCL_RULE_INDEX_CNS - 1,
                                                                       tcamRuleSize);

                /* add the rule to the PCL -- in order prior to  the 'trap' rule ! */
                rc = cpssDxChPclRuleSet(
                    devNum, ruleFormat, ruleIndex, 0 /*ruleOptionsBmp*/,
                    &mask, &pattern, &action);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            rc = cpssDxChPclPortLookupCfgTabAccessModeSet(
                    devNum, portNum,
                    CPSS_PCL_DIRECTION_INGRESS_E,
                    CPSS_PCL_LOOKUP_0_E,
                    0,
                    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E);
            if(rc != GT_OK)
            {
                return rc;
            }

            first = 0;
        }
    }
    else
    {
        /* restore default Ingress Policy state */
        rc = cpssDxChPclIngressPolicyEnable(devNum, prvTgfDefIngressPolicyState);
        if (GT_OK != rc)
        {
            return rc;
        }

        ruleIndex = TGF_DXCH_LION3_TCAM_PCL_CONVRT_IDX_GET_MAC(devNum,
                                                               TGF_PCL_RULE_INDEX_CNS,
                                                               tcamRuleSize);

        /* invalidate PCL rule */
        rc = cpssDxChPclRuleValidStatusSet(devNum, CPSS_PCL_RULE_SIZE_STD_E,
                                           ruleIndex, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(pclCaptureExcludeVidEnabled == GT_TRUE)
        {
            /* prepare rule index - 1*/
            ruleIndex = TGF_DXCH_LION3_TCAM_PCL_CONVRT_IDX_GET_MAC(devNum,
                                                                   TGF_PCL_RULE_INDEX_CNS - 1,
                                                                   tcamRuleSize);

            /* invalidate PCL rule */
            rc = cpssDxChPclRuleValidStatusSet(devNum, CPSS_PCL_RULE_SIZE_STD_E,
                                               ruleIndex, GT_FALSE);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        first = 1;
    }

    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.portNum = portNum;
    portInterface.devPort.hwDevNum  = devNum;

    TGF_RESET_PARAM_MAC(lookupCfg);

    lookupCfg.pclId        = TGF_CAPTURE_PCL_ID_CNS;
    lookupCfg.groupKeyTypes.nonIpKey = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    lookupCfg.groupKeyTypes.ipv4Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    lookupCfg.groupKeyTypes.ipv6Key  = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;
    /* start - enable lookup, stop - disable lookup */
    lookupCfg.enableLookup = start;

    rc = extUtilHwDeviceNumberGet(portInterface.devPort.hwDevNum,
                                 &portInterface.devPort.hwDevNum);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssDxChPclCfgTblSet(devNum, &portInterface,
                              CPSS_PCL_DIRECTION_INGRESS_E,
                              CPSS_PCL_LOOKUP_0_E,
                              &lookupCfg);
    if (GT_OK != rc)
    {
        return rc;
    }


    rc = cpssDxChPclPortIngressPolicyEnable(devNum, portNum, start);

    return rc;
}

/* flag allow to put conditional break point in cpssDxChBrgVlanPortVidSet to
   skip cases that called with pvid == 0*/
/* !! extern !!*/ GT_U32 setPvidFrom_prvTgfDxChMirroringCaptureSet = 0;

/*******************************************************************************
* prvTgfDxChMirroringCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port.
*       Temporary function for DxChXcat A1 that uses Mirroring for packet
*       capture, while PCL development in progress.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*           1. Enable mirroring for this port.
*           2. Drop all packets by forcing PVID = 0.
*           3. Set CPU port as analyzer port.
*
*       The Stop 'Capture' is emulated by setting next for the port:
*           1. Disable mirroring for this port.
*           2. Disable Force PVID.
*
*******************************************************************************/
static GT_STATUS prvTgfDxChMirroringCaptureSet
(
    IN GT_U8    devNum,
    IN GT_PORT_NUM   portNum,
    IN GT_BOOL  start
)
{
    GT_STATUS       rc = GT_OK, rc1;
    GT_U16          hwValue;
    CPSS_PP_FAMILY_TYPE_ENT  devFamily;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC analyzerInf;
    GT_HW_DEV_NUM   hwDevNum;
    GT_U32  ii;

    /*******************************/
    /* Configure Mirroring to CPU  */
    /*******************************/

    if(GT_TRUE == start)
    {
        for(ii = 0 ; ii < MAX_MIRROR_PORTS_CNS; ii ++)
        {
            if(prvTgfGlobalPvidDatabaseArr[ii].isValid == GT_FALSE)
            {
                /* a port must be removed before added again */
                /* so we not look for it as 'Already exists' */

                /* give the port free index */
                break;
            }
        }

        if(ii == MAX_MIRROR_PORTS_CNS)
        {
            /* no free ports in DB */
            return GT_FULL;
        }

        /* get current PVID */
        rc = cpssDxChBrgVlanPortVidGet(devNum,
                                       portNum,
                                       CPSS_DIRECTION_INGRESS_E,
                                       &hwValue);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* store current PVID value */
        prvTgfGlobalPvidDatabaseArr[ii].pvid = hwValue;
        prvTgfGlobalPvidDatabaseArr[ii].devNum = devNum;
        prvTgfGlobalPvidDatabaseArr[ii].portNum = portNum;
        prvTgfGlobalPvidDatabaseArr[ii].isValid  = GT_TRUE;

        setPvidFrom_prvTgfDxChMirroringCaptureSet = 1;

        /* set PVID = 0 for the port */
        rc = cpssDxChBrgVlanPortVidSet(devNum,
                                       portNum,
                                       CPSS_DIRECTION_INGRESS_E,
                                       0);

        setPvidFrom_prvTgfDxChMirroringCaptureSet = 0;
        if (GT_OK != rc)
        {
            return rc;
        }

        /* set force PVID for all packets in the port */
        rc = cpssDxChBrgVlanForcePvidEnable(devNum,
                                            portNum,
                                            GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }

        DEV_CHECK_MAC(devNum);
        devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;

        /* call device specific API */
        if (devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            rc = extUtilHwDeviceNumberGet(devNum, &hwDevNum);
            if(GT_OK != rc)
                return rc;
            rc = cpssDxChMirrorRxAnalyzerPortSet(devNum,
                                                 CPSS_CPU_PORT_NUM_CNS,
                                                 hwDevNum);
        }
        else
        {
            analyzerInf.interface.type = CPSS_INTERFACE_PORT_E;

            rc = extUtilHwDeviceNumberGet(devNum,
                                         &analyzerInf.interface.devPort.hwDevNum);
            if(GT_OK != rc)
                return rc;
            analyzerInf.interface.devPort.portNum = CPSS_CPU_PORT_NUM_CNS;

            /* use interface 0 for Rx analyzer by default */
            rc = cpssDxChMirrorAnalyzerInterfaceSet(devNum, 0, &analyzerInf);
        }

        /* enable mirroring */
        rc = cpssDxChMirrorRxPortSet(devNum,
                                     portNum,
                                     GT_FALSE, /* use ePort for Lion2 */
                                     GT_TRUE,
                                     0);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    else
    {
        /* disable mirroring */
        rc1 = cpssDxChMirrorRxPortSet(devNum,
                                     portNum,
                                     GT_FALSE, /* use ePort for Lion2 */
                                     GT_FALSE,
                                     0);
        if (GT_OK != rc1)
        {
            rc = rc1;
        }

        /* disable force PVID */
        rc1 = cpssDxChBrgVlanForcePvidEnable(devNum,
                                            portNum,
                                            GT_FALSE);
        if (GT_OK != rc1)
        {
            rc = rc1;
        }

        for(ii = 0 ; ii < MAX_MIRROR_PORTS_CNS; ii ++)
        {
            if(prvTgfGlobalPvidDatabaseArr[ii].isValid == GT_FALSE)
            {
                /* look for the port in the DB */
                continue;
            }

            if(prvTgfGlobalPvidDatabaseArr[ii].portNum == portNum &&
               prvTgfGlobalPvidDatabaseArr[ii].devNum == devNum )
            {
                /* port was found */
                break;
            }
        }

        if(ii == MAX_MIRROR_PORTS_CNS)
        {
            /* not found ?! */
            return GT_NOT_FOUND;
        }

        hwValue = prvTgfGlobalPvidDatabaseArr[ii].pvid;

        prvTgfGlobalPvidDatabaseArr[ii].isValid = GT_FALSE;

        /* restore PVID */
        rc = cpssDxChBrgVlanPortVidSet(devNum,
                                       portNum,
                                       CPSS_DIRECTION_INGRESS_E,
                                       hwValue);
        if (GT_OK != rc1)
        {
            rc = rc1;
        }

    }

    return rc;
}

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfExMxPmPclCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong interface
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*           1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop 'Capture' is emulated by setting next for the port:
*           1. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
static GT_STATUS prvTgfExMxPmPclCaptureSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  start
)
{
    CPSS_EXMXPM_TCAM_TYPE_ENT             tcamType;
    CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT     actionFormat;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT ruleInfo;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT       mask;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT       pattern;
    CPSS_EXMXPM_PCL_ACTION_UNT            action;
    CPSS_INTERFACE_INFO_STC               portInterface;
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC        lookupCfg;
    GT_STATUS                             rc;
    static GT_U32   first = 1;


    /* setup Ingress Policy for capturing */
    if (GT_TRUE == start)
    {
        if (first)
        {
            /* get default Ingress Policy state */
            rc = cpssExMxPmPclPolicyEnableGet(devNum, CPSS_PCL_DIRECTION_INGRESS_E,
                                              &prvTgfDefIngressPolicyState);
            if (GT_OK != rc)
            {
                return rc;
            }

            /* enable Ingress Policy */
            rc = cpssExMxPmPclPolicyEnableSet(devNum, CPSS_PCL_DIRECTION_INGRESS_E, GT_TRUE);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
    }
    else
    {
        /* restore default Ingress Policy state */
        rc = cpssExMxPmPclPolicyEnableSet(devNum, CPSS_PCL_DIRECTION_INGRESS_E,
                                          prvTgfDefIngressPolicyState);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    if ((first) || (GT_FALSE == start))
    {
        TGF_RESET_PARAM_MAC(mask);
        TGF_RESET_PARAM_MAC(pattern);
        TGF_RESET_PARAM_MAC(action);

        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;

        ruleInfo.internalTcamRuleStartIndex = TGF_PCL_RULE_INDEX_CNS;

        ruleFormat   = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
        actionFormat = CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;

        /* need to match Only the PCL id */
        mask.ruleIngrStandardNotIp.commonIngrStd.pclId    = 0x3FF;
        pattern.ruleIngrStandardNotIp.commonIngrStd.pclId = TGF_CAPTURE_PCL_ID_CNS;

        /* trap to CPU */
        action.ingressStandard.pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        /* set specific CPU code for those 'captured' packets */
        action.ingressStandard.mirror.cpuCode = CPSS_NET_LAST_USER_DEFINED_E;

        /* add the rule to the PCL */
        rc = cpssExMxPmPclRuleSet(devNum, tcamType, &ruleInfo, ruleFormat,
                                  actionFormat, start, &mask, &pattern, &action);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(pclCaptureExcludeVidEnabled == GT_TRUE)
        {
            /* we have single VLAN that we not want capture to the CPU*/
            mask.ruleIngrStandardNotIp.commonIngrStd.vid    = 0xFFF;/*exact match on this field */
            pattern.ruleIngrStandardNotIp.commonIngrStd.vid = pclCaptureExcludeVid;

            cpssOsMemSet(&action,0,sizeof(action));
            action.ingressStandard.pktCmd = CPSS_PACKET_CMD_FORWARD_E;
            action.ingressStandard.redirect.redirectCmd = CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_NONE_E;

            /* add the rule to the PCL -- in order prior to  the 'trap' rule ! */
            ruleInfo.internalTcamRuleStartIndex--;

            rc = cpssExMxPmPclRuleSet(devNum, tcamType, &ruleInfo, ruleFormat,
                                      actionFormat, start, &mask, &pattern, &action);
            if(rc != GT_OK)
            {
                return rc;
            }

            ruleInfo.internalTcamRuleStartIndex++;/*restore value*/
        }

        first = !start;
    }

    /* configure access accessMode */

    rc = cpssExMxPmPclPortLookupCfgTblAccessModeSet(
        devNum, portNum, CPSS_PCL_DIRECTION_INGRESS_E,
        CPSS_PCL_LOOKUP_0_E,
        CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_BY_PORT_E);
    if (GT_OK != rc)
    {
        PRV_TGF_LOG1_MAC(
            "[TGF]: cpssExMxPmPclPortLookupCfgTblAccessModeSet FAILED"
            ", rc = [%d]", rc);

        return rc;
    }

    portInterface.type = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.portNum = portNum;
    portInterface.devPort.hwDevNum  = devNum;

    TGF_RESET_PARAM_MAC(lookupCfg);

    lookupCfg.enableLookup = start;
        lookupCfg.pclId        = TGF_CAPTURE_PCL_ID_CNS;

        lookupCfg.groupKeyTypes.nonIpKey = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
        lookupCfg.groupKeyTypes.ipv4Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;
        lookupCfg.groupKeyTypes.ipv6Key  = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;

    rc = cpssExMxPmPclCfgTblSet(devNum, &portInterface,
                                CPSS_PCL_DIRECTION_INGRESS_E,
                                CPSS_PCL_LOOKUP_0_E,
                                &lookupCfg);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssExMxPmPclPortIngressPolicyEnableSet(devNum, portNum, start);

    return rc;
}

/*******************************************************************************
* prvTgfExMxPmMirroringCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port.
*       Temporary function for Puma A1 that uses Mirroring for packet
*       capture, while PCL development in progress.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*           1. Enable mirroring for this port.
*           2. Drop all packets by forcing PVID = 0.
*           3. Set CPU port as analyzer port.
*
*       The Stop 'Capture' is emulated by setting next for the port:
*           1. Disable mirroring for this port.
*           2. Disable Force PVID.
*
*******************************************************************************/
static GT_STATUS prvTgfExMxPmMirroringCaptureSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  start
)
{
    GT_STATUS                   rc = GT_OK, rc1;
    GT_U16                      hwValue;
    CPSS_EXMXPM_OUTLIF_INFO_STC analyzerInf;
    GT_U32                      ii;

    /*******************************/
    /* Configure Mirroring to CPU  */
    /*******************************/

    if(GT_TRUE == start)
    {
        for(ii = 0 ; ii < MAX_MIRROR_PORTS_CNS; ii ++)
        {
            if (prvTgfGlobalPvidDatabaseArr[ii].isValid == GT_FALSE)
            {
                /* a port must be removed before added again */
                /* so we not look for it as 'Already exists' */

                /* give the port free index */
                break;
            }
        }

        if (ii == MAX_MIRROR_PORTS_CNS)
        {
            /* no free ports in DB */
            return GT_FULL;
        }

        /* get current PVID */
        rc = cpssExMxPmBrgVlanPortPvidGet(
            devNum, portNum, &hwValue);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* store current PVID value */
        prvTgfGlobalPvidDatabaseArr[ii].pvid = hwValue;
        prvTgfGlobalPvidDatabaseArr[ii].devNum = devNum;
        prvTgfGlobalPvidDatabaseArr[ii].portNum = portNum;
        prvTgfGlobalPvidDatabaseArr[ii].isValid  = GT_TRUE;

        /* set PVID = 0 for the port */
        rc = cpssExMxPmBrgVlanPortPvidSet(
            devNum, portNum, 0);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* set force PVID for all packets in the port */
        rc = cpssExMxPmBrgVlanPortForcePvidEnableSet(
            devNum, portNum, GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }

        analyzerInf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        analyzerInf.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        analyzerInf.interfaceInfo.devPort.portNum =
            CPSS_CPU_PORT_NUM_CNS;
        rc = extUtilHwDeviceNumberGet(
            devNum, &analyzerInf.interfaceInfo.devPort.hwDevNum);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* interface index not relevant for Rx analyzer */
        rc = cpssExMxPmMirrorDestInterfaceSet(
            devNum, CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E,
            0, &analyzerInf);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* enable mirroring */
        rc = cpssExMxPmMirrorPortEnableSet(
            devNum, portNum,
            CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E, GT_TRUE);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    else
    {
        /* disable mirroring */
        rc1 = cpssExMxPmMirrorPortEnableSet(
            devNum, portNum,
            CPSS_EXMXPM_MIRROR_TYPE_INGRESS_E, GT_FALSE);
        if (GT_OK != rc1)
        {
            rc = rc1;
        }

        /* disable force PVID */
        rc1 = cpssExMxPmBrgVlanPortForcePvidEnableSet(
            devNum, portNum, GT_FALSE);
        if (GT_OK != rc1)
        {
            rc = rc1;
        }

        for(ii = 0 ; ii < MAX_MIRROR_PORTS_CNS; ii ++)
        {
            if(prvTgfGlobalPvidDatabaseArr[ii].isValid == GT_FALSE)
            {
                /* look for the port in the DB */
                continue;
            }

            if(prvTgfGlobalPvidDatabaseArr[ii].portNum == portNum &&
               prvTgfGlobalPvidDatabaseArr[ii].devNum == devNum )
            {
                /* port was found */
                break;
            }
        }

        if (ii == MAX_MIRROR_PORTS_CNS)
        {
            /* not found ?! */
            return GT_NOT_FOUND;
        }

        hwValue = prvTgfGlobalPvidDatabaseArr[ii].pvid;

        prvTgfGlobalPvidDatabaseArr[ii].isValid = GT_FALSE;

        /* restore PVID */
        rc1 = cpssExMxPmBrgVlanPortPvidSet(
            devNum, portNum, hwValue);
        if (GT_OK != rc1)
        {
            rc = rc1;
        }
    }

    return rc;
}

#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
/*******************************************************************************
* prvTgfExMxPclCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       start   - start/stop capture
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*           1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop 'Capture' is emulated by setting next for the port:
*           1. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
static GT_STATUS prvTgfExMxPclCaptureSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  start
)
{
    CPSS_EXMX_PCL_RULE_FORMAT_ENT      ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
    GT_U32                             ruleIndex  = 0;
    GT_STATUS                          rc         = GT_OK;
    static GT_U32                      first      = 1;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      pattern;
    CPSS_EXMX_POLICY_ACTION_STC        action;

    if(GT_TRUE == start)
    {
        if(first)
        {
            TGF_RESET_PARAM_MAC(mask);
            TGF_RESET_PARAM_MAC(pattern);
            TGF_RESET_PARAM_MAC(action);

            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
            ruleIndex  = TGF_PCL_RULE_INDEX_CNS;

            /* need to match Only the PCL id */
            mask.tgStd.pclId    = (GT_U8)0xFF;/*exact match on this field */
            pattern.tgStd.pclId = (GT_U8)TGF_CAPTURE_PCL_ID_CNS;

            /* trap to CPU */
            action.pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

            /* set specific CPU code for those 'captured' packets */
            action.mirror.mirrorToCpu = GT_TRUE;

            /* add the rule to the PCL */
            rc = cpssExMxPclRuleSet(devNum, ruleFormat, ruleIndex, &mask, &pattern, &action);
            if(rc != GT_OK)
            {
                return rc;
            }

            first = 0;
        }
    }

    return rc;
}
#endif /* EX_FAMILY */

/* for debug force specific capture mode , with out changing the test */
static GT_BOOL              forceCaptureModeEnabled = GT_FALSE;
static TGF_CAPTURE_MODE_ENT forceCaptureMode = TGF_CAPTURE_MODE_PCL_E;
/* debug function to force capture mode */
extern GT_STATUS prvTgfCaptureForceModeEnableSet(
    IN GT_BOOL                  enable ,
    IN TGF_CAPTURE_MODE_ENT     forcedMode
)
{
    if(enable != GT_FALSE &&
        forcedMode != TGF_CAPTURE_MODE_MIRRORING_E &&
        forcedMode != TGF_CAPTURE_MODE_PCL_E)
    {
        return GT_BAD_PARAM;
    }

    cpssOsPrintf("The capture force mode is : [%s] (was [%s]) \n" ,
        enable                  == GT_FALSE ? "disabled" : "enabled",
        forceCaptureModeEnabled == GT_FALSE ? "disabled" : "enabled");

    if(enable != GT_FALSE)
    {
        cpssOsPrintf("New mode is : [%s] (was [%s]) \n" ,
            forcedMode       == TGF_CAPTURE_MODE_PCL_E ? "PCL" : "Rx Mirroring",
            forceCaptureMode == TGF_CAPTURE_MODE_PCL_E ? "PCL" : "Rx Mirroring");
    }

    forceCaptureModeEnabled = enable;
    forceCaptureMode        = forcedMode;

    return GT_OK;
}

/*******************************************************************************
* prvTgfCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       mode             - packet capture mode
*       start            - start/stop capture on this port.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface, mode
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start 'Capture' is emulated by setting next for the port:
*       1. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*          we will assign it with 'Application specific cpu code' to
*          distinguish from other traffic go to the CPU
*
*       The Stop 'Capture' is emulated by setting next for the port:
*       2. remove the ingress PCL rule that function
*          tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
static GT_STATUS prvTgfCaptureSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN TGF_CAPTURE_MODE_ENT     mode,
    IN GT_BOOL                  start
)
{

    if(forceCaptureModeEnabled == GT_TRUE)
    {
        /* for debug force specific capture mode , with out changing the test */
        mode = forceCaptureMode;
    }


#ifdef CHX_FAMILY
#if 0
    {
        {
            GT_STATUS rc;
            /* disable any TTI lookups on this physical port ... to remove option that
               it will ignore the 'mirror'/'ipcl' configurations
               or
               restore the TTI lookups on this physical port */
            rc = prvTgfCaptureSet_ttiLookupInfo(
                (GT_U8)portInterfacePtr->devPort.hwDevNum,
                portInterfacePtr->devPort.portNum,
                start);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }
#endif
    switch (mode)
    {
        /* need to remove casting and to check code after lion2 development is done */
        CPSS_TBD_BOOKMARK_EARCH

        case TGF_CAPTURE_MODE_PCL_E:
            return  prvTgfDxChPclCaptureSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                           portInterfacePtr->devPort.portNum,
                                           start);
        case TGF_CAPTURE_MODE_MIRRORING_E:
            return  prvTgfDxChMirroringCaptureSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                                 portInterfacePtr->devPort.portNum,
                                                 start);
        default:
            return GT_BAD_PARAM;
    }



#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY

    switch (mode)
    {
        case TGF_CAPTURE_MODE_MIRRORING_E:
            return prvTgfExMxPmMirroringCaptureSet(
                (GT_U8)portInterfacePtr->devPort.hwDevNum,
                (GT_U8)portInterfacePtr->devPort.portNum,
                start);

        case TGF_CAPTURE_MODE_PCL_E:
            return prvTgfExMxPmPclCaptureSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                             (GT_U8)portInterfacePtr->devPort.portNum,
                                             start);
        default:
            return GT_BAD_PARAM;
    }

#endif /* EXMXPM_FAMILY */

#ifdef EX_FAMILY
    switch (mode)
    {
        case TGF_CAPTURE_MODE_PCL_E:
            return prvTgfExMxPclCaptureSet((GT_U8)portInterfacePtr->devPort.hwDevNum,
                                           (GT_U8)portInterfacePtr->devPort.portNum,
                                           start);
        default:
            return GT_BAD_PARAM;
    }

#endif /*EX_FAMILY*/

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY)
    TGF_PARAM_NOT_USED(portInterfacePtr);
    TGF_PARAM_NOT_USED(start);
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) && !(defined EX_FAMILY) */
}

/*******************************************************************************
* portTxEthCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port
*
* INPUTS:
*       portInterfacePtr - (pointer to) port interface
*       mode             - packet capture mode
*       start            - start/stop capture on this port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start capture is emulated by setting next for the port:
*           1. loopback on the port (so all traffic that egress the port will
*              ingress the same way)
*           2. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop capture is emulated by setting next for the port:
*           1. disable loopback on the port
*           2. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
static GT_STATUS portTxEthCaptureSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN TGF_CAPTURE_MODE_ENT      mode,
    IN GT_BOOL                   start
)
{
    GT_STATUS rc = GT_OK;
    CPSS_INTERFACE_INFO_STC  localPortInterface;

    CPSS_NULL_PTR_CHECK_MAC(portInterfacePtr);

    if(portInterfacePtr->type != CPSS_INTERFACE_PORT_E)
    {
        return GT_BAD_PARAM;
    }

    localPortInterface = *portInterfacePtr;

    portInterfacePtr = NULL;/* make sure not to use it any more ... only localPortInterface */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(localPortInterface.devPort.hwDevNum))
    {
        /* was not initialized properly ...
           WA: to not modify a lot of tests */
        localPortInterface.devPort.hwDevNum = prvTgfDevNum;
    }

    if (GT_FALSE == start)
    {
        GT_U32    loops;
#ifndef  EMBEDDED
        /* let the packets be captured */
        loops = prvTgfCaptureTimeLoop;
#else
        /* one sleep is enough for HW */
        loops = 1;
#endif
        while(loops--)/* allow debugging by set this value = 1000 */
        {
            cpssOsTimerWkAfter(TGF_DEFAULT_CAPTURE_SLEEP_TIME_CNS);
        }

#if defined(ASIC_SIMULATION) && !defined(GM_USED)
        {
            if(skernelNumOfPacketsInTheSystemGet())
            {
                loops = 1000;/* up to 10 sec */
                while(loops--)
                {
                    cpssOsTimerWkAfter(10);
                    if(skernelNumOfPacketsInTheSystemGet() == 0)
                    {
                        break;
                    }
                }
            }
        }
#endif /*ASIC_SIMULATION*/

        if(tgfTrafficGeneratorRxCaptureNum)
        {
            tgfTrafficGeneratorRxCaptureNum--;
        }
        else
        {
            /* error */
        }

        if(txEthCaptureDoLoopback == GT_TRUE)
        {
            /* Disable Port Loopback in the begging of configuration for DISABLE case.
               This is proper position for Capture Enable/Disable under traffic. */
            rc = tgfTrafficGeneratorPortLoopbackModeEnableSet(&localPortInterface, start);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
    }
    else
    {
        tgfTrafficGeneratorRxCaptureNum++;
    }

    rc = prvTgfCaptureSet(&localPortInterface, mode, start);
    if (GT_OK != rc)
    {
        return rc;
    }

#ifdef IMPL_GALTIS
    /* notify the 'galtis tables' of 'rx packet to CPU' that the enhUT uses
       'capture' with specific CPU codes */
    if (tgfCmdCpssNetEnhUtUseCaptureToCpuPtr != NULL)
    {
        *tgfCmdCpssNetEnhUtUseCaptureToCpuPtr = start;
        rc = GT_OK;
    }
    else
    {
        rc = GT_NOT_IMPLEMENTED;
    }
    if (GT_OK != rc)
    {
        return rc;
    }
#endif /*IMPL_GALTIS*/

    rc = prvTgfTrafficTableRxStartCapture(start);
    if (GT_OK != rc)
    {
        return rc;
    }

    if(txEthCaptureDoLoopback == GT_TRUE)
    {
        /* Enable Port Loopback only in the end of configuration for ENABLE case.
           This is proper position for Capture Enable/Disable under traffic. */
        if (GT_FALSE != start)
        {
            rc = tgfTrafficGeneratorPortLoopbackModeEnableSet(&localPortInterface, start);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
    }


    return rc;
}
/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCaptureSet
*
* DESCRIPTION:
*       Start/Stop capture the traffic that egress the port/trunk
*       when 'trunk' implementation actually loops on all enabled members.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port/trunk interface
*       mode             - packet capture mode
*       start            - start/stop capture on this port/trunk
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       The Start capture is emulated by setting next for the port:
*           1. loopback on the port (so all traffic that egress the port will
*              ingress the same way)
*           2. ingress PCL rule for ALL traffic from this port to TRAP to the CPU
*              we will assign it with 'Application specific cpu code' to
*              distinguish from other traffic go to the CPU
*
*       The Stop capture is emulated by setting next for the port:
*           1. disable loopback on the port
*           2. remove the ingress PCL rule that function
*              tgfTrafficGeneratorPortTxEthCaptureStart added
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthCaptureSet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN TGF_CAPTURE_MODE_ENT      mode,
    IN GT_BOOL                   start
)
{
    GT_U32                  numOfEnabledMembers;
    CPSS_TRUNK_MEMBER_STC   enabledMembersArray[PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                  ii;
    CPSS_INTERFACE_INFO_STC tmpInterface;
    GT_STATUS rc,rc1 = GT_OK;

    if (portInterfacePtr->devPort.portNum == NOT_VALID_CNS)
    {
        /* do capture of traffic trapped/mirrored to CPU due to test configuration without interventton of traffic generator */
        prvTgfTrafficTableRxStartCapture(start);
        return GT_OK;
    }

    if (portInterfacePtr->type == CPSS_INTERFACE_TRUNK_E) 
    {
        /* get all trunk members */
        numOfEnabledMembers = PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
        rc = prvCpssGenericTrunkDbEnabledMembersGet(prvTgfDevNum,portInterfacePtr->trunkId,&numOfEnabledMembers,enabledMembersArray);
        if(rc != GT_OK)
        {
            PRV_TGF_LOG2_MAC("prvCpssGenericTrunkDbEnabledMembersGet: %d %d",
                              prvTgfDevNum, portInterfacePtr->trunkId);
            return rc;
        }

        if(numOfEnabledMembers == 0)
        {
            /* empty trunk ??? */
            PRV_TGF_LOG0_MAC("[TGF]: tgfTrafficGeneratorPortTxEthCaptureSet no trunk members");
            return GT_EMPTY;
        }

        /*set all trunk members as 'captured' start/stop */
        tmpInterface.type = CPSS_INTERFACE_PORT_E;
        for(ii = 0 ; ii < numOfEnabledMembers; ii++)
        {
            tmpInterface.devPort.portNum = enabledMembersArray[ii].port;
            tmpInterface.devPort.hwDevNum = enabledMembersArray[ii].hwDevice;

            rc = extUtilSwDeviceNumberGet(tmpInterface.devPort.hwDevNum,
                                          &tmpInterface.devPort.hwDevNum);
            if(rc != GT_OK)
            {
                rc1 = rc;
            }

            rc = portTxEthCaptureSet(&tmpInterface,mode,start);
            if(rc != GT_OK)
            {
                rc1 = rc;
            }
        }

        return rc1;
    }

    return portTxEthCaptureSet(portInterfacePtr,mode,start);
}

/*******************************************************************************
* tgfTrafficGeneratorLoopTimeSet
*
* DESCRIPTION:
*       Set sleeping time after sending burst of packets from the CPU
*
* INPUTS:
*       timeInSec - time in seconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorLoopTimeSet
(
    IN GT_U32   timeInSec
)
{
    if(timeInSec)
    {
        prvTgfTimeLoop = timeInSec * 2;
    }
    else
    {
#ifdef  EMBEDDED
        /* run on hardware */
        prvTgfTimeLoop = 1;
#else
        /* run on simulation */
        prvTgfTimeLoop = TGF_DEFAULT_TIME_LOOP_NUM_CNS;
#endif
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorGmTimeFactorSet
*
* DESCRIPTION:
*       Set time factor for GM devices , due to very slow processing of
*       packets (2 per second)
*
* INPUTS:
*       timeFactor - factor for the time
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorGmTimeFactorSet
(
    IN GT_U32   timeFactor
)
{
    if(timeFactor == 0)
    {
        timeFactor = 1;
    }

    prvTgfGmTimeFactor = timeFactor;

    return GT_OK;
}


/*******************************************************************************
* tgfTrafficGeneratorCaptureLoopTimeSet
*
* DESCRIPTION:
*       Set sleeping time before disabling packet's capturing
*
* INPUTS:
*       timeInMSec - time in miliseconds (0 for restore defaults)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       Needed for debugging purposes
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorCaptureLoopTimeSet
(
    IN GT_U32   timeInMSec
)
{
    prvTgfCaptureTimeLoop = (timeInMSec) ? (timeInMSec / TGF_DEFAULT_CAPTURE_SLEEP_TIME_CNS) : TGF_DEFAULT_CAPTURE_LOOP_NUM_CNS;

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorEtherTypeForVlanTagSet
*
* DESCRIPTION:
*       set etherType to recognize 'Tagged packes' that send from the traffic
*       generator to the PP
*
* INPUTS:
*       etherType - new etherType
*                   (was initialized as 0x8100)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID tgfTrafficGeneratorEtherTypeForVlanTagSet
(
    IN GT_U16   etherType
)
{
    etherTypeForVlanTag = etherType;
}

/*******************************************************************************
* internalAutoChecksumField
*
* DESCRIPTION:
*       apply auto checksum fields after applying the VFD
*
* INPUTS:
*       checksumInfoArr - (array of) info about fields that need auto checksum build
*       checksumExtraInfoPtr -  (pointer of) single info for auto checksum build
*
* OUTPUTS:
*       None
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*       Buffer must be allocated for CRC (4 bytes)
*
*******************************************************************************/
static GT_STATUS internalAutoChecksumField
(
    IN TGF_AUTO_CHECKSUM_INFO_STC *checksumInfoPtr,
    IN TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
)
{
    GT_STATUS rc;
    GT_U32  ii,kk;
    TGF_AUTO_CHECKSUM_INFO_STC *currChecksumInfoPtr;/* current checksum info pointer */
    GT_U8   *pseudoPtr;/* pointer to pseudo header buffer */
    GT_U32  pseudoNumBytes;/* pseudo header number of bytes */
    GT_U32  headerLen,totalLen,l4Length;

    currChecksumInfoPtr = checksumInfoPtr;
    for(ii = 0 ; ii < TGF_AUTO_CHECKSUM_FIELD_LAST_E ; ii++,currChecksumInfoPtr++)
    {
        if(currChecksumInfoPtr->enable != GT_TRUE ||
           currChecksumInfoPtr->checkSumPtr == NULL)
        {
            continue;
        }

        switch(ii)
        {
            case TGF_AUTO_CHECKSUM_FIELD_TCP_E:
            case TGF_AUTO_CHECKSUM_FIELD_UDP_E:
                if(checksumExtraInfoPtr->isIpv4 == GT_FALSE)
                {   /* ipv6 */
                    /* Sip ,Dip */
                    for(kk = 0 ; kk < 32 ; kk++)
                    {
                        checksumExtraInfoPtr->pseudo.buffer[kk]   = checksumExtraInfoPtr->startL3HeadrPtr[8+kk];
                    }

                    /* zero */
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    /* l4 length*/
                    l4Length =  (checksumExtraInfoPtr->startL3HeadrPtr[4] << 8)  |
                                 checksumExtraInfoPtr->startL3HeadrPtr[5];
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = checksumExtraInfoPtr->startL3HeadrPtr[4];
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = checksumExtraInfoPtr->startL3HeadrPtr[5];

                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = checksumExtraInfoPtr->startL3HeadrPtr[6];

                    checksumExtraInfoPtr->pseudo.numBytes = kk;
                }
                else
                {
                    headerLen = 4 * (checksumExtraInfoPtr->startL3HeadrPtr[0] & 0xF);
                    totalLen =  (checksumExtraInfoPtr->startL3HeadrPtr[2] << 8) |
                                 checksumExtraInfoPtr->startL3HeadrPtr[3];
                    l4Length = totalLen - headerLen;

                    /* Sip ,Dip */
                    for(kk = 0 ; kk < 8 ; kk++)
                    {
                        checksumExtraInfoPtr->pseudo.buffer[kk] = checksumExtraInfoPtr->startL3HeadrPtr[12+kk];
                    }

                    /* zero */
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = 0;
                    /* protocol */
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = checksumExtraInfoPtr->startL3HeadrPtr[9];
                    /* l4 length*/
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = (GT_U8)(l4Length >> 8);
                    checksumExtraInfoPtr->pseudo.buffer[kk++] = (GT_U8)(l4Length);

                    checksumExtraInfoPtr->pseudo.numBytes = kk;

                }

                if(ii == TGF_AUTO_CHECKSUM_FIELD_TCP_E)
                {
                    /* TCP not hold length , so take it from ipv4/6 info */
                    currChecksumInfoPtr->numBytes = l4Length;
                }

                pseudoPtr = checksumExtraInfoPtr->pseudo.buffer;
                pseudoNumBytes = checksumExtraInfoPtr->pseudo.numBytes;

                if(checksumExtraInfoPtr->udpLengthFieldPtr)
                {
                    /* UDP field of length to be same as the calculated length */
                    checksumExtraInfoPtr->udpLengthFieldPtr[0] = (GT_U8)(l4Length >> 8);
                    checksumExtraInfoPtr->udpLengthFieldPtr[1] = (GT_U8)(l4Length);

                    currChecksumInfoPtr->numBytes = l4Length;
                }

                break;
            default:
                pseudoPtr = NULL;
                pseudoNumBytes = 0;
                break;
        }

        /* apply checksum auto-calc on needed section of the packet */
        currChecksumInfoPtr->checkSumPtr[0] = 0;
        currChecksumInfoPtr->checkSumPtr[1] = 0;
        rc = tgfTrafficEnginePacketCheckSum16BitsCalc(
                currChecksumInfoPtr->startPtr,
                currChecksumInfoPtr->numBytes,
                pseudoPtr,
                pseudoNumBytes,
                currChecksumInfoPtr->checkSumPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* autoChecksumField
*
* DESCRIPTION:
*       apply auto checksum fields after applying the VFD
*
* INPUTS:
*       checksumInfoArr - (array of) array of info about fields that need auto checksum build
*       checksumExtraInfoPtr -  (array of) extra info for auto checksum build
*
* OUTPUTS:
*       None
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*       Buffer must be allocated for CRC (4 bytes)
*
*******************************************************************************/
static GT_STATUS autoChecksumField
(
    IN TGF_AUTO_CHECKSUM_FULL_INFO_STC  *checksumInfoPtr,
    IN TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
)
{
    GT_STATUS rc;
    GT_U32  ii;

    /* do reverse loop because UDP/TCP checksum need to be calced only after whole it's payload already known */
    for(ii = TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS - 1;
         /*no end condition*/;
         ii--)
    {
        rc = internalAutoChecksumField(checksumInfoPtr[ii].singleInfoArr,&checksumExtraInfoPtr[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(ii == 0)
        {
            break;
        }
    }

    return GT_OK;
}

void setMiiCounter(IN GT_U32 newMiiCounter)
{
    miiCounter = newMiiCounter;
}


void debugForcePrint(IN GT_U32 forcePrint)
{
    debug_forcePrint = forcePrint;
}

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCapturePclExcludeVidSet
*
* DESCRIPTION:
*       do we need to capture by PCL all packets , exclude those of specific vlan ?
*       this is relevant only when 'PCL capture' used (TGF_CAPTURE_MODE_PCL_E)
*
* INPUTS:
*       excludeEnable - GT_TRUE - exclude
*                       GT_FALSE - no excluding.
*       excludedVid - the vid to exclude .
*                     relevant only when  excludeEnable = GT_TRUE
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthCapturePclExcludeVidSet
(
    IN GT_BOOL  excludeEnable,
    IN GT_U16   excludedVid
)
{

    pclCaptureExcludeVidEnabled = excludeEnable;
    pclCaptureExcludeVid = excludedVid;

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorRxInCpuNumWait
*
* DESCRIPTION:
*       wait for ALL packets that expected in the CPU, to received.
*
* INPUTS:
*       numOfExpectedPackets - number of packets
*       maxTimeToWait        - MAX time (in milliseconds) to wait for the packets.
*                              The functions will return as soon as all packets
*                              received (but not more then this max time)
*
* OUTPUTS:
*       timeActuallyWaitPtr - time in milliseconds actually waited for the packets.
*                             NOTE: this parameter can be NULL.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_NOT_FOUND  - not get all packets on time
*       GT_BAD_PARAM - wrong numOfExpectedPackets
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorRxInCpuNumWait
(
    IN GT_U32   numOfExpectedPackets,
    IN GT_U32   maxTimeToWait,
    OUT GT_U32  *timeActuallyWaitPtr
)
{
    GT_STATUS           rc             ;
    static GT_8         specificDeviceFormat[TGF_SPECIFIC_DEVICE_FORMAT_SIZE_CNS] = {0};
    static GT_U8        packetBuff[TGF_RX_BUFFER_MAX_SIZE_CNS] = {0};
    GT_U32              packetBufLen;
    GT_U32              origPacketLen = 0;
    GT_U8               devNum;
    GT_U8               queue;

    if(numOfExpectedPackets == 0)
    {
        return GT_BAD_PARAM;
    }


    if(maxTimeToWait == 0)
    {
        maxTimeToWait = prvTgfCaptureTimeLoop * TGF_DEFAULT_CAPTURE_SLEEP_TIME_CNS;
    }

    if(timeActuallyWaitPtr)
    {
        *timeActuallyWaitPtr = 0;
    }

    do{
        packetBufLen = TGF_RX_BUFFER_MAX_SIZE_CNS;
        rc = tgfTrafficTableRxPcktGet((numOfExpectedPackets - 1), packetBuff ,&packetBufLen, &origPacketLen,
                                      &devNum, &queue, specificDeviceFormat);

        if(rc == GT_NOT_FOUND)
        {
            if(maxTimeToWait--)
            {
                cpssOsTimerWkAfter(1);
                if(timeActuallyWaitPtr)
                {
                    (*timeActuallyWaitPtr) ++;
                }
            }
            else
            {
                /* the max time to wait is over ...*/
                break;
            }
        }
        else if(rc == GT_OK)
        {
            /* index of last packet is not empty */
            break;
        }
        else
        {
            /* some kind of error ? */
            break;
        }
    }while(1);

    return rc;
}


/*******************************************************************************
* tgfTrafficGeneratorExpectTraficToCpu
*
* DESCRIPTION:
*       state that current test expect traffic to the CPU
*       NOTE: this function should not be called for cases that test uses
*       'capture' to the CPU.
*
* INPUTS:
*       enable - GT_TRUE    - test expect traffic to the CPU.
*                GT_FALSE   - test NOT expect traffic to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorExpectTraficToCpu
(
    IN GT_BOOL  enable
)
{
    tgfTrafficGeneratorExpectTraficToCpuEnabled = enable;

    return GT_OK;
}

#ifdef CHX_FAMILY
/*******************************************************************************
* dxchDsaTagBuild
*
* DESCRIPTION:
*       Build DSA tag bytes on the packet from the DSA tag parameters --> dxch devices
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that need to be built into
*                   the packet
*
* OUTPUTS:
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*                     This pointer in the packet should hold space for :
*                     4 bytes when using regular DSA tag.
*                     8 bytes when using extended DSA tag.
*                     This function only set values into the 4 or 8 bytes
*                     according to the parameters in parameter dsaInfoPtr.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxchDsaTagBuild
(
    IN  TGF_PACKET_DSA_TAG_STC         *dsaInfoPtr,
    OUT GT_U8                          *dsaBytesPtr
)
{
    GT_U8                          devNum = prvTgfDevNum;
    CPSS_DXCH_NET_DSA_PARAMS_STC   dsaInfo;
    GT_STATUS                      rc;

    cpssOsMemSet(&dsaInfo,0,sizeof(dsaInfo));

    /* type of DSA : 1/2/3/4 words */
    dsaInfo.commonParams.dsaTagType = dsaInfoPtr->dsaType;
    /* command of DSA : to_cpu/from_cpu/to_analyzer/forward */
    dsaInfo.dsaType                 = dsaInfoPtr->dsaCommand;

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  vpt             );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  cfiBit          );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  vid             );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  dropOnSource    );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  packetIsLooped  );

    switch(dsaInfoPtr->dsaCommand)
    {
        case  TGF_DSA_CMD_TO_CPU_E:
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  isEgressPipe  );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  isTagged      );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  hwDevNum      );
            rc = extUtilHwDeviceNumberGet(dsaInfo.dsaInfo.toCpu.hwDevNum,
                                         &dsaInfo.dsaInfo.toCpu.hwDevNum);
            if(GT_OK != rc)
                return rc;
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  srcIsTrunk    );
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.toCpu.interface) != sizeof(dsaInfoPtr->dsaInfo.toCpu.interface))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(&dsaInfo.dsaInfo.toCpu,&dsaInfoPtr->dsaInfo.toCpu,  interface   );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  cpuCode       );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  wasTruncated  );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  originByteCount);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  timestamp     );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  packetIsTT    );
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.toCpu.flowIdTtOffset) != sizeof(dsaInfoPtr->dsaInfo.toCpu.flowIdTtOffset))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(&dsaInfo.dsaInfo.toCpu,&dsaInfoPtr->dsaInfo.toCpu,flowIdTtOffset);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  tag0TpidIndex );


            switch(dsaInfoPtr->dsaType)
            {
                case TGF_DSA_2_WORD_TYPE_E:
                    if(PRV_CPSS_IS_DUAL_HW_DEVICE_MAC(dsaInfo.dsaInfo.toCpu.hwDevNum))
                    {
                        dsaInfo.dsaInfo.toCpu.interface.portNum &= 0x7f;
                    }
                    else
                    {
                        dsaInfo.dsaInfo.toCpu.interface.portNum &= 0x3f;
                    }
                    break;
                case TGF_DSA_4_WORD_TYPE_E:
                    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
                    {
                        /* the DSA tag limited to 9 bits */
                        dsaInfo.dsaInfo.toCpu.interface.portNum &= 0x1ff;
                    }
                    else
                    {
                        /* the DSA tag limited to 8 bits */
                        dsaInfo.dsaInfo.toCpu.interface.portNum &= 0xff;
                    }
                    break;
                default:
                    break;
            }

            break;
        case TGF_DSA_CMD_FROM_CPU_E:
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  dstInterface  );
            if(dsaInfo.dsaInfo.fromCpu.dstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                rc = extUtilHwDeviceNumberGet(dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum,
                                             &dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.hwDevNum);
                if(GT_OK != rc)
                    return rc;
            }
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  tc            );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  dp            );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  egrFilterEn   );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  cascadeControl);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  egrFilterRegistered);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  srcId         );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  srcHwDev      );
            rc = extUtilHwDeviceNumberGet(dsaInfo.dsaInfo.fromCpu.srcHwDev,
                                         &dsaInfo.dsaInfo.fromCpu.srcHwDev);
            if(GT_OK != rc)
                return rc;
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.fromCpu.extDestInfo) != sizeof(dsaInfoPtr->dsaInfo.fromCpu.extDestInfo))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(&dsaInfo.dsaInfo.fromCpu,&dsaInfoPtr->dsaInfo.fromCpu,  extDestInfo   );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  isTrgPhyPortValid);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  dstEport      );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  tag0TpidIndex );

            if(dsaInfo.dsaInfo.fromCpu.dstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                if(dsaInfo.commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    if(dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum >= BIT_8)
                    {
                        if(dsaInfo.dsaInfo.fromCpu.isTrgPhyPortValid == GT_TRUE)
                        {
                            /* we must use the 'E2PHY' to convert eport to physical port */
                            dsaInfo.dsaInfo.fromCpu.isTrgPhyPortValid = GT_FALSE;
                            dsaInfo.dsaInfo.fromCpu.dstEport = dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum;
                        }
                        else
                        {
                            /* the dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum
                               is already 'dont care' */
                        }

                        /* do not let the CPSS to fail the operation */
                        dsaInfo.dsaInfo.fromCpu.dstInterface.devPort.portNum &= 0xFF;
                    }
                }
            }

            break;
        case TGF_DSA_CMD_TO_ANALYZER_E:
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer,  &dsaInfoPtr->dsaInfo.toAnalyzer,  rxSniffer    );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer,  &dsaInfoPtr->dsaInfo.toAnalyzer,  isTagged     );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer,  &dsaInfoPtr->dsaInfo.toAnalyzer,  analyzerTrgType);
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.toAnalyzer.devPort) != sizeof(dsaInfoPtr->dsaInfo.toAnalyzer.devPort))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer.devPort,  &dsaInfoPtr->dsaInfo.toAnalyzer.devPort,  hwDevNum);
            rc = extUtilHwDeviceNumberGet(dsaInfo.dsaInfo.toAnalyzer.devPort.hwDevNum,
                                         &dsaInfo.dsaInfo.toAnalyzer.devPort.hwDevNum);
            if(GT_OK != rc)
                return rc;
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer.devPort,  &dsaInfoPtr->dsaInfo.toAnalyzer.devPort,  portNum    );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer.devPort,  &dsaInfoPtr->dsaInfo.toAnalyzer.devPort,  ePort      );

#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.toAnalyzer.extDestInfo) != sizeof(dsaInfoPtr->dsaInfo.toAnalyzer.extDestInfo))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer,  &dsaInfoPtr->dsaInfo.toAnalyzer,  extDestInfo                );

            if(dsaInfoPtr->dsaInfo.toAnalyzer.analyzerTrgType == CPSS_INTERFACE_PORT_E)
            {
                rc = extUtilHwDeviceNumberGet(dsaInfo.dsaInfo.toAnalyzer.extDestInfo.devPort.analyzerHwTrgDev,
                                             &dsaInfo.dsaInfo.toAnalyzer.extDestInfo.devPort.analyzerHwTrgDev);
                if(GT_OK != rc)
                    return rc;
            }

            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer,  &dsaInfoPtr->dsaInfo.toAnalyzer,  tag0TpidIndex   );

            switch(dsaInfoPtr->dsaType)
            {
                case TGF_DSA_2_WORD_TYPE_E:
                    if(PRV_CPSS_IS_DUAL_HW_DEVICE_MAC(dsaInfo.dsaInfo.toAnalyzer.devPort.hwDevNum))
                    {
                        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum &= 0x7f;
                    }
                    else
                    {
                        dsaInfo.dsaInfo.toAnalyzer.devPort.portNum &= 0x3f;
                    }
                    break;
                case TGF_DSA_4_WORD_TYPE_E:
                    /* the DSA tag limited to 8 bits */
                    dsaInfo.dsaInfo.toAnalyzer.devPort.portNum &= 0xff;

                    if(dsaInfoPtr->dsaInfo.toAnalyzer.analyzerTrgType == CPSS_INTERFACE_PORT_E)
                    {
                        dsaInfo.dsaInfo.toAnalyzer.extDestInfo.devPort.analyzerTrgPort &= 0xff;
                    }
                    break;
                default:
                    break;
            }

            break;
        case TGF_DSA_CMD_FORWARD_E:
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  srcIsTagged           );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  srcHwDev                );
            rc = extUtilHwDeviceNumberGet(dsaInfo.dsaInfo.forward.srcHwDev,
                                         &dsaInfo.dsaInfo.forward.srcHwDev);
            if(GT_OK != rc)
                return rc;
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  srcIsTrunk            );
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.forward.source) != sizeof(dsaInfoPtr->dsaInfo.forward.source))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  source                );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  srcId                 );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  egrFilterRegistered   );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  wasRouted             );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  qosProfileIndex       );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  dstInterface          );
            if(dsaInfo.dsaInfo.forward.dstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                rc = extUtilHwDeviceNumberGet(dsaInfo.dsaInfo.forward.dstInterface.devPort.hwDevNum,
                                             &dsaInfo.dsaInfo.forward.dstInterface.devPort.hwDevNum);
                if(GT_OK != rc)
                    return rc;

                switch(dsaInfoPtr->dsaType)
                {
                    case TGF_DSA_2_WORD_TYPE_E:
                        if(PRV_CPSS_IS_DUAL_HW_DEVICE_MAC(dsaInfo.dsaInfo.forward.dstInterface.devPort.hwDevNum))
                        {
                            dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum &= 0x7f;
                        }
                        else
                        {
                            dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum &= 0x3f;
                        }
                        break;
                    case TGF_DSA_4_WORD_TYPE_E:
                        /* the DSA tag limited to 8 bits */
                        dsaInfo.dsaInfo.forward.dstInterface.devPort.portNum &= 0xff;
                        break;
                    default:
                        break;
                }
            }

            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  isTrgPhyPortValid     );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  dstEport              );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  tag0TpidIndex         );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  origSrcPhyIsTrunk     );
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.forward.origSrcPhy) != sizeof(dsaInfoPtr->dsaInfo.forward.origSrcPhy))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  origSrcPhy      );
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  phySrcMcFilterEnable  );

            switch(dsaInfoPtr->dsaType)
            {
                case TGF_DSA_1_WORD_TYPE_E:
                    if(dsaInfo.dsaInfo.forward.srcIsTrunk == GT_FALSE)
                    {
                        dsaInfo.dsaInfo.forward.source.portNum &= 0x1f;
                    }
                    dsaInfo.dsaInfo.forward.srcHwDev &= 0x1f;
                    break;
                case TGF_DSA_2_WORD_TYPE_E:
                    if(dsaInfo.dsaInfo.forward.srcIsTrunk == GT_FALSE)
                    {
                        if((PRV_CPSS_IS_DUAL_HW_DEVICE_MAC(dsaInfo.dsaInfo.forward.srcHwDev)) ||
                           (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum)))
                        {
                            dsaInfo.dsaInfo.forward.source.portNum &= 0x7f;
                        }
                        else
                        {
                            dsaInfo.dsaInfo.forward.source.portNum &= 0x3f;
                        }
                    }
                    dsaInfo.dsaInfo.forward.srcHwDev &= 0x1f;
                    break;
                case TGF_DSA_4_WORD_TYPE_E:
                    /* no limit on :
                       dsaInfo.dsaInfo.forward.source.portNum
                       as this is srcEport
                    */
                    /* the DSA tag IS NOT limited to 8 bits ,
                       but to 12 bits due to 'MUX' with trunkId !!!*/
                    dsaInfo.dsaInfo.forward.origSrcPhy.portNum &= 0xfff;
                    break;
                default:
                    break;
            }

            break;
        default:
            return GT_NOT_IMPLEMENTED;

    }

    /* call the CPSS to build the DSA tag according to the given info */
    rc = cpssDxChNetIfDsaTagBuild (devNum,&dsaInfo,dsaBytesPtr);


    return rc;
}
/*******************************************************************************
* dxchDsaTagParse
*
* DESCRIPTION:
*       parse the DSA tag bytes from the packet into the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dsaBytesPtr - pointer to the start of DSA tag in the packet.
*
* OUTPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that parsed from the packet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dxchDsaTagParse
(
    IN  GT_U8                          *dsaBytesPtr,
    OUT TGF_PACKET_DSA_TAG_STC         *dsaInfoPtr
)
{
    GT_U8                          devNum = prvTgfDevNum;
    CPSS_DXCH_NET_DSA_PARAMS_STC   dsaInfo;
    GT_STATUS                      rc;

    cpssOsMemSet(&dsaInfo,0,sizeof(dsaInfo));

    /* call the CPSS to parse the DSA tag parameters from the DSA tag on the packet */
    rc = cpssDxChNetIfDsaTagParse (devNum,dsaBytesPtr,&dsaInfo);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* type of DSA : 1/2/3/4 words */
    dsaInfoPtr->dsaType = dsaInfo.commonParams.dsaTagType;
    /* command of DSA : to_cpu/from_cpu/to_analyzer/forward */
    dsaInfoPtr->dsaCommand = dsaInfo.dsaType;

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  vpt             );
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  cfiBit          );
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  vid             );
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  dropOnSource    );
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.commonParams,  &dsaInfoPtr->commonParams,  packetIsLooped  );

    switch(dsaInfoPtr->dsaCommand)
    {
        case  TGF_DSA_CMD_TO_CPU_E:
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  isEgressPipe  );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  isTagged      );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  hwDevNum      );
            rc = extUtilSwDeviceNumberGet(dsaInfoPtr->dsaInfo.toCpu.hwDevNum,
                                               &dsaInfoPtr->dsaInfo.toCpu.hwDevNum);
            if(GT_OK != rc)
                return rc;
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  srcIsTrunk    );
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.toCpu.interface) != sizeof(dsaInfoPtr->dsaInfo.toCpu.interface))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_D2S_UNION_FIELD_COPY_MAC(&dsaInfo.dsaInfo.toCpu,&dsaInfoPtr->dsaInfo.toCpu,  interface   );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  cpuCode       );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  wasTruncated  );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  originByteCount);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  timestamp     );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  packetIsTT    );
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.toCpu.flowIdTtOffset) != sizeof(dsaInfoPtr->dsaInfo.toCpu.flowIdTtOffset))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_D2S_UNION_FIELD_COPY_MAC(&dsaInfo.dsaInfo.toCpu,&dsaInfoPtr->dsaInfo.toCpu,flowIdTtOffset);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toCpu,  &dsaInfoPtr->dsaInfo.toCpu,  tag0TpidIndex );
            break;
        case TGF_DSA_CMD_FROM_CPU_E:
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  dstInterface  );
            if(dsaInfo.dsaInfo.fromCpu.dstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                rc = extUtilSwDeviceNumberGet(dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.hwDevNum,
                                                   &dsaInfoPtr->dsaInfo.fromCpu.dstInterface.devPort.hwDevNum);
                if(GT_OK != rc)
                    return rc;
            }
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  tc            );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  dp            );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  egrFilterEn   );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  cascadeControl);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  egrFilterRegistered);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  srcId         );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  srcHwDev        );
            rc = extUtilSwDeviceNumberGet(dsaInfoPtr->dsaInfo.fromCpu.srcHwDev,
                                               &dsaInfoPtr->dsaInfo.fromCpu.srcHwDev);
            if(GT_OK != rc)
                return rc;
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.fromCpu.extDestInfo) != sizeof(dsaInfoPtr->dsaInfo.fromCpu.extDestInfo))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_D2S_UNION_FIELD_COPY_MAC(&dsaInfo.dsaInfo.fromCpu,&dsaInfoPtr->dsaInfo.fromCpu,  extDestInfo   );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  isTrgPhyPortValid);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  dstEport      );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.fromCpu,  &dsaInfoPtr->dsaInfo.fromCpu,  tag0TpidIndex );
            break;
        case TGF_DSA_CMD_TO_ANALYZER_E:
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer,  &dsaInfoPtr->dsaInfo.toAnalyzer,  rxSniffer    );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer,  &dsaInfoPtr->dsaInfo.toAnalyzer,  isTagged     );
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.toAnalyzer.devPort) != sizeof(dsaInfoPtr->dsaInfo.toAnalyzer.devPort))
            {
                return GT_BAD_STATE;
            }
#endif
            rc = extUtilSwDeviceNumberGet(dsaInfoPtr->dsaInfo.toAnalyzer.devPort.hwDevNum,
                                               &dsaInfoPtr->dsaInfo.toAnalyzer.devPort.hwDevNum);
            if(GT_OK != rc)
                return rc;
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer.devPort,  &dsaInfoPtr->dsaInfo.toAnalyzer.devPort,  portNum    );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer.devPort,  &dsaInfoPtr->dsaInfo.toAnalyzer.devPort,  ePort      );

#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.toAnalyzer.extDestInfo) != sizeof(dsaInfoPtr->dsaInfo.toAnalyzer.extDestInfo))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_D2S_UNION_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer,  &dsaInfoPtr->dsaInfo.toAnalyzer,  extDestInfo                );

            if(dsaInfoPtr->dsaInfo.toAnalyzer.analyzerTrgType == CPSS_INTERFACE_PORT_E)
            {
                rc = extUtilSwDeviceNumberGet(dsaInfoPtr->dsaInfo.toAnalyzer.extDestInfo.devPort.analyzerHwTrgDev,
                                                   &dsaInfoPtr->dsaInfo.toAnalyzer.extDestInfo.devPort.analyzerHwTrgDev);
                if(GT_OK != rc)
                    return rc;
            }

            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.toAnalyzer,  &dsaInfoPtr->dsaInfo.toAnalyzer,  tag0TpidIndex   );
            break;
        case TGF_DSA_CMD_FORWARD_E:
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  srcIsTagged           );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  srcHwDev                );
            rc = extUtilSwDeviceNumberGet(dsaInfoPtr->dsaInfo.forward.srcHwDev,
                                               &dsaInfoPtr->dsaInfo.forward.srcHwDev);
            if(GT_OK != rc)
                return rc;
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  srcIsTrunk            );
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.forward.source) != sizeof(dsaInfoPtr->dsaInfo.forward.source))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_D2S_UNION_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  source                );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  srcId                 );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  egrFilterRegistered   );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  wasRouted             );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  qosProfileIndex       );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  dstInterface          );
            if(dsaInfo.dsaInfo.forward.dstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                rc = extUtilSwDeviceNumberGet(dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.hwDevNum,
                                                   &dsaInfoPtr->dsaInfo.forward.dstInterface.devPort.hwDevNum);
                if(GT_OK != rc)
                    return rc;
            }
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  isTrgPhyPortValid     );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  dstEport              );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  tag0TpidIndex         );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  origSrcPhyIsTrunk     );
#ifdef CHECK_SIZE_MISMATCH
            if (sizeof(dsaInfo.dsaInfo.forward.origSrcPhy) != sizeof(dsaInfoPtr->dsaInfo.forward.origSrcPhy))
            {
                return GT_BAD_STATE;
            }
#endif
            PRV_TGF_STC_D2S_UNION_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  origSrcPhy      );
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(  &dsaInfo.dsaInfo.forward,  &dsaInfoPtr->dsaInfo.forward,  phySrcMcFilterEnable  );

            break;
        default:
            return GT_NOT_IMPLEMENTED;

    }

    return GT_OK;
}

#endif /*CHX_FAMILY*/

/*******************************************************************************
* prvTgfTrafficGeneratorPacketDsaTagBuild
*
* DESCRIPTION:
*       Build DSA tag bytes on the packet from the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that need to be built into
*                   the packet
*
* OUTPUTS:
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*                     This pointer in the packet should hold space for :
*                     4 bytes when using regular DSA tag.
*                     8 bytes when using extended DSA tag.
*                     This function only set values into the 4 or 8 bytes
*                     according to the parameters in parameter dsaInfoPtr.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrafficGeneratorPacketDsaTagBuild
(
    IN  TGF_PACKET_DSA_TAG_STC         *dsaInfoPtr,
    OUT GT_U8                          *dsaBytesPtr
)
{
    GT_STATUS   rc = GT_NOT_IMPLEMENTED;
#ifdef CHX_FAMILY
    rc =  dxchDsaTagBuild(dsaInfoPtr,dsaBytesPtr);
#endif /*CHX_FAMILY*/

    return rc;
}

/*******************************************************************************
* prvTgfTrafficGeneratorPacketDsaTagParse
*
* DESCRIPTION:
*       parse the DSA tag bytes from the packet into the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dsaBytesPtr - pointer to the start of DSA tag in the packet.
*
* OUTPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that parsed from the packet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrafficGeneratorPacketDsaTagParse
(
    IN  GT_U8                          *dsaBytesPtr,
    OUT TGF_PACKET_DSA_TAG_STC         *dsaInfoPtr
)
{
    GT_STATUS   rc = GT_NOT_IMPLEMENTED;
#ifdef CHX_FAMILY
    rc =  dxchDsaTagParse(dsaBytesPtr,dsaInfoPtr);
#endif /*CHX_FAMILY*/

    return rc;
}

/*******************************************************************************
* sprvTgfCscdPortTypeSet
*
* DESCRIPTION:
*            Configure a PP port to be a cascade port. Application is responsible
*            for setting the default values of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portDirection   - port's direction:
*                         CPSS_DIRECTION_INGRESS_E - source port
*                         CPSS_DIRECTION_EGRESS_E  - target port
*                         CPSS_DIRECTION_BOTH_E    - both source and target ports
*                      APPLICABLE DEVICES: Lion2; Bobcat2; Caelum; Bobcat3.
*       portNum  - The port to be configured as cascade
*       portType - cascade  type regular/extended DSA tag port or network port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS sprvTgfCscdPortTypeSet
(
    IN GT_U8                        devNum,
    IN CPSS_PORT_DIRECTION_ENT      portDirection,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN CPSS_CSCD_PORT_TYPE_ENT      portType
)
{

    if(portDirection != CPSS_PORT_DIRECTION_BOTH_E)
    {
        if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
        {
            return GT_NOT_SUPPORTED;
        }

        switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            case CPSS_PP_FAMILY_DXCH_LION_E   :
            case CPSS_PP_FAMILY_DXCH_LION2_E  :
            case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
            case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
                break;
            default:
                /* the device not supports different configuration for the ingress from egress */
                return GT_NOT_SUPPORTED;
        }
    }

#ifdef CHX_FAMILY
    return cpssDxChCscdPortTypeSet(devNum,portNum,portDirection,portType);
#else
    return GT_NOT_IMPLEMENTED;
#endif
}


/*******************************************************************************
* tgfTrafficGeneratorIngressCscdPortEnableSet
*
* DESCRIPTION:
*            enable/disable the port to assume that ingress traffic hold DSA tag
*            info.
*            NOTE: relevant only to devices that support detach of ingress and
*               egress cascade configurations.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - The port to be configured as cascade enable/disabled
*       enable - enable/disable the ingress DSA tag processing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this API needed to support testing of DSA tag processing by the device
*       (DSA that not processed on the ingress of the CPU_PORT)
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorIngressCscdPortEnableSet
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN GT_BOOL                      enable
)
{
    CPSS_CSCD_PORT_TYPE_ENT portType =  (enable == GT_TRUE) ?
            CPSS_CSCD_PORT_DSA_MODE_1_WORD_E : /*dummy value --> should be different then CPSS_CSCD_PORT_NETWORK_E */
            CPSS_CSCD_PORT_NETWORK_E;

    return sprvTgfCscdPortTypeSet(devNum,CPSS_PORT_DIRECTION_RX_E,portNum,portType);
}

/*******************************************************************************
* tgfTrafficGeneratorEgressCscdPortEnableSet
*
* DESCRIPTION:
*            enable/disable the port to assume that egress traffic hold DSA tag
*            info.
*            NOTE: relevant only to devices that support detach of ingress and
*               egress cascade configurations.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - The port to be configured as cascade enable/disabled
*       enable       - enable/disable the ingress DSA tag processing
*       cscdPortType - type of cascade port (APPLICABLE VALUES:
*                                            CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E,
*                                            CPSS_CSCD_PORT_DSA_MODE_4_WORDS_E,
*                                            CPSS_CSCD_PORT_NETWORK_E).
*                      Relevant only when enable is GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       this API needed to support testing of DSA tag processing by the device
*       (DSA that not processed on the ingress of the CPU_PORT)
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorEgressCscdPortEnableSet
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN GT_BOOL                      enable,
    IN CPSS_CSCD_PORT_TYPE_ENT      cscdPortType
)
{
    if ((cscdPortType != CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E) &&
        (cscdPortType != CPSS_CSCD_PORT_DSA_MODE_4_WORDS_E) &&
        (cscdPortType != CPSS_CSCD_PORT_NETWORK_E))
    {
        return GT_BAD_PARAM;
    }

    if (enable == GT_FALSE)
    {
        cscdPortType = CPSS_CSCD_PORT_NETWORK_E;
    }

    return sprvTgfCscdPortTypeSet(devNum, CPSS_PORT_DIRECTION_TX_E, portNum, cscdPortType);
}

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCaptureCompare
*
* DESCRIPTION:
*       compare captured packets with the input packet format (+VFD array)
*       NOTE: the function not care about reason why packet reached the CPU ,
*             is it due to 'capture' or other reason.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port/trunk interface
*                           NOTE: can be NULL --> meaning 'any interface'
*       packetInfoPtr   - the packet format to compare the captured packet with.
*       numOfPackets    - number of packets expected to be captured on the interface (can be 0)
*       numVfd           - number of VFDs (can be 0)
*       vfdArray         - pointer to array of VFDs (can be NULL when numVfd == 0)
*       byteNumMaskList  - pointer to array of bytes for which the comparison
*                           is prohibited (can be NULL when byteNumMaskListLen == 0)
*       byteNumMaskListLen - number of members in byteNumMaskList(can be 0)
*
* OUTPUTS:
*       actualCapturedNumOfPacketsPtr  - (pointer to) actual number of packet captured from
*                           the interface
*       onFirstPacketNumTriggersBmpPtr - (pointer to) bitmap of triggers - that
*                           relate only to first packet  (can be NULL when numVfd == 0)
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*       Triggers bitmap has the following structure:
*       K - number of VFDs (equal to numVfd)
*           |      Packet0     |
*           +----+----+---+----+
*           |Bit0|Bit1|...|BitK|
*           +----+----+---+----+
*       Bit[i] to 1 means that VFD[i] is matched for first captured packet .
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorPortTxEthCaptureCompare
(
    IN  CPSS_INTERFACE_INFO_STC     *portInterfacePtr,
    IN  TGF_PACKET_STC      *packetInfoPtr,
    IN  GT_U32              numOfPackets,
    IN  GT_U32              numVfd,
    IN  TGF_VFD_INFO_STC    vfdArray[],
    IN  GT_U32              byteNumMaskList[],
    IN  GT_U32              byteNumMaskListLen,
    OUT GT_U32              *actualCapturedNumOfPacketsPtr,
    OUT GT_U32              *onFirstPacketNumTriggersBmpPtr
)
{
    GT_U8       *expectedPacketBufferPtr = NULL;
    TGF_AUTO_CHECKSUM_FULL_INFO_STC checksumInfoArr[TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS];/* auto checksum info */
    TGF_AUTO_CHECKSUM_EXTRA_INFO_STC extraChecksumInfoArr[TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS];/* extra checksum info */
    GT_U32      expectedPacketNumOfBytesInPacketBuff;/* number of bytes in packetBuff*/ /* save space of CRC*/
    GT_U32      numOfMismatch;/* number of bytes mismatched */
    GT_U32      lastMismatchByteIndex;/* byte index of last mismatch ... in order to manage sequences of mismatch */
    GT_U32      lastMismatchStartByteIndex;/* byte index of start of last mismatch ... in order to manage sequences of mismatch */
    GT_U32      lastMismatchRecorded;/* last mismatch that we recorded */
    GT_STATUS       rc1      = GT_OK;
    GT_BOOL     useMii;/* do we use MII or SDMA */
    GT_BOOL     useEmunatedSdma;/* do we use MII but with emulated SDMA */
    GT_U8           devNum   = 0;
    GT_U8           queue    = 0;
    GT_BOOL         getFirst = GT_TRUE;
    GT_U32          i        = 0;
    GT_BOOL         match    = GT_FALSE;
    GT_STATUS       rc       = GT_OK;
    GT_U32          buffLen  = TGF_RX_BUFFER_MAX_SIZE_CNS;
    GT_U32          packetActualLength = 0;
    GT_U32          lengthTolerance = 0;
    GT_U32          packetCount = 0;
    GT_U8           packetBuff[TGF_RX_BUFFER_MAX_SIZE_CNS] = {0};
    TGF_NET_DSA_STC rxParam = {0};


#ifdef GM_USED
    GT_U32          maxWaitTime     = 1200; /* first packet wait time */
    GT_U32          nextWaitTime    = 200;
    GT_U32          delayTime       = 100;
#endif /*GM_USED*/
    GT_U32          origLogPrintMode;
    GT_U32          didError;


#ifdef GM_USED
    if(PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->devFamily == CPSS_PP_FAMILY_PUMA3_E)
    {
        maxWaitTime     = prvTgfGmTimeFactor*100; /* first packet wait time */
        nextWaitTime    = prvTgfGmTimeFactor*50;
        delayTime       = prvTgfGmTimeFactor*20;
    }
#endif /*GM_USED*/

#ifdef EXMXPM_FAMILY
    useMii = GT_FALSE;
    useEmunatedSdma = GT_FALSE;
#else
        #if defined CHX_FAMILY
            useMii =((PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_MII_E) ? GT_TRUE:
                     (PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_NONE_E) ? GT_FALSE:
                     PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(prvTgfCpuDevNum) ? GT_TRUE : GT_FALSE);

            useEmunatedSdma =((PRV_CPSS_PP_MAC(prvTgfCpuDevNum)->cpuPortMode == CPSS_NET_CPU_PORT_MODE_MII_E) ? GT_FALSE:
                              useMii);
        #else
            useMii = GT_FALSE;
        #endif
#endif

    CPSS_NULL_PTR_CHECK_MAC(actualCapturedNumOfPacketsPtr);

    if (numVfd != 0)
    {
        CPSS_NULL_PTR_CHECK_MAC(vfdArray);
        CPSS_NULL_PTR_CHECK_MAC(onFirstPacketNumTriggersBmpPtr);

        (*onFirstPacketNumTriggersBmpPtr) = 0;
    }

    if (portInterfacePtr != NULL &&
        portInterfacePtr->type != CPSS_INTERFACE_PORT_E &&
        portInterfacePtr->type != CPSS_INTERFACE_TRUNK_E)

    {
        return GT_BAD_PARAM;
    }

    cpssOsMemSet(checksumInfoArr,0,sizeof(checksumInfoArr));

    /* get buffer from the pool */
    expectedPacketBufferPtr = gtPoolGetBuf(prvTgfTxBuffersPoolId);
    if(NULL == expectedPacketBufferPtr)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    expectedPacketNumOfBytesInPacketBuff = TGF_RX_BUFFER_MAX_SIZE_CNS - 4;/* save space of CRC*/
    /* build the basic frame info */                            /* save 4 bytes space for CRC */
    rc = tgfTrafficEnginePacketBuild(packetInfoPtr, expectedPacketBufferPtr ,&expectedPacketNumOfBytesInPacketBuff, checksumInfoArr,extraChecksumInfoArr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    if((expectedPacketNumOfBytesInPacketBuff + TGF_CRC_LEN_CNS) > TGF_TX_BUFFER_MAX_SIZE_CNS)
    {
        rc =  GT_BAD_PARAM;
        goto exit_cleanly_lbl;
    }

    while(1)
    {
        didError = GT_FALSE;

        /* disable printings , so only the relevant packets will be printed */
        origLogPrintMode = tgfLogPrintModeFinal();

        buffLen  = TGF_RX_BUFFER_MAX_SIZE_CNS;
        /* get entry from rxNetworkIf table */
        rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_ANY_E,
                                           getFirst, GT_TRUE, packetBuff,
                                           &buffLen, &packetActualLength,
                                           &devNum, &queue, &rxParam);

        /* restore the print */
        tgfLogPrintModeRestore(origLogPrintMode);

#ifdef GM_USED
        /* wait for yet not reached packets               */
        if ((rc == GT_NO_MORE) && (maxWaitTime >= delayTime))
        {
            maxWaitTime -= delayTime;
            cpssOsTimerWkAfter(delayTime);
            continue;
        }
        /* for the fist packet we are ready to wait       */
        /* sevelal delays, for the nexts no more then one */
        maxWaitTime = nextWaitTime;
#endif /*GM_USED*/

        if (rc != GT_OK)
        {
            break;
        }

        getFirst = GT_FALSE; /* now we get the next */

        if(GT_FALSE ==
            checkPacketFromExpectedInterface(portInterfacePtr,&rxParam))
        {
            /* packet not came from expected interface */
            continue;
        }

        /* calculate number of captured packets */
        packetCount++;

        {
            for (i = 0; i < numVfd ;i++)
            {
                vfdArray[i].modeExtraInfo = (packetCount - 1);

                /* over ride the buffer with VFD info */
                rc = tgfTrafficEnginePacketVfdApply(&vfdArray[i], expectedPacketBufferPtr, expectedPacketNumOfBytesInPacketBuff);
                if(rc != GT_OK)
                {
                    goto exit_cleanly_lbl;
                }
            }

            /* apply auto checksum fields after applying the VFD*/
            rc = autoChecksumField(checksumInfoArr,extraChecksumInfoArr);
            if(rc != GT_OK)
            {
                goto exit_cleanly_lbl;
            }

            /* check the VFD parameters */
            for (i = 0; i < numVfd ;i++)
            {
                rc = tgfTrafficEnginePacketVfdCheck(
                    &vfdArray[i], packetBuff, packetActualLength, &match);

                if(onFirstPacketNumTriggersBmpPtr)
                {
                    /* set appropriate bit to 1 if VFD is matched */
                    (*onFirstPacketNumTriggersBmpPtr) |= ((GT_U32) match) << (i + (packetCount - 1) * numVfd);
                }
            }
        }

        if(packetActualLength == (expectedPacketNumOfBytesInPacketBuff + 4))
        {
            /* assume that the expected packet did not added CRC to calculations */
            expectedPacketNumOfBytesInPacketBuff += 4;
        }


        /* compare the packets length */
        if(packetActualLength != expectedPacketNumOfBytesInPacketBuff)
        {
            rc1 = GT_BAD_SIZE;
            PRV_TGF_LOG4_MAC("tgfTrafficGeneratorPortTxEthCaptureCompare: on [%s][%d] expected packet length [%d] , got packet length [%d] \n",
                              portInterfacePtr == NULL ? "any" : portInterfacePtr->type == CPSS_INTERFACE_PORT_E ? "port" : "trunk",
                              portInterfacePtr == NULL ? -1 : portInterfacePtr->type == CPSS_INTERFACE_PORT_E ? portInterfacePtr->devPort.portNum : portInterfacePtr->trunkId,
                              expectedPacketNumOfBytesInPacketBuff , packetActualLength);
            didError = GT_TRUE;
        }

        numOfMismatch = 0;
        lastMismatchByteIndex = 0;
        lastMismatchStartByteIndex = 0;
        lastMismatchRecorded = 0;

        if(useMii == GT_TRUE && useEmunatedSdma == GT_TRUE)
        {
            /* all the TO_CPU (in SDMA) come with 0x55555555 as CRC , so we not check it */
            lengthTolerance = 4;
        }
        else if(useMii == GT_FALSE && packetActualLength >= 4)
        {
            /* all the TO_CPU (in SDMA) come with 0x55555555 as CRC , so we not check it */
            lengthTolerance = 4;
        }

        /* compare the full packet */
        for(i = 0 ; (i < (packetActualLength - lengthTolerance)) && (i < expectedPacketNumOfBytesInPacketBuff)  ; i++)
        {
            if ((!internalIsNumInList(i, byteNumMaskList, byteNumMaskListLen)) &&
                (packetBuff[i] != expectedPacketBufferPtr[i]))
            {
                didError = GT_TRUE;

                if( numOfMismatch == 0)/* first mismatch */
                {
                    /* new mismatch sequence */
                    numOfMismatch++;
                    lastMismatchStartByteIndex = i;
                }
                else if((lastMismatchByteIndex + 1) == i)
                {
                    /* we are in the same mismatch sequence of the last one */
                    /* so update the last byte of this sequence */
                }
                else
                {
                    /* new mismatch sequence */
                    numOfMismatch++;
                    lastMismatchStartByteIndex = i;
                }

                lastMismatchByteIndex = i;
            }
            else /* current byte match between the packets */
            {
                /* when recognize end of mismatch sequence , print it */
                if(numOfMismatch < 10 &&
                   (lastMismatchRecorded != numOfMismatch) )
                {
                    GT_U32  iter;
                    GT_U8*  bufferPtr;
                    GT_U32  length;
                    /* record the last label */
recordLastMismatch_lbl:
                    lastMismatchRecorded = numOfMismatch;

                    bufferPtr = &packetBuff[lastMismatchStartByteIndex];
                    length = (lastMismatchByteIndex - lastMismatchStartByteIndex + 1);

                    rc1 = GT_BAD_VALUE;

                    PRV_TGF_LOG3_MAC("[TGF]: tgfTrafficGeneratorPortTxEthCaptureCompare: numOfMismatch[%d] :"
                        " bytes from captured packet from byte[%d] to byte [%d]\n",
                        numOfMismatch,
                        lastMismatchStartByteIndex,
                        lastMismatchByteIndex);

                    for(iter = 0; iter < length ; iter++)
                    {
                        if((iter & 0x0F) == 0)
                        {
                            PRV_TGF_LOG1_MAC("0x%4.4x :", iter);
                        }

                        PRV_TGF_LOG1_MAC(" %2.2x", bufferPtr[iter]);

                        if((iter & 0x0F) == 0x0F)
                        {
                            PRV_TGF_LOG0_MAC("\n");
                        }
                    }/*iter*/
                    PRV_TGF_LOG0_MAC("\n");

                    bufferPtr = &expectedPacketBufferPtr[lastMismatchStartByteIndex];

                    PRV_TGF_LOG0_MAC("[TGF]: bytes from expected packet \n");

                    for(iter = 0; iter < length ; iter++)
                    {
                        if((iter & 0x0F) == 0)
                        {
                            PRV_TGF_LOG1_MAC("0x%4.4x :", iter);
                        }

                        PRV_TGF_LOG1_MAC(" %2.2x", bufferPtr[iter]);

                        if((iter & 0x0F) == 0x0F)
                        {
                            PRV_TGF_LOG0_MAC("\n");
                        }
                    }/*iter*/
                    PRV_TGF_LOG0_MAC("\n");

                }/*numOfMismatch < 10*/
            }/*else --> current byte match between the packets */
        }/*for(i...)*/

        /* when recognize end of mismatch sequence , print it */
        if(numOfMismatch < 10 &&
           (lastMismatchRecorded != numOfMismatch) )
        {
            goto recordLastMismatch_lbl;
        }


        if(didError || packetCount == 1)
        {
            PRV_TGF_LOG2_MAC("tgfTrafficGeneratorPortTxEthCaptureCompare: the received packet on interface[%s][%d]: \n",
                portInterfacePtr == NULL ? "any" :portInterfacePtr->type == CPSS_INTERFACE_PORT_E ? "port" : "trunk" ,
                portInterfacePtr == NULL ? -1    :portInterfacePtr->type == CPSS_INTERFACE_PORT_E ? portInterfacePtr->devPort.portNum : portInterfacePtr->trunkId);
            tgfTrafficTracePacket(packetBuff, packetActualLength, GT_TRUE);

            if(didError)
            {
                PRV_TGF_LOG0_MAC("tgfTrafficGeneratorPortTxEthCaptureCompare: the expected packet : \n");
                tgfTrafficTracePacket(expectedPacketBufferPtr, expectedPacketNumOfBytesInPacketBuff, GT_TRUE);
            }
            else
            {
                PRV_TGF_LOG0_MAC("tgfTrafficGeneratorPortTxEthCaptureCompare: the expected packet -- the same as received ! \n");
            }
        }

    }/*while(1)*/


    if( numOfPackets != packetCount )
    {
        PRV_TGF_LOG4_MAC("tgfTrafficGeneratorPortTxEthCaptureCompare: on [%s][%d] expected [%d] packets , got [%d] packets",
                      portInterfacePtr == NULL ? "any" : portInterfacePtr->type == CPSS_INTERFACE_PORT_E ? "port" : "trunk",
                      portInterfacePtr == NULL ? -1    : portInterfacePtr->type == CPSS_INTERFACE_PORT_E ? portInterfacePtr->devPort.portNum : portInterfacePtr->trunkId,
                      numOfPackets, packetCount);

        /* the caller UT must fail if numOfPackets != packetCount */
        rc = GT_FAIL;
    }

exit_cleanly_lbl:

    /* restore default */
    tgfTrafficGeneratorExpectedInterfaceEport(PRV_TGF_EXPECTED_INTERFACE_TYPE_PHYSICAL_PORT_NUM_E);

    if(expectedPacketBufferPtr)
    {
        /* release the buffer back to the pool */
        gtPoolFreeBuf(prvTgfTxBuffersPoolId, expectedPacketBufferPtr);
    }

    PRV_TGF_LOG0_MAC("\n\n");
    *actualCapturedNumOfPacketsPtr = packetCount;

    if(rc == GT_NO_MORE)
    {
        rc = GT_OK;
    }

    return (rc1 != GT_OK) ? rc1 : rc;
}

/*******************************************************************************
* tgfTrafficGeneratorRxInCpuFromExpectedInterfaceGet
*
* DESCRIPTION:
*       Get entry from rxNetworkIf table , for expected interface only.
*       similar to tgfTrafficGeneratorRxInCpuGet , but filtering the packets
*       from non relevant interfaces.
*
* INPUTS:
*       portInterfacePtr - (pointer to) port/trunk interface
*                           NOTE: can be NULL --> meaning 'any interface'
*       packetType      - the packet type to get
*       getFirst        - get first/next entry
*       trace           - enable\disable packet tracing
*       packetBufLenPtr - the length of the user space for the packet
*
* OUTPUTS:
*       packetBufPtr    - packet's buffer (pre allocated by the user)
*       packetBufLenPtr - length of the copied packet to gtBuf
*       packetLenPtr    - Rx packet original length
*       devNumPtr       - packet's device number
*       queuePtr        - Rx queue in which the packet was received.
*       rxParamsPtr     - specific device Rx info format.
*
* RETURNS:
*       GT_OK      - on success
*       GT_NO_MORE - on more entries
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       For 'captured' see API tgfTrafficGeneratorPortTxEthCaptureSet
*
*******************************************************************************/
GT_STATUS tgfTrafficGeneratorRxInCpuFromExpectedInterfaceGet
(
    IN CPSS_INTERFACE_INFO_STC  *portInterfacePtr,
    IN    TGF_PACKET_TYPE_ENT   packetType,
    IN    GT_BOOL               getFirst,
    IN    GT_BOOL               trace,
    OUT   GT_U8                *packetBufPtr,
    INOUT GT_U32               *packetBufLenPtr,
    OUT   GT_U32               *packetLenPtr,
    OUT   GT_U8                *devNumPtr,
    OUT   GT_U8                *queuePtr,
    OUT   TGF_NET_DSA_STC      *rxParamsPtr
)
{
    GT_STATUS   rc;
    GT_BOOL     fromExpectedInterface;

    do{
        rc = tgfTrafficGeneratorRxInCpuGet(packetType,getFirst,trace,
                        packetBufPtr,packetBufLenPtr,
                        packetLenPtr,devNumPtr,queuePtr,rxParamsPtr);
        if(rc != GT_OK)
        {
            /* error / no more packets */
            return rc;
        }

        getFirst = GT_FALSE; /* now we get the next */

        fromExpectedInterface =
            checkPacketFromExpectedInterface(portInterfacePtr,rxParamsPtr);
    }/* skip packets from non wanted interfaces */
    while(fromExpectedInterface == GT_FALSE);


    /* restore default */
    tgfTrafficGeneratorExpectedInterfaceEport(PRV_TGF_EXPECTED_INTERFACE_TYPE_PHYSICAL_PORT_NUM_E);

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorStatDump
*
* DESCRIPTION:
*       Dump statistic of traffic generator
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
GT_VOID tgfTrafficGeneratorStatDump
(
    GT_VOID
)
{
    cpssOsPrintf(" Traffic Generator: Packet Sent OK %d\n", prvTgfTrafficGeneratorNumberPacketsSendOk);
    cpssOsPrintf(" Traffic Generator: Packet Sent Failed %d\n", prvTgfTrafficGeneratorNumberPacketsSendFailed);
}

/*******************************************************************************
* prvTgfTrafficPrintPacketTxEnableSet
*
* DESCRIPTION:
*       Enable or Disable printing of TX Packet event
*
* INPUTS:
*       printEnable - GT_FALSE - disable
*                     GT_TRUE  - enable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       previous state
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfTrafficPrintPacketTxEnableSet
(
    GT_BOOL printEnable
)
{
    GT_BOOL prevState = prvTgfTrafficPrintPacketTxEnable;
    prvTgfTrafficPrintPacketTxEnable = printEnable;
    return prevState;
}

/*******************************************************************************
* prvTgfTrafficPrintPacketTxEnableGet
*
* DESCRIPTION:
*       Returns Enable or Disable printing of TX Packet event
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_FALSE - disable
*       GT_TRUE  - enable
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfTrafficPrintPacketTxEnableGet(GT_VOID)
{
    return prvTgfTrafficPrintPacketTxEnable;
}


extern GT_VOID prvTgfTrafficTableRxPacketSystemReset(GT_VOID);
/*******************************************************************************
* tgfTrafficGeneratorSystemReset
*
* DESCRIPTION:
*       Preparation for system reset
*
* INPUTS:
*       None
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong interface
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID tgfTrafficGeneratorSystemReset
(
    GT_VOID
)
{
    prvTgfTrafficTableRxPacketSystemReset();
    if(prvTgfTxBuffersPoolId)
    {
        gtPoolDeletePool(prvTgfTxBuffersPoolId);
        prvTgfTxBuffersPoolId = 0;
    }

    /* we will need new DMA allocations after system reset */
    generatorPoolInit = GT_FALSE;
}

/*******************************************************************************
* tgfTrafficGeneratorPortTxEthCaptureDoLoopbackSet
*
* DESCRIPTION:
*       indication for function tgfTrafficGeneratorPortTxEthCaptureSet(...)
*       to allow loopback on the 'captured port' or not.
*
* INPUTS:
*       None
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID tgfTrafficGeneratorPortTxEthCaptureDoLoopbackSet
(
    IN GT_BOOL      doLoopback
)
{
    txEthCaptureDoLoopback =   doLoopback;
}

/*******************************************************************************
* prvTgfTxContModeEntryGet
*
* DESCRIPTION:
*       Get entry from transmit continuous mode DB
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       modePtr    - (pointer to) mode entry
*       idxPtr     - (pointer to) entry index
*       freeIdxPtr - (pointer to) free entry index
*
* RETURNS:
*       GT_OK        - on success.
*       GT_NO_SUCH   - on entry not exist.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTxContModeEntryGet
(
    IN GT_U8                            devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT PRV_TGF_TX_DEV_PORT_STC         **modePtr,
    OUT GT_U32                          *idxPtr,
    OUT GT_U32                          *freeIdxPtr
)
{
    GT_U32 freeIndex = PRV_TGF_TX_PORTS_MAX_NUM_CNS; /* index of first free entry */
    GT_U32 ii; /* iterator */
    GT_BOOL found = GT_FALSE; /* is entry found */
    GT_U32 foundIdx = 0; /* index of found entry */
    static PRV_TGF_TX_DEV_PORT_STC dummyEntry = {0};

    *idxPtr = 0;
    if(modePtr)
    {
        *modePtr = &dummyEntry;/* fix Klockwork issue 5287 */
    }

    /* check bitmap */
    if (txDevPortContUsedBmp == 0)
    {
        if(freeIdxPtr)
        {
            *freeIdxPtr = 0;
        }
        if(modePtr)
        {
            *modePtr = &(txDevPortArr[0]);
        }
        return GT_NO_SUCH;
    }

    /* find index in the DB */
    for (ii = 0; ii < PRV_TGF_TX_PORTS_MAX_NUM_CNS; ii++)
    {
        if (txDevPortContUsedBmp & (1 << ii))
        {
            /* check devNum and portNum */
            if ((txDevPortArr[ii].devNum == devNum) &&
                (txDevPortArr[ii].portNum == portNum))
            {
                /* entry found */
                found = GT_TRUE;
                foundIdx = ii;
            }
        }
        else
        {
            freeIndex = ii;
        }
    }

    if(freeIdxPtr)
    {
       *freeIdxPtr = freeIndex;
    }

   if (freeIndex != PRV_TGF_TX_PORTS_MAX_NUM_CNS)
   {
        if(modePtr)
        {
           *modePtr = &(txDevPortArr[freeIndex]);
        }
   }

   if (found == GT_FALSE)
       return GT_NO_SUCH;

    if(modePtr)
    {
       *modePtr = &(txDevPortArr[foundIdx]);
    }
   *idxPtr = foundIdx;

   return GT_OK;
}

/*******************************************************************************
* prvTgfTxModeSetupEth
*
* DESCRIPTION:
*       Transmit mode configuration
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS prvTgfTxModeSetupEth
(
    IN GT_U8                            devNum,
    IN GT_PHYSICAL_PORT_NUM             portNum,
    IN PRV_TGF_TX_MODE_ENT              mode,
    IN PRV_TGF_TX_CONT_MODE_UNIT_ENT    unitsType,
    IN GT_U32                           units
)
{
    GT_U32                      freeIndex;
    GT_U32                      index;
    PRV_TGF_TX_DEV_PORT_STC     *modePtr;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   analyzerInf;
    GT_STATUS                   rc;

    /* get DB entry  */
    rc = prvTgfTxContModeEntryGet(devNum, portNum, &modePtr, &index, &freeIndex);

    switch (mode)
    {
        case PRV_TGF_TX_MODE_SINGLE_BURST_E:
            prvTgfFwsUse = 0;
            if (rc != GT_OK)
            {
                /* port already in burst mode or FWS mode */
                return GT_OK;
            }

            /* mark entry as free*/
            PRV_TGF_TX_MODE_FREE_DB_ENTRY(index);

            if (!PRV_TRG_TX_MODE_DB_NOT_EMPTY)
            {
                /* restore mirroring mode */
                rc = cpssDxChMirrorToAnalyzerForwardingModeSet(devNum,mirrModeStore);
                if (rc != GT_OK)
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChMirrorToAnalyzerForwardingModeSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* restore Rx descriptors limits */
                rc = cpssDxChPortTxSniffedPcktDescrLimitSet(devNum,
                                                          mirrRxDescStore,
                                                          mirrTxDescStore);
                if (rc != GT_OK)
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChPortTxSniffedPcktDescrLimitSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* restore Rx buffers limits */
                rc = cpssDxChPortTxSniffedBuffersLimitSet(devNum,
                                                        mirrRxDescStore,
                                                        mirrTxDescStore);
                if ((rc != GT_OK) && (rc != GT_NOT_APPLICABLE_DEVICE))
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChPortTxSniffedBuffersLimitSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* restore mirroring enhanced priority mode */
                if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E ||
                   PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
                {
                    rc = cpssDxChMirrorEnhancedMirroringPriorityModeSet(devNum, mirrPriorityMode);
                    if ((rc != GT_OK) && (rc != GT_NOT_APPLICABLE_DEVICE))
                    {
                        PRV_TGF_LOG1_MAC("[TGF]: cpssDxChMirrorEnhancedMirroringPriorityModeSet FAILED, rc = [%d]", rc);
                        return rc;
                    }
                }
            }

            {
                /* restore analyzer */
                CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC cpss_analyzerInf;
                cpss_analyzerInf.interface = modePtr->analyzerInf.interface;
                rc = cpssDxChMirrorAnalyzerInterfaceSet(devNum,index,&cpss_analyzerInf);
                if (rc != GT_OK)
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChMirrorAnalyzerInterfaceSet FAILED, rc = [%d]", rc);
                    return rc;
                }
            }

            break;
        case PRV_TGF_TX_MODE_CONTINUOUS_E:
            prvTgfFwsUse = 0;
            if (rc == GT_OK)
            {
                /* port is already in the mode.
                  update DB entry. */
                modePtr->units = units;
                modePtr->unitsType = unitsType;
                return GT_OK;
            }

            if (freeIndex == PRV_TGF_TX_PORTS_MAX_NUM_CNS)
            {
                /* there is no place for new port */
                return GT_FULL;
            }

            if (!PRV_TRG_TX_MODE_DB_NOT_EMPTY)
            {
                /* this is first port in continuous mode.
                   configure mirroring.                    */
                /* store mode */
                rc = cpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mirrModeStore);
                if (rc != GT_OK)
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChMirrorToAnalyzerForwardingModeGet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* set source based mode */
                rc = cpssDxChMirrorToAnalyzerForwardingModeSet(devNum,CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);
                if (rc != GT_OK)
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChMirrorToAnalyzerForwardingModeSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* store Tx and Rx descriptors limits */
                rc = cpssDxChPortTxSniffedPcktDescrLimitGet(devNum, &mirrRxDescStore,&mirrTxDescStore);
                if (rc != GT_OK)
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChPortTxSniffedPcktDescrLimitGet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* set Rx descriptors limits to be WS burst * number of mirrored ports */
                rc = cpssDxChPortTxSniffedPcktDescrLimitSet(devNum,
                                                          prvTgfWsBurst * PRV_TGF_TX_PORTS_MAX_NUM_CNS,
                                                          mirrTxDescStore);
                if (rc != GT_OK)
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChPortTxSniffedPcktDescrLimitSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* set Rx buffers limits to be WS burst * number of mirrored ports */
                rc = cpssDxChPortTxSniffedBuffersLimitSet(devNum,
                                                        prvTgfWsBurst * PRV_TGF_TX_PORTS_MAX_NUM_CNS,
                                                        mirrTxDescStore);
                if ((rc != GT_OK) && (rc != GT_NOT_APPLICABLE_DEVICE))
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChPortTxSniffedBuffersLimitSet FAILED, rc = [%d]", rc);
                    return rc;
                }

                /* xCat and xCat3 device has weak mirroring duplication arbitrer with default settings.
                   Need to provide priority to duplicated traffic for stable WS generation. */
                if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E ||
                   PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
                {
                    /* store mirroring priority mode */
                    rc = cpssDxChMirrorEnhancedMirroringPriorityModeGet(devNum,&mirrPriorityMode);
                    /* first Xcat revisions does not support the feature - ignore error */
                    if ((rc != GT_OK) && (rc != GT_NOT_APPLICABLE_DEVICE))
                    {
                        PRV_TGF_LOG1_MAC("[TGF]: cpssDxChMirrorEnhancedMirroringPriorityModeGet FAILED, rc = [%d]", rc);
                        return rc;
                    }

                    if(rc == GT_OK)
                    {
                        rc = cpssDxChMirrorEnhancedMirroringPriorityModeSet(devNum,CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_SP_INGRESS_EGRESS_MIRROR_E);
                        if (rc != GT_OK)
                        {
                            PRV_TGF_LOG1_MAC("[TGF]: cpssDxChMirrorEnhancedMirroringPriorityModeSet FAILED, rc = [%d]", rc);
                            return rc;
                        }
                    }
                }
            }

            /* mark entry as allocated */
            PRV_TGF_TX_MODE_ALLOC_DB_ENTRY(freeIndex);
            modePtr->devNum = devNum;
            modePtr->portNum = portNum;
            modePtr->units = units;
            modePtr->unitsType = unitsType;


            {
                CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC cpss_analyzerInf;
                /* store analyzer interface */
                rc = cpssDxChMirrorAnalyzerInterfaceGet(devNum, freeIndex, &cpss_analyzerInf);
                modePtr->analyzerInf.interface = cpss_analyzerInf.interface;
                if (rc != GT_OK)
                {
                    PRV_TGF_LOG1_MAC("[TGF]: cpssDxChMirrorAnalyzerInterfaceGet FAILED, rc = [%d]", rc);
                    return rc;
                }
            }

            analyzerInf.interface.type = CPSS_INTERFACE_PORT_E;
            analyzerInf.interface.devPort.hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
            analyzerInf.interface.devPort.portNum = portNum;

            /* set analyzer to be Tx port */
            rc = cpssDxChMirrorAnalyzerInterfaceSet(devNum,freeIndex,&analyzerInf);
            if (rc != GT_OK)
            {
                PRV_TGF_LOG1_MAC("[TGF]: cpssDxChMirrorAnalyzerInterfaceSet FAILED, rc = [%d]", rc);
                return rc;
            }
            break;
        case PRV_TGF_TX_MODE_CONTINUOUS_FWS_E:
            prvTgfFwsUse = 1;
            break;
        default: return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficGeneratorWsModeOnPortSet
*
* DESCRIPTION:
*       set WS mode on port , by setting rxMirror and setting proper tx mode
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  tgfTrafficGeneratorWsModeOnPortSet(IN GT_U8    devNum, IN GT_U32  portNum)
{
    GT_U32                      mirrIndex;
    GT_STATUS rc,modeRc;
    /* configure WS mode */
    rc = prvTgfTxModeSetupEth(devNum, portNum,
                              PRV_TGF_TX_MODE_CONTINUOUS_E,
                              PRV_TGF_TX_CONT_MODE_UNIT_WS_E,
                              0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get DB entry  */
    modeRc = prvTgfTxContModeEntryGet(devNum, portNum,NULL, &mirrIndex, NULL);
    if (modeRc == GT_OK)
    {
        /* continuous mode is chosen */
        rc = cpssDxChMirrorRxPortSet(devNum, portNum,
                               GT_FALSE /* eport not physical port */ ,
                               GT_TRUE,/* enable */
                                mirrIndex);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}
/*******************************************************************************
* tgfTrafficGeneratorStopWsOnPort
*
* DESCRIPTION:
*       stop WS on port , by disable rxMirror and setting proper tx mode
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  tgfTrafficGeneratorStopWsOnPort(IN GT_U8    devNum, IN GT_U32  portNum)
{
    GT_STATUS                   rc,modeRc;
    GT_U32 mirrIndex;
    CPSS_INTERFACE_INFO_STC portInterface;

    /* get DB entry  */
    modeRc = prvTgfTxContModeEntryGet(devNum, portNum,NULL, &mirrIndex, NULL);
    if (modeRc != GT_OK)
    {
        /* port in burst mode. */
        return GT_NOT_FOUND;
    }

    /* do not return on error from here , because need to restore the mode */

    /*****************************/
    /** disable the mac loopback */
    /*****************************/

    cpssOsMemSet(&portInterface, 0, sizeof(portInterface));
    /* setup receive outPortInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.hwDevNum  = devNum;/*doing convert to PRV_CPSS_HW_DEV_NUM_MAC(devNum); inside tgfTrafficGeneratorPortLoopbackModeEnableSet */
    portInterface.devPort.portNum = portNum;

    /* the Port Loopback disable under traffic may result in problems on port.
      Disable port MAC before disable loopback to minimize problems */
    (void)cpssDxChPortEnableSet(devNum, portNum, GT_FALSE);

    /* disable loopback mode on port */
    (void) tgfTrafficGeneratorPortLoopbackModeEnableSet(&portInterface, GT_FALSE);

    /* restore the port */
    (void)cpssDxChPortEnableSet(devNum, portNum, GT_TRUE);

    /*****************************/
    /** disable the rx mirroring */
    /*****************************/
    (void)cpssDxChMirrorRxPortSet(devNum, portNum,
                           GT_FALSE /* eport not physical port */ ,
                           GT_FALSE,/* disable not enable */
                            mirrIndex);

    /* configure default mode */
    rc = prvTgfTxModeSetupEth(devNum, portNum,
                              PRV_TGF_TX_MODE_SINGLE_BURST_E,
                              PRV_TGF_TX_CONT_MODE_UNIT_WS_E,
                          0);
    return rc;
}


/*******************************************************************************
* prvTgfCommonDevicePortWsRateGet
*
* DESCRIPTION:
*       Get wire speed rate of port in packets per second
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       packetSize - packet size in bytes including CRC
*
* OUTPUTS:
*       none
*
* RETURNS:
*       wire speed rate of port in packets per second.
*       0xFFFFFFFF - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonDevicePortWsRateGet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN  GT_U32 packetSize
)
{
    GT_STATUS   rc;
    CPSS_PORT_SPEED_ENT portSpeed; /* port speed in enum values */
    GT_U32      speed;        /* speed in kilobits per second */
    double      packetRate;   /* packet rate in packets per second */
    GT_U32      l1PacketSize; /* packet size in media in bits */
    GT_U32      rateOut;      /* integer value of rate */

    /* get start value - reset counter */
    rc =  cpssDxChPortSpeedGet(devNum, portNum,&portSpeed);
    if (rc != GT_OK)
    {
        PRV_TGF_LOG1_MAC("cpssDxChPortSpeedGet: %d", portNum);
        return 0xFFFFFFFF;
    }

    switch(portSpeed)
    {
        case CPSS_PORT_SPEED_10_E:    speed =     10000; break;
        case CPSS_PORT_SPEED_100_E:   speed =    100000; break;
        case CPSS_PORT_SPEED_1000_E:  speed =   1000000; break;
        case CPSS_PORT_SPEED_10000_E: speed =  10000000; break;
        case CPSS_PORT_SPEED_12000_E: speed =  12000000; break;
        case CPSS_PORT_SPEED_2500_E:  speed =   2500000; break;
        case CPSS_PORT_SPEED_5000_E:  speed =   5000000; break;
        case CPSS_PORT_SPEED_13600_E: speed =  13600000; break;
        case CPSS_PORT_SPEED_20000_E: speed =  20000000; break;
        case CPSS_PORT_SPEED_40000_E: speed =  40000000; break;
        case CPSS_PORT_SPEED_16000_E: speed =  16000000; break;
        case CPSS_PORT_SPEED_15000_E: speed =  15000000; break;
        case CPSS_PORT_SPEED_100G_E:  speed = 100000000; break;
        case CPSS_PORT_SPEED_75000_E: speed =  75000000; break;
        default:
            PRV_TGF_LOG1_MAC("wrong speed: %d", portSpeed);return 0xFFFFFFFF;
    }

    /* add 20 bytes IPG and preamble to get L1 byte size */
    l1PacketSize = (packetSize + 20) * 8;

    packetRate = speed;
    packetRate = (packetRate * 1000) / l1PacketSize;

    /* convert floating point to integer */
    rateOut = (GT_U32) packetRate;

    return rateOut;
}

/*******************************************************************************
* prvTgfVntPortLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       timeStampValuePtr - (pointer to) timestamp of the last read operation
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfVntPortLastReadTimeStampGet
(
    IN  GT_U8                         devNum,
    IN  GT_PHYSICAL_PORT_NUM          portNum,
    OUT GT_U32                       *timeStampValuePtr
)
{
#ifdef CHX_FAMILY
    GT_PORT_GROUPS_BMP  portGroupBmp;

    /* port group are in bits 4, 5 of port number */
    portGroupBmp = 1 << (portNum >> 4);

    /* call device specific API */
    return cpssDxChVntPortGroupLastReadTimeStampGet(devNum, portGroupBmp, timeStampValuePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    GT_PORT_GROUPS_BMP  portGroupBmp;

    /* port group are in bits 4, 5 of port number */
    portGroupBmp = 1 << (portNum >> 4);

     /* call device specific API */
    return cpssExMxPmVntPortGroupLastReadTimeStampGet(devNum, portGroupBmp, timeStampValuePtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}


/*******************************************************************************
* prvUtfDeviceCoreClockGet
*
* DESCRIPTION:
*       Get core clock in Herz of device
*
* INPUTS:
*       dev         -   device id of ports
*
* OUTPUTS:
*       coreClockPtr - (pointer to)core clock in Herz
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfDeviceCoreClockGet
(
    IN GT_U8          dev,
    OUT GT_U32       *coreClockPtr
)
{
    /* device must be active */
    DEV_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(coreClockPtr);

    switch (PRV_CPSS_PP_MAC(dev)->coreClock)
    {
        case 222: *coreClockPtr = 222222222; break;
        case 167: *coreClockPtr = 166666667; break;
        case 362: *coreClockPtr = 362500000; break;
        default: *coreClockPtr = PRV_CPSS_PP_MAC(dev)->coreClock * 1000000;
            break;
    }
    return GT_OK;
}

/*******************************************************************************
* prvTgfCommonVntTimeStampsRateGet
*
* DESCRIPTION:
*       Get rate of events by VNT Time Stamps.
*
* INPUTS:
*       dev                 - device number
*       startTimeStamp      - start counting time stamp
*       endTimeStamp        - end counting time stamp
*       eventCount          - events count between end and start time stamps
*
* OUTPUTS:
*       none
*
* RETURNS:
*       events rate in events per second
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonVntTimeStampsRateGet
(
    IN  GT_U8          dev,
    IN  GT_U32         startTimeStamp,
    IN  GT_U32         endTimeStamp,
    IN  GT_U32         eventCount
)
{
    GT_STATUS rc;           /* return code*/
    GT_U32    coreClock;    /* device core clock in HZ */
    double    diff;         /* difference between end and start time stamps */
    double    rate;         /* rate of events */
    GT_U32    rateOut;      /* integer value of rate */

    /* get core closk in HZ */
    rc = prvUtfDeviceCoreClockGet(dev,&coreClock);
    if (rc != GT_OK)
    {
        PRV_TGF_LOG1_MAC("prvUtfDeviceCoreClockGet: %d", dev);
        return 0xFFFFFFFF;
    }

    /* calculate difference between end and start */
    if (startTimeStamp > endTimeStamp)
    {
       /* take care of wraparound of end time */
       diff = 0xFFFFFFFFU;
       diff = diff + 1 + endTimeStamp - startTimeStamp;
    }
    else
       diff = endTimeStamp - startTimeStamp;

   /* time stamp is in device core clocks.
     event rate is number of events divided by time between them.
     the time is core clock period multiplied on number of core clocks.
     rate = eventCount / time, where time = diff * (1/coreClock) */
   rate = eventCount;
   rate = (rate * coreClock) / diff;

   /* convert float point value to unsigned integer */
   rateOut = (GT_U32)rate;

   return rateOut;
}

/*******************************************************************************
* prvTgfGetRate
*
* DESCRIPTION:
*       Get rate in event per second.
*
* INPUTS:
*       countEnd   - events count in the end of period
*       countStart - events count in the start of period
*       timeOutMilliSec - timeout in milli seconds
*
* OUTPUTS:
*       None
*
* RETURNS:
*       rate in events per seconds
*
* COMMENTS:
*
*******************************************************************************/
GT_U32  prvTgfGetRate(IN GT_U32 countEnd, IN GT_U32 countStart, IN GT_U32 timeOutMilliSec)
{
    GT_U32 delta; /* difference between end and start */
    GT_U32 rate;  /* result */

    delta = countEnd - countStart;
    if(delta < (0xFFFFFFFF / 1000))
    {
        rate = (1000 * delta) / timeOutMilliSec;
    }
    else if (delta < (0xFFFFFFFF / 100))
    {
        rate = ((100 * delta) / timeOutMilliSec) * 10;
    }
    else if (delta < (0xFFFFFFFF / 10))
    {
        rate = ((10 * delta) / timeOutMilliSec) * 100;
    }
    else
    {
        rate = 1000 * (delta / timeOutMilliSec);
    }
    return rate;
}

/*******************************************************************************
* prvTgfCommonPortsCntrRateGet_byDevPort
*
* DESCRIPTION:
*       Get rate of specific ports MAC MIB for up to 2 counters.
*
* INPUTS:
*       devNumArr - array of devNum
*       portNumArr - array of portNum
*       numOfPorts - number of ports
*       cntrName1 - name of first counter
*       timeOut - timeout in milli seconds to calculate rate
*       cntrName2 - name of second counter
*
* OUTPUTS:
*       outRateArr1 - (pointer to) array of rate of ports for cntrName1.
*       outRateArr2 - (pointer to) array of rate of ports for cntrName2.
*                       if NULL then this rate ignored.
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCommonPortsCntrRateGet_byDevPort
(
    IN  GT_U8  devNumArr[],
    IN  GT_U32 portNumArr[],
    IN  GT_U32 numOfPorts,
    IN  GT_U32 timeOut,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName1,
    OUT GT_U32 outRateArr1[],
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName2,
    OUT GT_U32 outRateArr2[]
)
{
    GT_U64      cntrValueArr1[GET_RATE_MAX_PORTS_CNS];  /* value of counters */
    GT_U64      cntrValueArr2[GET_RATE_MAX_PORTS_CNS];  /* value of counters */
    GT_U32      startSec, endSec;     /* start and end time seconds part */
    GT_U32      startNano, endNano;   /* start and end time nanoseconds part */
    GT_U32      startMilli, endMilli; /* start and end time in milliseconds */
    GT_U32      portNum;              /* port number */
    GT_U8       devNum;
    GT_STATUS   rc;                   /* return code */
    GT_U32      startTimeStampArr[GET_RATE_MAX_PORTS_CNS];       /* VNT time stamp start */
    GT_U32      endTimeStampArr[GET_RATE_MAX_PORTS_CNS];         /* VNT time stamp start */
    GT_U32      rate;                 /* VNT time stamp based rate */
    GT_U32      ii;                   /* iterator */
    CPSS_PORT_MAC_COUNTERS_ENT        cntrNameArr1[3]; /* names of counters */
    CPSS_PORT_MAC_COUNTERS_ENT        cntrNameArr2[3]; /* names of counters */
    GT_U32      numOfCntrs1,numOfCntrs2;           /* number of counters */
    GT_U32      cntrNameIdx;          /* iterator */
    GT_U64      tmp;                  /* temp variable */

    for (ii = 0; ii < numOfPorts; ii++)
    {
        outRateArr1[ii] = 0;
        if(outRateArr2)
        {
            outRateArr2[ii] = 0;
        }
    }

    /* check number of ports */
    if (numOfPorts > GET_RATE_MAX_PORTS_CNS || numOfPorts == 0)
    {
        /*error*/
        return GT_BAD_SIZE;
    }

    if (cntrName1 == CPSS_GOOD_PKTS_RCV_E)
    {
        cntrNameArr1[0] = CPSS_BRDC_PKTS_RCV_E;
        cntrNameArr1[1] = CPSS_MC_PKTS_RCV_E;
        cntrNameArr1[2] = CPSS_GOOD_UC_PKTS_RCV_E;
        numOfCntrs1     = 3;
    }
    else if (cntrName1 == CPSS_GOOD_PKTS_SENT_E)
    {
        cntrNameArr1[0] = CPSS_BRDC_PKTS_SENT_E;
        cntrNameArr1[1] = CPSS_MC_PKTS_SENT_E;
        cntrNameArr1[2] = CPSS_GOOD_UC_PKTS_SENT_E;
        numOfCntrs1     = 3;
    }
    else
    {
        cntrNameArr1[0] = cntrName1;
        numOfCntrs1     = 1;
    }

    if(outRateArr2 == NULL)
    {
        numOfCntrs2 = 0;
    }
    else
    if (cntrName2 == CPSS_GOOD_PKTS_RCV_E)
    {
        cntrNameArr2[0] = CPSS_BRDC_PKTS_RCV_E;
        cntrNameArr2[1] = CPSS_MC_PKTS_RCV_E;
        cntrNameArr2[2] = CPSS_GOOD_UC_PKTS_RCV_E;
        numOfCntrs2     = 3;
    }
    else if (cntrName2 == CPSS_GOOD_PKTS_SENT_E)
    {
        cntrNameArr2[0] = CPSS_BRDC_PKTS_SENT_E;
        cntrNameArr2[1] = CPSS_MC_PKTS_SENT_E;
        cntrNameArr2[2] = CPSS_GOOD_UC_PKTS_SENT_E;
        numOfCntrs2     = 3;
    }
    else
    {
        cntrNameArr2[0] = cntrName2;
        numOfCntrs2     = 1;
    }

    /* reset counters by read them */
    for (ii = 0; ii < numOfPorts; ii++)
    {
        devNum =  devNumArr[ii];
        portNum = portNumArr[ii];

        if(0 == DEV_SUPPORT_VNT_TIME_STAMPS_MAC(devNum))
        {
            prvTgfCommonVntTimeStampsInUse = 0;
        }

        for (cntrNameIdx = 0; cntrNameIdx < numOfCntrs1; cntrNameIdx++)
        {
            rc = prvTgfPortMacCounterGet(devNum, portNum,
                                         cntrNameArr1[cntrNameIdx],
                                         &tmp);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        for (cntrNameIdx = 0; cntrNameIdx < numOfCntrs2; cntrNameIdx++)
        {
            rc = prvTgfPortMacCounterGet(devNum, portNum,
                                         cntrNameArr2[cntrNameIdx],
                                         &tmp);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        rc = prvTgfVntPortLastReadTimeStampGet(devNum, portNum, &startTimeStampArr[ii]);
        if(rc != GT_OK && prvTgfCommonVntTimeStampsInUse)
        {
            return rc;
        }
    }

    /* store start time */
    cpssOsTimeRT(&startSec,&startNano);

    /* sleep */
    cpssOsTimerWkAfter(timeOut);

    /* get counter */
    for (ii = 0; ii < numOfPorts; ii++)
    {
        devNum =  devNumArr[ii];
        portNum = portNumArr[ii];

        cntrValueArr1[ii].l[0] = 0;
        for (cntrNameIdx = 0; cntrNameIdx < numOfCntrs1; cntrNameIdx++)
        {
            rc =  prvTgfPortMacCounterGet(devNum, portNum,
                                          cntrNameArr1[cntrNameIdx],
                                          &tmp);
            if(rc != GT_OK)
            {
                return rc;
            }
            cntrValueArr1[ii].l[0] += tmp.l[0];
        }

        cntrValueArr2[ii].l[0] = 0;
        for (cntrNameIdx = 0; cntrNameIdx < numOfCntrs2; cntrNameIdx++)
        {
            rc =  prvTgfPortMacCounterGet(devNum, portNum,
                                          cntrNameArr2[cntrNameIdx],
                                          &tmp);
            if(rc != GT_OK)
            {
                return rc;
            }
            cntrValueArr2[ii].l[0] += tmp.l[0];
        }

        rc = prvTgfVntPortLastReadTimeStampGet(devNum, portNum, &endTimeStampArr[ii]);
        if(rc != GT_OK && prvTgfCommonVntTimeStampsInUse)
        {
            return rc;
        }
    }

    /* get end time */
    cpssOsTimeRT(&endSec,&endNano);

    startMilli = startSec * 1000 + startNano / 1000000;
    endMilli = endSec * 1000 + endNano / 1000000;

    timeOut = (endMilli - startMilli);

    for (ii = 0; ii < numOfPorts; ii++)
    {
        portNum = portNumArr[ii];
        outRateArr1[ii] = prvTgfGetRate(cntrValueArr1[ii].l[0], 0, timeOut);
        if (prvTgfCommonVntTimeStampsDebug)
        {
            PRV_TGF_LOG2_MAC("---cpssOsTimeRT port %d rate %d\n", portNum, outRateArr1[ii]);
        }

        if(outRateArr2)
        {
            outRateArr2[ii] = prvTgfGetRate(cntrValueArr2[ii].l[0], 0, timeOut);
            if (prvTgfCommonVntTimeStampsDebug)
            {
                PRV_TGF_LOG2_MAC("---cpssOsTimeRT port %d rate %d\n", portNum, outRateArr2[ii]);
            }
        }
    }

    if (prvTgfCommonVntTimeStampsInUse)
    {
        for (ii = 0; ii < numOfPorts; ii++)
        {
            devNum =  devNumArr[ii];

            outRateArr1[ii] = prvTgfCommonVntTimeStampsRateGet(devNum, startTimeStampArr[ii], endTimeStampArr[ii], cntrValueArr1[ii].l[0]);

            if(outRateArr2)
            {
                outRateArr2[ii] = prvTgfCommonVntTimeStampsRateGet(devNum, startTimeStampArr[ii], endTimeStampArr[ii], cntrValueArr2[ii].l[0]);
            }
        }
    }

    if (prvTgfCommonVntTimeStampsDebug)
    {
        for (ii = 0; ii < numOfPorts; ii++)
        {
            devNum =  devNumArr[ii];
            portNum = portNumArr[ii];

            rate = prvTgfCommonVntTimeStampsRateGet(devNum, startTimeStampArr[ii], endTimeStampArr[ii], cntrValueArr1[ii].l[0]);
            PRV_TGF_LOG5_MAC("---port %d rate1 %d, start %d end %d count %d \n", portNum, rate, startTimeStampArr[ii], endTimeStampArr[ii], cntrValueArr1[ii].l[0]);

            if(outRateArr2)
            {
                rate = prvTgfCommonVntTimeStampsRateGet(devNum, startTimeStampArr[ii], endTimeStampArr[ii], cntrValueArr2[ii].l[0]);
                PRV_TGF_LOG5_MAC("---port %d rate2 %d, start %d end %d count %d \n", portNum, rate, startTimeStampArr[ii], endTimeStampArr[ii], cntrValueArr2[ii].l[0]);
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfCommonDiffInPercentCalc
*
* DESCRIPTION:
*       Calculate difference between input values in percent
*
* INPUTS:
*       value1    - value1 to compare
*       value2    - value2 to compare
*
* OUTPUTS:
*       none
*
* RETURNS:
*       difference between input values in percent
*
* COMMENTS:
*
*******************************************************************************/
GT_U32 prvTgfCommonDiffInPercentCalc
(
    IN  GT_U32 value1,
    IN  GT_U32 value2
)
{
    GT_U32      delta ;      /* difference between ports WS rate and input one*/
    GT_U32      percent;     /* difference between ports WS rate
                               and input one in percent */

    if (value1 > value2)
    {
        delta = value1 - value2;
    }
    else
    {
        delta = value2 - value1;
    }

    /* calculate difference in percent of port's rate */
    /* By multiplyingby two, adding one and dividing by two, you are
        effectively adding a half. This makes the integer division do a
        rounding instead of truncating.*/
    percent = (200 * delta + 1) / (value1 * 2);

    return percent;
}

