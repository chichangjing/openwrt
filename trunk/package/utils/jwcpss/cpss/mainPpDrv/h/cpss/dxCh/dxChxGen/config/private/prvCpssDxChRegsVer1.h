/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChRegsVer1h.h
*
* DESCRIPTION:
*       This file includes the declaration of the structure to hold the
*       addresses of eArch PP registers for each different PP type.
*
*
* FILE REVISION NUMBER:
*       $Revision: 63 $
*
*******************************************************************************/
#ifndef __prvCpssDxChRegsVer1h
#define __prvCpssDxChRegsVer1h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
#include <cpss/generic/port/private/prvCpssPortTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChRegs.h>

#define REGISTER_COVERED_CNS

/* 512 registers for per port register */
#define PORTS_NUM_CNS       512
#define PORTS_BMP_NUM_CNS   ((PORTS_NUM_CNS + 31) / 32)

/* short name to TTI unit registers */
#define PRV_DXCH_REG1_UNIT_TTI_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TTI

/* short name to L2I unit registers */
#define PRV_DXCH_REG1_UNIT_L2I_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->L2I

/* short name to FDB unit registers */
#define PRV_DXCH_REG1_UNIT_FDB_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->FDB

/* short name to EQ unit registers */
#define PRV_DXCH_REG1_UNIT_EQ_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->EQ

/* short name to LPM unit registers */
#define PRV_DXCH_REG1_UNIT_LPM_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->LPM

/* short name to EGF_eft unit registers */
#define PRV_DXCH_REG1_UNIT_EGF_EFT_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->EGF_eft

/* short name to EGF_qag unit registers */
#define PRV_DXCH_REG1_UNIT_EGF_QAG_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->EGF_qag

/* short name to EGF_sht unit registers */
#define PRV_DXCH_REG1_UNIT_EGF_SHT_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->EGF_sht

/* short name to HA unit registers */
#define PRV_DXCH_REG1_UNIT_HA_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->HA

/* short name to ETS unit registers */
#define PRV_DXCH_REG1_UNIT_ETS_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->HA.ETS_temp

/* short name to MLL unit registers */
#define PRV_DXCH_REG1_UNIT_MLL_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->MLL

/* short name to PCL unit (IPCL) registers */
#define PRV_DXCH_REG1_UNIT_PCL_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->PCL

/* short name to EPCL unit (EPCL) registers */
#define PRV_DXCH_REG1_UNIT_EPCL_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->EPCL

/* short name to RXDMA unit registers */
#define PRV_DXCH_REG1_UNIT_RXDMA_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->rxDMA[0]

/* short name to RXDMA specific unit registers */
#define PRV_DXCH_REG1_UNIT_RXDMA_INDEX_MAC(_devNum,index)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->rxDMA[index]

/* short name to TXDMA unit registers */
#define PRV_DXCH_REG1_UNIT_TXDMA_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->txDMA[0]

/* short name to TXDMA specific unit registers */
#define PRV_DXCH_REG1_UNIT_TXDMA_INDEX_MAC(_devNum,index)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->txDMA[index]

/* short name to TX_FIFO unit registers */
#define PRV_DXCH_REG1_UNIT_TX_FIFO_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->txFIFO[0]

/* short name to TX_FIFO specific unit registers */
#define PRV_DXCH_REG1_UNIT_TX_FIFO_INDEX_MAC(_devNum,index)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->txFIFO[index]


/* short name to SIP_ETH_TXFIFO unit registers */
#define PRV_DXCH_REG1_UNIT_ETH_TXFIFO_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->SIP_ETH_TXFIFO

/* short name to SIP_ILKN_TXFIFO unit registers */
#define PRV_DXCH_REG1_UNIT_ILKN_TXFIFO_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->SIP_ILKN_TXFIFO


/* short name to ERMRK unit registers */
#define PRV_DXCH_REG1_UNIT_ERMRK_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->ERMRK

/* short name to OAM unit registers :
    _stage - 0 --> ingress    (CPSS_DXCH_OAM_STAGE_TYPE_INGRESS_E)
    _stage - 1 --> egress     (CPSS_DXCH_OAM_STAGE_TYPE_EGRESS_E)
*/
#define PRV_DXCH_REG1_UNIT_OAM_MAC(_devNum,_stage)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->OAMUnit[_stage]

/* TAI subunit                                                  */
/* _gop - Group Of Ports (port number can be converted to _gop) */
/* _id - TAI id in the GOP (0 or 1)                             */
#define PRV_DXCH_REG1_UNIT_GOP_TAI_MAC(_devNum, _gop, _id)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->GOP.TAI[_gop][_id]

/* PTP (TSU) subunit   */
/* _port - port number */
#define PRV_DXCH_REG1_UNIT_GOP_PTP_MAC(_devNum, _port)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->GOP.PTP[_port]

/* FCA subunit   */
/* _port - port/mac number */
#define PRV_DXCH_REG1_UNIT_GOP_FCA_MAC(_devNum, _port)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->GOP.FCA[_port]

#define PRV_DXCH_REG_UNIT_GOP_FCA_MAC(_devNum, _port)   \
    PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->macRegs.perPortRegs[_port].FCA

#define PRV_DXCH_REG_UNIT_GOP_CPU_FCA_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->macRegs.cpuPortRegs.FCA

/* short name to PLR unit (PLR) registers */
#define PRV_DXCH_REG1_UNIT_ILKN_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->GOP.ILKN

/* short name to PLR unit (PLR) registers */
#define PRV_DXCH_REG1_UNIT_PLR_MAC(_devNum,_stage)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->PLR[_stage]

/* short name to CNC unit registers */
#define PRV_DXCH_REG1_UNIT_CNC_MAC(_devNum, _instance)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->CNC[_instance]


/* short name to TXQ_DQ unit registers */
#define PRV_DXCH_REG1_UNIT_TXQ_DQ_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TXQ.dq[0]

/* short name to TXQ_DQ specific unit registers */
#define PRV_DXCH_REG1_UNIT_TXQ_DQ_INDEX_MAC(_devNum,index)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TXQ.dq[index]


/* short name to TXQ_LL unit registers */
#define PRV_DXCH_REG1_UNIT_TXQ_LL_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TXQ.ll

/* short name to TXQ_PFC unit registers */
#define PRV_DXCH_REG1_UNIT_TXQ_PFC_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TXQ.pfc

/* short name to TXQ_QCN unit registers */
#define PRV_DXCH_REG1_UNIT_TXQ_QCN_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TXQ.qcn

/* short name to TXQ_Q unit registers */
#define PRV_DXCH_REG1_UNIT_TXQ_Q_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TXQ.queue

/* short name to TM DROP unit registers */
#define PRV_DXCH_REG1_UNIT_TM_DROP_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TMDROP

/* short name to LMS unit registers */
#define PRV_DXCH_REG1_UNIT_LMS_MAC(_devNum,_instance)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->GOP.LMS[_instance]

/* short name to MG unit registers */
#define PRV_DXCH_REG1_UNIT_MG_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->MG

/* short name to TM QMAP unit registers */
#define PRV_DXCH_REG1_UNIT_TM_QMAP_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TMQMAP

/* short name to TM_INGR_GLUE unit registers */
#define PRV_DXCH_REG1_UNIT_TM_INGR_GLUE_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TM_INGR_GLUE

/* short name to TM_EGR_GLUE unit registers */
#define PRV_DXCH_REG1_UNIT_TM_EGR_GLUE_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TM_EGR_GLUE

/* short name to TM_FCU unit registers */
#define PRV_DXCH_REG1_UNIT_TM_FCU_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->TM_FCU

/* short name to BM unit registers */
#define PRV_DXCH_REG1_UNIT_BM_MAC(_devNum)   \
    PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(_devNum)->BM



typedef struct PRV_CPSS_DXCH_PP_DP_PIZZA_ARBITER_STCT
{
    GT_U32    pizzaArbiterConfigReg[85];
    GT_U32    pizzaArbiterCtrlReg;
    GT_U32    pizzaArbiterStatusReg;
}PRV_CPSS_DXCH_PP_DP_PIZZA_ARBITER_STC;

typedef struct PRV_CPSS_DXCH_PP_DP_GLUE_PIZZA_ARBITER_STCT
{
    GT_U32    pizzaArbiterConfigReg[37];
    GT_U32    pizzaArbiterCtrlReg;
    GT_U32    pizzaArbiterStatusReg;
}PRV_CPSS_DXCH_PP_DP_GLUE_PIZZA_ARBITER_STC;

/*--------------------------------------
 * /Cider/EBU/BobK/BobK {Current}/Switching Core/MPPM/Units/<MPPM_IP> MPPM IP Units/<Pizza_arb> SIP_COMMON_MODULES_IP Units/Pizza Arbiter/Pizza Arbiter Configuration Register %n
 *    Register CPU 0x47000200 Pizza Arbiter Control Register
 *    Register CPU 0x47000204 Pizza Arbiter Status
 *    Register CPU 0x47000208 Pizza Arbiter Configuration Register %n (0x47000208 + n*0x4 : where n (0-92) represents reg number)
 *    Register CPU 0x47000400 Pizza Arbiter debug register (Internal)
 *-----------------------------------------*/
typedef struct PRV_CPSS_DXCH_PP_MPPM_PIZZA_ARBITER_STCT
{
    GT_U32    pizzaArbiterConfigReg[93];
    GT_U32    pizzaArbiterCtrlReg;
    GT_U32    pizzaArbiterStatusReg;
}PRV_CPSS_DXCH_PP_MPPM_PIZZA_ARBITER_STC;


/*
 * Typedef: struct PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC
 *
 * Description: A struct including PP registers addresses for register access
 *              by different types of PPs.
 *
 * Fields: The struct includes fields by the names of PP registers, each
 *         field represents the respective register address.
 *
 * Notes:
 *          format of structure is similar to PRV_CPSS_DXCH_PP_REGS_ADDR_STC
 *
 */

typedef struct
{
    struct /*TTI*/{

        struct /*VLANAssignment*/{

            GT_U32    ingrTPIDConfig[4]/*VLAN_Ethertype_index*/;
            GT_U32    ingrTagProfileTPIDSelect[2][2]/*Tag*//*Profile group*/;
            GT_U32    ingrTPIDTagType;

        }VLANAssignment;

        struct /*userDefinedBytes*/{

            GT_U32    IPCLTCPPortRangeComparatorConfig[8]/*TCPCmp*/;
            GT_U32    IPCLUDPPortRangeComparatorConfig[8]/*UDPCmp*/;

        }userDefinedBytes;

        struct /*TTIUnitGlobalConfigs*/{

            GT_U32    TTIUnitGlobalConfig;
            GT_U32    TTIUnitGlobalConfigForCCPorts;
            GT_U32    TTIUnitGlobalConfigExt;

        }TTIUnitGlobalConfigs;

        struct /*TTIEngine*/{

            GT_U32    TTIEngineInterruptCause;
            GT_U32    TTIEngineInterruptMask;
            GT_U32    TTIEngineConfig;
            GT_U32    TTIPCLIDConfig0;
            GT_U32    TTIPCLIDConfig1;
            GT_U32    TTILookupIPExceptionCommands;
            GT_U32    TTIPktTypeKeys[8]/*packet types*/;
            GT_U32    TTILookupIPExceptionCodes0;
            GT_U32    TTIKeySegmentMode0;
            GT_U32    TTIKeySegmentMode1;
            GT_U32    TTIKeyType;
            GT_U32    TTILookupMPLSExceptionCommands;
            GT_U32    TTILookupIPExceptionCodes1;

        }TTIEngine;

        struct /*trunkHashSimpleHash*/{

            GT_U32    trunkHashConfigReg0;
            GT_U32    trunkHashConfigReg1;
            GT_U32    trunkHashConfigReg2;

        }trunkHashSimpleHash;

        struct /*TRILLConfigs*/{

            GT_U32    TRILLGlobalConfigs0;
            GT_U32    TRILLGlobalConfigs1;
            GT_U32    TRILLExceptionCommands0;
            GT_U32    TRILLExceptionCommands1;
            GT_U32    TRILLGlobalConfigs2;

        }TRILLConfigs;

        struct /*TRILLAdjacency*/{

            GT_U32    adjacencyEntryReg2[128]/*entry*/;
            GT_U32    adjacencyEntryReg1[128]/*entry*/;
            GT_U32    adjacencyEntryReg0[128]/*entry*/;

        }TRILLAdjacency;

        struct /*PTP*/{

            GT_U32    _1588V1DomainDomainNumber[4][4]/*i*//*Domain*/;
            GT_U32    PTPExceptionCheckingModeDomain[5]/*Domain*/;
            GT_U32    piggyBackTSConfigs[8]/*group of ports*/;
            GT_U32    PTPConfigs;
            GT_U32    PTPOverUDPDestinationPorts;
            GT_U32    PTPExceptionsAndCPUCodeConfig;
            GT_U32    PTPEtherTypes;
            GT_U32    PTPExceptionsCntr;
            GT_U32    _1588V2DomainDomainNumber;
            GT_U32    timestampEtherTypes;
            GT_U32    timestampConfigs;

        }PTP;

        struct /*pseudoWire*/{

            GT_U32    MPLSOAMChannelTypeProfile[15]/*channel type*/;
            GT_U32    PWE3Config;
            GT_U32    PWTagModeExceptionConfig;

        }pseudoWire;

        struct /*protocolMatch*/{

            GT_U32    protocolsConfig[6]/*Protocols*/;
            GT_U32    protocolsEncapsulationConfig0;
            GT_U32    protocolsEncapsulationConfig1;

        }protocolMatch;

        struct /*portMap*/{

            GT_U32    portGroupMap[32]/*8 ports*/;
            GT_U32    portListMap[43]/*6 ports*/;

        }portMap;

        struct /*phyPortAndEPortAttributes*/{

            GT_U32    myPhysicalPortAttributes[256]/*Port*/;
            GT_U32    remotePhysicalPortAssignmentGlobalConfig;

        }phyPortAndEPortAttributes;

        struct /*MRUException*/{

            GT_U32    MRUSize[4]/*2 profiles*/;
            GT_U32    MRUCommandAndCode;

        }MRUException;

        struct /*MAC2ME*/{

            GT_U32    MAC2MEHighMask[128]/*Number of MAC*/;
            GT_U32    MAC2MELow[128]/*Number of MAC*/;
            GT_U32    MAC2MEHigh[128]/*Number of MAC*/;
            GT_U32    MAC2MELowMask[128]/*Number of MAC*/;
            GT_U32    MAC2MESrcInterface[128]/*Mac2Me entry*/;
            GT_U32    MAC2MESrcInterfaceSel[128]/*Mac2Me Entry*/;

        }MAC2ME;

        struct /*IPv6SolicitedNode*/{

            GT_U32    IPv6SolicitedNodeMcAddrConfigReg0;
            GT_U32    IPv6SolicitedNodeMcAddrConfigReg1;
            GT_U32    IPv6SolicitedNodeMcAddrConfigReg2;
            GT_U32    IPv6SolicitedNodeMcAddrConfigReg3;
            GT_U32    IPv6SolicitedNodeMcAddrMaskReg0;
            GT_U32    IPv6SolicitedNodeMcAddrMaskReg1;
            GT_U32    IPv6SolicitedNodeMcAddrMaskReg2;
            GT_U32    IPv6SolicitedNodeMcAddrMaskReg3;

        }IPv6SolicitedNode;

        struct /*FCOE*/{

            GT_U32    FCoEGlobalConfig;
            GT_U32    FCoEExceptionConfigs;
            GT_U32    FCoEExceptionCntrs0;
            GT_U32    FCoEExceptionCntrs1;
            GT_U32    FCoEExceptionCntrs2;

        }FCOE;

        struct /*duplicationConfigs*/{

            GT_U32    duplicationConfigs0;
            GT_U32    duplicationConfigs1;
            GT_U32    MPLSMCDALow;
            GT_U32    MPLSMCDAHigh;
            GT_U32    MPLSMCDALowMask;
            GT_U32    MPLSMCDAHighMask;

        }duplicationConfigs;

        GT_U32    TTIKRuleMatchCntr[12]/*TTICounter*/;
        GT_U32    TTIUserDefinedEtherTypes[4]/*index*/;
        GT_U32    TTIIPv6GREEtherType;
        GT_U32    TTIIPv4GREEthertype;
        GT_U32    oamLabelGlobalConfiguration0;
        GT_U32    oamLabelGlobalConfiguration1;
        GT_U32    specialEtherTypes;
        GT_U32    sourceIDAssignment0;
        GT_U32    sourceIDAssignment1;
        GT_U32    receivedFlowCtrlPktsCntr;
        GT_U32    preRouteExceptionMasks;
        GT_U32    PCLUserDefinedEtherTypes[4]/*index*/;
        GT_U32    MPLSEtherTypes;
        GT_U32    IPv6ExtensionValue;
        GT_U32    IPMinimumOffset;
        GT_U32    IPLengthChecks;
        GT_U32    ECCTwoErrorsCntr;
        GT_U32    ECCOneErrorCntr;
        GT_U32    DSAConfig;
        GT_U32    droppedFlowCtrlPktsCntr;
        GT_U32    CNAndFCConfig;
        GT_U32    CFMEtherType;
        GT_U32    TTIMetalFix;

    }TTI;


    struct /*L2I*/{

        struct /*VLANRangesConfig*/{

REGISTER_COVERED_CNS            GT_U32    ingrVLANRangeConfig;

        }VLANRangesConfig;

        struct /*VLANMRUProfilesConfig*/{

REGISTER_COVERED_CNS            GT_U32    VLANMRUProfileConfig[4]/*Register*/;

        }VLANMRUProfilesConfig;

        struct /*UDPBcMirrorTrapUDPRelayConfig*/{

REGISTER_COVERED_CNS            GT_U32    UDPBcDestinationPortConfigTable[12]/*Port*/;

        }UDPBcMirrorTrapUDPRelayConfig;

        struct /*securityBreachStatus*/{

REGISTER_COVERED_CNS            GT_U32    securityBreachStatus0;
REGISTER_COVERED_CNS            GT_U32    securityBreachStatus1;
REGISTER_COVERED_CNS            GT_U32    securityBreachStatus2;
REGISTER_COVERED_CNS            GT_U32    securityBreachStatus3;

        }securityBreachStatus;

        struct /*MACBasedQoSTable*/{

REGISTER_COVERED_CNS            GT_U32    MACQoSTableEntry[7]/*QoS Profile*/;

        }MACBasedQoSTable;

        struct /*layer2BridgeMIBCntrs*/{

REGISTER_COVERED_CNS            GT_U32    setVLANIngrFilteredPktCount[2]/*SetNum*/;
REGISTER_COVERED_CNS            GT_U32    cntrsSetConfig0[2]/*Set*/;
REGISTER_COVERED_CNS            GT_U32    cntrsSetConfig1[2]/*Set*/;
REGISTER_COVERED_CNS            GT_U32    setIncomingPktCount[2]/*SetNum*/;
REGISTER_COVERED_CNS            GT_U32    setSecurityFilteredPktCount[2]/*SetNum*/;
REGISTER_COVERED_CNS            GT_U32    setBridgeFilteredPktCount[2]/*SetNum*/;
REGISTER_COVERED_CNS            GT_U32    MACAddrCount0;
REGISTER_COVERED_CNS            GT_U32    MACAddrCount1;
REGISTER_COVERED_CNS            GT_U32    MACAddrCount2;
REGISTER_COVERED_CNS            GT_U32    hostOutgoingMcPktCount;
REGISTER_COVERED_CNS            GT_U32    hostOutgoingPktsCount;
REGISTER_COVERED_CNS            GT_U32    hostIncomingPktsCount;
REGISTER_COVERED_CNS            GT_U32    hostOutgoingBcPktCount;
REGISTER_COVERED_CNS            GT_U32    matrixSourceDestinationPktCount;

        }layer2BridgeMIBCntrs;

        struct /*IPv6McBridgingBytesSelection*/{

REGISTER_COVERED_CNS            GT_U32    IPv6MCBridgingBytesSelectionConfig;

        }IPv6McBridgingBytesSelection;

        struct /*ingrPortsRateLimitConfig*/{

            GT_U32    portSpeedOverrideEnable[8]/*Register*/;
REGISTER_COVERED_CNS            GT_U32    MIISpeedGranularity[8]/*Register*/;
REGISTER_COVERED_CNS            GT_U32    GMIISpeedGranularity[8]/*Register*/;
REGISTER_COVERED_CNS            GT_U32    XGSpeedGranularity[8]/*Register*/;
REGISTER_COVERED_CNS            GT_U32    CGSpeedGranularity[8]/*Register*/;
REGISTER_COVERED_CNS            GT_U32    ingrRateLimitConfig0;
            GT_U32    ingrRateLimitConfig3;
            GT_U32    ingrRateLimitConfig2;
REGISTER_COVERED_CNS            GT_U32    ingrRateLimitConfig1;
REGISTER_COVERED_CNS            GT_U32    ingrRateLimitDropCntr310;
REGISTER_COVERED_CNS            GT_U32    ingrRateLimitDropCntr3932;

        }ingrPortsRateLimitConfig;

        struct /*globalEportConifguration*/{

REGISTER_COVERED_CNS            GT_U32    globalEPortValue;
REGISTER_COVERED_CNS            GT_U32    globalEPortMask;

        }globalEportConifguration;

        struct /*ctrlTrafficToCPUConfig*/{

REGISTER_COVERED_CNS            GT_U32    IPv6McLinkLocalCPUCodeIndex[8][2]/*CPU Code Index_n*//*CPU Code Index_t*/;
            GT_U32    IPv6McLinkLocalReged[8]/*Register*/;
REGISTER_COVERED_CNS            GT_U32    IPv6McLinkLocalConfig[32]/*Register*/;
REGISTER_COVERED_CNS            GT_U32    IPv6ICMPMsgTypeConfig[2]/*ICMP_Msg_Type*/;
REGISTER_COVERED_CNS            GT_U32    IPv6ICMPCommand;
            GT_U32    IPv4McLinkLocalReged[8]/*Register*/;
REGISTER_COVERED_CNS            GT_U32    IPv4McLinkLocalCPUCodeIndex[8][2]/*Register*//*Register_t*/;
REGISTER_COVERED_CNS            GT_U32    IPv4McLinkLocalConfig[32]/*Register*/;
            GT_U32    IEEEReservedMcReged[8]/*Register*/;
REGISTER_COVERED_CNS/*base from IEEEReservedMcConfig0*/            GT_U32    IEEEReservedMcCPUIndex[16]/*Register*/;
REGISTER_COVERED_CNS/*base from IEEEReservedMcConfig0*/            GT_U32    IEEEReservedMcConfig7[16]/*Register*/;
REGISTER_COVERED_CNS/*base from IEEEReservedMcConfig0*/            GT_U32    IEEEReservedMcConfig6[16]/*Register*/;
REGISTER_COVERED_CNS/*base from IEEEReservedMcConfig0*/            GT_U32    IEEEReservedMcConfig5[16]/*Register*/;
REGISTER_COVERED_CNS/*base from IEEEReservedMcConfig0*/            GT_U32    IEEEReservedMcConfig4[16]/*Register*/;
REGISTER_COVERED_CNS/*base from IEEEReservedMcConfig0*/            GT_U32    IEEEReservedMcConfig3[16]/*Register*/;
REGISTER_COVERED_CNS/*base from IEEEReservedMcConfig0*/            GT_U32    IEEEReservedMcConfig2[16]/*Register*/;
REGISTER_COVERED_CNS/*base from IEEEReservedMcConfig0*/            GT_U32    IEEEReservedMcConfig1[16]/*Register*/;
REGISTER_COVERED_CNS            GT_U32    IEEEReservedMcConfig0[16]/*Register*/;

        }ctrlTrafficToCPUConfig;

        struct /*bridgeEngineInterrupts*/{

REGISTER_COVERED_CNS            GT_U32    bridgeInterruptCause;   /*prvCpssDrvPpIntDefDxChLion2Init*/
REGISTER_COVERED_CNS            GT_U32    bridgeInterruptMask;    /*prvCpssDrvPpIntDefDxChLion2Init*/

        }bridgeEngineInterrupts;

        struct /*bridgeEngineConfig*/{

REGISTER_COVERED_CNS            GT_U32    bridgeGlobalConfig0;
REGISTER_COVERED_CNS            GT_U32    bridgeGlobalConfig1;
REGISTER_COVERED_CNS            GT_U32    bridgeCommandConfig0;
REGISTER_COVERED_CNS            GT_U32    bridgeCommandConfig1;
            GT_U32    bridgeCommandConfig2;
REGISTER_COVERED_CNS            GT_U32    bridgeCommandConfig3;
REGISTER_COVERED_CNS            GT_U32    bridgeSecurityBreachDropCntrCfg0;
REGISTER_COVERED_CNS            GT_U32    bridgeSecurityBreachDropCntrCfg1;
            GT_U32    bridgecpuPortConfig;

        }bridgeEngineConfig;

        struct /*bridgeDropCntrAndSecurityBreachDropCntrs*/{

REGISTER_COVERED_CNS            GT_U32    globalSecurityBreachFilterCntr;
REGISTER_COVERED_CNS            GT_U32    portVLANSecurityBreachDropCntr;
REGISTER_COVERED_CNS            GT_U32    bridgeFilterCntr;

        }bridgeDropCntrAndSecurityBreachDropCntrs;

        struct /*bridgeAccessMatrix*/{

REGISTER_COVERED_CNS            GT_U32    bridgeAccessMatrixLine[8]/*SA Access Level*/;
REGISTER_COVERED_CNS            GT_U32    bridgeAccessMatrixDefault;

        }bridgeAccessMatrix;

        struct /*eVlanMirroringToAnalyser*/{

                GT_U32 eVlanMirroringToAnalyserEnable[8];

        }eVlanMirroringToAnalyser;
        GT_U32    l2iBadAddressLatchReg;
    }L2I;

    struct /*FDB*/{

        struct /*FDBCore*/{

            struct /*sourceAddrNotLearntCntr*/{

                GT_U32    learnedEntryDiscardsCount;

            }sourceAddrNotLearntCntr;

            struct /*FDB*/{

                GT_U32    MTCPipeDispatcher;

            }FDB;

            struct /*FDBInterrupt*/{

                GT_U32    FDBInterruptCauseReg;
                GT_U32    FDBInterruptMaskReg;

            }FDBInterrupt;

            struct /*FDBInternal*/{

                GT_U32    metalFix;
                GT_U32    FDBIdleState;

            }FDBInternal;

            struct /*FDBIndirectAccess*/{

                GT_U32    FDBIndirectAccessData[5]/*word*/;
                GT_U32    FDBIndirectAccessCtrl;
                GT_U32    FDBIndirectAccessAccess;

            }FDBIndirectAccess;

            struct /*FDBHashResults*/{

                GT_U32    FDBMultiHashCRCResultReg[8]/*reg_num*/;
                GT_U32    FDBNonMulti_hash_crc_result;
                GT_U32    FDBXorHash;

            }FDBHashResults;

            struct /*FDBGlobalConfig*/{

                GT_U32    FDBGlobalConfig;
                GT_U32    FDBGlobalConfig1;
                GT_U32    FDBCRCHashInitConfig;
                GT_U32    bridgeAccessLevelConfig;
                GT_U32    FDBAgingWindowSize;

            }FDBGlobalConfig;

            struct /*FDBCntrsUpdate*/{

                GT_U32    FDBCntrsUpdateCtrl;

            }FDBCntrsUpdate;

            struct /*FDBBankCntrs*/{

                GT_U32    FDBBankCntr[16]/*BankNum*/;
                GT_U32    FDBBankRank0;
                GT_U32    FDBBankRank1;
                GT_U32    FDBBankRank2;
                GT_U32    FDBBankRank3;
                GT_U32    FDBBankRank4;
                GT_U32    FDBBankRank5;
                GT_U32    FDBBankRank6;
                GT_U32    FDBBankRank7;
                GT_U32    FDBBankRank8;
                GT_U32    FDBBankRank9;
                GT_U32    FDBBankRank13;
                GT_U32    FDBBankRank12;
                GT_U32    FDBBankRank11;
                GT_U32    FDBBankRank10;
                GT_U32    FDBBankRank14;
                GT_U32    FDBBankRank15;
                GT_U32    FDBBankCntrsCtrl;

            }FDBBankCntrs;

            struct /*FDBAddrUpdateMsgs*/{

                struct /*AUMsgToCPU*/{

                    GT_U32    AUMsgToCPU;
                    GT_U32    AUFIFOToCPUConfig;

                }AUMsgToCPU;

                struct /*AUMsgFromCPU*/{

                    GT_U32    AUMsgFromCPUWord[6]/*word*/;
                    GT_U32    AUMsgFromCPUCtrl;

                }AUMsgFromCPU;

            }FDBAddrUpdateMsgs;

            struct /*FDBAction*/{

                GT_U32    FDBActionGeneral;
                GT_U32    FDBAction0;
                GT_U32    FDBAction1;
                GT_U32    FDBAction2;
                GT_U32    FDBAction3;
                GT_U32    FDBAction4;
                GT_U32    FDBActionOldParameters;
                GT_U32    FDBActionNewParameters;

            }FDBAction;

            struct /*AUMsgRateLimitingCounter*/{

                GT_U32    AUMsgRateLimCnt;

            } AUMsgRateLimitingCounter;

            struct /*FDBScedulerConfiguration*/{

                GT_U32    FDBScedulerSWRRControl;
                GT_U32    FDBScedulerSWRRConfig;

            } FDBScedulerConfiguration;

        }FDBCore;

    }FDB;

    struct /*EQ*/{

        struct /*toCpuPktRateLimiters*/{

            GT_U32    CPUCodeRateLimiterDropCntr;
            GT_U32    toCpuRateLimiterPktCntr[255]/*Limiter*/;

        }toCpuPktRateLimiters;

        struct /*SCTRateLimitersInterrupt*/{

            GT_U32    CPUCodeRateLimitersInterruptCause[32]/*reg*/;
            GT_U32    CPUCodeRateLimitersInterruptMask[32]/*reg*/;
            GT_U32    CPUCodeRateLimitersInterruptSummary;
            GT_U32    CPUCodeRateLimitersInterruptSummaryMask;

        }SCTRateLimitersInterrupt;

        struct /*preEgrInterrupt*/{

            GT_U32    preEgrInterruptSummary;
            GT_U32    preEgrInterruptSummaryMask;

        }preEgrInterrupt;

        struct /*preEgrEngineGlobalConfig*/{

            GT_U32    duplicationOfPktsToCPUConfig[16]/*port*/;
            GT_U32    MTUTableEntry[4]/*port*/;
            GT_U32    preEgrEngineGlobalConfig;
            GT_U32    MTUGlobalConfig;
            GT_U32    lossyDropConfig;
            GT_U32    cscdTrunkHashBitSelectionConfig;
            GT_U32    targetEPortMTUExceededCntr;

        }preEgrEngineGlobalConfig;

        struct /*mirrToAnalyzerPortConfigs*/{

            GT_U32    mirrorInterfaceParameterReg[7]/*Parameter Number*/;
            GT_U32    analyzerPortGlobalConfig;
            GT_U32    ingrStatisticMirrToAnalyzerPortConfig;
            GT_U32    ingrAndEgrMonitoringToAnalyzerQoSConfig;

        }mirrToAnalyzerPortConfigs;

        struct /*L2ECMP*/{

            GT_U32    ePortECMPEPortValue;
            GT_U32    ePortECMPEPortMask;
            GT_U32    ePortECMPLTTIndexBaseEPort;
            GT_U32    ePortECMPLFSRConfig;
            GT_U32    ePortECMPLFSRSeed;
            GT_U32    ePortECMPHashBitSelectionConfig;
            GT_U32    trunkLFSRConfig;
            GT_U32    trunkLFSRSeed;
            GT_U32    trunkHashBitSelectionConfig;

        }L2ECMP;

        struct /*ingrSTCInterrupt*/{

            GT_U32    ingrSTCInterruptCause[32]/*Port*/;
            GT_U32    ingrSTCInterruptMask[32]/*Port*/;
            GT_U32    ingrSTCInterruptSummary;
            GT_U32    ingrSTCInterruptSummaryMask;
            GT_U32    ingrSTCInterruptSummary1;
            GT_U32    ingrSTCInterruptSummary1Mask;

        }ingrSTCInterrupt;

        struct /*ingrSTCConfig*/{

            GT_U32    ingrSTCConfig;

        }ingrSTCConfig;

        struct /*ingrForwardingRestrictions*/{

            GT_U32    ingrForwardingRestrictionsDroppedPktsCntr;
            GT_U32    ingrForwardingRestrictionsDropCode;

        }ingrForwardingRestrictions;

        struct /*ingrDropCntr*/{

            GT_U32    ingrDropCntrConfig;
            GT_U32    ingrDropCntr;

        }ingrDropCntr;

        struct /*ingrDropCodesCntr*/{

            GT_U32    dropCodesCntrConfig;
            GT_U32    dropCodesCntr;

        }ingrDropCodesCntr;

        struct /*ePort*/{

            GT_U32    ePortGlobalConfig;
            GT_U32    protectionLOCWriteMask;
            GT_U32    protectionSwitchingRXExceptionConfig;

        }ePort;

        struct /*criticalECCCntrs*/{

            GT_U32    criticalECCConfig;
            GT_U32    BMClearCriticalECCErrorCntr;
            GT_U32    BMClearCriticalECCErrorParams;
            GT_U32    BMIncCriticalECCErrorCntr;
            GT_U32    BMIncCriticalECCErrorParams;
            GT_U32    BMCriticalECCInterruptCause;
            GT_U32    BMCriticalECCInterruptMask;

        }criticalECCCntrs;

        struct /*CPUTargetDeviceConfig*/{

            GT_U32    CPUTargetDeviceConfigReg0;
            GT_U32    CPUTargetDeviceConfigReg1;
            GT_U32    CPUTargetDeviceConfigReg2;
            GT_U32    CPUTargetDeviceConfigReg3;

        }CPUTargetDeviceConfig;

        struct /*applicationSpecificCPUCodes*/{

            GT_U32    IPProtocolCPUCodeEntry[4]/*Protocol*/;
            GT_U32    TCPUDPDestPortRangeCPUCodeEntryWord0[16]/*Entry*/;
            GT_U32    TCPUDPDestPortRangeCPUCodeEntryWord1[16]/*Entry*/;
            GT_U32    IPProtocolCPUCodeValidConfig;

        }applicationSpecificCPUCodes;

    }EQ;

    struct /*LPM*/{
        GT_U32    globalConfig;
        GT_U32    directAccessMode;
        GT_U32    exceptionStatus;
        GT_U32    LPMInterruptsCause;
        GT_U32    LPMInterruptsMask;
        GT_U32    LPMGeneralIntCause;
        GT_U32    LPMGeneralIntMask;
    }LPM;

    struct /*TCAM*/{

        struct /*tcamInterrupts*/{

            GT_U32    TCAMInterruptsSummaryMask;
            GT_U32    TCAMInterruptsSummaryCause;
            GT_U32    TCAMInterruptMask;
            GT_U32    TCAMInterruptCause;
            GT_U32    tcamParityErrorAddr;

        }tcamInterrupts;

        struct /*tcamArrayConf*/{

            GT_U32    tcamActiveFloors;

        }tcamArrayConf;

        struct /*parityDaemonRegs*/{

            GT_U32    parityDaemonCtrl;
            GT_U32    parityDaemonWriteAdressStart;
            GT_U32    parityDaemonWriteAdressEnd;
            GT_U32    parityDaemonWriteAdressJump;
            GT_U32    parityDaemonReadAdressStart;
            GT_U32    parityDaemonReadAdressEnd;
            GT_U32    parityDaemonReadAdressJump;
            GT_U32    parityDaemonDefaultWriteDataX0;
            GT_U32    parityDaemonDefaultWriteDataX1;
            GT_U32    parityDaemonDefaultWriteDataY0;
            GT_U32    parityDaemonDefaultWriteDataX2;
            GT_U32    parityDaemonDefaultWriteDataY1;
            GT_U32    parityDaemonDefaultWriteDataY2;
            GT_U32    parityDaemonStatus;

        }parityDaemonRegs;

        struct /*mgAccess*/{

            GT_U32    mgKeySizeAndTypeGroup[5]/*Tcam Group Num*/;
            GT_U32    mgHitGroupHitNum[5][4]/*Tcam Group Num*//*Hit Number*/;
            GT_U32    mgKeyGroupChunk[5][21]/*Tcam Group Num*//*Chunk Of The Key*/;
            GT_U32    mgCompRequest;
            GT_U32    mgKeyTypeUpdateReq;
            GT_U32    keyTypeData;

        }mgAccess;

        GT_U32    groupClientEnable[5]          /*Tcam Group num*/;
        GT_U32    tcamHitNumAndGroupSelFloor[12]/*Floor num*/;
        GT_U32    tcamGlobalConfig;

    }TCAM;

    struct /*EGF_eft*/{

        struct /*MCFIFO*/{

            struct /*MCFIFOConfigs*/{

                GT_U32    portToMcFIFOAttributionReg[32]/*reg_offset*/;
                GT_U32    portToHemisphereMapReg[16]/*reg offset*/;
                GT_U32    MCFIFOGlobalConfigs;
                GT_U32    MCFIFOArbiterWeights0;
                GT_U32    lossyDropAndPfcUcThreshold;

            }MCFIFOConfigs;

        }MCFIFO;

        struct /*global*/{

            GT_U32    cpuPortMapOfReg[128]/*reg_offset*/;
            GT_U32    cpuPortDist;
            GT_U32    portIsolationLookup0;
            GT_U32    portIsolationLookup1;
            GT_U32    EFTInterruptsCause;
            GT_U32    EFTInterruptsMask;
            GT_U32    ECCConf;
            GT_U32    ECCStatus;

        }global;

        struct /*egrFilterConfigs*/{

            GT_U32    enFCTriggerByCNFrameOnPort[16]/*Port div 32*/;
            GT_U32    enCNFrameTxOnPort[16]/*Port div 32*/;
            GT_U32    unregedBCFilterEn[16]/*Port div 32*/;
            GT_U32    unknownUCFilterEn[16]/*Port div 32*/;
            GT_U32    unregedMCFilterEn[16]/*Port div 32*/;
            GT_U32    OAMLoopbackFilterEn[16]/*Port div 32*/;
            GT_U32    virtual2PhysicalPortRemap[64]/*offset*/;
            GT_U32    physicalPortLinkStatusMask[16]/*reg offset*/;
            GT_U32    egrFiltersGlobal;
            GT_U32    egrFiltersEnable;

        }egrFilterConfigs;

        struct /*deviceMapConfigs*/{

            GT_U32    devMapTableAddrConstructProfile[8]/*profile_num*/;
            GT_U32    localSrcPortMapOwnDevEn[16]/*Port div 32*/;
            GT_U32    localTrgPortMapOwnDevEn[16]/*Port div 32*/;
            GT_U32    portAddrConstructMode[256]/*local physical source port*/;

        }deviceMapConfigs;

        struct /*counters*/{

            GT_U32    eftDescEcc1ErrorCntr;

        }counters;

    }EGF_eft;

    struct /*EGF_qag*/{

        struct /*uniMultiArb*/{

            GT_U32    uniMultiCtrl;
            GT_U32    uniWeights;
            GT_U32    multiWeights;
            GT_U32    multiShaperCtrl;

        }uniMultiArb;

        struct /*global*/{

            GT_U32    QAGInterruptsCause;
            GT_U32    QAGInterruptsMask;

        }global;

        struct /*distributor*/{

            GT_U32    useVlanTag1ForTagStateReg[PORTS_BMP_NUM_CNS]/*reg_offset*/;
            GT_U32    toAnalyzerLoopback[PORTS_BMP_NUM_CNS]/*Reg Offset*/;
            GT_U32    TCRemap[32]/*remapped_tc*/;
            GT_U32    stackRemapEn[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    perTrgPhyPortLbProfile[2*PORTS_BMP_NUM_CNS]/*Reg Offset*/;
            GT_U32    perSrcPhyPortLbProfile[2*PORTS_BMP_NUM_CNS]/*Reg Offset*/;
            GT_U32    loopbackForwardingGlobalConfig;
            GT_U32    loopbackEnableConfig[4];
            GT_U32    evlanMirrEnable[PORTS_BMP_NUM_CNS]/*Reg Offset*/;
            GT_U32    DPRemap[16]/*remapped_dp*/;
            GT_U32    distributorGeneralConfigs;

        }distributor;

    }EGF_qag;

    struct /*EGF_sht*/{

        struct /*global*/{

            GT_U32    eportStpState[PORTS_BMP_NUM_CNS]/*Port div 8*/;
            GT_U32    eportEvlanFilteringEnable[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    SHTInterruptsCause;
            GT_U32    UCSrcIDFilterEn[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    SHTEgrFiltersEnable;
            GT_U32    MCLocalEn[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    ignorePhySrcMcFilterEn[PORTS_BMP_NUM_CNS]/*Reg Offset*/;
            GT_U32    eportVlanEgrFiltering;
            GT_U32    eportStpStateMode[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    eportOddOnlySrcIdFilterEnable[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    eportRoutedForwardRestricted[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    eportPortIsolationMode[2*PORTS_BMP_NUM_CNS]/*Port div 16*/;
            GT_U32    SHTGlobalConfigs;
            GT_U32    eportIPMCRoutedFilterEn[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    eportFromCPUForwardRestricted[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    SHTInterruptsMask;
            GT_U32    eportBridgedForwardRestricted[PORTS_BMP_NUM_CNS]/*Port div 32*/;
            GT_U32    meshIdConfigs;
            GT_U32    ePortMeshId[PORTS_NUM_CNS/8];
            GT_U32    dropOnEportVid1MismatchEn[PORTS_BMP_NUM_CNS]; /* port div 32 */
            GT_U32    eportVid1[PORTS_NUM_CNS/2];             /* port div 2 */

        }global;

    }EGF_sht;


    struct /*HA*/{

        GT_U32    invalidPTPHeaderCntr;
REGISTER_COVERED_CNS        GT_U32    TRILLEtherType;
REGISTER_COVERED_CNS        GT_U32    TPIDConfigReg[8]/*entry*/;
REGISTER_COVERED_CNS        GT_U32    routerMACSABaseReg1;
        GT_U32    PWCtrlWord[15]/*PW word*/;
        GT_U32    PWETreeEnable;
        GT_U32    invalidPTPOutgoingPiggybackCntr;
        GT_U32    PTPTimestampTagEthertype;
        GT_U32    PTPAndTimestampingExceptionConfig;
REGISTER_COVERED_CNS        GT_U32    MPLSEtherType;
        GT_U32    MPLSChannelTypeProfileReg7;
        GT_U32    MPLSChannelTypeProfileReg[7]/*Reg*/;
REGISTER_COVERED_CNS        GT_U32    IPLengthOffset;
REGISTER_COVERED_CNS        GT_U32    egrUDPPortRangeComparatorConfig[8]/*UDPCmp*/;
        GT_U32    invalidPTPIncomingPiggybackCntr;
        GT_U32    egrPolicyDIPSolicitationData1;
REGISTER_COVERED_CNS        GT_U32    ingrAnalyzerVLANTagConfig;
REGISTER_COVERED_CNS        GT_U32    IEtherType;
        GT_U32    haGlobalConfig1;
REGISTER_COVERED_CNS        GT_U32    haGlobalConfig;
        GT_U32    HAInterruptMask;
        GT_U32    HAInterruptCause;
        GT_U32    ethernetOverGREProtocolType;
REGISTER_COVERED_CNS        GT_U32    routerMACSABaseReg0;
REGISTER_COVERED_CNS        GT_U32    egrTCPPortRangeComparatorConfig[8]/*TCPCmp*/;
        GT_U32    egrPolicyDIPSolicitationMask3;
        GT_U32    egrPolicyDIPSolicitationMask2;
        GT_U32    egrPolicyDIPSolicitationMask1;
        GT_U32    egrPolicyDIPSolicitationMask0;
        GT_U32    egrPolicyDIPSolicitationData3;
        GT_U32    egrPolicyDIPSolicitationData2;
        GT_U32    tunnelStartFragmentIDConfig;
        GT_U32    egrPolicyDIPSolicitationData0;
REGISTER_COVERED_CNS        GT_U32    egrAnalyzerVLANTagConfig;
        GT_U32    dataECCStatus;
        GT_U32    CPIDReg1;
        GT_U32    CPIDReg0;
        GT_U32    congestionNotificationConfig;
        GT_U32    CNMHeaderConfig;
        GT_U32    BPEConfigReg2;
        GT_U32    BPEConfigReg1;
        GT_U32    haBadAddrLatchReg;/*manual added for Sip5_10 : Latch bad address in case of regfile bad address indication.*/
        GT_U32    NATConfig;        /*manual added for Sip5_10*/
        GT_U32    NATExceptionDropCounter;/*manual added for Sip5_10*/

        CPSS_TBD_BOOKMARK
        /* next manually added to stop us the
            PRV_CPSS_DXCH_PP_REGS_ADDR_STC::haRegs */
        struct ETS_temp_STCT{
REGISTER_COVERED_CNS            GT_U32  ptpGlobalConfig; /* Egress PTP Global Register */
REGISTER_COVERED_CNS            GT_U32  ptpEnableTimeStamp; /* Enable Time Stamping Register */
REGISTER_COVERED_CNS            GT_U32  ptpGlobalFifoCurEntry[2]; /* Global FIFO Current Entry */
REGISTER_COVERED_CNS            GT_U32  ptpTodCntrNanoSeconds; /* Egress TOD Counter - Nanoseconds */
REGISTER_COVERED_CNS            GT_U32  ptpTodCntrSeconds[2]; /* Egress TOD Counter - Seconds */
REGISTER_COVERED_CNS            GT_U32  ptpTodCntrShadowNanoSeconds; /* Egress TOD Counter Shadow - Nanoseconds */
REGISTER_COVERED_CNS            GT_U32  ptpTodCntrShadowSeconds[2]; /* Egress TOD Counter Shadow - Seconds */
        }ETS_temp;

    }HA;

    struct /*MLL*/{

        struct /*multiTargetRateShape*/{

            GT_U32    multiTargetRateShapeConfig;
            GT_U32    multiTargetRateShapeConfig2;
            GT_U32    multiTargetMTUReg;
            GT_U32    tokenBucketBaseLine;

        }multiTargetRateShape;

        struct /*mcLinkedListMLLTables*/{

REGISTER_COVERED_CNS            GT_U32    qoSProfileToMultiTargetTCQueuesReg[64]/*Index*/;
            GT_U32    extQoSModeMCQueueSelectionReg[2]/*Index*/;

        }mcLinkedListMLLTables;

        struct /*multiTargetVsUcSDWRRAndStrictPriorityScheduler*/{

REGISTER_COVERED_CNS            GT_U32    mcUcSDWRRAndStrictPriorityConfig;

        }multiTargetVsUcSDWRRAndStrictPriorityScheduler;

        struct /*multiTargetTCQueuesAndArbitrationConfig*/{

REGISTER_COVERED_CNS            GT_U32    multitargetTCQueuesGlobalConfig;
REGISTER_COVERED_CNS            GT_U32    multitargetTCQueuesStrictPriorityEnableConfig;
REGISTER_COVERED_CNS            GT_U32    multitargetTCQueuesWeightConfig;

        }multiTargetTCQueuesAndArbitrationConfig;

        struct /*multiTargetEPortMap*/{

            GT_U32    multiTargetEPortValue;
            GT_U32    multiTargetEPortMask;
            GT_U32    multiTargetEPortBase;
            GT_U32    ePortToEVIDXBase;

        }multiTargetEPortMap;

        struct /*MLLOutInterfaceCntrs*/{

            GT_U32    L2MLLOutInterfaceCntrConfig1[2]/*Counter*/;
            GT_U32    L2MLLOutInterfaceCntrConfig[2]/*Counter*/;
REGISTER_COVERED_CNS            GT_U32    L2MLLValidProcessedEntriesCntr[3]/*Counter*/;
REGISTER_COVERED_CNS            GT_U32    L2MLLTTLExceptionCntr;
REGISTER_COVERED_CNS            GT_U32    L2MLLSkippedEntriesCntr;
            GT_U32    L2MLLSilentDropCntr;
            GT_U32    L2MLLOutMcPktsCntr[2]/*Counter*/;
            GT_U32    MLLSilentDropCntr;
REGISTER_COVERED_CNS            GT_U32    MLLMCFIFODropCntr;
            GT_U32    IPMLLSkippedEntriesCntr;
REGISTER_COVERED_CNS            GT_U32    IPMLLOutMcPktsCntr[2]/*Counter*/;
REGISTER_COVERED_CNS            GT_U32    IPMLLOutInterfaceCntrConfig1[2]/*Counter*/;
REGISTER_COVERED_CNS            GT_U32    IPMLLOutInterfaceCntrConfig[2]/*Counter*/;

        }MLLOutInterfaceCntrs;

        struct /*MLLGlobalCtrl*/{

REGISTER_COVERED_CNS            GT_U32    MLLGlobalCtrl;
REGISTER_COVERED_CNS            GT_U32    MLLLookupTriggerConfig;
REGISTER_COVERED_CNS            GT_U32    globalTTLExceptionConfig;
REGISTER_COVERED_CNS            GT_U32    globalEPortRangeConfig;
REGISTER_COVERED_CNS            GT_U32    globalEPortRangeMask;
            GT_U32    IPMLLTableResourceSharingConfig;
            GT_U32    L2MLLTableResourceSharingConfig;
            GT_U32    MLLInterruptCauseReg;
            GT_U32    MLLInterruptMaskReg;
            GT_U32    MLLReplicatedTrafficCtrl;
            GT_U32    sourceBasedL2MLLFiltering;
            GT_U32    MLLEccStatus;
REGISTER_COVERED_CNS            GT_U32    MLLMCSourceID;

        }MLLGlobalCtrl;

    }MLL;

    struct /*PCL*/{

        struct /*OAMConfigs*/{

            GT_U32    channelTypeOAMOpcode[15]/*channel type*/;
REGISTER_COVERED_CNS            GT_U32    OAMRDIConfigs;
            GT_U32    OAMMEGLevelModeConfigs;
            GT_U32    OAMRFlagConfigs;
            GT_U32    RBitAssignmentEnable;

        }OAMConfigs;

        struct /*HASH*/{

REGISTER_COVERED_CNS            GT_U32    pearsonHashTable[16]/*index div 4*/;
REGISTER_COVERED_CNS            GT_U32    CRCHashConfig;
REGISTER_COVERED_CNS            GT_U32    CRC32Salt[20]/*n*/;
REGISTER_COVERED_CNS            GT_U32    CRC32Seed;

        }HASH;

REGISTER_COVERED_CNS        GT_U32    ingrPolicyGlobalConfig;
        GT_U32    PCLUnitInterruptCause;
        GT_U32    PCLUnitInterruptMask;
        GT_U32    pclBadAddressLatchReg;
REGISTER_COVERED_CNS        GT_U32    policyEngineConfig;
REGISTER_COVERED_CNS        GT_U32    policyEngineUserDefinedBytesConfig;
REGISTER_COVERED_CNS        GT_U32    CFMEtherType;
        GT_U32    lookupSourceIDMask[3]/*lookup num*/;
REGISTER_COVERED_CNS        GT_U32    policyEngineUserDefinedBytesExtConfig;
        GT_U32    TCAMKeyTypeMap[2]/*reg num*/;
REGISTER_COVERED_CNS        GT_U32    L2L3IngrVLANCountingEnable[16]/*port div 32*/;
REGISTER_COVERED_CNS        GT_U32    countingModeConfig;

    }PCL;

    struct /*EPCL*/{

REGISTER_COVERED_CNS        GT_U32    egrPolicyGlobalConfig;
        GT_U32    EPCLOAMPktDetection;
        GT_U32    EPCLOAMMEGLevelMode;
REGISTER_COVERED_CNS        GT_U32    EPCLOAMRDI;
        GT_U32    EPCLInterruptCause;
        GT_U32    EPCLInterruptMask;
        GT_U32    EPCLChannelTypeToOAMOpcodeMap[5]/*Set*/;
        GT_U32    EPCLReservedBitMask;
        GT_U32    TCAMKeyTypeMap;

        GT_U32    EPCLBadAddrLatchReg;
        GT_U32    oamRBbitConfigReg;
    }EPCL;

    struct /*rxDMA*/{

        struct /*singleChannelDMAConfigs*/{

            GT_U32    SCDMAConfig0[73]/*Port*/;
            GT_U32    SCDMAConfig1[73]/*Port*/;
            GT_U32    SCDMACTPktIndentification[73]/*port*/;

        }singleChannelDMAConfigs;

        struct /*globalRxDMAConfigs*/{

            struct /*rejectConfigs*/{

                GT_U32    rejectPktCommand;
                GT_U32    rejectCPUCode;

            }rejectConfigs;

            struct /*globalConfigCutThrough*/{

                GT_U32    CTEtherTypeIdentification;
                GT_U32    CTUPEnable;

            }globalConfigCutThrough;

            struct /*buffersLimitProfiles*/{

                GT_U32    portsBuffersLimitProfileXONXOFFConfig[8];
                GT_U32    portsBuffersLimitProfileRxlimitConfig[8];
            }buffersLimitProfiles;

            struct /*preIngrPrioritizationConfStatus*/{

                GT_U32    VLANUPPriorityArrayProfile[4]/*profile*/;
                GT_U32    MPLSEtherTypeConf[2]/*MPLS*/;
                GT_U32    VIDConf[4]/*index*/;
                GT_U32    genericEtherType[4]/*index*/;
                GT_U32    qoSDSAPriorityArrayProfileSegment[4][8]/*profile*//*segment*/;
                GT_U32    preingressPrioritizationEnable;
                GT_U32    PIPPriorityDropGlobalCntrsClear;
                GT_U32    PIPPrioThresholds;
                GT_U32    PIPMedPriorityDropGlobalCntrMsb;
                GT_U32    PIPMedPriorityDropGlobalCntrLsb;
                GT_U32    PIPLowPriorityDropGlobalCntrMsb;
                GT_U32    VLANEtherTypeConf[4]/*index*/;
                GT_U32    MACDA2MsbBytesConf[4]/*MAC DA conf*/;
                GT_U32    PIPHighPriorityDropGlobalCntrLsb;
                GT_U32    MPLSEXPPriorityArrayProfile[4]/*profile*/;
                GT_U32    UPDSATagArrayProfile[4]/*profile*/;
                GT_U32    MACDAPriorityArrayProfile[4]/*profile*/;
                GT_U32    MACDAMask4LsbConf[4]/*mask*/;
                GT_U32    MACDAMask2MsbConf[4]/*mask*/;
                GT_U32    MACDA4LsbBytesConf[4]/*MAC DA conf*/;
                GT_U32    PIPLowPriorityDropGlobalCntrLSb;
                GT_U32    IPv6TCPriorityArrayProfileSegment[4][16]/*profile*//*segment*/;
                GT_U32    IPv6EtherTypeConf;
                GT_U32    IPv4ToSPriorityArrayProfileSegment[4][16]/*profile*//*segment*/;
                GT_U32    IPv4EtherTypeConf;
                GT_U32    PIPHighPriorityDropGlobalCntrMsb;
                GT_U32    PIPDropGlobalCounterTrigger;
                GT_U32    etherTypePriorityArrayProfile[4]/*profile*/;

            }preIngrPrioritizationConfStatus;

        }globalRxDMAConfigs;

        struct /* SCDMA Debug*/{
            GT_U32 portBuffAllocCnt[73]/*Port*/;
        }scdmaDebug;

        struct /*SIP_COMMON_MODULES*/{
            PRV_CPSS_DXCH_PP_DP_PIZZA_ARBITER_STC pizzaArbiter;
        }SIP_COMMON_MODULES;

        struct /*interrupts*/{

            GT_U32    rxDMASCDMAInterruptMask[73]/*Port*/;
            GT_U32    rxDMASCDMAInterruptCause[73]/*Port*/;
            GT_U32    rxDMAInterrupt0Cause;
            GT_U32    rxDMAInterrupt0Mask;
            GT_U32    rxDMAInterruptSummary0Cause;
            GT_U32    rxDMAInterruptSummary0Mask;
            GT_U32    rxDMAInterruptSummary1Cause;
            GT_U32    rxDMAInterruptSummary1Mask;
            GT_U32    rxDMAInterruptSummary2Cause;
            GT_U32    rxDMAInterruptSummary2Mask;
            GT_U32    rxDMAInterruptSummary3Cause;
            GT_U32    rxDMAInterruptSummary3Mask;
            GT_U32    rxDMAInterruptSummaryCause;
            GT_U32    rxDMAInterruptSummaryMask;

        }interrupts;

        struct /* debug*/{

            struct /* counters_and_counters_statuses */{

                GT_U32    currentNumOfPacketsStoredInMppm;

            }countersAndCountersStatuses;

        }debug;

        struct /*singleChannelDMAPip*/ {
            GT_U32    SCDMA_n_PIP_Config[73];
            GT_U32    SCDMA_n_Drop_Pkt_Counter[73];
        }singleChannelDMAPip;

    }rxDMA[6];

    struct /*txDMA*/{

        struct /*txDMAThresholdsConfigs*/{

            GT_U32    RXDMAUpdatesFIFOsThresholds[1]/*rxdma*/;
            GT_U32    clearFIFOThreshold;
            GT_U32    headerReorderFifoThreshold;
            GT_U32    payloadReorderFifoThreshold;

        }txDMAThresholdsConfigs;

        struct /*txDMAPerSCDMAConfigs*/{

            GT_U32    burstLimiterSCDMA[74]/*SCDMA*/;
            GT_U32    FIFOsThresholdsConfigsSCDMAReg1[74]/*SCDMA*/;
            GT_U32    txFIFOCntrsConfigsSCDMA[74]/*SCDMA*/;
            GT_U32    SCDMAConfigs[74]/*SCDMA*/;
            GT_U32    SCDMAConfigs2[74]/*SCDMA reg2*/;
            GT_U32    SCDMAMetalFixReg[74]/*SCDMA*/;

        }txDMAPerSCDMAConfigs;

        struct /*txDMAInterrupts*/{

            GT_U32    SCDMAIDFIFOOverrunInterruptCauseReg1;
            GT_U32    SCDMAIDFIFOOverrunInterruptMaskReg1;
            GT_U32    SCDMAIDFIFOOverrunInterruptCauseReg2;
            GT_U32    SCDMAIDFIFOOverrunInterruptMaskReg2;
            GT_U32    SCDMAWaitingForNextReplyInterruptMaskReg3;
            GT_U32    SCDMAIDFIFOOverrunInterruptMaskReg3;
            GT_U32    RXDMAUpdatesFIFOsOverrunInterruptCauseReg1;
            GT_U32    RXDMAUpdatesFIFOsOverrunInterruptMaskReg1;
            GT_U32    SCDMAWaitingForNextReplyInterruptCauseReg1;
            GT_U32    SCDMAWaitingForNextReplyInterruptMaskReg1;
            GT_U32    SCDMAWaitingForNextReplyInterruptCauseReg2;
            GT_U32    SCDMAWaitingForNextReplyInterruptMaskReg2;
            GT_U32    SCDMAWaitingForNextReplyInterruptCauseReg3;
            GT_U32    SCDMAIDFIFOOverrunInterruptCauseReg3;
            GT_U32    txDMAGeneralCauseReg1;
            GT_U32    txDMAGeneralMaskReg1;
            GT_U32    txDMASummaryCauseReg;
            GT_U32    txDMASummaryMaskReg;

        }txDMAInterrupts;

        struct /*txDMAGlobalConfigs*/{

            GT_U32    txDMAGlobalConfig1;
            GT_U32    txDMAGlobalConfig3;
            GT_U32    ingrCoreIDToMPPMMap;
            GT_U32    TXDMAMetalFixReg;

        }txDMAGlobalConfigs;

        struct /*txDMADescCriticalFieldsECCConfigs*/{

            GT_U32    dataECCConfig;
            GT_U32    dataECCStatus;

        }txDMADescCriticalFieldsECCConfigs;

        struct /*txDMADebug*/{

            struct /*informativeDebug*/{

                GT_U32    nextBCFIFOFillLevel[1]/*core*/;
                GT_U32    nextBCFIFOMaxFillLevel[1]/*core*/;
                GT_U32    SCDMAStatusReg1[74]/*SCDMA*/;
                GT_U32    SCDMAStatusReg2[74]/*SCDMA*/;
                GT_U32    SCDMAPrefFIFOFillLevel[74]/*SCDMA*/;
                GT_U32    SCDMAPrefFIFOMaxFillLevel[74]/*SCDMA*/;
                GT_U32    descOverrunReg1;
                GT_U32    descOverrunReg2;
                GT_U32    descOverrunReg3;
                GT_U32    clearFIFOFillLevel;
                GT_U32    clearFIFOMaxFillLevel;
                GT_U32    headerReorderFIFOFillLevel;
                GT_U32    headerReorderFIFOMaxFillLevel;
                GT_U32    payloadReorderFIFOFillLevel;
                GT_U32    payloadReorderFIFOMaxFillLevel;
                GT_U32    descIDFIFOFillLevel;
                GT_U32    descIDFIFOMaxFillLevel;
                GT_U32    debugBusCtrlReg;
                GT_U32    txDMADebugBus;
                GT_U32    incomingDescsCntrLsb;
                GT_U32    incomingDescsCntrMsb;
                GT_U32    outgoingDescsCntrLsb;
                GT_U32    outgoingDescsCntrMsb;
                GT_U32    outgoingClearRequestsCntrLsb;
                GT_U32    outgoingClearRequestsCntrMsb;
                GT_U32    MPPMReadRequestsCntr;
                GT_U32    MPPMReadAnswersCntrLsb;
                GT_U32    MPPMReadAnswersCntrMsb;
                GT_U32    outgoingPayloadsCntrLsb;
                GT_U32    outgoingPayloadsCntrMsb;
                GT_U32    outgoingPayloadsEOPCntrLsb;
                GT_U32    outgoingPayloadsEOPCntrMsb;
                GT_U32    outgoingHeadersEOPCntrLsb;
                GT_U32    outgoingHeadersEOPCntrMsb;
                GT_U32    outgoingCTBCUpdatesCntr;
                GT_U32    HADescIDDBFreeIDFIFOFillLevel;
                GT_U32    internalDescIDDBFreeIDFIFOFillLevel;
                GT_U32    headerReorderMemIDDBFreeIDFIFOFillLevel;
                GT_U32    payloadReorderMemIDDBFreeIDFIFOFillLevel;

            }informativeDebug;

        }txDMADebug;

        struct /*txDMAPizzaArb*/{
            PRV_CPSS_DXCH_PP_DP_PIZZA_ARBITER_STC pizzaArbiter;
        }txDMAPizzaArb;

    }txDMA[6];

    struct /*txFIFO*/{

        struct /*txFIFOShiftersConfig*/{

            GT_U32    SCDMAShiftersConf[74]/*SCDMA*/;

        }txFIFOShiftersConfig;

        struct /*txFIFOLinkedLists*/{

            struct /*payloadLinkedList*/{

                struct /*payloadLinkListRAM*/{

                    GT_U32    payloadLinkListAddr;

                }payloadLinkListRAM;

            }payloadLinkedList;

            struct /*headerLinkedList*/{

                struct /*headerLinkListRAM*/{

                    GT_U32    headerLinkListAddr;

                }headerLinkListRAM;

            }headerLinkedList;

        }txFIFOLinkedLists;

        struct /*txFIFOInterrupts*/{

            GT_U32    txFIFOGeneralCauseReg1;
            GT_U32    txFIFOGeneralMaskReg1;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptCauseReg1;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptMaskReg1;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptCauseReg2;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptMaskReg2;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptCauseReg3;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptMaskReg3;
            GT_U32    CTByteCountArrivedLateInterruptCauseReg1;
            GT_U32    CTByteCountArrivedLateInterruptMaskReg1;
            GT_U32    CTByteCountArrivedLateInterruptCauseReg2;
            GT_U32    CTByteCountArrivedLateInterruptMaskReg2;
            GT_U32    CTByteCountArrivedLateInterruptCauseReg3;
            GT_U32    CTByteCountArrivedLateInterruptMaskReg3;
            GT_U32    txFIFOSummaryCauseReg;
            GT_U32    txFIFOSummaryMaskReg;

        }txFIFOInterrupts;

        struct /*txFIFOGlobalConfig*/{

            GT_U32    SCDMAPayloadThreshold[74]/*SCDMA*/;
            GT_U32    txFIFOMetalFixReg;

        }txFIFOGlobalConfig;

        struct /*txFIFOEnd2EndECCConfigs*/{

            GT_U32    dataECCConfig[2]/*ecc*/;
            GT_U32    dataECCStatus[2]/*ecc*/;

        }txFIFOEnd2EndECCConfigs;

        struct /*txFIFODebug*/{

            struct /*informativeDebug*/{

                GT_U32    SCDMAStatusReg6[74]/*SCDMA*/;
                GT_U32    SCDMAStatusReg1[74]/*SCDMA*/;
                GT_U32    SCDMAStatusReg2[74]/*SCDMA*/;
                GT_U32    SCDMAStatusReg3[74]/*SCDMA*/;
                GT_U32    SCDMAStatusReg4[74]/*SCDMA*/;
                GT_U32    SCDMAStatusReg5[74]/*SCDMA*/;
                GT_U32    debugBusCtrlReg;
                GT_U32    txFIFODebugBus;
                GT_U32    LLEOPCntrLsb;
                GT_U32    LLEOPCntrMsb;
                GT_U32    incomingHeadersCntrLsb;
                GT_U32    incomingHeadersCntrMsb;
                GT_U32    headerIDDBFreeIDFIFOFillLevel;
                GT_U32    payloadIDDBFreeIDFIFOFillLevel;
                GT_U32    CTBCIDDBFreeIDFIFOFillLevel;

            }informativeDebug;

        }txFIFODebug;

        struct /*txFIFOPizzaArb*/{
            PRV_CPSS_DXCH_PP_DP_PIZZA_ARBITER_STC pizzaArbiter;
        }txFIFOPizzaArb;

    }txFIFO[6];

    struct /*SIP_ETH_TXFIFO*/{

        struct /*txFIFOShiftersConfig*/{

            GT_U32    SCDMAShiftersConf[73]/*scdma*/;

        }txFIFOShiftersConfig;

        struct /*txFIFOPizzaArb*/{
            PRV_CPSS_DXCH_PP_DP_PIZZA_ARBITER_STC pizzaArbiter;
        }txFIFOPizzaArb;

        struct /*txFIFOLinkedLists*/{

            struct /*payloadLinkedList*/{

                struct /*payloadLinkListRAM*/{

                    GT_U32    payloadLinkListAddr;

                }payloadLinkListRAM;

            }payloadLinkedList;

            struct /*headerLinkedList*/{

                struct /*headerLinkListRAM*/{

                    GT_U32    headerLinkListAddr;

                }headerLinkListRAM;

            }headerLinkedList;

        }txFIFOLinkedLists;

        struct /*txFIFOInterrupts*/{

            GT_U32    txFIFOGeneralCauseReg1;
            GT_U32    txFIFOGeneralMaskReg1;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptCauseReg1;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptMaskReg1;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptCauseReg2;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptMaskReg2;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptCauseReg3;
            GT_U32    shifterSOPEOPFIFONotReadyInterruptMaskReg3;
            GT_U32    CTByteCountArrivedLateInterruptCauseReg1;
            GT_U32    CTByteCountArrivedLateInterruptMaskReg1;
            GT_U32    CTByteCountArrivedLateInterruptCauseReg2;
            GT_U32    CTByteCountArrivedLateInterruptMaskReg2;
            GT_U32    CTByteCountArrivedLateInterruptCauseReg3;
            GT_U32    CTByteCountArrivedLateInterruptMaskReg3;
            GT_U32    txFIFOSummaryCauseReg;
            GT_U32    txFIFOSummaryMaskReg;

        }txFIFOInterrupts;

        struct /*txFIFOGlobalConfig*/{

            GT_U32    SCDMAPayloadThreshold[73]/*scdma*/;
            GT_U32    txFIFOGlobalConfigs;
            GT_U32    TXFIFOMetalFixReg;

        }txFIFOGlobalConfig;

        struct /*txFIFOEnd2EndECCConfigs*/{

            GT_U32    dataECCStatus[1]/*ecc*/;
            GT_U32    dataECCConfig[1]/*ecc*/;
            GT_U32    eccErrorCntr;
            GT_U32    ECCFailedAddr;
            GT_U32    eccErrorType;
            GT_U32    lockErronousEvent;

        }txFIFOEnd2EndECCConfigs;

        struct /*txFIFODebug*/{

            struct /*informativeDebug*/{

                GT_U32    SCDMAStatusReg6[73]/*scdma*/;
                GT_U32    SCDMAStatusReg1[73]/*scdma*/;
                GT_U32    SCDMAStatusReg5[73]/*scdma*/;
                GT_U32    SCDMAStatusReg4[73]/*scdma*/;
                GT_U32    SCDMAStatusReg3[73]/*scdma*/;
                GT_U32    SCDMAStatusReg2[73]/*scdma*/;
                GT_U32    debugBusCtrlReg;
                GT_U32    txFIFODebugBus;
                GT_U32    LLEOPCntr;
                GT_U32    incomingHeadersCntr;
                GT_U32    headerIDDBFreeIDFIFOFillLevel;
                GT_U32    payloadIDDBFreeIDFIFOFillLevel;
                GT_U32    CTBCIDDBFreeIDFIFOFillLevel;

            }informativeDebug;

            struct /*activeDebug*/{

                GT_U32    debugSCDMA[73]/*scdma*/;
                GT_U32    globalActiveDebugReg;
                GT_U32    globalActiveDebugConfReg;
                GT_U32    debugSCDMAConfReg;

            }activeDebug;

        }txFIFODebug;

    }SIP_ETH_TXFIFO[2];

    struct /*SIP_ILKN_TXFIFO*/{

        struct /*ilknTxFIFOInterrupts*/{

            GT_U32    ilknTxfifoGeneralInterruptCause;
            GT_U32    ilknTxfifoGeneralInterruptMask;
            GT_U32    streamIDDataMemFullIlknTxfifoInterruptCauseReg1;
            GT_U32    streamIDDataMemFullIlknTxfifoInterruptMaskReg1;
            GT_U32    streamIDDescMemFullIlknTxfifoInterruptCauseReg1;
            GT_U32    streamIDDescMemFullIlknTxfifoInterruptMaskReg1;
            GT_U32    streamIDDataMemFullIlknTxfifoInterruptCauseReg2;
            GT_U32    streamIDDataMemFullIlknTxfifoInterruptMaskReg2;
            GT_U32    streamIDDescMemFullIlknTxfifoInterruptCauseReg2;
            GT_U32    streamIDDescMemFullIlknTxfifoInterruptMaskReg2;
            GT_U32    streamIDDataMemFullIlknTxfifoInterruptCauseReg3;
            GT_U32    streamIDDataMemFullIlknTxfifoInterruptMaskReg3;
            GT_U32    streamIDDescMemFullIlknTxfifoInterruptCauseReg3;
            GT_U32    streamIDDescMemFullIlknTxfifoInterruptMaskReg3;
            GT_U32    ilknTxfifoUnderrunInterruptCauseReg1;
            GT_U32    ilknTxfifoUnderrunInterruptMaskReg1;
            GT_U32    ilknTxfifoUnderrunInterruptCauseReg2;
            GT_U32    ilknTxfifoUnderrunInterruptMaskReg2;
            GT_U32    ilknTxfifoUnderrunInterruptCauseReg3;
            GT_U32    ilknTxfifoUnderrunInterruptMaskReg3;
            GT_U32    ilknTxfifoSummaryCauseReg;
            GT_U32    ilknTxfifoSummaryMaskReg;

        }ilknTxFIFOInterrupts;

        struct /*ilknTxFIFOGlobalConfig*/{

            GT_U32    portDynamicMemoryAlocation[64]/*port_num*/;
            GT_U32    portTransmittingThreshold[64]/*port num*/;
            GT_U32    portChannelEnable[64]/*port_num*/;
            GT_U32    globalConfig;

        }ilknTxFIFOGlobalConfig;

        struct /*ilknTxFIFODebug*/{

            GT_U32    dataFifoFillLevelForPort[64]/*port_num*/;

        }ilknTxFIFODebug;

        struct /*ilknTxFIFOArbiter*/{

            GT_U32    pizzaSlicePortMap[1064]/*slice num*/;
            GT_U32    pizzaArbiterCtrlReg;
            GT_U32    pizzaArbiterStatusReg;

        }ilknTxFIFOArbiter;

    }SIP_ILKN_TXFIFO;

    struct /*ERMRK*/{

        GT_U32    ERMRKGlobalConfig;
        GT_U32    ERMRKInterruptCause;
        GT_U32    ERMRKInterruptMask;
        GT_U32    ERMRKPTPConfig;
        GT_U32    timestampingUDPChecksumMode;
        GT_U32    timestampQueueMsgType;
        GT_U32    timestampQueueEntryID;
        GT_U32    PTPNTPOffset;
        GT_U32    ERMRKUP2UPMapTable[12]/*set*/;
        GT_U32    ERMRKPTPInvalidTimestampCntr;
        GT_U32    ERMRKDP2CFITable;
        GT_U32    ERMRKEXP2EXPMapTable[12]/*set*/;
        GT_U32    timestampQueueEntryWord0[4]/*queue*/;
        GT_U32    timestampQueueEntryWord1[4]/*queue*/;
        GT_U32    timestampQueueEntryWord2[2]/*queue*/;
        GT_U32    timestampQueueEntryIDClear;
        GT_U32    ECCErrorInjection;
        GT_U32    ERMRKBadAddrLatch;

    }ERMRK;

    struct /*BM*/{

        struct /*BMInterrupts*/{

            GT_U32    BMGeneralCauseReg1;
            GT_U32    BMGeneralMaskReg1;
            GT_U32    BMGeneralCauseReg2;
            GT_U32    BMGeneralMaskReg2;
            GT_U32    BMSummaryCauseReg;
            GT_U32    BMSummaryMaskReg;

        }BMInterrupts;

        struct /*BMGlobalConfigs*/{

            GT_U32    BMBufferTermination[5]/*reg*/;
            GT_U32    BMGlobalConfig1;
            GT_U32    BMGlobalConfig2;
            GT_U32    BMPauseConfig;
            GT_U32    BMBufferLimitConfig1;
            GT_U32    BMBufferLimitConfig2;
            GT_U32    BMMetalFixReg;

        }BMGlobalConfigs;

        struct /*BMDebug*/{

            GT_U32    rxDMANextUpdatesFIFOMaxFillLevel[6]/*core*/;
            GT_U32    rxDMANextUpdatesFIFOFillLevel[6]/*core*/;
            GT_U32    SCDMAInProgRegs[148]/*scdma*/;/*148 in bobk , 114 in bc2*/

        }BMDebug;

        struct /*BMCntrs*/{

            GT_U32    BMGlobalBufferCntr;
            GT_U32    rxDMAsAllocationsCntr;
            GT_U32    BMFreeBuffersInsideBMCntr;
            GT_U32    BMRxDMAPendingAllocationCntr;
            GT_U32    BMCTClearsInsideBMCntr;
            GT_U32    incomingCTClearsCntr;
            GT_U32    incomingRegularClearsCntr;
            GT_U32    nextTableAccessCntr;
            GT_U32    nextTableWriteAccessCntr;
            GT_U32    BMTerminatedBuffersCntr;
            GT_U32    BMLostClearRequestsCntr;

        }BMCntrs;

    }BM;

    struct /*OAMUnit*/{

        GT_U32    OAMGlobalCtrl;
        GT_U32    agingEntryGroupStatus[2]/*reg*/;
        GT_U32    summaryExceptionGroupStatus[2]/*reg*/;
        GT_U32    txPeriodExceptionGroupStatus[2]/*reg*/;
        GT_U32    sourceInterfaceMismatchCntr;
        GT_U32    OAMGlobalCtrl1;
        GT_U32    RDIStatusChangeExceptionGroupStatus[2]/*reg*/;
        GT_U32    RDIStatusChangeCntr;
        GT_U32    OAMUnitInterruptMask;
        GT_U32    OAMUnitInterruptCause;
        GT_U32    OAMTableAccessData[3]/*Reg*/;
        GT_U32    OAMTableAccessCtrl;
        GT_U32    OAMLossMeasurementOpcodes[4]/*reg*/;
        GT_U32    OAMLMCountedOpcodes[2]/*reg*/;
        GT_U32    OAMKeepAliveOpcodes[4]/*reg*/;
        GT_U32    OAMInvalidKeepaliveExceptionConfigs;
        GT_U32    agingPeriodEntryHigh[8]/*entry*/;
        GT_U32    dualEndedLM;
        GT_U32    OAMExceptionConfigs2;
        GT_U32    OAMExceptionConfigs1;
        GT_U32    OAMExceptionConfigs;
        GT_U32    OAMDelayMeasurementOpcodes[4]/*reg*/;
        GT_U32    MEGLevelGroupStatus[2]/*reg*/;
        GT_U32    MEGLevelExceptionCntr;
        GT_U32    sourceInterfaceGroupStatus[2]/*reg*/;
        GT_U32    invalidKeepaliveHashCntr;
        GT_U32    invalidKeepaliveGroupStatus[2]/*reg*/;
        GT_U32    hashBitsSelection;
        GT_U32    excessKeepaliveGroupStatus[2]/*reg*/;
        GT_U32    excessKeepaliveCntr;
        GT_U32    keepaliveAgingCntr;
        GT_U32    agingPeriodEntryLow[8]/*entry*/;
        GT_U32    txPeriodExceptionCntr;
        GT_U32    summaryExceptionCntr;

    }OAMUnit[2];

    struct /*GOP*/{

        struct /*9 GOPs + ILKN *//*two TAIs per GOP and two TAIs per ILKN*/{

            GT_U32    TAIInterruptCause;
            GT_U32    TAIInterruptMask;
            GT_U32    TAICtrlReg0;
            GT_U32    TAICtrlReg1;
            GT_U32    timeCntrFunctionConfig0;
            GT_U32    timeCntrFunctionConfig1;
            GT_U32    timeCntrFunctionConfig2;
            GT_U32    TODStepNanoConfigHigh;
            GT_U32    TODStepNanoConfigLow;
            GT_U32    TODStepFracConfigHigh;
            GT_U32    TODStepFracConfigLow;
            GT_U32    timeAdjustmentPropagationDelayConfigHigh;
            GT_U32    timeAdjustmentPropagationDelayConfigLow;
            GT_U32    triggerGenerationTODSecHigh;
            GT_U32    triggerGenerationTODSecMed;
            GT_U32    triggerGenerationTODSecLow;
            GT_U32    triggerGenerationTODNanoHigh;
            GT_U32    triggerGenerationTODNanoLow;
            GT_U32    triggerGenerationTODFracHigh;
            GT_U32    triggerGenerationTODFracLow;
            GT_U32    timeLoadValueSecHigh;
            GT_U32    timeLoadValueSecMed;
            GT_U32    timeLoadValueSecLow;
            GT_U32    timeLoadValueNanoHigh;
            GT_U32    timeLoadValueNanoLow;
            GT_U32    timeLoadValueFracHigh;
            GT_U32    timeLoadValueFracLow;
            GT_U32    timeCaptureValue0SecHigh;
            GT_U32    timeCaptureValue0SecMed;
            GT_U32    timeCaptureValue0SecLow;
            GT_U32    timeCaptureValue0NanoHigh;
            GT_U32    timeCaptureValue0NanoLow;
            GT_U32    timeCaptureValue0FracHigh;
            GT_U32    timeCaptureValue0FracLow;
            GT_U32    timeCaptureValue1SecHigh;
            GT_U32    timeCaptureValue1SecMed;
            GT_U32    timeCaptureValue1SecLow;
            GT_U32    timeCaptureValue1NanoHigh;
            GT_U32    timeCaptureValue1NanoLow;
            GT_U32    timeCaptureValue1FracHigh;
            GT_U32    timeCaptureValue1FracLow;
            GT_U32    timeCaptureStatus;
            GT_U32    timeUpdateCntr;
            GT_U32    PClkClockCycleConfigLow;
            GT_U32    PClkClockCycleConfigHigh;
            GT_U32    incomingClockInCountingConfigLow;
            GT_U32    incomingClockInCountingConfigHigh;
            GT_U32    generateFunctionMaskSecMed;
            GT_U32    generateFunctionMaskSecLow;
            GT_U32    generateFunctionMaskSecHigh;
            GT_U32    generateFunctionMaskNanoLow;
            GT_U32    generateFunctionMaskNanoHigh;
            GT_U32    generateFunctionMaskFracLow;
            GT_U32    generateFunctionMaskFracHigh;
            GT_U32    externalClockPropagationDelayConfigLow;
            GT_U32    externalClockPropagationDelayConfigHigh;
            GT_U32    driftThresholdConfigLow;
            GT_U32    driftThresholdConfigHigh;
            GT_U32    driftAdjustmentConfigLow;
            GT_U32    driftAdjustmentConfigHigh;
            GT_U32    clockCycleConfigLow;
            GT_U32    clockCycleConfigHigh;
            GT_U32    captureTriggerCntr;

        } TAI[10][2]/*9 GOPs + ILKN *//*two TAIs per GOP and two TAIs per ILKN*/;

        struct /*PTP[73]--Network PTPs[72] + ILKN PTP--*/{

            GT_U32    PTPInterruptCause;
            GT_U32    PTPInterruptMask;
            GT_U32    PTPGeneralCtrl;
            GT_U32    PTPTXTimestampQueue0Reg0;
            GT_U32    PTPTXTimestampQueue0Reg1;
            GT_U32    PTPTXTimestampQueue0Reg2;
            GT_U32    PTPTXTimestampQueue1Reg0;
            GT_U32    PTPTXTimestampQueue1Reg1;
            GT_U32    PTPTXTimestampQueue1Reg2;
            GT_U32    totalPTPPktsCntr;
            GT_U32    PTPv1PktCntr;
            GT_U32    PTPv2PktCntr;
            GT_U32    Y1731PktCntr;
            GT_U32    NTPTsPktCntr;
            GT_U32    NTPReceivePktCntr;
            GT_U32    NTPTransmitPktCntr;
            GT_U32    WAMPPktCntr;
            GT_U32    addCorrectedTimeActionPktCntr;
            GT_U32    NTPPTPOffsetHigh;
            GT_U32    noneActionPktCntr;
            GT_U32    forwardActionPktCntr;
            GT_U32    dropActionPktCntr;
            GT_U32    captureIngrTimeActionPktCntr;
            GT_U32    captureAddTimeActionPktCntr;
            GT_U32    captureAddIngrTimeActionPktCntr;
            GT_U32    captureAddCorrectedTimeActionPktCntr;
            GT_U32    captureActionPktCntr;
            GT_U32    addTimeActionPktCntr;
            GT_U32    addIngrTimeActionPktCntr;
            GT_U32    NTPPTPOffsetLow;
            GT_U32    txPipeStatusDelay;

        }PTP[73]/*Network PTPs + ILKN PTP*/;

        struct LMS_ST/*LMS*/{   /* Bobcat2 A0 */

            struct /*LMS0*/{

                struct /*LMS0Group0*/{

                    struct /*SMIConfig*/{

                        GT_U32    PHYAddrReg0forPorts0Through5;
                        GT_U32    PHYAutoNegConfigReg0;
                        GT_U32    SMI0Management;
                        GT_U32    LMS0MiscConfigs;

                    }SMIConfig;

                    struct /*portsMIBCntrsInterrupts*/{

                        GT_U32    portsGOP0MIBsInterruptCause;
                        GT_U32    portsGOP0MIBsInterruptMask;

                    }portsMIBCntrsInterrupts;

                    struct /*portsMACSourceAddr*/{

                        GT_U32    sourceAddrMiddle;
                        GT_U32    sourceAddrHigh;

                    }portsMACSourceAddr;

                    struct /*portsMACMIBCntrs*/{

                        GT_U32    MIBCntrsCtrlReg0forPorts0Through5;

                    }portsMACMIBCntrs;

                    struct /*portsAndMIBCntrsInterruptSummary*/{

                        GT_U32    MIBCntrsInterruptSummary;

                    }portsAndMIBCntrsInterruptSummary;

                    struct /*periodicFCRegs*/{

                        GT_U32    gigPortsPeriodicFCCntrMaxValueReg;

                    }periodicFCRegs;

                    struct /*LEDInterfaceConfig*/{

                        GT_U32    LEDInterface0CtrlReg0forPorts0Through11AndStackPort;
                        GT_U32    LEDInterface0Group01ConfigRegforPorts0Through11;
                        GT_U32    LEDInterface0Class01ManipulationRegforPorts0Through11;
                        GT_U32    LEDInterface0Class4ManipulationRegforPorts0Through11;
                        GT_U32    stackLEDInterface0Class04ManipulationRegforStackPorts;
                        GT_U32    stackLEDInterface0Class59ManipulationRegforStackPorts;
                        GT_U32    LEDInterface0FlexlinkPortsDebugSelectRegforStackPort;
                        GT_U32    LEDInterface0FlexlinkPortsDebugSelectReg1forStackPort;

                    }LEDInterfaceConfig;

                }LMS0Group0;

                struct /*LMS0Group1*/{

                    struct /*SMIConfig*/{

                        GT_U32    flowCtrlAdvertiseForFiberMediaSelectedConfigReg0forPorts0Through11;
                        GT_U32    PHYAddrReg1forPorts6Through11;
                        GT_U32    PHYAutoNegConfigReg1;

                    }SMIConfig;

                    struct /*portsMIBCntrsInterrupts*/{

                        GT_U32    triSpeedPortsGOP1MIBsInterruptCause;
                        GT_U32    triSpeedPortsGOP1MIBsInterruptMask;

                    }portsMIBCntrsInterrupts;

                    struct /*portsMACMIBCntrs*/{

                        GT_U32    MIBCntrsCtrlReg0forPorts6Through11;

                    }portsMACMIBCntrs;

                    struct /*portsAndMIBCntrsInterruptSummary*/{

                        GT_U32    portsMIBCntrsInterruptSummaryMask;
                        GT_U32    portsInterruptSummaryMask;

                    }portsAndMIBCntrsInterruptSummary;

                    struct /*periodicFCRegs*/{

                        GT_U32    twoAndHalfGigPortsPeriodicFCCntrMaxValueReg;

                    }periodicFCRegs;

                    struct /*LEDInterfaceConfig*/{

                        GT_U32    LEDInterface0CtrlReg1AndClass6ManipulationRegforPorts0Through11;
                        GT_U32    LEDInterface0Group23ConfigRegforPorts0Through11;
                        GT_U32    LEDInterface0Class23ManipulationRegforPorts0Through11;
                        GT_U32    LEDInterface0Class5ManipulationRegforPorts0Through11;
                        GT_U32    stackLEDInterface0Class1011ManipulationRegforStackPorts;
                        GT_U32    stackLEDInterface0Group01ConfigRegforStackPort;

                    }LEDInterfaceConfig;

                    GT_U32    stackPortsModeReg;

                }LMS0Group1;

            }LMS0;

            struct /*LMS1*/{

                struct /*LMS1Group0*/{

                    struct /*SMIConfig*/{

                        GT_U32    PHYAddrReg2forPorts12Through17;
                        GT_U32    PHYAutoNegConfigReg2;
                        GT_U32    SMI1Management;
                        GT_U32    LMS1MiscConfigs;

                    }SMIConfig;

                    struct /*portsMIBCntrsInterrupts*/{

                        GT_U32    portsGOP2MIBsInterruptCause;
                        GT_U32    portsGOP2MIBsInterruptMask;

                    }portsMIBCntrsInterrupts;

                    struct /*portsMACMIBCntrs*/{

                        GT_U32    MIBCntrsCtrlReg0forPorts12Through17;

                    }portsMACMIBCntrs;

                    struct /*portsInterruptSummary*/{

                        GT_U32    portsInterruptSummary;

                    }portsInterruptSummary;

                    struct /*LEDInterfaceConfig*/{

                        GT_U32    LEDInterface1CtrlReg0forPorts12Through23AndStackPort;
                        GT_U32    LEDInterface1Group01ConfigRegforPorts12Through23;
                        GT_U32    LEDInterface1Class01ManipulationRegforPorts12Through23;
                        GT_U32    LEDInterface1Class4ManipulationRegforPorts12Through23;
                        GT_U32    stackLEDInterface1Class04ManipulationRegforStackPorts;
                        GT_U32    stackLEDInterface1Class59ManipulationRegforStackPorts;
                        GT_U32    LEDInterface1FlexlinkPortsDebugSelectRegforStackPort;
                        GT_U32    LEDInterface1FlexlinkPortsDebugSelectReg1forStackPort;

                    }LEDInterfaceConfig;

                }LMS1Group0;

                struct /*LMS1Group1*/{

                    struct /*SMIConfig*/{

                        GT_U32    flowCtrlAdvertiseForFiberMediaSelectedConfigReg1forPorts12Through23;
                        GT_U32    PHYAddrReg3forPorts18Through23;
                        GT_U32    PHYAutoNegConfigReg3;

                    }SMIConfig;

                    struct /*portsMIBCntrsInterrupts*/{

                        GT_U32    portsGOP3AndStackMIBsInterruptCause;
                        GT_U32    portsGOP3AndStackMIBsInterruptMask;

                    }portsMIBCntrsInterrupts;

                    struct /*portsMACMIBCntrs*/{

                        GT_U32    MIBCntrsCtrlReg0forPorts18Through23;

                    }portsMACMIBCntrs;

                    struct /*periodicFCRegs*/{

                        GT_U32    stackPorts1GPeriodicFCCntrMaxValueReg;

                    }periodicFCRegs;

                    struct /*LEDInterfaceConfig*/{

                        GT_U32    LEDInterface1CtrlReg1AndClass6ManipulationforPorts12Through23;
                        GT_U32    LEDInterface1Group23ConfigRegforPorts12Through23;
                        GT_U32    LEDInterface1Class23ManipulationRegforPorts12Through23;
                        GT_U32    LEDInterface1Class5ManipulationRegforPorts12Through23;
                        GT_U32    stackLEDInterface1Class1011ManipulationRegforStackPort;
                        GT_U32    stackLEDInterface1Group01ConfigRegforStackPort;

                    }LEDInterfaceConfig;

                }LMS1Group1;

            }LMS1;

        }LMS[3];
        struct LED_ST    /* Bobcat2 B0 */
        {
            GT_U32 LEDControl;
            GT_U32 blinkGlobalControl;
            GT_U32 classGlobalControl;
            GT_U32 classesAndGroupConfig;
            GT_U32 classForcedData[6];
            GT_U32 portIndicationSelect[12];
            GT_U32 portTypeConfig[1];
            GT_U32 portIndicationPolarity[3];
            GT_U32 classRawData[6];
        }LED[5];
        struct SMI_ST     /* Bobcat2 B0 */
        {
            GT_U32 SMIManagement;
            GT_U32 SMIMiscConfiguration;
            GT_U32 PHYAutoNegotiationConfig;
            GT_U32 PHYAddress[16];
        }SMI[4];

        PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC FCA[72]/*ports 0..71*/;

        struct /*PR*/{

            struct /*PRMIB*/{

                GT_U32    MIBChannelSelect;
                GT_U32    MIBGeneralConfigs;
                GT_U32    MIBWraparoundInterruptCause0;
                GT_U32    MIBWraparoundInterruptMask0;
                GT_U32    MIBWraparoundInterruptCause1;
                GT_U32    MIBWraparoundInterruptMask1;
                GT_U32    MIBWraparoundInterruptCause2;
                GT_U32    MIBWraparoundInterruptMask2;

                GT_U32    MIBCountersBase;
            }PRMIB;

            struct /*PRInterrupts*/{

                GT_U32    rxFullInterruptCause0;
                GT_U32    rxFullInterruptMask0;
                GT_U32    rxFullInterruptCause1;
                GT_U32    rxFullInterruptMask1;
                GT_U32    rxFullInterruptCause2;
                GT_U32    rxFullInterruptMask2;
                GT_U32    noSOPInterruptCause0;
                GT_U32    noSOPInterruptMask0;
                GT_U32    noSOPInterruptCause1;
                GT_U32    noSOPInterruptMask2;
                GT_U32    noSOPInterruptCause2;
                GT_U32    noSOPInterruptMask1;
                GT_U32    fillOverrunInterruptCause0;
                GT_U32    fillOverrunInterruptMask0;
                GT_U32    fillOverrunInterruptCause1;
                GT_U32    fillOverrunInterruptMask1;
                GT_U32    noEOPInterruptMask2;
                GT_U32    fillOverrunInterruptMask2;
                GT_U32    noEOPInterruptMask0;
                GT_U32    fillUnderrunInterruptMask0;
                GT_U32    fillUnderrunInterruptCause1;
                GT_U32    fillUnderrunInterruptMask1;
                GT_U32    fillUnderrunInterruptCause2;
                GT_U32    fillUnderrunInterruptMask2;
                GT_U32    PRGeneralCauseReg;
                GT_U32    PRGeneralMaskReg;
                GT_U32    noEOPInterruptCause0;
                GT_U32    fillUnderrunInterruptCause0;
                GT_U32    noEOPInterruptCause1;
                GT_U32    noEOPInterruptMask1;
                GT_U32    noEOPInterruptCause2;
                GT_U32    fillOverrunInterruptCause2;
                GT_U32    PRSummaryCauseReg;
                GT_U32    PRSummaryMaskReg;

            }PRInterrupts;

            struct /*PRFunctional*/{

                GT_U32    channelConfig[64]/*Channel*/;
                GT_U32    minMaxPkts;
                GT_U32    general;

            }PRFunctional;

        }PR;

        struct /* Interlaken Register File */
        {
            GT_U32    ILKN_COUNT_BLK_TYPE_ERR_H[8]/*index*/;
            GT_U32    ILKN_COUNT_BLK_TYPE_ERR_L[16]/*index*/;
            GT_U32    ILKN_STAT_EN;
            GT_U32    ILKN_RX_PKT_GEN;
            GT_U32    ILKN_REMOTE_LANE_STATUS;
            GT_U32    ILKN_PTR_RST;
            GT_U32    ILKN_MAC_CFG_0;
            GT_U32    ILKN_PCS_PARAM;
            GT_U32    ILKN_MAC_CFG_1;
            GT_U32    ILKN_MAC_CFG_2;
            GT_U32    ILKN_MAC_CFG_3;
            GT_U32    ILKN_MAC_CFG_4;
            GT_U32    ILKN_MAC_CFG_5;
            GT_U32    ILKN_PCS_DEBUG_1;
            GT_U32    ILKN_PCS_DEBUG_0;
            GT_U32    ILKN_PCS_CFG_2;
            GT_U32    ILKN_PCS_CFG_1;
            GT_U32    ILKN_PCS_CFG_0;
            GT_U32    ILKN_OVF_7;
            GT_U32    ILKN_OVF_6;
            GT_U32    ILKN_MAC_RL_CFG_1;
            GT_U32    ILKN_OVF_4;
            GT_U32    ILKN_MAC_CFG_12;
            GT_U32    ILKN_CH_FC_CFG_1;
            GT_U32    ILKN_OVF_1;
            GT_U32    ILKN_OVF_0;
            GT_U32    ILKN_MAC_RL_CFG_2;
            GT_U32    ILKN_INT_CLR;
            GT_U32    ILKN_MAC_RL_CFG_0;
            GT_U32    ILKN_MAC_CBFC_BM_0_1;
            GT_U32    ILKN_MAC_CFG_9;
            GT_U32    ILKN_MAC_CFG_8;
            GT_U32    ILKN_MAC_CFG_7;
            GT_U32    ILKN_MAC_CFG_6;
            GT_U32    ILKN_MAC_CBFC_BM_2_0;
            GT_U32    ILKN_MAC_CBFC_BM_2_1;
            GT_U32    ILKN_MAC_CBFC_BM_2_2;
            GT_U32    ILKN_MAC_CBFC_BM_3_0;
            GT_U32    ILKN_MAC_CBFC_BM_3_1;
            GT_U32    ILKN_MAC_CFG_11;
            GT_U32    ILKN_MAC_CFG_10;
            GT_U32    ILKN_WORD_LOCK_STATUS;
            GT_U32    ILKN_SYNC_LOCK_STATUS;
            GT_U32    ILKN_MAC_CBFC_BM_CNTL;
            GT_U32    ILKN_MAC_CBFC_BM_3_2;
            GT_U32    ILKN_ERR_GEN_STATUS;
            GT_U32    ILKN_PCS_DEBUG_DATA;
            GT_U32    ILKN_MAC_FC_STATUS;
            GT_U32    ILKN_PROT_ERR_STATUS;
            GT_U32    ILKN_LINK_STATUS;
            GT_U32    ILKN_MAC_CBFC_BM_1_2;
            GT_U32    ILKN_MAC_CBFC_BM_1_1;
            GT_U32    ILKN_MAC_CBFC_BM_1_0;
            GT_U32    ILKN_MAC_CBFC_BM_0_2;
            GT_U32    ILKN_EN;
            GT_U32    ILKN_MAC_CBFC_BM_0_0;
            GT_U32    ILKN_LL_FC_STATUS;
            GT_U32    ILKN_FIFO_RST;
            GT_U32    ILKN_LANE_SWAP;
            GT_U32    ILKN_PCS_DEBUG_2;
            GT_U32    ILKN_PCS_DEBUG_3;
            GT_U32    ILKN_PCS_DEBUG_4;
            GT_U32    ILKN_PCS_DEBUG_5;
            GT_U32    ILKN_COUNT_WORD_SYNC_ERR_L[16]/*index*/;
            GT_U32    ILKN_COUNT_WORD_SYNC_ERR_H[8]/*index*/;
            GT_U32    ILKN_COUNT_TX_PKT_LOW;
            GT_U32    ILKN_COUNT_TX_PKT_HIGH;
            GT_U32    ILKN_COUNT_TX_BYTE_LOW;
            GT_U32    ILKN_COUNT_TX_BYTE_HIGH;
            GT_U32    ILKN_COUNT_TX_BAD_PKT_LOW;
            GT_U32    ILKN_COUNT_TX_BAD_PKT_HIGH;
            GT_U32    ILKN_COUNT_RX_PKT_LOW;
            GT_U32    ILKN_COUNT_RX_PKT_HIGH;
            GT_U32    ILKN_COUNT_RX_CRC_ERROR_LOW;
            GT_U32    ILKN_COUNT_RX_CRC_ERROR_HIGH;
            GT_U32    ILKN_COUNT_RX_BYTE_LOW;
            GT_U32    ILKN_COUNT_RX_BYTE_HIGH;
            GT_U32    ILKN_COUNT_RX_BURST_SIZE_ERROR_L;
            GT_U32    ILKN_COUNT_RX_BURST_SIZE_ERROR_H;
            GT_U32    ILKN_COUNT_RX_BAD_PKT_LOW;
            GT_U32    ILKN_COUNT_RX_BAD_PKT_HIGH;
            GT_U32    ILKN_COUNT_RX_ALIGNMENT_FAILURE;
            GT_U32    ILKN_COUNT_RX_ALIGNMENT_ERROR;
            GT_U32    ILKN_COUNT_DIAG_CRC_ERR_L[16]/*index*/;
            GT_U32    ILKN_COUNT_DIAG_CRC_ERR_H[8]/*index*/;
            GT_U32    ILKN_OVF_2;
            GT_U32    ILKN_OVF_3;
            GT_U32    ILKN_CH_FC_CFG_2;
            GT_U32    ILKN_OVF_5;
            GT_U32    ILKN_CH_FC_CFG_0;
            GT_U32    ILKN_ALIGN_STATUS;
        }ILKN;

        struct /* Interlaken Wrapper Register File */
        {
            GT_U32    sdFcLedConvertersControl0;
            GT_U32    txdmaConverterControl0;
            GT_U32    rxdmaConverterPacketSizeRestriction;

        }ILKN_WRP;
    }GOP;

    struct /*IPvX*/{
        struct /*routerManagementCntrsSets*/{

            GT_U32    routerManagementInUcNonRoutedExceptionPktsCntrSet[4]/*set number*/;
            GT_U32    routerManagementInMcTrappedMirroredPktCntrSet[4]/*set number*/;
            GT_U32    routerManagementInUcPktsCntrSet[4]/*set number*/;
            GT_U32    routerManagementInMcPktsCntrSet[4]/*set number*/;
            GT_U32    routerManagementInUcNonRoutedNonException[4]/*set number*/;
            GT_U32    routerManagementInMcNonRoutedNonExceptionPktsCntrSet[4]/*set number*/;
            GT_U32    routerManagementInMcNonRoutedExceptionPktsCntrSet[4]/*set nmber*/;
            GT_U32    routerManagementInUcTrappedMirroredPktCntrSet[4]/*set number*/;
            GT_U32    routerManagementInMcRPFFailCntrSet[4]/*set number*/;
            GT_U32    routerManagementOutUcPktCntrSet[4]/*set number*/;
            GT_U32    routerManagementCntrsSetConfig0[4]/*set number*/;
            GT_U32    routerManagementCntrsSetConfig1[4]/*set number*/;
            GT_U32    routerManagementIncomingPktCntr;
            GT_U32    routerManagementOutgoingPktCntr;
            GT_U32    routerManagementInSIPFilterCntr;

        }routerManagementCntrsSets;

        struct /*routerGlobalCtrl*/{

            GT_U32    routerGlobalCtrl0;
            GT_U32    routerMTUConfigReg[4]/*MTU Index*/;
            GT_U32    routerGlobalCtrl1;
            GT_U32    routerGlobalCtrl2;

        }routerGlobalCtrl;

        struct /*routerDropCntr*/{

            GT_U32    routerDropCntr;
            GT_U32    routerDropCntrConfig;

        }routerDropCntr;

        struct /*routerBridgedPktsExceptionCntr*/{

            GT_U32    routerBridgedPktExceptionsCntr;

        }routerBridgedPktsExceptionCntr;

        struct /*IPv6Scope*/{

            GT_U32    IPv6UcScopeTableReg[4]/*RegNum*/;
            GT_U32    IPv6McScopeTableReg[4]/*RegNum*/;
            GT_U32    IPv6UcScopePrefix[4];
            GT_U32    IPv6UcScopeLevel[4];

        }IPv6Scope;

        struct /*IPv6GlobalCtrl*/{

            GT_U32    IPv6CtrlReg0;
            GT_U32    IPv6CtrlReg1;

        }IPv6GlobalCtrl;

        struct /*IPv4GlobalCtrl*/{

            GT_U32    IPv4CtrlReg0;
            GT_U32    IPv4CtrlReg1;

        }IPv4GlobalCtrl;

        struct /*internalRouter*/{

            GT_U32    routerInterruptCause;
            GT_U32    routerInterruptMask;

        }internalRouter;

        struct /*FCoEGlobalCtrl*/{

            GT_U32    routerFCoEGlobalConfig;

        }FCoEGlobalCtrl;

        struct /*ECMPRoutingConfig*/{

            GT_U32    ECMPConfig;
            GT_U32    ECMPSeed;

        }ECMPRoutingConfig;

        struct /*CPUCodes*/{

            GT_U32    CPUCodes0;
            GT_U32    CPUCodes1;
            GT_U32    CPUCodes2;
            GT_U32    CPUCodes3;
            GT_U32    CPUCodes4;
            GT_U32    CPUCodes5;
            GT_U32    CPUCodes6;
            GT_U32    CPUCodes7;
            GT_U32    CPUCodes8;
            GT_U32    CPUCodes9;
            GT_U32    CPUCodes10;
            GT_U32    CPUCodes11;

        }CPUCodes;

    }IPvX;

    struct /*PLR*/{

REGISTER_COVERED_CNS        GT_U32    policerInitialDP[64]/*QoSProfile*/;
/*treated bits 0-3,5-18,21-22, 24-28,31*/        GT_U32    policerCtrl0;
/*treated bits 5,7-9*/        GT_U32    policerCtrl1;
        GT_U32    globalBillingCntrIndexingModeConfig0;
        GT_U32    policerScanCtrl;
        GT_U32    policerPortMeteringReg[16]/*register*/;
        GT_U32    policerPerPortRemarkTableID[32]/*Port*/;
        GT_U32    policerOAM;
REGISTER_COVERED_CNS        GT_U32    policerMRU;
REGISTER_COVERED_CNS        GT_U32    IPFIXSecondsMSbTimerStampUpload;
        GT_U32    policerCountingBaseAddr;
        GT_U32    policerMeteringBaseAddr;
REGISTER_COVERED_CNS        GT_U32    policerIPFIXPktCountWAThreshold;
REGISTER_COVERED_CNS        GT_U32    policerIPFIXDroppedPktCountWAThreshold;
REGISTER_COVERED_CNS        GT_U32    policerIPFIXByteCountWAThresholdMSB;
REGISTER_COVERED_CNS        GT_U32    policerIPFIXByteCountWAThresholdLSB;
        GT_U32    policerInterruptMask;
        GT_U32    policerInterruptCause;
REGISTER_COVERED_CNS        GT_U32    policerTableAccessData[8]/*Reg*/;
REGISTER_COVERED_CNS        GT_U32    IPFIXSampleEntriesLog1;
REGISTER_COVERED_CNS        GT_U32    policerError;
REGISTER_COVERED_CNS        GT_U32    policerErrorCntr;
        GT_U32    meteringAndCountingRAMSize0;
        GT_U32    meteringAndCountingRAMSize1;
        GT_U32    meteringAndCountingRAMSize2;
REGISTER_COVERED_CNS        GT_U32    policerTableAccessCtrl;
        GT_U32    policerMeteringRefreshScanRateLimit;
        GT_U32    policerMeteringRefreshScanAddrRange;
REGISTER_COVERED_CNS        GT_U32    IPFIXSecondsLSbTimerStampUpload;
REGISTER_COVERED_CNS        GT_U32    policerShadow[2]/*RegNum*/;
REGISTER_COVERED_CNS        GT_U32    IPFIXSampleEntriesLog0;
REGISTER_COVERED_CNS        GT_U32    IPFIXNanoTimerStampUpload;
REGISTER_COVERED_CNS        GT_U32    IPFIXCtrl;
/*treated bits 0-1,5-7*/        GT_U32    hierarchicalPolicerCtrl;
        GT_U32    globalBillingCntrIndexingModeConfig1;
REGISTER_COVERED_CNS       GT_U32    portAndPktTypeTranslationTable[512 ]/*port<perPort>*/;
    GT_U32      policerBadAddressLatch;
    GT_U32      policerManagementCntrsTbl;  /* PLR Management Counters */
    GT_U32      policerCountingTbl;         /* PLR Counting Table */
    GT_U32      policerTimerTbl;            /* PLR Timer Table */
    GT_U32      ipfixWrapAroundAlertTbl;    /* PLR IPFIX wrap around alert */
    GT_U32      ipfixAgingAlertTbl;         /* PLR IPFIX aging alert */
    GT_U32      meteringAddressingModeCfg0;     /* Metering Addressing Mode Configuration 0 */
    GT_U32      meteringAddressingModeCfg1;     /* Metering Addressing Mode Configuration 1 */
    GT_U32      mef10_3_bucket0MaxRateArr[128]; /* MEF 10.3 Bucket0 Max Rate */
    GT_U32      mef10_3_bucket1MaxRateArr[128]; /* MEF 10.3 Bucket1 Max Rate */
    GT_U32      qosProfile2PriorityMapArr[128]; /* Qos Profile to Priority Map */

    }PLR[3];

    struct /*CNC*/
    {
        struct /*perBlockRegs*/
        {
            struct /*wraparound*/
            {
                GT_U32    CNCBlockWraparoundStatusReg[16][4]/*Block*//*Entry*/;
            }wraparound;

            struct /*rangeCount*/
            {
                GT_U32    CNCBlockRangeCountEnable[2][16]/*group of ranges*//*block*/;
            }rangeCount;

            struct /*clientEnable*/
            {
                GT_U32    CNCBlockConfigReg0[16]/*Block*/;
            }clientEnable;

            struct /*entryMode*/
            {
                GT_U32    CNCBlocksCounterEntryModeRegister[2]/*Block/8*/;
            }entryMode;

            struct /*memory*/
            {
                GT_U32    baseAddress[16]/*Block memory base address*/;
            }memory;
        }perBlockRegs;

        struct /*globalRegs*/
        {
            GT_U32    CNCGlobalConfigReg;
            GT_U32    CNCFastDumpTriggerReg;
            GT_U32    CNCClearByReadValueRegWord0;
            GT_U32    CNCClearByReadValueRegWord1;
            GT_U32    CNCInterruptSummaryCauseReg;
            GT_U32    CNCInterruptSummaryMaskReg;
            GT_U32    wraparoundFunctionalInterruptCauseReg;
            GT_U32    wraparoundFunctionalInterruptMaskReg;
            GT_U32    rateLimitFunctionalInterruptCauseReg;
            GT_U32    rateLimitFunctionalInterruptMaskReg;
            GT_U32    miscFunctionalInterruptCauseReg;
            GT_U32    miscFunctionalInterruptMaskReg;
        }globalRegs;

    }CNC[2]/*CNC number*/;

    struct /*TXQ*/
    {
        struct /*dq*/
        {
#define MAX_DQ_NUM_PORTS_CNS        SIP_5_20_DQ_NUM_PORTS_CNS
#define SIP_5_20_DQ_NUM_PORTS_CNS   96/* 96 ports per DQ (BC3 with 6 DQs) */
#define SIP_5_DQ_NUM_PORTS_CNS      72/* 72 ports per DQ (BC2,bobk with single DQ) */
            struct /*statisticalAndCPUTrigEgrMirrToAnalyzerPort*/
            {
                                            /* 512/8 = 64 (the BC3 hold 72) */
                GT_U32    egrAnalyzerEnableIndex[PORTS_NUM_CNS/8]/*physical Port/8 -> 8 ports per register */;
                GT_U32    STCStatisticalTxSniffConfig;
                GT_U32    egrCTMTrigger;
                GT_U32    CTMAnalyzerIndex;
                GT_U32    STCAnalyzerIndex;

            }statisticalAndCPUTrigEgrMirrToAnalyzerPort;

            struct /*shaper*/
            {

                GT_U32    portTokenBucketMode[MAX_DQ_NUM_PORTS_CNS*6]/*DQ-Port*/;
                GT_U32    portDequeueEnable[MAX_DQ_NUM_PORTS_CNS*6]/*DQ-Port*/;
                GT_U32    tokenBucketUpdateRate;
                GT_U32    tokenBucketBaseLine;
                GT_U32    CPUTokenBucketMTU;
                GT_U32    portsTokenBucketMTU;

            }shaper;

            struct /*scheduler*/
            {

                struct /*schedulerConfig*/
                {

                    GT_U32    conditionalMaskForPort[MAX_DQ_NUM_PORTS_CNS*6]/*DQ-Port*/;
                    GT_U32    schedulerConfig;
                    GT_U32    pktLengthForTailDropDequeue;
                    GT_U32    highSpeedPorts[2];

                }schedulerConfig;

                struct /*priorityArbiterWeights*/
                {

                    GT_U32    profileSDWRRGroup[16]/*Profile*/;
                    GT_U32    profileSDWRRWeightsConfigReg0[16]/*Profile*/;
                    GT_U32    profileSDWRRWeightsConfigReg1[16]/*Profile*/;
                    GT_U32    profileSDWRREnable[16]/*Profile*/;
                    GT_U32    portSchedulerProfile[MAX_DQ_NUM_PORTS_CNS*6]/*DQ-Port*/;

                }priorityArbiterWeights;

                struct /*portShaper*/
                {

                    GT_U32    portRequestMaskSelector[MAX_DQ_NUM_PORTS_CNS*6]/*DQ-Port*/;
                    GT_U32    portRequestMask;
                    GT_U32    fastPortShaperThreshold;

                }portShaper;

                struct /*portArbiterConfig*/
                {

                    GT_U32    portsArbiterPortWorkConservingEnable[MAX_DQ_NUM_PORTS_CNS*6]/*DQ-port*/;
                    GT_U32    portsArbiterMap[85]/*Slice_Group*/;
                    GT_U32    portsArbiterConfig;
                    GT_U32    portsArbiterStatus;

                }portArbiterConfig;

            }scheduler;

            struct /*global*/
            {

                struct /*memoryParityError*/
                {

                    GT_U32    tokenBucketPriorityParityErrorCntr;
                    GT_U32    parityErrorBadAddr;

                }memoryParityError;

                struct /*interrupt*/
                {

                    GT_U32    flushDoneInterruptCause[4]/*pipe*/;
                    GT_U32    flushDoneInterruptMask[4]/*pipe*/;
                    GT_U32    egrSTCInterruptCause[4]/*pipe*/;
                    GT_U32    egrSTCInterruptMask[4]/*pipe*/;
                    GT_U32    txQDQInterruptSummaryCause;
                    GT_U32    txQDQInterruptSummaryMask;
                    GT_U32    txQDQMemoryErrorInterruptCause;
                    GT_U32    txQDQMemoryErrorInterruptMask;
                    GT_U32    txQDQGeneralInterruptCause;
                    GT_U32    txQDQGeneralInterruptMask;

                }interrupt;

                struct /*globalDQConfig*/
                {

                    GT_U32    portToDMAMapTable[MAX_DQ_NUM_PORTS_CNS*6]/*DQ-Port*/;
                    GT_U32    profileByteCountModification[16]/*Profile*/;
                    GT_U32    portBCConstantValue[MAX_DQ_NUM_PORTS_CNS*6]/*DQ-Port*/;
                    GT_U32    globalDequeueConfig;
                    GT_U32    BCForCutThrough;
                    GT_U32    creditCountersReset[3];

                }globalDQConfig;

                struct /*flushTrig*/
                {

                    GT_U32    portTxQFlushTrigger[MAX_DQ_NUM_PORTS_CNS*6]/*DQ-Port*/;

                }flushTrig;

                struct /*ECCCntrs*/
                {

                    GT_U32    DQIncECCErrorCntr;
                    GT_U32    DQClrECCErrorCntr;

                }ECCCntrs;

                struct /*dropCntrs*/
                {

                    GT_U32    egrMirroredDroppedCntr;
                    GT_U32    STCDroppedCntr;

                }dropCntrs;

                struct /*Credit_Counters*/
                {
                    GT_U32    txdmaPortCreditCounter[74];
                }creditCounters;

            }global;

            struct /*flowCtrlResponse*/
            {

                GT_U32    schedulerProfilePFCTimerToPriorityQueueMapEnable[16]/*profile*/;
                GT_U32    schedulerProfilePriorityQueuePFCFactor[16][8]/*Profile*//*TC*/;
                GT_U32    schedulerProfileLLFCXOFFValue[16]/*profile*/;
                GT_U32    portToTxQPortMap[256]/*Source Port*/;
                GT_U32    PFCTimerToPriorityQueueMap[8]/*TC*/;
                GT_U32    flowCtrlResponseConfig;

            }flowCtrlResponse;

        }dq[6];

        struct /*ll*/
        {

            struct /*global*/{

                struct /*portsLinkEnableStatus*/{

                    GT_U32    portsLinkEnableStatus[18];

                }portsLinkEnableStatus;

                struct /*interrupts*/{

                    GT_U32    LLInterruptCause;
                    GT_U32    LLInterruptMask;

                }interrupts;

                struct /*globalLLConfig*/{

                    GT_U32    profilePriorityQueueToPoolAssociation[16]/*profile*/;
                    GT_U32    portProfile[576]/*Port*/;
                    GT_U32    highSpeedPort[16]/*port*/;

                }globalLLConfig;

            }global;

        }ll;

        struct /*pfc*/
        {

            struct /*PFCInterrupts*/{

                GT_U32    PFCPortGroupGlobalOverUnderflowInterruptCause;
                GT_U32    PFCPortGroupGlobalOverUnderflowInterruptMask;
                GT_U32    PFCInterruptSummaryCause;
                GT_U32    PFCInterruptSummaryMask;
                GT_U32    PFCParityInterruptCause;
                GT_U32    PFCCntrsOverUnderFlowInterruptMask;
                GT_U32    PFCParityInterruptMask;
                GT_U32    PFCFIFOsOverrunsInterruptCause;
                GT_U32    PFCFIFOsOverrunsInterruptMask;
                GT_U32    PFCMsgsSetsOverrunsInterruptCause;
                GT_U32    PFCMsgsSetsOverrunsInterruptMask;
                GT_U32    PFCCntrsOverUnderFlowInterruptCause;

            }PFCInterrupts;

            GT_U32    LLFCTargetPortRemap[144]/*Port div 4*/;
            GT_U32    portFCMode[16]/*Port div 32*/;
            GT_U32    PFCSourcePortToPFCIndexMap[512]/*Global Port*/;
            GT_U32    PFCPortProfile[64]/*Port div 8*/;
            GT_U32    PFCPortGroupFlowCtrlIndicationFIFOOverrunEntry0[1]/*port group*/;
            GT_U32    PFCPortGroupFlowCtrlIndicationFIFOOverrunEntry1[1]/*port group*/;
            GT_U32    PFCPortGroupFlowCtrlIndicationFIFOOverrunCntr[1]/*port group*/;
            GT_U32    PFCPortGroupEqFIFOOverrunCntr[1]/*port group*/;
            GT_U32    PFCPortGroupCntrsStatus[1]/*port group*/;
            GT_U32    PFCPortGroupCntrsParityErrorsCntr[1]/*port group*/;
            GT_U32    PFCGroupOfPortsTCXonThresholds[8][1]/*tc*//*pipe*/;
            GT_U32    PFCGroupOfPortsTCXoffThresholds[8][1]/*tc*//*pipe*/;
            GT_U32    PFCGroupOfPortsTCDropThresholds[8][1]/*tc*//*pipe*/;
            GT_U32    PFCGroupOfPortsTCCntr[1][8]/*port group*//*tc*/;
            GT_U32    PFCTriggerGlobalConfig;
         /* GT_U32    FCModeProfileTCXOffThresholds[64] - new registers. CPSS use PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_E with the same format and number of entries */
         /* GT_U32    FCModeProfileTCXOnThresholds[64] - new registers. CPSS use PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_E with the same format and number of entries */

        }pfc;

        struct /*qcn*/
        {

            GT_U32    CCFCSpeedIndex[9]/*Reg offset*/;
            GT_U32    CNSampleTbl[8]/*qFb 3 msb*/;
            GT_U32    profilePriorityQueueCNThreshold[16][8]/*profile*//*priority*/;
            GT_U32    feedbackMIN;
            GT_U32    feedbackMAX;
            GT_U32    CNGlobalConfig;
            GT_U32    enFCTriggerByCNFrameOnPort[16]/*Port div 32*/;
            GT_U32    enCNFrameTxOnPort[16]/*Port div 32*/;
            GT_U32    feedbackCalcConfigs;
            GT_U32    txqECCConf;
            GT_U32    descECCSingleErrorCntr;
            GT_U32    descECCDoubleErrorCntr;
            GT_U32    CNTargetAssignment;
            GT_U32    CNDropCntr;
            GT_U32    QCNInterruptCause;
            GT_U32    QCNInterruptMask;
            GT_U32    CNDescAttributes;
            GT_U32    CNBufferFIFOParityErrorsCntr;
            GT_U32    CNBufferFIFOOverrunsCntr;
            GT_U32    ECCStatus;

        }qcn;

        struct /*queue*/
        {
            struct /*tailDrop*/{

                struct /*tailDropLimits*/{

                    GT_U32    poolBufferLimits[8]/*Priority*/;
                    GT_U32    poolDescLimits[8]/*Priority*/;
                    GT_U32    profilePortDescLimits[16]/*profile*/;
                    GT_U32    profilePortBufferLimits[16]/*profile*/;
                    GT_U32    globalDescsLimit;
                    GT_U32    globalBufferLimit;

                }tailDropLimits;

                struct /*tailDropCntrs*/{

                    GT_U32    portBuffersCntr[72]/*Port*/;
                    GT_U32    portDescCntr[72]/*Port*/;
                    GT_U32    priorityDescCntr[8]/*TC*/;
                    GT_U32    priorityBuffersCntr[8]/*TC*/;
                    GT_U32    totalDescCntr;
                    GT_U32    totalBuffersCntr;

                }tailDropCntrs;

                struct /*tailDropConfig*/{

                    GT_U32    portTailDropCNProfile[72]/*Port*/;
                    GT_U32    portEnqueueEnable[72]/*Port*/;
                    GT_U32    profileTailDropConfig[16]/*Profile*/;
                    GT_U32    profilePriorityQueueToPoolAssociation[16]/*profile*/;
                    GT_U32    profilePriorityQueueEnablePoolUsage[8]/*Priority*/;
                    GT_U32    profilePoolWRTDEn[16]/*profile*/;
                    GT_U32    profileQueueWRTDEn[16]/*profile*/;
                    GT_U32    profilePortWRTDEn[16]/*profile*/;
                    GT_U32    profileMaxQueueWRTDEn[16]/*profile*/;
                    GT_U32    profileECNMarkEn[16]/*Profile*/;
                    GT_U32    profileCNMTriggeringEnable[16]/*Profile*/;
                    GT_U32    globalTailDropConfig;
                    GT_U32    pktLengthForTailDropEnqueue;
                    GT_U32    maxQueueWRTDMasks;
                    GT_U32    portWRTDMasks;
                    GT_U32    queueWRTDMasks;
                    GT_U32    poolWRTDMasks;

                }tailDropConfig;

                struct /*resourceHistogram*/{

                    struct /*resourceHistogramLimits*/{

                        GT_U32    resourceHistogramLimitReg1;
                        GT_U32    resourceHistogramLimitReg2;

                    }resourceHistogramLimits;

                    struct /*resourceHistogramCntrs*/{

                        GT_U32    resourceHistogramCntr[4]/*counter*/;

                    }resourceHistogramCntrs;

                }resourceHistogram;

                struct /*mcFilterLimits*/{

                    GT_U32    mirroredPktsToAnalyzerPortDescsLimit;
                    GT_U32    ingrMirroredPktsToAnalyzerPortBuffersLimit;
                    GT_U32    egrMirroredPktsToAnalyzerPortBuffersLimit;
                    GT_U32    mcDescsLimit;
                    GT_U32    mcBuffersLimit;

                }mcFilterLimits;

                struct /*muliticastFilterCntrs*/{

                    GT_U32    snifferDescsCntr;
                    GT_U32    ingrSnifferBuffersCntr;
                    GT_U32    egrSnifferBuffersCntr;
                    GT_U32    mcDescsCntr;
                    GT_U32    mcBuffersCntr;

                }muliticastFilterCntrs;

                struct /*FCUAndQueueStatisticsLimits*/{

                    GT_U32    XONLimit[8]/*SharedLimit*/;
                    GT_U32    XOFFLimit[8]/*SharedLimit*/;
                    GT_U32    queueXONLimitProfile[16]/*Profile*/;
                    GT_U32    queueXOFFLimitProfile[16]/*Profile*/;
                    GT_U32    portXONLimitProfile[16]/*Profile*/;
                    GT_U32    portXOFFLimitProfile[16]/*Profile*/;
                    GT_U32    globalXONLimit;
                    GT_U32    globalXOFFLimit;
                    GT_U32    FCUMode;

                }FCUAndQueueStatisticsLimits;

            }tailDrop;

            struct /*global*/{

                struct /*interrupt*/{

                    struct /*queueStatistics*/{

                        GT_U32    lowCrossingStatusReg[18]/*Index*/;
                        GT_U32    highCrossingInterruptMaskPort[72]/*Port*/;
                        GT_U32    lowCrossingInterruptSummaryMaskPorts61To31;
                        GT_U32    lowCrossingInterruptSummaryMaskPorts30To0;
                        GT_U32    lowCrossingInterruptSummaryCausePorts71To62;
                        GT_U32    lowCrossingInterruptSummaryCausePorts61To31;
                        GT_U32    lowCrossingInterruptSummaryCausePorts30To0;
                        GT_U32    lowCrossingInterruptMaskPort[72]/*Port*/;
                        GT_U32    lowCrossingInterruptCausePort[72]/*Port*/;
                        GT_U32    highCrossingStatusReg[18]/*Index*/;
                        GT_U32    highCrossingInterruptSummaryMaskPorts71To62;
                        GT_U32    lowCrossingInterruptSummaryMaskPorts71To62;
                        GT_U32    highCrossingInterruptSummaryCausePorts30To0;
                        GT_U32    highCrossingInterruptSummaryMaskPorts30To0;
                        GT_U32    highCrossingInterruptSummaryCausePorts61To31;
                        GT_U32    highCrossingInterruptSummaryMaskPorts61To31;
                        GT_U32    highCrossingInterruptSummaryCausePorts71To62;
                        GT_U32    highCrossingInterruptCausePort[72]/*Port*/;

                    }queueStatistics;

                    GT_U32    transmitQueueInterruptSummaryCause;
                    GT_U32    transmitQueueInterruptSummaryMask;
                    GT_U32    generalInterrupt1Cause;
                    GT_U32    generalInterrupt1Mask;
                    GT_U32    portDescFullInterruptCause0;
                    GT_U32    portDescFullInterruptMask0;
                    GT_U32    portDescFullInterruptCause1;
                    GT_U32    portDescFullInterruptMask1;
                    GT_U32    portDescFullInterruptCause2;
                    GT_U32    portDescFullInterruptMask2;
                    GT_U32    portBufferFullInterruptCause0;
                    GT_U32    portBufferFullInterruptMask0;
                    GT_U32    portBufferFullInterruptCause1;
                    GT_U32    portBufferFullInterruptMask1;
                    GT_U32    portBufferFullInterruptCause2;
                    GT_U32    portBufferFullInterruptMask2;

                }interrupt;

                struct /*ECCCntrs*/{

                    GT_U32    TDClrECCErrorCntr;

                }ECCCntrs;

                struct /*dropCntrs*/{

                    GT_U32    clearPktsDroppedCntrPipe[4]/*Pipe*/;

                }dropCntrs;

            }global;

            struct /*peripheralAccess*/{

                struct /*peripheralAccessMisc*/{

                    GT_U32    portGroupDescToEQCntr[4]/*port group*/;
                    GT_U32    peripheralAccessMisc;
                    GT_U32    QCNIncArbiterCtrl;

                }peripheralAccessMisc;

                struct /*egrMIBCntrs*/{

                    GT_U32    txQMIBCntrsPortSetConfig[2]/*Set*/;
                    GT_U32    txQMIBCntrsSetConfig[2]/*Set*/;
                    GT_U32    setTailDroppedPktCntr[2]/*Set*/;
                    GT_U32    setOutgoingUcPktCntr[2]/*Set*/;
                    GT_U32    setOutgoingMcPktCntr[2]/*Set*/;
                    GT_U32    setOutgoingBcPktCntr[2]/*Set*/;
                    GT_U32    setMcFIFO7_4DroppedPktsCntr[2]/*Set*/;
                    GT_U32    setMcFIFO3_0DroppedPktsCntr[2]/*Set*/;
                    GT_U32    setCtrlPktCntr[2]/*Set*/;
                    GT_U32    setBridgeEgrFilteredPktCntr[2]/*Set*/;

                }egrMIBCntrs;

                struct /*CNCModes*/{

                    GT_U32    CNCModes;

                }CNCModes;

            }peripheralAccess;

        }queue;


    }TXQ;

    struct /*TMDROP*/
    {
        GT_U32    portsInterface[8]/*32 ports*/;
        GT_U32    TMDROPTMTCMap8To87[2]/*8 TM TC*/;
        GT_U32    globalConfigs;
        GT_U32    TMDROPInterruptCause;

        GT_U32    TMDROPInterruptMask;
    }TMDROP;

    struct /*TMQMAP*/
    {
        GT_U32    TC2TCMap8To16[6]/*8 TC2TC addresses*/;
        GT_U32    TMQMAPGlobalConfig;
        GT_U32    TMQMAPInterruptCause;
        GT_U32    TMQMAPInterruptMask;

    }TMQMAP;

    struct /*TM_INGR_GLUE*/{

        GT_U32    globalConfig;
        GT_U32    debug;
        GT_U32    TMIngrGlueInterruptCause;
        GT_U32    TMIngrGlueInterruptMask;

    }TM_INGR_GLUE;

    struct /*TM_EGR_GLUE*/
    {
        GT_U32    statisticalAgingTimeCntrMsb[100]/*statistical aging set*/;
        GT_U32    statisticalAgingTimeCntrLsb[100]/*statistical aging set*/;
        GT_U32    TMEgrGlueInterruptMask;
        GT_U32    TMEgrGlueInterruptCause;
        GT_U32    statisticalMinAgeTime[100]/*statistical aging set*/;
        GT_U32    statisticalMinAgeTimeShadow;
        GT_U32    statisticalMaxAgeTime[100]/*statistical aging set*/;
        GT_U32    statisticalMaxAgeTimeShadow;
        GT_U32    statisticalAgingTimeCntrsWraparoundInterruptMask0;
        GT_U32    statisticalAgingTimeCntrsWraparoundInterruptMask1;
        GT_U32    statisticalAgingTimeCntrsWraparoundInterruptMask2;
        GT_U32    statisticalAgingTimeCntrsWraparoundInterruptMask3;
        GT_U32    statisticalAgingTimeCntrsWraparoundInterruptCause0;
        GT_U32    statisticalAgingTimeCntrsWraparoundInterruptCause1;
        GT_U32    statisticalAgingTimeCntrsWraparoundInterruptCause2;
        GT_U32    statisticalAgingTimeCntrsWraparoundInterruptCause3;
        GT_U32    statisticalAgingTimeCntrsMsbShadow;
        GT_U32    statisticalAgingTimeCntrsLsbShadow;
        GT_U32    TMEgrGlueSummaryCause;
        GT_U32    TMEgrGlueSummaryMask;
        GT_U32    statisticalAgingQueueID[100]/*statistical aging set*/;
        GT_U32    statisticalAgingPktCntrsWraparoundInterruptMask0;
        GT_U32    statisticalAgingPktCntrsWraparoundInterruptMask1;
        GT_U32    statisticalAgingPktCntrsWraparoundInterruptMask2;
        GT_U32    statisticalAgingPktCntrsWraparoundInterruptMask3;
        GT_U32    statisticalAgingPktCntrsWraparoundInterruptCause0;
        GT_U32    statisticalAgingPktCntrsWraparoundInterruptCause1;
        GT_U32    statisticalAgingPktCntrsWraparoundInterruptCause2;
        GT_U32    statisticalAgingPktCntrsWraparoundInterruptCause3;
        GT_U32    statisticalAgingPktCntr[100]/*statistical aging set*/;
        GT_U32    statisticalAgingOctetCntrsWraparoundInterruptMask0;
        GT_U32    statisticalAgingOctetCntrsWraparoundInterruptMask1;
        GT_U32    statisticalAgingOctetCntrsWraparoundInterruptMask2;
        GT_U32    statisticalAgingOctetCntrsWraparoundInterruptMask3;
        GT_U32    statisticalAgingOctetCntrsWraparoundInterruptCause0;
        GT_U32    statisticalAgingOctetCntrsWraparoundInterruptCause1;
        GT_U32    statisticalAgingOctetCntrsWraparoundInterruptCause2;
        GT_U32    statisticalAgingOctetCntrsWraparoundInterruptCause3;
        GT_U32    statisticalAgingOctetCntrsMsbShadow;
        GT_U32    statisticalAgingOctetCntrsLsbShadow;
        GT_U32    statisticalAgingOctetCntrMsb[100]/*statistical aging set*/;
        GT_U32    statisticalAgingOctetCntrLsb[100]/*statistical aging set*/;
        GT_U32    statisticalAgingAgedOutPktCntrsWraparoundInterruptMask0;
        GT_U32    statisticalAgingAgedOutPktCntrsWraparoundInterruptMask1;
        GT_U32    statisticalAgingAgedOutPktCntrsWraparoundInterruptMask2;
        GT_U32    statisticalAgingAgedOutPktCntrsWraparoundInterruptMask3;
        GT_U32    statisticalAgingAgedOutPktCntrsWraparoundInterruptCause0;
        GT_U32    statisticalAgingAgedOutPktCntrsWraparoundInterruptCause1;
        GT_U32    statisticalAgingAgedOutPktCntrsWraparoundInterruptCause2;
        GT_U32    statisticalAgingAgedOutPktCntrsWraparoundInterruptCause3;
        GT_U32    statisticalAgingAgedOutPktCntrsShadow;
        GT_U32    statisticalAgingAgedOutPktCntr[100]/*statistical aging set*/;
        GT_U32    queueTimerConfig;
        GT_U32    queueTimer;
        GT_U32    portInterface0;
        GT_U32    portInterface1;
        GT_U32    portInterface2;
        GT_U32    perPortAgedOutPktCntr[192]/*TmPort*/;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptMask0;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptMask1;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptMask2;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptMask3;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptMask4;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptMask5;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptMask6;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptCause0;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptCause1;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptCause2;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptCause3;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptCause4;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptCause5;
        GT_U32    perPortAgedOutPktCntrsWraparoundInterruptCause6;
        GT_U32    globalConfig;
        GT_U32    ECCStatus;
        GT_U32    dropPortAllOutgoingPkts[192]/*TmPort*/;
        GT_U32    dropAgedPktCntrConfig;
        GT_U32    dropAgedPktCntr;
        GT_U32    debug;

    }TM_EGR_GLUE;

    struct /*TM_FCU*/
    {
        GT_U32    TMEgrFlowCtrlCntrs[64]/*word*/;
        GT_U32    TMEgrFlowCtrlXOFFThresholds[64]/*word*/;
        GT_U32    TMEgrFlowCtrlXONThresholds[64]/*word*/;
        GT_U32    TMFCUGlobalConfigs;

    }TM_FCU;

    /* Added manually */
    struct /* MG */{

        struct /*XSMI*/{

            GT_U32 XSMIManagement;
            GT_U32 XSMIAddress;
            GT_U32 XSMIConfiguration;
            GT_U32 XSMIInterruptCause;
            GT_U32 XSMIInterruptMask;

        }XSMI[2];

    }MG;

    struct PRV_CPSS_DXCH_MAC_PACKET_GEN_VER1_CONFIG_STC
    {
        GT_U32 macDa[3];
        GT_U32 macSa[3];
        GT_U32 etherType;
        GT_U32 vlanTag;
        GT_U32 packetLength;
        GT_U32 packetCount;
        GT_U32 ifg;
        GT_U32 macDaIncrementLimit;
        GT_U32 controlReg0;
        GT_U32 readAndWriteData;
        GT_U32 readAndWriteControl;
        GT_U32 dataPattern[32];
    }packGenConfig[18]; /* one packet generators per port group */

    struct PRV_CPSS_DXCH_RX_DMA_GLUE_STCT
    {
        PRV_CPSS_DXCH_PP_DP_GLUE_PIZZA_ARBITER_STC pizzaArbiter;
    }RX_DMA_GLUE;

    struct PRV_CPSS_DXCH_TX_DMA_GLUE_STCT
    {
        PRV_CPSS_DXCH_PP_DP_GLUE_PIZZA_ARBITER_STC pizzaArbiter;
    }TX_DMA_GLUE;

    struct PRV_CPSS_DXCH_MPPM_STCT
    {
        PRV_CPSS_DXCH_PP_MPPM_PIZZA_ARBITER_STC pizzaArbiter;
    }MPPM;

}PRV_CPSS_DXCH_PP_REGS_ADDR_VER1_STC;

typedef struct{

   struct /*DFXServerUnitsBC2SpecificRegs*/{

       GT_U32    deviceResetCtrl;
       GT_U32    configSkipInitializationMatrix;
       GT_U32    RAMInitSkipInitializationMatrix;
       GT_U32    trafficManagerSkipInitializationMatrix;
       GT_U32    tableSkipInitializationMatrix;
       GT_U32    SERDESSkipInitializationMatrix;
       GT_U32    EEPROMSkipInitializationMatrix;
       GT_U32    PCIeSkipInitializationMatrix;
       GT_U32    DDRSkipInitializationMatrix;
       GT_U32    deviceSAR1;
       GT_U32    deviceSAR2;
       GT_U32    deviceSAR1Override;
       GT_U32    deviceSAR2Override;
       GT_U32    deviceCtrl0;
       GT_U32    deviceCtrl4;
       GT_U32    deviceCtrl5;
       GT_U32    deviceCtrl8;
       GT_U32    deviceCtrl10;
       GT_U32    deviceCtrl12;
       GT_U32    deviceCtrl16;
       GT_U32    deviceCtrl21;
       GT_U32    deviceCtrl20;
       GT_U32    deviceCtrl19;
       GT_U32    deviceCtrl18;
       GT_U32    deviceCtrl15;
       GT_U32    deviceCtrl14;
       GT_U32    deviceStatus0;
       GT_U32    deviceStatus1;
       GT_U32    initializationStatusDone;
       GT_U32    pllMiscConfig;
       GT_U32    pllMiscParameters;
       GT_U32    deviceCtrl6;
       GT_U32    deviceCtrl7;
       GT_U32    deviceCtrl9;
       GT_U32    DFXSkipInitializationMatrix;
       GT_U32    CPUSkipInitializationMatrix;

   }DFXServerUnitsBC2SpecificRegs;

   struct /*DFXServerUnits*/{

       struct /*DFXServerRegs*/{

           GT_U32    AVSDisabledCtrl2[1]/*AVS*/;
           GT_U32    AVSDisabledCtrl1[1]/*AVS*/;
           GT_U32    temperatureSensor28nmCtrlLSB;
           GT_U32    serverXBARTargetPortConfig[4]/*Target Port*/;
           GT_U32    serverStatus;
           GT_U32    serverInterruptSummaryCause;
           GT_U32    serverInterruptSummaryMask;
           GT_U32    serverInterruptMask;
           GT_U32    serverInterruptCause;
           GT_U32    serverAddrSpace;
           GT_U32    pipeSelect;
           GT_U32    temperatureSensor28nmCtrlMSB;
           GT_U32    temperatureSensorStatus;
           GT_U32    AVSStatus[1]/*AVS*/;
           GT_U32    AVSMinStatus[1]/*AVS*/;
           GT_U32    AVSMaxStatus[1]/*AVS*/;
           GT_U32    AVSEnabledCtrl[1]/*AVS*/;
           GT_U32    snoopBusStatus;
           GT_U32    deviceIDStatus;
           GT_U32    JTAGDeviceIDStatus;
           GT_U32    PLLCoreParameters;
           GT_U32    PLLCoreConfig;
       }DFXServerRegs;

   }DFXServerUnits;

   struct /*DFXClientUnits*/{

       GT_U32    clientControl;
       GT_U32    clientDataControl;
       GT_U32    clientRamBISTInfo[4];
       GT_U32    BISTControl/*Target Port*/;
       GT_U32    BISTOpCode;
       GT_U32    BISTMaxAddress;
       GT_U32    dummyWrite;
       GT_U32    clientRamMC_Group[4];

   }DFXClientUnits;

   struct /*DFXRam*/{

       GT_U32   memoryControl;
   }DFXRam;

}PRV_CPSS_DXCH_RESET_AND_INIT_CTRL_REGS_ADDR_STC;


/*******************************************************************************
* prvCpssDxChHwRegAddrVer1Init
*
* DESCRIPTION:
*       This function initializes the registers struct for eArch devices.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrVer1Init
(
    IN  GT_U8 devNum
);

/*******************************************************************************
* prvCpssDxChHwResetAndInitControllerRegAddrInit
*
* DESCRIPTION:
*       This function initializes the Reset and Init Controller registers
*       struct for eArch devices.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwResetAndInitControllerRegAddrInit
(
    IN  GT_U8 devNum
);

/*******************************************************************************
* prvCpssDxChXcat3HwResetAndInitControllerRegAddrInit
*
* DESCRIPTION:
*       This function initializes the Reset and Init Controller (DFX) registers
*       struct for xCat3 devices.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat3HwResetAndInitControllerRegAddrInit
(
    IN  GT_U8 devNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChRegsVer1h */

