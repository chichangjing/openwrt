/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChInfoEnhanced.h
*
* DESCRIPTION:
*       This file includes the declaration of the structure to hold the
*       enhanced parameters that "fine tuning" the PP's capabilities in CPSS
*       point of view.
*
* FILE REVISION NUMBER:
*       $Revision: 50 $
*
*******************************************************************************/
#ifndef __prvCpssDxChInfoEnhancedh
#define __prvCpssDxChInfoEnhancedh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
/*
 * Typedef: struct PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC
 *
 * Description: A Structure including PP's tables sizes and parameters that
 *              define the "fine tuning" , of the specific device.
 * Notes:
 *         the initialization of those fields should be done during "phase 1".
 *
 * Fields:
 *      enhancedInfoPtr - (pointer to) the other enhanced features of the PP.
 *
 *      tableSize - structure that hold the size of tables that the PP hold.
 *                  the values refer to number of entries in the tables.
 *          fdb - the number of entries in bridge FDB table.
 *          vidxNum - number of VIDX (multicast groups).
 *          stgNum - number of MSTP groups . STG is spanning tree groups.
 *          router - the number of rows for router entries in router TCAM table (number of IPv6 entries)
 *          tunnelTerm - the number of tunnel termination (TTI) entries in router TCAM table or device TCAM.
 *          tunnelStart - the number of rows in router ARP and tunnel start table.(number of TS entries)
 *          routerArp - the number of ARP entries in router ARP and tunnel start table.
 *          routerNextHop - the number of entries in the router next hop table
 *          ecmpQos - the number of entries in the ECMP/QoS table
 *                    (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *          mllPairs - the number of mll pairs (multicast linked list)
 *          policyTcamRaws -  actual amount of TCAM rows
 *                            The DxCh3 TCAM has 4 banks multiply to policyTcamRaws of 24 Byte entries.
 *                            The DxCh and DxCh2 TCAM has 2 banks multiply to policyTcamRaws of 24 Byte entries
 *          cncBlocks      -  actual amount of CNC blocks.
 *                           The PPs address space represents the 12 blocks
 *                           but not all blocks present
 *          cncBlockNumEntries - number of entries in each CNC block
 *          cncUploadUseDfxUnit - indication that the CNC upload operation done on DFX unit and not on MG.
 *                                 (Bobcat3 uses the DFX)
 *          policersNum - number of policers (shared for ingress and egress)
 *                        Note: For xCat and above, this is the number of ingress policers
 *                        for both ingress stages.
 *          egressPolicersNum - Number of egress policers.
 *                              Note: Relevant for xCat and above.
 *          trunksNum - number of trunks.
 *          transmitDescr  - Transmit Descriptors number. Supported for DxCh3 and above.
 *          bufferMemory   - Buffer Memory size. Supported for DxCh3 and above.
 *          txQueuesNum    -  Transmit Queues Number
 *          lpmRam - Average number of IPv4 prefixes in the LPM RAM if assuming all RAM is used for IPv4 with no PBR.
 *                   But in reality this memory includes both LPM and PBR (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *          pipeBWCapacityInGbps - device' pipe BW capacity
 *          bpePointToPointEChannels - 802.1br BPE Point to Point E-Channels
 *                                     (APPLICABLE DEVICES: xCat3)
 *
 *      featureInfo - structure that hold the features that the PP may support/not support.
 *          tr101Supported - TR101 feature support. Supported for xCat and above.
 *                              GT_TRUE - supported
 *                              GT_FALSE - not supported
 *          vlanTranslationSupported - VLAN translation feature support. Supported for DxCh3 and above.
 *                              GT_TRUE - supported
 *                              GT_FALSE - not supported
 *          iplrSecondStageSupported -
 *                              GT_TRUE - There are two ingress stages.
 *                              GT_FALSE - There is only one ingress stage.
 *                              Note: Relevant for xCat and above.
 *          trunkCrcHashSupported - Enhanced Hash for load balancing (CRC based)
 *                              GT_TRUE - supported
 *                              GT_FALSE - not supported
 *                              Note: Relevant for Lion and above.
 */
typedef struct{
    void*       enhancedInfoPtr;

    struct{
        /* bridge section */
        GT_U32  fdb;
        GT_U32  vidxNum;
        GT_U32  stgNum;

        /* routing and TTI section */
        GT_U32 router;
        GT_U32 tunnelTerm;

        /* tunnel section*/
        GT_U32  tunnelStart;
        GT_U32  routerArp;

        /* ip section */
        GT_U32  routerNextHop;
        GT_U32  ecmpQos;
        GT_U32  mllPairs;

        /* PCL section */
        GT_U32 policyTcamRaws;

        /* CNC section */
        GT_U32 cncBlocks;
        GT_U32 cncBlockNumEntries;

        /* Policer Section */
        GT_U32  policersNum;
        GT_U32  egressPolicersNum;

        /* trunk Section */
        GT_U32  trunksNum;

        /* Transmit Descriptors */
        GT_U32 transmitDescr;

        /* Buffer Memory */
        GT_U32 bufferMemory;

        /*  Transmit Queues */
        GT_U32      txQueuesNum;

        /* LPM RAM */
        GT_U32 lpmRam;

        /* pipe BW capacity for Rx-DMA Tx-DMA TxQ Tx-FIFO Eth-TxFIFO units */
        GT_U32 pipeBWCapacityInGbps;

        /* 802.1br BPE Point to Point E-Channels */
        GT_U32 bpePointToPointEChannels;
        /****************************************************/
        /* add here other table sizes that are "parametric" */
        /****************************************************/
    }tableSize;

    struct
    {
        /* TR101 Feature support */
        GT_BOOL tr101Supported;

        /* VLAN translation support */
        GT_BOOL vlanTranslationSupported;

        /* Policer Ingress second stage support flag */
        GT_BOOL iplrSecondStageSupported;

        /* trunk CRC hash support flag */
        GT_BOOL trunkCrcHashSupported;

        /* TM support flag */
        GT_BOOL TmSupported;
    }featureInfo;

}PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC;

/* check policer Id , when used as 'key' we return GT_BAD_PARAM on error */
/* note : index is in the range: 0..maxNum-1 */
#define PRV_CPSS_DXCH_POLICERS_NUM_CHECK_MAC(_devNum,_stage,_index)              \
    if((_index) >= PRV_CPSS_DXCH_PP_MAC(_devNum)->policer.memSize[(_stage)])     \
    {                                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                     \
    }


/* check vidxId , when used as 'key' we return GT_BAD_PARAM on error */
/* note : index is in the range: 0..maxNum-1 */
#define PRV_CPSS_DXCH_VIDX_NUM_CHECK_MAC(_devNum,_index)                                \
    if(PRV_CPSS_PP_MAC(_devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)              \
    {                                                                                   \
        if((_index) >=                                                                  \
            PRV_CPSS_DXCH_PP_MAC(_devNum)->accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_MULTICAST_E].maxNumOfEntries)\
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                        \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        if((_index) >= PRV_CPSS_DXCH_PP_MAC(_devNum)->fineTuning.tableSize.vidxNum)     \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                        \
    }


/***************************************************
 * typedef: enum PRV_CPSS_DXCH_PORT_TYPE_ENT
 *
 * Description: port type used in mapping functions
 *
 *
 * Enumerations:
 *    PRV_CPSS_DXCH_PORT_TYPE_INVALID_E - invalide source
 *    PRV_CPSS_DXCH_PORT_TYPE_PHYS_E  - physical port
 *    PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E - rxDma port
 *    PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E - txDma port
 *    PRV_CPSS_DXCH_PORT_TYPE_MAC_E   - mac number
 *    PRV_CPSS_DXCH_PORT_TYPE_TXQ_E   - txq port
 *    PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E - Interlaken channel, relevant
 *          only when port mapping is CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E
 *    PRV_CPSS_DXCH_PORT_TYPE_TM_PORT_IDX_E  - traffic manager port idx , relavant if tmEnable == GT_TRUE
 *    PRV_CPSS_DXCH_PORT_TYPE_MAX_E   - max port , used in array declaration
 ****************************************************/
typedef enum
{
     PRV_CPSS_DXCH_PORT_TYPE_INVALID_E = -1,
     PRV_CPSS_DXCH_PORT_TYPE_PHYS_E = 0,
     PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E,
     PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E,
     PRV_CPSS_DXCH_PORT_TYPE_MAC_E,
     PRV_CPSS_DXCH_PORT_TYPE_TXQ_E,
     PRV_CPSS_DXCH_PORT_TYPE_ILKN_CHANNEL_E,
     PRV_CPSS_DXCH_PORT_TYPE_TM_PORT_IDX_E,
     PRV_CPSS_DXCH_PORT_TYPE_MAX_E              /* never cross this boundary */
}PRV_CPSS_DXCH_PORT_TYPE_ENT;

/*******************************************************************************
* PRV_CPSS_DXCH_PP_CONFIG_ENHANCED_INIT_FUNC
*
* DESCRIPTION:
*       do enhanced initialization , on devices that need to.
*       this function sets the fine tuning parameters needed by the CPSS SW.
*       Function may also do HW operations.
*
* APPLICABLE DEVICES:
*       NA
*
* INPUTS:
*       devNum      - The device number
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK         - on success
*       GT_HW_ERROR   - on hardware error.
*       GT_BAD_PARAM  - on bad devNum
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_PP_CONFIG_ENHANCED_INIT_FUNC)
(
    IN      GT_U8                           devNum
);

/* pointer to a function used for DXCH devices that will do enhanced
   initialization of SW and HW parameters --  for FUTURE compatibility */
extern PRV_CPSS_DXCH_PP_CONFIG_ENHANCED_INIT_FUNC prvCpssDxChPpConfigEnhancedInitFuncPtr;

/*
 * Typedef: struct PRV_CPSS_DXCH_PP_HW_INFO_DATA_PATH_INFO_STC
 *
 * Description: A Structure with info about data path (rxdma,txdma,txfifo).
 * Notes:
 *         the initialization of those fields should be done during "phase 1".
 *
 * Fields:
 *      dataPathFirstPort - first port used in the data path.
 *      dataPathNumOfPorts - number of ports in the DP .
 *                              if value is ZERO ... meaning 'not valid' DP .. ignored
 *      localDmaMapArr   - local DMA numbers per relative global numbers.
 *                         May be NULL if map is 1:1.
 *                         Used for Aldrin device.
 *      cpuPortDmaNum - if 0 this DP not support the CPU DMA
 *      tmDmaNum - (egress only) if 0 this DP not support the TM DMA (traffic manager)
 *
 *       NOTE: for bobk (global are also local):
 *       global DMA IDs 0..47   are 'DP 0' local dma 0..47
 *       global DMA IDs 56..71  are 'DP 1' local dma 56..71
 *
 *       NOTE: for bobcat3 (global are not local):
 *       global DMA IDs 0..11   are 'DP 0' local dma 0..11
 *       global DMA IDs 12..23  are 'DP 1' local dma 0..11
 *       global DMA IDs 24..35  are 'DP 2' local dma 0..11
 *       global DMA IDs 36..47  are 'DP 3' local dma 0..11
 *       global DMA IDs 48..59  are 'DP 4' local dma 0..11
 *       global DMA IDs 60..71  are 'DP 5' local dma 0..11
 *       global DMA ID  72      is  'DP 0' local dma 12
 *
 *       currently there is NO representation of :
 *           'DP 1' local dma 12
 *           'DP 2' local dma 12
 *           'DP 3' local dma 12
 *           'DP 4' local dma 12
 *           'DP 5' local dma 12
 *
 *       NOTE: for aldrin (global are not local):
 *       global DMA IDs 0..11   are 'DP 0' local dma 56..59, 64..71
 *       global DMA IDs 12..23  are 'DP 1' local dma 56..59, 64..71
 *       global DMA IDs 24..32  are 'DP 2' local dma 56..59, 64..67, 62
 *       global DMA ID  72      is  'DP 2' local dma 72
 *
*/
typedef struct{
    GT_U32  dataPathFirstPort;
    GT_U32  dataPathNumOfPorts;
    const GT_U32 *localDmaMapArr;

    GT_U32  cpuPortDmaNum;
    GT_U32  tmDmaNum;
}PRV_CPSS_DXCH_PP_HW_INFO_DATA_PATH_INFO_STC;
/* number of DP (data path) that the device support */
#define MAX_DP_CNS  6

/*
 * Typedef: struct PRV_CPSS_DXCH_PP_HW_INFO_PIPE_INFO_STC
 *
 * Description: A Structure with info about the pipe.
 * Notes:
 *         the initialization of those fields should be done during "phase 1".
 *
 * Fields:
 *      gop_info - GOP info (MAC ports)
 *          gopFirstPort  - first MAC port used in the GOP.
 *          gopNumOfPorts - number of MAC ports in the GOP .
 *                              if value is ZERO ... meaning 'not valid' GOP .. ignored
 *
 *          NOTE: for bobcat3 :
 *             global MAC ports 0..35  are 'pipe 0' local ports 0..35
 *             global MAC ports 36..71 are 'pipe 1' local ports 0..35
 *
 *             currently there is NO representation of :
 *                 'pipe 0' local ports 36
 *                 'pipe 1' local ports 36
 *
*/
typedef struct{
    struct{
        GT_U32  gopFirstPort;
        GT_U32  gopNumOfPorts;
    }gop_info;

}PRV_CPSS_DXCH_PP_HW_INFO_PIPE_INFO_STC;
/* number of pipes that the device support */
#define MAX_PIPES_CNS  2
/*
 * Typedef: struct PRV_CPSS_DXCH_PP_HW_INFO_STC
 *
 * Description: A Structure including PP's HW major differences between devices.
 * Notes:
 *         the initialization of those fields should be done during "phase 1".
 *
 * Fields:
 *      l2i - info about the bridge unit
 *              supportIngressRateLimiter - is the device support the
 *                  'ingress rate limiter' (in the L2i unit)
 *      cnc - info about the CNC units.
 *            for devices that have multiple instances per port group,
 *            the values below related to each port group.
 *              cncUnits           - number of max CNC units in one CNC unit
 *              cncBlocks          - number of max CNC blocks
 *              cncBlockNumEntries - number of entries in CNC block
 *      txq - info about the TXQ
 *          revision - the revision of the TXQ
 *      txqUnitsNum - the number of TXQ units in PP
 *
 *      ha - info about the Header alteration
 *          cpuPortBitIndex - CPU port index for traffic that access the HA unit .
 *                    ch1 :
 *                          CPU port --> bit 27
 *                          cpuPortBitIndex = 27
 *                    ch2,3,xcat :
 *                          CPU port --> bit 31
 *                          cpuPortBitIndex = 27
 *                    Lion A:
 *                          CPU port --> bit 31
 *                          cpuPortBitIndex = 27
 *                    Lion B:
 *                          cpuPortBitIndex = 63
 *      mg  - management unit
 *          metalFix        - the contents of metal Fix register
 *                            at power on time.
 *                            relevant for all DxCh devices:
 *                            For xCat: MG metal Fix register value.
 *                            For dxCh3 B2: 1 (lsb set).
 *                            Otherwise: 0 (zero).
 *      eArch - eArch unit
 *          supported - PP support of eArch
 *
 *      tti - tti unit
 *          numDefaultEports - number of entries in the TTI unit for the 'Default ePort table'
 *          metalFix        - the contents of metal Fix register
 *                            at power on time.
 *                            relevant for all DxCh devices:
 *                            For xCat: TTI metal Fix register value.
 *                            Otherwise: 0 (zero).
 *
 *      legacyVplsMode - info about VPLS mode
 *          supported - Pp support of VPLS mode
 *          enabled   - application enabled the usage of VPLS mode
 *                      may be set to GT_TRUE only when supported == GT_TRUE
 *
 *      dfxServer - DFX server unit.
 *          supported - the device supports DFX server.
 *
 *      flexFieldNumBitsSupport - structure for flexible length of fields (num of bits)
 *                                 between different devices
 *
 *                                'old device'     'eArch max'
 *          phyPort    -               6                8   (BC3 - 9 bits)
 *          trunkId    -               7               12
 *          ePort      -               6               20
 *          vid        -               12              16
 *          vidx       -               12              12
 *          eVidx      -               vidx            16
 *          hwDevNum   -               5               12
 *          sstId      -               Src Id, same as -- hwDevNum --
 *          stgId      -               8               12
 *          arpPtr     -               14              22
 *          tunnelStartPtr -           12              20
 *          fid        -               12              16   - was used as VID in legacy devices
 *          fdbUserDefined             4               8
 *          portGroupId -              2               4
 *
 *       trunk -
 *              useNonTrunkMembers2Table -
 *                      GT_FALSE - there is NO 'non trunk members 2' table
 *                      GT_TRUE  - there is 'non trunk members 2' table
 *              useNonTrunkMembers1TableAsCascade -
 *                      relevant when  useNonTrunkMembers2Table == GT_TRUE
 *                      GT_FALSE - the 'non trunk members' (not 2) table , used for cascade
 *                      GT_TRUE  - the 'non trunk members' (not 2) table , used for all purposes
 *              nonTrunkTable1NumEntries -
 *                      relevant when  useNonTrunkMembers1TableAsCascade == GT_TRUE
 *                      the number of entries in 'non trunk members'
 *
 *       dedicatedCpuMac -
 *          isNotSupported -
 *              GT_TRUE - there is no dedicated MAC for CPU port
 *              GT_FALSE - there is dedicated MAC for CPU port
 *
 *       portMapping -
 *          isUsedPortMappingCheck -
 *              GT_TRUE - all access to port registers should be validated with current port mapping
 *              GT_FALSE - otherwise
 *
 *      tcam-
 *          maxFloors   - max number of floors (bc2 - 12 , bobk - 6)
 *          numBanksForHitNumGranularity - number of banks that Hit num granularity can be associated with.
 *                          (bc2 - 2 (banks : 0,6) , bobk : 6 (0,2,4,6,8,10))
 *
 *      cpuPortInfo - cpu port info
 *              dmaNumber - the Rx/Tx DMA number for the CPU.
 *
 *      gop_lms - info about gop-lms
 *              notSupported - indication that LMS unit is not supported.(need to use SMI unit,LED unit)
 *
 *      gop_tai - info about gop-tai
 *              supportSingleInstance - indication about single instance
 *
 *      dualDataPath - info about devices that supports 'dual data path' like bobk.
 *              for more details see description for each field (inline).
 *
 *      trafficManager - traffic manager info
 *           supported - is the traffic manager supported. (Bobcat2,Bobk -- support)
 *                       (Bobcat3 not support)
 *      maxPhysicalPorts - the value is the same as 'PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(devNum) + 1'
 *      bmpOfTablesInHalfSizeModeArr - bmp of tables from 'PRV_CPSS_DXCH_TABLE_ENT'
 *                                  that currently in 'half size mode'
 *                                  NOTE: needed for Bobcat3 when work in 512 physical ports mode.
 *
*/
typedef struct{
    struct{
        GT_BOOL supportIngressRateLimiter;
    }l2i;
    struct{
        GT_U32 cncUnits;
        GT_U32 cncBlocks;
        GT_U32 cncBlockNumEntries;
        GT_BOOL cncUploadUseDfxUnit;
    }cnc;
    struct{
        GT_U32  revision;
        GT_U32  txqUnitsNum;
    }txq;

    struct{
        GT_U32  cpuPortBitIndex;
    }ha;

    struct{
        GT_U32  metalFix;
    }mg;

    struct {
        GT_BOOL  supported;
    } eArch;

    struct{
        GT_U32 numDefaultEports;
        GT_U32 metalFix;
    }tti;

    struct{
        GT_BOOL supported;
        GT_BOOL enabled;
    }legacyVplsMode;

    struct{
        GT_BOOL supported;
    }dfxServer;

    struct{
        GT_U32      phyPort;
        GT_U32      trunkId;
        GT_U32      ePort;
        GT_U32      vid;
        GT_U32      vidx;
        GT_U32      eVidx;
        GT_U32      hwDevNum;
        GT_U32      sstId;
        GT_U32      stgId;
        GT_U32      arpPtr;
        GT_U32      tunnelStartPtr;
        GT_U32      fid;
        GT_U32      fdbUserDefined;
        GT_U32      portGroupId;
    }flexFieldNumBitsSupport;

    struct{
        GT_BOOL     useNonTrunkMembers2Table;
        GT_BOOL     useNonTrunkMembers1TableAsCascade;
        GT_U32      nonTrunkTable1NumEntries;
    }trunk;

    struct{
        GT_BOOL     isNotSupported;
    }dedicatedCpuMac;

    struct{
        GT_BOOL     isUsedPortMappingCheck;
    }portMapping;

    struct{
        GT_U32      maxFloors;
        GT_U32      numBanksForHitNumGranularity;
    }tcam;

    struct{
        GT_U32      dmaNumber;
    }cpuPortInfo;

    struct{
        GT_BOOL     notSupported;
    }gop_lms;

    struct {
        GT_U32  numberOfSmiIf; /* mu,ber of smi interfaces */
        GT_U32  activeSMIList[12];
        GT_BOOL activeSMIBmp [12];
        GT_U32  smiAutoPollingList[12]; /* whether there any out-of-band outpolling ports , ex: BC2 ports 0-47 1G */
    }smi_support;

    struct{
        GT_BOOL     supportSingleInstance;
    }gop_tai;

    struct{
        GT_BOOL     supported;
    }gop_ilkn;

    struct{/* next are used when not 0 corresponding to the same resource by the
            same name in flexFieldNumBitsSupport */
        GT_U32      phyPort;/* currently for compilation purposes , may be 0 in bobk too */
                            /* (256 ports)BC3: support only 256 (not 512) */
        GT_U32      ePort;/* bobk supports 6k   although 'flex' 13 bits (that imply 8k) */
        GT_U32      vid;  /* bobk supports 4.5k although 'flex' 13 bits (that imply 8k) */
                          /* (512 ports)BC3: support only 4K (not 8K) */
        GT_U32      l2LttMll;/* bobk supports 8k , bc2 32k */
        GT_U32      sstId;/* (512 ports)BC3: support only 2K (not 4K) */

    }limitedResources;/* limitations when flexFieldNumBitsSupport is not enough */

    struct{
        GT_U32 numEntriesL2Ecmp;    /* number of entries in L2Ecmp */
        GT_U32 numEntriesL3Ecmp;    /* number of entries in L3Ecmp */
        GT_U32 numEntriesIpNh;      /* number of entries in IP NH */
        GT_U32 numEntriesMllLtt;    /* number of entries in MLL LTT */
        GT_U32 numEntriesMll;       /* number of entries in MLL */
        GT_U32 numEntriesIplrMetering;  /* number of entries in Iplr Metering */
        GT_U32 numEntriesEplrMetering;  /* number of entries in Eplr Metering */
        GT_U32 numEntriesOam;       /* number of entries in the OAM table */
        GT_U32 numEntriesTxqQueue;  /* number of TXQ queues */
        GT_U32 numEntriesProtectionLoc;/* number of entries in the protection LOC table */
    }parametericTables;/* tables with different number of entries between the devices */

    struct{
        GT_U32  numOfBlocks ;/* number of memory blocks that LPM uses . bc2 = 20 , bobk = 20 */
        GT_U32  numOfLinesInBlock;/* number of entries in each block memory . bc2 = 16k , bobk = 6k */
        GT_U32  totalNumOfLinesInBlockIncludingGap;/* The gap in lines between the start of 2 consecutive blocks: bc2 = 16K,bobk = 16K */
        GT_U32  indexForPbr;/* index of entry of start of BPR */
    }lpm;

    struct{
        GT_U32  supportMultiDataPath;/* bobk support split RXDMA,Txdam,txFifo.. :
                                    rxdma0 - ports 0..47
                                    rxdma1 - ports 56..59 , 64..71 , 72 (CPU) , 73(TM)*/
        GT_U32  rxNumPortsPerDp;/* per DP : rxdma max number of port supported (size of array)
                                    bobk : 73
                                    bc3 : 13
                                    */
        GT_U32  txNumPortsPerDp;/* per DP : txdma/txfifo max number of port supported (size of array)
                                    bobk : 74
                                    bc3 : 13
                                    */
        PRV_CPSS_DXCH_PP_HW_INFO_DATA_PATH_INFO_STC info[MAX_DP_CNS];
        GT_U32  maxDp;/* number of indexes valid in info[] */
        GT_U32  numTxqDq;/* bobk hold single DQ , but 2 rxdma,txdma,txfifo
                            bobcat3 hold (per pipe) 3 DQ,rxdma,txdma,txfifo */
        GT_U32  supportRelativePortNum;/* Bobcat3 hold relative port number as
                                        index to the DP units.

                                        Bobk - hold GLOBAL port number !
                                        */
        GT_U32  txqDqNumPortsPerDp;/* per DP : number of 'TXQ_ports' that TXQ_DQ supports */
        GT_U32  mapPhyPortToTxqDqUnitIndex[PRV_CPSS_MAX_PP_PORTS_NUM_CNS];/* index to the array is 'Physical port number'
                                    the value is 'TQX-DQ' unit index */
    }multiDataPath;

    struct{
        GT_U32  numOfPipes;/* number of pipes. (per core)
                            the bobcat3 is single core but with 2 control pipes
                            value 0 and value 1 meaning : no multi pipe.
                        */
        PRV_CPSS_DXCH_PP_HW_INFO_PIPE_INFO_STC pipeInfo[MAX_PIPES_CNS];
    }multiPipe;

    struct{
        GT_BOOL     supported;
    }trafficManager;

    GT_U32      maxPhysicalPorts;
    GT_U32      bmpOfTablesInHalfSizeModeArr[(PRV_CPSS_DXCH_TABLE_LAST_E/32) + 1];
}PRV_CPSS_DXCH_PP_HW_INFO_STC;

/* macro to get pointer to the hwInfo.txq in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_TXQ_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.txq

/* macro to get pointer to the hwInfo.ha in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_HA_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.ha

/* macro to get pointer to the hwInfo.cnc in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_CNC_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.cnc

/* macro to get pointer to the hwInfo.trunk in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_TRUNK_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.trunk


/* check if txq revision is 1 or above */
#define PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(_devNum)    \
    ((PRV_CPSS_DXCH_PP_HW_INFO_TXQ_MAC(_devNum).revision) ? 1 : 0)

/* txq units number in device  */
#define PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(_devNum)    \
    (PRV_CPSS_DXCH_PP_HW_INFO_TXQ_MAC(_devNum).txqUnitsNum)

/* check if txq units number is 2  */
#define PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_2_MAC(_devNum)    \
    ((PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_MAC(_devNum) == 2) ? 1 : 0)

/* convert global port to hemisphere local port  */
#define PRV_CPSS_DXCH_GLOBAL_TO_HEM_LOCAL_PORT(_devNum,_portNum)    \
    ((PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_2_MAC(_devNum)) ? ((_portNum) & (~BIT_6)) : (_portNum))

/* convert global port to hemisphere number port  */
#define PRV_CPSS_DXCH_GLOBAL_TO_HEM_NUMBER(_devNum,_portNum)    \
    (((_portNum) & BIT_6) >> 6)

/* convert hemisphere local port to global port */
#define PRV_CPSS_DXCH_HEM_LOCAL_TO_GLOBAL_PORT(_devNum,_portGroupId,_portNum)    \
    ((PRV_CPSS_DXCH_PP_HW_INFO_TXQ_UNITS_NUM_2_MAC(_devNum)) ? ((((_portGroupId) & 0x4) << 4) | ((_portNum) & 0x3F)) : (_portNum))

/* get the HA egress cpu port bit index */
#define PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(_devNum) \
    PRV_CPSS_DXCH_PP_HW_INFO_HA_MAC(_devNum).cpuPortBitIndex

/*  ch1 special macro support : cpu port in bit 27 and not 31 in the HA register */
#define PRV_CPSS_DXCH_CH1_SPECIAL_BIT_HA_CPU_PORT_BIT_INDEX_MAC(_devNum , ch1CpuPort) \
    ((PRV_CPSS_PP_MAC(_devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) ?       \
    PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(_devNum) : ch1CpuPort)

/* macro to get pointer to the hwInfo.l2i in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_L2I_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.l2i

/* macro to get pointer to the hwInfo.mg in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_MG_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.mg

/* macro to check if 'eArch' supported/enabled. (feature generation 5) */
#define PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(_devNum)    \
    (PRV_CPSS_SIP_5_CHECK_MAC(_devNum))

/* macro to check if eArch is supported, return GT_NOT_APPLICABLE_DEVICE if not */
#define PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(_devNum)           \
    if(GT_FALSE == PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(_devNum))     \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG)

/* macro to check if 'eArch' supported/enabled. (feature generation 5) */
#define PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC    \
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC

/* macro to get to the hwInfo.flexFieldNumBitsSupport in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum)    \
    (PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.flexFieldNumBitsSupport)

/* macro to get to the hwInfo.limitedResources in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_LIMITED_RESOURCES_FIELD_MAC(_devNum)    \
    (PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.limitedResources)

/* macro to get max value of resource .
    it selects value from hwInfo.limitedResources if non zero for that resource.
    otherwise selects from hwInfo.flexFieldNumBitsSupport
    NOTE: return 'the last valid value'
*/
#define PRV_CPSS_DXCH_PP_HW_INFO_RESOURCE_MAX_VALUE_GET_MAC(_devNum,_resource)       \
    ((PRV_CPSS_DXCH_PP_HW_INFO_LIMITED_RESOURCES_FIELD_MAC(_devNum)._resource) ?      \
     (PRV_CPSS_DXCH_PP_HW_INFO_LIMITED_RESOURCES_FIELD_MAC(_devNum)._resource - 1) :  \
     ((BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum)._resource))))

/* macro to get the last valid index in vlan table.
    NOTE: it may be lower then the number returned by
        PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum)
*/
#define PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_VLAN_MAC(devNum)  \
    PRV_CPSS_DXCH_PP_HW_INFO_RESOURCE_MAX_VALUE_GET_MAC(devNum,vid)

/* macro to get the last valid index in eport table.
    NOTE: it may be lower then the number returned by
        PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum)
*/
#define PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_EPORT_MAC(devNum)  \
    PRV_CPSS_DXCH_PP_HW_INFO_RESOURCE_MAX_VALUE_GET_MAC(devNum,eport)

/* macro to get the last valid index in src-id (sstId) table.
    NOTE: it may be lower then the number returned by
        PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(devNum)
*/
#define PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_SRC_ID_MAC(devNum)  \
    PRV_CPSS_DXCH_PP_HW_INFO_RESOURCE_MAX_VALUE_GET_MAC(devNum,sstId)

/* macro to get maxValue of phyPort */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).phyPort))

/* macro to get maxValue of trunkId */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).trunkId))

/* macro to get maxValue of ePort */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).ePort))

/* macro to get maxValue of vid */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).vid))

/* macro to get maxValue of vidx */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VIDX_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).vidx))

/* macro to get maxValue of eVidx */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).eVidx))

/* macro to get maxValue of hwDevNum */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).hwDevNum))

/* macro to get maxValue of sstId */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).sstId))

/* macro to get maxValue of stgId */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_STG_ID_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).stgId))

/* macro to get maxValue of arpPtr */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_ARP_PTR_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).arpPtr))

/* macro to get maxValue of tunnelStartPtr */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TS_PTR_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).tunnelStartPtr))

/* macro to get maxValue of fid */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_FID_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).fid))

/* macro to get maxValue of fdbUserDefined */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_FDB_USER_DEFINED_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).fdbUserDefined))

/* macro to get maxValue of portGroupId */
#define PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PORT_GROUP_ID_MAC(_devNum)    \
    (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).portGroupId))

/* macro to get number of bits needed for vid */
#define PRV_CPSS_DXCH_PP_HW_NUM_BITS_VID_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).vid

/* macro to get the last valid index in L2 LTT table.
    NOTE: it may be lower then the number returned by
        PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum)
*/
#define PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_L2_LTT_MAC(_devNum)  \
    ((PRV_CPSS_DXCH_PP_HW_INFO_LIMITED_RESOURCES_FIELD_MAC(_devNum).l2LttMll) ?      \
     (PRV_CPSS_DXCH_PP_HW_INFO_LIMITED_RESOURCES_FIELD_MAC(_devNum).l2LttMll - 1) :  \
     ((BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum).eVidx))))

/* macro to get pointer to the hwInfo.tti in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_TTI_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.tti

/* macro to get pointer to the hwInfo.legacyVplsMode in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_MAC(_devNum)    \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.legacyVplsMode

/* macro to get value of hwInfo.legacyVplsMode.supported in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(_devNum)    \
    (PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.legacyVplsMode.supported)

/* macro to check if VPLS mode is supported, return GT_NOT_APPLICABLE_DEVICE if not */
#define PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(_devNum)           \
    if(GT_FALSE == PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(_devNum))     \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG)

/* macro to get value of hwInfo.legacyVplsMode.enabled in PRV_CPSS_DXCH_PP_HW_INFO_STC */
#define PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(_devNum)    \
    (PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.legacyVplsMode.enabled)

/* internal macro not to be used by any C file */
/* macro to return max port number by number of supported bits.
    the macro takes into account that 'dual device id' actually support 2 times
    the number that the bits support (due to split on 2 deviceIds)
*/
#define ____MAX_PORT_MAC(_devNum,_portType)      \
    ((PRV_CPSS_DXCH_IS_DUAL_HW_DEVICE_MAC(PRV_CPSS_HW_DEV_NUM_MAC(_devNum))) ? \
        (2 * (BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(_devNum)._portType) + 1)) : \
        (PRV_CPSS_DXCH_PP_HW_INFO_RESOURCE_MAX_VALUE_GET_MAC(_devNum,_portType) + 1))

#define PRV_CPSS_DXCH_MAX_PORT_NUMBER_MAC(_devNum) \
    ____MAX_PORT_MAC(_devNum,ePort)

#define PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(_devNum) \
    ____MAX_PORT_MAC(_devNum,phyPort)

/* max number of default ePorts in the device */
#define PRV_CPSS_DXCH_MAX_DEFAULT_EPORT_NUMBER_MAC(_devNum) \
    (PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.tti.numDefaultEports)


/* check that the port number is valid return GT_BAD_PARAM on error */
#define PRV_CPSS_DXCH_PORT_CHECK_MAC(_devNum,_portNum)                \
    if ((_portNum) >= PRV_CPSS_DXCH_MAX_PORT_NUMBER_MAC(_devNum))     \
    {                                                                 \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                          \
    }

/* check that the physical port number is valid return GT_BAD_PARAM on error */
#define PRV_CPSS_DXCH_PHY_PORT_NUM_CHECK_MAC(_devNum,_portNum)        \
    if ((_portNum) >= PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(_devNum)) \
    {                                                                 \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                          \
    }

/* check that the port number is valid for 'default ePort' . return GT_BAD_PARAM on error */
#define PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(_devNum,_portNum)                \
    if ((_portNum) >= PRV_CPSS_DXCH_MAX_DEFAULT_EPORT_NUMBER_MAC(_devNum))     \
    {                                                                          \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                   \
    }

/* check that port mapping access check is used */
#define PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(_devNum) \
    (PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.portMapping.isUsedPortMappingCheck)


/* For legacy devices: regular physical port check.
   For E_ARCH devices: check and get MAC number for given physical port */
#define PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_MAC_NUM_GET_MAC(_devNum,_portNum,_portMacNum)  \
    if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(_devNum) == GT_TRUE)       \
    {                                                                                   \
        GT_STATUS _rc;                                                                  \
        _rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(_devNum, _portNum, PRV_CPSS_DXCH_PORT_TYPE_MAC_E, &_portMacNum);\
        if(GT_OK != _rc)                                                                \
        {                                                                               \
            return _rc;                                                                 \
        }                                                                               \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(_devNum,_portNum);                             \
        _portMacNum = _portNum;                                                         \
    }

/* For legacy devices: regular physical port check include CPU port.
   For E_ARCH devices: check and get MAC number for given physical port, NOT include
                       CPU port since it has no MAC */
#define PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(_devNum,_portNum,_portMacNum)  \
    if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(_devNum) == GT_TRUE)       \
    {                                                                                   \
        GT_STATUS _rc;                                                                  \
        _rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(_devNum, _portNum, PRV_CPSS_DXCH_PORT_TYPE_MAC_E, &_portMacNum);\
        if(GT_OK != _rc)                                                                \
        {                                                                               \
            return _rc;                                                                 \
        }                                                                               \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(_devNum,_portNum);                 \
        _portMacNum = _portNum;                                                         \
    }

/* For legacy devices: regular physical port check.
   For E_ARCH devices: check and get TXQ number for given physical port */
#define PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXQ_PORT_GET_MAC(_devNum,_portNum,_portTxqNum) \
    if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(_devNum) == GT_TRUE)       \
    {                                                                                   \
        GT_STATUS _rc;                                                                  \
        _rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(_devNum, _portNum, PRV_CPSS_DXCH_PORT_TYPE_TXQ_E, &_portTxqNum);\
        if(GT_OK != _rc)                                                                \
        {                                                                               \
            return _rc;                                                                 \
        }                                                                               \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(_devNum,_portNum);                             \
        _portTxqNum = _portNum;                                                         \
    }

/* For legacy devices: regular physical port check include CPU port.
   For E_ARCH devices: check and get TXQ number for given physical port, include CPU port */
#define PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_TXQ_PORT_GET_MAC(_devNum,_portNum,_portTxqNum) \
    if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(_devNum) == GT_TRUE)       \
    {                                                                                   \
        GT_STATUS _rc;                                                                  \
        _rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(_devNum, _portNum, PRV_CPSS_DXCH_PORT_TYPE_TXQ_E, &_portTxqNum);\
        if(GT_OK != _rc)                                                                \
        {                                                                               \
            return _rc;                                                                 \
        }                                                                               \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(_devNum,_portNum);                 \
        _portTxqNum = _portNum;                                                         \
    }

/* For legacy devices: regular physical port check.
   For E_ARCH devices: check and get RX_DMA number for given physical port */
#define PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_RXDMA_NUM_GET_MAC(_devNum,_portNum,_portRxdmaNum) \
    if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(_devNum) == GT_TRUE)       \
    {                                                                                   \
        GT_STATUS _rc;                                                                  \
        _rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(_devNum, _portNum, PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E, &_portRxdmaNum);\
        if(GT_OK != _rc)                                                                \
        {                                                                               \
            return _rc;                                                                 \
        }                                                                               \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(_devNum,_portNum);                             \
        _portRxdmaNum = _portNum;                                                       \
    }

/* For legacy devices: regular physical port check include CPU port.
   For E_ARCH devices: check and get RX_DMA number for given physical port, include CPU port */
#define PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_RXDMA_NUM_GET_MAC(_devNum,_portNum,_portRxdmaNum) \
    if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(_devNum) == GT_TRUE)       \
    {                                                                                   \
        GT_STATUS _rc;                                                                  \
        _rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(_devNum, _portNum, PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E, &_portRxdmaNum);\
        if(GT_OK != _rc)                                                                \
        {                                                                               \
            return _rc;                                                                 \
        }                                                                               \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(_devNum,_portNum);                 \
        _portRxdmaNum = _portNum;                                                       \
    }

/* For legacy devices: regular physical port check.
   For E_ARCH devices: check and get TX_DMA number for given physical port */
#define PRV_CPSS_DXCH_PORT_NUM_CHECK_AND_TXDMA_NUM_GET_MAC(_devNum,_portNum,_portTxdmaNum) \
    if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(_devNum) == GT_TRUE)       \
    {                                                                                   \
        GT_STATUS _rc;                                                                  \
        _rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(_devNum, _portNum, PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E, &_portTxdmaNum);\
        if(GT_OK != _rc)                                                                \
        {                                                                               \
            return _rc;                                                                 \
        }                                                                               \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(_devNum,_portNum);                             \
        _portTxdmaNum = _portNum;                                                       \
    }

/* For legacy devices: regular physical port check include CPU port.
   For E_ARCH devices: check and get TX_DMA number for given physical port, include CPU port */
#define PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_TXDMA_NUM_GET_MAC(_devNum,_portNum,_portTxdmaNum) \
    if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(_devNum) == GT_TRUE)       \
    {                                                                                   \
        GT_STATUS _rc;                                                                  \
        _rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(_devNum, _portNum, PRV_CPSS_DXCH_PORT_TYPE_TxDMA_E, &_portTxdmaNum);\
        if(GT_OK != _rc)                                                                \
        {                                                                               \
            return _rc;                                                                 \
        }                                                                               \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(_devNum,_portNum);                 \
        _portTxdmaNum = _portNum;                                                       \
    }

/* check if tableId is in half size mode */
#define PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_GET_MAC(_devNum,tableId)  \
    ((PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.bmpOfTablesInHalfSizeModeArr[(tableId)>>5] & 1<<((tableId)&0x1f)) ? 1 : 0)

/* set tableId to in half size mode */
#define PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_SET_MAC(_devNum,tableId)  \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.bmpOfTablesInHalfSizeModeArr[(tableId)>>5] |= 1<<((tableId)&0x1f)

/* clear tableId from half size mode */
#define PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_CLEAR_MAC(_devNum,tableId)  \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->hwInfo.bmpOfTablesInHalfSizeModeArr[(tableId)>>5] &= ~(1<<((tableId)&0x1f))


/*******************************************************************************
* prvCpssDxChPortPhysicalPortMapCheckAndConvert
*
* DESCRIPTION:
*       Function checks and gets valid mapping from shadow DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       physicalPortNum   - physical port to convert
*       convertedPortType - converted port type
*
* OUTPUTS:
*       convertedPortNumPtr  - (pointer to) converted port, according to port type
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, physicalPortNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPhysicalPortMapCheckAndConvert
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            physicalPortNum,
    IN  PRV_CPSS_DXCH_PORT_TYPE_ENT     convertedPortType,
    OUT GT_U32                          *convertedPortNumPtr
);


/*******************************************************************************
* prvCpssDxChPortPhysicalPortMapReverseMappingGet
*
* DESCRIPTION:
*       Function checks and returns physical port number that mapped to given
*       MAC/TXQ/DMA port number.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       origPortType      - original port type
*       origPortNum       - MAC/TXQ/DMA port number
*
* OUTPUTS:
*       physicalPortNumPtr  - (pointer to) physical port number
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, origPortNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPhysicalPortMapReverseMappingGet
(
    IN  GT_U8                           devNum,
    IN  PRV_CPSS_DXCH_PORT_TYPE_ENT     origPortType,
    IN  GT_U32                          origPortNum,
    OUT GT_PHYSICAL_PORT_NUM            *physicalPortNumPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChInfoEnhancedh */

