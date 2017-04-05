/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwTables.c
*
* DESCRIPTION:
*     Private API implementation for tables access.
*
* FILE REVISION NUMBER:
*       $Revision: 178 $
*
*******************************************************************************/
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>

/* "not modify" value for unsigned integer fields  */
#define NOT_MODIFIED_VALUE_CNS  0xFFFFFFFF

/* indication that the number in the lower 30 bits is 'fraction' (1/x) and not (x) */
#define FRACTION_INDICATION_CNS         BIT_31

/* indication that the number in the lower 30 bits is 'vertical' :
            the x entries in the first 'line' are not : 0,1
            the x entries in the first 'line' are     : 0,depth
 */
#define VERTICAL_INDICATION_CNS         BIT_30

/* indication that the number in the lower 30 bits is 'fraction' (1/x) and not (x)
    but 1/2 of the entry in second half of the table

    meaning that 'read'  of entry requires 2 'read' lines . from 2 half indexes in the table.
    meaning that 'write' of entry requires 2 'write' lines. to 2 half indexes in the table.
*/
#define FRACTION_HALF_TABLE_INDICATION_CNS         BIT_29


/* update the address according to new unit base address */
#define UPDATE_ADDR_WITH_NEW_UNIT_MAC(oldAddr,newUnitBaseAddr)  \
                    (oldAddr) = ((oldAddr) & 0x00FFFFFF) | (newUnitBaseAddr)

/* Object that holds callback function to table HW access */
CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC prvDxChTableCpssHwAccessObj =
                                            {NULL, NULL, NULL, NULL};

/* max num of vlans entries in Mapping table -- range 0..4095 */
#define PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS (4096)

/* max num of adjacency entries in TRILL Adjacency Check table -- range 0..511 */
#define PRV_CPSS_DXCH_MAX_NUM_ADJACENCY_CNS (256)

/* number of bits in the Bobcat2; Caelum; Bobcat3 - TTI action entry */
#define LION3_TTI_ACTION_BITS_SIZE_CNS  246
/* number of bits in the Bobcat2; Caelum; Bobcat3 - TTI physical port entry */
#define LION3_TTI_PHYSICAL_PORT_BITS_SIZE_CNS  56
/* number of bits in the Bobcat2; Caelum; Bobcat3 - TTI default port entry */
#define LION3_TTI_DEFAULT_PORT_BITS_SIZE_CNS  126
/* number of bits in the Bobcat2; Caelum; Bobcat3 - TTI PCL UDB entry */
#define LION3_TTI_PCL_UDB_BITS_SIZE_CNS  550
/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2I ingress vlan entry */
#define LION3_L2I_INGRESS_VLAN_BITS_SIZE_CNS  113
/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2I Ingress Bridge Port Membership Table */
#define LION3_L2I_INGRESS_BRIDGE_PORT_MEMBERS_BITS_SIZE_CNS  256
/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2I Ingress Span State Group Index Table */
#define LION3_L2I_INGRESS_SPAN_STATE_GROUP_INDEX_BITS_SIZE_CNS  12
/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2I ingress eport entry */
#define LION3_L2I_EPORT_BITS_SIZE_CNS  79
/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2I ingress STP entry */
#define LION3_L2I_INGRESS_STP_BITS_SIZE_CNS  512
/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2I ingress eport learn prio entry */
#define LION3_L2I_EPORT_LEARN_PRIO_BITS_SIZE_CNS  40
/* number of bits in the Bobcat2; Caelum; Bobcat3 - l2i source trunk attribute entry */
#define LION3_L2I_SOURCE_TRUNK_ATTRIBUTE_BITS_SIZE_CNS 40
/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2I ingress physical port entry */
#define LION3_L2I_PHYSICAL_PORT_BITS_SIZE_CNS   29
/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2I ingress physical port entry */
#define LION3_L2I_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_BITS_SIZE_CNS   22
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_QAG egress eport entry */
#define LION3_EGF_QAG_EGRESS_EPORT_BITS_SIZE_CNS    47
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_QAG egress eport entry */
#define LION3_EGF_QAG_EGRESS_EVLAN_BITS_SIZE_CNS    768
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_QAG Target Port Mapper entry */
#define LION3_EGF_QAG_TARGET_PORT_MAPPER_BITS_SIZE_CNS    8
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_QAG Code To Loopback Mapper entry */
#define LION3_EGF_QAG_CPU_CODE_TO_LOOPBACK_MAPPER_BITS_SIZE_CNS    17
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_QAG CPU To Loopback Mapper entry */
#define LION3_EGF_QAG_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_BITS_SIZE_CNS    16

/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_SHT egress eport entry */
#define LION3_EGF_SHT_EGRESS_EPORT_BITS_SIZE_CNS    15
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_SHT egress vlan entry */
#define LION3_EGF_SHT_EGRESS_VLAN_BITS_SIZE_CNS    257
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_SHT egress vlan attributes entry */
#define LION3_EGF_SHT_EGRESS_VLAN_ATTRIBUTES_BITS_SIZE_CNS 6
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_SHT egress vlan spanning entry */
#define LION3_EGF_SHT_EGRESS_VLAN_SPANNING_BITS_SIZE_CNS 12
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EGF_SHT egress vlan mapper entry */
#define LION3_EGF_SHT_EGRESS_VLAN_MAPPER_BITS_SIZE_CNS 12

/* bmp of 256 ports - 1 bit per port */
#define BMP_256_PORTS_CNS           256
/* bmp of 512 ports - 1 bit per port */
#define BMP_512_PORTS_CNS           512

/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA ePort 1 entry */
#define LION3_HA_EGRESS_EPORT_1_BITS_SIZE_CNS                   87
#define BOBCAT_B0_HA_EGRESS_EPORT_1_BITS_SIZE_CNS               85
/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA ePort 2 entry */
#define LION3_HA_EGRESS_EPORT_2_BITS_SIZE_CNS                   24
#define BOBCAT_B0_HA_EGRESS_EPORT_2_BITS_SIZE_CNS               25
/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA physical Port 1 entry */
#define LION3_HA_EGRESS_PHYSICAL_PORT_1_BITS_SIZE_CNS           27
/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA physical Port 2 entry */
#define LION3_HA_EGRESS_PHYSICAL_PORT_2_BITS_SIZE_CNS           77
/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA tunnel start / ARP entry */
#define LION3_HA_TUNNEL_START_BITS_SIZE_CNS                     384
/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA qos profile to exp entry */
#define LION3_HA_QOS_PROFILE_TO_EXP_BITS_SIZE_CNS                3
/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA EPCL UDB configuration entry */
#define LION3_HA_EPCL_UDB_CONFIG_BITS_SIZE_CNS                  600
/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA Domain Table entry */
#define LION3_HA_PTP_DOMAIN_BITS_SIZE_CNS                       35
/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA Generic Tunnel Start Table entry */
#define LION3_HA_GEN_TS_PROFILE_BITS_SIZE_CNS                   534

/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA Global MAC SA Table entry */
#define LION3_HA_GLOBAL_MAC_SA_BITS_SIZE_CNS                    48

/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA egress vlan entry */
#define LION3_HA_EGRESS_VLAN_BITS_SIZE_CNS                              36
/* number of bits in the Bobcat2; Caelum; Bobcat3 - HA egress vlan MAC_SA entry */
#define LION3_HA_EGRESS_VLAN_MAC_SA_SIZE_CNS                     8
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EQ ingress STC for physical port entry */
#define LION3_EQ_INGRESS_STC_PHYSICAL_PORT_SIZE_CNS   80
/* number of bits in the Bobcat2; Caelum; Bobcat3 - MLL entry */
#define LION3_MLL_ENTRY_BITS_SIZE_CNS       158
/* number of bits in the Bobcat2; Caelum; Bobcat3 - L2 MLL LTT entry */
#define LION3_L2_MLL_LTT_ENTRY_BITS_SIZE_CNS       20
/* number of bits in the Bobcat2; Caelum; Bobcat3 - FDB unit FDB entry */
#define LION3_FDB_FDB_BITS_SIZE_CNS     138
/* number of bits in the Bobcat2; Caelum; Bobcat3 - PCL unit trunk hash mask CRC entry */
#define LION3_PCL_TRUNK_HASH_MASK_CRC_BITS_SIZE_CNS   74
/* number of bits in the Bobcat2; Caelum; Bobcat3 - PCL unit PCL configuration entry */
#define LION3_PCL_PCL_CONFIG_BITS_SIZE_CNS   27
/* number of bits in the Bobcat2; Caelum; Bobcat3 - PCL unit PCL UDB select entry*/
#define LION3_IPCL_UDB_SELECT_BITS_SIZE_CNS 316
/* number of bits in the Bobcat2; Caelum; Bobcat3 - TCAM action entry (for TTI,IPCL(0,1,2),EPCL) */
#define LION3_TCAM_PCL_TTI_ACTION_BITS_SIZE_CNS 240
/* number of bits in the Bobcat2; Caelum; Bobcat3 - ERMRK Timestamp Configuration Table entry*/
#define LION3_ERMRK_TIMESTAMP_CFG_BITS_SIZE_CNS 26
#define BOBCAT2_B0_ERMRK_TIMESTAMP_CFG_BITS_SIZE_CNS 27
/* number of bits in the Bobcat2; Caelum; Bobcat3 - ERMRK Local Action Table entry*/
#define LION3_ERMRK_PTP_LOCAL_ACTION_BITS_SIZE_CNS 6
/* number of bits in the Bobcat2; Caelum; Bobcat3 - ERMRK Target Port Table entry*/
#define LION3_ERMRK_PTP_TARGET_PORT_BITS_SIZE_CNS 321
/* number of bits in the Bobcat2; Caelum; Bobcat3 - ERMRK Source Port Table entry*/
#define LION3_ERMRK_PTP_SOURCE_PORT_BITS_SIZE_CNS 321
/* number of bits in the Bobcat2; Caelum; Bobcat3 - ERMRK QoS Map Dscp Table entry*/
#define LION3_ERMRK_QOS_DSCP_MAP_BITS_SIZE_CNS 24
/* number of bits in the Bobcat2; Caelum; Bobcat3 - ERMRK QoS Map TC_DP Table entry*/
#define LION3_ERMRK_QOS_TC_DP_MAP_BITS_SIZE_CNS 24

/* number of bits in the Bobcat2; Caelum; Bobcat3 - EPCL unit EPCL configuration entry */
#define LION3_EPCL_PCL_CONFIG_BITS_SIZE_CNS   27
/* number of bits in the Bobcat2; Caelum; Bobcat3 - EPCL unit UDB Select table entry */
#define LION3_EPCL_UDB_SELECT_BITS_SIZE_CNS   306

/* number of bits in the Bobcat2; Caelum; Bobcat3 - BMA port maping table entry */
#define LION3_BMA_PORT_MAPPING_BIT_SIZE_CNS   8
/* number of bits in the Bobcat2; Caelum; Bobcat3 - BMA multicast counters table entry */
#define LION3_BMA_MULTICAST_COUNTERS_BIT_SIZE_CNS   11

/* number of bits in the Bobcat2; Caelum; Bobcat3 - TTI PTP Command Table entry */
#define LION3_TTI_PTP_COMMAND_BITS_SIZE_CNS   256
/* number of bits in the Bobcat2; Caelum; Bobcat3 - IPvX unit Router eVlan entry */
#define LION3_IPVX_ROUTER_EVLAN_BITS_SIZE_CNS   66
/* number of bits in the Bobcat2; Caelum; Bobcat3 - IPvX unit Router ePort entry */
#define LION3_IPVX_ROUTER_EPORT_BITS_SIZE_CNS   128
/* number of bits in the Bobcat2; Caelum; Bobcat3 - IPvX unit Router Nexthop entry */
#define LION3_IPVX_ROUTER_NEXTHOP_BITS_SIZE_CNS 104
/* number of bits in the Bobcat2; Caelum; Bobcat3 - IPvX unit Router Nexthop Age Bits entry */
#define LION3_IPVX_ROUTER_NEXTHOP_AGE_BITS_BITS_SIZE_CNS 32
/* number of bits in the Bobcat2; Caelum; Bobcat3 - IPvX unit Router Access Matrix entry */
#define LION3_IPVX_ROUTER_ACCESS_MATRIX_BITS_SIZE_CNS 128
/* number of bits in the Bobcat2; Caelum; Bobcat3 - IPvX unit Router Qos Profile Offsets entry */
#define LION3_IPVX_ROUTER_QOS_PROFILE_OFFSETS_BITS_SIZE_CNS 12
/* number of bits in the Bobcat2; Caelum; Bobcat3 - TCAM entry */
#define LION3_TCAM_BITS_SIZE_CNS 84
/* number of bits in the Bobcat2; Caelum; Bobcat3 - Policer Management Counters entry */
#define LION3_IPVX_POLICER_MANAGEMENT_COUNTERS_BITS_SIZE_CNS 74
/* number of bits in the Bobcat2; Caelum; Bobcat3 - e Attributes entry */
#define LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS 36
/* number of bits in the Bobcat2; Caelum; Bobcat3 - Re-Marking entry */
#define LION3_IPVX_POLICER_REMARKING_BITS_SIZE_CNS 51
/* number of bits in the Bobcat2; Caelum; Bobcat3 - Hierarchical Policing entry */
#define LION3_IPVX_POLICER_HIERARCHICAL_BITS_SIZE_CNS 18
/* number of bits in the Bobcat2; Caelum; Bobcat3 - Metering entry */
#define LION3_IPVX_POLICER_METERING_BITS_SIZE_CNS 241
/* number of bits in the Bobcat2; Caelum; Bobcat3 - Counting (max) entry */
#define LION3_IPVX_POLICER_COUNTING_BITS_SIZE_CNS 241

/* number of bits in the Bobcat2; Caelum; Bobcat3 - tail drop max queue limit table entry */
#define LION3_TAIL_DROP_MAX_QUEUE_LIMITS_BITS_SIZE_CNS  34
/* number of bits in the Bobcat2; Caelum; Bobcat3 - tail drop eq queue limits dp0 table entry */
#define LION3_TAIL_DROP_EQ_QUEUE_LIMITS_DP0_BITS_SIZE_CNS  48
/* number of bits in the Bobcat2; Caelum; Bobcat3 - tail drop eq queue limits dp0 table entry */
#define LION3_TAIL_DROP_DQ_QUEUE_BUF_BITS_SIZE_CNS  34
/* number of bits in the Bobcat2; Caelum; Bobcat3 - tail drop eq queue limits dp0 table entry */
#define LION3_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_BITS_SIZE_CNS  14
/* number of bits in the Bobcat2; Caelum; Bobcat3 - tail drop eq queue limits dp0 table entry */
#define LION3_TAIL_DROP_EQ_QUEUE_LIMITS_DP1_BITS_SIZE_CNS  48
/* number of bits in the Bobcat2; Caelum; Bobcat3 - tail drop eq queue limits dp0 table entry */
#define LION3_TAIL_DROP_EQ_QUEUE_LIMITS_DP2_BITS_SIZE_CNS  48

/* number of bits in the Bobcat2; Caelum; Bobcat3 - CN sample interval table entry */
#define LION3_CN_SAMPLE_INTERVALS_BITS_SIZE_CNS  36

/* number of bits in the Bobcat2; Caelum; Bobcat3 - tail drop counters - Buffers Queue Maintenance counters table entry */
#define LION3_TAIL_DROP_COUNTERS_MAIN_BUFF_BITS_SIZE_CNS  14
/* number of bits in the Bobcat2; Caelum; Bobcat3 - tail drop counters - Multicast Buffers Queue Maintenance counters table entry */
#define LION3_TAIL_DROP_COUNTERS_MAIN_MC_BUFF_BITS_SIZE_CNS  20

/* number of bits in the Bobcat2; Caelum; Bobcat3 - TXQ LL - Descriptors Queue Maintenance counters table entry */
#define LION3_TXQ_LINK_LIST_COUNTERS_MAIN_DESC_BITS_SIZE_CNS  14

/* number of bits in the Bobcat2; Caelum; Bobcat3 - FC Mode Profile TC XOFF Thresholds table entry */
#define LION3_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_BITS_SIZE_CNS  29
/* number of bits in the Bobcat2; Caelum; Bobcat3 - FC Mode Profile TC XON Thresholds table entry */
#define LION3_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_BITS_SIZE_CNS  29
/* number of bits in the Bobcat2; Caelum; Bobcat3 - PFC/LLFC Counters table entry */
#define LION3_PFC_LLFC_COUNTERS_BITS_SIZE_CNS  29

/* number of bits in the Bobcat2 - TM DROP Drop Masking table entry */
#define LION3_TM_DROP_DROP_MASKING_BITS_SIZE_CNS               12
/* number of bits in the Bobcat2 - TM DROP Queue Profile Id table entry */
#define LION3_TM_DROP_QUEUE_PROFILE_ID_BITS_SIZE_CNS           48

/* number of bits in the Bobcat2 - TM QMAP CPU Code to TC table entry */
#define LION3_TM_QMAP_CPU_CODE_TO_TC_BITS_SIZE_CNS             4
/* number of bits in the Bobcat2 - TM QMAP Target Device To Seletor Index table entry */
#define LION3_TM_QMAP_TARGET_DEV_TO_INDEX_BITS_SIZE_CNS        8
/* number of bits in the Bobcat2 - TM QMAP Policing Enable per Queue table entry */
#define LION3_TM_QMAP_TM_QMAP_POLICING_ENABLE_BITS_SIZE_CNS    32
/* number of bits in the Bobcat2 - TM QMAP Queue Id Selector table entry */
#define LION3_TM_QMAP_QUEUE_ID_SELECTOR_BITS_SIZE_CNS          126

/* number of bits in the Bobcat2 - TM Ingress Glue Packet Length Offset table entry */
#define LION3_TM_INGR_GLUE_L1_PKT_LEN_OFFSET_SIZE_CNS          32

/* number of bits in the Bobcat2 - TM Egress Glue Aging Queue Profile table entry */
#define LION3_TM_EGR_GLUE_AGING_QUEUE_PROFILE_SIZE_CNS         64
/* number of bits in the Bobcat2 - TM Egress Glue Aging Profile Thresholds table entry */
#define LION3_TM_EGR_GLUE_AGING_PROFILE_THESHOLDS_SIZE_CNS     60
/* number of bits in the Bobcat2 - TM Egress Glue Packet Length Offset table entry */
#define LION3_TM_EGR_GLUE_TARGET_INTERFACE_SIZE_CNS            32

/* number of bits in the Bobcat2 - TM FCU Ethernet DMA To Port Mapping table entry */
#define LION3_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_SIZE_CNS       32
/* number of bits in the Bobcat2 - TM FCU Interlaken DMA To Port Mapping table entry table entry */
#define LION3_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_SIZE_CNS       32
/* number of bits in the Bobcat2 - TM FCU PFC Port To CNode Port Mapping table entry */
#define LION3_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_SIZE_CNS    72
/* number of bits in the Bobcat2 - TM FCU Port Ingress Timer Configuration table entry */
#define LION3_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_SIZE_CNS       32
/* number of bits in the Bobcat2 - TM FCU Ingress Timers table entry */
#define LION3_TM_FCU_INGRESS_TIMERS_SIZE_CNS                   256
/* number of bits in the Bobcat2 - TM FCU PFC Port To Pysical Port Mapping table entry */
#define LION3_TM_FCU_PORT_TO_PHYSICAL_PORT_MAPPING_SIZE_CNS 32


/* number of bits in the Bobcat2; Caelum; Bobcat3 - TXQ DQ - Priority Token Bucket Configuration table entry */
#define LION3_PRIO_TOKEN_BUCKET_CONFIGURATION_BITS_SIZE_CNS  488

/* number of bits in the Bobcat2; Caelum; Bobcat3 - TXQ DQ - Port Token Bucket Configuration table entry */
#define LION3_PORT_TOKEN_BUCKET_CONFIGURATION_BITS_SIZE_CNS  61

/* number of bits in the Bobcat2; Caelum; Bobcat3 - TXQ DQ - Egress STC Table */
#define LION3_EGRESS_STC_TABLE_BITS_SIZE_CNS  80

/* number of table lines in the Bobcat2; Caelum; Bobcat3 - e Attributes table */
#define LION3_IPVX_POLICER_E_ATTRIBUTES_MAX_ENTRY_CNS   5120

/* SIP5 TXQ DQ Unit ports numbers for single port group devices  */
#define SIP5_SINGLE_PORT_GROUP_TXQ_DQ_PORTS_NUM_CNS 72

/* SIP5 TXQ DQ Unit ports numbers for multi port group devices  */
#define SIP5_MULTI_PORT_GROUP_TXQ_DQ_PORTS_NUM_CNS 18

/* number of bits in the BobK - IPvX unit Router ePort entry */
#define BOBK_IPVX_ROUTER_EPORT_BITS_SIZE_CNS   112

/* number of bits in the Bobk - Metering configuration entry */
#define BOBK_POLICER_METERING_CONFIG_BITS_SIZE_CNS 51
/* number of bits in the Bobk - Metering token bucket entry */
#define BOBK_POLICER_METERING_TOKEN_BUCKET_BITS_SIZE_CNS 211
/* number of bits in the Bobk - Metering Conformance Sign entry */
#define BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS 2

#define PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum) \
           (PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoSize)
/* the number of flows that the PCL classifies */
#define PCL_NUM_FLOWS_CNS   12
/* Bobcat2; Caelum; Bobcat3 the number of flows that the PCL classifies */
#define LION3_PCL_NUM_FLOWS_CNS   16

/* macro to get the alignment as number of words from the number of bits in the entry */
#define BITS_TO_BYTES_ALIGNMENT_MAC(bits) \
    (((bits) > 512) ? 128 :      \
     ((bits) > 256) ?  64 :      \
     ((bits) > 128) ?  32 :      \
     ((bits) >  64) ?  16 :      \
     ((bits) >  32) ?   8 :   4)


/*calc number of words form number of bits */
#define NUM_WORDS_FROM_BITS_MAC(x)   (((x) + 31)>>5)

/* number of elements in static array (non dynamic alloc) */
#define NUM_ELEMENTS_IN_ARR_MAC(_array) \
    sizeof(_array)/sizeof(_array[0])

/* bind the array of fields formats to the DB of tables format according to table index
    tblDbPtr -  PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo
    tblFormatIndex - value from PRV_CPSS_DXCH_TABLE_FORMAT_ENT
    tblFormatArr - static array of type PRV_CPSS_ENTRY_FORMAT_TABLE_STC
*/
#define BIND_FIELDS_FORMAT_TO_TABLE_MAC(tblDbPtr , tblFormatIndex , tblFormatArr ) \
    tblDbPtr[tblFormatIndex].fieldsInfoPtr = tblFormatArr;                       \
    tblDbPtr[tblFormatIndex].numFields =  NUM_ELEMENTS_IN_ARR_MAC(tblFormatArr); \
    tblDbPtr[tblFormatIndex].patternNum = 0;                                     \
    tblDbPtr[tblFormatIndex].patternBits = 0

/* bind the array of fields formats to the DB of tables format according to table index
    tblDbPtr -  PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo
    tblFormatIndex - value from PRV_CPSS_DXCH_TABLE_FORMAT_ENT
    tblFormatArr - static array of type PRV_CPSS_ENTRY_FORMAT_TABLE_STC
    tblPatternBits - number of bits in pattern
    tblPatterNum - number of patterns in table entry
*/
#define BIND_PATTERN_FIELDS_FORMAT_TO_TABLE_MAC(tblDbPtr , tblFormatIndex , tblFormatArr, tblPatternBits, tblPatternNum ) \
    tblDbPtr[tblFormatIndex].fieldsInfoPtr = tblFormatArr;                      \
    tblDbPtr[tblFormatIndex].numFields =  NUM_ELEMENTS_IN_ARR_MAC(tblFormatArr); \
    tblDbPtr[tblFormatIndex].patternBits = tblPatternBits;                       \
    tblDbPtr[tblFormatIndex].patternNum = tblPatternNum


/*******************************************************************************
* PRV_TABLE_INFO_PTR_GET_MAC
*
* DESCRIPTION:
*       get the table info
* INPUTS:
*       devNum - device number
*       tableType - the table to get info
* OUTPUTS:
*       NONE
*
* RETURNS:
*       pointer to the table's info
*
* COMMENTS:
*
*
*******************************************************************************/
#define PRV_TABLE_INFO_PTR_GET_MAC(_devNum, _tableType) \
 (&PRV_CPSS_DXCH_PP_MAC(_devNum)->accessTableInfoPtr[_tableType])

/* access to info of the DxCh device tables */
#define PRV_CPSS_DXCH_DEV_TBLS_MAC(devNum)  \
    (PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr)

/* max number of words in entry */
#define MAX_ENTRY_SIZE_CNS   64

/* for indirect information table */
/* for cheetah2 only */
typedef enum {
    PRV_CPSS_DXCH2_TABLE_STP_E       = PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E + 1,
    PRV_CPSS_DXCH2_TABLE_VLAN_E,
    PRV_CPSS_DXCH2_TABLE_MULTICAST_E,
    PRV_CPSS_LION3_FDB_TABLE_INDIRECT_E,
    PRV_CPSS_LION3_IOAM_TABLE_INDIRECT_E,
    PRV_CPSS_LION3_EOAM_TABLE_INDIRECT_E
}INDIRECT_INDEX_ENT;

/* This value is used when control registor relates only to one table.
   In this case there is no need to update table index in control register.
   The action that is done for all tables is: table index << table index offset
   0 << 0 doesn't have any impact */
#define PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS           0

/* number of words in the cheetah QoS profile entry */
#define PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS       1


/* number of entries in cheetah vlan table */
#define PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS                       4096
/* number of words in the cheetah vlan entry */
#define PRV_CPSS_DXCH_VLAN_ENTRY_WORDS_COUNT_CNS              3
#define PRV_CPSS_DXCH2_VLAN_ENTRY_WORDS_COUNT_CNS             4
#define PRV_CPSS_DXCHXCAT_VLAN_ENTRY_WORDS_COUNT_CNS          6

/* number of words in the cheetah multicast entry */
#define PRV_CPSS_DXCH_MULTICAST_ENTRY_WORDS_COUNT_CNS         1

/* number of words in the cheetah mac sa entry */
#define PRV_CPSS_DXCH_ROUTE_HA_MAC_SA_ENTRY_WORDS_COUNT_CNS   1


#define PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS 64

/* 16K */
#define PRV_CPSS_DXCH_FDB_MAX_ENTRIES_CNS                0x4000
#define PRV_CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS       4

/* 32K */
#define PRV_CPSS_DXCH3_FDB_MAX_ENTRIES_CNS                0x8000

/* number of entries in cheetah policer table */
#define PRV_CPSS_DXCH_POLICER_MAX_ENTRIES_CNS                256
/* number of words in cheetah policer entry*/
#define PRV_CPSS_DXCH_POLICER_ENTRY_WORDS_SIZE_CNS           2

/* number of entries in cheetah policer counters table */
#define PRV_CPSS_DXCH_POLICER_COUNTERS_MAX_ENTRIES_CNS      16
/* number of words in cheetah policer counters entry*/
#define PRV_CPSS_DXCH_POLICER_COUNTERS_ENTRY_WORDS_SIZE_CNS  2

/* 8 registers are in the same entry */
#define PRV_CPSS_DXCH_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS   8
#define PRV_CPSS_DXCHXCAT_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS   1

/* number of byte in the cheetah port vlan qos entry */
#define PRV_CPSS_DXCH_VLAN_PORT_QOS_ENTRY_WORDS_COUNT_CNS     2

/* 128 trunks * 8 members in trunk = 1024 */
#define PRV_CPSS_DXCH_TRUNK_MEMBERS_TABLE_MAX_ENTRIES_CNS     1024

/* number of words in the cheetah trunk entry */
#define PRV_CPSS_DXCH_TRUNK_ENTRY_WORDS_COUNT_CNS             1

/* number of entries in cheetah rate limit table */
#define PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS        32

/* number of words in the cheetah rate limit entry */
#define PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS        1

/* number of entries in cheetah pcl config table */
#define PRV_CPSS_DXCH_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS        1152
#define PRV_CPSS_DXCH2_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS       4224

#define PRV_CPSS_DXCH2_EGRESS_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS 4160

/* number of words in the cheetah pcl config entry */
#define PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS        1

/* number of words in the cheetah pcl config entry */
#define PRV_CPSS_DXCH3_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS        2

/* number of entries in cheetah arp da table */
#define PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS   1024

/* number of words in the cheetah arp da entry */
#define PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS   2
#define PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS  6

/* number of words in the cheetah2 QoSProfile-to-Route-Block-Offset entry */
#define PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_WORDS_COUNT_CNS   1
/* number of entries in the cheetah2 QoSProfile-to-Route-Block-Offset table */
#define PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_TABLE_MAX_ENTRIES_CNS    16

/* number of words in the cheetah2 Router Access Matrix Line entry */
#define PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_WORDS_COUNT_CNS   1
/* number of entries in the cheetah2 Router Access Matrix Line table */
#define PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_TABLE_MAX_ENTRIES_CNS    8

/* number of words in LTT and TT Action Table entry */
#define PRV_CPSS_DXCH2_LTT_TT_ACTION_WORDS_COUNT_CNS   4
/* number of entries in the cheetah2 LTT and TT Action table */
#define PRV_CPSS_DXCH2_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS    1024

/* number of words in Unicast/Multicast Router Next Hop  entry */
#define PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_WORDS_COUNT_CNS   4
/* number of entries in the cheetah2 Unicast/Multicast Router Next Hop table */
#define PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_TABLE_MAX_ENTRIES_CNS    4096

/* number of words in yhe cheetah2 Router Next Hop Table Age Bits entry */
#define PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_COUNT_CNS   1
/* number of entries in the cheetah2 Router Next Hop Table Age Bits Entry table */
#define PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_TABLE_MAX_ENTRIES_CNS    128
/* number of words in the cheetah3 mac2me table */
#define PRV_CPSS_DXCH3_MAC2ME_WORDS_COUNT_CNS   4
/* number of entries in the cheetah3 mac2me table */
#define PRV_CPSS_DXCH3_MAC2ME_TABLE_MAX_ENTRIES_CNS    8
/* number of words in the chhetah3 vlan translation table (ingress/egress) */
#define PRV_CPSS_DXCH3_VLAN_TRANSLATION_WORDS_COUNT_CNS     1
/* number of entries in the cheetah3 vlan translation tables (ingress/egress) */
#define PRV_CPSS_DXCH3_VLAN_TRANSLATION_TABLE_MAX_ENTRIES_CNS       4096
/* number of words in the chhetah3 VRF ID table */
#define PRV_CPSS_DXCH3_VRF_ID_WORDS_COUNT_CNS     1
/* number of entries in the cheetah3 VRF ID table */
#define PRV_CPSS_DXCH3_VRF_ID_TABLE_MAX_ENTRIES_CNS       4096
/* number of words in LTT and TT Action Table entry (Ch3) */
#define PRV_CPSS_DXCH3_LTT_TT_ACTION_WORDS_COUNT_CNS    4
/* number of entries in the cheetah3 LTT and TT Action table */
#define PRV_CPSS_DXCH3_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS    1024
/* number of words in LTT and TT Action Table entry (xCat) */
#define PRV_CPSS_XCAT_LTT_TT_ACTION_WORDS_COUNT_CNS    5

/* number of entries in cheetah cpu code table */
#define PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS          256

/* number of words in the cheetah cpu code entry */
#define PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS          1

/* STG entry size -- in words */
#define PRV_CPSS_DXCH_STG_ENTRY_WORDS_SIZE_CNS           2

/* cheetah default value for next word offset for table direct access */
#define PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS           4

typedef enum {
    SPECIAL_ADDRESS_NOT_MODIFIED_E = 0x0EEEEEEE,/* unchanged address indication */
    SPECIAL_ADDRESS_NOT_EXISTS_E   = 0x0FFFFFFF /* non exists address */
}SPECIAL_ADDRESS_ENT;

#define TABLES_INFO_DIRECT_NOT_EXISTS_CNS   \
    SPECIAL_ADDRESS_NOT_EXISTS_E, 0 , 0

#define TABLES_INFO_DIRECT_NOT_MODIFIED_CNS   \
    SPECIAL_ADDRESS_NOT_MODIFIED_E, 0 , 0

static PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC indirectAccessTableInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x0B800328,  0x0B800300,   0,  2,  2,  10,  1},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {0x0B800328,  0x0B800320,   0,  2,  1,  10,  1},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {0x0B000028,  0x0B001000,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {0x0B000034,  0x0B004000,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {0x0B000030,  0x0B003000,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {0x0B800204,  0x0B800200,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {0x0B00002C,  0x0B002000,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {0x0C000024,  0x0C000020,   0 , 2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {0x0A00000C,  0x0A000004,   15, 0,  2,  13,  12},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {0x0A00000C,  0x0A000000,   15, 0,  0,  13,  12},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {0x0A00000C,  0x0A000008,   15, 0,  1,  13,  12},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {0x07800208,  0x07800204,   0,  3,  1,  2,   1},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {0x07800208,  0x07800200,   0,  3,  0,  2,   1},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {0x06000064,  0x06000054,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E */
    {0x0C000014,  0x0C00000C,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {0x0C000038,  0x0C000030,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_DXCH2_TABLE_STP_E */
    {0x0A000010,  0x0A000008,   15, 0,  2,  13,  12},

    /* PRV_CPSS_DXCH2_TABLE_VLAN_E */
    {0x0A000010,  0x0A000000,   15, 0,  0,  13,  12},

    /* PRV_CPSS_DXCH2_TABLE_MULTICAST_E */
    {0x0A000010,  0x0A00000C,   15, 0,  1,  13,  12},

    /* PRV_CPSS_LION3_FDB_TABLE_INDIRECT_E - for Bobcat2; Caelum; Bobcat3 : different then ch1/2/3..xcat/lion/lion2
       used instead of 'PRV_CPSS_DXCH_TABLE_FDB_E' */
    {0x04000130,  0x04000134,   0,  2, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_LION3_IOAM_TABLE_INDIRECT_E - for Bobcat2 */
    {0x1C0000D0, 0x1C0000D4,    0, 16, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1},

    /* PRV_CPSS_LION3_EOAM_TABLE_INDIRECT_E - for Bobcat2 */
    {0x1E0000D0, 0x1E0000D4,    0, 16, PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS,
          PRV_CPSS_DXCH_TABLE_NON_MULTI_MODE_CNS, 1}

};

/* xCat3 indirect table information */
static PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC indirectAccessXcat3TableInfo[] =
{
    /* PRV_DXCH_XCAT3_TABLE_TRUNK_MEMBERS_E */
    {0x14000028,  0x14001000,   0,  2,  0,  0,  1},

    /* PRV_DXCH_XCAT3_TABLE_STATISTICAL_RATE_LIMIT_E */
    {0x14000034,  0x14004000,   0,  2,  0,  0,  1},

    /* PRV_DXCH_XCAT3_TABLE_CPU_CODE_E */
    {0x14000030,  0x14003000,   0,  2,  0,  0,  1},

    /* PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_E */
    {0x1400002C,  0x14002000,   0,  2,  0,  0,  1},

    /* PRV_DXCH_XCAT3_TABLE_FDB_E */
    {0x0B000064,  0x0B000054,   0,  2,  0,  0,  1},

    /* PRV_DXCH_XCAT3_TABLE_LOGICAL_TARGET_MAPPING_E */
    {0x140000A0,  0x1400A000,   0,  2,  0,  0,  1},

    /* PRV_DXCH_XCAT3_TABLE_VLAN_MAPPING_E */
    {0x140000A4,  0x140000A8,   0,  2,  0,  0,  1},

    /* PRV_DXCH_XCAT3_TABLE_LP_EGRESS_VLAN_MEMBER_E */
    {0x140000AC,  0x140000B0,   0,  2,  0,  0,  1},

    /* PRV_DXCH_XCAT3_TABLE_L2MLL_VIDX_ENABLE_E */
    {0x19000464,  0x19000460,   0,  2,  0,  0,  1},

    /* PRV_DXCH_XCAT3_TABLE_L2MLL_POINTER_MAPPING_E */
    {0x19000474,  0x19000470,   0,  2,  0,  0,  1}
};

/* Cheetah's 2 and 3 only direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC directAccessTableInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x0B810800, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {0x0B810000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {0x0B400000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {0x0B100000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {0x0B200000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {0x0B840000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {0x0B300000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* used for CH3 only, CH2 uses indirect access, the base address is for CH3 only */
    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {0x0c080000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {0x0A100000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {0x0A400000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {0x0A200000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {0x07F80000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {0x07E80000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {0x06400000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E */
    {0x0C100000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {0x0C300000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E */
    {0x07700000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E */
    {0x07E80000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E */
    {0x02800300, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E */
    {0x02800440, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_LTT_TT_ACTION_E */
    {0x02900000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E */
    {0x02B00000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E */
    {0x02801000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_TABLE_MAC2ME_E */
    {0xB800700, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E */
    {0x0B804000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E */
    {0x07FC0000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_TABLE_VRF_ID_E */
    {0x0A300000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_DXCH3_LTT_TT_ACTION_E */
    {0x02900000, 0x4, 0x5000}
};

/* CH3 Ingress PCL configuration table direct info */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC dxCh3IpclCfgTblInfo =
    {0x0B840000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS};

/* CH3 Egress PCL configuration table direct info */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC dxCh3EpclCfgTblInfo =
    {0x07f00000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS};

/*
 * typedef: enum PRV_DXCH_XCAT_TABLES_E
 *
 * Description:
 *    Index for directAccessXcatTableInfo table
 *
 * Enumerations:
 *    PRV_DXCH_XCAT_TABLE_VLAN_E                - VLAN table index
 *    PRV_DXCH_XCAT_TABLE_VRF_ID_E              - VRD-ID table index
 *    PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E       - Port VLAN QoS table index
 *    PRV_DXCH_XCAT_TABLE_STG_E                 - STG table index
 *    PRV_DXCH_XCAT_LTT_TT_ACTION_E             - LTT action table index
 *    PRV_DXCH_XCAT_TABLE_MULTICAST_E           - Multicast table index
 *    PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E     - Router HA MAC SA table index
 *    PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E     - Router HA ARP DA table index
 *    PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E - Ingress VLAN translation table index
 *    PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E  - Egress VLAN translation table index
 *    PRV_DXCH_XCAT_TABLE_IPCL_CFG_E            - Ingress PCL configuration table
 *    PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E    - Ingress PCL Lookup1
 *                                                configuration table
 *    PRV_DXCH_XCAT_TABLE_EPCL_CFG_E            - Egress PCL configuration table
 *    PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E        - Ingress PCL UDB Cfg table
 *    PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E    - Tunnel Start table
 *    PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E  - Port Protocol VID and QoS
 *                                                Configuration Table
 *    PRV_CPSS_XCAT_TABLE_MAC2ME_E              - TTI MAC2ME table
 *    PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E         - BCN Profiles Table
 *    PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E - Egress Policer Remarking table
 *    PRV_DXCH_XCAT_TABLE_L2MLL_VIDX_ENABLE_E        - L2MLL VIDX enable table
 *    PRV_DXCH_XCAT_TABLE_L2MLL_POINTER_MAPPING_E    - L2MLL Pointer Mapping table
 */
typedef enum {
    PRV_DXCH_XCAT_TABLE_VLAN_E = 0,
    PRV_DXCH_XCAT_TABLE_VRF_ID_E,
    PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E,
    PRV_DXCH_XCAT_TABLE_STG_E,
    PRV_DXCH_XCAT_LTT_TT_ACTION_E,
    PRV_DXCH_XCAT_TABLE_MULTICAST_E,
    PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E,
    PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E,
    PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E,
    PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E,
    PRV_DXCH_XCAT_TABLE_IPCL_CFG_E,
    PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E,
    PRV_DXCH_XCAT_TABLE_EPCL_CFG_E,
    PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E,
    PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E,
    PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E,
    PRV_CPSS_XCAT_TABLE_MAC2ME_E,
    PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E,
    PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,
    PRV_DXCH_XCAT_TABLE_L2MLL_VIDX_ENABLE_E,
    PRV_DXCH_XCAT_TABLE_L2MLL_POINTER_MAPPING_E
}PRV_DXCH_XCAT_TABLES_E;

/*
 * typedef: enum PRV_DXCH_XCAT3_DIRECT_ACCESS_TABLES_E
 *
 * Description:
 *    Index for directAccessXcatTableInfo table
 *
 * Enumerations:
 *    PRV_DXCH_XCAT3_TABLE_VLAN_PORT_PROTOCOL_E  - Port Protocol VID and QoS
 *                                                 Configuration Table
 *    PRV_DXCH_XCAT3_TABLE_PORT_VLAN_QOS_E       - Port VLAN QoS table
 *    PRV_DXCH_XCAT3_TABLE_IPCL_CFG_E            - Ingress PCL configuration table
 *    PRV_DXCH_XCAT3_TABLE_STG_E                 - STG table
 *    PRV_DXCH_XCAT3_TABLE_VLAN_E                - VLAN table
 *    PRV_DXCH_XCAT3_TABLE_MULTICAST_E           - Multicast table
 *    PRV_DXCH_XCAT3_TABLE_ROUTE_HA_MAC_SA_E     - Router HA MAC SA table
 *    PRV_DXCH_XCAT3_TABLE_ROUTE_HA_ARP_DA_E     - Router HA ARP DA table
 *    PRV_DXCH_XCAT3_TABLE_EPCL_CFG_E            - Egress PCL configuration table
 *    PRV_DXCH_XCAT3_TABLE_TUNNEL_START_CFG_E    - Tunnel Start table
 *    PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E - QoS Profile To Route
 *                                                        Block Offset table
 *    PRV_DXCH_XCAT3_TABLE_ROUTE_ACCESS_MATRIX_E - Router Access Matrix table
 *    PRV_DXCH_XCAT3_UC_MC_ROUTE_NEXT_HOP_E      - Router Next Hop table
 *    PRV_DXCH_XCAT3_ROUTE_NEXT_HOP_AGE_E        - Router Next Hop Aging Bits table
 *    PRV_DXCH_XCAT3_TABLE_MAC2ME_E              - TTI MAC2ME table
 *    PRV_DXCH_XCAT3_TABLE_INGRESS_TRANSLATION_E - Ingress VLAN translation table
 *    PRV_DXCH_XCAT3_TABLE_EGRESS_TRANSLATION_E  - Egress VLAN translation table
 *    PRV_DXCH_XCAT3_TABLE_VRF_ID_E              - VRD-ID table
 *    PRV_DXCH_XCAT3_LTT_TT_ACTION_E             - LTT action table
 *    PRV_DXCH_XCAT3_TABLE_IPCL_LOOKUP1_CFG_E    - Ingress PCL Lookup1
 *                                                configuration table
 *    PRV_DXCH_XCAT3_TABLE_IPCL_UDB_CFG_E        - Ingress PCL UDB Cfg table
 *    PRV_DXCH_XCAT3_TABLE_BCN_PROFILE_E         - BCN Profiles Table
 *    PRV_DXCH_XCAT3_TABLE_EGRESS_POLICER_REMARKING_E - Egress Policer Remarking table
 *    PRV_DXCH_XCAT3_TABLE_MLL_L2MLL_VIDX_ENABLE_E        - L2MLL VIDX enable table
 *    PRV_DXCH_XCAT3_TABLE_MLL_L2MLL_POINTER_MAPPING_E    - L2MLL Pointer Mapping table
 */
typedef enum {
    PRV_DXCH_XCAT3_TABLE_VLAN_PORT_PROTOCOL_E = 0,
    PRV_DXCH_XCAT3_TABLE_PORT_VLAN_QOS_E,
    PRV_DXCH_XCAT3_TABLE_IPCL_CFG_E,
    PRV_DXCH_XCAT3_TABLE_STG_E,
    PRV_DXCH_XCAT3_TABLE_VLAN_E,
    PRV_DXCH_XCAT3_TABLE_MULTICAST_E,
    PRV_DXCH_XCAT3_TABLE_ROUTE_HA_MAC_SA_E,
    PRV_DXCH_XCAT3_TABLE_ROUTE_HA_ARP_DA_E,
    PRV_DXCH_XCAT3_TABLE_EPCL_CFG_E,
    PRV_DXCH_XCAT3_TABLE_TUNNEL_START_CFG_E,
    PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E,
    PRV_DXCH_XCAT3_TABLE_ROUTE_ACCESS_MATRIX_E,
    PRV_DXCH_XCAT3_UC_MC_ROUTE_NEXT_HOP_E,
    PRV_DXCH_XCAT3_ROUTE_NEXT_HOP_AGE_E,
    PRV_DXCH_XCAT3_TABLE_MAC2ME_E,
    PRV_DXCH_XCAT3_TABLE_INGRESS_TRANSLATION_E,
    PRV_DXCH_XCAT3_TABLE_EGRESS_TRANSLATION_E,
    PRV_DXCH_XCAT3_TABLE_VRF_ID_E,
    PRV_DXCH_XCAT3_LTT_TT_ACTION_E,
    PRV_DXCH_XCAT3_TABLE_IPCL_LOOKUP1_CFG_E,
    PRV_DXCH_XCAT3_TABLE_IPCL_UDB_CFG_E,
    PRV_DXCH_XCAT3_TABLE_BCN_PROFILE_E,
    PRV_DXCH_XCAT3_TABLE_EGRESS_POLICER_REMARKING_E,
    PRV_DXCH_XCAT3_TABLE_MLL_L2MLL_VIDX_ENABLE_E,
    PRV_DXCH_XCAT3_TABLE_MLL_L2MLL_POINTER_MAPPING_E
}PRV_DXCH_XCAT3_DIRECT_ACCESS_TABLES_E;

/*
 * typedef: enum PRV_DXCH_XCAT3_INDIRECT_ACCESS_TABLES_E
 *
 * Description:
 *    Index for indirectAccessXcatTableInfo table
 *
 * Enumerations:
 *    PRV_DXCH_XCAT3_TABLE_TRUNK_MEMBERS_E          - Trunk Members table.
 *    PRV_DXCH_XCAT3_TABLE_STATISTICAL_RATE_LIMIT_E - Statistical Rate Linit table
 *    PRV_DXCH_XCAT3_TABLE_CPU_CODE_E               - CPU Code table
 *    PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_E            - Qos Profile To Qos table
 *    PRV_DXCH_XCAT3_TABLE_FDB_E                    - FDB table
 *    PRV_DXCH_XCAT3_TABLE_LOGICAL_TARGET_MAPPING_E - Logical Target Mapping table
 *    PRV_DXCH_XCAT3_TABLE_VLAN_MAPPING_E           - Egress Filter Vlan Mapping table
 *    PRV_DXCH_XCAT3_TABLE_LP_EGRESS_VLAN_MEMBER_E  - Egress Filter Vlan Member table
 *    PRV_DXCH_XCAT3_TABLE_L2MLL_VIDX_ENABLE_E      - L2MLL VIDX enable table
 *    PRV_DXCH_XCAT3_TABLE_L2MLL_POINTER_MAPPING_E  - L2MLL Pointer Mapping table
 *
 */
typedef enum {
    PRV_DXCH_XCAT3_TABLE_TRUNK_MEMBERS_E = 0,
    PRV_DXCH_XCAT3_TABLE_STATISTICAL_RATE_LIMIT_E,
    PRV_DXCH_XCAT3_TABLE_CPU_CODE_E,
    PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_E,
    PRV_DXCH_XCAT3_TABLE_FDB_E,
    PRV_DXCH_XCAT3_TABLE_LOGICAL_TARGET_MAPPING_E,
    PRV_DXCH_XCAT3_TABLE_VLAN_MAPPING_E,
    PRV_DXCH_XCAT3_TABLE_LP_EGRESS_VLAN_MEMBER_E,
    PRV_DXCH_XCAT3_TABLE_L2MLL_VIDX_ENABLE_E,
    PRV_DXCH_XCAT3_TABLE_L2MLL_POINTER_MAPPING_E
}PRV_DXCH_XCAT3_INDIRECT_ACCESS_TABLES_E;

/*
 * typedef: enum PRV_DXCH_LION_TABLES_E
 *
 * Description:
 *    Index for directAccessLionBTableInfoExt table
 *
 * Enumerations:
 *      PRV_DXCH_LION_TABLE_START_E         - index of last xCat table
 *      PRV_DXCH_LION_TABLE_VLAN_INGRESS_E  - Ingress VLAN table index
 *      PRV_DXCH_LION_TABLE_VLAN_EGRESS_E   - Egress VLAN table index
 *      PRV_DXCH_LION_TABLE_STG_INGRESS_E   - Ingress STG table index
 *      PRV_DXCH_LION_TABLE_STG_EGRESS_E    - Egress STG table index
 *      PRV_CPSS_LION_TABLE_PORT_ISOLATION_L2_E - L2 port isolation table index
 *      PRV_CPSS_LION_TABLE_PORT_ISOLATION_L3_E - L3 port isolation table index
 *      PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E -
 *                                                shaper per port per priority
 *                                                table index
 *      PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E-
 *                                                shaper per port table index
 *      PRV_CPSS_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E - Source ID table index
 *      PRV_CPSS_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E - Non trunk table index
 *      PRV_CPSS_LION_TABLE_TXQ_DESIGNATED_PORT_E   - Trunk designated table index
 *      PRV_CPSS_LION_TABLE_TXQ_EGRESS_STC_E        - Egress STC table index
 *      PRV_CPSS_LION_TABLE_ROUTER_VLAN_URPF_STC_E  - VLAN URPF table index
 *      PRV_CPSS_LION_TABLE_TRUNK_HASH_MASK_CRC_E   - Trunk Hash mask table index
 */
typedef enum {
    PRV_DXCH_LION_TABLE_START_E = PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,

    /* new Lion tables */

    PRV_DXCH_LION_TABLE_VLAN_INGRESS_E,
    PRV_DXCH_LION_TABLE_VLAN_EGRESS_E,

    PRV_DXCH_LION_TABLE_STG_INGRESS_E,
    PRV_DXCH_LION_TABLE_STG_EGRESS_E,

    PRV_CPSS_LION_TABLE_PORT_ISOLATION_L2_E,
    PRV_CPSS_LION_TABLE_PORT_ISOLATION_L3_E,

    PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E,
    PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E,

    PRV_CPSS_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
    PRV_CPSS_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E,
    PRV_CPSS_LION_TABLE_TXQ_DESIGNATED_PORT_E,

    PRV_CPSS_LION_TABLE_TXQ_EGRESS_STC_E,
    PRV_CPSS_LION_TABLE_ROUTER_VLAN_URPF_STC_E,
    PRV_CPSS_LION_TABLE_TRUNK_HASH_MASK_CRC_E

}PRV_DXCH_LION_TABLES_E;

/*
 * typedef: enum PRV_DXCH_XCAT2_TABLES_E
 *
 * Description:
 *    Index for directAccessXCAT2TableInfo table
 *
 * Enumerations:
 *    PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E  - IPCL Lookup 0_1
 *                                                Configuration table index
 *    PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E   - IPCL Lookup 1
 *                                                Configuration table index
 *    PRV_DXCH_XCAT2_TABLE_MAC2ME_E             - MAC-2-Me table index
*/
typedef enum {
    PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E = 0,
    PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E,
    PRV_DXCH_XCAT2_TABLE_MAC2ME_E
}PRV_DXCH_XCAT2_TABLES_E;

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3TtiDefaultEportTableFieldsFormat[LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS___LAST_VALUE___E] =
{
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PROT_BASED_QOS_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PROT_BASED_VLAN_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_DEF_TAG1_VLAN_ID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PORT_UP0_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_INGRESS_TAG0_TPID_PROFILE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_INGRESS_TAG1_TPID_PROFILE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_VLAN_TRANSLATION_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_P_VID_PRECEDENCE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_P_EVLAN_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_P_EVLAN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_802_1AH_MAC_TO_ME_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_EN_802_1AH_TTI_LOOKUP_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_ETHERNET_CLASSIFIER_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_MPLS_TUNNEL_TERMINATION_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_MPLS_MAC_TO_ME_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_IPV4_MAC_TO_ME_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_TTI_FOR_TT_ONLY_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_TUNNEL_TERMINATION_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_EXTENDED_DSA_BYPASS_BRIDGE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PORT_QOS_PROFILE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(10),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRUST_DSA_TAG_QOS_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRUST_EXP_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRUST_DSCP_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRUST_UP_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_MAP_DSCP_TO_DSCP_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRUST_QOS_MAPPING_TABLE_INDEX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PORT_MODIFY_DSCP_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PORT_MODIFY_UP_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PORT_QOS_PRECEDENCE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_NESTED_VLAN_ACCESS_PORT_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_OVERSIZE_UNTAGGED_PKTS_FILTER_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRILL_ENGINE_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRILL_INGRESS_OUTER_VID0_E */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_802_1AH_PASSENGER_STAG_IS_TAG0_1_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_6_TOTAL_LENGTH_DEDUCTION_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_OAM_LINK_LAYER_PDU_TRAP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_OAM_LINK_LAYER_LOOPBACK_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_UP_CFI_TO_QOS_TABLE_SELECT_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP0_PCL_CFG_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP1_PCL_CFG_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_LOOKUP2_PCL_CFG_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_ASSIGN_VF_ID_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRUST_L2_QOS_TAG0_OR_TAG1_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_PKT_TYPE_UDB_KEY_I_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_MC_DUPLICATION_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV6_MC_DUPLICATION_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_MPLS_MC_DUPLICATION_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRILL_MC_DUPLICATION_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PBB_MC_DUPLICATION_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_MRU_INDEX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3TtiPhysicalPortTableFieldsFormat[LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS___LAST_VALUE___E] =
{
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_DEFAULT_SOURCE_EPORT_NUMBER_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_IS_RING_CORE_PORT_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_IS_LOOPED_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_ENABLE_MRU_CHECK_ON_CASCADE_PORT_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_CC_LINE_CARD_PORTS_DEFAULT_EPORT_BASE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_CC_LINE_CARD_TRUNK_DEFAULT_EPORT_BASE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_CENTRALIZED_CHASSIS_PORT_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_TRUNK_ID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_QOS_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3TtiEportAttributesTableFieldsFormat[LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS___LAST_VALUE___E] =
{
/* LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_ING_TAG0_TPID_PROFILE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/* LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_ING_TAG1_TPID_PROFILE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/* LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_NUM_OF_TAGS_TO_POP_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/* LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_ING_POLICY_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_DIS_IPCL0_FOR_ROUTED_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_TRUST_L2_QOS_TAG0_OR_TAG1_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_OVERRIDE_MASK_HASH_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_HASH_MASK_INDEX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/* LION3_TTI_EPORT_ATTRIBUTES_TABLE_FIELDS_PHY_SRC_MC_FILTERING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3TtiDefaultPortProtocolEvlanAndQosConfigTableFieldsFormat[LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS___LAST_VALUE___E] =
{
/* LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_EVLAN_E, */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/* LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_EVLAN_COMMAND_E, */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/* LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_EVLAN_PRECEDENCE_E, */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_VALID_E, */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_MODIFY_UP_E, */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_MODIFY_DSCP_E, */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_MODIFY_QOS_PROFILE_E, */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/* LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_QOS_PROFILE_E, */
    PRV_CPSS_STANDARD_FIELD_MAC(10),
/* LION3_TTI_DEFAULT_PORT_PROTOCOL_EVLAN_AND_QOS_CONFIGURATION_TABLE_FIELDS_PROTOCOL_QOS_PRECEDENCE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3TtiActionTableFieldsFormat[LION3_TTI_ACTION_TABLE_FIELDS___LAST_VALUE___E] =
{
/*        LION3_TTI_ACTION_TABLE_FIELDS_COMMAND*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),

/*        LION3_TTI_ACTION_TABLE_FIELDS_CPU_CODE*/
    PRV_CPSS_STANDARD_FIELD_MAC(8),

/*,        LION3_TTI_ACTION_TABLE_FIELDS_MIRROR_TO_ANALYZER_PORT*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),

/*,        LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),

/*IF <REDIRECT COMMAND> = NO REDIRECT (0)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_FLOW_ID*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_IPCL_UDB_CONFIGURATION_TABLE_UDE_INDEX*/
        PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_RESERVED*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*     <REDIRECT COMMAND> != EGRESS_INTERFACE (0,2,3..)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_TTI_RESERVED_ENABLE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_TTI_RESERVED_VALUE*/
        PRV_CPSS_STANDARD_FIELD_MAC(13),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY2_LOOKUP_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY1_LOOKUP_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY0_LOOKUP_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_IPCL_PROFILE_INDEX*/
        PRV_CPSS_STANDARD_FIELD_MAC(13),

/*ELSE IF <REDIRECT COMMAND> = REDIRECT TO EGRESS INTERFACE (1)*/
/*    IF <USEVIDX> = 0*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_ISTRUNK*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             1,
             LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
    /*    ELSE IF <USEVIDX> = 0 & <ISTRUNK> = 1*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TRUNK_NUMBER*/
                    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                     12,
                     LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_IS_TRUNK_E},
    /*            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_RESERVED*/
    /*    ELSE IF <USEVIDX> = 0 & <ISTRUNK> = 0*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EPORT_NUMBER*/
                    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                     13,
                     LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_IS_TRUNK_E},
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_DEVICE_NUMBER*/
                    PRV_CPSS_STANDARD_FIELD_MAC(10),
/*    ELSE, WHEN <USEVIDX> = 1*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EVIDX*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             16,
             LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
/*,               LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_RESERVED*/
            PRV_CPSS_STANDARD_FIELD_MAC(8),
/*,            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_USE_EVIDX*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_VNT_L2_ECHO*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),

/*    TUNNEL START*/
/*,            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    IF (<TUNNEL START> == 1)*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_INDEX*/
            PRV_CPSS_STANDARD_FIELD_MAC(15),
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_PASSENGER_TYPE*/
            PRV_CPSS_STANDARD_FIELD_MAC(1),
/*            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_RESERVED*/
/*    ELSE, <TUNNEL START> == 0*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_ARP_POINTER*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             17,
             LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_E},

/*,           LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_DA*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,           LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_SA*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),

/*ELSE IF <REDIRECT COMMAND> = REDIRECT TO ROUTER (2)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_ROUTER_ROUTER_LTT_INDEX*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},

/*        LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT TO ROUTER    TTI RESERVED
        LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT TO ROUTER    IPCL CONFIGURATION TABLE POINTER*/

/*ELSE IF <REDIRECT COMMAND> = ASSIGN VRF-ID (4)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN_VRF_ID_VRF_ID*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
/*        LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN VRF-ID     RESERVED
        LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN VRF-ID     TTI RESERVED
        LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN VRF-ID     IPCL CONFIGURATION TABLE POINTER*/

/*,        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_CNC_COUNTER*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_IPCL_PROFILE_INDEX_E},
/*,        LION3_TTI_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX*/
    PRV_CPSS_STANDARD_FIELD_MAC(14),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_METER*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_COUNTER*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_POLICER_INDEX*/
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_ID_SET_ENABLE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_ID*/
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ACTION_STOP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_BRIDGE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_INGRESS_PIPE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN_PRECEDENCE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_NESTED_VLAN*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_EVID_CMD*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN*/
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_VID1_CMD*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_VID1*/
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_QOS_PRECEDENCE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_QOS_PROFILE*/
    PRV_CPSS_STANDARD_FIELD_MAC(7),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_DSCP*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_UP*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_KEEP_PREVIOUS_QOS*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_UP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_DSCP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_EXP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MAP_DSCP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP0*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP1_COMMAND*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP1*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PACKET_TYPE*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_COPY_TTL*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TUNNEL_TERMINATION*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MPLS_COMMAND*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_HASH_MASK_INDEX*/
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_QOS_MAPPING_TABLE_INDEX*/
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_L_LSP_QOS_PROFILE_ENABLE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TTL*/
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_DEC_TTL*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT_ASSIGNMENT_ENABLE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT*/
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TT_HEADER_LENGTH*/
    PRV_CPSS_STANDARD_FIELD_MAC(5),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/*IF <IS PTP PACKET> = 0*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_TIMESTAMP_ENABLE*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET_E},
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_OFFSET_INDEX*/
        PRV_CPSS_STANDARD_FIELD_MAC(7),
/*        LION3_TTI_ACTION_TABLE_FIELDS_    RESERVED*/
/*ELSE IF <IS PTP PACKET> =1*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_TRIGGER_TYPE*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         2,
         LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET_E},
/*,           LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_OFFSET*/
        PRV_CPSS_STANDARD_FIELD_MAC(7),

/*LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_OAM_PROCESSING_WHEN_GAL_OR_OAL_EXISTS_E*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     0,
     LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_OFFSET_E},
/*,        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROCESSING_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_CW_BASED_PSEUDO_WIRE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TTL_EXPIRY_VCCV_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PWE3_FLOW_LABEL_EXIST*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PW_CW_BASED_E_TREE_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP_CFI_TO_QOS_TABLE_SELECT_MODE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_RX_IS_PROTECTION_PATH*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_RX_ENABLE_PROTECTION_SWITCHING*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SET_MAC2ME*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROFILE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_APPLY_NON_DATA_CW_COMMAND*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PW_TAG_MODE*/
     PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_TTI_1_LOOKUP*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_CHANNEL_TYPE_TO_OPCODE_MAPPING_EN*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_NON_MPLS_TRANSIT_TUNNEL_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_10TtiActionTableFieldsFormat[LION3_TTI_ACTION_TABLE_FIELDS___LAST_VALUE___E] =
{
/*        LION3_TTI_ACTION_TABLE_FIELDS_COMMAND*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),

/*        LION3_TTI_ACTION_TABLE_FIELDS_CPU_CODE*/
    PRV_CPSS_STANDARD_FIELD_MAC(8),

/*,        LION3_TTI_ACTION_TABLE_FIELDS_MIRROR_TO_ANALYZER_PORT*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),

/*,        LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),

/*IF <REDIRECT COMMAND> = NO REDIRECT (0)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_FLOW_ID*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_IPCL_UDB_CONFIGURATION_TABLE_UDE_INDEX*/
        PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_RESERVED*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*     <REDIRECT COMMAND> != EGRESS_INTERFACE (0,2,3..)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_TTI_RESERVED_ENABLE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_TTI_RESERVED_VALUE*/
        PRV_CPSS_STANDARD_FIELD_MAC(13),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY2_LOOKUP_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY1_LOOKUP_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY0_LOOKUP_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_IPCL_PROFILE_INDEX*/
        PRV_CPSS_STANDARD_FIELD_MAC(13),

/*ELSE IF <REDIRECT COMMAND> = REDIRECT TO EGRESS INTERFACE (1)*/
/*    IF <USEVIDX> = 0*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_ISTRUNK*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             1,
             LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
    /*    ELSE IF <USEVIDX> = 0 & <ISTRUNK> = 1*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TRUNK_NUMBER*/
                    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                     12,
                     LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_IS_TRUNK_E},
    /*            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_RESERVED*/
    /*    ELSE IF <USEVIDX> = 0 & <ISTRUNK> = 0*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EPORT_NUMBER*/
                    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                     13,
                     LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_IS_TRUNK_E},
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_DEVICE_NUMBER*/
                    PRV_CPSS_STANDARD_FIELD_MAC(10),
/*    ELSE, WHEN <USEVIDX> = 1*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EVIDX*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             16,
             LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
/*,               LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_RESERVED*/
            PRV_CPSS_STANDARD_FIELD_MAC(8),
/*,            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_USE_EVIDX*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_VNT_L2_ECHO*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),

/*    TUNNEL START*/
/*,            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    IF (<TUNNEL START> == 1)*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_INDEX*/
            PRV_CPSS_STANDARD_FIELD_MAC(15),
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_PASSENGER_TYPE*/
            PRV_CPSS_STANDARD_FIELD_MAC(1),
/*            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_RESERVED*/
/*    ELSE, <TUNNEL START> == 0*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_ARP_POINTER*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             17,
             LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_E},

/*,           LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_DA*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,           LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_SA*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),

/*ELSE IF <REDIRECT COMMAND> = REDIRECT TO ROUTER (2)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_ROUTER_ROUTER_LTT_INDEX*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},

/*        LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT TO ROUTER    TTI RESERVED
        LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT TO ROUTER    IPCL CONFIGURATION TABLE POINTER*/

/*ELSE IF <REDIRECT COMMAND> = ASSIGN VRF-ID (4)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN_VRF_ID_VRF_ID*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
/*        LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN VRF-ID     RESERVED
        LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN VRF-ID     TTI RESERVED
        LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN VRF-ID     IPCL CONFIGURATION TABLE POINTER*/

/*,        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_CNC_COUNTER*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_IPCL_PROFILE_INDEX_E},
/*,        LION3_TTI_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX*/
    PRV_CPSS_STANDARD_FIELD_MAC(14),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_METER*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_COUNTER*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_POLICER_INDEX*/
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_ID_SET_ENABLE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_ID*/
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ACTION_STOP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_BRIDGE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_INGRESS_PIPE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN_PRECEDENCE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_NESTED_VLAN*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_EVID_CMD*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN*/
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_VID1_CMD*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_VID1*/
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_QOS_PRECEDENCE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_QOS_PROFILE*/
    PRV_CPSS_STANDARD_FIELD_MAC(7),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_DSCP*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_UP*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_KEEP_PREVIOUS_QOS*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_UP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_DSCP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_EXP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MAP_DSCP*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP0*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP1_COMMAND*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP1*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PACKET_TYPE*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_COPY_TTL*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TUNNEL_TERMINATION*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MPLS_COMMAND*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_HASH_MASK_INDEX*/
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_QOS_MAPPING_TABLE_INDEX*/
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_L_LSP_QOS_PROFILE_ENABLE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TTL*/
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_DEC_TTL*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT_ASSIGNMENT_ENABLE*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT*/
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TT_HEADER_LENGTH*/
    PRV_CPSS_STANDARD_FIELD_MAC(5),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/*IF <IS PTP PACKET> = 0*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_TIMESTAMP_ENABLE*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET_E},
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_OFFSET_INDEX*/
        PRV_CPSS_STANDARD_FIELD_MAC(7),
/*        LION3_TTI_ACTION_TABLE_FIELDS_    RESERVED*/
/*ELSE IF <IS PTP PACKET> =1*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_TRIGGER_TYPE*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         2,
         LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET_E},
/*,           LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_OFFSET*/
        PRV_CPSS_STANDARD_FIELD_MAC(6),/*was 7 in sip 5*/

/*LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_OAM_PROCESSING_WHEN_GAL_OR_OAL_EXISTS_E*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,/*was 0 in sip 5*/
     LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_OFFSET_E},
/*,        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROCESSING_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_CW_BASED_PSEUDO_WIRE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TTL_EXPIRY_VCCV_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PWE3_FLOW_LABEL_EXIST*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PW_CW_BASED_E_TREE_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP_CFI_TO_QOS_TABLE_SELECT_MODE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_RX_IS_PROTECTION_PATH*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_RX_ENABLE_PROTECTION_SWITCHING*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SET_MAC2ME*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROFILE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_APPLY_NON_DATA_CW_COMMAND*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PW_TAG_MODE*/
     PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_TTI_1_LOOKUP*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_CHANNEL_TYPE_TO_OPCODE_MAPPING_EN*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_NON_MPLS_TRANSIT_TUNNEL_ENABLE*/
     PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20TtiActionTableFieldsFormat[LION3_TTI_ACTION_TABLE_FIELDS___LAST_VALUE___E] =
{
/*        LION3_TTI_ACTION_TABLE_FIELDS_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),

/*        LION3_TTI_ACTION_TABLE_FIELDS_CPUCODE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(8),

/*,        LION3_TTI_ACTION_TABLE_FIELDS_MIRRORTOANALYZERPORT_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),

/*,        LION3_TTI_ACTION_TABLE_FIELDS_REDIRECTCMD_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),

/*If <Redirect Command> = No Redirect (0)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_FLOW_ID_E*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         13,/*in sip 5 was 12_E*/
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_IPCL_UDB_CONFIGURATION_TABLE_UDE_INDEX_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_RESERVED_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(2),
/*     <Redirect Command> != Egress_Interface (0,2,3..)*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_TTI_RESERVED_ENABLE_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_TTI_RESERVED_VALUE_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(13),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY2_LOOKUP_MODE_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY1_LOOKUP_MODE_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY0_LOOKUP_MODE_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_IPCL_PROFILE_INDEX_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(13),

/*else If <Redirect Command> = Redirect to Egress Interface (1)*/
/*    if <UseVIDX> = 0_E*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_ISTRUNK_E*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             1,
             LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
    /*    else if <UseVIDX> = 0 & <IsTrunk> = 1_E*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TRUNKNUMBER_E*/
                    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                     12,
                     LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_IS_TRUNK_E},
    /*            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_RESERVED_E*/
    /*    else if <UseVIDX> = 0 & <IsTrunk> = 0_E*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EPORT_NUMBER_E*/
                    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                     14,/* in sip 5 was 13 */
                     LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_IS_TRUNK_E},
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_DEVICE_NUMBER_E*/
                    PRV_CPSS_STANDARD_FIELD_MAC(10),
/*    else, when <UseVIDX> = 1_E*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EVIDX_E*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             16,
             LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
/*,               LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_RESERVED_E*/
            PRV_CPSS_STANDARD_FIELD_MAC(9),
/*,            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_USE_EVIDX_E*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_DEVICE_NUMBER_E},
/*,            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_VNT_L2_ECHO_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),

/*    Tunnel Start_E*/
/*,            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    if (<Tunnel Start> == 1)_E*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_INDEX_E*/
            PRV_CPSS_STANDARD_FIELD_MAC(16),/* in sip 5 was 15 */
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_PASSENGER_TYPE_E*/
            PRV_CPSS_STANDARD_FIELD_MAC(1),
/*            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_RESERVED_E*/
/*    else, <Tunnel Start> == 0_E*/
/*,                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_ARP_POINTER_E*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             18,/* in sip 5 was 17 */
             LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_E},

/*,           LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_DA_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,           LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_SA_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),

/*else If <Redirect Command> = Redirect To Router (2)_E*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_ROUTER_ROUTER_LTT_INDEX_E*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         18,/*in sip5 was 16_E*/
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},

/*        LION3_TTI_ACTION_TABLE_FIELDS_Redirect To Router    TTI reserved
        LION3_TTI_ACTION_TABLE_FIELDS_Redirect To Router    IPCL Configuration Table Pointer_E*/

/*else If <Redirect Command> = Assign VRF-ID (4)_E*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN_VRF_ID_VRF_ID_E*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E},
/*        LION3_TTI_ACTION_TABLE_FIELDS_Assign VRF-ID     Reserved
        LION3_TTI_ACTION_TABLE_FIELDS_Assign VRF-ID     TTI reserved
        LION3_TTI_ACTION_TABLE_FIELDS_Assign VRF-ID     IPCL Configuration Table Pointer_E*/

/*,        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_CNC_COUNTER_E*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_IPCL_PROFILE_INDEX_E},
/*,        LION3_TTI_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(14),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_METER_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_COUNTER_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_POLICER_INDEX_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(14),/* in sip 5 was 16 */
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCEID_SET_ENABLE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_ID_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ACTION_STOP_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_BRIDGE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_INGRESS_PIPE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN_PRECEDENCE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_NESTED_VLAN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_EVID_CMD_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_VID1_CMD_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_VID1_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_QOS_PRECEDENCE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_QOSPROFILE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(7),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_DSCP_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_UP_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_KEEP_PREVIOUS_QOS_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_UP_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_DSCP_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_EXP_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MAP_DSCP_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP0_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP1_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP1_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PACKET_TYPE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_COPY_TTL_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TUNNEL_TERMINATION_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_MPLS_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_HASH_MASK_INDEX_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_QOS_MAPPING_TABLE_INDEX_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_L_LSP_QOS_PROFILE_ENABLE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TTL_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_DEC_TTL_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT_ASSIGNMENT_ENABLE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(14),/* in sip 5 was 13 */
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TT_HEADER_LENGTH_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(6), /* in sip 5 was 5 */
/*,        LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/*If <Is PTP PAcket> = 0_E*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_TIMESTAMP_ENABLE_E*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET_E},
/*,           LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_OFFSET_INDEX_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(7),
/*        LION3_TTI_ACTION_TABLE_FIELDS_    RESERVED_E*/
/*else If <Is PTP PAcket> =1_E*/
/*,           LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_TRIGGER_TYPE_E*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         2,
         LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET_E},
/*,           LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_OFFSET_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(6),/*was 7 in sip 5_E*/

/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_OAM_PROCESSING_WHEN_GAL_OR_OAL_EXISTS_E*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,/*was 0 in sip 5_E*/
     LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_OFFSET_E},
/*,        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROCESSING_ENABLE_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_CW_BASED_PSEUDO_WIRE_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_TTL_EXPIRY_VCCV_ENABLE_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PWE3_FLOW_LABEL_EXIST_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PW_CW_BASED_E_TREE_ENABLE_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_UP_CFI_TO_QOS_TABLE_SELECT_MODE_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_RX_IS_PROTECTION_PATH_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_RX_ENABLE_PROTECTION_SWITCHING_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_SET_MAC2ME_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROFILE_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_APPLY_NON_DATA_CW_COMMAND_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PW_TAG_MODE_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_NEXT_LOOKUP_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_CHANNEL_TYPE_TO_OPCODE_MAPPING_EN_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_ENABLE_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(2),
/*,        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_NON_MPLS_TRANSIT_TUNNEL_ENABLE_E*/
     PRV_CPSS_STANDARD_FIELD_MAC(1)
};


static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3L2iIngressVlanTableFieldsFormat[
    LION3_L2I_INGRESS_VLAN_TABLE_FIELDS___LAST_VALUE___E] =
{
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VALID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NEW_SRC_ADDR_IS_NOT_SECURITY_BREACH_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IP_MULTICAST_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_MULTICAST_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV6_MULTICAST_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKNOWN_UNICAST_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IGMP_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MIRROR_TO_INGRESS_ANALYZER_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_ICMP_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_CONTROL_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_BC_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IPV4_BC_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_UNICAST_ROUTE_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MULTICAST_ROUTE_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_UNICAST_ROUTE_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MULTICAST_ROUTE_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_SITEID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_AUTO_LEARN_DIS_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NA_MSG_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MRU_INDEX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_BC_UDP_TRAP_MIRROR_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_CONTROL_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_EVIDX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VRF_ID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UC_LOCAL_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_VIDX_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MC_BC_TO_MIRROR_ANLYZER_IDX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MC_TO_MIRROR_ANALYZER_IDX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKOWN_MAC_SA_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FCOE_FORWARDING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(16)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3L2iEPortTableFieldsFormat[
    LION3_L2I_EPORT_TABLE_FIELDS___LAST_VALUE___E] =
{
    /*LION3_L2I_EPORT_TABLE_FIELDS_NA_MSG_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_AUTO_LEARN_DIS_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_NA_STORM_PREV_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_NEW_SRC_ADDR_SECURITY_BREACH_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_VLAN_INGRESS_FILTERING_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_ACCEPT_FRAME_TYPE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_L2I_EPORT_TABLE_FIELDS_UC_LOCAL_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_UNKNOWN_SRC_ADDR_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_VLAN_IS_TRUNK_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_TRG_EPORT_TRUNK_NUM_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_TRG_DEV_E */
    PRV_CPSS_STANDARD_FIELD_MAC(10),
    /*LION3_L2I_EPORT_TABLE_FIELDS_ALL_PKT_TO_PVLAN_UPLINK_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_IGMP_TRAP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_ARP_BC_TRAP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_EN_LEARN_ON_TRAP_IEEE_RSRV_MC_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_IEEE_RSVD_MC_TABLE_SEL_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_SOURCE_ID_FORCE_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_SRC_ID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
    /*LION3_L2I_EPORT_TABLE_FIELDS_ARP_MAC_SA_MIS_DROP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_PORT_UNKNOWN_UC_FILTER_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_PORT_UNREG_MC_FILTER_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_PORT_BC_FILTER_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_EPORT_STP_STATE_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_EPORT_SPANNING_TREE_STATE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_L2I_EPORT_TABLE_FIELDS_IPV4_CONTROL_TRAP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_IPV6_CONTROL_TRAP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_BC_UDP_TRAP_OR_MIRROR_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_EN_LEARN_ORIG_TAG1_VID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_FDB_UC_IPV4_ROUTING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_FDB_UC_IPV6_ROUTING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_FDB_FCOE_ROUTING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_MOVED_MAC_SA_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3)

};


static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3FdbFdbTableFieldsFormat[SIP5_10_FDB_FDB_TABLE_FIELDS___LAST_VALUE___E] =
{
    /*LION3_FDB_FDB_TABLE_FIELDS_VALID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_SKIP_E  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_AGE_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_FDB_ENTRY_TYPE_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_FID_E              */
    ,PRV_CPSS_STANDARD_FIELD_MAC(13)

    /*    LION3_FDB_FDB_TABLE_FIELDS_MAC_ADDR_E     */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         48,
         LION3_FDB_FDB_TABLE_FIELDS_FID_E}
    /*    LION3_FDB_FDB_TABLE_FIELDS_DEV_ID_E       */
        ,PRV_CPSS_STANDARD_FIELD_MAC(10)
    /*    LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_5_0_E*/
        ,PRV_CPSS_STANDARD_FIELD_MAC(6)
    /*    LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E     */
        ,PRV_CPSS_STANDARD_FIELD_MAC(1)

    /*    LION3_FDB_FDB_TABLE_FIELDS_DIP_E          */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         32,
         LION3_FDB_FDB_TABLE_FIELDS_FID_E}
    /*    LION3_FDB_FDB_TABLE_FIELDS_SIP_E          */
        ,PRV_CPSS_STANDARD_FIELD_MAC(32)


    /*    LION3_FDB_FDB_TABLE_FIELDS_VIDX_E         */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_FDB_FDB_TABLE_FIELDS_SIP_E}

    /*        LION3_FDB_FDB_TABLE_FIELDS_TRUNK_NUM_E      */
            ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             12,
             LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E}

    /*        LION3_FDB_FDB_TABLE_FIELDS_EPORT_NUM_E      */
            ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             13,
             LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E}



    /*    LION3_FDB_FDB_TABLE_FIELDS_USER_DEFINED_E       */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         8,
         LION3_FDB_FDB_TABLE_FIELDS_VIDX_E}


    /*    LION3_FDB_FDB_TABLE_FIELDS_RESERVED_99_103_E    */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         5,
         LION3_FDB_FDB_TABLE_FIELDS_VIDX_E}

    /*    LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_11_9_E     */
        ,PRV_CPSS_STANDARD_FIELD_MAC(3)



    /*    LION3_FDB_FDB_TABLE_FIELDS_DA_ACCESS_LEVEL_E    */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         3,
         LION3_FDB_FDB_TABLE_FIELDS_USER_DEFINED_E}
    /*    LION3_FDB_FDB_TABLE_FIELDS_SA_ACCESS_LEVEL_E    */
        ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*    LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_8_6_E      */
        ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*    LION3_FDB_FDB_TABLE_FIELDS_RESERVED_116_118_E   */
        ,PRV_CPSS_STANDARD_FIELD_MAC(3)

    /*    LION3_FDB_FDB_TABLE_FIELDS_ORIG_VID1_E          */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_FDB_FDB_TABLE_FIELDS_USER_DEFINED_E}

    /*LION3_FDB_FDB_TABLE_FIELDS_IS_STATIC_E              */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_FDB_TABLE_FIELDS_ORIG_VID1_E}
    /*LION3_FDB_FDB_TABLE_FIELDS_MULTIPLE_E               */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_DA_CMD_E                 */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_SA_CMD_E                 */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_DA_ROUTE_E               */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_SP_UNKNOWN_E             */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_SA_QOS_PARAM_SET_IDX_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_DA_QOS_PARAM_SET_IDX_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_E  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_SA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)

    /*LION3_FDB_FDB_TABLE_FIELDS_VRF_ID_E              */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     12,
     LION3_FDB_FDB_TABLE_FIELDS_FDB_ENTRY_TYPE_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_SCOPE_CHECK_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_FCOE_D_ID_E*/
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     24,
     LION3_FDB_FDB_TABLE_FIELDS_VRF_ID_E}
    /*LION3_FDB_FDB_TABLE_FIELDS_IPV4_DIP_E              */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_FDB_FDB_TABLE_FIELDS_VRF_ID_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DST_SITE_ID_E */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_FDB_TABLE_FIELDS_IPV6_SCOPE_CHECK_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_FDB_TABLE_FIELDS_IPV4_DIP_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /* LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_INDEX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(7)
    /* LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_MODIFY_UP_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(2)
    /* LION3_FDB_FDB_TABLE_FIELDS_MODIFY_DSCP_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(2)
    /* LION3_FDB_FDB_TABLE_FIELDS_COUNTER_SET_INDEX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /* LION3_FDB_FDB_TABLE_FIELDS_ARP_BC_TRAP_MIRROR_EN_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_DIP_ACCESS_LEVEL_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(6)
    /* LION3_FDB_FDB_TABLE_FIELDS_ICMP_REDIRECT_EXCEP_MIRROR_EN_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_MTU_INDEX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /* LION3_FDB_FDB_TABLE_FIELDS_USE_VIDX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_TARGET_DEVICE_E */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     10,
     LION3_FDB_FDB_TABLE_FIELDS_EPORT_NUM_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_NEXT_HOP_EVLAN_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(13)
    /* LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_START_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_TYPE_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_ARP_PTR_E*/
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     17,
     LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_START_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_PTR_E*/
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     15,
     LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_TYPE_E}

    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_0_E*/
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_FDB_FDB_TABLE_FIELDS_FDB_ENTRY_TYPE_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_1_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(32)
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_2_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(32)
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_3_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(32)
    /* LION3_FDB_FDB_TABLE_FIELDS_NH_DATA_BANK_NUM_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(4)

    /* SIP5_10_FDB_FDB_TABLE_FIELDS_UC_ROUTE_TS_IS_NAT_E */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_PTR_E}

};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3FdbAuMsgTableFieldsFormat[SIP5_10_FDB_AU_MSG_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_FDB_AU_MSG_TABLE_FIELDS_MESSAGE_ID_E                               */
PRV_CPSS_STANDARD_FIELD_MAC(1)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_MSG_TYPE_E                                 */
,PRV_CPSS_STANDARD_FIELD_MAC(3)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_FDB_ENTRY_TYPE_E                           */
,PRV_CPSS_STANDARD_FIELD_MAC(3)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_VALID_E                                    */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_SKIP_E                                     */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E                                      */
,PRV_CPSS_STANDARD_FIELD_MAC(1)


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_8_0_E                   */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     9,
     LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E}


/*     LION3_FDB_AU_MSG_TABLE_FIELDS_CHAIN_TOO_LONG_E                      */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E}

/*     LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_OFFSET_E                     */
,     PRV_CPSS_STANDARD_FIELD_MAC(5)
/*     LION3_FDB_AU_MSG_TABLE_FIELDS_ENTRY_FOUND_E                         */
,     PRV_CPSS_STANDARD_FIELD_MAC(1)


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_E                             */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     48,
     LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_8_0_E}

/*     LION3_FDB_AU_MSG_TABLE_FIELDS_DEV_ID_E                              */
,     PRV_CPSS_STANDARD_FIELD_MAC(10)
/*     LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_5_0_E                       */
,     PRV_CPSS_STANDARD_FIELD_MAC(6)


/*     LION3_FDB_AU_MSG_TABLE_FIELDS_DIP                                 */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_8_0_E}

/*     LION3_FDB_AU_MSG_TABLE_FIELDS_SIP_E                                 */
,     PRV_CPSS_STANDARD_FIELD_MAC(32)


/*LION3_FDB_AU_MSG_TABLE_FIELDS_FID_E                                      */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 13,
 LION3_FDB_AU_MSG_TABLE_FIELDS_SIP_E}


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_VIDX_E                                 */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     16,
     LION3_FDB_AU_MSG_TABLE_FIELDS_FID_E}


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_IS_TRUNK_E                             */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_AU_MSG_TABLE_FIELDS_FID_E}

/*        LION3_FDB_AU_MSG_TABLE_FIELDS_EPORT_NUM_E                        */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         13,
         LION3_FDB_AU_MSG_TABLE_FIELDS_IS_TRUNK_E}

/*        LION3_FDB_AU_MSG_TABLE_FIELDS_TRUNK_NUM_E                        */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_FDB_AU_MSG_TABLE_FIELDS_IS_TRUNK_E}


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_USER_DEFINED_E                         */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         8,
         LION3_FDB_AU_MSG_TABLE_FIELDS_VIDX_E}

/*    LION3_FDB_AU_MSG_TABLE_FIELDS_RESERVED_109_113_E                     */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         5,
         LION3_FDB_AU_MSG_TABLE_FIELDS_VIDX_E}

/*    LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_11_9_E                       */
,    PRV_CPSS_STANDARD_FIELD_MAC(3)


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_ORIG_VID1_E                            */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_FDB_AU_MSG_TABLE_FIELDS_USER_DEFINED_E}

/*    LION3_FDB_AU_MSG_TABLE_FIELDS_DA_ACCESS_LEVEL_E                      */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         3,
         LION3_FDB_AU_MSG_TABLE_FIELDS_USER_DEFINED_E}
/*    LION3_FDB_AU_MSG_TABLE_FIELDS_SA_ACCESS_LEVEL_E                      */
 ,   PRV_CPSS_STANDARD_FIELD_MAC(3)
/*    LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_8_6_E                        */
,    PRV_CPSS_STANDARD_FIELD_MAC(3)


/*LION3_FDB_AU_MSG_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_E                    */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_ORIG_VID1_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_SP_UNKNOWN_E                               */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_SEARCH_TYPE_E                              */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_20_9_E                      */
,PRV_CPSS_STANDARD_FIELD_MAC(12)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_MULTIPLE_E                                 */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_DA_ROUTE_E                                 */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_SA_QOS_PARAM_SET_IDX_E                     */
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_DA_QOS_PARAM_SET_IDX_E                     */
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_IS_STATIC_E                                */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_DA_CMD_E                                   */
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_SA_CMD_E                                   */
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_SA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E     */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E     */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_FDB_LOOKUP_KEY_MODE_E */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/* fields for the format of : MAC NA moved update Message */

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_UP0_E                             */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 3,
 LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_OFFSET_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_IS_MOVED_E                        */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_SEARCH_TYPE_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_IS_TRUNK_E                    */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/*    LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_EPORT_E                   */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     13,
     LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_IS_TRUNK_E}


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_TRUNK_NUM_E               */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     12,
     LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_IS_TRUNK_E}


/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_DEVICE_E                      */
,PRV_CPSS_EXPLICIT_FIELD_MAC(152, 10)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_SRC_ID_E                      */
,PRV_CPSS_STANDARD_FIELD_MAC(12)


    /* additional fields : for FUTURE support of 'NA move Entry' */

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVE_ENTRY_ENABLE_E                     */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVE_ENTRY_INDEX_E                      */
,PRV_CPSS_STANDARD_FIELD_MAC(19)

 /* fields for FDB Routing */

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MAC_ADDR_INDEX_E                 */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 21,
 LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E                 */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 12,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MAC_ADDR_INDEX_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_SCOPE_CHECK_E                      */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

 /* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_FCOE_D_ID_E*/
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 24,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV4_DIP_E              */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 32,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DST_SITE_ID_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_SCOPE_CHECK_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_DEC_TTL_OR_HOP_COUNT_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV4_DIP_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_INGRESS_MIRROR_TO_ANALYZER_INDEX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_MARKING_EN_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_INDEX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(7)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_PRECEDENCE_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MODIFY_UP_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(2)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MODIFY_DSCP_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(2)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_COUNTER_SET_INDEX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ARP_BC_TRAP_MIRROR_EN_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_DIP_ACCESS_LEVEL_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(6)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ICMP_REDIRECT_EXCEP_MIRROR_EN_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MTU_INDEX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_USE_VIDX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IS_TRUNK_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EVIDX_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 16,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_USE_VIDX_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TARGET_DEVICE_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 10,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IS_TRUNK_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TRUNK_NUM_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
12,
LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IS_TRUNK_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EPORT_NUM_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 13,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TARGET_DEVICE_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_NEXT_HOP_EVLAN_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 13,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EPORT_NUM_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_START_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_TYPE_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ARP_PTR_E*/
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 17,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_START_E}
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_PTR_E*/
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 15,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_TYPE_E}


/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DIP_0_E*/
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 32,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MAC_ADDR_INDEX_E}
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DIP_1_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(32)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DIP_2_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(32)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DIP_3_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(32)

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_NH_DATA_BANK_NUM_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(4)

/* SIP5_10_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TS_IS_NAT_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_PTR_E}

};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3EgfQagEgressEPortTableFieldsFormat[
    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS___LAST_VALUE___E] =
{
/*    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_DP_TO_CFI_REMAP_ENABLE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EGRESS_MIRROR_TO_ANALYZER_INDEX_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EPORT_TAG_STATE_MODE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EPORT_TAG_STATE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EVLAN_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_VID1_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP0_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP1_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EVLAN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_VID1_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP0_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP1_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_REMOVE_VLAN_TAG_1_IF_RX_WITHOUT_TAG_1_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1)

};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3EgfShtEgressEVlanTableFieldsFormat[
    LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS___LAST_VALUE___E]=
{
/*     LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_VALID_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_PORT_X_MEMBER_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(256)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20EgfShtEgressEVlanTableFieldsFormat[
    LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS___LAST_VALUE___E]=
{
/*     LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_VALID_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(0),
/*    ,LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_PORT_X_MEMBER_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(256)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20EgfShtEgressEVlanTableFieldsFormat_512Ports_mode[
    LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS___LAST_VALUE___E]=
{
/*     LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_VALID_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(0),
/*    ,LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_TABLE_FIELDS_PORT_X_MEMBER_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(512)
};


static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3EgfShtEgressEVlanAttributesTableFieldsFormat[
    LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS___LAST_VALUE___E]=
{
/*    ,LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_LOCAL_SWITHING_EN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_PORT_ISOLATION_VLAN_CMD_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*    ,LION3_EGRESS_EGF_SHT_EGRESS_EVLAN_ATTRIBUTES_TABLE_FIELDS_EGRESS_MIRROR_TO_ANALYZER_INDEX_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3EgfShtEgressEPortTableFieldsFormat[
    LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS___LAST_VALUE___E] =
{
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_FROM_CPU_FWD_RESTRICT_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_BRIDGED_FWD_RESTRICT_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_ROUTED_FWD_RESTRICT_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_IPMC_ROUTED_FILTER_EN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_EGRESS_EVLAN_FILTER_EN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_PORT_ISOLATION_MODE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_EPORT_STP_STATE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_STP_STATE_MODE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_ODD_ONLY_SRC_ID_FILTER_EN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_EGRESS_EPORT_VLAN_FILTER_EN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_EGRESS_EGF_SHT_EGRESS_EPORT_TABLE_FIELDS_MESH_ID_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(4)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3L2MllTableFieldsFormat[
    LION3_L2_MLL_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_L2_MLL_TABLE_FIELDS_LAST_0                                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_BIND_TO_MLL_COUNTER_0                     */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_L2_MLL_TABLE_FIELDS_MASK_BITMAP_0                             */
    PRV_CPSS_STANDARD_FIELD_MAC(15),
/*LION3_L2_MLL_TABLE_FIELDS_USE_VIDX_0                                */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*UseVIDX0 = "False"*/
    /*LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0                         */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_L2_MLL_TABLE_FIELDS_USE_VIDX_0_E},

        /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
        /*LION3_L2_MLL_TABLE_FIELDS_TRG_EPORT_0                               */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             13,
             LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

        /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "True")*/
        /*LION3_L2_MLL_TABLE_FIELDS_TRG_TRUNK_ID_0                             */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             12,
             LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

    /*UseVIDX0 = "True"*/
    /*LION3_L2_MLL_TABLE_FIELDS_VIDX_0                                    */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},


/*LION3_L2_MLL_TABLE_FIELDS_UNKNOWN_UC_FILTER_ENABLE_0                */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_L2_MLL_TABLE_FIELDS_TRG_EPORT_0_E},
/*LION3_L2_MLL_TABLE_FIELDS_UNREGISTERED_MC_FILTER_ENABLE_0           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_BC_FILTER_ENABLE_0                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
/*LION3_L2_MLL_TABLE_FIELDS_TRG_DEV_0                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(10),

/*LION3_L2_MLL_TABLE_FIELDS_ONE_PLUS_ONE_FILTERING_ENABLE_0           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_TTL_THRESHOLD_0                           */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_L2_MLL_TABLE_FIELDS_MESH_ID_0                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_L2_MLL_TABLE_FIELDS_MC_LOCAL_SWITCHING_ENABLE_0               */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_MAX_HOP_COUNT_ENABLE_0                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_MAX_OUTGOING_HOP_COUNT_0                  */
    PRV_CPSS_STANDARD_FIELD_MAC(6),
/*LION3_L2_MLL_TABLE_FIELDS_RESERVED_1*/
    PRV_CPSS_STANDARD_FIELD_MAC(0),

/*LION3_L2_MLL_TABLE_FIELDS_LAST_1                                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_BIND_TO_MLL_COUNTER_1                     */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_L2_MLL_TABLE_FIELDS_MASK_BITMAP_1                             */
    PRV_CPSS_STANDARD_FIELD_MAC(15),
/*LION3_L2_MLL_TABLE_FIELDS_USE_VIDX_1                                */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*UseVIDX0 = "False"*/
    /*LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1                         */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_L2_MLL_TABLE_FIELDS_USE_VIDX_1_E},

        /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
        /*LION3_L2_MLL_TABLE_FIELDS_TRG_EPORT_1                               */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             13,
             LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

        /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "True")*/
        /*LION3_L2_MLL_TABLE_FIELDS_TRG_TRUNK_ID_1                             */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             12,
             LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

    /*UseVIDX0 = "True"*/
    /*LION3_L2_MLL_TABLE_FIELDS_VIDX_1                                    */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},


/*LION3_L2_MLL_TABLE_FIELDS_UNKNOWN_UC_FILTER_ENABLE_1                */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_L2_MLL_TABLE_FIELDS_TRG_EPORT_1_E},
/*LION3_L2_MLL_TABLE_FIELDS_UNREGISTERED_MC_FILTER_ENABLE_1           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_BC_FILTER_ENABLE_1                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
/*LION3_L2_MLL_TABLE_FIELDS_TRG_DEV_1                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(10),

/*LION3_L2_MLL_TABLE_FIELDS_ONE_PLUS_ONE_FILTERING_ENABLE_1           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_TTL_THRESHOLD_1                           */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_L2_MLL_TABLE_FIELDS_MESH_ID_1                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_L2_MLL_TABLE_FIELDS_MC_LOCAL_SWITCHING_ENABLE_1               */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_MAX_HOP_COUNT_ENABLE_1                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_MAX_OUTGOING_HOP_COUNT_1                  */
    PRV_CPSS_STANDARD_FIELD_MAC(6),
/*LION3_L2_MLL_TABLE_FIELDS_RESERVED_2*/
    PRV_CPSS_STANDARD_FIELD_MAC(0),

/*LION3_L2_MLL_TABLE_FIELDS_ENTRY_SELECTOR                            */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_L2_NEXT_MLL_PTR                           */
    PRV_CPSS_STANDARD_FIELD_MAC(15)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3IpMllTableFieldsFormat[
    LION3_IP_MLL_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_IP_MLL_TABLE_FIELDS_LAST_0                                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_START_OF_TUNNEL_0                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_MLL_RPF_FAIL_CMD_0                          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_IP_MLL_TABLE_FIELDS_MLL_EVID_0                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*LION3_IP_MLL_TABLE_FIELDS_USE_VIDX_0                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*UseVIDX0 = "True"*/
    /*LION3_IP_MLL_TABLE_FIELDS_EVIDX_0                                     */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

    /*UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
    /*LION3_IP_MLL_TABLE_FIELDS_TRG_EPORT_0                                 */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         13,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

    /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "True")*/
    /*LION3_IP_MLL_TABLE_FIELDS_TRG_TRUNK_ID_0                              */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

    /*LION3_IP_MLL_TABLE_FIELDS_RESEREVED_2_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(0),/* not exists */

/*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
/*LION3_IP_MLL_TABLE_FIELDS_TRG_DEV_0                                   */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     10,
     LION3_IP_MLL_TABLE_FIELDS_EVIDX_0_E},

/*LION3_IP_MLL_TABLE_FIELDS_EXCLUDE_SRC_VLAN_0                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_TTL_THRESHOLD_0_OR_HOP_LIMIT_THRESHOLD_0    */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_IP_MLL_TABLE_FIELDS_TUNNEL_PTR_0                                */
    PRV_CPSS_STANDARD_FIELD_MAC(15),
/*LION3_IP_MLL_TABLE_FIELDS_TUNNEL_TYPE_0                               */
    PRV_CPSS_STANDARD_FIELD_MAC(1),


/*LION3_IP_MLL_TABLE_FIELDS_LAST_1                                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_START_OF_TUNNEL_1                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_MLL_RPF_FAIL_CMD_1                          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_IP_MLL_TABLE_FIELDS_MLL_EVID_1                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*LION3_IP_MLL_TABLE_FIELDS_USE_VIDX_1                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*UseVIDX0 = "True"*/
    /*LION3_IP_MLL_TABLE_FIELDS_EVIDX_1                                     */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

    /*UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
    /*LION3_IP_MLL_TABLE_FIELDS_TRG_EPORT_1                                 */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         13,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

    /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "True")*/
    /*LION3_IP_MLL_TABLE_FIELDS_TRG_TRUNK_ID_1                              */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

    /*LION3_IP_MLL_TABLE_FIELDS_RESEREVED_3_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(0),/* not exists */

/*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
/*LION3_IP_MLL_TABLE_FIELDS_TRG_DEV_1                                   */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     10,
     LION3_IP_MLL_TABLE_FIELDS_EVIDX_1_E},

/*LION3_IP_MLL_TABLE_FIELDS_EXCLUDE_SRC_VLAN_1                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_TTL_THRESHOLD_1_OR_HOP_LIMIT_THRESHOLD_1    */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_IP_MLL_TABLE_FIELDS_TUNNEL_PTR_1                                */
    PRV_CPSS_STANDARD_FIELD_MAC(15),
/*LION3_IP_MLL_TABLE_FIELDS_TUNNEL_TYPE_1                               */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_NEXT_MLL_PTR                                */
    PRV_CPSS_STANDARD_FIELD_MAC(16)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3HaPhysicalPort1TableFieldsFormat[
    LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_ROUTED_MAC_SA_MOD_EN                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_ROUTER_MAC_SA_ASSIGNMENT_MODE             */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_PER_UP0_KEEP_VLAN1_ENABLE                 */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_IP_TUNNEL_LENGTH_OFFSET_ENABLE            */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_LOOKUP_CONFIGURATION_MODE            */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_NON_TS_DATA_PORT_ENABLE              */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_TS_DATA_PORT_ENABLE                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_FROM_CPU_DATA_PACKETS_PORT_ENABLE    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_EPCL_FROM_CPU_CONTROL_PACKETS_PORT_ENABLE */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_PORT_GROUP                                */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_PHYSICAL_PORT_TABLE_1_FIELDS_PORT_LIST_BIT_VECTOR_OFFSET               */
    PRV_CPSS_STANDARD_FIELD_MAC(5)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3HaPhysicalPort2TableFieldsFormat[
    LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_CASCADE_PORT_ENABLE                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_EGRESS_DSA_TAG_TYPE                          */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_SER_CHECK_ENABLE                             */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_DISABLE_CRC_ADDING                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_RETAIN_EXTERNAL_CRC                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_PACKET_ID_ENABLE                             */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_ROUTED_SRC_DEVICE_ID_PORT_MODE_ENABLE_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_SET_SOURCE_TO_LOCAL                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_LOCAL_DEVICE_NUMBER                          */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_LOCAL_SOURCE_ID                              */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_EPCL_TO_ANALYZER_PACKETS_PORT_ENABLE         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_EPCL_TO_CPU_PACKETS_PORT_ENABLE              */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_PTP_TIMESTAMP_TAG_MODE                       */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_INVALID_CRC_MODE                             */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_TIMESTAMP_RECEPTION_EN                       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_TRUNK_ID                                     */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_FORCE_NEW_DSA_TO_CPU                         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_FORCE_NEW_DSA_FORWARD_OR_FROM_CPU            */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_PRE_PEND_TWO_BYTES_HEADER_EN                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_DSA_QOS_MODE                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_REMOTE_PHYSICAL_PORT_MAP_ENABLE              */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_REMOTE_PHYSICAL_PORT_NUMBER                  */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_REMOTE_PHYSICAL_PORT_DEVICE_ID               */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/* LION3_HA_PHYSICAL_PORT_TABLE_2_FIELDS_INSERT_HASH_INTO_FORWARD_DSA_ENABLE_E */
    PRV_CPSS_EXPLICIT_FIELD_MAC(80,1)
};


static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3HaEPort1TableFieldsFormat[
    LION3_HA_EPORT_TABLE_1_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_EPORT_TABLE_1_FIELDS_VLAN_TRANS_EN                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG0_TPID_INDEX          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG1_TPID_INDEX          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX            */
    PRV_CPSS_STANDARD_FIELD_MAC(3),

    /* TunnelType != "MPLS" */

    /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_EXT                          */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E},

    /* TunnelType == "MPLS" */

    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_PUSH_ENABLE       */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E},
    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL                   */
    PRV_CPSS_STANDARD_FIELD_MAC(20),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_EXP               */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_TTL               */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_PUSH_MPLS_FLOW_LABEL_ENABLE     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_FORCE_ARP_TS_EPORT_DECISION     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*TunnelStart = "1"*/
        /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER                  */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         18,
         LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E},

    /*TunnelStart = "0"*/
        /*LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER                 */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         20,
         LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E},

    /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_PASSENGER_TYPE               */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER_E},

    /*LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_SA                   */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER_E},

/*LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_DA                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_TRILL_INTERFACE_ENABLE          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG0_TPID         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG1_TPID         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_UP_MAPPING        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_DSCP_MAPPING      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_EXP_MAPPING       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_TC_DP_MAPPING     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_QOS_MAPPING_TABLE_INDEX         */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ROUTER_MAC_SA_INDEX             */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_HA_EPORT_TABLE_1_FIELDS_DP_TO_CFI_MAP_ENABLE_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3HaEPort2TableFieldsFormat[
    LION3_HA_EPORT_TABLE_2_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_EPORT_TABLE_2_MIRROR_TO_ANALYZER_KEEP_TAGS   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_2_TO_ANALYZER_VLAN_TAG_ADD_EN    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_2_PUSH_VLAN_COMMAND              */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_2_PUSHED_TAG_TPID_SELECT         */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_2_PUSHED_TAG_VALUE               */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*LION3_HA_EPORT_TABLE_2_UP_CFI_ASSIGNMENT_COMMAND      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_2_UP                             */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_2_CFI                            */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_2_FORCE_E_TAG_IE_PID_TO_DEFAULT  */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3HaTunnelStartTableFieldsFormat[
    LION3_HA_TUNNEL_START_TABLE_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_TUNNEL_TYPE           */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_UP_MARKING_MODE       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_UP                    */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_TAG_ENABLE            */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_VID                   */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_TTL                   */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_RETAIN_INNER_CRC      */
    /*TunnelType = "MPLS" or TunnelType = "MacInMac" or TunnelType = "CAPWAP"*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED              */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_KEY_MODE          */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         2,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED_E},

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_PROTOCOL              */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         2,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED_E},


/*LION3_HA_TUNNEL_START_TABLE_FIELDS_NEXT_HOP_MAC_DA       */
    PRV_CPSS_STANDARD_FIELD_MAC(48),


    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_PROTOCOL          */
        /*TunnelType = "IPv4"*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_NEXT_HOP_MAC_DA_E},

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED_1            */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_NEXT_HOP_MAC_DA_E},

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_IPV4_GRE_KEY_1*/
        /*TunnelType = "IPv4"*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_NEXT_HOP_MAC_DA_E},

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_DST_PORT*/
        /*TunnelType = "CAPWAP"*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_NEXT_HOP_MAC_DA_E},


/*LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP                  */
    /*TunnelType = "IPv4" or TunnelType = "CAPWAP"*/
    PRV_CPSS_STANDARD_FIELD_MAC(6),
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_MARKING_MODE     */
    /*TunnelType = "IPv4" or TunnelType = "CAPWAP"*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_TUNNEL_START_TABLE_FIELDS_DF_FLAG               */
    /*TunnelType = "IPv4" or TunnelType = "CAPWAP"*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED_2            */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         24,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_DF_FLAG_E},

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_RID*/
        /*TunnelType = "CAPWAP"*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         5,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_DF_FLAG_E},
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_CAPWAP_FLAGS*/
        /*TunnelType = "CAPWAP"*/
        PRV_CPSS_STANDARD_FIELD_MAC(3),

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_AUTO_TUNNEL_OFFSET    */
        /*TunnelType = "IPv4"*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         4,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_DF_FLAG_E},
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_AUTO_TUNNEL_FLAG      */
        PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_IPV4_GRE_ENABLE*/
        /*TunnelType = "IPv4"*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_IPV4_GRE_KEY_0*/
        /*TunnelType = "IPv4"*/
        PRV_CPSS_STANDARD_FIELD_MAC(16),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED_4*/
        /*TunnelType = "IPv4"*/
        PRV_CPSS_STANDARD_FIELD_MAC(2),

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_TRILL_HEADER*/
        /*TunnelType = "TRILL"*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         48,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_DST_PORT_E},

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_LABEL1      */
        /*TunnelType = "MPLS"*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         20,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_DST_PORT_E},
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP1*/
        /*TunnelType = "MPLS"*/
        PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP1_MARKING_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_NUMBER_OF_MPLS_LABELS */
        PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED_3*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_SET_S_BIT */
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_SWAP_TTL_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_MPLS_MC_UPSTREAM_ASSIGNED_LABEL
     (MPLS Ethertype Select)*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /* LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED_7_E*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_LABEL2*/
        PRV_CPSS_STANDARD_FIELD_MAC(20),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP2*/
        PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP2_MARKING_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_PW_CONTROL_INDEX*/
        PRV_CPSS_STANDARD_FIELD_MAC(4),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED_5*/
        PRV_CPSS_STANDARD_FIELD_MAC(4),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_LABEL3*/
        PRV_CPSS_STANDARD_FIELD_MAC(20),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP3*/
        PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_EXP3_MARKING_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),


    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_I_SID       */
        /*TunnelType = "MacInMac"*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         24,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_DST_PORT_E},
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_I_UP_MARKING_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_RESERVED_6*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_I_UP*/
        PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_I_DEI*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_I_DEI_MARKING_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_I_SID_ASSIGN_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_I_TAG_RES2*/
        PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_I_TAG_RES1*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_I_TAG_NCA*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_B_DA_ASSIGN_MODE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_PROFILE_NUMBER_E*/
        /*TunnelType = Generic IPv4/6*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         3,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_AUTO_TUNNEL_FLAG_E},
        /*LION3_HA_TUNNEL_START_TABLE_FIELDS_UDP_SRC_PORT_E        */
        /*TunnelType = Generic IPv4/6*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_PROFILE_NUMBER_E},

    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_GRE_FLAGS_AND_VER_E*/
        /*TunnelType = "generic IPv4/6"*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_PROFILE_NUMBER_E},
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_DIP_E                   */
        PRV_CPSS_STANDARD_FIELD_MAC(32),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_SIP_E                   */
        PRV_CPSS_STANDARD_FIELD_MAC(32),
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_FLOW_LABEL_MODE_E       */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_DSCP_MARKING_MODE_E},

    /* Bobcat2 B0 */
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_MPLS_PW_EXP_MARKING_MODE_E*/
        {80,/*startBit*/
         1,
         0},

    /* BobK */
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_1_E */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_MPLS_PW_EXP_MARKING_MODE_E},
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_2_E */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_1_E},
    /*LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_3_E */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_TUNNEL_START_TABLE_FIELDS_PUSH_ELI_AND_EL_AFTER_LABEL_2_E}
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3HaGenTunnelStartProfileTableFieldsFormat[
    LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE0_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE1_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE2_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE3_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE4_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE5_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE6_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE7_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE8_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT0_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT1_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT2_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT3_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT4_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT5_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT6_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE9_BIT7_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT0_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT1_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT2_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT3_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT4_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT5_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT6_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE10_BIT7_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT0_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT1_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT2_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT3_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT4_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT5_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT6_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE11_BIT7_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT0_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT1_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT2_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT3_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT4_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT5_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT6_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE12_BIT7_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT0_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT1_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT2_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT3_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT4_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT5_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT6_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE13_BIT7_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT0_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT1_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT2_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT3_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT4_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT5_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT6_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE14_BIT7_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT0_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT1_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT2_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT3_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT4_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT5_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT6_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_BYTE15_BIT7_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_HASH_CIRCULAR_SHIFT_LEFT_E  */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_TEMPLATE_SIZE_E             */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_UDP_SRC_PORT_MODE_E         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_SERVICE_ID_E                */
    PRV_CPSS_STANDARD_FIELD_MAC(5),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_MAC_DA_MODE_E               */
    PRV_CPSS_STANDARD_FIELD_MAC(5),
/*LION3_HA_GEN_TUNNEL_START_PROFILE_TABLE_FIELDS_PROFILE_DIP_MODE_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(5)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3HaNat44TableFieldsFormat[
    LION3_HA_NAT44_TABLE_FIELDS____LAST_VALUE___E] =
{
/* bobcat B0 */

/*LION3_HA_NAT44_TABLE_FIELDS_MAC_DA_E           */
    PRV_CPSS_STANDARD_FIELD_MAC(48),
/*LION3_HA_NAT44_TABLE_FIELDS_MODIFY_DIP_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_NAT44_TABLE_FIELDS_NEW_DIP_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(32),
/*LION3_HA_NAT44_TABLE_FIELDS_MODIFY_SIP_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_NAT44_TABLE_FIELDS_NEW_SIP_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(32),
/*LION3_HA_NAT44_TABLE_FIELDS_MODIFY_TCP_UDP_DST_PORT_E     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_NAT44_TABLE_FIELDS_NEW_TCP_UDP_DST_PORT_E        */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/*LION3_HA_NAT44_TABLE_FIELDS_MODIFY_TCP_UDP_SRC_PORT_E     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_HA_NAT44_TABLE_FIELDS_NEW_TCP_UDP_SRC_PORT_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/*LION3_HA_NAT44_TABLE_FIELDS_NAT_ENTRY_TYPE_E              */
    {190,2,0/*NA*/} /*bit 190-191, fixed startBit position  */

};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3HaNat66TableFieldsFormat[
    LION3_HA_NAT66_TABLE_FIELDS____LAST_VALUE___E] =
{
/* bobk */

/*LION3_HA_NAT66_TABLE_FIELDS_MAC_DA_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(48),
/*LION3_HA_NAT66_TABLE_FIELDS_MODIFY_COMMAND_E                  */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_HA_NAT66_TABLE_FIELDS_ADDRESS_E                         */
    PRV_CPSS_STANDARD_FIELD_MAC(128),
/*LION3_HA_NAT66_TABLE_FIELDS_PREFIX_SIZE_E                     */
    PRV_CPSS_STANDARD_FIELD_MAC(6),
/*LION3_HA_NAT66_TABLE_FIELDS_NAT_ENTRY_TYPE_E                  */
   {190,2,0/*NA*/} /*bit 190-191, fixed startBit position       */

};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3IpclActionTableFieldsFormat[
    LION3_IPCL_ACTION_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_IPCL_ACTION_TABLE_FIELDS_COMMAND                               */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_IPCL_ACTION_TABLE_FIELDS_CPU_CODE                              */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_IPCL_ACTION_TABLE_FIELDS_MIRROR_TO_ANALYZER_PORT_0             */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_REDIRECT_COMMAND                      */
    PRV_CPSS_STANDARD_FIELD_MAC(3),

    /*If <Redirect Command> = No Redirect (0)*/
    /*LION3_IPCL_ACTION_TABLE_FIELDS_RESERVED_46_15                        */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         32,
         LION3_IPCL_ACTION_TABLE_FIELDS_REDIRECT_COMMAND_E},
    /*LION3_IPCL_ACTION_TABLE_FIELDS_POLICY1_LOOKUP_MODE                   */
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_IPCL_ACTION_TABLE_FIELDS_POLICY2_LOOKUP_MODE                   */
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_IPCL_ACTION_TABLE_FIELDS_IPCL_PROFILE_INDEX                    */
        PRV_CPSS_STANDARD_FIELD_MAC(13),

    /* If <Redirect Command> = 1 (Redirect to egress interface)*/
    /*LION3_IPCL_ACTION_TABLE_FIELDS_RESERVED_17_15                        */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         3,
         LION3_IPCL_ACTION_TABLE_FIELDS_REDIRECT_COMMAND_E},

         /*<UseVIDX> = 1*/
        /*LION3_IPCL_ACTION_TABLE_FIELDS_VIDX                                  */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_IPCL_ACTION_TABLE_FIELDS_RESERVED_17_15_E},

         /*if <UseVIDX> = 0*/
        /*LION3_IPCL_ACTION_TABLE_FIELDS_IS_TRUNK                              */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPCL_ACTION_TABLE_FIELDS_RESERVED_17_15_E},

            /*if <UseVIDX> = 0 & <IsTrunk> = 0*/
            /*LION3_IPCL_ACTION_TABLE_FIELDS_TRG_PORT                              */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             13,
             LION3_IPCL_ACTION_TABLE_FIELDS_IS_TRUNK_E},

            /*LION3_IPCL_ACTION_TABLE_FIELDS_TARGET_DEVICE                         */
            PRV_CPSS_STANDARD_FIELD_MAC(10),

            /*if <UseVIDX> = 0 & <IsTrunk> = 1*/
            /*LION3_IPCL_ACTION_TABLE_FIELDS_TRUNK_ID                              */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             12,
             LION3_IPCL_ACTION_TABLE_FIELDS_IS_TRUNK_E},


        /*LION3_IPCL_ACTION_TABLE_FIELDS_USE_VIDX                              */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             1,
             LION3_IPCL_ACTION_TABLE_FIELDS_TARGET_DEVICE_E},

        /*LION3_IPCL_ACTION_TABLE_FIELDS_VNT_L2_ECHO                           */
            PRV_CPSS_STANDARD_FIELD_MAC(1),
        /*LION3_IPCL_ACTION_TABLE_FIELDS_TUNNEL_START                          */
            PRV_CPSS_STANDARD_FIELD_MAC(1),

            /*if <Tunnel Start> == 0*/
            /*LION3_IPCL_ACTION_TABLE_FIELDS_ARP_POINTER                           */
                {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                 17,
                 LION3_IPCL_ACTION_TABLE_FIELDS_TUNNEL_START_E},

            /*if <Tunnel Start> == 1*/
            /*LION3_IPCL_ACTION_TABLE_FIELDS_TUNNEL_POINTER                        */
                {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                 15,
                 LION3_IPCL_ACTION_TABLE_FIELDS_TUNNEL_START_E},
            /*LION3_IPCL_ACTION_TABLE_FIELDS_TUNNEL_START_PASSENGER_TYPE           */
                PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*If <Redirect Command> = 2 (Redirect to NHE)*/
    /*LION3_IPCL_ACTION_TABLE_FIELDS_IP_NEXT_HOP_ENTRY_INDEX               */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_IPCL_ACTION_TABLE_FIELDS_REDIRECT_COMMAND_E},

    /*if <Redirect Command> = 4 (Assign VRF_ID)*/
    /*LION3_IPCL_ACTION_TABLE_FIELDS_VRF_ID                                */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_IPCL_ACTION_TABLE_FIELDS_REDIRECT_COMMAND_E},


    /* if (<Redirect Command> = 6 - Replace MAC SA)*/
    /*LION3_IPCL_ACTION_TABLE_FIELDS_MAC_SA_29_0                           */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         30,
         LION3_IPCL_ACTION_TABLE_FIELDS_REDIRECT_COMMAND_E},

/*LION3_IPCL_ACTION_TABLE_FIELDS_BIND_TO_CNC_COUNTER                   */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_IPCL_ACTION_TABLE_FIELDS_ARP_POINTER_E},
/*LION3_IPCL_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX                      */
    PRV_CPSS_STANDARD_FIELD_MAC(14),

    /* if (<Redirect Command> = 6 - Replace MAC SA)*/
    /*LION3_IPCL_ACTION_TABLE_FIELDS_MAC_SA_47_30                          */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         18,
         LION3_IPCL_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX_E},


     /*if (<Redirect Command> != 6 - Replace MAC SA)*/
    /*LION3_IPCL_ACTION_TABLE_FIELDS_ACTIVATE_METER                        */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPCL_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX_E},
    /*LION3_IPCL_ACTION_TABLE_FIELDS_BIND_TO_POLICER_COUNTER               */
        PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_IPCL_ACTION_TABLE_FIELDS_POLICER_PTR                           */
        PRV_CPSS_STANDARD_FIELD_MAC(16),

/*LION3_IPCL_ACTION_TABLE_FIELDS_SOURCE_ID_SET_ENABLE                  */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_IPCL_ACTION_TABLE_FIELDS_MAC_SA_47_30_E},
/*LION3_IPCL_ACTION_TABLE_FIELDS_SOURCE_ID                             */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*LION3_IPCL_ACTION_TABLE_FIELDS_ACTION_STOP                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_BRIDGE_BYPASS                         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_BYPASS_INGRESS_PIPE                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_VLAN_PRECEDENCE                       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_EN_NESTED_VLAN                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_VLAN_COMMAND                          */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_IPCL_ACTION_TABLE_FIELDS_VID0                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*LION3_IPCL_ACTION_TABLE_FIELDS_QOS_PROFILE_MARKING_ENABLE            */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_QOS_PRECEDENCE                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_QOS_PROFILE                           */
    PRV_CPSS_STANDARD_FIELD_MAC(10),
/*LION3_IPCL_ACTION_TABLE_FIELDS_MODIFY_DSCP                           */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_IPCL_ACTION_TABLE_FIELDS_MODIFY_UP                             */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_IPCL_ACTION_TABLE_FIELDS_ENABLE_MIRROR_TCP_RST_OR_FIN          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_MIRROR_TO_ANALYZER_PORT_2_1           */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_IPCL_ACTION_TABLE_FIELDS_MODIFY_MAC_DA                         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*If <Redirect Command> = 1 (Redirect to egress interface)*/
    /*LION3_IPCL_ACTION_TABLE_FIELDS_MODIFY_MAC_SA                         */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPCL_ACTION_TABLE_FIELDS_MODIFY_MAC_DA_E},

    /* If <Redirect Command> != 1 (Redirect to egress interface)*/
    /*LION3_IPCL_ACTION_TABLE_FIELDS_SET_EGRESS_FILTER_REGISTERED          */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPCL_ACTION_TABLE_FIELDS_MODIFY_MAC_DA_E},

/*LION3_IPCL_ACTION_TABLE_FIELDS_VLAN1_CMD                             */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     2,
     LION3_IPCL_ACTION_TABLE_FIELDS_SET_EGRESS_FILTER_REGISTERED_E},

/*LION3_IPCL_ACTION_TABLE_FIELDS_UP1_CMD                               */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_IPCL_ACTION_TABLE_FIELDS_VID1                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*LION3_IPCL_ACTION_TABLE_FIELDS_UP1                                   */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_IPCL_ACTION_TABLE_FIELDS_FLOW_ID                               */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*LION3_IPCL_ACTION_TABLE_FIELDS_IPCL_RESERVED_EN                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_IPCL_TO_TXQ_RESERVED                  */
    PRV_CPSS_STANDARD_FIELD_MAC(20),
/*LION3_IPCL_ACTION_TABLE_FIELDS_SET_MAC2ME                            */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_TIMESTAMP_EN                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_OFFSET_INDEX                          */
    PRV_CPSS_STANDARD_FIELD_MAC(7),
/*LION3_IPCL_ACTION_TABLE_FIELDS_OAM_PROCESSING_EN                     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_OAM_PROFILE                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_ASSIGN_SRC_EPORT_EN                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IPCL_ACTION_TABLE_FIELDS_SRC_EPORT                             */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*LION3_IPCL_ACTION_TABLE_FIELDS_USER_AC_ENABLE                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3EpclActionTableFieldsFormat[
    LION3_EPCL_ACTION_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_EPCL_ACTION_TABLE_FIELDS_COMMAND_E                                   */
     PRV_CPSS_STANDARD_FIELD_MAC(3 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_DSCP_EXP_E                                  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(6 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_UP0_E                                       */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_MODIFY_DSCP_EXP_E                           */
    ,PRV_CPSS_STANDARD_FIELD_MAC(2 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_ENABLE_MODIFY_UP0_E                         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(2 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_BIND_TO_CNC_COUNTER_E                       */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX_E                         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(14)
/*LION3_EPCL_ACTION_TABLE_FIELDS_TAG1_VID_CMD_E                              */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_ENABLE_MODIFY_UP1_E                         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TAG1_VID_E                                  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(12)
/*LION3_EPCL_ACTION_TABLE_FIELDS_UP1_E                                       */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TAG0_VLAN_CMD_E                             */
    ,PRV_CPSS_STANDARD_FIELD_MAC(2 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TAG0_VID_E                                  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(12)
/*LION3_EPCL_ACTION_TABLE_FIELDS_BIND_TO_POLICER_COUNTER_E                   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_BIND_TO_POLICER_METER_E                     */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_POLICER_INDEX_E                             */
    ,PRV_CPSS_STANDARD_FIELD_MAC(13)
/*LION3_EPCL_ACTION_TABLE_FIELDS_RESERVED_E                                  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
/*LION3_EPCL_ACTION_TABLE_FIELDS_FLOW_ID_E                                   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(12)
/*LION3_EPCL_ACTION_TABLE_FIELDS_OAM_PROCESSING_ENABLE_E                     */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_OAM_PROFILE_E                               */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TIME_STAMP_ENABLE_E                         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TIME_STAMP_OFFSET_INDEX_E                   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(7 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_CHANNEL_TYPE_TO_OPCODE_MAPPING_EN_E         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_EPCL_RESERVED_ENABLE_E                      */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_EPCL_RESERVED_E                             */
    ,PRV_CPSS_STANDARD_FIELD_MAC(20)
/*LION3_EPCL_ACTION_TABLE_FIELDS_TM_QUEUE_ID_E                               */
    ,PRV_CPSS_STANDARD_FIELD_MAC(14)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3IpvxRouterNextHopTableFieldsFormat[
    LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COMMAND_E                           */
    PRV_CPSS_STANDARD_FIELD_MAC(3 ),
/*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1 ),
/*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_CPU_CODE_INDEX_E                    */
    PRV_CPSS_STANDARD_FIELD_MAC(2 ),
/*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E  */
    PRV_CPSS_STANDARD_FIELD_MAC(3 ),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E       */
        /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E       */
         /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_INDEX_E            */
        /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         7,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_DSCP_E                  */
        /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         2,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_INDEX_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E                    */
        /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         2,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_DSCP_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_E */
            /* Multicast entry when MC_Vlan_Check = "RPF_check" */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             13,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_MRST_INDEX_E  */
            /* Multicast entry when MC_Vlan_Check = "bidir_tree_check" */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             6,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_28_34_E           */
            /* Multicast entry when MC_Vlan_Check = "bidir_tree_check" */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             7,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_MRST_INDEX_E},

    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_35_47_E                */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(13),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_INGRESS_VLAN_CHECK_FAIL_CMD_E */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(3 ),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_RPF_FAIL_CMD_MODE_E   */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_52_E                   */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INTERNAL_MLL_PTR_E             */
        /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(16),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EXTERNAL_MLL_PTR_MSB_BITS_15_13_E */
        /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_72_74_E                */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_INGRESS_VLAN_CHECK_E  */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(2 ),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EXTERNAL_MLL_PTR_E             */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(13),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_RESERVED_90_E   */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_DEST_SITE_ID_E  */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_SCOPE_CHECK_EN_E */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_VID1_E */
        /* Unicast entry when VID1 unicast routing is enabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_21_E */
        /* Unicast entry when VID1 unicast routing is enabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_VID1_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_EVLAN_E */
        /* Unicast entry */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         13,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_21_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E */
        /* Unicast entry */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_EVLAN_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EVIDX_E */
            /* Unicast entry when use_vidx=1 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             16,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_52_59_E */
            /* Unicast entry when use_vidx=1 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             8,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EVIDX_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TARGET_IS_TRUNK_E */
            /* Unicast entry when use_vidx=0 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             1,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E},

            /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_EPORT_E */
                /* Unicast entry when use_vidx=0 and target_is_trunk=0 */
                {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                 13,
                 LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TARGET_IS_TRUNK_E},

            /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_DEV_E */
                /* Unicast entry when use_vidx=0 and target_is_trunk=0 */
                {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                 10,
                 LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_EPORT_E},

            /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_TRUNK_ID_E */
                /* Unicast entry when use_vidx=0 and target_is_trunk=1 */
                {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                 12,
                 LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TARGET_IS_TRUNK_E},

            /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_49_59_E */
                /* Unicast entry when use_vidx=0 and target_is_trunk=1 */
                {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                 11,
                 LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_TRUNK_ID_E},

    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DIP_ACCESS_LEVEL_E  */
         /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(6 ),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_SIP_ACCESS_LEVEL_E  */
         /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(6 ),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E  */
         /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ARP_PTR_E */
            /* Unicast entry when VID1 startOfTunnel=0 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             17,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TUNNEL_PTR_E */
            /* Unicast entry when VID1 startOfTunnel=1 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             15,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_88_E */
            /* Unicast entry when VID1 startOfTunnel=1 */
            PRV_CPSS_STANDARD_FIELD_MAC(1),

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_TS_IS_NAT_E */
            /* Unicast entry when VID1 startOfTunnel=1 */
            PRV_CPSS_STANDARD_FIELD_MAC(1),

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_TUNNEL_TYPE_E */
            /* Unicast entry */
            PRV_CPSS_STANDARD_FIELD_MAC(1),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_DEST_SITE_ID_E */
        /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_SCOPE_CHECK_EN_E */
        /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1),

/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COUNTER_SET_INDEX_E                */
    PRV_CPSS_STANDARD_FIELD_MAC(3 ),
/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MTU_INDEX_E                        */
    PRV_CPSS_STANDARD_FIELD_MAC(3 ),
/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ARP_BC_TRAP_MIRROR_EN_E            */
    PRV_CPSS_STANDARD_FIELD_MAC(1 ),
/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E         */
    PRV_CPSS_STANDARD_FIELD_MAC(1 ),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_101_E                 */
        /* Multicast entry */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_SIP_FILTER_EN_E                */
        /* Unicast entry */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E},

/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E         */
    PRV_CPSS_STANDARD_FIELD_MAC(1 ),

/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ICMP_REDIRECT_EXCEP_MIRROR_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(1 )
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3IpvxRouterEportTableFieldsFormat[
    LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS___LAST_VALUE___E] =
{
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_ROUTER_PER_EPORT_SIP_SA_CHECK_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_FCOE_FORWARDING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_IPV4_UC_ROUTING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_IPV4_MC_ROUTING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_IPV6_UC_ROUTING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_IPV6_MC_ROUTING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_URPF_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_DISABLE_SIP_LOOKUP */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChLion3PlrMeteringTableFieldsFormat[
    LION3_PLR_METERING_TABLE_FIELDS___LAST_VALUE___E]=
{
/*LION3_PLR_METERING_TABLE_FIELDS_LAST_UPDATE_TIME0_E                            */
    PRV_CPSS_STANDARD_FIELD_MAC(28),
/*LION3_PLR_METERING_TABLE_FIELDS_LAST_UPDATE_TIME1_E                            */
    PRV_CPSS_STANDARD_FIELD_MAC(28),
/*LION3_PLR_METERING_TABLE_FIELDS_WRAP_AROUND_INDICATOR0_E                       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_WRAP_AROUND_INDICATOR1_E                       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_BUCKET_SIZE0_E                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(32),
/*LION3_PLR_METERING_TABLE_FIELDS_BUCKET_SIZE1_E                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(32),
/*LION3_PLR_METERING_TABLE_FIELDS_RATE_TYPE0_E                                   */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_PLR_METERING_TABLE_FIELDS_RATE_TYPE1_E                                   */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_PLR_METERING_TABLE_FIELDS_RATE0_E                                        */
    PRV_CPSS_STANDARD_FIELD_MAC(17),
/*LION3_PLR_METERING_TABLE_FIELDS_RATE1_E                                        */
    PRV_CPSS_STANDARD_FIELD_MAC(17),
/*LION3_PLR_METERING_TABLE_FIELDS_MAX_BURST_SIZE0_E                              */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/*LION3_PLR_METERING_TABLE_FIELDS_MAX_BURST_SIZE1_E                              */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/*LION3_PLR_METERING_TABLE_FIELDS_COLOR_MODE_E                                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_POLICER_MODE_E                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_PLR_METERING_TABLE_FIELDS_MG_COUNTERS_SET_EN_E                           */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_PLR_METERING_TABLE_FIELDS_BILLING_PTR_E                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/*LION3_PLR_METERING_TABLE_FIELDS_BYTE_OR_PACKET_COUNTING_MODE_E                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_PACKET_SIZE_MODE_E                             */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_TUNNEL_TERMINATION_PACKET_SIZE_MODE_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_INCLUDE_LAYER1_OVERHEAD_E                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_DSA_TAG_COUNTING_MODE_E                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/* ingress only fields */

/*LION3_PLR_METERING_TABLE_FIELDS_INGRESS_RED_CMD_E                              */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     2,
     LION3_PLR_METERING_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E},
/*LION3_PLR_METERING_TABLE_FIELDS_INGRESS_YELLOW_CMD_E                           */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_PLR_METERING_TABLE_FIELDS_INGRESS_GREEN_CMD_E                            */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_PLR_METERING_TABLE_FIELDS_INGRESS_MODIFY_DSCP_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_PLR_METERING_TABLE_FIELDS_INGRESS_MODIFY_UP_E                            */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_PLR_METERING_TABLE_FIELDS_INGRESS_QOS_PROFILE_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(10),

/* egress only fields */

/*LION3_PLR_METERING_TABLE_FIELDS_EGRESS_REMARK_MODE_E                           */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_PLR_METERING_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E},
/*LION3_PLR_METERING_TABLE_FIELDS_EGRESS_DROP_RED_E                              */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_EXP_E                         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_DSCP_E                        */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_TC_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_UP_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_DP_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_YELLOW_ECN_MARKING_E                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /* sip5_15 only fields */
/*SIP5_15_PLR_METERING_TABLE_FIELDS_BUCKET_0_PRIORITY_E                */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     3,
     LION3_PLR_METERING_TABLE_FIELDS_MAX_BURST_SIZE1_E},
/*SIP5_15_PLR_METERING_TABLE_FIELDS_BUCKET_1_PRIORITY_E               */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*SIP5_15_PLR_METERING_TABLE_FIELDS_BUCKET_0_COLOR_E                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_TABLE_FIELDS_BUCKET_1_COLOR_E                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_TABLE_FIELDS_COUPLING_FLAG_E                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_TABLE_FIELDS_COUPLING_FLAG_0_E                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_TABLE_FIELDS_EIR_AND_CIR_MAX_INDEX_E           */
    PRV_CPSS_STANDARD_FIELD_MAC(7),

/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_MEF_10_3_ENV_SIZE_E___EMULATION_ONLY_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(0) /* <--- field for emulation only !!! */

};

/* new table in sip 5_15 that was not in sip5 */
static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_15PlrMeteringConfigurationTableFieldsFormat[
    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS___LAST_VALUE___E]=
{
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_COLOR_MODE_E                                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_POLICER_MODE_E                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_MG_COUNTERS_SET_EN_E                           */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_BILLING_PTR_E                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_BYTE_OR_PACKET_COUNTING_MODE_E                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_PACKET_SIZE_MODE_E                             */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TUNNEL_TERMINATION_PACKET_SIZE_MODE_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INCLUDE_LAYER1_OVERHEAD_E                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_DSA_TAG_COUNTING_MODE_E                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/* ingress only fields */

/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_RED_CMD_E                              */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     2,
     SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E},
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_YELLOW_CMD_E                           */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_GREEN_CMD_E                            */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_MODIFY_DSCP_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_MODIFY_UP_E                            */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_QOS_PROFILE_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(10),

/* egress only fields */

/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_REMARK_MODE_E                           */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E},
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_DROP_RED_E                              */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_EXP_E                         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_DSCP_E                        */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_TC_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_UP_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_DP_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_YELLOW_ECN_MARKING_E                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/* field common to ingress and to egress */

/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_MEF_10_3_ENV_SIZE_E                           */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     3,
     SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_QOS_PROFILE_E}
};

/* new table in sip 5_15 that was not in sip5 */
static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChPlrMeteringConformanceSignTableFieldsFormat[
    PRV_CPSS_DXCH_PLR_METERING_CONFORM_SIGN_TABLE_FIELDS___LAST_VALUE___E]=
{
    /*PRV_CPSS_DXCH_PLR_METERING_CONFORM_SIGN_TABLE_FIELDS_BUCKET0_SIGN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*PRV_CPSS_DXCH_PLR_METERING_CONFORM_SIGN_TABLE_FIELDS_BUCKET1_SIGN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

/* Number of bits in table pattern entry */
#define BOBCAT2_IPVX_ROUTER_EPORT_TABLE_PATTERN_BITS_CNS         8

/* Number of ports (patterns) per table entry */
#define BOBCAT2_IPVX_ROUTER_EPORT_TABLE_PATTERN_NUM_CNS         16

/* BOBCAT2 B0 TABLES - Start */
static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChBobcat2B0TtiPhysicalPortTableFieldsFormat[LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS___LAST_VALUE___E] =
{
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_DEFAULT_SOURCE_EPORT_NUMBER_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_IS_RING_CORE_PORT_E */
    PRV_CPSS_STANDARD_FIELD_MAC(0),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_IS_LOOPED_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_ENABLE_MRU_CHECK_ON_CASCADE_PORT_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_CC_LINE_CARD_PORTS_DEFAULT_EPORT_BASE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_CC_LINE_CARD_TRUNK_DEFAULT_EPORT_BASE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_CENTRALIZED_CHASSIS_PORT_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_TRUNK_ID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_QOS_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChBobcat2B0L2iIngressVlanTableFieldsFormat[
LION3_L2I_INGRESS_VLAN_TABLE_FIELDS___LAST_VALUE___E] =
{
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VALID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NEW_SRC_ADDR_IS_NOT_SECURITY_BREACH_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IP_MULTICAST_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_MULTICAST_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV6_MULTICAST_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKNOWN_UNICAST_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IGMP_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MIRROR_TO_INGRESS_ANALYZER_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_ICMP_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_CONTROL_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_IPM_BRIDGING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_IPM_BRIDGING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_IPV4_BC_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREGISTERED_NON_IPV4_BC_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_UNICAST_ROUTE_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MULTICAST_ROUTE_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_UNICAST_ROUTE_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MULTICAST_ROUTE_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_SITEID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_AUTO_LEARN_DIS_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_NA_MSG_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_MRU_INDEX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_BC_UDP_TRAP_MIRROR_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_CONTROL_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_EVIDX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_VRF_ID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UC_LOCAL_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FLOOD_VIDX_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV4_MC_BC_TO_MIRROR_ANLYZER_IDX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_IPV6_MC_TO_MIRROR_ANALYZER_IDX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNKOWN_MAC_SA_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FCOE_FORWARDING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_UNREG_IPM_EVIDX_E */
    PRV_CPSS_STANDARD_FIELD_MAC(16),
    /* LION3_L2I_INGRESS_VLAN_TABLE_FIELDS_FDB_LOOKUP_KEY_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChBobcat2B0EgfQagEgressEPortTableFieldsFormat[
    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS___LAST_VALUE___E] =
{
/*    LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_DP_TO_CFI_REMAP_ENABLE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EGRESS_MIRROR_TO_ANALYZER_INDEX_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EPORT_TAG_STATE_MODE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EPORT_TAG_STATE_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EVLAN_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_VID1_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP0_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP1_COMMAND_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_EVLAN_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_VID1_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP0_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_UP1_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*    ,LION3_EGRESS_EGF_QAG_EGRESS_EPORT_TABLE_FIELDS_REMOVE_VLAN_TAG_1_IF_RX_WITHOUT_TAG_1_E*/
    PRV_CPSS_STANDARD_FIELD_MAC(1)

};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChBobcat2B0HaEPort1TableFieldsFormat[
    LION3_HA_EPORT_TABLE_1_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_EPORT_TABLE_1_FIELDS_VLAN_TRANS_EN                   */
    PRV_CPSS_STANDARD_FIELD_MAC(2),/* was 1 bit in sip5*/
/*LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG0_TPID_INDEX          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG1_TPID_INDEX          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX            */
    PRV_CPSS_STANDARD_FIELD_MAC(3),

    /* TunnelType != "MPLS" */

    /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_EXT                          */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E},

    /* TunnelType == "MPLS" */

    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_PUSH_ENABLE       */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E},
    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL                   */
    PRV_CPSS_STANDARD_FIELD_MAC(20),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_EXP               */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_TTL               */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_PUSH_MPLS_FLOW_LABEL_ENABLE     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_FORCE_ARP_TS_EPORT_DECISION     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*TunnelStart = "1"*/
        /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER                  */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         15,/*was 18 in sip5*/
         LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E},

    /*TunnelStart = "0"*/
        /*LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER                 */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         17,/*was 20 in sip5*/
         LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E},

    /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_PASSENGER_TYPE               */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER_E},

    /*LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_SA                   */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER_E},

/*LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_DA                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_TRILL_INTERFACE_ENABLE          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG0_TPID         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG1_TPID         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_UP_MAPPING        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_DSCP_MAPPING      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_EXP_MAPPING       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_TC_DP_MAPPING     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_QOS_MAPPING_TABLE_INDEX         */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ROUTER_MAC_SA_INDEX             */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_HA_EPORT_TABLE_1_FIELDS_DP_TO_CFI_MAP_ENABLE_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChBobcat2B0HaEPort2TableFieldsFormat[
    LION3_HA_EPORT_TABLE_2_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_EPORT_TABLE_2_MIRROR_TO_ANALYZER_KEEP_TAGS   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_2_TO_ANALYZER_VLAN_TAG_ADD_EN    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_2_PUSH_VLAN_COMMAND              */
    PRV_CPSS_STANDARD_FIELD_MAC(2),/*was 1 bit in sip5 */
/*LION3_HA_EPORT_TABLE_2_PUSHED_TAG_TPID_SELECT         */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_2_PUSHED_TAG_VALUE               */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/*LION3_HA_EPORT_TABLE_2_UP_CFI_ASSIGNMENT_COMMAND      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_2_UP                             */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_2_CFI                            */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_2_FORCE_E_TAG_IE_PID_TO_DEFAULT  */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChBobKIpvxRouterEportTableFieldsFormat[
    LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS___LAST_VALUE___E] =
{
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_ROUTER_PER_EPORT_SIP_SA_CHECK_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_FCOE_FORWARDING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_IPV4_UC_ROUTING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_IPV4_MC_ROUTING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_IPV6_UC_ROUTING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_IPV6_MC_ROUTING_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_URPF_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(0),
/* LION3_IPVX_ROUTER_EPORT_TABLE_FIELDS_DISABLE_SIP_LOOKUP */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
};
/* Number of bits in table pattern entry */
#define BOBK_IPVX_ROUTER_EPORT_TABLE_PATTERN_BITS_CNS         7


/* BOBCAT2 B0 TABLES - End */

/* bobk tables - start */
static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChBobkHaEPort1TableFieldsFormat[
    LION3_HA_EPORT_TABLE_1_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_EPORT_TABLE_1_FIELDS_VLAN_TRANS_EN                   */
    PRV_CPSS_STANDARD_FIELD_MAC(2),/* was 1 bit in sip5*/
/*LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG0_TPID_INDEX          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG1_TPID_INDEX          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX            */
    PRV_CPSS_STANDARD_FIELD_MAC(3),

    /* TunnelType != "MPLS" */

    /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_EXT                          */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E},

    /* TunnelType == "MPLS" */

    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_PUSH_ENABLE       */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E},
    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL                   */
    PRV_CPSS_STANDARD_FIELD_MAC(20),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_EXP               */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_TTL               */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_PUSH_MPLS_FLOW_LABEL_ENABLE     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_FORCE_ARP_TS_EPORT_DECISION     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*TunnelStart = "1"*/
        /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER                  */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         14,/*was 15 in sip5_10*/
         LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E},

    /*TunnelStart = "0"*/
        /*LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER                 */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,/*was 17 in sip5_10*/
         LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E},

        /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_PASSENGER_TYPE               */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER_E},

/*LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_SA                   */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER_E},

/*LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_DA                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_TRILL_INTERFACE_ENABLE          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG0_TPID         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG1_TPID         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_UP_MAPPING        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_DSCP_MAPPING      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_EXP_MAPPING       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_TC_DP_MAPPING     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_QOS_MAPPING_TABLE_INDEX         */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ROUTER_MAC_SA_INDEX             */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_HA_EPORT_TABLE_1_FIELDS_DP_TO_CFI_MAP_ENABLE_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(1)

    /* add here new fields that were not in sip5_10 */

};
/* bobk tables - end */

/* bobcat3 tables - start */
static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20TtiPhysicalPortTableFieldsFormat[LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS___LAST_VALUE___E] =
{
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_DEFAULT_SOURCE_EPORT_NUMBER_E */
    PRV_CPSS_STANDARD_FIELD_MAC(14),  /*+1 from sip5_10*/
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_IS_RING_CORE_PORT_E */
    PRV_CPSS_STANDARD_FIELD_MAC(0),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_IS_LOOPED_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_ENABLE_MRU_CHECK_ON_CASCADE_PORT_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_CC_LINE_CARD_PORTS_DEFAULT_EPORT_BASE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(14),  /*+1 from sip5_10*/
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_CC_LINE_CARD_TRUNK_DEFAULT_EPORT_BASE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(14),  /*+1 from sip5_10*/
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_CENTRALIZED_CHASSIS_PORT_ENABLE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_TRUNK_ID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
/* LION3_TTI_PHYSICAL_PORT_TABLE_FIELDS_PORT_QOS_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};
static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20L2iEPortTableFieldsFormat[
    LION3_L2I_EPORT_TABLE_FIELDS___LAST_VALUE___E] =
{
    /*LION3_L2I_EPORT_TABLE_FIELDS_NA_MSG_TO_CPU_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_AUTO_LEARN_DIS_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_NA_STORM_PREV_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_NEW_SRC_ADDR_SECURITY_BREACH_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_VLAN_INGRESS_FILTERING_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_ACCEPT_FRAME_TYPE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_L2I_EPORT_TABLE_FIELDS_UC_LOCAL_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_UNKNOWN_SRC_ADDR_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_VLAN_IS_TRUNK_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_TRG_EPORT_TRUNK_NUM_E */
    PRV_CPSS_STANDARD_FIELD_MAC(14),/* was 13 in sip5 */
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_PVLAN_TRG_DEV_E */
    PRV_CPSS_STANDARD_FIELD_MAC(10),
    /*LION3_L2I_EPORT_TABLE_FIELDS_ALL_PKT_TO_PVLAN_UPLINK_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_IGMP_TRAP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_ARP_BC_TRAP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_EN_LEARN_ON_TRAP_IEEE_RSRV_MC_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_IEEE_RSVD_MC_TABLE_SEL_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_PORT_SOURCE_ID_FORCE_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_SRC_ID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(12),
    /*LION3_L2I_EPORT_TABLE_FIELDS_ARP_MAC_SA_MIS_DROP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_PORT_UNKNOWN_UC_FILTER_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_PORT_UNREG_MC_FILTER_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_PORT_BC_FILTER_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_EPORT_STP_STATE_MODE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_INGRESS_EPORT_SPANNING_TREE_STATE_E */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
    /*LION3_L2I_EPORT_TABLE_FIELDS_IPV4_CONTROL_TRAP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_IPV6_CONTROL_TRAP_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_BC_UDP_TRAP_OR_MIRROR_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_EN_LEARN_ORIG_TAG1_VID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_FDB_UC_IPV4_ROUTING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_FDB_UC_IPV6_ROUTING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_FDB_FCOE_ROUTING_EN_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*LION3_L2I_EPORT_TABLE_FIELDS_MOVED_MAC_SA_CMD_E */
    PRV_CPSS_STANDARD_FIELD_MAC(3)

};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20HaEPort1TableFieldsFormat[
    LION3_HA_EPORT_TABLE_1_FIELDS____LAST_VALUE___E] =
{
/*LION3_HA_EPORT_TABLE_1_FIELDS_VLAN_TRANS_EN_E                   */
    PRV_CPSS_STANDARD_FIELD_MAC(2),/* was 1 bit in sip5*/
/*LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG0_TPID_INDEX_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_1_FIELDS_EGRESS_TAG1_TPID_INDEX_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E            */
    PRV_CPSS_STANDARD_FIELD_MAC(3),

    /* TunnelType != "MPLS" */

        /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_EXT_E                          */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         32,
         LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E},

    /* TunnelType == "MPLS" */

        /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_PUSH_ENABLE_E       */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_EPORT_TABLE_1_FIELDS_TS_HEADER_TPID_INDEX_E},
        /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_E                   */
        PRV_CPSS_STANDARD_FIELD_MAC(20),
        /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_EXP_E               */
        PRV_CPSS_STANDARD_FIELD_MAC(3),
        /*LION3_HA_EPORT_TABLE_1_FIELDS_MPLS_PW_LABEL_TTL_E               */
        PRV_CPSS_STANDARD_FIELD_MAC(8),
        /*LION3_HA_EPORT_TABLE_1_FIELDS_PUSH_MPLS_FLOW_LABEL_ENABLE_E     */
        PRV_CPSS_STANDARD_FIELD_MAC(1),
        /*LION3_HA_EPORT_TABLE_1_FIELDS_FORCE_ARP_TS_EPORT_DECISION_E     */
        PRV_CPSS_STANDARD_FIELD_MAC(1),
        /*LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E                    */
        PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*TunnelStart = "1"*/
        /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER_E                      */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,/*was 14 in sip5_15*/
         LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E},

    /*TunnelStart = "0"*/
        /*LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER_E                     */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         18,/*was 16 in sip5_15*/
         LION3_HA_EPORT_TABLE_1_FIELDS_TUNNEL_START_E},

    /*LION3_HA_EPORT_TABLE_1_FIELDS_TS_PASSENGER_TYPE               */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_HA_EPORT_TABLE_1_FIELDS_TS_POINTER_E},

/*LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_SA_E                   */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_HA_EPORT_TABLE_1_FIELDS_ARP_POINTER_E},

/*LION3_HA_EPORT_TABLE_1_FIELDS_MODIFY_MAC_DA_E                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_TRILL_INTERFACE_ENABLE_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG0_TPID_E         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_KEEP_ORIGINAL_TAG1_TPID_E         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_UP_MAPPING_E        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_DSCP_MAPPING_E      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_EXP_MAPPING_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ENABLE_EGRESS_TC_DP_MAPPING_E     */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_HA_EPORT_TABLE_1_FIELDS_QOS_MAPPING_TABLE_INDEX_E         */
    PRV_CPSS_STANDARD_FIELD_MAC(4),
/*LION3_HA_EPORT_TABLE_1_FIELDS_ROUTER_MAC_SA_INDEX_E             */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_HA_EPORT_TABLE_1_FIELDS_DP_TO_CFI_MAP_ENABLE_E             */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20EpclActionTableFieldsFormat[
    LION3_EPCL_ACTION_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_EPCL_ACTION_TABLE_FIELDS_COMMAND_E                                   */
     PRV_CPSS_STANDARD_FIELD_MAC(3 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_DSCP_EXP_E                                  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(6 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_UP0_E                                       */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_MODIFY_DSCP_EXP_E                           */
    ,PRV_CPSS_STANDARD_FIELD_MAC(2 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_ENABLE_MODIFY_UP0_E                         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(2 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_BIND_TO_CNC_COUNTER_E                       */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX_E                         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(14)
/*LION3_EPCL_ACTION_TABLE_FIELDS_TAG1_VID_CMD_E                              */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_ENABLE_MODIFY_UP1_E                         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TAG1_VID_E                                  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(12)
/*LION3_EPCL_ACTION_TABLE_FIELDS_UP1_E                                       */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TAG0_VLAN_CMD_E                             */
    ,PRV_CPSS_STANDARD_FIELD_MAC(2 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TAG0_VID_E                                  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(12)
/*LION3_EPCL_ACTION_TABLE_FIELDS_BIND_TO_POLICER_COUNTER_E                   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_BIND_TO_POLICER_METER_E                     */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_POLICER_INDEX_E                             */
    ,PRV_CPSS_STANDARD_FIELD_MAC(14)/*increase by 1 */
/*LION3_EPCL_ACTION_TABLE_FIELDS_RESERVED_E                                  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(0) /*removed*/
/*LION3_EPCL_ACTION_TABLE_FIELDS_FLOW_ID_E                                   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(13)/*increase by 1*/
/*LION3_EPCL_ACTION_TABLE_FIELDS_OAM_PROCESSING_ENABLE_E                     */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_OAM_PROFILE_E                               */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TIME_STAMP_ENABLE_E                         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_TIME_STAMP_OFFSET_INDEX_E                   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(7 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_CHANNEL_TYPE_TO_OPCODE_MAPPING_EN_E         */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_EPCL_RESERVED_ENABLE_E                      */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1 )
/*LION3_EPCL_ACTION_TABLE_FIELDS_EPCL_RESERVED_E                             */
    ,PRV_CPSS_STANDARD_FIELD_MAC(20)
/*LION3_EPCL_ACTION_TABLE_FIELDS_TM_QUEUE_ID_E                               */
    ,PRV_CPSS_STANDARD_FIELD_MAC(0)/*field removed*/
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20EgfQagPortTargetAttributesTableFieldsFormat[
    SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS___LAST_VALUE___E] =
{
    /*SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_ETARGET_PHYSICAL_PORT_LOOPBACK_PROFILE_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(2)
    /*SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_ETARGET_PHYSICAL_PORT_TC_PROFILE_E             */
   ,PRV_CPSS_STANDARD_FIELD_MAC(2)
    /*SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_ETARGET_PHYSICAL_PORT_EVLAN_MIRRORING_ENABLE_E */
   ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_EUSE_VLAN_TAG_1_FOR_TAG_STATE_E                */
   ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_ETARGET_PHYSICAL_PORT_ENQ_PROFILE_E            */
   ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_ELOOPBACK_ENQ_PROFILE_E                        */
   ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*SIP5_20_EGF_QAG_PORT_TARGET_ATTRIBUTES_TABLE_FIELDS_ELOOPBACK_PORT_E                               */
   ,PRV_CPSS_STANDARD_FIELD_MAC(9)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20IpvxRouterNextHopTableFieldsFormat[
    LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COMMAND_E                           */
    PRV_CPSS_STANDARD_FIELD_MAC(3 ),
/*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1 ),
/*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_CPU_CODE_INDEX_E                    */
    PRV_CPSS_STANDARD_FIELD_MAC(2 ),
/*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E  */
    PRV_CPSS_STANDARD_FIELD_MAC(3 ),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E       */
        /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E       */
         /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_INDEX_E            */
        /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         7,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_DSCP_E                  */
        /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         2,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_QOS_PROFILE_INDEX_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E                    */
        /* Multicast entry or Unicast entry when VID1 unicast routing is disabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         2,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_DSCP_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_E */
            /* Multicast entry when MC_Vlan_Check = "RPF_check" */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             13,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_MRST_INDEX_E  */
            /* Multicast entry when MC_Vlan_Check = "bidir_tree_check" */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             6,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MODIFY_UP_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_28_34_E           */
            /* Multicast entry when MC_Vlan_Check = "bidir_tree_check" */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             7,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MC_RPF_EVLAN_MRST_INDEX_E},

    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_35_47_E                */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(13),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_INGRESS_VLAN_CHECK_FAIL_CMD_E */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(3 ),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_RPF_FAIL_CMD_MODE_E   */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_52_E                   */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INTERNAL_MLL_PTR_E             */
        /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(16),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EXTERNAL_MLL_PTR_MSB_BITS_15_13_E */
        /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_72_74_E                */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(3),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MULTICAST_INGRESS_VLAN_CHECK_E  */
         /* Multicast entry */
        PRV_CPSS_EXPLICIT_FIELD_MAC(75,2),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EXTERNAL_MLL_PTR_E             */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(13),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_RESERVED_90_E   */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(3 ),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_DEST_SITE_ID_E  */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),
    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_IPV6_MULTICAST_SCOPE_CHECK_EN_E */
         /* Multicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_VID1_E */
        /* Unicast entry when VID1 unicast routing is enabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_21_E */
        /* Unicast entry when VID1 unicast routing is enabled */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_VID1_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_EVLAN_E */
        /* Unicast entry */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         13,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_21_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E */
        /* Unicast entry */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_NEXT_HOP_EVLAN_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EVIDX_E */
            /* Unicast entry when use_vidx=1 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             16,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_52_59_E */
            /* Unicast entry when use_vidx=1 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             8,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_EVIDX_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TARGET_IS_TRUNK_E */
            /* Unicast entry when use_vidx=0 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             1,
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_USE_VIDX_E},

            /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_EPORT_E */
                /* Unicast entry when use_vidx=0 and target_is_trunk=0 */
                {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                 14,/* was 13 in sip 5_15 */
                 LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TARGET_IS_TRUNK_E},

            /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_DEV_E */
                /* Unicast entry when use_vidx=0 and target_is_trunk=0 */
                PRV_CPSS_STANDARD_FIELD_MAC(10 ),

            /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TRG_TRUNK_ID_E */
                /* Unicast entry when use_vidx=0 and target_is_trunk=1 */
                {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
                 12,
                 LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TARGET_IS_TRUNK_E},

            /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_49_59_E */
                /* Unicast entry when use_vidx=0 and target_is_trunk=1 */
                PRV_CPSS_STANDARD_FIELD_MAC(12 ),/*was11 in sip 5*/

    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DIP_ACCESS_LEVEL_E  */
         /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(6 ),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_SIP_ACCESS_LEVEL_E  */
         /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(6 ),
    /*LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E  */
         /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1 ),

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ARP_PTR_E */
            /* Unicast entry when VID1 startOfTunnel=0 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             18,/* was 17 in sip 5_15 */
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_TUNNEL_PTR_E */
            /* Unicast entry when VID1 startOfTunnel=1 */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             16,/* was 15 in sip 5_15 */
             LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_START_OF_TUNNEL_E},

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_88_E */
            /* Unicast entry when VID1 startOfTunnel=1 */
            PRV_CPSS_STANDARD_FIELD_MAC(1),

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_TS_IS_NAT_E */
            /* Unicast entry when VID1 startOfTunnel=1 */
            PRV_CPSS_STANDARD_FIELD_MAC(1),

        /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_TUNNEL_TYPE_E */
            /* Unicast entry */
            PRV_CPSS_STANDARD_FIELD_MAC(1),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_DEST_SITE_ID_E */
        /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_UNICAST_SCOPE_CHECK_EN_E */
        /* Unicast entry */
        PRV_CPSS_STANDARD_FIELD_MAC(1),

/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_COUNTER_SET_INDEX_E                */
    PRV_CPSS_STANDARD_FIELD_MAC(3 ),
/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_MTU_INDEX_E                        */
    PRV_CPSS_STANDARD_FIELD_MAC(3 ),
/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ARP_BC_TRAP_MIRROR_EN_E            */
    PRV_CPSS_STANDARD_FIELD_MAC(1 ),
/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E         */
    PRV_CPSS_STANDARD_FIELD_MAC(1 ),

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_RESERVED_101_E                 */
        /* Multicast entry */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E},

    /* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_SIP_FILTER_EN_E                */
        /* Unicast entry */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_EN_E},

/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E         */
    PRV_CPSS_STANDARD_FIELD_MAC(1 ),

/* LION3_IPVX_ROUTER_NEXTHOP_TABLE_FIELDS_ICMP_REDIRECT_EXCEP_MIRROR_E       */
    PRV_CPSS_STANDARD_FIELD_MAC(1 )
};


static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20L2MllTableFieldsFormat[
    LION3_L2_MLL_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_L2_MLL_TABLE_FIELDS_LAST_0                                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_BIND_TO_MLL_COUNTER_0                     */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_L2_MLL_TABLE_FIELDS_MASK_BITMAP_0                             */
    PRV_CPSS_STANDARD_FIELD_MAC(15),
/*LION3_L2_MLL_TABLE_FIELDS_USE_VIDX_0                                */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*UseVIDX0 = "False"*/
    /*LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0                         */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_L2_MLL_TABLE_FIELDS_USE_VIDX_0_E},

        /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
        /*LION3_L2_MLL_TABLE_FIELDS_TRG_EPORT_0                               */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             14,/*was 13 in sip5 */
             LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

        /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "True")*/
        /*LION3_L2_MLL_TABLE_FIELDS_TRG_TRUNK_ID_0                             */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             12,
             LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

    /*UseVIDX0 = "True"*/
    /*LION3_L2_MLL_TABLE_FIELDS_VIDX_0                                    */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},


/*LION3_L2_MLL_TABLE_FIELDS_UNKNOWN_UC_FILTER_ENABLE_0                */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_L2_MLL_TABLE_FIELDS_TRG_EPORT_0_E},
/*LION3_L2_MLL_TABLE_FIELDS_UNREGISTERED_MC_FILTER_ENABLE_0           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_BC_FILTER_ENABLE_0                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
/*LION3_L2_MLL_TABLE_FIELDS_TRG_DEV_0                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(10),

/*LION3_L2_MLL_TABLE_FIELDS_ONE_PLUS_ONE_FILTERING_ENABLE_0           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_TTL_THRESHOLD_0                           */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_L2_MLL_TABLE_FIELDS_MESH_ID_0                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_L2_MLL_TABLE_FIELDS_MC_LOCAL_SWITCHING_ENABLE_0               */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_MAX_HOP_COUNT_ENABLE_0                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_MAX_OUTGOING_HOP_COUNT_0                  */
    PRV_CPSS_STANDARD_FIELD_MAC(6),
/*LION3_L2_MLL_TABLE_FIELDS_RESERVED_1                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),


/*LION3_L2_MLL_TABLE_FIELDS_LAST_1                                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_BIND_TO_MLL_COUNTER_1                     */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*LION3_L2_MLL_TABLE_FIELDS_MASK_BITMAP_1                             */
    PRV_CPSS_STANDARD_FIELD_MAC(15),
/*LION3_L2_MLL_TABLE_FIELDS_USE_VIDX_1                                */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*UseVIDX0 = "False"*/
    /*LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1                         */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         LION3_L2_MLL_TABLE_FIELDS_USE_VIDX_1_E},

        /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
        /*LION3_L2_MLL_TABLE_FIELDS_TRG_EPORT_1                               */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             14,/*was 13 in sip5 */
             LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

        /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "True")*/
        /*LION3_L2_MLL_TABLE_FIELDS_TRG_TRUNK_ID_1                             */
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             12,
             LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

    /*UseVIDX0 = "True"*/
    /*LION3_L2_MLL_TABLE_FIELDS_VIDX_1                                    */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_L2_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},


/*LION3_L2_MLL_TABLE_FIELDS_UNKNOWN_UC_FILTER_ENABLE_1                */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_L2_MLL_TABLE_FIELDS_TRG_EPORT_1_E},
/*LION3_L2_MLL_TABLE_FIELDS_UNREGISTERED_MC_FILTER_ENABLE_1           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_BC_FILTER_ENABLE_1                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
/*LION3_L2_MLL_TABLE_FIELDS_TRG_DEV_1                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(10),

/*LION3_L2_MLL_TABLE_FIELDS_ONE_PLUS_ONE_FILTERING_ENABLE_1           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_TTL_THRESHOLD_1                           */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_L2_MLL_TABLE_FIELDS_MESH_ID_1                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_L2_MLL_TABLE_FIELDS_MC_LOCAL_SWITCHING_ENABLE_1               */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_MAX_HOP_COUNT_ENABLE_1                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_MAX_OUTGOING_HOP_COUNT_1                  */
    PRV_CPSS_STANDARD_FIELD_MAC(6),
/*LION3_L2_MLL_TABLE_FIELDS_RESERVED_2*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/*LION3_L2_MLL_TABLE_FIELDS_ENTRY_SELECTOR                            */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_L2_MLL_TABLE_FIELDS_L2_NEXT_MLL_PTR                           */
    PRV_CPSS_STANDARD_FIELD_MAC(15)
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20IpMllTableFieldsFormat[
    LION3_IP_MLL_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_IP_MLL_TABLE_FIELDS_LAST_0                                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_START_OF_TUNNEL_0                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_MLL_RPF_FAIL_CMD_0                          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_IP_MLL_TABLE_FIELDS_MLL_EVID_0                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*LION3_IP_MLL_TABLE_FIELDS_USE_VIDX_0                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*UseVIDX0 = "True"*/
    /*LION3_IP_MLL_TABLE_FIELDS_EVIDX_0                                     */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

    /*UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
    /*LION3_IP_MLL_TABLE_FIELDS_TRG_EPORT_0                                 */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         14,/*was 13 in sip5 */
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

    /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "True")*/
    /*LION3_IP_MLL_TABLE_FIELDS_TRG_TRUNK_ID_0                              */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_0_E},

    /*LION3_IP_MLL_TABLE_FIELDS_RESEREVED_2_E*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_IP_MLL_TABLE_FIELDS_EVIDX_0_E},

/*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
/*LION3_IP_MLL_TABLE_FIELDS_TRG_DEV_0                                   */
    PRV_CPSS_STANDARD_FIELD_MAC(10),

/*LION3_IP_MLL_TABLE_FIELDS_EXCLUDE_SRC_VLAN_0                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_TTL_THRESHOLD_0_OR_HOP_LIMIT_THRESHOLD_0    */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_IP_MLL_TABLE_FIELDS_TUNNEL_PTR_0                                */
    PRV_CPSS_STANDARD_FIELD_MAC(16),/*was 15 in sip5 */
/*LION3_IP_MLL_TABLE_FIELDS_TUNNEL_TYPE_0                               */
    PRV_CPSS_STANDARD_FIELD_MAC(1),


/*LION3_IP_MLL_TABLE_FIELDS_LAST_1                                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_START_OF_TUNNEL_1                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_MLL_RPF_FAIL_CMD_1                          */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*LION3_IP_MLL_TABLE_FIELDS_MLL_EVID_1                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(13),
/*LION3_IP_MLL_TABLE_FIELDS_USE_VIDX_1                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1                           */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

    /*UseVIDX0 = "True"*/
    /*LION3_IP_MLL_TABLE_FIELDS_EVIDX_1                                     */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

    /*UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
    /*LION3_IP_MLL_TABLE_FIELDS_TRG_EPORT_1                                 */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         14,/*was 13 in sip5 */
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

    /*(UseVIDX0 = "False") and (TargetIsTrunk0 = "True")*/
    /*LION3_IP_MLL_TABLE_FIELDS_TRG_TRUNK_ID_1                              */
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_IP_MLL_TABLE_FIELDS_TARGET_IS_TRUNK_1_E},

    /*LION3_IP_MLL_TABLE_FIELDS_RESEREVED_3_E*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_IP_MLL_TABLE_FIELDS_EVIDX_1_E},
/*(UseVIDX0 = "False") and (TargetIsTrunk0 = "False")*/
/*LION3_IP_MLL_TABLE_FIELDS_TRG_DEV_1                                   */
    PRV_CPSS_STANDARD_FIELD_MAC(10),

/*LION3_IP_MLL_TABLE_FIELDS_EXCLUDE_SRC_VLAN_1                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_TTL_THRESHOLD_1_OR_HOP_LIMIT_THRESHOLD_1    */
    PRV_CPSS_STANDARD_FIELD_MAC(8),
/*LION3_IP_MLL_TABLE_FIELDS_TUNNEL_PTR_1                                */
    PRV_CPSS_STANDARD_FIELD_MAC(16),/*was 15 in sip5 */
/*LION3_IP_MLL_TABLE_FIELDS_TUNNEL_TYPE_1                               */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*LION3_IP_MLL_TABLE_FIELDS_NEXT_MLL_PTR                                */
    PRV_CPSS_STANDARD_FIELD_MAC(16)
};

/* new table in sip 5_15 that was not in sip5 */
static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20PlrMeteringConfigurationTableFieldsFormat[
    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS___LAST_VALUE___E]=
{
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_COLOR_MODE_E                                   */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_POLICER_MODE_E                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(3),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_MG_COUNTERS_SET_EN_E                           */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_BILLING_PTR_E                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(17),/*was 16 in sip5_15 */
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_BYTE_OR_PACKET_COUNTING_MODE_E                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_PACKET_SIZE_MODE_E                             */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TUNNEL_TERMINATION_PACKET_SIZE_MODE_E          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INCLUDE_LAYER1_OVERHEAD_E                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_DSA_TAG_COUNTING_MODE_E                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/* ingress only fields */

/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_RED_CMD_E                              */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     2,
     SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E},
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_YELLOW_CMD_E                           */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_GREEN_CMD_E                            */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_MODIFY_DSCP_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_MODIFY_UP_E                            */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_QOS_PROFILE_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(10),

/* egress only fields */

/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_REMARK_MODE_E                           */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E},
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_DROP_RED_E                              */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_EXP_E                         */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_DSCP_E                        */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_TC_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_UP_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_DP_E                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_YELLOW_ECN_MARKING_E                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1),

/* field common to ingress and to egress */

/*SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_MEF_10_3_ENV_SIZE_E                           */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     3,
     SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_QOS_PROFILE_E}
};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20FdbFdbTableFieldsFormat[SIP5_10_FDB_FDB_TABLE_FIELDS___LAST_VALUE___E] =
{
    /*LION3_FDB_FDB_TABLE_FIELDS_VALID_E */
    PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_SKIP_E  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_AGE_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_FDB_ENTRY_TYPE_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_FID_E              */
    ,PRV_CPSS_STANDARD_FIELD_MAC(13)

    /*    LION3_FDB_FDB_TABLE_FIELDS_MAC_ADDR_E     */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         48,
         LION3_FDB_FDB_TABLE_FIELDS_FID_E}
    /*    LION3_FDB_FDB_TABLE_FIELDS_DEV_ID_E       */
        ,PRV_CPSS_STANDARD_FIELD_MAC(10)
    /*    LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_5_0_E*/
        ,PRV_CPSS_STANDARD_FIELD_MAC(6)
    /*    LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E     */
        ,PRV_CPSS_STANDARD_FIELD_MAC(1)

    /*    LION3_FDB_FDB_TABLE_FIELDS_DIP_E          */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         32,
         LION3_FDB_FDB_TABLE_FIELDS_FID_E}
    /*    LION3_FDB_FDB_TABLE_FIELDS_SIP_E          */
        ,PRV_CPSS_STANDARD_FIELD_MAC(32)


    /*    LION3_FDB_FDB_TABLE_FIELDS_VIDX_E         */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         LION3_FDB_FDB_TABLE_FIELDS_SIP_E}

    /*        LION3_FDB_FDB_TABLE_FIELDS_TRUNK_NUM_E      */
            ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             12,
             LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E}

    /*        LION3_FDB_FDB_TABLE_FIELDS_EPORT_NUM_E      */
            ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             14,/*was 13 in sip5 */
             LION3_FDB_FDB_TABLE_FIELDS_IS_TRUNK_E}



    /*    LION3_FDB_FDB_TABLE_FIELDS_USER_DEFINED_E       */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         8,
         LION3_FDB_FDB_TABLE_FIELDS_VIDX_E}


    /*    LION3_FDB_FDB_TABLE_FIELDS_RESERVED_99_103_E    */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         5,
         LION3_FDB_FDB_TABLE_FIELDS_VIDX_E}

    /*    LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_11_9_E     */
        ,PRV_CPSS_STANDARD_FIELD_MAC(3)



    /*    LION3_FDB_FDB_TABLE_FIELDS_DA_ACCESS_LEVEL_E    */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         3,
         LION3_FDB_FDB_TABLE_FIELDS_USER_DEFINED_E}
    /*    LION3_FDB_FDB_TABLE_FIELDS_SA_ACCESS_LEVEL_E    */
        ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*    LION3_FDB_FDB_TABLE_FIELDS_SOURCE_ID_8_6_E      */
        ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*    LION3_FDB_FDB_TABLE_FIELDS_RESERVED_116_118_E   */
        ,PRV_CPSS_STANDARD_FIELD_MAC(3)

    /*    LION3_FDB_FDB_TABLE_FIELDS_ORIG_VID1_E          */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_FDB_FDB_TABLE_FIELDS_USER_DEFINED_E}

    /*LION3_FDB_FDB_TABLE_FIELDS_IS_STATIC_E              */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_FDB_TABLE_FIELDS_ORIG_VID1_E}
    /*LION3_FDB_FDB_TABLE_FIELDS_MULTIPLE_E               */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_DA_CMD_E                 */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_SA_CMD_E                 */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_DA_ROUTE_E               */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_SP_UNKNOWN_E             */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_SA_QOS_PARAM_SET_IDX_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_DA_QOS_PARAM_SET_IDX_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /*LION3_FDB_FDB_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_E  */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_SA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /*LION3_FDB_FDB_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E   */
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)

    /*LION3_FDB_FDB_TABLE_FIELDS_VRF_ID_E              */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     12,
     LION3_FDB_FDB_TABLE_FIELDS_FDB_ENTRY_TYPE_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_SCOPE_CHECK_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_FCOE_D_ID_E*/
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     24,
     LION3_FDB_FDB_TABLE_FIELDS_VRF_ID_E}
    /*LION3_FDB_FDB_TABLE_FIELDS_IPV4_DIP_E              */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_FDB_FDB_TABLE_FIELDS_VRF_ID_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DST_SITE_ID_E */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_FDB_TABLE_FIELDS_IPV6_SCOPE_CHECK_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_DEC_TTL_OR_HOP_COUNT_E */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_FDB_TABLE_FIELDS_IPV4_DIP_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_INGRESS_MIRROR_TO_ANALYZER_INDEX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /* LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_MARKING_EN_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_INDEX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(7)
    /* LION3_FDB_FDB_TABLE_FIELDS_QOS_PROFILE_PRECEDENCE_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_MODIFY_UP_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(2)
    /* LION3_FDB_FDB_TABLE_FIELDS_MODIFY_DSCP_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(2)
    /* LION3_FDB_FDB_TABLE_FIELDS_COUNTER_SET_INDEX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /* LION3_FDB_FDB_TABLE_FIELDS_ARP_BC_TRAP_MIRROR_EN_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_DIP_ACCESS_LEVEL_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(6)
    /* LION3_FDB_FDB_TABLE_FIELDS_ICMP_REDIRECT_EXCEP_MIRROR_EN_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_MTU_INDEX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(3)
    /* LION3_FDB_FDB_TABLE_FIELDS_USE_VIDX_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_TARGET_DEVICE_E */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     10,
     LION3_FDB_FDB_TABLE_FIELDS_EPORT_NUM_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_NEXT_HOP_EVLAN_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(13)
    /* LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_START_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_TYPE_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(1)
    /* LION3_FDB_FDB_TABLE_FIELDS_ARP_PTR_E*/
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     18,/*was 17 in sip5 */
     LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_START_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_PTR_E*/
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     16,/*was 15 in sip5 */
     LION3_FDB_FDB_TABLE_FIELDS_TUNNEL_TYPE_E}

    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_0_E*/
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_FDB_FDB_TABLE_FIELDS_FDB_ENTRY_TYPE_E}
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_1_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(32)
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_2_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(32)
    /* LION3_FDB_FDB_TABLE_FIELDS_IPV6_DIP_3_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(32)
    /* LION3_FDB_FDB_TABLE_FIELDS_NH_DATA_BANK_NUM_E*/
    ,PRV_CPSS_STANDARD_FIELD_MAC(4)

    /* SIP5_10_FDB_FDB_TABLE_FIELDS_UC_ROUTE_TS_IS_NAT_E */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_PTR_E}

};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20FdbAuMsgTableFieldsFormat[SIP5_10_FDB_AU_MSG_TABLE_FIELDS___LAST_VALUE___E] =
{
/*LION3_FDB_AU_MSG_TABLE_FIELDS_MESSAGE_ID_E                               */
PRV_CPSS_STANDARD_FIELD_MAC(1)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_MSG_TYPE_E                                 */
,PRV_CPSS_STANDARD_FIELD_MAC(3)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_FDB_ENTRY_TYPE_E                           */
,PRV_CPSS_STANDARD_FIELD_MAC(3)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_VALID_E                                    */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_SKIP_E                                     */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E                                      */
,PRV_CPSS_STANDARD_FIELD_MAC(1)


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_8_0_E                   */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     9,
     LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E}


/*     LION3_FDB_AU_MSG_TABLE_FIELDS_CHAIN_TOO_LONG_E                      */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E}

/*     LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_OFFSET_E                     */
,     PRV_CPSS_STANDARD_FIELD_MAC(5)
/*     LION3_FDB_AU_MSG_TABLE_FIELDS_ENTRY_FOUND_E                         */
,     PRV_CPSS_STANDARD_FIELD_MAC(1)


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_E                             */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     48,
     LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_8_0_E}

/*     LION3_FDB_AU_MSG_TABLE_FIELDS_DEV_ID_E                              */
,     PRV_CPSS_STANDARD_FIELD_MAC(10)
/*     LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_5_0_E                       */
,     PRV_CPSS_STANDARD_FIELD_MAC(6)


/*     LION3_FDB_AU_MSG_TABLE_FIELDS_DIP                                 */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_8_0_E}

/*     LION3_FDB_AU_MSG_TABLE_FIELDS_SIP_E                                 */
,     PRV_CPSS_STANDARD_FIELD_MAC(32)


/*LION3_FDB_AU_MSG_TABLE_FIELDS_FID_E                                      */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 13,
 LION3_FDB_AU_MSG_TABLE_FIELDS_SIP_E}


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_VIDX_E                                 */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     16,
     LION3_FDB_AU_MSG_TABLE_FIELDS_FID_E}


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_IS_TRUNK_E                             */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_FDB_AU_MSG_TABLE_FIELDS_FID_E}

/*        LION3_FDB_AU_MSG_TABLE_FIELDS_EPORT_NUM_E                        */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         14,/*was 13 in sip5 */
         LION3_FDB_AU_MSG_TABLE_FIELDS_IS_TRUNK_E}

/*        LION3_FDB_AU_MSG_TABLE_FIELDS_TRUNK_NUM_E                        */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_FDB_AU_MSG_TABLE_FIELDS_IS_TRUNK_E}


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_USER_DEFINED_E                         */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         8,
         LION3_FDB_AU_MSG_TABLE_FIELDS_VIDX_E}

/*    LION3_FDB_AU_MSG_TABLE_FIELDS_RESERVED_109_113_E                     */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         5,
         LION3_FDB_AU_MSG_TABLE_FIELDS_VIDX_E}

/*    LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_11_9_E                       */
,    PRV_CPSS_STANDARD_FIELD_MAC(3)


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_ORIG_VID1_E                            */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         12,
         LION3_FDB_AU_MSG_TABLE_FIELDS_USER_DEFINED_E}

/*    LION3_FDB_AU_MSG_TABLE_FIELDS_DA_ACCESS_LEVEL_E                      */
        ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         3,
         LION3_FDB_AU_MSG_TABLE_FIELDS_USER_DEFINED_E}
/*    LION3_FDB_AU_MSG_TABLE_FIELDS_SA_ACCESS_LEVEL_E                      */
 ,   PRV_CPSS_STANDARD_FIELD_MAC(3)
/*    LION3_FDB_AU_MSG_TABLE_FIELDS_SOURCE_ID_8_6_E                        */
,    PRV_CPSS_STANDARD_FIELD_MAC(3)


/*LION3_FDB_AU_MSG_TABLE_FIELDS_APP_SPECIFIC_CPU_CODE_E                    */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_ORIG_VID1_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_SP_UNKNOWN_E                               */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_SEARCH_TYPE_E                              */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_INDEX_20_9_E                      */
,PRV_CPSS_STANDARD_FIELD_MAC(12)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_MULTIPLE_E                                 */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_DA_ROUTE_E                                 */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_SA_QOS_PARAM_SET_IDX_E                     */
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_DA_QOS_PARAM_SET_IDX_E                     */
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_IS_STATIC_E                                */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_DA_CMD_E                                   */
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_SA_CMD_E                                   */
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_SA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E     */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/*LION3_FDB_AU_MSG_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E     */
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_FDB_LOOKUP_KEY_MODE_E */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/* fields for the format of : MAC NA moved update Message */

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_UP0_E                             */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 3,
 LION3_FDB_AU_MSG_TABLE_FIELDS_MAC_ADDR_OFFSET_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_IS_MOVED_E                        */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_SEARCH_TYPE_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_IS_TRUNK_E                    */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/*    LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_EPORT_E                   */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
    14,/*was 13 in sip5 */
     LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_IS_TRUNK_E}


/*    LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_TRUNK_NUM_E               */
    ,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     12,
     LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_IS_TRUNK_E}


/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_DEVICE_E                      */
,PRV_CPSS_EXPLICIT_FIELD_MAC(152, 10)

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVED_OLD_SRC_ID_E                      */
,PRV_CPSS_STANDARD_FIELD_MAC(12)


    /* additional fields : for FUTURE support of 'NA move Entry' */

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVE_ENTRY_ENABLE_E                     */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_DA_LOOKUP_INGRESS_MIRROR_TO_ANALYZER_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_NA_MOVE_ENTRY_INDEX_E                      */
,PRV_CPSS_STANDARD_FIELD_MAC(19)

 /* fields for FDB Routing */

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MAC_ADDR_INDEX_E                 */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 21,
 LION3_FDB_AU_MSG_TABLE_FIELDS_AGE_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E                 */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 12,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MAC_ADDR_INDEX_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_SCOPE_CHECK_E                      */
,PRV_CPSS_STANDARD_FIELD_MAC(1)

 /* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_FCOE_D_ID_E*/
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 24,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E}

/*LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV4_DIP_E              */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 32,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_VRF_ID_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DST_SITE_ID_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_SCOPE_CHECK_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_DEC_TTL_OR_HOP_COUNT_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV4_DIP_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_BYPASS_TTL_OPTIONS_OR_HOP_EXTENSION_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_INGRESS_MIRROR_TO_ANALYZER_INDEX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_MARKING_EN_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_INDEX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(7)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_QOS_PROFILE_PRECEDENCE_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MODIFY_UP_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(2)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MODIFY_DSCP_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(2)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_COUNTER_SET_INDEX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ARP_BC_TRAP_MIRROR_EN_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_DIP_ACCESS_LEVEL_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(6)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ICMP_REDIRECT_EXCEP_MIRROR_EN_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MTU_INDEX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(3)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_USE_VIDX_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IS_TRUNK_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EVIDX_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 16,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_USE_VIDX_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TARGET_DEVICE_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 10,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IS_TRUNK_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TRUNK_NUM_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
12,
LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IS_TRUNK_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EPORT_NUM_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 14,/*was 13 in sip5 */
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TARGET_DEVICE_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_NEXT_HOP_EVLAN_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 13,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_EPORT_NUM_E}

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_START_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_TYPE_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(1)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_ARP_PTR_E*/
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 18,/*was 17 in sip5 */
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_START_E}
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_PTR_E*/
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 16,/*was 15 in sip5 */
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_TYPE_E}


/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DIP_0_E*/
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 32,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_MAC_ADDR_INDEX_E}
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DIP_1_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(32)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DIP_2_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(32)
/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_IPV6_DIP_3_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(32)

/* LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_NH_DATA_BANK_NUM_E*/
,PRV_CPSS_STANDARD_FIELD_MAC(4)

/* SIP5_10_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TS_IS_NAT_E */
,{PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
 1,
 LION3_FDB_AU_MSG_TABLE_FIELDS_UC_ROUTE_TUNNEL_PTR_E}

};

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC prvCpssDxChSip5_20IpclActionTableFieldsFormat[
    SIP5_20_IPCL_ACTION_TABLE_FIELDS___LAST_VALUE___E] =
{
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_CPU_CODE*/
PRV_CPSS_STANDARD_FIELD_MAC(8),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_COMMAND*/
PRV_CPSS_STANDARD_FIELD_MAC(3),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_REDIRECT_COMMAND*/
PRV_CPSS_STANDARD_FIELD_MAC(3),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_ACTION_STOP*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_SET_MAC2ME*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*==================*/
/*If <Redirect Command> = No Redirect (0)*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_POLICY1_LOOKUP_MODE*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     SIP5_20_IPCL_ACTION_TABLE_FIELDS_SET_MAC2ME_E},
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_POLICY2_LOOKUP_MODE*/
            PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_IPCL_PROFILE_INDEX*/
            PRV_CPSS_STANDARD_FIELD_MAC(13),
/*==================*/
/* If <Redirect Command> = 1 (Redirect to egress interface)*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_USE_VIDX*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     SIP5_20_IPCL_ACTION_TABLE_FIELDS_SET_MAC2ME_E},
     /*<UseVIDX> = 1*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_VIDX*/
    PRV_CPSS_STANDARD_FIELD_MAC(16),
     /*if <UseVIDX> = 0*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_IS_TRUNK*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         1,
         SIP5_20_IPCL_ACTION_TABLE_FIELDS_USE_VIDX_E},
        /*if <UseVIDX> = 0 & <IsTrunk> = 0*/
        /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_TRG_PORT*/
            {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
             14,
             SIP5_20_IPCL_ACTION_TABLE_FIELDS_IS_TRUNK_E},
        /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_TARGET_DEVICE*/
        PRV_CPSS_STANDARD_FIELD_MAC(10),
        /*if <UseVIDX> = 0 & <IsTrunk> = 1*/
        /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_TRUNK_ID*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         14,
         SIP5_20_IPCL_ACTION_TABLE_FIELDS_IS_TRUNK_E},
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_VNT_L2_ECHO*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     SIP5_20_IPCL_ACTION_TABLE_FIELDS_TARGET_DEVICE_E},
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_TUNNEL_START*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
        /*if <Tunnel Start> == 0*/
        /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_ARP_POINTER*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         18,
         SIP5_20_IPCL_ACTION_TABLE_FIELDS_TUNNEL_START_E},
        /*if <Tunnel Start> == 1*/
        /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_TUNNEL_POINTER*/
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
         16,
         SIP5_20_IPCL_ACTION_TABLE_FIELDS_TUNNEL_START_E},
        /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_TUNNEL_START_PASSENGER_TYPE*/
        PRV_CPSS_STANDARD_FIELD_MAC(1),
/*==================*/
/*If <Redirect Command> = 2 (Redirect to NHE)*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_PBR_MODE*/
    PRV_CPSS_EXPLICIT_FIELD_MAC(32,1),
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_IP_NEXT_HOP_ENTRY_INDEX*/
    PRV_CPSS_STANDARD_FIELD_MAC(18),
/*==================*/
/*else if <Redirect Command> = 4 (Assign VRF_ID)*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_VRF_ID*/
    PRV_CPSS_EXPLICIT_FIELD_MAC(52,12),
/*==================*/
/*else if (<Redirect Command> = 6 - Replace MAC SA)*/
/* 43 16 Mac Sa[27:0]
Mac_Sa
 81 62 Mac Sa [47:28]
Mac_Sa_47_28*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_MAC_SA_27_0*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     28,
     SIP5_20_IPCL_ACTION_TABLE_FIELDS_SET_MAC2ME_E},
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_MAC_SA_47_28*/
    PRV_CPSS_EXPLICIT_FIELD_MAC(62,20),
/*==================*/

/*else if (<Redirect Command> = !6 - Replace MAC SA)*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_ACTIVATE_METER*/
    PRV_CPSS_EXPLICIT_FIELD_MAC(64,1),
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_BIND_TO_POLICER_COUNTER*/
    PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_POLICER_PTR*/
    PRV_CPSS_STANDARD_FIELD_MAC(14),
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_RESERVED*/
    PRV_CPSS_STANDARD_FIELD_MAC(2),
/*==================*/
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_ENABLE_MIRROR_TCP_RST_OR_FIN*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_MIRROR_TO_ANALYZER_PORT*/
PRV_CPSS_STANDARD_FIELD_MAC(3),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_MODIFY_MAC_DA*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
    /*If <Redirect Command> = 1 (Redirect to egress interface)*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_MODIFY_MAC_SA*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     SIP5_20_IPCL_ACTION_TABLE_FIELDS_MODIFY_MAC_DA_E},
    /* If <Redirect Command> != 1 (Redirect to egress interface)*/
    /*SIP5_20_IPCL_ACTION_TABLE_FIELDS_SET_EGRESS_FILTER_REGISTERED*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     SIP5_20_IPCL_ACTION_TABLE_FIELDS_MODIFY_MAC_DA_E},
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_BRIDGE_BYPASS*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_BYPASS_INGRESS_PIPE*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_EN_NESTED_VLAN*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_SET_SST_ID*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_SST_ID*/
PRV_CPSS_STANDARD_FIELD_MAC(12),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_USER_AC_ENABLE*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_ACTIVATE_COUNTER*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_COUNTER_INDEX*/
PRV_CPSS_STANDARD_FIELD_MAC(14),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_VLAN_PRECEDENCE*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_VLAN_COMMAND*/
PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_VID0*/
PRV_CPSS_STANDARD_FIELD_MAC(13),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_QOS_PROFILE_MARKING_ENABLE*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_QOS_PRECEDENCE*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_QOS_PROFILE*/
PRV_CPSS_STANDARD_FIELD_MAC(10),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_MODIFY_DSCP*/
PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_MODIFY_UP*/
PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_VLAN1_CMD*/
PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_UP1_CMD*/
PRV_CPSS_STANDARD_FIELD_MAC(2),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_VID1*/
PRV_CPSS_STANDARD_FIELD_MAC(12),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_UP1*/
PRV_CPSS_STANDARD_FIELD_MAC(3),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_IPCL_RESERVED_EN*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_IPCL_TO_TXQ_RESERVED*/
PRV_CPSS_STANDARD_FIELD_MAC(20),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_TIMESTAMP_EN*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_OFFSET_INDEX*/
PRV_CPSS_STANDARD_FIELD_MAC(7),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_OAM_PROCESSING_EN*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_OAM_PROFILE*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_FLOW_ID*/
PRV_CPSS_STANDARD_FIELD_MAC(13),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_RESERVED_1*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_ASSIGN_SRC_EPORT_EN*/
PRV_CPSS_STANDARD_FIELD_MAC(1),
/*SIP5_20_IPCL_ACTION_TABLE_FIELDS_SRC_EPORT*/
PRV_CPSS_STANDARD_FIELD_MAC(14)
};
/* bobcat3 tables - end */


/* Xcat revision 1 and above direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC directAccessXcatTableInfo[] =
{
    /* PRV_DXCH_XCAT_TABLE_VLAN_E */
    {0x0A200000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_VRF_ID_E */
    {0x0A180000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E */
    {0x0c001000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_STG_E */
    {0x0A080000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_LTT_TT_ACTION_E */
    {0x0e8c0000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_MULTICAST_E */
    {0x0A100000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E */
    {0x0F008000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E */
    {0x0F040000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E */
    {0x0C00C000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E */
    {0x0F010000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_IPCL_CFG_E */
    {0x0B810000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E */
    {0x0B820000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_EPCL_CFG_E */
    {0x0F808000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E */
    {0x0C010000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E */
    {0x0F040000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x0C004000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_CPSS_XCAT_TABLE_MAC2ME_E */
    {0x0C001600, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E */
    {0x0A400100, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E*/
    {0x03880000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_L2MLL_VIDX_ENABLE_E */
    {0x0D808000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT_TABLE_L2MLL_POINTER_MAPPING_E */
    {0x0D809000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

};

/* xCat3 direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC directAccessXcat3TableInfo[] =
{
    /* PRV_DXCH_XCAT3_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x16004000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_PORT_VLAN_QOS_E */
    {0x16001000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_IPCL_CFG_E */
    {0x15010000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_STG_E */
    {0x03880000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_VLAN_E */
    {0x03A00000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_MULTICAST_E */
    {0x03900000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_ROUTE_HA_MAC_SA_E */
    {0x1C008000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_ROUTE_HA_ARP_DA_E */
    {0x1C040000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_EPCL_CFG_E */
    {0x1D008000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_TUNNEL_START_CFG_E */
    {0x1C040000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E */
    {0x04000300, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_ROUTE_ACCESS_MATRIX_E */
    {0x04000440, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_UC_MC_ROUTE_NEXT_HOP_E */
    {0x04300000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_ROUTE_NEXT_HOP_AGE_E */
    {0x04001000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_MAC2ME_E */
    {0x16001600, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_INGRESS_TRANSLATION_E */
    {0x1600C000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_EGRESS_TRANSLATION_E */
    {0x1C010000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_VRF_ID_E */
    {0x03980000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_LTT_TT_ACTION_E */
    {0x1B0C0000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_IPCL_LOOKUP1_CFG_E */
    {0x15020000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT3_TABLE_IPCL_UDB_CFG_E */
    {0x16010000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT3_TABLE_BCN_PROFILE_E */
    {0x03400100, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT3_TABLE_EGRESS_POLICER_REMARKING_E*/
    {0x06080000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT3_TABLE_MLL_L2MLL_VIDX_ENABLE_E*/
    {0x19008000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT_TABLE_MLL_L2MLL_POINTER_MAPPING_E*/
    {0x19009000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

};

typedef struct{
    PRV_CPSS_DXCH_TABLE_ENT                 globalIndex;
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC    directAccessInfo;
}PRV_CPSS_DXCH_TABLES_INFO_DIRECT_EXT_STC;

typedef struct{
    PRV_CPSS_DXCH_TABLE_ENT          globalIndex;
    PRV_CPSS_DXCH_TABLES_INFO_STC    tableInfo;
}PRV_CPSS_DXCH_TABLES_INFO_EXT_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC
 *
 * Description: A structure to hold direct access table entry info per unit.
 *
 * Fields:
 *      globalIndex         - table index
 *      unitIndex           - unit index
 *      directAccessInfo    - direct access table info
 */
typedef struct{
    PRV_CPSS_DXCH_TABLE_ENT                 globalIndex;
    PRV_CPSS_DXCH_UNIT_ENT                  unitIndex;
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC    directAccessInfo;
}PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC;

/* Lion B and above direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_EXT_STC directAccessLionBTableInfoExt[] =
{
    {PRV_CPSS_DXCH_TABLE_VLAN_E,
    /* PRV_DXCH_XCAT_TABLE_VLAN_E */
    {  TABLES_INFO_DIRECT_NOT_EXISTS_CNS }}, /* see ingress/egress tables instead */

    {PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
    /* PRV_DXCH_XCAT_TABLE_VRF_ID_E */
    {  TABLES_INFO_DIRECT_NOT_EXISTS_CNS }}, /* the field is in ingress vlan entry */

    {PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    /* PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E */
    {0x01001000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_STG_E,
    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {  TABLES_INFO_DIRECT_NOT_EXISTS_CNS }}, /* see ingress/egress tables instead */

    {PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
    /* PRV_DXCH_XCAT_LTT_TT_ACTION_E */
    {0x0D8C0000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_MULTICAST_E,
    /* PRV_DXCH_XCAT_TABLE_MULTICAST_E */
    {0x11860000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
    /* PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E         */
    {0x0E808000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
    /* PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E         */
    {0x0E840000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E,
    /* PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E */
    {0x0100C000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E,
    /* PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E      */
    {0x0E810000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
    /* PRV_DXCH_XCAT_TABLE_IPCL_CFG_E                */
    {TABLES_INFO_DIRECT_NOT_MODIFIED_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E,
    /* PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E        */
    {TABLES_INFO_DIRECT_NOT_MODIFIED_CNS}},

    {PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E,
    /* PRV_DXCH_XCAT_TABLE_EPCL_CFG_E                */
    {0x0E008000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E,
    /* PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E */
    {0x01010000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
    /* PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E        */
    {0x0E840000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
    /* PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x01004000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_MAC2ME_E,
    /* PRV_CPSS_XCAT_TABLE_MAC2ME_E */
    {0x01001600, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E,
    /* PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E             */
    {TABLES_INFO_DIRECT_NOT_EXISTS_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E,
    /*PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E */
    {0x07880000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
    /* PRV_DXCH_LION_TABLE_VLAN_INGRESS_E            */
    {0x118A0000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
    /* PRV_DXCH_LION_TABLE_VLAN_EGRESS_E             */
    {0x11800000, 0x40, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
    /* PRV_DXCH_LION_TABLE_STG_INGRESS_E             */
    {0x118D0000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
    /* PRV_DXCH_LION_TABLE_STG_EGRESS_E              */
    {0x11840000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E,
    /*PRV_CPSS_LION_TABLE_PORT_ISOLATION_L2_E*/
    {0x11880000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E,
    /*PRV_CPSS_LION_TABLE_PORT_ISOLATION_L3_E*/
    {0x11890000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E,
    /*PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E*/
    {0x11002400, 0x40, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E,
    /*PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E*/
    {0x11002800, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
    /*PRV_CPSS_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E*/
    {0x01810500, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E,
    /*PRV_CPSS_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E*/
    {0x01810800, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E,
    /*PRV_CPSS_LION_TABLE_TXQ_DESIGNATED_PORT_E*/
    {0x01810200, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,
    /*PRV_CPSS_LION_TABLE_TXQ_EGRESS_STC_E*/
    {0x11004400, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E,
    /*PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E*/
    {0x02801400, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E,
    /*PRV_CPSS_LION_TABLE_TRUNK_HASH_MASK_CRC_E*/
    {0x0b800400 , 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},
};

static GT_U32   directAccessLionBTableInfoExtNumEntries =
    NUM_ELEMENTS_IN_ARR_MAC(directAccessLionBTableInfoExt);

/* xCat2 direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC directAccessXCAT2TableInfo[] =
{
    /*PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E*/
    {0x0B820000 , 0x08, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /*PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E*/
    {0x0B830000 , 0x08, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS},

    /* PRV_DXCH_XCAT2_TABLE_MAC2ME_E */
    {0x0C001600, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}
};


/* Lion2 direct table information */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_EXT_STC directAccessLion2TableInfo[] =
{
    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
    /*PRV_CPSS_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E*/
    {0x12010500, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E,
    /*PRV_CPSS_LION_TABLE_TXQ_DESIGNATED_PORT_E*/
    {0x12010200, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E,
    /*PRV_CPSS_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E*/
    {0x12010800, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},
};

static GT_U32   directAccessLion2TableInfoNumEntries =
    NUM_ELEMENTS_IN_ARR_MAC(directAccessLion2TableInfo);

#define NO_DIRECT_ACCESS_FROM_CPU_TO_ADDRESS_CNS    0xFFFFFFFF

/*
 * Typedef: enum PRV_DXCH_BOBCAT2_TABLES_ENT
 *
 * Description:
 *      enumeration fields of the directAccessBobcat2TableInfo table.
 *
 * Enumerations:
 *
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_STATISTICAL_RATE_LIMIT_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_CPU_CODE_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_QOS_PROFILE_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_INGRESS_VLAN_TRANSLATION_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_INGRESS_PCL_UDB_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_VLAN_PORT_PROTOCOL_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_MAC2ME_E
 *   PRV_DXCH_BOBCAT2_TABLE_L2I_VLAN_INGRESS_E
 *   PRV_DXCH_BOBCAT2_TABLE_L2I_INGRESS_BRIDGE_PORT_MEMBERS_E
 *   PRV_DXCH_BOBCAT2_TABLE_L2I_INGRESS_SPAN_STATE_GROUP_INDEX_E
 *   PRV_DXCH_BOBCAT2_TABLE_L2I_STG_INGRESS_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_EGRESS_STC_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_PRE_LOOKUP_INGRESS_EPORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_POST_LOOKUP_INGRESS_EPORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_INGRESS_EPORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E
 *   PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_SOURCE_TRUNK_ATTRIBUTE_E
 *   PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_PHYSICAL_PORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_EPORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_TRUNK_LTT_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_L2_ECMP_LTT_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_L2_ECMP_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_TO_CPU_RATE_LIMITER_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_TX_PROTECTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_EPORT_TO_LOC_MAPPING_E
 *   PRV_DXCH_BOBCAT2_TABLE_EQ_PROTECTION_LOC_E
 *   PRV_DXCH_BOBCAT2_TABLE_LPM_MEM_E
 *   PRV_DXCH_BOBCAT2_TABLE_LPM_IPV4_VRF_E
 *   PRV_DXCH_BOBCAT2_TABLE_LPM_IPV6_VRF_E
 *   PRV_DXCH_BOBCAT2_TABLE_LPM_FCOE_VRF_E
 *   PRV_DXCH_BOBCAT2_TABLE_LPM_ECMP_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_EFT_EGRESS_FAST_STACK_FAILOVER_SECONDARY_TARGET_PORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_EGRESS_EPORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_EGRESS_VLAN_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_TARGET_PORT_MAPPER_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_CPU_CODE_TO_LOOPBACK_MAPPER_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_EGRESS_EPORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_DEVICE_MAP_TABLE_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_VLAN_EGRESS_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_ATTRIBUTES_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_SPANNING_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_MAPPER_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_MULTICAST_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_STG_EGRESS_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_PORT_ISOLATION_L2_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_PORT_ISOLATION_L3_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_SOURCE_ID_MEMBERS_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_NON_TRUNK_MEMBERS_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_NON_TRUNK_MEMBERS2_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_EPORT_FILTER_E
 *   PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_DESIGNATED_PORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_1_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_2_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_PHYSICAL_PORT_1_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_PHYSICAL_PORT_2_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_QOS_PROFILE_TO_EXP_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_EPCL_UDB_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_PTP_DOMAIN_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_GENERIC_TS_PROFILE_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_GLOBAL_MAC_SA_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_TUNNEL_START_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_VLAN_TRANSLATION_E
 *   PRV_DXCH_BOBCAT2_TABLE_HA_ROUTE_MAC_SA_E
 *   PRV_DXCH_BOBCAT2_TABLE_MLL_IP_E
 *   PRV_DXCH_BOBCAT2_TABLE_MLL_L2_E
 *   PRV_DXCH_BOBCAT2_TABLE_MLL_L2_LTT_E
 *   PRV_DXCH_BOBCAT2_TABLE_PCL_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_PCL_INGRESS_LOOKUP01_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_PCL_INGRESS_LOOKUP1_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_PCL_TRUNK_HASH_MASK_CRC_E
 *   PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL0_UDB_SELECT_E
 *   PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL1_UDB_SELECT_E
 *   PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL2_UDB_SELECT_E
 *   PRV_DXCH_BOBCAT2_TABLE_TCAM_PCL_TTI_ACTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_ERMRK_TIMESTAMP_CFG_E
 *   PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_LOCAL_ACTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_TARGET_PORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_SOURCE_PORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_ERMRK_QOS_DSCP_MAP_E
 *   PRV_DXCH_BOBCAT2_TABLE_ERMRK_QOS_TC_DP_MAP_E
 *   PRV_DXCH_BOBCAT2_TABLE_TCAM_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_ADJACENCY_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_RBID_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_E_ATTRIBUTES_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_REMARKING_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_HIERARCHICAL_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_METERING_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_E_ATTRIBUTES_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_REMARKING_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_METERING_E
 *   PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_E_ATTRIBUTES_E
 *   PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_REMARKING_E
 *   PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_METERING_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPVX_INGRESS_EPORT_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPVX_EVLAN_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_NEXTHOP_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E
 *   PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_QOS_PROFILE_OFFSETS_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_KEEPALIVE_AGING_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_MEG_EXCEPTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_INVALID_KEEPALIVE_HASH_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_KEEPALIVE_EXCESS_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_EXCEPTION_SUMMARY_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_TX_PERIOD_EXCEPTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_OPCODE_PACKET_COMMAND_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_KEEPALIVE_AGING_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_MEG_EXCEPTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_INVALID_KEEPALIVE_HASH_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_KEEPALIVE_EXCESS_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_EXCEPTION_SUMMARY_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_TX_PERIOD_EXCEPTION_E
 *   PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_OPCODE_PACKET_COMMAND_E
 *   PRV_DXCH_BOBCAT2_TABLE_ERMRK_OAM_LM_OFFSET_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_DSCP_TO_QOS_PROFILE_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_UP_CFI_TO_QOS_PROFILE_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_EXP_TO_QOS_PROFILE_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_DSCP_TO_DSCP_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_UDB_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_TTI_PTP_COMMAND_E
 *   PRV_DXCH_BOBCAT2_TABLE_EPCL_EGRESS_PCL_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_EPCL_UDB_SELECT_E
 *   PRV_DXCH_BOBCAT2_TABLE_BMA_PORT_MAPPING_E
 *   PRV_DXCH_BOBCAT2_TABLE_BMA_MULTICAST_COUNTERS_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_MAX_QUEUE_LIMITS_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP0_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_DQ_QUEUE_BUF_LIMITS_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP1_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP2_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_COUNTERS_Q_MAIN_BUFF_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_COUNTERS_Q_MAIN_MC_BUFF_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_LL_TXQ_LINK_LIST_COUNTERS_Q_MAIN_DESC_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_E
 *   PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_LLFC_COUNTERS_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_DROP_DROP_MASKING_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_DROP_QUEUE_PROFILE_ID_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_CPU_CODE_TO_TC_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_TARGET_DEV_TO_INDEX_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_POLICING_ENABLE_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_QUEUE_ID_SELECTOR_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_INGRESS_GLUE_L1_PKT_LEN_OFFSET_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_AGING_QUEUE_PROFILE_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_AGING_PROFILE_THESHOLDS_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_TARGET_INTERFACE_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E
 *   PRV_DXCH_BOBCAT2_TABLE_TM_FCU_INGRESS_TIMERS_E
 *   PRV_DXCH_BOBCAT2_TABLE_PORT_TO_PHYSICAL_PORT_MAPPING_E
 *   PRV_DXCH_BOBCAT2_TABLE_CN_SAMPLE_INTERVALS_E
 */
typedef enum {
    PRV_DXCH_BOBCAT2_TABLE_EQ_STATISTICAL_RATE_LIMIT_E = 0,
    PRV_DXCH_BOBCAT2_TABLE_EQ_CPU_CODE_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_QOS_PROFILE_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_INGRESS_VLAN_TRANSLATION_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_INGRESS_PCL_UDB_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_VLAN_PORT_PROTOCOL_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_MAC2ME_E,
    PRV_DXCH_BOBCAT2_TABLE_L2I_VLAN_INGRESS_E,
    PRV_DXCH_BOBCAT2_TABLE_L2I_INGRESS_BRIDGE_PORT_MEMBERS_E,
    PRV_DXCH_BOBCAT2_TABLE_L2I_INGRESS_SPAN_STATE_GROUP_INDEX_E,
    PRV_DXCH_BOBCAT2_TABLE_L2I_STG_INGRESS_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_EGRESS_STC_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_PRE_LOOKUP_INGRESS_EPORT_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_POST_LOOKUP_INGRESS_EPORT_E,
    PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_INGRESS_EPORT_E,
    PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E,
    PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_SOURCE_TRUNK_ATTRIBUTE_E,
    PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_PHYSICAL_PORT_E,
    PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_EPORT_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_TRUNK_LTT_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_L2_ECMP_LTT_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_L2_ECMP_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_TO_CPU_RATE_LIMITER_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_TX_PROTECTION_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_EPORT_TO_LOC_MAPPING_E,
    PRV_DXCH_BOBCAT2_TABLE_EQ_PROTECTION_LOC_E,
    PRV_DXCH_BOBCAT2_TABLE_LPM_MEM_E,
    PRV_DXCH_BOBCAT2_TABLE_LPM_IPV4_VRF_E,
    PRV_DXCH_BOBCAT2_TABLE_LPM_IPV6_VRF_E,
    PRV_DXCH_BOBCAT2_TABLE_LPM_FCOE_VRF_E,
    PRV_DXCH_BOBCAT2_TABLE_LPM_ECMP_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_EFT_EGRESS_FAST_STACK_FAILOVER_SECONDARY_TARGET_PORT_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_EGRESS_EPORT_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_EGRESS_VLAN_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_TARGET_PORT_MAPPER_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_CPU_CODE_TO_LOOPBACK_MAPPER_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_EGRESS_EPORT_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_DEVICE_MAP_TABLE_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_VLAN_EGRESS_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_ATTRIBUTES_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_SPANNING_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_MAPPER_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_MULTICAST_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_STG_EGRESS_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_PORT_ISOLATION_L2_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_PORT_ISOLATION_L3_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_SOURCE_ID_MEMBERS_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_NON_TRUNK_MEMBERS_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_NON_TRUNK_MEMBERS2_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_EPORT_FILTER_E,
    PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_DESIGNATED_PORT_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_1_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_2_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_PHYSICAL_PORT_1_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_PHYSICAL_PORT_2_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_QOS_PROFILE_TO_EXP_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_EPCL_UDB_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_PTP_DOMAIN_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_GENERIC_TS_PROFILE_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_GLOBAL_MAC_SA_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_TUNNEL_START_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_VLAN_TRANSLATION_E,
    PRV_DXCH_BOBCAT2_TABLE_HA_ROUTE_MAC_SA_E,
    PRV_DXCH_BOBCAT2_TABLE_MLL_IP_E,
    PRV_DXCH_BOBCAT2_TABLE_MLL_L2_E,
    PRV_DXCH_BOBCAT2_TABLE_MLL_L2_LTT_E,
    PRV_DXCH_BOBCAT2_TABLE_PCL_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_PCL_INGRESS_LOOKUP01_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_PCL_INGRESS_LOOKUP1_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_PCL_TRUNK_HASH_MASK_CRC_E,
    PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL0_UDB_SELECT_E,
    PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL1_UDB_SELECT_E,
    PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL2_UDB_SELECT_E,
    PRV_DXCH_BOBCAT2_TABLE_TCAM_PCL_TTI_ACTION_E,
    PRV_DXCH_BOBCAT2_TABLE_ERMRK_TIMESTAMP_CFG_E,
    PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_LOCAL_ACTION_E,
    PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_TARGET_PORT_E,
    PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_SOURCE_PORT_E,
    PRV_DXCH_BOBCAT2_TABLE_ERMRK_QOS_DSCP_MAP_E,
    PRV_DXCH_BOBCAT2_TABLE_ERMRK_QOS_TC_DP_MAP_E,
    PRV_DXCH_BOBCAT2_TABLE_TCAM_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_ADJACENCY_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_RBID_E,
    PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_E_ATTRIBUTES_E,
    PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_REMARKING_E,
    PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_HIERARCHICAL_E,
    PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_METERING_E,
    PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_E_ATTRIBUTES_E,
    PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_REMARKING_E,
    PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_METERING_E,
    PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_E_ATTRIBUTES_E,
    PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_REMARKING_E,
    PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_METERING_E,
    PRV_DXCH_BOBCAT2_TABLE_IPVX_INGRESS_EPORT_E,
    PRV_DXCH_BOBCAT2_TABLE_IPVX_EVLAN_E,
    PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_NEXTHOP_E,
    PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E,
    PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E,
    PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_QOS_PROFILE_OFFSETS_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_KEEPALIVE_AGING_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_MEG_EXCEPTION_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_INVALID_KEEPALIVE_HASH_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_KEEPALIVE_EXCESS_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_EXCEPTION_SUMMARY_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_TX_PERIOD_EXCEPTION_E,
    PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_OPCODE_PACKET_COMMAND_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_KEEPALIVE_AGING_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_MEG_EXCEPTION_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_INVALID_KEEPALIVE_HASH_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_KEEPALIVE_EXCESS_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_EXCEPTION_SUMMARY_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_TX_PERIOD_EXCEPTION_E,
    PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_OPCODE_PACKET_COMMAND_E,
    PRV_DXCH_BOBCAT2_TABLE_ERMRK_OAM_LM_OFFSET_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_DSCP_TO_QOS_PROFILE_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_UP_CFI_TO_QOS_PROFILE_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_EXP_TO_QOS_PROFILE_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_DSCP_TO_DSCP_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_UDB_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_TTI_PTP_COMMAND_E,
    PRV_DXCH_BOBCAT2_TABLE_EPCL_EGRESS_PCL_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_EPCL_UDB_SELECT_E,
    PRV_DXCH_BOBCAT2_TABLE_BMA_PORT_MAPPING_E,
    PRV_DXCH_BOBCAT2_TABLE_BMA_MULTICAST_COUNTERS_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_MAX_QUEUE_LIMITS_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP0_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_DQ_QUEUE_BUF_LIMITS_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP1_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP2_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_COUNTERS_Q_MAIN_BUFF_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_COUNTERS_Q_MAIN_MC_BUFF_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_LL_TXQ_LINK_LIST_COUNTERS_Q_MAIN_DESC_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_E,
    PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_LLFC_COUNTERS_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_DROP_DROP_MASKING_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_DROP_QUEUE_PROFILE_ID_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_CPU_CODE_TO_TC_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_TARGET_DEV_TO_INDEX_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_POLICING_ENABLE_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_QUEUE_ID_SELECTOR_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_INGRESS_GLUE_L1_PKT_LEN_OFFSET_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_AGING_QUEUE_PROFILE_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_AGING_PROFILE_THESHOLDS_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_TARGET_INTERFACE_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E,
    PRV_DXCH_BOBCAT2_TABLE_TM_FCU_INGRESS_TIMERS_E,
    PRV_DXCH_BOBCAT2_TABLE_PORT_TO_PHYSICAL_PORT_MAPPING_E,
    PRV_DXCH_BOBCAT2_TABLE_CN_SAMPLE_INTERVALS_E,

    PRV_DXCH_BOBCAT2_TABLE___LAST_E, /* last bobcat 2 direct tables */
    /* add here additional direct access tables for bobk */

    PRV_DXCH_BOBK_TABLE_PLR_INGRESS_POLICER_0_METERING_CONFIG_E = PRV_DXCH_BOBCAT2_TABLE___LAST_E,
    PRV_DXCH_BOBK_TABLE_PLR_INGRESS_POLICER_1_METERING_CONFIG_E,
    PRV_DXCH_BOBK_TABLE_PLR_EGRESS_POLICER_METERING_CONFIG_E,

    PRV_DXCH_BOBK_TABLE_PLR_INGRESS_POLICER_0_METERING_CONFORM_SIGN_E,
    PRV_DXCH_BOBK_TABLE_PLR_INGRESS_POLICER_1_METERING_CONFORM_SIGN_E,
    PRV_DXCH_BOBK_TABLE_PLR_EGRESS_POLICER_METERING_CONFORM_SIGN_E,

    PRV_DXCH_BOBK_TABLE___LAST_E, /* last bobk direct tables */

    /* add here additional direct access tables for bobcat3 */
    PRV_CPSS_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E = PRV_DXCH_BOBK_TABLE___LAST_E,
    PRV_CPSS_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
    PRV_CPSS_BOBCAT3_TABLE_EQ_TX_PROTECTION_LOC_E,
    PRV_CPSS_BOBCAT3_TABLE_EGF_QAG_TC_DP_MAPPER_E,

    /* MULTI-instance support */
    PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E,        /* DQ[1] */
    PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_LAST___E =    /* DQ[5] */
        PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E + 4,

    PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E,         /* DQ[1] */
    PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_LAST___E =     /* DQ[5] */
        PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E+4,

    PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E,      /* DQ[1] */
    PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_LAST___E=   /* DQ[5] */
        PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E+4,

    PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT___PIPE_1___E , /*pipe [1]*/

    PRV_DXCH_BOBCAT3_TABLE___LAST_E /* last bobk direct tables */


}PRV_DXCH_BOBCAT2_TABLES_ENT;


/* Bobcat2 direct table information */
/* NOTE: index to this table is from PRV_DXCH_BOBCAT2_TABLES_ENT */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC directAccessBobcat2TableInfo[] =
{
    {PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00100000, 4 , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_CPU_CODE_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00120000, 4 , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00110000, 4 , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    /* PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E */
    {0x01000000 + 0x00014000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    /* PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E */
    {0x01000000 + 0x00020000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TTI_PCL_UDB_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    /* PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E */
    {0x01000000 + 0x00008000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_MAC2ME_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    /* PRV_CPSS_XCAT_TABLE_MAC2ME_E */
    {0x01000000 + 0x00001600, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E, PRV_CPSS_DXCH_UNIT_L2I_E,
    {0x03000000 + 0x002C0000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_INGRESS_VLAN_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E, PRV_CPSS_DXCH_UNIT_L2I_E,
    {0x03000000 + 0x00280000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_INGRESS_BRIDGE_PORT_MEMBERS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_SPAN_STATE_GROUP_INDEX_E, PRV_CPSS_DXCH_UNIT_L2I_E,
    {0x03000000 + 0x002E0000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_INGRESS_SPAN_STATE_GROUP_INDEX_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E, PRV_CPSS_DXCH_UNIT_L2I_E,
    /* PRV_DXCH_LION_TABLE_STG_INGRESS_E             */
    {0x03000000 + 0x00240000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_INGRESS_STP_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E, PRV_CPSS_DXCH_UNIT_TXQ_DQ_E,
    /*PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E*/
    {0x40000000 + 0x00006000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PRIO_TOKEN_BUCKET_CONFIGURATION_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E, PRV_CPSS_DXCH_UNIT_TXQ_DQ_E,
    /*PRV_CPSS_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E*/
    {0x40000000 + 0x0000A000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PORT_TOKEN_BUCKET_CONFIGURATION_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E, PRV_CPSS_DXCH_UNIT_TXQ_DQ_E,
    /*PRV_CPSS_LION_TABLE_TXQ_EGRESS_STC_E*/
    {0x40000000 + 0x0000D800, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGRESS_STC_TABLE_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},
    /* Bobcat2, Caelum, Bobcat3 start new tables */
    /* TTI tables */
    {PRV_CPSS_DXCH_LION3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00100000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TTI_PHYSICAL_PORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00210000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TTI_DEFAULT_PORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00230000 , 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* Bridge Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E, PRV_CPSS_DXCH_UNIT_L2I_E,
    {0x03000000 + 0x00220000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_EPORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E, PRV_CPSS_DXCH_UNIT_L2I_E,
    {0x03000000 + 0x00210000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_EPORT_LEARN_PRIO_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_SOURCE_TRUNK_ATTRIBUTE_E, PRV_CPSS_DXCH_UNIT_L2I_E,
    {0x03000000 + 0x00202000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_SOURCE_TRUNK_ATTRIBUTE_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_E, PRV_CPSS_DXCH_UNIT_L2I_E,
    {0x03000000 + 0x00200000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_PHYSICAL_PORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_E, PRV_CPSS_DXCH_UNIT_L2I_E,
    {0x03000000 + 0x00201000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

     /* EQ Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_EPORT_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00700000, 4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00040000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EQ_INGRESS_STC_PHYSICAL_PORT_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x0000B300, 4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_TRUNK_LTT_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00160000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_L2_ECMP_LTT_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00800000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_L2_ECMP_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00900000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00c00000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_TO_CPU_RATE_LIMITER_CONFIG_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00130000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_TX_PROTECTION_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00a00000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_LOC_MAPPING_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00a80000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_PROTECTION_LOC_E, PRV_CPSS_DXCH_UNIT_EQ_E,
    {0x0D000000 + 0x00b00000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* LPM memory */
    {PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E, PRV_CPSS_DXCH_UNIT_LPM_E,
    {0x58000000 + 0x00000000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* IPv4 VRF Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_IPV4_VRF_E, PRV_CPSS_DXCH_UNIT_LPM_E,
    {0x58000000 + 0x00D10000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* IPv6 VRF Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_IPV6_VRF_E, PRV_CPSS_DXCH_UNIT_LPM_E,
    {0x58000000 + 0x00D20000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* FCoE VRF Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_FCOE_VRF_E, PRV_CPSS_DXCH_UNIT_LPM_E,
    {0x58000000 + 0x00D30000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* ECMP Table */
    {PRV_CPSS_DXCH_LION3_TABLE_ECMP_E, PRV_CPSS_DXCH_UNIT_LPM_E,
    {0x58000000 + 0x00D40000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* EGF_EFT Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_EFT_FAST_STACK_FAILOVER_SECONDARY_TARGET_PORT_E, PRV_CPSS_DXCH_UNIT_EGF_EFT_E,
    {0x35000000 + 0x00000000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* EGF_QAG Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E, PRV_CPSS_DXCH_UNIT_EGF_QAG_E,
    {0x3B000000 + 0x00800000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_QAG_EGRESS_EPORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_VLAN_E, PRV_CPSS_DXCH_UNIT_EGF_QAG_E,
    {0x3B000000 + 0x00000000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_QAG_EGRESS_EVLAN_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_TARGET_PORT_MAPPER_E, PRV_CPSS_DXCH_UNIT_EGF_QAG_E,
    {0x3B000000 + 0x00900000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_QAG_TARGET_PORT_MAPPER_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_CPU_CODE_TO_LOOPBACK_MAPPER_E, PRV_CPSS_DXCH_UNIT_EGF_QAG_E,
    {0x3B000000 + 0x00900400, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_QAG_CPU_CODE_TO_LOOPBACK_MAPPER_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_E, PRV_CPSS_DXCH_UNIT_EGF_QAG_E,
    {0x3B000000 + 0x00900800, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_QAG_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* EGF_SHT Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EGRESS_EPORT_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x02200000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_SHT_EGRESS_EPORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_DEVICE_MAP_TABLE_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x02100000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x03000000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_SHT_EGRESS_VLAN_BITS_SIZE_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x03400000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_SHT_EGRESS_VLAN_ATTRIBUTES_BITS_SIZE_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_SPANNING_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x03440000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_SHT_EGRESS_VLAN_SPANNING_BITS_SIZE_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_MAPPER_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x02110000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGF_SHT_EGRESS_VLAN_MAPPER_BITS_SIZE_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_MULTICAST_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x020C0000, BITS_TO_BYTES_ALIGNMENT_MAC(BMP_256_PORTS_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x02040000, BITS_TO_BYTES_ALIGNMENT_MAC(BMP_256_PORTS_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x00000000, BITS_TO_BYTES_ALIGNMENT_MAC(BMP_256_PORTS_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x04000000, BITS_TO_BYTES_ALIGNMENT_MAC(BMP_256_PORTS_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x02080000, BITS_TO_BYTES_ALIGNMENT_MAC(BMP_256_PORTS_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x020E0000, BITS_TO_BYTES_ALIGNMENT_MAC(BMP_256_PORTS_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_NON_TRUNK_MEMBERS2_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x02060000, BITS_TO_BYTES_ALIGNMENT_MAC(BMP_256_PORTS_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EPORT_FILTER_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x020A0000, BITS_TO_BYTES_ALIGNMENT_MAC(BMP_256_PORTS_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E, PRV_CPSS_DXCH_UNIT_EGF_SHT_E,
    {0x48000000 + 0x02120000, BITS_TO_BYTES_ALIGNMENT_MAC(BMP_256_PORTS_CNS) , PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* HA Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00100000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_EGRESS_EPORT_1_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00030000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_EGRESS_EPORT_2_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00001000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_EGRESS_PHYSICAL_PORT_1_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00002000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_EGRESS_PHYSICAL_PORT_2_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_HA_QOS_PROFILE_TO_EXP_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00004000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_QOS_PROFILE_TO_EXP_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_HA_EPCL_UDB_CONFIG_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00060000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_EPCL_UDB_CONFIG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_HA_PTP_DOMAIN_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00040000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_PTP_DOMAIN_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_HA_GENERIC_TS_PROFILE_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00050000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_GEN_TS_PROFILE_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_HA_GLOBAL_MAC_SA_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00003000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_GLOBAL_MAC_SA_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00200000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_TUNNEL_START_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00010000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_EGRESS_VLAN_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E, PRV_CPSS_DXCH_UNIT_HA_E,
    {0x0F000000 + 0x00020000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_EGRESS_VLAN_MAC_SA_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},


    /* MLL Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E, PRV_CPSS_DXCH_UNIT_MLL_E,
    {0x1D000000 + 0x00080000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_MLL_ENTRY_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_E,/* must be the same as 'IP_MLL' */ PRV_CPSS_DXCH_UNIT_MLL_E,
    {0x1D000000 + 0x00080000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_MLL_ENTRY_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_LTT_E, PRV_CPSS_DXCH_UNIT_MLL_E,
    {0x1D000000 + 0x00040000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2_MLL_LTT_ENTRY_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*IPCL tables */
    {PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E, PRV_CPSS_DXCH_UNIT_PCL_E ,/* table for first lookup */
    {0x02000000 + 0x00010000,BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PCL_PCL_CONFIG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E, PRV_CPSS_DXCH_UNIT_PCL_E ,  /* table for second lookup  */
    {0x02000000 + 0x00020000,BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PCL_PCL_CONFIG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E, PRV_CPSS_DXCH_UNIT_PCL_E ,  /* table for third lookup */
    {0x02000000 + 0x00030000,BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PCL_PCL_CONFIG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E, PRV_CPSS_DXCH_UNIT_PCL_E ,
    {0x02000000 + 0x00000C00 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PCL_TRUNK_HASH_MASK_CRC_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_IPCL0_UDB_SELECT_E, PRV_CPSS_DXCH_UNIT_PCL_E ,
    {0x02000000 + 0x00040000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPCL_UDB_SELECT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_IPCL1_UDB_SELECT_E, PRV_CPSS_DXCH_UNIT_PCL_E ,
    {0x02000000 + 0x00042000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPCL_UDB_SELECT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_IPCL2_UDB_SELECT_E, PRV_CPSS_DXCH_UNIT_PCL_E ,
    {0x02000000 + 0x00044000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPCL_UDB_SELECT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E, PRV_CPSS_DXCH_UNIT_TCAM_E ,
    {0x05000000 + 0x00200000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TCAM_PCL_TTI_ACTION_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_TIMESTAMP_CFG_E, PRV_CPSS_DXCH_UNIT_ERMRK_E ,
    {0x15000000 + 0x00005000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_ERMRK_TIMESTAMP_CFG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_LOCAL_ACTION_E, PRV_CPSS_DXCH_UNIT_ERMRK_E ,
    {0x15000000 + 0x00100000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_ERMRK_PTP_LOCAL_ACTION_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E, PRV_CPSS_DXCH_UNIT_ERMRK_E ,
    {0x15000000 + 0x00200000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_ERMRK_PTP_TARGET_PORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E, PRV_CPSS_DXCH_UNIT_ERMRK_E ,
    {0x15000000 + 0x00300000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_ERMRK_PTP_SOURCE_PORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_QOS_DSCP_MAP_E, PRV_CPSS_DXCH_UNIT_ERMRK_E ,
    {0x15000000 + 0x00003000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_ERMRK_QOS_DSCP_MAP_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_QOS_TC_DP_MAP_E, PRV_CPSS_DXCH_UNIT_ERMRK_E ,
    {0x15000000 + 0x00003300, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_ERMRK_QOS_TC_DP_MAP_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},


    {PRV_CPSS_DXCH_LION3_TABLE_TCAM_E, PRV_CPSS_DXCH_UNIT_TCAM_E ,
    {0x05000000 + 0x00000000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TCAM_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* TRILL Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_ADJACENCY_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_ADJACENCY_E */
    {0x01000000 + 0x00006000, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_RBID_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00080000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* Policer Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_E_ATTRIBUTES_E, PRV_CPSS_DXCH_UNIT_IPLR_E,
    {0x0B000000 + 0x10000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_REMARKING_E, PRV_CPSS_DXCH_UNIT_IPLR_E,
    {0x0B000000 + 0x80000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_REMARKING_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_HIERARCHICAL_E, PRV_CPSS_DXCH_UNIT_IPLR_E,
    {0x0B000000 + 0x90000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_HIERARCHICAL_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_METERING_E, PRV_CPSS_DXCH_UNIT_IPLR_E,
    {0x0B000000 + 0x100000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_METERING_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_1_E_ATTRIBUTES_E, PRV_CPSS_DXCH_UNIT_IPLR_1_E,
    {0x20000000 + 0x10000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_1_REMARKING_E, PRV_CPSS_DXCH_UNIT_IPLR_1_E,
    {0x20000000 + 0x80000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_REMARKING_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_1_METERING_E, PRV_CPSS_DXCH_UNIT_IPLR_1_E,
    {0x20000000 + 0x100000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_METERING_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_POLICER_E_ATTRIBUTES_E, PRV_CPSS_DXCH_UNIT_EPLR_E,
    {0x0E000000 + 0x10000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E, PRV_CPSS_DXCH_UNIT_EPLR_E,
    {0x0E000000 + 0x80000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_REMARKING_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_POLICER_METERING_E, PRV_CPSS_DXCH_UNIT_EPLR_E,
    {0x0E000000 + 0x100000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_METERING_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* IPVX Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_INGRESS_EPORT_E, PRV_CPSS_DXCH_UNIT_IPVX_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_IPVX_INGRESS_EPORT_E */
    {0x06000000 + 0x00200000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_ROUTER_EPORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_EVLAN_E, PRV_CPSS_DXCH_UNIT_IPVX_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_IPVX_EVLAN_E */
    {0x06000000 + 0x00100000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_ROUTER_EVLAN_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E, PRV_CPSS_DXCH_UNIT_IPVX_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E */
    {0x06000000 + 0x00400000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_ROUTER_NEXTHOP_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E, PRV_CPSS_DXCH_UNIT_IPVX_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E */
    {0x06000000 + 0x00020000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_ROUTER_NEXTHOP_AGE_BITS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E, PRV_CPSS_DXCH_UNIT_IPVX_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E */
    {0x06000000 + 0x00060000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_ROUTER_ACCESS_MATRIX_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_QOS_PROFILE_OFFSETS_E, PRV_CPSS_DXCH_UNIT_IPVX_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_QOS_PROFILE_OFFSETS_E */
    {0x06000000 + 0x00010000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_ROUTER_QOS_PROFILE_OFFSETS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_E */
    {0x1C000000 + 0x00070000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_AGING_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_AGING_E */
    {0x1C000000 + 0x00000C00, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_MEG_EXCEPTION_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_MEG_EXCEPTION_E */
    {0x1C000000 + 0x00010000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E */
    {0x1C000000 + 0x00018000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_INVALID_KEEPALIVE_HASH_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_INVALID_KEEPALIVE_HASH_E */
    {0x1C000000 + 0x00020000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_EXCESS_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_EXCESS_E */
    {0x1C000000 + 0x00028000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_EXCEPTION_SUMMARY_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_EXCEPTION_SUMMARY_E */
    {0x1C000000 + 0x00030000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E */
    {0x1C000000 + 0x00038000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_TX_PERIOD_EXCEPTION_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_TX_PERIOD_EXCEPTION_E */
    {0x1C000000 + 0x00040000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_OPCODE_PACKET_COMMAND_E, PRV_CPSS_DXCH_UNIT_IOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_OPCODE_PACKET_COMMAND_E */
    {0x1C000000 + 0x00060000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_E */
    {0x1E000000 + 0x00070000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_AGING_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_AGING_E */
    {0x1E000000 + 0x00000C00, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_MEG_EXCEPTION_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_MEG_EXCEPTION_E */
    {0x1E000000 + 0x00010000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E */
    {0x1E000000 + 0x00018000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_INVALID_KEEPALIVE_HASH_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_INVALID_KEEPALIVE_HASH_E */
    {0x1E000000 + 0x00020000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_EXCESS_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_EXCESS_E */
    {0x1E000000 + 0x00028000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_EXCEPTION_SUMMARY_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_EXCEPTION_SUMMARY_E */
    {0x1E000000 + 0x00030000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E */
    {0x1E000000 + 0x00038000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_TX_PERIOD_EXCEPTION_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_TX_PERIOD_EXCEPTION_E */
    {0x1E000000 + 0x00040000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_OPCODE_PACKET_COMMAND_E, PRV_CPSS_DXCH_UNIT_EOAM_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_OPCODE_PACKET_COMMAND_E */
    {0x1E000000 + 0x00060000, 0x8, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_OAM_LM_OFFSET_E, PRV_CPSS_DXCH_UNIT_ERMRK_E,
    /* PRV_CPSS_DXCH_LION3_TABLE_OAM_LM_OFFSET_E */
    {0x15000000 + 0x00002000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TTI_QOS_MAP_DSCP_TO_QOS_PROFILE_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00000800, 0x80, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TTI_QOS_MAP_UP_CFI_TO_QOS_PROFILE_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00000E00, 0x20, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TTI_QOS_MAP_EXP_TO_QOS_PROFILE_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00000700, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TTI_QOS_MAP_DSCP_TO_DSCP_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00000400, 0x40, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TTI_UDB_CONFIG_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00040000, 0x40, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TTI_PTP_COMMAND_E, PRV_CPSS_DXCH_UNIT_TTI_E,
    {0x01000000 + 0x00004000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TTI_PTP_COMMAND_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* EPCL unit */
    {PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E, PRV_CPSS_DXCH_UNIT_EPCL_E,
    {0x14000000 + 0x00008000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EPCL_PCL_CONFIG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},


    {PRV_CPSS_DXCH_LION3_TABLE_EPCL_UDB_SELECT_E, PRV_CPSS_DXCH_UNIT_EPCL_E,
    {0x14000000 + 0x00010000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EPCL_UDB_SELECT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* BMA unit */
    {PRV_CPSS_DXCH_LION3_TABLE_BMA_PORT_MAPPING_E, PRV_CPSS_DXCH_UNIT_BMA_E,
    {0x2D000000 + 0x0001A000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_BMA_PORT_MAPPING_BIT_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_BMA_MULTICAST_COUNTERS_E, PRV_CPSS_DXCH_UNIT_BMA_E,
    {0x2D000000 + 0x00000000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_BMA_MULTICAST_COUNTERS_BIT_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* Tail Drop Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_MAX_QUEUE_LIMITS_E, PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,
    {0x3C000000 + 0x000A1000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TAIL_DROP_MAX_QUEUE_LIMITS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_EQ_QUEUE_LIMITS_DP0_E, PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,
    {0x3C000000 + 0x000A1800, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TAIL_DROP_EQ_QUEUE_LIMITS_DP0_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_DQ_QUEUE_BUF_LIMITS_E, PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,
    {0x3C000000 + 0x000A2000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TAIL_DROP_DQ_QUEUE_BUF_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_E, PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,
    {0x3C000000 + 0x000A2800, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_EQ_QUEUE_LIMITS_DP1_E, PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,
    {0x3C000000 + 0x000A3000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TAIL_DROP_EQ_QUEUE_LIMITS_DP1_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_EQ_QUEUE_LIMITS_DP2_E, PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,
    {0x3C000000 + 0x000A3400, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TAIL_DROP_EQ_QUEUE_LIMITS_DP2_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_COUNTERS_Q_MAIN_BUFF_E, PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,
    {0x3C000000 + 0x000A7000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TAIL_DROP_COUNTERS_MAIN_BUFF_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_COUNTERS_Q_MAIN_MC_BUFF_E, PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,
    {0x3C000000 + 0x000A8000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TAIL_DROP_COUNTERS_MAIN_MC_BUFF_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TXQ_LINK_LIST_COUNTERS_Q_MAIN_DESC_E, PRV_CPSS_DXCH_UNIT_TXQ_LL_E,
    {0x3D000000 + 0x0008C000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TXQ_LINK_LIST_COUNTERS_MAIN_DESC_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_E, PRV_CPSS_DXCH_UNIT_TXQ_PFC_E,
    {0x3E000000 + 0x00001800, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_E, PRV_CPSS_DXCH_UNIT_TXQ_PFC_E,
    {0x3E000000 + 0x00001A00, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_PFC_LLFC_COUNTERS_E, PRV_CPSS_DXCH_UNIT_TXQ_PFC_E,
    {0x3E000000 + 0x00002000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PFC_LLFC_COUNTERS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TM_DROP_DROP_MASKING_E, PRV_CPSS_DXCH_UNIT_TM_DROP_E,
    {0x09000000 + 0x00001000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_DROP_DROP_MASKING_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TM_DROP_QUEUE_PROFILE_ID_E, PRV_CPSS_DXCH_UNIT_TM_DROP_E,
    {0x09000000 + 0x00010000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_DROP_QUEUE_PROFILE_ID_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_CPU_CODE_TO_TC_E, PRV_CPSS_DXCH_UNIT_TM_QMAP_E,
    {0x0A000000 + 0x00001000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_QMAP_CPU_CODE_TO_TC_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_TARGET_DEV_TO_INDEX_E, PRV_CPSS_DXCH_UNIT_TM_QMAP_E,
    {0x0A000000 + 0x00010000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_QMAP_TARGET_DEV_TO_INDEX_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_POLICING_ENABLE_E, PRV_CPSS_DXCH_UNIT_TM_QMAP_E,
    {0x0A000000 + 0x00020000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_QMAP_TM_QMAP_POLICING_ENABLE_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_QUEUE_ID_SELECTOR_E, PRV_CPSS_DXCH_UNIT_TM_QMAP_E,
    {0x0A000000 + 0x00030000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_QMAP_QUEUE_ID_SELECTOR_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TM_INGR_GLUE_L1_PKT_LEN_OFFSET_E, PRV_CPSS_DXCH_UNIT_TM_INGRESS_GLUE_E,
    {0x18000000 + 0x00001000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_INGR_GLUE_L1_PKT_LEN_OFFSET_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TM_EGR_GLUE_AGING_QUEUE_PROFILE_E, PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E,
    {0x19000000 + 0x00004000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_EGR_GLUE_AGING_QUEUE_PROFILE_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_AGING_PROFILE_THESHOLDS_E*/
    {PRV_CPSS_DXCH_LION3_TABLE_TM_EGR_GLUE_AGING_PROFILE_THESHOLDS_E, PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E,
    {0x19000000 + 0x00006000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_EGR_GLUE_AGING_PROFILE_THESHOLDS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_TARGET_INTERFACE_E*/
    {PRV_CPSS_DXCH_LION3_TABLE_TM_EGR_GLUE_TARGET_INTERFACE_E, PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E,
    {0x19000000 + 0x00006200, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_EGR_GLUE_TARGET_INTERFACE_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_E*/
    {PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_E, PRV_CPSS_DXCH_UNIT_TM_FCU_E,
    {0x08000000 + 0x00000500, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E*/
    {PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E, PRV_CPSS_DXCH_UNIT_TM_FCU_E,
    {0x08000000 + 0x00000700, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*PRV_DXCH_BOBCAT2_TABLE_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_E*/
    {PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_E, PRV_CPSS_DXCH_UNIT_TM_FCU_E,
    {0x08000000 + 0x00001000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*PRV_DXCH_BOBCAT2_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E*/
    {PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E, PRV_CPSS_DXCH_UNIT_TM_FCU_E,
    {0x08000000 + 0x00003000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*PRV_DXCH_BOBCAT2_TABLE_TM_FCU_INGRESS_TIMERS_E*/
    {PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_INGRESS_TIMERS_E, PRV_CPSS_DXCH_UNIT_TM_FCU_E,
    {0x08000000 + 0x00008000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_FCU_INGRESS_TIMERS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*PRV_DXCH_BOBCAT2_TABLE_TM_FCU_PFC_PORT_TO_PHYSICAL_PORT_MAPPING_E*/
    {PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_PORT_TO_PHYSICAL_PORT_MAPPING_E, PRV_CPSS_DXCH_UNIT_TM_FCU_E,
    {0x08000000 + 0x00000100, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TM_FCU_PORT_TO_PHYSICAL_PORT_MAPPING_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*PRV_DXCH_BOBCAT2_TABLE_CN_SAMPLE_INTERVALS_E*/
    {PRV_CPSS_DXCH_LION3_TABLE_CN_SAMPLE_INTERVALS_E, PRV_CPSS_DXCH_UNIT_TXQ_QCN_E,
    {0x3F000000 + 0x00002000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_CN_SAMPLE_INTERVALS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},
};

/* Bobcat2 B0 direct table information , that need to override A0 info from directAccessBobcat2TableInfo */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_EXT_STC directAccessBobcat2B0TableInfo_overrideA0[] =
{
    /* TTI  */
    {PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
    {0x01000000 + 0x00010000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,
    {0x01000000 + 0x00240000 , 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E,
    {0x01000000 + 0x00050000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_SPAN_STATE_GROUP_INDEX_E,
    {0x03000000 + 0x00218000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_INGRESS_SPAN_STATE_GROUP_INDEX_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* L2 */
    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
    {0x03000000 + 0x00300000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_L2I_EPORT_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

     /* PLR */
    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_E_ATTRIBUTES_E,
    {0x0B000000 + 0x00040000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_1_E_ATTRIBUTES_E,
    {0x20000000 + 0x00040000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_POLICER_E_ATTRIBUTES_E,
    {0x0E000000 + 0x00040000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* TXQ */
    {PRV_CPSS_DXCH_LION3_TABLE_TXQ_LINK_LIST_COUNTERS_Q_MAIN_DESC_E,
    {0x3D000000 + 0x0008D000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_TXQ_LINK_LIST_COUNTERS_MAIN_DESC_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* HA */
    {PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
    {0x0F000000 + 0x00100000, BITS_TO_BYTES_ALIGNMENT_MAC(BOBCAT_B0_HA_EGRESS_EPORT_1_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
    {0x0F000000 + 0x00200000, BITS_TO_BYTES_ALIGNMENT_MAC(BOBCAT_B0_HA_EGRESS_EPORT_2_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
    {0x0F000000 + 0x00400000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_HA_TUNNEL_START_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /* ERMRK */
    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_TIMESTAMP_CFG_E,
    {0x15000000 + 0x00005000, BITS_TO_BYTES_ALIGNMENT_MAC(BOBCAT2_B0_ERMRK_TIMESTAMP_CFG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}

    /*CPFC*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_PFC_LLFC_COUNTERS_E,
    {0x3e000000 + 0x00008000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PFC_LLFC_COUNTERS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}


};

/* XCat A1 revision Logical Target Mapping table indirect info */
static PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC xcatA1LogicalTargetMappingTblInfo =
    {0x0B0000A0, 0x0B00A000, 0, 2, 0, 0, 1};

/* XCat C0 revision Egress Filter Vlan Mapping table indirect info */
static PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC xcatC0EgressFilterVlanMappingTblInfo =
    {0x0B0000A4, 0x0B0000A8, 0, 2, 0, 0, 1};

/* XCat C0 revision Egress Filter Vlan Member table indirect info */
static PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC xcatC0EgressFilterVlanMemberTblInfo =
    {0x0B0000AC, 0x0B0000B0, 0, 2, 0, 0, 1};

/* XCat C0 revision L2Mll vidx enable table indirect info */
static PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC xcatC0L2MllVidxEnableTblInfo =
    {0x0D800464, 0x0D800460, 0, 2, 0, 0, 1};

/* XCat C0 revision L2Mll pointer map table indirect info */
static PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC xcatC0L2MllPointerMapTblInfo =
    {0x0D800474, 0x0D800470, 0, 2, 0, 0, 1};


/* Cheetah's table information */
static PRV_CPSS_DXCH_TABLES_INFO_STC cheetahTablesInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS ,PRV_CPSS_DXCH_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                         PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
    PRV_CPSS_DXCH_INDIRECT_ACCESS_E, (indirectAccessTableInfo +
                                       PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E)},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_VLAN_PORT_QOS_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {PRV_CPSS_DXCH_TRUNK_MEMBERS_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_TRUNK_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                             PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                             PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                     PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
    PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                     PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
      PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_CPU_CODE_E},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {PRV_CPSS_DXCH_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                               PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {PRV_CPSS_DXCH_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
       PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                              PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E},
    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {PRV_CPSS_DXCH_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_REMARKING_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_REMARKING_E},
    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {PRV_CPSS_DXCH_STG_MAX_NUM_CNS, PRV_CPSS_DXCH_STG_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                        PRV_CPSS_DXCH_TABLE_STG_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS, PRV_CPSS_DXCH_VLAN_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_VLAN_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_VLAN_E},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS, PRV_CPSS_DXCH_MULTICAST_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_MULTICAST_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_MULTICAST_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS,  PRV_CPSS_DXCH_ROUTE_HA_MAC_SA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                            PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {PRV_CPSS_DXCH_FDB_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                       PRV_CPSS_DXCH_TABLE_FDB_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                       PRV_CPSS_DXCH_TABLE_FDB_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E */
    {PRV_CPSS_DXCH_POLICER_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_POLICER_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                   PRV_CPSS_DXCH_TABLE_POLICER_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                   PRV_CPSS_DXCH_TABLE_POLICER_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {PRV_CPSS_DXCH_POLICER_COUNTERS_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_POLICER_COUNTERS_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E}

};

static const GT_U32 cheetahTableInfoSize =
                          NUM_ELEMENTS_IN_ARR_MAC(cheetahTablesInfo);



/* Cheetah2's table information */
/* All CH2 tables beside FDB and VLAN has direct access by PSS default.
   FDB access is indirect to save entries consistency in the read or
   write under traffic.
   VLAN write access is indirect to save entries consistency in the write under
   traffic only. There is no inconsistency in the read VLAN because this
   table may be changed only by CPU.

   Errata issues:
   1)
   Due to Cheetah 2 Errata - "4.1 Errata (FEr# ) Direct Access to Policer space"
   -- see PRV_CPSS_DXCH2_DIRECT_ACCESS_TO_POLICER_ADDRESS_SPACE_WA_E
   indirect access should be used for:
         PRV_CPSS_DXCH_TABLE_REMARKING_E
         PRV_CPSS_DXCH_TABLE_POLICER_E
         PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E
   2)
   Due to Cheetah 2 Errata - Router ARP and Tunnel Start table do not support
   burst access via PCI (FEr#1003)
   -- see PRV_CPSS_DXCH2_ROUTER_ARP_AND_TS_TABLE_NOT_SUPPORT_BURST_ACCESS_VIA_PCI_WA_E
   The device supports burst reads from its internal memory address
   space. However, due to this erratum, the Router Address
   Resolution Protocol (ARP) and Tunnel Start (TS) table,
   0x07E80000 - 0x07E87FF4, cannot be accessed by using the burst
   access feature.
   NOTE : the CPSS_DXCH currently not support PCI bursts (so no issue here).

   3)
   Due to Cheetah 2 Errata -
   RdWrTrig, bit[15] in the VLT Tables Access Control register is ignored (FEr#1007)
   -- see PRV_CPSS_DXCH2_RDWRTRIG_BIT_IN_VLT_TABLES_ACCESS_CONTROL_REGISTER_IGNORED_E
   No dummy write commands should be done to the VLT Tables Access Control
   register (offset: 0x0A000010).
   NOTE : no "dummy" writes done in the "non-direct" access engine of DXCH


*/

static PRV_CPSS_DXCH_TABLES_INFO_STC cheetah2TablesInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                        PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                       PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_VLAN_PORT_QOS_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                           + PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                          + PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {PRV_CPSS_DXCH_TRUNK_MEMBERS_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_TRUNK_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                           + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                          + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                   PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_CPU_CODE_E},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {PRV_CPSS_DXCH2_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                               PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                               PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                              PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E},
    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_REMARKING_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_REMARKING_E},
    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {PRV_CPSS_DXCH_STG_MAX_NUM_CNS, PRV_CPSS_DXCH_STG_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS,PRV_CPSS_DXCH2_VLAN_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                     PRV_CPSS_DXCH_TABLE_VLAN_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                    PRV_CPSS_DXCH2_TABLE_VLAN_E},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS, PRV_CPSS_DXCH_MULTICAST_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_MULTICAST_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_MULTICAST_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS, PRV_CPSS_DXCH_ROUTE_HA_MAC_SA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {PRV_CPSS_DXCH_FDB_MAX_ENTRIES_CNS , PRV_CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                       PRV_CPSS_DXCH_TABLE_FDB_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_FDB_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E */
    {PRV_CPSS_DXCH_POLICER_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_POLICER_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                   PRV_CPSS_DXCH_TABLE_POLICER_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_POLICER_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {PRV_CPSS_DXCH_POLICER_COUNTERS_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_POLICER_COUNTERS_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                         PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E},

    /* PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E */
    {PRV_CPSS_DXCH2_EGRESS_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS,
                                            PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                        PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                       PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E},
    /* PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
                                      PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                      PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                     PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E},

    /* PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E */
    {PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_TABLE_MAX_ENTRIES_CNS,
                                         PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                               PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                              PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E},

    /* PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E */
    {PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_TABLE_MAX_ENTRIES_CNS,
                                        PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                      PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                     PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E},

    /* PRV_CPSS_DXCH2_LTT_TT_ACTION_E */
    {PRV_CPSS_DXCH2_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH2_LTT_TT_ACTION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH2_LTT_TT_ACTION_E},

    /* PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E */
    {PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_TABLE_MAX_ENTRIES_CNS,
                                       PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E},



     /* PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E */
    {PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E}
};


static const GT_U32 cheetah2TableInfoSize =
                       NUM_ELEMENTS_IN_ARR_MAC(cheetah2TablesInfo);

/* Cheetah3's table information */
/* All CH3 tables beside FDB and VLAN has direct access by PSS default.
   FDB access is indirect to save entries consistency in the read or
   write under traffic.
   VLAN write access is indirect to save entries consistency in the write under
   traffic only. There is no inconsistency in the read VLAN because this
   table may be changed only by CPU.

*/

static PRV_CPSS_DXCH_TABLES_INFO_STC cheetah3TablesInfo[] =
{
    /* PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                        PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                       PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E},

    /* PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E */
    {PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS , PRV_CPSS_DXCH_VLAN_PORT_QOS_ENTRY_WORDS_COUNT_CNS ,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                           + PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                          + PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E},

    /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
    {PRV_CPSS_DXCH_TRUNK_MEMBERS_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_TRUNK_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                           + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo
                                          + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E},

    /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
    {PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                   PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E},

    /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
    {PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_CPU_CODE_E},

    /* PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E */
    {PRV_CPSS_DXCH2_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH3_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, &dxCh3IpclCfgTblInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, &dxCh3IpclCfgTblInfo},

    /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
    {PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                               PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                              PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E},
    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS, PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH_TABLE_REMARKING_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_REMARKING_E},
    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {PRV_CPSS_DXCH_STG_MAX_NUM_CNS, PRV_CPSS_DXCH_STG_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_STG_E},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS,PRV_CPSS_DXCH2_VLAN_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                     PRV_CPSS_DXCH_TABLE_VLAN_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                    PRV_CPSS_DXCH2_TABLE_VLAN_E},

    /* PRV_CPSS_DXCH_TABLE_MULTICAST_E */
    {PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS, PRV_CPSS_DXCH_MULTICAST_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_MULTICAST_E,
    PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                PRV_CPSS_DXCH_TABLE_MULTICAST_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E */
    {PRV_CPSS_DXCH_MAX_VLANS_NUM_CNS, PRV_CPSS_DXCH_ROUTE_HA_MAC_SA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E},

    /* PRV_CPSS_DXCH_TABLE_FDB_E */
    {PRV_CPSS_DXCH3_FDB_MAX_ENTRIES_CNS , PRV_CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                       PRV_CPSS_DXCH_TABLE_FDB_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, indirectAccessTableInfo +
                                                      PRV_CPSS_DXCH_TABLE_FDB_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_E - not supported for CH3.
        The dedicated algorithm is used for the table */
    {0, 0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                   PRV_CPSS_DXCH_TABLE_POLICER_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH_TABLE_POLICER_E},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E  - not supported for CH3.
        The dedicated algorithm is used for the table */
    {0, 0,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E, directAccessTableInfo +
                                         PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E},

    /* PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E */
    {PRV_CPSS_DXCH2_EGRESS_PCL_CONFIG_TABLE_MAX_ENTRIES_CNS,
                                            PRV_CPSS_DXCH_PCL_CONFIG_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, &dxCh3EpclCfgTblInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, &dxCh3EpclCfgTblInfo},

    /* PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E */
    {PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
                                      PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                      PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                     PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E},

    /* PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E */
    {PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_TABLE_MAX_ENTRIES_CNS,
                                         PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                               PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                              PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E},

    /* PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E */
    {PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_TABLE_MAX_ENTRIES_CNS,
                                        PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                      PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                     PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E},

    /* PRV_CPSS_DXCH2_LTT_TT_ACTION_E */
    {PRV_CPSS_DXCH2_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH2_LTT_TT_ACTION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH2_LTT_TT_ACTION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH2_LTT_TT_ACTION_E},

    /* PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E */
    {PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_TABLE_MAX_ENTRIES_CNS,
                                       PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                           PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                          PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E},

     /* PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E */
    {PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E},

    /*PRV_CPSS_DXCH3_TABLE_MAC2ME_E */
    {PRV_CPSS_DXCH3_MAC2ME_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH3_MAC2ME_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_MAC2ME_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_MAC2ME_E},

     /* PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E */
    {PRV_CPSS_DXCH3_VLAN_TRANSLATION_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH3_VLAN_TRANSLATION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E},

     /* PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E */
    {PRV_CPSS_DXCH3_VLAN_TRANSLATION_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH3_VLAN_TRANSLATION_WORDS_COUNT_CNS,\
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                            PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E},

    /* PRV_CPSS_DXCH3_TABLE_VRF_ID_E */
    {PRV_CPSS_DXCH3_VRF_ID_TABLE_MAX_ENTRIES_CNS,
                                    PRV_CPSS_DXCH3_VRF_ID_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                             PRV_CPSS_DXCH3_TABLE_VRF_ID_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                            PRV_CPSS_DXCH3_TABLE_VRF_ID_E},

    /* PRV_CPSS_DXCH3_LTT_TT_ACTION_E */
    {PRV_CPSS_DXCH3_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS, PRV_CPSS_DXCH3_LTT_TT_ACTION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                  PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E, directAccessTableInfo +
                                                 PRV_CPSS_DXCH3_LTT_TT_ACTION_E}

};

#define  CHEETAH3_TABLE_INFO_SIZE_CNS (NUM_ELEMENTS_IN_ARR_MAC(cheetah3TablesInfo))

static const GT_U32 cheetah3TableInfoSize = CHEETAH3_TABLE_INFO_SIZE_CNS;


/* xCat's table information */
/* All xCat tables beside FDB and VLAN has direct access by CPSS default.
   FDB access is indirect to save entries consistency in the read or
   write under traffic.
   VLAN write access is indirect to save entries consistency in the write under
   traffic only. There is no inconsistency in the read VLAN because this
   table may be changed only by CPU.
*/

#define XCAT_TABLES_NUM_CNS  CHEETAH3_TABLE_INFO_SIZE_CNS

/* xCatA1 table is greater the the CH3's table */
static PRV_CPSS_DXCH_TABLES_INFO_STC dxChXcatTablesInfo[PRV_CPSS_DXCH_XCAT_TABLE_LAST_E] =
{
    {0}
    /* in runtime the tables of the ch3 will be copied here ,
       and will be updated by the relevant xCat Rev1 code -->
       see dxChXcatInitTable() */

    /* in runtime the 'Extra tables' of the xCat will be copied here*/


};

static const GT_U32 dxChXcatTableInfoSize = PRV_CPSS_DXCH_XCAT_TABLE_LAST_E;

static PRV_CPSS_DXCH_TABLES_INFO_STC dxChXcat3TablesInfo[PRV_CPSS_DXCH_XCAT3_TABLE_LAST_E] =
{
    /*PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E*/
    {   _1K,
        PRV_CPSS_DXCHXCAT_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_VLAN_PORT_PROTOCOL_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_VLAN_PORT_PROTOCOL_E]},

    /*PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E*/
    {   PRV_CPSS_DXCH_PORT_TBLS_MAX_INDEX_CNS,
        3,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_PORT_VLAN_QOS_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_PORT_VLAN_QOS_E]},

    /*PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E*/
    {   PRV_CPSS_DXCH_TRUNK_MEMBERS_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH_TRUNK_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_TRUNK_MEMBERS_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_TRUNK_MEMBERS_E]},

    /*PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E*/
    {   PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_STATISTICAL_RATE_LIMIT_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_STATISTICAL_RATE_LIMIT_E]},

    /*PRV_CPSS_DXCH_TABLE_CPU_CODE_E*/
    {   PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_CPU_CODE_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_CPU_CODE_E]},

    /*PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E*/     /*first lookup (00)*/
    {   4224,
        2,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_IPCL_CFG_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_IPCL_CFG_E]},

    /*PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E*/
    {   PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS,
        PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_E]},

    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
    },

    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {   PRV_CPSS_DXCH_STG_MAX_NUM_CNS,
        PRV_CPSS_DXCH_STG_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_STG_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_STG_E]},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {   PRV_CPSS_DXCH_MAX_NUM_VLANS_CNS,
        PRV_CPSS_DXCHXCAT_VLAN_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_VLAN_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_VLAN_E]},

    /*PRV_CPSS_DXCH_TABLE_MULTICAST_E*/
    {   _12K,
        PRV_CPSS_DXCH_MULTICAST_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_MULTICAST_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_MULTICAST_E]},

    /*PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E*/
    {   4160,
        PRV_CPSS_DXCH_ROUTE_HA_MAC_SA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_ROUTE_HA_MAC_SA_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_ROUTE_HA_MAC_SA_E]},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {   PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_ROUTE_HA_ARP_DA_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_ROUTE_HA_ARP_DA_E]},

    /*PRV_CPSS_DXCH_TABLE_FDB_E*/
    {   _16K,
        PRV_CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_FDB_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_FDB_E]},

    /*PRV_CPSS_DXCH_TABLE_POLICER_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
    },

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
    },

    /* PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E */
    {   4160,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_EPCL_CFG_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_EPCL_CFG_E]},

    /*PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E*/
    {   PRV_CPSS_DXCH_ROUTE_HA_ARP_DA_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH2_ROUTE_HA_ARP_DA_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_TUNNEL_START_CFG_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_TUNNEL_START_CFG_E]},

    /* PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E */
    {   PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH2_QOS_TO_ROUTE_BLOCK_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E]},

    /* PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E */
    {   PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH2_ROUTE_ACCESS_MATRIX_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_ROUTE_ACCESS_MATRIX_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_ROUTE_ACCESS_MATRIX_E]},

    /* PRV_CPSS_DXCH2_LTT_TT_ACTION_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
    },

    /* PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E */
    {   PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_UC_MC_ROUTE_NEXT_HOP_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_UC_MC_ROUTE_NEXT_HOP_E]},

    /* PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E */
    {   64,
        PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_ROUTE_NEXT_HOP_AGE_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_ROUTE_NEXT_HOP_AGE_E]},

    /*PRV_CPSS_DXCH3_TABLE_MAC2ME_E*/
    {   PRV_CPSS_DXCH3_MAC2ME_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH3_MAC2ME_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_MAC2ME_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_MAC2ME_E]},


    /*PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E*/
    {   PRV_CPSS_DXCH3_VLAN_TRANSLATION_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH3_VLAN_TRANSLATION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_INGRESS_TRANSLATION_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_INGRESS_TRANSLATION_E]},

    /*PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E*/
    {   PRV_CPSS_DXCH3_VLAN_TRANSLATION_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH3_VLAN_TRANSLATION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_EGRESS_TRANSLATION_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_EGRESS_TRANSLATION_E]},

    /*PRV_CPSS_DXCH3_TABLE_VRF_ID_E*/
    {   PRV_CPSS_DXCH3_VRF_ID_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH3_VRF_ID_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_VRF_ID_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_VRF_ID_E]},

    /*PRV_CPSS_DXCH3_LTT_TT_ACTION_E*/
    {   PRV_CPSS_DXCH3_LTT_TT_ACTION_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_XCAT_LTT_TT_ACTION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_LTT_TT_ACTION_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_LTT_TT_ACTION_E]},

    /*PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E*//*third lookup(1)*/
    {   4224,
        2,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_IPCL_LOOKUP1_CFG_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_IPCL_LOOKUP1_CFG_E]},

    /*PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E*/
    {   8,
        8,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_IPCL_UDB_CFG_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_IPCL_UDB_CFG_E]},

    /*PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E*/
    {   2048,
        2,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_LOGICAL_TARGET_MAPPING_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_LOGICAL_TARGET_MAPPING_E]},

    /*PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E*/
    {   8,
        4,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_BCN_PROFILE_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_BCN_PROFILE_E]},

    /*PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E*/
    {   80,
        2,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_EGRESS_POLICER_REMARKING_E],
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_EGRESS_POLICER_REMARKING_E]},

    /*PRV_CPSS_DXCH_XCAT_TABLE_EQ_VLAN_MAPPING_E*/
    {   4096,
        1,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_VLAN_MAPPING_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_VLAN_MAPPING_E]},

    /*PRV_CPSS_DXCH_XCAT_TABLE_EQ_LP_EGRESS_VLAN_MEMBER_E*/
    {   2048,
        2,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_LP_EGRESS_VLAN_MEMBER_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_LP_EGRESS_VLAN_MEMBER_E]},

    /*PRV_CPSS_DXCH_XCAT_TABLE_MLL_L2MLL_VIDX_ENABLE_E*/
    {   128,
        1,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_L2MLL_VIDX_ENABLE_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_L2MLL_VIDX_ENABLE_E]},

    /*PRV_CPSS_DXCH_XCAT_TABLE_MLL_L2MLL_POINTER_MAPPING_E,*/
    {   1024,
        1,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_L2MLL_POINTER_MAPPING_E],
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        &indirectAccessXcat3TableInfo[PRV_DXCH_XCAT3_TABLE_L2MLL_POINTER_MAPPING_E]}

};

static const GT_U32 dxChXcat3TableInfoSize =
    (NUM_ELEMENTS_IN_ARR_MAC(dxChXcat3TablesInfo));

/* xCat2 table is greater the the XCAT's table */
static PRV_CPSS_DXCH_TABLES_INFO_STC dxChXcat2TablesInfo[PRV_CPSS_DXCH_XCAT2_TABLE_LAST_E] =
{
    {0}
    /* in runtime the tables of the xCat Rev1 will be copied here ,
       and will be updated by the relevant xCat2 code -->
       see dxChXcat2InitTable() */

    /* in runtime the 'Extra tables' of the xCat2 will be copied here*/


};

static const GT_U32 dxChXcat2TableInfoSize = PRV_CPSS_DXCH_XCAT2_TABLE_LAST_E;

static void dxChXcatInitTable(void);
static void dxChXcat2InitTable(void);
/* Lion */
#define LION_TABLES_NUM_CNS  XCAT_TABLES_NUM_CNS
/* lion  B table*/
static PRV_CPSS_DXCH_TABLES_INFO_STC lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_LAST_E] =
{
    {0}
    /* in runtime the tables of the xcat will be copied here ,
       and will be updated by the relevant lion B code -->
       see lionInitTable() */

    /* in runtime the 'Extra tables' of the lion will be copied here*/


};

/* lion B */
static const GT_U32 lionBTableInfoSize = PRV_CPSS_DXCH_LION_TABLE_LAST_E;

/* lion  2 table*/
static PRV_CPSS_DXCH_TABLES_INFO_STC lion2TablesInfo[PRV_CPSS_DXCH_LION2_TABLE_LAST_E] =
{
    {0}
    /* in runtime the tables of the lionB will be copied here ,
       and will be updated by the relevant lion 2 code -->
       see lion2InitTable() */

    /* in runtime the 'Extra tables' of the lion 2 will be copied here*/


};

/* lion 2 */
static const GT_U32 lion2TableInfoSize = PRV_CPSS_DXCH_LION2_TABLE_LAST_E;

/* bobcat 2 table*/
static PRV_CPSS_DXCH_TABLES_INFO_STC bobcat2TablesInfo[PRV_CPSS_DXCH_BOBCAT2_TABLE_LAST_E] =
{
   /*PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E*/
    {   _8K, /*PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.tti.numDefaultEports * 16;*//*512 * 16 = _8K (16 protocols * 512 default ePorts)*/
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_VLAN_PORT_PROTOCOL_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_VLAN_PORT_PROTOCOL_E].directAccessInfo},

    /*PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*    PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E*/
    {   PRV_CPSS_DXCH_RATE_LIMIT_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH_RATE_LIMIT_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_STATISTICAL_RATE_LIMIT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_STATISTICAL_RATE_LIMIT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_TABLE_CPU_CODE_E*/
    {    PRV_CPSS_DXCH_CPU_CODE_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH_CPU_CODE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_CPU_CODE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_CPU_CODE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E*/     /*first lookup (00)*/
    {   4352 ,
        BITS_TO_WORDS_MAC(LION3_PCL_PCL_CONFIG_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E*/
    {   _1K,
        PRV_CPSS_DXCH_QOS_PROFILE_ENTRY_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_QOS_PROFILE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_QOS_PROFILE_E].directAccessInfo},

    /* PRV_CPSS_DXCH_TABLE_REMARKING_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /* PRV_CPSS_DXCH_TABLE_STG_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /* PRV_CPSS_DXCH_TABLE_VLAN_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        NULL},/*indirect*/

    /*PRV_CPSS_DXCH_TABLE_MULTICAST_E*/
    {   _4K,
        BITS_TO_WORDS_MAC(BMP_256_PORTS_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_MULTICAST_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_MULTICAST_E].directAccessInfo},

    /*PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E*/
    {   _4K,
        BITS_TO_WORDS_MAC(LION3_HA_EGRESS_VLAN_MAC_SA_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_ROUTE_MAC_SA_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_ROUTE_MAC_SA_E].directAccessInfo},

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_TABLE_FDB_E*/
    {   _256K ,
        BITS_TO_WORDS_MAC(LION3_FDB_FDB_BITS_SIZE_CNS) ,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        indirectAccessTableInfo + PRV_CPSS_LION3_FDB_TABLE_INDIRECT_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        indirectAccessTableInfo + PRV_CPSS_LION3_FDB_TABLE_INDIRECT_E},/*indirect*/

    /*PRV_CPSS_DXCH_TABLE_POLICER_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /* PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E */
    {   0,
        0,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        NULL},/*indirect*/

    /* PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E */
    {    4352,
        BITS_TO_WORDS_MAC(LION3_EPCL_PCL_CONFIG_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPCL_EGRESS_PCL_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPCL_EGRESS_PCL_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E*/
    {   _16K,
        BITS_TO_WORDS_MAC(LION3_HA_TUNNEL_START_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_TUNNEL_START_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_TUNNEL_START_CONFIG_E].directAccessInfo},

    /* PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /* PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /* PRV_CPSS_DXCH2_LTT_TT_ACTION_E */
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /* PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E */
    {    0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

     /* PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E */
    {    0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH3_TABLE_MAC2ME_E*/
    {   128,
        6, /* support 6 words */
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_MAC2ME_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_MAC2ME_E].directAccessInfo},

    /*PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E*/
    {   PRV_CPSS_DXCH3_VLAN_TRANSLATION_TABLE_MAX_ENTRIES_CNS,
        PRV_CPSS_DXCH3_VLAN_TRANSLATION_WORDS_COUNT_CNS,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_INGRESS_VLAN_TRANSLATION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_INGRESS_VLAN_TRANSLATION_E].directAccessInfo},

    /*PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_HA_EGRESS_VLAN_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_VLAN_TRANSLATION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_VLAN_TRANSLATION_E].directAccessInfo},

    /*PRV_CPSS_DXCH3_TABLE_VRF_ID_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH3_LTT_TT_ACTION_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E*//*third lookup(1)*/
    {   4352,
        BITS_TO_WORDS_MAC(LION3_PCL_PCL_CONFIG_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_INGRESS_LOOKUP1_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_INGRESS_LOOKUP1_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E*/
    {   16,
        BITS_TO_WORDS_MAC(LION3_TTI_PCL_UDB_BITS_SIZE_CNS),/* changed from 8 words to 18 words */
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_INGRESS_PCL_UDB_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_INGRESS_PCL_UDB_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E*/
    {   1024,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_REMARKING_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_REMARKING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_REMARKING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_XCAT_TABLE_EQ_VLAN_MAPPING_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_XCAT_TABLE_EQ_LP_EGRESS_VLAN_MEMBER_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_XCAT_MLL_L2MLL_VIDX_ENABLE_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_XCAT_MLL_L2MLL_POINTER_MAPPING_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E*/
    {   4352,
        BITS_TO_WORDS_MAC(LION3_PCL_PCL_CONFIG_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_INGRESS_LOOKUP01_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_INGRESS_LOOKUP01_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E*/
    {   (BIT_4 + LION3_PCL_NUM_FLOWS_CNS), /* num entries = 4 bit index + 16 'flows' */
        BITS_TO_WORDS_MAC(LION3_PCL_TRUNK_HASH_MASK_CRC_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_TRUNK_HASH_MASK_CRC_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_TRUNK_HASH_MASK_CRC_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_L2I_INGRESS_VLAN_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_VLAN_INGRESS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_VLAN_INGRESS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_EGF_SHT_EGRESS_VLAN_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_VLAN_EGRESS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_VLAN_EGRESS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E*/
    {   _4K,
        BITS_TO_WORDS_MAC(LION3_L2I_INGRESS_STP_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_STG_INGRESS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_STG_INGRESS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E*/
    {   _4K,
        BITS_TO_WORDS_MAC(BMP_256_PORTS_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_STG_EGRESS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_STG_EGRESS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E*/
    {   2176,
        BITS_TO_WORDS_MAC(BMP_256_PORTS_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_PORT_ISOLATION_L2_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_PORT_ISOLATION_L2_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E*/
    {   2176,
        BITS_TO_WORDS_MAC(BMP_256_PORTS_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_PORT_ISOLATION_L3_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_PORT_ISOLATION_L3_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E*/
    {   SIP5_SINGLE_PORT_GROUP_TXQ_DQ_PORTS_NUM_CNS,
        BITS_TO_WORDS_MAC(LION3_PRIO_TOKEN_BUCKET_CONFIGURATION_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E*/
    {   SIP5_SINGLE_PORT_GROUP_TXQ_DQ_PORTS_NUM_CNS,
        BITS_TO_WORDS_MAC(LION3_PORT_TOKEN_BUCKET_CONFIGURATION_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E*/
    {   _4K,
        BITS_TO_WORDS_MAC(BMP_256_PORTS_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_SOURCE_ID_MEMBERS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_SOURCE_ID_MEMBERS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E*/
    {   256,
        BITS_TO_WORDS_MAC(BMP_256_PORTS_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_NON_TRUNK_MEMBERS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_NON_TRUNK_MEMBERS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E*/
    {   128,
        BITS_TO_WORDS_MAC(BMP_256_PORTS_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_DESIGNATED_PORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_TXQ_DESIGNATED_PORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E*/
    {   SIP5_SINGLE_PORT_GROUP_TXQ_DQ_PORTS_NUM_CNS,
        3,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_EGRESS_STC_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_DQ_EGRESS_STC_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_LION_TABLE_LAST_E*/
    {   0,
        0,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        NULL},

    /*PRV_CPSS_DXCH_LION3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_TTI_PHYSICAL_PORT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E*/
    {   512,
        BITS_TO_WORDS_MAC(LION3_TTI_DEFAULT_PORT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_PRE_LOOKUP_INGRESS_EPORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_PRE_LOOKUP_INGRESS_EPORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E*/
    {  _8K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_POST_LOOKUP_INGRESS_EPORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_POST_LOOKUP_INGRESS_EPORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_L2I_EPORT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_INGRESS_EPORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_INGRESS_EPORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E*/
    {   _1K,
        BITS_TO_WORDS_MAC(LION3_L2I_EPORT_LEARN_PRIO_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_SOURCE_TRUNK_ATTRIBUTE_E*/
    {   512,
        BITS_TO_WORDS_MAC(LION3_L2I_SOURCE_TRUNK_ATTRIBUTE_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_SOURCE_TRUNK_ATTRIBUTE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_SOURCE_TRUNK_ATTRIBUTE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_L2I_PHYSICAL_PORT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_PHYSICAL_PORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_PHYSICAL_PORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_L2I_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_BRIDGE_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_EPORT_E*/
    {   _2K,/* 4 eports per entry */
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_EPORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_EPORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_EQ_INGRESS_STC_PHYSICAL_PORT_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E*/
    {   32,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_TRUNK_LTT_E*/
    {   _4K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_TRUNK_LTT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_TRUNK_LTT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_L2_ECMP_LTT_E*/
    {   _8K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_L2_ECMP_LTT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_L2_ECMP_LTT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_L2_ECMP_E*/
    {   _8K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_L2_ECMP_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_L2_ECMP_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E*/
    {   _8K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_TO_CPU_RATE_LIMITER_CONFIG_E*/
    {   256,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_TO_CPU_RATE_LIMITER_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_TO_CPU_RATE_LIMITER_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_TX_PROTECTION_E*/
    {   256,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_TX_PROTECTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_TX_PROTECTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_LOC_MAPPING_E*/
    {   _4K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_EPORT_TO_LOC_MAPPING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_EPORT_TO_LOC_MAPPING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EQ_PROTECTION_LOC_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_PROTECTION_LOC_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EQ_PROTECTION_LOC_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E*/
    {   20*_16K,/* 20 consecutive memories of 16K */
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_MEM_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_MEM_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPV4_VRF_E*/
    {   _4K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_IPV4_VRF_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_IPV4_VRF_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPV6_VRF_E*/
    {   _4K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_IPV6_VRF_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_IPV6_VRF_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_FCOE_VRF_E*/
    {   _4K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_FCOE_VRF_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_FCOE_VRF_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_ECMP_E*/
    {   12 * _1K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_ECMP_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_LPM_ECMP_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_EFT_FAST_STACK_FAILOVER_SECONDARY_TARGET_PORT_E*/
    {   256,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_EFT_EGRESS_FAST_STACK_FAILOVER_SECONDARY_TARGET_PORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_EFT_EGRESS_FAST_STACK_FAILOVER_SECONDARY_TARGET_PORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_EGF_QAG_EGRESS_EPORT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_EGRESS_EPORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_EGRESS_EPORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_VLAN_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_EGF_QAG_EGRESS_EVLAN_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_EGRESS_VLAN_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_EGRESS_VLAN_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_TARGET_PORT_MAPPER_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_EGF_QAG_TARGET_PORT_MAPPER_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_TARGET_PORT_MAPPER_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_TARGET_PORT_MAPPER_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_CPU_CODE_TO_LOOPBACK_MAPPER_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_EGF_QAG_CPU_CODE_TO_LOOPBACK_MAPPER_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_CPU_CODE_TO_LOOPBACK_MAPPER_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_CPU_CODE_TO_LOOPBACK_MAPPER_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_EGF_QAG_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_QAG_EGRESS_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EGRESS_EPORT_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_EGF_SHT_EGRESS_EPORT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_EGRESS_EPORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_EGRESS_EPORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_DEVICE_MAP_TABLE_E*/
    {   _4K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_DEVICE_MAP_TABLE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_DEVICE_MAP_TABLE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_HA_EGRESS_EPORT_1_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_1_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_1_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_HA_EGRESS_EPORT_2_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_2_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_2_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_HA_EGRESS_PHYSICAL_PORT_1_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_PHYSICAL_PORT_1_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_PHYSICAL_PORT_1_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_HA_EGRESS_PHYSICAL_PORT_2_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_PHYSICAL_PORT_2_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_PHYSICAL_PORT_2_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_HA_QOS_PROFILE_TO_EXP_E*/
    {   _1K,
        BITS_TO_WORDS_MAC(LION3_HA_QOS_PROFILE_TO_EXP_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_QOS_PROFILE_TO_EXP_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_QOS_PROFILE_TO_EXP_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_HA_EPCL_UDB_CONFIG_E*/
    {   16,
        BITS_TO_WORDS_MAC(LION3_HA_EPCL_UDB_CONFIG_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EPCL_UDB_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EPCL_UDB_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_HA_PTP_DOMAIN_E*/
    {   1280,
        BITS_TO_WORDS_MAC(LION3_HA_PTP_DOMAIN_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_PTP_DOMAIN_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_PTP_DOMAIN_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_HA_GENERIC_TS_PROFILE_E*/
    {   8,
        BITS_TO_WORDS_MAC(LION3_HA_GEN_TS_PROFILE_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_GENERIC_TS_PROFILE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_GENERIC_TS_PROFILE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_HA_GLOBAL_MAC_SA_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_HA_GLOBAL_MAC_SA_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_GLOBAL_MAC_SA_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_GLOBAL_MAC_SA_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E*/
    {   _16K,
        BITS_TO_WORDS_MAC(LION3_MLL_ENTRY_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_MLL_IP_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_MLL_IP_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_E*/
    {   _16K,
        BITS_TO_WORDS_MAC(LION3_MLL_ENTRY_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_MLL_L2_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_MLL_L2_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_LTT_E*/
    {   _32K,
        BITS_TO_WORDS_MAC(LION3_L2_MLL_LTT_ENTRY_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_MLL_L2_LTT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_MLL_L2_LTT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_ADJACENCY_E*/
    {   PRV_CPSS_DXCH_MAX_NUM_ADJACENCY_CNS,
        8,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_ADJACENCY_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_ADJACENCY_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_RBID_E*/
    {   _64K,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_RBID_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_RBID_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_E_ATTRIBUTES_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_E_ATTRIBUTES_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_E_ATTRIBUTES_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_REMARKING_E*/
    {   _1K,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_REMARKING_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_REMARKING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_REMARKING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_HIERARCHICAL_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_HIERARCHICAL_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_HIERARCHICAL_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_HIERARCHICAL_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_METERING_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_METERING_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_METERING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_INGRESS_POLICER_0_METERING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_1_E_ATTRIBUTES_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_E_ATTRIBUTES_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_E_ATTRIBUTES_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_1_REMARKING_E*/
    {   _1K,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_REMARKING_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_REMARKING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_REMARKING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_1_METERING_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_METERING_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_METERING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPLR_1_INGRESS_POLICER_1_METERING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_POLICER_E_ATTRIBUTES_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_E_ATTRIBUTES_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_E_ATTRIBUTES_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_E_ATTRIBUTES_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_POLICER_METERING_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_IPVX_POLICER_METERING_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_METERING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPLR_EGRESS_POLICER_METERING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPVX_INGRESS_EPORT_E*/
    {   512,
        BITS_TO_WORDS_MAC(LION3_IPVX_ROUTER_EPORT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_INGRESS_EPORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_INGRESS_EPORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPVX_EVLAN_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_IPVX_ROUTER_EVLAN_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_EVLAN_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_EVLAN_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E*/
    {   _24K,
        BITS_TO_WORDS_MAC(LION3_IPVX_ROUTER_NEXTHOP_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_NEXTHOP_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_NEXTHOP_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E*/
    {   768,
        BITS_TO_WORDS_MAC(LION3_IPVX_ROUTER_NEXTHOP_AGE_BITS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E*/
    {   192,
        BITS_TO_WORDS_MAC(LION3_IPVX_ROUTER_ACCESS_MATRIX_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_QOS_PROFILE_OFFSETS_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_IPVX_ROUTER_QOS_PROFILE_OFFSETS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_QOS_PROFILE_OFFSETS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IPVX_ROUTER_QOS_PROFILE_OFFSETS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_E*/
    {   _2K,
        3,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        indirectAccessTableInfo + PRV_CPSS_LION3_IOAM_TABLE_INDIRECT_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        indirectAccessTableInfo + PRV_CPSS_LION3_IOAM_TABLE_INDIRECT_E},/*indirect*/

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_AGING_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_KEEPALIVE_AGING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_KEEPALIVE_AGING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_MEG_EXCEPTION_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_MEG_EXCEPTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_MEG_EXCEPTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_INVALID_KEEPALIVE_HASH_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_INVALID_KEEPALIVE_HASH_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_INVALID_KEEPALIVE_HASH_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_EXCESS_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_KEEPALIVE_EXCESS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_KEEPALIVE_EXCESS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_EXCEPTION_SUMMARY_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_EXCEPTION_SUMMARY_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_EXCEPTION_SUMMARY_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_TX_PERIOD_EXCEPTION_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_TX_PERIOD_EXCEPTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_TX_PERIOD_EXCEPTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_OPCODE_PACKET_COMMAND_E*/
    {   256,
        2,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_OPCODE_PACKET_COMMAND_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_IOAM_INGRESS_OAM_OPCODE_PACKET_COMMAND_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_E*/
    {   _2K,
        3,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        indirectAccessTableInfo + PRV_CPSS_LION3_EOAM_TABLE_INDIRECT_E,
        PRV_CPSS_DXCH_INDIRECT_ACCESS_E,
        indirectAccessTableInfo + PRV_CPSS_LION3_EOAM_TABLE_INDIRECT_E},/*indirect*/

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_AGING_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_KEEPALIVE_AGING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_KEEPALIVE_AGING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_MEG_EXCEPTION_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_MEG_EXCEPTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_MEG_EXCEPTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_INVALID_KEEPALIVE_HASH_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_INVALID_KEEPALIVE_HASH_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_INVALID_KEEPALIVE_HASH_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_EXCESS_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_KEEPALIVE_EXCESS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_KEEPALIVE_EXCESS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_EXCEPTION_SUMMARY_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_EXCEPTION_SUMMARY_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_EXCEPTION_SUMMARY_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_TX_PERIOD_EXCEPTION_E*/
    {   64,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_TX_PERIOD_EXCEPTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_TX_PERIOD_EXCEPTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_OPCODE_PACKET_COMMAND_E*/
    {   256,
        2,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_OPCODE_PACKET_COMMAND_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EOAM_EGRESS_OAM_OPCODE_PACKET_COMMAND_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_OAM_LM_OFFSET_E*/
    {   128,
        1,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_OAM_LM_OFFSET_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_OAM_LM_OFFSET_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TTI_QOS_MAP_DSCP_TO_QOS_PROFILE_E*/
    {   12,
        BITS_TO_WORDS_MAC(640),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_DSCP_TO_QOS_PROFILE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_DSCP_TO_QOS_PROFILE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TTI_QOS_MAP_UP_CFI_TO_QOS_PROFILE_E*/
    {   12,
        BITS_TO_WORDS_MAC(160),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_UP_CFI_TO_QOS_PROFILE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_UP_CFI_TO_QOS_PROFILE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TTI_QOS_MAP_EXP_TO_QOS_PROFILE_E*/
    {   12,
        BITS_TO_WORDS_MAC(80),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_EXP_TO_QOS_PROFILE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_EXP_TO_QOS_PROFILE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TTI_QOS_MAP_DSCP_TO_DSCP_E*/
    {   12,
        BITS_TO_WORDS_MAC(384),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_DSCP_TO_DSCP_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_QOS_MAP_DSCP_TO_DSCP_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TTI_UDB_CONFIG_E*/
    {   20,
        BITS_TO_WORDS_MAC(330),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_UDB_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_UDB_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TTI_PTP_COMMAND_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_TTI_PTP_COMMAND_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_PTP_COMMAND_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TTI_PTP_COMMAND_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPCL0_UDB_SELECT_E*/
    {   96,
        BITS_TO_WORDS_MAC(LION3_IPCL_UDB_SELECT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL0_UDB_SELECT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL0_UDB_SELECT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPCL1_UDB_SELECT_E*/
    {   96,
        BITS_TO_WORDS_MAC(LION3_IPCL_UDB_SELECT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL1_UDB_SELECT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL1_UDB_SELECT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_IPCL2_UDB_SELECT_E*/
    {   96,
        BITS_TO_WORDS_MAC(LION3_IPCL_UDB_SELECT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL2_UDB_SELECT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PCL_IPCL2_UDB_SELECT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E*/
    {   49152,
        BITS_TO_WORDS_MAC(LION3_TCAM_PCL_TTI_ACTION_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TCAM_PCL_TTI_ACTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TCAM_PCL_TTI_ACTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_ERMRK_TIMESTAMP_CFG_E*/
    {   128,
        BITS_TO_WORDS_MAC(LION3_ERMRK_TIMESTAMP_CFG_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_TIMESTAMP_CFG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_TIMESTAMP_CFG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_LOCAL_ACTION_E*/
    {   32768,
        BITS_TO_WORDS_MAC(LION3_ERMRK_PTP_LOCAL_ACTION_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_LOCAL_ACTION_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_LOCAL_ACTION_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_ERMRK_PTP_TARGET_PORT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_TARGET_PORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_TARGET_PORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_ERMRK_PTP_SOURCE_PORT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_SOURCE_PORT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_PTP_SOURCE_PORT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_ERMRK_QOS_DSCP_MAP_E*/
    {   192,
        BITS_TO_WORDS_MAC(LION3_ERMRK_QOS_DSCP_MAP_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_QOS_DSCP_MAP_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_QOS_DSCP_MAP_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_ERMRK_QOS_TC_DP_MAP_E*/
    {   192,
        BITS_TO_WORDS_MAC(LION3_ERMRK_QOS_TC_DP_MAP_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_QOS_TC_DP_MAP_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_QOS_TC_DP_MAP_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TCAM_E*/
    {   98304,
        BITS_TO_WORDS_MAC(LION3_TCAM_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TCAM_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TCAM_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_L2I_INGRESS_BRIDGE_PORT_MEMBERS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_INGRESS_BRIDGE_PORT_MEMBERS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_INGRESS_BRIDGE_PORT_MEMBERS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_INGRESS_SPAN_STATE_GROUP_INDEX_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_L2I_INGRESS_SPAN_STATE_GROUP_INDEX_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_INGRESS_SPAN_STATE_GROUP_INDEX_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_L2I_INGRESS_SPAN_STATE_GROUP_INDEX_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_EGF_SHT_EGRESS_VLAN_ATTRIBUTES_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_ATTRIBUTES_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_ATTRIBUTES_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_SPANNING_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_EGF_SHT_EGRESS_VLAN_SPANNING_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_SPANNING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_SPANNING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_MAPPER_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_EGF_SHT_EGRESS_VLAN_MAPPER_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_MAPPER_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_VLAN_MAPPER_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_NON_TRUNK_MEMBERS2_E*/
    {   _4K,
        BITS_TO_WORDS_MAC(BMP_256_PORTS_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_NON_TRUNK_MEMBERS2_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_NON_TRUNK_MEMBERS2_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EPORT_FILTER_E*/
    {   _4K,
        BITS_TO_WORDS_MAC(BMP_256_PORTS_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_EPORT_FILTER_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EGF_SHT_EGRESS_EPORT_FILTER_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_EPCL_UDB_SELECT_E*/
    {   96,
        BITS_TO_WORDS_MAC(LION3_EPCL_UDB_SELECT_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPCL_UDB_SELECT_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_EPCL_UDB_SELECT_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_BMA_PORT_MAPPING_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_BMA_PORT_MAPPING_BIT_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_BMA_PORT_MAPPING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_BMA_PORT_MAPPING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_BMA_MULTICAST_COUNTERS_E*/
    {   _8K,
        BITS_TO_WORDS_MAC(LION3_BMA_MULTICAST_COUNTERS_BIT_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_BMA_MULTICAST_COUNTERS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_BMA_MULTICAST_COUNTERS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_MAX_QUEUE_LIMITS_E*/
    {   128,
        BITS_TO_WORDS_MAC(LION3_TAIL_DROP_MAX_QUEUE_LIMITS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_MAX_QUEUE_LIMITS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_MAX_QUEUE_LIMITS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_EQ_QUEUE_LIMITS_DP0_E*/
    {   128,
        BITS_TO_WORDS_MAC(LION3_TAIL_DROP_EQ_QUEUE_LIMITS_DP0_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP0_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP0_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_DQ_QUEUE_BUF_LIMITS_E*/
    {   128,
        BITS_TO_WORDS_MAC(LION3_TAIL_DROP_DQ_QUEUE_BUF_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_DQ_QUEUE_BUF_LIMITS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_DQ_QUEUE_BUF_LIMITS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_E*/
    {   128,
        BITS_TO_WORDS_MAC(LION3_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_EQ_QUEUE_LIMITS_DP1_E*/
    {   128,
        BITS_TO_WORDS_MAC(LION3_TAIL_DROP_EQ_QUEUE_LIMITS_DP1_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP1_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP1_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_EQ_QUEUE_LIMITS_DP2_E*/
    {   128,
        BITS_TO_WORDS_MAC(LION3_TAIL_DROP_EQ_QUEUE_LIMITS_DP2_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP2_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_EQ_QUEUE_LIMITS_DP2_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_COUNTERS_Q_MAIN_BUFF_E*/
    {   576,
        BITS_TO_WORDS_MAC(LION3_TAIL_DROP_COUNTERS_MAIN_BUFF_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_COUNTERS_Q_MAIN_BUFF_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_COUNTERS_Q_MAIN_BUFF_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_COUNTERS_Q_MAIN_MC_BUFF_E*/
    {   576,
        BITS_TO_WORDS_MAC(LION3_TAIL_DROP_COUNTERS_MAIN_MC_BUFF_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_COUNTERS_Q_MAIN_MC_BUFF_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_COUNTERS_Q_MAIN_MC_BUFF_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TXQ_LINK_LIST_COUNTERS_Q_MAIN_DESC_E*/
    {   576,
        BITS_TO_WORDS_MAC(LION3_TXQ_LINK_LIST_COUNTERS_MAIN_DESC_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_LL_TXQ_LINK_LIST_COUNTERS_Q_MAIN_DESC_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_LL_TXQ_LINK_LIST_COUNTERS_Q_MAIN_DESC_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_E*/
    {   64,
        BITS_TO_WORDS_MAC(LION3_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_MODE_PROFILE_TC_XOFF_THRESHOLDS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_E*/
    {   64,
        BITS_TO_WORDS_MAC(LION3_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_MODE_PROFILE_TC_XON_THRESHOLDS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_PFC_LLFC_COUNTERS_E*/
    {   1024,
        BITS_TO_WORDS_MAC(LION3_PFC_LLFC_COUNTERS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_LLFC_COUNTERS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_PFC_LLFC_COUNTERS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_DROP_DROP_MASKING_E*/
    {   128,
        BITS_TO_WORDS_MAC(LION3_TM_DROP_DROP_MASKING_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_DROP_DROP_MASKING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_DROP_DROP_MASKING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_DROP_QUEUE_PROFILE_ID_E*/
    {   1024,
        BITS_TO_WORDS_MAC(LION3_TM_DROP_QUEUE_PROFILE_ID_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_DROP_QUEUE_PROFILE_ID_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_DROP_QUEUE_PROFILE_ID_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_CPU_CODE_TO_TC_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_TM_QMAP_CPU_CODE_TO_TC_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_CPU_CODE_TO_TC_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_CPU_CODE_TO_TC_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_TARGET_DEV_TO_INDEX_E*/
    {   1024,
        BITS_TO_WORDS_MAC(LION3_TM_QMAP_TARGET_DEV_TO_INDEX_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_TARGET_DEV_TO_INDEX_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_TARGET_DEV_TO_INDEX_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_POLICING_ENABLE_E*/
    {   1024,
        BITS_TO_WORDS_MAC(LION3_TM_QMAP_TM_QMAP_POLICING_ENABLE_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_POLICING_ENABLE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_POLICING_ENABLE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_QMAP_QUEUE_ID_SELECTOR_E*/
    {   256,
        BITS_TO_WORDS_MAC(LION3_TM_QMAP_QUEUE_ID_SELECTOR_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_QUEUE_ID_SELECTOR_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_QMAP_QUEUE_ID_SELECTOR_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_INGR_GLUE_L1_PKT_LEN_OFFSET_E*/
    {   64,
        BITS_TO_WORDS_MAC(LION3_TM_INGR_GLUE_L1_PKT_LEN_OFFSET_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_INGRESS_GLUE_L1_PKT_LEN_OFFSET_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_INGRESS_GLUE_L1_PKT_LEN_OFFSET_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_EGR_GLUE_AGING_QUEUE_PROFILE_E*/
    {   1024,
        BITS_TO_WORDS_MAC(LION3_TM_EGR_GLUE_AGING_QUEUE_PROFILE_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_AGING_QUEUE_PROFILE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_AGING_QUEUE_PROFILE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_EGR_GLUE_AGING_PROFILE_THESHOLDS_E*/
    {   16,
        BITS_TO_WORDS_MAC(LION3_TM_EGR_GLUE_AGING_PROFILE_THESHOLDS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_AGING_PROFILE_THESHOLDS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_AGING_PROFILE_THESHOLDS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_EGR_GLUE_TARGET_INTERFACE_E*/
    {   64,
        BITS_TO_WORDS_MAC(LION3_TM_EGR_GLUE_TARGET_INTERFACE_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_TARGET_INTERFACE_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_EGRESS_GLUE_TARGET_INTERFACE_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_E*/
    {   32,
        BITS_TO_WORDS_MAC(LION3_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ETH_DMA_TO_TM_PORT_MAPPING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E*/
    {   32,
        BITS_TO_WORDS_MAC(LION3_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_ILK_DMA_TO_TM_PORT_MAPPING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_E*/
    {   64,
        BITS_TO_WORDS_MAC(LION3_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_TC_PORT_TO_CNODE_PORT_MAPPING_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E*/
    {   64,
        BITS_TO_WORDS_MAC(LION3_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_PORT_INGRESS_TIMERS_CONFIG_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_INGRESS_TIMERS_E*/
    {   64,
        BITS_TO_WORDS_MAC(LION3_TM_FCU_INGRESS_TIMERS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_INGRESS_TIMERS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TM_FCU_INGRESS_TIMERS_E].directAccessInfo},

    /*PRV_CPSS_DXCH_LION3_TABLE_TM_FCU_PORT_TO_PHYSICAL_PORT_MAPPING_E*/
    {   64,
        BITS_TO_WORDS_MAC(LION3_TM_FCU_PORT_TO_PHYSICAL_PORT_MAPPING_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PORT_TO_PHYSICAL_PORT_MAPPING_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_PORT_TO_PHYSICAL_PORT_MAPPING_E].directAccessInfo},


    /*PRV_CPSS_DXCH_LION3_TABLE_CN_SAMPLE_INTERVALS_E*/
    {   576,
        BITS_TO_WORDS_MAC(LION3_CN_SAMPLE_INTERVALS_BITS_SIZE_CNS),
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_CN_SAMPLE_INTERVALS_E].directAccessInfo,
        PRV_CPSS_DXCH_DIRECT_ACCESS_E,
        &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_CN_SAMPLE_INTERVALS_E].directAccessInfo},
};

/* bobcat 2 */
static const GT_U32 bobcat2TableInfoSize =
    (NUM_ELEMENTS_IN_ARR_MAC(bobcat2TablesInfo));


/* Bobcat2 B0 table information , that need to override A0 info from bobcat2TablesInfo */
/* NOTE : the entries in this array need to be matched with entries in
        bobcat2TablesInfo , according to field */
static PRV_CPSS_DXCH_TABLES_INFO_EXT_STC bobcat2B0TablesInfo_overrideA0[] =
{
    /* HA */
    {PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
        /*PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E*/
        {   _8K,
            BITS_TO_WORDS_MAC(BOBCAT_B0_HA_EGRESS_EPORT_1_BITS_SIZE_CNS),
            PRV_CPSS_DXCH_DIRECT_ACCESS_E,
            &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_1_E].directAccessInfo,
            PRV_CPSS_DXCH_DIRECT_ACCESS_E,
            &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_1_E].directAccessInfo}
    },

    {PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
        /*PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E*/
        {   _8K,
            BITS_TO_WORDS_MAC(BOBCAT_B0_HA_EGRESS_EPORT_2_BITS_SIZE_CNS),
            PRV_CPSS_DXCH_DIRECT_ACCESS_E,
            &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_2_E].directAccessInfo,
            PRV_CPSS_DXCH_DIRECT_ACCESS_E,
            &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_HA_EGRESS_EPORT_2_E].directAccessInfo}
    },

    /* ERMRK */
    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_TIMESTAMP_CFG_E,
        /*PRV_CPSS_DXCH_LION3_TABLE_ERMRK_TIMESTAMP_CFG_E*/
        {   128,
            BITS_TO_WORDS_MAC(BOBCAT2_B0_ERMRK_TIMESTAMP_CFG_BITS_SIZE_CNS),
            PRV_CPSS_DXCH_DIRECT_ACCESS_E,
            &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_TIMESTAMP_CFG_E].directAccessInfo,
            PRV_CPSS_DXCH_DIRECT_ACCESS_E,
            &directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_ERMRK_TIMESTAMP_CFG_E].directAccessInfo},
    }

};
/* indication that directAccessBobkTableInfo was initialized */
static GT_BOOL directAccessBobkTableInfo_initDone = GT_FALSE;
/* copied in runtime from directAccessBobcat2TableInfo[] */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC directAccessBobkTableInfo[PRV_DXCH_BOBK_TABLE___LAST_E] =
{
    {0}
    /* in runtime the tables of the bobcat2_b0 will be copied here ,
       and will be updated by the relevant bobk code -->
       see bobkInitTable() */

    /* in runtime the 'Extra tables' of the bobk will be copied here from :
        directAccessBobkTableInfo_new[] */
};

typedef struct{
    PRV_DXCH_BOBCAT2_TABLES_ENT             newIndex; /* index in directAccessBobkTableInfo */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC    directAccessInfo;
}BOBK_TABLES_INFO_DIRECT_NEW_INDEX_STC;

/* Bobk direct table information , that are new (not exists in bobcat2 A0/B0) */
/* register new tables that not exists in bobcat2 !!!! */
static BOBK_TABLES_INFO_DIRECT_NEW_INDEX_STC directAccessBobkTableInfo_new[] =
{
    {PRV_DXCH_BOBK_TABLE_PLR_INGRESS_POLICER_0_METERING_CONFIG_E,/* index in directAccessBobkTableInfo */
        {PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_0_METERING_CONFIG_E, PRV_CPSS_DXCH_UNIT_IPLR_E,
         {0x0B000000 + 0x00300000, BITS_TO_BYTES_ALIGNMENT_MAC(BOBK_POLICER_METERING_CONFIG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}}

    ,{PRV_DXCH_BOBK_TABLE_PLR_INGRESS_POLICER_1_METERING_CONFIG_E,/* index in directAccessBobkTableInfo */
         {PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_1_METERING_CONFIG_E, PRV_CPSS_DXCH_UNIT_IPLR_1_E,
         {0x20000000 + 0x00300000, BITS_TO_BYTES_ALIGNMENT_MAC(BOBK_POLICER_METERING_CONFIG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}}

    ,{PRV_DXCH_BOBK_TABLE_PLR_EGRESS_POLICER_METERING_CONFIG_E,/* index in directAccessBobkTableInfo */
         {PRV_CPSS_DXCH_BOBK_TABLE_EGRESS_POLICER_METERING_CONFIG_E, PRV_CPSS_DXCH_UNIT_EPLR_E,
         {0x0E000000 + 0x00300000, BITS_TO_BYTES_ALIGNMENT_MAC(BOBK_POLICER_METERING_CONFIG_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}}

    ,{PRV_DXCH_BOBK_TABLE_PLR_INGRESS_POLICER_0_METERING_CONFORM_SIGN_E,/* index in directAccessBobkTableInfo */
        {PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_0_METERING_CONFORM_SIGN_E, PRV_CPSS_DXCH_UNIT_IPLR_E,
         {0x0B000000 + 0x00400000, BITS_TO_BYTES_ALIGNMENT_MAC(BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}}

    ,{PRV_DXCH_BOBK_TABLE_PLR_INGRESS_POLICER_1_METERING_CONFORM_SIGN_E,/* index in directAccessBobkTableInfo */
         {PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_1_METERING_CONFORM_SIGN_E, PRV_CPSS_DXCH_UNIT_IPLR_1_E,
         {0x20000000 + 0x00400000, BITS_TO_BYTES_ALIGNMENT_MAC(BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}}

    ,{PRV_DXCH_BOBK_TABLE_PLR_EGRESS_POLICER_METERING_CONFORM_SIGN_E,/* index in directAccessBobkTableInfo */
         {PRV_CPSS_DXCH_BOBK_TABLE_EGRESS_POLICER_METERING_CONFORM_SIGN_E, PRV_CPSS_DXCH_UNIT_EPLR_E,
         {0x0E000000 + 0x00400000, BITS_TO_BYTES_ALIGNMENT_MAC(BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}}
};

#define BOBK_NUM_EPORTS_MAC     _6K /*8k in bc2 */
#define BOBK_NUM_PHY_PORTS_MAC  128 /*256 in bc2 */
#define BOBK_NUM_EVLANS_MAC     (_4K + 512) /*4.5k*/ /*8k in bc2 */
#define BOBK_NUM_STG_MAC        _1K /*4K in bc2 */
#define BOBK_NUM_IPVX_NH_MAC    _8K /* 24K in bc2 */
#define BOBK_NUM_MLL_MAC        _4K /*8K in bc2 */
#define BOBK_NUM_EVIDX_MAC       _8K /*16K in bc2 */
#define BOBK_NUM_L3_ECMP_MAC    _4K /*12K in bc2 */
#define BOBK_NUM_IPLR_METERS_MAC    _6K
#define BOBK_NUM_EPLR_METERS_MAC    _4K
#define BOBK_NUM_BMA_PORT_MAP_ENTRIES_MAC    (4*256)

#define BOBK_L2I_INGRESS_STP_BITS_SIZE_CNS                  (2*BOBK_NUM_PHY_PORTS_MAC)/* 2 bits per port */
#define BOBK_L2I_INGRESS_BRIDGE_PORT_MEMBERS_BITS_SIZE_CNS  BOBK_NUM_PHY_PORTS_MAC

/* Bobk direct table information , that need to override bobcat2 info from directAccessBobkTableInfo */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_EXT_STC directAccessBobkTableInfo_overrideBobcat2[] =
{
    /*L2i tables */
    {PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
    {NOT_MODIFIED_VALUE_CNS, BITS_TO_BYTES_ALIGNMENT_MAC(BOBK_L2I_INGRESS_STP_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E,
    {NOT_MODIFIED_VALUE_CNS, BITS_TO_BYTES_ALIGNMENT_MAC(BOBK_L2I_INGRESS_BRIDGE_PORT_MEMBERS_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},


    /* MLL Tables */
    {PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E,
    {0x1D000000 + 0x00100000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_MLL_ENTRY_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_E,/* must be the same as 'IP_MLL' */
    {0x1D000000 + 0x00100000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_MLL_ENTRY_BITS_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*IOAM*/
    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_E,
    {0x1C000000 + 0x00080000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_AGING_E,
    {0x1C000000 + 0x00007000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    /*EOAM*/
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_E,
    {0x1E000000 + 0x00080000, 0x10, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_AGING_E,
    {0x1E000000 + 0x00007000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}},

};

/* bobk table - in runtime copy table from this of bobcat2_b0 ... and continue from there ... */
static PRV_CPSS_DXCH_TABLES_INFO_STC bobkTablesInfo[PRV_CPSS_DXCH_BOBK_TABLE_LAST_E] =
{
    {0}
    /* in runtime the tables of the bobcat2_b0 will be copied here ,
       and will be updated by the relevant bobk code -->
       see bobkInitTable() */

    /* in runtime the 'Extra tables' of the bobk will be copied here*/
};

/* bobk info */
static const GT_U32 bobkTableInfoSize =
    (NUM_ELEMENTS_IN_ARR_MAC(bobkTablesInfo));

/* "not modify" value for pointers:
   0xFFFFFFFF for 32 bits CPUs, 0xFFFFFFFF FFFFFFFF for 64 bits CPUs*/
#define NOT_MODIFIED_POINTER_CNS  ((void *)(~(GT_UINTPTR)0))

/* override value if new value is 'modified' */
#define OVERRIDE_DIRECT_INFO_IF_MODIFIED_MAC(_overrideInfoPtr,_infoPtr,_field) \
    if(_overrideInfoPtr->_field != NOT_MODIFIED_VALUE_CNS)                     \
        _infoPtr->_field = _overrideInfoPtr->_field

/* override value of pointer if new value is 'modified' */
#define OVERRIDE_DIRECT_INFO_IF_MODIFIED_POINTER_MAC(_overrideInfoPtr,_infoPtr,_field) \
    if(_overrideInfoPtr->_field != NOT_MODIFIED_POINTER_CNS)                           \
        _infoPtr->_field = _overrideInfoPtr->_field

/* set valid field only : maxNumOfEntries */
#define OVERRIDE_INFO_NUM_ENTRIES_MAC(maxNumOfEntries)                  \
        {   maxNumOfEntries,                   /*maxNumOfEntries*/      \
            NOT_MODIFIED_VALUE_CNS,            /*entrySize      */      \
            NOT_MODIFIED_VALUE_CNS,            /*readAccessType */      \
            NOT_MODIFIED_POINTER_CNS,          /*readTablePtr   */      \
            NOT_MODIFIED_VALUE_CNS,            /*writeAccessType*/      \
            NOT_MODIFIED_POINTER_CNS}          /*writeTablePtr  */

/* set valid field only : entrySize */
#define OVERRIDE_INFO_WIDTH_MAC(entrySize_inBits)                       \
        {   NOT_MODIFIED_VALUE_CNS,            /*maxNumOfEntries*/      \
            BITS_TO_WORDS_MAC(entrySize_inBits),/*entrySize      */     \
            NOT_MODIFIED_VALUE_CNS,            /*readAccessType */      \
            NOT_MODIFIED_POINTER_CNS,          /*readTablePtr   */      \
            NOT_MODIFIED_VALUE_CNS,            /*writeAccessType*/      \
            NOT_MODIFIED_POINTER_CNS}          /*writeTablePtr  */

/* set valid field only : maxNumOfEntries, entrySize
   NOTE: entrySize is in 'bits' !!!
*/
#define OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(maxNumOfEntries,entrySize_inBits) \
        {   maxNumOfEntries,                   /*maxNumOfEntries*/      \
            BITS_TO_WORDS_MAC(entrySize_inBits),/*entrySize      */     \
            NOT_MODIFIED_VALUE_CNS,            /*readAccessType */      \
            NOT_MODIFIED_POINTER_CNS,          /*readTablePtr   */      \
            NOT_MODIFIED_VALUE_CNS,            /*writeAccessType*/      \
            NOT_MODIFIED_POINTER_CNS}          /*writeTablePtr  */

/*  table information , that need to override Bobcat2 B0 info from bobkTablesInfo */
/* NOTE : the entries in this array need to be matched with entries in
        bobcat2TablesInfo , according to field */
static PRV_CPSS_DXCH_TABLES_INFO_EXT_STC bobkTablesInfo_overrideBobcat2[] =
{
    /* L2i */
    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC/8)/* 8 eports per entry */
    },
    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },
    {PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(BOBK_NUM_STG_MAC , BOBK_L2I_INGRESS_STP_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_SPAN_STATE_GROUP_INDEX_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVLANS_MAC)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(BOBK_NUM_EVLANS_MAC , BOBK_L2I_INGRESS_BRIDGE_PORT_MEMBERS_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVLANS_MAC)
    },

    /* tti */
    {PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },

    /* epcl */
    {PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(_4K + BOBK_NUM_PHY_PORTS_MAC)
    },

    /* eq */
    {PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_EPORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC / 4) /* 4 eports per entry */
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_L2_ECMP_LTT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_L2_ECMP_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(_4K)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_TX_PROTECTION_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC / 32) /* 32 eports per entry */
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_LOC_MAPPING_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC / 2) /* 2 eports per entry */
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC)
    },

    /*ERMRK*/
    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },

    /*ha*/
    {PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },

    {PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(_8K)
    },

    {PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVLANS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_HA_PTP_DOMAIN_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC * 5) /* each port with 5 domains */
    },

    /* ipcl */
    {PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(_4K + BOBK_NUM_PHY_PORTS_MAC)
    },
    {PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(_4K + BOBK_NUM_PHY_PORTS_MAC)
    },
    {PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(_4K + BOBK_NUM_PHY_PORTS_MAC)
    },

    /* ipvx*/
    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_INGRESS_EPORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC((BOBK_NUM_EPORTS_MAC / 16) /* 16 eports per entry */, BOBK_IPVX_ROUTER_EPORT_BITS_SIZE_CNS)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_IPVX_NH_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_EVLAN_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVLANS_MAC)
    },


    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_IPVX_NH_MAC / 32) /* 32 NH per entry */
    },

    /* mll */
    {PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_MLL_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_MLL_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_LTT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVIDX_MAC)
    },

    /* egf */
        /*qag*/
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_VLAN_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVLANS_MAC)/* NOTE: keep width as if 256 ports exists !!! */
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_TARGET_PORT_MAPPER_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_CPU_CODE_TO_LOOPBACK_MAPPER_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },

        /*eft*/
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_EFT_FAST_STACK_FAILOVER_SECONDARY_TARGET_PORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_PHY_PORTS_MAC)
    },

        /*sht*/
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_SPANNING_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVLANS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EGRESS_EPORT_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EPORTS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVLANS_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_MAPPER_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVLANS_MAC)
    },

    {PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_STG_MAC) /* NOTE: keep width as if 256 ports exists !!! */
    },

    /*PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E, --> NOTE: keep width as if 256 ports exists !!! */
    /*PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E,  --> NOTE: keep width as if 256 ports exists !!! */
    /*PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E,  --> NOTE: keep width as if 256 ports exists !!! */
    /*PRV_CPSS_DXCH_TABLE_MULTICAST_E,  --> NOTE: keep width as if 256 ports exists !!! */
    /*PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,  --> NOTE: keep width as if 256 ports exists !!! */
    /*PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E,  --> NOTE: keep width as if 256 ports exists !!! */
    {PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_EVLANS_MAC )/* NOTE: keep width as if 256 ports exists !!! */
    },

    /*lpm*/
    CPSS_TBD_BOOKMARK_BOBCAT2_BOBK
    /* need to support (6K * 20) sub memories of 16K range for each of the 20 */
    {PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E,
        {   NOT_MODIFIED_VALUE_CNS,            /*maxNumOfEntries*/
            NOT_MODIFIED_VALUE_CNS,            /*entrySize      */
            NOT_MODIFIED_VALUE_CNS,            /*readAccessType */
            NOT_MODIFIED_POINTER_CNS,          /*readTablePtr   */
            NOT_MODIFIED_VALUE_CNS,            /*writeAccessType*/
            NOT_MODIFIED_POINTER_CNS}          /*writeTablePtr  */
    },

    {PRV_CPSS_DXCH_LION3_TABLE_ECMP_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_L3_ECMP_MAC)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_E,
        OVERRIDE_INFO_WIDTH_MAC(104)
    },

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_E,
        OVERRIDE_INFO_WIDTH_MAC(104)
    },

    /* PLR */
    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_METERING_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(BOBK_NUM_IPLR_METERS_MAC,
            BOBK_POLICER_METERING_TOKEN_BUCKET_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_1_METERING_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(BOBK_NUM_IPLR_METERS_MAC,
            BOBK_POLICER_METERING_TOKEN_BUCKET_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_POLICER_METERING_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(BOBK_NUM_EPLR_METERS_MAC,
            BOBK_POLICER_METERING_TOKEN_BUCKET_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_0_METERING_CONFORM_SIGN_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(BOBK_NUM_IPLR_METERS_MAC,
            BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_1_METERING_CONFORM_SIGN_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(BOBK_NUM_IPLR_METERS_MAC,
            BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_BOBK_TABLE_EGRESS_POLICER_METERING_CONFORM_SIGN_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(BOBK_NUM_EPLR_METERS_MAC,
            BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS)
    },

    /* BMA */
    {PRV_CPSS_DXCH_LION3_TABLE_BMA_PORT_MAPPING_E,
        OVERRIDE_INFO_NUM_ENTRIES_MAC(BOBK_NUM_BMA_PORT_MAP_ENTRIES_MAC)
    }
};

typedef enum{
    ENTRY_TYPE_NOT_VALID_E/* indication that table is not valid (0 entries)*/

    ,ENTRY_TYPE_EPORT_E
    ,ENTRY_TYPE_PHY_PORT_E
    ,ENTRY_TYPE_EVLAN_E
    ,ENTRY_TYPE_EVIDX_E
    ,ENTRY_TYPE_STG_E
    ,ENTRY_TYPE_L2_ECMP_E
    ,ENTRY_TYPE_L3_ECMP_E
    ,ENTRY_TYPE_TUNNEL_START_E
    ,ENTRY_TYPE_IPVX_NH_E
    ,ENTRY_TYPE_MLL_LTT_E
    ,ENTRY_TYPE_MLL_E
    ,ENTRY_TYPE_IPLR_METERING_E
    ,ENTRY_TYPE_EPLR_METERING_E
    ,ENTRY_TYPE_OAM_E
    ,ENTRY_TYPE_TXQ_QUEUES_E
    ,ENTRY_TYPE_LOC_E
    ,ENTRY_TYPE_EXPLICIT_NUM_E
    ,ENTRY_TYPE_SOURCE_ID_E
    ,ENTRY_TYPE_TRUNK_E
    ,ENTRY_TYPE_TXQ_DQ_PORTS_E

    /* must be last */
    ,ENTRY_TYPE___LAST__E

}ENTRY_TYPE_ENT;

typedef struct{
    PRV_CPSS_DXCH_TABLE_ENT tableType;
    ENTRY_TYPE_ENT  entryType;
    GT_U32   divider;/* number of 'info' per entry.
                        for example: entry with 8 eports per entry , divide = 8
                       NOTE: value 0 or 1 are ignored.
                       NOTE: if value hold FRACTION_INDICATION_CNS then logic
                            is 'multiplier' (instead of divider)

                       NOTE: if value hold VERTICAL_INDICATION_CNS then logic
                        the x entries in the first 'line' are not : 0,1
                        the x entries in the first 'line' are     : 0,depth

                        meaning that line y start with 'entry' y

                        NOTE: if value FRACTION_HALF_TABLE_INDICATION_CNS
                        meaning that single logical entry is spread on 2 half of the table.
                        in indexes : x,(x+'depth')

                        */
    GT_U32          additionalFixedNumberOfEntries;/* additional number of entries.
                            like PCL configuration table that hold 4K + 'phy ports' entries */
}TABLE_NUM_ENTRIES_INFO_STC;

/*xxx*/
/* SIP5_20 : info about the number of entries in the tables */
static TABLE_NUM_ENTRIES_INFO_STC   sip5_20tableNumEntriesInfoArr[] =
{
    /* L2i */
    {PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E,
        ENTRY_TYPE_EPORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E,
        ENTRY_TYPE_EPORT_E,8,0}/* 8 eports per entry */
    ,{PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_E,
        ENTRY_TYPE_EXPLICIT_NUM_E,0,256}/* supports only 256 ports although the device may work in 512 ports mode */
    ,{PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_E,
        ENTRY_TYPE_EXPLICIT_NUM_E,0,256}/* supports only 256 ports although the device may work in 512 ports mode */
    ,{PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
        ENTRY_TYPE_STG_E,VERTICAL_INDICATION_CNS | 2,0}/*2 STGs per entry (each support 256 ports * 2 bits)*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_SPAN_STATE_GROUP_INDEX_E,
        ENTRY_TYPE_EVLAN_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E,
        ENTRY_TYPE_EVLAN_E,VERTICAL_INDICATION_CNS | 2,0}/*2 vlans per entry*/
    ,{PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E,
        ENTRY_TYPE_EVLAN_E,0,0}

    /* tti */
    ,{PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E,
        ENTRY_TYPE_EPORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}

    /* epcl */
    ,{PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E,
        ENTRY_TYPE_PHY_PORT_E,0,_4K}

    /* eq */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_EPORT_E,
        ENTRY_TYPE_EPORT_E,4,0}/* 4 eports per entry */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EQ_L2_ECMP_LTT_E,
        ENTRY_TYPE_EPORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EQ_L2_ECMP_E,
        ENTRY_TYPE_L2_ECMP_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EQ_TX_PROTECTION_E,
        ENTRY_TYPE_EPORT_E,32,0}/* 32 eports per entry */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_LOC_MAPPING_E,
        ENTRY_TYPE_EPORT_E,2,0}/* 2 eports per entry */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_PHYSICAL_PORT_TARGET_MAPPING_E,
        ENTRY_TYPE_EPORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E,
        ENTRY_TYPE_PHY_PORT_E,8,0}/* 8 ports in entry */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}

    /*ERMRK*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_LOCAL_ACTION_E,
        ENTRY_TYPE_PHY_PORT_E,(FRACTION_INDICATION_CNS | 16),0}/* 16 PtpMessageType */
    ,{PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_TARGET_PORT_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_SOURCE_PORT_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}

    /*ha*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
        ENTRY_TYPE_EPORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E,
        ENTRY_TYPE_EPORT_E,0,0}

    ,{PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}

    ,{PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E,
        ENTRY_TYPE_TUNNEL_START_E,0,0}

    ,{PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E,
        ENTRY_TYPE_EPORT_E,0,0}

    ,{PRV_CPSS_DXCH_LION3_TABLE_HA_PTP_DOMAIN_E,
        ENTRY_TYPE_PHY_PORT_E , (FRACTION_INDICATION_CNS | 5),0} /* each port with 5 domains */

    /* ipcl */
    ,{PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E,
        ENTRY_TYPE_PHY_PORT_E,0,_4K}
    ,{PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E,
        ENTRY_TYPE_PHY_PORT_E,0,_4K}
    ,{PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E,
        ENTRY_TYPE_PHY_PORT_E,0,_4K}

    /* ipvx*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_IPVX_INGRESS_EPORT_E,
        ENTRY_TYPE_EPORT_E,16,0}/* 16 eports per entry */
    ,{PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E,
        ENTRY_TYPE_IPVX_NH_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_IPVX_EVLAN_E,
        ENTRY_TYPE_EVLAN_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E,
        ENTRY_TYPE_IPVX_NH_E , 32,0}/* 32 NH per entry */

    /* mll */
    ,{PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E,
        ENTRY_TYPE_MLL_E,0,0}

    ,{PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_E,
        ENTRY_TYPE_MLL_E,0,0}

    ,{PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_LTT_E,
        ENTRY_TYPE_MLL_LTT_E,0,0}

    /* egf */
        /*qag*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E,
        ENTRY_TYPE_EPORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_VLAN_E,
        ENTRY_TYPE_EVLAN_E, (FRACTION_INDICATION_CNS | 16),0}/*each evlan with 16 lines*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_TARGET_PORT_MAPPER_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_FWD_FROM_CPU_TO_LOOPBACK_MAPPER_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_CPU_CODE_TO_LOOPBACK_MAPPER_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}

        /*eft*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_EFT_FAST_STACK_FAILOVER_SECONDARY_TARGET_PORT_E,
        ENTRY_TYPE_PHY_PORT_E,0,0}
        /*sht*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_SPANNING_E,
        ENTRY_TYPE_EVLAN_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EGRESS_EPORT_E,
        ENTRY_TYPE_EPORT_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E,
        ENTRY_TYPE_EVLAN_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_MAPPER_E,
        ENTRY_TYPE_EVLAN_E,0,0}

    ,{PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
        ENTRY_TYPE_STG_E,FRACTION_HALF_TABLE_INDICATION_CNS | 1/*place holder*/,0}

    ,{PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
        ENTRY_TYPE_EVLAN_E,FRACTION_HALF_TABLE_INDICATION_CNS | 1/*place holder*/,0}

    ,{PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E,
        ENTRY_TYPE_SOURCE_ID_E,FRACTION_HALF_TABLE_INDICATION_CNS | 1/*place holder*/,0}

    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_NON_TRUNK_MEMBERS2_E,
        ENTRY_TYPE_TRUNK_E,FRACTION_HALF_TABLE_INDICATION_CNS | 1/*place holder*/,0}


    ,{PRV_CPSS_DXCH_LION3_TABLE_ECMP_E,
        ENTRY_TYPE_L3_ECMP_E,0,0}

    /* PLR */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_METERING_E,
        ENTRY_TYPE_IPLR_METERING_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_1_METERING_E,
        ENTRY_TYPE_IPLR_METERING_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_POLICER_METERING_E,
        ENTRY_TYPE_EPLR_METERING_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_POLICER_0_HIERARCHICAL_E,
        ENTRY_TYPE_IPLR_METERING_E,0,0}
    ,{PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_0_METERING_CONFIG_E,
        ENTRY_TYPE_IPLR_METERING_E,0,0}
    ,{PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_1_METERING_CONFIG_E,
        ENTRY_TYPE_IPLR_METERING_E,0,0}
    ,{PRV_CPSS_DXCH_BOBK_TABLE_EGRESS_POLICER_METERING_CONFIG_E,
        ENTRY_TYPE_EPLR_METERING_E,0,0}

    /* OAM */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_AGING_E,
        ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_MEG_EXCEPTION_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_INVALID_KEEPALIVE_HASH_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_KEEPALIVE_EXCESS_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_EXCEPTION_SUMMARY_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_TX_PERIOD_EXCEPTION_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_INGRESS_OAM_E,
         ENTRY_TYPE_OAM_E,0,0}

    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_AGING_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_MEG_EXCEPTION_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_SOURCE_INTERFACE_EXCEPTION_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_INVALID_KEEPALIVE_HASH_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_KEEPALIVE_EXCESS_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_EXCEPTION_SUMMARY_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_RDI_STATUS_CHANGE_EXCEPTION_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_TX_PERIOD_EXCEPTION_E,
         ENTRY_TYPE_OAM_E,32,0}/*32 oam per line */
    ,{PRV_CPSS_DXCH_LION3_TABLE_EGRESS_OAM_E,
         ENTRY_TYPE_OAM_E,0,0}

    ,{PRV_CPSS_DXCH_LION3_TABLE_TTI_PTP_COMMAND_E,
        ENTRY_TYPE_NOT_VALID_E,0,0}

    ,{PRV_CPSS_DXCH_LION3_TABLE_TXQ_LINK_LIST_COUNTERS_Q_MAIN_DESC_E,
         ENTRY_TYPE_TXQ_QUEUES_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_COUNTERS_Q_MAIN_BUFF_E,
         ENTRY_TYPE_TXQ_QUEUES_E,0,0}
    ,{PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_COUNTERS_Q_MAIN_MC_BUFF_E,
         ENTRY_TYPE_TXQ_QUEUES_E,0,0}


    ,{PRV_CPSS_DXCH_LION3_TABLE_EQ_PROTECTION_LOC_E,
        ENTRY_TYPE_LOC_E,32,0}/*32 entries per line */
    ,{PRV_CPSS_DXCH_BOBCAT3_TABLE_EQ_TX_PROTECTION_LOC_E,
        ENTRY_TYPE_LOC_E,32,0}/*32 entries per line */

    ,{PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_TC_DP_MAPPER_E,
        ENTRY_TYPE_EXPLICIT_NUM_E,0,_4K}/*explicit state that 4K entries*/

    /*CPFC*/
    ,{PRV_CPSS_DXCH_LION3_TABLE_PFC_LLFC_COUNTERS_E,
        ENTRY_TYPE_EXPLICIT_NUM_E,0,624}/*explicit state that 624 entries*/

    /*TXQ DQ */
    ,{PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E,                    /*DQ[0]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E + 0,/*DQ[1]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E + 1,/*DQ[2]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E + 2,/*DQ[3]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E + 3,/*DQ[4]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E + 4,/*DQ[5]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}

    ,{PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E,                    /*DQ[0]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E + 0,/*DQ[1]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E + 1,/*DQ[2]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E + 2,/*DQ[3]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E + 3,/*DQ[4]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E + 4,/*DQ[5]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}

    ,{PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E,                    /*DQ[0]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E + 0,/*DQ[1]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E + 1,/*DQ[2]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E + 2,/*DQ[3]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E + 3,/*DQ[4]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E + 4,/*DQ[5]*/
         ENTRY_TYPE_TXQ_DQ_PORTS_E,0,0}

    /* pipe 1 support */
    ,{PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT___PIPE_1___E,
        ENTRY_TYPE_PHY_PORT_E,0,0}


    /* must be last */
    ,{PRV_CPSS_DXCH_TABLE_LAST_E , ENTRY_TYPE___LAST__E , 0, 0}
};




/* number of bits in the sip5_20 - EGF_QAG egress eport entry */
#define SIP5_20_EGF_QAG_PORT_TARGET_BITS_SIZE_CNS    17
/* number of bits in the sip5_20 - EGF_QAG egress eport entry */
#define SIP5_20_EGF_QAG_EGRESS_EVLAN_BITS_SIZE_CNS   (16*3)


/* number of bits in the sip5_20 - TTI physical port entry 2 */
#define SIP5_20_TTI_PHYSICAL_PORT_2_BITS_SIZE_CNS       251
/* number of bits in the sip5_20 - TTI PCL UDB entry */
#define SIP5_20_TTI_PCL_UDB_BITS_SIZE_CNS               770

/* number of bits in the sip5_20 - PCL unit PCL UDB select entry*/
#define SIP5_20_IPCL_UDB_SELECT_BITS_SIZE_CNS       506


/* number of bits in the sip5_20 - IPvX unit Router Nexthop entry */
#define SIP5_20_IPVX_ROUTER_NEXTHOP_BITS_SIZE_CNS   106

/* number of bits in the sip5_20 - MLL entry */
#define SIP5_20_LION3_MLL_ENTRY_BITS_SIZE_CNS   162



/*xxx*/
/* bobcat3 table - in runtime copy table from this of bobk ... and continue from there ... */
static PRV_CPSS_DXCH_TABLES_INFO_STC bobcat3TablesInfo[PRV_CPSS_DXCH_BOBCAT3_TABLE_LAST_E] =
{
    {0}
    /* in runtime the tables of the bobk will be copied here ,
       and will be updated by the relevant bobcat3 code -->
       see bobcat3InitTable() */

    /* in runtime the 'Extra tables' of the bobk will be copied here*/
};
static const GT_U32 bobcat3TableInfoSize =
    (NUM_ELEMENTS_IN_ARR_MAC(bobcat3TablesInfo));

/* indication that directAccessBobcat3TableInfo was initialized */
static GT_BOOL directAccessBobCat3TableInfo_initDone = GT_FALSE;
/* copied in runtime from directAccessBobkTableInfo[] */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC directAccessBobcat3TableInfo[PRV_DXCH_BOBCAT3_TABLE___LAST_E] =
{
    {0}
    /* in runtime the tables of the bobk will be copied here ,
       and will be updated by the relevant bobcat3 code -->
       see bobcat3InitTable() */

    /* in runtime the 'Extra tables' of the bobcat3 will be copied here from :
        directAccessBobcat3TableInfo_new[] */
};

#define MULTI_INSTANCE_TABLE_ENTRY_MAC(                             \
        localTableType/*PRV_DXCH_BOBCAT2_TABLES_ENT*/,              \
        globalTableType/*PRV_CPSS_DXCH_TABLE_ENT*/,                 \
        baseUnitIndex /*PRV_CPSS_DXCH_UNIT_ENT*/,                   \
        instanceId ,                                                \
        table_addr,                                                 \
        bytesAlignment)                                             \
     {localTableType + instanceId,                                  \
        {globalTableType + instanceId, baseUnitIndex + instanceId,  \
        {table_addr, bytesAlignment, NOT_MODIFIED_VALUE_CNS}}\
    }

#define DP_MULTI_INSTANCE_TABLE_ENTRY_MAC(                          \
        localTableType/*PRV_DXCH_BOBCAT2_TABLES_ENT*/,              \
        globalTableType/*PRV_CPSS_DXCH_TABLE_ENT*/,                 \
        baseUnitIndex /*PRV_CPSS_DXCH_UNIT_ENT*/,                   \
        table_addr,                                                 \
        bytesAlignment)                                             \
     MULTI_INSTANCE_TABLE_ENTRY_MAC(localTableType,globalTableType,baseUnitIndex,0,table_addr,bytesAlignment) \
    ,MULTI_INSTANCE_TABLE_ENTRY_MAC(localTableType,globalTableType,baseUnitIndex,1,table_addr,bytesAlignment) \
    ,MULTI_INSTANCE_TABLE_ENTRY_MAC(localTableType,globalTableType,baseUnitIndex,2,table_addr,bytesAlignment) \
    ,MULTI_INSTANCE_TABLE_ENTRY_MAC(localTableType,globalTableType,baseUnitIndex,3,table_addr,bytesAlignment) \
    ,MULTI_INSTANCE_TABLE_ENTRY_MAC(localTableType,globalTableType,baseUnitIndex,4,table_addr,bytesAlignment)



/* Bobcat3 direct table information , that are new (not exists in bobk) */
/* register new tables that not exists in bobk !!!! */
static BOBK_TABLES_INFO_DIRECT_NEW_INDEX_STC directAccessBobcat3TableInfo_new[] =
{
    /* NOTE: addresses in this table MUST be '0' based (relative to unit) and
       not global address of the device*/

    /* NOTE: the alignment is taken auto calculated from bobcat3TablesInfo_overrideBobk[] */

    {PRV_CPSS_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E,
        {PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E, PRV_CPSS_DXCH_UNIT_EGF_QAG_E,
        {0x00921000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}}
    }

    ,{PRV_CPSS_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
        {PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E, PRV_CPSS_DXCH_UNIT_TTI_E,
        {0x00160000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}}
    }

    ,{PRV_CPSS_BOBCAT3_TABLE_EQ_TX_PROTECTION_LOC_E,
        {PRV_CPSS_DXCH_BOBCAT3_TABLE_EQ_TX_PROTECTION_LOC_E, PRV_CPSS_DXCH_UNIT_EQ_E,
        {0x00b10000, 0x4, PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}
    }

    ,{PRV_CPSS_BOBCAT3_TABLE_EGF_QAG_TC_DP_MAPPER_E,
        {PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_TC_DP_MAPPER_E, PRV_CPSS_DXCH_UNIT_EGF_QAG_E,
        {0x00900000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}}
    }

    /* generate extra 5 tables of PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E
       to support the DQ[1] .. DQ[5] */
    ,DP_MULTI_INSTANCE_TABLE_ENTRY_MAC(
        PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E,
        PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E,
        PRV_CPSS_DXCH_UNIT_TXQ_DQ1_E ,
        0x0000D800 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EGRESS_STC_TABLE_BITS_SIZE_CNS))

    /* generate extra 5 tables of PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E
       to support the DQ[1] .. DQ[5] */
    ,DP_MULTI_INSTANCE_TABLE_ENTRY_MAC(
        PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E,
        PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E,
        PRV_CPSS_DXCH_UNIT_TXQ_DQ1_E ,
        0x00006000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PRIO_TOKEN_BUCKET_CONFIGURATION_BITS_SIZE_CNS))

    /* generate extra 5 tables of PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
       to support the DQ[1] .. DQ[5] */
    ,DP_MULTI_INSTANCE_TABLE_ENTRY_MAC(
        PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E,
        PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E,
        PRV_CPSS_DXCH_UNIT_TXQ_DQ1_E ,
        0x0000A000 , BITS_TO_BYTES_ALIGNMENT_MAC(LION3_PORT_TOKEN_BUCKET_CONFIGURATION_BITS_SIZE_CNS))



    /* extra tables of PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E
       to support the pipe 1 */
    ,{PRV_CPSS_BOBCAT3_MULTI_INSTANCE_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT___PIPE_1___E,
        {PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT___PIPE_1___E, PRV_CPSS_DXCH_UNIT_EQ_1_E,
        {0x00040000, BITS_TO_BYTES_ALIGNMENT_MAC(LION3_EQ_INGRESS_STC_PHYSICAL_PORT_SIZE_CNS), PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS}}
    }

};

/* Bobcat3 direct table information , that need to override bobk info from directAccessBobcat3TableInfo */
static PRV_CPSS_DXCH_TABLES_INFO_DIRECT_EXT_STC directAccessBobcat3TableInfo_overrideBobk[] =
{
    /* NOTE: addresses in this table MUST be '0' based (relative to unit) and
       not global address of the device*/

    /* NOTE: the alignment when stated NOT_MODIFIED_VALUE_CNS :
        is taken auto calculated from bobcat3TablesInfo_overrideBobk[] */

    {PRV_CPSS_DXCH_LION3_TABLE_HA_GLOBAL_MAC_SA_E,
    {0x00005000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}},

    {PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E,
    {0x02400000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_CPU_CODE_TO_LOOPBACK_MAPPER_E,
    {0x00900400, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_BMA_PORT_MAPPING_E,
    {0x0005A000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}},

    /* NOTE: this table in sip5_20 is called "Port Enq Attributes Table" (Port_Enq_Attributes_Table) */
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_TARGET_PORT_MAPPER_E,
    {0x00922000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}},


    {PRV_CPSS_DXCH_LION3_TABLE_TXQ_LINK_LIST_COUNTERS_Q_MAIN_DESC_E,
    {0x00040000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_COUNTERS_Q_MAIN_BUFF_E,
    {0x000c0000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}},

    {PRV_CPSS_DXCH_LION3_TABLE_TAIL_DROP_COUNTERS_Q_MAIN_MC_BUFF_E,
    {0x000d0000, NOT_MODIFIED_VALUE_CNS, NOT_MODIFIED_VALUE_CNS}},



};

/*  table information , that need to override Bobk info from bobcat3TablesInfo */
/* NOTE : the entries in this array need to be matched with entries in
        bobkTablesInfo , according to field */
static PRV_CPSS_DXCH_TABLES_INFO_EXT_STC bobcat3TablesInfo_overrideBobk[] =
{
    /*SHT*/
    {PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E,
        OVERRIDE_INFO_WIDTH_MAC(BMP_256_PORTS_CNS)
    },
    {PRV_CPSS_DXCH_TABLE_MULTICAST_E,
        OVERRIDE_INFO_WIDTH_MAC(BMP_512_PORTS_CNS)
    },
    {PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E,
        OVERRIDE_INFO_WIDTH_MAC(BMP_512_PORTS_CNS)
    },
    {PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E,
        OVERRIDE_INFO_WIDTH_MAC(BMP_512_PORTS_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EPORT_FILTER_E,
        OVERRIDE_INFO_NUM_ENTRIES_AND_WIDTH_MAC(_16K,64)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E,
        OVERRIDE_INFO_WIDTH_MAC(BMP_512_PORTS_CNS)
    },
    {PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
        OVERRIDE_INFO_WIDTH_MAC(2*BMP_512_PORTS_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_VLAN_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_EGF_QAG_EGRESS_EVLAN_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_TTI_PCL_UDB_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_IPCL0_UDB_SELECT_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_IPCL_UDB_SELECT_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_IPCL1_UDB_SELECT_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_IPCL_UDB_SELECT_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_IPCL2_UDB_SELECT_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_IPCL_UDB_SELECT_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_EGF_QAG_PORT_TARGET_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_BOBCAT3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_2_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_TTI_PHYSICAL_PORT_2_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_IPVX_ROUTER_NEXTHOP_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_LION3_MLL_ENTRY_BITS_SIZE_CNS)
    },
    {PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_E,
        OVERRIDE_INFO_WIDTH_MAC(SIP5_20_LION3_MLL_ENTRY_BITS_SIZE_CNS)
    },

};

typedef struct{
    INDIRECT_INDEX_ENT               indirectIndex;/*index in indirectAccessTableInfo[]*/
    PRV_CPSS_DXCH_UNIT_ENT           unitIndex;    /*index of the unit*/
}INDIRECT_EXT_INFO_STC;

/* the connection between indirect table and it's unit */
static INDIRECT_EXT_INFO_STC bindIndirectToUnit[] =
{
    {PRV_CPSS_LION3_FDB_TABLE_INDIRECT_E,/* used for indirect access to 'PRV_CPSS_DXCH_TABLE_FDB_E' */
     PRV_CPSS_DXCH_UNIT_FDB_E
    },

    {PRV_CPSS_LION3_IOAM_TABLE_INDIRECT_E,
     PRV_CPSS_DXCH_UNIT_IOAM_E
    },

    {PRV_CPSS_LION3_EOAM_TABLE_INDIRECT_E,
     PRV_CPSS_DXCH_UNIT_EOAM_E
    },
};


static void lionInitTable(void);
static void lion2InitTable(void);
static GT_STATUS bobcat2InitTable(GT_U8 devNum);
static GT_STATUS bobkInitTable(GT_U8 devNum);
static GT_STATUS bobcat3InitTable(GT_U8 devNum);

static void indexAsPortNumConvert
(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    OUT GT_U32                 *portGroupIdPtr,
    OUT GT_U32                 *updatedEntryIndexPtr,
    OUT PRV_CPSS_DXCH_TABLE_ENT *updatedTableTypePtr
);

extern void  copyBits(
    IN GT_U32                  *targetMemPtr,
    IN GT_U32                  targetStartBit,
    IN GT_U32                  *sourceMemPtr,
    IN GT_U32                  sourceStartBit,
    IN GT_U32                  numBits
);

static GT_BOOL multiEntriesInLineIndexAndGlobalBitConvert
(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    INOUT GT_U32               *entryIndexPtr,
    INOUT GT_U32               *fieldOffsetPtr,
    OUT GT_U32                 *numBitsPerEntryPtr,
    OUT GT_U32                 *numEntriesPerLinePtr
);

/*******************************************************************************
* directAccessGlobalIndexToLocalIndexConvert
*
* DESCRIPTION:
*       direct tables info :
*       find global index in the array and return the 'Local index' of the
*       matched index.
*
* INPUTS:
*       globalIndex - global Index (need to match infoDirectUnitPtr[localIndex].globalIndex)
*       infoDirectUnitPtr - (pointer to) direct table info
*       numOfElements - number of elements under infoDirectUnitPtr
*
* OUTPUTS:
*       localIndexPtr - (pointer to) the local index that matched
*                   infoDirectUnitPtr[localIndex].globalIndex == globalIndex
*
* RETURNS:
*       GT_OK        - on success.
*       GT_NOT_FOUND - when globalIndex was not matched
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS directAccessGlobalIndexToLocalIndexConvert(
    IN PRV_CPSS_DXCH_TABLE_ENT                      globalIndex,
    IN PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC    *infoDirectUnitPtr,
    IN GT_U32                                       numOfElements,
    OUT GT_U32                                      *localIndexPtr
)
{
    GT_U32      ii;

    *localIndexPtr = 0;

    for(ii = 0 ; ii < numOfElements; ii++)
    {
        if(infoDirectUnitPtr[ii].globalIndex == globalIndex)
        {
            /* match found */
            *localIndexPtr = ii;
            return GT_OK;
        }
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);

}

/*******************************************************************************
* checkStatusOfPreviousAccess
*
* DESCRIPTION:
*       Check status of previous access.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id
*       tableInfoPtr - pointer to table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS checkStatusOfPreviousAccess
(
    IN GT_U8                                 devNum,
    IN GT_U32                                portGroupId,
    IN PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *tableInfoPtr
)
{
    return prvCpssPortGroupBusyWait(devNum,portGroupId,
                tableInfoPtr->controlReg,
                tableInfoPtr->trigBit,
                GT_FALSE);
}


/*******************************************************************************
* setReadWriteCommandToControlReg
*
* DESCRIPTION:
*       Set Read / Write command to Control register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId  - the port group Id
*       tableInfoPtr - pointer to table
*       entryIndex   - index in the table
*       accessAction - access action to table: PRV_CPSS_DXCH_ACCESS_ACTION_READ_E,
*                                              PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS setReadWriteCommandToControlReg
(
    IN GT_U8                                    devNum,
    IN GT_U32                                   portGroupId,
    IN PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC   *tableInfoPtr,
    IN GT_U32                                   entryIndex,
    IN PRV_CPSS_DXCH_ACCESS_ACTION_ENT          accessAction
)
{
   GT_U32 controlValue;

   controlValue  = (entryIndex << tableInfoPtr->indexBit);

   /* trigger the action */
   controlValue |= (1 << tableInfoPtr->trigBit);

   /* add the specific table when this control register has more than one table
   if there is no specific table, specificTableValue = 0*/
   controlValue |= (tableInfoPtr->specificTableValue <<
                     tableInfoPtr->specificTableBit);

   /* set type of action - R/W */
   controlValue |= (accessAction << tableInfoPtr->actionBit);

   return prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId, tableInfoPtr->controlReg, controlValue);


}

/*******************************************************************************
* tableFieldsFormatInfoGet
*
* DESCRIPTION:
*       function return the table format that relate to the specific table.
*       for not supported table --> return NULL
*
* INPUTS:
*       devNum          - device number
*       tableType       - the specific table name
*
* OUTPUTS:
*       None.
*
*
* RETURNS:
*       pointer to 'Table format info'
*
* COMMENTS:
*
*
*******************************************************************************/
static PRV_CPSS_DXCH_PP_TABLE_FORMAT_INFO_STC *tableFieldsFormatInfoGet
(
    IN GT_U8 devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType
)
{
    PRV_CPSS_DXCH_TABLE_FORMAT_ENT tableFormat;
    switch(tableType)
    {
        /* TTI */
        case PRV_CPSS_DXCH_LION3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_TTI_PHYSICAL_PORT_ATTRIBUTE_E;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_TTI_DEFAULT_EPORT_E;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_POST_TTI_LOOKUP_INGRESS_EPORT_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_TTI_EPORT_ATTRIBUTES_E;
            break;
        case PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E;
            break;

        /* L2I */
        case PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_BRIDGE_INGRESS_EPORT_E;
            break;
        case PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_BRIDGE_INGRESS_EVLAN_E;
            break;

        /* FDB */
        case PRV_CPSS_DXCH_TABLE_FDB_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_FDB_E;
            break;

        /* MLL */
        case PRV_CPSS_DXCH_LION3_TABLE_IP_MLL_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_IP_MLL_E;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_L2_MLL_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_L2_MLL_E;
            break;

        /* EGF_QAG */
        case PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_EPORT_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_EGF_QAG_EGRESS_EPORT_E;
            break;

        case PRV_CPSS_DXCH_BOBCAT3_TABLE_EGF_QAG_PORT_TARGET_ATTRIBUTES_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_EGF_QAG_PORT_TARGET_ATTRIBUTES_E;
            break;

        /* EGF_SHT */
        case PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EVLAN_E;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_VLAN_ATTRIBUTES_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_VLAN_ATTRIBUTES_E;
            break;

        case PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_EGRESS_EPORT_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EPORT_E;
            break;

        /* HA */
        case PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_2_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_2_E;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_HA_PHYSICAL_PORT_1_E;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_HA_PHYSICAL_PORT_2_E;
            break;
        case PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_TUNNEL_START_E;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_HA_GENERIC_TS_PROFILE_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_HA_GENERIC_TS_PROFILE_E;
            break;
        /*case PRV_CPSS_LION3_TABLE_NAT44_CONFIG_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_NAT44_E;
            break;*/
        case PRV_CPSS_DXCH_LION3_TABLE_IPVX_INGRESS_EPORT_E:
            tableFormat = PRV_CPSS_DXCH_TABLE_FORMAT_ROUTER_EPORT_E;
            break;
        default:
            return NULL;
    }

    return &PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo[tableFormat];
}

/*******************************************************************************
* convertFieldInfoToGlobalBit
*
* DESCRIPTION:
*       function converts : fieldWordNum,fieldOffset,fieldLength
*       from value relate to PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*
* INPUTS:
*       devNum  - device number
*       tableType - the specific table name
*       fieldWordNumPtr - field word number  PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*       fieldOffsetPtr  - field offset       field name
*       fieldLengthPtr - field length        number of fields or bit offset in field
*       value          - value is checked to not 'over flow' the number of bits.
*                        this is to remove the need from the 'cpss APIs' to be aware to field size!
*
* OUTPUTS:
*       fieldWordNumPtr - field word number PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*       fieldOffsetPtr  - field offset      global bit position
*       fieldLengthPtr - field length       number of bits
*
*
* RETURNS:
*       GT_OK - success
*       GT_BAD_STATE - fail
*       GT_OUT_OF_RANGE - when value > (max valid value)
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS    convertFieldInfoToGlobalBit(
    IN GT_U8        devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    INOUT GT_U32   *fieldWordNumPtr,
    INOUT GT_U32   *fieldOffsetPtr,
    INOUT GT_U32   *fieldLengthPtr,
    IN    GT_U32    value
)
{
    PRV_CPSS_DXCH_PP_TABLE_FORMAT_INFO_STC *tableFieldsFormatInfoPtr;
    PRV_CPSS_ENTRY_FORMAT_TABLE_STC *tableFieldsFormatPtr;
    GT_U32   fieldId = *fieldOffsetPtr;
    GT_U32   totalLength;
    GT_U32   numOfFields = *fieldLengthPtr;
    GT_U32   tmpLen;
    GT_U32   tmpOffset;

    tableFieldsFormatInfoPtr = tableFieldsFormatInfoGet(devNum, tableType);
    if(tableFieldsFormatInfoPtr == NULL)
    {
        /* the table is not bound to fields format */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    tableFieldsFormatPtr = tableFieldsFormatInfoPtr->fieldsInfoPtr;

    if(tableFieldsFormatPtr == NULL)
    {
        /* the table is not bound to fields format */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* convert the value into 'global field offsets' */
    *fieldWordNumPtr = PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS;
    GET_FIELD_INFO_BY_ID_MAC(tableFieldsFormatPtr, fieldId, *fieldOffsetPtr,totalLength);

    if(numOfFields)
    {
        /* indication to use the 'Auto calc' or to use 'Length caller' ...
           this to support setting of consecutive' fields ! */
        while(--numOfFields)
        {
            fieldId ++;
            GET_FIELD_INFO_BY_ID_MAC(tableFieldsFormatPtr, fieldId, tmpOffset,tmpLen);
            totalLength += tmpLen;
        }
    }

    *fieldLengthPtr = totalLength;

    if(totalLength <= 32 &&
        (value > BIT_MASK_MAC(totalLength)))
    {
        /* value > (max valid value) */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* convertSubFieldInfoToGlobalBit
*
* DESCRIPTION:
*       the function retain parameters of 'sub-field' with in 'parent field'.
*       function converts : fieldWordNum,fieldOffset,fieldLength
*       from value relate to PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*       to value relate to PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*
* INPUTS:
*       devNum  - device number
*       tableType - the specific table name
*       subFieldOffset - the bit index of the sub field (within the 'parent field')
*       subFieldLength - the length of the sub field
*       fieldWordNumPtr - field word number  PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*       fieldOffsetPtr  - field offset       field name
*       fieldLengthPtr - not used
*       value          - value is checked to not 'over flow' the number of bits.
*                        this is to remove the need from the 'cpss APIs' to be aware to field size!
*
* OUTPUTS:
*       fieldWordNumPtr - field word number PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*       fieldOffsetPtr  - field offset      global bit position
*       fieldLengthPtr - field length       number of bits
*
*
* RETURNS:
*       GT_OK - success
*       GT_BAD_STATE - fail
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS    convertSubFieldInfoToGlobalBit(
    IN GT_U8       devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN    GT_U32   subFieldOffset,
    IN    GT_U32   subFieldLength,
    INOUT GT_U32   *fieldWordNumPtr,
    INOUT GT_U32   *fieldOffsetPtr,
    INOUT GT_U32   *fieldLengthPtr,
    IN    GT_U32    value
)
{
    GT_STATUS    rc;


    /* rest the fieldLength , before calling convertFieldInfoToGlobalBit */
    *fieldLengthPtr = PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS;

    rc = convertFieldInfoToGlobalBit(devNum,tableType,fieldWordNumPtr,fieldOffsetPtr,fieldLengthPtr,value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* (*fieldWordNumPtr) value is PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS */

    /* calc global bit index of the sub field */
    (*fieldOffsetPtr) += subFieldOffset;

    /* use the length as defined by the caller for the sub field */
    *fieldLengthPtr = subFieldLength;

    return GT_OK;

}

/*******************************************************************************
* convertPatternFieldInfoToGlobalBit
*
* DESCRIPTION:
*       function converts : fieldWordNum,fieldOffset,fieldLength
*       from value related to PRV_CPSS_DXCH_TABLE_WORD_INDICATE_PATTERN_FIELD_NAME_CNS
*       to value relate to PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*
* INPUTS:
*       devNum  - device number
*       tableType - the specific table name
*       entryIndexPtr - the entry index
*       fieldWordNumPtr - field word number  PRV_CPSS_DXCH_TABLE_WORD_INDICATE_PATTERN_FIELD_NAME_CNS
*       fieldOffsetPtr  - field offset       field name
*       fieldLengthPtr - not used
*       val            - value is checked to not 'overflow' the number of bits.
*                        this is to remove the need from the 'cpss APIs' to be aware to field size!
*
* OUTPUTS:
*       entryIndexPtr - the table entry index
*       fieldWordNumPtr - field word number PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*       fieldOffsetPtr  - field offset      global bit position
*       fieldLengthPtr - field length       number of bits
*
*
* RETURNS:
*       GT_OK - success
*       GT_BAD_STATE - fail
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS    convertPatternFieldInfoToGlobalBit(
    IN GT_U8       devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    INOUT GT_U32   *entryIndexPtr,
    INOUT GT_U32   *fieldWordNumPtr,
    INOUT GT_U32   *fieldOffsetPtr,
    INOUT GT_U32   *fieldLengthPtr,
    IN    GT_U32    val
)
{
    GT_STATUS    rc;
    PRV_CPSS_DXCH_PP_TABLE_FORMAT_INFO_STC *  fieldsInfoPtr;
    GT_U32 patternIndex;

    fieldsInfoPtr = tableFieldsFormatInfoGet(devNum, tableType);
    if(fieldsInfoPtr == NULL)
    {
        /* the table is not bound to fields format */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* Convert entry index to pattern index */
    patternIndex = (*entryIndexPtr) % fieldsInfoPtr->patternNum;

    /* Claculate global field bit and length */
    rc = convertFieldInfoToGlobalBit(devNum, tableType, fieldWordNumPtr, fieldOffsetPtr, fieldLengthPtr, val);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* Calculate global bit index pattern field */
    (*fieldOffsetPtr) += patternIndex * fieldsInfoPtr->patternBits;

    /* Calculate table entry index */
    (*entryIndexPtr) /= fieldsInfoPtr->patternNum;

    return GT_OK;

}

/*******************************************************************************
* indirectReadTableEntry
*
* DESCRIPTION:
*       Read a whole entry from table by indirect access method.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId  - the portGroupId
*       tableType - the specific table name
*       tableInfoPtr - pointer to table information
*       entryIndex   - index in the table
*       tablePtr     - pointer to indirect information
*
* OUTPUTS:
*       entryValuePtr - pointer to the data read from the table
*
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS indirectReadTableEntry
(
    IN GT_U8                                    devNum,
    IN GT_U32                                   portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT                  tableType,
    IN PRV_CPSS_DXCH_TABLES_INFO_STC            *tableInfoPtr,
    IN GT_U32                                   entryIndex,
    IN PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC   *tablePtr,
    OUT GT_U32                                  *entryValuePtr
)
{
    GT_STATUS   rc;
    GT_U32      wordNum; /* iterator */

    /* busy wait for the control reg to show "read was done" */
    rc = checkStatusOfPreviousAccess(devNum, portGroupId,tablePtr);
    if(rc != GT_OK)
        return rc;
    /* set Control register for Read action */
    rc = setReadWriteCommandToControlReg(devNum, portGroupId, tablePtr, entryIndex,
                                          PRV_CPSS_DXCH_ACCESS_ACTION_READ_E);
    if(rc != GT_OK)
        return rc;

    /* busy wait for the control reg to show "read was done" */
    rc = checkStatusOfPreviousAccess(devNum, portGroupId,tablePtr);
    if(rc != GT_OK)
        return rc;

    /* table entry read - read to the buffer  - entryValuePtr != NULL */
    /* field table read, field table write - entryValuePtr == NULL  */
    if( entryValuePtr != NULL)
    {
        /* VLAN and STG indirect access tables need word swapping */
        if((tableType == PRV_CPSS_DXCH_TABLE_VLAN_E) ||
           (tableType == PRV_CPSS_DXCH_TABLE_STG_E))
        {
            for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
            {
                /* read data register and perform swapping */
                rc = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,tablePtr->dataReg + wordNum * 4,
                                           entryValuePtr +
                                      (tableInfoPtr->entrySize - wordNum - 1));
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        else
        {
            /* read all data registers */
            rc =  prvCpssHwPpPortGroupReadRam(devNum,portGroupId,
                            tablePtr->dataReg,
                            tableInfoPtr->entrySize,/* num of words */
                            entryValuePtr);
        }

    }

    return rc;
}

/*******************************************************************************
* indirectWriteTableEntry
*
* DESCRIPTION:
*       Write a whole entry to table by indirect access method.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id
*       tableType - the specific table name
*       tableInfoPtr - pointer to table information
*       entryIndex   - index in the table
*       entryValuePtr - pointer to the data read from the table
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS indirectWriteTableEntry
(
    IN GT_U8                            devNum,
    IN GT_U32                           portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT          tableType,
    IN PRV_CPSS_DXCH_TABLES_INFO_STC    *tableInfoPtr,
    IN GT_U32                           entryIndex,
    IN GT_U32                           *entryValuePtr
)
{
    GT_U32      wordNum; /* iterator */
    GT_STATUS   rc;
    /* pointer to indirect table info */
    PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *tablePtr;
    GT_U32      address;  /* address to write the information */

    tablePtr =
         (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC*)(tableInfoPtr->writeTablePtr);

    /* busy wait for the control reg to show "read was done" */
    rc = checkStatusOfPreviousAccess(devNum, portGroupId,tablePtr);
    if(rc != GT_OK)
        return rc;

    /* entryValuePtr == NULL for Cheetah VB init of VLAN table */
    if(entryValuePtr != NULL)
    {
        /* VLAN and STG indirect access tables need word swapping */
        if((tableType == PRV_CPSS_DXCH_TABLE_VLAN_E) ||
           (tableType == PRV_CPSS_DXCH_TABLE_STG_E))
        {
            for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
            {
                address = tablePtr->dataReg + wordNum * 4;
                /* write to data register and perform swapping */
                rc = prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId,address,
                           entryValuePtr[tableInfoPtr->entrySize - wordNum - 1]);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

        }
        else
        {
           /* non-direct table --- write to it as continues memory */
            rc = prvCpssHwPpPortGroupWriteRam(devNum,portGroupId,
                             tablePtr->dataReg,
                             tableInfoPtr->entrySize,
                             entryValuePtr);

            if (rc != GT_OK)
            {
                return rc;
            }

        }

    }

    /* set Control register for Write action */
    return setReadWriteCommandToControlReg(devNum, portGroupId, tablePtr, entryIndex,
                                            PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E);

}

/*******************************************************************************
* indirectReadTableEntryField
*
* DESCRIPTION:
*       Read a field of entry from table by indirect access method.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id
*       tableType - the specific table name
*       tableInfoPtr - pointer to table information
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*       fieldOffset  - field offset
*       fieldLength - field length
*
* OUTPUTS:
*       fieldValuePtr - pointer to the data read from the table
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/

static GT_STATUS indirectReadTableEntryField
(
    IN GT_U8                        devNum,
    IN GT_U32                       portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT       tableType,
    IN PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr,
    IN GT_U32                       entryIndex,
    IN GT_U32                       fieldWordNum,
    IN GT_U32                       fieldOffset,
    IN GT_U32                       fieldLength,
    OUT GT_U32                      *fieldValuePtr
)
{
    GT_STATUS   rc;
    GT_U32 address; /* address to read from */
    /* pointer to indirect table info */
    PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *tablePtr;

    tablePtr = (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC*)(tableInfoPtr->readTablePtr);

    /* prepare to read action */
    rc = indirectReadTableEntry(devNum,portGroupId,tableType,tableInfoPtr,
                                entryIndex, tablePtr, NULL);
    if(rc != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* VLAN and STG indirect access tables need word swapping */
    if((tableType == PRV_CPSS_DXCH_TABLE_VLAN_E) ||
       (tableType == PRV_CPSS_DXCH_TABLE_STG_E))
    {
        fieldWordNum = tableInfoPtr->entrySize - fieldWordNum - 1;
    }

    address = tablePtr->dataReg + fieldWordNum * 0x4;

    /* read the field from the data register */
    if(prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,address, fieldOffset, fieldLength, fieldValuePtr))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }

    return GT_OK;

}


/*******************************************************************************
* indirectWriteTableEntryField
*
* DESCRIPTION:
*       Read a field from the indirect access table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id
*       tableType - the specific table name
*       tableInfoPtr - pointer to table information
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*       fieldOffset  - field offset
*       fieldLength - field length
*       fieldValue  - value that will be written to the table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/

static GT_STATUS indirectWriteTableEntryField
(
    IN GT_U8                          devNum,
    IN GT_U32                         portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT         tableType,
    IN PRV_CPSS_DXCH_TABLES_INFO_STC  *tableInfoPtr,
    IN GT_U32                         entryIndex,
    IN GT_U32                         fieldWordNum,
    IN GT_U32                         fieldOffset,
    IN GT_U32                         fieldLength,
    IN GT_U32                         fieldValue
)
{
    GT_STATUS   rc;
    GT_U32 address; /* address to read from */
    /* pointer to indirect table info */
    PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *tablePtr;

    tablePtr =
         (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC*)(tableInfoPtr->writeTablePtr);

    /* read / write tableEntryField per port group */
    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS && PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        GT_PORT_GROUPS_BMP  portGroupsBmp;/* bmp of port groups */
        if(GT_TRUE ==
            prvCpssDuplicatedMultiPortGroupsGet(devNum,portGroupId,tablePtr->controlReg,
                &portGroupsBmp,NULL,NULL))
        {
            /* when the entry involve 'per port info' and 'global info'
             (like vlan entry) between hemispheres , need to update each port group
             according to it's existing info and not according to 'representative'
             port group */

            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum, portGroupsBmp , portGroupId)
            {
                rc = indirectWriteTableEntryField(
                    devNum, portGroupId, tableType, tableInfoPtr,
                    entryIndex, fieldWordNum,
                    fieldOffset, fieldLength, fieldValue);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum, portGroupsBmp , portGroupId)

            return GT_OK;
        }
    }

    /* read the entry -- put entry data to Data register */
    rc = indirectReadTableEntry(devNum,portGroupId , tableType, tableInfoPtr,
                                 entryIndex, tablePtr, NULL);
    if(rc != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* VLAN and STG indirect access tables need word swapping */
    if(((GT_U32)tableType == PRV_CPSS_DXCH_TABLE_VLAN_E) ||
       (tableType == PRV_CPSS_DXCH_TABLE_STG_E))
    {
        fieldWordNum = tableInfoPtr->entrySize - fieldWordNum - 1;
    }

    address = tablePtr->dataReg + fieldWordNum * 0x4;

    /* non-direct table write */
    rc =  prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,address,fieldOffset,fieldLength,fieldValue);
    if(rc != GT_OK)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* set Control register for Write action */
    return setReadWriteCommandToControlReg(devNum, portGroupId ,tablePtr, entryIndex,
                                            PRV_CPSS_DXCH_ACCESS_ACTION_WRITE_E);

}


/*******************************************************************************
* prvCpssDxChReadTableEntry
*
* DESCRIPTION:
*       Read a whole entry from the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*
* OUTPUTS:
*       entryValuePtr - (pointer to) the data read from the table
*                       may be NULL in the case of indirect table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send read entry command to device. And entry is stored in the
*      data registers of a indirect table
*******************************************************************************/
GT_STATUS prvCpssDxChReadTableEntry

(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    OUT GT_U32                 *entryValuePtr
)
{
    GT_U32      portGroupId;/* support multi-port-groups device for table accesses with index = 'per port' */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* convert if needed the entry index , get specific portGroupId (or all port groups) */
    indexAsPortNumConvert(devNum,tableType,entryIndex, &portGroupId , &entryIndex,&tableType);

    return prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,tableType,entryIndex,entryValuePtr);

}

/*******************************************************************************
* prvCpssDxChWriteTableEntry
*
* DESCRIPTION:
*       Write a whole entry to the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       entryValuePtr - (pointer to) the data that will be written to the table
*                       may be NULL in the case of indirect table.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send write entry command to device. And entry is taken from the
*      data registers of a indirect table.
*******************************************************************************/
GT_STATUS prvCpssDxChWriteTableEntry
(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    IN GT_U32                  *entryValuePtr
)
{
    GT_U32      portGroupId;/* support multi-port-groups device for table accesses with index = 'per port' */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* convert if needed the entry index , get specific portGroupId (or all port groups) */
    indexAsPortNumConvert(devNum,tableType,entryIndex, &portGroupId , &entryIndex,&tableType);

    return prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,tableType,entryIndex,entryValuePtr);
}

/*******************************************************************************
* prvCpssDxChReadTableEntryField
*
* DESCRIPTION:
*       Read a field from the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*                   if need global offset in the field of fieldOffset
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   if need 'field name' in the field of fieldOffset
*       fieldOffset  - field offset
*       fieldLength - field length
*                   when fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   the field 'fieldLength' is used ONLY if  it's value != 0
*                   and then it means 'Mumber of consecutive fields' !!!
*
* OUTPUTS:
*       fieldValuePtr - (pointer to) the data read from the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChReadTableEntryField
(
    IN GT_U8                  devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    OUT GT_U32                *fieldValuePtr
)
{
    GT_U32      portGroupId;/* support multi-port-groups device for table accesses with index = 'per port' */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* convert if needed the entry index , get specific portGroupId (or all port groups) */
    indexAsPortNumConvert(devNum,tableType,entryIndex, &portGroupId , &entryIndex,&tableType);

    return prvCpssDxChPortGroupReadTableEntryField(devNum,portGroupId ,
                tableType,entryIndex,fieldWordNum,
                fieldOffset,fieldLength,fieldValuePtr);
}

/*******************************************************************************
* prvCpssDxChWriteTableEntryField
*
* DESCRIPTION:
*       Write a field to the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*                   if need global offset in the field of fieldOffset
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   if need 'field name' in the field of fieldOffset
*       fieldOffset  - field offset
*       fieldLength - field length
*                   when fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   the field 'fieldLength' is used ONLY if  it's value != 0
*                   and then it means 'Mumber of consecutive fields' !!!
*       fieldValue - the data write to the table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChWriteTableEntryField
(
    IN GT_U8                  devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    IN GT_U32                 fieldValue
)
{
    GT_U32      portGroupId;/* support multi-port-groups device for table accesses with index = 'per port' */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* convert if needed the entry index , get specific portGroupId (or all port groups) */
    indexAsPortNumConvert(devNum,tableType,entryIndex, &portGroupId , &entryIndex,&tableType);

    return prvCpssDxChPortGroupWriteTableEntryField(devNum,portGroupId ,
                tableType,entryIndex,fieldWordNum,
                fieldOffset,fieldLength,fieldValue);

}

/*******************************************************************************
* prvCpssDxChTableNumEntriesGet
*
* DESCRIPTION:
*       get the number of entries in a table
*       needed for debug purpose
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries in the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTableNumEntriesGet
(
    IN GT_U8                    devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    OUT GT_U32                  *numEntriesPtr
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* validity check */
    if((GT_U32)tableType >= PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    if(numEntriesPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    *numEntriesPtr = tableInfoPtr->maxNumOfEntries;

    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChTablesAccessInit
*
* DESCRIPTION:
*       Initializes all structures for table access.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS  prvCpssDxChTablesAccessInit
(
    IN GT_U8    devNum
)
{
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    PRV_CPSS_DXCH_TABLES_INFO_STC      *accessTableInfoPtr;/*pointer to tables info*/
    GT_U32                              accessTableInfoSize;/*tables info size */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr = cheetahTablesInfo;
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoSize = cheetahTableInfoSize;

        /* update tables size according to "fine tuning" */
        cheetahTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E].maxNumOfEntries =
            fineTuningPtr->tableSize.routerArp;

    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr = cheetah2TablesInfo;
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoSize = cheetah2TableInfoSize;

        /* update tables size according to "fine tuning" */
        cheetah2TablesInfo[PRV_CPSS_DXCH2_LTT_TT_ACTION_E].maxNumOfEntries =
            PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(devNum);
        cheetah2TablesInfo[PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E].maxNumOfEntries =
            fineTuningPtr->tableSize.routerNextHop;
        cheetah2TablesInfo[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].maxNumOfEntries =
            fineTuningPtr->tableSize.tunnelStart;
    }
    else /* Cheetah3 */
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {
            accessTableInfoPtr = cheetah3TablesInfo;
            accessTableInfoSize = cheetah3TableInfoSize;
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            accessTableInfoPtr = dxChXcatTablesInfo;
            accessTableInfoSize = dxChXcatTableInfoSize;
            dxChXcatInitTable();
            if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                /* Logical Target Mapping Table */
                dxChXcatTablesInfo[PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E].maxNumOfEntries = 2048;
                dxChXcatTablesInfo[PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E].entrySize = 2;
            }
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
        {
            accessTableInfoPtr = dxChXcat3TablesInfo;
            accessTableInfoSize = dxChXcat3TableInfoSize;
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            accessTableInfoPtr = dxChXcat2TablesInfo;
            accessTableInfoSize = dxChXcat2TableInfoSize;
            dxChXcat2InitTable();
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            accessTableInfoPtr = lionBTablesInfo;
            accessTableInfoSize = lionBTableInfoSize;
            lionInitTable();
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
        {
            accessTableInfoPtr = lion2TablesInfo;
            accessTableInfoSize = lion2TableInfoSize;
            lion2InitTable();
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E)
        {   CPSS_TBD_BOOKMARK_BOBCAT3
            accessTableInfoPtr = bobcat3TablesInfo;
            accessTableInfoSize = bobcat3TableInfoSize;
            rc = bobcat3InitTable(devNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
        {
            if(PRV_CPSS_PP_MAC(devNum)->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
            {
                accessTableInfoPtr = bobkTablesInfo;
                accessTableInfoSize = bobkTableInfoSize;
                rc = bobkInitTable(devNum);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
            else
            {
                accessTableInfoPtr = bobcat2TablesInfo;
                accessTableInfoSize = bobcat2TableInfoSize;
                rc = bobcat2InitTable(devNum);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E)
        {
            /* aldrin is based on cetus design - use bobk tables */
            accessTableInfoPtr = bobkTablesInfo;
            accessTableInfoSize = bobkTableInfoSize;
            rc = bobkInitTable(devNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* not supported/implemented yet */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr = accessTableInfoPtr;
        PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoSize = accessTableInfoSize;

        /* update tables size according to "fine tuning" */
        accessTableInfoPtr[PRV_CPSS_DXCH3_LTT_TT_ACTION_E].maxNumOfEntries =
            PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(devNum);
        accessTableInfoPtr[PRV_CPSS_DXCH2_LTT_TT_ACTION_E].maxNumOfEntries =
            PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(devNum);
        accessTableInfoPtr[PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E].maxNumOfEntries =
            fineTuningPtr->tableSize.routerNextHop;
        /* Need to take the bigger value between ARP entries / 4 and TS entries */
        if ((PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerArp/4) >
            (PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart))
        {
            accessTableInfoPtr[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].maxNumOfEntries =
                PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerArp / 4;
        }
        else
        {
            accessTableInfoPtr[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].maxNumOfEntries =
                PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart;
        }
        accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E].maxNumOfEntries = 4160;

        /* Due to Cheetah 3 Errata - " FEr#2028: Direct access under traffic to Pre-Egress
         Engine (EQ) tables is not functional"
        -- see PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E))
        {
            /* PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E */
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E].readAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E].readTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E].writeAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E].writeTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E;

            /* PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E */
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E].readAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E].readTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E].writeAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E].writeTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E;

            /* PRV_CPSS_DXCH_TABLE_CPU_CODE_E */
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_CPU_CODE_E].readAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_CPU_CODE_E].readTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_CPU_CODE_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_CPU_CODE_E].writeAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_CPU_CODE_E].writeTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_CPU_CODE_E;

            /* PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E */
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E].readAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E].readTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E].writeAccessType =
                PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E].writeTablePtr =
                indirectAccessTableInfo + PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E;
        }


        /* FEr#2006 */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_VLT_INDIRECT_ACCESS_WA_E))
        {
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_VLAN_E].writeAccessType =
                PRV_CPSS_DXCH_DIRECT_ACCESS_E;
            accessTableInfoPtr[PRV_CPSS_DXCH_TABLE_VLAN_E].writeTablePtr =
                directAccessTableInfo + PRV_CPSS_DXCH_TABLE_VLAN_E;
        }

    }

    /* do additional TTI tables init */
    rc = prvCpssDxChTtiDbTablesInit(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntryFieldList
*
* DESCRIPTION:
*       Write a list of fields to the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType         - HW table Id
*       entryIndex        - entry Index
*       entryMemoBufArr   - the work memory for read, update and write the entry
*       fieldsAmount      - amount of updated fields in the entry
*       fieldOffsetArr    - (array) the offset of the field in bits
*                           from the entry origin
*       fieldLengthArr    - (array) the length of the field in bits,
*                           0 - means to skip this subfield
*       fieldValueArr     - (array) the value of the field
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupWriteTableEntryFieldList
(
    IN GT_U8                                  devNum,
    IN GT_U32                                 portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT                tableType,
    IN GT_U32                                 entryIndex,
    IN GT_U32                                 entryMemoBufArr[],
    IN GT_U32                                 fieldsAmount,
    IN GT_U32                                 fieldOffsetArr[],
    IN GT_U32                                 fieldLengthArr[],
    IN GT_U32                                 fieldValueArr[]
)
{
    GT_STATUS                  res;           /* return code               */
    GT_U32                     idx;           /* field index               */
    GT_U32                     wordIdx;       /* word index                */
    GT_U32                     wordShift;     /* Shift in the word         */
    GT_U32                     len0;          /* length in the 1-th word   */
    GT_U32                     len1;          /* length in the 2-th word   */

    /* read / write tableEntryField per port group */
    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        /* update table in all port groups */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
        {
            res = prvCpssDxChPortGroupWriteTableEntryFieldList(
                devNum, portGroupId, tableType, entryIndex,
                entryMemoBufArr, fieldsAmount,
                fieldOffsetArr, fieldLengthArr, fieldValueArr);
            if( res != GT_OK)
            {
                return res;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)

        return GT_OK;
    }

    res = prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId,tableType, entryIndex, entryMemoBufArr);
    if (res != GT_OK)
    {
        return res;
    }

    for (idx = 0; (idx < fieldsAmount); idx++)
    {
        if (fieldLengthArr[idx] > 32)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (fieldLengthArr[idx] == 0)
            continue;

        wordIdx   = fieldOffsetArr[idx] >> 5;
        wordShift = fieldOffsetArr[idx] & 0x1F;

        /* the length of the field or of it's intersection with the first word */
        len0 = ((wordShift + fieldLengthArr[idx]) <= 32)
            ? fieldLengthArr[idx] : (32 - wordShift);

        /* copy the field or it's intersection with the first word */
        U32_SET_FIELD_MASKED_MAC(
           entryMemoBufArr[wordIdx], wordShift, len0, fieldValueArr[idx]);

        /* copy the field intersection with the second word */
        if (len0 != fieldLengthArr[idx])
        {
            len1 = fieldLengthArr[idx] - len0;
            U32_SET_FIELD_MASKED_MAC(
               entryMemoBufArr[wordIdx + 1] ,0 /*offset*/, len1,
               (fieldValueArr[idx] >> len0));
        }
    }

    return  prvCpssDxChPortGroupWriteTableEntry(
        devNum, portGroupId,tableType, entryIndex, entryMemoBufArr);
}

/*******************************************************************************
* prvCpssDxChReadTableEntryFieldList
*
* DESCRIPTION:
*       Read a list of fields from the table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableId           - HW table Id
*       entryIndex        - entry Index
*       entryMemoBufArr   - the work memory for read, update and write the entry
*       fieldsAmount      - amount of updated fields in the entry
*       fieldOffsetArr    - (array) the offset of the field in bits
*                           from the entry origin
*       fieldLengthArr    - (array) the length of the field in bits,
*                           0 - means to skip this subfield

* OUTPUTS:
*       fieldValueArr     - (array) the value of the field
*
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupReadTableEntryFieldList
(
    IN  GT_U8                                  devNum,
    IN GT_U32                                 portGroupId,
    IN  PRV_CPSS_DXCH_TABLE_ENT                tableId,
    IN  GT_U32                                 entryIndex,
    IN  GT_U32                                 entryMemoBufArr[],
    IN  GT_U32                                 fieldsAmount,
    IN  GT_U32                                 fieldOffsetArr[],
    IN  GT_U32                                 fieldLengthArr[],
    OUT GT_U32                                 fieldValueArr[]
)
{
    GT_STATUS                  res;           /* return code               */
    GT_U32                     idx;           /* field index               */
    GT_U32                     wordIdx;       /* word index                */
    GT_U32                     wordShift;     /* Shift in the word         */
    GT_U32                     len0;          /* length in the 1-th word   */
    GT_U32                     len1;          /* length in the 2-th word   */

    res = prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId, tableId, entryIndex, entryMemoBufArr);
    if (res != GT_OK)
    {
        return res;
    }

    for (idx = 0; (idx < fieldsAmount); idx++)
    {
        if (fieldLengthArr[idx] > 32)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (fieldLengthArr[idx] == 0)
        {
            fieldValueArr[idx] = 0;
            continue;
        }

        wordIdx   = fieldOffsetArr[idx] >> 5;
        wordShift = fieldOffsetArr[idx] & 0x1F;

        /* the length of the field or of it's intersection with the first word */
        len0 = ((wordShift + fieldLengthArr[idx]) <= 32)
            ? fieldLengthArr[idx] : (32 - wordShift);

        /* copy the field or it's intersection with the first word */
        fieldValueArr[idx] =
            U32_GET_FIELD_MAC(entryMemoBufArr[wordIdx], wordShift, len0);

        /* copy the field intersection with the second word */
        if (len0 != fieldLengthArr[idx])
        {
            len1 = fieldLengthArr[idx] - len0;
            fieldValueArr[idx] |=
                ((U32_GET_FIELD_MAC(
                    entryMemoBufArr[wordIdx + 1], 0/*offset*/, len1))
                 << len0);
        }
    }

    return  GT_OK;
}


#ifdef CHEETAH_TABLE_DEBUG

GT_STATUS readWriteFieldTest
(
    IN GT_U8                  devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32 field;
    GT_U32 readVal;
    GT_U32 wordNum;
    GT_U32 fieldOffset;
    GT_STATUS rc;
    GT_STATUS fieldLength;

    fieldLength = 32;
    tableInfoPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr[tableType]);


    cpssOsSrand(5);

    for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
    {
        for(fieldOffset = 0; fieldOffset < 0; fieldOffset+=fieldLength)
        {
            field = cpssOsRand();
            rc= coreCheetahWriteTableEntryField(devNum,tableType,1,wordNum,
                                                fieldOffset, fieldLength,field);
            if(rc != GT_OK)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        }
    }

    cpssOsSrand(5);
    for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
    {
        for(fieldOffset = 0; fieldOffset < 0; fieldOffset+=fieldLength)
        {
            field = cpssOsRand();
            rc= coreCheetahReadTableEntryField(devNum,tableType,1,wordNum,
                                            fieldOffset, fieldLength, &readVal);
            if(rc != GT_OK)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            if(field != readVal)
            {
                cpssOsPrintf("\n wordNum =  %d ",wordNum);
                cpssOsPrintf("\n fieldOffset =  %d ",fieldOffset);
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

        }

    }
    cpssOsPrintf("\n Field test OK, tableType =  %d ",tableType);


    return GT_OK;
}




GT_U32 getMaskBits
(
    IN PRV_CPSS_DXCH_TABLE_ENT tableType
)
{
    GT_U32     maskBits;


        switch(tableType)
        {
            case PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E:
                maskBits = 0x7ff;
                break;

            case PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E:
                maskBits = 0x7ff;
                break;

            case PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E:
                maskBits = 0x7ff;
                break;

            case PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E:
                maskBits = 0xffffffff;
                break;

            case PRV_CPSS_DXCH_TABLE_CPU_CODE_E:
                maskBits = 0x3fff;
                break;

            case PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E:
                maskBits = 0x1fffffff;
                break;

            case PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E:
                maskBits = 0x3fff;
                break;

            case PRV_CPSS_DXCH_TABLE_REMARKING_E:
                maskBits = 0xff;
                break;

            case PRV_CPSS_DXCH_TABLE_STG_E:
                maskBits = 0x3fffff;
                break;

            case PRV_CPSS_DXCH_TABLE_VLAN_E:
                maskBits = 0xff7d7ffb;
                break;

            case PRV_CPSS_DXCH_TABLE_MULTICAST_E:
                maskBits = 0xfffffff;/* word1 */
                break;

            case PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E:
                maskBits = 0xff;
                break;

            case PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E:
                maskBits = 0xffff; /* word1 */
                break;

            case PRV_CPSS_DXCH_TABLE_FDB_E:
                maskBits = 0x3ff;
                break;

            default:
                maskBits = 0xffffffff;
                break;
        }

        return maskBits;
}



GT_STATUS readWriteEntryTest
(
    IN GT_U8                  devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32 wordValue[CH_TABLE_ENTRY_MAX_WORDS_SIZE_CNS];
    GT_U32 readValue[CH_TABLE_ENTRY_MAX_WORDS_SIZE_CNS];
    GT_U32 wordNum;
    GT_U32 seed;
    GT_U32 entryNum;
    GT_STATUS rc;
#ifdef TBL_DEBUG_SIMULATION
    GT_STATUS i;
    GT_U32     trunkId;
    GT_U32     trunkMemberIdx;
    GT_U32 directReadValue[CH_TABLE_ENTRY_MAX_WORDS_SIZE_CNS];
    GT_U32 regAddress;
#endif /*TBL_DEBUG_SIMULATION*/


    tableInfoPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->accessTableInfoPtr[tableType]);

    seed = 5;
    /*tableInfoPtr = PRV_CPSS_DXCH_TABLE_GET(devNum,tableType);*/
    cpssOsSrand(seed);


    for(entryNum = 0; entryNum < tableInfoPtr->maxNumOfEntries; entryNum++)
    {
        for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
        {
            wordValue[wordNum] = cpssOsRand() & getMaskBits(tableType);
        }

        rc = coreCheetahWriteTableEntry(devNum, tableType, entryNum, wordValue);
        if(rc != GT_OK)
            return rc;

    }

    cpssOsSrand(seed);

    for(entryNum = 0; entryNum < tableInfoPtr->maxNumOfEntries; entryNum++)
    {
#ifdef TBL_DEBUG_SIMULATION
        switch(tableType)
        {
            case PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E:
                for (i = 0; i < tableInfoPtr->entrySize; i++)
                {
                  regAddress = SMEM_CHT_PROT_VLAN_QOS_TBL_ADDR(i, entryNum);
                  rc = prvCpssHwPpReadRegister(devNum, regAddress,
                                                      &directReadValue[i]);
                }
                break;

            case PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E:
                regAddress = SMEM_CHT_VLAN_QOS_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E:

                trunkId = (entryNum >> 3) & 0x7f;
                trunkMemberIdx = entryNum &  0x7;
                regAddress = SMEM_CHT_TRUNK_TBL_ADDR(trunkMemberIdx, trunkId);
                break;

            case PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E:
                regAddress = SMEM_CHT_STAT_RATE_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_CPU_CODE_E:
                regAddress = SMEM_CHT_CPU_CODE_TBL_ADDR(entryNum);

                break;
            case PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E:
                regAddress = SMEM_CHT_PCL_ID_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_QOS_PROFILE_E:
                regAddress = SMEM_CHT_QOS_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_REMARKING_E:
                regAddress = SMEM_CHT_POLICER_QOS_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_STG_E:
                regAddress = SMEM_CHT_STP_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_VLAN_E:
                regAddress = SMEM_CHT_VLAN_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_MULTICAST_E:
                regAddress = SMEM_CHT_MCST_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E:
                regAddress = SMEM_CHT_MAC_SA_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E:
                regAddress = SMEM_CHT_ARP_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_FDB_E:
                regAddress = SMEM_CHT_MAC_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_POLICER_E:
                regAddress = SMEM_CHT_POLICER_TBL_ADDR(entryNum);
                break;

            case PRV_CPSS_DXCH_TABLE_POLICER_COUNTERS_E:
                regAddress = SMEM_CHT_POLICER_CNT_TBL_ADDR(entryNum);
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        }

        if (tableType != PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E)
        {
            rc = prvCpssHwPpReadRam(devNum, regAddress,  tableInfoPtr->entrySize,
                                                               directReadValue);
            if(rc != GT_OK)
                return rc;

        }
#endif  /*TBL_DEBUG_SIMULATION*/

        rc = coreCheetahReadTableEntry(devNum,tableType, entryNum, readValue);
        if(rc != GT_OK)
            return rc;

        for(wordNum = 0; wordNum < tableInfoPtr->entrySize; wordNum++)
        {
#ifdef TBL_DEBUG_SIMULATION

            if(readValue[wordNum] != directReadValue[wordNum])
            {
                cpssOsPrintf("\n Direct: %d = wordNum",wordNum);
                cpssOsPrintf("\n Direct: %d = entrydNum",entryNum);

            }

#endif  /*TBL_DEBUG_SIMULATION*/

            wordValue[wordNum] = cpssOsRand() & getMaskBits(tableType);

            if(readValue[wordNum] != wordValue[wordNum])
            {
                cpssOsPrintf("\n Indirect %d = wordNum",wordNum);
                cpssOsPrintf("\n Indirect %d = entrydNum",entryNum);

                 CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

        }
    }
    readWriteFieldTest(devNum,tableType);
    return GT_OK;
}
#endif /*CHEETAH_TABLE_DEBUG*/


/*******************************************************************************
* dxChXcatInitTable
*
* DESCRIPTION:
*       init the xCat tables info
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static void dxChXcatInitTable
(
    void
)
{
    GT_U32 ii;

    /* start by copying the ch3 info */
    for(ii = 0 ; ii < cheetah3TableInfoSize; ii++)
    {
        dxChXcatTablesInfo[ii] = cheetah3TablesInfo[ii];
    }

    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_FDB_E].maxNumOfEntries = _16K;
    dxChXcatTablesInfo[PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E].maxNumOfEntries = 64;

    /* fill the table for the XCAT A1 devices -- regardless to the rev of current device */

    /* update VLAN Entry table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].entrySize =
        PRV_CPSS_DXCHXCAT_VLAN_ENTRY_WORDS_COUNT_CNS;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].readAccessType =
        PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VLAN_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].writeAccessType =
        PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VLAN_E];

    /* Update VRF ID Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_VRF_ID_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VRF_ID_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_VRF_ID_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VRF_ID_E];

    /* Update Port VLAN QoS table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E].entrySize = 3;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_PORT_VLAN_QOS_E];

    /* Update STG Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_STG_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_STG_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_STG_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_STG_E];

    /* Update Multicast Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_MULTICAST_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_MULTICAST_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_MULTICAST_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_MULTICAST_E];

    /* Update HA MAC SA table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_MAC_SA_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_ROUTE_HA_MAC_SA_E];

    /* Update HA ARP DA table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_ROUTE_HA_ARP_DA_E];

    /* Update Ingress VLAN Translation Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_INGRESS_VLAN_TRANSLATION_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_INGRESS_TRANSLATION_E];

    /* Update Egress VLAN Translation Table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_EGRESS_VLAN_TRANSLATION_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EGRESS_TRANSLATION_E];

    /* Update LTT and TT Action table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_LTT_TT_ACTION_E].entrySize =
        PRV_CPSS_XCAT_LTT_TT_ACTION_WORDS_COUNT_CNS;
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_LTT_TT_ACTION_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_LTT_TT_ACTION_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_LTT_TT_ACTION_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_LTT_TT_ACTION_E];

    /* Update Tunnel Start table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_TUNNEL_START_CFG_E];

    /* PCL tables */

    /* IPCL Configuration table */
    ii = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 4224;
    dxChXcatTablesInfo[ii].entrySize = 2;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_CFG_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_CFG_E];

    /* IPCL Lookup1 Configuration table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 4224;
    dxChXcatTablesInfo[ii].entrySize = 2;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_LOOKUP1_CFG_E];

    /* EPCL Configuration table */
    ii = PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 4160;
    dxChXcatTablesInfo[ii].entrySize = 1;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EPCL_CFG_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EPCL_CFG_E];

    /* IPCL User Defined Bytes Configuration table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 8;
    dxChXcatTablesInfo[ii].entrySize = 8;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_IPCL_UDB_CFG_E];

    /* update Port Protocol VID and QoS Configuration table */
    /* - table holds 12 protocol entries per port,
         if (line % 16 >= 12) then the line is reserved.
       - 64 ports are taking in to account.
       - every entry comprise data only for one protocol of specific port.
    */
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E].maxNumOfEntries = _1KB;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E].entrySize =
        PRV_CPSS_DXCHXCAT_PRTCL_BASED_VLAN_ENTRY_WORDS_COUNT_CNS;
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_VLAN_PORT_PROTOCOL_E];

    /* Logical Target Mapping Table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_LOGICAL_TARGET_MAPPING_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 512;
    dxChXcatTablesInfo[ii].entrySize = 1;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &xcatA1LogicalTargetMappingTblInfo;
    dxChXcatTablesInfo[ii].writeTablePtr =
        &xcatA1LogicalTargetMappingTblInfo;

     /* Egress Filter Vlan Mapping table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_EQ_VLAN_MAPPING_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 4096;
    dxChXcatTablesInfo[ii].entrySize = 1;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &xcatC0EgressFilterVlanMappingTblInfo;
    dxChXcatTablesInfo[ii].writeTablePtr =
        &xcatC0EgressFilterVlanMappingTblInfo;

    /* Egress Filter Vlan Member table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_EQ_LP_EGRESS_VLAN_MEMBER_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 2048;
    dxChXcatTablesInfo[ii].entrySize = 2;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &xcatC0EgressFilterVlanMemberTblInfo;
    dxChXcatTablesInfo[ii].writeTablePtr =
        &xcatC0EgressFilterVlanMemberTblInfo;

    /* L2MLL VIDX enable table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_MLL_L2MLL_VIDX_ENABLE_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 128;
    dxChXcatTablesInfo[ii].entrySize = 1;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &xcatC0L2MllVidxEnableTblInfo;
    dxChXcatTablesInfo[ii].writeTablePtr =
        &xcatC0L2MllVidxEnableTblInfo;

    /* L2MLL VIDX enable table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_MLL_L2MLL_POINTER_MAPPING_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 1024;
    dxChXcatTablesInfo[ii].entrySize = 1;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_INDIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &xcatC0L2MllPointerMapTblInfo;
    dxChXcatTablesInfo[ii].writeTablePtr =
        &xcatC0L2MllPointerMapTblInfo;

    /* Update MAC2ME table */
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_MAC2ME_E].readTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_MAC2ME_E];
    dxChXcatTablesInfo[PRV_CPSS_DXCH3_TABLE_MAC2ME_E].writeTablePtr =
        &directAccessXcatTableInfo[PRV_CPSS_XCAT_TABLE_MAC2ME_E];

    /* BCN Profiles Table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_BCN_PROFILES_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 8;
    dxChXcatTablesInfo[ii].entrySize = 4;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_BCN_PROFILE_E];

    /* Egress Policer Remarking Table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E;
    dxChXcatTablesInfo[ii].maxNumOfEntries = 80;
    dxChXcatTablesInfo[ii].entrySize = 2;
    dxChXcatTablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcatTablesInfo[ii].readTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E];
    dxChXcatTablesInfo[ii].writeTablePtr =
        &directAccessXcatTableInfo[PRV_DXCH_XCAT_TABLE_EGRESS_POLICER_REMARKING_E];
}

/*******************************************************************************
* dxChXcat2InitTable
*
* DESCRIPTION:
*       init the xCat2 tables info
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static void dxChXcat2InitTable
(
    void
)
{
    GT_U32 ii;

    dxChXcatInitTable();

    /* start by copying the xCat info */
    for(ii = 0 ; (ii < dxChXcatTableInfoSize); ii++)
    {
        dxChXcat2TablesInfo[ii] = dxChXcatTablesInfo[ii];
    }

    /* IPCL lookup1 Configuration table */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 4224;
    dxChXcat2TablesInfo[ii].entrySize = 2;
    dxChXcat2TablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].readTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E];
    dxChXcat2TablesInfo[ii].writeTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP1_CFG_E];

    /* mac to me registers (in TTI unit) */
    ii = PRV_CPSS_DXCH3_TABLE_MAC2ME_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 8;
    dxChXcat2TablesInfo[ii].entrySize = 4;
    dxChXcat2TablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].readTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_MAC2ME_E];
    dxChXcat2TablesInfo[ii].writeTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_MAC2ME_E];

    /* the XCAT2 have reduced/removed tables , but not needed for other future devices (Lion; Lion2; Bobcat2; Caelum; Bobcat3) */

    /* PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E xCat2 - 256 entries, xCat - 1024 */
    ii = PRV_CPSS_DXCH_TABLE_ROUTE_HA_ARP_DA_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 256;

    /*PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E not present*/
    ii = PRV_CPSS_DXCH2_TABLE_QOS_PROFILE_TO_ROUTE_BLOCK_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;

    /*PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E not present*/
    ii = PRV_CPSS_DXCH2_TABLE_ROUTE_ACCESS_MATRIX_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;

    /*PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E not present*/
    ii = PRV_CPSS_DXCH2_UC_MC_ROUTE_NEXT_HOP_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;

    /*PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E not present*/
    ii = PRV_CPSS_DXCH2_ROUTE_NEXT_HOP_AGE_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;


    /*PRV_CPSS_DXCH3_TABLE_VRF_ID_E not present*/
    ii = PRV_CPSS_DXCH3_TABLE_VRF_ID_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 0;

    /*PRV_CPSS_DXCH3_LTT_TT_ACTION_E 1024 entries*/
    ii = PRV_CPSS_DXCH3_LTT_TT_ACTION_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 1024;

    /*PRV_CPSS_DXCH_TABLE_MULTICAST_E 1024 entries*/
    ii = PRV_CPSS_DXCH_TABLE_MULTICAST_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 1024;

    /*PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E 12 entries*/
    ii = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 12;

    /* PCL unit - hash mask table (for CRC hashing for trunk) */
    ii = PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = (BIT_4 + PCL_NUM_FLOWS_CNS); /* num entries = 4 bit index + 12 'flows' */
    dxChXcat2TablesInfo[ii].entrySize = 3;

    /* use LionB info */
    dxChXcat2TablesInfo[ii].readAccessType  =
    dxChXcat2TablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].readTablePtr  =
    dxChXcat2TablesInfo[ii].writeTablePtr =
        &(directAccessLionBTableInfoExt[PRV_CPSS_LION_TABLE_TRUNK_HASH_MASK_CRC_E].directAccessInfo);

    /*******************************************/
    /* next tables are actually 'new' in xcat2 */
    /*******************************************/

    /* PCL tables */

    /* IPCL lookup01 Configuration table */
    ii = PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E;
    dxChXcat2TablesInfo[ii].maxNumOfEntries = 4224;
    dxChXcat2TablesInfo[ii].entrySize = 2;
    dxChXcat2TablesInfo[ii].readAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
    dxChXcat2TablesInfo[ii].readTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E];
    dxChXcat2TablesInfo[ii].writeTablePtr =
        &directAccessXCAT2TableInfo[PRV_DXCH_XCAT2_TABLE_IPCL_LOOKUP01_CFG_E];

}

/*******************************************************************************
* lionInitTable
*
* DESCRIPTION:
*       init the Lion tables info
* INPUTS:
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static void lionInitTable
(
    void
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC   *tableInfoPtr;/* pointer to current table info */
    GT_U32 ii;
    GT_U32  index;

    /* init the xcat tables */
    dxChXcatInitTable();

    /* for Lion B */
    /* start by copying the xcat A1 info */
    for(ii = 0 ; ii < dxChXcatTableInfoSize; ii++)
    {
        lionBTablesInfo[ii] = dxChXcatTablesInfo[ii];
    }

    /******************************************/
    /* override it with  Lion B specific info */
    /******************************************/
    for(ii = 0 ; ii < directAccessLionBTableInfoExtNumEntries ; ii++)
    {
        if( directAccessLionBTableInfoExt[ii].directAccessInfo.baseAddress ==
            SPECIAL_ADDRESS_NOT_MODIFIED_E)
        {
            /* no need to modify the entry (same as XCAT A1) */
            continue;
        }

        index = directAccessLionBTableInfoExt[ii].globalIndex;

        lionBTablesInfo[index].readTablePtr =
        lionBTablesInfo[index].writeTablePtr =
            &directAccessLionBTableInfoExt[ii].directAccessInfo;
    }


    /* lion has 32K FDB per port group */
    lionBTablesInfo[PRV_CPSS_DXCH_TABLE_FDB_E].maxNumOfEntries = _32K;

    /***************************/
    /* updated existing tables */
    /***************************/

    /* Update Multicast Table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_TABLE_MULTICAST_E];
    tableInfoPtr->entrySize = 3;/* 3 words */

    /* IPCL User Defined Bytes Configuration table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_UDB_CONFIG_E];
    tableInfoPtr->maxNumOfEntries = 12;/* up to UDE 4 */

    /* Tunnel Start and Arp table entry size*/
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH2_TABLE_TUNNEL_START_CONFIG_E];
    tableInfoPtr->entrySize = 7;

    /******************/
    /* set new tables */
    /******************/

    /* ingress VLAN */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_VLAN_INGRESS_E];
    tableInfoPtr->maxNumOfEntries = _4K;
    tableInfoPtr->entrySize = 5;

    /* egress VLAN */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E];
    tableInfoPtr->maxNumOfEntries = _4K;
    tableInfoPtr->entrySize = 9;

    /* ingress STG */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E];
    tableInfoPtr->maxNumOfEntries = 256;
    tableInfoPtr->entrySize = 4;

    /* egress STG */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E];
    tableInfoPtr->maxNumOfEntries = 256;
    tableInfoPtr->entrySize = 4;

    /* port isolation L2 */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L2_E];
    tableInfoPtr->maxNumOfEntries = _2K+128;
    tableInfoPtr->entrySize = 3;

    /* port isolation L3 */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_PORT_ISOLATION_L3_E];
    tableInfoPtr->maxNumOfEntries = _2K+128;
    tableInfoPtr->entrySize = 3;

    /* txq shaper per port per TC */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E];
    tableInfoPtr->maxNumOfEntries = 16;/* local ports num */
    tableInfoPtr->entrySize = 14;

    /* txq shaper per port */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E];
    tableInfoPtr->maxNumOfEntries = 16;/* local ports num */
    tableInfoPtr->entrySize = 2;


    /* txq sourceId members table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E];
    tableInfoPtr->maxNumOfEntries = 32;/* source Id groups  */
    tableInfoPtr->entrySize = 2;

    /* txq non-trunk members table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E];
    tableInfoPtr->maxNumOfEntries = 128;/* trunkId (include 0)  */
    tableInfoPtr->entrySize = 2;


    /* txq designated members table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E];
    tableInfoPtr->maxNumOfEntries = 64;/* num entries  */
    tableInfoPtr->entrySize = 2;


    /* TXQ - egress STC table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E];
    tableInfoPtr->maxNumOfEntries = 16;/* local ports number !!! --
                                          but we will emulate index as 'global port' */
    tableInfoPtr->entrySize = 3;

    /* VLAN UC Rpf mode table */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_ROUTER_VLAN_URPF_STC_E];
    tableInfoPtr->maxNumOfEntries = 256;/* num entries  */
    tableInfoPtr->entrySize = 1;

    /* PCL unit - hash mask table (for CRC hashing for trunk) */
    tableInfoPtr = &lionBTablesInfo[PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E];
    tableInfoPtr->maxNumOfEntries = (BIT_4 + PCL_NUM_FLOWS_CNS); /* num entries = 4 bit index + 12 'flows' */
    tableInfoPtr->entrySize = 3;

}

/*******************************************************************************
* lion2InitTable
*
* DESCRIPTION:
*       init the Lion tables info
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static void lion2InitTable
(
    void
)
{
    GT_U32  ii;
    GT_U32  index;

    /* init the Lion B tables */
    lionInitTable();
    /* init the new 'xcat2' tables */
    dxChXcat2InitTable();

    /* start by copying the LionB info */
    for(ii = 0 ; (ii < lionBTableInfoSize); ii++)
    {
        lion2TablesInfo[ii] = lionBTablesInfo[ii];
    }

    /***************************/
    /*set new tables like xcat2*/
    /***************************/
    /* IPCL lookup01 Configuration table */
    ii = PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E;
    lion2TablesInfo[ii] = dxChXcat2TablesInfo[ii];
    /* IPCL lookup1 Configuration table changed base address */
    ii = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
    lion2TablesInfo[ii] = dxChXcat2TablesInfo[ii];

    /******************************************/
    /* override it with  Lion2 specific info */
    /******************************************/
    for(ii = 0 ; ii < directAccessLion2TableInfoNumEntries ; ii++)
    {
        if((directAccessLion2TableInfo[ii].directAccessInfo.baseAddress ==
            SPECIAL_ADDRESS_NOT_MODIFIED_E) ||
            ( directAccessLion2TableInfo[ii].directAccessInfo.baseAddress ==
            SPECIAL_ADDRESS_NOT_EXISTS_E))
        {
            /* no need to modify the entry (same as Lion / xcat2) */
            continue;
        }

        index = directAccessLion2TableInfo[ii].globalIndex;

        lion2TablesInfo[index].readTablePtr =
        lion2TablesInfo[index].writeTablePtr =
            &directAccessLion2TableInfo[ii].directAccessInfo;
    }

    /* Updated tables */

    /* PRV_CPSS_DXCH_TABLE_FDB_E 64k entries*/
    ii = PRV_CPSS_DXCH_TABLE_FDB_E;
    lion2TablesInfo[ii].maxNumOfEntries = _64K;
}

/*******************************************************************************
* initTablesDb
*
* DESCRIPTION:
*       init the Bobcat2, Caelum, Bobcat3 tables info
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS initTablesDb
(
    IN GT_U8  devNum
)
{
    GT_U32  ii;
    GT_STATUS rc;
    PRV_CPSS_DXCH_PP_TABLE_FORMAT_INFO_STC * initTablesFormatPtr;
    PRV_CPSS_DXCH_TABLE_ENT  globalIndex;
    GT_U32                   localIndex;

    initTablesFormatPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->tableFormatInfo;

    /* TTI */
    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_TTI_DEFAULT_EPORT_E ,
        prvCpssDxChLion3TtiDefaultEportTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_TTI_PHYSICAL_PORT_ATTRIBUTE_E ,
        prvCpssDxChLion3TtiPhysicalPortTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_TTI_EPORT_ATTRIBUTES_E ,
        prvCpssDxChLion3TtiEportAttributesTableFieldsFormat);


    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_TTI_ACTION_E ,
        prvCpssDxChLion3TtiActionTableFieldsFormat);

    /* VLAN */
    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_VLAN_PORT_PROTOCOL_E ,
        prvCpssDxChLion3TtiDefaultPortProtocolEvlanAndQosConfigTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_BRIDGE_INGRESS_EVLAN_E ,
        prvCpssDxChLion3L2iIngressVlanTableFieldsFormat);


    /* L2 */
    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_BRIDGE_INGRESS_EPORT_E ,
        prvCpssDxChLion3L2iEPortTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_FDB_E ,
        prvCpssDxChLion3FdbFdbTableFieldsFormat);

    /* IP MLL */
    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_IP_MLL_E ,
        prvCpssDxChLion3IpMllTableFieldsFormat);

    /* L2 MLL */
    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_L2_MLL_E ,
        prvCpssDxChLion3L2MllTableFieldsFormat);

    /* EGF */
    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_EGF_QAG_EGRESS_EPORT_E ,
        prvCpssDxChLion3EgfQagEgressEPortTableFieldsFormat);

    if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EVLAN_E ,
            prvCpssDxChLion3EgfShtEgressEVlanTableFieldsFormat);
    }
    else
    {
        if(PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_GET_MAC(devNum,PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E))
        {
            BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
                PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EVLAN_E ,
                prvCpssDxChSip5_20EgfShtEgressEVlanTableFieldsFormat_512Ports_mode);
        }
        else
        {
            BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
                PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EVLAN_E ,
                prvCpssDxChSip5_20EgfShtEgressEVlanTableFieldsFormat);
        }
    }

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_VLAN_ATTRIBUTES_E ,
        prvCpssDxChLion3EgfShtEgressEVlanAttributesTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_EGF_SHT_EGRESS_EPORT_E ,
        prvCpssDxChLion3EgfShtEgressEPortTableFieldsFormat);

    /* HA */
    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E ,
        prvCpssDxChLion3HaEPort1TableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_2_E ,
        prvCpssDxChLion3HaEPort2TableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_HA_PHYSICAL_PORT_1_E ,
        prvCpssDxChLion3HaPhysicalPort1TableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_HA_PHYSICAL_PORT_2_E ,
        prvCpssDxChLion3HaPhysicalPort2TableFieldsFormat);

    /* TS  */
    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_TUNNEL_START_E ,
        prvCpssDxChLion3HaTunnelStartTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_HA_GENERIC_TS_PROFILE_E ,
        prvCpssDxChLion3HaGenTunnelStartProfileTableFieldsFormat);

    /* PCL */
    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_IPCL_ACTION_E ,
        prvCpssDxChLion3IpclActionTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_EPCL_ACTION_E ,
        prvCpssDxChLion3EpclActionTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_FDB_AU_MSG_E ,
        prvCpssDxChLion3FdbAuMsgTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_IPVX_ROUTER_NEXT_HOP_E ,
        prvCpssDxChLion3IpvxRouterNextHopTableFieldsFormat);


    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_ROUTER_EPORT_E ,
        prvCpssDxChLion3IpvxRouterEportTableFieldsFormat);

    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_PLR_METER_E ,
        prvCpssDxChLion3PlrMeteringTableFieldsFormat);

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        /* Override with BC2 B0 specific formats */
        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_TTI_PHYSICAL_PORT_ATTRIBUTE_E ,
            prvCpssDxChBobcat2B0TtiPhysicalPortTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_BRIDGE_INGRESS_EVLAN_E ,
            prvCpssDxChBobcat2B0L2iIngressVlanTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_EGF_QAG_EGRESS_EPORT_E ,
            prvCpssDxChBobcat2B0EgfQagEgressEPortTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E ,
            prvCpssDxChBobcat2B0HaEPort1TableFieldsFormat);


        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_2_E ,
            prvCpssDxChBobcat2B0HaEPort2TableFieldsFormat);

        /* NAT44 - only in B0 */
        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_NAT44_E ,
            prvCpssDxChLion3HaNat44TableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_TTI_ACTION_E ,
            prvCpssDxChSip5_10TtiActionTableFieldsFormat);

    }

    if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E ,
            prvCpssDxChBobkHaEPort1TableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_ROUTER_EPORT_E ,
            prvCpssDxChBobKIpvxRouterEportTableFieldsFormat);

        /* new table and format (in sip5_15) */
        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_PLR_METER_CONFIG_E ,
            prvCpssDxChSip5_15PlrMeteringConfigurationTableFieldsFormat);

        /* new table and format (in sip5_15) */
        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr,
            PRV_CPSS_DXCH_TABLE_FORMAT_PLR_METER_CONFORM_SIGN_E,
            prvCpssDxChPlrMeteringConformanceSignTableFieldsFormat);

        /* NAT66 - only in BobK */
        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_NAT66_E ,
            prvCpssDxChLion3HaNat66TableFieldsFormat);
    }

    if(PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
    {
        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_HA_EGRESS_EPORT_1_E ,
            prvCpssDxChSip5_20HaEPort1TableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_EPCL_ACTION_E ,
            prvCpssDxChSip5_20EpclActionTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_EGF_QAG_PORT_TARGET_ATTRIBUTES_E ,
            prvCpssDxChSip5_20EgfQagPortTargetAttributesTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_TTI_PHYSICAL_PORT_ATTRIBUTE_E ,
            prvCpssDxChSip5_20TtiPhysicalPortTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_IPVX_ROUTER_NEXT_HOP_E ,
            prvCpssDxChSip5_20IpvxRouterNextHopTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_IP_MLL_E ,
            prvCpssDxChSip5_20IpMllTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_L2_MLL_E ,
            prvCpssDxChSip5_20L2MllTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_PLR_METER_CONFIG_E ,
            prvCpssDxChSip5_20PlrMeteringConfigurationTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_FDB_E ,
            prvCpssDxChSip5_20FdbFdbTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_FDB_AU_MSG_E ,
            prvCpssDxChSip5_20FdbAuMsgTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_IPCL_ACTION_E ,
            prvCpssDxChSip5_20IpclActionTableFieldsFormat);

        BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
            PRV_CPSS_DXCH_TABLE_FORMAT_TTI_ACTION_E ,
            prvCpssDxChSip5_20TtiActionTableFieldsFormat);


    BIND_FIELDS_FORMAT_TO_TABLE_MAC(initTablesFormatPtr ,
        PRV_CPSS_DXCH_TABLE_FORMAT_BRIDGE_INGRESS_EPORT_E ,
        prvCpssDxChSip5_20L2iEPortTableFieldsFormat);
    }


    for(ii = 0; ii < PRV_CPSS_DXCH_TABLE_LAST_FORMAT_E; ii++)
    {
        if(initTablesFormatPtr[ii].fieldsInfoPtr)
        {
            /* init tables */
            rc = prvCpssFillFieldsStartBitInfo(initTablesFormatPtr[ii].numFields,
                                               initTablesFormatPtr[ii].fieldsInfoPtr);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        /* override the direct access info with B0 specifics */
        for(ii = 0;
            ii < NUM_ELEMENTS_IN_ARR_MAC(directAccessBobcat2B0TableInfo_overrideA0);
            ii++)
        {
            globalIndex = directAccessBobcat2B0TableInfo_overrideA0[ii].globalIndex;

            rc = directAccessGlobalIndexToLocalIndexConvert(globalIndex,
                directAccessBobcat2TableInfo,
                NUM_ELEMENTS_IN_ARR_MAC(directAccessBobcat2TableInfo),
                &localIndex);/* index in directAccessBobcat2TableInfo */
            if(rc != GT_OK)
            {
                return rc;
            }

            /* override the direct access info with B0 specifics */
            directAccessBobcat2TableInfo[localIndex].directAccessInfo =
                directAccessBobcat2B0TableInfo_overrideA0[ii].directAccessInfo;
        }

        /* override the tables info with B0 specifics (probably needed for entry width) */
        for(ii = 0;
            ii < NUM_ELEMENTS_IN_ARR_MAC(bobcat2B0TablesInfo_overrideA0);
            ii++)
        {
            globalIndex = bobcat2B0TablesInfo_overrideA0[ii].globalIndex;

            /* the table MUST be 'globally' indexed !!!! --> no convert needed */
            localIndex = globalIndex;

            if(localIndex >= bobcat2TableInfoSize)
            {
                /* error in the DB !!! (do not cause access violation in the array) */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            /* override the direct access info with B0 specifics */
            bobcat2TablesInfo[localIndex] =
                bobcat2B0TablesInfo_overrideA0[ii].tableInfo;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* bobcat2InitTable
*
* DESCRIPTION:
*       init the Bobcat2 tables info
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS bobcat2InitTable
(
     IN GT_U8 devNum
)
{
    GT_STATUS rc;

    rc = initTablesDb(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* For Bobcat2 A0:
        Table:  Queue Descriptor Limits - Dequeue
                Entry width - 14 bit
                Line Address Alignment: 8 bytes
        Therefore macro BITS_TO_WORDS_MAC is wrong */
    if(!PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        /* override the direct access info with 40 bits instead of 14 - to get 8 bytes alignment */
        directAccessBobcat2TableInfo[PRV_DXCH_BOBCAT2_TABLE_TXQ_QUEUE_TAIL_DROP_DQ_QUEUE_DESC_LIMITS_E].directAccessInfo.step = BITS_TO_BYTES_ALIGNMENT_MAC(40);
    }

    return GT_OK;
}

/*******************************************************************************
* bobkNewTablesInit
*
* DESCRIPTION:
*       init new tables
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS bobkNewTablesInit
(
     void
)
{
    GT_U32  ii;
    /* new tables */
    ii = PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_0_METERING_CONFIG_E;
    bobkTablesInfo[ii].maxNumOfEntries = BOBK_NUM_IPLR_METERS_MAC;
    bobkTablesInfo[ii].entrySize = BITS_TO_WORDS_MAC(BOBK_POLICER_METERING_CONFIG_BITS_SIZE_CNS);

    /* copy info from IPLR 0 , and update the difference */
    ii = PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_1_METERING_CONFIG_E;
    bobkTablesInfo[ii].maxNumOfEntries = BOBK_NUM_IPLR_METERS_MAC;
    bobkTablesInfo[ii].entrySize = BITS_TO_WORDS_MAC(BOBK_POLICER_METERING_CONFIG_BITS_SIZE_CNS);

    /* copy info from IPLR 1 , and update the difference */
    ii = PRV_CPSS_DXCH_BOBK_TABLE_EGRESS_POLICER_METERING_CONFIG_E;
    bobkTablesInfo[ii].maxNumOfEntries = BOBK_NUM_EPLR_METERS_MAC;
    bobkTablesInfo[ii].entrySize = BITS_TO_WORDS_MAC(BOBK_POLICER_METERING_CONFIG_BITS_SIZE_CNS);

    /* copy info from EPLR , and update the difference */
    ii = PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_0_METERING_CONFORM_SIGN_E;
    bobkTablesInfo[ii].maxNumOfEntries = BOBK_NUM_IPLR_METERS_MAC;
    bobkTablesInfo[ii].entrySize = BITS_TO_WORDS_MAC(BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS);

    /* copy info from IPLR 1 , and update the difference */
    ii = PRV_CPSS_DXCH_BOBK_TABLE_INGRESS_POLICER_1_METERING_CONFORM_SIGN_E;
    bobkTablesInfo[ii].maxNumOfEntries = BOBK_NUM_IPLR_METERS_MAC;
    bobkTablesInfo[ii].entrySize = BITS_TO_WORDS_MAC(BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS);

    /* copy info from EPLR 0 , and update the difference */
    ii = PRV_CPSS_DXCH_BOBK_TABLE_EGRESS_POLICER_METERING_CONFORM_SIGN_E;
    bobkTablesInfo[ii].maxNumOfEntries = BOBK_NUM_EPLR_METERS_MAC;
    bobkTablesInfo[ii].entrySize = BITS_TO_WORDS_MAC(BOBK_POLICER_METERING_CONFORM_SIGN_BITS_SIZE_CNS);

    return GT_OK;
}
/*******************************************************************************
* bobkInitTableSpecifics
*
* DESCRIPTION:
*       update the Bobk specific tables info
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS bobkInitTableSpecifics
(
     GT_VOID
)
{
    PRV_CPSS_DXCH_TABLE_ENT  globalIndex;
    GT_U32                   localIndex;
    GT_U32  ii,jj;
    PRV_CPSS_DXCH_TABLES_INFO_STC *tblInfoPtr;
    PRV_CPSS_DXCH_TABLES_INFO_STC *tblInfoOverridePtr;
    PRV_CPSS_DXCH_TABLES_INFO_STC   *tableInfoPtr;/* pointer to current table info */
    GT_U32  lastIndexIn_directAccessBobcat2TableInfo =
        NUM_ELEMENTS_IN_ARR_MAC(directAccessBobcat2TableInfo) - 1;
    GT_U32  offsetIn_directAccessBobcat2TableInfo;
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC  *directAccessPtr;
    GT_U32 baseAddr;/* table base address */

    if(directAccessBobkTableInfo_initDone == GT_TRUE)
    {
        /* all operations are 'single time' on the filling the common bobk info
           and not 'per device' */
        return GT_OK;
    }

    directAccessBobkTableInfo_initDone = GT_TRUE;

    /* init the tables info with bc2 B0 specifics */
    for(ii = 0;
        ii < NUM_ELEMENTS_IN_ARR_MAC(bobcat2TablesInfo);
        ii++)
    {
        /* copy all elements from bobcat2TablesInfo */
        bobkTablesInfo[ii] = bobcat2TablesInfo[ii];
    }


    /* init the direct access info with bc2 B0 specifics */
    for(ii = 0;
        ii < NUM_ELEMENTS_IN_ARR_MAC(directAccessBobcat2TableInfo);
        ii++)
    {
        /* copy all elements from directAccessBobcat2TableInfo */
        directAccessBobkTableInfo[ii] = directAccessBobcat2TableInfo[ii];
    }


    /* add new entries that not exists in bobcat2 */
    /* bind 'direct table info' to the 'table info' */
    for(ii = 0;
        ii < NUM_ELEMENTS_IN_ARR_MAC(directAccessBobkTableInfo_new);
        ii++)
    {
        localIndex = directAccessBobkTableInfo_new[ii].newIndex;

        directAccessBobkTableInfo[localIndex] =
            directAccessBobkTableInfo_new[ii].directAccessInfo;

        /* bind 'direct table info' to the 'table info' */
        globalIndex = directAccessBobkTableInfo[localIndex].globalIndex;

        bobkTablesInfo[globalIndex].readAccessType =
        bobkTablesInfo[globalIndex].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
        bobkTablesInfo[globalIndex].readTablePtr =
        bobkTablesInfo[globalIndex].writeTablePtr = &directAccessBobkTableInfo[localIndex].directAccessInfo;
    }

    /* override all indications of directAccessBobcat2TableInfo with
       directAccessBobkTableInfo */
    for(ii = 0; ii < bobkTableInfoSize; ii++)
    {
        tableInfoPtr = &bobkTablesInfo[ii];

        if(tableInfoPtr->readAccessType == PRV_CPSS_DXCH_DIRECT_ACCESS_E &&
           tableInfoPtr->readTablePtr != NULL)
        {
            /* calculate the offset of readTablePtr in directAccessBobcat2TableInfo
               and replace it with offset in directAccessBobkTableInfo */
            directAccessPtr = tableInfoPtr->readTablePtr;

            /* handle the 'read' part */
            if(directAccessPtr >= &directAccessBobcat2TableInfo[0] &&
               directAccessPtr <= &directAccessBobcat2TableInfo[lastIndexIn_directAccessBobcat2TableInfo])
            {
                offsetIn_directAccessBobcat2TableInfo =
                    (GT_U32)((GT_CHAR*)directAccessPtr - (GT_CHAR*)directAccessBobcat2TableInfo);
                offsetIn_directAccessBobcat2TableInfo /= sizeof(PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC);

                tableInfoPtr->readTablePtr = &directAccessBobkTableInfo[offsetIn_directAccessBobcat2TableInfo].directAccessInfo;
            }
        }

        if(tableInfoPtr->writeAccessType == PRV_CPSS_DXCH_DIRECT_ACCESS_E &&
           tableInfoPtr->writeTablePtr != NULL)
        {
            /* calculate the offset of writeTablePtr in directAccessBobcat2TableInfo
               and replace it with offset in directAccessBobkTableInfo */
            directAccessPtr = tableInfoPtr->writeTablePtr;

            /* handle the 'write' part */
            if(directAccessPtr >= &directAccessBobcat2TableInfo[0] &&
               directAccessPtr <= &directAccessBobcat2TableInfo[lastIndexIn_directAccessBobcat2TableInfo])
            {
                offsetIn_directAccessBobcat2TableInfo =
                    (GT_U32)((GT_CHAR*)directAccessPtr - (GT_CHAR*)directAccessBobcat2TableInfo);
                offsetIn_directAccessBobcat2TableInfo /= sizeof(PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC);

                tableInfoPtr->writeTablePtr = &directAccessBobkTableInfo[offsetIn_directAccessBobcat2TableInfo].directAccessInfo;
            }
        }
    }


    /* override the direct access info with bobk specifics */
    for(ii = 0;
        ii < NUM_ELEMENTS_IN_ARR_MAC(bobkTablesInfo_overrideBobcat2);
        ii++)
    {
        globalIndex = bobkTablesInfo_overrideBobcat2[ii].globalIndex;

        localIndex = globalIndex;

        /* override the direct access info with bobk specifics */
        tblInfoPtr = &bobkTablesInfo[localIndex];
        tblInfoOverridePtr = &bobkTablesInfo_overrideBobcat2[ii].tableInfo;

        OVERRIDE_DIRECT_INFO_IF_MODIFIED_MAC(tblInfoOverridePtr,tblInfoPtr,maxNumOfEntries);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_MAC(tblInfoOverridePtr,tblInfoPtr,entrySize);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_MAC(tblInfoOverridePtr,tblInfoPtr,readAccessType);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_POINTER_MAC(tblInfoOverridePtr,tblInfoPtr,readTablePtr);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_MAC(tblInfoOverridePtr,tblInfoPtr,writeAccessType);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_POINTER_MAC(tblInfoOverridePtr,tblInfoPtr,writeTablePtr);
    }

    for(ii = 0;
        ii < NUM_ELEMENTS_IN_ARR_MAC(directAccessBobkTableInfo_overrideBobcat2);
        ii++)
    {
        globalIndex = directAccessBobkTableInfo_overrideBobcat2[ii].globalIndex;

        /* look for this global index in directAccessBobkTableInfo[] */
        for(jj = 0 ;
            jj < NUM_ELEMENTS_IN_ARR_MAC(directAccessBobkTableInfo);
            jj++)
        {
            if(directAccessBobkTableInfo[jj].globalIndex == globalIndex)
            {
                if (NOT_MODIFIED_VALUE_CNS ==
                    directAccessBobkTableInfo_overrideBobcat2[ii].directAccessInfo.baseAddress)
                {
                    /* keep the original base address */
                    baseAddr = directAccessBobkTableInfo[jj].directAccessInfo.baseAddress;
                }
                else
                {
                    /* got new base address than this of bc2 */
                    baseAddr = directAccessBobkTableInfo_overrideBobcat2[ii].directAccessInfo.baseAddress;
                }

                directAccessBobkTableInfo[jj].directAccessInfo =
                    directAccessBobkTableInfo_overrideBobcat2[ii].directAccessInfo;
                /* set proper base address */
                directAccessBobkTableInfo[jj].directAccessInfo.baseAddress = baseAddr;

                break;
            }
        }
    }

    return bobkNewTablesInit();
}
/*******************************************************************************
* bobkInitTable
*
* DESCRIPTION:
*       init the Bobk tables info
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS bobkInitTable
(
     IN GT_U8 devNum
)
{
    GT_STATUS rc;

    /* most tables are like in bobcat2 ... */
    rc = bobcat2InitTable(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* do specific bobk adaptations */
    rc = bobkInitTableSpecifics();/* not 'per device' */

    return rc;
}

/*******************************************************************************
* genericInitTableSpecifics
*
* DESCRIPTION:
*       generic update of specific tables info , to fill - devTablesInfo , directAccessTableInfo
*
* INPUTS:
*       devNum  - device number
*       prevDevSpecialIndication - special devNum indication for understand the offset
*               of table from the start of the unit ... for 'large' units.
*       isInitDone - indication that init already done
*       numEntriesInfo - (pointer to) hold number of entries for parametric tables.
*
* -- general info --
*       numElements_prevDevTablesInfo - number of elements in prevDevTablesInfo
*       prevDevTablesInfo - (pointer to) previous device to copy 'general' info from.
*
*       numElements_devTablesInfo_override - number of elements in devTablesInfo_override
*       devTablesInfo_override - (pointer to) 'general' info for override info copied from previous device.
*
* -- direct access --
*       numElements_prevDevDirectAccessTableInfo - number of elements in prevDevDirectAccessTableInfo
*       prevDevDirectAccessTableInfo - (pointer to) previous device to copy 'direct access' info from.
*
*       numElements_directAccessTableInfo_override - number of elements in directAccessTableInfo_override
*       directAccessTableInfo_override - (pointer to) 'direct access' info for override info copied from previous device.
*
*       numElements_directAccessTableInfo_new - number of elements in directAccessTableInfo_new
*       directAccessTableInfo_new - (pointer to) 'direct access' info for new info that not exists in previous device.
*
* -- indirect access --
*       numElements_bindIndirectToUnit - number of elements in bindIndirectToUnit
*       bindIndirectToUnit - (pointer to) 'indirect access' connection to unit.
*
*  ---
*       numElements_devTablesInfo - number of elements in devTablesInfo
*       numElements_directAccessTableInfo - number of elements in directAccessTableInfo
*
* OUTPUTS:
*       devTablesInfo - (pointer to) 'general' info for the device.
*       directAccessTableInfo - (pointer to) 'direct access' info for the device.
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS genericInitTableSpecifics
(
    IN GT_U8 devNum,
    IN GT_U8 prevDevSpecialIndication,
    IN GT_BOOL isInitDone,

    IN TABLE_NUM_ENTRIES_INFO_STC   numEntriesInfo[],

    IN GT_U32 numElements_prevDevTablesInfo,
    IN PRV_CPSS_DXCH_TABLES_INFO_STC prevDevTablesInfo[],

    IN GT_U32 numElements_devTablesInfo_override,
    IN PRV_CPSS_DXCH_TABLES_INFO_EXT_STC        devTablesInfo_override[],

    IN GT_U32 numElements_prevDevDirectAccessTableInfo,
    IN PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC prevDevDirectAccessTableInfo[],

    IN GT_U32 numElements_directAccessTableInfo_override,
    IN PRV_CPSS_DXCH_TABLES_INFO_DIRECT_EXT_STC directAccessTableInfo_override[],

    IN GT_U32 numElements_directAccessTableInfo_new,
    IN BOBK_TABLES_INFO_DIRECT_NEW_INDEX_STC directAccessTableInfo_new[],

    IN GT_U32 numElements_bindIndirectToUnit,
    IN INDIRECT_EXT_INFO_STC bindIndirectToUnit[],


    IN GT_U32 numElements_devTablesInfo,
    OUT PRV_CPSS_DXCH_TABLES_INFO_STC devTablesInfo[],

    IN GT_U32 numElements_directAccessTableInfo,
    OUT PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC directAccessTableInfo[]
)
{
    PRV_CPSS_DXCH_TABLE_ENT  globalIndex;
    GT_U32                   localIndex;
    GT_U32  ii,jj;
    PRV_CPSS_DXCH_TABLES_INFO_STC *tblInfoPtr;
    PRV_CPSS_DXCH_TABLES_INFO_STC *tblInfoOverridePtr;
    PRV_CPSS_DXCH_TABLES_INFO_STC   *tableInfoPtr;/* pointer to current table info */
    GT_U32  lastIndexIn_directAccessTableInfo = numElements_prevDevDirectAccessTableInfo;
    GT_U32  offsetIn_directAccessTableInfo;
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC  *directAccessPtr;
    GT_U32 numOfEntries;/* number of entries in a table */
    PRV_CPSS_DXCH_UNIT_ENT unitIndex;/* unit index */
    GT_U32  unitBaseAddr;/* base address of the unit in the device */
    GT_U32  unitBaseAddr_prevDev;/* base address of the unit in the 'prev device' */
    GT_BOOL     didError;/* indication for error */
    GT_BOOL     found;/* found indication */

    if(isInitDone == GT_TRUE)
    {
        /* not needed more than once . */
        return GT_OK;
    }

    /* init the tables info with previous specifics */
    for(ii = 0;
        ii < numElements_prevDevTablesInfo;
        ii++)
    {
        /* copy all elements from prevDevTablesInfo */
        devTablesInfo[ii] = prevDevTablesInfo[ii];
    }


    /* init the direct access info with device specifics */
    for(ii = 0;
        ii < numElements_prevDevDirectAccessTableInfo;
        ii++)
    {
        /* copy all elements from directAccessTableInfo */
        directAccessTableInfo[ii] = prevDevDirectAccessTableInfo[ii];
    }

    /* add new entries that not exists in previous */
    /* bind 'direct table info' to the 'table info' */
    for(ii = 0;
        ii < numElements_directAccessTableInfo_new;
        ii++)
    {
        localIndex = directAccessTableInfo_new[ii].newIndex;

        directAccessTableInfo[localIndex] =
            directAccessTableInfo_new[ii].directAccessInfo;

        /* bind 'direct table info' to the 'table info' */
        globalIndex = directAccessTableInfo[localIndex].globalIndex;

        devTablesInfo[globalIndex].readAccessType =
        devTablesInfo[globalIndex].writeAccessType = PRV_CPSS_DXCH_DIRECT_ACCESS_E;
        devTablesInfo[globalIndex].readTablePtr =
        devTablesInfo[globalIndex].writeTablePtr = &directAccessTableInfo[localIndex].directAccessInfo;


        if (NOT_MODIFIED_VALUE_CNS ==
            directAccessTableInfo[localIndex].directAccessInfo.nextWordOffset)
        {
            directAccessTableInfo[localIndex].directAccessInfo.nextWordOffset = PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS;
        }

        if(directAccessTableInfo[localIndex].directAccessInfo.step == 0xFFFFFFFF)
        {
            /* default value */
            directAccessTableInfo[localIndex].directAccessInfo.step = 4;
        }

        /* calculate entry size from 'alignment' of the table <step> */
        devTablesInfo[globalIndex].entrySize =
            directAccessTableInfo[localIndex].directAccessInfo.step / 4;
    }

    /* override all indications of prevDevDirectAccessTableInfo with
        directAccessTableInfo */
    for(ii = 0; ii <  numElements_devTablesInfo; ii++)
    {
        tableInfoPtr = &devTablesInfo[ii];

        if(tableInfoPtr->readAccessType == PRV_CPSS_DXCH_DIRECT_ACCESS_E &&
           tableInfoPtr->readTablePtr != NULL)
        {
            /* calculate the offset of readTablePtr in prevDevDirectAccessTableInfo
               and replace it with offset in directAccessTableInfo */
            directAccessPtr = tableInfoPtr->readTablePtr;

            /* handle the 'read' part */
            if(directAccessPtr >= &prevDevDirectAccessTableInfo[0] &&
               directAccessPtr < &prevDevDirectAccessTableInfo[lastIndexIn_directAccessTableInfo])
            {
                offsetIn_directAccessTableInfo =
                    (GT_U32)((GT_CHAR*)directAccessPtr - (GT_CHAR*)prevDevDirectAccessTableInfo);
                offsetIn_directAccessTableInfo /= sizeof(PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC);

                tableInfoPtr->readTablePtr = &directAccessTableInfo[offsetIn_directAccessTableInfo].directAccessInfo;
            }
        }

        if(tableInfoPtr->writeAccessType == PRV_CPSS_DXCH_DIRECT_ACCESS_E &&
           tableInfoPtr->writeTablePtr != NULL)
        {
            /* calculate the offset of writeTablePtr in prevDevDirectAccessTableInfo
               and replace it with offset in directAccessTableInfo */
            directAccessPtr = tableInfoPtr->writeTablePtr;

            /* handle the 'write' part */
            if(directAccessPtr >= &prevDevDirectAccessTableInfo[0] &&
               directAccessPtr < &prevDevDirectAccessTableInfo[lastIndexIn_directAccessTableInfo])
            {
                offsetIn_directAccessTableInfo =
                    (GT_U32)((GT_CHAR*)directAccessPtr - (GT_CHAR*)prevDevDirectAccessTableInfo);
                offsetIn_directAccessTableInfo /= sizeof(PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC);

                tableInfoPtr->writeTablePtr = &directAccessTableInfo[offsetIn_directAccessTableInfo].directAccessInfo;
            }
        }
    }


    /* override the direct access info with bobcat3 specifics */
    for(ii = 0;
        ii < numElements_devTablesInfo_override;
        ii++)
    {
        globalIndex = devTablesInfo_override[ii].globalIndex;

        localIndex = globalIndex;

        /* override the direct access info with device specifics */
        tblInfoPtr = &devTablesInfo[localIndex];
        tblInfoOverridePtr = &devTablesInfo_override[ii].tableInfo;

        OVERRIDE_DIRECT_INFO_IF_MODIFIED_MAC(tblInfoOverridePtr,tblInfoPtr,maxNumOfEntries);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_MAC(tblInfoOverridePtr,tblInfoPtr,entrySize);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_MAC(tblInfoOverridePtr,tblInfoPtr,readAccessType);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_POINTER_MAC(tblInfoOverridePtr,tblInfoPtr,readTablePtr);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_MAC(tblInfoOverridePtr,tblInfoPtr,writeAccessType);
        OVERRIDE_DIRECT_INFO_IF_MODIFIED_POINTER_MAC(tblInfoOverridePtr,tblInfoPtr,writeTablePtr);

        if(tblInfoOverridePtr->entrySize != NOT_MODIFIED_VALUE_CNS)
        {
           if(tblInfoPtr->writeAccessType == PRV_CPSS_DXCH_DIRECT_ACCESS_E)
            {
                /* auto calculate the info for alignment */
                ((PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)
                    tblInfoPtr->writeTablePtr)->step =
                        BITS_TO_BYTES_ALIGNMENT_MAC(tblInfoOverridePtr->entrySize * 32);
            }
            else
            if(tblInfoPtr->readAccessType == PRV_CPSS_DXCH_DIRECT_ACCESS_E)
            {
                /* auto calculate the info for alignment */
                ((PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)
                    tblInfoPtr->readTablePtr)->step =
                        BITS_TO_BYTES_ALIGNMENT_MAC(tblInfoOverridePtr->entrySize * 32);
            }
        }

    }


    /* update the base address of the indirect tables */
    for(ii = 0;
        ii < numElements_bindIndirectToUnit;
        ii++)
    {
        PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *indirectTablePtr;

        /* index in indirectAccessTableInfo[]  */
        globalIndex = bindIndirectToUnit[ii].indirectIndex;
        unitIndex = bindIndirectToUnit[ii].unitIndex;

        /* update the base address in indirectTablePtr */
        unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,unitIndex,&didError);

        indirectTablePtr = &indirectAccessTableInfo[globalIndex];
        UPDATE_ADDR_WITH_NEW_UNIT_MAC(indirectTablePtr->controlReg,
            unitBaseAddr);
        UPDATE_ADDR_WITH_NEW_UNIT_MAC(indirectTablePtr->dataReg,
            unitBaseAddr);

    }


    for(ii = 0;                /*xxx*/
        ii < numElements_directAccessTableInfo_override;
        ii++)
    {
        globalIndex = directAccessTableInfo_override[ii].globalIndex;

        /* look for this global index in directAccessTableInfo[] */
        for(jj = 0 ;
            jj < numElements_directAccessTableInfo;
            jj++)
        {
            if(directAccessTableInfo[jj].globalIndex != globalIndex)
            {
                continue;
            }

            if (NOT_MODIFIED_VALUE_CNS !=
                directAccessTableInfo_override[ii].directAccessInfo.baseAddress)
            {
                /* got new base address than this of prev device */
                directAccessTableInfo[jj].directAccessInfo.baseAddress =
                    directAccessTableInfo_override[ii].directAccessInfo.baseAddress;
            }

            if (NOT_MODIFIED_VALUE_CNS !=
                directAccessTableInfo_override[ii].directAccessInfo.step)
            {
                /* got new step than this of prev device */
                directAccessTableInfo[jj].directAccessInfo.step =
                    directAccessTableInfo_override[ii].directAccessInfo.step;
            }

            if (NOT_MODIFIED_VALUE_CNS !=
                directAccessTableInfo_override[ii].directAccessInfo.nextWordOffset)
            {
                /* got new nextWordOffset than this of prev device */
                directAccessTableInfo[jj].directAccessInfo.nextWordOffset =
                    directAccessTableInfo_override[ii].directAccessInfo.nextWordOffset;
            }

            break;
        }
    }

    for(ii = 0;
        numEntriesInfo[ii].tableType != PRV_CPSS_DXCH_TABLE_LAST_E;
        ii++)
    {
        globalIndex = numEntriesInfo[ii].tableType;

        switch(numEntriesInfo[ii].entryType)
        {
            case ENTRY_TYPE_NOT_VALID_E:
                numOfEntries = 0;
                break;
            case ENTRY_TYPE_EPORT_E:
                /* from 'flexFieldNumBitsSupport' */
                numOfEntries = 1 + PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum);
                break;
            case ENTRY_TYPE_PHY_PORT_E:
                /* from 'flexFieldNumBitsSupport' */
                numOfEntries = 1 + PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(devNum);
                break;
            case ENTRY_TYPE_EVLAN_E:
                /* from 'flexFieldNumBitsSupport' */
                numOfEntries = 1 + PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum);
                break;
            case ENTRY_TYPE_EVIDX_E:
                /* from 'flexFieldNumBitsSupport' */
                numOfEntries = 1 + PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VIDX_MAC(devNum);
                break;
            case ENTRY_TYPE_STG_E:
                /* from 'flexFieldNumBitsSupport' */
                numOfEntries = 1 + PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_STG_ID_MAC(devNum);
                break;
            case ENTRY_TYPE_L2_ECMP_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesL2Ecmp;
                break;
            case ENTRY_TYPE_L3_ECMP_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesL3Ecmp;
                break;
            case ENTRY_TYPE_TUNNEL_START_E:
                numOfEntries = 1 + BIT_MASK_MAC(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).tunnelStartPtr);
                break;
            case ENTRY_TYPE_IPVX_NH_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesIpNh;
                break;
            case ENTRY_TYPE_MLL_LTT_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesMllLtt;
                break;
            case ENTRY_TYPE_MLL_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesMll;
                break;
            case ENTRY_TYPE_IPLR_METERING_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesIplrMetering;
                break;
            case ENTRY_TYPE_EPLR_METERING_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesEplrMetering;
                break;
            case ENTRY_TYPE_OAM_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesOam;
                break;
            case ENTRY_TYPE_TXQ_QUEUES_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesTxqQueue;
                break;
            case ENTRY_TYPE_LOC_E:
                /* 32 entries in line (each entry is single bit)*/
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.parametericTables.numEntriesProtectionLoc;
                break;
            case ENTRY_TYPE_EXPLICIT_NUM_E:
                /* the explicit value is in parameter <additionalFixedNumberOfEntries>*/
                numOfEntries = 0xFFFFFFFF;
                break;
            case ENTRY_TYPE_SOURCE_ID_E:
                /* from 'flexFieldNumBitsSupport' */
                numOfEntries = 1 + PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(devNum);
                break;
            case ENTRY_TYPE_TRUNK_E:
                /* from 'flexFieldNumBitsSupport' */
                numOfEntries = 1 + PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum);
                break;
            case ENTRY_TYPE_TXQ_DQ_PORTS_E:
                numOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.txqDqNumPortsPerDp;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
        }

        if(numOfEntries)/* check that the table is valid */
        {
            if(numOfEntries == 0xFFFFFFFF)
            {
                /*case of ENTRY_TYPE_EXPLICIT_NUM_E*/
                /* the explicit value is in parameter <additionalFixedNumberOfEntries>*/
                numOfEntries = 0;
            }
            else
            {
                if(numEntriesInfo[ii].divider & FRACTION_HALF_TABLE_INDICATION_CNS)
                {
                    /* each 'entry' spread on more than single LINE */
                    numOfEntries *= (numEntriesInfo[ii].divider - FRACTION_HALF_TABLE_INDICATION_CNS);
                }
                else
                if(numEntriesInfo[ii].divider & FRACTION_INDICATION_CNS)
                {
                    /* each 'entry' spread on more than single LINE */
                    numOfEntries *= (numEntriesInfo[ii].divider - FRACTION_INDICATION_CNS);
                }
                else
                if(numEntriesInfo[ii].divider > 1)
                {
                    /* each LINE hold more than single 'entry' */
                    numOfEntries /= numEntriesInfo[ii].divider & (~VERTICAL_INDICATION_CNS);
                }
            }

            if(numEntriesInfo[ii].additionalFixedNumberOfEntries)
            {
                numOfEntries += numEntriesInfo[ii].additionalFixedNumberOfEntries;
            }
            /* this is actually 'number of lines' in the table */
            devTablesInfo[globalIndex].maxNumOfEntries = numOfEntries;

        }
        else
        {
            /* clear the fields of this table as it is not valid */
            cpssOsMemSet(&devTablesInfo[globalIndex],
                        0,
                        sizeof(devTablesInfo[globalIndex]));
        }
    }

    /* set proper addresses according to the actual addresses of the units in the device */
    for(ii = 0 ;
        ii < numElements_directAccessTableInfo;
        ii++)
    {
        unitIndex = directAccessTableInfo[ii].unitIndex;
        unitBaseAddr = prvCpssDxChHwUnitBaseAddrGet(devNum,unitIndex,&didError);
        if(1 == (unitBaseAddr & 1))
        {
            /* the unit is not valid for the device .... allow it. */
            /* set the table with 'bad address'  ... that will cause fatal error
               on simulation */
            directAccessTableInfo[ii].directAccessInfo.baseAddress =
                PRV_CPSS_SW_PTR_ENTRY_UNUSED;
        }
        else
        {
            if(didError == GT_TRUE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            switch(unitIndex)
            {
                case PRV_CPSS_DXCH_UNIT_EGF_SHT_E:

                    found = GT_FALSE;
                    /* special treat for entries that exists in 'directAccessTableInfo_new' */
                    for(jj = 0; jj < numElements_directAccessTableInfo_new ; jj++)
                    {
                        if(directAccessTableInfo_new[jj].directAccessInfo.globalIndex ==
                           directAccessTableInfo[ii].globalIndex)
                        {
                            found = GT_TRUE;
                            break;
                        }
                    }

                    if(found == GT_FALSE)
                    {
                        /* special treat for entries that exists in 'directAccessTableInfo_override' */
                        for(jj = 0; jj < numElements_directAccessTableInfo_override ; jj++)
                        {
                            if(directAccessTableInfo_override[jj].globalIndex ==
                               directAccessTableInfo[ii].globalIndex)
                            {
                                found = GT_TRUE;
                                break;
                            }
                        }
                    }


                    if(found == GT_TRUE)
                    {
                        /* found in the 'directAccessTableInfo_new/directAccessTableInfo_override' ..
                           so do not use 'prev device' info   */
                        /* ASSUME that the address is 0 based in 'directAccessTableInfo_new/directAccessTableInfo_override' */
                        directAccessTableInfo[ii].directAccessInfo.baseAddress += unitBaseAddr;
                    }
                    else
                    {
                        /* this is large unit , that takes more than single unit memory space.
                            need to calculate the offset from the start.

                            for that we need the base address of the unit in the 'prev device'
                        */
                        unitBaseAddr_prevDev =
                            prvCpssDxChHwUnitBaseAddrGet(prevDevSpecialIndication,unitIndex,NULL);

                        directAccessTableInfo[ii].directAccessInfo.baseAddress -= unitBaseAddr_prevDev;
                        directAccessTableInfo[ii].directAccessInfo.baseAddress += unitBaseAddr;
                    }

                    break;
                default:
                    UPDATE_ADDR_WITH_NEW_UNIT_MAC(directAccessTableInfo[ii].directAccessInfo.baseAddress,
                        unitBaseAddr);
                    break;
            }



        }
    }

    return GT_OK;
}
/*******************************************************************************
* bobcat3InitTable
*
* DESCRIPTION:
*       init the Bobcat3 tables info
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS bobcat3InitTable
(
     IN GT_U8 devNum
)
{
    GT_STATUS rc;

    /* most tables are like in bobk ... */
    rc = bobkInitTable(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* do specific bobcat3 adaptations */
    rc = genericInitTableSpecifics(
            devNum,
            /*  special devNum indication for understand the offset of table from
                the start of the unit ... for 'large' units.*/
            BOBK_DEVICE_INDICATION_CNS ,

            directAccessBobCat3TableInfo_initDone,

            /* info about the number of entries in the tables */
            sip5_20tableNumEntriesInfoArr,

            /* use bobk as 'base' for the info */
            NUM_ELEMENTS_IN_ARR_MAC(bobkTablesInfo),
            bobkTablesInfo,
            /* the tables used to override values of bobk that are not ok for bobcat3 -- general info */
            NUM_ELEMENTS_IN_ARR_MAC(bobcat3TablesInfo_overrideBobk),
            bobcat3TablesInfo_overrideBobk,

            /* use bobk as 'base' for the info */
            NUM_ELEMENTS_IN_ARR_MAC(directAccessBobkTableInfo),
            directAccessBobkTableInfo,

            /* the tables used to override values of bobk that are not ok for bobcat3 -- direct access info */
            NUM_ELEMENTS_IN_ARR_MAC(directAccessBobcat3TableInfo_overrideBobk),
            directAccessBobcat3TableInfo_overrideBobk,

            /* the new tables (not exist in bobk) for bobcat3 -- direct access info */
            NUM_ELEMENTS_IN_ARR_MAC(directAccessBobcat3TableInfo_new),
            directAccessBobcat3TableInfo_new,

            /* the indirect tables connection to units */
            NUM_ELEMENTS_IN_ARR_MAC(bindIndirectToUnit),
            bindIndirectToUnit,

            /* the table to fill for bc3 -- general info */
            NUM_ELEMENTS_IN_ARR_MAC(bobcat3TablesInfo),
            bobcat3TablesInfo,

            /* the table to fill for bc3 -- direct access info */
            NUM_ELEMENTS_IN_ARR_MAC(directAccessBobcat3TableInfo),
            directAccessBobcat3TableInfo);


    directAccessBobCat3TableInfo_initDone = GT_TRUE;

    return rc;
}




/*******************************************************************************
* indexAsPortNumConvert
*
* DESCRIPTION:
*       convert index that is portNum ('Global port num') to portGroupId and
*       'Local port num'
*       for tables that are not with index = portNum , no modification in the
*       index , and portGroupId will be 'first active port group == 0'
*       for non multi-port-groups device : portGroupId is 'all port groups' , no index conversion
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*
* OUTPUTS:
*       portGroupIdPtr - (pointer to) the portGroupId to use
*       updatedEntryIndexPtr - (pointer to) the updated index
*       updatedTableTypePtr - (pointer to) the NEW specific table name to use
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
static void indexAsPortNumConvert
(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    OUT GT_U32                 *portGroupIdPtr,
    OUT GT_U32                 *updatedEntryIndexPtr,
    OUT PRV_CPSS_DXCH_TABLE_ENT *updatedTableTypePtr
)
{
    GT_STATUS   rc;
    GT_U32  portNum; /* temp local port number */
    GT_U32  offset;  /* temp offset of port from the base port index */
    GT_U32  newIndex;/* temp index for the port calculation */
    PRV_CPSS_DXCH_TABLE_ENT newTableOffset;
    GT_U32  dmaPortNum;/* temp DMA port number */
    GT_U32  dpIndex ,pipeId;/* temp DP index , local DMA and Pipe number */

    /* converts that done also for 'non-multi port group' device */
    *updatedTableTypePtr = tableType;

    /*default is 'unaware'*/
    *portGroupIdPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiPipe.numOfPipes > 1)
    {
        switch(tableType)
        {
            case PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E:
            case PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E:
            case PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E:
                offset = entryIndex / PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.txqDqNumPortsPerDp;
                newIndex = entryIndex % PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.txqDqNumPortsPerDp;

                if(tableType == PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E)
                {
                    newTableOffset = PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E;
                }
                else
                if(tableType == PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E)
                {
                    newTableOffset = PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG____DQ_1___E;
                }
                else
                if(tableType == PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E)
                {
                    newTableOffset = PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG____DQ_1___E;
                }
                else
                {
                    newTableOffset = 0;
                    /* ERROR */
                    *updatedTableTypePtr = PRV_CPSS_DXCH_TABLE_LAST_E;/* will generate error in the caller */
                    newIndex = 0;
                    offset = 0;
                }

                if(offset != 0)
                {
                    *updatedTableTypePtr = newTableOffset + (offset-1);

                    if((offset-1) > (PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_LAST___E -
                                     PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_TXQ_EGRESS_STC____DQ_1___E))
                    {
                        /* ERROR */
                        *updatedTableTypePtr = PRV_CPSS_DXCH_TABLE_LAST_E;/* will generate error in the caller */
                        newIndex = 0;
                    }
                }

                entryIndex = newIndex;
                break;

            case PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E:
                /* we must read/write from/to single Pipe */

                portNum = entryIndex;

                /* we must convert the physical port to DMA Number and to find the 'pipeId' */
                /* for ports without mapping ... consider pipe 0 (should not happen with real application) */
                /* do CPSS_LOG_WARNING message  */
                rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portNum, PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E, &dmaPortNum);
                if(rc != GT_OK)
                {
                    /* keep index as as into the pipe '0' */
                    break;
                }

                rc = prvCpssDxChHwPpDmaGlobalNumToLocalNumInDpConvert(devNum,dmaPortNum,&dpIndex ,NULL);
                if(rc != GT_OK)
                {
                    break;
                }

                pipeId = (dpIndex * PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiPipe.numOfPipes) /
                                    PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.multiDataPath.maxDp;

                offset = pipeId;

                if(tableType == PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E)
                {
                    newTableOffset = PRV_CPSS_DXCH_BOBCAT3_MULTI_INSTANCE_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT___PIPE_1___E;
                }
                else
                {
                    newTableOffset = 0;
                    /* ERROR */
                    *updatedTableTypePtr = PRV_CPSS_DXCH_TABLE_LAST_E;/* will generate error in the caller */
                    newIndex = 0;
                    offset = 0;
                }

                if(offset != 0)
                {
                    *updatedTableTypePtr = newTableOffset + (offset-1);
                }

                break;
            default:
                break;
        }
    }

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0)
    {
        /* no conversion for non-multi port group device */
        *updatedEntryIndexPtr = entryIndex;

        return ;
    }

    switch(tableType)
    {
        case PRV_CPSS_DXCH_TABLE_VLAN_PORT_PROTOCOL_E:
            /* special case :
                for some devices this is a 'per port' table
                for other devices this is a '16 entries per port' table

                see calculations in function cpssDxChBrgVlanPortProtoVlanQosSet(...)
            */
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* the index supposed to be ((portNum * 16) + entryNum)  */
                portNum = entryIndex / 16;/*portNum*/
                offset = entryIndex % 16; /*entryNum*/

                if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
                {
                    /*no global port to local convert needed*/

                    /* the new index is the 'input index'*/
                    newIndex = entryIndex;
                }
                else
                {
                    *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portNum);
                    newIndex = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
                    /* build the new index from the local port and the offset */
                    newIndex = newIndex * 16 + offset;
                }

                *updatedEntryIndexPtr = newIndex;
            }
            else
            {
                /* the index supposed to be portNum */
                *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,entryIndex);
                *updatedEntryIndexPtr = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,entryIndex);
            }
            break;
        case PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E:
        case PRV_CPSS_DXCH_LION_TABLE_TXQ_EGRESS_STC_E:
            /* the index supposed to be portNum */
            *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,entryIndex);
            *updatedEntryIndexPtr = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,entryIndex);
            break;
        case PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E:
        case PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_PER_PRIO_TOKEN_BUCKET_CONFIG_E:
            *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,entryIndex);
            /* the index supposed to be localPortNum , but the CPU port must be converted to 0xF */
            *updatedEntryIndexPtr = entryIndex & 0xf;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_1_E:
            /* the index is 'Global port' and MUST not be converted to local port */
            /* in SIP5 we not need to convert to local port */
            /* and we must write to all port groups , because table can be accessed as 'SRC PORT' */
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_HA_PHYSICAL_PORT_2_E:
            /* the index is 'Global port' and MUST not be converted to local port */
            /* in SIP5 we not need to convert to local port */
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_TTI_PHYSICAL_PORT_ATTRIBUTE_E:
            /* the index is 'Global port' and MUST not be converted to local port */
            /* in SIP5 we not need to convert to local port */
            break;

        case PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_E:
        case PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_PHYSICAL_PORT_RATE_LIMIT_COUNTERS_E:
            /* the index is 'Global port' and MUST not be converted to local port */
            /* the index is 'Global port' and MUST not be converted to local port */
            /* in SIP5 we not need to convert to local port */
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_MIRROR_PHYSICAL_PORT_E:
            /* the index is 'Global port' and MUST not be converted to local port */
            /* in SIP5 we not need to convert to local port */
            *updatedEntryIndexPtr /= 8; /* 8 ports in entry */
            break;

        case PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_STC_PHYSICAL_PORT_E:
            /* the index is 'Global port' and MUST not be converted to local port */
            /* the index is 'Global port' and MUST not be converted to local port */
            /* in SIP5 we not need to convert to local port */
            break;

        default:
            /*no global port to local convert needed*/
            *updatedEntryIndexPtr = entryIndex;
            break;
    }

    return;
}

/*******************************************************************************
* writeTableEntry_splitOn2entries
*
* DESCRIPTION:
*       function to write logical entry that is split to 2 lines in 2 parts of
*       the table.(offset of 1/2 table between them)
*       NOTE: function must be called only when :
*           0 == PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_GET_MAC(devNum,tableType)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table (of the first half of entry)
*       entryValuePtr - (pointer to) the data that will be written to the table
*                       may be NULL in the case of indirect table.
*       halfTableIndexOffset - offset of the second part of the entry from the 'entryIndex'
*       halfEntryBitOffset   - offset of the second part of the data (bit index) in entryValuePtr[]
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS writeTableEntry_splitOn2entries
(
    IN GT_U8                    devNum,
    IN GT_U32                   portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    IN GT_U32                  *entryValuePtr,
    IN GT_U32                   halfTableIndexOffset,
    IN GT_U32                   halfEntryBitOffset
)
{
    GT_STATUS   rc;

    /*write first half of the entry*/
    rc = prvCpssDxChPortGroupWriteTableEntry(
        devNum,
        portGroupId,
        tableType,
        entryIndex,
        entryValuePtr
    );
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed to write table[%d] : first half of the entry index [%d]",
            tableType,entryIndex);
    }

    /*write second half of the entry*/
    rc = prvCpssDxChPortGroupWriteTableEntry(
        devNum,
        portGroupId,
        tableType,
        entryIndex + halfTableIndexOffset,
        &entryValuePtr[halfEntryBitOffset>>5]
    );
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed to write table[%d] : second half of the entry index [%d]",
            tableType,entryIndex);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntry_splitOn2entries
*
* DESCRIPTION:
*       function to write logical entry that is split to 2 lines in 2 parts of
*       the table.(offset of 1/2 table between them)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table (of the first half of entry)
*       entryValuePtr - (pointer to) the data that will be written to the table
*                       may be NULL in the case of indirect table.
*       halfTableIndexOffset - offset of the second part of the entry from the 'entryIndex'
*       halfEntryBitOffset   - offset of the second part of the data (bit index) in entryValuePtr[]
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortGroupWriteTableEntry_splitOn2entries
(
    IN GT_U8                    devNum,
    IN GT_U32                   portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    IN GT_U32                  *entryValuePtr,
    IN GT_U32                   halfTableIndexOffset,
    IN GT_U32                   halfEntryBitOffset
)
{
    GT_STATUS   rc;
    GT_U32      needToRestoreHalfTableMode;

    needToRestoreHalfTableMode = PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_GET_MAC(devNum,tableType);
    if(needToRestoreHalfTableMode)
    {
        /* remove the option to get 'FRACTION_HALF_TABLE_INDICATION_CNS' from function
            multiEntriesInLineIndexAndGlobalBitConvert() that is called from
            prvCpssDxChPortGroupWriteTableEntry(...) */
        PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_CLEAR_MAC(devNum,tableType);
    }

    rc = writeTableEntry_splitOn2entries(
        devNum,
        portGroupId,
        tableType,
        entryIndex,
        entryValuePtr,
        halfTableIndexOffset,
        halfEntryBitOffset
    );
    if(needToRestoreHalfTableMode)
    {
        /* restore value */
        PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_SET_MAC(devNum,tableType);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntry
*
* DESCRIPTION:
*       Write a whole entry to the table. - for specific portGroupId
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*       entryValuePtr - (pointer to) the data that will be written to the table
*                       may be NULL in the case of indirect table.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send write entry command to device. And entry is taken from the
*      data registers of a indirect table.
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupWriteTableEntry
(
    IN GT_U8                    devNum,
    IN GT_U32                   portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    IN GT_U32                  *entryValuePtr
)
{
    GT_U32  entryMemoBufArr[MAX_ENTRY_SIZE_CNS];
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32      address;  /* address to write the value */
    /* pointer to direct access table information */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *tablePtr;
    GT_U32      i;
    GT_U32      tmpReadBuff;
    GT_STATUS   status = GT_OK;
    GT_U32  tmpPortGroupId = portGroupId;/* temporary port group id */
    GT_U32  originPortGroupId = portGroupId;
    GT_U32  orig_entryIndex; /* orig entry index as calculate in this function */
    GT_U32  *orig_entryValuePtr;/* original pointer from the caller */
    GT_U32  startBit;/* start bit in the case of multi entries in single LINE */
    GT_U32  numBitsPerEntry;/*the number of bits that the 'entry' is using in the LINE.*/
    GT_U32  numEntriesPerLine;/*the number of entries in single LINE.
                        NOTE: if value hold FRACTION_INDICATION_CNS meaning that the
                        number in the lower 31 bits is 'fraction' (1/x) and not (x)*/
    GT_U32  numLinesPerEntry = 1;/* the number of lines that the entry occupy.
                        NOTE: value used only if MORE than 1 */
    GT_U32  lineIndex;/* line index when iteration over numLinesPerEntry */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS ||
       (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0))
    {
        /* this needed for direct calls to prvCpssDxChPortGroupReadTableEntry with
           portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS , and still need convert
           of the entryIndex.
           for example :
           PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
        */

        /* convert if needed the entry index , get specific portGroupId (or all port groups) */
        indexAsPortNumConvert(devNum,tableType,entryIndex, &tmpPortGroupId , &entryIndex,&tableType);
    }

    /* validity check */
    if((GT_U32)tableType >= PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);


    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    orig_entryIndex = entryIndex;
    orig_entryValuePtr = entryValuePtr;
    startBit = 0;
    numBitsPerEntry = 0;/*dont care*/
    numEntriesPerLine = 0;/*dont care*/
    /* check if the table entry is actually implemented as
       'several entries' in single 'line' */
    if(GT_TRUE == multiEntriesInLineIndexAndGlobalBitConvert(devNum,tableType,
                INOUT &entryIndex,INOUT &startBit ,
                OUT &numBitsPerEntry , OUT &numEntriesPerLine))
    {
        if(numEntriesPerLine & FRACTION_HALF_TABLE_INDICATION_CNS)
        {
            if(orig_entryIndex >= (tableInfoPtr->maxNumOfEntries/2))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, "index[%d] is out of range [0..%d]",
                    orig_entryIndex,
                    (tableInfoPtr->maxNumOfEntries/2)-1);
            }

            /* indication that the entry uses multiple lines but not consecutive */
            status = prvCpssDxChPortGroupWriteTableEntry_splitOn2entries(
                devNum,
                portGroupId,
                tableType,
                orig_entryIndex,
                entryValuePtr,
                tableInfoPtr->maxNumOfEntries / 2,   /* halfTableIndexOffset */
                numBitsPerEntry                      /* halfEntryBitOffset   */
            );

            return status;
        }

        if(numEntriesPerLine & FRACTION_INDICATION_CNS)
        {
            /* indication that the entry uses multiple lines */
            numLinesPerEntry = numEntriesPerLine - FRACTION_INDICATION_CNS;
        }

        if(entryIndex >= tableInfoPtr->maxNumOfEntries)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, "tableType[%d] index[%d] is out of range [0..%d]",
                tableType,
                entryIndex,
                tableInfoPtr->maxNumOfEntries-1);
        }

        if((tableInfoPtr->entrySize * numLinesPerEntry) > MAX_ENTRY_SIZE_CNS)
        {
            /* need to set MAX_ENTRY_SIZE_CNS with larger value ?! */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
        if(tableInfoPtr->writeAccessType != PRV_CPSS_DXCH_DIRECT_ACCESS_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
        }

        /* Read the a whole entry from table by direct access method */
        tablePtr =
            (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->writeTablePtr);

        if(tablePtr->baseAddress == SPECIAL_ADDRESS_NOT_EXISTS_E)
        {
            /* the table not supported */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        if(numLinesPerEntry == 1)
        {
            for(lineIndex = 0 ; lineIndex < numLinesPerEntry ; lineIndex++ , entryIndex++)
            {
                address = tablePtr->baseAddress + entryIndex * tablePtr->step;

                status  = prvCpssHwPpPortGroupReadRam (devNum, portGroupId, address,
                    tableInfoPtr->entrySize, &entryMemoBufArr[0]);
                if(status != GT_OK)
                {
                   CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }
                /*
                   point the function to use the 'local' buffer that should support the
                   multi entries in single LINE , because the caller is not aware to
                   the actual size of the entry.
                */
                entryValuePtr = &entryMemoBufArr[0];

                /* we read full entry from HW , now update the relevant part with the
                   info from the caller  , so we can write it to the HW */
                copyBits(entryValuePtr,/* target */
                         startBit + (lineIndex * numBitsPerEntry),/* start bit in target */
                         orig_entryValuePtr,/*source*/
                         0,/*startBit In the source */
                         numBitsPerEntry);/* number of bits in the entry */

            }
            entryIndex -= numLinesPerEntry;/* restore entryIndex */
        }
        else
        {
            /* we write several lines ... not need to read any line */
            /* but we need to set the 'entry from caller' into 'lines info' */

            /*
               point the function to use the 'local' buffer that should support the
               multi entries in single LINE , because the caller is not aware to
               the actual size of the entry.
            */
            entryValuePtr = &entryMemoBufArr[0];

            for(lineIndex = 0 ; lineIndex < numLinesPerEntry ; lineIndex++)
            {
                /* we read full entry from HW , now update the relevant part with the
                   info from the caller  , so we can write it to the HW */
                copyBits(entryValuePtr,/* target */
                         lineIndex * tableInfoPtr->entrySize * 32,/* start bit in target */
                         orig_entryValuePtr,/*source*/
                         lineIndex*numBitsPerEntry,/*startBit In the source */
                         numBitsPerEntry);/* number of bits in the entry */
            }
        }
    }


    if(entryIndex >= tableInfoPtr->maxNumOfEntries)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, "tableType[%d] index[%d] is out of range [0..%d]",
            tableType,
            entryIndex,
            tableInfoPtr->maxNumOfEntries-1);
    }


    /* Check if HW access required. */
    if(prvDxChTableCpssHwAccessObj.hwAccessTableEntryWriteFunc != NULL)
    {
       status = prvDxChTableCpssHwAccessObj.hwAccessTableEntryWriteFunc(
                                                  devNum, tmpPortGroupId, tableType,
                                                  orig_entryIndex, orig_entryValuePtr, originPortGroupId,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E, status);
       if(status == GT_ABORTED)
        return GT_OK;

       if(status != GT_OK)
        return status;
    }

    if(tableInfoPtr->writeAccessType == PRV_CPSS_DXCH_INDIRECT_ACCESS_E)
        /* Write a whole entry to table by indirect access method */
        status = indirectWriteTableEntry(devNum, portGroupId,tableType, tableInfoPtr,
                                                     orig_entryIndex, orig_entryValuePtr);
    else
    {
        /* there is no information to write */
        if(entryValuePtr == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        /* Write a whole entry to table by direct access method */
        tablePtr =
            (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->writeTablePtr);

        if(tablePtr->baseAddress == SPECIAL_ADDRESS_NOT_EXISTS_E)
        {
            /* the table not supported */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        for(lineIndex = 0 ; lineIndex < numLinesPerEntry ; lineIndex++ , entryIndex++)
        {
            address = tablePtr->baseAddress + entryIndex * tablePtr->step;

            if (tablePtr->nextWordOffset == PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS)
            {
                /* VLT tables WA */
                if((GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_VLT_TABLES_WRITE_DIRECT_ACCESS_WA_E)) &&
                   ((PRV_CPSS_DXCH_TABLE_STG_E == tableType) ||
                   (PRV_CPSS_DXCH_TABLE_VLAN_E == tableType)))
                {
                    status  = prvCpssHwPpPortGroupReadRam (devNum, portGroupId, address, 1, &tmpReadBuff);
                    if(status != GT_OK)
                    {
                       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                }

                status = prvCpssHwPpPortGroupWriteRam(devNum, portGroupId, address, tableInfoPtr->entrySize,
                                                                     entryValuePtr);
            }
            else
            {
                address -= tablePtr->nextWordOffset;

                for (i=0; i<tableInfoPtr->entrySize; i++)
                {
                    address += tablePtr->nextWordOffset;

                    status =  prvCpssHwPpPortGroupWriteRam(devNum, portGroupId, address, 1, &entryValuePtr[i]);

                    if (status != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                }

                status = GT_OK;
            }

            /* jump to next partial entry */
            entryValuePtr += tableInfoPtr->entrySize;
        }
        entryIndex -= numLinesPerEntry;/* restore entryIndex */

    }

    /* Check if HW access required. */
    if(prvDxChTableCpssHwAccessObj.hwAccessTableEntryWriteFunc != NULL)
    {
       /* Don't care of returned status */
       prvDxChTableCpssHwAccessObj.hwAccessTableEntryWriteFunc(
                                                  devNum, tmpPortGroupId, tableType,
                                                  orig_entryIndex, orig_entryValuePtr, originPortGroupId,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E, status);
    }

    return status;
}

/*******************************************************************************
* readTableEntry_splitOn2entries
*
* DESCRIPTION:
*       function to read logical entry that is split to 2 lines in 2 parts of
*       the table.(offset of 1/2 table between them)
*       NOTE: function must be called only when :
*           0 == PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_GET_MAC(devNum,tableType)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table (of the first half of entry)
*       halfTableIndexOffset - offset of the second part of the entry from the 'entryIndex'
*       halfEntryBitOffset   - offset of the second part of the data (bit index) in entryValuePtr[]
* OUTPUTS:
*       entryValuePtr - (pointer to) the data read from the table
*                       may be NULL in the case of indirect table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS readTableEntry_splitOn2entries
(
    IN GT_U8                    devNum,
    IN GT_U32                   portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    OUT GT_U32                  *entryValuePtr,
    IN GT_U32                   halfTableIndexOffset,
    IN GT_U32                   halfEntryBitOffset
)
{
    GT_STATUS   rc;

    /*Read first half of the entry*/
    rc = prvCpssDxChPortGroupReadTableEntry(
        devNum,
        portGroupId,
        tableType,
        entryIndex,
        entryValuePtr
    );
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed to read table[%d] : first half of the entry index [%d]",
            tableType,entryIndex);
    }

    /*Read second half of the entry*/
    rc = prvCpssDxChPortGroupReadTableEntry(
        devNum,
        portGroupId,
        tableType,
        entryIndex + halfTableIndexOffset,
        &entryValuePtr[halfEntryBitOffset>>5]
    );
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "failed to read table[%d] : second half of the entry index [%d]",
            tableType,entryIndex);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntry_splitOn2entries
*
* DESCRIPTION:
*       function to read logical entry that is split to 2 lines in 2 parts of
*       the table.(offset of 1/2 table between them)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table (of the first half of entry)
*       halfTableIndexOffset - offset of the second part of the entry from the 'entryIndex'
*       halfEntryBitOffset   - offset of the second part of the data (bit index) in entryValuePtr[]
* OUTPUTS:
*       entryValuePtr - (pointer to) the data read from the table
*                       may be NULL in the case of indirect table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortGroupReadTableEntry_splitOn2entries
(
    IN GT_U8                    devNum,
    IN GT_U32                   portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT  tableType,
    IN GT_U32                   entryIndex,
    OUT GT_U32                  *entryValuePtr,
    IN GT_U32                   halfTableIndexOffset,
    IN GT_U32                   halfEntryBitOffset
)
{
    GT_STATUS   rc;
    GT_U32      needToRestoreHalfTableMode;

    needToRestoreHalfTableMode = PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_GET_MAC(devNum,tableType);
    if(needToRestoreHalfTableMode)
    {
        /* remove the option to get 'FRACTION_HALF_TABLE_INDICATION_CNS' from function
            multiEntriesInLineIndexAndGlobalBitConvert() that is called from
            prvCpssDxChPortGroupWriteTableEntry(...) */
        PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_CLEAR_MAC(devNum,tableType);
    }

    rc = readTableEntry_splitOn2entries(
        devNum,
        portGroupId,
        tableType,
        entryIndex,
        entryValuePtr,
        halfTableIndexOffset,
        halfEntryBitOffset
    );
    if(needToRestoreHalfTableMode)
    {
        /* restore value */
        PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_SET_MAC(devNum,tableType);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortGroupReadTableEntry
*
* DESCRIPTION:
*       Read a whole entry from the table. - for specific portGroupId
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*
* OUTPUTS:
*       entryValuePtr - (pointer to) the data read from the table
*                       may be NULL in the case of indirect table.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*      In the case of entryValuePtr == NULL and indirect table the function
*      just send read entry command to device. And entry is stored in the
*      data registers of a indirect table
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupReadTableEntry
(
    IN GT_U8                   devNum,
    IN GT_U32                  portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  entryIndex,
    OUT GT_U32                 *entryValuePtr
)
{
    GT_U32  entryMemoBufArr[MAX_ENTRY_SIZE_CNS];
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* table info */
    GT_U32      address;  /* address the information is read from */
    /* pointer to direct access table information */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *tablePtr;
    GT_U32      i;
    GT_STATUS   status = GT_OK;
    GT_U32  tmpPortGroupId = portGroupId;/* temporary port group id */
    GT_U32  originPortGroupId = portGroupId;
    GT_U32  *orig_entryValuePtr;/* original pointer from the caller */
    GT_U32  startBit;/* start bit in the case of multi entries in single LINE */
    GT_U32  numBitsPerEntry;/*the number of bits that the 'entry' is using in the LINE.*/
    GT_BOOL tableOfMultiEntriesInLine;/* indication of multi entries in single LINE */
    GT_U32  numEntriesPerLine;/*the number of entries in single LINE.
                        NOTE: if value hold FRACTION_INDICATION_CNS meaning that the
                        number in the lower 31 bits is 'fraction' (1/x) and not (x)*/
    GT_U32  numLinesPerEntry = 1;/* the number of lines that the entry occupy.
                        NOTE: value used only if MORE than 1 */
    GT_U32  lineIndex;/* line index when iteration over numLinesPerEntry */
    GT_U32  orig_entryIndex; /* orig entry index as calculate in this function */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS ||
       (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0))
    {
        /* this needed for direct calls to prvCpssDxChPortGroupReadTableEntry with
           portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS , and still need convert
           of the entryIndex.
           for example :
           PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
        */

        /* convert if needed the entry index , get specific portGroupId (or all port groups) */
        indexAsPortNumConvert(devNum,tableType,entryIndex, &tmpPortGroupId , &entryIndex,&tableType);
    }

    /* validity check */
    if((GT_U32)tableType >=  PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    orig_entryIndex = entryIndex;
    orig_entryValuePtr = entryValuePtr;
    startBit = 0;
    numBitsPerEntry = 0;/*dont care*/
    numEntriesPerLine = 0;/*dont care*/
    /* check if the table entry is actually implemented as
       'several entries' in single 'line' */
    if(GT_TRUE == multiEntriesInLineIndexAndGlobalBitConvert(devNum,tableType,
                INOUT &entryIndex,INOUT &startBit ,
                OUT &numBitsPerEntry, OUT &numEntriesPerLine))
    {
        if(numEntriesPerLine & FRACTION_HALF_TABLE_INDICATION_CNS)
        {
            if(orig_entryIndex >= (tableInfoPtr->maxNumOfEntries/2))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, "index[%d] is out of range [0..%d]",
                    orig_entryIndex,
                    (tableInfoPtr->maxNumOfEntries/2)-1);
            }
            /* indication that the entry uses multiple lines but not consecutive */
            status = prvCpssDxChPortGroupReadTableEntry_splitOn2entries(
                devNum,
                portGroupId,
                tableType,
                orig_entryIndex,
                entryValuePtr,
                tableInfoPtr->maxNumOfEntries / 2,   /* halfTableIndexOffset */
                numBitsPerEntry                      /* halfEntryBitOffset   */
            );

            return status;
        }

        if(numEntriesPerLine & FRACTION_INDICATION_CNS)
        {
            /* indication that the entry uses multiple lines */
            numLinesPerEntry = numEntriesPerLine - FRACTION_INDICATION_CNS;
        }

        /*
           point the function to use the 'local' buffer that should support the
           multi entries in single LINE , because the caller is not aware to
           the actual size of the entry.
        */
        entryValuePtr = &entryMemoBufArr[0];

        tableOfMultiEntriesInLine = GT_TRUE;
    }
    else
    {
        tableOfMultiEntriesInLine = GT_FALSE;
    }

    if(entryIndex >= tableInfoPtr->maxNumOfEntries)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, "tableType[%d] index[%d] is out of range [0..%d]",
            tableType,
            entryIndex,
            tableInfoPtr->maxNumOfEntries-1);
    }

    if(prvDxChTableCpssHwAccessObj.hwAccessTableEntryReadFunc != NULL)
    {
       status = prvDxChTableCpssHwAccessObj.hwAccessTableEntryReadFunc(
                                                  devNum, tmpPortGroupId, tableType,
                                                  entryIndex, entryValuePtr, originPortGroupId,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E, status);
       if(status == GT_ABORTED)
        return GT_OK;

       if(status != GT_OK)
        return status;
    }

    for(lineIndex = 0 ; lineIndex < numLinesPerEntry ; lineIndex++ , entryIndex++)
    {
        if(tableInfoPtr->readAccessType == PRV_CPSS_DXCH_INDIRECT_ACCESS_E)
        {
            /* Read a whole entry from table by indirect access method */
            status = indirectReadTableEntry(devNum, portGroupId, tableType, tableInfoPtr,
                                                         entryIndex,
                 (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC*)(tableInfoPtr->readTablePtr),
                                              entryValuePtr);
        }
        else
        {    /* Read a whole entry from table by direct access method */
            tablePtr =
                (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->readTablePtr);

            if(tablePtr->baseAddress == SPECIAL_ADDRESS_NOT_EXISTS_E)
            {
                /* the table not supported */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            address = tablePtr->baseAddress + entryIndex * tablePtr->step;

            if (tablePtr->nextWordOffset == PRV_CPSS_DXCH_DEFAULT_NEXT_WORD_OFFSET_CNS)
            {
                status = prvCpssHwPpPortGroupReadRam(devNum, portGroupId,address, tableInfoPtr->entrySize,
                                                                     entryValuePtr);
            }
            else
            {
                address -= tablePtr->nextWordOffset;

                for (i=0; i<tableInfoPtr->entrySize; i++)
                {
                    address += tablePtr->nextWordOffset;

                    status =  prvCpssHwPpPortGroupReadRam(devNum, portGroupId,address, 1, &entryValuePtr[i]);

                    if (status != GT_OK)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                    }
                }

                status = GT_OK;
            }
        }

        if(tableOfMultiEntriesInLine == GT_TRUE)
        {
            /* we are done reading the info from HW , but we need to prepare it for
               the 'caller' */
            copyBits(orig_entryValuePtr ,/* target */
                     numBitsPerEntry * lineIndex,/* start bit in target */
                     entryValuePtr,/*source*/
                     startBit,/*startBit In the source */
                     numBitsPerEntry);/* number of bits in the entry */
        }
    }
    entryIndex -= numLinesPerEntry;/* restore entryIndex */

    if(tableOfMultiEntriesInLine == GT_TRUE)
    {
        entryValuePtr = orig_entryValuePtr;
    }


    if(prvDxChTableCpssHwAccessObj.hwAccessTableEntryReadFunc != NULL)
    {
       prvDxChTableCpssHwAccessObj.hwAccessTableEntryReadFunc(
                                                  devNum, tmpPortGroupId, tableType,
                                                  entryIndex, entryValuePtr, originPortGroupId,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E, status);
    }

    return status;
}

/*******************************************************************************
* prvCpssDxChPortGroupReadTableEntryField
*
* DESCRIPTION:
*       Read a field from the table. - for specific portGroupId
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*                   if need global offset in the field of fieldOffset
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   if need 'field name' in the field of fieldOffset
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   if need 'field name' in the field of fieldOffset
*       fieldOffset  - field offset
*       fieldLength - field length
*                   when fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   the field 'fieldLength' is used ONLY if  it's value != 0
*                   and then it means 'Mumber of consecutive fields' !!!
*
* OUTPUTS:
*       fieldValuePtr - (pointer to) the data read from the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupReadTableEntryField
(
    IN GT_U8                  devNum,
    IN GT_U32                  portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    OUT GT_U32                *fieldValuePtr
)
{
    GT_U32  entryMemoBufArr[MAX_ENTRY_SIZE_CNS];
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32 address; /* address to read from */
    /* pointer to direct table info */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *tablePtr;
    GT_STATUS status = GT_OK;
    GT_U32  tmpPortGroupId = portGroupId;/* temporary port group id */
    GT_U32  originPortGroupId = portGroupId;
    GT_U32  subFieldOffset;/* the sub field offset */
    GT_U32  subFieldLength;/* the sub field length */
    PRV_CPSS_DXCH_PP_TABLE_FORMAT_INFO_STC *  fieldsInfoPtr;
    GT_U32  orig_entryIndex; /* orig entry index as calculate in this function */
    GT_U32  orig_fieldOffset;/* orig entry field Offset as calculate in this function */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS ||
       (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0))
    {
        /* this needed for direct calls to prvCpssDxChPortGroupReadTableEntry with
           portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS , and still need convert
           of the entryIndex.
           for example :
           PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
        */

        /* convert if needed the entry index , get specific portGroupId (or all port groups) */
        indexAsPortNumConvert(devNum,tableType,entryIndex, &tmpPortGroupId , &entryIndex,&tableType);
    }

    /* validity check */
    if((GT_U32)tableType >= PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    if(fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS)
    {
        fieldsInfoPtr = tableFieldsFormatInfoGet(devNum, tableType);
        if(fieldsInfoPtr == NULL)
        {
            /* the table is not bound to fields format */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        if (fieldsInfoPtr->patternNum == 0)
        {
            /* convert field info to global bit position */
            status = convertFieldInfoToGlobalBit(devNum,tableType,&fieldWordNum,&fieldOffset,&fieldLength,0);
            if(status != GT_OK)
            {
                return status;
            }
        }
        else    /* pattern entry */
        {
            /* convert index and pattern field info to new index and global bit position */
            status = convertPatternFieldInfoToGlobalBit(devNum, tableType, &entryIndex, &fieldWordNum, &fieldOffset, &fieldLength, 0);
            if(status != GT_OK)
            {
                return status;
            }
        }
    }
    else if(fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_SUB_FIELD_IN_FIELD_NAME_CNS)
    {
        /* reverse the operation of PRV_CPSS_DXCH_TABLE_SUB_FIELD_OFFSET_AND_LENGTH_MAC */
        subFieldOffset = fieldLength & 0xFFFF;
        subFieldLength = fieldLength >> 16;

        /* convert field info to global bit position */
        status = convertSubFieldInfoToGlobalBit(devNum,tableType,subFieldOffset,subFieldLength,&fieldWordNum,&fieldOffset,&fieldLength,0);
        if(status != GT_OK)
        {
            return status;
        }
    }

    orig_entryIndex  = entryIndex;
    orig_fieldOffset = fieldOffset;

    if(fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS)
    {
        /* check if the table entry is actually implemented as
           'several entries' in single 'line' */
        multiEntriesInLineIndexAndGlobalBitConvert(devNum,tableType,
                    INOUT &entryIndex,INOUT &fieldOffset,NULL,NULL);

        /* the field of fieldOffset is used as 'Global offset' in the entry */
        if((entryIndex >= tableInfoPtr->maxNumOfEntries)    ||
           ((fieldOffset + fieldLength) > (tableInfoPtr->entrySize * 32)) ||
           ((fieldLength == 0) || (fieldLength > 32)))
        {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* check if the write is to single word or more */
        if(/*(fieldLength > 1) && */OFFSET_TO_WORD_MAC(fieldOffset) != OFFSET_TO_WORD_MAC(fieldOffset + fieldLength))
        {
            /* writing to more than single word in the entry */

            if(tableInfoPtr->entrySize > MAX_ENTRY_SIZE_CNS)
            {
                /* need to set MAX_ENTRY_SIZE_CNS with larger value ?! */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            return prvCpssDxChPortGroupReadTableEntryFieldList(devNum,portGroupId ,
                tableType,orig_entryIndex,entryMemoBufArr,1,
                &orig_fieldOffset,&fieldLength,fieldValuePtr);
        }

        fieldWordNum = OFFSET_TO_WORD_MAC(fieldOffset);
        fieldOffset  = OFFSET_TO_BIT_MAC(fieldOffset);
    }
    else
    if((entryIndex >= tableInfoPtr->maxNumOfEntries)    ||
       ((fieldOffset + fieldLength) > 32)               ||
       (fieldLength == 0)                               ||
       (fieldWordNum >= tableInfoPtr->entrySize))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);


    /* Check if HW access required. */
    if(prvDxChTableCpssHwAccessObj.hwAccessTableEntryFieldReadFunc != NULL)
    {
       status = prvDxChTableCpssHwAccessObj.hwAccessTableEntryFieldReadFunc(
                                                  devNum, tmpPortGroupId, tableType,
                                                  entryIndex, fieldWordNum, fieldOffset,
                                                  fieldLength,fieldValuePtr, originPortGroupId,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E, status);
       if(status == GT_ABORTED)
        return GT_OK;

       if(status != GT_OK)
        return status;
    }

    if(tableInfoPtr->readAccessType == PRV_CPSS_DXCH_INDIRECT_ACCESS_E)
        /*Read a field of entry from table by indirect access method. */
        status = indirectReadTableEntryField(devNum, portGroupId,tableType, tableInfoPtr,
               entryIndex,fieldWordNum, fieldOffset, fieldLength, fieldValuePtr);
    else
    {
        /*Read a field of entry from table by direct access method. */
        tablePtr =
             (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->readTablePtr);

        if(tablePtr->baseAddress == SPECIAL_ADDRESS_NOT_EXISTS_E)
        {
            /* the table not supported */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        address = tablePtr->baseAddress + entryIndex * tablePtr->step +
                                                               fieldWordNum * tablePtr->nextWordOffset;

        status = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, address,fieldOffset, fieldLength,
                                                                fieldValuePtr);

    }

    /* Check if HW access required. */
    if(prvDxChTableCpssHwAccessObj.hwAccessTableEntryFieldReadFunc != NULL)
    {
       prvDxChTableCpssHwAccessObj.hwAccessTableEntryFieldReadFunc(
                                                  devNum, tmpPortGroupId, tableType,
                                                  entryIndex, fieldWordNum, fieldOffset,
                                                  fieldLength,fieldValuePtr, originPortGroupId,
                                                  CPSS_DRV_HW_ACCESS_STAGE_POST_E, status);
    }

    return status;

}

/*******************************************************************************
* prvCpssDxChPortGroupWriteTableEntryField
*
* DESCRIPTION:
*       Write a field to the table.  - for specific portGroupId
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       tableType - the specific table name
*       entryIndex   - index in the table
*       fieldWordNum - field word number
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
*                   if need global offset in the field of fieldOffset
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   if need 'field name' in the field of fieldOffset
*                   use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   if need 'field name' in the field of fieldOffset
*       fieldOffset  - field offset
*       fieldLength - field length
*                   when fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS
*                   the field 'fieldLength' is used ONLY if  it's value != 0
*                   and then it means 'Mumber of consecutive fields' !!!
*       fieldValue - the data write to the table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In xCat A1 and above devices the data is updated only when the last
*       word in the entry was written.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupWriteTableEntryField
(
    IN GT_U8                  devNum,
    IN GT_U32                 portGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                 entryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    IN GT_U32                 fieldValue
)
{
    GT_U32  entryMemoBufArr[MAX_ENTRY_SIZE_CNS];
    GT_STATUS   status = GT_OK;  /* return code */
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    GT_U32 address; /* address to write to */
    /* pointer to direct table info */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *tablePtr;
    GT_U32      entrySize;  /* table entry size in words */
    GT_U32  tmpPortGroupId = portGroupId;/* temporary port group id */
    GT_U32  originPortGroupId = portGroupId;
    GT_U32  contWriteTable = 1;
    GT_U32  subFieldOffset;/* the sub field offset */
    GT_U32  subFieldLength;/* the sub field length */
    PRV_CPSS_DXCH_PP_TABLE_FORMAT_INFO_STC *  fieldsInfoPtr;
    GT_U32  orig_entryIndex; /* orig entry index as calculate in this function */
    GT_U32  orig_fieldOffset;/* orig entry field Offset as calculate in this function */
    GT_U32  orig_fieldWordNum;/* orig entry field wordNum as calculate in this function */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS ||
       (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == 0))
    {
        /* this needed for direct calls to prvCpssDxChPortGroupReadTableEntry with
           portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS , and still need convert
           of the entryIndex.
           for example :
           PRV_CPSS_DXCH_LION_TABLE_TXQ_SHAPER_PER_PORT_TOKEN_BUCKET_CONFIG_E
        */

        /* convert if needed the entry index , get specific portGroupId (or all port groups) */
        indexAsPortNumConvert(devNum,tableType,entryIndex, &tmpPortGroupId , &entryIndex,&tableType);
    }

    /* validity check */
    if((GT_U32)tableType >= PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum))
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);

    entrySize = tableInfoPtr->entrySize;

    if(fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS)
    {
        fieldsInfoPtr = tableFieldsFormatInfoGet(devNum, tableType);
        if(fieldsInfoPtr == NULL)
        {
            /* the table is not bound to fields format */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
        if (fieldsInfoPtr->patternNum == 0)
        {
            /* convert field info to global bit position */
            status = convertFieldInfoToGlobalBit(devNum,tableType,&fieldWordNum,&fieldOffset,&fieldLength,fieldValue);
            if(status != GT_OK)
            {
                return status;
            }
        }
        else    /* pattern entry */
        {
            /* convert index and pattern field info to new index and global bit position */
            status = convertPatternFieldInfoToGlobalBit(devNum, tableType, &entryIndex, &fieldWordNum, &fieldOffset, &fieldLength, fieldValue);
            if(status != GT_OK)
            {
                return status;
            }
        }
    }
    else if(fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_SUB_FIELD_IN_FIELD_NAME_CNS)
    {
        /* reverse the operation of PRV_CPSS_DXCH_TABLE_SUB_FIELD_OFFSET_AND_LENGTH_MAC */
        subFieldOffset = fieldLength & 0xFFFF;
        subFieldLength = fieldLength >> 16;

        /* convert field info to global bit position */
        status = convertSubFieldInfoToGlobalBit(devNum,tableType,subFieldOffset,subFieldLength,&fieldWordNum,&fieldOffset,&fieldLength,fieldValue);
        if(status != GT_OK)
        {
            return status;
        }
    }

    orig_entryIndex  = entryIndex;
    orig_fieldOffset = fieldOffset;
    orig_fieldWordNum = fieldWordNum;

    if(fieldWordNum == PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS)
    {
        /* check if the table entry is actually implemented as
           'several entries' in single 'line' */
        multiEntriesInLineIndexAndGlobalBitConvert(devNum,tableType,
                    INOUT &entryIndex,INOUT &fieldOffset,NULL,NULL);


        /* the field of fieldOffset is used as 'Global offset' in the entry */
        if((entryIndex >= tableInfoPtr->maxNumOfEntries)    ||
           ((fieldOffset + fieldLength) > (entrySize * 32)) ||
           ((fieldLength == 0) || (fieldLength > 32)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }

        /* check if the write is to single word or more */
        if(/*(fieldLength > 1) && */OFFSET_TO_WORD_MAC(fieldOffset) != OFFSET_TO_WORD_MAC(fieldOffset + fieldLength))
        {
            /* writing to more than single word in the entry */

            if(entrySize > MAX_ENTRY_SIZE_CNS)
            {
                /* need to set MAX_ENTRY_SIZE_CNS with larger value ?! */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }

            return prvCpssDxChPortGroupWriteTableEntryFieldList(devNum,portGroupId ,
                tableType,orig_entryIndex,entryMemoBufArr,1,
                &orig_fieldOffset,&fieldLength,&fieldValue);
        }

        orig_fieldWordNum = OFFSET_TO_WORD_MAC(orig_fieldOffset);

        fieldWordNum = OFFSET_TO_WORD_MAC(fieldOffset);
        fieldOffset  = OFFSET_TO_BIT_MAC(fieldOffset);
    }
    else
    if((entryIndex >= tableInfoPtr->maxNumOfEntries)    ||
       ((fieldOffset + fieldLength) > 32)               ||
       (fieldLength == 0)                               ||
       (fieldWordNum >= entrySize))
    {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* Check if HW access required. */
    if(prvDxChTableCpssHwAccessObj.hwAccessTableEntryFieldWriteFunc != NULL)
    {
       status = prvDxChTableCpssHwAccessObj.hwAccessTableEntryFieldWriteFunc(
                                                  devNum, tmpPortGroupId, tableType,
                                                  entryIndex, fieldWordNum, fieldOffset,
                                                  fieldLength,fieldValue, originPortGroupId,
                                                  CPSS_DRV_HW_ACCESS_STAGE_PRE_E, status);
       if(status == GT_ABORTED)
        return GT_OK;

       if(status != GT_OK)
        return status;
    }

    if(tableInfoPtr->writeAccessType == PRV_CPSS_DXCH_INDIRECT_ACCESS_E)
        /* Write a field to table by indirect access method. */
        status = indirectWriteTableEntryField(devNum, portGroupId, tableType, tableInfoPtr,
                 entryIndex,fieldWordNum, fieldOffset, fieldLength, fieldValue);
    else
    {
        /* Write a field to table by direct access method. */
        tablePtr =
             (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->writeTablePtr);

        if(tablePtr->baseAddress == SPECIAL_ADDRESS_NOT_EXISTS_E)
        {
            /* the table not supported */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* read / write tableEntryField per port group */
        if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS && PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            GT_PORT_GROUPS_BMP  portGroupsBmp;/* bmp of port groups */
            if(GT_TRUE ==
                prvCpssDuplicatedMultiPortGroupsGet(devNum,portGroupId,tablePtr->baseAddress,
                    &portGroupsBmp,NULL,NULL))
            {
                /* when the entry involve 'per port info' and 'global info'
                 (like vlan entry) between hemispheres , need to update each port group
                 according to it's existing info and not according to 'representative'
                 port group */

                PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum, portGroupsBmp , portGroupId)
                {
                    status = prvCpssDxChPortGroupWriteTableEntryField(
                        devNum, portGroupId, tableType,
                        orig_entryIndex, orig_fieldWordNum,
                        fieldOffset, fieldLength, fieldValue);
                    if (status != GT_OK)
                    {
                        return status;
                    }
                }
                PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum, portGroupsBmp , portGroupId)

                contWriteTable = 0;
                status = GT_OK;
            }
        }
        if (contWriteTable)
        {
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* In xCat and above devices the data is updated only when the last */
                /* word in the entry was written. */
                if(entrySize > MAX_ENTRY_SIZE_CNS)
                {
                    /* need to set MAX_ENTRY_SIZE_CNS with larger value ?! */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                }

                address = tablePtr->baseAddress + entryIndex * tablePtr->step;

                /* read whole entry */
                status = prvCpssHwPpPortGroupReadRam(devNum, portGroupId, address, entrySize,
                                                                 entryMemoBufArr);
                if (status != GT_OK)
                {
                    return status;
                }

                /* update field */
                U32_SET_FIELD_MAC(entryMemoBufArr[fieldWordNum], fieldOffset, fieldLength, fieldValue);

                /* write whole entry */
                status = prvCpssHwPpPortGroupWriteRam(devNum, portGroupId, address, entrySize,
                                                                 entryMemoBufArr);
                if (status != GT_OK)
                {
                    return status;
                }
            }
            else
            {
                 address = tablePtr->baseAddress +
                                    entryIndex * tablePtr->step + fieldWordNum * tablePtr->nextWordOffset;

                status =  prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, address,fieldOffset,fieldLength,fieldValue);
            }
        } /*contWriteTable*/
    }

    if(prvDxChTableCpssHwAccessObj.hwAccessTableEntryFieldWriteFunc != NULL)
    {
       prvDxChTableCpssHwAccessObj.hwAccessTableEntryFieldWriteFunc(
                                                      devNum, tmpPortGroupId, tableType,
                                                      entryIndex, fieldWordNum, fieldOffset,
                                                      fieldLength,fieldValue, originPortGroupId,
                                                      CPSS_DRV_HW_ACCESS_STAGE_POST_E, status);

    }

    return status;
}

/*******************************************************************************
* prvCpssDxChHwTblAddrStcInfoGet
*
* DESCRIPTION:
*   This function return the address of the tables struct of cheetah devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       tblAddrPtrPtr - (pointer to) address of the tables struct
*       sizePtr - (pointer to) size of tables struct
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwTblAddrStcInfoGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    **tblAddrPtrPtr,
    OUT GT_U32    *sizePtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    *tblAddrPtrPtr = (GT_U32 *)PRV_CPSS_DXCH_DEV_TBLS_MAC(devNum);
    *sizePtr = PRV_CPSS_DXCH_TABLE_SIZE_GET_MAC(devNum);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChWriteTableMultiEntry
*
* DESCRIPTION:
*       Write number of entries to the table in consecutive indexes.
* INPUTS:
*       devNum               - the device number
*       tableType            - the specific table name
*       startIndex           - index to the first table entry
*       numOfEntries         - the number of consecutive entries to write
*       entryValueArrayPtr   - (pointer to) the data that will be written to the table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None;
*******************************************************************************/
GT_STATUS prvCpssDxChWriteTableMultiEntry
(
    IN GT_U8                            devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT          tableType,
    IN GT_U32                           startIndex,
    IN GT_U32                           numOfEntries,
    IN GT_U32                           *entryValueArrayPtr
)
{
    GT_U32  ii;         /* loop iterator            */
    GT_U32  entrySize;  /* entry size in words      */
    GT_U32  rc;         /* return code              */
    PRV_CPSS_DXCH_TABLES_INFO_STC   *tableInfoPtr;

    CPSS_NULL_PTR_CHECK_MAC(entryValueArrayPtr);

    if( 0 == numOfEntries )
        return GT_OK;

    /* calculate entry size in words*/
    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);
    entrySize = NUM_WORDS_FROM_BITS_MAC(tableInfoPtr->entrySize);

    for( ii = startIndex ; ii < startIndex + numOfEntries ; ii++ )
    {
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        tableType,
                                        ii,
                                        entryValueArrayPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        entryValueArrayPtr+=entrySize;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChReadTableMultiEntry
*
* DESCRIPTION:
*       Read number of entries from the table in consecutive indexes.
* INPUTS:
*       devNum          - the device number
*       tableType       - the specific table name
*       startIndex      - index to the first table entry
*       numOfEntries    - the number of consecutive entries to read
*
* OUTPUTS:
*       entryValueArrayPtr   - (pointer to) the data that will be read from the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None;
*******************************************************************************/
GT_STATUS prvCpssDxChReadTableMultiEntry
(
    IN  GT_U8                            devNum,
    IN  PRV_CPSS_DXCH_TABLE_ENT          tableType,
    IN  GT_U32                           startIndex,
    IN  GT_U32                           numOfEntries,
    OUT GT_U32                           *entryValueArrayPtr
)
{
    GT_U32  ii;         /* loop iterator            */
    GT_U32  entrySize;  /* entry size in words      */
    GT_U32  rc;         /* return code              */
    PRV_CPSS_DXCH_TABLES_INFO_STC   *tableInfoPtr;

    CPSS_NULL_PTR_CHECK_MAC(entryValueArrayPtr);

    if( 0 == numOfEntries )
        return GT_OK;

    /* calculate entry size in words*/
    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);
    entrySize = NUM_WORDS_FROM_BITS_MAC(tableInfoPtr->entrySize);

    for( ii = startIndex ; ii < startIndex + numOfEntries ; ii++ )
    {
        rc = prvCpssDxChReadTableEntry(devNum,
                                       tableType,
                                       ii,
                                       entryValueArrayPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        entryValueArrayPtr+=entrySize;
    }

    return GT_OK;
}

#ifdef DXCH_TABLES_DEBUG
GT_VOID debugPrintTable
(
     IN GT_U8  devNum
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC   *tableInfoPtr;
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_UNIT_STC * directAccessTableInfoPtr;
    FILE    *memObjFilePtr;             /* Dump memory file descriptor */
    char buffer[128];                  /* Read/Write buffer */
    GT_U32 ii;
    GT_U32 index;
    char *caption[]    = {"Index", "EntrySize", "EntryNums", "AccessType", "Address", "WordOffset", "Step"};
    char *directCaption[] =   {"Index", "EntrySize", "EntryNums", "Read/Write", "Address", "WordOffset", "Step"};
    char *indirectCaption[] = {"Index", "EntrySize", "EntryNums", "Read/Write", "CtrlolReg", "DataReg", "TriggerBit", "IndexBit", "specTblVal", "specTblBit", "ActionBit"};
    char *device[] = {"Bobcat2", "xCat3", "xCat","Bobcat3"};    /* Bobcat2/Caelum, Bobcat3, xCat3 or xCat device name */
    GT_U32   directAccessBobcat2TableInfoNumEntries =
    NUM_ELEMENTS_IN_ARR_MAC(directAccessBobcat2TableInfo);

    GT_U32 tableInfoSize;

    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *directAccessInfoPtr;
    PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *indirectAccessInfoPtr;

    /* Open file for write */
    memObjFilePtr = fopen("table_info.txt", "w+");
    if (memObjFilePtr == NULL)
    {
        cpssOsPrintf("File open error!!!\n");
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E)
    {
        directAccessTableInfoPtr = directAccessBobcat2TableInfo;
        tableInfoPtr = bobcat2TablesInfo;
        ii = 0;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        tableInfoPtr = dxChXcat3TablesInfo;
        tableInfoSize = dxChXcat3TableInfoSize;
        ii = 1;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        tableInfoPtr = dxChXcatTablesInfo;
        tableInfoSize = dxChXcatTableInfoSize;
        ii = 2;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E)
    {
        CPSS_TBD_BOOKMARK_BOBCAT3
        directAccessTableInfoPtr = directAccessBobcat2TableInfo;
        tableInfoPtr = bobcat2TablesInfo;
        ii = 3;
    }
    else
    {
        fprintf(memObjFilePtr, "Running device does not support <debugPrintTable> utility\n");
        fclose(memObjFilePtr);
        return;
    }

    cpssOsSprintf(buffer, "Device: %10s\n", device[ii]);
    fprintf(memObjFilePtr, buffer);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E ||
        PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E)
    {
        cpssOsSprintf(buffer, "%10s %10s %10s %10s %10s %10s %10s\n",
                  caption[0], caption[1], caption[2], caption[3], caption[4],
                  caption[5], caption[6]);

        fprintf(memObjFilePtr, buffer);

        for(ii = 0 ; ii < directAccessBobcat2TableInfoNumEntries ; ii++)
        {
            if((directAccessTableInfoPtr[ii].directAccessInfo.baseAddress ==
                SPECIAL_ADDRESS_NOT_MODIFIED_E) ||
                ( directAccessTableInfoPtr[ii].directAccessInfo.baseAddress ==
                SPECIAL_ADDRESS_NOT_EXISTS_E))
            {
                /* no need to modify the entry (same as Lion2 / Lion / xcat2) */
                continue;
            }
            index = directAccessTableInfoPtr[ii].globalIndex;

            cpssOsSprintf(buffer, "%10d %10d %10d %10d 0x%08x %10d %10d\n",
                          index,
                          tableInfoPtr[index].entrySize,
                          tableInfoPtr[index].maxNumOfEntries,
                          tableInfoPtr[index].writeAccessType,
                          directAccessTableInfoPtr[ii].directAccessInfo.baseAddress,
                          directAccessTableInfoPtr[ii].directAccessInfo.nextWordOffset,
                          directAccessTableInfoPtr[ii].directAccessInfo.step);

            fprintf(memObjFilePtr, buffer);
        }
    }
    else /* CPSS_PP_FAMILY_DXCH_XCAT3_E, CPSS_PP_FAMILY_DXCH_XCAT_E */
    {
        fprintf(memObjFilePtr, "Direct Access Tables info:\n");

        cpssOsSprintf(buffer, "%10s %10s %10s %10s %10s %10s %10s\n",
                      directCaption[0], directCaption[1], directCaption[2],
                      directCaption[3], directCaption[4], directCaption[5],
                      directCaption[6]);

        fprintf(memObjFilePtr, buffer);

        fprintf(memObjFilePtr, "Indirect Access Tables info:\n");

        cpssOsSprintf(buffer, "%10s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s\n\n",
                      indirectCaption[0], indirectCaption[1], indirectCaption[2],
                      indirectCaption[3], indirectCaption[4], indirectCaption[5],
                      indirectCaption[6], indirectCaption[7], indirectCaption[8],
                      indirectCaption[9], indirectCaption[10]);

        fprintf(memObjFilePtr, buffer);

        for(ii = 0 ; ii < tableInfoSize ; ii++)
        {
            if( tableInfoPtr[ii].readTablePtr != NULL )
            {
                if( tableInfoPtr[ii].readAccessType ==
                                                PRV_CPSS_DXCH_DIRECT_ACCESS_E )
                {
                    directAccessInfoPtr = (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *)
                                                     tableInfoPtr[ii].readTablePtr;
                    cpssOsSprintf(buffer, "%10d %10d %10d %10s 0x%08x %10d %10d\n",
                              ii,
                              tableInfoPtr[ii].entrySize,
                              tableInfoPtr[ii].maxNumOfEntries,
                              "Read",
                              directAccessInfoPtr->baseAddress,
                              directAccessInfoPtr->nextWordOffset,
                              directAccessInfoPtr->step);

                    fprintf(memObjFilePtr, buffer);
                }

                if( tableInfoPtr[ii].readAccessType ==
                                                PRV_CPSS_DXCH_INDIRECT_ACCESS_E )
                {
                    indirectAccessInfoPtr = (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *)
                                                     tableInfoPtr[ii].readTablePtr;
                    cpssOsSprintf(buffer, "%10d %10d %10d %10s 0x%08x 0x%08x %10d %10d %10d %10d %10d\n",
                              ii,
                              tableInfoPtr[ii].entrySize,
                              tableInfoPtr[ii].maxNumOfEntries,
                              "I-Read",
                              indirectAccessInfoPtr->controlReg,
                              indirectAccessInfoPtr->dataReg,
                              indirectAccessInfoPtr->trigBit,
                              indirectAccessInfoPtr->indexBit,
                              indirectAccessInfoPtr->specificTableValue,
                              indirectAccessInfoPtr->specificTableBit,
                              indirectAccessInfoPtr->actionBit);

                    fprintf(memObjFilePtr, buffer);
                }
            }

            if( tableInfoPtr[ii].writeTablePtr != NULL )
            {
                if( tableInfoPtr[ii].writeAccessType ==
                                                PRV_CPSS_DXCH_DIRECT_ACCESS_E )
                {
                    directAccessInfoPtr = (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *)
                                                     tableInfoPtr[ii].writeTablePtr;
                    cpssOsSprintf(buffer, "%10d %10d %10d %10s 0x%08x %10d %10d\n",
                              ii,
                              tableInfoPtr[ii].entrySize,
                              tableInfoPtr[ii].maxNumOfEntries,
                              "Write",
                              directAccessInfoPtr->baseAddress,
                              directAccessInfoPtr->nextWordOffset,
                              directAccessInfoPtr->step);

                    fprintf(memObjFilePtr, buffer);
                }



                if( tableInfoPtr[ii].writeAccessType ==
                                                PRV_CPSS_DXCH_INDIRECT_ACCESS_E )
                {
                    indirectAccessInfoPtr = (PRV_CPSS_DXCH_TABLES_INFO_INDIRECT_STC *)
                                                     tableInfoPtr[ii].writeTablePtr;
                    cpssOsSprintf(buffer, "%10d %10d %10d %10s 0x%08x 0x%08x %10d %10d %10d %10d %10d\n",
                              ii,
                              tableInfoPtr[ii].entrySize,
                              tableInfoPtr[ii].maxNumOfEntries,
                              "I-Write",
                              indirectAccessInfoPtr->controlReg,
                              indirectAccessInfoPtr->dataReg,
                              indirectAccessInfoPtr->trigBit,
                              indirectAccessInfoPtr->indexBit,
                              indirectAccessInfoPtr->specificTableValue,
                              indirectAccessInfoPtr->specificTableBit,
                              indirectAccessInfoPtr->actionBit);

                    fprintf(memObjFilePtr, buffer);
                }
            }
        }
    }

    fclose(memObjFilePtr);
}
#endif

/*******************************************************************************
* prvCpssDxChTableBaseAddrGet
*
* DESCRIPTION:
*       This function retrieves base address of table.(for 'Direct access' tables)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*       devNum               - the device number
*       tableType            - the specific table name
* OUTPUTS:
*       errorPtr - (pointer to) indication that function did error. (can be NULL)
*
* RETURNS:
*       on success - return the base address
*       on error   - return address that ((address % 4) != 0)
*
* COMMENTS:
*
*******************************************************************************/
GT_U32  prvCpssDxChTableBaseAddrGet
(
    IN GT_U8                            devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT          tableType,
    OUT GT_BOOL                         *errorPtr
)
{
    PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr; /* pointer to table info */
    PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC *tablePtr;/* pointer to direct table info */
    GT_U32      address;

    tableInfoPtr = PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);
    if(tableInfoPtr->writeAccessType == PRV_CPSS_DXCH_INDIRECT_ACCESS_E)
    {
        if(errorPtr)
        {
            *errorPtr = GT_TRUE;
        }
        return SPECIAL_ADDRESS_NOT_EXISTS_E;
    }
    tablePtr =
         (PRV_CPSS_DXCH_TABLES_INFO_DIRECT_STC*)(tableInfoPtr->writeTablePtr);

    address = tablePtr->baseAddress;
    if(errorPtr)
    {
        if(address == SPECIAL_ADDRESS_NOT_EXISTS_E)
        {
            *errorPtr = GT_TRUE;
        }
        else
        {
            *errorPtr = GT_FALSE;
        }
    }

    return address;
}

/*******************************************************************************
* prvCpssDxChTablePortsBmpCheck
*
* DESCRIPTION:
*       This function checks that the bmp of ports can be supported by the device.
*       assumption that it is bmp of port of 'this' device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum               - the device number
*       portsBmpPtr          - pointer to the bmp of ports
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - when bmp hold ports that not supported by the device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   prvCpssDxChTablePortsBmpCheck
(
    IN GT_U8                     devNum,
    IN CPSS_PORTS_BMP_STC       *portsBmpPtr
)
{
    return prvCpssDxChTablePortsBmpLimitedNumCheck(devNum,0xFFFFFFFF,portsBmpPtr);
}

/*******************************************************************************
* prvCpssDxChTablePortsBmpLimitedNumCheck
*
* DESCRIPTION:
*       This function checks that the bmp of ports can be supported by the device.
*       assumption that it is bmp of port of 'this' device.
*       with limited number of ports that supported
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum               - the device number
*       limitedNum           - limited number of ports in the BMP that allowed
*                              to be different from zero.
*       portsBmpPtr          - pointer to the bmp of ports
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - when bmp hold ports that not supported by the device or
*                      bit is set over 'limitedNum'
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   prvCpssDxChTablePortsBmpLimitedNumCheck
(
    IN GT_U8                     devNum,
    IN GT_U32                    limitedNum,
    IN CPSS_PORTS_BMP_STC       *portsBmpPtr
)
{
    /* assume that device is DX and exists */
    CPSS_PORTS_BMP_STC          existingPorts;
    CPSS_PORTS_BMP_STC          tmpPortsBmp;
    GT_U32  numOfPorts = MIN(limitedNum,(PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(devNum)+1));

    /* check that pointer is valid */
    CPSS_NULL_PTR_CHECK_MAC(portsBmpPtr);

    /* get the bmp of supported port on this device */
    prvCpssDrvPortsFullMaskGet(
            numOfPorts ,
            &existingPorts);

    /* check if the IN bmp hold ports that are not in the supported ports */
    CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&tmpPortsBmp,portsBmpPtr,&existingPorts);

    if(CPSS_PORTS_BMP_IS_ZERO_MAC(&tmpPortsBmp))
    {
        return GT_OK;
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
        "The bitmap of ports hold 'set' bits over the supported amount[%d]",
        numOfPorts);
}

/*******************************************************************************
* prvCpssDxChTablePortsBmpAdjustToDevice
*
* DESCRIPTION:
*       This function removes none exists ports from bmp of ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum               - the device number
*       IN_portsBmpPtr       - pointer to the bmp of ports
* OUTPUTS:
*       OUT_portsBmpPtr      - pointer to the bmp of ports , that hold no ports
*                              that not supported by the device
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   prvCpssDxChTablePortsBmpAdjustToDevice
(
    IN GT_U8                     devNum,
    IN  CPSS_PORTS_BMP_STC       *IN_portsBmpPtr,
    OUT CPSS_PORTS_BMP_STC       *OUT_portsBmpPtr
)
{
    return prvCpssDxChTablePortsBmpAdjustToDeviceLimitedNum(devNum,0xFFFFFFFF,IN_portsBmpPtr,OUT_portsBmpPtr);
}

/*******************************************************************************
* prvCpssDxChTablePortsBmpAdjustToDeviceLimitedNum
*
* DESCRIPTION:
*       This function removes none exists ports from bmp of ports.
*       with limited number of ports that supported
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum               - the device number
*       limitedNum           - limited number of ports in the BMP that allowed
*                              to be different from zero.
*       IN_portsBmpPtr       - pointer to the bmp of ports
* OUTPUTS:
*       OUT_portsBmpPtr      - pointer to the bmp of ports , that hold no ports
*                              that not supported by the device
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   prvCpssDxChTablePortsBmpAdjustToDeviceLimitedNum
(
    IN GT_U8                     devNum,
    IN GT_U32                    limitedNum,
    IN  CPSS_PORTS_BMP_STC       *IN_portsBmpPtr,
    OUT CPSS_PORTS_BMP_STC       *OUT_portsBmpPtr
)
{
    /* assume that device is DX and exists */
    CPSS_PORTS_BMP_STC          existingPorts;
    GT_U32  numOfPorts = MIN(limitedNum,(PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum)));

    /* get the bmp of supported port on this device */
    prvCpssDrvPortsFullMaskGet(
        numOfPorts ,
            &existingPorts);

    /* remove from IN bmp ports that are not in the supported ports ..
        and set result into 'OUT' bmp */
    CPSS_PORTS_BMP_BITWISE_AND_MAC(OUT_portsBmpPtr,IN_portsBmpPtr,&existingPorts);

    return GT_OK;
}


/*******************************************************************************
* tablePlrMeterFieldInfoGet
*
* DESCRIPTION:
*       This function convert field for the 'generic meter' entry to :
*           1. the exact table type to use
*           2. the exact field name to use
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - the device number
*       fieldName            - name of the field to set
* OUTPUTS:
*       tableIdPtr           - pointer to table id of the actual table:
*                               0 - meter entry (token bucket in sip 5_15)
*                               1 - meter config entry (sip5_15 only)
*       convertedFieldNamePtr - pointer to the field name in the actual table
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS   tablePlrMeterFieldInfoGet
(
    IN GT_U8                    devNum,
    IN LION3_PLR_METERING_TABLE_FIELDS_ENT      fieldName,
    OUT GT_U32                  *tableIdPtr,
    OUT GT_U32                  *convertedFieldNamePtr
)
{
    GT_U32  converted_fieldName = 0xFFFFFFFF; /* dummy initialization */

    *tableIdPtr = 0; /* the 'meter' entry */
    *convertedFieldNamePtr = fieldName;

    switch(fieldName)
    {
        case LION3_PLR_METERING_TABLE_FIELDS_LAST_UPDATE_TIME0_E          :
        case LION3_PLR_METERING_TABLE_FIELDS_LAST_UPDATE_TIME1_E          :
        case LION3_PLR_METERING_TABLE_FIELDS_WRAP_AROUND_INDICATOR0_E     :
        case LION3_PLR_METERING_TABLE_FIELDS_WRAP_AROUND_INDICATOR1_E     :
        case LION3_PLR_METERING_TABLE_FIELDS_BUCKET_SIZE0_E               :
        case LION3_PLR_METERING_TABLE_FIELDS_BUCKET_SIZE1_E               :
        case LION3_PLR_METERING_TABLE_FIELDS_RATE_TYPE0_E                 :
        case LION3_PLR_METERING_TABLE_FIELDS_RATE_TYPE1_E                 :
        case LION3_PLR_METERING_TABLE_FIELDS_RATE0_E                      :
        case LION3_PLR_METERING_TABLE_FIELDS_RATE1_E                      :
        case LION3_PLR_METERING_TABLE_FIELDS_MAX_BURST_SIZE0_E            :
        case LION3_PLR_METERING_TABLE_FIELDS_MAX_BURST_SIZE1_E            :
            return GT_OK;
        /* sip5_15 only fields */
        case SIP5_15_PLR_METERING_TABLE_FIELDS_BUCKET_0_PRIORITY_E        :
        case SIP5_15_PLR_METERING_TABLE_FIELDS_BUCKET_1_PRIORITY_E        :
        case SIP5_15_PLR_METERING_TABLE_FIELDS_BUCKET_0_COLOR_E           :
        case SIP5_15_PLR_METERING_TABLE_FIELDS_BUCKET_1_COLOR_E           :
        case SIP5_15_PLR_METERING_TABLE_FIELDS_COUPLING_FLAG_E            :
        case SIP5_15_PLR_METERING_TABLE_FIELDS_COUPLING_FLAG_0_E          :
        case SIP5_15_PLR_METERING_TABLE_FIELDS_EIR_AND_CIR_MAX_INDEX_E    :
            if(! PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
            }
            return GT_OK;

        case LION3_PLR_METERING_TABLE_FIELDS_COLOR_MODE_E                          :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_COLOR_MODE_E                           ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_POLICER_MODE_E                        :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_POLICER_MODE_E                         ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_MG_COUNTERS_SET_EN_E                  :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_MG_COUNTERS_SET_EN_E                   ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_BILLING_PTR_E                         :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_BILLING_PTR_E                          ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_BYTE_OR_PACKET_COUNTING_MODE_E        :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_BYTE_OR_PACKET_COUNTING_MODE_E         ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_PACKET_SIZE_MODE_E                    :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_PACKET_SIZE_MODE_E                     ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_TUNNEL_TERMINATION_PACKET_SIZE_MODE_E :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TUNNEL_TERMINATION_PACKET_SIZE_MODE_E  ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_INCLUDE_LAYER1_OVERHEAD_E             :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INCLUDE_LAYER1_OVERHEAD_E              ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_DSA_TAG_COUNTING_MODE_E               :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_DSA_TAG_COUNTING_MODE_E                ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E         :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_TIMESTAMP_TAG_COUNTING_MODE_E          ;
            break;
        /* ingress only fields*/
        case LION3_PLR_METERING_TABLE_FIELDS_INGRESS_RED_CMD_E                     :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_RED_CMD_E                      ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_INGRESS_YELLOW_CMD_E                  :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_YELLOW_CMD_E                   ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_INGRESS_GREEN_CMD_E                   :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_GREEN_CMD_E                    ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_INGRESS_MODIFY_DSCP_E                 :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_MODIFY_DSCP_E                  ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_INGRESS_MODIFY_UP_E                   :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_MODIFY_UP_E                    ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_INGRESS_QOS_PROFILE_E                 :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_INGRESS_QOS_PROFILE_E                  ;
            break;
        /* egress only fields*/                                                                                           /* egress only fields*/
        case LION3_PLR_METERING_TABLE_FIELDS_EGRESS_REMARK_MODE_E                  :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_REMARK_MODE_E                   ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_EGRESS_DROP_RED_E                     :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_DROP_RED_E                      ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_EXP_E                :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_EXP_E                 ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_DSCP_E               :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_DSCP_E                ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_TC_E                 :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_TC_E                  ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_UP_E                 :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_UP_E                  ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_MODIFY_DP_E                 :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_MODIFY_DP_E                  ;
            break;
        case LION3_PLR_METERING_TABLE_FIELDS_EGRESS_EN_YELLOW_ECN_MARKING_E        :     /* in sip5_15 moved to 'Metering Configuration Entry' */
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_EGRESS_EN_YELLOW_ECN_MARKING_E         ;
            break;

        case SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_MEF_10_3_ENV_SIZE_E___EMULATION_ONLY_E:/*emulation of SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_MEF_10_3_ENV_SIZE_E */
            if(! PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
            }
            converted_fieldName =    SIP5_15_PLR_METERING_CONFIG_TABLE_FIELDS_MEF_10_3_ENV_SIZE_E                    ;
            break;

        default:    /* should not get here ... which field is it ?! */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
    {
        *tableIdPtr = 1;/* 'meter config' table */
        *convertedFieldNamePtr = converted_fieldName;
    }
    else
    {
        /* we already stated , at start of the function:
            *tableIdPtr = 0;
            *convertedFieldNamePtr = fieldName;
        */
    }


    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChTablePlrMeterFieldBuild
*
* DESCRIPTION:
*       This function build the value into needed field in the proper buffer.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - the device number
*       hwMeterArr           - pointer to 'meter' entry (in 5_15 'token bucket' entry)
*       hwMeterConfigArr     - pointer to 'meter config' entry (relevant only to 5_15)
*       fieldName            - name of the field to set
*       value                - value to set
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_NOT_IMPLEMENTED - on non implemented case
*       GT_NOT_APPLICABLE_DEVICE - on non relevant field to the device
*       GT_BAD_PARAM - on value that not fit into field
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   prvCpssDxChTablePlrMeterFieldBuild
(
    IN GT_U8                    devNum,
    IN GT_U32                   hwMeterArr[],
    IN GT_U32                   hwMeterConfigArr[],
    IN LION3_PLR_METERING_TABLE_FIELDS_ENT      fieldName,
    IN GT_U32                   value
)
{
    GT_STATUS   rc;
    GT_U32  converted_fieldName = 0;
    GT_U32  tableId = 0;

    /* convert field into proper table and proper field */
    rc = tablePlrMeterFieldInfoGet(devNum,fieldName,&tableId,&converted_fieldName);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(tableId)
    {
        case 0:
            /* field in 'meter' entry (in 5_15 'token bucket' entry) */
            __LION3_PLR_METER_FIELD_SET_MAC(devNum,hwMeterArr,converted_fieldName,value);
            break;
        case 1:
            /* field in 'meter config' entry (relevant only to 5_15) */
            __SIP5_15_PLR_METER_CONFIG_FIELD_SET_MAC(devNum,hwMeterConfigArr,converted_fieldName,value);
            break;
        default: /* which table is it ?*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChTablePlrMeterFieldParse
*
* DESCRIPTION:
*       This function parse the value for needed field from the proper buffer.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - the device number
*       hwMeterArr           - pointer to 'meter' entry (in 5_15 'token bucket' entry)
*       hwMeterConfigArr     - pointer to 'meter config' entry (relevant only to 5_15)
*       fieldName            - name of the field to set
* OUTPUTS:
*       value                - (pointer to) value to get
*
* RETURNS:
*       GT_OK - on success
*       GT_NOT_IMPLEMENTED - on non implemented case
*       GT_NOT_APPLICABLE_DEVICE - on non relevant field to the device
*       GT_BAD_PARAM - on value that not fit into field
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS   prvCpssDxChTablePlrMeterFieldParse
(
    IN GT_U8                    devNum,
    IN GT_U32                   hwMeterArr[],
    IN GT_U32                   hwMeterConfigArr[],
    IN LION3_PLR_METERING_TABLE_FIELDS_ENT      fieldName,
    IN GT_U32                   *valuePtr
)
{
    GT_STATUS   rc;
    GT_U32  converted_fieldName = 0;
    GT_U32  tableId = 0;
    GT_U32  value;

    /* convert field into proper table and proper field */
    rc = tablePlrMeterFieldInfoGet(devNum,fieldName,&tableId,&converted_fieldName);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(tableId)
    {
        case 0:
            /* field in 'meter' entry (in 5_15 'token bucket' entry) */
            __LION3_PLR_METER_FIELD_GET_MAC(devNum,hwMeterArr,converted_fieldName,value);
            break;
        case 1:
            /* field in 'meter config' entry (relevant only to 5_15) */
            __SIP5_15_PLR_METER_CONFIG_FIELD_GET_MAC(devNum,hwMeterConfigArr,converted_fieldName,value);
            break;
        default: /* which table is it ?*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }

    *valuePtr = value;

    return GT_OK;
}

/*******************************************************************************
*   fieldValueGet
*
* DESCRIPTION:
*        get the value of field (up to 32 bits) that located in any start bit in
*       memory
* INPUTS:
*        startMemPtr - pointer to memory
*        startBit  - start bit of field (0..)
*        numBits   - number of bits of field (0..31)
* OUTPUTS:
*
* COMMENTS:
*        copied from snetFieldValueGet()
*******************************************************************************/
static GT_U32  fieldValueGet(
    IN GT_U32                  *startMemPtr,
    IN GT_U32                  startBit,
    IN GT_U32                  numBits
)
{
    GT_U32  actualStartWord = startBit >> 5;/*/32*/
    GT_U32  actualStartBit  = startBit & 0x1f;/*%32*/
    GT_U32  actualValue;
    GT_U32  workValue;
    GT_U32  numBitsFirst;
    GT_U32  numBitsLeft;

    if (numBits > 32)
    {
        /*ERROR*/
        numBits = 32;
    }

    if ((actualStartBit + numBits) <= 32)
    {
        numBitsFirst = numBits;
        numBitsLeft  = 0;
    }
    else
    {
        numBitsFirst = 32 - actualStartBit;
        numBitsLeft  = numBits - numBitsFirst;
    }

    actualValue = U32_GET_FIELD_MAC(
        startMemPtr[actualStartWord], actualStartBit, numBitsFirst);

    if (numBitsLeft > 0)
    {
        /* retrieve the rest of the value from the second word */
        workValue = U32_GET_FIELD_MAC(
            startMemPtr[actualStartWord + 1], 0, numBitsLeft);

        /* place it to the high bits of the result */
        actualValue |= (workValue << numBitsFirst);
    }

    return actualValue;
}


/*******************************************************************************
*   fieldValueSet
*
* DESCRIPTION:
*        set the value to field (up to 32 bits) that located in any start bit in
*       memory
* INPUTS:
*        startMemPtr - pointer to memory
*        startBit  - start bit of field (0..)
*        numBits   - number of bits of field (0..31)
*        value     - value to write to
* OUTPUTS:
*
* COMMENTS:
*        copied from snetFieldValueSet
*******************************************************************************/
static void  fieldValueSet(
    IN GT_U32                  *startMemPtr,
    IN GT_U32                  startBit,
    IN GT_U32                  numBits,
    IN GT_U32                  value
)
{
    GT_U32  actualStartWord = startBit >> 5;/*/32*/
    GT_U32  actualStartBit  = startBit & 0x1f;/*%32*/
    GT_U32  numBitsFirst;
    GT_U32  numBitsLeft;

    if (numBits > 32)
    {
        /*ERROR*/
        numBits = 32 ;
    }

    if ((actualStartBit + numBits) <= 32)
    {
        numBitsFirst = numBits;
        numBitsLeft  = 0;
    }
    else
    {
        numBitsFirst = 32 - actualStartBit;
        numBitsLeft  = numBits - numBitsFirst;
    }

    U32_SET_FIELD_MASKED_MAC(
        startMemPtr[actualStartWord], actualStartBit, numBitsFirst,value);

    if (numBitsLeft > 0)
    {
        /* place rest of value to the high bits of the result */
        U32_SET_FIELD_MASKED_MAC(
            startMemPtr[actualStartWord + 1], 0, numBitsLeft,(value>>numBitsFirst));
    }

    return ;
}

/*******************************************************************************
*   copyBits
*
* DESCRIPTION:
*        copy bits from source to target.
*        NOTE: function not supports overlap of copied memories.
*              for overlapping use copyBitsInMemory(...)
* INPUTS:
*        targetMemPtr - (pointer to) target memory to write to.
*        targetStartBit  - start bit in the target to write to.
*        sourceMemPtr - (pointer to) source memory to copy from.
*        sourceStartBit  - start bit in the source to copy from.
*        numBits   - number of bits of field (0..31)
*
* OUTPUTS:
*
* COMMENTS:
*
*******************************************************************************/
void  copyBits(
    IN GT_U32                  *targetMemPtr,
    IN GT_U32                  targetStartBit,
    IN GT_U32                  *sourceMemPtr,
    IN GT_U32                  sourceStartBit,
    IN GT_U32                  numBits
)
{
    GT_U32  ii;
    GT_U32  value;
    GT_U32  numOfFull32Bits = numBits / 32;
    GT_U32  numBitsLastWord = numBits % 32;

    /* copy 32 bits at each time */
    for(ii = 0 ; ii < numOfFull32Bits; ii++)
    {
        /* get 32 bits from source */
        value = fieldValueGet(sourceMemPtr,sourceStartBit + (ii*32), 32);

        /* set 32 bits into target */
        fieldValueSet(targetMemPtr,targetStartBit + (ii*32), 32,value);
    }

    if(numBitsLastWord)
    {
        /* get last less than 32 bits from source */
        value = fieldValueGet(sourceMemPtr,sourceStartBit + (ii*32), numBitsLastWord);

        /* set last less than 32 bits into target */
        fieldValueSet(targetMemPtr,targetStartBit + (ii*32), numBitsLastWord,value);
    }

}
/*******************************************************************************
*   copyBitsInMemory
*
* DESCRIPTION:
*        copy bits from source to target in a memory.
*        the function support overlap of copied bits from src to dst
* INPUTS:
*        memPtr - (pointer to) memory to update.
*        targetStartBit  - start bit in the memory to write to.
*        sourceStartBit  - start bit in the memory to copy from.
*        numBits   - number of bits of field (0..31)
*
* OUTPUTS:
*
* COMMENTS:
*
*******************************************************************************/
void  copyBitsInMemory(
    IN GT_U32                  *memPtr,
    IN GT_U32                  targetStartBit,
    IN GT_U32                  sourceStartBit,
    IN GT_U32                  numBits
)
{
    GT_U32  ii;
    GT_U32  value;
    GT_U32  numOfFull32Bits = numBits / 32;
    GT_U32  numBitsLastWord = numBits % 32;
    GT_U32  numBitsWithoutOverLap;
    GT_U32  valueToSaveFromTarget;

    if(targetStartBit >= sourceStartBit &&
       targetStartBit <  (sourceStartBit + numBits))
    {
        /* target is in the range of source */
        numBitsWithoutOverLap = targetStartBit - sourceStartBit;

        if(numBitsWithoutOverLap >= 32)
        {
            /* we not care about the overlap */
            copyBits(memPtr,targetStartBit,memPtr,sourceStartBit,numBits);
            return;
        }
    }
    else
    if(sourceStartBit >= targetStartBit &&
       sourceStartBit <  (targetStartBit + numBits))
    {
        /* source is in the range of target */

        /* we not care about the overlap */
        copyBits(memPtr,targetStartBit,memPtr,sourceStartBit,numBits);
        return;
    }
    else
    {
        /* no overlap */
        copyBits(memPtr,targetStartBit,memPtr,sourceStartBit,numBits);
        return;
    }

    valueToSaveFromTarget = 0;

    /* copy 32 bits at each time */
    for(ii = 0 ; ii < numOfFull32Bits; ii++)
    {
        /* get 32 bits from source */
        value = fieldValueGet(memPtr,sourceStartBit + (ii*32), 32);

        if(ii != 0)
        {
            /* update the value with info saved from the target */
            fieldValueSet(&value,0,numBitsWithoutOverLap,valueToSaveFromTarget);
        }

        /* save bits from target before overriding it */
        valueToSaveFromTarget = fieldValueGet(memPtr,targetStartBit + ((ii+1)*32) - numBitsWithoutOverLap, numBitsWithoutOverLap);

        /* set 32 bits into target */
        fieldValueSet(memPtr,targetStartBit + (ii*32), 32,value);
    }

    if(numBitsLastWord)
    {
        /* get last less than 32 bits from source */
        value = fieldValueGet(memPtr,sourceStartBit + (ii*32), numBitsLastWord);

        if(numOfFull32Bits != 0)
        {
            /* update the value with info saved from the target */
            fieldValueSet(&value,0,numBitsWithoutOverLap,valueToSaveFromTarget);
        }

        /* set last less than 32 bits into target */
        fieldValueSet(memPtr,targetStartBit + (ii*32), numBitsLastWord,value);
    }

}

/*******************************************************************************
* multiEntriesInLineIndexAndGlobalBitConvert
*
* DESCRIPTION:
*       check if the table entry is actually implemented as
*               'several entries' in single 'line'
*
*       NOTE: if the table is not of type 'multi entries' in line , the function
*             not update the INOUT parameters
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndexPtr - (pointer to) entry index , as 'anaware' that there are
*               multi entries in single line.
*       fieldOffsetPtr - (pointer to) offset of the 'field' in the entry ,
*                   as 'anaware' that there are multi entries in single line.
*
* OUTPUTS:
*       entryIndexPtr - (pointer to) LINE index , as 'ware' that there are
*               multi entries in single line.
*       fieldOffsetPtr - (pointer to) offset of the 'field' in the LINE ,
*                   as 'aware' that there are multi entries in single line.
*       numBitsPerEntryPtr - (pointer to) the number of bits that the 'entry' is
*                   using in the LINE.
*                   NOTE: can be NULL
*       numEntriesPerLinePtr - (pointer to) the number of entries in single LINE.
*                   NOTE: can be NULL
*                   NOTE: if value hold FRACTION_INDICATION_CNS meaning that the
*                         number in the lower 31 bits is 'fraction' (1/x) and not (x)
* RETURNS:
*       GT_TRUE  - convert done
*       GT_FALSE - no convert done
*
* COMMENTS:
*
*******************************************************************************/
static GT_BOOL multiEntriesInLineIndexAndGlobalBitConvert
(
    IN GT_U8                   devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    INOUT GT_U32               *entryIndexPtr,
    INOUT GT_U32               *fieldOffsetPtr,
    OUT GT_U32                 *numBitsPerEntryPtr,
    OUT GT_U32                 *numEntriesPerLinePtr
)
{
    GT_U32  numBitsPerEntry;/* number of bits per entry in the LINE */
    GT_U32  numEntriesPerLine;/* number of entries per line */
    GT_U32  fractionValue;/*fraction value*/
    GT_U32  fieldOffset;/* field offset */
    GT_U32  entryIndex; /* entry index */

    switch(tableType)
    {
        case PRV_CPSS_DXCH_LION3_TABLE_IPVX_INGRESS_EPORT_E:
            if(PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
            {
                numBitsPerEntry = 7;
            }
            else
            {
                numBitsPerEntry = 8;
            }
            numEntriesPerLine = 16;
            break;

        case PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_NEXTHOP_AGE_BITS_E:
            /* the API return manipulate only FULL LINE and not the 'NH age bit'
            numBitsPerEntry = 1;
            numEntriesPerLine = 32;
            */
            return GT_FALSE;

        case PRV_CPSS_DXCH_LION3_TABLE_EQ_TX_PROTECTION_E:
            numBitsPerEntry = 1;
            numEntriesPerLine = 32;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_EQ_EPORT_TO_LOC_MAPPING_E:
            numBitsPerEntry = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) ? 13 : 11;
            numEntriesPerLine = 2;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_EQ_PROTECTION_LOC_E:
            numBitsPerEntry = 1;
            numEntriesPerLine = 32;
            break;
        case PRV_CPSS_DXCH_BOBCAT3_TABLE_EQ_TX_PROTECTION_LOC_E:
            numBitsPerEntry = 1;
            numEntriesPerLine = 32;
            break;

        case PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_EPORT_E:
            if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
            {
                numBitsPerEntry = 7;
            }
            else
            {
                numBitsPerEntry = 8;
            }
            numEntriesPerLine = 4;
            break;

        case PRV_CPSS_DXCH_LION3_TABLE_BRIDGE_INGRESS_EPORT_LEARN_PRIO_E:
            numBitsPerEntry = 5;
            numEntriesPerLine = 8;
            break;

        case PRV_CPSS_DXCH_LION3_TABLE_INGRESS_BRIDGE_PORT_MEMBERS_E:
            if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                return GT_FALSE;
            }
            numBitsPerEntry = BMP_256_PORTS_CNS;/*1 bit per port*/
            numEntriesPerLine = 2 | VERTICAL_INDICATION_CNS;
            break;
        case PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E:
            if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                return GT_FALSE;
            }

            numBitsPerEntry = 2*BMP_256_PORTS_CNS;/*2 bit per port*/
            numEntriesPerLine = 2 | VERTICAL_INDICATION_CNS;
            break;
        case PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_QAG_EGRESS_VLAN_E:
            if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                return GT_FALSE;
            }

            /* we have only 16 ports per line ... which is actually 1/16 of the entry */
            /* so this is special entry that takes more than single line */
            numBitsPerEntry = SIP5_20_EGF_QAG_EGRESS_EVLAN_BITS_SIZE_CNS;

            /* indicate that the 16 is actually 'fraction' (1/16) */
            numEntriesPerLine  = FRACTION_INDICATION_CNS | 16;
            break;

        case PRV_CPSS_DXCH_LION3_TABLE_ERMRK_PTP_LOCAL_ACTION_E:
            if(!PRV_CPSS_SIP_5_20_CHECK_MAC(devNum))
            {
                return GT_FALSE;
            }
            /* the actual 5 domains (out of 8 range) are in single line */
            numEntriesPerLine = 8;
            numBitsPerEntry = 6;
            break;

        case PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E:
        case PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E:
        case PRV_CPSS_DXCH_LION_TABLE_TXQ_SOURCE_ID_MEMBERS_E:
        case PRV_CPSS_DXCH_LION3_TABLE_EGRESS_EGF_SHT_NON_TRUNK_MEMBERS2_E:
            if(0 == PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_GET_MAC(devNum,tableType))
            {
                return GT_FALSE;
            }

            /* those 4 tables on 'HALF_SIZE_MODE' implemented as 512 ports
               spread on 2 lines of 256 ports each , with 'half table' between
               the entries */
            numBitsPerEntry = BMP_256_PORTS_CNS;/*1 bit per port*/
            numEntriesPerLine = 2 | FRACTION_HALF_TABLE_INDICATION_CNS;
            break;

        default:
            return GT_FALSE;
    }

    if(PRV_CPSS_DXCH_BMP_OF_TABLES_IN_HALF_SIZE_MODE_GET_MAC(devNum,tableType))
    {
        if(numEntriesPerLine & FRACTION_INDICATION_CNS)
        {
            numEntriesPerLine = (numEntriesPerLine - FRACTION_INDICATION_CNS) * 2;

            /* still need indication */
            numEntriesPerLine |= FRACTION_INDICATION_CNS;
        }
        else
        if(numEntriesPerLine & VERTICAL_INDICATION_CNS)
        {
            numEntriesPerLine = (numEntriesPerLine - VERTICAL_INDICATION_CNS) / 2;
            numBitsPerEntry *= 2;

            if(numEntriesPerLine > 1)
            {
                /* still need indication */
                numEntriesPerLine |= VERTICAL_INDICATION_CNS;
            }
        }
    }

    if(numEntriesPerLine & FRACTION_HALF_TABLE_INDICATION_CNS)
    {
        if((*fieldOffsetPtr) >= numBitsPerEntry)
        {
            PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr = /* pointer to table info */
                            PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);
            GT_U32 maxLines = tableInfoPtr->maxNumOfEntries;

            (*fieldOffsetPtr) -= numBitsPerEntry;
            (*entryIndexPtr)  += (maxLines / 2);
        }
    }
    else
    if(numEntriesPerLine & FRACTION_INDICATION_CNS)
    {
        /* the entry from the caller is taking more than single line */
        fractionValue = numEntriesPerLine - FRACTION_INDICATION_CNS;

        fieldOffset = (*fieldOffsetPtr) % numBitsPerEntry;
        entryIndex  = ((*entryIndexPtr) * fractionValue) + ((*fieldOffsetPtr) / numBitsPerEntry);

        *fieldOffsetPtr = fieldOffset;
        *entryIndexPtr  = entryIndex;
    }
    else if(numEntriesPerLine & VERTICAL_INDICATION_CNS)
    {
        PRV_CPSS_DXCH_TABLES_INFO_STC *tableInfoPtr = /* pointer to table info */
                        PRV_TABLE_INFO_PTR_GET_MAC(devNum,tableType);
        GT_U32 maxLines = tableInfoPtr->maxNumOfEntries;

        numEntriesPerLine -= VERTICAL_INDICATION_CNS;

        if((*entryIndexPtr) >= (maxLines*numEntriesPerLine))
        {
            (*entryIndexPtr) = 0x0FFFFFFF;/* cause error on the caller function */
            *fieldOffsetPtr = 0;
        }
        else
        {
            *fieldOffsetPtr += (((*entryIndexPtr)/maxLines) % numEntriesPerLine) * numBitsPerEntry;
            (*entryIndexPtr) %= maxLines;
        }
    }
    else
    {
        *fieldOffsetPtr += ((*entryIndexPtr) % numEntriesPerLine) * numBitsPerEntry;
        *entryIndexPtr  /= numEntriesPerLine;
    }

    if(numBitsPerEntryPtr)
    {
        *numBitsPerEntryPtr = numBitsPerEntry;
    }

    if(numEntriesPerLinePtr)
    {
        *numEntriesPerLinePtr = numEntriesPerLine;
    }


    return GT_TRUE;

}

