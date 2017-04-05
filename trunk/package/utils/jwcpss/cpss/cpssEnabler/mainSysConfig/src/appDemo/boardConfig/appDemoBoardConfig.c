/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* appDemoBoardConfig.c
*
* DESCRIPTION:
*       Includes board specific initialization definitions and data-structures.
*
* FILE REVISION NUMBER:
*       $Revision: 131 $
*
*******************************************************************************/

#if defined(SHARED_MEMORY)
/* to allow vfork() and kill() */
#define _GNU_SOURCE
#endif
#include <gtExtDrv/drivers/gtIntDrv.h>
#include <gtExtDrv/drivers/gtPciDrv.h>
#include <gtExtDrv/drivers/gtCacheMng.h>
#include <gtExtDrv/drivers/gtSmiHwCtrl.h>
#include <gtExtDrv/drivers/gtTwsiHwCtrl.h>
#include <gtExtDrv/drivers/gtDmaDrv.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>
#include <gtExtDrv/drivers/gtHsuDrv.h>
#include <gtExtDrv/drivers/gtI2cDrv.h>
#include <gtExtDrv/drivers/gtHwIfDrv.h>
#include <gtExtDrv/drivers/gtDragoniteDrv.h>

#include <cpss/generic/init/cpssInit.h>
#include <cpss/extServices/cpssExtServices.h>

#include <gtOs/gtOsExc.h>
#include <gtOs/gtOsInet.h>
#include <gtStack/gtOsSocket.h>
#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsDebug.h>

#ifdef  LINUX
#include <gtOs/gtOsStdLib.h>
#endif

#include <gtOs/gtOsMsgQ.h>
#include <gtUtil/gtBmPool.h>
#include <gtOs/gtOsStr.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsTimer.h>
#include <gtOs/gtOsStdLib.h>
#include <gtOs/gtOsMem.h>

#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/gtDbDxBobcat2PhyConfig.h>
#include <appDemo/confi/confi2.h>

#include <appDemo/userExit/userEventHandler.h>

/* For software CPSS version information */
#include <cpss/generic/version/cpssGenVersion.h>
#include <cpss/generic/version/cpssGenStream.h>

#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <extUtils/trafficEngine/tgfTrafficGenerator.h>
#include <cmdShell/FS/cmdFS.h>


#if defined(SHARED_MEMORY)
#if (!defined(LINUX_SIM))
/* Some ABI for BM with shared memory should use work-around for pointers to functions */
#include <gtOs/gtOsSharedFunctionPointers.h>
#endif
#include <gtOs/gtOsSharedUtil.h>

#include <sys/types.h>
#include <sys/signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#endif

#if (defined(LINUX) && !defined(ASIC_SIMULATION))
#include <stdlib.h>
#endif

#include <cmdShell/common/cpssApiCallLock.h>
#include <cpss/generic/private/prvCpssGlobalMutex.h>

#ifdef IMPL_FA
#include <cpssFa/generic/version/gtFaVersion.h>
#endif  /* IMPL_FA */

#ifdef IMPL_XBAR
#include <cpssXbar/generic/version/gtXbarVersion.h>
#endif  /* IMPL_XBAR */

#if (defined EX_FAMILY) || (defined MX_FAMILY)
#include <cpss/exMx/exMxGen/version/cpssExMxVersion.h>
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY) */

#ifdef CHX_FAMILY
        #include <cpss/dxCh/dxChxGen/version/cpssDxChVersion.h>
        #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
        #include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDescriptor.h>
        #include <appDemo/utils/hsu/appDemoDxChHsuUtils.h>

        extern const GT_CHAR* mvHwsVersionGet(void);
        extern GT_STATUS appDemoBc2PortInterfaceInit
        (
                IN  GT_U8 dev,
                IN  GT_U8 boardRevId
        );
        GT_STATUS bobcat2BoardPhyConfig
        (
                IN  GT_U8       boardRevId,
                IN  GT_U8       devNum
        );
        GT_STATUS cpssDxChPhyPortSmiInit
        (
                IN GT_U8 devNum
        );
        GT_STATUS cpssDxChBrgVlanPortDelete
        (
                IN GT_U8                        devNum,
                IN GT_U16                       vlanId,
                IN GT_PHYSICAL_PORT_NUM         portNum
        );
#endif  /* CHX_FAMILY */

#ifdef SAL_FAMILY
        #include <cpss/dxSal/version/cpssDxSalVersion.h>
#endif  /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
        #include <cpss/exMxPm/exMxPmGen/version/cpssExMxPmVersion.h>
        #include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
        #include <appDemo/utils/extTcam/appDemoExMxPmExternalTcamConfig.h>
        #include <appDemo/utils/extTcam/appDemoExMxPmExternalTcamUtils.h>
        #include <cpss/exMxPm/exMxPmGen/diag/cpssExMxPmDiagDescriptor.h>
        #include <appDemo/utils/hsu/appDemoExMxPmHsuUtils.h>
        #include <appDemo/utils/port/appDemoExMxPmPortUtils.h>
        #include <extUtils/trafficEngine/tgfTrafficEngine.h>
        extern const GT_CHAR* mvHwsVersionGet(void);
#endif  /* EXMXPM_FAMILY */

#ifdef SHARED_MEMORY
        extern APP_DEMO_SYS_CONFIG_STC appDemoSysConfig;
        CPSS_DXCH_MP_SMI_TRANSMIT_PARAMS_STC ethPortParams;
#endif /*SHLIB */


#include <cmdShell/common/cmdCommon.h>
#include <appDemo/userExit/userEventHandler.h>

#include <extUtils/auEventHandler/auFdbUpdateLock.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#ifdef IMPL_GALTIS
        extern GT_U32 *enhUtUseCaptureToCpuPtr;
        #include <extUtils/trafficEngine/private/prvTgfInternalDefs.h>
#endif
#ifdef CHX_FAMILY
        #include <cpss/dxCh/dxChxGen/tcam/cpssDxChTcam.h>
        extern GT_U32 appDemoDxChLion3TcamPclConvertedIndexGet
        (
                IN     GT_U8                                devNum,
                IN     GT_U32                               index,
                IN     CPSS_DXCH_TCAM_RULE_SIZE_ENT         ruleSize
        );
        typedef GT_U32 (*APP_DEMO_DXCH_LION3_TCAM_PCL_CONVERTED_INDEX_GET_FUN)
        (
                IN     GT_U8                                devNum,
                IN     GT_U32                               index,
                IN     CPSS_DXCH_TCAM_RULE_SIZE_ENT         ruleSize
        );
        extern APP_DEMO_DXCH_LION3_TCAM_PCL_CONVERTED_INDEX_GET_FUN appDemoDxChLion3TcamPclConvertedIndexGetPtr;

#endif /* CHX_FAMILY */

        typedef GT_STATUS (*appDemoDbEntryGet_TYPE)
        (
            IN  GT_CHAR *namePtr,
            OUT GT_U32  *valuePtr
        );
        extern appDemoDbEntryGet_TYPE appDemoDbEntryGet_func;

#if (!defined(FREEBSD) && !defined(UCLINUX) && !defined(LINUX) && !defined(_WIN32) && !defined(ASIC_SIMULATION))
        /* This is standard VxWorks function which returns the BSP version */
        extern char *sysBspRev(void);
#endif

#if (defined CHX_FAMILY) && (!defined EX_FAMILY)
        GT_STATUS gtAppDemoLionPortModeSpeedSet(GT_U8 devNum, GT_U8 portNum, CPSS_PORT_INTERFACE_MODE_ENT ifMode, CPSS_PORT_SPEED_ENT speed);
        GT_STATUS gtAppDemoXcat2StackPortsModeSpeedSet
        (
                IN  GT_U8     devNum,
                IN  GT_U8     portNum,
                IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
                IN  CPSS_PORT_SPEED_ENT  speed
        );
        GT_STATUS gtAppDemoXcat2SfpPortEnable
        (
                IN  GT_U8     devNum,
                IN  GT_U8     portNum,
                IN  GT_BOOL   enable
        );
        GT_STATUS util_appDemoDxChPortFWSRatesTableGet
        (
                IN  GT_U8 devNum,
                IN  GT_U32 arrSize,
                OUT CMD_PORT_FWS_RATE_STC *rateDbPtr
        );
#endif


/**********************************************************************************
        if multiProcessAppDemo = 0 the appDemo run as regular (one process)
        if multiProcessAppDemo = 1 the appDemo run as multiprocess application
                the multiprocess application includes:
                - appDemo process
                - fdb learning process
                - RxTx process
        the variable multiProcessAppDemo used in file userEventHandler.c to define
        the event handler configuration
*********************************************************************************/
int multiProcessAppDemo = 0;

/*******************************************************************************
 * External variables
 ******************************************************************************/
GT_U32                          appDemoPpConfigDevAmount;
CPSS_PP_CONFIG_INIT_STC         ppUserLogicalConfigParams[PRV_CPSS_MAX_PP_DEVICES_CNS];
extern APP_DEMO_PP_CONFIG       appDemoPpConfigList[];
extern GT_BOOL                  systemInitialized;
extern GT_BOARD_LIST_ELEMENT    boardsList[];
extern GT_U32                   boardListLen;   /* Length of boardsList array.  */
/* Enable printing inside interrupt routine. */
extern GT_STATUS extDrvUartInit
(
    GT_VOID
);
#ifdef  INCLUDE_UTF
/* Initialize Unit Test framework for CPSS */
extern GT_STATUS utfPreInitPhase
(
    GT_VOID
);

extern GT_STATUS utfInit
(
    GT_U8 firstDevNum
);
#endif
#ifdef IMPL_DUNE
extern GT_STATUS duneUtfPreInitPhase
(
    GT_VOID
);

extern GT_STATUS duneUtfInit
(
    GT_VOID
);
#endif /* IMPL_DUNE */

#ifdef  INCLUDE_UTF

extern GT_VOID utfPostInitPhase
(
    GT_STATUS st
);

GT_U32* utfTestTaskIdAddrGet
(
    GT_VOID
);

GT_STATUS prvUtfReduceLogSizeFlagSet
(
    GT_BOOL
);

#endif  /* INCLUDE_UTF */

/* when appDemoOnDistributedSimAsicSide == 1
   this image is running on simulation:
   1. our application on distributed "Asic side"
   2. the Other application that run on the "application side" is in
      charge on all Asic configurations
   3. our application responsible to initialize the cpss DB,
      to allow "Galtis wrappers" to be called on this side !
      (because the Other side application may have difficulty running
       "Galtis wrappers" from it's side)
*/
GT_U32  appDemoOnDistributedSimAsicSide = 0;

#ifdef ASIC_SIMULATION
    #include <gtStack/gtOsSocket.h>
    /* Simulation H files */
    #include <common/Version/sstream.h>
    #include <asicSimulation/SInit/sinit.h>
    #include <os/simTypesBind.h>
    #define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
    #include <os/simOsBindOwn.h>
    #undef EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES

    #include <asicSimulation/SDistributed/sdistributed.h>
    #include <asicSimulation/SEmbedded/simFS.h>
    GT_U32 skernelStatusGet
    (
        void
    );

    #define BIND_FUNC(funcName) \
            simOsBindInfo.funcName = funcName

    #define BIND_LEVEL_FUNC(level,funcName)     \
            simOsBindInfo.level.funcName = funcName
#endif /*ASIC_SIMULATION*/


#define BIND_APP_LEVEL_FUNC(infoPtr,level,funcName)     \
        infoPtr->level.funcName = funcName

#define BIND_APP2_LEVEL_FUNC(infoPtr,level,funcInLevel,funcName)     \
        infoPtr->level.funcInLevel = funcName

#define APP_DEMO_CV_INIT_MASK 0xC8

/* HW Access DB step size (this is the step to increment the DB each time,
  when its size is too small) */
#define APP_DEMO_CPSS_HW_ACCESS_DB_STEP_SIZE_CNS   _128K

/* HW Access DB maximal number of steps (Each step is made when db size is too small */

#define APP_DEMO_CPSS_HW_ACCESS_MAX_DB_STEP_COUNT   10


/*******************************************************************************
 * Local usage variables
 ******************************************************************************/
static GT_BOOL  appDemoPrePhase1Init = GT_FALSE;
static GT_BOOL  appDemoInitSystemOnlyPhase1and2 = GT_FALSE;
static GT_BOOL  appDemoInitSystemPhase1and2_done = GT_FALSE;
static GT_BOOL  gReloadEeprom;
static GT_BOOL  gIsB2bSystem;
static GT_BOOL  appDemoCpssInitialized = GT_FALSE;
static GT_BOOL  appDemoTrafficEnableDisableMode = GT_FALSE;
static GT_BOOL  appDemoInitRegDefaults = GT_FALSE;
static GT_BOARD_CONFIG_FUNCS   boardCfgFuncs;  /* Board specific configuration functions.  */

/* do we bypass the initialization of the events */
GT_BOOL appDemoBypassEventInitialize = GT_FALSE;
/* pointer to the function that allow override of setting done by appDemoSysConfigFuncsGet */
/* TBD - not supported yet */
APP_DEMO_SYS_CONFIG_FUNCS_EXTENTION_GET_FUNC appDemoSysConfigFuncsExtentionGetPtr = NULL;


/* information about read and write hw access */
static APP_DEMO_CPSS_HW_ACCESS_INFO_STC  appDemoAccessInfo;

/* pointer to readand write  hw access db */
static APP_DEMO_CPSS_HW_ACCESS_DB_STC    *appDemoAccessDbPtr;
/* flag for tracing read hw access */
static GT_BOOL                            traceReadHwAccess = GT_FALSE;
/* flag for tracing write hw access */
static GT_BOOL                            traceWriteHwAccess = GT_FALSE;
/* flag for tracing write hw access */
static GT_BOOL                            traceDelayHwAccess = GT_FALSE;
/* trace output mode */
APP_DEMO_TRACE_OUTPUT_MODE_ENT appDemoTraceOutputMode = APP_DEMO_TRACE_OUTPUT_MODE_DIRECT_E;

/* first stored parameter of cpssInitSystem */
static GT_U32                             storedBoardIdx;
/* second stored parameter of cpssInitSystem */
static GT_U32                             storedBoardRevId;
/* third stored parameter of cpssInitSystem */
static GT_U32                             storedReloadEeprom;
/*stored value about the devices that pex/smi recognized on the 'board'*/
static GT_U8 storedNumOfPp,storedNumOfFa,storedNumOfXbar;

/*
 * Typedef: enum APP_DEMO_CPSS_LOG_MODE_ENT
 *
 * Description:
 *      This enum defines the different Modes of the log.
 *
 * Enumerations:
 *     APP_DEMO_CPSS_LOG_MODE_PRINT_E          - print the logs on the screen
 *     APP_DEMO_CPSS_LOG_MODE_MEMFS_FILE_E     - write the log into a file on the memFS
 *     APP_DEMO_CPSS_LOG_MODE_LOCAL_FILE_E           - write the log into a file on a local FS
 *
 *
 *  Comments:
 *      None.
 *
 */

typedef enum {
    APP_DEMO_CPSS_LOG_MODE_PRINT_E,
    APP_DEMO_CPSS_LOG_MODE_MEMFS_FILE_E,
    APP_DEMO_CPSS_LOG_MODE_LOCAL_FILE_E
}APP_DEMO_CPSS_LOG_MODE_ENT;


/* store the cpss Log print mode */
APP_DEMO_CPSS_LOG_MODE_ENT prvOsLogMode=APP_DEMO_CPSS_LOG_MODE_PRINT_E;
/** a cpss log thread mode */
APP_DEMO_CPSS_LOG_TASK_MODE_ENT prvOsLogThreadMode = APP_DEMO_CPSS_LOG_TASK_ALL_E;

/* an id of a task (thread) to be logged. Relevant only if prvOsLogThreadMode == APP_DEMO_CPSS_LOG_TASK_SINGLE_E */
static GT_U32 prvOsLogTaskId = 0;
static GT_U32 *prvOsLogTaskIdPtr = &prvOsLogTaskId;

/* a pointer to a memFS file to store the logs in */
static int prvOsLogMemFsFile = 0;
/* a pointer to a local file to store the logs in */
static FILE * prvOsLogLocalFile = NULL;
/* file pointers array per device per core that points */
/* to trace info for given device,core                 */
#ifdef ASIC_SIMULATION
static FILE *devCoreTraceFilePtr[PRV_CPSS_MAX_PP_DEVICES_CNS][APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS] = {{NULL}};
#define  APP_DEMO_FILE_MAX_LINE_LENGTH_CNS   128
#endif

/* macro to force casting between 2 functions prototypes */
#define FORCE_FUNC_CAST (void*)

#ifdef CHX_FAMILY
static GT_CHAR* FIELDS_ARRAY[] =
{
    "AppSpecificCPUCode",
    "ARPPtr",
    "BadIPxBC",
    "Bank",
    "BillingEn",
    "BufReuse",
    "BypassBridge_Dup",
    "BypassBridge",
    "BypassIngressPipe",
    "ByteCount",
    "CCFC",
    "CFI0",
    "CFI1",
    "CFI",
    "CFMPkt",
    "CongestedPortNumber",
    "CongestedQueueNumber",
    "ControlTC",
    "CopyTTL",
    "CPIDMatch",
    "CPU2CPUMirror",
    "CPUCode",
    "CPUMailToNeigborCPU",
    "CPUTrgDev",
    "DecTTL",
    "DIP bits 0..31",
    "DIP bits 32..63",
    "DIP bits 64..95",
    "DIP bits 96..127",
    "DIP",
    "DisableIPCL1_for_RoutedPackets",
    "DoRouteHa",
    "DoRoute",
    "DP",
    "DropCode",
    "DropOnSource",
    "DropPkt",
    "DSCP",
    "EgressFilterEn",
    "EgressFilterRegistered",
    "EgressTrgDev",
    "EgressTrgPort_dup_2_0",
    "EgressTrgPort",
    "EnableBridgeLogFlow",
    "EnableBridge",
    "EnableDsaTagManipulation",
    "EnableRouterLogFlow",
    "EnableSourceLocalFiltering",
    "Enable_IPCL0_lookup",
    "Enable_Policy",
    "EtherType",
    "ExcludedDevIsLocal",
    "ExcludedPort",
    "ExcludedTrunk",
    "ExcludeIsTrunk",
    "EXP",
    "FirstBuffer",
    "FloodMLLPointer",
    "FlowID",
    "FlowSubTemplate",
    "Force_IPCL1_configurations",
    "Fragmented",
    "FromCPUDP",
    "FromCPUTC",
    "HashMaskIndex",
    "IGMPNonQuery",
    "IGMPQuery",
    "InLIF_Mode",
    "InnerIsLLCNonSnap",
    "InnerL3Offset1_dup",
    "InnerL3Offset",
    "InnerPktType",
    "IPCL_Profile_Index",
    "IPLegal",
    "IPMgmntEn",
    "IPModifyDSCP",
    "IPModifyUP",
    "IPM",
    "IPQoSProfile",
    "Ipv4MulticastEn",
    "IPv4OptionField",
    "IPv4RIPv1",
    "Ipv4UnicastEn",
    "IPv4_Bad_GRE_Header",
    "IPv4_DF",
    "IPv6FlowLabel",
    "IPv6HBHExt",
    "IPv6HBHOptions",
    "Ipv6ICMPType",
    "IPv6ICMP",
    "Ipv6MulticastEn",
    "IPv6NonHBHExt",
    "IPv6SiteID",
    "Ipv6UnicastEn",
    "IPv6_EH",
    "IPxHeaderTotalLength",
    "IPxMACMcMismatch",
    "IPxMcLinkLocalProt",
    "IPxProtocol",
    "IP_Fragmented",
    "IP_Header_Info",
    "IsARPReply",
    "IsARP",
    "IsBC",
    "IsIPv4",
    "IsIPv4_ForKey",
    "IsIPv6LinkLocal",
    "IsIPv6MLD",
    "IsIPv6",
    "IsIPv6_ForKey",
    "IsIP",
    "IsIP_ForKey",
    "IsMPLS",
    "IsND",
    "IsPTP",
    "IsSYN",
    "L2Echo",
    "L2Encapsulation",
    "L2_Valid",
    "L3OffsetInvalid",
    "L3TotalOffset_or_InnerHeaderTotalOffset",
    "L4Byte13",
    "L4Bytes3_0",
    "L4OffsetInvalid",
    "L4Valid",
    "LLT",
    "LocalDeviceSrcTrunkID",
    "LocalDevSrcPort",
    "Lookup0_PCL_mode",
    "Lookup0_UseIndexFromDesc",
    "Lookup1_PCL_mode",
    "Lookup1_UseIndexFromDesc",
    "Lookup2_PCL_mode",
    "Lookup2_UseIndexFromDesc",
    "mac2me",
    "MacDaType",
    "MACDA bits 0..31",
    "MACDA bits 32..48",
    "MacSAArpSAMismatch",
    "MACSA bits 0..31",
    "MACSA bits 32..48",
    "MailBoxToNeighborCPU",
    "MarvellTaggedExtended",
    "MarvellTagged",
    "MCRPFFail",
    "MDB",
    "MeteringEn",
    "MLLEn",
    "MLLLast",
    "MLLPtr",
    "MLL",
    "MLL_Pointer",
    "ModifyDP",
    "ModifyDSCP",
    "ModifyDSCP_EXP",
    "ModifyEXP",
    "ModifyTC",
    "ModifyUP",
    "MPLSCmd",
    "MPLSLabel",
    "MPLS_Label0",
    "MPLS_Label1",
    "MPLS_Label2",
    "MPLS_OuterEXP",
    "MPLS_OuterLabel",
    "MPLS_OuterS_Bit",
    "NestedVlanAccessPort",
    "NHEMCRPFFailCmd",
    "Num_of_IngressTags",
    "OrigIsTrunk",
    "OrigQCNPrio",
    "OrigSrcDev",
    "OrigSrcPort",
    "OrigSrcTrunk",
    "OrigVID",
    "OuterIsLLCNonSnap",
    "OuterL3TotalOffset",
    "OuterSrcTag",
    "OutGoingMtagCmd",
    "OutLIF_Type",
    "PacketCmd",
    "PacketIsCNTagged",
    "PacketIsLLCSnap",
    "PacketReject",
    "PacketTOS",
    "PacketType",
    "PacketUP",
    "PCL1_IPv4_Key_Type",
    "PCL1_IPv6_Key_Type",
    "PCL1_Lookup_mode",
    "PCL1_Lookup_Type",
    "PCL1_NonIP_Key_Type",
    "PCL1_PCL_ID",
    "PCLAssignedSSTID",
    "PktCmd",
    "PktHash",
    "PktIsLooped",
    "PktTypeForKey",
    "PolicerEn",
    "PolicerPtr",
    "PolicyBasedRouted",
    "PolicyRTTIndex",
    "PortSpeedIndex_or_OrigQCNPrio",
    "PrioTagged",
    "PseudoWireID",
    "PTPTriggerType",
    "QCNPacket",
    "QCNqDelta",
    "QCNqFb",
    "QCNqOffset",
    "QCNqSaturated",
    "QCN",
    "QinQ",
    "QosProfilePrecedence",
    "QosProfile",
    "RecalcCRC",
    "RedirectCmd",
    "Reserved bits 0..31",
    "Reserved bits 32..63",
    "Reserved bits 64..65",
    "Reserved bits 64..90",
    "Reserved",
    "RGARPIndex",
    "RGBypassTTLandOptionChecks",
    "RGDecTTL",
    "RGEgressInterface",
    "RGFwdCmd",
    "RGICMP_RedirectCheckEn",
    "RGMirrorToIngressAnalyzerPort",
    "RGModifyDSCP",
    "RGModifyUP",
    "RGNHEVID",
    "RGPCERouteEntry",
    "RGQoSProfile",
    "RGQoS_Precedence",
    "RGQoS_ProfileMarkingEn",
    "Routed",
    "RouteToEntry",
    "RsrvdCopy",
    "RxSniff",
    "SIP bits 0..31",
    "SIP bits 32..63",
    "SIP bits 64..95",
    "SIP bits 96..127",
    "SniffTrgDev",
    "SniffTrgPort",
    "SniffVIDX",
    "SolicitationMulticastMessage",
    "SrcDevIsOwn",
    "SrcDev",
    "SrcIsLoopback",
    "SrcPort",
    "SrcTagged_dup",
    "SrcTagged",
    "SrcTCPUDPPortIs0",
    "SrcTrg",
    "Src_is_PE",
    "SST_ID",
    "SYNWithData",
    "TargetIsTrunk",
    "TCPFlags",
    "TCP_UDPDestPort",
    "TCP_UDP_SrcPort",
    "TC",
    "TrgDevice",
    "TrgDevice_VirtualSrcDev",
    "TrgDev",
    "TrgIsTrunk",
    "TrgPort",
    "TrgPort_VirtualSrcPort",
    "TrgTagged_dup",
    "TrgTagged",
    "TrgTrunkID",
    "Truncated",
    "TS_Passenger_Type",
    "TTL",
    "TunnelPtr",
    "TunnelStart",
    "TunnelTerminated",
    "TunnelType",
    "TxSampled",
    "UDB0",
    "UDB10",
    "UDB11",
    "UDB12",
    "UDB13",
    "UDB14",
    "UDB15",
    "UDB16",
    "UDB17",
    "UDB18",
    "UDB19",
    "UDB1",
    "UDB20",
    "UDB21",
    "UDB22",
    "UDB2",
    "UDB3",
    "UDB4",
    "UDB5",
    "UDB6",
    "UDB7",
    "UDB8",
    "UDB9",
    "UDB_BytesValid",
    "UP1",
    "UP",
    "UseInterfaceBasedIndex",
    "UseVIDX",
    "VID1",
    "VIDModified",
    "VIDX",
    "VID",
    "VID_or_InLIF_ID",
    "VID_Precedence",
    "VirtualInterface",
    "VLAN1_Exist",
    "VRF_ID",
    "VR_ID",
    "SrcOwnDev0",
    "CongestedQueueFillLevel"
};
#endif

/* Do common initialization for all families */
static GT_STATUS appDemoInitGlobalModuls
(
    IN  GT_U32  reloadEeprom
);

/* Cpss initialization */
GT_STATUS appDemoCpssInit
(
    GT_VOID
);

/* if not used shared memory or it's unix-like simulation do compile this */
#if (!defined(SHARED_MEMORY) || defined(LINUX_SIM))
static GT_STATUS cpssGetDefaultExtDrvFuncs(
    OUT CPSS_EXT_DRV_FUNC_BIND_STC  *extDrvFuncBindInfoPtr
);


static GT_STATUS cpssGetDefaultOsBindFuncs(
    OUT CPSS_OS_FUNC_BIND_STC *osFuncBindPtr
);


static GT_STATUS cpssGetDefaultTraceFuncs(
    OUT CPSS_TRACE_FUNC_BIND_STC  *traceFuncBindInfoPtr
);
#endif

/* HW phase 1 initialization */
static GT_STATUS appDemoBoardPhase1Init
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
);

/* HW phase 2 initialization */
static GT_STATUS appDemoBoardPhase2Init
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
);

/* Logical phase initialization */
static GT_STATUS appDemoBoardLogicalInit
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
);

/* General phase initialization */
static GT_STATUS appDemoBoardGeneralInit
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
);

/* Updates PP phase 1 params according to app demo database. */
static GT_STATUS appDemoUpdatePpPhase1Params
(
    INOUT CPSS_PP_PHASE1_INIT_PARAMS    *paramsPtr
);

/* Updates PP phase 2 params according to app demo database. */
static GT_STATUS appDemoUpdatePpPhase2Params
(
    INOUT CPSS_PP_PHASE2_INIT_PARAMS    *paramsPtr
);

/* Updates PP logical init params according to app demo database. */
static GT_STATUS appDemoUpdatePpLogicalInitParams
(
    INOUT CPSS_PP_CONFIG_INIT_STC       *paramsPtr
);

/* Updates lib init params according to app demo database. */
static GT_STATUS appDemoUpdateLibInitParams
(
    INOUT APP_DEMO_LIB_INIT_PARAMS      *paramsPtr
);

void appDemoRtosOnSimulationInit
(
    void
);


static GT_STATUS appDemoSetHwAccessDbActiveState
(
    IN      GT_BOOL                             active,
    INOUT   APP_DEMO_CPSS_HW_ACCESS_DB_STC      **dbPtrPtr,
    INOUT   APP_DEMO_CPSS_HW_ACCESS_INFO_STC    *dbInfoPtr
);

static GT_STATUS appDemoTraceHwAccessAction
(
    IN GT_U8                                devNum,
    IN GT_U32                               portGroupId,
    IN GT_BOOL                              isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT         addrSpace,
    IN GT_U32                               addr,
    IN GT_U32                               length,
    IN GT_U32                               *dataPtr,
    IN APP_DEMO_TRACE_OUTPUT_MODE_ENT       outputMode,
    IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT         accessType
);

static GT_STATUS appDemoTraceStoreHwData
(
    IN GT_U8                                devNum,
    IN GT_U32                               portGroupId,
    IN GT_BOOL                              isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT         addrSpace,
    IN GT_U32                               addr,
    IN GT_U32                               length,
    IN GT_U32                               *dataPtr,
    IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT    accessType,
    INOUT APP_DEMO_CPSS_HW_ACCESS_DB_STC    **dbPtrPtr,
    INOUT APP_DEMO_CPSS_HW_ACCESS_INFO_STC  *infoDbPtr
);

#if (!defined(SHARED_MEMORY) || defined(LINUX_SIM))
static GT_STATUS appDemoTraceWriteHwAccess
(
    IN GT_U8       devNum,
    IN GT_U32      portGroupId,
    IN GT_BOOL     isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT         addrSpace,
    IN GT_U32      addr,
    IN GT_U32      length,
    IN GT_U32      *dataPtr
);

static GT_STATUS appDemoTraceReadHwAccess
(
    IN GT_U8       devNum,
    IN GT_U32      portGroupId,
    IN GT_BOOL     isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT         addrSpace,
    IN GT_U32      addr,
    IN GT_U32      length,
    IN GT_U32      *dataPtr
);

static GT_STATUS appDemoTraceDelayHwAccess
(
    IN GT_U8       devNum,
    IN GT_U32      portGroupId,
    IN GT_U32      millisec
);
#endif
static GT_STATUS prvAppDemoTraceHwAccessEnable
(
    IN GT_U8                devNum,
    IN GT_U32               accessType,
    IN GT_BOOL              enable
);

static GT_STATUS prvAppDemoTraceHwAccessOutputModeSet
(
    IN GT_U32   mode
);

/* function to detect memory leakage between calls to it */
static void memoryLeakageDbg(void);
static GT_U32 initCounter = 0;
static GT_U32 resetCounter = 0;

#ifdef SHARED_MEMORY
static pid_t fdbLearningProcessId = 0;
static pid_t RxTxProcessId = 0;
#endif

/*******************************************************************************
* FUNCP_TRACE_PRINTF_DATA
*
* DESCRIPTION:
*       Write a formatted string to the standard output stream.
*
* INPUTS:
*       format  - format string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of characters written, or a negative value if
*       an output error occurs.
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef int (*FUNCP_TRACE_PRINTF_DATA)(const char* format, ...);

static GT_VOID appDemoTracePrintHwData
(
    IN GT_U32                   addr,
    IN GT_U32                   length,
    IN GT_U32                   *dataPtr,
    IN FUNCP_TRACE_PRINTF_DATA   printFuncPtr
);

/* DB to store device type id */
static GT_U32   deviceIdDebug[PRV_CPSS_MAX_PP_DEVICES_CNS];

/* Flag for using device id from deviceIdDebug array */
static GT_BOOL  useDebugDeviceId = GT_FALSE;

/* flag to state that the trace will print also functions that return GT_OK */
/* usually we not care about 'GT_OK' only about fails */
/* the flag can be changed before running the cpssInitSystem(...) --> from terminal vxWorks */
GT_U32 appDemoTraceOn_GT_OK = 0;

/*******************************************************************************
* appDemoTraceOn_GT_OK_Set
*
* DESCRIPTION:
*       This function configures 'print returning GT_OK' functions flag
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
*       - in unix-like systems special function needed to configure appDemoTraceOn_GT_OK
*           specific ifdef's needless, because this function can't harm anything
*       - must be external to ensure it's not ommited by optimization.
*******************************************************************************/
GT_VOID appDemoTraceOn_GT_OK_Set
(
    GT_U32 enable
)
{
    appDemoTraceOn_GT_OK = enable;
    return;
}

/*************************************************************************/
/* Switched tracing mechanism */

/*
 * Typedef: struct __TRACE_ENABLE_SEGMENT_STC
 *
 * Description: the structure for trace statement enabling rule
 *
 * Fields:
 *      nextPtr        - pointer to next rule (NULL is last).
 *      startLineNum   - start line number of line range
 *      endLineNum     - end line number of line range
 *      substingInPath - string to be checked to be substring of the file path
 *
 */
typedef struct __TRACE_ENABLE_SEGMENT_STC
{
    struct __TRACE_ENABLE_SEGMENT_STC *nextPtr;
    GT_U32 startLineNum;
    GT_U32 endLineNum;
    GT_U8  substingInPath[1]; /* the real length ahould be more */
} TRACE_ENABLE_SEGMENT_STC;

/* pointer to the first rule in the chain */
static TRACE_ENABLE_SEGMENT_STC *appDemoTraceEnableFilterPtr = NULL;

/*******************************************************************************
* appDemoTraceEnableFilterAdd
*
* DESCRIPTION:
*       This function adds trace filtering rule.
*       The rule is condition to enable the tracing statement
*       according to path of containing file and line number.
*
* INPUTS:
*       startLineNum    - the low bound of line range.
*       endLineNum      - the high bound of line range.
*       substingInPath  - the string to be matched in source file path.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*******************************************************************************/
GT_STATUS appDemoTraceEnableFilterAdd
(
    IN GT_U32 startLineNum,
    IN GT_U32 endLineNum,
    IN GT_U8  *substingInPath
)
{
    TRACE_ENABLE_SEGMENT_STC* node =
        (TRACE_ENABLE_SEGMENT_STC*)cpssOsMalloc(
            sizeof(TRACE_ENABLE_SEGMENT_STC) + cpssOsStrlen(substingInPath) + 1);
    if (node == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }
    node->nextPtr = appDemoTraceEnableFilterPtr;
    node->startLineNum = startLineNum;
    node->endLineNum = endLineNum;
    cpssOsStrCpy((GT_CHAR*)&(node->substingInPath[0]), (const GT_CHAR*)substingInPath);
    appDemoTraceEnableFilterPtr = node;
    return GT_OK;
}

/*******************************************************************************
* appDemoTraceEnableFilterReset
*
* DESCRIPTION:
*       This function resets all trace filtering rules
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
*******************************************************************************/
GT_VOID appDemoTraceEnableFilterReset()
{
    TRACE_ENABLE_SEGMENT_STC* node = appDemoTraceEnableFilterPtr;
    TRACE_ENABLE_SEGMENT_STC* nextNode;
    while (node != NULL)
    {
        nextNode = node->nextPtr;
        cpssOsFree(node);
        node = nextNode;
    }
    appDemoTraceEnableFilterPtr = NULL;
}

/*******************************************************************************
* appDemoTraceEnableFilterCheck
*
* DESCRIPTION:
*       This function ckecks is trace printing enable for this trace statement
*
* INPUTS:
*       filePath - path to source file returned by __FILE__ macro.
*       lineNum  - number of line returned by __LINE__ macro.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*******************************************************************************/
GT_BOOL appDemoTraceEnableFilterCheck
(
    IN GT_U8  *filePath,
    IN GT_U32 lineNum
)
{
    GT_U32 filePathLen = cpssOsStrlen(filePath);
    GT_U32 substrLen;
    GT_32  comparePos;
    TRACE_ENABLE_SEGMENT_STC* node = appDemoTraceEnableFilterPtr;
    for (; (node != NULL); node = node->nextPtr)
    {
        if ((lineNum < node->startLineNum) || (lineNum > node->endLineNum))
        {
            continue;
        }
        substrLen = cpssOsStrlen(node->substingInPath);
        if (substrLen == 0)
        {
            return GT_TRUE;
        }
        if (substrLen > filePathLen)
        {
            continue;
        }
        comparePos = (GT_32)(filePathLen - substrLen);
        for (; ((comparePos + 1) > 0); comparePos--)
        {
            if (filePath[comparePos] != node->substingInPath[0])
            {
                continue;
            }
            if (0 == cpssOsMemCmp(&(filePath[comparePos]), node->substingInPath, substrLen))
            {
                return GT_TRUE;
            }
        }
    }
    return GT_FALSE;
}

/*******************************************************************************
 * Public API implementation
 ******************************************************************************/

/*******************************************************************************
* appDemoShowCpssSwVersion
*
* DESCRIPTION:
*       This function returns CPSS SW version. Also, it calls
*       appDemoCpssInit() for CPSS initialization.
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
*
*******************************************************************************/
GT_STATUS appDemoShowCpssSwVersion(GT_VOID)
{
    GT_STATUS               rc = GT_OK;

#ifndef DUNE_FE_IMAGE
    CPSS_VERSION_INFO_STC   versionInfo;    /* Holds CPSS versions SW */
#endif /* DUNE_FE_IMAGE */

    #if (!defined(FREEBSD) && !defined(UCLINUX) && !defined(LINUX) && !defined(_WIN32) && !defined(ASIC_SIMULATION))
    /* This is standard VxWorks function which returns the BSP version */
    cpssOsPrintf("\nBSP  Version: %s", sysBspRev());
    #endif
    cpssOsPrintf("\nCPSS Version Stream: %s\n",CPSS_STREAM_NAME_CNS);

    #ifdef ASIC_SIMULATION
        cpssOsPrintf("\nCHIP SIMULTATION Version Stream: %s\n",CHIP_SIMULATION_STREAM_NAME_CNS);
        if (simFSiniFileDirectory[0] != 0)
        {
            cpssOsPrintf("The embedded FS INI file directory:\n%s\n", simFSiniFileDirectory);
        }
    #endif

    #if (defined(CHX_FAMILY) || defined(EXMXPM_FAMILY))
    cpssOsPrintf("%s\n", mvHwsVersionGet());
    #endif

#ifndef DUNE_FE_IMAGE

#ifdef IMPL_FA
    rc = gtFaVersion(&versionInfo);
    if (rc != GT_OK)
    {
        return rc;
    }
#endif  /* IMPL_FA */

#ifdef IMPL_XBAR
{
    GT_VERSION xbarVersion;
    rc = gtXbarVersion(&xbarVersion);
    if (rc != GT_OK)
    {
        return rc;
    }
}
#endif  /* IMPL_XBAR */


#if (defined EX_FAMILY) || (defined MX_FAMILY)
    rc = cpssExMxVersionGet(&versionInfo);
#endif /*(defined EX_FAMILY) || (defined MX_FAMILY) */

#ifdef CHX_FAMILY
    rc = cpssDxChVersionGet(&versionInfo);
    if (rc == GT_OK)
    {
        osPrintf("Cpss DxCh version: %s\n", versionInfo.version);
    }
#endif  /* CHX_FAMILY */

#ifdef SAL_FAMILY
    rc = cpssDxSalVersionGet(&versionInfo);
#endif  /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    rc = cpssExMxPmVersionGet(&versionInfo);
#endif  /* EXMXPM_FAMILY */


#if (defined(LINUX) && !defined(ASIC_SIMULATION))
    FILE *lspfile;
    char  lspbuf[4096] = {0};

    /* read lsp revision from /dev/shm/LSP_VERS.TXT file created from init.sh *
     * by command:  dmesg -s 32768 |grep -i LSP > /dev/shm/LSP_VERS.TXT       */
    if( (lspfile = fopen("/dev/shm/LSP_VERS.TXT", "r")) != NULL)
    {
        /* read revision line from file */
        if(fgets(lspbuf, sizeof(lspbuf), lspfile) != NULL)
        {
            cpssOsPrintf("LSP version:%s", lspbuf);
        }
        fclose(lspfile);
    }
#endif

#endif  /* ifndef DUNE_FE_IMAGE */

    return rc;
}

/*******************************************************************************
* appDemoShowSingleBoard
*
* DESCRIPTION:
*       This function displays single board which have an automatic
*       initialization support.
*
* INPUTS:
*       pBoardInfo - (pointer to) the board info
*       id - the first parameter of gtInitSystem function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       The printed table should look like:
*
*       | 02 - DB-MX610-48F4GS          |                       |
*       |                               | 01 - Rev 1.0          |
*       |                               | 02 - Rev 2.5          |
*       +-------------------------------+-----------------------+
*
*******************************************************************************/
GT_STATUS appDemoShowSingleBoard(
    IN GT_BOARD_LIST_ELEMENT   *pBoardInfo,
    IN GT_U32   id
)
{
    GT_U32  j; /* Loops index.                 */

    osPrintf("| %02d - %-27s",id,pBoardInfo->boardName);
    osPrintf("|%-25c|\n",' ');

    for(j = 0; j < pBoardInfo->numOfRevs; j++)
    {
        if(0 == osStrCmp(GT_BOARD_LIST_NOT_USE_REVISION_STR_CNS , pBoardInfo->boardRevs[j]))
        {
            /* skip the not used revisions */
            continue;
        }

        osPrintf("|%-33c|",' ');
        osPrintf(" %02d - %-19s|\n",j + 1,pBoardInfo->boardRevs[j]);
    }
    osPrintf("+---------------------------------+-------------------------"
             "+\n");

    return GT_OK;
}

/*******************************************************************************
* appDemoShowBoardsList
*
* DESCRIPTION:
*       This function displays the boards list which have an automatic
*       initialization support.
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
*       The printed table should look like:
*       +-------------------------------+-----------------------+
*       | Board name                    | Revisions             |
*       +-------------------------------+-----------------------+
*       | 01 - RD-EX120-24G             |                       |
*       |                               | 01 - Rev 1.0          |
*       |                               | 02 - Rev 2.0          |
*       +-------------------------------+-----------------------+
*       | 02 - DB-MX610-48F4GS          |                       |
*       |                               | 01 - Rev 1.0          |
*       |                               | 02 - Rev 2.5          |
*       +-------------------------------+-----------------------+
*
*******************************************************************************/
GT_STATUS appDemoShowBoardsList(GT_VOID)
{
    GT_BOARD_LIST_ELEMENT   *pBoardInfo;    /* Holds the board information. */
    GT_U32                  i;              /* Loops index.                 */
    GT_STATUS               rc = GT_OK;


    if (systemInitialized != GT_TRUE)
    {
        /* do not show board list after system initialized */

    osPrintf("\nSupported boards:\n");
    osPrintf("+---------------------------------+-------------------------+\n");
    osPrintf("| Board name                      | Revisions               |\n");
    osPrintf("+---------------------------------+-------------------------+\n");

    for(i = 0; i < boardListLen; i++)
    {
        pBoardInfo = &(boardsList[i]);
        if(osStrlen(pBoardInfo->boardName) == 0)
        {
            continue;
        }

        /* print the line */
        appDemoShowSingleBoard(pBoardInfo,i+1);
    }
    osPrintf("\n");

    osPrintf("Call cpssInitSystem(index,boardRevId,reloadEeprom), where:\n");
    osPrintf("\tindex        - The index of the system to be initialized.\n");
    osPrintf("\tboardRevId   - The index of the board revision.\n");
    osPrintf("\treloadEeprom - Whether the device's eeprom should be reloaded \n"
             "\t\t\t after start-init.\n");

    }

    /* show the versions after all other printings ... so it will have better visibility */
    rc = appDemoShowCpssSwVersion();
    if (rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoShowCpssSwVersion", rc);
        return rc;
    }

    osPrintf("\n");

    return GT_OK;
} /* appDemoShowBoardsList */

/*******************************************************************************
* appDemoTrafficEnable
*
* DESCRIPTION:
*       Enables all PP devices and all ports
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
static GT_STATUS appDemoTrafficEnable
(
    GT_VOID
)
{
    GT_STATUS                   rc;             /* return code                 */
    GT_U32                      devIdx;         /* device index                */
    APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;    /* pointer to System Functions */
    GT_U8                       dev;            /* device number               */

    /* Enable all devices and all ports */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0);
          (devIdx < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount)); devIdx++)
    {

        /* get init parameters from appdemo init array */
        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;
        dev = appDemoPpConfigList[devIdx].devNum;

        if(sysCfgFuncs->cpssTrafficEnable)
        {
            /* Enable traffic for given device */
            if(GT_FALSE == appDemoTrafficEnableDisableMode)
            {
                rc = sysCfgFuncs->cpssTrafficEnable(dev);

                CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssTrafficEnable", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* localUtfInit
*
* DESCRIPTION:
*       init the UTF
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
static GT_STATUS localUtfInit(GT_VOID)
{
    GT_STATUS rc = GT_OK;

#ifdef INCLUDE_UTF
    /* Initialize unit tests for CPSS */
    rc = utfPreInitPhase();
    if (rc != GT_OK)
    {
        utfPostInitPhase(rc);
        return rc;
    }

    rc = utfInit(appDemoPpConfigList[SYSTEM_DEV_NUM_MAC(0)].devNum);
    if (rc != GT_OK)
    {
        utfPostInitPhase(rc);
        return rc;
    }
#ifdef IMPL_DUNE
    /* Initialize unit tests for DUNE */
    rc = duneUtfPreInitPhase();
    if (rc == GT_OK)
    {
        rc = duneUtfInit();
        if (rc != GT_OK)
        {
            utfPostInitPhase(rc);
            return rc;
        }
    }
    else
    {
        /* there are no FX devices was found.
           it's legal for using FX image for boards without FX devices. */
        rc = GT_OK;
    }
#endif /* IMPL_DUNE */

    utfPostInitPhase(rc);
#endif /* INCLUDE_UTF */

    return rc;
}

/*******************************************************************************
* localInitSharedMemoryTasks
*
* DESCRIPTION:
* Run fdbLearning abd RxTx processes
* for multi process appDemo
*
*  multiProcessAppDemo set by cpssInitSystem
*  if argv[2] == 2
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
static GT_STATUS localInitSharedMemoryTasks(GT_VOID)
{
#ifdef SHARED_MEMORY

    if (multiProcessAppDemo == 1)
    {

        /*****************************************/
        /*   Run fdbLearning and RxTx processes  */
        /****************************************/

        fdbLearningProcessId = vfork();

        if(fdbLearningProcessId < 0)
        {
                printf("Run fdbLearningProcessId failed\n");
                return GT_FAIL;
        }

        if(fdbLearningProcessId == 0)
        {
#ifndef LINUX_SIM
                execl("/usr/bin/fdbLearning", "fdbLearning",NULL);
#else
                execlp("fdbLearning", "fdbLearning",NULL);
#endif
                _exit (1);
        }


        RxTxProcessId = vfork();

        if(RxTxProcessId < 0)
        {
                printf("Run fdbLearningProcessId failed\n");
                return GT_FAIL;
        }

        if(RxTxProcessId == 0)
        {
#ifndef LINUX_SIM
                execl("/usr/bin/RxTxProcess", "RxTxProcess",NULL);
#else
                execlp("RxTxProcess", "RxTxProcess",NULL);
#endif
                _exit (1);
        }

        /* TODO: sleep to make chance fdbLearning and RxTxProcess to start */

    #ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
        /* Shared Memory implementation should be updated to use native MII APIs. */
        CPSS_TBD_BOOKMARK

        /* set eth port parameters for RxTxProcess */
        ethPortParams.valid = GT_TRUE;
        ethPortParams.appDemoSysConfig = appDemoSysConfig;
        rc = cpssMultiProcComSetSmiTransmitParams(&ethPortParams);
    #endif

    }
#endif /* SHARED_MEMORY */

    return GT_OK;
}

/*******************************************************************************
* cpssInitSystem_afterBasicConfig
*
* DESCRIPTION:
*       This 'after basic config' part of board initialization function, the
*       function using pre-called cpssInitSystem parameters.
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
GT_STATUS cpssInitSystem_afterBasicConfig
(
    GT_VOID
)
{
    GT_STATUS   rc;
    GT_STATUS   stat;
    /* define parameters to keep code as less impact as possible */

    GT_U32  boardRevId = storedBoardRevId;

    GT_U8   numOfPp   = storedNumOfPp;       /* Number of Pp's in system.                */
    GT_U8   numOfFa   = storedNumOfFa;       /* Number of Fa's in system.                */
    GT_U8   numOfXbar = storedNumOfXbar;     /* Number of Xbar's in system.              */
    GT_U32  value     = 0;

    if(appDemoInitSystemPhase1and2_done == GT_FALSE)
    {
        /* must run first part before this one ... we not want to crash on non initialized variables */
        return GT_NOT_INITIALIZED;
    }

    /*****************************************************************/
    /* Logic phase initialization                                    */
    /*****************************************************************/
    rc = appDemoBoardLogicalInit((GT_U8)boardRevId,
                                 &boardCfgFuncs,
                                 numOfPp,
                                 numOfFa,
                                 numOfXbar);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoBoardLogicalInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    stat=appDemoDbEntryGet("initSystemWithoutInterrupts", &value);
    if ((appDemoPrePhase1Init == GT_TRUE) && ((stat == GT_NO_SUCH) || ((stat == GT_OK) && (value == 0))))
    {
        /* for this special mode only events handler should be invoked */
        /* spawn the user event handler processes */
        rc = appDemoEventRequestDrvnModeInit();
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoEventRequestDrvnModeInit", rc);
        return rc;

    }

    /*****************************************************************/
    /* General phase initialization                                  */
    /*****************************************************************/
    rc = appDemoBoardGeneralInit((GT_U8)boardRevId,
                                 &boardCfgFuncs,
                                 numOfPp,
                                 numOfFa,
                                 numOfXbar);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoBoardGeneralInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = auFdbUpdateLockInit();
    if (rc != GT_OK)
        return rc;

    if(appDemoOnDistributedSimAsicSide == 0)
    {
        /*****************************************************************/
        /* Perform board specific settings after logical phase           */
        /*****************************************************************/
        if (boardCfgFuncs.boardAfterInitConfig != NULL)
        {
            rc = boardCfgFuncs.boardAfterInitConfig((GT_U8)boardRevId);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardAfterInitConfig", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        stat=appDemoDbEntryGet("initSystemWithoutInterrupts", &value);
        if ((numOfPp + numOfFa + numOfXbar) != 0       &&
            (appDemoBypassEventInitialize == GT_FALSE) &&
            ( (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_HA_E ) &&
              (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E )) &&
            ((stat == GT_NO_SUCH) || ((stat == GT_OK) && (value == 0))))
        {
            /* spawn the user event handler processes */
            rc = appDemoEventRequestDrvnModeInit();
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoEventRequestDrvnModeInit", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* Enable all PP devices and all ports */
    rc = appDemoTrafficEnable();
    if (rc != GT_OK)
    {
        return rc;
    }

    systemInitialized = GT_TRUE;

    /****************************************
    * Run fdbLearning abd RxTx processes
    * for multi process appDemo
    *
    *  multiProcessAppDemo set by cpssInitSystem
    *  if argv[2] == 2
    ****************************************/
    rc = localInitSharedMemoryTasks();
    CPSS_ENABLER_DBG_TRACE_RC_MAC("localInitSharedMemoryTasks", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /*****************************************************************/
    /* External modules initializations                              */
    /*****************************************************************/

    rc = localUtfInit();
    return rc;
}

/*******************************************************************************
* cpssInitSystem
*
* DESCRIPTION:
*       This is the main board initialization function.
*
* INPUTS:
*       boardIdx      - The index of the board to be initialized from the board
*                       list.
*       boardRevId    - Board revision Id.
*       reloadEeprom  - Whether the Eeprom should be reloaded when
*                       corePpHwStartInit() is called.
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
GT_STATUS cpssInitSystem
(
    IN  GT_U32  boardIdx,
    IN  GT_U32  boardRevId,
    IN  GT_U32  reloadEeprom
)
{
    GT_STATUS               rc = GT_OK;
    GT_BOARD_LIST_ELEMENT   *pBoardInfo;    /* Holds the board information              */
    GT_U8                   numOfPp;        /* Number of Pp's in system.                */
    GT_U8                   numOfFa;        /* Number of Fa's in system.                */
    GT_U8                   numOfXbar;      /* Number of Xbar's in system.              */
    GT_U32                  value;/*tmp value*/
    GT_U32      start_sec  = 0;
    GT_U32      start_nsec = 0;
    GT_U32      end_sec  = 0;
    GT_U32      end_nsec = 0;
    GT_U32      diff_sec;
    GT_U32      diff_nsec;
    /*CPSS_SYSTEM_RECOVERY_INFO_STC   hsuInit = {CPSS_SYSTEM_RECOVERY_INIT_STATE_E,
        {GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE},CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E}; *//* To deny register access during init */
    GT_U32  secondsStart, secondsEnd,
            nanoSecondsStart, nanoSecondsEnd,
            seconds, nanoSec; /* time of init */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddr;

    rc = cpssOsTimeRT(&secondsStart,&nanoSecondsStart);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssOsTimeRT", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    if((appDemoDbEntryGet("initSystemOnlyPhase1and2", &value) == GT_OK) && (value != 0))
    {
        appDemoInitSystemOnlyPhase1and2 = GT_TRUE;
    }

    memoryLeakageDbg();
        /* indicate that we want ability to detect 'Cached DMA' overflow */
        osCacheDmaOverflowDetectionEnableSet(GT_TRUE);

    /* Init CPSS with register defaults and HW aceess */
    if((appDemoDbEntryGet("initRegDefaults", &value) == GT_OK) && (value != 0))
    {
        appDemoInitRegDefaults = GT_TRUE;

        /* when preventing from writing to HW the PP init stage will result in EEPROMDoneIntMemNotDone
           register 0x58 bits 17-18; EPROM initialization is performed and Internal memory initialization is not performed.
           The correct PP init stage should be FullyFunc, Pipe Initialization complete. */

            /* Use HSU to prevent init system from writing to HW during init */
            /*rc = cpssSystemRecoveryStateSet(&hsuInit);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssSystemRecoveryStateSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }*/
    }
    else
    {
        appDemoInitRegDefaults = GT_FALSE;
    }

                /*   set multiprocess or regular appDemo      */
    if ( (boardIdx & APP_DEMO_CV_INIT_MASK) == APP_DEMO_CV_INIT_MASK)
    {
        appDemoPrePhase1Init = GT_TRUE;
        boardIdx = boardIdx - APP_DEMO_CV_INIT_MASK;
    }

#ifdef SHARED_MEMORY

    if(reloadEeprom == 2)
        multiProcessAppDemo = 1;
    else
        multiProcessAppDemo = 0;

#endif


    /****************************************************/
    /* Check parameters                                 */
    /****************************************************/
    if ((boardIdx > boardListLen) || (boardIdx == 0))
    {
        return GT_BAD_PARAM;
    }

    pBoardInfo = &(boardsList[boardIdx - 1]);
    if (boardRevId > pBoardInfo->numOfRevs)
    {
        return GT_FAIL;
    }

    storedBoardIdx     = boardIdx;
    storedBoardRevId   = boardRevId;
    storedReloadEeprom = reloadEeprom;

    /* check if we run RTOS on simulation , to connect remote simulation */
#if (defined ASIC_SIMULATION) &&  (defined RTOS_ON_SIM)
    /*simulation initialization*/
    appDemoRtosOnSimulationInit();

#endif /*(defined ASIC_SIMULATION) &&  (defined RTOS_ON_SIM)*/

#ifdef ASIC_SIMULATION
    while(simulationInitReady == 0)
    {
        /* wait for simulation to be in operational state */
        /* otherwise the init will fail for devices with multi-port groups , like:
        Lion,Lion2,macPhy
        or for systems with multiple devices */
        cpssOsTimerWkAfter(200);
        cpssOsPrintf(".");
    }
    cpssOsPrintf("\n");

    if(sasicgSimulationRole != SASICG_SIMULATION_ROLE_NON_DISTRIBUTED_E)
    {
        switch(sasicgSimulationRole)
        {
            case SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_E:
            case SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_VIA_BROKER_E:
                break;
            default:
                /*bypass the need to call from terminal*/
                appDemoOnDistributedSimAsicSide = 1;
                break;
        }

        cpssOsPrintf(" cpssInitSystem : initialize for Distributed Simulation [%s]\n",consoleDistStr[sasicgSimulationRole]);
    }
#endif /*ASIC_SIMULATION*/

    /* Enable printing inside interrupt routine. */
    extDrvUartInit();

    /* Call to fatal_error initialization, use default fatal error call_back */
    rc = osFatalErrorInit((FATAL_FUNC_PTR)NULL);
    if (rc != GT_OK)
    {
        return rc;
    }

#ifdef VXWORKS
    {
        /* init Debug Services by default */
        GT_BOOL forceNoOsDebugServicesInit = GT_FALSE;

        if(appDemoDbEntryGet("forceNoOsDebugServicesInit", &value) == GT_OK)
        {
            if (value == 0)
            {
                forceNoOsDebugServicesInit = GT_FALSE;
            }
            else
            {
                forceNoOsDebugServicesInit = GT_TRUE;
            }


        }

        if (forceNoOsDebugServicesInit == GT_FALSE)
        {
            /* Initialize the Debug Services */
            rc = osDebugServicesInit();
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

#endif


    /*****************************************************************/
    /* global initialization phase must be done before phase 1 and 2 */
    /*****************************************************************/

    /* Make sure all pointers are NULL in case user won't init all pointers */
    osMemSet(&boardCfgFuncs, 0, sizeof(GT_BOARD_CONFIG_FUNCS));

    /* Make sure all data is cleared */
    osMemSet(&appDemoSysConfig, 0, sizeof(appDemoSysConfig));

    /* ability to set different device numbers for stacking */
    if(appDemoDbEntryGet("firstDevNum", &value) == GT_OK)
    {
        appDemoSysConfig.firstDevNum = (GT_U8)value;
    }
    else
    {
        appDemoSysConfig.firstDevNum = 0;
    }

    /* Check if board registerFunc is not NULL */
    if (NULL == pBoardInfo->registerFunc)
    {
        return GT_NOT_SUPPORTED;
    }

    /* Get board specific functions accordingly to board rev. id */
    rc = pBoardInfo->registerFunc((GT_U8)boardRevId, &boardCfgFuncs);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("pBoardInfo->registerFunc", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
        /* simplified Init */
    if(boardCfgFuncs.boardSimpleInit != NULL)
    {
        rc = boardCfgFuncs.boardSimpleInit(boardRevId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardSimpleInit", rc);
        return rc;
    }

    /* Do global initialization for AppDemo before all phases */
    rc = appDemoInitGlobalModuls(reloadEeprom);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoInitGlobalModuls", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* Get board information */
    rc = boardCfgFuncs.boardGetInfo((GT_U8)boardRevId,
                                    &numOfPp,
                                    &numOfFa,
                                    &numOfXbar,
                                    &gIsB2bSystem);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardGetInfo", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* for fasten the "all PPs loops */
    appDemoPpConfigDevAmount = numOfPp;

    storedNumOfPp =    numOfPp;
    storedNumOfFa =    numOfFa;
    storedNumOfXbar =  numOfXbar;

    /* Does board specific settings before phase 1 */
    if (boardCfgFuncs.boardBeforePhase1Config != NULL)
    {
        rc = boardCfgFuncs.boardBeforePhase1Config((GT_U8)boardRevId);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardBeforePhase1Config", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* take time from the 'phase1 init' stage (not including the 'PCI scan' operations) */
    cpssOsTimeRT(&start_sec, &start_nsec);

    /*****************************************************************/
    /* HW phase 1 initialization                                     */
    /*****************************************************************/
    rc = appDemoBoardPhase1Init((GT_U8)boardRevId,
                                &boardCfgFuncs,
                                numOfPp,
                                numOfFa,
                                numOfXbar);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoBoardPhase1Init", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (appDemoInitRegDefaults == GT_TRUE)
    {

        if(PRV_CPSS_PP_MAC(appDemoPpConfigList[0].devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            regsAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(appDemoPpConfigList[0].devNum);
            /* Set CPU to work in SDMA mode */
            rc = prvCpssHwPpPortGroupSetRegField(appDemoPpConfigList[0].devNum, 0, regsAddr->globalRegs.cpuPortCtrlReg, 1, 1, 1);
            /* Set <sel_port_sdma> in General Configuration register to be 0 */
            rc = prvCpssHwPpSetRegField(appDemoPpConfigList[0].devNum, regsAddr->globalRegs.generalConfigurations, 0, 1, 0);
        }
         /* when preventing from writing to HW the PP init stage will result in EEPROMDoneIntMemNotDone
           register 0x58 bits 17-18; EPROM initialization is performed and Internal memory initialization is not performed.
           The correct PP init stage should be FullyFunc, Pipe Initialization complete. */

            /* Retun HSU to normal state for HW access */
            /*hsuInit.systemRecoveryState = CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E;
            rc = cpssSystemRecoveryStateSet(&hsuInit);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssSystemRecoveryStateSet", rc);*/

        cpssOsPrintf("\nRegister defaults are used\n");
        /* Stop init system */
        return rc;
    }

    if((appDemoOnDistributedSimAsicSide == 0) && (appDemoPrePhase1Init == GT_FALSE))
    {
        /* Does board specific settings after phase 1 */
        if (boardCfgFuncs.boardAfterPhase1Config != NULL)
        {
            rc = boardCfgFuncs.boardAfterPhase1Config((GT_U8)boardRevId);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardAfterPhase1Config", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /*****************************************************************/
    /* HW phase 2 initialization                                     */
    /*****************************************************************/
    rc = appDemoBoardPhase2Init((GT_U8)boardRevId,
                                &boardCfgFuncs,
                                numOfPp,
                                numOfFa,
                                numOfXbar);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoBoardPhase2Init", rc);
    if (rc != GT_OK)
    {
        return rc;
    }
    if((appDemoOnDistributedSimAsicSide == 0) && (appDemoPrePhase1Init == GT_FALSE))
    {
        /* Does board specific settings after phase 2 */
        if (boardCfgFuncs.boardAfterPhase2Config != NULL)
        {
            rc = boardCfgFuncs.boardAfterPhase2Config((GT_U8)boardRevId);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardAfterPhase2Config", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* indicate that phase 1,2 done*/
    appDemoInitSystemPhase1and2_done = GT_TRUE;

    if (appDemoInitSystemOnlyPhase1and2 == GT_FALSE)
    {
        /* continue to 'after basic config' */
        rc = cpssInitSystem_afterBasicConfig();
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssInitSystem_afterBasicConfig", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssOsTimeRT(&secondsEnd,&nanoSecondsEnd);
        if(rc != GT_OK)
        {
            return rc;
        }

        seconds = secondsEnd-secondsStart;
        if(nanoSecondsEnd >= nanoSecondsStart)
        {
            nanoSec = nanoSecondsEnd-nanoSecondsStart;
        }
        else
        {
            nanoSec = (1000000000 - nanoSecondsStart) + nanoSecondsEnd;
            seconds--;
        }
        cpssOsPrintf("cpssInitSystem time: %d sec., %d nanosec.\n", seconds, nanoSec);
    }

    cpssOsTimeRT(&end_sec, &end_nsec);
    if(end_nsec < start_nsec)
    {
        end_nsec += 1000000000;
        end_sec  -= 1;
    }
    diff_sec  = end_sec  - start_sec;
    diff_nsec = end_nsec - start_nsec;

    cpssOsPrintf("Time processing the cpssInitSystem (from 'phase1 init') is [%d] seconds + [%d] nanoseconds \n" , diff_sec , diff_nsec);

    return rc;
} /* cpssInitSystem */
#if 0
/*******************************************************************************
* cpssInitSystem_reducedAfterPhase2InitPortsAndTraffic
*
* DESCRIPTION:
*       function for basic testing of the "appDemoInitSystemOnlyPhase1and2" mode.
*
*       function implemented only for 'DXCH' for bobcat2.
*
*       run :
*        @@appDemoDbEntryAdd "initSystemOnlyPhase1and2",1
*        cpssInitSystem 29,1,0
*        cpssInitSystem_reducedAfterPhase2InitPortsAndTraffic
*
*       --> this will allow test 'flood' in vlan 1 and with 'add mac entry' (by configuration)
*           also to test 'single destination'.
*
*           NOTE: no 'controlled learning' as there are no 'tasks' that handle events ...
*           also event not enabled on the device.
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
*       GT_NOT_INITIALIZED - cpssInitSystem was not previously called.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssInitSystem_reducedAfterPhase2InitPortsAndTraffic(GT_VOID)
{
    GT_STATUS   rc = GT_OK;

    if(appDemoInitSystemPhase1and2_done == GT_FALSE)
    {
        /* must run first part before this one ... we not want to crash on non initialized variables */
        return GT_NOT_INITIALIZED;
    }

#ifdef CHX_FAMILY
    {
        GT_U32  boardRevId = storedBoardRevId;
        GT_U32  devIdx;
        GT_U32  numOfDev = storedNumOfPp;
        GT_U8   dev;
        GT_U16  vlanId;
        GT_PHYSICAL_PORT_NUM portNum;
        /* loop the devices  */
        for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfDev); devIdx++)
        {
            if (appDemoPpConfigList[devIdx].valid != GT_TRUE)
            {
                continue;
            }

            dev = appDemoPpConfigList[devIdx].devNum;

            if(PRV_CPSS_SIP_5_CHECK_MAC(dev))
            {
                /* init the ports */
                rc = appDemoBc2PortInterfaceInit(dev,(GT_U8)boardRevId);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoBc2PortInterfaceInit", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                rc = cpssDxChPhyPortSmiInit(dev);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiInit", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                /* init the phys */
                rc = bobcat2BoardPhyConfig((GT_U8)boardRevId,dev);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("bobcat2BoardPhyConfig", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
                /*
                    the default of HW is all 256 ports in vlan=1 (ports 0..255)

                    but we need to remove port 255 from vlan ...
                    because port 255 is 'force link up' ... for 'cpu_port' purposes
                    (used internally by the cpss implementation.. see use of
                    PRV_CPSS_DXCH_TARGET_LOCAL_LINK_STATUS_CPU_PORT_NUM_CNS)

                   also remove port 63 the 'cpu port' from vlan so it will not get 'flooding'
                */
                vlanId = 1;
                portNum = 255;
                rc = cpssDxChBrgVlanPortDelete(dev,vlanId,portNum);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanPortDelete", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }

                vlanId = 1;
                portNum = CPSS_CPU_PORT_NUM_CNS;/*63*/
                rc = cpssDxChBrgVlanPortDelete(dev,vlanId,portNum);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanPortDelete", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            /*enabe the traffic on the device and on the ports */
            rc = appDemoPpConfigList[devIdx].sysConfigFuncs.cpssTrafficEnable(dev);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssTrafficEnable", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
#endif /*CHX_FAMILY*/

    return rc;
}
#endif

/* function to detect memory leakage between calls to it */
static void memoryLeakageDbg(void)
{
    if(initCounter) /* not relevant for first time as can't compare with previous one */
    {
        if(initCounter >= 2)
        {
            /*
                in the case of initCounter == 1
                we will find huge amount of 'memory leaks' due to pointers that
                are going to be used after second initialization too.
                ('static' allocations , and DB allocations that re-used (not done free))

                the 'memory leakage' detected :
                not free memory that was alloc/realloc total of [9203056] bytes in [1596] elements
            */

            osPrintSync("cpssInitSystem : start print memory leaks \n");
            osMallocMemoryLeakageDbgPrintStandingAllocated();
            osPrintSync("cpssInitSystem : end print memory leaks \n");
        }

        osPrintSync("cpssInitSystem : state to STOP recording allocations and free management to find memory leaks \n");
        /*state to stop recording allocations and free management to find memory leaks */
        osMallocMemoryLeakageDbgAction(OSMEM_MEMORY_LEAKAGE_STATE_DBG_OFF_E);
    }

    if(resetCounter)
    {
        /* next should be done before :
            osMallocMemoryLeakageDbgAction(OSMEM_MEMORY_LEAKAGE_STATE_DBG_ON_E);

           and after
            osMallocMemoryLeakageDbgAction(OSMEM_MEMORY_LEAKAGE_STATE_DBG_OFF_E);

          because it originally happen before cpssInitSystem --> meaning in 'off state'
        */
        osPrintSync("cpssResetSystem : appdemo pre init (to be ready for cpssInitSystem) \n");
        appDemoCpssInit();
    }

    osPrintSync("cpssInitSystem : state to START recording allocations and free management to find memory leaks \n");
    /*state to start recording allocations and free management to find memory leaks */
    osMallocMemoryLeakageDbgAction(OSMEM_MEMORY_LEAKAGE_STATE_DBG_ON_E);

    initCounter++;
}

/*******************************************************************************
* cpssInitSystemGet
*
* DESCRIPTION:
*       Function gets parameters of cpss init system proccess.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       boardIdxPtr     - (pointer to) The index of the board to be initialized
*                         from the board list.
*       boardRevIdPtr   - (pointer to) Board revision Id.
*       reloadEepromPtr - (pointer to) Whether the Eeprom should be reloaded when
*                         corePpHwStartInit() is called.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void cpssInitSystemGet
(
    OUT GT_U32  *boardIdxPtr,
    OUT GT_U32  *boardRevIdPtr,
    OUT GT_U32  *reloadEepromPtr
)
{

    *boardIdxPtr = storedBoardIdx;
    *boardRevIdPtr = storedBoardRevId;
    *reloadEepromPtr = storedReloadEeprom;

}

/*******************************************************************************
* appDemoOsLog
*
* DESCRIPTION:
*       Function for printing the logs of cpss log
*
* INPUTS:
*       lib - the function will print the log of the functions in "lib".
*       type - the function will print the logs from type "type".
*       format - usual printf format string.
*       ... - additional parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID appDemoOsLog
(
    IN    CPSS_LOG_LIB_ENT      lib,
    IN    CPSS_LOG_TYPE_ENT     type,
    IN    const char*           format,
    ...
)
{
    va_list args;
    char buffer[2048];
    GT_U32 tid = 0;

    /*cpssOsPrintf("Lib: %d\n",lib);
    cpssOsPrintf("Log type: %d\n",type);*/
    va_start(args, format);
    cpssOsVsprintf(buffer, format, args);
    va_end(args);

    if (prvOsLogThreadMode == APP_DEMO_CPSS_LOG_TASK_SINGLE_E
        || (prvOsLogThreadMode == APP_DEMO_CPSS_LOG_TASK_REF_E))
    {
        /* we can use prvOsLogTaskIdPtr in both cases because in case of
         * APP_DEMO_CPSS_LOG_TASK_SINGLE_E it will point to prvOsLogTaskId.*/
        if ((osTaskGetSelf(&tid) != GT_OK)
            || (prvOsLogTaskIdPtr == NULL)
            || (*prvOsLogTaskIdPtr != tid))
            return;
    }

    if (prvOsLogMode == APP_DEMO_CPSS_LOG_MODE_PRINT_E) /* print on the screen */
    {
        osPrintf(buffer);
    }

#ifdef CMD_LUA_CLI
    else if (prvOsLogMode == APP_DEMO_CPSS_LOG_MODE_MEMFS_FILE_E && prvOsLogMemFsFile >= 0) /* write to a memFs file */
    {
            if (cmdFS.write(prvOsLogMemFsFile,buffer,osStrlen(buffer)) < 0)
            {
                osPrintf("%s\n", cmdFS.lastError());
            }
    }
#endif  /* CMD_LUA_CLI */

#ifdef ASIC_SIMULATION
    else if (prvOsLogMode == APP_DEMO_CPSS_LOG_MODE_LOCAL_FILE_E && prvOsLogLocalFile >= 0) /* write to a local file */
    {
        if (fprintf(prvOsLogLocalFile, "%s", buffer) < 0)
        {
            osPrintf("log fprintf error\n");
        }
    }
#endif  /* ASIC_SIMULATION */
}

/*******************************************************************************
* appDemoOsLogModeSet
*
* DESCRIPTION:
*       Function for setting the mode of cpss log
*
* INPUTS:
*       mode - mode to be set.
*       name - the name of the file.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - if succsess
*       GT_BAD_PTR    - failed to open the file
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoOsLogModeSet
(
    IN    APP_DEMO_CPSS_LOG_MODE_ENT      mode,
    IN    GT_CHAR_PTR                     name
)
{
    /* close the log file descriptor (if needed) opened by previous mode */

    if (prvOsLogMode == APP_DEMO_CPSS_LOG_MODE_MEMFS_FILE_E && prvOsLogMemFsFile >= 0)
    {
#ifdef CMD_LUA_CLI
        cmdFS.close(prvOsLogMemFsFile);
#endif
        prvOsLogMemFsFile = -1;
    }
    else if (prvOsLogMode == APP_DEMO_CPSS_LOG_MODE_LOCAL_FILE_E && prvOsLogLocalFile != NULL)
    {
#ifdef ASIC_SIMULATION
            fclose(prvOsLogLocalFile);
#endif  /* ASIC_SIMULATION */
            prvOsLogLocalFile = NULL;
    }

    prvOsLogMode = mode;

    /* log into a file */
    if (mode == APP_DEMO_CPSS_LOG_MODE_MEMFS_FILE_E || mode == APP_DEMO_CPSS_LOG_MODE_LOCAL_FILE_E)
    {
        if (NULL == name)       /* log into a file with default name */
        {
#ifdef _WIN32            /* win32 */
            name = "c:\\temp\\dtemp\\cpssLog.txt";
#else                    /* linux */
            name = "testLog.txt";
#endif  /* _WIN32 */
        }

        if (mode == APP_DEMO_CPSS_LOG_MODE_MEMFS_FILE_E)
        {
#ifdef CMD_LUA_CLI
            prvOsLogMemFsFile = cmdFS.open(name,GEN_FS_O_WRONLY|GEN_FS_O_CREAT|GEN_FS_O_TRUNC);
#endif
            if (prvOsLogMemFsFile < 0)
            {
                osPrintf("failed to open memFS file: [%s] \n",name);
                return GT_BAD_PTR;
            }
        }
        else /* APP_DEMO_CPSS_LOG_MODE_LOCAL_FILE_E */
        {
#ifdef ASIC_SIMULATION
            prvOsLogLocalFile = fopen(name, "w");
#endif
            if (prvOsLogLocalFile == NULL)
            {
                osPrintf("failed to open local file: [%s] \n",name);
                return GT_BAD_PTR;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoOsLogStop
*
* DESCRIPTION:
*       Function for stop writing the logs into the file
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID appDemoOsLogStop()
{

    if (prvOsLogMode == APP_DEMO_CPSS_LOG_MODE_MEMFS_FILE_E && prvOsLogMemFsFile >= 0)
    {
#ifdef CMD_LUA_CLI
            cmdFS.close(prvOsLogMemFsFile);
#endif
        prvOsLogMemFsFile = -1;
    }
    else if (prvOsLogMode == APP_DEMO_CPSS_LOG_MODE_LOCAL_FILE_E && prvOsLogLocalFile != NULL)
    {
#ifdef ASIC_SIMULATION
        fclose(prvOsLogLocalFile);
#endif  /* ASIC_SIMUMATION */
        prvOsLogLocalFile = NULL;
    }
}

/*******************************************************************************
 * Private functions implementation
 ******************************************************************************/

/*******************************************************************************
* appDemoInitGlobalModuls
*
* DESCRIPTION: Initialize global settings for CPSS
*
* INPUTS:

*       reloadEeprom    -   reload eeprom flag
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
static GT_STATUS appDemoInitGlobalModuls
(
    IN  GT_U32  reloadEeprom
)
{
    GT_U32          i;            /* Loop index */

    /* save reload eeprom for future use */
    gReloadEeprom = (reloadEeprom == 1 ? GT_TRUE : GT_FALSE);

    /* Initialize the PP array with default parameters */
    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        osMemSet(&appDemoPpConfigList[i], 0, sizeof(appDemoPpConfigList[i]));

        appDemoPpConfigList[i].ppPhase1Done = GT_FALSE;
        appDemoPpConfigList[i].ppPhase2Done = GT_FALSE;
        appDemoPpConfigList[i].ppLogicalInitDone = GT_FALSE;
        appDemoPpConfigList[i].ppGeneralInitDone = GT_FALSE;
        appDemoPpConfigList[i].valid = GT_FALSE;

        /* default value for cpu tx/rx mode */
        appDemoPpConfigList[i].cpuPortMode = CPSS_NET_CPU_PORT_MODE_SDMA_E;
        appDemoPpConfigList[i].allocMethod = CPSS_RX_BUFF_STATIC_ALLOC_E;

        /* cascading information */
        appDemoPpConfigList[i].numberOfCscdTrunks = 0;
        appDemoPpConfigList[i].numberOfCscdPorts = 0;
        appDemoPpConfigList[i].numberOfCscdTargetDevs = 0;
        appDemoPpConfigList[i].numberOf10GPortsToConfigure = 0;
        appDemoPpConfigList[i].internal10GPortConfigFuncPtr = NULL;
        appDemoPpConfigList[i].internalCscdPortConfigFuncPtr = NULL;
    }
    return GT_OK;
} /* appDemoInitGlobalModuls */

/*******************************************************************************
* cpssGetDefaultOsBindFuncs
*
* DESCRIPTION:
*       Receives default cpss bind from OS
* INPUTS:
*       None.
* OUTPUTS:
*       osFuncBindPtr - (pointer to) set of call back functions
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Function should reside into CPSS library to resolve correct
*       pointers to functions.
*
*******************************************************************************/
/* if not used shared memory or it's unix-like simulation do compile this */
#if (!defined(SHARED_MEMORY) || defined(LINUX_SIM))

static GT_STATUS cpssGetDefaultOsBindFuncs(
    OUT CPSS_OS_FUNC_BIND_STC *osFuncBindPtr
)
{

    osMemSet(osFuncBindPtr,0,sizeof(*osFuncBindPtr));

    /* bind the OS functions to the CPSS */
    osFuncBindPtr->osMemBindInfo.osMemBzeroFunc =             osBzero;
    osFuncBindPtr->osMemBindInfo.osMemSetFunc   =             osMemSet;
    osFuncBindPtr->osMemBindInfo.osMemCpyFunc   =             osMemCpy;
    osFuncBindPtr->osMemBindInfo.osMemMoveFunc  =             osMemMove;
    osFuncBindPtr->osMemBindInfo.osMemCmpFunc   =             osMemCmp;
    osFuncBindPtr->osMemBindInfo.osMemStaticMallocFunc =      osStaticMalloc;
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    osFuncBindPtr->osMemBindInfo.osMemMallocFunc =            osDbgMalloc_MemoryLeakageDbg;
    osFuncBindPtr->osMemBindInfo.osMemReallocFunc =           osRealloc_MemoryLeakageDbg;
    osFuncBindPtr->osMemBindInfo.osMemFreeFunc   =            osFree_MemoryLeakageDbg;
#else
    osFuncBindPtr->osMemBindInfo.osMemMallocFunc =            osDbgMalloc;
    osFuncBindPtr->osMemBindInfo.osMemReallocFunc =           osRealloc;
    osFuncBindPtr->osMemBindInfo.osMemFreeFunc   =            osFree;
#endif
    osFuncBindPtr->osMemBindInfo.osMemCacheDmaMallocFunc =    osCacheDmaMalloc;
    osFuncBindPtr->osMemBindInfo.osMemCacheDmaFreeFunc =      osCacheDmaFree;
    osFuncBindPtr->osMemBindInfo.osMemPhyToVirtFunc =         osPhy2Virt;
    osFuncBindPtr->osMemBindInfo.osMemVirtToPhyFunc =         osVirt2Phy;

    osFuncBindPtr->osStrBindInfo.osStrlenFunc       = osStrlen;
    osFuncBindPtr->osStrBindInfo.osStrCpyFunc       = osStrCpy;
    osFuncBindPtr->osStrBindInfo.osStrNCpyFunc      = osStrNCpy;
    osFuncBindPtr->osStrBindInfo.osStrChrFunc       = osStrChr;
    osFuncBindPtr->osStrBindInfo.osStrStrFunc       = osStrStr;
    osFuncBindPtr->osStrBindInfo.osStrRevChrFunc    = osStrrChr;
    osFuncBindPtr->osStrBindInfo.osStrCmpFunc       = osStrCmp;
    osFuncBindPtr->osStrBindInfo.osStrNCmpFunc      = osStrNCmp;
    osFuncBindPtr->osStrBindInfo.osStrCatFunc       = osStrCat;
    osFuncBindPtr->osStrBindInfo.osStrStrNCatFunc   = osStrNCat;
    osFuncBindPtr->osStrBindInfo.osStrChrToUpperFunc= osToUpper;
    osFuncBindPtr->osStrBindInfo.osStrTo32Func      = osStrTo32;
    osFuncBindPtr->osStrBindInfo.osStrToU32Func     = osStrToU32;

    osFuncBindPtr->osSemBindInfo.osMutexCreateFunc     = osMutexCreate;
    osFuncBindPtr->osSemBindInfo.osMutexDeleteFunc     = osMutexDelete;
    osFuncBindPtr->osSemBindInfo.osMutexLockFunc       = osMutexLock;
    osFuncBindPtr->osSemBindInfo.osMutexUnlockFunc     = osMutexUnlock;

    osFuncBindPtr->osSemBindInfo.osSigSemBinCreateFunc = FORCE_FUNC_CAST osSemBinCreate;
#ifdef  LINUX
    osFuncBindPtr->osSemBindInfo.osSigSemMCreateFunc   = osSemMCreate;
    osFuncBindPtr->osSemBindInfo.osSigSemCCreateFunc   = osSemCCreate;
#endif
    osFuncBindPtr->osSemBindInfo.osSigSemDeleteFunc    = osSemDelete;
    osFuncBindPtr->osSemBindInfo.osSigSemWaitFunc      = osSemWait;
    osFuncBindPtr->osSemBindInfo.osSigSemSignalFunc    = osSemSignal;

    osFuncBindPtr->osIoBindInfo.osIoBindStdOutFunc  = osBindStdOut;
    osFuncBindPtr->osIoBindInfo.osIoPrintfFunc      = osPrintf;
#ifdef  LINUX
    osFuncBindPtr->osIoBindInfo.osIoVprintfFunc     = osVprintf;
#endif
    osFuncBindPtr->osIoBindInfo.osIoSprintfFunc     = osSprintf;
    osFuncBindPtr->osIoBindInfo.osIoSnprintfFunc    = osSnprintf;
#if defined(LINUX) || defined(VXWORKS) || defined(WIN32)
    osFuncBindPtr->osIoBindInfo.osIoVsprintfFunc    = osVsprintf;
    osFuncBindPtr->osIoBindInfo.osIoVsnprintfFunc   = osVsnprintf;
#endif
    osFuncBindPtr->osIoBindInfo.osIoPrintSynchFunc  = osPrintSync;
    osFuncBindPtr->osIoBindInfo.osIoGetsFunc        = osGets;

#if defined(LINUX) || defined(FREEBSD) || defined(WIN32)
    osFuncBindPtr->osIoBindInfo.osIoFlastErrorStrFunc = osFlastErrorStr;
    osFuncBindPtr->osIoBindInfo.osIoFopenFunc       = osFopen;
    osFuncBindPtr->osIoBindInfo.osIoFcloseFunc      = osFclose;
    osFuncBindPtr->osIoBindInfo.osIoRewindFunc      = osRewind;
    osFuncBindPtr->osIoBindInfo.osIoFprintfFunc     = osFprintf;
    osFuncBindPtr->osIoBindInfo.osIoFgets           = osFgets;
    osFuncBindPtr->osIoBindInfo.osIoFwriteFunc      = osFwrite;
    osFuncBindPtr->osIoBindInfo.osIoFreadFunc       = osFread;
    osFuncBindPtr->osIoBindInfo.osIoFgetLengthFunc  = osFgetLength;
    osFuncBindPtr->osIoBindInfo.osIoFatalErrorFunc  = (CPSS_OS_FATAL_ERROR_FUNC)osFatalError;
#endif

    osFuncBindPtr->osInetBindInfo.osInetNtohlFunc = osNtohl;
    osFuncBindPtr->osInetBindInfo.osInetHtonlFunc = osHtonl;
    osFuncBindPtr->osInetBindInfo.osInetNtohsFunc = osNtohs;
    osFuncBindPtr->osInetBindInfo.osInetHtonsFunc = osHtons;
    osFuncBindPtr->osInetBindInfo.osInetNtoaFunc  = osInetNtoa;

    osFuncBindPtr->osTimeBindInfo.osTimeWkAfterFunc = osTimerWkAfter;
    osFuncBindPtr->osTimeBindInfo.osTimeTickGetFunc = osTickGet;
    osFuncBindPtr->osTimeBindInfo.osTimeGetFunc     = osTime;
    osFuncBindPtr->osTimeBindInfo.osTimeRTFunc      = osTimeRT;
    osFuncBindPtr->osTimeBindInfo.osStrftimeFunc    = osStrftime;
#if defined(LINUX) || defined(VXWORKS) || defined(WIN32)
    osFuncBindPtr->osTimeBindInfo.osGetSysClockRateFunc = osGetSysClockRate;
#endif

#ifdef  LINUX
    osFuncBindPtr->osTimeBindInfo.osDelayFunc       = osDelay;
#endif

#if !defined(UNIX) || defined(ASIC_SIMULATION)
    osFuncBindPtr->osIntBindInfo.osIntEnableFunc   = osIntEnable;
    osFuncBindPtr->osIntBindInfo.osIntDisableFunc  = osIntDisable;
    osFuncBindPtr->osIntBindInfo.osIntConnectFunc  = osInterruptConnect;
#endif
#if (!defined(FREEBSD) && !defined(UCLINUX)) || defined(ASIC_SIMULATION)
    /* this function required for sand_os_mainOs_interface.c
     * Now it is implemented for:
     *   all os with ASIC simulation
     *   vxWorks
     *   Win32
     *   Linux (stub which does nothing)
     */
    osFuncBindPtr->osIntBindInfo.osIntModeSetFunc  = FORCE_FUNC_CAST osSetIntLockUnlock;
#endif
    osFuncBindPtr->osRandBindInfo.osRandFunc  = osRand;
    osFuncBindPtr->osRandBindInfo.osSrandFunc = osSrand;

    osFuncBindPtr->osTaskBindInfo.osTaskCreateFunc = osTaskCreate;
    osFuncBindPtr->osTaskBindInfo.osTaskDeleteFunc = osTaskDelete;
/*    osFuncBindPtr->osTaskBindInfo.osTaskGetSelfFunc= osTaskGetSelf; */
    osFuncBindPtr->osTaskBindInfo.osTaskLockFunc   = osTaskLock;
    osFuncBindPtr->osTaskBindInfo.osTaskUnLockFunc = osTaskUnLock;

#if defined(LINUX) || defined(VXWORKS) || defined(ASIC_SIMULATION)
    osFuncBindPtr->osStdLibBindInfo.osQsortFunc    = osQsort;
    osFuncBindPtr->osStdLibBindInfo.osBsearchFunc  = osBsearch;
#endif

    osFuncBindPtr->osMsgQBindInfo.osMsgQCreateFunc  = osMsgQCreate;
    osFuncBindPtr->osMsgQBindInfo.osMsgQDeleteFunc  = osMsgQDelete;
    osFuncBindPtr->osMsgQBindInfo.osMsgQSendFunc    = osMsgQSend;
    osFuncBindPtr->osMsgQBindInfo.osMsgQRecvFunc    = osMsgQRecv;
    osFuncBindPtr->osMsgQBindInfo.osMsgQNumMsgsFunc = osMsgQNumMsgs;
    osFuncBindPtr->osMsgQBindInfo.osMsgQNumMsgsFunc = osMsgQNumMsgs;
    osFuncBindPtr->osLogBindInfo.osLogFunc          = appDemoOsLog;

    osFuncBindPtr->osSocketInfo.osSocketLastErrorFunc = osSocketLastError;
    osFuncBindPtr->osSocketInfo.osSocketLastErrorStrFunc = osSocketLastErrorStr;
    osFuncBindPtr->osSocketInfo.osSocketTcpCreateFunc = osSocketTcpCreate;
    osFuncBindPtr->osSocketInfo.osSocketUdpCreateFunc = osSocketUdpCreate;
    osFuncBindPtr->osSocketInfo.osSocketTcpDestroyFunc = osSocketTcpDestroy;
    osFuncBindPtr->osSocketInfo.osSocketUdpDestroyFunc = osSocketUdpDestroy;
    osFuncBindPtr->osSocketInfo.osSocketCreateAddrFunc = osSocketCreateAddr;
    osFuncBindPtr->osSocketInfo.osSocketDestroyAddrFunc = osSocketDestroyAddr;
    osFuncBindPtr->osSocketInfo.osSocketBindFunc = osSocketBind;
    osFuncBindPtr->osSocketInfo.osSocketListenFunc = osSocketListen;
    osFuncBindPtr->osSocketInfo.osSocketAcceptFunc = osSocketAccept;
    osFuncBindPtr->osSocketInfo.osSocketConnectFunc = osSocketConnect;
    osFuncBindPtr->osSocketInfo.osSocketSetNonBlockFunc = osSocketSetNonBlock;
    osFuncBindPtr->osSocketInfo.osSocketSetBlockFunc = osSocketSetBlock;
    osFuncBindPtr->osSocketInfo.osSocketSendFunc = osSocketSend;
    osFuncBindPtr->osSocketInfo.osSocketSendToFunc = osSocketSendTo;
    osFuncBindPtr->osSocketInfo.osSocketRecvFunc = osSocketRecv;
    osFuncBindPtr->osSocketInfo.osSocketRecvFromFunc = osSocketRecvFrom;
    osFuncBindPtr->osSocketInfo.osSocketSetSocketNoLingerFunc = osSocketSetSocketNoLinger;
    osFuncBindPtr->osSocketInfo.osSocketExtractIpAddrFromSocketAddrFunc = osSocketExtractIpAddrFromSocketAddr;
    osFuncBindPtr->osSocketInfo.osSocketGetSocketAddrSizeFunc = osSocketGetSocketAddrSize;
    osFuncBindPtr->osSocketInfo.osSocketShutDownFunc = (CPSS_SOCKET_SHUTDOWN_FUNC)osSocketShutDown;

    osFuncBindPtr->osSocketSelectInfo.osSelectCreateSetFunc = osSelectCreateSet;
    osFuncBindPtr->osSocketSelectInfo.osSelectEraseSetFunc = osSelectEraseSet;
    osFuncBindPtr->osSocketSelectInfo.osSelectZeroSetFunc = osSelectZeroSet;
    osFuncBindPtr->osSocketSelectInfo.osSelectAddFdToSetFunc = osSelectAddFdToSet;
    osFuncBindPtr->osSocketSelectInfo.osSelectClearFdFromSetFunc = osSelectClearFdFromSet;
    osFuncBindPtr->osSocketSelectInfo.osSelectIsFdSetFunc = osSelectIsFdSet;
    osFuncBindPtr->osSocketSelectInfo.osSelectCopySetFunc = osSelectCopySet;
    osFuncBindPtr->osSocketSelectInfo.osSelectFunc = osSelect;
    osFuncBindPtr->osSocketSelectInfo.osSocketGetSocketFdSetSizeFunc = osSocketGetSocketFdSetSize;

    /* Now we should be sure that ALL pointers are set.
     * Occasionally you can add new field into the structure and forget
     * to initialize it!
     *
     * Warning! Here we suppose that compiler doesn't realign this structure!!!
     * Thus we can scan all words inside structure as pointers, there are no
     * alignment bytes there! */

    /* Wrong assumption !!! Compiler CAN realign structure!
    for(i=0; i<sizeof(*osFuncBindPtr); i+=4)
    {
        void **func_ptr=((unsigned char*)osFuncBindPtr) + i;
        if(*func_ptr == NULL)
        {
            osPrintf("Error: pointer into CPSS_OS_FUNC_BIND_STC struct is NULL (offset %d), sizeof(CPSS_OS_FUNC_BIND_STC)=%d\n",
                     i, sizeof(CPSS_OS_FUNC_BIND_STC));
            return GT_FAIL;
        }
    }
    */
    return GT_OK;
}

/*******************************************************************************
* cpssGetDefaultExtDrvFuncs
*
* DESCRIPTION:
*       Receives default cpss bind from extDrv
* INPUTS:
*       None.
* OUTPUTS:
*       extDrvFuncBindInfoPtr - (pointer to) set of call back functions
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Function should reside into CPSS library to resolve correct
*       pointers to functions.
*
*******************************************************************************/
static GT_STATUS cpssGetDefaultExtDrvFuncs(
    OUT CPSS_EXT_DRV_FUNC_BIND_STC  *extDrvFuncBindInfoPtr
)
{
    osMemSet(extDrvFuncBindInfoPtr,0,sizeof(*extDrvFuncBindInfoPtr));

    /* bind the external drivers functions to the CPSS */
    extDrvFuncBindInfoPtr->extDrvMgmtCacheBindInfo.extDrvMgmtCacheFlush      = FORCE_FUNC_CAST extDrvMgmtCacheFlush;
    extDrvFuncBindInfoPtr->extDrvMgmtCacheBindInfo.extDrvMgmtCacheInvalidate = FORCE_FUNC_CAST extDrvMgmtCacheInvalidate;

    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiInitDriverFunc      = hwIfSmiInitDriver;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiWriteRegFunc        = hwIfSmiWriteReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiReadRegFunc         = hwIfSmiReadReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamReadFunc  = hwIfSmiTskRegRamRead;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamWriteFunc = hwIfSmiTskRegRamWrite;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecReadFunc  = hwIfSmiTskRegVecRead;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecWriteFunc = hwIfSmiTskRegVecWrite;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteRegFunc    = hwIfSmiTaskWriteReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskReadRegFunc     = hwIfSmiTaskReadReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntReadRegFunc      = hwIfSmiInterruptReadReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntWriteRegFunc     = hwIfSmiInterruptWriteReg;
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiDevVendorIdGetFunc  = extDrvSmiDevVendorIdGet;
/*  used only in linux -- will need to be under some kind of COMPILATION FLAG
    extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteFieldFunc  = hwIfSmiTaskWriteRegField;
*/

#ifdef GT_I2C
    extDrvFuncBindInfoPtr->extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiInitDriverFunc = hwIfTwsiInitDriver;
    extDrvFuncBindInfoPtr->extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiWriteRegFunc   = hwIfTwsiWriteReg;
    extDrvFuncBindInfoPtr->extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiReadRegFunc    = hwIfTwsiReadReg;
#endif /* GT_I2C */


/*  XBAR related services */
#if defined(IMPL_FA) || defined(IMPL_XBAR)
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvI2cMgmtMasterInitFunc    = gtI2cMgmtMasterInit;
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtReadRegisterFunc     = FORCE_FUNC_CAST extDrvMgmtReadRegister;
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtWriteRegisterFunc    = FORCE_FUNC_CAST extDrvMgmtWriteRegister;
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtIsrReadRegisterFunc  = FORCE_FUNC_CAST extDrvMgmtIsrReadRegister;
    extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtIsrWriteRegisterFunc = FORCE_FUNC_CAST extDrvMgmtIsrWriteRegister;
#endif

    extDrvFuncBindInfoPtr->extDrvDmaBindInfo.extDrvDmaWriteDriverFunc = extDrvDmaWrite;
    extDrvFuncBindInfoPtr->extDrvDmaBindInfo.extDrvDmaReadFunc        = extDrvDmaRead;

    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortRxInitFunc            = extDrvEthPortRxInit;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxInitFunc            = extDrvEthPortTxInit;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortEnableFunc            = extDrvEthPortEnable;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortDisableFunc           = extDrvEthPortDisable;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxFunc                = extDrvEthPortTx;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortInputHookAddFunc      = extDrvEthInputHookAdd;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxCompleteHookAddFunc = extDrvEthTxCompleteHookAdd;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortRxPacketFreeFunc      = extDrvEthRxPacketFree;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxModeSetFunc         = FORCE_FUNC_CAST extDrvEthPortTxModeSet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthRawSocketModeSetFunc  = extDrvEthRawSocketModeSet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthRawSocketModeGetFunc  = extDrvEthRawSocketModeGet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvLinuxModeSetFunc  = extDrvLinuxModeSet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvLinuxModeGetFunc  = extDrvLinuxModeGet;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthRawSocketRxHookAddFunc = extDrvEthRawSocketRxHookAdd;

    extDrvFuncBindInfoPtr->extDrvHsuDrvBindInfo.extDrvHsuMemBaseAddrGetFunc = extDrvHsuMemBaseAddrGet;
    extDrvFuncBindInfoPtr->extDrvHsuDrvBindInfo.extDrvHsuWarmRestartFunc = extDrvHsuWarmRestart;
    extDrvFuncBindInfoPtr->extDrvHsuDrvBindInfo.extDrvHsuInboundSdmaEnableFunc = extDrvHsuInboundSdmaEnable;
    extDrvFuncBindInfoPtr->extDrvHsuDrvBindInfo.extDrvHsuInboundSdmaDisableFunc = extDrvHsuInboundSdmaDisable;
    extDrvFuncBindInfoPtr->extDrvHsuDrvBindInfo.extDrvHsuInboundSdmaStateGetFunc = extDrvHsuInboundSdmaStateGet;

#if defined (XCAT_DRV)
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthCpuCodeToQueueFunc        = extDrvEthCpuCodeToQueue;
    extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPrePendTwoBytesHeaderSetFunc = extDrvEthPrePendTwoBytesHeaderSet;
#endif

    extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntConnectFunc     = extDrvIntConnect;
    extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntEnableFunc      = extDrvIntEnable;
    extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntDisableFunc     = extDrvIntDisable;
    extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntLockModeSetFunc = FORCE_FUNC_CAST extDrvSetIntLockUnlock;

    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciConfigWriteRegFunc        = extDrvPciConfigWriteReg;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciConfigReadRegFunc         = extDrvPciConfigReadReg;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciDevFindFunc               = extDrvPciFindDev;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciIntVecFunc                = FORCE_FUNC_CAST extDrvGetPciIntVec;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciIntMaskFunc               = FORCE_FUNC_CAST extDrvGetIntMask;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciCombinedAccessEnableFunc  = extDrvEnableCombinedPciAccess;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciDoubleWriteFunc           = extDrvPciDoubleWrite;
    extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciDoubleReadFunc            = extDrvPciDoubleRead;

#ifdef DRAGONITE_TYPE_A1
    extDrvFuncBindInfoPtr->extDrvDragoniteInfo.extDrvDragoniteShMemBaseAddrGetFunc = extDrvDragoniteShMemBaseAddrGet;
#endif

    return GT_OK;
}

/*******************************************************************************
* appDemoPrintSynchronizedSet
*
* DESCRIPTION:
*       Set cpssOsPrintf synchronized or queued.
* INPUTS:
*       synchronized - GT_TRUE - synchronized, GT_FALSE - queued.
*       delay        - delay in miliseconds (when synchronized == GT_TRUE)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - any case
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 synchronizedPrintDelay = 0;
#ifdef  LINUX
static char synchronizedPrintBuf[2048];
static int synchronizedPrintWithDelay(const char* format, ...)
{
    int rc;
    va_list args;
    va_start (args, format);
    osVsprintf(synchronizedPrintBuf, format, args);
    rc = osPrintSync(synchronizedPrintBuf);
    osTimerWkAfter(synchronizedPrintDelay);
    return rc;
}
#endif
GT_STATUS appDemoPrintSynchronizedSet(
    IN GT_BOOL   synchronized,
    IN GT_U32    delay
)
{
    synchronizedPrintDelay = delay;
    if (synchronized == GT_FALSE)
    {
        cpssOsPrintf = osPrintf;
    }
    else
    {
#ifdef  LINUX
        cpssOsPrintf = synchronizedPrintWithDelay;
#else
        cpssOsPrintf = osPrintSync;
#endif
    }
    return GT_OK;
}

/*******************************************************************************
* cpssGetDefaultTraceFuncs
*
* DESCRIPTION:
*       Receives default cpss bind from trace
* INPUTS:
*       None.
* OUTPUTS:
*       extDrvFuncBindInfoPtr - (pointer to) set of call back functions
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Function should reside into CPSS library to resolve correct
*       pointers to functions.
*
*******************************************************************************/
static GT_STATUS cpssGetDefaultTraceFuncs(
    OUT CPSS_TRACE_FUNC_BIND_STC  *traceFuncBindInfoPtr
)
{
    osMemSet(traceFuncBindInfoPtr,0,sizeof(*traceFuncBindInfoPtr));

    /* bind the external drivers functions to the CPSS */
    traceFuncBindInfoPtr->traceHwBindInfo.traceHwAccessReadFunc  = appDemoTraceReadHwAccess;
    traceFuncBindInfoPtr->traceHwBindInfo.traceHwAccessWriteFunc = appDemoTraceWriteHwAccess;
    traceFuncBindInfoPtr->traceHwBindInfo.traceHwAccessDelayFunc = appDemoTraceDelayHwAccess;

    return GT_OK;
}

#endif
/*******************************************************************************
* appDemoCpssInit
*
* DESCRIPTION:
*       This function call CPSS to do initial initialization.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*       This function must be called before phase 1 init.
*
*******************************************************************************/
GT_STATUS appDemoCpssInit
(
    GT_VOID
)
{
    GT_STATUS                   rc;
    CPSS_EXT_DRV_FUNC_BIND_STC  extDrvFuncBindInfo;
    CPSS_OS_FUNC_BIND_STC       osFuncBind;
    CPSS_TRACE_FUNC_BIND_STC    traceFuncBindInfo;

    if (appDemoCpssInitialized == GT_TRUE)
    {
        return GT_OK;
    }

#if defined(SHARED_MEMORY) && (!defined(LINUX_SIM))
    /* Shared memory approach on ARM should use dlsym-based workaround for pointers to functions */
    rc = shrMemGetDefaultOsBindFuncsThruDynamicLoader(&osFuncBind);
#else
    rc = cpssGetDefaultOsBindFuncs(&osFuncBind);
#endif

    if(rc != GT_OK) return rc;

#if defined(SHARED_MEMORY) && (!defined(LINUX_SIM))
    /* Shared memory approach on ARM should use dlsym-based workaround for pointers to functions */
    rc = shrMemGetDefaultExtDrvFuncs(&extDrvFuncBindInfo);
#else
    rc = cpssGetDefaultExtDrvFuncs(&extDrvFuncBindInfo);
#endif



#if defined(SHARED_MEMORY) && (!defined(LINUX_SIM))
    /* Shared memory approach on ARM should use dlsym-based workaround for pointers to functions */

    /* Shared memory for Trace should be implemented later */
    CPSS_TBD_BOOKMARK
#else
    rc = cpssGetDefaultTraceFuncs(&traceFuncBindInfo);
#endif



    if(rc != GT_OK) return rc;

    /* 1'st step */
    rc = cpssExtServicesBind(&extDrvFuncBindInfo, &osFuncBind, &traceFuncBindInfo);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* 2'nd step */
    rc = cpssPpInit();
    if(rc != GT_OK)
    {
        return rc;
    }

#if defined(CPSS_LOG_ENABLE)
{
    char                        buffer[80];
    const char                * prvCpssLogErrorLogFileName;

#if defined  WIN32
    prvCpssLogErrorLogFileName = "c:\\temp\\cpss_log_entry";
#else
    prvCpssLogErrorLogFileName = "/tmp/cpss_log_entry";
#endif
    /* Create unique history file name from constant string and current time*/
    cpssOsSprintf(buffer, "%s_%d", prvCpssLogErrorLogFileName, osTime());
    /* Set log history file name */
    rc = cpssLogHistoryFileNameSet(buffer);
    if(rc != GT_OK)
    {
        return rc;
    }
}
#endif

#ifdef  IMPL_GALTIS
#ifdef SHARED_MEMORY
    enhUtUseCaptureToCpuPtr = (GT_U32*)osMalloc(sizeof(GT_U32)); /* allocated in shared area */
#endif
    tgfCmdCpssNetEnhUtUseCaptureToCpuPtr = enhUtUseCaptureToCpuPtr;
#endif /* IMPL_GALTIS */

#ifdef CHX_FAMILY
    appDemoDxChLion3TcamPclConvertedIndexGetPtr = appDemoDxChLion3TcamPclConvertedIndexGet;
#endif
    appDemoDbEntryGet_func = appDemoDbEntryGet;

#ifdef  CMD_LUA_CLI
    /* Initialize lock/unlock mutex */
    cpssApiCallLockUnlockBind(
            cpssGlobalMtxLock,
            cpssGlobalMtxUnlock);
#endif

    appDemoCpssInitialized = GT_TRUE;

    return GT_OK;
} /* appDemoCpssInit */



/*******************************************************************************
* appDemoBoardPhase1Init
*
* DESCRIPTION:
*       Perform phase1 initialization for all devices (Pp, Fa, Xbar).
*
* INPUTS:
*       boardRevId      - Board revision Id.
*       boardCfgFuncs   - Board configuration functions.
*       numOfPp         - Number of Pp's in system.
*       numOfFa         - Number of Fa's in system.
*       numOfXbar       - Number of Xbar's in system.
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
static GT_STATUS appDemoBoardPhase1Init
(
    IN  GT_U8                   boardRevId,       /* Revision of this board             */
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,   /* Config functions for this board    */
    IN  GT_U8                   numOfDev,         /* Number of devices in this board    */
    IN  GT_U8                   numOfFa,          /* Number of FAs in this board        */
    IN  GT_U8                   numOfXbar         /* Number of xbars in this board      */
)
{
    CPSS_PP_PHASE1_INIT_PARAMS      corePpPhase1Params;     /* Phase 1 PP params                            */
    CPSS_REG_VALUE_INFO_STC         *regCfgList;            /* Config functions for this board              */
    GT_U32                          regCfgListSize;         /* Number of config functions for this board    */
    GT_STATUS                       rc;                     /* Func. return val.                            */
    GT_U8                           devIdx;                 /* Loop index.                                  */
    CPSS_PP_DEVICE_TYPE             ppDevType;              /* CPSS Pp device type.                         */

#ifdef IMPL_XBARDRIVER
    GT_XBAR_CORE_SYS_CFG            xbarSysCfg;
    GT_XBAR_DEVICE                  xbarDevType;            /* Xbar device type.*/
    GT_XBAR_PHASE1_INIT_PARAMS      coreXbarPhase1Params;
#endif

#ifdef IMPL_FA
    GT_U8                           deviceNumber;           /* Real device number: FA or PP                 */
    GT_FA_PHASE1_INIT_PARAMS        coreFaPhase1Params;
    GT_FA_DEVICE                    faDevType;              /* Fa device type.  */
#endif
#ifdef IMPL_PP
    GT_U32                          value = 0;/*value from the DB*/
#endif /*IMPL_PP*/

    /* Set xbar configuration parameters.           */
#ifdef IMPL_XBARDRIVER
    if((boardCfgFuncs->boardGetXbarCfgParams != NULL) &&
       (numOfXbar != 0))
    {
        rc = boardCfgFuncs->boardGetXbarCfgParams((GT_U8)boardRevId, &xbarSysCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetXbarCfgParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* CPSS works only in event driven mode */
        xbarSysCfg.eventRequestDriven = GT_TRUE;

        /* Initialize xbar core & driver dbases */
        rc = xbarSysConfig(&xbarSysCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("xbarSysConfig", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
#endif

#ifdef IMPL_PP

    if((appDemoDbEntryGet("DualDeviceIdMode", &value) == GT_OK) && value)
    {
        /* We force the system to work with dual device IDs only */
        /* this option needed for checking the behavior of such system when the
           Lion2 device is not the tested device ! */

        /* at this stage we are after cpssPpInit(...) and before cpssDxChHwPpPhase1Init(...) */
        rc = cpssSystemDualDeviceIdModeEnableSet(GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Set Pp Phase1 configuration parameters.      */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfDev); devIdx++)
    {
        APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;

        /* Get PP config of this device */
        rc = boardCfgFuncs->boardGetPpPh1Params((GT_U8)boardRevId,
                                                devIdx,
                                                &corePpPhase1Params);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpPh1Params", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        value = 0;
        if ((appDemoDbEntryGet("dontOverrideSip5DefaultPciChannelType", &value) != GT_OK)
            || (value == 0))
        {
            /* Override SIP5 devices Default PCI compatible bus to use */
            /* 8 Address Completion Region mode                        */
            if ((appDemoPpConfigList[devIdx].devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E) ||
                (appDemoPpConfigList[devIdx].devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E) ||
                (appDemoPpConfigList[devIdx].devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E))
            {
                corePpPhase1Params.mngInterfaceType = CPSS_CHANNEL_PEX_MBUS_E;
                /* Address Completion Region 1 - for Interrupt Handling    */
                corePpPhase1Params.isrAddrCompletionRegionsBmp = 0x02;
                /* Address Completion Regions 2,3,4,5 - for Application    */
                corePpPhase1Params.appAddrCompletionRegionsBmp = 0x3C;
                /* Address Completion Regions 6,7 - reserved for other CPU */
            }
        }

        /* override PP phase 1 parameters according to app demo database */
        rc = appDemoUpdatePpPhase1Params(&corePpPhase1Params);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdatePpPhase1Params", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Update PP config list device number */
        appDemoPpConfigList[devIdx].devNum   = corePpPhase1Params.devNum;
        appDemoPpConfigList[devIdx].valid    = GT_TRUE;

        /* get family type to understand which API should be used below */
        /* There are two type of API here: EX and DX                    */
        rc = appDemoSysConfigFuncsGet(corePpPhase1Params.deviceId,
                                      &appDemoPpConfigList[devIdx].sysConfigFuncs,
                                      &appDemoPpConfigList[devIdx].apiSupportedBmp);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoSysConfigFuncsGet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }


        if(appDemoSysConfigFuncsExtentionGetPtr)
        {
            rc = appDemoSysConfigFuncsExtentionGetPtr(corePpPhase1Params.deviceId,
                                      &appDemoPpConfigList[devIdx].sysConfigFuncs,
                                      &appDemoPpConfigList[devIdx].apiSupportedBmp);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoSysConfigFuncsExtentionGetPtr", rc);
            if (GT_OK != rc)
            {
                return rc;
            }
        }

        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;

#ifdef ASIC_SIMULATION
        if(sasicgSimulationRole == SASICG_SIMULATION_ROLE_DISTRIBUTED_APPLICATION_SIDE_VIA_BROKER_E)
        {
            /* bind interrupts when work via broker */
            simDistributedRegisterInterrupt(corePpPhase1Params.intVecNum,
                    corePpPhase1Params.devNum,/* use the device ID as the signal ID */
                    sdistAppViaBrokerInterruptMaskMode);
        }
#endif /*ASIC_SIMULATION*/

        /* check if debug device id was set */
        if (useDebugDeviceId == GT_TRUE)
        {
            /* write device id to internal DB */
            rc = prvCpssDrvDebugDeviceIdSet(appDemoPpConfigList[devIdx].devNum,
                                            deviceIdDebug[appDemoPpConfigList[devIdx].devNum]);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssDrvDebugDeviceIdSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if (appDemoPrePhase1Init == GT_TRUE)
        {
            rc = prvCpssPrePhase1PpInit(PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_NO_PP_INIT_E);
            if (rc != GT_OK)
            {
                return rc;
            }

        }
        /* Do HW phase 1 */
        rc = sysCfgFuncs->cpssHwPpPhase1Init(appDemoPpConfigList[devIdx].devNum,
                                             &corePpPhase1Params,
                                             &ppDevType);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssHwPpPhase1Init", rc);
        if (rc != GT_OK)
        {
            return rc;
        }




        /* Update PP config list element of device */
        appDemoPpConfigList[devIdx].ppPhase1Done = GT_TRUE;
        appDemoPpConfigList[devIdx].deviceId = ppDevType;
        appDemoPpConfigList[devIdx].maxPortNumber = PRV_CPSS_PP_MAC(appDemoPpConfigList[devIdx].devNum)->numOfPorts;


        if(PRV_CPSS_PP_MAC(appDemoPpConfigList[devIdx].devNum)->devFamily == CPSS_PP_FAMILY_PUMA3_E &&
           PRV_CPSS_PP_MAC(appDemoPpConfigList[devIdx].devNum)->revision > 0 )
        {
            /*Puma3, starting revision B0*/
            appDemoPpConfigList[devIdx].devSupportSystemReset  = GT_TRUE;
        }
        else
        if((PRV_CPSS_PP_MAC(appDemoPpConfigList[devIdx].devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
            (PRV_CPSS_PP_MAC(appDemoPpConfigList[devIdx].devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E))
        {
            appDemoPpConfigList[devIdx].devSupportSystemReset  = GT_TRUE;
        }
        else
        if(PRV_CPSS_SIP_5_CHECK_MAC(appDemoPpConfigList[devIdx].devNum))
        {
            appDemoPpConfigList[devIdx].devSupportSystemReset  = GT_TRUE;
        }
        else
        if(PRV_CPSS_PP_MAC(appDemoPpConfigList[devIdx].devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E ||
           PRV_CPSS_PP_MAC(appDemoPpConfigList[devIdx].devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E ||
           PRV_CPSS_PP_MAC(appDemoPpConfigList[devIdx].devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E )
        {
            appDemoPpConfigList[devIdx].devSupportSystemReset  = GT_TRUE;
        }
        else
        {
            appDemoPpConfigList[devIdx].devSupportSystemReset  = GT_FALSE;
        }

        #ifdef GM_USED
        /* the GM not supports the 'soft reset' so we can not support system with it */
        appDemoPpConfigList[devIdx].devSupportSystemReset  = GT_FALSE;
        #endif /*GM_USED*/


        /* If app demo inits in reg defaults mode skip the rest of the loop */
        if (appDemoInitRegDefaults == GT_TRUE)
            continue;

        /* Get list of registers to be configured.  */
        if (boardCfgFuncs->boardGetPpRegCfgList == NULL)
        {
            /* if there is no board-specific function, call the common one */
            rc = appDemoGetPpRegCfgList(ppDevType,
                                        gIsB2bSystem,
                                        &regCfgList,
                                        &regCfgListSize);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoGetPpRegCfgList", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            rc = boardCfgFuncs->boardGetPpRegCfgList(boardRevId,
                                                     appDemoPpConfigList[devIdx].devNum,
                                                     &regCfgList,
                                                     &regCfgListSize);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpRegCfgList", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if (appDemoPrePhase1Init == GT_FALSE)
        {
            if(sysCfgFuncs->cpssHwPpStartInit)
            {
                /* Set PP's registers */
                rc = sysCfgFuncs->cpssHwPpStartInit(appDemoPpConfigList[devIdx].devNum,
                                                    gReloadEeprom,
                                                    regCfgList,
                                                    regCfgListSize);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssHwPpStartInit", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }/* for (Loop on devices) */
#endif /* IMPL_PP */

    /* If app demo inits in reg defaults mode return */
    if (appDemoInitRegDefaults == GT_TRUE)
        return GT_OK;


    /* Set Fa Phase1 configuration parameters.      */
#ifdef IMPL_FA
    if(boardCfgFuncs->boardGetFaPh1Params != NULL)
    {
        /* Assume num of PP, num of FA and num of XBAR are the same */
        for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfFa); devIdx++)
        {
            /* Get FA config of this device */
            rc = boardCfgFuncs->boardGetFaPh1Params((GT_U8)boardRevId,
                                                    devIdx,
                                                    &coreFaPhase1Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetFaPh1Params", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* Phase 1 init & registration of FA */

            rc = coreFaHwPhase1Init(&coreFaPhase1Params, &faDevType);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreFaHwPhase1Init", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Set Fa Start-Init configuration parameters.  */
            /* Get list of registers to be configured.  */
            if (boardCfgFuncs->boardGetFaRegCfgList == NULL)
            {
                /* if there is no board-specific function, call the common one */
                rc = appDemoGetFaRegCfgList(faDevType,
#ifdef IMPL_PP
                                            appDemoPpConfigList[devIdx].deviceId,
#else
                                            APP_DEMO_CPSS_NO_PP_CONNECTED_CNS,/*0xFFFFFFFF*/
#endif
                                            &regCfgList,
                                            &regCfgListSize);

                CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoGetFaRegCfgList", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
            else
            {
                rc = boardCfgFuncs->boardGetFaRegCfgList(boardRevId,
                                                         appDemoPpConfigList[devIdx].devNum,
                                                         &regCfgList,
                                                         &regCfgListSize);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetFaRegCfgList", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
            /* if NO PP used device number is FA device number */
            deviceNumber = (numOfDev > 0) ? appDemoPpConfigList[devIdx].devNum : devIdx;


            /* Set FA's registers */
            rc = coreFaHwStartInit(deviceNumber,
                                   GT_FALSE,
                                   (GT_REG_VALUE_INFO*)regCfgList,
                                   regCfgListSize);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreFaHwStartInit", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }/* Loop on FAs */
    }/* FA exists */
#endif /*IMPL_FA*/

#ifdef IMPL_XBARDRIVER
    /* Set xbar core Phase1 configuration parameters.    */
    if(boardCfgFuncs->boardGetXbarPh1Params != NULL)
    {
        /* Assume num of PP, num of FA and num of XBAR are the same */
        for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfXbar); devIdx++)
        {
            rc = boardCfgFuncs->boardGetXbarPh1Params((GT_U8)boardRevId,
                                                      devIdx,
                                                      &coreXbarPhase1Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetXbarPh1Params", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = coreXbarHwPhase1Init(&coreXbarPhase1Params, &xbarDevType);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreXbarHwPhase1Init", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }/* Loop on XBARs */
    } /* Xbar exists */
#endif /*IMPL_XBARDRIVER*/

    return GT_OK;
} /* appDemoBoardPhase1Init */

/*******************************************************************************
* appDemoBoardPhase2Init
*
* DESCRIPTION:
*       Perform phase2 initialization for all devices (Pp, Fa, Xbar).
*
* INPUTS:
*       boardRevId      - Board revision Id.
*       boardCfgFuncs   - Board configuration functions
*       numOfDev        - Number of devices in devList
*       numOfFa         - Number of Fa's in system
*       numOfXbar       - Number of Xbar's in system
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
static GT_STATUS appDemoBoardPhase2Init
(
    IN  GT_U8                   boardRevId,     /* Board revision Id             */
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs, /* Board configuration functions */
    IN  GT_U8                   numOfDev,       /* Number of devices in devList  */
    IN  GT_U8                   numOfFa,        /* Number of Fa's in system      */
    IN  GT_U8                   numOfXbar       /* Number of Xbar's in system    */
)
{
    CPSS_PP_PHASE2_INIT_PARAMS          cpssPpPhase2Params;     /* PP phase 2 params                        */
    GT_STATUS                           rc;                     /* Func. return val.                        */
    GT_U8                               devIdx;                 /* Loop index.                              */
    GT_32                               intKey;                 /* Interrupt key.                           */

#ifdef IMPL_XBARDRIVER
    GT_XBAR_PHASE2_INIT_PARAMS          coreXbarPhase2Params;   /* XBAR phase 2 params */
#endif

#ifdef IMPL_FA
    GT_U8                               deviceNumber;           /* Real device number: FA or PP                 */
    GT_FA_PHASE2_INIT_PARAMS            coreFaPhase2Params;     /* FA phase 2 params */
#endif

#ifdef IMPL_PP
    /* Set Pp Phase2 configuration parameters */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfDev); devIdx++)
    {
        APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;

        /* Get PP phase 2 params */
        rc = boardCfgFuncs->boardGetPpPh2Params(boardRevId,
                                                appDemoPpConfigList[devIdx].devNum,
                                                &cpssPpPhase2Params);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpPh2Params", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* override PP phase 2 parameters according to app demo database */
        rc = appDemoUpdatePpPhase2Params(&cpssPpPhase2Params);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdatePpPhase2Params", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(appDemoOnDistributedSimAsicSide)
        {
            /* no need to init the external driver */
            appDemoSysConfig.cpuEtherInfo.initFunc = NULL;
            /* we must not init the HW */
            cpssPpPhase2Params.netIfCfg.txDescBlock = NULL;
            cpssPpPhase2Params.netIfCfg.rxDescBlock = NULL;
            cpssPpPhase2Params.auqCfg.auDescBlock   = NULL;
        }

        appDemoPpConfigList[devIdx].oldDevNum = appDemoPpConfigList[devIdx].devNum;
        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;

        if(sysCfgFuncs->cpssHwPpPhase2Init)
        {
            /* Lock the interrupts, this phase changes the interrupts nodes pool data */
            extDrvSetIntLockUnlock(INTR_MODE_LOCK, &intKey);

            /* PP HW phase 2 Init */
            rc = sysCfgFuncs->cpssHwPpPhase2Init(appDemoPpConfigList[devIdx].oldDevNum,
                                                 &cpssPpPhase2Params);

            CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssHwPpPhase2Init", rc);
            /* Unlock interrupts after phase 2 init */
            extDrvSetIntLockUnlock(INTR_MODE_UNLOCK, &intKey);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* Current PP init completed */
        appDemoPpConfigList[devIdx].ppPhase2Done = GT_TRUE;
    }
#endif /* IMPL_PP */
    /* Set Fa Phase2 configuration parameters.      */
#ifdef IMPL_FA
    if(boardCfgFuncs->boardGetFaPh2Params != NULL)
    {
        /* Assume num of PP, num of FA and num of XBAR are the same */
        for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfFa); devIdx++)
        {
            /* if NO PP used device number is FA device number */
            deviceNumber = (numOfDev > 0) ? appDemoPpConfigList[devIdx].devNum : devIdx;

            rc = boardCfgFuncs->boardGetFaPh2Params((GT_U8)boardRevId,
                                                    deviceNumber,
                                                    &coreFaPhase2Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetFaPh2Params", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            coreFaPhase2Params.devNum = deviceNumber;

            /* Init FA */
            rc = coreFaHwPhase2Init(&coreFaPhase2Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreFaHwPhase2Init", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
#endif /*IMPL_FA*/

    /* Set xbar Phase2 configuration parameters.    */
#ifdef IMPL_XBARDRIVER
    if(boardCfgFuncs->boardGetXbarPh2Params != NULL)
    {
        /* Assume num of PP, num of FA and num of XBAR are the same */
        for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfXbar); devIdx++)
        {
            /* Get XBAR phase 2 params */
            rc = boardCfgFuncs->boardGetXbarPh2Params((GT_U8)boardRevId,
                                                      appDemoPpConfigList[devIdx].devNum,
                                                      &coreXbarPhase2Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetXbarPh2Params", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            coreXbarPhase2Params.devNum = appDemoPpConfigList[devIdx].devNum;

            /* Init Xbar */
            rc = coreXbarHwPhase2Init(&coreXbarPhase2Params);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("coreXbarHwPhase2Init", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        } /* Loop on XBARs */
    } /* Xbar exists */
#endif /*IMPL_XBARDRIVER*/

    return GT_OK;
} /* appDemoBoardPhase2Init */

/*******************************************************************************
* appDemoBoardLogicalInit
*
* DESCRIPTION:
*       Perform logical phase initialization for all devices (Pp, Fa, Xbar).
*
* INPUTS:
*       boardRevId      - Board revision Id.
*       boardCfgFuncs   - Board configuration functions
*       numOfDev        - Number of devices in devList
*       numOfFa         - Number of Fa's in system
*       numOfXbar       - Number of Xbar's in system
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
static GT_STATUS appDemoBoardLogicalInit
(
    IN  GT_U8                   boardRevId,
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,
    IN  GT_U8                   numOfDev,
    IN  GT_U8                   numOfFa,
    IN  GT_U8                   numOfXbar
)
{
    GT_STATUS                   rc;
    CPSS_PP_CONFIG_INIT_STC     ppLogicalConfigParams;
    GT_U8                       devIdx;


    /* Set Pp Logical configuration parameters */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfDev); devIdx++)
    {
        APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;

        /* update device config list */
        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;


        /* Get PP logical init configuration */
        rc = boardCfgFuncs->boardGetPpLogInitParams(boardRevId,
                                                    appDemoPpConfigList[devIdx].devNum,
                                                    &ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpLogInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* override logical init configuration according to app demo database */
        rc = appDemoUpdatePpLogicalInitParams(&ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdatePpLogicalInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Do CPSS logical init and fill PP_INFO structure */
        rc = sysCfgFuncs->cpssPpLogicalInit(appDemoPpConfigList[devIdx].devNum,
                                            &ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssPpLogicalInit", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        appDemoPpConfigList[devIdx].ppLogicalInitDone = GT_TRUE;
        osMemCpy(&ppUserLogicalConfigParams[appDemoPpConfigList[devIdx].devNum],
                 &ppLogicalConfigParams,
                 sizeof(CPSS_PP_CONFIG_INIT_STC));
    }

    return GT_OK;

} /* appDemoBoardLogicalInit */

/*******************************************************************************
* appDemoBoardGeneralInit
*
* DESCRIPTION:
*       Perform general initialization for all devices (Pp, Fa, Xbar).
*       This function includes initializations that common for all devices.
*
* INPUTS:
*       boardRevId      - Board revision Id.
*       boardCfgFuncs   - Board configuration functions.
*       numOfPp         - Number of Pp's in system.
*       numOfFa         - Number of Fa's in system.
*       numOfXbar       - Number of Xbar's in system.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       This function must be called after logical init.
*
*******************************************************************************/
static GT_STATUS appDemoBoardGeneralInit
(
    IN  GT_U8                   boardRevId,       /* Revision of this board             */
    IN  GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs,   /* Config functions for this board    */
    IN  GT_U8                   numOfDev,         /* Number of devices in this board    */
    IN  GT_U8                   numOfFa,          /* Number of FAs in this board        */
    IN  GT_U8                   numOfXbar         /* Number of xbars in this board      */
)
{
    GT_STATUS                   rc = GT_OK;
    GT_U8                       devIdx;
    APP_DEMO_LIB_INIT_PARAMS    libInitParams;
    CPSS_PP_CONFIG_INIT_STC     ppLogicalConfigParams;


    /* Do general configuration for all devices */
    for (devIdx = SYSTEM_DEV_NUM_MAC(0); devIdx < SYSTEM_DEV_NUM_MAC(numOfDev); devIdx++)
    {
        APP_DEMO_SYS_CONFIG_FUNCS*  sysCfgFuncs;
        GT_U8                       dev;

        /* get init parameters from appdemo init array */
        sysCfgFuncs = &appDemoPpConfigList[devIdx].sysConfigFuncs;
        dev = appDemoPpConfigList[devIdx].devNum;

        /* Get PP logical init configuration */
        rc = boardCfgFuncs->boardGetPpLogInitParams(boardRevId,
                                                    appDemoPpConfigList[devIdx].devNum,
                                                    &ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetPpLogInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* override logical init configuration according to app demo database */
        rc = appDemoUpdatePpLogicalInitParams(&ppLogicalConfigParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdatePpLogicalInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* Get library initialization parameters */
        rc = boardCfgFuncs->boardGetLibInitParams(boardRevId,
                                                  appDemoPpConfigList[devIdx].devNum,
                                                  &libInitParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs->boardGetLibInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* override library initialization parameters according to app demo database */
        rc = appDemoUpdateLibInitParams(&libInitParams);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoUpdateLibInitParams", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Initialize CPSS libraries accordingly to a given parameters */
        if(sysCfgFuncs->cpssLibrariesInit)
        {
            rc = sysCfgFuncs->cpssLibrariesInit(appDemoPpConfigList[devIdx].devNum,
                                                &libInitParams,
                                                &ppLogicalConfigParams);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssLibrariesInit", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if(sysCfgFuncs->cpssPpGeneralInit)
        {
            /* Do CPSS general initialization for given device id */
            rc = sysCfgFuncs->cpssPpGeneralInit(dev);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("sysCfgFuncs->cpssPpGeneralInit", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        appDemoPpConfigList[devIdx].ppGeneralInitDone = GT_TRUE;
    }
    return GT_OK;
} /* appDemoBoardGeneralInit */


/*******************************************************************************
* appDemoUpdatePpPhase1Params
*
* DESCRIPTION:
*       Updates PP phase 1 params according to app demo database.
*
* INPUTS:
*       paramsPtr       - points to params to be updated
*
* OUTPUTS:
*       paramsPtr       - points to params after update
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoUpdatePpPhase1Params
(
    INOUT CPSS_PP_PHASE1_INIT_PARAMS    *paramsPtr
)
{
    GT_U32  value;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    /* follow this sample code to override a parameter according        */
    /* to appDemo database entries                                      */
    /* please keep the convention that database entry name is the       */
    /* same as parameter to update                                      */
    /* if(appDemoDbEntryGet("[databse entry name]", &value) == GT_OK)   */
    /*    paramsPtr->[parameter to update] = value;                     */

    /* ingress buffer SRAM */
    if(appDemoDbEntryGet("ingressBufferMemoryEnableScrambling", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->ingressBufferMemoryEnableScrambling = GT_FALSE;
        }
        else
        {
            paramsPtr->ingressBufferMemoryEnableScrambling = GT_TRUE;
        }
    }

    /* egress buffer SRAM */
    if(appDemoDbEntryGet("egressBufferMemoryEnableScrambling", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->egressBufferMemoryEnableScrambling = GT_FALSE;
        }
        else
        {
            paramsPtr->egressBufferMemoryEnableScrambling = GT_TRUE;
        }
    }
    if(appDemoDbEntryGet("egressBufferRamType", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->egressBufferRamType = APPDEMO_EXMXPM_RAM_TYPE_SRAM_E;
        }
        else
        {
            paramsPtr->egressBufferRamType = APPDEMO_EXMXPM_RAM_TYPE_RLDRAM_E;
        }
    }

    /* routingSramCfgType (->external LPM sram) */
    if(appDemoDbEntryGet("routingSramCfgType", &value) == GT_OK)
    {
        switch(value)
        {
            case 0: paramsPtr->routingSramCfgType = CPSS_TWIST_INTERNAL_E;
                    break;
            case 1: paramsPtr->routingSramCfgType = CPSS_TWIST_EXTERNAL_E;
                    break;
            default: return GT_FAIL;
        }
    }

    /* external CSU */
    if(appDemoDbEntryGet("controlSramUsedArray_0", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->controlSramUsedArray[0] = GT_FALSE;
        }
        else
        {
            paramsPtr->controlSramUsedArray[0] = GT_TRUE;
        }
    }
    if(appDemoDbEntryGet("controlSramUsedArray_1", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->controlSramUsedArray[1] = GT_FALSE;
        }
        else
        {
            paramsPtr->controlSramUsedArray[1] = GT_TRUE;
        }
    }
    if(appDemoDbEntryGet("controlSramProtectArray_0", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->controlSramProtectArray[0] = APPDEMO_EXMXPM_CSU_PROTECT_PARITY_E;
        }
        else
        {
            paramsPtr->controlSramProtectArray[0] = APPDEMO_EXMXPM_CSU_PROTECT_ECC_E;
        }
    }
    if(appDemoDbEntryGet("controlSramProtectArray_1", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->controlSramProtectArray[1] = APPDEMO_EXMXPM_CSU_PROTECT_PARITY_E;
        }
        else
        {
            paramsPtr->controlSramProtectArray[1] = APPDEMO_EXMXPM_CSU_PROTECT_ECC_E;
        }
    }

    /* external DRAM*/
    if(appDemoDbEntryGet("luDunitDramSize", &value) == GT_OK)
        paramsPtr->extDramCfg.luDunitDramSize = (CPSS_DRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("fwdDunitDramSize", &value) == GT_OK)
        paramsPtr->extDramCfg.fwdDunitDramSize = (CPSS_DRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("externalDramInUse", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->extDramCfg.externalDramInUse = GT_FALSE;
        }
        else
        {
            paramsPtr->extDramCfg.externalDramInUse = GT_TRUE;
        }
    }

    if(appDemoDbEntryGet("dramLogicalMode", &value) == GT_OK)
    {
        switch (value)
        {
        case 0:
            paramsPtr->extDramCfg.dramLogicalMode = APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E;
            break;
        case 1:
            paramsPtr->extDramCfg.dramLogicalMode = APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_4BANKS_E;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    /* external TCAM */
    if(appDemoDbEntryGet("externalTcamUsed", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->externalTcamUsed = GT_FALSE;
        }
        else
        {
            paramsPtr->externalTcamUsed = GT_TRUE;
        }
    }

    if(appDemoDbEntryGet("intVecNum", &value) == GT_OK)
    {
        paramsPtr->intVecNum = value;
    }

    /* allow this case also from 'specific board config file' (not only from appDemoDbEntryGet(...))*/
    if(paramsPtr->intVecNum == CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS/*0xFFFFFFFF*/)
    {
        appDemoBypassEventInitialize = GT_TRUE;
    }

    /* initSerdesDefaults */
    if(appDemoDbEntryGet("initSerdesDefaults", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->initSerdesDefaults = GT_FALSE;
        }
        else
        {
            paramsPtr->initSerdesDefaults = GT_TRUE;
        }
    }
    /* serdes reference clock */
    if(appDemoDbEntryGet("serdesRefClock", &value) == GT_OK)
    {
        switch(value)
        {
        case 1:
            paramsPtr->serdesRefClock = APPDEMO_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E;
            break;
        case 2:
            paramsPtr->serdesRefClock = APPDEMO_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E;
            break;
        case 5: paramsPtr->serdesRefClock = APPDEMO_SERDES_REF_CLOCK_INTERNAL_125_E;
            break;
        default:
            return GT_FAIL;
        }
    }

    /* TCAM Parity Daemon */
    if(appDemoDbEntryGet("tcamParityCalcEnable", &value) == GT_OK)
    {
        if(value == 0)
        {
            paramsPtr->tcamParityCalcEnable = GT_FALSE;
        }
        else
        {
            paramsPtr->tcamParityCalcEnable = GT_TRUE;
        }
    }

    /*PCI/PEX/PEX_MBUS alteration options*/
    switch (paramsPtr->mngInterfaceType)
    {
        case CPSS_CHANNEL_PCI_E:
        case CPSS_CHANNEL_PEX_E:
        case CPSS_CHANNEL_PEX_MBUS_E:
            if (appDemoDbEntryGet("mngInterfaceType", &value) == GT_OK)
            {
                switch (value)
                {
                    case CPSS_CHANNEL_PCI_E:
                    case CPSS_CHANNEL_PEX_E:
                    case CPSS_CHANNEL_PEX_MBUS_E:
                        paramsPtr->mngInterfaceType = value;
                        break;
                    default: break;
                }
            }
            break;
        default: break;
    }

    if(appDemoDbEntryGet("isrAddrCompletionRegionsBmp", &value) == GT_OK)
    {
        paramsPtr->isrAddrCompletionRegionsBmp = value;
    }

    if(appDemoDbEntryGet("appAddrCompletionRegionsBmp", &value) == GT_OK)
    {
        paramsPtr->appAddrCompletionRegionsBmp = value;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoUpdatePpPhase2Params
*
* DESCRIPTION:
*       Updates PP phase 2 params according to app demo database.
*
* INPUTS:
*       paramsPtr       - points to params to be updated
*
* OUTPUTS:
*       paramsPtr       - points to params after update
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoUpdatePpPhase2Params
(
    INOUT CPSS_PP_PHASE2_INIT_PARAMS    *paramsPtr
)
{
    GT_U32  value;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    /* follow this sample code to override a parameter according        */
    /* to appDemo database entries                                      */
    /* please keep the convention that database entry name is the       */
    /* same as parameter to update                                      */
    /* if(appDemoDbEntryGet("[databse entry name]", &value) == GT_OK)   */
    /*    paramsPtr->[parameter to update] = value;                     */

    if(appDemoDbEntryGet("noCpu", &value) == GT_OK)
    {
        paramsPtr->noTraffic2CPU = (GT_BOOL)value;
    }
    if(appDemoDbEntryGet("netifSdmaPortGroupId", &value) == GT_OK)
    {
        paramsPtr->netifSdmaPortGroupId = value;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoUpdatePpLogicalInitParams
*
* DESCRIPTION:
*       Updates PP logical init params according to app demo database.
*
* INPUTS:
*       paramsPtr       - points to params to be updated
*
* OUTPUTS:
*       paramsPtr       - points to params after update
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - on wrong value
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoUpdatePpLogicalInitParams
(
    INOUT CPSS_PP_CONFIG_INIT_STC       *paramsPtr
)
{
    GT_U32  value;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    /* follow this sample code to override a parameter according        */
    /* to appDemo database entries                                      */
    /* please keep the convention that database entry name is the       */
    /* same as parameter to update                                      */
    /* if(appDemoDbEntryGet("[databse entry name]", &value) == GT_OK)   */
    /*    paramsPtr->[parameter to update] = value;                     */

    /* Inlif */
    if(appDemoDbEntryGet("maxNumOfLifs", &value) == GT_OK)
        paramsPtr->maxNumOfLifs = value;
    if(appDemoDbEntryGet("inlifPortMode", &value) == GT_OK)
        paramsPtr->inlifPortMode = (APP_DEMO_CPSS_INLIF_PORT_MODE_ENT)value;

    /* Policer */
    if(appDemoDbEntryGet("maxNumOfPolicerEntries", &value) == GT_OK)
        paramsPtr->maxNumOfPolicerEntries = value;
    if(appDemoDbEntryGet("policerConformCountEn", &value) == GT_OK)
        paramsPtr->policerConformCountEn = (GT_BOOL)value;

    /* MPLS    */
    if(appDemoDbEntryGet("maxNumOfNhlfe", &value) == GT_OK)
        paramsPtr->maxNumOfNhlfe = value;
    if(appDemoDbEntryGet("maxNumOfMplsIfs", &value) == GT_OK)
        paramsPtr->maxNumOfMplsIfs = value;
    if(appDemoDbEntryGet("ipMplsMemLocation", &value) == GT_OK)
    {
        switch(value)
        {
        case 0:
            paramsPtr->ipMplsMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_INTERNAL_E;
            break;
        case 1:
            paramsPtr->ipMplsMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    /* PCL     */
    if(appDemoDbEntryGet("maxNumOfPclAction", &value) == GT_OK)
        paramsPtr->maxNumOfPclAction = value;
    if(appDemoDbEntryGet("pclActionSize", &value) == GT_OK)
        paramsPtr->pclActionSize = value;

    /* Policy-based routing  */
    if(appDemoDbEntryGet("maxNumOfPceForIpPrefixes", &value) == GT_OK)
        paramsPtr->maxNumOfPceForIpPrefixes = value;
    if(appDemoDbEntryGet("usePolicyBasedRouting", &value) == GT_OK)
        paramsPtr->usePolicyBasedRouting = value;
    if(appDemoDbEntryGet("usePolicyBasedDefaultMc", &value) == GT_OK)
        paramsPtr->usePolicyBasedDefaultMc = value;

    /* LPM PBR entries */
    if(appDemoDbEntryGet("maxNumOfPbrEntries", &value) == GT_OK)
        paramsPtr->maxNumOfPbrEntries = value;

    /* Bridge           */
    if(appDemoDbEntryGet("maxVid", &value) == GT_OK)
        paramsPtr->maxVid = (GT_U16)value;
    if(appDemoDbEntryGet("maxMcGroups", &value) == GT_OK)
        paramsPtr->maxMcGroups = (GT_U16)value;

    /* IP Unicast     */
    if(appDemoDbEntryGet("maxNumOfVirtualRouters", &value) == GT_OK)
        paramsPtr->maxNumOfVirtualRouters = value;
    if(appDemoDbEntryGet("maxNumOfIpNextHop", &value) == GT_OK)
        paramsPtr->maxNumOfIpNextHop = value;

    /* IPv4 Unicast     */
    if(appDemoDbEntryGet("maxNumOfIpv4Prefixes", &value) == GT_OK)
        paramsPtr->maxNumOfIpv4Prefixes = value;

    /* IPv4/IPv6 Multicast   */
    if(appDemoDbEntryGet("maxNumOfIpv4McEntries", &value) == GT_OK)
        paramsPtr->maxNumOfIpv4McEntries = value;
    if(appDemoDbEntryGet("maxNumOfMll", &value) == GT_OK)
        paramsPtr->maxNumOfMll = value;

    /* IPv6 Unicast     */
    if(appDemoDbEntryGet("maxNumOfIpv6Prefixes", &value) == GT_OK)
        paramsPtr->maxNumOfIpv6Prefixes = value;

    /* IPv6 Multicast   */
    if(appDemoDbEntryGet("maxNumOfIpv6McGroups", &value) == GT_OK)
        paramsPtr->maxNumOfIpv6McGroups = value;

    /* Tunnels */
    if(appDemoDbEntryGet("maxNumOfTunnelEntries", &value) == GT_OK)
        paramsPtr->maxNumOfTunnelEntries = value;
    if(appDemoDbEntryGet("maxNumOfIpv4TunnelTerms", &value) == GT_OK)
        paramsPtr->maxNumOfIpv4TunnelTerms = value;

    /* tunnel termination (of all types) / TTI entries - for Dx devices */
    if(appDemoDbEntryGet("maxNumOfTunnelTerm", &value) == GT_OK)
        paramsPtr->maxNumOfTunnelTerm = value;

    /* inlifs */
    if(appDemoDbEntryGet("vlanInlifEntryType", &value) == GT_OK)
        paramsPtr->vlanInlifEntryType = value;

    /* if GT_FALSE - no division of memory pools for the use of different IP versions */
    if(appDemoDbEntryGet("ipMemDivisionOn", &value) == GT_OK)
        paramsPtr->ipMemDivisionOn = (GT_BOOL)value;

    /* devision of memory between IPv4 and IPv6 prefixes*/
    if(appDemoDbEntryGet("ipv6MemShare", &value) == GT_OK)
        paramsPtr->ipv6MemShare = value;

    /* Number of trunks */
    if(appDemoDbEntryGet("numOfTrunks", &value) == GT_OK)
        paramsPtr->numOfTrunks = value;

    /* PCL rule index and PCL ID for defualt IPv6 MC entry for devices */
    /* where IPv6 MC group search is implemented in PCL                */
    if(appDemoDbEntryGet("defIpv6McPclRuleIndex", &value) == GT_OK)
        paramsPtr->defIpv6McPclRuleIndex = value;
    if(appDemoDbEntryGet("vrIpv6McPclId", &value) == GT_OK)
        paramsPtr->vrIpv6McPclId = value;

    /* TCAM LPM DB configurations */
    if(appDemoDbEntryGet("lpmDbPartitionEnable", &value) == GT_OK)
        paramsPtr->lpmDbPartitionEnable = (GT_BOOL)value;
    if(appDemoDbEntryGet("lpmDbFirstTcamLine", &value) == GT_OK)
        paramsPtr->lpmDbFirstTcamLine = value;
    if(appDemoDbEntryGet("lpmDbLastTcamLine", &value) == GT_OK)
        paramsPtr->lpmDbLastTcamLine = value;
    if(appDemoDbEntryGet("lpmDbSupportIpv4", &value) == GT_OK)
        paramsPtr->lpmDbSupportIpv4 = (GT_BOOL)value;
    if(appDemoDbEntryGet("lpmDbSupportIpv6", &value) == GT_OK)
        paramsPtr->lpmDbSupportIpv6 = (GT_BOOL)value;

    /* number of memory configurations supported by the lpm db */
    if(appDemoDbEntryGet("lpmDbNumOfMemCfg", &value) == GT_OK)
        paramsPtr->lpmDbNumOfMemCfg = value;

    /* LPM RAM Configuration for Dynamic LPM Management */
    if(appDemoDbEntryGet("lpmRamMemoryBlocksCfg.lpmRamBlocksAllocationMethod", &value) == GT_OK)
        paramsPtr->lpmRamMemoryBlocksCfg.blocksAllocationMethod = (APP_DEMO_CPSS_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT)value;

    /* array of memory configurations supported by the lpm db */
    if(appDemoDbEntryGet("lpmDbMemCfgArray->routingSramCfgType", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->routingSramCfgType = (APP_DEMO_CPSS_LPM_MEM_CFG_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[0]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[0] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[1]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[1] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[2]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[2] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[2]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[2] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[3]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[3] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[4]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[4] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[5]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[5] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[6]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[6] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[7]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[7] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[8]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[8] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->sramsSizeArray[9]", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->sramsSizeArray[9] = (CPSS_SRAM_SIZE_ENT)value;
    if(appDemoDbEntryGet("lpmDbMemCfgArray->numOfSramsSizes", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->numOfSramsSizes = value;

    if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem0", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->lpmInternalSramSizeArray[0] = value;
    if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem1", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->lpmInternalSramSizeArray[1] = value;
    if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem2", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->lpmInternalSramSizeArray[2] = value;
    if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem3", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->lpmInternalSramSizeArray[3] = value;

    if(appDemoDbEntryGet("lpmDbMemCfgArray->extMemBankMode", &value) == GT_OK)
    {
        switch (value)
        {
        case 0:
            paramsPtr->lpmDbMemCfgArray->extMemBankMode = APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E;
            break;
        case 1:
            paramsPtr->lpmDbMemCfgArray->extMemBankMode = APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_4BANKS_E;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }
    if(appDemoDbEntryGet("lpmDbMemCfgArray->extMemSize", &value) == GT_OK)
        paramsPtr->lpmDbMemCfgArray->extMemSize = value;

    if(appDemoDbEntryGet("extLpmMemSize", &value) == GT_OK)
        paramsPtr->extLpmMemSize = value;

    if(appDemoDbEntryGet("lpmStagesMemAssociation", &value) == GT_OK)
        paramsPtr->lpmStagesMemAssociation = (APP_DEMO_CPSS_LPM_STAGES_MEM_ASSOCIATION_ENT)value;

    /* DIT configuration */
    if(appDemoDbEntryGet("ditMemLocation", &value) == GT_OK)
    {
        switch(value)
        {
        case 0:
            paramsPtr->ditMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_INTERNAL_E;
            break;
        case 1:
            paramsPtr->ditMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    if(appDemoDbEntryGet("ditMemoryMode", &value) == GT_OK)
        paramsPtr->ditMemoryMode = (APP_DEMO_CPSS_DIT_MEMORY_MODE_ENT)value;
    if(appDemoDbEntryGet("ditIpMcMode", &value) == GT_OK)
        paramsPtr->ditIpMcMode = (APP_DEMO_CPSS_DIT_IP_MC_MODE_ENT)value;
    if(appDemoDbEntryGet("maxNumOfDitUcIpEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitUcIpEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitUcMplsEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitUcMplsEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitUcOtherEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitUcOtherEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitMcIpEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitMcIpEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitMcMplsEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitMcMplsEntries = value;
    if(appDemoDbEntryGet("maxNumOfDitMcOtherEntries", &value) == GT_OK)
        paramsPtr->maxNumOfDitMcOtherEntries = value;

    /* fdb table mode */
    if(appDemoDbEntryGet("fdbTableInfo.fdbTableMode", &value) == GT_OK)
        paramsPtr->fdbTableInfo.fdbTableMode = value;
    if(appDemoDbEntryGet("fdbTableInfo.fdbToLutRatio", &value) == GT_OK)
        paramsPtr->fdbTableInfo.fdbToLutRatio = value;
    if(appDemoDbEntryGet("fdbTableInfo.fdbHashSize", &value) == GT_OK)
        paramsPtr->fdbTableInfo.fdbHashSize = value;
    if(appDemoDbEntryGet("fdbTableInfo.extFdbHashMode", &value) == GT_OK)
        paramsPtr->fdbTableInfo.extFdbHashMode = value;

    /* note that external TCAM info parameters can't be modified */

    return GT_OK;
}

/*******************************************************************************
* appDemoUpdateLibInitParams
*
* DESCRIPTION:
*       Updates lib init params according to app demo database.
*
* INPUTS:
*       paramsPtr       - points to params to be updated
*
* OUTPUTS:
*       paramsPtr       - points to params after update
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoUpdateLibInitParams
(
    INOUT APP_DEMO_LIB_INIT_PARAMS      *paramsPtr
)
{
    GT_U32  value;

    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    /* follow this sample code to override a parameter according        */
    /* to appDemo database entries                                      */
    /* please keep the convention that database entry name is the       */
    /* same as parameter to update                                      */
    /* if(appDemoDbEntryGet("[databse entry name]", &value) == GT_OK)   */
    /*    paramsPtr->[parameter to update] = value;                     */

    if(appDemoDbEntryGet("initBridge", &value) == GT_OK)
        paramsPtr->initBridge = (GT_BOOL)value;
    if(appDemoDbEntryGet("initClassifier", &value) == GT_OK)
        paramsPtr->initClassifier = (GT_BOOL)value;
    if(appDemoDbEntryGet("initCos", &value) == GT_OK)
        paramsPtr->initCos = (GT_BOOL)value;
    if(appDemoDbEntryGet("initIpv4", &value) == GT_OK)
        paramsPtr->initIpv4 = (GT_BOOL)value;
    if(appDemoDbEntryGet("initIpv6", &value) == GT_OK)
        paramsPtr->initIpv6 = (GT_BOOL)value;
    if(appDemoDbEntryGet("initIpv4Filter", &value) == GT_OK)
        paramsPtr->initIpv4Filter = (GT_BOOL)value;
    if(appDemoDbEntryGet("initIpv4Tunnel", &value) == GT_OK)
        paramsPtr->initIpv4Tunnel = (GT_BOOL)value;
    if(appDemoDbEntryGet("initLif", &value) == GT_OK)
        paramsPtr->initLif = (GT_BOOL)value;
    if(appDemoDbEntryGet("initMirror", &value) == GT_OK)
        paramsPtr->initMirror = (GT_BOOL)value;
    if(appDemoDbEntryGet("initMpls", &value) == GT_OK)
        paramsPtr->initMpls = (GT_BOOL)value;
    if(appDemoDbEntryGet("initMplsTunnel", &value) == GT_OK)
        paramsPtr->initMplsTunnel = (GT_BOOL)value;
    if(appDemoDbEntryGet("initNetworkIf", &value) == GT_OK)
        paramsPtr->initNetworkIf = (GT_BOOL)value;
    if(appDemoDbEntryGet("initI2c", &value) == GT_OK)
        paramsPtr->initI2c = (GT_BOOL)value;
    if(appDemoDbEntryGet("initPhy", &value) == GT_OK)
        paramsPtr->initPhy = (GT_BOOL)value;
    if(appDemoDbEntryGet("initPort", &value) == GT_OK)
        paramsPtr->initPort = (GT_BOOL)value;
    if(appDemoDbEntryGet("initTc", &value) == GT_OK)
        paramsPtr->initTc = (GT_BOOL)value;
    if(appDemoDbEntryGet("initTrunk", &value) == GT_OK)
        paramsPtr->initTrunk = (GT_BOOL)value;
    if(appDemoDbEntryGet("initPcl", &value) == GT_OK)
        paramsPtr->initPcl = (GT_BOOL)value;
    if(appDemoDbEntryGet("initTcam", &value) == GT_OK)
        paramsPtr->initTcam = (GT_BOOL)value;
    if(appDemoDbEntryGet("initPolicer", &value) == GT_OK)
        paramsPtr->initPolicer = (GT_BOOL)value;
    if(appDemoDbEntryGet("initAction", &value) == GT_OK)
        paramsPtr->initAction = (GT_BOOL)value;
    if(appDemoDbEntryGet("initSflow", &value) == GT_OK)
        paramsPtr->initSflow = (GT_BOOL)value;

    return GT_OK;
}

/*******************************************************************************
* appDemoOnDistributedSimAsicSideSet
*
* DESCRIPTION:
*       set value to the appDemoOnDistributedSimAsicSide flag.
*
* INPUTS:
*       value  - value to be set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_STATE - if called after initialization of system -- after cpssInitSystem
*
* COMMENTS:
*       Function is called from the TERMINAL
*
*******************************************************************************/
GT_STATUS appDemoOnDistributedSimAsicSideSet
(
    IN GT_U32  value
)
{
    if(systemInitialized == GT_TRUE)
    {
        /* will not have effect if called after initialization of system */
        osPrintf("appDemoOnDistributedSimAsicSideSet : should be called before cpssInitSystem \n");

        return GT_BAD_STATE;
    }

    /* when appDemoOnDistributedSimAsicSide != 0
       this image is running on simulation:
       1. our application on distributed "Asic side"
       2. the Other application that run on the "application side" is in
          charge about all Asic configurations
       3. our application responsible to initialize the cpss DB.
       4. the main motivation is to allow "Galtis wrappers" to be called on
          this side !
          (because the Other side application may have difficult running
           "Galtis wrappers" from there)
    */
    appDemoOnDistributedSimAsicSide = value;

    return GT_OK;
}

#ifdef ASIC_SIMULATION
/*******************************************************************************
* appDemoRtosOnSimulationInit
*
* DESCRIPTION:
*       initialize the Simulation section on the "target board" part of the
*       "RTOS on simulation" .
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
*
* COMMENTS:
*       Function is called as first step of the cpssInitSystem
*
*******************************************************************************/
void appDemoRtosOnSimulationInit
(
    void
)
{
    SIM_OS_FUNC_BIND_STC simOsBindInfo;
    static int initDone = 0;

    if(initDone)
    {
        /* allow to call this before cpssInitSystem */
        return;
    }

    /* Must be called before use any of OS functions. */
    osWrapperOpen(NULL);

    /**********************************************/
    /* bind the simulation with extra OS services */
    /**********************************************/
    {
        /* reset all fields of simOsBindInfo */
        osMemSet(&simOsBindInfo,0,sizeof(simOsBindInfo));

        /* let the simulation OS bind it's own functions to the simulation */
        simOsFuncBindOwnSimOs();

        BIND_FUNC(osWrapperOpen);

        BIND_LEVEL_FUNC(sockets,osSocketTcpCreate);
        BIND_LEVEL_FUNC(sockets,osSocketUdpCreate);
        BIND_LEVEL_FUNC(sockets,osSocketTcpDestroy);
        BIND_LEVEL_FUNC(sockets,osSocketUdpDestroy);
        BIND_LEVEL_FUNC(sockets,osSocketCreateAddr);
        BIND_LEVEL_FUNC(sockets,osSocketDestroyAddr);
        BIND_LEVEL_FUNC(sockets,osSocketBind);
        BIND_LEVEL_FUNC(sockets,osSocketListen);
        BIND_LEVEL_FUNC(sockets,osSocketAccept);
        BIND_LEVEL_FUNC(sockets,osSocketConnect);
        BIND_LEVEL_FUNC(sockets,osSocketSetNonBlock);
        BIND_LEVEL_FUNC(sockets,osSocketSetBlock);
        BIND_LEVEL_FUNC(sockets,osSocketSend);
        BIND_LEVEL_FUNC(sockets,osSocketSendTo);
        BIND_LEVEL_FUNC(sockets,osSocketRecv);
        BIND_LEVEL_FUNC(sockets,osSocketRecvFrom);
        BIND_LEVEL_FUNC(sockets,osSocketSetSocketNoLinger);
        BIND_LEVEL_FUNC(sockets,osSocketExtractIpAddrFromSocketAddr);
        BIND_LEVEL_FUNC(sockets,osSocketGetSocketAddrSize);
        BIND_LEVEL_FUNC(sockets,osSocketShutDown);

        BIND_LEVEL_FUNC(sockets,osSelectCreateSet);
        BIND_LEVEL_FUNC(sockets,osSelectEraseSet);
        BIND_LEVEL_FUNC(sockets,osSelectZeroSet);
        BIND_LEVEL_FUNC(sockets,osSelectAddFdToSet);
        BIND_LEVEL_FUNC(sockets,osSelectClearFdFromSet);
        BIND_LEVEL_FUNC(sockets,osSelectIsFdSet);
        BIND_LEVEL_FUNC(sockets,osSelectCopySet);
        BIND_LEVEL_FUNC(sockets,osSelect);
        BIND_LEVEL_FUNC(sockets,osSocketGetSocketFdSetSize);

        /* this needed for binding the OS of simulation with our OS functions */
        simOsFuncBind(&simOsBindInfo);
    }

    /************************************************/
    /* init the kernel of simulation -- the PPs,FAs */
    /************************************************/
    simulationLibInit();

    /* state that init was done */
    initDone = 1;

    return;
}
#endif /*ASIC_SIMULATION*/


/*******************************************************************************
* appDemoCpuEtherPortUsed
*
* DESCRIPTION:
*           is CPU Ethernet port used (and not SDMA)
*
* INPUTS:
*           none
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE - if CPU Ethernet port used (and not SDMA)
*       GT_FALSE - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_BOOL appDemoCpuEtherPortUsed(void)
{
    return appDemoSysConfig.cpuEtherPortUsed;
}

/*******************************************************************************
* APP_DEMO_CPSS_EVENT_BIND_FUNC
*
* DESCRIPTION:
*       This routine binds a user process to unified event. The routine returns
*       a handle that is used when the application wants to wait for the event
*       (cpssEventSelect), receive the event(cpssEventRecv) or transmit a packet
*       using the Network Interface.
*
*       NOTE : the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       uniEventArr - The unified event list.
*       arrLength   - The unified event list length.
*
* OUTPUTS:
*       hndlPtr     - (pointer to) The user handle for the bounded events.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_FULL - when trying to set the "tx buffer queue unify event"
*                 (CPSS_PP_TX_BUFFER_QUEUE_E) with other events in the same
*                 handler
*       GT_ALREADY_EXIST - one of the unified events already bound to another
*                 handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoEventBind
(
    IN  CPSS_UNI_EV_CAUSE_ENT uniEventArr[],
    IN  GT_U32               arrLength,
    OUT GT_UINTPTR           *hndlPtr
)
{
    return cpssEventBind(uniEventArr,arrLength,hndlPtr);
}

/*******************************************************************************
* APP_DEMO_CPSS_EVENT_SELECT_FUNC
*
* DESCRIPTION:
*       This function waiting for one of the events ,relate to the handler,
*       to happen , and gets a list of events (in array of bitmaps format) that
*       occurred .
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl                - The user handle for the bounded events.
*       timeoutPtr          - (pointer to) Wait timeout in milliseconds
*                             NULL pointer means wait forever.
*       evBitmapArrLength   - The bitmap array length (in words).
*
* OUTPUTS:
*       evBitmapArr         - The bitmap array of the received events.
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_TIMEOUT - when the "time out" requested by the caller expired and no
*                    event occurred during this period
*       GT_BAD_PARAM - bad hndl parameter , the hndl parameter is not legal
*                     (was not returned by cpssEventBind(...))
*       GT_BAD_PTR  - evBitmapArr parameter is NULL pointer
*                     (and evBitmapArrLength != 0)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoEventSelect
(
    IN  GT_UINTPTR           hndl,
    IN  GT_U32              *timeoutPtr,
    OUT GT_U32              evBitmapArr[],
    IN  GT_U32              evBitmapArrLength
)
{
    return  cpssEventSelect(hndl,timeoutPtr,evBitmapArr,evBitmapArrLength);
}

/*******************************************************************************
* APP_DEMO_CPSS_EVENT_RECV_FUNC
*
* DESCRIPTION:
*       This function gets general information about the selected unified event.
*       The function retrieve information about the device number that relate to
*       the event , and extra info about port number / priority queue that
*       relate to the event.
*       The function retrieve the info about the first occurrence of this event
*       in the queue.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl        - The user handle for the bounded events.
*       evCause     - The specified unify event that info about it's first
*                     waiting occurrence required.
* OUTPUTS:
*       evExtDataPtr- (pointer to)The additional date (port num / priority
*                     queue number) the event was received upon.
*       evDevPtr    - (pointer to)The device the event was received upon
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_NO_MORE   - There is no more info to retrieve about the specified
*                      event .
*       GT_BAD_PARAM - bad hndl parameter ,
*                      or hndl bound to CPSS_PP_TX_BUFFER_QUEUE_E --> not allowed
*                      use dedicated "get tx ended info" function instead
*       GT_BAD_PTR  - one of the parameters is NULL pointer
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS appDemoEventRecv
(
    IN  GT_UINTPTR            hndl,
    IN  CPSS_UNI_EV_CAUSE_ENT evCause,
    OUT GT_U32              *evExtDataPtr,
    OUT GT_U8               *evDevPtr
)
{
    return cpssEventRecv(hndl,evCause,evExtDataPtr,evDevPtr);
}

/*******************************************************************************
* appDemoCpssEventDeviceMaskSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
static GT_STATUS appDemoEventDeviceMaskSet
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN CPSS_EVENT_MASK_SET_ENT  operation
)
{
    return cpssEventDeviceMaskSet(devNum,uniEvent,operation);
}


/******************************************************************************
* cmdAppPpConfigGet
*
* DESCRIPTION:
*       get pointer to the application's info about the PP
*
* INPUTS:
*       devIndex - index of the device in the Application DB (0..127)
*
* OUTPUTS:
*       ppConfigPtr - the info about the PP
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PTR - on NULL pointer
*       GT_OUT_OF_RANGE - on devIndex > 127
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS appDemoPpConfigGet
(
    IN GT_U8    devIndex,
    OUT CMD_APP_PP_CONFIG* ppConfigPtr
)
{
    if(ppConfigPtr == NULL)
    {
        return GT_BAD_PTR;
    }

    if(devIndex >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_OUT_OF_RANGE;
    }

    ppConfigPtr->deviceId = appDemoPpConfigList[devIndex].deviceId;
    ppConfigPtr->devNum = appDemoPpConfigList[devIndex].devNum;
    ppConfigPtr->valid = appDemoPpConfigList[devIndex].valid;
    ppConfigPtr->wa.trunkToCpuPortNum = appDemoPpConfigList[devIndex].wa.trunkToCpuPortNum;

    return GT_OK;
}

/*******************************************************************************
* cmdAppIsSystemInitialized
** DESCRIPTION:
*       is system initialized
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - system initialized
*       GT_FALSE  - system NOT initialized
*
* COMMENTS:
*
*******************************************************************************/
static GT_BOOL appDemoIsSystemInitialized
(
    void
)
{
    return systemInitialized;
}

/*******************************************************************************
* prvAppDemoEventFatalErrorEnable
*
* DESCRIPTION:
*       Set fatal error handling type.
*
* INPUTS:
*       fatalErrorType  - fatal error handling type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - on wrong fatalErrorType
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvAppDemoEventFatalErrorEnable
(
    IN GT_32 fatalErrorType
)
{
    APP_DEMO_FATAL_ERROR_TYPE  type = (APP_DEMO_FATAL_ERROR_TYPE) fatalErrorType;

    return appDemoEventFatalErrorEnable(type);
}


/*******************************************************************************
* appDemoPpConfigPrint
*
* DESCRIPTION:
*       Return system configuration that was set by user .
*
* INPUTS:
*       devNum              - device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK               - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS appDemoPpConfigPrint
(
    IN  GT_U8  devNum
)
{

#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
#ifdef EXMXPM_FAMILY
PRV_CPSS_EXMXPM_MODULE_CONFIG_STC *moduleCnfgPtr = &PRV_CPSS_EXMXPM_PP_MAC(devNum)->moduleCfg;
PRV_CPSS_EXMXPM_PP_TBLS_ADDR_STC  *tblsPtr       = &PRV_CPSS_EXMXPM_PP_MAC(devNum)->tblsAddr;
#endif

#ifdef CHX_FAMILY
       PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
#endif


        osPrintf("\n");
        osPrintf("                  CPSS System configuration                \n");
        osPrintf(" __________________________________________________________\n");
        osPrintf("|          Config parameter            |        value      |\n");
        osPrintf("|__________________________________________________________|\n");
        osPrintf("|__________numOfTrunks_________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].numOfTrunks);
        osPrintf("|__________maxNumOfVirtualRouters______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfVirtualRouters);
        osPrintf("|__________maxNumOfIpNextHop___________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpNextHop);
#ifdef CHX_FAMILY

        osPrintf("|__________maxNumOfMll_________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfMll);
        osPrintf("|__________maxNumOfIpv4Prefixes________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv4Prefixes);
        osPrintf("|__________maxNumOfIpv6Prefixes________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv6Prefixes);

        osPrintf("|__________maxNumOfIpv4McEntries_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv4McEntries);

        osPrintf("|__________maxNumOfPceForIpPrefixes____|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfPceForIpPrefixes);
        osPrintf("|__________usePolicyBasedRouting_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].usePolicyBasedRouting);
        osPrintf("|__________usePolicyBasedDefaultMc_____|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].usePolicyBasedDefaultMc);
        osPrintf("|__________maxNumOfPbrEntries__________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfPbrEntries);
        osPrintf("|__________maxNumOfTunnelEntries_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfTunnelEntries);
        osPrintf("|__________maxNumOfIpv4TunnelTerms_____|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv4TunnelTerms);
        osPrintf("|__________lpmDbPartitionEnable________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].lpmDbPartitionEnable);
        osPrintf("|__________lpmDbSupportIpv4____________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].lpmDbSupportIpv4);
        osPrintf("|__________lpmDbSupportIpv6____________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].lpmDbSupportIpv6);
        osPrintf("|__________lpmDbFirstTcamLine__________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].lpmDbFirstTcamLine);
        osPrintf("|__________lpmDbLastTcamLine___________|________%-8d___|\n",
                 ppUserLogicalConfigParams[devNum].lpmDbLastTcamLine);

        osPrintf("|__________lpmRamBlocksAllocationMethod___|______%-8d___|\n",
                 ppUserLogicalConfigParams[devNum].lpmRamMemoryBlocksCfg.blocksAllocationMethod);

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
        osPrintf("|__________maxNumOfLifs________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfLifs);
        osPrintf("|__________inlifPortMode_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].inlifPortMode);
        osPrintf("|__________maxNumOfNhlfe_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfNhlfe);
        osPrintf("|__________maxNumOfMplsIfs_____________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfMplsIfs);
        osPrintf("|__________ipv6MemShare________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].ipv6MemShare);
        osPrintf("|__________maxNumOfPolicerEntries______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfPolicerEntries);
        osPrintf("|__________lpmDbNumOfMemCfg____________|________%-8d___|\n",
                 ppUserLogicalConfigParams[devNum].lpmDbNumOfMemCfg);
        osPrintf("|__________lpm db ip memory config:____|_______________|\n");
        osPrintf("|__________routingSramCfgType__________|________%-8d___|\n",
                 ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->routingSramCfgType);

        osPrintf("|__________numOfSramsSizes_____________|________%-8d___|\n",
                 ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->numOfSramsSizes);
        for( i=0; i<ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->numOfSramsSizes; i++)
        {
        osPrintf("|__________sramsSizeArray[%d]__________|________%-8d___|\n",
                     i,ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->sramsSizeArray[i]);
        }
        osPrintf("|__________useLpmInternalSram_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->useLpmInternalSram);
        for( i=0; i<CPSS_MAX_NUMBER_LPM_INTERNAL_SRAMS_CNS; i++)
        {
        osPrintf("|__________lpmInternalSramSizeArray[%d]__________|________%-8d___|\n",
                     i,ppUserLogicalConfigParams[devNum].lpmDbMemCfgArray->lpmInternalSramSizeArray[i]);
        }
        osPrintf("|__________ditMemoryMode_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].ditMemoryMode);
        osPrintf("|__________ditIpMcMode_________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].ditIpMcMode);
        osPrintf("|__________maxNumOfDitUcIpEntries______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitUcIpEntries);
        osPrintf("|__________maxNumOfDitUcMplsEntries____|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitUcMplsEntries);
        osPrintf("|__________maxNumOfDitUcOtherEntries___|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitUcOtherEntries);
        osPrintf("|__________maxNumOfDitMcIpEntries______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitMcIpEntries);
        osPrintf("|__________maxNumOfDitMcMplsEntries____|________%-8d___|\n",
                   ppUserLogicalConfigParams[devNum].maxNumOfDitMcMplsEntries);
        osPrintf("|__________maxNumOfDitMcOtherEntries___|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfDitMcOtherEntries);
        osPrintf("|__________fdbHashSize_________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].fdbTableInfo.fdbHashSize);
        osPrintf("|__________fdbTableMode________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].fdbTableInfo.fdbTableMode);
        osPrintf("|__________fdbToLutRatio_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].fdbTableInfo.fdbToLutRatio);
        osPrintf("|__________extFdbHashMode______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].fdbTableInfo.extFdbHashMode);
        osPrintf("|__________extTcamActionLinesMemCntl0__|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].externalTcamInfo.actionLinesInMemoryControl0);
        osPrintf("|__________extTcamActionLinesMemCntl1__|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].externalTcamInfo.actionLinesInMemoryControl1);

#endif  /* EXMXPM_FAMILY */

        osPrintf("|______________________________________________________|\n");
        osPrintf("|___________________other_params_______________________|\n");
        osPrintf("|______________________________________________________|\n");
        osPrintf("|__________maxMcGroups_________________|_______________|\n",
                    ppUserLogicalConfigParams[devNum].maxMcGroups);
        osPrintf("|__________maxVid______________________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxVid);
        osPrintf("|__________maxNumOfPclAction___________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfPclAction);
        osPrintf("|__________pclActionSize_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].pclActionSize);
        osPrintf("|__________policerConformCountEn_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].policerConformCountEn);
        osPrintf("|__________maxNumOfIpv6McGroups________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfIpv6McGroups);
        osPrintf("|__________maxNumOfTunnelTerm__________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].maxNumOfTunnelTerm);
        osPrintf("|__________vlanInlifEntryType__________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].vlanInlifEntryType);
        osPrintf("|__________ipMemDivisionOn_____________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].ipMemDivisionOn);
        osPrintf("|__________defIpv6McPclRuleIndex_______|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].defIpv6McPclRuleIndex);
        osPrintf("|__________vrIpv6McPclId_______________|________%-8d___|\n",
                    ppUserLogicalConfigParams[devNum].vrIpv6McPclId);
        osPrintf("|______________________________________________________|\n");
        osPrintf("|_________________some_table_sizes_____________________|\n");
        osPrintf("|______________________________________________________|\n");

#ifdef CHX_FAMILY
        osPrintf("|__________amount of CNC blocks________|________%-8d___|\n",
                    fineTuningPtr->tableSize.cncBlocks);
        osPrintf("|__________entries num in  CNC block___|________%-8d___|\n",
                    fineTuningPtr->tableSize.cncBlockNumEntries);
        osPrintf("|__________bufferMemory________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.bufferMemory);
        osPrintf("|__________fdb_________________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.fdb);
        osPrintf("|__________mllPairs____________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.mllPairs);
        osPrintf("|__________egressPolicersNum___________|________%-8d___|\n",
                    fineTuningPtr->tableSize.egressPolicersNum);
        osPrintf("|__________policersNum_________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.policersNum);
        osPrintf("|__________policyTcamRaws______________|________%-8d___|\n",
                    fineTuningPtr->tableSize.policyTcamRaws);
        osPrintf("|__________router______________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.router);
        osPrintf("|__________tunnelTerm__________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.tunnelTerm);
        osPrintf("|__________tunnelStart_________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.tunnelStart);
        osPrintf("|__________routerArp___________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.routerArp);
        osPrintf("|__________routerNextHop_______________|________%-8d___|\n",
                    fineTuningPtr->tableSize.routerNextHop);
        osPrintf("|__________ecmpQos_____________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.ecmpQos);
        osPrintf("|__________lpmRam______________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.lpmRam);
        osPrintf("|__________stgNum______________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.stgNum);
        osPrintf("|__________transmitDescr_______________|________%-8d___|\n",
                    fineTuningPtr->tableSize.transmitDescr);
        osPrintf("|__________vidxNum_____________________|________%-8d___|\n",
                    fineTuningPtr->tableSize.vidxNum);
#endif

#ifdef EXMXPM_FAMILY
        osPrintf("|__________fdbMemoryMode_______________|________%-8d___|\n",
                    moduleCnfgPtr->bridgeCfg.fdbMemoryMode);
        osPrintf("|__________fdbNumOfEntries_____________|________%-8d___|\n",
                    moduleCnfgPtr->bridgeCfg.fdbNumOfEntries);
        osPrintf("|__________fdbTcamNumOfEntries_________|________%-8d___|\n",
                    moduleCnfgPtr->bridgeCfg.fdbTcamNumOfEntries);
        osPrintf("|__________ditMcIpEntriesNum___________|________%-8d___|\n",
                    moduleCnfgPtr->ditCfg.mcIpEntriesNum);
        osPrintf("|__________ditUcIpEntriesNum___________|________%-8d___|\n",
                    moduleCnfgPtr->ditCfg.ucIpEntriesNum);
        osPrintf("|__________externalTcamUsed____________|________%-8d___|\n",
                    moduleCnfgPtr->externalTcamCfg.externalTcamUsed);
        osPrintf("|__________stgNum______________________|________%-8d___|\n",
                    tblsPtr->VLAN.STGTable.numOfEntries);
        osPrintf("|__________vidxNum_____________________|________%-8d___|\n",
                    tblsPtr->VLAN.VIDXTable.numOfEntries);
        osPrintf("|__________vlanNum_____________________|________%-8d___|\n",
                    tblsPtr->VLAN.VLANTable.numOfEntries);
        osPrintf("|__________ttiDSATagQoS2QoSNum_________|________%-8d___|\n",
                    tblsPtr->TTI.DSATagQoS2QoSParametersMapMemory.numOfEntries);
        osPrintf("|__________ttiDSCP2QoS_________________|________%-8d___|\n",
                    tblsPtr->TTI.DSCP2QoSParametersMapMemory.numOfEntries);
        osPrintf("|__________ttiMac2MeNum________________|________%-8d___|\n",
                    tblsPtr->TTI.MAC2METCAM.numOfEntries);
        osPrintf("|__________ttiPortProtocolVIDAndQoSNum_|________%-8d___|\n",
                    tblsPtr->TTI.portProtocolVIDAndQoSConfigTable.numOfEntries);
        osPrintf("|__________ttiPortVLANAndQoS___________|________%-8d___|\n",
                    tblsPtr->TTI.portVLANAndQoSConfigTable.numOfEntries);
        osPrintf("|__________ttiVirtualPortID____________|________%-8d___|\n",
                    tblsPtr->TTI.virtualPortIDAssignmentMemory.numOfEntries);
        osPrintf("|__________ttiUserDefinedBytesCnfg_____|________%-8d___|\n",
                    tblsPtr->TTI.userDefinedBytesConfigMemory.numOfEntries);
        osPrintf("|__________ttiUserDefinedBytesId_______|________%-8d___|\n",
                    tblsPtr->TTI.userDefinedBytesIDMemory);
        osPrintf("|__________pclInternalInLIFNum_________|________%-8d___|\n",
                    tblsPtr->PCL.internalInLIFMemory.numOfEntries);
        osPrintf("|__________pclIPCLConfigNum____________|________%-8d___|\n",
                    tblsPtr->PCL.IPCLConfigMemory.numOfEntries);
        osPrintf("|__________mtARPTunnelMemoryNum________|________%-8d___|\n",
                    tblsPtr->MT.ARPTunnelMemory.numOfEntries);
        osPrintf("|__________mtFdbEntriesTcamNum_________|________%-8d___|\n",
                    tblsPtr->MT.fdbEntriesForTcam.numOfEntries);
        osPrintf("|__________mtFDBMemoryNum______________|________%-8d___|\n",
                    tblsPtr->MT.FDBMemory.numOfEntries);
        osPrintf("|__________mtFDBTCAMNum________________|________%-8d___|\n",
                    tblsPtr->MT.FDBTCAM.numOfEntries);
        osPrintf("|__________mtFDBTSNum__________________|________%-8d___|\n",
                    tblsPtr->MT.FDBTSMemory.numOfEntries);
        osPrintf("|__________extTcamAction1Num___________|________%-8d___|\n",
                    tblsPtr->externalMemory.extTcamAction1Table.numOfEntries);
        osPrintf("|__________extTcamActionNum____________|________%-8d___|\n",
                    tblsPtr->externalMemory.extTcamActionTable.numOfEntries);
        osPrintf("|__________extFdbLutNum________________|________%-8d___|\n",
                    tblsPtr->externalMemory.fdbLutTable.numOfEntries);
        osPrintf("|__________extFdbNum___________________|________%-8d___|\n",
                    tblsPtr->externalMemory.fdbTable.numOfEntries);
        osPrintf("|__________extInlif____________________|________%-8d___|\n",
                    tblsPtr->externalMemory.inlif.numOfEntries);
        osPrintf("|__________tccInternalActionNum________|________%-8d___|\n",
                    tblsPtr->legacyUnits.TCC.internalAction.numOfEntries);
        osPrintf("|__________tccInternalTcamNum__________|________%-8d___|\n",
                    tblsPtr->legacyUnits.TCC.internalTcam.numOfEntries);
        osPrintf("|__________MPLSInterfaceNum____________|________%-8d___|\n",
                    tblsPtr->MPLS.MPLSInterfaceTable.numOfEntries);
        osPrintf("|__________MPLSNhlfeNum________________|________%-8d___|\n",
                    tblsPtr->MPLS.MPLSNhlfeTable.numOfEntries);
        osPrintf("|__________ingrPortsBridge_____________|________%-8d___|\n",
                    tblsPtr->L2I.ingrPortsBridgeConfig.ingrPortBridgeConfigTable.numOfEntries);
        osPrintf("|__________routerNSTNum________________|________%-8d___|\n",
                    tblsPtr->IPvX.routerNSTTable.numOfEntries);
        osPrintf("|__________routerNextHopNum____________|________%-8d___|\n",
                    tblsPtr->IPvX.routerNextHopTable.numOfEntries);
        osPrintf("|__________iCCCInternalNum_____________|________%-8d___|\n",
                    tblsPtr->legacyUnits.ICCC.iCCCInternalMemory.numOfEntries);
        osPrintf("|__________eCCCInternalNum_____________|________%-8d___|\n",
                    tblsPtr->legacyUnits.ECCC.eCCCInternalMemory.numOfEntries);
        osPrintf("|__________egrPolicerManagementNum_____|________%-8d___|\n",
                    tblsPtr->EPLR.egrPolicerManagementCntrsMemory.numOfEntries);
        osPrintf("|__________egrPolicerReMarkingNum______|________%-8d___|\n",
                    tblsPtr->EPLR.egrPolicerReMarkingMemory.numOfEntries);
        osPrintf("|__________ingrPolicerManagementNum____|________%-8d___|\n",
                    tblsPtr->IPLR.ingrPolicerManagementCntrsMemory.numOfEntries);
        osPrintf("|__________iingrPolicerReMarkingNum____|________%-8d___|\n",
                    tblsPtr->IPLR.ingrPolicerReMarkingMemory.numOfEntries);

#endif /* EXMXPM_FAMILY */


#endif  /* (defined CHX_FAMILY) || (defined EXMXPM_FAMILY) */

        return GT_OK;

}




/*******************************************************************************
* applicationExtServicesBind
*
* DESCRIPTION:
*       the mainCmd calls this function , so the application (that implement
*       this function) will bind the mainCmd (and GaltisWrapper) with OS ,
*       external driver functions  and other application functions
*
* INPUTS:
*       none.
* OUTPUTS:
*       extDrvFuncBindInfoPtr - (pointer to) set of external driver call back functions
*       osFuncBindPtr - (pointer to) set of OS call back functions
*       osExtraFuncBindPtr - (pointer to) set of extra OS call back functions (that CPSS not use)
*       extraFuncBindPtr - (pointer to) set of extra call back functions (that CPSS not use) (non OS functions)
*       traceFuncBindPtr - (pointer to) set of Trace call back functions
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       This function must be implemented by the Application !!!
*
*******************************************************************************/
GT_STATUS   applicationExtServicesBind(
    OUT CPSS_EXT_DRV_FUNC_BIND_STC   *extDrvFuncBindInfoPtr,
    OUT CPSS_OS_FUNC_BIND_STC        *osFuncBindPtr,
    OUT CMD_OS_FUNC_BIND_EXTRA_STC   *osExtraFuncBindPtr,
    OUT CMD_FUNC_BIND_EXTRA_STC      *extraFuncBindPtr,
    OUT CPSS_TRACE_FUNC_BIND_STC     *traceFuncBindPtr
)
{
    GT_STATUS   rc;


    if(extDrvFuncBindInfoPtr == NULL ||
       osFuncBindPtr == NULL ||
       osExtraFuncBindPtr == NULL ||
       extraFuncBindPtr == NULL ||
       traceFuncBindPtr == NULL)
    {
        return GT_BAD_PTR;
    }

#if defined(SHARED_MEMORY) && (!defined(LINUX_SIM))
    /* Shared memory approach on ARM should use dlsym-based workaround for pointers to functions */
    rc = shrMemGetDefaultExtDrvFuncs(extDrvFuncBindInfoPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = shrMemGetDefaultOsBindFuncsThruDynamicLoader(osFuncBindPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Shared memory for Trace should be implemented later */
    CPSS_TBD_BOOKMARK
#else
    rc = cpssGetDefaultExtDrvFuncs(extDrvFuncBindInfoPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssGetDefaultOsBindFuncs(osFuncBindPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssGetDefaultTraceFuncs(traceFuncBindPtr);
    if(rc != GT_OK)
    {
        return rc;
    }
#endif

    osFuncBindPtr->osTaskBindInfo.osTaskCreateFunc = osTaskCreate;
    osFuncBindPtr->osTaskBindInfo.osTaskDeleteFunc = osTaskDelete;
/*    osFuncBindPtr->osTaskBindInfo.osTaskGetSelfFunc = osTaskGetSelf; */

    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketTcpCreate                       );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketTcpCreate                       );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketUdpCreate                       );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketTcpDestroy                      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketUdpDestroy                      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketCreateAddr                      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketDestroyAddr                     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketBind                            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketListen                          );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketAccept                          );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketConnect                         );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSetNonBlock                     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSetBlock                        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSend                            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSendTo                          );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketRecv                            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketRecvFrom                        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketSetSocketNoLinger               );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketExtractIpAddrFromSocketAddr     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketGetSocketAddrSize               );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketShutDown                        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectCreateSet                       );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectEraseSet                        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectZeroSet                         );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectAddFdToSet                      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectClearFdFromSet                  );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectIsFdSet                         );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelectCopySet                         );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSelect                                );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osSocketsBindInfo,osSocketGetSocketFdSetSize              );

    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osTasksBindInfo,osTaskGetSelf                            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osTasksBindInfo,osSetTaskPrior                           );

    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolCreatePool        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolCreateDmaPool     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolReCreatePool      );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolDeletePool        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolGetBuf            );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolFreeBuf           );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolGetBufSize        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolExpandPool        );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolGetBufFreeCnt     );
    BIND_APP_LEVEL_FUNC(osExtraFuncBindPtr,osPoolsBindInfo,gtPoolPrintStats        );

    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,cpuEthernetPortBindInfo,cmdIsCpuEtherPortUsed        ,appDemoCpuEtherPortUsed             );

    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,eventsBindInfo,cmdCpssEventBind         ,appDemoEventBind             );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,eventsBindInfo,cmdCpssEventSelect       ,appDemoEventSelect           );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,eventsBindInfo,cmdCpssEventRecv         ,appDemoEventRecv             );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,eventsBindInfo,cmdCpssEventDeviceMaskSet,appDemoEventDeviceMaskSet    );


    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppDbEntryAdd ,appDemoDbEntryAdd );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppDbEntryGet ,appDemoDbEntryGet );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppDbDump     ,appDemoDbDump     );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppPpConfigGet,appDemoPpConfigGet);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdInitSystem    ,cpssInitSystem    );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdInitSystemGet ,cpssInitSystemGet );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdResetSystem   ,cpssResetSystem   );
   /* BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdInitConfi     ,confi2InitSystem    );*/
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppShowBoardsList         ,appDemoShowBoardsList    );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppIsSystemInitialized    ,appDemoIsSystemInitialized);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppPpConfigPrint,appDemoPpConfigPrint);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppAllowProcessingOfAuqMessages ,
                         appDemoAllowProcessingOfAuqMessages);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppTraceHwAccessEnable ,
                         prvAppDemoTraceHwAccessEnable);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppTraceHwAccessOutputModeSet ,
                         prvAppDemoTraceHwAccessOutputModeSet);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppStartHeapAllocCounter  ,osMemStartHeapAllocationCounter );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppGetHeapAllocCounter    ,osMemGetHeapAllocationCounter   );

    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppEventFatalErrorEnable ,
                         prvAppDemoEventFatalErrorEnable);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppDemoEventsDataBaseGet ,
                         appDemoEventsDataBaseGet);


#ifdef EXMXPM_FAMILY
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamRuleIndexByLenAndLookupGet,appDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamActionIndexGet            ,appDemoExMxPmExternalTcamActionIndexGet            );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamRuleAndActionIndexGet     ,appDemoExMxPmExternalTcamRuleAndActionIndexGet     );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamClientKeyLookupSet        ,appDemoExMxPmExternalTcamClientKeyLookupSet        );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamClientKeyLookupGet        ,appDemoExMxPmExternalTcamClientKeyLookupGet        );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamClientKeyLookupSetByIndex ,appDemoExMxPmExternalTcamClientKeyLookupSetByIndex );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamClientKeyLookupGetByIndex ,appDemoExMxPmExternalTcamClientKeyLookupGetByIndex );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamConfig                    ,appDemoExMxPmExternalTcamConfig                    );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamRuleStateGet              ,appDemoExMxPmExternalTcamRuleStateGet              );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamRuleValidStatusSet        ,appDemoExMxPmExternalTcamRuleValidStatusSet        );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmExternalTcamRuleAndActionCopy         ,appDemoExMxPmExternalTcamRuleAndActionCopy         );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmHsuOldImagePreUpdatePreparation       ,appDemoExMxPmHsuOldImagePreUpdatePreparation       );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppExMxPmHsuNewImageUpdateAndSynch             ,appDemoExMxPmHsuNewImageUpdateAndSynch             );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppPuma3PortModeSpeedSet                       ,gtAppDemoPuma3PortModeSpeedSet                     );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppPuma3LoopbackEnableSet                      ,appDemoPuma3LoopbackEnableSet                      );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppPuma3BoardTypeSet                           ,appDemoPuma3BoardTypeSet                           );
#endif /*EXMXPM_FAMILY*/

#if (defined CHX_FAMILY) && (!defined EX_FAMILY)
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdPortRateTableGet, util_appDemoDxChPortFWSRatesTableGet);
#if 0
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppLionPortModeSpeedSet,gtAppDemoLionPortModeSpeedSet);
#endif
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppXcat2StackPortsModeSpeedSet,gtAppDemoXcat2StackPortsModeSpeedSet);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppXcat2SfpPortEnable,gtAppDemoXcat2SfpPortEnable);
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppDxChHsuOldImagePreUpdatePreparation       ,appDemoDxChHsuOldImagePreUpdatePreparation       );
    BIND_APP2_LEVEL_FUNC(extraFuncBindPtr,appDbBindInfo,cmdAppDxChHsuNewImageUpdateAndSynch             ,appDemoDxChHsuNewImageUpdateAndSynch             );

#endif
    return GT_OK;
}

/*******************************************************************************
* appDemoDbPpDump
*
* DESCRIPTION:
*       Dump App Demo DB
*
* INPUTS:
*       none.
* OUTPUTS:
*       none.
* RETURNS:
*       GT_OK   - GT_OK ant case
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDbPpDump(void)
{

    GT_U32 i, ii; /* iterator*/

    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid)
        {
          osPrintf("Found device with devNum %d, device id 0x%8X, family %d, mng interface %d\n",
                   appDemoPpConfigList[i].devNum, appDemoPpConfigList[i].deviceId,
                   appDemoPpConfigList[i].devFamily, appDemoPpConfigList[i].channel);
          if(appDemoPpConfigList[i].channel==CPSS_CHANNEL_PCI_E ||
             appDemoPpConfigList[i].channel==CPSS_CHANNEL_PEX_E)
          {
              if (appDemoPpConfigList[i].numOfPortGroups > 1)
              {
                  for (ii = 0; ii < appDemoPpConfigList[i].numOfPortGroups; ii++)
                  {
                      /* print PCI/PEX info */
                      osPrintf(" Port Group %d Internal Regs BAR 0x%8X, PCI/PEX regs BAR 0x%8X\n",
                               ii, appDemoPpConfigList[i].portGroupsInfo[ii].portGroupPciInfo.pciBaseAddr,
                               appDemoPpConfigList[i].portGroupsInfo[ii].portGroupPciInfo.internalPciBase);
                  }
              }
              else
              {
                  /* print PCI/PEX info */
                  osPrintf("   Internal Regs BAR 0x%8X, PCI/PEX regs BAR 0x%8X\n",
                           appDemoPpConfigList[i].pciInfo.pciBaseAddr, appDemoPpConfigList[i].pciInfo.internalPciBase);
                  prvPresteraPciInfoDump(&(appDemoPpConfigList[i].pciInfo));
              }
          }
          else if (appDemoPpConfigList[i].channel==CPSS_CHANNEL_SMI_E)
          {
              osPrintf("   SMI address %d\n",
                       appDemoPpConfigList[i].smiInfo.smiIdSel);
          }

          osPrintf(  "Init flags: ppPhase1Done %d, ppPhase2Done %d, ppLogicalInitDone %d, ppGeneralInitDone %d\n",
                     appDemoPpConfigList[i].ppPhase1Done, appDemoPpConfigList[i].ppPhase2Done,
                     appDemoPpConfigList[i].ppLogicalInitDone, appDemoPpConfigList[i].ppGeneralInitDone);

          osPrintf("   Interrupt line %d\n",
                   appDemoPpConfigList[i].intLine);

        }
    }


    return 0;
}


/******************************************************************************
* appDemoDebugDeviceIdSet
*
* DESCRIPTION:
*       Set given device ID value for specific device.
*       Should be called before cpssInitSystem().
*
* INPUTS:
*       devIndex - index of the device in the Application DB (0..127)
*       deviceId - device type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PTR - on NULL pointer
*       GT_OUT_OF_RANGE - on devIndex > 127
*
* COMMENTS:
*       None.
*
******************************************************************************/
extern GT_STATUS appDemoDebugDeviceIdSet
(
    IN GT_U8    devNum,
    IN GT_U32   deviceId
)
{
    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_OUT_OF_RANGE;
    }

    /* only 16 MSBits used by prvCpssDrvDebugDeviceIdSet(...) */
    osPrintf(" NOTE: only 16 MSBits [0x%4.4x] will be used by the 'cpssDriver' !! \n",
        (deviceId >> 16));

    deviceIdDebug[devNum] = deviceId;

    useDebugDeviceId = GT_TRUE;

    return GT_OK;
}

/******************************************************************************
* appDemoDebugDeviceIdGet
*
* DESCRIPTION:
*       Get given device ID value for specific device.
*
* INPUTS:
*       devIndex - index of the device in the Application DB (0..127)
*
* OUTPUTS:
*       deviceIdPtr - pointer to device type
*
* RETURNS:
*       GT_OK      - on success
*       GT_NO_SUCH - on not set debug device ID
*       GT_OUT_OF_RANGE - on devIndex > 127
*
* COMMENTS:
*       None.
*
******************************************************************************/
extern GT_STATUS appDemoDebugDeviceIdGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *deviceIdPtr
)
{
    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_OUT_OF_RANGE;
    }

    if ((useDebugDeviceId == GT_FALSE) ||
        (0 == deviceIdDebug[devNum]))
    {
        return GT_NO_SUCH;
    }

    *deviceIdPtr = deviceIdDebug[devNum];

    return GT_OK;
}

/******************************************************************************
* appDemoTracePrintHwData
*
* DESCRIPTION:
*       Print data of HW Trace.
*
* INPUTS:
*       addr        - start address that the access was made to
*       length      - length of the data that was written in words
*       dataPtr     - (pointer to) data that was written
*       printFuncPtr - (pointer to) print function
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_VOID appDemoTracePrintHwData
(
    IN GT_U32                   addr,
    IN GT_U32                   length,
    IN GT_U32                   *dataPtr,
    IN FUNCP_TRACE_PRINTF_DATA   printFuncPtr
)
{
    GT_U32  ii;

    printFuncPtr("ADDR  0x%08X", addr);
    printFuncPtr("  ");

    for (ii = 0; ii < length; ii++)
    {
        printFuncPtr("0x%08X", dataPtr[ii]);
        printFuncPtr(" ");

        /* the data is printed so, that 4 words are printed per line */
        if ((ii != 0) && (ii % 3 == 0))
        {
            printFuncPtr("\n");
        }
    }
}

/******************************************************************************
* appDemoTraceStoreHwData
*
* DESCRIPTION:
*       Store data of HW Trace to HW Access DB.
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - ports group number
*       isrContext  - GT_TRUE: called from ISR context (cpssDrvPpHwIsrWrite,
*                                                       cpssDrvPpHwIsrRead)
*                    GT_FALSE: called from not ISR context.
*       addrSpace   - memory address space
*       addr        - start address that the access was made to
*       length      - length of the data that was written in words
*       dataPtr     - (pointer to) data that was written
*       dbPtr       - (pointer to) read/write HW Access DB
*       infoDbPtr   - (pointer to) read/write HW Access Info
*
* OUTPUTS:
*       dbPtr       - (pointer to) read/write HW Access DB
*       infoDbPtr   - (pointer to) read/write HW Access Info
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS appDemoTraceStoreHwData
(
    IN GT_U8                                devNum,
    IN GT_U32                               portGroupId,
    IN GT_BOOL                              isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT         addrSpace,
    IN GT_U32                               addr,
    IN GT_U32                               length,
    IN GT_U32                               *dataPtr,
    IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT    accessType,
    INOUT APP_DEMO_CPSS_HW_ACCESS_DB_STC    **dbPtrPtr,
    INOUT APP_DEMO_CPSS_HW_ACCESS_INFO_STC  *infoDbPtr
)
{
    GT_U32  newDbSize;
    GT_U32  tmpAccessParamsBmp;
    GT_U32  ii;
    GT_U32  curDbIndex;
    GT_32   key = 0;

    /* check if we are in task context */
    if (isrContext == GT_FALSE)
    {
        /* task context, lock ISR */
        osSetIntLockUnlock(INTR_MODE_LOCK, &key);
    }

    /* check if there is enough space in DB */
    if ((infoDbPtr->maxSize - infoDbPtr->curSize) < length)
    {
        /*limit data base size to APP_DEMO_CPSS_HW_ACCESS_MAX_DB_STEP_COUNT -1  of incrementations */
        if((infoDbPtr->curSize + APP_DEMO_CPSS_HW_ACCESS_DB_STEP_SIZE_CNS) >(APP_DEMO_CPSS_HW_ACCESS_DB_STEP_SIZE_CNS*APP_DEMO_CPSS_HW_ACCESS_MAX_DB_STEP_COUNT))
        {

           if(infoDbPtr->corrupted == GT_FALSE)
           {
            osPrintf("Warning: The register access DB  has reached the limit . Current size %d (0x%x) . \n",infoDbPtr->curSize ,infoDbPtr->curSize);

            infoDbPtr->corrupted = GT_TRUE;
           }

            /* check if we are in task context */
            if (isrContext == GT_FALSE)
            {
                /* task context, unlock ISR */
                osSetIntLockUnlock(INTR_MODE_UNLOCK, &key);
            }

            return GT_OK;
        }

        /* reallocate the DB */
        newDbSize = infoDbPtr->curSize + APP_DEMO_CPSS_HW_ACCESS_DB_STEP_SIZE_CNS;

        *dbPtrPtr = cpssOsRealloc(*dbPtrPtr, (sizeof(APP_DEMO_CPSS_HW_ACCESS_DB_STC)* newDbSize));

        if (*dbPtrPtr == NULL)
        {
            /* check if we are in task context */
            if (isrContext == GT_FALSE)
            {
                /* task context, unlock ISR */
                osSetIntLockUnlock(INTR_MODE_UNLOCK, &key);
            }

            return GT_OUT_OF_CPU_MEM;
        }

        /* Update MAX size of allocated memory space */
        infoDbPtr->maxSize = newDbSize;
    }

    /* set device number */
    tmpAccessParamsBmp = devNum;
    /* set port group id */
    tmpAccessParamsBmp |= ((0xFF &portGroupId) << 8);
    /* set ISR Context */
    tmpAccessParamsBmp |= (BOOL2BIT_MAC(isrContext) << 16);
    /* set PP,PCI/PEX or Reset and Init Controller space */
    tmpAccessParamsBmp |= (addrSpace << 17);

   if(accessType == APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E)
   {
     /* update access type */
           tmpAccessParamsBmp |= (1 << 26);
   }

    /* set current DB index */
    curDbIndex = infoDbPtr->curSize;

    if (addr == 0)  /*delay,  save milisec as length*/
    {
        (*dbPtrPtr)[curDbIndex].addr = addr;
        (*dbPtrPtr)[curDbIndex].data = *dataPtr;
        (*dbPtrPtr)[curDbIndex].accessParamsBmp = tmpAccessParamsBmp;
        /* move cursor to point to the next DB entry */
        infoDbPtr->curSize++;

    }
    else
    {
        if (length == 1)
        {
            /* the access is register access */

            (*dbPtrPtr)[curDbIndex].addr = addr;
            (*dbPtrPtr)[curDbIndex].data = *dataPtr;
            (*dbPtrPtr)[curDbIndex].accessParamsBmp = tmpAccessParamsBmp;
            /* move cursor to point to the next DB entry */
            infoDbPtr->curSize++;
        }
        else
        {
            /* the access is memory access */
           tmpAccessParamsBmp |= (1 << 24);

           for (ii = 0; ii < length; ii++)
           {
               (*dbPtrPtr)[curDbIndex].addr = addr;
               (*dbPtrPtr)[curDbIndex].data = dataPtr[ii];
               (*dbPtrPtr)[curDbIndex].accessParamsBmp = tmpAccessParamsBmp;


               if (ii == (length - 1))
               {
                   /* Set the end bit of the memory (bit 25) */
                   (*dbPtrPtr)[curDbIndex].accessParamsBmp |= (1 << 25);
               }

               /* Increment Address */
               addr = addr + 4;
               /* move cursor to point to the next DB entry */
               curDbIndex++;
           }

           /* Update size of HW Access DB, the curSize points to the next DB entry */
           infoDbPtr->curSize = curDbIndex;
        }
    }


    /* check if we are in task context */
    if (isrContext == GT_FALSE)
    {
        /* task context, unlock ISR */
        osSetIntLockUnlock(INTR_MODE_UNLOCK, &key);
    }

    return GT_OK;
}

/* trace of ISR only */
GT_U32  appDemoTraceIsrOnly = 0;

/******************************************************************************
* appDemoTraceIsrOnlySet
*
* DESCRIPTION:
*       Enable tracing of ISR only.
*
* INPUTS:
*       value      - 0 - enable trace for all
*                    1 - enable trace for ISR only
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       old state of ISR only trace.
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_U32 appDemoTraceIsrOnlySet
(
    IN GT_U32 value
)
{
    GT_U32 oldvalue = appDemoTraceIsrOnly; /* old value */

    appDemoTraceIsrOnly = value;
    return oldvalue;
}

/******************************************************************************
* appDemoTraceHwAccessAction
*
* DESCRIPTION:
*       Trace HW Access action: print or store.
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - ports group number
*       isrContext  - GT_TRUE: called from ISR context (cpssDrvPpHwIsrWrite,
*                                                       cpssDrvPpHwIsrRead)
*                    GT_FALSE: called from not ISR context.
*       addrSpace   - memory address space
*       addr        - start address that the access was made to
*       length      - length of the data that was written in words
*       dataPtr     - (pointer to) data that was written
*       outputMode  - output mode: print, synchronious print and store
*       accessType  - access type: read or write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS appDemoTraceHwAccessAction
(
    IN GT_U8                                devNum,
    IN GT_U32                               portGroupId,
    IN GT_BOOL                              isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT         addrSpace,
    IN GT_U32                               addr,
    IN GT_U32                               length,
    IN GT_U32                               *dataPtr,
    IN APP_DEMO_TRACE_OUTPUT_MODE_ENT       outputMode,
    IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT    accessType
)
{
    GT_STATUS                           rc;
    FUNCP_TRACE_PRINTF_DATA             printFuncPtr;

    #ifdef ASIC_SIMULATION
    FILE  *filePtr;
    GT_CHAR fileNameString[30] = {0};
    GT_CHAR *fileNamePtr;
    GT_CHAR buffer[APP_DEMO_FILE_MAX_LINE_LENGTH_CNS] = {0};
    GT_U32 i = 0;
    #endif
    if (appDemoTraceIsrOnly && (isrContext == GT_FALSE))
    {
        return GT_OK;
    }

    /* switch output mode */
    switch (outputMode)
    {
        case APP_DEMO_TRACE_OUTPUT_MODE_DIRECT_E:
        case APP_DEMO_TRACE_OUTPUT_MODE_DIRECT_SYNC_E:
            /* print the data */

            printFuncPtr = (outputMode == APP_DEMO_TRACE_OUTPUT_MODE_DIRECT_E) ?
                osPrintf : osPrintSync;

            if ((accessType==APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_DELAY_E) || ((addr == 0) && (accessType == APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E)))
            {
                printFuncPtr("DL");
                printFuncPtr("  ");
                printFuncPtr("%d", devNum);
                printFuncPtr("  ");
                printFuncPtr("%d", portGroupId);
                printFuncPtr(" ");
                printFuncPtr("\t\t\t\t%8d", *dataPtr);

            }
            else
            {
                if (accessType == APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E)
                {
                    printFuncPtr("RD");
                }
                else
                {
                    printFuncPtr("WR");
                }
                /* print devNum and portGroup */

                printFuncPtr("  ");
                printFuncPtr("%d", devNum);
                printFuncPtr("  ");
                printFuncPtr("%d", portGroupId);
                printFuncPtr("  ");

                if (isrContext)
                {
                    printFuncPtr("ISR");
                }
                else
                {
                    printFuncPtr("TSK");
                }
                printFuncPtr("  ");

                if (CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PCI_PEX_E == addrSpace)
                {
                    printFuncPtr("PEX");
                }
                if (CPSS_TRACE_HW_ACCESS_ADDR_SPACE_RESET_AND_INIT_CTRL_E == addrSpace)
                {
                    printFuncPtr("DFX");
                }
                else
                {
                    printFuncPtr("REG");
                }
                printFuncPtr("  ");
                /* print the data */
                appDemoTracePrintHwData(addr, length, dataPtr, printFuncPtr);

            }


            printFuncPtr("\n");

            break;

        case APP_DEMO_TRACE_OUTPUT_MODE_DB_E:
            /* store the data */

            rc = appDemoTraceStoreHwData(devNum,
                                        portGroupId,
                                        isrContext,
                                        addrSpace,
                                        addr,
                                        length,
                                        dataPtr,
                                        accessType,
                                         &appDemoAccessDbPtr,
                                        &appDemoAccessInfo);

            if (rc != GT_OK)
            {
                return rc;
            }
            break;
        case APP_DEMO_TRACE_OUTPUT_MODE_FILE_E:
            #ifdef ASIC_SIMULATION
                /* open trace data files for each core */
                if (devCoreTraceFilePtr[devNum][portGroupId] == NULL)
                {
                    sprintf(fileNameString, "device%u_asic%u_mem_dump.txt", devNum,portGroupId+1);
                    fileNamePtr = fileNameString;
                    devCoreTraceFilePtr[devNum][portGroupId] = fopen(fileNamePtr, "w");
                    if (devCoreTraceFilePtr[devNum][portGroupId] == NULL)
                    {
                        return GT_FAIL;
                    }
                }
                filePtr = devCoreTraceFilePtr[devNum][portGroupId];
                for (i = 0; i < length; i++)
                {
                    sprintf(buffer, "%08x %08x\n", addr, dataPtr[i]);
                    fprintf(filePtr, buffer);
                    addr += 4;
                    osMemSet(buffer,0,sizeof(buffer));
                }

            #else
               return GT_NOT_SUPPORTED;
            #endif
            break;
        default:
            return GT_BAD_STATE;
    }

    return GT_OK;
}
#if (!defined(SHARED_MEMORY) || defined(LINUX_SIM))
/******************************************************************************
* appDemoTraceWriteHwAccess
*
* DESCRIPTION:
*       Trace HW write access information.
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - ports group number
*       isrContext  - GT_TRUE: called from ISR context (cpssDrvPpHwIsrWrite,
*                                                       cpssDrvPpHwIsrRead)
*                    GT_FALSE: called from not ISR context.
*       addrSpace   - memory address space
*       addr        - start address that the access was made to
*       length      - length of the data that was written in words
*       dataPtr     - (pointer to) data that was written
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on fail
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS appDemoTraceWriteHwAccess
(
    IN GT_U8       devNum,
    IN GT_U32      portGroupId,
    IN GT_BOOL     isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT         addrSpace,
    IN GT_U32      addr,
    IN GT_U32      length,
    IN GT_U32      *dataPtr
)
{
    return appDemoTraceHwAccessAction(devNum,
                                      portGroupId,
                                      isrContext,
                                      addrSpace,
                                      addr,
                                      length,
                                      dataPtr,
                                      appDemoTraceOutputMode,
                                      APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E);
}

/******************************************************************************
* appDemoTraceReadHwAccess
*
* DESCRIPTION:
*       Trace HW read access information.
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - ports group number
*       isrContext  - GT_TRUE: called from ISR context (cpssDrvPpHwIsrWrite,
*                                                       cpssDrvPpHwIsrRead)
*                    GT_FALSE: called from not ISR context.
*       addrSpace   - memory address space
*       addr        - start address that the access was made to
*       length      - length of the data that was written in words
*       dataPtr     - (pointer to) data that was read
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on fail
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS appDemoTraceReadHwAccess
(
    IN GT_U8       devNum,
    IN GT_U32      portGroupId,
    IN GT_BOOL     isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT         addrSpace,
    IN GT_U32      addr,
    IN GT_U32      length,
    IN GT_U32      *dataPtr
)
{
    return appDemoTraceHwAccessAction(devNum,
                                      portGroupId,
                                      isrContext,
                                      addrSpace,
                                      addr,
                                      length,
                                      dataPtr,
                                      appDemoTraceOutputMode,
                                      APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E);
}


/******************************************************************************
* appDemoTraceDelayHwAccess
*
* DESCRIPTION:
*       Trace HW write access information.
*
* INPUTS:
*       devNum      - PP device number
*       portGroupId - ports group number
*       millisec   -  the delay in millisec
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on fail
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS appDemoTraceDelayHwAccess
(
       IN GT_U8       devNum,
       IN GT_U32      portGroupId,
       IN GT_U32      millisec
)
{
    return appDemoTraceHwAccessAction(devNum,
                                      ((portGroupId==0xFFFFFFFF)?0:portGroupId),
                                      GT_FALSE,
                                      GT_FALSE,
                                      0,
                                      1,
                                      &millisec,
                                      appDemoTraceOutputMode,
                                      APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_DELAY_E);
}
#endif
/******************************************************************************
* appDemoSetHwAccessDbActiveState
*
* DESCRIPTION:
*       Update HW Access Read/Write DB state.
*
* INPUTS:
*       enable      - new DB state
*                     GT_TRUE: enable tracing for given DB
*                     GT_FALSE: disable tracing for given DB
*       dbPtr       - (pointer to) read/write HW Access DB
*       dbInfoPtr   - (pointer to) read/write HW Access info
*
* OUTPUTS:
*       dbPtr       - (pointer to) read/write HW Access DB
*       dbInfoPtr   - (pointer to) read/write HW Access info
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad access type
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS appDemoSetHwAccessDbActiveState
(
    IN            GT_BOOL                                                                         active,
    INOUT   APP_DEMO_CPSS_HW_ACCESS_DB_STC      **dbPtrPtr,
    INOUT   APP_DEMO_CPSS_HW_ACCESS_INFO_STC    *dbInfoPtr
)
{
    if (active == GT_TRUE)
    {
        /* Enable Tracing */

        if (dbInfoPtr->outputToDbIsActive == GT_FALSE)
        {
            if (*dbPtrPtr == NULL)
            {
                /* Initialize HW Access DB for the first time */
                *dbPtrPtr = osMalloc(APP_DEMO_CPSS_HW_ACCESS_DB_STEP_SIZE_CNS *
                                            sizeof(APP_DEMO_CPSS_HW_ACCESS_DB_STC));
                if (*dbPtrPtr == NULL)
                {
                    return GT_OUT_OF_CPU_MEM;
                }

                dbInfoPtr->maxSize = APP_DEMO_CPSS_HW_ACCESS_DB_STEP_SIZE_CNS;

                    /* The Access DB was already initialized.
                    Move cursor to point to the first access entry */
                dbInfoPtr->curSize = 0;
                dbInfoPtr->corrupted = GT_FALSE;
            }

       /* enable HW Access DB */
            dbInfoPtr->outputToDbIsActive = GT_TRUE;


        }
        else
            /* The HW Access DB is already initialized */
            return GT_OK;
    }
    else
    {
        /* Disable Tracing */

        if (dbInfoPtr->outputToDbIsActive == GT_TRUE)
        {
             if ((traceReadHwAccess == GT_FALSE) &&(traceWriteHwAccess == GT_FALSE))
             {
                /* disable HW Access DB */
                dbInfoPtr->outputToDbIsActive = GT_FALSE;
              }
             else
            {
                 return GT_BAD_STATE;
            }
        }
        else
            /* The HW Access DB is already disabled */
            return GT_OK;
    }

    return GT_OK;
}
#ifdef ASIC_SIMULATION
/******************************************************************************
* prvAppDemoTraceHwFilesClose
*
* DESCRIPTION:
*       Close all Trace HW files.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on file close error
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS prvAppDemoTraceHwFilesClose
(
    GT_VOID
)
{
    int rc = 0; /* fclose return */
    GT_U32 i,j;
    /* Mode is going to be changed. Close all open files */
    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        for (j = 0; j < APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS; j++)
        {
            if (devCoreTraceFilePtr[i][j] != NULL)
            {
                rc = fclose(devCoreTraceFilePtr[i][j]);
                devCoreTraceFilePtr[i][j] = NULL;
                if (rc != 0)
                {
                    return GT_FAIL;
                }
            }
        }
    }
    return GT_OK;
}
#endif
/******************************************************************************
* prvAppDemoTraceHwAccessEnable
*
* DESCRIPTION:
*       Trace HW read access information.
*
* INPUTS:
*       devNum      - PP device number
*       accessType  - access type: read or write
*       enable      - GT_TRUE: enable tracing for given access type
*                     GT_FALSE: disable tracing for given access type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad access type
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS prvAppDemoTraceHwAccessEnable
(
    IN GT_U8                devNum,
    IN GT_U32               accessType,
    IN GT_BOOL              enable
)
{
    return appDemoTraceHwAccessEnable(devNum,
                                      (APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT)accessType,
                                      enable);
}

/******************************************************************************
* appDemoTraceHwAccessEnable
*
* DESCRIPTION:
*       Trace HW read access information.
*
* INPUTS:
*       devNum      - PP device number
*       accessType  - access type: read or write
*       enable      - GT_TRUE: enable tracing for given access type
*                     GT_FALSE: disable tracing for given access type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad access type
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoTraceHwAccessEnable
(
    IN GT_U8                                devNum,
    IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT    accessType,
    IN GT_BOOL                              enable
)
{
    GT_STATUS                   rc;
    CPSS_DRV_HW_TRACE_TYPE_ENT  cpssDrvType;
    switch (accessType)
    {
        case APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E:
            /* Read HW Access */
            traceReadHwAccess = enable;

            if (appDemoTraceOutputMode == APP_DEMO_TRACE_OUTPUT_MODE_DB_E)
            {
                /* update Read HW Access DB */
                rc = appDemoSetHwAccessDbActiveState(enable,
                                                  &appDemoAccessDbPtr,
                                                  &appDemoAccessInfo);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
            cpssDrvType = CPSS_DRV_HW_TRACE_TYPE_READ_E;


            break;
            /* No break*/
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[unterminated_case] */
        case APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_DELAY_E:
            traceDelayHwAccess = enable;

        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[fallthrough] */
        case APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E:
            /* Write HW Access */
             traceWriteHwAccess = enable;

            if (appDemoTraceOutputMode == APP_DEMO_TRACE_OUTPUT_MODE_DB_E)
            {
                /* update Write HW Access DB */
                rc = appDemoSetHwAccessDbActiveState(enable,
                                                  &appDemoAccessDbPtr,
                                                  &appDemoAccessInfo);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }


            if (accessType==APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_DELAY_E)
            {
                cpssDrvType = CPSS_DRV_HW_TRACE_TYPE_WRITE_DELAY_E;
            }
            else
            {
                cpssDrvType = CPSS_DRV_HW_TRACE_TYPE_WRITE_E;
            }

            if (enable==GT_FALSE) /*Delay access stops with write access*/
            {
                traceDelayHwAccess=GT_FALSE;
            }

            break;

        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[unterminated_case] */
        case APP_DEMO_TRACE_HW_ACCESS_TYPE_ALL_E: /* No break*/
            traceDelayHwAccess=enable;

        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[fallthrough] */
        case APP_DEMO_TRACE_HW_ACCESS_TYPE_BOTH_E:
            /* Read and Write HW Access */
             traceWriteHwAccess = enable;
            traceReadHwAccess = enable;

            if (appDemoTraceOutputMode == APP_DEMO_TRACE_OUTPUT_MODE_DB_E)
            {
                /* update Read HW Access DB */
                rc = appDemoSetHwAccessDbActiveState(enable,
                                                  &appDemoAccessDbPtr,
                                                  &appDemoAccessInfo);
                if (rc != GT_OK)
                {
                    return rc;
                }


            }


            if (accessType==APP_DEMO_TRACE_HW_ACCESS_TYPE_ALL_E)
            {
                cpssDrvType = CPSS_DRV_HW_TRACE_TYPE_ALL_E;
            }
            else
            {
                cpssDrvType = CPSS_DRV_HW_TRACE_TYPE_BOTH_E;
            }

            if (enable==GT_FALSE) /*Delay access stops with write access*/
            {
                traceDelayHwAccess=GT_FALSE;
            }


            break;

        default:
            return GT_BAD_PARAM;
    }
    if (enable == GT_FALSE)
    {
        /* trace hw access was disabled */
        #ifdef ASIC_SIMULATION
        if (appDemoTraceOutputMode == APP_DEMO_TRACE_OUTPUT_MODE_FILE_E)
        {
            rc = prvAppDemoTraceHwFilesClose();
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        #endif
    }
    return cpssDrvPpHwTraceEnable(devNum, cpssDrvType, enable);
}


/******************************************************************************
* prvAppDemoTraceHwAccessOutputModeSet
*
* DESCRIPTION:
*       Set output tracing mode.
*
* INPUTS:
*       mode        - output tracing mode: print, printSync or store.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_STATE - on bad state
*       GT_BAD_PARAM - on bad mode
*
* COMMENTS:
*       It is not allowed to change mode, while one of the HW Access DB
*       is enabled.
*
******************************************************************************/
static GT_STATUS prvAppDemoTraceHwAccessOutputModeSet
(
    IN GT_U32   mode
)
{
    return appDemoTraceHwAccessOutputModeSet((APP_DEMO_TRACE_OUTPUT_MODE_ENT)mode);
}
/******************************************************************************
* appDemoTraceHwAccessOutputModeSet
*
* DESCRIPTION:
*       Set output tracing mode.
*
* INPUTS:
*       mode        - output tracing mode: print, printSync or store.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_STATE - on bad state
*       GT_BAD_PARAM - on bad mode
*
* COMMENTS:
*       It is not allowed to change mode, while one of the HW Access DB
*       is enabled.
*
******************************************************************************/
GT_STATUS appDemoTraceHwAccessOutputModeSet
(
    IN APP_DEMO_TRACE_OUTPUT_MODE_ENT   mode
)
{
    GT_STATUS   rc;

    if (mode >= APP_DEMO_TRACE_OUTPUT_MODE_LAST_E)
    {
        return GT_BAD_PARAM;
    }

    if (appDemoTraceOutputMode == mode)
    {
        /* this mode is already set */
        return GT_OK;
    }

    if (appDemoAccessInfo.outputToDbIsActive == GT_TRUE)
    {
        /* check that  DB is enabled */
        return GT_BAD_STATE;
    }

    if (mode == APP_DEMO_TRACE_OUTPUT_MODE_DB_E)
    {
        if ((traceReadHwAccess == GT_TRUE) ||(traceWriteHwAccess == GT_TRUE))
        {
            /* update HW Access DB */
            rc = appDemoSetHwAccessDbActiveState(GT_TRUE,
                                              &appDemoAccessDbPtr,
                                              &appDemoAccessInfo);
            if (rc != GT_OK)
            {
                return rc;
            }
        }


    }
    if (appDemoTraceOutputMode == APP_DEMO_TRACE_OUTPUT_MODE_FILE_E)
    {
        #ifdef ASIC_SIMULATION
        rc = prvAppDemoTraceHwFilesClose();
        if (rc != GT_OK)
        {
            return rc;
        }
        #else
          return GT_NOT_SUPPORTED;
        #endif
    }
    appDemoTraceOutputMode = mode;

    return GT_OK;
}

/******************************************************************************
* appDemoTraceHwAccessDbDump
*
* DESCRIPTION:
*       Dump HW Access DB.
*
* INPUTS:
*       accessType - the type of DB to dump: read or write.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoTraceHwAccessDbDump
(
    IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT accessType
)
{
    APP_DEMO_CPSS_HW_ACCESS_INFO_STC    infoDb;         /* DB information */
    APP_DEMO_CPSS_HW_ACCESS_DB_STC      *accessDbPtr;   /* hw access DB */
    GT_U32                              portGroupId = 0;    /* port group ID */
    GT_BOOL                             isrContext = GT_FALSE;     /* flag for ISR context */
    CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT addrSpace = CPSS_TRACE_HW_ACCESS_ADDR_SPACE_PP_E; /* flag for PCI */
    GT_U8                               devNum = 0;         /* device number */
    GT_U32                              addr = 0;           /* hw address */
    GT_U32                              length = 0;         /* length of the data */
    GT_U32                              dataArr[100];   /* array to store data */
    GT_U32                              ii;             /* loop iterator */
    GT_BOOL                             isMemNextWord = GT_FALSE;
    /* flag for next word in the memory */
    GT_BOOL                             dumpDataAndInfo = GT_FALSE;
    /* flag to dump data */
    GT_STATUS                           rc;             /* return code */
    APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT entryAccessType = APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E;  /*set default due to compilation warning*/


    switch (accessType)
    {
    case APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E:
    case APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E:
    case APP_DEMO_TRACE_HW_ACCESS_TYPE_BOTH_E:
        infoDb = appDemoAccessInfo;
        accessDbPtr = appDemoAccessDbPtr;
        break;

    default:
        return GT_BAD_PARAM;
    }

    if (infoDb.curSize == 0)
    {
        /* There is no Data in this DB */
        return GT_OK;
    }

    if(infoDb.corrupted == GT_TRUE)
    {
        osPrintf("\n\n\nWarning: The register access DB  has reached the limit  and loging has been stopped\n\n\n");
    }

    for (ii = 0; ii < infoDb.curSize; ii++)
    {
        /* parse the data from DB entry */

        /* check if the current word is not memory continuation */
        if (isMemNextWord)
        {
            /* we are still in the memory context */

            dataArr[length] = accessDbPtr[ii].data;
            length++;

            /* check if it is the last word of the current memory entry, bit25 */
            if ((accessDbPtr[ii].accessParamsBmp >> 25) & 0x1)
            {
                isMemNextWord = GT_FALSE;
                /* the data should be printed */
                dumpDataAndInfo = GT_TRUE;
            }
        }
        else
        {
            /* Register access or start of memory context */
            if(BIT2BOOL_MAC((accessDbPtr[ii].accessParamsBmp >> 26) & 0x1)  )
            {
                entryAccessType = APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E;
            }
            else
            {
                entryAccessType = APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E;
            }

            /*dump only required access type*/
            if((accessType == APP_DEMO_TRACE_HW_ACCESS_TYPE_BOTH_E) ||(entryAccessType == accessType))
            {

                /* devNum */
                devNum = (accessDbPtr[ii].accessParamsBmp & 0xFF);
                /* portGroupId */
                portGroupId = ((accessDbPtr[ii].accessParamsBmp >> 8) & 0xFF);
                /* isrContext */
                isrContext = BIT2BOOL_MAC((accessDbPtr[ii].accessParamsBmp >> 16) & 0x1);
                /* addrSpace */
                addrSpace = ((accessDbPtr[ii].accessParamsBmp >> 17) & 0x3);



                /* check if the data is for register access or memory access */
                if ((accessDbPtr[ii].accessParamsBmp >> 24) & 0x1)
                {
                    /* memory access */

                    /* set the first word data memory */
                    addr = accessDbPtr[ii].addr;
                    length = 0;
                    dataArr[length] = accessDbPtr[ii].data;
                    isMemNextWord = GT_TRUE;
                    length++;
                }
                else
                {
                    /* register access */
                    length = 1;
                    addr = accessDbPtr[ii].addr;
                    dataArr[0] = accessDbPtr[ii].data;
                    /* the data should be printed */
                    dumpDataAndInfo = GT_TRUE;
                }/* register access */
            }

        }/* register access or start of memory context */

        if (dumpDataAndInfo == GT_TRUE)
        {
            /* dump info and data */
            rc = appDemoTraceHwAccessAction(devNum,
                portGroupId,
                isrContext,
                addrSpace,
                addr,
                length,
                dataArr,
                APP_DEMO_TRACE_OUTPUT_MODE_DIRECT_E,
                entryAccessType);
            if (rc != GT_OK)
            {
                return rc;
            }

            dumpDataAndInfo = GT_FALSE;
        }
    } /* for */

    return GT_OK;
}

/******************************************************************************
* appDemoTraceHwAccessInfoCompare
*
* DESCRIPTION:
*       Compare given data and parameters with trace hw access DB info on
*       specified index.
*
* INPUTS:
*       dev         - device number
*       accessType  - access type: read or write
*       index       - index in the hw access db
*       portGroupId - port group ID
*       isrContext  - GT_TRUE: ISR contextt
*                     GT_FALSE: TASK context
*       addrSpace   - memory address space
*       addrPtr     - addresses to compare
*       mask        - mask to mask the data from the db
*       dataPtr     - data (read/written) to compare
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - when there is no match between db info and input
*                      parameters.
*       GT_BAD_STATE - on unexpected ISR context
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoTraceHwAccessInfoCompare
(
    IN GT_U8                            dev,
    APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT   accessType,
    IN GT_U32                           index,
    IN GT_U32                           portGroupId,
    IN GT_BOOL                          isrContext,
    IN CPSS_TRACE_HW_ACCESS_ADDR_SPACE_ENT addrSpace,
    IN GT_U32                           addr,
    IN GT_U32                           mask,
    IN GT_U32                           data
)
{
    APP_DEMO_CPSS_HW_ACCESS_DB_STC      *accessDbPtr;
    APP_DEMO_CPSS_HW_ACCESS_DB_STC      accessDbEntry;
    GT_U8   writeBit;


    switch (accessType)
    {
        case APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E:
        case APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E:
            accessDbPtr = appDemoAccessDbPtr;
            break;
        default:
            return GT_BAD_PARAM;
    }

    writeBit = (accessType == APP_DEMO_TRACE_HW_ACCESS_TYPE_READ_E)?0x0:0x1;

    accessDbEntry = accessDbPtr[index];

    /* check if the expected context is TASK and it is same as in DB,
       if not skip, this may be not expected ISR context */
    if (isrContext == GT_FALSE)
    {
        /* expected context is TASK */
        if ((BIT2BOOL_MAC((accessDbEntry.accessParamsBmp >> 16) & 0x1)) != isrContext)
        {
            /* skip */
            return GT_BAD_STATE;
        }
    }

    /* parse the data from DB entry and compare to input data and parameters */
    if ((accessDbEntry.addr != addr) || /* hw address */
        ((accessDbEntry.data & mask) != data) || /* hw data */
        ((accessDbEntry.accessParamsBmp & 0xFF) != dev) || /* devNum */
        (((accessDbEntry.accessParamsBmp >> 8) & 0xFF) != portGroupId) || /* portGroupId */
        ((BIT2BOOL_MAC((accessDbEntry.accessParamsBmp >> 16) & 0x1)) != isrContext) || /* isrContext */
        (((accessDbEntry.accessParamsBmp >> 17) & 0x3) != addrSpace)|| /* addrSpace */
        (((accessDbEntry.accessParamsBmp >> 26) & 0x1) != writeBit))/*read or write*/
    {
        /* there is no match between parameters or data */
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

typedef enum {
    APP_DEMO_DESCR_FIELD_TYPE_DEC_E,
    APP_DEMO_DESCR_FIELD_TYPE_HEX_E,
    APP_DEMO_DESCR_FIELD_TYPE_MAC_E,
    APP_DEMO_DESCR_FIELD_TYPE_IPV6_E,
    APP_DEMO_DESCR_FIELD_TYPE_IPV4_E,
    APP_DEMO_DESCR_FIELD_TYPE_HEX32_E
}APP_DEMO_DESCR_FIELD_TYPE_ENT;

static GT_BOOL appDemoDescrDumpRawDumpEn = GT_FALSE;

void appDemoDescrDumpRawDumpEnable
(
    IN GT_BOOL enable
)
{
    appDemoDescrDumpRawDumpEn = enable;
}

#ifdef CHX_FAMILY

static GT_STATUS appDemoDxChDescrDump
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT descriptorType,
    IN  GT_BOOL                           dumpAll
)
{
    GT_STATUS rc;
    GT_U32 i;  /* iterator */
    CPSS_DXCH_DIAG_DESCRIPTOR_TYPE_ENT descr;  /* iterator */
    GT_CHAR unitName[50];    /* unit name */
    /* pointer to descriptor information */
    CPSS_DXCH_DIAG_DESCRIPTOR_STC *descriptorPtr = NULL;
    APP_DEMO_DESCR_FIELD_TYPE_ENT fieldType; /* field type */
    GT_U32  *memoPtr;   /* memory pointer for unknown format */
    GT_U32   memoSize;  /* size of descriptor in bytes       */

    if(dumpAll == GT_FALSE)
    {
        descr = descriptorType;
    }
    else
    {
        descr = 0;
    }

    descriptorPtr = (CPSS_DXCH_DIAG_DESCRIPTOR_STC*)
                     cpssOsMalloc(sizeof(CPSS_DXCH_DIAG_DESCRIPTOR_STC));

    for(; descr < CPSS_DXCH_DIAG_DESCRIPTOR_LAST_E; descr++)
    {

        switch(descr)
        {
            case CPSS_DXCH_DIAG_DESCRIPTOR_WR_DMA_TO_TTI_E:
                cpssOsStrCpy(unitName, "WR_DMA_TO_TTI");
                break;

            case CPSS_DXCH_DIAG_DESCRIPTOR_TTI_TO_PCL_E:
                cpssOsStrCpy(unitName, "TTI_TO_PCL");
                break;

            case CPSS_DXCH_DIAG_DESCRIPTOR_PCL_TO_BRIDGE_E:
                cpssOsStrCpy(unitName, "PCL_TO_BRIDGE");
                break;

            case CPSS_DXCH_DIAG_DESCRIPTOR_BRIDGE_TO_ROUTER_E:
                cpssOsStrCpy(unitName, "BRIDGE_TO_ROUTER");
                break;

            case CPSS_DXCH_DIAG_DESCRIPTOR_ROUTER_TO_INGRESS_POLICER_E:
                /* invalid descriptor for xCat and xCat2*/
                if(((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
                    (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E) ||
                    (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))&&
                   (dumpAll == GT_TRUE))
                {
                    continue;
                }
                cpssOsStrCpy(unitName, "ROUTER_TO_INGRESS_POLICER_E");
                break;

            case CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER0_TO_INGRESS_POLICER1_E:
                cpssOsStrCpy(unitName, "POLICER0_TO_INGRESS_POLICER1");
                break;

            case CPSS_DXCH_DIAG_DESCRIPTOR_INGRESS_POLICER1_TO_PRE_EGRESS_E:
                cpssOsStrCpy(unitName, "INGRESS_POLICER1_TO_PRE_EGRESS");
                break;

            case CPSS_DXCH_DIAG_DESCRIPTOR_PRE_EGRESS_TO_TXQ_E:
                cpssOsStrCpy(unitName, "PRE_EGRESS_TO_TXQ");
                break;

            case CPSS_DXCH_DIAG_DESCRIPTOR_TXQ_TO_HEADER_ALTERATION_E:
                cpssOsStrCpy(unitName, "TXQ_TO_HEADER_ALTERATION");
                break;

            case CPSS_DXCH_DIAG_DESCRIPTOR_EGRESS_POLICER_TO_EGRESS_PCL_E:
                cpssOsStrCpy(unitName, "EGRESS_POLICER_TO_EGRESS_PCL");
                break;

            default:
                cpssOsFree(descriptorPtr);
                return GT_BAD_PARAM;
        }

        /* Print in Raw Format */
        if (appDemoDescrDumpRawDumpEn != GT_FALSE)
        {
            memoSize = sizeof(CPSS_DXCH_DIAG_DESCRIPTOR_STC);
            memoPtr  = (GT_U32*)descriptorPtr;
            rc = cpssDxChDiagDescriptorPortGroupRawGet(
                devNum, portGroupsBmp, descr,
                &memoSize, memoPtr);
            if (rc != GT_OK)
            {
                cpssOsFree(descriptorPtr);
                return rc;
            }
            for (i = 0; (i < ((memoSize + 3) / 4)); i++)
            {
                if ((i % 8) == 0)
                {
                    cpssOsPrintf("%04X: ", (i * 4));
                }
                cpssOsPrintf("%08X ", memoPtr[i]);
                if ((i % 8) == 3)
                {
                    cpssOsPrintf("   ");
                }
                if ((i % 8) == 7)
                {
                    cpssOsPrintf("\n");
                }
            }
            cpssOsPrintf("\n");
        }

        rc = cpssDxChDiagDescriptorPortGroupGet(devNum, portGroupsBmp,
                                            descr, descriptorPtr);
        if(rc!= GT_OK)
        {
            cpssOsFree(descriptorPtr);
            return rc;
        }

        for(i = 0; i < CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_LAST_E; i++)
        {
            if(descriptorPtr->fieldValueValid[i] == GT_TRUE)
            {
                switch (i)
                {
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_ETHERTYPE_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB0_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB10_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB11_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB12_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB13_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB14_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB15_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB16_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB17_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB18_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB19_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB1_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB20_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB21_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB22_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB2_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB3_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB4_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB5_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB6_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB7_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB8_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB9_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_UDB_BYTESVALID_E:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_HEX_E ;
                        break;
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MACDA_0_31_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_MACSA_0_31_E:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_MAC_E ;
                        break;
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DIP_0_31_E:
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_SIP_0_31_E:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_IPV6_E ;
                        break;
                    case CPSS_DXCH_DIAG_DESCRIPTOR_FIELD_TYPE_DIP_E:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_IPV4_E ;
                        break;
                    default:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_DEC_E;
                        break;
                }

                switch(fieldType)
                {
                    case APP_DEMO_DESCR_FIELD_TYPE_HEX_E:
                        cpssOsPrintf("%s; %s; 0x%X;\n", unitName, FIELDS_ARRAY[i],
                                     descriptorPtr->fieldValue[i]);
                        break;
                    case APP_DEMO_DESCR_FIELD_TYPE_MAC_E:
                        cpssOsPrintf("%s; %s; %02X:%02X:%02X:%02X:%02X:%02X;\n", unitName, FIELDS_ARRAY[i],
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+1],8,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+1],0,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],24,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],16,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],8,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],0,8));
                        /* the MAC use two fields, skip second one */
                        i++;
                        break;
                    case APP_DEMO_DESCR_FIELD_TYPE_IPV6_E:
                        cpssOsPrintf("%s; %s; %X:%X:%X:%X:%X:%X:%X:%X;\n", unitName, FIELDS_ARRAY[i],
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+3],16,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+3],0,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+2],16,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+2],0,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+1],16,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+1],0,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],16,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],0,16));
                        /* the IPV6 use four fields, skip three one */
                        i += 3;
                        break;
                    case APP_DEMO_DESCR_FIELD_TYPE_IPV4_E:
                        cpssOsPrintf("%s; %s; %d.%d.%d.%d;\n", unitName, FIELDS_ARRAY[i],
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],24,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],16,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],8,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],0,8));
                        break;
                    default:
                        cpssOsPrintf("%s; %s; %d;\n", unitName, FIELDS_ARRAY[i],
                                     descriptorPtr->fieldValue[i]);
                        break;
                }

            }
        }
        if(dumpAll == GT_FALSE)
        {
            break;
        }
        /* separate descriptors */
        cpssOsPrintf("\n\n");
    }

    cpssOsFree(descriptorPtr);
    return GT_OK;

}

#elif defined EXMXPM_FAMILY

static GT_CHAR* exmxpm_field_strings[] =
{
    "AppSpecificCPUCode",
    "ARPBCToCPUEnable",
    "ARPPointer",
    "ARPPtr",
    "ARP_Pointer",
    "AutoDSATag",
    "AutoLearnEnable",
    "Bank",
    "BCUDPRelayEnable",
    "BillingEn",
    "BypassIngressPipe",
    "ByteCount",
    "CFI1",
    "CFI",
    "CFMPkt",
    "CongestedPortNumber",
    "CongestedQueueNumber",
    "ControlTC",
    "CopyDSCPEXPToInner",
    "CopyTTL",
    "CPU2CPUMirror",
    "CPUCode",
    "CPUMailToNeigborCPU",
    "CpuTrgDev",
    "CpuTrgPort",
    "DecTTL",
    "DIP[127:0] bits 0..31",
    "DIP[127:0] bits 32..63",
    "DIP[127:0] bits 64..95",
    "DIP[127:0] bits 96..127",
    "DIP",
    "DoHA",
    "DontFragment",
    "DoRouteHA",
    "DoRoute",
    "DoSnifferHA",
    "DP_Dup",
    "DP",
    "DropCode",
    "DropOnSource",
    "DropPacket",
    "DSCP",
    "EgressFilterEn",
    "EgressFilterRegistered",
    "EnableARPBCToME",
    "EnableBridgeLogFlow",
    "EnableBridge",
    "EnableBRI",
    "EnableDsaTagManipulation",
    "EnableIPv4IcmpRedirect",
    "EnableIPv6IcmpRedirect",
    "EnableIPv6NeighborSolicitation",
    "EnableMPLSRouting",
    "EnableNAStormPrevention",
    "EnableRouterLogFlow",
    "EnableSourceLocalFiltering_Dup",
    "EnableSourceLocalFiltering",
    "Enable_IPCL0_lookup",
    "Enable_Policy",
    "Ethertype",
    "ExcludedDevIsLocal",
    "ExcludedPort",
    "ExcludedTrunk",
    "ExcludeInputPort",
    "ExcludeIsTrunk",
    "EXP",
    "ExtendedDsaBridgeBypass_Dup",
    "ExtendedDsaBridgeBypass",
    "FabricDP",
    "FabricTC",
    "FabricToNetwork",
    "FabricVIDX",
    "fabric_desc bits 0..31",
    "fabric_desc bits 128..159",
    "fabric_desc bits 160..191",
    "fabric_desc bits 32..63",
    "fabric_desc bits 64..95",
    "fabric_desc bits 96..127",
    "FirstBuffer",
    "FloodMLLPointer",
    "FlowID",
    "FlowSubTemplate",
    "Force_IPCL1_configurations",
    "Fragmented",
    "FromCPUDP",
    "FromCPUTC",
    "IGMPNonQuery",
    "IGMPQuery",
    "InLIF_ID",
    "InLIF_Mode",
    "InnerIsLLCNonSnap",
    "InnerL3Offset",
    "InnerPktType",
    "InputEncap",
    "IPActionEntry",
    "IpICMPType",
    "IPLegal",
    "IPLookupHit",
    "IPMgmntEn",
    "IPModifyDSCP",
    "IPModifyUP",
    "IPM",
    "IPQoSProfile",
    "IPSecurityProfile",
    "IPTunnelCmd",
    "IPv4IGMPToCPUEn",
    "IPv4LinkLocalMCCmdEnable",
    "IPv4MulticastRouteEnable",
    "IPv4OptionField",
    "IPv4RIPv1",
    "IPv4UnicastRouteEnable",
    "IPv4_6_ICMP",
    "IPv4_Bad_GRE_Header",
    "IPv6FlowLabel",
    "IPv6HBHExt",
    "IPv6ICMPToCPUEn",
    "IPv6LinkLocalMCCmdEnable",
    "IPv6MulticastRouteEnable",
    "IPv6NonHBHExt",
    "IPv6UnicastRouteEnable",
    "IPv6_EH",
    "IPxHeaderTotalLength",
    "IPxMACMcMismatch",
    "IPxMcLinkLocalProt",
    "IPxProtocol",
    "IP_Fragmented",
    "IP_Header_Info",
    "IP_Protocol",
    "IP_Total_length",
    "IsARPReply",
    "IsARP",
    "IsBC",
    "IsIPv4",
    "IsIPv4_ForKey",
    "IsIPv6LinkLocal",
    "IsIPv6MLD",
    "IsIPv6",
    "IsIPv6_ForKey",
    "IsIP",
    "IsIP_ForKey",
    "IsMPLS",
    "IsND",
    "IsPTP",
    "IsSYN",
    "L2Echo",
    "L2Encapsulation",
    "L2_Valid",
    "L2_VID",
    "L3OffsetInvalid",
    "L3TotalOffset_or_InnerHeaderTotalOffset",
    "L3_Offset",
    "L4Byte13",
    "L4Bytes3_0",
    "L4OffsetInvalid",
    "L4Valid",
    "LBI",
    "LLT",
    "LocalDeviceSrcTrunkID",
    "LocalDevSrcPort",
    "Lookup0_PCL_mode",
    "Lookup1_PCL_mode",
    "mac2me",
    "MACDALookupResult",
    "MacDaType",
    "MACDA bits 0..31",
    "MACDA bits 32..48",
    "MacSAArpSAMismatch",
    "MACSA bits 0..31",
    "MACSA bits 32..48",
    "MailBoxToNeighborCPU",
    "MarvellTaggedExtended",
    "MarvellTagged",
    "MarvellTagSize",
    "MCRPFFail",
    "MDB",
    "MeteringEn",
    "MirrorOrTrap",
    "MirrorToCpu",
    "MLLCpuCode",
    "MLLEn_Dup",
    "MLLEn",
    "MLLFirst",
    "MLLHash",
    "MLLLast",
    "MLLPtr",
    "MLL",
    "MLL_Pointer",
    "ModifyDSCP",
    "ModifyDSCP_EXP",
    "ModifyEXP",
    "ModifyUP",
    "MPLSCmd",
    "MPLSLabel",
    "MPLS_OuterEXP",
    "MPLS_OuterLabel",
    "MPLS_OuterS_Bit",
    "MtagDstTagged",
    "NAMessageToCPUEn",
    "NestedVlanAccessPort",
    "NewSAIsNotSecurityBreach",
    "NHEMCRPFFailCmd",
    "NLPAboveTunnel",
    "Num_of_IngressTags",
    "OrigIsTrunk",
    "OrigSrcPort",
    "OrigSrcTrunk",
    "OrigVID",
    "OuterIsLLCNonSnap",
    "OuterL3TotalOffset",
    "OuterSrcTag",
    "OutGoingMtagCmd",
    "OutgoingMtagCommand",
    "OutLIFType",
    "OutLIF bits 0..31",
    "OutLIF bits 32..35",
    "OutLIF_Type",
    "OutLinkLayer",
    "OutTunnelLayer",
    "P2_desc bits 0..31",
    "P2_desc bits 128..159",
    "P2_desc bits 160..191",
    "P2_desc bits 192..202",
    "P2_desc bits 32..63",
    "P2_desc bits 64..95",
    "P2_desc bits 96..127",
    "PacketCmd",
    "PacketIsCNTagged",
    "PacketIsLLCSnap",
    "PacketTOS",
    "PacketType",
    "PacketUP",
    "PCL1_IPv4_Key_Type",
    "PCL1_IPv6_Key_Type",
    "PCL1_Lookup_mode",
    "PCL1_Lookup_Type",
    "PCL1_NonIP_Key_Type",
    "PCL1_PCL_ID",
    "PclRxSniffer",
    "PktHash",
    "PktIsLooped",
    "PktTypeForKey",
    "PolicerEn",
    "PolicerPtr",
    "PolicyBasedRouted",
    "PolicyRouterNhIndex",
    "PortSpeedIndex_or_OrigQCNPrio",
    "PortVlanSel",
    "PreservedLocalDevSrcPort",
    "PrioTagged",
    "PseudoWireID",
    "PTPTriggerType",
    "QCNqDelta",
    "QCNqFb",
    "QCNqOffset",
    "QCNqSaturated",
    "QCN",
    "QinQ",
    "QosProfilePrecedence",
    "QosProfile",
    "RecalcCRC",
    "RedirectCmd",
    "Reserved bits 0..31",
    "Reserved bits 32..63",
    "Reserved bits 64..92",
    "Reserved",
    "RipV1MirrorEnable",
    "Routed",
    "RPFCheckFail",
    "RpfCompMode",
    "RxSniff",
    "SIP bits 0..31",
    "SIP bits 32..63",
    "SIP bits 64..95",
    "SIP bits 96..127",
    "SnifferTrgDev",
    "SnifferTrgPort",
    "SnifferVIDX",
    "Sniffer",
    "SniffUseVIDX",
    "SolicitationMulticastMessage",
    "SrcDevice",
    "SrcDevice_or_TrgDevice",
    "SrcDevIsOwn",
    "SrcDev",
    "SrcIsLoopback",
    "SrcOwnDev0",
    "SrcPortInfo",
    "SrcPort",
    "SrcPort_or_TrgPort",
    "SrcTagged_dup",
    "SrcTagged",
    "SrcTagged_or_TrgTagged",
    "SrcTCPUDPPortIs0",
    "SrcTrg",
    "SrcTrunkID",
    "SrcVlanTag",
    "Src_is_PE",
    "SST_ID",
    "StreamID",
    "SYNWithData",
    "TCPFlags",
    "TCP_UDPDestPort",
    "TC_Dup",
    "TC",
    "TrgDevice",
    "TrgDev",
    "TrgIsLoopback",
    "TrgIsTrunk",
    "TrgPort",
    "TrgTagged_dup",
    "TrgTagged",
    "TrgTagMode",
    "TrgTrunkID",
    "Truncated",
    "TsPassengerType",
    "TS_Passenger_Type",
    "TTI_ActionExtended",
    "TTI_Match",
    "TTL",
    "TTPassengerType",
    "TunnelPtr",
    "TunnelStart",
    "TunnelTerminated",
    "TunnelType",
    "Tunnel_Pointer",
    "UDB0",
    "UDB10",
    "UDB11",
    "UDB12",
    "UDB13",
    "UDB14",
    "UDB15",
    "UDB16",
    "UDB17",
    "UDB18",
    "UDB19",
    "UDB1",
    "UDB20",
    "UDB21",
    "UDB22",
    "UDB2",
    "UDB3",
    "UDB4",
    "UDB5",
    "UDB6",
    "UDB7",
    "UDB8",
    "UDB9",
    "UDB_BytesValid",
    "UnknownUnicastDACmd",
    "UnknownUnicastSACmd",
    "UnregisteredIPMCCmd",
    "UnregisteredIPv4BCCmd",
    "UnregisteredNonIPMCCmd",
    "UnregisteredNonIPv4BCCmd",
    "UntaggedMRUIndex",
    "UP1",
    "UP",
    "UseDescSourceID",
    "UseFabricVIDX",
    "UseVIDX",
    "VID1",
    "VIDModified",
    "VIDX",
    "VID",
    "VID_or_InLIF_ID",
    "VID_Precedence",
    "VlanCmd",
    "VplsBcFilterEnable",
    "VplsPortType",
    "VplsUnknownUcFilterEnable",
    "VplsUnregMcFilterEnable",
    "VR_ID"
};

static GT_STATUS appDemoExMxPmDescrDump
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_EXMXPM_DIAG_DESCRIPTOR_TYPE_ENT descriptorType,
    IN  GT_BOOL                             dumpAll
)
{
    GT_STATUS rc;
    GT_U32 i;  /* iterator */
    CPSS_EXMXPM_DIAG_DESCRIPTOR_TYPE_ENT descr;      /* iterator */
    CPSS_EXMXPM_DIAG_DESCRIPTOR_TYPE_ENT descrSave;  /* iterator copy */
    GT_CHAR unitName[50];    /* unit name */
    /* pointer to descriptor information */
    CPSS_EXMXPM_DIAG_DESCRIPTOR_STC *descriptorPtr = NULL;
    APP_DEMO_DESCR_FIELD_TYPE_ENT fieldType; /* field type */
    GT_U32  *memoPtr;   /* memory pointer for unknown format */
    GT_U32   memoSize;  /* size of descriptor in bytes       */

    if(dumpAll == GT_FALSE)
    {
        descr = descriptorType;
    }
    else
    {
        descr = 0;
    }

    descriptorPtr = (CPSS_EXMXPM_DIAG_DESCRIPTOR_STC*)
                     cpssOsMalloc(sizeof(CPSS_EXMXPM_DIAG_DESCRIPTOR_STC));
    if (descriptorPtr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    descrSave = descr;
    for(; descr < CPSS_EXMXPM_DIAG_DESCRIPTOR_LAST_E; descr++)
    {
        if ((dumpAll == GT_FALSE) && (descrSave != descr))
        {
            break;
        }

        /* Print in Raw Format */
        if (appDemoDescrDumpRawDumpEn != GT_FALSE)
        {
            memoSize = sizeof(CPSS_EXMXPM_DIAG_DESCRIPTOR_STC);
            memoPtr  = (GT_U32*)descriptorPtr;
            rc = cpssExMxPmDiagDescriptorPortGroupRawGet(
                devNum, portGroupsBmp, descr,
                &memoSize, memoPtr);
            if (rc != GT_OK)
            {
                cpssOsFree(descriptorPtr);
                return rc;
            }
            for (i = 0; (i < ((memoSize + 3) / 4)); i++)
            {
                if ((i % 8) == 0)
                {
                    cpssOsPrintf("%04X: ", (i * 4));
                }
                cpssOsPrintf("%08X ", memoPtr[i]);
                if ((i % 8) == 3)
                {
                    cpssOsPrintf("   ");
                }
                if ((i % 8) == 7)
                {
                    cpssOsPrintf("\n");
                }
            }
            cpssOsPrintf("\n");
        }

        switch(descr)
        {
            case CPSS_EXMXPM_DIAG_DESCRIPTOR_WR_DMA_TO_TTI_E:
                cpssOsStrCpy(unitName, "WR_DMA_TO_TTI");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_TTI_TO_PCL_E:
                cpssOsStrCpy(unitName, "TTI_TO_PCL");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_PCL_TO_BRIDGE_E:
                cpssOsStrCpy(unitName, "PCL_TO_BRIDGE");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_BRIDGE_TO_ROUTER_E:
                cpssOsStrCpy(unitName, "BRIDGE_TO_ROUTER");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_ROUTER_TO_INGRESS_POLICER_E:
                cpssOsStrCpy(unitName, "ROUTER_TO_INGRESS_POLICER_E");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_INGRESS_POLICER_TO_PRE_EGRESS_E:
                cpssOsStrCpy(unitName, "INGRESS_POLICER_TO_PRE_EGRESS");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_PRE_EGRESS_TO_VOQ_E:
                cpssOsStrCpy(unitName, "PRE_EGRESS_TO_VOQ");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_FABRIC_HEADER_ALTERATION_INPUT_E:
                cpssOsStrCpy(unitName, "FABRIC_HEADER_ALTERATION_INPUT");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_DIT_TO_TXQ_E:
                cpssOsStrCpy(unitName, "DIT_TO_TXQ");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_EGRESS_PCL_TO_EGRESS_POLICER_E:
                cpssOsStrCpy(unitName, "EGRESS_PCL_TO_EGRESS_POLICER");
                break;

            case CPSS_EXMXPM_DIAG_DESCRIPTOR_TXQ_TO_TXDMA_E:
                cpssOsStrCpy(unitName, "TXQ_TO_TXDMA");
                break;

            default:
                cpssOsFree(descriptorPtr);
                return GT_BAD_PARAM;
        }

        rc = cpssExMxPmDiagDescriptorPortGroupGet(
            devNum, portGroupsBmp, descr, descriptorPtr);

        if (rc == GT_NOT_SUPPORTED)
        {
            continue;
        }

        if (rc != GT_OK)
        {
            cpssOsFree(descriptorPtr);
            return rc;
        }

        for(i = 0; i < CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_LAST_E; i++)
        {
            if(descriptorPtr->fieldValueValid[i] == GT_TRUE)
            {
                switch (i)
                {
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_ETHERTYPE_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB0_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB10_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB11_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB12_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB13_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB14_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB15_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB16_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB17_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB18_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB19_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB1_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB20_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB21_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB22_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB2_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB3_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB4_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB5_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB6_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB7_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB8_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB9_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_UDB_BYTESVALID_E:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_HEX_E ;
                        break;
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_FABRIC_DESC_BITS_0_31_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_FABRIC_DESC_BITS_128_159_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_FABRIC_DESC_BITS_160_191_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_FABRIC_DESC_BITS_32_63_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_FABRIC_DESC_BITS_64_95_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_FABRIC_DESC_BITS_96_127_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_OUTLIF_BITS_0_31_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_OUTLIF_BITS_32_35_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_P2_DESC_BITS_0_31_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_P2_DESC_BITS_128_159_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_P2_DESC_BITS_160_191_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_P2_DESC_BITS_192_202_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_P2_DESC_BITS_32_63_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_P2_DESC_BITS_64_95_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_P2_DESC_BITS_96_127_E:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_HEX32_E ;
                        break;
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_MACDA_BITS_0_31_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_MACSA_BITS_0_31_E:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_MAC_E ;
                        break;
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_DIP_BITS_0_31_E:
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_SIP_BITS_0_31_E:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_IPV6_E ;
                        break;
                    case CPSS_EXMXPM_DIAG_DESCRIPTOR_FIELD_TYPE_DIP_E:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_IPV4_E ;
                        break;
                    default:
                        fieldType = APP_DEMO_DESCR_FIELD_TYPE_DEC_E;
                        break;
                }

                switch(fieldType)
                {
                    case APP_DEMO_DESCR_FIELD_TYPE_HEX_E:
                        cpssOsPrintf("%s; %s; 0x%X;\n", unitName, exmxpm_field_strings[i],
                                     descriptorPtr->fieldValue[i]);
                        break;
                    case APP_DEMO_DESCR_FIELD_TYPE_HEX32_E:
                        cpssOsPrintf("%s; %s; 0x%08X;\n", unitName, exmxpm_field_strings[i],
                                     descriptorPtr->fieldValue[i]);
                        break;
                    case APP_DEMO_DESCR_FIELD_TYPE_MAC_E:
                        cpssOsPrintf("%s; %s; %02X:%02X:%02X:%02X:%02X:%02X;\n", unitName, exmxpm_field_strings[i],
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+1],8,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+1],0,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],24,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],16,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],8,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],0,8));
                        /* the MAC use two fields, skip second one */
                        i++;
                        break;
                    case APP_DEMO_DESCR_FIELD_TYPE_IPV6_E:
                        cpssOsPrintf("%s; %s; %X:%X:%X:%X:%X:%X:%X:%X;\n", unitName, exmxpm_field_strings[i],
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+3],16,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+3],0,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+2],16,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+2],0,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+1],16,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i+1],0,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],16,16),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],0,16));
                        /* the IPV6 use four fields, skip three one */
                        i += 3;
                        break;
                    case APP_DEMO_DESCR_FIELD_TYPE_IPV4_E:
                        cpssOsPrintf("%s; %s; %d.%d.%d.%d;\n", unitName, exmxpm_field_strings[i],
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],24,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],16,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],8,8),
                                     U32_GET_FIELD_MAC(descriptorPtr->fieldValue[i],0,8));
                        break;
                    default:
                        cpssOsPrintf("%s; %s; %d;\n", unitName, exmxpm_field_strings[i],
                                     descriptorPtr->fieldValue[i]);
                        break;
                }

            }
        }

        /* separate descriptors */
        cpssOsPrintf("\n\n");
    }

    cpssOsFree(descriptorPtr);
    return GT_OK;

}

#endif

GT_STATUS appDemoDescrDump
(
    IN GT_U8                   devNum,
    IN GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN GT_U32                  descriptorType,
    IN GT_BOOL                 dumpAll
)
{
#ifdef CHX_FAMILY
    return appDemoDxChDescrDump(devNum, portGroupsBmp, descriptorType, dumpAll);
#elif defined EXMXPM_FAMILY
    return appDemoExMxPmDescrDump(devNum, portGroupsBmp, descriptorType, dumpAll);
#else
    return GT_NOT_SUPPORTED;
#endif
}

/******************************************************************************
* appDemoTrafficDisableSet
*
* DESCRIPTION:
*       Set traffic enable/disable mode - API that will enable/disable
*       execution of traffic enable algorithm during cpssInitSystem
*
* INPUTS:
*       disable    - GT_TRUE - do not enable traffic during cpssInitSystem
*                  - GT_FALSE - enable traffic during cpssInitSystem
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
******************************************************************************/
GT_VOID appDemoTrafficDisableSet
(
    IN GT_BOOL disable
)
{
    appDemoTrafficEnableDisableMode = disable;
}

/******************************************************************************
* appDemoTraceHwAccessDbLine
*
* DESCRIPTION:
*       returns a line of the trace database, either the read or write database,
*       delay uses the write database
*
* INPUTS:
*       index    - index of the line to retreive
*       accessType - access type, either read, write or write and delay
*
* OUTPUTS:
*       dbLine - A non-null pointer to a APP_DEMO_CPSS_HW_ACCESS_DB_STC struct
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/
GT_STATUS appDemoTraceHwAccessDbLine
(
    GT_U32 index,
    OUT APP_DEMO_CPSS_HW_ACCESS_DB_STC * dbLine
)
{
    CPSS_NULL_PTR_CHECK_MAC(dbLine);

    if (index<appDemoAccessInfo.curSize)
    {
        dbLine->accessParamsBmp=appDemoAccessDbPtr[index].accessParamsBmp;
        dbLine->data=appDemoAccessDbPtr[index].data;
        dbLine->addr=appDemoAccessDbPtr[index].addr;
        return GT_OK;
    }
    else
    {
        return GT_BAD_PARAM;
    }

}

/******************************************************************************
* appDemoTraceHwAccessClearDb
*
* DESCRIPTION:
*       Clear HW access db
*
* INPUTS:
*      none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/

GT_STATUS appDemoTraceHwAccessClearDb
(
    GT_VOID
)
{

    appDemoAccessInfo.curSize = 0;
    appDemoAccessInfo.corrupted = GT_FALSE;

    return GT_OK;

}

/******************************************************************************
* appDemoTraceHwAccessDbIsCorrupted
*
* DESCRIPTION:
*       Check if the HW access data base has reached the limit
*
* INPUTS:
*      none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/

GT_STATUS appDemoTraceHwAccessDbIsCorrupted
(
    GT_BOOL * corruptedPtr
)
{

   * corruptedPtr = appDemoAccessInfo.corrupted ;

   return GT_OK;
}






/******************************************************************************
* appDemoSupportAaMessageSet
*
* DESCRIPTION:
*       Configure the appDemo to support/not support the AA messages to CPU
*
* INPUTS:
*      supportAaMessage -
*                      GT_FALSE - not support processing of AA messages
*                      GT_TRUE - support processing of AA messages
*
* OUTPUTS:
*       none
*
* RETURNS:
*       nothing
*
* COMMENTS:
*
*
******************************************************************************/
GT_VOID appDemoSupportAaMessageSet
(
    IN GT_BOOL supportAaMessage
)
{
    appDemoSysConfig.supportAaMessage = supportAaMessage;
}

/* return GT_ABORT in callbacks: 0 - do not return GT_ABORT, != 0 - return GT_ABORT */
GT_U32 appDemoAccessToAddrAbort = 0;

/* register's address to trace */
GT_U32 appDemoAddrToTrace = 0;

/*******************************************************************************
 * appDemoRegWriteCb
 *
 * DESCRIPTION:
 *    Register write callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       regAddr      - The register's address to read from.
 *       data         - The value to be written.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK       - callback success to get the data.
 *       GT_FAIL     - callback failed.
 *       GT_ABORTED  - operation done and aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
static GT_STATUS appDemoRegWriteCb
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    IN  GT_U32   data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
)
{
    char *stageStr;
    stageStr = (CPSS_DRV_HW_ACCESS_STAGE_PRE_E == stage)? "PRE" : "POST";
    if (appDemoAddrToTrace == regAddr)
    {
        osPrintf("appDemoRegWriteCb devNum[%d] portGroupId[%d] regAddr[0X%08X] data[0X%08X] stage[%s]\n", devNum, portGroupId, regAddr, data, stageStr);
        if (appDemoAccessToAddrAbort)
        {
            return GT_ABORTED;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* appDemoHwAccessTraceBind
*
* DESCRIPTION:
*         The function binds/unbinds a callback routines for HW access.
*
* INPUTS:
*    bind           -  GT_TRUE - bind callback routines.
*                      GT_FALSE - un bind callback routines.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoHwAccessTraceBind
(
    GT_BOOL                     bind
)
{
    CPSS_DRV_HW_ACCESS_OBJ_STC hwAccessObj;
    GT_STATUS rc;
    osMemSet(&hwAccessObj, 0, sizeof(hwAccessObj));
    hwAccessObj.hwAccessRegisterWriteFunc = appDemoRegWriteCb;

    rc = cpssDrvHwAccessObjectBind(&hwAccessObj, bind);
    return rc;
}

static GT_U32 appDemoHwAccessFailCounterLimit = 0;
static GT_U32 appDemoHwWriteAccessCounter = 0;
static GT_BOOL appDemoInsertHwFailEnable = GT_FALSE;

/*******************************************************************************
 * appDemoWriteFailerCb
 *
 * DESCRIPTION:
 *     write fail callback function for Hw access.
 *
 * INPUTS:
 *       devNum       -  The PP to read from.
 *       portGroupId  - The port group id. relevant only to 'multi-port-groups'
 *                      devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *       regAddr      - The register's address to read from.
 *       length       - Number of Words (4 byte) to write.
 *       data         - The value to be written.
 *       stage        - stage for callback run.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK       - callback success to get the data.
 *       GT_FAIL     - callback failed.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
static GT_STATUS appDemoWriteFailerCb
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    IN  GT_U32   length,
    IN  GT_U32   *data,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT  stage
)
{

    /* to prevent warnings */
    if (devNum);
    if (portGroupId);
    if (regAddr);
    if (length);
    if (data);
    if (appDemoInsertHwFailEnable == GT_TRUE)
    {
        if (stage == CPSS_DRV_HW_ACCESS_STAGE_PRE_E)
        {
            appDemoHwWriteAccessCounter++;
        }

        if (appDemoHwAccessFailCounterLimit > 0)
        {
            if (appDemoHwWriteAccessCounter >= appDemoHwAccessFailCounterLimit)
            {
                appDemoHwWriteAccessCounter = 0;
                appDemoHwAccessFailCounterLimit = 0;
                return GT_FAIL;
            }
        }
    }
    return GT_OK;
}


/*******************************************************************************
* appDemoHwAccessFailerBind
*
* DESCRIPTION:
*         The function binds/unbinds a appDemoWriteFailerCb callback for HW write access
*         and set hw write fail counter.
*
* INPUTS:
*    bind           -  GT_TRUE - bind callback routines.
*                      GT_FALSE - un bind callback routines.
*   failCounter     - hw write fail counter: set hw write sequence
*                     number on which write operation should fail.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoHwAccessFailerBind
(
    IN GT_BOOL                  bind,
    GT_U32                      failCounter
)
{
    CPSS_DRV_HW_ACCESS_OBJ_STC hwAccessObj;
    GT_STATUS rc;
    osMemSet(&hwAccessObj, 0, sizeof(hwAccessObj));
    hwAccessObj.hwAccessRamWriteFunc = appDemoWriteFailerCb;

    rc = cpssDrvHwAccessObjectBind(&hwAccessObj, bind);
    appDemoHwWriteAccessCounter = 0;
    if (bind == GT_TRUE)
    {
        appDemoInsertHwFailEnable = GT_TRUE;
        appDemoHwAccessFailCounterLimit  = failCounter;
    }
    else
    {
        appDemoInsertHwFailEnable = GT_FALSE;
        appDemoHwAccessFailCounterLimit = 0;
    }
    return rc;
}

/*******************************************************************************
* appDemoHwAccessCounterGet
*
* DESCRIPTION:
*         The function gets number of hw accesses
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       hwAccessCounterPtr - points to hw access counter value.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoHwAccessCounterGet
(
    OUT GT_U32   *hwAccessCounterPtr
)
{
    *hwAccessCounterPtr = appDemoHwWriteAccessCounter;
    return GT_OK;
}

extern GT_STATUS wrCpssResetLib
(
    GT_VOID
);

extern GT_STATUS appDemoDxChDbReset
(
    IN  GT_U8  devNum
);

extern GT_STATUS appDemoExMxPmDbReset
(
    IN  GT_U8  devNum
);


/******************************************************************************
* internal_cpssResetSystem
*
* DESCRIPTION:
*       API performs CPSS reset that includes device remove, mask interrupts,
*       queues and tasks destroy, all static variables/arrays clean.
*
* INPUTS:
*       noSoftReset - if true no DB reset, only remove device and do board specific type reset
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/
static GT_STATUS internal_cpssResetSystem
(
    IN  GT_BOOL     noSoftReset
)
{
    GT_STATUS       rc;         /* To hold funcion return code  */
    GT_U32          i;          /* Loop indexes                 */
    GT_U8           devNum;     /* Device number                */
    GT_32           intKey;     /* The interrupt lock key       */
    GT_U32          allowInterruptLock;   /* current task priority        */
    GT_U32  secondsStart, secondsEnd,
            nanoSecondsStart, nanoSecondsEnd,
            seconds, nanoSec; /* time of Reset system */

    if(initCounter == 0)
    {
        /* indication that the cpssInitSystem did not run event for first time ,
        so nothing to reset */
        /* this code for the 'Generic phase' between tests that do:
        1. cpssResetSystem
        2. cpssInitSystem
        */
        return GT_NOT_INITIALIZED;
    }

    if(NULL == (GT_PTR)boardCfgFuncs.boardCleanDbDuringSystemReset)
    {
        /* the specific board not implemented 'System reset' */
        return GT_NOT_SUPPORTED;
    }

    /* first check that all devices in the system supports the 'system reset' */
    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_FALSE)
        {
            continue;
        }

        if(appDemoPpConfigList[i].devSupportSystemReset == GT_FALSE)
        {
            /* the device not support the 'System reset' */
            return GT_NOT_SUPPORTED;
        }
    }

    osPrintSync("cpssResetSystem : started \n");

    /******************************/
    /* take the time before start */
    /******************************/
    rc = cpssOsTimeRT(&secondsStart,&nanoSecondsStart);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssOsTimeRT", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    allowInterruptLock = 1;
    #ifdef _WIN32
        /* don't know why get dead lock during slan unbind when we are under
           the interrupts lock */
        allowInterruptLock = 0;
        /* the function appDemoEventRequestDrvnModeReset will make sure that
           interrupts will not come from the PP .. by masking the interrupts */
    #endif /*_WIN32*/


    if(allowInterruptLock)
    {
        /*osPrintSync("cpssResetSystem : Lock interrupts \n");*/
        /* Lock interrupts */
        extDrvSetIntLockUnlock(INTR_MODE_LOCK, &intKey);
    }

    appDemoPrePhase1Init = GT_FALSE;
    appDemoCpssInitialized = GT_FALSE;

    /*osPrintSync("cpssResetSystem : reset events engine and kill even handlers tasks \n");*/
    rc = appDemoEventRequestDrvnModeReset();
    if (rc != GT_OK)
    {
        goto exitCleanly_lbl;
    }

    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            devNum = appDemoPpConfigList[i].devNum;
            /*osPrintSync("cpssResetSystem : start remove device[%d] \n",
                devNum);*/

            /* next is done inside prvCpssPpConfigDevDbRelease(...)
                the DXCH remove device call it ...
                the puma should too... */
            /*rc = cpssBmPoolDelete(devPtr->intCtrl.txDescList[queueNum].poolId);*/

            /* do generic family type reset */
            rc = GT_NOT_IMPLEMENTED;
#ifdef EXMXPM_FAMILY
            if(CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[i].devFamily))
            {
                rc = appDemoExMxPmDbReset(devNum);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoExMxPmDbReset", rc);
            }
#endif

#ifdef CHX_FAMILY
            if(CPSS_IS_DXCH_FAMILY_MAC(appDemoPpConfigList[i].devFamily))
            {
                if (noSoftReset == GT_FALSE)
                {
                    rc = appDemoDxChDbReset(devNum);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChDbReset", rc);
                }
                else
                {

                    /* no DB reset, only remove device */
                    rc = cpssDxChCfgDevRemove(devNum);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgDevRemove", rc);
                    if (rc != GT_OK)
                    {
                        goto exitCleanly_lbl;
                    }

                    /* do board specific type reset */
                    rc = boardCfgFuncs.boardCleanDbDuringSystemReset(devNum);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardCleanDbDuringSystemReset", rc);

                    goto exitCleanly_lbl;
                }
            }
#endif
            if(GT_OK != rc)
            {
                goto exitCleanly_lbl;
            }

            osMemSet(&(appDemoPpConfigList[i]), 0, sizeof(appDemoPpConfigList[i]));

            appDemoPpConfigList[i].ppPhase1Done = GT_FALSE;
            appDemoPpConfigList[i].ppPhase2Done = GT_FALSE;
            appDemoPpConfigList[i].ppLogicalInitDone = GT_FALSE;
            appDemoPpConfigList[i].ppGeneralInitDone = GT_FALSE;
            appDemoPpConfigList[i].valid = GT_FALSE;

            /* default value for cpu tx/rx mode */
            appDemoPpConfigList[i].cpuPortMode = CPSS_NET_CPU_PORT_MODE_SDMA_E;
            appDemoPpConfigList[i].allocMethod = CPSS_RX_BUFF_STATIC_ALLOC_E;

            /* cascading information */
            appDemoPpConfigList[i].numberOfCscdTrunks = 0;
            appDemoPpConfigList[i].numberOfCscdPorts = 0;
            appDemoPpConfigList[i].numberOfCscdTargetDevs = 0;
            appDemoPpConfigList[i].numberOf10GPortsToConfigure = 0;
            appDemoPpConfigList[i].internal10GPortConfigFuncPtr = NULL;
            appDemoPpConfigList[i].internalCscdPortConfigFuncPtr = NULL;

            /*osPrintSync("cpssResetSystem : end remove device[%d] \n",
                devNum);*/
        }
    }

    /*osPrintSync("cpssResetSystem : destroy generic global cpss DB \n");*/
    rc = cpssPpDestroy();
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssPpDestroy", rc);
    if(GT_OK != rc)
    {
        goto exitCleanly_lbl;
    }


    /*osPrintSync("cpssResetSystem : board specific type reset \n");*/
    /* do board specific type reset */
    rc = boardCfgFuncs.boardCleanDbDuringSystemReset((GT_U8)storedBoardIdx);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("boardCfgFuncs.boardCleanDbDuringSystemReset", rc);
    if(GT_OK != rc)
    {
        goto exitCleanly_lbl;
    }

#ifdef IMPL_GALTIS
    /*osPrintSync("cpssResetSystem : galtis wrappers reset \n");*/
    rc = wrCpssResetLib();
    CPSS_ENABLER_DBG_TRACE_RC_MAC("wrCpssResetLib", rc);
    if(GT_OK != rc)
    {
        goto exitCleanly_lbl;
    }
#endif /* IMPL_GALTIS */
    tgfTrafficGeneratorSystemReset();

    /* destroy DMA only after other registered function detached from the DMA */
    /*osPrintSync("cpssResetSystem : destroy All DMA allocations \n");*/
    osCacheDmaFreeAll();
    CPSS_ENABLER_DBG_TRACE_RC_MAC("osCacheDmaFreeAll", rc);
    if(GT_OK != rc)
    {
        goto exitCleanly_lbl;
    }

    /********************************/
    /* wait until the reset is done */
    /********************************/
    #ifdef ASIC_SIMULATION
    #ifndef _WIN32 /* it seems that linux miss something */
        SIM_OS_MAC(simOsSleep)(100);
    #endif /*_WIN32*/
    while(skernelStatusGet())
    {
        osTimerWkAfter(10);
        osPrintf("7");
    }
    osPrintf("\n");
    #else /* not ASIC_SIMULATION */
    osTimerWkAfter(10);
    #endif /* not ASIC_SIMULATION */

    auFdbUpdateLockReset();

#ifdef SHARED_MEMORY
    /* stop fdbLearning and RxTxProcess */
    osPrintf("stopping fdbLearning and RxTxProcess...");
    if (fdbLearningProcessId > 0)
    {
        kill(fdbLearningProcessId, SIGTERM);
        waitpid(fdbLearningProcessId, NULL, 0);
        fdbLearningProcessId = 0;
    }
    if (RxTxProcessId > 0)
    {
        kill(RxTxProcessId, SIGTERM);
        waitpid(RxTxProcessId, NULL, 0);
        RxTxProcessId = 0;
    }
    osPrintf(" done\n");
#endif


exitCleanly_lbl:

    if(allowInterruptLock)
    {
        /*osPrintSync("cpssResetSystem : Unlock interrupts \n");*/
        /* Unlock interrupts */
        extDrvSetIntLockUnlock(INTR_MODE_UNLOCK, &intKey);
    }

    /******************************/
    /* take the time after finish */
    /******************************/
    if (cpssOsTimeRT(&secondsEnd,&nanoSecondsEnd) != GT_OK)
    {
        return rc;
    }

    seconds = secondsEnd-secondsStart;
    if(nanoSecondsEnd >= nanoSecondsStart)
    {
        nanoSec = nanoSecondsEnd-nanoSecondsStart;
    }
    else
    {
        nanoSec = (1000000000 - nanoSecondsStart) + nanoSecondsEnd;
        seconds--;
    }

    osPrintSync("cpssResetSystem iteration#(%d) --  ended time: %d sec., %d nanosec.\n", resetCounter , seconds, nanoSec);

    resetCounter++;

    return rc;
}

/******************************************************************************
* cpssResetSystem
*
* DESCRIPTION:
*       API performs CPSS reset that includes device remove, mask interrupts,
*       queues and tasks destroy, all static variables/arrays clean.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/
GT_STATUS cpssResetSystem(void)
{
    return internal_cpssResetSystem(GT_FALSE);
}


/******************************************************************************
* cpssResetSystemNoSoftReset
*
* DESCRIPTION:
*       API performs device remove and do board specific type reset.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS cpssResetSystemNoSoftReset(void)
{
    return internal_cpssResetSystem(GT_TRUE);
}


/******************************************************************************
* cpssDevSupportSystemResetSet
*
* DESCRIPTION:
*       API controls if SYSTEM reset is supported, it is used to disable the
*       support on devices that do support. vice versa is not possible
*
* INPUTS:
*       IN  GT_U8       devNum
*       IN  GT_BOOL     enable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success else if failed
*
* COMMENTS:
*
*
******************************************************************************/
GT_STATUS cpssDevSupportSystemResetSet(

    IN  GT_U8       devNum,
    IN  GT_BOOL     enable

)
{

    appDemoPpConfigList[devNum].devSupportSystemReset = enable;
    return GT_OK;
}


/*******************************************************************************
* boardPhyConfig
*
* DESCRIPTION:
*       Board specific PHY configurations after board initialization.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - device number
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
GT_STATUS boardPhyConfig
(
    IN  GT_U8       boardRevId,
    IN  GT_U8       devNum
)
{
    GT_STATUS rc = GT_OK;

#ifdef CHX_FAMILY
#if 0
    if (PRV_CPSS_PP_MAC(devNum)->devFamily ==  CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
    {
        rc = bobcat2BoardPhyConfig(boardRevId,devNum);
        return rc;
    }
#endif
#endif
    return rc;
}

/*******************************************************************************
* boardExtPhyConfig
*
* DESCRIPTION:
*       External PHY configuration for BOBK devices.
*
* INPUTS:
*       devNum          - device number
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
GT_STATUS boardExtPhyConfig
(
    IN  GT_U8       devNum,
    IN  GT_U32      phyType,
    IN  GT_U32      smiInterface,
    IN  GT_U32      smiStartAddr
)
{
    GT_STATUS rc = GT_OK;

#ifdef CHX_FAMILY
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily ==  CPSS_PP_FAMILY_DXCH_BOBCAT2_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E))
    {
#if 0
        rc = bobkExternalBoardPhyConfig(devNum, phyType, smiInterface, smiStartAddr);
        return rc;
#endif
    }
#endif
    return rc;
}

/*******************************************************************************
* appDemoDevIdxGet
*
* DESCRIPTION:
*       Find device index in "appDemoPpConfigList" array based on device SW number.
*
* INPUTS:
*       devNum - SW device number
*
* OUTPUTS:
*       devIdxPtr - (pointer to) device index in array "appDemoPpConfigList".
*
* RETURNS:
*       GT_OK   - on success, match found for devNum.
*       GT_FAIL - no match found for devNum.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDevIdxGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *devIdxPtr
)
{
    GT_U32 ii;

    for ( ii = 0 ; ii < PRV_CPSS_MAX_PP_DEVICES_CNS ; ii++ )
    {
        if ( appDemoPpConfigList[ii].devNum == devNum)
        {
            *devIdxPtr = ii;
            return GT_OK;
        }
    }

    return GT_FAIL;
}


/*******************************************************************************
* appDemoCopyFileFromRamFs
*
* DESCRIPTION:
*       Copy file from RAMFS to local FS
*
* INPUTS:
*       srcFileName - a source file name (in RAMFS)
*       destFileName - a destination file name (in local FS)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - on srcFileName opening error
*       GT_CREATE_ERROR - on destFileName creating error
*       GT_FAIL     - on srcFileName reading error or destFileName writing error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoCopyFileFromRamFs
(
    IN GT_CHAR_PTR srcFileName,
    IN GT_CHAR_PTR destFileName
)
{
    GT_STATUS rc = GT_OK;
#if (defined (ASIC_SIMULATION)) && (defined(CMD_LUA_CLI))
    char   buf[2048];
    int    fdSrc  = -1;
    FILE * fdDest = NULL;
    int    num;

    do
    {
        fdSrc = cmdFS.open(srcFileName, GEN_FS_O_RDONLY);
        if (fdSrc < 0)             /* no such file */
        {
            rc = GT_NOT_FOUND;
            break;
        }

        fdDest = fopen(destFileName, "w+");
        if (fdDest == NULL)             /* couldn't open file for writing */
        {
            rc = GT_CREATE_ERROR;
            break;
        }

        while ( (num = cmdFS.read(fdSrc, buf, sizeof(buf))) > 0)
        {
            if (fwrite(buf, 1, num, fdDest) < (GT_U32)num) /* file writing error */
            {
                rc = GT_FAIL;
                break;
            }
        }
        if (num < 0)             /* file reading error */
        {
            rc = GT_FAIL;
        }

    } while (0);

    /* cleanup */
    if (fdSrc >= 0)
    {
        cmdFS.close(fdSrc);
    }

    if (fdDest != NULL)
    {
        fclose(fdDest);
    }
#endif  /* ASIC_SIMULATION */
    return rc;
}

/*******************************************************************************
* appDemoOsLogThreadModeSet
*
* DESCRIPTION:
*       Function for setting the thread mode of cpss log
*
* INPUTS:
*       mode -   mode to be set.
*       tid  -   the task (thread) id. Relevant for mode =
*                APP_DEMO_CPSS_LOG_TASK_SINGLE_E only
*       tidPtr - an adress the task (thread) id should be taken from.
*                Relevant for mode = APP_DEMO_CPSS_LOG_TASK_REF_E only
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID appDemoOsLogThreadModeSet
(
    IN    APP_DEMO_CPSS_LOG_TASK_MODE_ENT mode,
    IN    GT_U32                          tid,
    IN    GT_U32                         *tidPtr
)
{
    prvOsLogThreadMode = mode;
    if (mode == APP_DEMO_CPSS_LOG_TASK_SINGLE_E)
    {
        prvOsLogTaskId = tid;
        prvOsLogTaskIdPtr = &prvOsLogTaskId;
    }
    else if (mode == APP_DEMO_CPSS_LOG_TASK_REF_E)
    {
        prvOsLogTaskIdPtr = tidPtr;
    }

}


/*******************************************************************************
* appDemoOsLogMainUtOnly
*
* DESCRIPTION:
*       Enables CPSS API logging for main UT task(s)
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoOsLogMainUtOnly
(
    GT_VOID
)
{
#ifdef  INCLUDE_UTF
    appDemoOsLogThreadModeSet(APP_DEMO_CPSS_LOG_TASK_REF_E, 0, utfTestTaskIdAddrGet());
#endif  /* INCLUDE_UTF */
    return GT_OK;
}

/*******************************************************************************
* appDemoLogUtModeEnable
*
* DESCRIPTION:
*       Enable  CPSS API logging (with redirecting an output to the file) for
*       API calls from main UT's only and set some other useful options
*
* INPUTS:
*       logFileName - a file name where log  will be stored.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoLogUtModeEnable
(
    IN GT_CHAR_PTR logFileName
)
{
#ifdef CPSS_LOG_ENABLE
    GT_STATUS rc = GT_OK;

    /* enable a CPSS API logging for all libs */
    cpssLogEnableSet(GT_TRUE);
    rc = cpssLogLibEnableSet(CPSS_LOG_LIB_ALL_E, CPSS_LOG_TYPE_ALL_E, GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* log main UT API calls only */
    appDemoOsLogMainUtOnly();

    /* add prefix to pointer's to distinguish from them usual hex values */
    cpssLogPointerFormatSet(CPSS_LOG_POINTER_FORMAT_PREFIX_E);

    /* don't log line number */
    cpssLogLineNumberEnableSet(GT_FALSE);

    /* enable some API calls skipping to reduce the log size */
#ifdef  INCLUDE_UTF
    prvUtfReduceLogSizeFlagSet(GT_TRUE);
#endif  /* INCLUDE_UTF */

    /* log into the file on local file system */
    rc = appDemoOsLogModeSet(APP_DEMO_CPSS_LOG_MODE_LOCAL_FILE_E, logFileName);
    if (rc != GT_OK)
    {
        return rc;
    }
#endif /* CPSS_LOG_ENABLE */

    return GT_OK;
}
