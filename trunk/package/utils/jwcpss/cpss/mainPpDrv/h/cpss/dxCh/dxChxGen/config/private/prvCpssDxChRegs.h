/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChRegs.h
*
* DESCRIPTION:
*       This file includes the declaration of the structure to hold the
*       addresses of PP registers for each different PP type.
*
*
* FILE REVISION NUMBER:
*       $Revision: 170 $
*
*******************************************************************************/
#ifndef __prvCpssDxChRegsh
#define __prvCpssDxChRegsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
#include <cpss/generic/port/private/prvCpssPortTypes.h>

/* support 4 user defined bytes */
#define USER_DEFINED_REGISTER(index)    (0x000000f0 + 4*(index))

#define REGISTER_COVERED_CNS

#define LED_MAX_NUM_OF_INTERFACE 2
#define LED_MAX_NUM_OF_CLASS     7
#define LED_MAX_NUM_OF_GROUP     4

#define MAX_NUM_OF_QOS_PROFILE_CNS 16

/* 256 registers for per port register */
#define PORTS_NUM_CNS       256

/* 64 local ports */
#define LOCAL_PORTS_NUM_CNS       64

/* 64 local ports */
#define LOCAL_PORTS_BMP_NUM_CNS   2

/* 2 registers for bmp of ports */
/* for device that supports 256 ports bmp , uses 8 words . CPU port at bit '63' */
/* for device that supports 64 ports bmp , uses 2 words . CPU port at bit '63' */
/* for device that supports 28 ports bmp + cpu bit '31' , uses only first word */
#define PORTS_BMP_NUM_CNS   ((PORTS_NUM_CNS + 31) / 32)

/* 8 registers for 'per TC' registers */
#define TC_NUM_CNS          8
/* 8 registers for 'per profile' registers */
#define PROFILE_NUM_CNS     8
/* 8 shared pools */
#define SHARED_POOLS_NUM_CNS     8
/* 2 egress counter sets */
#define EGRESS_COUNTERS_SETS_NUM_CNS     2
/* number of TPIDs */
#define TPID_NUM_CNS    8
/* number of registers in Pearson hash table */
#define PEARSON_HASH_TABLE_NUM_CNS      16
/* number of MC FIFOs */
#define MC_FIFO_NUM_CNS 2

typedef struct __txPortRegs
{
    GT_U32 txConfig;            /* Port Transmit Configuration  */
    GT_U32 wdTxFlush;           /* Port Watchdog Register       */
    GT_U32 tokenBuck;
    GT_U32 tokenBuckLen;
    GT_U32 prioTokenBuck[8];    /* per priority */
    GT_U32 prioTokenBuckLen[8]; /* per priority */
    GT_U32 wrrWeights0;         /* Port WRR Weights 0 Register  */
    GT_U32 wrrWeights1;         /* Port WRR Weights 1 Register  */
    GT_U32 wrrStrictPrio;       /* Port WRR & Strict Priority   */
    GT_U32 descrCounter;        /* Port Descriptors Counter Register */
    GT_U32 bufferCounter;       /* Port Buffers Counter Register */
    GT_U32 tcDescrCounter[8];   /* Port TC Descriptors Counter Register */
    GT_U32 tcBufferCounter[8];  /* Port TC Buffers Counter Register */
    GT_U32 l1OverheadConfig;    /* Port L1Overhead Configuration */

}dxch_txPortRegs;

/* number of lanes in the XPCS */
#define PRV_CPSS_DXCH_NUM_LANES_CNS 4
#define PRV_CPSS_DXCH_LION2_NUM_LANES_CNS 6

typedef struct
{
    GT_U32    LLFCDatabase[8]/*reg*/;
    GT_U32    portSpeedTimer[8]/*reg*/;
    GT_U32    lastSentTimer[8]/*reg*/;
    GT_U32    FCACtrl;
    GT_U32    DBReg0;
    GT_U32    DBReg1;
    GT_U32    LLFCFlowCtrlWindow0;
    GT_U32    LLFCFlowCtrlWindow1;
    GT_U32    MACDA0To15;
    GT_U32    MACDA16To31;
    GT_U32    MACDA32To47;
    GT_U32    MACSA0To15;
    GT_U32    MACSA16To31;
    GT_U32    MACSA32To47;
    GT_U32    L2CtrlReg0;
    GT_U32    L2CtrlReg1;
    GT_U32    DSATag0To15;
    GT_U32    DSATag16To31;
    GT_U32    DSATag32To47;
    GT_U32    DSATag48To63;
    GT_U32    PFCPriorityMask;
    GT_U32    sentPktsCntrLSb;
    GT_U32    sentPktsCntrMSb;
    GT_U32    periodicCntrLSB;
    GT_U32    periodicCntrMSB;

}PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC;


/*
 * Typedef: struct PRV_CPSS_DXCH_PORT_MAC_REGS_ADDR_STC
 *
 * Description: A struct including usefull mac configuration registers
 *              addresses - used for cases when port has multiple MAC
 *              units, like Tri-Speed, XG, XLG etc.
 *
 * Fields: The struct includes fields by the names of PP registers, each
 *         field represents the respective register address.
 *   macCtrl - MAC Control Register or Register0
 *   macCtrl1 - MAC Control Register1
 *   macCtrl2 - MAC Control Register2
 *   macCtrl3 - MAC Control Register3
 *   macCtrl4 - MAC Control Register4
 *   miscConfig - Misc Configuration Register
 *   fcDsaTag2 - FC DSA tag Register2
 *   fcDsaTag3 - FC DSA tag Register3
 *   ccfcFcTimer - CCFC FC Timer Register
 *   pcs40GCommonCtrl - XLG PCS common control register
 *   pcs40GCommonStatus - XLG PCS common status register
 *   pcs40GGearBoxStatus - XLG PCS gear box (state machine) status register
 *   pcs40GFecDecStatus - FEC DEC Status register
 *   ppfcControl - "Periodic Priority Flow Control" Control register
 *
 * Notes:
 *          1. MAC registers addresses of MAC appropriate for currently used
 *              port MAC see in dxch_perPortRegs structure with same names
 *          2. Not related for CPU port
 */
typedef struct
{
    GT_U32 macCtrl;
    GT_U32 macCtrl1;
    GT_U32 macCtrl2;
    GT_U32 macCtrl3;
    GT_U32 macCtrl4;
    GT_U32 macCtrl5;    /* bobcat 2 only cuurently */
    GT_U32 miscConfig;
    GT_U32 fcDsaTag2;
    GT_U32 fcDsaTag3;

    GT_U32 ccfcFcTimer;
    GT_U32 pcs40GCommonCtrl;
    GT_U32 pcs40GCommonStatus;
    GT_U32 pcs40GGearBoxStatus;
    GT_U32 pcs40GFecDecStatus;

    GT_U32 ppfcControl;

}PRV_CPSS_DXCH_PORT_MAC_REGS_ADDR_STC;

/* number of SERDESs for the port */
#define PRV_CPSS_DXCH_NUM_SERDESS_CNS 4
struct dxch_perPortRegs
{
    GT_U32 macStatus;           /* MAC Status Register      */
    GT_U32 macCounters;         /* MAC counters register.   */
    GT_U32 macCaptureCounters;   /* MAC capture port counters register.   */
    GT_U32 autoNegCtrl;         /* AutoNeg Control Register */
    GT_U32 serdesCnfg;          /* Serdes Configuration Register 0 */
    GT_U32 serdesCnfg2;         /* Serdes Configuration Register 2 */
    GT_U32 serdesCnfg3;         /* Serdes Configuration Register 3 */
    /* SERDESs registers */
    GT_U32 serdesCommon[PRV_CPSS_DXCH_NUM_SERDESS_CNS];     /* Serdes Common Module Configuration Register */
    GT_U32 serdesPowerUp1[PRV_CPSS_DXCH_NUM_SERDESS_CNS];   /* Serdes Power Up Register1 */
    GT_U32 serdesPowerUp2[PRV_CPSS_DXCH_NUM_SERDESS_CNS];   /* Serdes Power Up Register2 */
    GT_U32 serdesReset[PRV_CPSS_DXCH_NUM_SERDESS_CNS];      /* Serdes Reset Register */
    GT_U32 serdesTxSyncConf[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Tx Sync Conf Register */
    GT_U32 serdesSpeed1[PRV_CPSS_DXCH_NUM_SERDESS_CNS];     /* Serdes speed register 1 */
    GT_U32 serdesSpeed2[PRV_CPSS_DXCH_NUM_SERDESS_CNS];     /* Serdes speed register 2 */
    GT_U32 serdesSpeed3[PRV_CPSS_DXCH_NUM_SERDESS_CNS];     /* Serdes speed register 3 */
    GT_U32 serdesTxConfReg1[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Tx Configuration Register1 */
    GT_U32 serdesTxConfReg2[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Tx Configuration Register2 */
    GT_U32 serdesRxConfReg1[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Rx Configuration Register1 */
    GT_U32 serdesRxConfReg2[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Rx Configuration Register2 */
    GT_U32 serdesDtlConfReg2[PRV_CPSS_DXCH_NUM_SERDESS_CNS];/* Serdes DTL Conf Register2 */
    GT_U32 serdesDtlConfReg3[PRV_CPSS_DXCH_NUM_SERDESS_CNS];/* Serdes DTL Conf Register3 */
    GT_U32 serdesDtlConfReg5[PRV_CPSS_DXCH_NUM_SERDESS_CNS];/* Serdes DTL Conf Register5 */
    GT_U32 serdesMiscConf[PRV_CPSS_DXCH_NUM_SERDESS_CNS];   /* Serdes Misc. Configuration Register */
    GT_U32 serdesReservConf[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Reserv Conf Register */
    GT_U32 serdesIvrefConf1[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes IVREF Configuration Register1 */
    GT_U32 serdesIvrefConf2[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes IVREF Configuration Register2 */
    GT_U32 serdesProcessMon[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Process Monitor Configuration Register */
    GT_U32 serdesCalibConf1[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Calibration Configuration Register (1) */
    GT_U32 serdesCalibConf2[PRV_CPSS_DXCH_NUM_SERDESS_CNS]; /* Serdes Calibration Configuration Register 2 */
    GT_U32 serdesMetalFix;                                  /* Serdes Metal Fix Register */
    GT_U32 xgGlobalConfReg0;    /* XG Global Configuration Register 0 */
    GT_U32 xgGlobalConfReg1;    /* XG Global Configuration Register 1 */
    GT_U32 xgXpcsGlobalStatus;  /* XG XPCS Global Status Register */
    GT_U32 xpcsGlobalMaxIdleCounter;    /* XPCS Global Max Idle Counter Register */
    GT_U32 metalFix;            /* Metal Fix Register */
    GT_U32 miscConfig;          /* Misc Configuration Register */
    GT_U32 xgMibCountersCtrl;   /* XG MIB Counters Control Register - Cheetah3 only */
    GT_U32 mibCountersCtrl;     /* MIB Counters Control Register */
    GT_U32 serialParameters;    /* Serial Parameters Register */
    GT_U32 serialParameters1;   /* Serial Parameters Register 1 - Xcat2 only */
    GT_U32 hxPortConfig0[2];    /* HX Port Configuration Registers 0 -
                                   Cheetah HX ports only */
    GT_U32 hxPortConfig1[2];    /* HX Port Configuration Registers 1 -
                                   Cheetah HX ports only */
    GT_U32 prbsCheckStatus;     /* PRBS Check Status register */
    GT_U32 prbsErrorCounter;    /* PRBS Error Counter Register */
    GT_U32 ccfcFcTimerBaseGig;  /* CCFC FC Timer Register for Gig port*/
    GT_U32 ccfcFcTimerBaseXg;   /* CCFC FC Timer Register for XG port*/
    GT_U32 Config100Fx;         /* 100FX Configuration Register */

    /* lane registers */
    GT_U32 laneConfig0[PRV_CPSS_DXCH_LION2_NUM_LANES_CNS];      /* Lane Configuration Register0 */
    GT_U32 laneConfig1[PRV_CPSS_DXCH_LION2_NUM_LANES_CNS];      /* Lane Configuration Register1 */
    GT_U32 disparityErrorCounter[PRV_CPSS_DXCH_LION2_NUM_LANES_CNS];/* Disparity Error Counter Register */
    GT_U32 prbsErrorCounterLane[PRV_CPSS_DXCH_LION2_NUM_LANES_CNS];/* PRBS Error Counter Register */
    GT_U32 laneStatus[PRV_CPSS_DXCH_LION2_NUM_LANES_CNS];       /* Lane Status Register */
    GT_U32 cyclicData[PRV_CPSS_DXCH_LION2_NUM_LANES_CNS];       /* Cyclic Data First Register*/

    PRV_CPSS_DXCH_PORT_MAC_REGS_ADDR_STC macRegsPerType[PRV_CPSS_PORT_NOT_APPLICABLE_E]; /* array of mac registers
                                                                                addresses per available port types */
    GT_U32 xlgMibCountersCtrl; /* XLG MIB Counters Control Register */
    GT_U32 xlgExternalControlRegister; /* XLG External Control register */
    GT_U32 externalUnitsIntmask; /* External Units Interrupts mask */

    GT_U32 hglMacDroppedRxCellCntrsReg0; /* HyperG.Link MAC Dropped Received Cell Counters 0 Register */
    GT_U32 hglMacDroppedRxCellCntrsReg1; /* HyperG.Link MAC Dropped Received Cell Counters 1 Register */
    GT_U32 hglMacConfigReg0; /* HyperG.Link MAC Configuration 0 Register */
    GT_U32 hglGlobalConfigReg0; /* HyperG.Link Global Configuration 0 Register */
    GT_U32 hglGlobalConfigReg1; /* HyperG.Link Global Configuration 1 Register */
    GT_U32 hglMacCellCounterTypeReg; /* HGL MAC Cell Counter Type Register */
    GT_U32 hglMacRxCellCounterReg; /* HGL MAC Received Cell Counter Register */
    GT_U32 hglMacSaReg[3]; /* MAC-SA of LL-FC and E2E constructed packets. */
    GT_U32 hglMacE2eConfigReg[5]; /* E2E Configuration Registers */
    GT_U32 hglMacLLFCConfigReg[4]; /* Link Layer FC Configuration Registers */

    GT_U32 macStatus1;  /* MAC Status Register 1 for now in GE Unit only */
    GT_U32 txConfigDataReg; /* Transmit Config Reg Value Register */
    GT_U32 macroControl;    /* Macro Control (first use in XLG MAC Lion2) */

    GT_U32  xlgPortFIFOsThresholdsConfig; /* Port FIFOs Thresholds Configuration */
    GT_U32  gePortFIFOConfigReg0; /* Port FIFO Configuration Register 0 */
    GT_U32  gePortFIFOConfigReg1; /* Port FIFO Configuration Register 1 */

    PRV_CPSS_DXCH_PORT_FCA_REGS_ADDR_STC FCA;

    /* EEE support */
    GT_U32 eeeLpiControl[3];/*0,1,2*/
    GT_U32 eeeLpiStatus;
    GT_U32 eeeLpiCounter;


};

struct dxch_bufMngPerPort
{
    GT_U32 portBufAllocCounter;        /* Port Buffers Allocation Counter */
};


struct dxch_bufLimitsProfile
{
    GT_U32 associationReg0;   /* Buffers Limits Profile Association Register0 */
    GT_U32 associationReg1;   /* Buffers Limits Profile Association Register1 */
};

/* Caution: when updating this structure must update
 *          PRV_CPSS_DXCH_PORT_SERDES_REG_OFFSET_ENT
 */
struct _lpSerdesConfig
{
    GT_U32 pllIntpReg1;   /* PLL/INTP Register 1                    0   */
    GT_U32 pllIntpReg2;   /* PLL/INTP Register 2                    1   */
    GT_U32 pllIntpReg3;   /* PLL/INTP Register 3                    2   */
    GT_U32 pllIntpReg4;   /* PLL/INTP Register 4                    3   */
    GT_U32 pllIntpReg5;   /* PLL/INTP Register 5                    4   */
    GT_U32 calibrationReg0; /* Calibration Register 0               5   */
    GT_U32 calibrationReg1; /* Calibration Register 1               6   */
    GT_U32 calibrationReg2; /* Calibration Register 2               7   */
    GT_U32 calibrationReg3; /* Calibration Register 3               8   */
    GT_U32 calibrationReg5; /* Calibration Register 5               9   */
    GT_U32 calibrationReg7; /* Calibration Register 7               10  */
    GT_U32 transmitterReg0; /* Transmitter Register 0               11  */
    GT_U32 transmitterReg1; /* Transmitter Register 1               12  */
    GT_U32 transmitterReg2; /* Transmitter Register 2               13  */
    GT_U32 receiverReg0;    /* Receiver Register 0                  14  */
    GT_U32 receiverReg1;    /* Receiver Register 1                  15  */
    GT_U32 receiverReg2;    /* Receiver Register 2                  16  */
    GT_U32 ffeReg;          /* FFE Register 0                       17  */
    GT_U32 analogReservReg; /* Analog Reserved Register 0           18  */
    GT_U32 slcReg;          /* SLC Register                         19  */
    GT_U32 referenceReg;    /* Reference Register 0                 20  */
    GT_U32 powerReg;        /* Power Register 0                     21  */
    GT_U32 resetReg;        /* Reset Register 0                     22  */
    GT_U32 digitalIfReg0;   /* Digital Interface Register 0         23  */
    GT_U32 standaloneCntrlReg; /* Standalone Control Register 0     24  */
    GT_U32 serdesExternalReg1; /* Serdes External Configuration 1   25  (1 for xCat\0 for Lion) */
    GT_U32 serdesExternalReg2; /* Serdes External Configuration 2   26  (2 for xCat\1 For Lion) */
    GT_U32 serdesExternalReg3; /* Serdes External Configuration 2   27  (2 For Lion) */
    GT_U32 phyTestReg0;        /* PHY Test Register 0               28  */
    GT_U32 phyTestReg1;        /* PHY Test Register 1               29  */
    GT_U32 phyTestDataReg5;    /* PHY Test Data Register 5          30  */
    GT_U32 phyTestPrbsCntReg2; /* PHY Test PRBS Counter Register 2  31  */
    GT_U32 phyTestPrbsErrCntReg0; /* PHY Test PRBS Error Counter Register 0 32  */
    GT_U32 phyTestPrbsErrCntReg1; /* PHY Test PRBS Error Counter Register 1 33  */
    GT_U32 dfeF0F1CoefCtrl; /* DFE F0-F1 Coefficient Control        34  */
    GT_U32 testReg0;        /* Test Register 0                      35  */
};

/*
 * Typedef: struct PRV_CPSS_DXCH_PP_REGS_ADDR_STC
 *
 * Description: A struct including PP registers addresses for register access
 *              by different types of PPs.
 *
 * Fields: The struct includes fields by the names of PP registers, each
 *         field represents the respective register address.
 *
 * Notes:
 *      1.  Groups of registers (e.g: port(0,..9) control) will be represented
 *          by an array of registers addresses, for easy access to these
 *          registers.
 *      2.  Before each field which is write-only registers set and/or a pointer
 *          to a dynamically allocated buffer, bookmark is added to be used for
 *          scanning the PRV_CPSS_DXCH_PP_REGS_ADDR_STC struct linearly, without
 *          depending on the names of the fields.
 *          The fields are:
 *              sw<FieldName>Bookmark - Indicates that a size field is
 *                                      declared two words ahead.
 *              sw<FieldName>Type     - Indicates the type of data following the
 *                                      bookmark:
 *                                      can be write only data and/or pointer
 *                                      field
 *              sw<FieldName>Size     - Indicates the size (in bytes) of the
 *                                      next field (which is a pointer).
 *                                      or/and number of bytes to skip in dump
 *                                      operation
 *                                      (if case then the data is write only)
 *          The above fields must appear immediately before every dynamically
 *          allocated pointer and in the above order.
 *
 */

/* The registers DB (PRV_CPSS_DXCH_PP_REGS_ADDR_STC) is used in several generic
   algorithm like registers dump or base address auto conversion. These
   algorithms require that the PRV_CPSS_DXCH_PP_REGS_ADDR_STC structure
   and PRV_CPSS_BOOKMARK_STC have packed members. This is native in 32 Bit
   machines but not true in 64 Bit machines.
   Use 4 byte alignment to guaranty that nextPtr will be right after size in
   both registers DB and in the PRV_CPSS_BOOKMARK_STC. */
#if __WORDSIZE == 64
#pragma pack(4)
#endif
typedef struct
{

     struct _global
    {
        GT_U32          globalControl;  /* Global Control                      */
        GT_U32          extendedGlobalControl;  /* Extended Global Control */
        GT_U32          extendedGlobalControl2; /* Extended Global Control2 */
        GT_U32          generalConfigurations;  /* General Configurations   */
        GT_U32          auqConfig_generalControl;/* global */
        GT_U32          auqConfig_hostConfig;   /* Host Configuration */
        GT_U32          addrCompletion; /* PCI to memory regions map  */
        GT_U32          auQBaseAddr;    /* Addr Update Block Base address.     */
        GT_U32          fuQBaseAddr;    /* Fu Addr Update Block Base address.  */
        GT_U32          auQControl;     /* Addr Update Queue Control.          */
        GT_U32          fuQControl;     /* Fu Addr Update Queue Control.       */
        GT_U32          interruptCoalescing; /* Interrupt Coalesing Conf. Register  */
        GT_U32          addrUpdate[4];  /* address update registers            */
        GT_U32          addrUpdateControlReg;/*address update control registers*/
        GT_U32          dummyReadAfterWriteReg; /* Used for read after write   */
        GT_U32          cpuPortCtrlReg; /* CPU port control register */
        GT_U32          lastReadTimeStampReg; /* Last Read Time Stamp Register */
        GT_U32          sampledAtResetReg;    /* Sampled at Reset Register */
        GT_U32          ftdllReg;             /* Fine tune DLL Register */
        GT_U32          analogCfgReg;         /* analog config register */
        GT_U32          globalControl3;  /* Global Control3                      */
        GT_U32          globalControl4;  /* Global Control4                      */
        GT_U32          mediaInterface;  /* Media Interface */
        GT_U32          tcamPllParams;      /* TCAM PLL parameters register */
        GT_U32          tcamPllCtrl;        /* TCAM PLL control register */
        GT_U32          sdPllParams;      /* SD PLL parameters register */
        GT_U32          sdPllCtrl;        /* SD PLL control register */
        struct{
            GT_U32      mppmPllParams; /* MPPM PLL parameters register */
            GT_U32      mppmPllCtrl;   /* MPPM PLL control register */
        }mppmPllConfig;

        GT_U32          chipId;  /*Chip_ID*/
        GT_U32          metalFix;  /* Metal Fix */
        struct{
            GT_U32      baseAddress[6]; /* Winodw Base Address */
            GT_U32      windowSize[6];        /* Windows Size */
            GT_U32      highAddressRemap[6]; /* Address remap  */
            GT_U32      baseAddressEnable[6]; /* Base Address Enable */
            GT_U32      accessProtect; /* Access Protect (AP) Register */
        }addressDecoding;

        struct{
            GT_U32      padControl1;    /* Pad control 1 */
        }padsControl;

        struct{
            GT_U32      hostCpuDoorBell;
        }coprocessor;

        struct /*globalInterrupt*/
        {
            GT_U32    dfxInterruptCause;
            GT_U32    dfxInterruptMask;

        }globalInterrupt;

        GT_U32          genxsRateConfig;    /* General Access Engine (Genxs) Rate Configuration */

    }globalRegs;

    /* in different families it placed under different units,
        so let it be as independent structure */
    struct _debugBus
    {
        GT_U32  debugCtrl2;
        GT_U32  debugBus;
    }debugBusRegs;

    struct _bufferMng
    {
        struct _eqBlkCfgRegs
        {
REGISTER_COVERED_CNS            GT_U32          trSrcSniff;
REGISTER_COVERED_CNS            GT_U32          trapSniffed;
REGISTER_COVERED_CNS            GT_U32          sniffQosCfg; /* Ingress and Egress Monitoring
                                             to Analyzer QoS Configuration
                                             Register*/
REGISTER_COVERED_CNS            GT_U32          sniffPortsCfg; /* Analyzers Configuration Register*/
REGISTER_COVERED_CNS            GT_U32          analyzerPortGlobalConfig; /* Analyzer Port Global
                                                     Configuration Register.
                                                     For xCat and above */
REGISTER_COVERED_CNS            GT_U32          mirrorInterfaceParameterReg[7]; /* Mirror Interface
                                                            Parameter Register.
                                                       For xCat and above */
REGISTER_COVERED_CNS            GT_U32          portTxMirrorIndex[14]; /* Port Egress Mirror Index
                                                        For xCat and above */
REGISTER_COVERED_CNS            GT_U32          portRxMirrorIndex[3]; /* Port Ingress Mirror Index
                                                        For xCat and above */
REGISTER_COVERED_CNS            GT_U32  cpuRateLimiterConfBase; /* TO CPU Rate Limiter Configuration Register */
REGISTER_COVERED_CNS            GT_U32  cpuRateLimiterPktCntrBase; /* TO CPU Rate Limiter Packet Counter Register */
REGISTER_COVERED_CNS            GT_U32  cpuRateLimiterDropCntrReg; /* TO CPU Rate Limiter Drop Counter Register */
REGISTER_COVERED_CNS            GT_U32  preEgrEngineGlobal; /* Pre Egress Engine Global Register */
REGISTER_COVERED_CNS            GT_U32  dupPktsToCpuConfReg;/*Duplication of Packets to CPU Configuration Register*/
REGISTER_COVERED_CNS            GT_U32  ieeeResMcstCpuCodeIndexBase; /* IEEE Reserved Multicast
                                                      CPU Code Index Register */
REGISTER_COVERED_CNS            GT_U32  ipv4McstLinkLocalCpuCodeIndexBase; /* IPv4 Multicast Link Local
                                                             CPU Code Index Register */
REGISTER_COVERED_CNS            GT_U32  ipv6McstLinkLocalCpuCodeIndexBase; /* IPv6 Multicast Link Local
                                                             CPU Code Index Register */
REGISTER_COVERED_CNS            GT_U32  tcpUdpDstPortRangeCpuCodeWord0Base; /* TCP/UDP Destination Port
                                                    Range CPU Code Entry Word 0 */
REGISTER_COVERED_CNS            GT_U32  tcpUdpDstPortRangeCpuCodeWord1Base; /* TCP/UDP Destination Port
                                                    Range CPU Code Entry Word 1 */
REGISTER_COVERED_CNS            GT_U32  ipProtCpuCodeBase;         /* IP protocol CPU Code Register */
REGISTER_COVERED_CNS            GT_U32  ipProtCpuCodeValid;    /* IP protocol CPU Code Valid Register */








REGISTER_COVERED_CNS            GT_U32  cpuTargDevConfReg0;   /* CPU Code Target device configuration Register */
REGISTER_COVERED_CNS            GT_U32  cpuTargDevConfReg1;   /* CPU Code Target device configuration Register */

REGISTER_COVERED_CNS            GT_U32  logicalTargetDeviceMappingConfReg; /*Logical Target Device Mapping Configuration */
REGISTER_COVERED_CNS            GT_U32  cscdEgressMonitoringEnableConfReg; /* Cascade Egress Monitoring Enable Configuration */
REGISTER_COVERED_CNS            GT_U32  ingressDropCntrReg;   /* Ingress Drop Counter  Register */
REGISTER_COVERED_CNS            GT_U32  ingressDropCntrConfReg;   /* Ingress Drop Counter Configuration Register */
                                GT_U32  vlanEgressFilterCounterReg; /* VLAN Egress Filter Counter Register */

        } eqBlkCfgRegs;

        struct _trunkTblCfgRegs
        {

            GT_U32  baseAddrNonTrunk; /* base address of non trunk members table */

            GT_U32  baseAddrTrunkDesig; /* base address of trunk designated port table */

        } trunkTblCfgRegs;

        GT_U32  srcIdEggFltrTbl[32];
        GT_U32  devMapTbl[32];
        GT_U32  swBufMngPerPortBookmark;
        GT_U32  swBufMngPerPortType;
        GT_U32  swBufMngPerPortSize;
        struct  dxch_bufMngPerPort *bufMngPerPort GT_PACKED;   /* array of per port regs */
        struct  dxch_bufMngPerPort cpuBufMngPerPort;
        GT_U32  bufMngAgingConfig; /*Buffer Management Aging Configuration Reg*/
        GT_U32  bufMngGlobalLimit; /*Buffer Management Global Limits Reg*/
        GT_U32  cpuRxBufLimit;          /* CPU Receive Buffer Limit Reg */
        GT_U32  cpuRxBufCount;          /* CPU Receive Buffer Limit Cnt */

        GT_U32  bufMngGigaPortGroupLimit; /*Buffer Management Giga Port Group Limits Reg*/
        GT_U32  bufMngHyperGStackPortGroupLimit; /*Buffer Management HyperG.Stack Port Group Limits Reg*/

        struct  dxch_bufLimitsProfile bufLimitsProfile;
        GT_U32  bufMngSharedBufConfigReg; /* Buffer Management Shared Buffers Conf. Reg. */

        GT_U32  bufMngMetalFix;     /* Buffer Management Metal Fix Register */

        GT_U32  txdmaThresholdOverrideEn[2]; /* Threshold override enable regs */
        GT_U32  txdmaBufMemMiscTresholdsCfg; /* Buffer Memory Misc. Thresholds Configuration register */

        GT_U32  bufLimitProfileConfig[4]; /* Ports Buffers Limit Profile Configuration */
        GT_U32  totalBufAllocCntr;
        GT_U32  bufMngPacketCntr;
        GT_U32  netPortGroupAllocCntr;
        GT_U32  stackPortGroupAllocCntr;
        GT_U32  txdmaBufMemFifosThreshold[5]; /* Buffer Memory FIFOs Threshold        */
        GT_U32  bufMngAgedBuffersCntr;       /* Number of aged buffers                  */
        GT_U32  mcCntrsMemBaseAddr;          /* Multicast Counters Memory Base Address  */

        GT_U32  buffMemTxFifoThreshMPPMAccessArb;          /* Buffer Memory Tx Fifo Threshold and MPPM Access Arbiter Configuration Register  */

        GT_U32  buffMemMppmConfig;
        GT_U32  txdmaBurstLimitGlobalConfig;    /*  Burst Limit Global Configuration */
        GT_U32  txdmaPortBurstLimitThresholds[PORTS_NUM_CNS]; /* Port%p Burst Limit Thresholds */
        GT_U32  linkedListTailPtr;  /* Linked List Tail Pointer register */

        GT_U32  txdmaPZCtrlReg;
        GT_U32  txdmaPZSliceMapReg[8];
        GT_U32  txdmaCTUCtrlReg;        /* TXDMA cutThrough  */
        GT_U32  txdmaCTUSliceMapReg[8];
        GT_U32  txdmaBufMemClearType;   /* Tx DMA Buffer Memory Clear Type Register */

        GT_U32  rxdmaPZCtrlReg;
        GT_U32  rxdmaPZSliceMapReg[8];
        GT_U32  rxdmaCTUCtrlReg;         /* RXDMA cutThrough  */
        GT_U32  rxdmaCTUSliceMapReg[8];
        GT_U32  rxdmaDfxMetalFix;        /* Rx DMA Metal Fix Register            */

        GT_U32  bufMngPZGenConfiglReg;
        GT_U32  bufMngPZSlotEnableReg;
        GT_U32  bufMngPZSlotConfigReg[4];

        GT_U32  rxdmaIfConfig[2];

        GT_U32  txdmaExtendedPortsConfig;

        GT_U32  thresholdOverrideEnable[2];

        GT_U32  txdmaCountersReset;
        GT_U32  txdmaBufMemTxFifoThreshold; /* TXDMA Buffer Memory Tx Fifo Threshold and MPPM access Arbiter Configuration Register */
    }bufferMng;

    struct _mppm
    {
        GT_U32  xlgMode;
        struct _ecc
        {
            GT_U32 eccLastFailedAddress;
        }ecc;
    }mppm[2];

    struct _egrTxQConf              /* Egress and TxQ Configuration */
    {

        struct _setsConfigRegs /* per TD profile registers */
        {
            GT_U32 tcDp0Red[8][8];      /* TC<%n> DP0 Tail Drop Limits */
            GT_U32 tcDp1Red[8][8];      /* TC<%n> DP1 Tail Drop Limits */
            GT_U32 portLimitsConf[8];   /* Port Tail Drop Limits Configuration */
            GT_U32 portWRTDEn[8];       /* Port WRTD En */
            GT_U32 queueWRTDEn[8];      /* Queue WRTD En */
            GT_U32 sharedPoolWRTDEn[8]; /* Shared Pool WRTD En */
        }setsConfigRegs;

        struct _txQCountSet         /* Transmit Queue Count SET 0,1 */
        {
            GT_U32  txQConfig;         /* Configuration Register             */
            GT_U32  outUcPkts;         /* Outgoing Unicast Packet Count       */
            GT_U32  outMcPkts;         /* Outgoing Multicast Packet Count     */
            GT_U32  outBytePkts;       /* Outgoing Byte Count Packet Count    */
            GT_U32  brgEgrFiltered;    /* Bridge Egress Filtered Packet Count */
            GT_U32  txQFiltered;       /* Transmit Queue Filtered Packet Count*/
            GT_U32  outCtrlPkts;       /*outgoing control (to cpu, from cpu and
                                   to analyzer) packet count.  For Cheetah only*/
            GT_U32  egrFrwDropPkts;   /*egress forwarding restriction dropped
                               Packets Count Register. For 98DX2x5 devices only*/
        }txQCountSet[2];

        GT_U32     trStatSniffAndStcReg; /* Cheetah Tx Statistical Mirroring
                                            and STC config */
        GT_U32     txQueueConfig;       /* Transmit Queue Control       */
        GT_U32     txQueueMiscCtrl;     /* Transmit Queue Misc. Control (Metal Fix) Register */
        GT_U32     txQMcFifoEccConfig;  /* Txq Multicast FIFO and ECC Configuration */

        dxch_txPortRegs  txCpuRegs;  /* CPU Tx regs                  */
        GT_U32  swTxPortRegsBookmark;
        GT_U32  swTxPortRegsType;
        GT_U32  swTxPortRegsSize;

        dxch_txPortRegs *txPortRegs GT_PACKED; /* ptr to Tx regs struct for ports */

        GT_U32     cncCountMode;

        GT_U32     sniffTailDropCfgReg;
        GT_U32     txQueueConfigExt;   /* Transmit Queue Control Extended */
        GT_U32     tcDp0CscdDataRemap; /* Data Packets TC and DP0
                                          Remapping Register */
        GT_U32     tcDp1CscdDataRemap; /* Data Packets TC and DP0
                                          Remapping Register */
        GT_U32     txQueueResSharingAndTunnelEgrFltr; /* Transmit Queue Resource
                   Sharing And Tunnnel Egress Filtering */

        GT_U32     txQueueDpToCfiReg;          /* DP to CFI Reg. */
        GT_U32     txQueueDpToCfiPerPortEnReg; /* DP2CFI Enable Per Port Register */
        GT_U32     txQueueSharedPriorityReg[2]; /* Shared priority Reg. */

        /* Cheetah3 registers */
        GT_U32     secondaryTargetPortMapTbl[64]; /* Secondary Target Port Map Table */
        GT_U32     stackRemapPortEnReg; /* Stack remap enable per port register
                                           for fast stack failover */
        GT_U32     stackTcPerProfileRemapTbl[8]; /* Stack TC per Profile remap table */

        GT_U32     totalBuffersCounter;         /* Total Buffers Counter               */
        GT_U32     totalDescCounter;            /* Total Allocated Descriptors Counter */
        GT_U32     multicastDescriptorsCounter; /* Multicast Descriptors Counter       */
        GT_U32     snifferDescriptorsCounter;   /* Sniffer Descriptors Counter         */

        /* xCat and above registers */
        GT_U32     l2PortIsolationTableBase;  /* L2 Port Isolation table base address */
        GT_U32     l3PortIsolationTableBase;  /* L3 Port Isolation table base address */
        GT_U32     totalBufferLimitConfig;    /* Total Buffer Limit Configuration */

        /* xCat2 registers */
        GT_U32     queueWRTDMask;             /* Queue WRTD Mask */
        GT_U32     portWRTDMask;              /* Port WRTD Mask */
        GT_U32     sharedPoolWRTDMask;        /* Shared Pool WRTD Mask */

        GT_U32     dequeueSWRRWeightReg3; /* Dequeue SWRR Weight Register 3 */
        GT_U32     linkListControlConfig; /* Link List Control Config */

        struct _br_802_1_config /* BR 802.1 configuration registers */
        {
            GT_U32 globalConfig;        /* Global Configuration */
            GT_U32 txQueueDpToCfi1PerPortEnReg; /* DP2CFI1 Enable Per Port Register */
            GT_U32 pcidOfTrgPort[32];   /* PCID of target port */
        }br802_1ConfigRegs;

        GT_U32 ingressEcidTableBase;  /* Ingress ECID table */

        GT_U32 xgControl;             /* Invert XSMI MDS  */

    }egrTxQConf;


    struct _interrupts
    {

        GT_U32  intSumCauseBookmark;
        GT_U32  intSumCauseType;
        GT_U32  intSumCauseSize;
        GT_U32  intSumCause;        /* PCI Interrupt Summary Cause             */
        GT_U32  intSumMask;         /* PCI Interrupt Summary Mask              */
        GT_U32  ethBrgCause;        /* Ethernet Bridge Interrupt Cause         */
        GT_U32  ethBrgMask;         /* Ethernet Bridge Interrupt Mask Cause    */
        GT_U32  bmIntCause;         /* EPF Buffer Management Interrupt Cause   */
        GT_U32  bmIntMask;          /* EPF Buffer Management Interrupt Mask    */
        GT_U32  miscIntCause;       /* Miscellaneous Interrupt Cause           */
        GT_U32  miscIntMask;        /* Miscellaneous Interrupt Mask            */
        GT_U32  txqIntCause;        /* Transmit Queue Interrupt Cause          */
        GT_U32  txqIntMask;         /* Transmit Queue Interrupt Mask           */
        GT_U32  rxDmaIntCause;      /* Receive SDMA Interrupt Cause            */
        GT_U32  rxDmaIntMask;       /* Receive SDMA Interrupt Mask             */
        GT_U32  txDmaIntCause;      /* Transmit SDMA Interrupt Cause           */
        GT_U32  txDmaIntMask;       /* Transmit SDMA Interrupt Mask            */
    }interrupts;

    GT_U32  swTwsiRegBookmark;
    GT_U32  swTwsiRegType;
    GT_U32  swTwsiRegSize;

    struct _twsiReg
    {
          GT_U32  slaveAddr;
          GT_U32  slaveAddrExtend;
          GT_U32  data;
          GT_U32  ctrl;
          GT_U32  status;
          GT_U32  baudRate;
          GT_U32  softReset;
          GT_U32  serInitCtrl;
          GT_U32  serFinishAddr;
    }twsiReg;


    struct _sdmaRegs
    {
        GT_U32  sdmaCfgReg;         /* SDMA configuration register.         */
        GT_U32  rxQCmdReg;          /* Receive Queue Command Reg.           */
        GT_U32  rxDmaCdp[8];        /* Rx SDMA Current Rx Desc. Pointer Reg */
        GT_U32  txQCmdReg;          /* Transmit Queue Command Reg.          */
        GT_U32  txDmaCdp[8];        /* Tx Current Desc Pointer Reg.         */
        GT_U32  rxDmaResErrCnt[2];  /* Rx SDMA resource error count regs0/1.*/
        GT_U32  rxDmaPcktCnt[8];    /* Rx SDMA packet count.                */
        GT_U32  rxDmaByteCnt[8];    /* Rx SDMA byte count.                  */
        GT_U32  txQWrrPrioConfig[8];/* Tx Queue WRR Priority Configuration. */
        GT_U32  txQFixedPrioConfig; /* Tx Queue Fixed Prio Config Register. */
        GT_U32  txSdmaTokenBucketQueueCnt[8];   /* Tx SDMA Token-Bucket Queue Counter */
        GT_U32  txSdmaTokenBucketQueueConfig[8];/* Tx SDMA Token Bucket Queue Configuration */
        GT_U32  txSdmaWrrTokenParameters; /* Transmit SDMA WRR Token Parameters */
        GT_U32  txSdmaPacketGeneratorConfigQueue[8]; /* Tx SDMA Packet Generator Config Queue */
        GT_U32  txSdmaPacketCountConfigQueue[8];     /* Tx SDMA Packet Count Config Queue */
        GT_U32  rxSdmaResourceErrorCountAndMode[8];  /* Rx SDMA Resource Error Count and Mode */
    }sdmaRegs;

    struct _macRegs                 /* mac related */
    {
        GT_U32  srcAddrMiddle;      /* Source Address Middle Register  */
        GT_U32  srcAddrHigh;        /* Source Address High Register    */

        GT_U32  swPerPortRegsBookmark;
        GT_U32  swPerPortRegsType;
        GT_U32  swPerPortRegsSize;
        struct  dxch_perPortRegs *perPortRegs GT_PACKED; /* MAC regs for ports */
        GT_U32  macCountersCtrl[4];  /* MAC counters control register */
        GT_U32  hgXsmiiCountersCtr; /* HyperG.Stack Ports MIB Counters and
                                       XSMII Configuration Register */
        GT_U32  xsmiConfig;

        struct  dxch_perPortRegs cpuPortRegs;/* MAC regs for Ethernet CPU port */
        GT_U32  hxPortGlobalConfig;  /* HXPorts Global Configuration - DX269 only */

        struct { /* new xCat and above registers */
            GT_U32 QSGMIIControlRegister0;  /* 0x0a80004c + 4*n*0x400: where n (0-5) represents QSGMII */
            GT_U32 QSGMIIStatusRegister;    /* 0x0a800050 + 4*n*0x400: where n (0-5) represents QSGMII */
            GT_U32 QSGMIIPRBSErrorCounter; /* 0x0a800054 + 4*n*0x400: where n (0-5) represents QSGMII */
            GT_U32 QSGMIIControlRegister1;  /* 0x0a80044c + 4*n*0x400: where n (0-5) represents QSGMII */
            GT_U32 QSGMIIControlRegister2;  /* 0x0a80084c + 4*n*0x400: where n (0-5) represents QSGMII */
        }QSGMIIControlAndStatus[6];

    }macRegs;

    struct _xlgRegs                 /* xlg related */
    {
        struct _pcs40GRegs
        {
            GT_U32 commonCtrl;
            GT_U32 commonStatus;
            GT_U32 gearBoxStatus;
        }pcs40GRegs;

    }xlgRegs;

    struct _ledRegs
    {
        GT_U32  ledControl[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledClass  [LED_MAX_NUM_OF_INTERFACE][LED_MAX_NUM_OF_CLASS];
        GT_U32  ledGroup  [LED_MAX_NUM_OF_INTERFACE][LED_MAX_NUM_OF_GROUP];
        GT_U32  ledXgClass04Manipulation[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledXgClass59Manipulation[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledXgClass1011Manipulation[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledXgGroup01Configuration[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledHyperGStackDebugSelect[LED_MAX_NUM_OF_INTERFACE];
        GT_U32  ledHyperGStackDebugSelect1[LED_MAX_NUM_OF_INTERFACE];

    }ledRegs;


    struct _smiRegs
    {
        GT_U32 smi10GeControl;          /* 10GE SMI Control Register */
        GT_U32 smi10GeAddr;             /* 10GE SMI Address Register */
        GT_U32 smiControl;              /* SMI Control Register      */
        GT_U32 smiPhyAddr;              /* PHY Address Register0     */
        /* 98DX260/270 */
        GT_U32 swSmi10GePhyCfg0Bookmark;
        GT_U32 swSmi10GePhyCfg0Type;
        GT_U32 swSmi10GePhyCfg0Size;
        GT_U32 *smi10GePhyConfig0 GT_PACKED; /* 10GE PHY Configuration Register for ports */
        GT_U32 swSmi10GePhyCfg1Bookmark;
        GT_U32 swSmi10GePhyCfg1Type;
        GT_U32 swSmi10GePhyCfg1Size;
        GT_U32 *smi10GePhyConfig1 GT_PACKED; /* 10GE PHY Configuration Register for ports */
        GT_U32 lms0MiscConfig; /* LMS0 Misc Configurations */
        GT_U32 lms1MiscConfig; /* LMS1 Misc Configurations */
        GT_U32 xsmiConfig;     /* XSMI Configuration */

        GT_U32 lms0PhyAutoNeg[2]; /* LMS0 PHY Autoneg. */
        GT_U32 lms1PhyAutoNeg[2]; /* LMS1 PHY Autoneg. */

        GT_U32 lms0GigPortsModeReg1[2];
        GT_U32 lms1GigPortsModeReg1[2];

    }smiRegs;

    struct _miscellaneousRegs
    {
        GT_U32 lms0PeriodicFlowControlInterval0; /* Gig ports Periodic Flow Control Interval0 Register */
        GT_U32 lms1PeriodicFlowControlInterval1;  /* XG ports Periodic Flow Control Interval1 Register */
        GT_U32 lms1PeriodicFlowControlIntervalSel;/* Periodic Flow Control Interval Selection Register */
    }miscellaneousRegs;

    struct _bridge
    {
        GT_U32      swPortControlBookmark;
        GT_U32      swPortControlType;
        GT_U32      swPortControlSize;
        GT_U32      *portControl GT_PACKED;    /* Bridge Ports Control                 */
        GT_U32      swPortControl1Bookmark;
        GT_U32      swPortControl1Type;
        GT_U32      swPortControl1Size;
        GT_U32      *portControl1 GT_PACKED;    /* Bridge Ports Control                 */
        GT_U32      cpuPortControl;   /* Bridge CPU port Control */
        GT_U32      cpuPortControl1;   /* Bridge CPU port Control */
        GT_U32      l2IngressControl;   /* Layer 2 Ingress Control           */

        GT_U32      macTblAction0;      /* MAC Table Action0                 */
        GT_U32      macTblAction1;      /* MAC Table Action1  - Salsa only   */
        GT_U32      macTblAction2;      /* MAC Table Action2  - Salsa only   */

        GT_U32      macRangeFltrBase[4];      /* MAC Range Filtering bases */

        GT_U32      ipv6IcmpMsgTypeBase;    /* IPv6 ICMP Message type config */
        GT_U32      ipv6IcmpMsgCmd;         /* IPv6 ICMP Message type trap/mirror command */
        GT_U32      ipv4McastLinkLocalBase; /* IPv4 Mcast Link Local Config */
        GT_U32      ipv6McastLinkLocalBase; /* IPv6 Mcast Link Local Config */
REGISTER_COVERED_CNS        GT_U32      ipv6McastSolicitNodeAddrBase; /* IPv6 Solicit Node Multicast Address
                                                     Configuration Register 0 */
REGISTER_COVERED_CNS        GT_U32      ipv6McastSolicitNodeMaskBase; /* IPv6 Solicit Node Multicast Address
                                                     Configuration Register 0 */
        GT_U32      deviceTableBaseAddr;       /* Device Table base address Register */
REGISTER_COVERED_CNS        GT_U32      trunkTable;         /* Number of ports in each trunk*/
        GT_U32      dropIngrCntr;       /* Bridge Ingress Drop Counter */
        GT_U32      brgMacCntr0;        /* base address for MAC Addr Count0-2*/
        GT_U32      hostInPckt;         /* Host Incoming Packets Count */
        GT_U32      hostOutPckt;        /* Host Outgoing Packets Count */
        GT_U32      hostOutBrdPckt;     /* Host Outgoing Broadcast Pckt Count */
        GT_U32      hostOutMcPckt;      /* Host Outgoing Multicast Pckt Count */
        GT_U32      matrixPckt;         /* Matrix Src/Dst Packet Count */
        GT_U32      learnedDisc;        /* Learned Entry Discards Count */
        GT_U32      vlan0;              /* VLAN0 Register */
        GT_U32      vlan1;              /* VLAN1 Register */
        GT_U32      egressVlanEtherTypeSel;     /* Egress Vlan EtherType
                                                   Configuration Register
                                                   for tagged packets */
        GT_U32      egressVlanEtherTypeConfig;  /* Egress Vlan EtherType
                                                   Select Register for
                                                   tagged packets */
        GT_U32      macErrorIndicationPortConfReg; /* MAC error indication port conf. register */
        GT_U32      macErrorIndicationStatusReg[2];/* MAC error indication status register */
        GT_U32      ingFltStatus;       /* Ingress Filter */
        GT_U32      egressFilter;       /* Egress Bridging */
        GT_U32      egressFilter2;      /* Egress Bridging 2 register(Twist)*/
        GT_U32      egressFilterUnregBc; /* Egress BC Unreg. packets Filter - for 98DX2x5 devices only*/
        GT_U32      mcastLocalEnableConfig;  /* Malticast Local Enable Config Reg*/
        GT_U32      egressFilterIpMcRouted;/*Egress Filtering for IP Multicat Routed packets*/
        GT_U32      macControlReg;      /* MAC Table Control Register       */
        GT_U32      udpBcDestPortConfigReg[12]; /* UDP BC destination port Config. reg. */
        GT_U32      bridgeIpv6BytesSelection; /* IPv6 MC Bridging Bytes
                                            Selection Configuration Register */
        GT_U32      vlanMruProfilesConfigReg; /* Vlan MPU profiles confog reg -
                                                for 98DX2x5 devices only */

        struct _brgCntrSet
        {
            GT_U32  cntrSetCfg;         /* Counter Set Configuration      */
            GT_U32  inPckt;             /* Incoming Packet Count          */
            GT_U32  inFltPckt;          /* Ingress Filtered Packet Count  */
            GT_U32  secFltPckt;         /* Security Filtered Packet Count */
            GT_U32  brgFltPckt;         /* Bridge Filtered Packet Count   */
        }brgCntrSet[2];

        struct _nstRegs /* for 98DX2x5 devices only */
        {
            GT_U32  brgAccessMatrix;       /* Bridge Access Matrix */
            GT_U32  brgSecureLevelConfReg; /* Bridge Security Levels configuration Register  */
REGISTER_COVERED_CNS            GT_U32  cpuIngrFrwFltConfReg; /* To CPU ingress forwarding restrictions Config Reg */
REGISTER_COVERED_CNS            GT_U32  netIngrFrwFltConfReg; /* To Network ingress forwarding restrictions Config Reg */
REGISTER_COVERED_CNS            GT_U32  analyzerIngrFrwFltConfReg; /* To Analyzer ingress forwarding restrictions Config Reg */
            GT_U32  cpuPktsFrwFltConfReg; /* From CPU packets filter Config Reg */
            GT_U32  brgPktsFrwFltConfReg; /* Bridged packets filter Config Reg */
            GT_U32  routePktsFrwFltConfReg;/* Routed packets filter Config Reg */
REGISTER_COVERED_CNS            GT_U32  ingressFrwDropCounter;    /* Ingress Forwarding Restrictions Drop counter */
        }nstRegs;

        struct _srcIdReg /*For 98DX2x5 devices only */
        {
                GT_U32  srcIdAssignedModeConfReg; /* Source ID assigment mode Config. Reg.*/
                GT_U32  srcIdUcEgressFilterConfReg; /* Unicast SrcId Egress filtering Config. Reg.*/
        }srcIdReg;

        struct _vntReg /*For Ch3 devices only */
        {
            GT_U32  vntOamLoopbackConfReg;  /* OAM loopback enable per port register .*/
            GT_U32  vntCfmEtherTypeConfReg; /* CFM EtherType Config. Reg.*/
            GT_U32  vntGreEtherTypeConfReg; /* GRE EtherType Config. Reg */
        }vntReg;

        struct _vlanRegs
        {
            GT_U32      protoTypeBased;     /* Protocols               */
            GT_U32      protoEncBased;      /* Protocols Encapsulation */
            GT_U32      protoEncBased1;     /* Protocols Encapsulation1 */
        }vlanRegs;

        struct _qosProfileRegs
        {
            GT_U32  dscp2QosProfileMapTable;    /* DSCP2QoS-Profile Map Table */
            GT_U32  up2QosProfileMapTable;      /* UP2QoS-Profile Map Table */
            GT_U32  exp2QosProfileMapTable;     /* EXP2QoS-Profile Map Table */
            GT_U32  dscp2DscpMapTable;          /* Dscp2Dscp Mutation Table */
            GT_U32  macQosTable;                /* MAC QoS Table*/
        }qosProfileRegs;

        GT_U32  bridgeRateLimitConfigRegArray[2];     /*Bridge RateLimit Configuration Registers for cheetah*/
        GT_U32  bridgeGlobalConfigRegArray[3];        /* Bridge Global Configuration Registers */
        GT_U32  bridgeGlobalSecurBreachDropCounter;   /* Bridge Global Security Breach DropCounter - Cheetah */
        GT_U32  bridgePortVlanSecurBreachDropCounter; /* Bridge Port/VLAN Security Breach DropCounter - Cheetah */
        GT_U32  bridgeSecurBreachStatusRegArray[3];   /* Bridge Security Breach Status Registers - Cheetah */
        GT_U32  bridgeRateLimitDropCounter[2];        /* Bridge Rate Limit Drop Counter */

        struct _ingressLog /*For Ch3 devices only */
        {
            GT_U32  etherType;      /* Ingress Log EtherType    */
            GT_U32  daLow;          /* Ingress Log DA low       */
            GT_U32  daHigh;         /* Ingress Log DA high      */
            GT_U32  daLowMask;      /* Ingress Log DA low Mask  */
            GT_U32  daHighMask;     /* Ingress Log DA high Mask */
            GT_U32  saLow;          /* Ingress Log SA low       */
            GT_U32  saHigh;         /* Ingress Log SA high      */
            GT_U32  saLowMask;      /* Ingress Log SA low Mask  */
            GT_U32  saHighMask;     /* Ingress Log SA high Mask */
            GT_U32  macDaResult;    /* MAC DA result            */
            GT_U32  macSaResult;    /* MAC SA result            */
        }ingressLog;

        /* xCat and above section */
        GT_U32  bridgeIngressVlanEtherTypeTable[4]; /* Ingress Vlan Ethertype table configuration registers */
        GT_U32  bridgeIngressVlanSelect[LOCAL_PORTS_NUM_CNS/2];  /* Ingress Vlan Select registers */

        GT_U32  ieeeReservedMcastProfileSelect0; /* IEEE Reserved Multicast Profile table select 0 */
        GT_U32  ieeeReservedMcastProfileSelect1; /* IEEE Reserved Multicast Profile table select 1 */

        /* xCat2 and above */
        GT_U32 miiSpeedGranularity;   /* Ingress rate limit MII Speed Granularity */
        GT_U32 gmiiSpeedGranularity;  /* Ingress rate limit GMII Speed Granularity */
        GT_U32 stackSpeedGranularity; /* Ingress rate limit Stack Ports Speed Granularity */

        struct _bridgeInternal
        {
            GT_U32  dft;    /* DFT register */
        }bridgeInternal;

        struct _fdbInternal
        {
            GT_U32  metalFix;    /* FDB metal fix register */
        }fdbInternal;

    }bridgeRegs;

    struct _pcl
    {
        GT_U32  PclBookmark;         /* because pcl need special function to read fromwe need book mark*/
        GT_U32  PclType;             /* because pcl need special function to read fromwe need book mark*/
        GT_U32  PclSize;             /* because pcl need special function to read fromwe need book mark*/
REGISTER_COVERED_CNS        GT_U32  control;            /* PCL control                          */
        /* 98DX250/260/270 */
        /* registers - buffer for triggered operation */
        GT_U32 tcamWriteData[12];
        GT_U32 tcamReadMaskBase;
        GT_U32 tcamReadPatternBase;
        GT_U32 tcamReadPatternParityBase;
        GT_U32 tcamReadMaskParityBase;
        GT_U32 tcamOperationTrigger;
        GT_U32 tcamOperationParam1;
        GT_U32 tcamOperationParam2;

        GT_U32 tcamReadMaskControlBase;
        GT_U32 tcamReadPatternControlBase;
        GT_U32 actionTableBase;

        /* user defined bytes configurations */
REGISTER_COVERED_CNS        GT_U32 udbConfigKey0bytes0_2;
REGISTER_COVERED_CNS        GT_U32 udbConfigKey0bytes3_5;
REGISTER_COVERED_CNS        GT_U32 udbConfigKey1bytes0_2;
REGISTER_COVERED_CNS        GT_U32 udbConfigKey1bytes3_5;
REGISTER_COVERED_CNS        GT_U32 udbConfigKey2bytes0_2;
REGISTER_COVERED_CNS        GT_U32 udbConfigKey3bytes0_2;
REGISTER_COVERED_CNS        GT_U32 udbConfigKey4bytes0_1;
REGISTER_COVERED_CNS        GT_U32 udbConfigKey5bytes0_2;
        /* rule match conters base 0x0B801000 */
REGISTER_COVERED_CNS        GT_U32 ruleMatchCounersBase;

        /*98DX255/265/275/285*/
        /* EPCL - egress PCL engine */
        GT_U32 epclGlobalConfig;
        /* bit per port registers */
REGISTER_COVERED_CNS        GT_U32 epclCfgTblAccess[PORTS_BMP_NUM_CNS];
REGISTER_COVERED_CNS        GT_U32 epclEnPktNonTs[PORTS_BMP_NUM_CNS];
REGISTER_COVERED_CNS        GT_U32 epclEnPktTs[PORTS_BMP_NUM_CNS];
REGISTER_COVERED_CNS        GT_U32 epclEnPktToCpu[PORTS_BMP_NUM_CNS];
REGISTER_COVERED_CNS        GT_U32 epclEnPktFromCpuData[PORTS_BMP_NUM_CNS];
REGISTER_COVERED_CNS        GT_U32 epclEnPktFromCpuControl[PORTS_BMP_NUM_CNS];
REGISTER_COVERED_CNS        GT_U32 epclEnPktToAnalyzer[PORTS_BMP_NUM_CNS];
        /* TCP/UDP port comparators (8 register sequences) */
REGISTER_COVERED_CNS        GT_U32 ipclTcpPortComparator0;
REGISTER_COVERED_CNS        GT_U32 ipclUdpPortComparator0;
REGISTER_COVERED_CNS        GT_U32 epclTcpPortComparator0;
REGISTER_COVERED_CNS        GT_U32 epclUdpPortComparator0;
        /* Ch3 registers */
REGISTER_COVERED_CNS        GT_U32 fastStack; /* fast stack failover */
REGISTER_COVERED_CNS        GT_U32 loopPortReg;         /* loop port for fast stack failover */
       /* CNC relted registers */
REGISTER_COVERED_CNS        GT_U32 cncCountMode;
REGISTER_COVERED_CNS        GT_U32 cncl2l3IngressVlanCountEnable;
        /* TCAM low level HW configuration - CH3 only */
        GT_U32 pclTcamConfig0;
        GT_U32 pclTcamConfig2;
        GT_U32 pclTcamControl;
        /*ch3 only*/
REGISTER_COVERED_CNS        GT_U32 routedPacket;
        /* xCat and above registers */
REGISTER_COVERED_CNS        GT_U32 pclUDBConfig;
REGISTER_COVERED_CNS        GT_U32 pclEngineConfig;
REGISTER_COVERED_CNS        GT_U32 pclEngineExtUdbConfig;
        GT_U32 ttiUnitConfig;
        GT_U32 ttiEngineConfig;
        /* Egress Policer Global Configuration*/
REGISTER_COVERED_CNS        GT_U32 eplrGlobalConfig;
REGISTER_COVERED_CNS        GT_U32 eplrPortBasedMetering[PORTS_BMP_NUM_CNS];   /* Egress Policer Port Based Metering */

        struct _policyTcamTest
        {
            GT_U32  pointerRelReg[14];  /* Pointer (to line for effuse replacement) Register */
            GT_U32  configStatusReg;    /* config status Register */
            GT_U32  opcodeExpectedHitReg; /* Opcode Expected Hit Rel */
            GT_U32  opcodeCommandReg;     /* Opcode Command Register */
        }policyTcamTest;

        struct _pclTccRegs
        {
            GT_U32  policyTcamControl;          /* Policy TCAM Control register */
            GT_U32  policyTccErrorInformation;  /* Policy TCC error information register */
            GT_U32  policyTcamErrorCounter;     /* Policy TCAM error counter */
        }pclTccRegs;

REGISTER_COVERED_CNS        GT_U32  crcHashConfigurationReg;/* CRC Hash Configuration register */
REGISTER_COVERED_CNS        GT_U32  pearsonHashTableReg[PEARSON_HASH_TABLE_NUM_CNS];/* Pearson Hash Table registers */
        /* Policy Engine Interrupt Cause */
REGISTER_COVERED_CNS        GT_U32 ipclEngineInterruptCause;

REGISTER_COVERED_CNS        GT_U32 configTblParityErrCtr[2]; /* Config Table Parity Error Counter per PclConfigTable */

        struct _macToMe
        {
REGISTER_COVERED_CNS            GT_U32  macLow; /* for now to save place I add just this register, other could be added
                               when needed */
        }macToMe[8];

    }pclRegs;

    struct _ipv4Regs
    {
        GT_U32  ipTunnelTerm0;      /* IP tunnel termination register 0.    */
        GT_U32  ipTunnelTerm1;      /* IP tunnel termination register 1.    */
        GT_U32  vrTableBase;        /* Base address of the Vr Table.        */
        GT_U32  mcLpmRootBase;      /* Multicast LPM Root Node Base Address */
        GT_U32  defMcTblBase;       /* Base address of the Default Mc Tbl.  */
        GT_U32  mllBaseAddr;        /* Base address of Mll memory block.    */
        GT_U32  ipv4CtrlReg;        /* IPv4 Global control register.        */
        GT_U32  rxPcktCnt[3];       /* Receive Packet Counter Table Register*/
        GT_U32  rxOctetCntLow[3];   /* Receive Octet Counter Low Register   */
        GT_U32  rxOctetCntHigh[3];  /* Receive Octet Counter High Register  */
        GT_U32  discardCnt[3];      /* Discard Packet Counter Register      */
        GT_U32  discardOctet[3];    /* Discard Octet Counter Register       */
        GT_U32  ipMcPcktCnt[3];     /* Ip-Mc packets Counter Register       */
        GT_U32  trapPcktCnt[3];     /* Trapped Packet Counter Register      */
        GT_U32  tunnelTermPcktCnt[3];/* Tunnel Terminate Packet Counter Reg.*/
        GT_U32  failRpfPcktCnt[3];  /* Failed Reverse Path Forwarding Packet*/
                                    /* Counter Register.                    */
        GT_U32  ipGlobalCntReg0;    /* IPv4 Global Counter 0 Register.      */
        GT_U32  ipGlobalCntReg1;    /* IPv4 Global Counter 1 Register.      */
        GT_U32  ipGlobalCntReg2;    /* IPv4 Global Counter 2 Register.      */
        GT_U32  ipGlobalDiscardPcktCnt; /* IPv4 Discard Packet Counter Reg. */
        GT_U32  ipGlbDiscardOctetCntLow;/* IPv4 Discard Octet Low Counter   */
                                    /* Register.                            */
        GT_U32  ipGlbDiscardOctetCntHigh;/* IPv4 Discard Octet Low Counter  */
                                    /* Register.                            */
        GT_U32  ipv4ShadowCntReg;   /* IPv4 Shadow Counter Register.        */
        GT_U32  dscp2CosTable;      /* dscp to cos table                    */

        /* cheetah2 regs */

        GT_U32 ctrlReg0;    /* IPv4 Control Register0 */

        GT_U32 ctrlReg1;    /* IPv4 Control Register1 */

        GT_U32 ucEnCtrlReg; /* IPv4 Unicast Enable Control Register */

        GT_U32 mcEnCtrlReg; /* IPv4 Multicast Enable Control Register */

    }ipv4Regs;               /* ipRegs is Registers set for IPv4 & IPv6     */

    struct _ipv6Regs
    {
        GT_U32 ipv6CtrlReg;
        GT_U32 erroneousLpmTbl;
        GT_U32 ipv6vrTableBase; /* Base address of the ipv6 Vr Table.        */

        GT_U32 ucScopeTblRegs[4]; /* IPv6 Unicast Scope Command Table
                                     Registers */
        GT_U32 mcScopeTblRegs[4]; /* IPv6 multicast Scope Command Table
                                     Registers */
        GT_U32 ipScopeRegs[4]; /* IPv6 Unicast Scope Prefix<n> Registers */
        GT_U32 ipScopeCmdReg;

        GT_U32 mldSnoopingRegs[3];
        GT_U32 outblockCtrlReg;

        GT_U32  rxPcktCnt[3];       /* Receive Packet Counter Table Register*/
        GT_U32  rxOctetCntLow[3];   /* Receive Octet Counter Low Register   */
        GT_U32  rxOctetCntHigh[3];  /* Receive Octet Counter High Register  */
        GT_U32  discardCnt[3];      /* Discard Packet Counter Register      */
        GT_U32  discardOctet[3];    /* Discard Octet Counter Register       */
        GT_U32  ipMcPcktCnt[3];     /* Ip-Mc packets Counter Register       */
        GT_U32  trapPcktCnt[3];     /* Trapped Packet Counter Register      */
        GT_U32  failRpfPcktCnt[3];  /* Failed Reverse Path Forwarding Packet*/
                                    /* Counter Register.                    */
        GT_U32  ipGlobalCntReg0;    /* IPv6 Global Counter 0 Register.      */
        GT_U32  ipGlobalCntReg1;    /* IPv6 Global Counter 1 Register.      */
        GT_U32  ipGlobalCntReg2;    /* IPv6 Global Counter 2 Register.      */
        GT_U32  ipGlobalDiscardPcktCnt; /* IPv6 Discard Packet Counter Reg. */
        GT_U32  ipGlbDiscardOctetCntLow;/* IPv6 Discard Octet Low Counter   */
                                    /* Register.                            */
        GT_U32  ipGlbDiscardOctetCntHigh;/* IPv6 Discard Octet Low Counter  */
                                    /* Register.                            */
        GT_U32  ipv6ShadowCntReg;   /* IPv6 Shadow Counter Register.        */

        /* cheetah2 regs */

        GT_U32 ctrlReg0;    /* IPv6 Control Register0 */
        GT_U32 ctrlReg1;    /* IPv6 Control Register1 */
        GT_U32 ucEnCtrlReg; /* IPv6 Unicast Enable Control Register */
        GT_U32 mcEnCtrlReg; /* IPv6 Multicast Enable Control Register */

        GT_U32 ipScopeLevel[4]; /* IPv6 Unicast Scope<n> Level Registers */

    }ipv6Regs;

    struct _ipRegs
    {

        GT_U32 ctrlReg0;
        /* cheetah + Ctrl regs */

        /* cheetah2 regs */
        GT_U32 routerGlobalReg;             /*Router Global Control Register*/

        GT_U32 routerMtuCfgRegs[4];         /*Router MTU Configuration Register*/

        GT_U32 routerTtTcamAccessCtrlReg; /*Router TCAM Access Control
                                            Register*/

        GT_U32 routerTtTcamAccessDataCtrlReg; /*Router and TT TCAM Access Data
                                                Control Register*/

        GT_U32 routerTtTcamAccessDataRegsBase; /*Router and TT TCAM Access Data
                                                Registers base*/

        GT_U32 routerTtTcamBankAddr[5];     /* bank address for direct access to single entry*/
                                            /* in router / tunnel termination TCAM           */

        GT_U32 ucMcRouteEntriesAgeBitsBase; /*Router Next Hop Table Age Bits
                                              Entries Base*/

        GT_U32 routerMngCntSetCfgRegs[4]; /* Router Management Counters Set<n>
                                             Configuration Register*/

        GT_U32 routerMngInUcPktCntSet[4]; /* Router Management In Unicast
                                             Packets Counter Set<n> */
        GT_U32 routerMngMcPktCntSet[4];   /* Router Management In Multicast
                                             Packets Counter Set<n> */
        GT_U32 routerMngInUcNonRoutedNonExcptPktCntSet[4];/*Router Management In
                                                          Unicast Non-Routed
                                                          Non-Exception Packets
                                                          Counter Set<n> */
        GT_U32 routerMngInUcNonRoutedExcptPktCntSet[4];/*Router Management In
                                                       Unicast Non-Routed
                                                       Exception Packets Counter
                                                       Set<n> */

        GT_U32 routerMngInMcNonRoutedNonExcptPktCntSet[4];/*Router Management In
                                                          Multicast Non-Routed
                                                          Non-Exception Packets
                                                          Counter Set<n> */

        GT_U32 routerMngInMcNonRoutedExcptCntSet[4];/*Router Management In
                                                    Multicast Non-Routed
                                                    Exception Packets Counter
                                                    Set<n> */
        GT_U32 routerMngInUcTrapMrrPktCntSet[4]; /* Router Management In Unicast
                                                    Trapped / Mirrored Packet
                                                    Counter Set<n> */
        GT_U32 routerMngInMcTrapMrrPktCntSet[4]; /* Router Management In
                                                    Multicast Trapped / Mirrored
                                                    Packet Counter Set<n> */
        GT_U32 routerMngInMcRPFFailCntSet[4]; /* Router Management In Multicast
                                                 RPF Fail Counter Set<n> */
        GT_U32 routerMngOutUcPktCntSet[4]; /* Router Management Out Unicast
                                              Packet Counter Set<n> */

        GT_U32 routerBridgedPktExcptCnt; /* Router Bridged Pkt Exceptions
                                            Counter */
        GT_U32 routerDropCntCfgReg; /* Router Drop Counter Configuration
                                       Register */
        GT_U32 routerDropCnt; /* Router Drop Counter */

        GT_U32 routerInterruptCauseReg; /* Router Interrupt Cause Register */

        GT_U32 routerInterruptMaskReg; /* Router Interrupt Mask Register */

        GT_U32 routerAdditionalCtrlReg; /* Router Additional Control Register */

        GT_U32 ucRoutingEngineConfigurationReg; /* Unicast Routing Engine Configuration Register*/

        GT_U32 qoSProfile2RouteBlockOffsetMapTable[MAX_NUM_OF_QOS_PROFILE_CNS];   /* QoSProfile to Route Block Offset Table */

        GT_U32 routerAccessMatrixBase; /* router Access matrix Base address*/

        struct _ipHitLog
        {
            GT_U32  dipAddrReg[4];       /* IP Hit-Log dip address */
            GT_U32  dipMaskAddrReg[4];   /* IP Hit-Log dip mask address */
            GT_U32  sipAddrReg[4];       /* IP Hit-Log sip address */
            GT_U32  sipMaskAddrReg[4];   /* IP Hit-Log sip mask address */
            GT_U32  layer4DestPortReg;   /* IP Hit-Log Layer4 Destination Port Register */
            GT_U32  layer4SrcPortReg;    /* IP Hit-Log Layer4 Source Port Register */
            GT_U32  protocolReg;         /* IP Hit-Log Protocol Register */
            GT_U32  dstIpNheEntryReg[3]; /* IP Hit-Log Destination IP NHE Entry Register */
            GT_U32  srcIpNheEntryReg[3]; /* IP Hit-Log Source IP NHE Entry Register */
        }ipHitLog;

        /* TCAM low level HW configuration - CH3 only */
        GT_U32 ipTcamConfig0;
        GT_U32 ipTcamControl;

        struct _routerTcamTest
        {
            GT_U32  pointerRelReg[20];  /* Pointer (to line for effuse replacement) Register */
            GT_U32  configStatusReg;    /* config status Register */
            GT_U32  opcodeExpectedHitReg; /* Opcode Expected Hit Rel */
            GT_U32  opcodeCommandReg;     /* Opcode Command Register */
        }routerTcamTest;

        /* Lion Regs */
        GT_U32 routerPerPortSipSaEnable0; /* Router per Port SIP/SA Enable*/

        /* Lion2 Regs */
        GT_U32 routerDataError; /* Internal Router Data Error register */

        /* Lion2 TCAM registers */
        struct _ipTccRegs
        {
            GT_U32  ipTcamControl;          /* IPvX TCAM Control register */
            GT_U32  ipTccErrorInformation;  /* Ip TCC error information register */
            GT_U32  ipTcamErrorCounter;     /* Ip TCAM error counter */
            GT_U32  ipTcamReadPatternParityBase; /* Base address of Parity Table X */
            GT_U32  ipTcamReadMaskParityBase; /* Base address of Parity Table Y */
        }ipTccRegs;

    }ipRegs;               /* ipRegs is Registers set for IPv4 & IPv6     */

    struct _ipMtRegs
    {

REGISTER_COVERED_CNS        GT_U32 qoSProfile2MTTCQueuesMapTable[8];   /* QoSProfile to Multi Target TC Queues Table */

REGISTER_COVERED_CNS        GT_U32 mllGlobalReg;            /* MLL Global Control Register */

REGISTER_COVERED_CNS        GT_U32 multiTargetRateShapingReg;   /* Multi Target Rate Shaping Register */

REGISTER_COVERED_CNS        GT_U32 mtUcSchedulerModeReg;    /* Multi Target Vs. Unicast SDWRR */
                                        /* and Strict Priority Scheduler */

REGISTER_COVERED_CNS        GT_U32 mtTcQueuesSpEnableReg;   /* Multi Target TC Queues Strict Priority */
                                        /* Enable Configuration Register*/

REGISTER_COVERED_CNS        GT_U32 mtTcQueuesWeightReg;     /* Multi Target TC Queues Weight */
                                        /* Configuration Register*/

REGISTER_COVERED_CNS        GT_U32 mtTcQueuesGlobalReg;     /* Multi Target TC Queues Global */
                                        /* Configuration Register */

REGISTER_COVERED_CNS        GT_U32 mllMcFifoFullDropCnt;    /* MLL MC Fifo Drop Counter */

REGISTER_COVERED_CNS        GT_U32 mllTableBase;            /* Router Multicast Linked List (MLL) Table Base */

REGISTER_COVERED_CNS        GT_U32 mllOutInterfaceCfg[2];   /* MLL Out Interface Counter<n> Configuration Register */

REGISTER_COVERED_CNS        GT_U32 mllOutMcPktCnt[2];       /* MLL Out Multicat Packets Counter<n> */

REGISTER_COVERED_CNS        GT_U32 dft1Reg;                 /* DFT1 Register */

REGISTER_COVERED_CNS        GT_U32 mllMetalFix;             /* MLL Metal Fix */

    }ipMtRegs;             /* IP Multi Traffic Registers (Cheetah 2) */

    struct _l2Mll
    {
        GT_U32 vplsDropCounter;             /* L2 MLL VPLS Drop Counter */

    }l2MllRegs; /* L2 MLL Registers */

    struct _PLR /* Policer */
    {
        GT_U32      policerControlReg;          /* PLR Control */
        GT_U32      policerPortMeteringEnReg[2];/* PLR Port Metering Enable */
        GT_U32      policerMRUReg;              /* PLR MRU */
        GT_U32      policerErrorReg;            /* PLR Error */
        GT_U32      policerErrorCntrReg;        /* PLR Error Counter */

        GT_U32      policerTblAccessControlReg; /* PLR Table Access Control */
        GT_U32      policerTblAccessDataReg;    /* PLR Table Access Data */
        GT_U32      policerRefreshScanRateReg;  /* PLR Metering refresh
                                                   Scan Rate Limit */
        GT_U32      policerRefreshScanRangeReg; /* PLR Metering refresh
                                                   Scan Address Range */
        GT_U32      policerInitialDPReg;        /* PLR Initial DP - there   */
                                                /* are 8 registers for DxCh3 */
        GT_U32      policerIntCauseReg;         /* PLR Interrupt Cause */
        GT_U32      policerIntMaskReg;          /* PLR Interrupt Mask */
        GT_U32      policerShadowReg;           /* PLR Shadow */

        /* Xcat and above registers */
        GT_U32      policerControl1Reg;         /* PLR Control 1 */
        GT_U32      policerControl2Reg;         /* PLR Control 2 */
        GT_U32      portMeteringPtrIndexReg;    /* PLR Port metering pointer index */
        GT_U32      ipfixControl;               /* PLR IPFIX Control */
        GT_U32      ipfixNanoTimerStampUpload;  /* PLR IPFIX nano timer stamp upload */
        GT_U32      ipfixSecLsbTimerStampUpload;/* PLR IPFIX Seconds LSb timer stamp upload */
        GT_U32      ipfixSecMsbTimerStampUpload;/* PLR IPFIX Seconds MSb timer stamp upload */
        GT_U32      ipfixDroppedWaThreshold;    /* PLR IPFIX dropped packet count WA threshold */
        GT_U32      ipfixPacketWaThreshold;     /* PLR IPFIX packet count WA threshold */
        GT_U32      ipfixByteWaThresholdLsb;    /* PLR IPFIX byte count WA threshold LSB */
        GT_U32      ipfixByteWaThresholdMsb;    /* PLR IPFIX byte count WA threshold MSB */
        GT_U32      ipfixSampleEntriesLog0;     /* PLR IPFIX Sample Entries Log0 */
        GT_U32      ipfixSampleEntriesLog1;     /* PLR IPFIX Sample Entries Log1 */
        GT_U32      metalFix;                   /* PLR metal fix */

        /*Policer Tables */

        GT_U32      policerBookmark;            /* exclude Meter/Counting tables from registers dump because IPLR may have one stage only */
        GT_U32      policerType;                /* exclude Meter/Counting tables from registers dump because IPLR may have one stage only */
        GT_U32      policerSize;                /* exclude Meter/Counting tables from registers dump because IPLR may have one stage only */
        GT_U32      policerMeteringCountingTbl; /* PLR Metering and Counting */
        GT_U32      policerCountingTbl;         /* PLR Counting - Xcat and above */
        GT_U32      policerQosRemarkingTbl;     /* PLR Relative Qos Remarking */
        GT_U32      policerManagementCntrsTbl;  /* PLR Management Counters */

        /* Xcat and above Tables*/
        GT_U32      policerTimerTbl;            /* PLR Timers   */
        GT_U32      ipfixWrapAroundAlertTbl;    /* PLR IPFIX wrap around alert */
        GT_U32      ipfixAgingAlertTbl;         /* PLR IPFIX aging alert */

    }PLR[3]; /* Policer */

    struct _sFlowRegs
    {
REGISTER_COVERED_CNS        GT_U32  sFlowControl;           /* sflow control               */

        /* Cheetah ingress sampling to cpu registers */
REGISTER_COVERED_CNS        GT_U32  ingStcTblAccessCtrlReg;
REGISTER_COVERED_CNS        GT_U32  ingStcTblWord0ValueReg;
REGISTER_COVERED_CNS        GT_U32  ingStcTblWord1ValueReg; /* read only */
REGISTER_COVERED_CNS        GT_U32  ingStcTblWord2ValueReg;
        /* GT_U32  ingStcIntCauseReg; */

        /* Cheetah egress STC table registers (three words per port) */

        GT_U32  swEgrStcTblW0Bookmark;
        GT_U32  swEgrStcTblW0Type;
        GT_U32  swEgrStcTblW0Size;
        GT_U32  *egrStcTblWord0 GT_PACKED;
        GT_U32  swEgrStcTblW1Bookmark;
        GT_U32  swEgrStcTblW1Type;
        GT_U32  swEgrStcTblW1Size;
        GT_U32  *egrStcTblWord1 GT_PACKED;
        GT_U32  swEgrStcTblW2Bookmark;
        GT_U32  swEgrStcTblW2Type;
        GT_U32  swEgrStcTblW2Size;
        GT_U32  *egrStcTblWord2 GT_PACKED;

    }sFlowRegs;


    struct _addrSpace
    {
        GT_U32  buffMemWriteControl;    /* BufMem write control        */
        GT_U32  buffMemBank0Write;      /* bank0 write                 */
        GT_U32  buffMemBank1Write;      /* bank1 write                 */
        GT_U32  buffMemClearType;       /* Buffer Memory Clear Type Register */
    }addrSpace;

    struct _cncRegs
    {
        /* global configuration           */
        GT_U32  globalCfg;
        GT_U32  blockUploadTriggers;
        GT_U32  clearByReadValueWord0;
        GT_U32  clearByReadValueWord1;
        /* per couner block configuration */
        GT_U32  blockClientCfgAddrBase;
        GT_U32  blockWraparoundStatusAddrBase;
        /* counters in blocks */
        GT_U32  blockCountersAddrBase;
    }cncRegs;

    struct _ttiRegs
    {
        GT_U32 pclIdConfig0;
        GT_U32 pclIdConfig1;
        GT_U32 ttiIpv4GreEthertype;
        GT_U32 ipMinimumOffset;
        GT_U32 specialEthertypes;
        GT_U32 udeEthertypesBase; /* 64 bits */
        GT_U32 mplsEthertypes; /* The Ethertype used to identify MPLS
                                   Unicast/Multicast packets */
        GT_U32 trunkHashCfg[3];
        GT_U32 ccfcEthertype;
        GT_U32 globalUnitConfig;
        GT_U32 globalUnitConfigForCcPorts; /* TTI Unit Global Configuration for Centralized Chassis Ports*/
        GT_U32 ptpEthertypes;   /* The Ethertype used to identify PTP packets */
        GT_U32 ptpUdpDstPorts;  /* PTP over UDP Destination Ports */
        GT_U32 ptpPacketCmdCfg0Reg; /* PTP Packet Command Configuration 0 */
        GT_U32 ptpPacketCmdCfg1Reg; /* PTP Packet Command Configuration 1 */
        GT_U32 ptpTimeStampPortEnReg[LOCAL_PORTS_BMP_NUM_CNS]; /* Ingress Timestamping port enable */
        GT_U32 ptpGlobalConfig; /* Ingress PTP Global Register */
        GT_U32 ptpEnableTimeStamp; /* Enable Time Stamping Register */
        GT_U32 ptpGlobalFifoCurEntry[2]; /* Global FIFO Current Entry */
        GT_U32 ptpTodCntrNanoSeconds; /* Ingress TOD Counter - Nanoseconds */
        GT_U32 ptpTodCntrSeconds[2]; /* Ingress TOD Counter - Seconds */
        GT_U32 ptpTodCntrShadowNanoSeconds; /* Ingress TOD Counter Shadow - Nanoseconds */
        GT_U32 ptpTodCntrShadowSeconds[2]; /* Ingress TOD Counter Shadow - Seconds */
        GT_U32 ttiMiscConfig; /* TTI Misc Configuration */
        GT_U32 ttiMetalFix;
        GT_U32 trillMacToMeSrcInterfaceInfo[8]; /* MAC2ME Entry Fields */
        struct{
            GT_U32  cfiUpToQoSProfileMappingTableSelector[LOCAL_PORTS_NUM_CNS / 4];/* 4 ports per register */
        }qos;

        GT_U32  coreRingPortEnable[2];
        GT_U32 timestampTagConfig; /* Timestamp tag configuration */

        GT_U32 srcPortBpeEnable; /* source port BPE enable */
        GT_U32  ttIngressTpidSelect[32]; /* TTI Ingress Tag Protocol ID Select registers */
        GT_U32 ttiPwConfig; /* TTI Pseudo Wire Configurations */

    }ttiRegs;

    struct {/* EQ - pre egress */
        struct {
REGISTER_COVERED_CNS            GT_U32  multiCoreLookup0;
REGISTER_COVERED_CNS            GT_U32  multiCoreLookupInfo[8];/* support 16 src port groups */
        }multiCoreLookup;
        struct {
REGISTER_COVERED_CNS            GT_U32  dualDevIdAndLossyDropConfig;
        }globalConfig;

    }eq;/* EQ - pre egress */

    struct _haRegs
    {

REGISTER_COVERED_CNS        GT_U32 hdrAltMplsEthertypes; /* The MPLS Ethertype to be used when MPLS
                                       headers need to be added or recognized*/

REGISTER_COVERED_CNS        GT_U32 hdrAltIEthertype; /* The I-Ethertype to be used when Mac
                                    In Mac headers need to be added */

REGISTER_COVERED_CNS        GT_U32 hdrAltGlobalConfig;
REGISTER_COVERED_CNS        GT_U32 hdrAltCnConfig;
REGISTER_COVERED_CNS        GT_U32 hdrAltCpidReg0;
REGISTER_COVERED_CNS        GT_U32 hdrAltCpidReg1;
REGISTER_COVERED_CNS        GT_U32 hdrAltCnTagFlowId;
REGISTER_COVERED_CNS        GT_U32 hdrAltCnmHeaderConfig;

        /* local ports bmp */
REGISTER_COVERED_CNS        GT_U32 cscdHeadrInsrtConf[LOCAL_PORTS_BMP_NUM_CNS];/*ports bmp */     /* Cascading and Header Insertion Configuration */
        /* local ports bmp */
REGISTER_COVERED_CNS        GT_U32 eggrDSATagTypeConf[LOCAL_PORTS_BMP_NUM_CNS];/*ports bmp */     /* Egress DSA Tag Type Configuration */
        /* local ports bmp */
REGISTER_COVERED_CNS        GT_U32 invalidCrcModeConfigReg[LOCAL_PORTS_BMP_NUM_CNS];/*ports bmp *//* Invalid CRC Mode Configuration Register */

REGISTER_COVERED_CNS        GT_U32 trunkNumConfigReg[PORTS_NUM_CNS / 4];/* 4 ports in register */
                                        /* 64 - for Lion2 (for 256 ports)
                                           16 - for lion A,B    (for 64 ports)
                                           7  - for xcat/ch1,2,3  (for 28 ports)
                                           4 bits per port
                                           addresses of trunk numbers of
                                           the ports (4 ports in reg) */
REGISTER_COVERED_CNS        GT_U32 routerMacSaBaseReg[2]; /* router Mac SA Base Regs 0&1            */

                /* local ports bmp */
REGISTER_COVERED_CNS        GT_U32 routerHdrAltEnMacSaModifyReg[LOCAL_PORTS_BMP_NUM_CNS];/*ports bmp */
                                            /* router Header Alteration enable*/
                                            /* Mac SA modification           */
        /* local ports bmp */
REGISTER_COVERED_CNS        GT_U32 routerDevIdModifyEnReg[LOCAL_PORTS_BMP_NUM_CNS];/*ports bmp */
                                            /* Device ID Modification Enable */

        /* local ports bmp */
REGISTER_COVERED_CNS        GT_U32 routerHdrAltMacSaModifyMode[PORTS_BMP_NUM_CNS*2];
                                                /* 2 - for xcat/ch1,2,3 */
                                                /* 4 - for lion B */
                                                /* 2 bits per port */
                                                /* router Header Alteration MAC SA */
                                                /* Modification Mode               */

        /* local ports bmp */
REGISTER_COVERED_CNS        GT_U32 haVlanTransEnReg[LOCAL_PORTS_BMP_NUM_CNS];/*ports bmp *//* Vlan Translation Enable Register */

REGISTER_COVERED_CNS        GT_U32 qosProfile2ExpTable;        /* QoSProfile to EXP Table */
REGISTER_COVERED_CNS        GT_U32 qosProfile2DpTable;         /* QoSProfile to DP Table */

        /* local ports bmp Lion A */
        /* !!! global ports bmp for Lion B and above  !!! */
REGISTER_COVERED_CNS        GT_U32 toAnalyzerVlanAddConfig[LOCAL_PORTS_BMP_NUM_CNS];/*ports bmp */ /* TO ANALYZER VLAN Adding Configuration Registers */
REGISTER_COVERED_CNS        GT_U32 ingAnalyzerVlanTagConfig;/* Ingress Analyzer VLAN Tag Configuration Register */
REGISTER_COVERED_CNS        GT_U32 egrAnalyzerVlanTagConfig;/* Egress Analyzer VLAN Tag Configuration Register */

REGISTER_COVERED_CNS        GT_U32  bridgeEgressVlanEtherTypeTable[TPID_NUM_CNS/2];/*2 TPIDs in register*/  /* Egress Vlan Ethertype table configuration registers */
REGISTER_COVERED_CNS        GT_U32  bridgeEgressPortTag0TpidSelect[LOCAL_PORTS_NUM_CNS/8];/*8 ports in register*/  /* Egress port Tag0 TPID select register */
REGISTER_COVERED_CNS        GT_U32  bridgeEgressPortTag1TpidSelect[LOCAL_PORTS_NUM_CNS/8];/*8 ports in register*/  /* Egress port Tag1 TPID select register */
REGISTER_COVERED_CNS        GT_U32  miscConfig; /* Misc Configuration Register */
REGISTER_COVERED_CNS        GT_U32  mirrorToAnalyzerHeaderConfReg[LOCAL_PORTS_BMP_NUM_CNS];/*ports bmp */ /* Mirror to Analyzer Header Configuration Register */
REGISTER_COVERED_CNS        GT_U32  ptpTimeStampPortEnReg[LOCAL_PORTS_BMP_NUM_CNS]; /* Egress Timestamping port enable */
REGISTER_COVERED_CNS        GT_U32  ptpGlobalConfig; /* Egress PTP Global Register */
REGISTER_COVERED_CNS        GT_U32  ptpEnableTimeStamp; /* Enable Time Stamping Register */
REGISTER_COVERED_CNS        GT_U32  ptpGlobalFifoCurEntry[2]; /* Global FIFO Current Entry */
REGISTER_COVERED_CNS        GT_U32  ptpTodCntrNanoSeconds; /* Egress TOD Counter - Nanoseconds */
REGISTER_COVERED_CNS        GT_U32  ptpTodCntrSeconds[2]; /* Egress TOD Counter - Seconds */
REGISTER_COVERED_CNS        GT_U32  ptpTodCntrShadowNanoSeconds; /* Egress TOD Counter Shadow - Nanoseconds */
REGISTER_COVERED_CNS        GT_U32  ptpTodCntrShadowSeconds[2]; /* Egress TOD Counter Shadow - Seconds */

REGISTER_COVERED_CNS        GT_U32  keepVlan1Reg[LOCAL_PORTS_NUM_CNS/4]; /* 4 ports in register */ /* Keep Vlan1 Registers */

REGISTER_COVERED_CNS        GT_U32  coreRingConfiguration[2];
REGISTER_COVERED_CNS        GT_U32  erspanEntryWord1[8];
                            GT_U32  perPortBpeEnable; /* per port BPE enable */
                            GT_U32  mplsControlWord[7]; /* MPLS control word register */
                            GT_U32  tunnelStartEgressVlanEtherTypeTable[TPID_NUM_CNS/2];/*2 TPIDs in register*/  /* Tunnel start Egress Vlan Ethertype table configuration registers */
                            GT_U32  tunnelStartEgressPortTagTpidSelect[PORTS_NUM_CNS/8];/*8 ports in register*/  /* Tunnel Start Egress port Tag TPID select register */
                            GT_U32  passengerEgressVlanEtherTypeTable[TPID_NUM_CNS/2]; /*2 TPIDs in register*/  /* passenger Egress Vlan Ethertype table configuration registers */
                            GT_U32  passengerEgressPortTagTpidSourceSelect; /* Passenger TPID Index Source Register */
    }haRegs;

    struct _bcnRegs
    {

        GT_U32 bcnControlReg;
        GT_U32 bcnPauseTriggerEnableReg;
        GT_U32 bcnGlobalPauseThresholdsReg;
        GT_U32 portsBcnProfileReg0;
        GT_U32 portsBcnAwarenessTbl;
        GT_U32 portsSpeedIndexesReg0;

    }bcnRegs;

    GT_U32  serdesConfigRegsBookmark;
    GT_U32  serdesConfigRegsType;
    GT_U32  serdesConfigRegsSize;
    struct _lpSerdesConfig *serdesConfig GT_PACKED; /* array of per SERDES regs */

    struct _cutThroughRegs /* For lion and above. */
    {
        GT_U32 ctEnablePerPortReg;
        GT_U32 ctEthertypeReg;
        GT_U32 ctUpEnableReg;
        GT_U32 ctPacketIndentificationReg;
        GT_U32 ctCascadingPortReg;
        GT_U32 ctPortMemoryRateLimitThresholdReg[6];
        GT_U32 ctCpuPortMemoryRateLimitThresholdReg;
        GT_U32 ctGlobalConfigurationReg;
    }cutThroughRegs;

    /* TXQ ver 1 --  see use of macro
              PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(...) */
    struct/*txqVer1*/
    {
        struct{/*egr*/
            struct{/*global*/
                GT_U32  cpuPortDistribution;
                GT_U32  egressInterruptMask;
            }global;

            struct{/*mcFifoConfig*/
                GT_U32  globalConfig;
                GT_U32  arbiterWeights;
                GT_U32  distributionMask[MC_FIFO_NUM_CNS][PORTS_BMP_NUM_CNS];/* ports bmp */
            }mcFifoConfig;

            struct{/*devMapTableConfig*/
                GT_U32  addrConstruct;
                GT_U32  localSrcPortMapOwnDevEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  localTrgPortMapOwnDevEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  secondaryTargetPortTable[PORTS_NUM_CNS];/*port*/
            }devMapTableConfig;

            struct{/*filterConfig*/
                GT_U32  globalEnables;
                GT_U32  unknownUcFilterEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  unregisteredMcFilterEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  unregisteredBcFilterEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  mcLocalEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  ipmcRoutedFilterEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  ucSrcIdFilterEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  oamLoopbackFilterEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  egrCnEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  egrCnFcEn[PORTS_BMP_NUM_CNS];/* ports bmp */

            }filterConfig;

        }egr;

        struct{/*queue*/
            struct{/*distributor*/
                GT_U32  generalConfig;
                GT_U32  tcDpRemapping[2];/*DP*/
                GT_U32  stackTcRemapping[TC_NUM_CNS];/*TC*/
                GT_U32  stackRemapEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  controlTcRemapEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  dataTcRemapEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  dpToCfiRemapEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  fromCpuForwardRestricted[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  bridgedForwardRestricted[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  routedForwardRestricted[PORTS_BMP_NUM_CNS];/* ports bmp */
            }distributor;

            struct{/*peripheralAccess*/
                struct{/*misc*/
                    GT_U32  misc;
                }misc;

                struct{/*egressMibCounterSet*/
                    GT_U32  config[EGRESS_COUNTERS_SETS_NUM_CNS];/* counters Sets*/
                    GT_U32  outgoingUnicastPacketCounter[EGRESS_COUNTERS_SETS_NUM_CNS];/* counters Sets*/
                    GT_U32  outgoingMulticastPacketCounter[EGRESS_COUNTERS_SETS_NUM_CNS];/* counters Sets*/
                    GT_U32  outgoingBroadcastPacketCounter[EGRESS_COUNTERS_SETS_NUM_CNS];/* counters Sets*/
                    GT_U32  bridgeEgressFilteredPacketCounter[EGRESS_COUNTERS_SETS_NUM_CNS];/* counters Sets*/
                    GT_U32  tailDroppedPacketCounter[EGRESS_COUNTERS_SETS_NUM_CNS];/* counters Sets*/
                    GT_U32  controlPacketCounter[EGRESS_COUNTERS_SETS_NUM_CNS];/* counters Sets*/
                    GT_U32  egressForwardingRestrictionDroppedPacketsCounter[EGRESS_COUNTERS_SETS_NUM_CNS];/* counters Sets*/
                    GT_U32  multicastFifoDroppedPacketsCounter[EGRESS_COUNTERS_SETS_NUM_CNS];/* counters Sets*/
                }egressMibCounterSet;

                struct{/*cncModes*/
                    GT_U32  modesRegister;
                }cncModes;

            }peripheralAccess;

            struct{/*tailDrop*/
                struct{/*config*/
                    GT_U32  config;
                    GT_U32  byteCount;
                    GT_U32  portEnqueueEnable[PORTS_NUM_CNS];/*port*/
                    GT_U32  tailDropCnProfile[PORTS_NUM_CNS];/*port*/
                    GT_U32  tcProfileEnableSharedPoolUsage[TC_NUM_CNS];/*TC*/
                    GT_U32  profileEnableWrtdDp[4];/*DP?*/
                    GT_U32  profilePriorityQueueToSharedPoolAssociation[PROFILE_NUM_CNS];/*profile*/
                    GT_U32  wrtdMask0;
                    GT_U32  wrtdMask1;
                }config;

                struct{/*limits*/
                    GT_U32  globalDescriptorsLimit;
                    GT_U32  globalBufferLimit;
                    GT_U32  profilePortLimits[PROFILE_NUM_CNS];/*profile*/
                    GT_U32  sharedPoolLimits[TC_NUM_CNS];/*TC*/

                    GT_U32  maximumQueueLimits[TC_NUM_CNS][PROFILE_NUM_CNS];/*TC*//*profile*/
                    GT_U32  eqQueueLimitsDp0[TC_NUM_CNS][PROFILE_NUM_CNS];/*TC*//*profile*/
                    GT_U32  dqQueueBufLimits[TC_NUM_CNS][PROFILE_NUM_CNS];/*TC*//*profile*/
                    GT_U32  dqQueueDescLimits[TC_NUM_CNS][PROFILE_NUM_CNS];/*TC*//*profile*/
                    GT_U32  eqQueueLimitsDp1[TC_NUM_CNS][PROFILE_NUM_CNS];/*TC*//*profile*/
                    GT_U32  eqQueueLimitsDp2[TC_NUM_CNS][PROFILE_NUM_CNS];/*TC*//*profile*/

                }limits;

                struct{/*counters*/
                    GT_U32  totalDescCounter;
                    GT_U32  totalBuffersCounter;
                    GT_U32  portDescCounter[PORTS_NUM_CNS];/*port*/
                    GT_U32  portBuffersCounter[PORTS_NUM_CNS];/*port*/
                    GT_U32  tcSharedDescCounter[TC_NUM_CNS];/*TC*/
                    GT_U32  tcSharedBuffersCounter[TC_NUM_CNS];/*TC*/

                    GT_U32  qMainDesc[PORTS_NUM_CNS][TC_NUM_CNS];/*port*//*TC*/
                    GT_U32  qMainBuff[PORTS_NUM_CNS][TC_NUM_CNS];/*port*//*TC*/
                }counters;

                struct{/*mcFilterLimits*/
                    GT_U32  mirroredPacketsToAnalyzerPortDescriptorsLimit;
                    GT_U32  mirroredPacketsToAnalyzerPortBuffersLimit;
                    GT_U32  multicastDescriptorsLimit;
                    GT_U32  multicastBuffersLimit;
                }mcFilterLimits;

                struct{/*mcFilterCounters*/
                    GT_U32  snifferDescriptorsCounter;
                    GT_U32  snifferBuffersCounter;
                    GT_U32  multicastDescriptorsCounter;
                    GT_U32  multicastBuffersCounter;
                }mcFilterCounters;

                struct{/*resourceHistogram*/
                    GT_U32  limitRegister1;
                    GT_U32  limitRegister2;

                    GT_U32  counters[4];/*counter*/
                  }resourceHistogram;

            }tailDrop;

            struct{/*congestNotification*/
                GT_U32 cnGlobalConfigReg;
                GT_U32 cnPrioQueueEnProfile;
                GT_U32 cnEnCnFrameTx[PORTS_BMP_NUM_CNS]; /* ports bmp */
                GT_U32 cnFcEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32 fbCalcConfigReg;
                GT_U32 fbMinReg;
                GT_U32 fbMaxReg;
                GT_U32 cnProfileThreshold[TC_NUM_CNS][PROFILE_NUM_CNS];/*TC*//*profile*/
                GT_U32 cnSampleTbl;
                GT_U32 cnBufferFifoParityErrorsCnt;
            }congestNotification;

            struct{/*pfcRegs*/
                GT_U32 pfcGlobalConfigReg;
                GT_U32 pfcSrcPortFcMode;
                GT_U32 pfcSourcePortProfile;
                GT_U32 pfcGlobaGroupOfPortsThresholds;
                GT_U32 pfcProfileXoffThresholds;
                GT_U32 pfcProfileXonThresholds;
            }pfcRegs;

            struct{/*llfcRegs*/
                GT_U32 pfcGlobalConfigReg;
                GT_U32 pfcProfileXoffThresholds;
                GT_U32 pfcGlobalGroupOfPortsThresholds;
                GT_U32 pfcPipeCountersParityErrorsCounter[8];
            }llfcRegs;

            struct{/*ucMcArb*/
                GT_U32 mcShaperControl;
            }ucMcArb;
        }queue;

        struct{/*ll*/
            struct{/* link list */
                struct{ /* Link List ECC Control */
                    GT_U32  fbufRamEccStatus;
                    GT_U32  fbufRamEccErrorCounter;
                }linkListEccControl;
            }linkList;
        }ll;

        struct{/*dq*/
            struct{/*global*/
                struct{/*config*/
                    GT_U32  config;
                }config;

                struct{/*flushTrig*/
                    GT_U32  portTxqFlushTrigger[PORTS_NUM_CNS];/* local port emulated as global port !!! */
                }flushTrig;

                struct{/*debug*/
                    GT_U32  metalFix;
                }debug;

                struct{/*memory parity error*/
                    GT_U32  tokenBucketPriorityParityErrorCounter;
                    GT_U32  stateVariablesParityErrorCounter;
                    GT_U32  parityErrorBadAddress;
                }memoryParityError;
            }global;
            struct{/*scheduler*/
                struct{/*config*/
                    GT_U32  config;
                    GT_U32  portRequestMask;
                    GT_U32  schedulerByteCountForTailDrop;
                    GT_U32  portRequestMaskSelector[PORTS_NUM_CNS];/* local port emulated as global port !!! */
                }config;

                struct{/*priorityArbiterWeights*/
                    GT_U32  profileSdwrrWeightsConfig0[PROFILE_NUM_CNS];/*profile*/
                    GT_U32  profileSdwrrWeightsConfig1[PROFILE_NUM_CNS];/*profile*/
                    GT_U32  profileSdwrrGroup[PROFILE_NUM_CNS];/*profile*/
                    GT_U32  profileSdwrrEnable[PROFILE_NUM_CNS];/*profile*/
                    GT_U32  portCpuSchedulerProfile;
                    GT_U32  portSchedulerProfile[PORTS_NUM_CNS];/* local port emulated as global port !!! */
                }priorityArbiterWeights;

                struct{/*portArbiter*/
                    GT_U32  pizzaArbiterConfig;
                    GT_U32  pizzaArbiterMap[42];
                }portArbiter;
            }scheduler;

            struct{/*shaper*/
                GT_U32  tokenBucketUpdateRate;
                GT_U32  tokenBucketBaseLine;
                GT_U32  cpuTokenBucketMtuConfig;
                GT_U32  portsTokenBucketMtuConfig;
                GT_U32  tokenBucketMode;
                GT_U32  portDequeueEnable[PORTS_NUM_CNS];/* local port emulated as global port !!! */
            }shaper;

            struct{/*flowControl*/
                GT_U32  flowControlConfig;
                GT_U32  schedulerProfileLlfcMtu[PROFILE_NUM_CNS];/*profile*/
                GT_U32  pfcTimerToPriorityMapping[TC_NUM_CNS];/*TC*/
                GT_U32  schedulerProfileTcToPriorityMapEnable[PROFILE_NUM_CNS];/*profile*/
                GT_U32  tcProfilePfcFactor[TC_NUM_CNS][PROFILE_NUM_CNS];/*TC*//*profile*/
            }flowControl;

            struct{/*statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort*/
                GT_U32  stcStatisticalTxSniffConfig;
                GT_U32  egressCtmTrigger;
                GT_U32  egressAnalyzerEnable;
            }statisticalAndCpuTriggeredEgressMirroringToAnalyzerPort;

        }dq;

        struct{/*sht*/
            struct{/*egressTables*/
                GT_U32  deviceMapTableBaseAddress;
            }egressTables;
            struct{/*global*/
                GT_U32  shtGlobalConfig;
                GT_U32  vlanParityErrorLastEntries;
                GT_U32  egrVlanParityErrorCounter;
                GT_U32  ingVlanParityErrorCounter;
                GT_U32  portIsolationParityErrorLastEntries;
                GT_U32  vidxParityErrorLastEntry;
                GT_U32  l2PortIsolationParityErrorCounter;
                GT_U32  l3PortIsolationParityErrorCounter;
                GT_U32  vidxParityErrorCounter;

                /* Lion2 support */
                GT_U32     portIsolationLookupConfiguration;
            }global;
            struct{
                GT_U32  dfx0; /*DFX Register 0*/
                GT_U32  dfx1; /*DFX Register 1*/
                GT_U32  dfx2; /*DFX Register 2*/
                GT_U32  dfx3; /*DFX Register 3*/
                GT_U32  dfxStatus0; /*DFX Status Register 0 */
            }dfx;
        }sht;

        struct{/*dist*/
            struct{/* Device Map Table */
                GT_U32  distEntryDevMapTableBaseAddress;
                GT_U32  addrConstruct;
                GT_U32  localSrcPortMapOwnDevEn[PORTS_BMP_NUM_CNS];/* ports bmp */
                GT_U32  localTrgPortMapOwnDevEn[PORTS_BMP_NUM_CNS];/* ports bmp */
            }deviceMapTable;
            struct{ /* Global*/
                GT_U32     distGlobalControlReg;   /* Dist Global Control */
            }global;
        }dist;

    }txqVer1;

    /* For xCat2 and above */
    struct{/* dfxUnits*/
        struct{/* server */
            GT_U32  pipeSelect;
            GT_U32  temperatureSensorStatus;    /* Temperature Sensor Status */
            GT_U32  resetControl;
            GT_U32  serverSkipInitMatrix;
            GT_U32  genSkipInitMatrix0;
            GT_U32  genSkipInitMatrix1;
            GT_U32  ledPadControl;
            GT_U32  gppPadControl;
            GT_U32  rgmiiPadControl;
            GT_U32  ssmiiPadControl;
            GT_U32  initDoneStatus;
            GT_U32  temperatureSensorControlMsb; /* Temperature Sensor Control MSB*/
            GT_U32  functionalSarStatusLsb;/*Functional SAR Status LSB*/
        }server;

        struct{/* client */
            GT_U32  dataCtrl;
            GT_U32  dataSamplingCtrl;
            GT_U32  debugInfo;
        }client[2];

        struct{/* ingressLower */
            struct{/* tcBist */
                GT_U32  tcamBistConfigStatus;
            }tcBist;
        }ingressLower;
        struct{/* ingressUpper */
            struct{/* tcBist */
                GT_U32  tcamBistConfigStatus;
            }tcBist;
        }ingressUpper;

    }dfxUnits;

     /* For xCat2 */
    struct{
        GT_U32 wrDmaToTti;
        GT_U32 ttiToPcl;
        GT_U32 pclToBridge;
        GT_U32 bridgeToRouterSet0;
        GT_U32 bridgeToRouterSet1;
REGISTER_COVERED_CNS        GT_U32 preEgressToTxq0;
REGISTER_COVERED_CNS        GT_U32 preEgressToTxq1;
        GT_U32 ingressPolicer0ToIngressPolicer1Set0;
        GT_U32 ingressPolicer0ToIngressPolicer1Set1;
        GT_U32 ingressPolicer1ToPreEgressSet0;
        GT_U32 ingressPolicer1ToPreEgressSet1;
        GT_U32 egressPolicerEgressToPclSet0;
        GT_U32 egressPolicerEgressToPclSet1;
        GT_U32 txqToHeaderAlteration;
        GT_U32 routerToIngressPolicer; /*  xcat2 do not support it */
    }descriptorsConfig;

    struct PRV_CPSS_DXCH_MAC_PACKET_GEN_CONFIG_STC
    {
        GT_U32 macDa[3];
        GT_U32 macSa[3];
        GT_U32 etherType;
        GT_U32 vlanTag;
        GT_U32 dataPattern[4];
        GT_U32 packetLength;
        GT_U32 packetCount;
        GT_U32 ifg;
        GT_U32 controlReg0;
    }packGenConfig[4]; /* four packet generators per port group */

    struct {/* BMA */
        struct {
            GT_U32  mcCntParityErrorCounter;
        }bmaCore[4];

    }bma;/* BMA */

}PRV_CPSS_DXCH_PP_REGS_ADDR_STC;

/* return regAddr depending on is this CPU port or physical port
 */
#define PRV_CPSS_DEV_BRG_PORT_CONFIG_0_REGADDR_PTR(devNum,portNum)   \
    (((portNum) == CPSS_CPU_PORT_NUM_CNS) ? \
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.cpuPortControl : \
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[portNum])

/* return regAddr depending on is this CPU port or physical port
 * for 'Bridge Configuration1'
 */
#define PRV_CPSS_DEV_BRG_PORT_CONFIG_1_REGADDR_PTR(devNum,portNum)   \
    (((portNum) == CPSS_CPU_PORT_NUM_CNS) ? \
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.cpuPortControl1 : \
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl1[portNum])

/* restore structure packing to default */
#if __WORDSIZE == 64
#pragma pack()
#endif

/* allow those macros only inside this H file*/
#undef PORTS_NUM_CNS
#undef PORTS_BMP_NUM_CNS



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChRegsh */

