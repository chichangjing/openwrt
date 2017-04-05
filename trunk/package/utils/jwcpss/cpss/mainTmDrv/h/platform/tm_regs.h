#ifndef _TM_ALIAS_H_
#define _TM_ALIAS_H_

/*
 * (c), Copyright 2009-2014, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief declaration for TM registers alias structure.
 *
 * @file tm_regs.h
 *
 * $Revision: 2.0 $
 */



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>

/** Maximum Baps num */
#define TM_MAX_BAP 10

struct tm_alias
{
    struct
	{
        GT_U64 ExcSrc;
        GT_U64 IntMask;
        GT_U64 FirstExc;
    }Gen;
    struct
	{
        GT_U64 HWInitPorts;
        GT_U64 PgHdrAdmission;
        GT_U64 PkgHdrAdmission;
        GT_U64 Internal_Uncorrectable_ECC_Error_status;
        GT_U64 PkgFIFOOvrflowErrStus;
        GT_U64 PgFIFOOvrflowErrStus;
        GT_U64 InitSnapshot;
        GT_U64 PkgBPSnapshot;
        GT_U64 PkgBPSnapshotMask;
        GT_U64 PkgBPSnapshotCnt;
        GT_U64 PkgBPSnapshotConfig;
        GT_U64 PgBPSnapshot;
        GT_U64 PgBPSnapshotMask;
        GT_U64 PgBPSnapshotCnt;
        GT_U64 PgBPSnapshotConfig;
        GT_U64 PktEnqFSMFIFOLevel;
        GT_U64 MessageFIFOLevel;
        GT_U64 PkgSnpAvailFreeCnt;
        GT_U64 PgSnpAvailFreeCnt;
        GT_U64 PkgContECCErrLogLSB;
        GT_U64 PgContECCErrLogLSB;
        GT_U64 PkgContECCErrLogMSB;
        GT_U64 PgContECCErrLogMSB;
        GT_U64 ErrStus;
        GT_U64 FirstExc;
        GT_U64 ErrCnt;
        GT_U64 ExcCnt;
        GT_U64 ExcMask;
        GT_U64 ID;
        GT_U64 ForceErr;
        GT_U64 MgmtEvtStus;
        GT_U64 MgmtEvtMask;
        GT_U64 NumOfLADs;
        GT_U64 NumOfPagesPerBank;
        GT_U64 MinPkgSize;
        GT_U64 NumOfPkgsPerBank;
        GT_U64 DDRChunnel5Map;
        GT_U64 CRC16En;
        GT_U64 TMHeaderProtocolVersion;
        GT_U64 OORPkgDropThresh;
        GT_U64 OORPageDropThresh;
        GT_U64 OORPkgPanicThresh;
        GT_U64 OORPagePanicThresh;
        GT_U64 StartTMExtMemInit;
        GT_U64 TMExtMemInitDone;
        GT_U64 QueueInstallCmd;
        GT_U64 QueueRemoveCmd;
        GT_U64 TMtoSMSbp;
        GT_U64 FreePkgHdrFIFOBp;
        GT_U64 FreePageHdrFIFOBp;
        GT_U64 FreePkgHdrRtnFIFOBp;
        GT_U64 FreePageHdrRtnFIFOBp;
        GT_U64 PkgHdrRespFIFOBp;
        GT_U64 PageHdrRespFIFOBp;
        GT_U64 RemoveQueueFIFOBp;
        GT_U64 PktEnqFSMFIFOBp;
        GT_U64 TMtoTMIncPktsFIFOBp;
        GT_U64 PgCtlRdCacheFIFOBp;
        GT_U64 InputSynchFifoConf;
        GT_U64 FromPipeNonErrPktCnt;
        GT_U64 FromPipeErrDrpPktCnt;
        GT_U64 FromPipeErrPasPktCnt;
        GT_U64 UninstalledQueueDrpPktCnt;
        GT_U64 VerMisDrpPktCnt;
        GT_U64 TMtoTMCRCErrDrpPktCnt;
        GT_U64 PerfCntrsPortAlloc;
        GT_U64 PerfCntrsTimerPer;
        GT_U64 UninstalledQueueLog;
        GT_U64 Mg2tm_error_log;
        GT_U64 Eco_Reg;
        GT_U64 EccConfig;
        GT_U64 Internal_Correctable_ECC_Error_status;
        GT_U64 PkgEccMemParams0;
        GT_U64 PkgEccMemParams1;
        GT_U64 PkgEccMemParams2;
        GT_U64 PkgEccMemParams3;
        GT_U64 PkgEccMemParams4;
        GT_U64 PkgEccMemParams5;
        GT_U64 PkgEccMemParams6;
        GT_U64 PkgEccMemParams7;
        GT_U64 PkgEccMemParams8;
        GT_U64 PkgEccMemParams9;
        GT_U64 PkgEccMemParams10;
        GT_U64 PkgEccMemParams11;
        GT_U64 PkgEccMemParams12;
        GT_U64 PkgEccMemParams13;
        GT_U64 PkgEccMemParams14;
        GT_U64 PkgEccMemParams15;
        GT_U64 PageEccMemParams0;
        GT_U64 PageEccMemParams1;
        GT_U64 PageEccMemParams2;
        GT_U64 PageEccMemParams3;
        GT_U64 PageEccMemParams4;
        GT_U64 PageEccMemParams5;
        GT_U64 PageEccMemParams6;
        GT_U64 PageEccMemParams7;
        GT_U64 PageEccMemParams8;
        GT_U64 PageEccMemParams9;
        GT_U64 PageEccMemParams10;
        GT_U64 PageEccMemParams11;
        GT_U64 PageEccMemParams12;
        GT_U64 PageEccMemParams13;
        GT_U64 PageEccMemParams14;
        GT_U64 PageEccMemParams15;
        GT_U64 PageEccMemParams16;
        GT_U64 PageEccMemParams17;
        GT_U64 PageEccMemParams18;
        GT_U64 ROC_Shadow;
        GT_U64 PageAdmission;
        GT_U64 PgDeqFSMCache;
        GT_U64 PkgBoth;
        GT_U64 PkgTail;
        GT_U64 PkgHead;
        GT_U64 PgEnq;
        GT_U64 PgDeq;
        GT_U64 PgDeqCacheLSB;
        GT_U64 PgDeqCacheMSB;
        GT_U64 PortChunksEmitPerSel;
        GT_U64 ReorderBufferAdmission;
        GT_U64 PerfCntrsSets;
        GT_U64 FreePkgStus_Grp;
        GT_U64 FreePgStus_Grp;
        GT_U64 PortFlush;
        GT_U64 QueueFlush;
        GT_U64 QueueLength;
    }QMgr;
/**************************************************************************
   to  change     all entries  with  .Table,  Cos, & Color  to structures
****************************************************************************/
    struct
	{
        GT_U64 ErrStus;
        GT_U64 FirstExc;
        GT_U64 ErrCnt;
        GT_U64 ExcCnt;
        GT_U64 ExcMask;
        GT_U64 Id;
        GT_U64 ForceErr;
        GT_U64 WREDDropProbMode;
        GT_U64 WREDMaxProbModePerColor;
        GT_U64 DPSource;
        GT_U64 RespLocalDPSel;
        GT_U64 TMtoTMPktGenQuantum;
        GT_U64 TMtoTMDPCoSSel;
        GT_U64 AgingUpdEnable;
        GT_U64 Drp_Decision_to_Query_debug;
        GT_U64 Drp_Decision_hierarchy_to_Query_debug;
        GT_U64 EccConfig;
        GT_U64 EccMemParams0;
        GT_U64 EccMemParams1;
        GT_U64 EccMemParams2;
        GT_U64 EccMemParams3;
        GT_U64 EccMemParams4;
        GT_U64 EccMemParams5;
        GT_U64 EccMemParams6;
        GT_U64 EccMemParams7;
        GT_U64 EccMemParams8;
        GT_U64 EccMemParams9;
        GT_U64 EccMemParams10;
        GT_U64 EccMemParams11;
        GT_U64 EccMemParams12;
        GT_U64 EccMemParams13;
        GT_U64 EccMemParams14;
        GT_U64 EccMemParams15;
        GT_U64 EccMemParams16;
        GT_U64 EccMemParams17;
        GT_U64 EccMemParams18;
        GT_U64 EccMemParams19;
        GT_U64 EccMemParams20;
        GT_U64 EccMemParams21;
        GT_U64 EccMemParams22;
        GT_U64 EccMemParams23;
        GT_U64 EccMemParams24;
        GT_U64 EccMemParams25;
        GT_U64 EccMemParams26;
        GT_U64 EccMemParams27;
        GT_U64 EccMemParams28;
        GT_U64 EccMemParams29;
        GT_U64 EccMemParams30;
        GT_U64 EccMemParams31;
        GT_U64 EccMemParams32;
        GT_U64 EccMemParams33;
        GT_U64 EccMemParams34;
        GT_U64 EccMemParams35;
        GT_U64 EccMemParams36;
        GT_U64 EccMemParams37;
        GT_U64 EccMemParams38;
        GT_U64 EccMemParams39;
        GT_U64 EccMemParams40;
        GT_U64 EccMemParams41;
        GT_U64 EccMemParams42;
        GT_U64 PortDropPrfWREDParams_CoS[8];
        GT_U64 PortDropPrfWREDScaleRatio_CoS[8];
        GT_U64 PortDropPrfWREDMinThresh_CoS[8];
        GT_U64 PortDropPrfWREDDPRatio_CoS[8];
        GT_U64 PortDropPrfTailDrpThresh_CoS[8];
        GT_U64 PortREDCurve_CoS[8];
        GT_U64 PortDropPrfWREDParamsG;
        GT_U64 PortDropPrfWREDScaleRatioG;
        GT_U64 PortDropPrfWREDMinThreshG;
        GT_U64 PortDropPrfWREDDPRatioG;
        GT_U64 PortDropPrfTailDrpThreshG;
        GT_U64 PortREDCurveG;
        GT_U64 ClvlDropProfPtr_CoS[8];
        GT_U64 ClvlDropPrfWREDParams_CoS[8];
        GT_U64 ClvlDropPrfWREDScaleRatio_CoS[8];
        GT_U64 ClvlDropPrfWREDMinThresh_CoS[8];
        GT_U64 ClvlDropPrfWREDDPRatio_CoS[8];
        GT_U64 ClvlDropPrfTailDrpThresh_CoS[8];
        GT_U64 ClvlREDCurve_CoS[8];
        GT_U64 BlvlDropProfPtr;
        GT_U64 BlvlDropPrfWREDParams;
        GT_U64 BlvlDropPrfWREDScaleRatio;
        GT_U64 BlvlDropPrfWREDMinThresh;
        GT_U64 BlvlDropPrfWREDDPRatio;
        GT_U64 BlvlDropPrfTailDrpThresh;

		GT_U64 BlvlREDCurve_Table[4];
        GT_U64 AlvlDropProfPtr;
        GT_U64 AlvlDropPrfWREDParams;
        GT_U64 AlvlDropPrfWREDScaleRatio;
        GT_U64 AlvlDropPrfWREDMinThresh;
        GT_U64 AlvlDropPrfWREDDPRatio;
        GT_U64 AlvlDropPrfTailDrpThresh;
        GT_U64 AlvlREDCurve_Color[3];

		GT_U64 QueueDropProfPtr;
        GT_U64 QueueDropPrfWREDParams;
        GT_U64 QueueDropPrfWREDScaleRatio;
        GT_U64 QueueDropPrfWREDMinThresh;
        GT_U64 QueueDropPrfWREDDPRatio;
        GT_U64 QueueDropPrfTailDrpThresh;
        GT_U64 QueueREDCurve_Color[3];
        GT_U64 QueueCoSConf;
        GT_U64 PortInstAndAvgQueueLength;
        GT_U64 PortDropProb;
        GT_U64 PortInstAndAvgQueueLengthPerCoS[8];
        GT_U64 PortDropProbPerCoS[8];
        GT_U64 ClvlInstAndAvgQueueLength;
        GT_U64 ClvlDropProb;
        GT_U64 BlvlInstAndAvgQueueLength;
        GT_U64 BlvlDropProb;
        GT_U64 AlvlInstAndAvgQueueLength;
        GT_U64 AlvlDropProb;
        GT_U64 QueueAvgQueueLength;
        GT_U64 QueueDropProb;
    }Drop;
    struct
	{
        GT_U64 DummyResponseMode;
        GT_U64 DecoderFIFOLevel;
        GT_U64 BPSnapshot0;
        GT_U64 BPSnapshot1;
        GT_U64 BPSnapshot2;
        GT_U64 BPSnapshot3;
        GT_U64 BPSnapshotMask0;
        GT_U64 BPSnapshotMask1;
        GT_U64 BPSnapshotMask2;
        GT_U64 BPSnapshotMask3;
        GT_U64 BPSnapshotCnt;
        GT_U64 FIFOOvrflowErrStus;
        GT_U64 CRCErrLog;
        GT_U64 ExtECCErrCtlLog;
        GT_U64 ErrStus;
        GT_U64 FirstExc;
        GT_U64 ErrCnt;
        GT_U64 ExcCnt;
        GT_U64 ExcMask;
        GT_U64 ID;
        GT_U64 ForceErr;
        GT_U64 PerfCntrsPortAlloc;
        GT_U64 PerfCntrsTimerPer;
        GT_U64 NonErrToSMSPktCnt;
        GT_U64 CRCErrPktCnt;
        GT_U64 SMSErrPasPktCnt;
        GT_U64 PortFlushDrpDataPktCnt;
        GT_U64 RCBTxFIFOBp;
        GT_U64 ChunkRdDataDecoderFIFOBp;
        GT_U64 Ecc_Config;
        GT_U64 EccMemParams0;
        GT_U64 ROC_shadow;
        GT_U64 Message_Packet_via_Sync_FIFO;
        GT_U64 ExtECCErrDataLog;
        GT_U64 PerfCntrsSets;
    }RCB;
    struct
	{
        GT_U64 GeneralEccErrStatus;
        GT_U64 ScrubDisable;
        GT_U64 TreeRRDWRRStatus;
        GT_U64 PortPerRateShpPrmsInt;
        GT_U64 PortEccErrStatus;
        GT_U64 PortBankEccErrStatus;
        GT_U64 PortPerStatus;
        GT_U64 ClvlPerRateShpPrmsInt;
        GT_U64 ClvlEccErrStatus;
        GT_U64 ClvlMyQEccErrStatus;
        GT_U64 ClvlBankEccErrStatus;
        GT_U64 ClvlPerStatus;
        GT_U64 BlvlPerRateShpPrmsInt;
        GT_U64 BlvlEccErrStatus;
        GT_U64 BlvlMyQEccErrStatus;
        GT_U64 BlvlBankEccErrStatus;
        GT_U64 BlvlPerStatus;
        GT_U64 AlvlPerRateShpPrmsInt;
        GT_U64 AlvlEccErrStatus;
        GT_U64 AlvlMyQEccErrStatus;
        GT_U64 AlvlBankEccErrStatus;
        GT_U64 AlvlPerStatus;
        GT_U64 QueueEccErrStatus;
        GT_U64 QueuePerRateShpPrmsInt;
        GT_U64 QueueBank0EccErrStatus;
        GT_U64 QueueBank1EccErrStatus;
        GT_U64 QueueBank2EccErrStatus;
        GT_U64 QueueBank3EccErrStatus;
        GT_U64 ErrStus;
        GT_U64 FirstExc;
        GT_U64 ErrCnt;
        GT_U64 ExcCnt;
        GT_U64 ExcMask;
        GT_U64 Id;
        GT_U64 ForceErr;
        GT_U64 ScrubSlotAlloc;
        GT_U64 TMtoTMBpFIFOBp;
        GT_U64 TreeDeqEn;
        GT_U64 TreeDWRRPrioEn;
        GT_U64 PortPerConf;
        GT_U64 PortPerRateShpPrms;
        GT_U64 PortExtBPEn;
        GT_U64 PortDWRRBytesPerBurstsLimit;
        GT_U64 ClvlPerConf;
        GT_U64 ClvlPerRateShpPrms;
        GT_U64 BlvlPerConf;
        GT_U64 BlvlPerRateShpPrms;
        GT_U64 AlvlPerConf;
        GT_U64 AlvlPerRateShpPrms;
        GT_U64 QueuePerConf;
        GT_U64 QueuePerRateShpPrms;
        GT_U64 EccConfig;
        GT_U64 EccMemParams0;
        GT_U64 EccMemParams1;
        GT_U64 EccMemParams2;
        GT_U64 EccMemParams3;
        GT_U64 EccMemParams4;
        GT_U64 EccMemParams5;
        GT_U64 EccMemParams6;
        GT_U64 EccMemParams7;
        GT_U64 EccMemParams8;
        GT_U64 EccMemParams9;
        GT_U64 EccMemParams10;
        GT_U64 EccMemParams11;
        GT_U64 EccMemParams12;
        GT_U64 EccMemParams13;
        GT_U64 EccMemParams14;
        GT_U64 EccMemParams15;
        GT_U64 EccMemParams16;
        GT_U64 EccMemParams17;
        GT_U64 EccMemParams18;
        GT_U64 EccMemParams19;
        GT_U64 EccMemParams20;
        GT_U64 EccMemParams21;
        GT_U64 EccMemParams22;
        GT_U64 EccMemParams23;
        GT_U64 EccMemParams24;
        GT_U64 EccMemParams25;
        GT_U64 EccMemParams26;
        GT_U64 EccMemParams27;
        GT_U64 EccMemParams28;
        GT_U64 EccMemParams29;
        GT_U64 EccMemParams30;
        GT_U64 EccMemParams31;
        GT_U64 EccMemParams32;
        GT_U64 EccMemParams33;
        GT_U64 EccMemParams34;
        GT_U64 EccMemParams35;
        GT_U64 EccMemParams36;
        GT_U64 EccMemParams37;
        GT_U64 EccMemParams38;
        GT_U64 EccMemParams39;
        GT_U64 EccMemParams40;
        GT_U64 EccMemParams41;
        GT_U64 EccMemParams42;
        GT_U64 EccMemParams43;
        GT_U64 EccMemParams44;
        GT_U64 EccMemParams45;
        GT_U64 PortNodeState;
        GT_U64 PortMyQ;
        GT_U64 PortRRDWRRStatusLSB;
        GT_U64 PortRRDWRRStatusMSB;
        GT_U64 PortWFS;
        GT_U64 PortBPFromSTF;
        GT_U64 PortBPFromQMgr;
        GT_U64 ClvlNodeState;
        GT_U64 ClvlMyQ;
        GT_U64 ClvlRRDWRRStatusLSB;
        GT_U64 ClvlRRDWRRStatusMSB;
        GT_U64 ClvlWFS;
        GT_U64 ClvlL0ClusterState;
        GT_U64 ClvlBPFromSTF;
        GT_U64 TMtoTMClvlBPState;
        GT_U64 BlvlNodeState;
        GT_U64 BlvlMyQ;
        GT_U64 BlvlRRDWRRStatusLSB;
        GT_U64 BlvlRRDWRRStatusMSB;
        GT_U64 BlvlWFS;
        GT_U64 BlvlL0ClusterState;
        GT_U64 BlvlL1ClusterStateLo;
        GT_U64 BlvlL1ClusterStateHi;
        GT_U64 AlvlNodeState;
        GT_U64 AlvlMyQ;
        GT_U64 AlvlRRDWRRStatusLSB;
        GT_U64 AlvlRRDWRRStatusMSB;
        GT_U64 AlvlWFS;
        GT_U64 AlvlL0ClusterState;
        GT_U64 AlvlL1ClusterStateLo;
        GT_U64 AlvlL1ClusterStateHi;
        GT_U64 AlvlL2ClusterStateLo;
        GT_U64 AlvlL2ClusterStateHi;
        GT_U64 QueueNodeState;
        GT_U64 QueueWFS;
        GT_U64 QueueL0ClusterState;
        GT_U64 QueueL1ClusterStateLo;
        GT_U64 QueueL1ClusterStateHi;
        GT_U64 QueueL2ClusterStateLo;
        GT_U64 QueueL2ClusterStateHi;
        GT_U64 TMtoTMQueueBPState;
        GT_U64 QueuePerStatus;
        GT_U64 PortEligPrioFunc_Entry;
        GT_U64 PortEligPrioFuncPtr;
        GT_U64 PortTokenBucketTokenEnDiv;
        GT_U64 PortTokenBucketBurstSize;
        GT_U64 PortDWRRPrioEn;
        GT_U64 PortQuantumsPriosLo;
        GT_U64 PortQuantumsPriosHi;
        GT_U64 PortRangeMap;
        GT_U64 ClvlEligPrioFunc_Entry;
        GT_U64 ClvlEligPrioFuncPtr;
        GT_U64 ClvlTokenBucketTokenEnDiv;
        GT_U64 ClvlTokenBucketBurstSize;
        GT_U64 ClvlDWRRPrioEn;
        GT_U64 ClvlQuantum;
        GT_U64 ClvltoPortAndBlvlRangeMap;
        GT_U64 BlvlEligPrioFunc_Entry;
        GT_U64 BlvlEligPrioFuncPtr;
        GT_U64 BlvlTokenBucketTokenEnDiv;
        GT_U64 BlvlTokenBucketBurstSize;
        GT_U64 BlvlDWRRPrioEn;
        GT_U64 BlvlQuantum;
        GT_U64 BLvltoClvlAndAlvlRangeMap;
        GT_U64 AlvlEligPrioFunc_Entry;
        GT_U64 AlvlEligPrioFuncPtr;
        GT_U64 AlvlTokenBucketTokenEnDiv;
        GT_U64 AlvlTokenBucketBurstSize;
        GT_U64 AlvlDWRRPrioEn;
        GT_U64 AlvlQuantum;
        GT_U64 ALvltoBlvlAndQueueRangeMap;
        GT_U64 QueueEligPrioFunc;
        GT_U64 QueueEligPrioFuncPtr;
        GT_U64 QueueTokenBucketTokenEnDiv;
        GT_U64 QueueTokenBucketBurstSize;
        GT_U64 QueueQuantum;
        GT_U64 QueueAMap;
        GT_U64 PortShpBucketLvls;
        GT_U64 PortDefPrioHi;
        GT_U64 PortDefPrioLo;
        GT_U64 ClvlShpBucketLvls;
        GT_U64 CLvlDef;
        GT_U64 BlvlShpBucketLvls;
        GT_U64 BlvlDef;
        GT_U64 AlvlShpBucketLvls;
        GT_U64 AlvlDef;
        GT_U64 QueueShpBucketLvls;
        GT_U64 QueueDef;
    }Sched;
    struct
	{
        GT_U64 ErrStus;
        GT_U64 FirstExc;
        GT_U64 ErrCnt;
        GT_U64 ExcCnt;
        GT_U64 ExcMask;
        GT_U64 Id;
        GT_U64 ForceErr;
        GT_U64 TMtoTMEn;
        GT_U64 TMtoTMPktCRCEn;
        GT_U64 QmapHeaderCont;
        GT_U64 QmapHeaderEn;
        GT_U64 ExtHeadersSize;
        GT_U64 TMHeader;
        GT_U64 ContHeader;
        GT_U64 TMtoTMPktStructure;
        GT_U64 Delta2PrioMap;
        GT_U64 TMPort;
        GT_U64 RemoteDPMode;
        GT_U64 TMtoTMEgTMLevelMap;
        GT_U64 TMtoTMElementsPerEgTMLevel;
        GT_U64 TM2TMGenModePerLevel;
        GT_U64 TMtoTMBaseOffsetPerIngTMLevel;
        GT_U64 TMtoTMEgTMBPLevelMap;
        GT_U64 TM2TMXonXoffThresh;
        GT_U64 RespRemoteDPSel;
        GT_U64 EccConfig;
        GT_U64 EccMemParams0;
        GT_U64 EccMemParams1;
        GT_U64 EccMemParams2;
        GT_U64 EccMemParams3;
        GT_U64 ExtHeadersContent;
        GT_U64 PortDropProb;
        GT_U64 NodeDropProb;
    }TM2TM;
	/* added manually   - only necessary registers */
   struct /*BAP[10]*/{
        GT_U64    ErrStus;
        GT_U64    FirstExc;
        GT_U64    ErrCnt;
        GT_U64    ExcCnt;
        GT_U64    ExcMask;
        GT_U64    ID;
        GT_U64    ForceErr;
        GT_U64    MgmtEvtStus;
        GT_U64    MgmtEvtMask;
        GT_U64    ReqAcssMode;
        GT_U64    ReqAcssModeFirstContWrEn;
        GT_U64    OEMode;
        GT_U64    MaxReads;
        GT_U64    MaxReadsPanic;
        GT_U64    MaxWrites;
        GT_U64    MaxWritesPanic;
        GT_U64    tRC;
        GT_U64    tFAW;
        GT_U64    tRRD;
        GT_U64    RdApToActDelay;
        GT_U64    WrApToActDelay;
        GT_U64    Read2WriteNOPs;
        GT_U64    Write2ReadNOPs;
        GT_U64    AfterRefNOPs;
        GT_U64    RefInterval;
        GT_U64    RefCalNRatio;
        GT_U64    AfterCalNOPs;
        GT_U64    WritePanicThresh;
        GT_U64    ActRdReqCode;
        GT_U64    RdApReqCode;
        GT_U64    RdReqCode;
        GT_U64    ActRdApReqCode;
        GT_U64    ActWrReqCode;
        GT_U64    WrApReqCode;
        GT_U64    WrReqCode;
        GT_U64    ActWrApReqCode;
        GT_U64    IdleReqCode;
        GT_U64    RefReqCode;
        GT_U64    CalReqCode;
        GT_U64    LADRdReqCode;
        GT_U64    LADWrReqCode;
        GT_U64    ActWrFirstReqCode;
        GT_U64    ActWrApFirstReqCode;
        GT_U64    LADNormalReqPrms;
        GT_U64    LADXCMReqPrms;
        GT_U64    ReqEngAddr;
        GT_U64    XCMAccsLADReqAddrCtl;
        GT_U64    XCMAccsLADRespStus;
        GT_U64    XCMAccsDRAMReqAddrCtl;
        GT_U64    XCMAccsDRAMRespStus;
        GT_U64    DramDataWrFIFOsBp;
        GT_U64    DramDataRdFIFOsBp;
        GT_U64    DramCtlWrFIFOsBp;
        GT_U64    DramCtlRdFIFOsBp;
        GT_U64    RCBRdLLFIFOBp;
        GT_U64    ReqTypeAttrFIFOBp;
        GT_U64    GlobalBPThresh;
        GT_U64    ReqShpPrms;
        GT_U64    BAPPHYWritePath;
		GT_U64    BAPPHYSamplePath;
        GT_U64    BAPPHYReadPath;	        
	    /* Tables */
        GT_U64    XCMAccsLADReqWrData;
        GT_U64    XCMAccsLADRespData;
        GT_U64    XCMAccsDRAMReqWrData;
        GT_U64    XCMAccsDRAMRespData;
        GT_U64    PortBPThresh;
    }BAP[TM_MAX_BAP]/*index*/;

};

extern struct tm_alias TM;

void init_tm_alias_struct(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif 
