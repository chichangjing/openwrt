/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTti.c
*
* DESCRIPTION:
*       CPSS tunnel termination implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 158$
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>
#include <cpss/dxCh/dxChxGen/tti/private/prvCpssDxChTtiLog.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/tcam/private/prvCpssDxChTcam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/hw/prvCpssDxChLpmHw.h>
#include <cpss/generic/log/prvCpssLog.h>

/* the size of tti macToMe table entry in words */
#define TTI_MAC2ME_SIZE_CNS             6   /* in Bobcat2; Caelum; Bobcat3 the 'entry' is 6 words */

/* the size of tti Adjacency table entry in words */
#define TTI_ADJACENCY_SIZE_CNS          8

/* the size of tti action in words */
#define TTI_ACTION_SIZE_CNS             4

/* the size of tti action type 2 in words */
#define TTI_ACTION_TYPE_2_SIZE_CNS      8

/* maximum value for IPv4 tunnel type field */
#define IPV4_TUNNEL_TYPE_MAX_CNS        7

/* maximum value for MPLS label field */
#define MPLS_LABEL_MAX_CNS              1048575

/* maximum value for MPLS labels field */
#define MPLS_LABELS_MAX_CNS             3

/* maximum value for MPLS protocol above MPLS field */
#define MPLS_PROTOCOL_ABOVE_MAX_CNS     2

/* maximum value for ETH cfi field */
#define ETH_CFI_MAX_CNS                 1

/* maximum value for vidx interface field */
#define VIDX_INTERFACE_MAX_CNS          4095

/* maximum value for QOS profile field */
#define QOS_PROFILE_MAX_CNS             127

/* maximum value for QOS profile field for E-arch devices */
#define QOS_PROFILE_E_ARCH_MAX_CNS       1023


/* maximum value for LTT index field */

/* maximum value for VRF ID field */
#define VRF_ID_MAX_CNS                  4095


/* maximum value for source id field */
#define SOURCE_ID_MAX_CNS               31

/* maximum value for IPCL index */
#define IPCL_INDEX_MAX_CNS              8191

/* maximum value for central counter index */
#define CENTRAL_COUNTER_MAX_CNS         16383

/* maximum value for DP field */
#define DP_MAX_CNS                      1

/* maximum value for MIM ISID field */
#define MIM_ISID_MAX_CNS                16777215

/* maximum value for MIM iRES field */
#define MIM_IRES_MAX_CNS                3

/* maximum value for time to live (TTL) field */
#define TTL_MAX_CNS                     255

/* maximum value for Adjacency index */
#define ADJACENCY_MAX_CNS              255

/* minimum value for cpu code base field */
#define CPU_CODE_BASE_MIN_CNS                192

/* maximum value for PWE3 cpu code base field */
#define PRV_CPSS_DXCH_TTI_PWE3_CPU_CODE_BASE_MAX_CNS    251

/* maximum value for TRILL cpu code base field */
#define PRV_CPSS_DXCH_TTI_TRILL_CPU_CODE_BASE_MAX_CNS   244

/* maximum value for qos mapping table index */
#define QOS_MAPPING_TABLE_INDEX_MAX_CNS     12

/* HW values for offset types in UDBs */
#define UDB_HW_L2_CNS                               0
#define UDB_HW_MPLS_CNS                             1
#define UDB_HW_L3_CNS                               2
#define UDB_HW_L4_CNS                               3
#define UDB_HW_METADATA_CNS                         6

/* maximum value for udb index field */
#define UDB_CONFIG_NUM_WORDS_MAX_CNS   30

#define KEY_UDB_IPV4_TCP_OFFSET_IN_ARRAY_CNS        0
#define KEY_UDB_IPV4_UDP_OFFSET_IN_ARRAY_CNS        1
#define KEY_UDB_MPLS_OFFSET_IN_ARRAY_CNS            2
#define KEY_UDB_IPV4_FRAGMENT_OFFSET_IN_ARRAY_CNS   3
#define KEY_UDB_IPV4_OTHER_OFFSET_IN_ARRAY_CNS      4
#define KEY_UDB_ETHERNET_OTHER_OFFSET_IN_ARRAY_CNS  5
#define KEY_UDB_IPV6_OFFSET_IN_ARRAY_CNS            6
#define KEY_UDB_IPV6_TCP_OFFSET_IN_ARRAY_CNS        7
#define KEY_UDB_IPV6_UDP_OFFSET_IN_ARRAY_CNS        8
#define KEY_UDB_UDE_OFFSET_IN_ARRAY_CNS             9
#define KEY_UDB_UDE1_OFFSET_IN_ARRAY_CNS            10
#define KEY_UDB_UDE2_OFFSET_IN_ARRAY_CNS            11
#define KEY_UDB_UDE3_OFFSET_IN_ARRAY_CNS            12
#define KEY_UDB_UDE4_OFFSET_IN_ARRAY_CNS            13
#define KEY_UDB_UDE5_OFFSET_IN_ARRAY_CNS            14
#define KEY_UDB_UDE6_OFFSET_IN_ARRAY_CNS            15

/*
 * Typedef: struct TTI_ACTION_HW_VALUES_STC
 *
 * Description: A structure to tti action info in HW format between function that
 *              do 'SW format check' + 'convert to HW value' to function that use
 *              HW values to build the actual full HW entry.
 *
 * Fields:
 *    ttPassengerPktType;        -  tt passenger packet type (hw format)
 *    tsPassengerPacketType;     -  ts passenger packet type (hw format)
 *    mplsCommand;               -  mpls command (hw format)
 *    pktCommand;                -  tti packet forwarding command (hw format)
 *    redirectCommand;           -  where to redirect the packet (hw format)
 *    userDefinedCpuCode;        -  user defined cpu code (hw format)
 *    tag0VlanPrecedence;        -  VLAN assignment precedence mode (hw format)
 *    qosPrecedence;             -  QoS precedence mode (hw format)
 *    modifyDSCP;                -  modify DSCP mode (hw format)
 *    modifyTag0Up;              -  modify user priority mode (hw format)
 *    tag1UpCommand;             -  tag 1 up command (hw format)
 *    tag0VlanCmd;               -  tag 0 vlan command applied to packets matching the TTI (hw format)
 *    tag1VlanCmd;               -  tag 1 vlan command applied to packets matching the TTI (hw format)
 *    pcl0OverrideConfigIndex;   -  pcl 0 Override Config Index (hw format)
 *    pcl0_1OverrideConfigIndex; -  pcl 0-1 Override Config Index (hw format)
 *    pcl1OverrideConfigIndex;   -  pcl 1 Override Config Index (hw format)
 *    routerLttPtr;              -  pointer to the Router Lookup Translation Table entry (hw format)
 *    tempAnalyzerIndex;         -  analyzer index application is giving is increased by 1 (hw format)
 *    hwDev;                     -  hw devNum
 *    hwPort;                    -  hw portNum
 *
 *
 */
typedef struct{
    GT_U32      ttPassengerPktType;
    GT_U32      tsPassengerPacketType;
    GT_U32      mplsCommand;
    GT_U32      pktCommand;
    GT_U32      redirectCommand;
    GT_U32      userDefinedCpuCode;
    GT_U32      tag0VlanPrecedence;
    GT_U32      qosPrecedence;
    GT_U32      modifyDSCP;
    GT_U32      modifyTag0Up;
    GT_U32      tag1UpCommand;
    GT_U32      tag0VlanCmd;
    GT_U32      tag1VlanCmd;
    GT_U32      pcl0OverrideConfigIndex;
    GT_U32      pcl0_1OverrideConfigIndex;
    GT_U32      pcl1OverrideConfigIndex;
    GT_U32      routerLttPtr;
    GT_U32      tempAnalyzerIndex;
    GT_HW_DEV_NUM  hwDev;
    GT_PORT_NUM    hwPort;
}TTI_ACTION_HW_VALUES_STC;

/* macro to set value to field of tti ACTION entry format in buffer */
#define LION3_TTI_ACTION_FIELD_SET_MAC(_dev,_hwDataArr,_fieldName,_value)    \
    U32_SET_FIELD_BY_ID_MAC(_hwDataArr,                             \
        PRV_CPSS_DXCH_PP_MAC(_dev)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_TTI_ACTION_E].fieldsInfoPtr,             \
        _fieldName,                                                 \
        _value)

/* macro to Get value to field of tti ACTION entry format in buffer */
#define LION3_TTI_ACTION_FIELD_GET_MAC(_dev,_hwDataArr,_fieldName,_value)    \
    U32_GET_FIELD_BY_ID_MAC(_hwDataArr,                             \
        PRV_CPSS_DXCH_PP_MAC(_dev)->tableFormatInfo[PRV_CPSS_DXCH_TABLE_FORMAT_TTI_ACTION_E].fieldsInfoPtr,             \
        _fieldName,                                                 \
        _value)





/* Converts rule type to rule size value */
#define PRV_CPSS_DXCH_TTI_CONVERT_RULE_TYPE_TO_TCAM_RULE_SIZE_VAL_MAC(_ruleSize, _ruleType)     \
    switch (_ruleType)                                                                      \
    {                                                                                       \
        case CPSS_DXCH_TTI_RULE_UDB_10_E:                                                   \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_10_B_E;                                    \
            break;                                                                          \
        case CPSS_DXCH_TTI_RULE_UDB_20_E:                                                   \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_20_B_E;                                    \
            break;                                                                          \
        case CPSS_DXCH_TTI_RULE_UDB_30_E:                                                   \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_30_B_E;                                    \
            break;                                                                          \
        default:                                                                            \
            _ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_30_B_E;                                    \
            break;                                                                          \
    }

typedef enum {
    LION3_TTI_META_DATA_FIELDS_IS_ARP_E,
    LION3_TTI_META_DATA_FIELDS_IPV4_TUNNELING_PROTOCOL_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_0_E,
    LION3_TTI_META_DATA_FIELDS_IPV4_ARP_DIP_E,
    LION3_TTI_META_DATA_FIELDS_IPV4_ARP_SIP_E,
    LION3_TTI_META_DATA_FIELDS_PROTOCOL_ABOVE_MPLS_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_LABEL_EXISTS_E,
    LION3_TTI_META_DATA_FIELDS_DATA_AFTER_INNER_LABEL_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_LABEL_VALUE_E,
    LION3_TTI_META_DATA_FIELDS_CONTROL_WORD_CHANNEL_TYPE_PROFILE_E,
    LION3_TTI_META_DATA_FIELDS_UP0_B_UP_E,
    LION3_TTI_META_DATA_FIELDS_CFI0_B_DEI__E,
    LION3_TTI_META_DATA_FIELDS_VLAN_TAG0_EXISTS_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_1_E,
    LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_CFI_E,
    LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_VID_7_0_E,
    LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_VID_11_8_E,
    LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_UP_E,
    LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_EXIST_E,
    LION3_TTI_META_DATA_FIELDS_VLAN1_7_0_E,
    LION3_TTI_META_DATA_FIELDS_VLAN1_11_8_E,
    LION3_TTI_META_DATA_FIELDS_UP1_E,
    LION3_TTI_META_DATA_FIELDS_CFI1_E,
    LION3_TTI_META_DATA_FIELDS_VLAN1_EXIST_E,
    LION3_TTI_META_DATA_FIELDS_TAG0_TPID_INDEX_E,
    LION3_TTI_META_DATA_FIELDS_TAG1_TPID_INDEX_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_2_E,
    LION3_TTI_META_DATA_FIELDS_DSA_QOS_PROFILE_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_3_E,
    LION3_TTI_META_DATA_FIELDS_DSA_SOURCE_ID__7_0_E,
    LION3_TTI_META_DATA_FIELDS_DSA_SOURCE_ID__11_8_E,
    LION3_TTI_META_DATA_FIELDS_NUMBER_OF_MPLS_LABELS_14_12_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_4_E,
    LION3_TTI_META_DATA_FIELDS_TRILL_OPTIONS_1ST_NIBBLE_E,
    LION3_TTI_META_DATA_FIELDS_TRILL_OPTIONS_EXIST_E,
    LION3_TTI_META_DATA_FIELDS_TRILL_M_BIT_E,
    LION3_TTI_META_DATA_FIELDS_TRILL_OPTION_LENGTH_EXCEEDED_E,
    LION3_TTI_META_DATA_FIELDS_REP_LAST_E,
    LION3_TTI_META_DATA_FIELDS_IPV6_TUNNELING_PROTOCOL_E,
    LION3_TTI_META_DATA_FIELDS_L3_DATA_WORD0_FIRST_NIBBLE_E,
    LION3_TTI_META_DATA_FIELDS_IPV6_L4_VALID_E,
    LION3_TTI_META_DATA_FIELDS_PCLID_4_0_E,
    LION3_TTI_META_DATA_FIELDS_LOCAL_DEVICE_SOURCE_IS_TRUNK_E,
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCISTRUNK_E,
    LION3_TTI_META_DATA_FIELDS_MAC_TO_ME_E,
    LION3_TTI_META_DATA_FIELDS_PCLID_9_5_E,
    LION3_TTI_META_DATA_FIELDS_SRC_COREID_E,
    LION3_TTI_META_DATA_FIELDS_EVLAN_7_0_E,
    LION3_TTI_META_DATA_FIELDS_EVLAN_12_8_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_5_E,
    LION3_TTI_META_DATA_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_7_0_E,
    LION3_TTI_META_DATA_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_12_8_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_6_E,
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_7_0_E,
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_12_8_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_7_E,
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCDEV_7_0_E,
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCDEV_9_8_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_8_E,
    LION3_TTI_META_DATA_FIELDS_MUST_BE_1_E,

    LION3_TTI_META_DATA_FIELDS___LAST_VALUE___E/* used for array size */
}LION3_TTI_META_DATA_FIELDS_ENT;

/* number of bytes that metadata hold */
#define NUM_OF_META_DATA_BYTES_CNS \
    (((lion3TtiMetaDataFieldsFormat[LION3_TTI_META_DATA_FIELDS___LAST_VALUE___E - 1].startBit +          \
       lion3TtiMetaDataFieldsFormat[LION3_TTI_META_DATA_FIELDS___LAST_VALUE___E - 1].numOfBits) + 7) / 8)


static PRV_CPSS_ENTRY_FORMAT_TABLE_STC lion3TtiMetaDataFieldsFormat[LION3_TTI_META_DATA_FIELDS___LAST_VALUE___E] =
{ /* byte 0 */
    /*LION3_TTI_META_DATA_FIELDS_IS_ARP_E,                                    */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_IPV4_TUNNELING_PROTOCOL_E,                   */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_0_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
/* byte 1..4 */
    /*LION3_TTI_META_DATA_FIELDS_IPV4_ARP_DIP_E,                              */
    PRV_CPSS_STANDARD_FIELD_MAC(32 ),
/* byte 5..8 */
    /*LION3_TTI_META_DATA_FIELDS_IPV4_ARP_SIP_E,                              */
    PRV_CPSS_STANDARD_FIELD_MAC(32 ),
/* byte 9 */
    /*LION3_TTI_META_DATA_FIELDS_PROTOCOL_ABOVE_MPLS_E,                       */
    PRV_CPSS_STANDARD_FIELD_MAC(2  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_LABEL_EXISTS_E,                     */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_DATA_AFTER_INNER_LABEL_E,                    */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
/* byte 10 */
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_LABEL_VALUE_E,                      */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_META_DATA_FIELDS_CONTROL_WORD_CHANNEL_TYPE_PROFILE_E,         */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
/* byte 11 */
    /*LION3_TTI_META_DATA_FIELDS_UP0_B_UP_E,                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_META_DATA_FIELDS_CFI0_B_DEI__E,                               */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_VLAN_TAG0_EXISTS_E,                          */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_1_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(2  ),
    /*LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_CFI_E,                       */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 12 */
    /*LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_VID_7_0_E,               */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 13 */
    /*LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_VID_11_8_E,              */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_UP_E,                    */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_EXIST_E,                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 14 */
    /*LION3_TTI_META_DATA_FIELDS_VLAN1_7_0_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 15 */
    /*LION3_TTI_META_DATA_FIELDS_VLAN1_11_8_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_META_DATA_FIELDS_UP1_E,                                       */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_META_DATA_FIELDS_CFI1_E,                                      */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 16 */
    /*LION3_TTI_META_DATA_FIELDS_VLAN1_EXIST_E,                               */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_TAG0_TPID_INDEX_E,                           */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_META_DATA_FIELDS_TAG1_TPID_INDEX_E,                           */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_2_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 17 */
    /*LION3_TTI_META_DATA_FIELDS_DSA_QOS_PROFILE_E,                           */
    PRV_CPSS_STANDARD_FIELD_MAC(7  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_3_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 18 */
    /*LION3_TTI_META_DATA_FIELDS_DSA_SOURCE_ID__7_0_E,                        */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 19 */
    /*LION3_TTI_META_DATA_FIELDS_DSA_SOURCE_ID__11_8_E,                       */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_META_DATA_FIELDS_NUMBER_OF_MPLS_LABELS_14_12_E,               */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_4_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 20 */
    /*LION3_TTI_META_DATA_FIELDS_TRILL_OPTIONS_1ST_NIBBLE_E,                  */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_META_DATA_FIELDS_TRILL_OPTIONS_EXIST_E,                       */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_TRILL_M_BIT_E,                               */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_TRILL_OPTION_LENGTH_EXCEEDED_E,              */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_REP_LAST_E,                                  */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 21 */
    /*LION3_TTI_META_DATA_FIELDS_IPV6_TUNNELING_PROTOCOL_E,                   */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_META_DATA_FIELDS_L3_DATA_WORD0_FIRST_NIBBLE_E,                */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_META_DATA_FIELDS_IPV6_L4_VALID_E,                             */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 22 */
    /*LION3_TTI_META_DATA_FIELDS_PCLID_4_0_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_META_DATA_FIELDS_LOCAL_DEVICE_SOURCE_IS_TRUNK_E,              */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCISTRUNK_E,                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_META_DATA_FIELDS_MAC_TO_ME_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 23 */
    /*LION3_TTI_META_DATA_FIELDS_PCLID_9_5_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_META_DATA_FIELDS_SRC_COREID_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
/* byte 24 */
    /*LION3_TTI_META_DATA_FIELDS_EVLAN_7_0_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 25 */
    /*LION3_TTI_META_DATA_FIELDS_EVLAN_12_8_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_5_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
/* byte 26 */
    /*LION3_TTI_META_DATA_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_7_0_E,     */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 27 */
    /*LION3_TTI_META_DATA_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_12_8_E,    */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_6_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
/* byte 28 */
    /*LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_7_0_E,      */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 29 */
    /*LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_12_8_E,     */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_7_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
/* byte 30 */
    /*LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCDEV_7_0_E,                        */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 31 */
    /*LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCDEV_9_8_E,                        */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_META_DATA_FIELDS_RESERVED_8_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_META_DATA_FIELDS_MUST_BE_1_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1  )
};
/* macro used for array and it's size as function parameters */
#define PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(arrayName)   \
    arrayName , sizeof(arrayName) / sizeof(arrayName[0])

/* fields from metadata that located at bytes 22..29 */
static LION3_TTI_META_DATA_FIELDS_ENT  lion3LegacyKey_common_metadata22_29_fieldsArr[]={
    LION3_TTI_META_DATA_FIELDS_PCLID_4_0_E,
    LION3_TTI_META_DATA_FIELDS_LOCAL_DEVICE_SOURCE_IS_TRUNK_E,
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCISTRUNK_E,
    LION3_TTI_META_DATA_FIELDS_MAC_TO_ME_E,
    LION3_TTI_META_DATA_FIELDS_PCLID_9_5_E,
    LION3_TTI_META_DATA_FIELDS_SRC_COREID_E,
    LION3_TTI_META_DATA_FIELDS_EVLAN_7_0_E,
    LION3_TTI_META_DATA_FIELDS_EVLAN_12_8_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_5_E,
    LION3_TTI_META_DATA_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_7_0_E,
    LION3_TTI_META_DATA_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_12_8_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_6_E,
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_7_0_E,
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_12_8_E
};

/* fields from metadata that located at bytes 11 */
static LION3_TTI_META_DATA_FIELDS_ENT  lion3LegacyKey_common_metadata11_fieldsArr[]=
{
    LION3_TTI_META_DATA_FIELDS_VLAN_TAG0_EXISTS_E
};

/* fields from metadata that located at bytes 30..31 */
static LION3_TTI_META_DATA_FIELDS_ENT  lion3LegacyKey_common_metadata30_31_fieldsArr[]={
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCDEV_7_0_E,
    LION3_TTI_META_DATA_FIELDS_DSA_TAG_SRCDEV_9_8_E
};

/* fields from metadata that located at bytes 30..31 */
static LION3_TTI_META_DATA_FIELDS_ENT  lion3LegacyKey_ip_metadata0_fieldsArr[]={
    LION3_TTI_META_DATA_FIELDS_IS_ARP_E,
    LION3_TTI_META_DATA_FIELDS_IPV4_TUNNELING_PROTOCOL_E
};

static LION3_TTI_META_DATA_FIELDS_ENT  lion3LegacyKey_mpls_metadata10_fieldsArr[]={
    LION3_TTI_META_DATA_FIELDS_RESERVED_LABEL_VALUE_E,
    LION3_TTI_META_DATA_FIELDS_CONTROL_WORD_CHANNEL_TYPE_PROFILE_E
};

static LION3_TTI_META_DATA_FIELDS_ENT  lion3LegacyKey_mpls_metadata9_fieldsArr[]={
    LION3_TTI_META_DATA_FIELDS_PROTOCOL_ABOVE_MPLS_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_LABEL_EXISTS_E,
    LION3_TTI_META_DATA_FIELDS_DATA_AFTER_INNER_LABEL_E
};

static LION3_TTI_META_DATA_FIELDS_ENT  lion3LegacyKey_ethernet_metadata14_15_fieldsArr[]={
    LION3_TTI_META_DATA_FIELDS_VLAN1_7_0_E,
    LION3_TTI_META_DATA_FIELDS_VLAN1_11_8_E,
    LION3_TTI_META_DATA_FIELDS_UP1_E,
    LION3_TTI_META_DATA_FIELDS_CFI1_E
};

static LION3_TTI_META_DATA_FIELDS_ENT  lion3LegacyKey_ethernet_metadata16_19_fieldsArr[]={
    LION3_TTI_META_DATA_FIELDS_VLAN1_EXIST_E,
    LION3_TTI_META_DATA_FIELDS_TAG0_TPID_INDEX_E,
    LION3_TTI_META_DATA_FIELDS_TAG1_TPID_INDEX_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_2_E,
    LION3_TTI_META_DATA_FIELDS_DSA_QOS_PROFILE_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_3_E,
    LION3_TTI_META_DATA_FIELDS_DSA_SOURCE_ID__7_0_E,
    LION3_TTI_META_DATA_FIELDS_DSA_SOURCE_ID__11_8_E,
    LION3_TTI_META_DATA_FIELDS_RESERVED_4_E
};

static LION3_TTI_META_DATA_FIELDS_ENT  lion3LegacyKey_mim_metadata12_13_fieldsArr[]={
    LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_VID_7_0_E,
    LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_VID_11_8_E,
    LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_UP_E,
    LION3_TTI_META_DATA_FIELDS_PASSENGER_OUTER_TAG_EXIST_E
};




/* the index (byte index) in the legacy key of Bobcat2 and above that should represent the mac SA/DA */
static GT_U32   lion3LegacyMacSaDaIndexInKey = 0;

typedef enum {
    LION3_TTI_BYTES_FROM_PACKET_L2_MAC_DA_E,        /*6 bytes in network order*/  /*L2 offset 0..5 */
    LION3_TTI_BYTES_FROM_PACKET_L2_MAC_SA_E,        /*6 bytes in network order*/  /*L2 offset 6..11 */

    LION3_TTI_BYTES_FROM_PACKET_L3_ETHERTYPE_E,     /*16 bits */ /*L3 offset 0..1 */

    LION3_TTI_BYTES_FROM_PACKET_L3_IP_PROTOCOL_E,   /*8 bits */  /*L3 offset 1 */

    LION3_TTI_BYTES_FROM_PACKET_L3_IPV4_SIP_E,      /*4 bytes in network order*/ /*L3 offset 14..17 */
    LION3_TTI_BYTES_FROM_PACKET_L3_IPV4_DIP_E,      /*4 bytes in network order*/ /*L3 offset 18..21 */

    LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_1_LABEL_E,   /*20 bits */  /*MPLS offset 2..4  from bit 4 in byte4 towards byte 2 */
    LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_1_EXP_E,     /* 3 bits */  /*MPLS offset 4 bit 1*/
    LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_1_S_BIT_E,   /* 1 bit  */  /*MPLS offset 4 bit 0*/

    LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_2_LABEL_E,   /*20 bits */  /*MPLS offset 6..8  from bit 4 in byte8 towards byte 6 */
    LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_2_EXP_E,     /* 3 bits */  /*MPLS offset 8 bit 1*/
    LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_2_S_BIT_E,   /* 1 bit  */  /*MPLS offset 8 bit 0*/

    LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_3_LABEL_E,   /*20 bits */  /*MPLS offset 10..12 from bit 4 in byte12 towards byte 10 */
    LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_3_EXP_E,     /* 3 bits */  /*MPLS offset 12 bit 1*/
    LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_3_S_BIT_E,   /* 1 bit  */  /*MPLS offset 12 bit 0*/

    LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SERVICE_PRI_E,   /* 3 bits */  /*L3 offset 2 bit 5 */
    LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SERVICE_DEI_E,   /* 1 bit  */  /*L3 offset 2 bit 4 */
    LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SERVICE_NO_CUSTOMER_ADDRESS_INDICATION_E,   /* 4 bits */ /*L3 offset 2 bit 0 */

    LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SID_E,          /*24 bits */

    LION3_TTI_BYTES_FROM_PACKET_L4_IP_DATA_WORD_0_E,     /*32 bits */  /*L4 offset 0..3 */
    LION3_TTI_BYTES_FROM_PACKET_L4_IP_DATA_WORD_1_E,     /*32 bits */  /*L4 offset 4..7 */
    LION3_TTI_BYTES_FROM_PACKET_L4_IP_DATA_WORD_2_E,     /*32 bits */  /*L4 offset 8..11 */

    LION3_TTI_BYTES_FROM_PACKET___LAST_VALUE___E/* used for array size */
}LION3_TTI_BYTES_FROM_PACKET_ENT;

typedef enum{
    FLAG_BYTES_ARRAY_E     ,
    FLAG_VALUE_E           ,
    FLAG_VALUE_REVERED_E
}PACKET_PART_INFO_ENT;

typedef struct{
    CPSS_DXCH_TTI_OFFSET_TYPE_ENT   partType;
    GT_U32                          startByteOffset; /*when FLAG_VALUE_REVERED_E indicate 'end byte' */
    GT_U32                          numOfBits;
    GT_U32                          startBitInByte;  /*when FLAG_VALUE_REVERED_E indicate 'end bit' */
    PACKET_PART_INFO_ENT            flag;
}PACKET_PART_INFO_STC;

#define BYTES_TO_BITS_MAC(numBytes) (numBytes * 8)

/* convert labels to start offset from 'mpls-2' : 2,6,10 */
#define MPLS_LABEL_OFFSET_MAC(label)    (2 + (label * 4) )

static PACKET_PART_INFO_STC lion3TtiWellKnownPacketPartsArr[LION3_TTI_BYTES_FROM_PACKET___LAST_VALUE___E] =
{
    /*LION3_TTI_BYTES_FROM_PACKET_L2_MAC_DA_E,        *//*6 bytes in network order  L2 offset 0..5  */
    {CPSS_DXCH_TTI_OFFSET_L2_E , 0 , BYTES_TO_BITS_MAC(6) , 0 , FLAG_BYTES_ARRAY_E},
    /*LION3_TTI_BYTES_FROM_PACKET_L2_MAC_SA_E,        *//*6 bytes in network order  L2 offset 6..11 */
    {CPSS_DXCH_TTI_OFFSET_L2_E , 6 , BYTES_TO_BITS_MAC(6) , 0 , FLAG_BYTES_ARRAY_E},

    /*LION3_TTI_BYTES_FROM_PACKET_L3_ETHERTYPE_E,     *//*16 bits  L3 offset 0..1                   */
    {CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E , 0 , 16 , 7/*end at bit 7 byte 0*/ , FLAG_VALUE_REVERED_E},

    /*LION3_TTI_BYTES_FROM_PACKET_L3_IP_PROTOCOL_E,   *//*8 bits   L3 offset 1                      */
    {CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E , 1 , 8 , 0 , FLAG_VALUE_E},

    /*LION3_TTI_BYTES_FROM_PACKET_L3_IPV4_SIP_E,      *//*4 bytes in network order L3 offset 14..17                  */
    {CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E , 14 , BYTES_TO_BITS_MAC(4) , 0 , FLAG_BYTES_ARRAY_E},
    /*LION3_TTI_BYTES_FROM_PACKET_L3_IPV4_DIP_E,      *//*4 bytes in network order L3 offset 18..21                  */
    {CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E , 18 , BYTES_TO_BITS_MAC(4) , 0 , FLAG_BYTES_ARRAY_E},

    /*LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_1_LABEL_E, *//*  20 bits   MPLS offset 2..4              */
    {CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E , MPLS_LABEL_OFFSET_MAC(0) , 20 , 7/*end at bit 7 byte 2*/ , FLAG_VALUE_REVERED_E},
    /*LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_1_EXP_E,   *//*   3 bits   MPLS offset 4 bit 1           */
    {CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E , MPLS_LABEL_OFFSET_MAC(0)+2 , 3 , 4 , FLAG_VALUE_E},
    /*LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_1_S_BIT_E, *//*   1 bit    MPLS offset 4 bit 0           */
    {CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E , MPLS_LABEL_OFFSET_MAC(0)+2 , 1 , 7 , FLAG_VALUE_E},

    /*LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_2_LABEL_E, *//*  20 bits   MPLS offset 6..8              */
    {CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E , MPLS_LABEL_OFFSET_MAC(1) , 20 , 7/*end at bit 7 byte 6*/ , FLAG_VALUE_REVERED_E},
    /*LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_2_EXP_E,   *//*   3 bits   MPLS offset 8 bit 1           */
    {CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E , MPLS_LABEL_OFFSET_MAC(1)+2 , 3 , 4 , FLAG_VALUE_E},
    /*LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_2_S_BIT_E, *//*   1 bit    MPLS offset 8 bit 0           */
    {CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E , MPLS_LABEL_OFFSET_MAC(1)+2 , 1 , 7 , FLAG_VALUE_E},

    /*LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_3_LABEL_E, *//*  20 bits   MPLS offset 10..12            */
    {CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E , MPLS_LABEL_OFFSET_MAC(2) , 20 , 7/*end at bit 7 byte 10*/ , FLAG_VALUE_REVERED_E},
    /*LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_3_EXP_E,   *//*   3 bits   MPLS offset 12 bit 1          */
    {CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E , MPLS_LABEL_OFFSET_MAC(2)+2 , 3 , 4 , FLAG_VALUE_E},
    /*LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_3_S_BIT_E, *//*   1 bit    MPLS offset 12 bit 0          */
    {CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E , MPLS_LABEL_OFFSET_MAC(2)+2 , 1 , 7 , FLAG_VALUE_E},

    /*LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SERVICE_PRI_E,    *//*3 bits   L3 offset 2 bit 5         */
    {CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E , 2 , 3 , 5 , FLAG_VALUE_E},
    /*LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SERVICE_DEI_E,    *//*1 bit    L3 offset 2 bit 4         */
    {CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E , 2 , 1 , 4 , FLAG_VALUE_E},
    /*LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SERVICE_NO_CUSTOMER_ADDRESS_INDICATION_E,    *//*4 bits  L3 offset 2 bit 0*/
    {CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E , 2 , 4 , 0 , FLAG_VALUE_E},

    /*LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SID_E,          *//*24 bits  L3 offset 3..5 */
    {CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E , 3 , 24 , 7/*end at bit 7 byte 3*/ , FLAG_VALUE_REVERED_E},

    /*LION3_TTI_BYTES_FROM_PACKET_L4_IP_DATA_WORD_0_E,     *//*32 bits   L4 offset 0..3             */
    {CPSS_DXCH_TTI_OFFSET_L4_E , 0 , 32 , 7/*end at bit 7 byte 0*/ , FLAG_VALUE_REVERED_E},
    /*LION3_TTI_BYTES_FROM_PACKET_L4_IP_DATA_WORD_1_E,     *//*32 bits   L4 offset 4..7             */
    {CPSS_DXCH_TTI_OFFSET_L4_E , 4 , 32 , 7/*end at bit 7 byte 4*/ , FLAG_VALUE_REVERED_E},
    /*LION3_TTI_BYTES_FROM_PACKET_L4_IP_DATA_WORD_2_E,     *//*32 bits   L4 offset 8..11            */
    {CPSS_DXCH_TTI_OFFSET_L4_E , 8 , 32 , 7/*end at bit 7 byte 8*/ , FLAG_VALUE_REVERED_E},

};

static GT_STATUS  lion3WellKnownPacketPartsUsedSet
(
    IN GT_U8            devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType,
    IN LION3_TTI_BYTES_FROM_PACKET_ENT  field,
    IN GT_U32              startIndexInKey,
    OUT GT_U32             *numOfUdbsPtr
);

typedef enum{
    /*common to all*/
    LION3_TTI_LEGACY_KEY_FIELDS_PCLID_4_0_E,
    LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_IS_TRUNK_E,
    LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCISTRUNK_E,
    LION3_TTI_LEGACY_KEY_FIELDS_MAC_TO_ME_E,
    LION3_TTI_LEGACY_KEY_FIELDS_PCLID_9_5_E,
    LION3_TTI_LEGACY_KEY_FIELDS_SRC_COREID_E,
    LION3_TTI_LEGACY_KEY_FIELDS_EVLAN_7_0_E,
    LION3_TTI_LEGACY_KEY_FIELDS_EVLAN_12_8_E,
    LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_0_E,
    LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_7_0_E,
    LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_12_8_E,
    LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_1_E,
    LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_7_0_E,
    LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_12_8_E,
    LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_2_E,

    /* mim and trill and ethernet */
    LION3_TTI_LEGACY_KEY_FIELDS_UP0_E,
    LION3_TTI_LEGACY_KEY_FIELDS_CFI0_E,
    LION3_TTI_LEGACY_KEY_FIELDS_VLAN_TAG0_EXISTS_E,
    LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_4_E,
    LION3_TTI_LEGACY_KEY_FIELDS_PASSENGER_OUTER_CFI_E,
    /* end mim and trill */

    LION3_TTI_LEGACY_KEY_FIELDS_MAC_DA_MAC_SA_E,

    LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCDEV_7_0_E,
    LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCDEV_9_8_E,
    LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_3_E,
    LION3_TTI_LEGACY_KEY_FIELDS_MUST_BE_1_E,

    /* ipv4 */
    LION3_TTI_LEGACY_KEY_FIELDS_IPV4_SIP_E,
    LION3_TTI_LEGACY_KEY_FIELDS_IPV4_DIP_E,
    LION3_TTI_LEGACY_KEY_FIELDS_IPV4_IS_ARP_E,
    LION3_TTI_LEGACY_KEY_FIELDS_IPV4_TUNNELING_PROTOCOL_E,

    /* mpls */
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S0_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP0_E    ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL0_E  ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S1_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP1_E    ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL1_E  ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S2_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP2_E    ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL2_E  ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_RESERVED_LABEL_VALUE_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_CONTROL_WORD_CHANNEL_TYPE_PROFILE_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_PROTOCOL_AFTER_MPLS_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_RESERVED_LABEL_EXISTS_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MPLS_DATA_AFTER_INNER_LABEL_E      ,

    /* mim (part2)*/
    LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_SID_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_TAG_OCTET1_BITS_1_4_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_DP_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_UP_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_VID_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_UP_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_EXISTS_E      ,

    /*ethernet*/
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_VLAN1_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_UP1_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_CFI1_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_ETHERTYPE_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_VLAN_TAG1_EXISTS_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_TAG0_TPID_INDEX_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_TAG1_TPID_INDEX_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_RESERVED_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_DSA_QOS_PROFILE_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_RESERVED1_E      ,
    LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_DSA_SOURCE_ID_11_0_E      ,

    LION3_TTI_LEGACY_KEY_FIELDS___LAST_VALUE___E/* used for array size */
}LION3_TTI_LEGACY_KEY_FIELDS_ENT;

static PRV_CPSS_ENTRY_FORMAT_TABLE_STC lion3TtiLegacyKeyFieldsFormat[LION3_TTI_LEGACY_KEY_FIELDS___LAST_VALUE___E] =
{
/* byte 0 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_PCLID_4_0_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_IS_TRUNK_E,              */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCISTRUNK_E,                        */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MAC_TO_ME_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 1 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_PCLID_9_5_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_SRC_COREID_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
/* byte 2 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_EVLAN_7_0_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 3 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_EVLAN_12_8_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_0_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
/* byte 4 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_7_0_E,     */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 5 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_12_8_E,    */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_1_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
/* byte 6 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_7_0_E,      */
    PRV_CPSS_STANDARD_FIELD_MAC(8  ),
/* byte 7 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_12_8_E,     */
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_2_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),

    /* mim and trill and ethernet */
/* byte 8 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_UP0_E,           */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_CFI0_E,        */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_VLAN_TAG0_EXISTS_E,   */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_4_E,         */
    PRV_CPSS_STANDARD_FIELD_MAC(2  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_PASSENGER_OUTER_CFI_E,*/
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),

/* byte 9..14 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_MAC_DA_MAC_SA_E,     */
    PRV_CPSS_STANDARD_FIELD_MAC(48  ),

/* byte 28 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCDEV_7_0_E,                        */
    {(28*8),/*explicit byte 28*/
     8,
     0/*not care when startBit is explicit*/},
/* byte 29 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCDEV_9_8_E,                        */
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_RESERVED_3_E,                                */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MUST_BE_1_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),

/* IPV4 specific fields */
/* byte 15..18 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_IPV4_SIP_E,                                 */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     32,
     LION3_TTI_LEGACY_KEY_FIELDS_MAC_DA_MAC_SA_E},
/* byte 19..22 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_IPV4_DIP_E,                                 */
    PRV_CPSS_STANDARD_FIELD_MAC(32  ),
/* byte 23 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_IPV4_IS_ARP_E,                              */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_IPV4_TUNNELING_PROTOCOL_E,                  */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),

/* MPLS specific fields */
/* byte 15..17 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S0_E      ,*/
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     1,
     LION3_TTI_LEGACY_KEY_FIELDS_MAC_DA_MAC_SA_E},
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP0_E    ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL0_E  ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(20  ),

/* byte 18..20 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S1_E      ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP1_E    ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL1_E  ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(20  ),
/* byte 21..23 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S2_E      ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP2_E    ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL2_E  ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(20  ),
/* byte 24 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_RESERVED_LABEL_VALUE_E      ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_CONTROL_WORD_CHANNEL_TYPE_PROFILE_E      ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
/* byte 25 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_PROTOCOL_AFTER_MPLS_E      ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(2  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_RESERVED_LABEL_EXISTS_E      ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MPLS_DATA_AFTER_INNER_LABEL_E      ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(5  ),


/* MIM specific fields */
/* byte 15..17 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_SID_E      ,                */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     24,
     LION3_TTI_LEGACY_KEY_FIELDS_MAC_DA_MAC_SA_E},
/* byte 18 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_TAG_OCTET1_BITS_1_4_E      ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(4  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_DP_E      ,                 */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_UP_E      ,                 */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),

/* byte 19..20*/
    /*LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_VID_E      ,   */
    PRV_CPSS_STANDARD_FIELD_MAC(12  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_UP_E      ,    */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_EXISTS_E      ,*/
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),

/* ethernet specific fields */
/* byte 15..16 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_VLAN1_E      ,               */
    {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,
     12,
     LION3_TTI_LEGACY_KEY_FIELDS_MAC_DA_MAC_SA_E},
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_UP1_E      ,                 */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_CFI1_E      ,                */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 17..18 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_ETHERTYPE_E      ,           */
    PRV_CPSS_STANDARD_FIELD_MAC(16  ),
/* byte 19 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_VLAN_TAG1_EXISTS_E      ,    */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_TAG0_TPID_INDEX_E      ,     */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_TAG1_TPID_INDEX_E      ,     */
    PRV_CPSS_STANDARD_FIELD_MAC(3  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_RESERVED_E      ,            */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 20 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_DSA_QOS_PROFILE_E      ,     */
    PRV_CPSS_STANDARD_FIELD_MAC(7  ),
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_RESERVED1_E      ,            */
    PRV_CPSS_STANDARD_FIELD_MAC(1  ),
/* byte 21..22 */
    /*LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_DSA_SOURCE_ID_11_0_E      ,  */
    PRV_CPSS_STANDARD_FIELD_MAC(12  )
};
/* fill _hwValue with value from 'fieldName' in hwFormatArray according to format of lion3TtiLegacyKeyFieldsFormat */
#define LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(_hwValue,fieldName)  \
    U32_GET_FIELD_BY_ID_MAC(hwFormatArray,lion3TtiLegacyKeyFieldsFormat,fieldName,_hwValue)

/* fill hwFormatArray with value of _hwValue into field 'fieldName' according to format of lion3TtiLegacyKeyFieldsFormat */
#define LION3_SET_FIELD_TO_LEGACY_KEY_MAC(_hwValue,fieldName)  \
    U32_SET_FIELD_BY_ID_MAC(hwFormatArray,lion3TtiLegacyKeyFieldsFormat,fieldName,_hwValue)



/*******************************************************************************
* ttiConfigCheckCommon
*
* DESCRIPTION:
*       checks rule common fields.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       commonPtr           - type of the tunnel
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiConfigCheckCommon
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_RULE_COMMON_STC       *commonPtr
)
{
    GT_BOOL                 srcIsTrunk      = GT_FALSE;
    GT_U32                  srcPortTrunk    = GT_FALSE;
    GT_BOOL                 isTagged        = GT_FALSE;
    GT_BOOL                 dsaSrcIsTrunk   = GT_FALSE;
    GT_U32                  dsaSrcPortTrunk = 0;
    GT_U32                  dsaSrcDevice    = 0;
    GT_U32                  sourcePortGroupId = 0;

    CPSS_NULL_PTR_CHECK_MAC(commonPtr);

    srcIsTrunk = BOOL2BIT_MAC(commonPtr->srcIsTrunk);
    srcPortTrunk = commonPtr->srcPortTrunk;
    isTagged = BOOL2BIT_MAC(commonPtr->isTagged);
    dsaSrcIsTrunk = BOOL2BIT_MAC(commonPtr->dsaSrcIsTrunk);
    dsaSrcPortTrunk = commonPtr->dsaSrcPortTrunk;
    dsaSrcDevice = commonPtr->dsaSrcDevice;
    sourcePortGroupId = commonPtr->sourcePortGroupId;

    PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC(devNum,commonPtr->vid);

    if (commonPtr->pclId >= BIT_10)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (sourcePortGroupId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PORT_GROUP_ID_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (srcIsTrunk)
    {
        case GT_FALSE: /* src is port */
            PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,srcPortTrunk);
            break;
        case GT_TRUE: /* src is trunk */
            if(srcPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
    }

    if (isTagged)
    {
        switch (dsaSrcIsTrunk)
        {
            case GT_FALSE: /* src is port */
                if(dsaSrcPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                break;
            case GT_TRUE: /* src is trunk */
                if(dsaSrcPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                break;
        }

        if(dsaSrcDevice > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    return GT_OK;
}

/*******************************************************************************
* ttiConfigSetLogic2HwFormat
*
* DESCRIPTION:
*       Write a given logic format to hw format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       ruleType        - TTI rule type
*       logicFormatPtr  - points to tunnel termination configuration in
*                         logic format
* OUTPUTS:
*       hwFormatArray   - the configuration in HW format (6 words).
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiConfigSetLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  CPSS_DXCH_TTI_RULE_UNT             *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    switch (ruleType)
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:

        /* handle word 0 (bits 0-31) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,10,logicFormatPtr->ipv4.common.pclId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],10,8,logicFormatPtr->ipv4.common.srcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],18,1,BOOL2BIT_MAC(logicFormatPtr->ipv4.common.srcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],19,12,logicFormatPtr->ipv4.common.vid);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,BOOL2BIT_MAC(logicFormatPtr->ipv4.common.isTagged));

        /* handle word 1 (bits 32-63) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,8,logicFormatPtr->ipv4.common.mac.arEther[5]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],8,8,logicFormatPtr->ipv4.common.mac.arEther[4]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,8,logicFormatPtr->ipv4.common.mac.arEther[3]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],24,8,logicFormatPtr->ipv4.common.mac.arEther[2]);

        /* handle word 2 (bits 64-95) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,8,logicFormatPtr->ipv4.common.mac.arEther[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],8,8,logicFormatPtr->ipv4.common.mac.arEther[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,7,logicFormatPtr->ipv4.common.dsaSrcDevice);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,3,logicFormatPtr->ipv4.tunneltype);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],26,6,logicFormatPtr->ipv4.srcIp.arIP[3]);

        /* handle word 3 (bits 96-127) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,2,(logicFormatPtr->ipv4.srcIp.arIP[3] >> 6));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],2,8,logicFormatPtr->ipv4.srcIp.arIP[2]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],10,8,logicFormatPtr->ipv4.srcIp.arIP[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],18,8,logicFormatPtr->ipv4.srcIp.arIP[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],26,6,logicFormatPtr->ipv4.destIp.arIP[3]);

        /* handle word 4 (bits 128-159) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],0,2,(logicFormatPtr->ipv4.destIp.arIP[3] >> 6));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],2,8,logicFormatPtr->ipv4.destIp.arIP[2]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],10,8,logicFormatPtr->ipv4.destIp.arIP[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],18,8,logicFormatPtr->ipv4.destIp.arIP[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],26,1,BOOL2BIT_MAC(logicFormatPtr->ipv4.isArp));

        /* handle word 5 (bits 160-191) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],0,7,logicFormatPtr->ipv4.common.dsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],7,1,BOOL2BIT_MAC(logicFormatPtr->ipv4.common.dsaSrcIsTrunk));

        if(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).portGroupId > 2)
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],28,3,logicFormatPtr->ipv4.common.sourcePortGroupId);
        }
        else
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],29,2,logicFormatPtr->ipv4.common.sourcePortGroupId);
        }
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],31,1,1); /* must be 1 */

        break;

    case CPSS_DXCH_TTI_RULE_MPLS_E:

        /* handle word 0 (bits 0-31) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,10,logicFormatPtr->mpls.common.pclId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],10,8,logicFormatPtr->mpls.common.srcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],18,1,BOOL2BIT_MAC(logicFormatPtr->mpls.common.srcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],19,12,logicFormatPtr->mpls.common.vid);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,BOOL2BIT_MAC(logicFormatPtr->mpls.common.isTagged));

        /* handle word 1 (bits 32-63) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,8,logicFormatPtr->mpls.common.mac.arEther[5]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],8,8,logicFormatPtr->mpls.common.mac.arEther[4]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,8,logicFormatPtr->mpls.common.mac.arEther[3]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],24,8,logicFormatPtr->mpls.common.mac.arEther[2]);

        /* handle word 2 (bits 64-95) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,8,logicFormatPtr->mpls.common.mac.arEther[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],8,8,logicFormatPtr->mpls.common.mac.arEther[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,7,logicFormatPtr->mpls.common.dsaSrcDevice);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,3,logicFormatPtr->mpls.exp0);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],26,3,logicFormatPtr->mpls.exp1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],29,3,logicFormatPtr->mpls.label0);

        /* handle word 3 (bits 96-127) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,17,(logicFormatPtr->mpls.label0 >> 3));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],17,15,logicFormatPtr->mpls.label1);

        /* handle word 4 (bits 128-159) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],0,5,(logicFormatPtr->mpls.label1 >> 15));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],5,3,logicFormatPtr->mpls.exp2);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],8,20,logicFormatPtr->mpls.label2);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],28,2,logicFormatPtr->mpls.numOfLabels);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],30,2,logicFormatPtr->mpls.protocolAboveMPLS);

        /* handle word 5 (bits 160-191) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],0,7,logicFormatPtr->mpls.common.dsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],7,1,BOOL2BIT_MAC(logicFormatPtr->mpls.common.dsaSrcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],8,4,logicFormatPtr->mpls.cwFirstNibble);
        if(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).portGroupId > 2)
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],28,3,logicFormatPtr->mpls.common.sourcePortGroupId);
        }
        else
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],29,2,logicFormatPtr->mpls.common.sourcePortGroupId);
        }
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],31,1,1); /* must be 1 */

        break;

    case CPSS_DXCH_TTI_RULE_ETH_E:

        /* handle word 0 (bits 0-31) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,10,logicFormatPtr->eth.common.pclId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],10,8,logicFormatPtr->eth.common.srcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],18,1,BOOL2BIT_MAC(logicFormatPtr->eth.common.srcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],19,12,logicFormatPtr->eth.common.vid);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,BOOL2BIT_MAC(logicFormatPtr->eth.common.isTagged));

        /* handle word 1 (bits 32-63) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,8,logicFormatPtr->eth.common.mac.arEther[5]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],8,8,logicFormatPtr->eth.common.mac.arEther[4]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,8,logicFormatPtr->eth.common.mac.arEther[3]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],24,8,logicFormatPtr->eth.common.mac.arEther[2]);

        /* handle word 2 (bits 64-95) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,8,logicFormatPtr->eth.common.mac.arEther[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],8,8,logicFormatPtr->eth.common.mac.arEther[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,7,logicFormatPtr->eth.common.dsaSrcDevice);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,3,logicFormatPtr->eth.up0);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],26,1,logicFormatPtr->eth.cfi0);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],27,1,BOOL2BIT_MAC(logicFormatPtr->eth.isVlan1Exists));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],28,4,logicFormatPtr->eth.vid1);

        /* handle word 3 (bits 96-127) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,8,(logicFormatPtr->eth.vid1 >> 4));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],8,3,logicFormatPtr->eth.up1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],11,1,logicFormatPtr->eth.cfi1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],12,16,logicFormatPtr->eth.etherType);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],28,1,BOOL2BIT_MAC(logicFormatPtr->eth.macToMe));

        /* handle word 5 (bits 160-191) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],0,7,logicFormatPtr->eth.common.dsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],7,1,BOOL2BIT_MAC(logicFormatPtr->eth.common.dsaSrcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],8,5,logicFormatPtr->eth.srcId);

        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],13,7,logicFormatPtr->eth.dsaQosProfile);

        if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],20,2,logicFormatPtr->eth.eTagGrp);
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).portGroupId > 2)
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],28,3,logicFormatPtr->eth.common.sourcePortGroupId);
        }
        else
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],29,2,logicFormatPtr->eth.common.sourcePortGroupId);
        }
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],31,1,1); /* must be 1 */

        break;

    case CPSS_DXCH_TTI_RULE_MIM_E:

        /* handle word 0 (bits 0-31) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,10,logicFormatPtr->mim.common.pclId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],10,8,logicFormatPtr->mim.common.srcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],18,1,BOOL2BIT_MAC(logicFormatPtr->mim.common.srcIsTrunk));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],19,12,logicFormatPtr->mim.common.vid);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,BOOL2BIT_MAC(logicFormatPtr->mim.common.isTagged));

        /* handle word 1 (bits 32-63) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,8,logicFormatPtr->mim.common.mac.arEther[5]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],8,8,logicFormatPtr->mim.common.mac.arEther[4]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,8,logicFormatPtr->mim.common.mac.arEther[3]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],24,8,logicFormatPtr->mim.common.mac.arEther[2]);

        /* handle word 2 (bits 64-95) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,8,logicFormatPtr->mim.common.mac.arEther[1]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],8,8,logicFormatPtr->mim.common.mac.arEther[0]);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,7,logicFormatPtr->mim.common.dsaSrcDevice);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,3,logicFormatPtr->mim.bUp);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],26,1,logicFormatPtr->mim.bDp);

        /* note bit 27 is reserved */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],28,4,(logicFormatPtr->mim.iSid & 0xf));

        /* handle word 3 (bits 96-127) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,20,(logicFormatPtr->mim.iSid >> 4));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],20,3,logicFormatPtr->mim.iUp);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],23,1,logicFormatPtr->mim.iDp);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],24,2,logicFormatPtr->mim.iRes1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],26,2,logicFormatPtr->mim.iRes2);
        /* handle word 4 (bits 128-159) */

        /* handle word 5 (bits 160-191) */
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],0,7,logicFormatPtr->mim.common.dsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],7,1,BOOL2BIT_MAC(logicFormatPtr->mim.common.dsaSrcIsTrunk));
        if(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).portGroupId > 2)
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],28,3,logicFormatPtr->mim.common.sourcePortGroupId);
        }
        else
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],29,2,logicFormatPtr->mim.common.sourcePortGroupId);
        }
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[5],31,1,1); /* must be 1 */

        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* lion3TtiConfigSetLogic2HwUdbFormat
*
* DESCRIPTION:
*       Write a given logic format to hw format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2;DxCh3; xCat; xCat3; Lion; xCat2; Lion2;
*
* INPUTS:
*       ruleType        - TTI rule type
*       logicFormatPtr  - points to tunnel termination configuration in
*                         logic format
*       isPattern       - this is pattern
* OUTPUTS:
*       hwFormatArray   - the configuration in HW format (8 words).
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lion3TtiConfigSetLogic2HwUdbFormat
(
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  GT_BOOL                             isPattern,
    IN  CPSS_DXCH_TTI_RULE_UNT             *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    /* In order to support backward compatibility for numberOfLabels*/
    /* only "full don't care" (mask 0) and  "full care" (mask 3) are supported.*/
    /* The numberOfLabels pattern and mask are translated to corresponding s-bits.*/
    /* The pattern s-bit decisions are needed to make correct s-bit masks.*/
    static GT_U32 patternS0 = 0;
    static GT_U32 patternS1 = 0;
    static GT_U32 patternS2 = 0;

    GT_U32 s0=0,s1=0,s2=0; /* stop bit for mpls label */
    GT_U32 i,length = 0;
    GT_U32 hwValue;
    GT_U32 tmpValue;
    CPSS_DXCH_TTI_RULE_COMMON_STC *commonPtr = NULL;
    CPSS_DXCH_TTI_IPV4_RULE_STC   *ipv4Ptr;
    CPSS_DXCH_TTI_MPLS_RULE_STC   *mplsPtr;
    CPSS_DXCH_TTI_ETH_RULE_STC    *ethPtr;
    CPSS_DXCH_TTI_MIM_RULE_STC    *mimPtr;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    switch (ruleType)
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:
        commonPtr = &logicFormatPtr->ipv4.common;
        ipv4Ptr = &logicFormatPtr->ipv4;

        hwValue =
            (ipv4Ptr->srcIp.arIP[3] <<  0) |
            (ipv4Ptr->srcIp.arIP[2] <<  8) |
            (ipv4Ptr->srcIp.arIP[1] << 16) |
            (ipv4Ptr->srcIp.arIP[0] << 24) ;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_IPV4_SIP_E);


        hwValue =
            (ipv4Ptr->destIp.arIP[3] <<  0) |
            (ipv4Ptr->destIp.arIP[2] <<  8) |
            (ipv4Ptr->destIp.arIP[1] << 16) |
            (ipv4Ptr->destIp.arIP[0] << 24) ;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_IPV4_DIP_E);

        hwValue = BOOL2BIT_MAC(ipv4Ptr->isArp);
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_IPV4_IS_ARP_E);

        hwValue = ipv4Ptr->tunneltype;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_IPV4_TUNNELING_PROTOCOL_E);

        break;

    case CPSS_DXCH_TTI_RULE_MPLS_E:
        commonPtr = &logicFormatPtr->mpls.common;
        mplsPtr = &logicFormatPtr->mpls;

        if (isPattern)
        {
            switch (logicFormatPtr->mpls.numOfLabels & 0x3)
            {
                case 0:
                    s0 = 1;
                    patternS0 = s0;
                    break;
                case 1:
                    s1 = 1;
                    patternS1 = s1;
                    break;
                case 2:
                    s2 = 1;
                    patternS2 = s2;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                    break;
            }
        }
        else
        {
            switch (logicFormatPtr->mpls.numOfLabels & 0x3)
            {
            case 0:
                s0 = s1 = s2 = 0;
                break;
            case 3:
                if (patternS0 == 1)
                {
                    s0 = 1;
                    s1 = 0;
                    s2 = 0;
                }
                if (patternS1 == 1)
                {
                    s1 = 1;
                    /* in order to prevent match with illegal packet contained more than 1 sbit*/
                    /* s0 mask is set to check s0 bit as well                                  */
                    s0 = 1;
                    s2 = 0;
                }
                if (patternS2 == 1)
                {
                    s2 = 1;
                    /* in order to prevent match with illegal packet contained more than 1 sbit*/
                    /* s0,s1 masks are set to check s0,s1 bits as well                                  */
                    s0 = 1;
                    s1 = 1;
                }
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                break;
            }
            patternS0 = patternS1 = patternS2 = 0;
        }

        hwValue = s0;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S0_E);
        hwValue = mplsPtr->exp0;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP0_E);
        hwValue = mplsPtr->label0;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL0_E);

        hwValue = s1;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S1_E);
        hwValue = mplsPtr->exp1;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP1_E);
        hwValue = mplsPtr->label1;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL1_E);

        hwValue = s2;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S2_E);
        hwValue = mplsPtr->exp2;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP2_E);
        hwValue = mplsPtr->label2;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL2_E);


        hwValue = mplsPtr->reservedLabelValue;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_RESERVED_LABEL_VALUE_E);
        hwValue = mplsPtr->channelTypeProfile;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_CONTROL_WORD_CHANNEL_TYPE_PROFILE_E);
        hwValue = mplsPtr->protocolAboveMPLS;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_PROTOCOL_AFTER_MPLS_E);
        hwValue = BOOL2BIT_MAC(mplsPtr->reservedLabelExist);
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_RESERVED_LABEL_EXISTS_E);
        hwValue = mplsPtr->dataAfterInnerLabel;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_DATA_AFTER_INNER_LABEL_E);


        break;

    case CPSS_DXCH_TTI_RULE_ETH_E:
        commonPtr = &logicFormatPtr->eth.common;
        ethPtr = &logicFormatPtr->eth;

        hwValue = ethPtr->up0;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_UP0_E);
        hwValue = ethPtr->cfi0;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_CFI0_E);
        hwValue = BOOL2BIT_MAC(ethPtr->macToMe);
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MAC_TO_ME_E);
        hwValue = ethPtr->vid1;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_VLAN1_E);
        hwValue = ethPtr->up1;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_UP1_E);
        hwValue = ethPtr->cfi1;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_CFI1_E);
        hwValue = ethPtr->etherType;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_ETHERTYPE_E);
        hwValue = BOOL2BIT_MAC(ethPtr->isVlan1Exists);
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_VLAN_TAG1_EXISTS_E);
        hwValue = ethPtr->tag0TpidIndex;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_TAG0_TPID_INDEX_E);
        hwValue = ethPtr->tag1TpidIndex;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_TAG1_TPID_INDEX_E);

        hwValue = ethPtr->dsaQosProfile;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_DSA_QOS_PROFILE_E);
        hwValue = ethPtr->srcId;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_DSA_SOURCE_ID_11_0_E);

        break;

    case CPSS_DXCH_TTI_RULE_MIM_E:
        commonPtr = &logicFormatPtr->mim.common;
        mimPtr = &logicFormatPtr->mim;

        hwValue = mimPtr->bUp;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_UP0_E);
        hwValue = mimPtr->bDp;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_CFI0_E);
        hwValue = BOOL2BIT_MAC(mimPtr->macToMe);
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MAC_TO_ME_E);
        hwValue = mimPtr->passengerPacketOuterTagDei;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_PASSENGER_OUTER_CFI_E);
        hwValue = mimPtr->iSid;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_SID_E);
        hwValue = (mimPtr->iRes1 & 0x3) |         /*2 bits : 0..1*/
                  ((mimPtr->iRes2 & 0x3) << 2);   /*2 bits : 2..3*/
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_TAG_OCTET1_BITS_1_4_E);
        hwValue = mimPtr->iDp;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_DP_E);
        hwValue = mimPtr->iUp;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_UP_E);
        hwValue = mimPtr->passengerPacketOuterTagVid;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_VID_E);
        hwValue = mimPtr->passengerPacketOuterTagUp;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_UP_E);
        hwValue = BOOL2BIT_MAC(mimPtr->passengerPacketOuterTagExists);
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_EXISTS_E);


        break;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
        length = 10;

        break;

    case CPSS_DXCH_TTI_RULE_UDB_20_E:
        length = 20;

        break;

    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        length = 30;

        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(commonPtr)/*legacy keys only */
    {
        /* convert the common part */

        /* the PCL-ID is split field */
        tmpValue = commonPtr->pclId;
        hwValue = tmpValue & 0x1f;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_PCLID_4_0_E);
        hwValue = tmpValue >> 5;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_PCLID_9_5_E);

        hwValue = BOOL2BIT_MAC(commonPtr->srcIsTrunk);
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_IS_TRUNK_E);

        hwValue = BOOL2BIT_MAC(commonPtr->dsaSrcIsTrunk);
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCISTRUNK_E);

        hwValue = BOOL2BIT_MAC(commonPtr->isTagged);
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_VLAN_TAG0_EXISTS_E);

        hwValue = commonPtr->sourcePortGroupId;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_SRC_COREID_E);

        tmpValue = commonPtr->vid;
        hwValue = tmpValue & 0xff;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_EVLAN_7_0_E);
        hwValue = tmpValue >> 8;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_EVLAN_12_8_E);

        tmpValue = commonPtr->srcPortTrunk;
        hwValue = tmpValue & 0xff;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_7_0_E);
        hwValue = tmpValue >> 8;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_12_8_E);

        tmpValue = commonPtr->dsaSrcPortTrunk;
        hwValue = tmpValue & 0xff;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_7_0_E);
        hwValue = tmpValue >> 8;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_12_8_E);

        MAC_ADDR_SET_FIELD_BY_ID_MAC(hwFormatArray,
            lion3TtiLegacyKeyFieldsFormat,
            LION3_TTI_LEGACY_KEY_FIELDS_MAC_DA_MAC_SA_E,
            &commonPtr->mac.arEther[0]);

        tmpValue = commonPtr->dsaSrcDevice;
        hwValue = tmpValue & 0xff;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCDEV_7_0_E);
        hwValue = tmpValue >> 8;
        LION3_SET_FIELD_TO_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCDEV_9_8_E);

    }
    else
    {
        /* setting UDBs */
        for (i =0 ; i < length; i++)
        {
            U32_SET_FIELD_IN_ENTRY_MAC(hwFormatArray,(8*i),8,logicFormatPtr->udbArray[i]);
        }

        /* if rule is less then 30 bytes mask the irrelevant bytes */
        for (/* continue i */; i < 30; i++)
        {
            U32_SET_FIELD_IN_ENTRY_MAC(hwFormatArray,(8*i),8,0);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* ttiConfigLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tunnel termination configuration from logic format
*       to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                 - device number
*       ruleType               - TTI rule type
*       patternLogicFormatPtr  - points to tunnel termination configuration in
*                                pattern logic format
*       maskLogicFormatPtr     - points to tunnel termination configuration in
*                                mask logic format

*
* OUTPUTS:
*       patternHwFormatArray   - the pattern configuration in HW format (6 words).
*       maskHwFormatArray      - the mask configuration in HW format (6 words).
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiConfigLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  CPSS_DXCH_TTI_RULE_UNT             *patternLogicFormatPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT             *maskLogicFormatPtr,
    OUT GT_U32                             *patternHwFormatArray,
    OUT GT_U32                             *maskHwFormatArray
)
{
    CPSS_DXCH_TTI_RULE_COMMON_STC       *commonPtr = NULL;
    CPSS_DXCH_TTI_RULE_COMMON_STC       *commonMaskPtr = NULL;
    GT_U32   dsaDevLsb;           /* dsa device LSBit */
    GT_U32   dsaDevMaskLsb;       /* dsa device mask LSBit */
    GT_U32   newDsaSrcPortTrunk;  /* dsa trunk/port */
    GT_U32   save_dsaSrcPortTrunk = 0;
    GT_U32   save_PatternDsaSrcDevice = 0;
    GT_U32   save_maskDsaSrcDevice = 0;
    GT_STATUS rc;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(patternLogicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskLogicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(patternHwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(maskHwFormatArray);

     /* zero out hw format */
    cpssOsMemSet(patternHwFormatArray,0,sizeof(GT_U32)*TTI_RULE_SIZE_CNS);
    cpssOsMemSet(maskHwFormatArray,0,sizeof(GT_U32)*TTI_RULE_SIZE_CNS);

    /* check ipv4/mpls/eth configuration pararmeters only for config, not for mask */
    switch (ruleType)
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:
        commonPtr = &(patternLogicFormatPtr->ipv4.common);
        commonMaskPtr = &(maskLogicFormatPtr->ipv4.common);

        if (patternLogicFormatPtr->ipv4.tunneltype > IPV4_TUNNEL_TYPE_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;

    case CPSS_DXCH_TTI_RULE_MPLS_E:
        commonPtr = &(patternLogicFormatPtr->mpls.common);
        commonMaskPtr = &(maskLogicFormatPtr->mpls.common);

        PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(patternLogicFormatPtr->mpls.exp0);
        PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(patternLogicFormatPtr->mpls.exp1);
        PRV_CPSS_DXCH_COS_CHECK_EXP_MAC(patternLogicFormatPtr->mpls.exp2);
        if (patternLogicFormatPtr->mpls.label0 > MPLS_LABEL_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
         /* the check is needed only when we have more then 1 label*/
        if ((patternLogicFormatPtr->mpls.numOfLabels >= 1) && (patternLogicFormatPtr->mpls.label1 > MPLS_LABEL_MAX_CNS))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if ((patternLogicFormatPtr->mpls.numOfLabels >= 2) && (patternLogicFormatPtr->mpls.label2 > MPLS_LABEL_MAX_CNS))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (patternLogicFormatPtr->mpls.numOfLabels > MPLS_LABELS_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (patternLogicFormatPtr->mpls.numOfLabels < MPLS_LABELS_MAX_CNS)
        {
            if (patternLogicFormatPtr->mpls.protocolAboveMPLS > MPLS_PROTOCOL_ABOVE_MAX_CNS)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
          if (patternLogicFormatPtr->mpls.cwFirstNibble >= BIT_4)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if (patternLogicFormatPtr->mpls.reservedLabelExist && patternLogicFormatPtr->mpls.reservedLabelValue >= BIT_4)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            if (patternLogicFormatPtr->mpls.channelTypeProfile >= BIT_4)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            if (patternLogicFormatPtr->mpls.dataAfterInnerLabel >= BIT_5)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        break;

    case CPSS_DXCH_TTI_RULE_ETH_E:
        commonPtr = &(patternLogicFormatPtr->eth.common);
        commonMaskPtr = &(maskLogicFormatPtr->eth.common);

        PRV_CPSS_DXCH_COS_CHECK_UP_MAC(patternLogicFormatPtr->eth.up0);
        if (patternLogicFormatPtr->eth.cfi0 > ETH_CFI_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (patternLogicFormatPtr->eth.isVlan1Exists)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(patternLogicFormatPtr->eth.vid1);
            PRV_CPSS_DXCH_COS_CHECK_UP_MAC(patternLogicFormatPtr->eth.up1);
            if (patternLogicFormatPtr->eth.cfi1 > ETH_CFI_MAX_CNS)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (patternLogicFormatPtr->eth.dsaQosProfile > QOS_PROFILE_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (patternLogicFormatPtr->eth.srcId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(devNum))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if ((patternLogicFormatPtr->eth.tag0TpidIndex >= BIT_3)||
                (patternLogicFormatPtr->eth.tag1TpidIndex >= BIT_3))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            if (patternLogicFormatPtr->eth.eTagGrp >= BIT_2)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        break;

    case CPSS_DXCH_TTI_RULE_MIM_E:
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

        commonPtr = &(patternLogicFormatPtr->mim.common);
        commonMaskPtr = &(maskLogicFormatPtr->mim.common);

        PRV_CPSS_DXCH_COS_CHECK_UP_MAC(patternLogicFormatPtr->mim.bUp);
        if (patternLogicFormatPtr->mim.bDp > DP_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (patternLogicFormatPtr->mim.iSid > MIM_ISID_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        PRV_CPSS_DXCH_COS_CHECK_UP_MAC(patternLogicFormatPtr->mim.iUp);
        if (patternLogicFormatPtr->mim.iDp > DP_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (patternLogicFormatPtr->mim.iRes1 > MIM_IRES_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        if (patternLogicFormatPtr->mim.iRes2 > MIM_IRES_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(patternLogicFormatPtr->mim.passengerPacketOuterTagVid);
            PRV_CPSS_DXCH_COS_CHECK_UP_MAC(patternLogicFormatPtr->mim.passengerPacketOuterTagUp);
            PRV_CPSS_DXCH3_CHECK_CFI_DEI_BIT_MAC(patternLogicFormatPtr->mim.passengerPacketOuterTagDei);
        }

        break;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        /* do nothing */
        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((sysGenGlobalInfo.supportDualDeviceId == GT_TRUE) && (commonPtr != NULL) && (commonMaskPtr != NULL))
    {
        newDsaSrcPortTrunk = commonPtr->dsaSrcPortTrunk;

        /*
         *       A. If <isTrunk> is exact match = ‘port’
         *           Then Cpss do :
         *           < hwDevNum >[0] .Pattern = <port/trunk>[6].Pattern
         *           < hwDevNum >[0] .Mask = <port/trunk>[6].Mask
         *           <port/trunk>[6]. Pattern = 0
         *           <port/trunk>[6]. Mask = no change
         */
        if ((commonMaskPtr->dsaSrcIsTrunk == 1) && (commonPtr->dsaSrcIsTrunk == 0))
        {
            dsaDevLsb     = U32_GET_FIELD_MAC(commonPtr->dsaSrcPortTrunk,6,1);
            dsaDevMaskLsb = U32_GET_FIELD_MAC(commonMaskPtr->dsaSrcPortTrunk,6,1);
            U32_SET_FIELD_MASKED_MAC(newDsaSrcPortTrunk,6,1,0);
        }
        /*
         *       B. Else If <isTrunk> is exact match = ‘trunk’
         *           Then Cpss do :
         *           < hwDevNum >[0] .Mask = ‘don’t care’
         */
        else if ((commonMaskPtr->dsaSrcIsTrunk == 1) && (commonPtr->dsaSrcIsTrunk == 1))
        {
            dsaDevLsb     = 0;
            dsaDevMaskLsb = 0;
        }
        /*
         *      C. Else // <isTrunk> is ‘don’t care’
         *          1) if <port/trunk>[6] is ‘don’t care’
         *              Then Cpss do :
         *              < hwDevNum >[0] .Mask = ‘don’t care’
         *          2) else //exact match – 0 or 1
         *              return bad state //because trunk can come from 2 hemispheres
         */
        else
        {
             if(U32_GET_FIELD_MAC(commonMaskPtr->dsaSrcPortTrunk,6,1) == 1)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            dsaDevLsb     = 0;
            dsaDevMaskLsb = 0;
        }

        /* save values for restore */
        save_dsaSrcPortTrunk = commonPtr->dsaSrcPortTrunk;
        save_PatternDsaSrcDevice = commonPtr->dsaSrcDevice;
        save_maskDsaSrcDevice = commonMaskPtr->dsaSrcDevice;

        /* update the fields */
        U32_SET_FIELD_MASKED_MAC(commonPtr->dsaSrcPortTrunk,0,7,newDsaSrcPortTrunk);
        U32_SET_FIELD_MASKED_MAC(commonPtr->dsaSrcDevice,0,1,dsaDevLsb);
        U32_SET_FIELD_MASKED_MAC(commonMaskPtr->dsaSrcDevice,0,1,dsaDevMaskLsb);
    }

    switch (ruleType)
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:
    case CPSS_DXCH_TTI_RULE_MPLS_E:
    case CPSS_DXCH_TTI_RULE_ETH_E:
    case CPSS_DXCH_TTI_RULE_MIM_E:
        if (ttiConfigCheckCommon(devNum,commonPtr) != GT_OK)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    case CPSS_DXCH_TTI_RULE_UDB_10_E:
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        /* do nothing */
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
    {
        rc = ttiConfigSetLogic2HwFormat(devNum,ruleType,patternLogicFormatPtr,patternHwFormatArray);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = ttiConfigSetLogic2HwFormat(devNum,ruleType,maskLogicFormatPtr,maskHwFormatArray);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* convert 'old keys' as udbs */
        rc = lion3TtiConfigSetLogic2HwUdbFormat(ruleType,GT_TRUE,patternLogicFormatPtr,patternHwFormatArray);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = lion3TtiConfigSetLogic2HwUdbFormat(ruleType,GT_FALSE,maskLogicFormatPtr,maskHwFormatArray);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* restore the values */
    if ((sysGenGlobalInfo.supportDualDeviceId == GT_TRUE) && (commonPtr != NULL) && (commonMaskPtr != NULL))
    {
        commonPtr->dsaSrcPortTrunk = save_dsaSrcPortTrunk;
        commonPtr->dsaSrcDevice = save_PatternDsaSrcDevice;
        commonMaskPtr->dsaSrcDevice = save_maskDsaSrcDevice;
    }

    return GT_OK;
}

/*******************************************************************************
* ttiConfigHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tti configuration from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       ruleType          - TTI rule type
*       hwFormatArray     - tunnel termination configuration in HW format (6 words).
*
* OUTPUTS:
*       logicFormatPtr    - points to tti configuration in logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiConfigHw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_DXCH_TTI_RULE_UNT              *logicFormatPtr
)
{
    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    switch (ruleType)
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:

        logicFormatPtr->ipv4.common.pclId        = U32_GET_FIELD_MAC(hwFormatArray[0],0,10);
        logicFormatPtr->ipv4.common.srcPortTrunk = U32_GET_FIELD_MAC(hwFormatArray[0],10,8);
        logicFormatPtr->ipv4.common.srcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],18,1));
        logicFormatPtr->ipv4.common.vid          = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],19,12);
        logicFormatPtr->ipv4.common.isTagged     = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],31,1));

        logicFormatPtr->ipv4.common.mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],0,8);
        logicFormatPtr->ipv4.common.mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],8,8);
        logicFormatPtr->ipv4.common.mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],16,8);
        logicFormatPtr->ipv4.common.mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],24,8);

        logicFormatPtr->ipv4.common.mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],0,8);
        logicFormatPtr->ipv4.common.mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],8,8);
        logicFormatPtr->ipv4.common.dsaSrcDevice   = U32_GET_FIELD_MAC(hwFormatArray[2],16,7);
        logicFormatPtr->ipv4.tunneltype            = U32_GET_FIELD_MAC(hwFormatArray[2],23,3);
        logicFormatPtr->ipv4.srcIp.arIP[3]         = (GT_U8)(U32_GET_FIELD_MAC(hwFormatArray[2],26,6) |
                                                            (U32_GET_FIELD_MAC(hwFormatArray[3],0,2) << 6));

        logicFormatPtr->ipv4.srcIp.arIP[2]  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[3],2,8);
        logicFormatPtr->ipv4.srcIp.arIP[1]  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[3],10,8);
        logicFormatPtr->ipv4.srcIp.arIP[0]  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[3],18,8);
        logicFormatPtr->ipv4.destIp.arIP[3] = (GT_U8)(U32_GET_FIELD_MAC(hwFormatArray[3],26,6) |
                                                     (U32_GET_FIELD_MAC(hwFormatArray[4],0,2) << 6));

        logicFormatPtr->ipv4.destIp.arIP[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[4],2,8);
        logicFormatPtr->ipv4.destIp.arIP[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[4],10,8);
        logicFormatPtr->ipv4.destIp.arIP[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[4],18,8);
        logicFormatPtr->ipv4.isArp          = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],26,1));

        logicFormatPtr->ipv4.common.dsaSrcPortTrunk = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[5],0,7);

        logicFormatPtr->ipv4.common.dsaSrcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[5],7,1));

        if(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).portGroupId > 2)
        {
            /* To allow support for larger amounts of cores in a single device,
               an additional 1 bits must be added to all TTI keys */
            logicFormatPtr->ipv4.common.sourcePortGroupId = U32_GET_FIELD_MAC(hwFormatArray[5],28,3);
        }
        else
        {
            logicFormatPtr->ipv4.common.sourcePortGroupId = U32_GET_FIELD_MAC(hwFormatArray[5],29,2);
        }
        break;

    case CPSS_DXCH_TTI_RULE_MPLS_E:

        logicFormatPtr->mpls.common.pclId        = U32_GET_FIELD_MAC(hwFormatArray[0],0,10);
        logicFormatPtr->mpls.common.srcPortTrunk = U32_GET_FIELD_MAC(hwFormatArray[0],10,8);
        logicFormatPtr->mpls.common.srcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],18,1));
        logicFormatPtr->mpls.common.vid          = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],19,12);
        logicFormatPtr->mpls.common.isTagged     = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],31,1));

        logicFormatPtr->mpls.common.mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],0,8);
        logicFormatPtr->mpls.common.mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],8,8);
        logicFormatPtr->mpls.common.mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],16,8);
        logicFormatPtr->mpls.common.mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],24,8);

        logicFormatPtr->mpls.common.mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],0,8);
        logicFormatPtr->mpls.common.mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],8,8);
        logicFormatPtr->mpls.common.dsaSrcDevice   = U32_GET_FIELD_MAC(hwFormatArray[2],16,7);
        logicFormatPtr->mpls.exp0                  = U32_GET_FIELD_MAC(hwFormatArray[2],23,3);
        logicFormatPtr->mpls.exp1                  = U32_GET_FIELD_MAC(hwFormatArray[2],26,3);
        logicFormatPtr->mpls.label0                = U32_GET_FIELD_MAC(hwFormatArray[2],29,3) |
                                                     (U32_GET_FIELD_MAC(hwFormatArray[3],0,17) << 3);

        logicFormatPtr->mpls.label1 = U32_GET_FIELD_MAC(hwFormatArray[3],17,15) |
                                      (U32_GET_FIELD_MAC(hwFormatArray[4],0,5) << 15);

        logicFormatPtr->mpls.exp2              = U32_GET_FIELD_MAC(hwFormatArray[4],5,3);
        logicFormatPtr->mpls.label2            = U32_GET_FIELD_MAC(hwFormatArray[4],8,20);
        logicFormatPtr->mpls.numOfLabels       = U32_GET_FIELD_MAC(hwFormatArray[4],28,2);
        logicFormatPtr->mpls.protocolAboveMPLS = U32_GET_FIELD_MAC(hwFormatArray[4],30,2);

        logicFormatPtr->mpls.common.dsaSrcPortTrunk = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[5],0,7);
        logicFormatPtr->mpls.common.dsaSrcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[5],7,1));
        logicFormatPtr->mpls.cwFirstNibble = U32_GET_FIELD_MAC(hwFormatArray[5],8,4);
         if(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).portGroupId > 2)
        {
            /* To allow support for larger amounts of cores in a single device,
               an additional 1 bits must be added to all TTI keys */
            logicFormatPtr->mpls.common.sourcePortGroupId = U32_GET_FIELD_MAC(hwFormatArray[5],28,3);
        }
        else
        {
            logicFormatPtr->mpls.common.sourcePortGroupId = U32_GET_FIELD_MAC(hwFormatArray[5],29,2);
        }

        break;

    case CPSS_DXCH_TTI_RULE_ETH_E:

        logicFormatPtr->eth.common.pclId         = U32_GET_FIELD_MAC(hwFormatArray[0],0,10);
        logicFormatPtr->eth.common.srcPortTrunk = U32_GET_FIELD_MAC(hwFormatArray[0],10,8);
        logicFormatPtr->eth.common.srcIsTrunk    = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],18,1));
        logicFormatPtr->eth.common.vid       = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],19,12);
        logicFormatPtr->eth.common.isTagged      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],31,1));

        logicFormatPtr->eth.common.mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],0,8);
        logicFormatPtr->eth.common.mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],8,8);
        logicFormatPtr->eth.common.mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],16,8);
        logicFormatPtr->eth.common.mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],24,8);

        logicFormatPtr->eth.common.mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],0,8);
        logicFormatPtr->eth.common.mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],8,8);
        logicFormatPtr->eth.common.dsaSrcDevice   = U32_GET_FIELD_MAC(hwFormatArray[2],16,7);

        logicFormatPtr->eth.up0           = U32_GET_FIELD_MAC(hwFormatArray[2],23,3);
        logicFormatPtr->eth.cfi0          = U32_GET_FIELD_MAC(hwFormatArray[2],26,1);
        logicFormatPtr->eth.isVlan1Exists = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],27,1));
        logicFormatPtr->eth.vid1          = (GT_U16)(U32_GET_FIELD_MAC(hwFormatArray[2],28,4) |
                                                    (U32_GET_FIELD_MAC(hwFormatArray[3],0,8) << 4));

        logicFormatPtr->eth.up1       = U32_GET_FIELD_MAC(hwFormatArray[3],8,3);
        logicFormatPtr->eth.cfi1      = U32_GET_FIELD_MAC(hwFormatArray[3],11,1);
        logicFormatPtr->eth.etherType = U32_GET_FIELD_MAC(hwFormatArray[3],12,16);
        logicFormatPtr->eth.macToMe   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],28,1));

        logicFormatPtr->eth.common.dsaSrcPortTrunk = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[5],0,7);
        logicFormatPtr->eth.common.dsaSrcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[5],7,1));
        logicFormatPtr->eth.srcId                  = U32_GET_FIELD_MAC(hwFormatArray[5],8,5);

        logicFormatPtr->eth.dsaQosProfile          = U32_GET_FIELD_MAC(hwFormatArray[5],13,7);

        if(CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            logicFormatPtr->eth.eTagGrp = U32_GET_FIELD_MAC(hwFormatArray[5],20,2);
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).portGroupId > 2)
        {
            /* To allow support for larger amounts of cores in a single device,
               an additional 1 bits must be added to all TTI keys */
            logicFormatPtr->eth.common.sourcePortGroupId = U32_GET_FIELD_MAC(hwFormatArray[5],28,3);

        }
        else
        {
            logicFormatPtr->eth.common.sourcePortGroupId = U32_GET_FIELD_MAC(hwFormatArray[5],29,2);
        }

        break;

    case CPSS_DXCH_TTI_RULE_MIM_E:
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

        /* MIM tunnel type is supported by xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        logicFormatPtr->mim.common.pclId         = U32_GET_FIELD_MAC(hwFormatArray[0],0,10);
        logicFormatPtr->mim.common.srcPortTrunk = U32_GET_FIELD_MAC(hwFormatArray[0],10,8);
        logicFormatPtr->mim.common.srcIsTrunk    = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],18,1));
        logicFormatPtr->mim.common.vid       = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],19,12);
        logicFormatPtr->mim.common.isTagged      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],31,1));

        logicFormatPtr->mim.common.mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],0,8);
        logicFormatPtr->mim.common.mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],8,8);
        logicFormatPtr->mim.common.mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],16,8);
        logicFormatPtr->mim.common.mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],24,8);

        logicFormatPtr->mim.common.mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],0,8);
        logicFormatPtr->mim.common.mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[2],8,8);
        logicFormatPtr->mim.common.dsaSrcDevice   = U32_GET_FIELD_MAC(hwFormatArray[2],16,7);

        logicFormatPtr->mim.bUp   = U32_GET_FIELD_MAC(hwFormatArray[2],23,3);
        logicFormatPtr->mim.bDp   = U32_GET_FIELD_MAC(hwFormatArray[2],26,1);

        logicFormatPtr->mim.iSid  = U32_GET_FIELD_MAC(hwFormatArray[2],28,4) |
                                    (U32_GET_FIELD_MAC(hwFormatArray[3],0,20) << 4);

        logicFormatPtr->mim.iUp   = U32_GET_FIELD_MAC(hwFormatArray[3],20,3);
        logicFormatPtr->mim.iDp   = U32_GET_FIELD_MAC(hwFormatArray[3],23,1);
        logicFormatPtr->mim.iRes1 = U32_GET_FIELD_MAC(hwFormatArray[3],24,2);
        logicFormatPtr->mim.iRes2 = U32_GET_FIELD_MAC(hwFormatArray[3],26,2);

        logicFormatPtr->mim.common.dsaSrcPortTrunk = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[5],0,7);
        logicFormatPtr->mim.common.dsaSrcIsTrunk   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[5],7,1));

        if(PRV_CPSS_DXCH_PP_HW_INFO_FLEX_FIELD_NUM_BITS_SUPPORTED_MAC(devNum).portGroupId > 2)
        {
            /* To allow support for larger amounts of cores in a single device,
               an additional 1 bits must be added to all TTI keys */
            logicFormatPtr->mim.common.sourcePortGroupId = U32_GET_FIELD_MAC(hwFormatArray[5],28,3);
        }
        else
        {
            logicFormatPtr->mim.common.sourcePortGroupId = U32_GET_FIELD_MAC(hwFormatArray[5],29,2);
        }

        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* ttiConfigHwUdb2LogicFormat
*
* DESCRIPTION:
*       Converts a given tti configuration from hardware format
*       to logic format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       ruleType          - TTI rule type
*       hwFormatArray     - tunnel termination configuration in HW format (6 words).
*       isPattern         - this is pattern
*
* OUTPUTS:
*       logicFormatPtr    - points to tti configuration in logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiConfigHwUdb2LogicFormat
(
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  GT_BOOL                             isPattern,
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_DXCH_TTI_RULE_UNT              *logicFormatPtr
)
{

    GT_U32 sbits[3]={0};   /* stop bit for mpls label */

    GT_U32 i;
    GT_U32 length = 0;
    GT_U32 maskedSbitCounter = 0;
    GT_U32 hwValue;
    GT_U32 tmpValue;
    CPSS_DXCH_TTI_RULE_COMMON_STC *commonPtr = NULL;
    CPSS_DXCH_TTI_IPV4_RULE_STC   *ipv4Ptr;
    CPSS_DXCH_TTI_MPLS_RULE_STC   *mplsPtr;
    CPSS_DXCH_TTI_ETH_RULE_STC    *ethPtr;
    CPSS_DXCH_TTI_MIM_RULE_STC    *mimPtr;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    switch (ruleType)
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:
        commonPtr = &logicFormatPtr->ipv4.common;
        ipv4Ptr = &logicFormatPtr->ipv4;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_IPV4_SIP_E);

        ipv4Ptr->srcIp.arIP[3]              = (GT_U8)(hwValue >>  0);
        ipv4Ptr->srcIp.arIP[2]              = (GT_U8)(hwValue >>  8);
        ipv4Ptr->srcIp.arIP[1]              = (GT_U8)(hwValue >> 16);
        ipv4Ptr->srcIp.arIP[0]              = (GT_U8)(hwValue >> 24);

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_IPV4_DIP_E);

        ipv4Ptr->destIp.arIP[3]             = (GT_U8)(hwValue >>  0);
        ipv4Ptr->destIp.arIP[2]             = (GT_U8)(hwValue >>  8);
        ipv4Ptr->destIp.arIP[1]             = (GT_U8)(hwValue >> 16);
        ipv4Ptr->destIp.arIP[0]             = (GT_U8)(hwValue >> 24);

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_IPV4_IS_ARP_E);
        ipv4Ptr->isArp                      = BIT2BOOL_MAC(hwValue);

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_IPV4_TUNNELING_PROTOCOL_E);
        ipv4Ptr->tunneltype                 = hwValue;

        break;

    case CPSS_DXCH_TTI_RULE_MPLS_E:
        commonPtr = &logicFormatPtr->mpls.common;
        mplsPtr = &logicFormatPtr->mpls;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S0_E);
        sbits[0]                            = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP0_E);
        mplsPtr->exp0                       = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL0_E);
        mplsPtr->label0                     = hwValue;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S1_E);
        sbits[1]                            = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP1_E);
        mplsPtr->exp1                       = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL1_E);
        mplsPtr->label1                     = hwValue;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_S2_E);
        sbits[2]                            = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_EXP2_E);
        mplsPtr->exp2                       = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_LABEL2_E);
        mplsPtr->label2                     = hwValue;


        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_RESERVED_LABEL_VALUE_E);
        mplsPtr->reservedLabelValue         = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_CONTROL_WORD_CHANNEL_TYPE_PROFILE_E);
        mplsPtr->channelTypeProfile         = hwValue;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_PROTOCOL_AFTER_MPLS_E);
        mplsPtr->protocolAboveMPLS          = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_RESERVED_LABEL_EXISTS_E);
        mplsPtr->reservedLabelExist         = BIT2BOOL_MAC(hwValue);
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MPLS_DATA_AFTER_INNER_LABEL_E);
        mplsPtr->dataAfterInnerLabel        = hwValue;

        if (isPattern)
        {
            if (sbits[0] == 1)
            {
                mplsPtr->numOfLabels = 0; /* One label*/
            }
            else if (sbits[1] == 1)
            {
                mplsPtr->numOfLabels = 1; /* two labels*/
            }
            else if (sbits[2] == 1)
            {
                mplsPtr->numOfLabels = 2; /* three labels*/
            }
        }
        else /* mask */
        {
            for (i = 0; i < 3; i++)
            {
                if (sbits[i] == 1)
                {
                    maskedSbitCounter++;
                }
            }
            if (maskedSbitCounter == 0)
            {
                mplsPtr->numOfLabels = 0;
            }
            else
            {
                mplsPtr->numOfLabels = 3;
            }
        }

        break;

    case CPSS_DXCH_TTI_RULE_ETH_E:
        commonPtr = &logicFormatPtr->eth.common;
        ethPtr = &logicFormatPtr->eth;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_UP0_E);
        ethPtr->up0                         = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_CFI0_E);
        ethPtr->cfi0                        = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MAC_TO_ME_E);
        ethPtr->macToMe                     = BIT2BOOL_MAC(hwValue);
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_VLAN1_E);
        ethPtr->vid1                        = (GT_U16)hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_UP1_E);
        ethPtr->up1                         = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_CFI1_E);
        ethPtr->cfi1                        = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_ETHERTYPE_E);
        ethPtr->etherType                   = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_VLAN_TAG1_EXISTS_E);
        ethPtr->isVlan1Exists               = BIT2BOOL_MAC(hwValue);
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_TAG0_TPID_INDEX_E);
        ethPtr->tag0TpidIndex               = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_TAG1_TPID_INDEX_E);
        ethPtr->tag1TpidIndex               = hwValue;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_DSA_QOS_PROFILE_E);
        ethPtr->dsaQosProfile               = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_ETHERNET_DSA_SOURCE_ID_11_0_E);
        ethPtr->srcId                       = hwValue;

        break;

    case CPSS_DXCH_TTI_RULE_MIM_E:
        commonPtr = &logicFormatPtr->mim.common;
        mimPtr = &logicFormatPtr->mim;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_UP0_E);
        mimPtr->bUp                         = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_CFI0_E);
        mimPtr->bDp                         = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MAC_TO_ME_E);
        mimPtr->macToMe                     = BIT2BOOL_MAC(hwValue);
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_PASSENGER_OUTER_CFI_E);
        mimPtr->passengerPacketOuterTagDei          = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_SID_E);
        mimPtr->iSid                        = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_TAG_OCTET1_BITS_1_4_E);
        mimPtr->iRes1                       = hwValue & 0x3;/*2 bits : 0..1*/
        mimPtr->iRes2                       = (hwValue >> 2) & 0x3;/*2 bits : 2..3*/
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_DP_E);
        mimPtr->iDp                         = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_I_UP_E);
        mimPtr->iUp                         = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_VID_E);
        mimPtr->passengerPacketOuterTagVid          = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_UP_E);
        mimPtr->passengerPacketOuterTagUp           = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_MIM_PASSENGER_S_TAG_EXISTS_E);
        mimPtr->passengerPacketOuterTagExists       = BIT2BOOL_MAC(hwValue);


        break;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
        length = 10;

        break;

    case CPSS_DXCH_TTI_RULE_UDB_20_E:
        length = 20;

        break;

    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        length = 30;

        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(commonPtr)/*legacy keys only */
    {
        /* convert the common part */

        /* the PCL-ID is split field */
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_PCLID_4_0_E);
        tmpValue = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_PCLID_9_5_E);
        tmpValue += hwValue << 5;

        commonPtr->pclId                = tmpValue;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_IS_TRUNK_E);
        commonPtr->srcIsTrunk           = BIT2BOOL_MAC(hwValue);

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCISTRUNK_E);
        commonPtr->dsaSrcIsTrunk        = BIT2BOOL_MAC(hwValue);

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_VLAN_TAG0_EXISTS_E);
        commonPtr->isTagged             = BIT2BOOL_MAC(hwValue);

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_SRC_COREID_E);
        commonPtr->sourcePortGroupId    = hwValue;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_EVLAN_7_0_E);
        tmpValue = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_EVLAN_12_8_E);
        tmpValue += hwValue << 8;

        commonPtr->vid                  = (GT_U16)tmpValue;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_7_0_E);
        tmpValue = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_LOCAL_DEVICE_SOURCE_EPORT_TRUNKID_12_8_E);
        tmpValue += hwValue << 8;

        commonPtr->srcPortTrunk         = tmpValue;

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_7_0_E);
        tmpValue = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCPORT_SRCEPORT_TRUNKID_12_8_E);
        tmpValue += hwValue << 8;

        commonPtr->dsaSrcPortTrunk      = tmpValue;

        MAC_ADDR_GET_FIELD_BY_ID_MAC(hwFormatArray,
            lion3TtiLegacyKeyFieldsFormat,
            LION3_TTI_LEGACY_KEY_FIELDS_MAC_DA_MAC_SA_E,
            &commonPtr->mac.arEther[0]);

        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCDEV_7_0_E);
        tmpValue = hwValue;
        LION3_GET_FIELD_FROM_LEGACY_KEY_MAC(hwValue,LION3_TTI_LEGACY_KEY_FIELDS_DSA_TAG_SRCDEV_9_8_E);
        tmpValue += hwValue << 8;

        commonPtr->dsaSrcDevice        = tmpValue;

    }
    else
    {
        /* getting UDBs */
        for (i=0; i<length; i++)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(hwFormatArray,(8*i),8,hwValue);
            logicFormatPtr->udbArray[i] = (GT_U8)hwValue;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* ttiConfigLogicFormatForDualDeviceId
*
* DESCRIPTION:
*       Converts a given tti configuration from hardware format
*       to logic format for dual device id.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       ruleType              - TTI rule type
*       patternLogicFormatPtr - points to tunnel termination configuration in
*                               pattern logic format
*       maskLogicFormatPtr    - points to tunnel termination configuration in
*                               mask logic format
*
* OUTPUTS:
*       patternLogicFormatPtr - points to tunnel termination configuration in
*                               pattern logic format
*       maskLogicFormatPtr    - points to tunnel termination configuration in
*                               mask logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameters.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       bit '0' of dsaSrcDevice is always returned as 'don't care'
*
*******************************************************************************/
static GT_STATUS ttiConfigLogicFormatForDualDeviceId
(
    IN     CPSS_DXCH_TTI_RULE_TYPE_ENT        ruleType,
    INOUT  CPSS_DXCH_TTI_RULE_UNT             *patternLogicFormatPtr,
    INOUT  CPSS_DXCH_TTI_RULE_UNT             *maskLogicFormatPtr
)
{
    CPSS_DXCH_TTI_RULE_COMMON_STC   *commonPatternPtr;
    CPSS_DXCH_TTI_RULE_COMMON_STC   *commonMaskPtr;

    if (sysGenGlobalInfo.supportDualDeviceId == GT_FALSE)
    {
        return GT_OK;
    }

    if ((ruleType == CPSS_DXCH_TTI_RULE_UDB_10_E) ||
        (ruleType == CPSS_DXCH_TTI_RULE_UDB_20_E) ||
        (ruleType == CPSS_DXCH_TTI_RULE_UDB_30_E))
    {
        /* nothing to do for UDB rules */
        return GT_OK;
    }

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(patternLogicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskLogicFormatPtr);

    switch (ruleType)
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:
        commonPatternPtr = &patternLogicFormatPtr->ipv4.common;
        commonMaskPtr    = &maskLogicFormatPtr->ipv4.common;
        break;
    case CPSS_DXCH_TTI_RULE_MPLS_E:
        commonPatternPtr = &patternLogicFormatPtr->ipv4.common;
        commonMaskPtr    = &maskLogicFormatPtr->ipv4.common;
        break;
    case CPSS_DXCH_TTI_RULE_ETH_E:
        commonPatternPtr = &patternLogicFormatPtr->ipv4.common;
        commonMaskPtr    = &maskLogicFormatPtr->ipv4.common;
        break;
    case CPSS_DXCH_TTI_RULE_MIM_E:
        commonPatternPtr = &patternLogicFormatPtr->ipv4.common;
        commonMaskPtr    = &maskLogicFormatPtr->ipv4.common;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(commonMaskPtr->dsaSrcIsTrunk == 1 && commonPatternPtr->dsaSrcIsTrunk == 0)
    {
        /*<isTrunk> is exact match = ‘port’*/
        U32_SET_FIELD_MASKED_MAC(commonPatternPtr->dsaSrcPortTrunk,6,1,
                                 U32_GET_FIELD_MAC(commonPatternPtr->dsaSrcDevice,0,1));
        U32_SET_FIELD_MASKED_MAC(commonMaskPtr->dsaSrcPortTrunk,6,1,
                                 U32_GET_FIELD_MAC(commonMaskPtr->dsaSrcDevice,0,1));
    }
    else if(commonMaskPtr->dsaSrcIsTrunk == 1 && commonPatternPtr->dsaSrcIsTrunk == 1)
    {
        /*<isTrunk> is exact match = ‘trunk’*/

        /* no manipulation needed */
    }
    else
    {
        /*<isTrunk> is ‘don’t care’*/

        /* no manipulation needed */
    }

    /* bit '0' of dsaSrcDevice is always returned as 'don't care' */
    U32_SET_FIELD_MASKED_MAC(commonPatternPtr->dsaSrcDevice,0,1,0);
    U32_SET_FIELD_MASKED_MAC(commonMaskPtr->dsaSrcDevice,0,1,0);

    return GT_OK;
}

/*******************************************************************************
* ttiActionType1Logic2HwFormat
*
* DESCRIPTION:
*       Converts a given tti action type 1 from logic format to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*
*       devNum            - device number
*       logicFormatPtr    - points to tti action in logic format
*
* OUTPUTS:
*       hwFormatArray     - tti action in hardware format (4 words)
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiActionType1Logic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_ACTION_STC           *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    GT_U32      pktCommand;         /* tti packet forwarding command (hw format) */
    GT_U32      redirectCommand;    /* where to redirect the packet (hw format) */
    GT_U32      userDefinedCpuCode; /* user defined cpu code (hw format) */
    GT_U32      passengerPktType;   /* passenger packet type (hw format) */
    GT_U32      vlanPrecedence;     /* VLAN assignment precedence mode (hw format) */
    GT_U32      qosPrecedence;      /* QoS precedence mode (hw format) */
    GT_U32      modifyDSCP;         /* modify DSCP mode (hw format) */
    GT_U32      modifyUP;           /* modify user priority mode (hw format) */
    GT_U32      vlanCmd;            /* vlan command applied to packets matching the TTI (hw format) */
    GT_U32      qosTrustMode;       /* qos profile marking (hw format) */
    GT_U32      routerLookupPtr = 0;/* pointer to the Router Lookup Translation Table entry (hw format) */
    GT_STATUS   rc;                 /* return status */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_HW_DEV_NUM  hwDev = 0;
    GT_PORT_NUM    hwPort = 0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E |
                                          CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

     /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TTI_ACTION_SIZE_CNS);

    /* translate tunnel termination action forwarding command parameter */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(pktCommand,logicFormatPtr->command);

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        if ((logicFormatPtr->userDefinedCpuCode >= CPSS_NET_FIRST_USER_DEFINED_E) &&
            (logicFormatPtr->userDefinedCpuCode <= CPSS_NET_LAST_USER_DEFINED_E))
        {
            rc = prvCpssDxChNetIfCpuToDsaCode(logicFormatPtr->userDefinedCpuCode,
                                         (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT*)(GT_UINTPTR)&userDefinedCpuCode);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
        userDefinedCpuCode = 0;

    /* translate QoS precedence from action parameter */
    switch (logicFormatPtr->qosPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        qosPrecedence = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
        qosPrecedence = 0;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (logicFormatPtr->qosProfile > QOS_PROFILE_MAX_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* translate tunnel termination action modify DSCP parameter */
    switch (logicFormatPtr->modifyDscpEnable)
    {
    case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E:
        modifyDSCP = 0;
        break;
    case CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E:
        modifyDSCP = 1;
        break;
    case CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E:
        modifyDSCP = 2;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate tunnel termination action modify User Priority parameter */
    switch (logicFormatPtr->modifyUpEnable)
    {
    case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E:
        modifyUP = 0;
        break;
    case CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E:
        modifyUP = 1;
        break;
    case CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E:
        modifyUP = 2;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate tunnel termination action redirect command parameter */
    switch (logicFormatPtr->redirectCommand)
    {
    case CPSS_DXCH_TTI_NO_REDIRECT_E:
        redirectCommand = 0;
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
        redirectCommand = 1;
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
        redirectCommand = 2;
        routerLookupPtr = logicFormatPtr->routerLookupPtr;
        if (logicFormatPtr->routerLookupPtr > (fineTuningPtr->tableSize.tunnelTerm * 4))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        /* FEr#2018: Limited number of Policy-based routes */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E))
        {
            switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
            {
            /* DxCh3 devices support index 0,1,2,3…(max IP TCAM row) only */
            case CPSS_PP_FAMILY_CHEETAH3_E:
                if (logicFormatPtr->routerLookupPtr >= fineTuningPtr->tableSize.tunnelTerm)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                break;
            default:
                /* Do nothing */
                break;
            }
        }
        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        redirectCommand = 4;
        if (logicFormatPtr->vrfId > VRF_ID_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(logicFormatPtr->redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                logicFormatPtr->egressInterface.devPort.hwDevNum,
                logicFormatPtr->egressInterface.devPort.portNum);
            hwDev = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                logicFormatPtr->egressInterface.devPort.hwDevNum,
                logicFormatPtr->egressInterface.devPort.portNum);
            hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                logicFormatPtr->egressInterface.devPort.hwDevNum,
                logicFormatPtr->egressInterface.devPort.portNum);

            if(hwPort >= BIT_6)
            {
                /* 6 bits for the port number */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
         case CPSS_INTERFACE_TRUNK_E:
            if(logicFormatPtr->egressInterface.trunkId >= BIT_7)
            {
                /* 7 bits for the trunkId */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_INTERFACE_VIDX_E:
            if (logicFormatPtr->egressInterface.vidx > VIDX_INTERFACE_MAX_CNS)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* translate VLAN precedence from action parameter */
    switch (logicFormatPtr->vlanPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        vlanPrecedence = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
        vlanPrecedence = 0;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate VLAN command from action parameter */
    switch (logicFormatPtr->vlanCmd)
    {
    case CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E:
        vlanCmd = 0;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E:
        vlanCmd = 1;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E:
        vlanCmd = 2;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E:
        vlanCmd = 3;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->vlanId);

    if (logicFormatPtr->policerIndex >= BIT_12)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* translate tunnel termination action qos trust mode parameter */
    switch (logicFormatPtr->qosTrustMode)
    {
    case CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E:
        qosTrustMode = 0;
        break;
    case CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_E:
        qosTrustMode = 1;
        break;
    case CPSS_DXCH_TTI_QOS_TRUST_PASS_L3_E:
        qosTrustMode = 2;
        break;
    case CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_L3_E:
        qosTrustMode = 3;
        break;
    case CPSS_DXCH_TTI_QOS_UNTRUST_E:
        qosTrustMode = 4;
        break;
    case CPSS_DXCH_TTI_QOS_TRUST_MPLS_EXP_E:
        qosTrustMode = 5;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate passenger packet type from action parameter */
    switch (logicFormatPtr->passengerPacketType)
    {
    case CPSS_DXCH_TTI_PASSENGER_IPV4_E:
        passengerPktType = 0;
        break;
    case CPSS_DXCH_TTI_PASSENGER_IPV6_E:
        passengerPktType = 1;
        break;
    case CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E:
        passengerPktType = 2;
        break;
    case CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E:
        passengerPktType = 3;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->up);

    if (logicFormatPtr->sourceId >SOURCE_ID_MAX_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (logicFormatPtr->tunnelStartPtr >= fineTuningPtr->tableSize.tunnelStart)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (logicFormatPtr->counterIndex >= BIT_14)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* handle word 0 (bits 0-31) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,3,pktCommand);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],3,8,userDefinedCpuCode);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],11,1,BOOL2BIT_MAC(logicFormatPtr->mirrorToIngressAnalyzerEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],12,1,qosPrecedence);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],14,7,logicFormatPtr->qosProfile);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],21,2,modifyDSCP);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],23,2,modifyUP);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],25,1,BOOL2BIT_MAC(logicFormatPtr->activateCounter));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],26,6,(logicFormatPtr->counterIndex));
    /* handle word 1 (bits 32-63) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,2,redirectCommand); /* sets only 2 bits. the last bit handled later */

    switch (logicFormatPtr->redirectCommand)
    {
    case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,1,0);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],3,6,hwPort);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],9,5,hwDev);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],14,1,0);
            break;
        case CPSS_INTERFACE_TRUNK_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,1,1);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],3,7,logicFormatPtr->egressInterface.trunkId);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],14,1,0);
            break;
        case CPSS_INTERFACE_VIDX_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,12,logicFormatPtr->egressInterface.vidx);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],14,1,1);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,13,routerLookupPtr);
        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],2,12,logicFormatPtr->vrfId);
        break;
    default:
        /* do noting */
        break;
    }

    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],15,1,vlanPrecedence);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],16,1,BOOL2BIT_MAC(logicFormatPtr->nestedVlanEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],17,2,vlanCmd);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],19,12,logicFormatPtr->vlanId);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],31,1,BOOL2BIT_MAC(logicFormatPtr->remapDSCP));

    /* handle word 2 (bits 64-95) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,1,BOOL2BIT_MAC(logicFormatPtr->bindToPolicer));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],1,8,logicFormatPtr->policerIndex);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],9,1,BOOL2BIT_MAC(logicFormatPtr->vntl2Echo));
    if (redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],10,1,BOOL2BIT_MAC(logicFormatPtr->tunnelStart));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],11,10,logicFormatPtr->tunnelStartPtr);
    }
    else
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],10,1,0);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],11,10,0);
    }
    /* bit 21 is reserved */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],22,3,qosTrustMode);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],25,2,passengerPktType);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],27,1,BOOL2BIT_MAC(logicFormatPtr->copyTtlFromTunnelHeader));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],28,1,BOOL2BIT_MAC(logicFormatPtr->bridgeBypass));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],29,3,logicFormatPtr->up);

    /* handle word 3 (bits 96-127) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,1,BOOL2BIT_MAC(logicFormatPtr->tunnelTerminate));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],1,1,BOOL2BIT_MAC(logicFormatPtr->actionStop));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],2,8,(logicFormatPtr->counterIndex >> 6));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],10,4,(logicFormatPtr->policerIndex >> 8));
    if (redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],14,3,(logicFormatPtr->tunnelStartPtr >> 10));
    }
    else
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],14,3,0);
    }
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],17,1,BOOL2BIT_MAC(logicFormatPtr->sourceIdSetEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],18,5,logicFormatPtr->sourceId);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],23,1,(redirectCommand >> 2));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],24,2,(routerLookupPtr >> 13));

    return GT_OK;
}

/*******************************************************************************
* ttiActionType1Hw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tti action type 1 from hardware format to
*       logic format.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       hwFormatArray     - tti action in hardware format (4 words)
*
* OUTPUTS:
*       logicFormatPtr    - points to tti action in logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiActionType1Hw2LogicFormat
(
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_DXCH_TTI_ACTION_STC            *logicFormatPtr
)
{
    GT_STATUS rc = GT_OK;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(logicFormatPtr->command, U32_GET_FIELD_MAC(hwFormatArray[0],0,3));

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        rc = prvCpssDxChNetIfDsaToCpuCode((U32_GET_FIELD_MAC(hwFormatArray[0],3,8)),
                                   &logicFormatPtr->userDefinedCpuCode);
        if(rc != GT_OK)
            return rc;
    }
    else
        logicFormatPtr->userDefinedCpuCode = 0;

    logicFormatPtr->mirrorToIngressAnalyzerEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],11,1));


    switch (U32_GET_FIELD_MAC(hwFormatArray[0],12,1))
    {
        case 0:
            logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            break;
        case 1:
            logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->qosProfile = U32_GET_FIELD_MAC(hwFormatArray[0],14,7);

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],21,2))
    {
        case 0:
            logicFormatPtr->modifyDscpEnable = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
            break;
        case 1:
            logicFormatPtr->modifyDscpEnable = CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E;
            break;
        case 2:
            logicFormatPtr->modifyDscpEnable = CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],23,2))
    {
        case 0:
            logicFormatPtr->modifyUpEnable = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E;
            break;
        case 1:
            logicFormatPtr->modifyUpEnable = CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E;
            break;
        case 2:
            logicFormatPtr->modifyUpEnable = CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],25,1))
    {
        case 0:
            logicFormatPtr->activateCounter = GT_FALSE;
            break;
        case 1:
            logicFormatPtr->activateCounter = GT_TRUE;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->counterIndex = U32_GET_FIELD_MAC(hwFormatArray[0],26,6) |
                                  (U32_GET_FIELD_MAC(hwFormatArray[3],2,8) << 6);
             ;
    switch (U32_GET_FIELD_MAC(hwFormatArray[1],0,2) |
            (U32_GET_FIELD_MAC(hwFormatArray[3],23,1) << 2))
    {
    case 0:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_NO_REDIRECT_E;
        break;
    case 1:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;
        if (U32_GET_FIELD_MAC(hwFormatArray[1],14,1) == 0) /* unicast */
        {
            if (U32_GET_FIELD_MAC(hwFormatArray[1],2,1) == 0) /* port */
            {
                logicFormatPtr->egressInterface.devPort.portNum = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],3,6);
                logicFormatPtr->egressInterface.devPort.hwDevNum  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[1],9,5);
                logicFormatPtr->egressInterface.type = CPSS_INTERFACE_PORT_E;
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(logicFormatPtr->egressInterface));
            }
            else /* trunk */
            {
                logicFormatPtr->egressInterface.trunkId = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[1],3,7);
                logicFormatPtr->egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
            }
        }
        else /* multicast */
        {
            logicFormatPtr->egressInterface.vidx = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[1],2,12);
            logicFormatPtr->egressInterface.type = CPSS_INTERFACE_VIDX_E;
        }
        break;
    case 2:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;
        logicFormatPtr->routerLookupPtr = U32_GET_FIELD_MAC(hwFormatArray[1],2,13) |
                                         (U32_GET_FIELD_MAC(hwFormatArray[3],24,2) << 13);
        break;
    case 4:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_VRF_ID_ASSIGN_E;
        logicFormatPtr->vrfId           = U32_GET_FIELD_MAC(hwFormatArray[1],2,12);
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[1],15,1))
    {
        case 0:
            logicFormatPtr->vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            break;
        case 1:
            logicFormatPtr->vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->nestedVlanEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],16,1));

    switch (U32_GET_FIELD_MAC(hwFormatArray[1],17,2))
    {
        case 0:
            logicFormatPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E;
            break;
        case 1:
            logicFormatPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;
        case 2:
            logicFormatPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E;
            break;
        case 3:
            logicFormatPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->vlanId         = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[1],19,12);
    logicFormatPtr->remapDSCP      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],31,1));
    logicFormatPtr->bindToPolicer  = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],0,1));
    logicFormatPtr->policerIndex   = U32_GET_FIELD_MAC(hwFormatArray[2],1,8) |
                                    (U32_GET_FIELD_MAC(hwFormatArray[3],10,4) << 8);
    logicFormatPtr->vntl2Echo      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],9,1));
    logicFormatPtr->tunnelStart    = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],10,1));
    logicFormatPtr->tunnelStartPtr = U32_GET_FIELD_MAC(hwFormatArray[2],11,10) |
                                    (U32_GET_FIELD_MAC(hwFormatArray[3],14,3) << 10);

    switch (U32_GET_FIELD_MAC(hwFormatArray[2],22,3))
    {
        case 0:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E;
            break;
         case 1:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_E;
            break;
         case 2:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_PASS_L3_E;
            break;
         case 3:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_L3_E;
            break;
         case 4:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_UNTRUST_E;
            break;
         case 5:
            logicFormatPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_MPLS_EXP_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[2],25,2))
    {
         case 0:
            logicFormatPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV4_E;
            break;
         case 1:
            logicFormatPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV6_E;
            break;
         case 2:
            logicFormatPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E;
            break;
         case 3:
            logicFormatPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E;
            break;
         default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->copyTtlFromTunnelHeader = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],27,1));
    logicFormatPtr->bridgeBypass             = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],28,1));
    logicFormatPtr->up                       = U32_GET_FIELD_MAC(hwFormatArray[2],29,3);
    logicFormatPtr->tunnelTerminate         = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],0,1));
    logicFormatPtr->actionStop           = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],1,1));
    logicFormatPtr->sourceIdSetEnable       = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],17,1));
    logicFormatPtr->sourceId             = U32_GET_FIELD_MAC(hwFormatArray[3],18,5);

    return GT_OK;
}


/*******************************************************************************
* ttiActionXcatLogic2HwFormat
*
* DESCRIPTION:
*       Xcat and above (not sip5) Converts a given tti action (type 2) from logic format to hardware format.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*
*       devNum            - device number
*       logicFormatPtr    - points to tti action in logic format
*       hwValuesInfoPtr   - the parameters that already set in hw values during
*                           'parameter check'  --> this to avoid duplication of
*                           large code of checking parameters
*                           (shared with xCat,xCat3,Lion,xcat2,lion2 devices).
*
* OUTPUTS:
*       hwFormatArray     - tti action in hardware format (8 words)
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiActionXcatLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_ACTION_2_STC         *logicFormatPtr,
    IN  TTI_ACTION_HW_VALUES_STC           *hwValuesInfoPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    GT_U32 tempRouterLttPtr; /* pointer to the Router Lookup Translation Table entry (hw format) */
    GT_U32      pwTagMode = 0;                  /* Pseudowire Tag Mode (hw format) */
    GT_U32      unknownSaCommand = 0;           /* unkown macSA command (hw format) */
    GT_U32      sourceMeshId = 0;               /* source mesh ID (hw format) */
    GT_U32      cwBasedPw = 0;                  /* Pseudowire has control word (hw format) */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwValuesInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch(logicFormatPtr->pwTagMode)
        {
        case CPSS_DXCH_TTI_PW_TAG_DISABLED_MODE_E:
            pwTagMode = 0;
            break;
        case CPSS_DXCH_TTI_PW_TAG_RAW_PW_MODE_E:
            pwTagMode = 1;
            break;
        case CPSS_DXCH_TTI_PW_TAG_TAGGED_PW_MODE_E:
            pwTagMode = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (logicFormatPtr->sourceMeshIdSetEnable == GT_TRUE)
        {
            if (logicFormatPtr->sourceMeshId >= BIT_2)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            sourceMeshId = logicFormatPtr->sourceMeshId;
        }

        cwBasedPw = ((logicFormatPtr->cwBasedPw == GT_TRUE) ? 1 : 0);

        if (logicFormatPtr->unknownSaCommandEnable == GT_TRUE)
        {
            switch (logicFormatPtr->unknownSaCommand)
            {
            case CPSS_PACKET_CMD_FORWARD_E:
                unknownSaCommand = 0;
                break;
            case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
                unknownSaCommand = 1;
                break;
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                unknownSaCommand = 2;
                break;
            case CPSS_PACKET_CMD_DROP_HARD_E:
                unknownSaCommand = 3;
                break;
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                unknownSaCommand = 4;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        else /* logicFormatPtr->unknownSaCommandEnable == GT_FALSE */
        {
            unknownSaCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
        }
    }

    /* handle word 0 (bits 0-31) and also some of word 1 (bits 32-63) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],0,3,hwValuesInfoPtr->pktCommand);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],3,8,hwValuesInfoPtr->userDefinedCpuCode);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],11,1,BOOL2BIT_MAC(logicFormatPtr->mirrorToIngressAnalyzerEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],12,3,hwValuesInfoPtr->redirectCommand);

    switch (logicFormatPtr->redirectCommand)
    {
    case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,1,0); /* target is not trunk */
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],16,6,hwValuesInfoPtr->hwPort);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],22,5,hwValuesInfoPtr->hwDev);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],27,1,0); /* target not VIDX */
            break;
        case CPSS_INTERFACE_TRUNK_E:
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,1,1); /* target is trunk */
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],16,7,logicFormatPtr->egressInterface.trunkId);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],27,1,0); /* target not VIDX */
            break;
        case CPSS_INTERFACE_VIDX_E:
        case CPSS_INTERFACE_VID_E:
            if(logicFormatPtr->egressInterface.type == CPSS_INTERFACE_VID_E)
            {
                U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,12,0xFFF);/* special VIDX */
            }
            else
            {
                U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,12,logicFormatPtr->egressInterface.vidx);
            }
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],27,1,1); /* target is VIDX */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],28,1,BOOL2BIT_MAC(logicFormatPtr->tunnelStart));
        if (logicFormatPtr->tunnelStart == GT_TRUE)
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],29,3,logicFormatPtr->tunnelStartPtr & 0x7);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,10,logicFormatPtr->tunnelStartPtr >> 3);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],10,1,hwValuesInfoPtr->tsPassengerPacketType);
        }
        else
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],29,3,logicFormatPtr->arpPtr & 0x7);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,11,logicFormatPtr->arpPtr >> 3);
        }
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],11,1,BOOL2BIT_MAC(logicFormatPtr->vntl2Echo));
        if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) &&
            (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E))
        {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],22,1,BOOL2BIT_MAC(logicFormatPtr->modifyMacDa));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],23,1,BOOL2BIT_MAC(logicFormatPtr->modifyMacSa));
        }
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
       /* LTT entry is row based however in this field the LTT is treated as column based.
          Bits [11:0] indicate row while bits [14:13] indicate column, bit 12 is not used.
          The Formula for translating the LTT entry to column based is as follow:
          [11:0] << 2 + [14:13]   (Note: bit 12 not used). */
        tempRouterLttPtr = (((hwValuesInfoPtr->routerLttPtr & 0x3FFC) >> 2) | ((hwValuesInfoPtr->routerLttPtr & 0x3) << 13));
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,15,tempRouterLttPtr);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,logicFormatPtr->iPclConfigIndex & 0x1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,12,logicFormatPtr->iPclConfigIndex >> 1);
        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],22,1,BOOL2BIT_MAC(logicFormatPtr->ResetSrcPortGroupId));
        }
        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,12,logicFormatPtr->vrfId);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,logicFormatPtr->iPclConfigIndex & 0x1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,12,logicFormatPtr->iPclConfigIndex >> 1);
        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],22,1,BOOL2BIT_MAC(logicFormatPtr->ResetSrcPortGroupId));
        }
        break;

    case CPSS_DXCH_TTI_LOGICAL_PORT_ASSIGN_E:
        /* redirect to logical port is supported only for Lion2-B0 and above; xCat C0; xCat3*/
        if((PRV_CPSS_DXCH_LION2_B0_AND_ABOVE_CHECK_MAC(devNum)) ||
           (PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE))
        {
            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                logicFormatPtr->egressInterface.devPort.hwDevNum,
                logicFormatPtr->egressInterface.devPort.portNum);
            hwValuesInfoPtr->hwDev = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                logicFormatPtr->egressInterface.devPort.hwDevNum,
                logicFormatPtr->egressInterface.devPort.portNum);
            hwValuesInfoPtr->hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                logicFormatPtr->egressInterface.devPort.hwDevNum,
                logicFormatPtr->egressInterface.devPort.portNum);


            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,1,0); /* target is not trunk */
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],16,6,hwValuesInfoPtr->hwPort);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],22,5,hwValuesInfoPtr->hwDev);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],27,1,0); /* target not VIDX */

            U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,logicFormatPtr->iPclConfigIndex & 0x1);
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,12,logicFormatPtr->iPclConfigIndex >> 1);
            if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
            {
                U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],22,1,BOOL2BIT_MAC(logicFormatPtr->ResetSrcPortGroupId));
            }

            if (PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE)
            {

                U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],27,2,pwTagMode);
                U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],29,2,sourceMeshId);
            }
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        break;

    case CPSS_DXCH_TTI_NO_REDIRECT_E:
        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],15,1,BOOL2BIT_MAC(logicFormatPtr->multiPortGroupTtiEnable));
        }
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[0],31,1,logicFormatPtr->iPclConfigIndex & 0x1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],0,12,logicFormatPtr->iPclConfigIndex >> 1);
        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],22,1,BOOL2BIT_MAC(logicFormatPtr->ResetSrcPortGroupId));
        }
    default:
        /* do noting */
        break;
    }

    /* handle word 1 (bits 32-63) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],12,1,BOOL2BIT_MAC(logicFormatPtr->bindToCentralCounter));
    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],13,12,logicFormatPtr->centralCounterIndex);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],25,1,cwBasedPw);
    }
    else
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],13,14,logicFormatPtr->centralCounterIndex);
    }
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],27,1,BOOL2BIT_MAC(logicFormatPtr->bindToPolicerMeter));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],28,1,BOOL2BIT_MAC(logicFormatPtr->bindToPolicer));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],29,3,logicFormatPtr->policerIndex & 0x7);

    /* handle word 2 (bits 64-95) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],0,9,logicFormatPtr->policerIndex >> 3);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],9,1,BOOL2BIT_MAC(logicFormatPtr->sourceIdSetEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],10,5,logicFormatPtr->sourceId);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],15,1,BOOL2BIT_MAC(logicFormatPtr->actionStop));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],16,1,BOOL2BIT_MAC(logicFormatPtr->bridgeBypass));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],17,1,BOOL2BIT_MAC(logicFormatPtr->ingressPipeBypass));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],18,1,hwValuesInfoPtr->pcl1OverrideConfigIndex);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],19,1,hwValuesInfoPtr->pcl0_1OverrideConfigIndex);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],20,1,hwValuesInfoPtr->pcl0OverrideConfigIndex);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],21,1,hwValuesInfoPtr->tag0VlanPrecedence);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],22,1,BOOL2BIT_MAC(logicFormatPtr->nestedVlanEnable));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],23,2,hwValuesInfoPtr->tag0VlanCmd);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[2],25,7,logicFormatPtr->tag0VlanId & 0x7F);

    /* handle word 3 (bits 96-127) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],0,5,logicFormatPtr->tag0VlanId >> 7);
    if (logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],5,1,hwValuesInfoPtr->tag1VlanCmd);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],6,12,logicFormatPtr->tag1VlanId);
    }
    else
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],5,8,logicFormatPtr->mplsTtl);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],13,1,BOOL2BIT_MAC(logicFormatPtr->enableDecrementTtl));
    }
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],18,1,hwValuesInfoPtr->qosPrecedence);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],19,7,logicFormatPtr->qosProfile);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],26,2,hwValuesInfoPtr->modifyDSCP);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],28,2,hwValuesInfoPtr->modifyTag0Up);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],30,1,BOOL2BIT_MAC(logicFormatPtr->keepPreviousQoS));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[3],31,1,BOOL2BIT_MAC(logicFormatPtr->trustUp));

    /* handle word 4 (bits 128-159) */
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],0,1,BOOL2BIT_MAC(logicFormatPtr->trustDscp));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],1,1,BOOL2BIT_MAC(logicFormatPtr->trustExp));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],2,1,BOOL2BIT_MAC(logicFormatPtr->remapDSCP));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],3,3,logicFormatPtr->tag0Up);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],6,2,hwValuesInfoPtr->tag1UpCommand);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],8,3,logicFormatPtr->tag1Up);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],11,2,hwValuesInfoPtr->ttPassengerPktType);
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],13,1,BOOL2BIT_MAC(logicFormatPtr->copyTtlExpFromTunnelHeader));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],14,1,BOOL2BIT_MAC(logicFormatPtr->tunnelTerminate));
    U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],15,3,hwValuesInfoPtr->mplsCommand);
    if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[1],26,1,unknownSaCommand & 0x1);
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],18,2,unknownSaCommand >> 1);
    }
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) &&
            (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        U32_SET_FIELD_MASKED_MAC(hwFormatArray[4],18,4,logicFormatPtr->hashMaskIndex & 0xF);
    }


    return GT_OK;
}

/*******************************************************************************
* ttiActionLion3Logic2HwFormat
*
* DESCRIPTION:
*       Bobcat2; Caelum; Bobcat3 Converts a given tti action (type 2) from logic format to hardware format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*
*       devNum            - device number
*       logicFormatPtr    - points to tti action in logic format
*       hwValuesInfoPtr   - the parameters that already set in hw values during
*                           'parameter check'  --> this to avoid duplication of
*                           large code of checking parameters
*
* OUTPUTS:
*       hwFormatArray     - tti action in hardware format (8 words)
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiActionLion3Logic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_ACTION_2_STC         *logicFormatPtr,
    IN  TTI_ACTION_HW_VALUES_STC           *hwValuesInfoPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    GT_U32      value;
    GT_BOOL     enablePtr;
    GT_STATUS   rc;
    GT_U32      lpmIndex;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwValuesInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    /********************************************************/
    /* check params and translate params to hardware format */
    /********************************************************/

    /****************************************/
    /* convert SW to HW  to hardware format */
    /****************************************/

    value = hwValuesInfoPtr->pktCommand;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_COMMAND_E,
        value);

    value = hwValuesInfoPtr->userDefinedCpuCode;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_CPU_CODE_E,
        value);

    if(logicFormatPtr->mirrorToIngressAnalyzerEnable == GT_TRUE)
    {
        value = logicFormatPtr->mirrorToIngressAnalyzerIndex + 1;
        if(0 == CHECK_BITS_DATA_RANGE_MAC(logicFormatPtr->mirrorToIngressAnalyzerIndex + 1, 3))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        value = 0;
    }
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MIRROR_TO_ANALYZER_PORT_E,
        value);

    value = hwValuesInfoPtr->redirectCommand;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E,
        value);


    switch (logicFormatPtr->redirectCommand)
    {
    case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            /* target is not trunk */
            value = 0;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_IS_TRUNK_E,
                value);

            value = hwValuesInfoPtr->hwPort;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EPORT_NUMBER_E,
                value);

            value = hwValuesInfoPtr->hwDev;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_DEVICE_NUMBER_E,
                value);

            /* target not VIDX */
            value = 0;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_USE_EVIDX_E,
                value);

            break;
        case CPSS_INTERFACE_TRUNK_E:
            /* target is trunk */
            value = 1;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_IS_TRUNK_E,
                value);

            value = logicFormatPtr->egressInterface.trunkId;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TRUNK_NUMBER_E,
                value);

            /* target not VIDX */
            value = 0;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_USE_EVIDX_E,
                value);
            break;
        case CPSS_INTERFACE_VIDX_E:
        case CPSS_INTERFACE_VID_E:
            if(logicFormatPtr->egressInterface.type == CPSS_INTERFACE_VID_E)
            {
                value = 0xfff;
            }
            else
            {
                value = logicFormatPtr->egressInterface.vidx;
            }

            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EVIDX_E,
                value);

            /* target is VIDX */
            value = 1;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_USE_EVIDX_E,
                value);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        value = BOOL2BIT_MAC(logicFormatPtr->tunnelStart);
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_E,
            value);


        if (logicFormatPtr->tunnelStart == GT_TRUE)
        {
            value = logicFormatPtr->tunnelStartPtr;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_INDEX_E,
                value);

            value = hwValuesInfoPtr->tsPassengerPacketType;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_PASSENGER_TYPE_E,
                value);
        }
        else
        {
            value = logicFormatPtr->arpPtr;
            LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_ARP_POINTER_E,
                value);
        }

        value = BOOL2BIT_MAC(logicFormatPtr->vntl2Echo);
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_VNT_L2_ECHO_E,
            value);

        value = BOOL2BIT_MAC(logicFormatPtr->modifyMacDa);
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_DA_E,
            value);

        value = BOOL2BIT_MAC(logicFormatPtr->modifyMacSa);
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_SA_E,
            value);

        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
        rc = prvCpssDxChLpmLeafEntryCalcLpmHwIndexFromSwIndex(devNum,hwValuesInfoPtr->routerLttPtr,&lpmIndex);
        if (rc != GT_OK)
        {
            return rc;
        }
        value = lpmIndex - PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.indexForPbr;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_ROUTER_ROUTER_LTT_INDEX_E,
            value);

        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        value = logicFormatPtr->vrfId;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN_VRF_ID_VRF_ID_E,
            value);
        break;
    case CPSS_DXCH_TTI_NO_REDIRECT_E:
        /*logicFormatPtr->multiPortGroupTtiEnable --> not applicable for Bobcat2; Caelum; Bobcat3 ! */

        /* set flow id field*/
        value = logicFormatPtr->flowId;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                                LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_FLOW_ID_E,
                                value);

        if (logicFormatPtr->iPclUdbConfigTableEnable == GT_TRUE)
        {
            switch (logicFormatPtr->iPclUdbConfigTableIndex)
            {
                case CPSS_DXCH_PCL_PACKET_TYPE_UDE_E:
                    value = 1;
                    break;
                case CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E:
                    value = 2;
                    break;
                case CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E:
                    value = 3;
                    break;
                case CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E:
                    value = 4;
                    break;
                case CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E:
                    value = 5;
                    break;
                case CPSS_DXCH_PCL_PACKET_TYPE_UDE5_E:
                    value = 6;
                    break;
                case CPSS_DXCH_PCL_PACKET_TYPE_UDE6_E:
                    value = 7;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            value = 0;
        }

        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
                                LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_IPCL_UDB_CONFIGURATION_TABLE_UDE_INDEX_E,
                                value);
        break;
    default:
        /* do noting */
        break;
    }

    if(logicFormatPtr->redirectCommand != CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        value = hwValuesInfoPtr->pcl1OverrideConfigIndex;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY2_LOOKUP_MODE_E,
            value);

        value = hwValuesInfoPtr->pcl0_1OverrideConfigIndex;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY1_LOOKUP_MODE_E,
            value);

        value = hwValuesInfoPtr->pcl0OverrideConfigIndex;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY0_LOOKUP_MODE_E,
            value);

        value = logicFormatPtr->iPclConfigIndex;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_IPCL_PROFILE_INDEX_E,
            value);

        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            /* no such field any more logicFormatPtr->ResetSrcPortGroupId*/
        }
    }

    value = BOOL2BIT_MAC(logicFormatPtr->bindToCentralCounter);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_CNC_COUNTER_E,
        value);


    value = logicFormatPtr->centralCounterIndex;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX_E,
        value);


    value = BOOL2BIT_MAC(logicFormatPtr->bindToPolicerMeter);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_METER_E,
        value);


    value = BOOL2BIT_MAC(logicFormatPtr->bindToPolicer);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_COUNTER_E,
        value);

    value = logicFormatPtr->policerIndex;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_POLICER_INDEX_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->sourceIdSetEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_ID_SET_ENABLE_E,
        value);

    value = logicFormatPtr->sourceId;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_ID_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->actionStop);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_ACTION_STOP_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->bridgeBypass);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_BRIDGE_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->ingressPipeBypass);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_INGRESS_PIPE_E,
        value);

    value = hwValuesInfoPtr->tag0VlanPrecedence;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN_PRECEDENCE_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->nestedVlanEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_NESTED_VLAN_E,
        value);

    value = hwValuesInfoPtr->tag0VlanCmd;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_EVID_CMD_E,
        value);

    value = logicFormatPtr->tag0VlanId;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN_E,
        value);

    if (logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
    {
        value = hwValuesInfoPtr->tag1VlanCmd;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_VID1_CMD_E,
            value);

        value = logicFormatPtr->tag1VlanId;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_VID1_E,
            value);
    }
    else
    {
        value = logicFormatPtr->mplsTtl;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_TTL_E,
            value);

        value = BOOL2BIT_MAC(logicFormatPtr->enableDecrementTtl);
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_DEC_TTL_E,
            value);
    }

    value = hwValuesInfoPtr->qosPrecedence;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_QOS_PRECEDENCE_E,
        value);

    value = logicFormatPtr->qosProfile;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_QOS_PROFILE_E,
        value);

    value = hwValuesInfoPtr->modifyDSCP;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_DSCP_E,
        value);

    value = hwValuesInfoPtr->modifyTag0Up;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_UP_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->keepPreviousQoS);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_KEEP_PREVIOUS_QOS_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->trustUp);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_UP_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->trustDscp);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_DSCP_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->trustExp);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_EXP_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->remapDSCP);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MAP_DSCP_E,
        value);

    value = logicFormatPtr->tag0Up;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_UP0_E,
        value);

    value = hwValuesInfoPtr->tag1UpCommand;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_UP1_COMMAND_E,
        value);

    value = logicFormatPtr->tag1Up;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_UP1_E,
        value);

    value = hwValuesInfoPtr->ttPassengerPktType;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PACKET_TYPE_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->copyTtlExpFromTunnelHeader);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_COPY_TTL_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->tunnelTerminate);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TUNNEL_TERMINATION_E,
        value);

    value = hwValuesInfoPtr->mplsCommand;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MPLS_COMMAND_E,
        value);

    value = logicFormatPtr->hashMaskIndex;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_HASH_MASK_INDEX_E,
        value);

    /* Source ePort Assignment Enable */
    value = BOOL2BIT_MAC(logicFormatPtr->sourceEPortAssignmentEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT_ASSIGNMENT_ENABLE_E,
        value);

    /* Source ePort: assigned by TTI entry when
      <Source ePort Assignment Enable> = Enabled */
    value = logicFormatPtr->sourceEPort;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->mplsLLspQoSProfileEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_L_LSP_QOS_PROFILE_ENABLE_E,
        value);

    value = logicFormatPtr->ttHeaderLength;
    /* this is the Tunnel header length in units of Bytes. Granularity is in 2 Bytes. */
    if ((value % 2) != 0)/* length of field is checked by LION3_TTI_ACTION_FIELD_SET_MAC(...) */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    value = value / 2;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TT_HEADER_LENGTH_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->qosUseUpAsIndexEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_UP_CFI_TO_QOS_TABLE_SELECT_MODE_E,
        value);

    if (logicFormatPtr->qosUseUpAsIndexEnable == GT_TRUE)
    {
        if (logicFormatPtr->qosMappingTableIndex >= QOS_MAPPING_TABLE_INDEX_MAX_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    value = logicFormatPtr->qosMappingTableIndex;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_QOS_MAPPING_TABLE_INDEX_E,
        value);


    value = BOOL2BIT_MAC(logicFormatPtr->setMacToMe);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_SET_MAC2ME_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->isPtpPacket);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET_E,
        value);

    if (logicFormatPtr->isPtpPacket == GT_TRUE)
    {
        switch (logicFormatPtr->ptpTriggerType)
        {
        case CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_L2_E:
            value = 0;
            break;
        case CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_IPV4_UDP_E:
            value = 1;
            break;
        case CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_IPV6_UDP_E:
            value = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_TRIGGER_TYPE_E,
            value);

        value = logicFormatPtr->ptpOffset;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_OFFSET_E,
            value);
    }
    else
    {
        value = BOOL2BIT_MAC(logicFormatPtr->oamTimeStampEnable);
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_TIMESTAMP_ENABLE_E,
            value);

        value = logicFormatPtr->oamOffsetIndex;
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_OFFSET_INDEX_E,
            value);
    }

    value = BOOL2BIT_MAC(logicFormatPtr->oamProcessEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROCESSING_ENABLE_E,
        value);

    value = logicFormatPtr->oamProfile;
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROFILE_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->oamChannelTypeToOpcodeMappingEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_CHANNEL_TYPE_TO_OPCODE_MAPPING_EN_E,
        value);

    /* The global enable bit is retained for BWC.  If enabled, it overrides the TTI Action */
    rc = cpssDxChPclL3L4ParsingOverMplsEnableGet(devNum,&enablePtr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (enablePtr == GT_FALSE)
    {
        switch (logicFormatPtr->passengerParsingOfTransitMplsTunnelMode)
        {
        case CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_DISABLE_E:
            value = 0;
            break;
        case CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_IP_E:
            value = 1;
            break;
        case CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_CW_ETH_E:
            value = 2;
            break;
        case CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_ETH_E:
            value = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_ENABLE_E,
            value);

        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_BOBCAT2_MPLS_TRANSIT_TUNNEL_PARSING_WA_E))
        {
            /* FEr#4300730: MPLS Transit Tunnel Parsing requires setting the Tunnel Header Length in the TTI Action Entry */
            if ((value != 0) && (logicFormatPtr->tunnelTerminate == GT_FALSE) && (logicFormatPtr->ttHeaderLength == 0))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }

    value = BOOL2BIT_MAC(logicFormatPtr->passengerParsingOfTransitNonMplsTransitTunnelEnable);

    /* When setting <Passenger Parsing of Non-MPLS Transit tunnel Enable> == 1 and
       <TT Passenger Type> == MPLS --> unexpected behavior */
    if (value && logicFormatPtr->ttPassengerPacketType == CPSS_DXCH_TTI_PASSENGER_MPLS_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_NON_MPLS_TRANSIT_TUNNEL_ENABLE_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->rxIsProtectionPath);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_RX_IS_PROTECTION_PATH_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->rxProtectionSwitchEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_RX_ENABLE_PROTECTION_SWITCHING_E,
        value);

    switch (logicFormatPtr->pwTagMode)
    {
    case CPSS_DXCH_TTI_PW_TAG_DISABLED_MODE_E:
        value = 0;
        break;
    case CPSS_DXCH_TTI_PW_TAG_RAW_PW_MODE_E:
        value = 1;
        break;
    case CPSS_DXCH_TTI_PW_TAG_TAGGED_PW_MODE_E:
        value = 2;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PW_TAG_MODE_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->continueToNextTtiLookup);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_TTI_1_LOOKUP_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->cwBasedPw);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_CW_BASED_PSEUDO_WIRE_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->ttlExpiryVccvEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TTL_EXPIRY_VCCV_ENABLE_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->pwe3FlowLabelExist);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PWE3_FLOW_LABEL_EXIST_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->pwCwBasedETreeEnable);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PW_CW_BASED_E_TREE_ENABLE_E,
        value);

    value = BOOL2BIT_MAC(logicFormatPtr->applyNonDataCwCommand);
    LION3_TTI_ACTION_FIELD_SET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_APPLY_NON_DATA_CW_COMMAND_E,
        value);

    return GT_OK;
}

/*******************************************************************************
* ttiActionType2Logic2HwFormat
*
* DESCRIPTION:
*       Converts a given tti action type 2 from logic format to hardware format.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*
*       devNum            - device number
*       logicFormatPtr    - points to tti action in logic format
*
* OUTPUTS:
*       hwFormatArray     - tti action in hardware format (5 words)
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiActionType2Logic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_ACTION_2_STC         *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{
    TTI_ACTION_HW_VALUES_STC    hwValues;/* hw values to set to hw */
    GT_STATUS   rc;                             /* return status */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* zero out hw format */
    cpssOsMemSet(hwFormatArray,0,sizeof(GT_U32)*TTI_ACTION_TYPE_2_SIZE_CNS);
    cpssOsMemSet(&hwValues,0,sizeof(hwValues));

    /********************************************************/
    /* check params and translate params to hardware format */
    /********************************************************/

    /* check and convert tt passenger packet type */
    switch (logicFormatPtr->ttPassengerPacketType)
    {
    case CPSS_DXCH_TTI_PASSENGER_IPV4V6_E:
        hwValues.ttPassengerPktType = 0;
        break;
    case CPSS_DXCH_TTI_PASSENGER_MPLS_E:
        hwValues.ttPassengerPktType = 1;
        break;
    case CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E:
        hwValues.ttPassengerPktType = 2;
        break;
    case CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E:
        hwValues.ttPassengerPktType = 3;
        break;
    default:
        /* if not TT then TT Passenger Type is not relevant */
        if(logicFormatPtr->tunnelTerminate == GT_FALSE)
        {
            hwValues.ttPassengerPktType = 0;
            break;
        }
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* in MPLS LSR functionality, tt passenger type must be MPLS */
    if ((logicFormatPtr->tunnelTerminate == GT_FALSE) && (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE))
    {
        if ((logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_POP1_CMD_E ||
            logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_POP2_CMD_E ||
            logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E))
        {
            if (logicFormatPtr->ttPassengerPacketType != CPSS_DXCH_TTI_PASSENGER_MPLS_E)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* check and convert ts passenger type */
    if (logicFormatPtr->redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E &&
        logicFormatPtr->tunnelStart == GT_TRUE)
    {
        if ((logicFormatPtr->tunnelTerminate == GT_FALSE) &&
            (logicFormatPtr->mplsCommand != CPSS_DXCH_TTI_MPLS_NOP_CMD_E))
        {
            if (logicFormatPtr->tsPassengerPacketType != CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* modifyMacDa == GT_TRUE requires that tunnelStart == GT_FALSE*/
        if (logicFormatPtr->modifyMacDa == GT_TRUE &&
            ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) &&
             (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        switch (logicFormatPtr->tsPassengerPacketType)
        {
        case CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E:
            hwValues.tsPassengerPacketType = 0;
            break;
        case CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E:
            hwValues.tsPassengerPacketType = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* check and convert mpls command */
    switch (logicFormatPtr->mplsCommand)
    {
    case CPSS_DXCH_TTI_MPLS_NOP_CMD_E:
        hwValues.mplsCommand = 0;
        break;
    case CPSS_DXCH_TTI_MPLS_SWAP_CMD_E:
        hwValues.mplsCommand = 1;
        break;
    case CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E:
        hwValues.mplsCommand = 2;
        break;
    case CPSS_DXCH_TTI_MPLS_POP1_CMD_E:
        hwValues.mplsCommand = 3;
        break;
    case CPSS_DXCH_TTI_MPLS_POP2_CMD_E:
        hwValues.mplsCommand = 4;
        break;
    case CPSS_DXCH_TTI_MPLS_POP3_CMD_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* POP3 supported only for eArch */
            hwValues.mplsCommand = 5;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        break;
    case CPSS_DXCH_TTI_MPLS_POP_AND_SWAP_CMD_E:
        hwValues.mplsCommand = 7;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (logicFormatPtr->mplsCommand != CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
    {
        if (logicFormatPtr->mplsTtl > TTL_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate tunnel termination action forwarding command parameter */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(hwValues.pktCommand,logicFormatPtr->command);

    /* translate tunnel termination action redirect command parameter */
    switch (logicFormatPtr->redirectCommand)
    {
    case CPSS_DXCH_TTI_NO_REDIRECT_E:
        hwValues.redirectCommand = 0;
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
        hwValues.redirectCommand = 1;
        break;
    case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
        hwValues.redirectCommand = 2;
        if(logicFormatPtr->routerLttPtr >= BIT_15)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        hwValues.routerLttPtr = logicFormatPtr->routerLttPtr;
        if (logicFormatPtr->routerLttPtr > (fineTuningPtr->tableSize.tunnelTerm * 4))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        /* FEr#2018: Limited number of Policy-based routes */
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E))
        {
            /* xCat devices support index 0,4,8,12(max IP TCAM row * 4) only */
            if ((logicFormatPtr->routerLttPtr % 4) != 0)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        break;
    case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
        hwValues.redirectCommand = 4;
        if (logicFormatPtr->vrfId > VRF_ID_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    case CPSS_DXCH_TTI_LOGICAL_PORT_ASSIGN_E:
        /* redirect to logical port is supported only for Lion2-B0 and above; xCat C0; xCat3*/
        if((PRV_CPSS_DXCH_LION2_B0_AND_ABOVE_CHECK_MAC(devNum)) ||
           (PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE))
        {
            hwValues.redirectCommand = 5;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(logicFormatPtr->redirectCommand == CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        switch (logicFormatPtr->egressInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                logicFormatPtr->egressInterface.devPort.hwDevNum,
                logicFormatPtr->egressInterface.devPort.portNum);
            hwValues.hwDev = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                logicFormatPtr->egressInterface.devPort.hwDevNum,
                logicFormatPtr->egressInterface.devPort.portNum);
            hwValues.hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                logicFormatPtr->egressInterface.devPort.hwDevNum,
                logicFormatPtr->egressInterface.devPort.portNum);
            if(hwValues.hwPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
            {
                /* 6 bits for the port number */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
         case CPSS_INTERFACE_TRUNK_E:
            if(logicFormatPtr->egressInterface.trunkId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
            {
                /* 7 bits for the trunkId */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_INTERFACE_VIDX_E:
            if (logicFormatPtr->egressInterface.vidx > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_VIDX_MAC(devNum))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        case CPSS_INTERFACE_VID_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (logicFormatPtr->tunnelStart == GT_TRUE)
        {
            if (logicFormatPtr->tunnelStartPtr > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TS_PTR_MAC(devNum))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        else
        {
            if (logicFormatPtr->arpPtr > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_ARP_PTR_MAC(devNum))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

    }

    if (logicFormatPtr->sourceIdSetEnable == GT_TRUE)
    {
        if (logicFormatPtr->sourceId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_SRC_ID_MAC(devNum))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    }

    /* translate tag 0 VLAN command from action parameter */
    switch (logicFormatPtr->tag0VlanCmd)
    {
    case CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E:
        hwValues.tag0VlanCmd = 0;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E:
        hwValues.tag0VlanCmd = 1;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E:
        hwValues.tag0VlanCmd = 2;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E:
        hwValues.tag0VlanCmd = 3;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC(devNum, logicFormatPtr->tag0VlanId);

    /* translate tag 1 VLAN command from action parameter */
    switch (logicFormatPtr->tag1VlanCmd)
    {
    case CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E:
        hwValues.tag1VlanCmd = 0;
        break;
    case CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E:
        hwValues.tag1VlanCmd = 1;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* the tag1VlanId  is checked with PRV_CPSS_VLAN_VALUE_CHECK_MAC  and
       not with PRV_CPSS_DXCH_VLAN_VALUE_CHECK_MAC , because tag1VlanId not
       supporting enhanced vid */
    PRV_CPSS_VLAN_VALUE_CHECK_MAC(logicFormatPtr->tag1VlanId);

    /* translate VLAN precedence from action parameter */
    switch (logicFormatPtr->tag0VlanPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        hwValues.tag0VlanPrecedence = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
        hwValues.tag0VlanPrecedence = 0;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (logicFormatPtr->bindToPolicerMeter == GT_TRUE ||
        logicFormatPtr->bindToPolicer == GT_TRUE)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if (logicFormatPtr->policerIndex >= BIT_16)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        else
        {
            if (logicFormatPtr->policerIndex >= BIT_12)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* translate QoS precedence from action parameter */
    switch (logicFormatPtr->qosPrecedence)
    {
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
        hwValues.qosPrecedence = 1;
        break;
    case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
        hwValues.qosPrecedence = 0;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (logicFormatPtr->keepPreviousQoS == GT_FALSE)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            if (logicFormatPtr->qosProfile > QOS_PROFILE_E_ARCH_MAX_CNS)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        else
        {
            if (logicFormatPtr->qosProfile > QOS_PROFILE_MAX_CNS)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }


    /* translate tunnel termination action modify User Priority parameter */
    switch (logicFormatPtr->modifyTag0Up)
    {
    case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E:
        hwValues.modifyTag0Up = 0;
        break;
    case CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E:
        hwValues.modifyTag0Up = 1;
        break;
    case CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E:
        hwValues.modifyTag0Up = 2;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate tunnel termination tag1 up command parameter */
    switch (logicFormatPtr->tag1UpCommand)
    {
    case CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E:
        hwValues.tag1UpCommand = 0;
        break;
    case CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E:
        hwValues.tag1UpCommand = 1;
        break;
    case CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E:
        hwValues.tag1UpCommand = 2;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate tunnel termination action modify DSCP parameter */
    switch (logicFormatPtr->modifyDscp)
    {
    case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E:
        hwValues.modifyDSCP = 0;
        break;
    case CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E:
        hwValues.modifyDSCP = 1;
        break;
    case CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E:
        hwValues.modifyDSCP = 2;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->tag0Up);
    PRV_CPSS_DXCH_COS_CHECK_UP_MAC(logicFormatPtr->tag1Up);

    if (logicFormatPtr->hashMaskIndex >= BIT_4 &&
        ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) &&
         (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (logicFormatPtr->redirectCommand != CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        switch (logicFormatPtr->pcl0OverrideConfigIndex)
        {
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            hwValues.pcl0OverrideConfigIndex = 0;
            break;
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            hwValues.pcl0OverrideConfigIndex = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        switch (logicFormatPtr->pcl0_1OverrideConfigIndex)
        {
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            hwValues.pcl0_1OverrideConfigIndex = 0;
            break;
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            hwValues.pcl0_1OverrideConfigIndex = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        switch (logicFormatPtr->pcl1OverrideConfigIndex)
        {
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            hwValues.pcl1OverrideConfigIndex = 0;
            break;
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            hwValues.pcl1OverrideConfigIndex = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (logicFormatPtr->iPclConfigIndex > IPCL_INDEX_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        if ((logicFormatPtr->userDefinedCpuCode >= CPSS_NET_FIRST_USER_DEFINED_E) &&
            (logicFormatPtr->userDefinedCpuCode <= CPSS_NET_LAST_USER_DEFINED_E))
        {
            rc = prvCpssDxChNetIfCpuToDsaCode(logicFormatPtr->userDefinedCpuCode,
                                         (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT*)(GT_UINTPTR)&hwValues.userDefinedCpuCode);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else
        hwValues.userDefinedCpuCode = 0;

    if (logicFormatPtr->bindToCentralCounter == GT_TRUE)
    {
        if (logicFormatPtr->centralCounterIndex > CENTRAL_COUNTER_MAX_CNS)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* all common checks done */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* call the Bobcat2; Caelum; Bobcat3 format */
        return ttiActionLion3Logic2HwFormat(devNum, logicFormatPtr, &hwValues, hwFormatArray);
    }
    else
    {
        /* call the xCat, xCat3, Lion, xCat2, Lion2 format */
        return ttiActionXcatLogic2HwFormat(devNum, logicFormatPtr, &hwValues, hwFormatArray);
    }

}

/*******************************************************************************
* ttiActionType2Hw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tti action type 2 from hardware format to
*       logic format.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - device number
*       hwFormatArray     - tti action in hardware format (5 words)
*
* OUTPUTS:
*       logicFormatPtr    - points to tti action in logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiActionType2Hw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_DXCH_TTI_ACTION_2_STC          *logicFormatPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    tempRouterLttPtr=0;


    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(logicFormatPtr->command, U32_GET_FIELD_MAC(hwFormatArray[0],0,3));

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        rc = prvCpssDxChNetIfDsaToCpuCode((U32_GET_FIELD_MAC(hwFormatArray[0],3,8)),
                                   &logicFormatPtr->userDefinedCpuCode);
        if(rc != GT_OK)
            return rc;
    }

    logicFormatPtr->mirrorToIngressAnalyzerEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],11,1));

    switch (U32_GET_FIELD_MAC(hwFormatArray[0],12,3))
    {
    case 0:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_NO_REDIRECT_E;
        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            logicFormatPtr->multiPortGroupTtiEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],15,1));
        }
        logicFormatPtr->iPclConfigIndex = U32_GET_FIELD_MAC(hwFormatArray[0],31,1) |
                                          (U32_GET_FIELD_MAC(hwFormatArray[1],0,12) << 1);
        break;
    case 1:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;
        if (U32_GET_FIELD_MAC(hwFormatArray[0],27,1) == 0) /* target is not VIDX */
        {
            if (U32_GET_FIELD_MAC(hwFormatArray[0],15,1) == 0) /* target is not trunk */
            {
                logicFormatPtr->egressInterface.devPort.portNum = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[0],16,6);
                logicFormatPtr->egressInterface.devPort.hwDevNum  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[0],22,5);
                logicFormatPtr->egressInterface.type = CPSS_INTERFACE_PORT_E;
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(logicFormatPtr->egressInterface));
            }
            else /* target is trunk */
            {
                logicFormatPtr->egressInterface.trunkId = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],16,7);
                logicFormatPtr->egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
            }
        }
        else /* target is VIDX */
        {
            logicFormatPtr->egressInterface.vidx = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[0],15,12);
            if(logicFormatPtr->egressInterface.vidx == 0xFFF)
            {
                logicFormatPtr->egressInterface.type = CPSS_INTERFACE_VID_E;
                logicFormatPtr->egressInterface.vidx = 0;
            }
            else
            {
                logicFormatPtr->egressInterface.type = CPSS_INTERFACE_VIDX_E;
            }
        }

        logicFormatPtr->tunnelStart = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[0],28,1));
        if (logicFormatPtr->tunnelStart == GT_TRUE)
        {
            logicFormatPtr->tunnelStartPtr = U32_GET_FIELD_MAC(hwFormatArray[0],29,3) |
                                             (U32_GET_FIELD_MAC(hwFormatArray[1],0,10) << 3);
            switch (U32_GET_FIELD_MAC(hwFormatArray[1],10,1))
            {
            case 0:
                logicFormatPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;
                break;
            case 1:
                logicFormatPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            logicFormatPtr->arpPtr = U32_GET_FIELD_MAC(hwFormatArray[0],29,3) |
                                     (U32_GET_FIELD_MAC(hwFormatArray[1],0,11) << 3);
        }
        logicFormatPtr->vntl2Echo = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],11,1));
        if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) &&
            (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E))
        {
        logicFormatPtr->modifyMacDa = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],22,1));
        logicFormatPtr->modifyMacSa = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],23,1));
        }
        break;
    case 2:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;
        /* LTT entry is row based however in this field the LTT is treated as column based.
          Bits [11:0] indicate row while bits [14:13] indicate column, bit 12 is not used.
          The Formula for translating the LTT entry to column based is as follow:
          [11:0] << 2 + [14:13]   (Note: bit 12 not used). */
        tempRouterLttPtr = U32_GET_FIELD_MAC(hwFormatArray[0],15,15);
        logicFormatPtr->routerLttPtr = (((tempRouterLttPtr & 0xFFF) << 2) |
                                        ((tempRouterLttPtr & 0x6000) >> 13));

        logicFormatPtr->iPclConfigIndex = U32_GET_FIELD_MAC(hwFormatArray[0],31,1) |
                                          (U32_GET_FIELD_MAC(hwFormatArray[1],0,12) << 1);
        break;
    case 4:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_VRF_ID_ASSIGN_E;
        logicFormatPtr->vrfId           = U32_GET_FIELD_MAC(hwFormatArray[0],15,12);
        logicFormatPtr->iPclConfigIndex = U32_GET_FIELD_MAC(hwFormatArray[0],31,1) |
                                          (U32_GET_FIELD_MAC(hwFormatArray[1],0,12) << 1);
        break;
    case 5:
        /* redirect to logical port is supported only for Lion2-B0 and above; xCat C0; xCat3*/
        if((PRV_CPSS_DXCH_LION2_B0_AND_ABOVE_CHECK_MAC(devNum)) ||
           (PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_TRUE))
        {
            logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_LOGICAL_PORT_ASSIGN_E;
            logicFormatPtr->egressInterface.devPort.portNum = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[0],16,6);
            logicFormatPtr->egressInterface.devPort.hwDevNum  = (GT_U8)U32_GET_FIELD_MAC(hwFormatArray[0],22,5);
            PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(logicFormatPtr->egressInterface));

            if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
            {
                switch (U32_GET_FIELD_MAC(hwFormatArray[0],27,2))
                {
                    case 0:
                        logicFormatPtr->pwTagMode = CPSS_DXCH_TTI_PW_TAG_DISABLED_MODE_E;
                        break;
                    case 1:
                        logicFormatPtr->pwTagMode = CPSS_DXCH_TTI_PW_TAG_RAW_PW_MODE_E;
                        break;
                    case 2:
                        logicFormatPtr->pwTagMode = CPSS_DXCH_TTI_PW_TAG_TAGGED_PW_MODE_E;
                        break;
                    default:
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                if (U32_GET_FIELD_MAC(hwFormatArray[0],29,2))
                {
                    logicFormatPtr->sourceMeshIdSetEnable = GT_TRUE;
                    logicFormatPtr->sourceMeshId = U32_GET_FIELD_MAC(hwFormatArray[0],29,2);
                }
                else
                {
                    logicFormatPtr->sourceMeshIdSetEnable = GT_FALSE;
                    logicFormatPtr->sourceMeshId = 0;
                }

                logicFormatPtr->iPclConfigIndex = U32_GET_FIELD_MAC(hwFormatArray[0],31,1) |
                                              (U32_GET_FIELD_MAC(hwFormatArray[1],0,10) << 1);
            }
            else
            {
                logicFormatPtr->iPclConfigIndex = U32_GET_FIELD_MAC(hwFormatArray[0],31,1) |
                    (U32_GET_FIELD_MAC(hwFormatArray[1],0,12) << 1);
            }
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->bindToCentralCounter = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],12,1));
     if(PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_ENABLED_MAC(devNum) == GT_TRUE)
    {
        if (logicFormatPtr->bindToCentralCounter == GT_TRUE)
        {
            logicFormatPtr->centralCounterIndex   = U32_GET_FIELD_MAC(hwFormatArray[1],13,12);
        }
        logicFormatPtr->cwBasedPw = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],25,1));
        switch (U32_GET_FIELD_MAC(hwFormatArray[1],26,1)  | (U32_GET_FIELD_MAC(hwFormatArray[4],18,2) << 1))
        {
            case 0:
                logicFormatPtr->unknownSaCommandEnable = GT_TRUE;
                logicFormatPtr->unknownSaCommand = CPSS_PACKET_CMD_FORWARD_E;
                break;
            case 1:
                logicFormatPtr->unknownSaCommandEnable = GT_TRUE;
                logicFormatPtr->unknownSaCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
                break;
            case 2:
                logicFormatPtr->unknownSaCommandEnable = GT_TRUE;
                logicFormatPtr->unknownSaCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
                break;
            case 3:
                logicFormatPtr->unknownSaCommandEnable = GT_TRUE;
                logicFormatPtr->unknownSaCommand = CPSS_PACKET_CMD_DROP_HARD_E;
                break;
            case 4:
                logicFormatPtr->unknownSaCommandEnable = GT_TRUE;
                logicFormatPtr->unknownSaCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
                break;
            case 7:
                logicFormatPtr->unknownSaCommandEnable = GT_FALSE;
                logicFormatPtr->unknownSaCommand = CPSS_PACKET_CMD_FORWARD_E; /* dummy */
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if (logicFormatPtr->bindToCentralCounter == GT_TRUE)
        {
            logicFormatPtr->centralCounterIndex   = U32_GET_FIELD_MAC(hwFormatArray[1],13,14);
        }
    }

    logicFormatPtr->bindToPolicerMeter = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],27,1));
    logicFormatPtr->bindToPolicer = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[1],28,1));
    if ((logicFormatPtr->bindToPolicerMeter == GT_TRUE) ||
        (logicFormatPtr->bindToPolicer == GT_TRUE))
    {
        logicFormatPtr->policerIndex = U32_GET_FIELD_MAC(hwFormatArray[1],29,3) |
                                       (U32_GET_FIELD_MAC(hwFormatArray[2],0,9) << 3);
    }

    logicFormatPtr->sourceIdSetEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],9,1));
    if (logicFormatPtr->sourceIdSetEnable == GT_TRUE)
    {
        logicFormatPtr->sourceId   = U32_GET_FIELD_MAC(hwFormatArray[2],10,5);
    }

    logicFormatPtr->actionStop        = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],15,1));
    logicFormatPtr->bridgeBypass      = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],16,1));
    logicFormatPtr->ingressPipeBypass = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],17,1));

    if (logicFormatPtr->redirectCommand != CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        switch (U32_GET_FIELD_MAC(hwFormatArray[2],18,1))
        {
            case 0:
                logicFormatPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
                break;
            case 1:
                logicFormatPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        switch (U32_GET_FIELD_MAC(hwFormatArray[2],19,1))
        {
            case 0:
                logicFormatPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
                break;
            case 1:
                logicFormatPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        switch (U32_GET_FIELD_MAC(hwFormatArray[2],20,1))
        {
            case 0:
                logicFormatPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
                break;
            case 1:
                logicFormatPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            logicFormatPtr->ResetSrcPortGroupId = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],22,1));
        }
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[2],21,1))
    {
    case 0:
        logicFormatPtr->tag0VlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        break;
    case 1:
        logicFormatPtr->tag0VlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->nestedVlanEnable = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[2],22,1));

    switch (U32_GET_FIELD_MAC(hwFormatArray[2],23,2))
    {
    case 0:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E;
        break;
    case 1:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
        break;
    case 2:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E;
        break;
    case 3:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->tag0VlanId = (GT_U16)(U32_GET_FIELD_MAC(hwFormatArray[2],25,7) |
                                          (U32_GET_FIELD_MAC(hwFormatArray[3],0,5) << 7));

    if(logicFormatPtr->egressInterface.type == CPSS_INTERFACE_VID_E)
    {
        logicFormatPtr->egressInterface.vlanId = logicFormatPtr->tag0VlanId;
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[4],15,3))
    {
    case 0:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_NOP_CMD_E;
        break;
    case 1:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_SWAP_CMD_E;
        break;
    case 2:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E;
        break;
    case 3:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP1_CMD_E;
        break;
    case 4:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP2_CMD_E;
        break;
    case 5:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    case 7:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP_AND_SWAP_CMD_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    if (logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
    {
        switch (U32_GET_FIELD_MAC(hwFormatArray[3],5,1))
        {
        case 0:
            logicFormatPtr->tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;
        case 1:
            logicFormatPtr->tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
        logicFormatPtr->tag1VlanId = (GT_U16)U32_GET_FIELD_MAC(hwFormatArray[3],6,12);
    }
    else
    {
        if (logicFormatPtr->mplsCommand != CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
        {
            logicFormatPtr->mplsTtl = U32_GET_FIELD_MAC(hwFormatArray[3],5,8);
        }
        logicFormatPtr->enableDecrementTtl = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],13,1));
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[3],18,1))
    {
    case 0:
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        break;
    case 1:
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->keepPreviousQoS = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],30,1));
    if (logicFormatPtr->keepPreviousQoS == GT_FALSE)
    {
        logicFormatPtr->qosProfile   = U32_GET_FIELD_MAC(hwFormatArray[3],19,7);
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[3],26,2))
    {
    case 0:
        logicFormatPtr->modifyDscp = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
        break;
    case 1:
        logicFormatPtr->modifyDscp = CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E;
        break;
    case 2:
        logicFormatPtr->modifyDscp = CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch (U32_GET_FIELD_MAC(hwFormatArray[3],28,2))
    {
    case 0:
        logicFormatPtr->modifyTag0Up = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E;
        break;
    case 1:
        logicFormatPtr->modifyTag0Up = CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E;
        break;
    case 2:
        logicFormatPtr->modifyTag0Up = CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->trustUp   = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[3],31,1));
    logicFormatPtr->trustDscp = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],0,1));
    logicFormatPtr->trustExp  = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],1,1));
    logicFormatPtr->remapDSCP = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],2,1));
    logicFormatPtr->tag0Up    = U32_GET_FIELD_MAC(hwFormatArray[4],3,3);

    switch (U32_GET_FIELD_MAC(hwFormatArray[4],6,2))
    {
    case 0:
        logicFormatPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E;
        break;
    case 1:
        logicFormatPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E;
        break;
    case 2:
        logicFormatPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->tag1Up    = U32_GET_FIELD_MAC(hwFormatArray[4],8,3);

    switch (U32_GET_FIELD_MAC(hwFormatArray[4],11,2))
    {
    case 0:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV4V6_E;
        break;
    case 1:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_MPLS_E;
        break;
    case 2:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E;
        break;
    case 3:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    logicFormatPtr->copyTtlExpFromTunnelHeader = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],13,1));
    logicFormatPtr->tunnelTerminate         = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwFormatArray[4],14,1));
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) &&
        (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E))
    {
        logicFormatPtr->hashMaskIndex           = U32_GET_FIELD_MAC(hwFormatArray[4],18,4);
    }

    return GT_OK;
}

/*******************************************************************************
* ttiActionLion3Hw2LogicFormat
*
* DESCRIPTION:
*       Bobcat2; Caelum; Bobcat3 : Converts a given tti action (type 2) from hardware format to
*       logic format.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       hwFormatArray     - tti action in hardware format (8 words)
*
* OUTPUTS:
*       logicFormatPtr    - points to tti action in logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiActionLion3Hw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *hwFormatArray,
    OUT CPSS_DXCH_TTI_ACTION_2_STC          *logicFormatPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    value;
    GT_BOOL   enablePtr;
    GT_U32    leafSwIndex;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_COMMAND_E,
        value);

    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(logicFormatPtr->command, value);

    /* translate user defined cpu code from action parameter */
    /* note that cpu code is relevant only for trap or mirror commands */
    if ((logicFormatPtr->command == CPSS_PACKET_CMD_MIRROR_TO_CPU_E) ||
        (logicFormatPtr->command == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_CPU_CODE_E,
            value);

        rc = prvCpssDxChNetIfDsaToCpuCode(value,
                                   &logicFormatPtr->userDefinedCpuCode);
        if(rc != GT_OK)
            return rc;
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MIRROR_TO_ANALYZER_PORT_E,
        value);

    if(value)
    {
        logicFormatPtr->mirrorToIngressAnalyzerEnable = GT_TRUE;
        logicFormatPtr->mirrorToIngressAnalyzerIndex  = value - 1;
    }
    else
    {
        logicFormatPtr->mirrorToIngressAnalyzerEnable = GT_FALSE;
        logicFormatPtr->mirrorToIngressAnalyzerIndex = 0;
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_CMD_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_NO_REDIRECT_E;
/*  not supported ???
        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            logicFormatPtr->multiPortGroupTtiEnable = ???;
        }
*/
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_FLOW_ID_E,
            value);
        logicFormatPtr->flowId = value;

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_IPCL_UDB_CONFIGURATION_TABLE_UDE_INDEX_E,
            value);

        if (value)
        {
            logicFormatPtr->iPclUdbConfigTableEnable = GT_TRUE;
            switch (value)
            {
            case 1:
                logicFormatPtr->iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE_E;
                break;
            case 2:
                logicFormatPtr->iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E;
                break;
            case 3:
                logicFormatPtr->iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E;
                break;
            case 4:
                logicFormatPtr->iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E;
                break;
            case 5:
                logicFormatPtr->iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E;
                break;
            case 6:
                logicFormatPtr->iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE5_E;
                break;
            case 7:
                logicFormatPtr->iPclUdbConfigTableIndex = CPSS_DXCH_PCL_PACKET_TYPE_UDE6_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            logicFormatPtr->iPclUdbConfigTableEnable = GT_FALSE;
        }
        break;
    case 1:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_USE_EVIDX_E,
            value);

        if (value == 0) /* target is not VIDX */
        {
            LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_IS_TRUNK_E,
                value);

            if (value == 0) /* target is not trunk */
            {
                LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
                    LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EPORT_NUMBER_E,
                    value);

                logicFormatPtr->egressInterface.devPort.portNum = value;

                LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
                    LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_DEVICE_NUMBER_E,
                    value);

                logicFormatPtr->egressInterface.devPort.hwDevNum  = value;

                logicFormatPtr->egressInterface.type = CPSS_INTERFACE_PORT_E;
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(logicFormatPtr->egressInterface));
            }
            else /* target is trunk */
            {
                LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
                    LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TRUNK_NUMBER_E,
                    value);

                logicFormatPtr->egressInterface.trunkId = (GT_U16)value;
                logicFormatPtr->egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
            }
        }
        else /* target is VIDX */
        {
            LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_EVIDX_E,
                value);

            logicFormatPtr->egressInterface.vidx = (GT_U16)value;

            if(logicFormatPtr->egressInterface.vidx == 0xFFF)
            {
                logicFormatPtr->egressInterface.type = CPSS_INTERFACE_VID_E;
                logicFormatPtr->egressInterface.vidx = 0;
            }
            else
            {
                logicFormatPtr->egressInterface.type = CPSS_INTERFACE_VIDX_E;
            }
        }

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_E,
            value);

        logicFormatPtr->tunnelStart = BIT2BOOL_MAC(value);
        if (logicFormatPtr->tunnelStart == GT_TRUE)
        {
            LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_INDEX_E,
                value);
            logicFormatPtr->tunnelStartPtr = value;

            LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_TUNNEL_START_PASSENGER_TYPE_E,
                value);
            switch (value)
            {
            case 0:
                logicFormatPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;
                break;
            case 1:
                logicFormatPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_ARP_POINTER_E,
                value);
            logicFormatPtr->arpPtr = value;
        }

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_INTERFACE_VNT_L2_ECHO_E,
            value);
        logicFormatPtr->vntl2Echo = BIT2BOOL_MAC(value);

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_DA_E,
            value);
        logicFormatPtr->modifyMacDa = BIT2BOOL_MAC(value);

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_EGRESS_MODIFY_MAC_SA_E,
            value);
        logicFormatPtr->modifyMacSa = BIT2BOOL_MAC(value);

        break;
    case 2:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_REDIRECT_TO_ROUTER_ROUTER_LTT_INDEX_E,
            value);
        /* get the HW index and translate it to SW index that take into consideration
           "holes" that we might have in the LPM RAM*/
        prvCpssDxChLpmLeafEntryCalcLpmSwIndexFromHwIndex(devNum,value,&leafSwIndex);
        logicFormatPtr->routerLttPtr = leafSwIndex;

        break;
    case 4:
        logicFormatPtr->redirectCommand = CPSS_DXCH_TTI_VRF_ID_ASSIGN_E;

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_ASSIGN_VRF_ID_VRF_ID_E,
            value);
        logicFormatPtr->vrfId           = value;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    if(logicFormatPtr->redirectCommand != CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_IPCL_PROFILE_INDEX_E,
            value);

        logicFormatPtr->iPclConfigIndex = value;
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_CNC_COUNTER_E,
        value);
    logicFormatPtr->bindToCentralCounter = BIT2BOOL_MAC(value);
    if (logicFormatPtr->bindToCentralCounter == GT_TRUE)
    {
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_CNC_COUNTER_INDEX_E,
            value);
        logicFormatPtr->centralCounterIndex   = value;
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_METER_E,
        value);
    logicFormatPtr->bindToPolicerMeter = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BIND_TO_POLICER_COUNTER_E,
        value);
    logicFormatPtr->bindToPolicer = BIT2BOOL_MAC(value);

    if ((logicFormatPtr->bindToPolicerMeter == GT_TRUE) ||
        (logicFormatPtr->bindToPolicer == GT_TRUE))
    {
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_POLICER_INDEX_E,
            value);

        logicFormatPtr->policerIndex = value;
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_ID_SET_ENABLE_E,
        value);
    logicFormatPtr->sourceIdSetEnable = BIT2BOOL_MAC(value);

    if (logicFormatPtr->sourceIdSetEnable == GT_TRUE)
    {
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_ID_E,
            value);
        logicFormatPtr->sourceId   = value;
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_ACTION_STOP_E,
        value);
    logicFormatPtr->actionStop        = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_BRIDGE_E,
        value);
    logicFormatPtr->bridgeBypass      = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_BYPASS_INGRESS_PIPE_E,
        value);
    logicFormatPtr->ingressPipeBypass = BIT2BOOL_MAC(value);

    if (logicFormatPtr->redirectCommand != CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E)
    {

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY2_LOOKUP_MODE_E,
            value);

        switch (value)
        {
        case 0:
            logicFormatPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;
        case 1:
            logicFormatPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY1_LOOKUP_MODE_E,
            value);

        switch (value)
        {
        case 0:
            logicFormatPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;
        case 1:
            logicFormatPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NO_REDIRECT_TO_EGRESS_INTERFACE_POLICY0_LOOKUP_MODE_E,
            value);
        switch (value)
        {
        case 0:
            logicFormatPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;
        case 1:
            logicFormatPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            /*no such field any more logicFormatPtr->ResetSrcPortGroupId*/
        }
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN_PRECEDENCE_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->tag0VlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        break;
    case 1:
        logicFormatPtr->tag0VlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_NESTED_VLAN_E,
        value);
    logicFormatPtr->nestedVlanEnable = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_EVID_CMD_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E;
        break;
    case 1:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
        break;
    case 2:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E;
        break;
    case 3:
        logicFormatPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_EVLAN_E,
        value);

    logicFormatPtr->tag0VlanId = (GT_U16)value;

    if(logicFormatPtr->egressInterface.type == CPSS_INTERFACE_VID_E)
    {
        logicFormatPtr->egressInterface.vlanId = logicFormatPtr->tag0VlanId;
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MPLS_COMMAND_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_NOP_CMD_E;
        break;
    case 1:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_SWAP_CMD_E;
        break;
    case 2:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E;
        break;
    case 3:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP1_CMD_E;
        break;
    case 4:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP2_CMD_E;
        break;
    case 5:
        /* POP3 supported only for eArch */
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP3_CMD_E;
        break;
    case 7:
        logicFormatPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP_AND_SWAP_CMD_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    if (logicFormatPtr->mplsCommand == CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
    {

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_VID1_CMD_E,
            value);

        switch (value)
        {
        case 0:
            logicFormatPtr->tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;
        case 1:
            logicFormatPtr->tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_VID1_E,
            value);
        logicFormatPtr->tag1VlanId = (GT_U16)value;
    }
    else
    {
        if (logicFormatPtr->mplsCommand != CPSS_DXCH_TTI_MPLS_NOP_CMD_E)
        {
            LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
                LION3_TTI_ACTION_TABLE_FIELDS_TTL_E,
                value);
            logicFormatPtr->mplsTtl = value;
        }

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_DEC_TTL_E,
            value);
        logicFormatPtr->enableDecrementTtl = BIT2BOOL_MAC(value);
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_QOS_PRECEDENCE_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        break;
    case 1:
        logicFormatPtr->qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_KEEP_PREVIOUS_QOS_E,
        value);
    logicFormatPtr->keepPreviousQoS = BIT2BOOL_MAC(value);
    if (logicFormatPtr->keepPreviousQoS == GT_FALSE)
    {
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_QOS_PROFILE_E,
            value);
        logicFormatPtr->qosProfile   = value;
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_DSCP_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->modifyDscp = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
        break;
    case 1:
        logicFormatPtr->modifyDscp = CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E;
        break;
    case 2:
        logicFormatPtr->modifyDscp = CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MODIFY_UP_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->modifyTag0Up = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E;
        break;
    case 1:
        logicFormatPtr->modifyTag0Up = CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E;
        break;
    case 2:
        logicFormatPtr->modifyTag0Up = CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_UP_E,
        value);
    logicFormatPtr->trustUp   = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_DSCP_E,
        value);
    logicFormatPtr->trustDscp = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_EXP_E,
        value);
    logicFormatPtr->trustExp  = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_MAP_DSCP_E,
        value);
    logicFormatPtr->remapDSCP = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_UP0_E,
        value);
    logicFormatPtr->tag0Up    = value;

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_UP1_COMMAND_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E;
        break;
    case 1:
        logicFormatPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E;
        break;
    case 2:
        logicFormatPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_UP1_E,
        value);
    logicFormatPtr->tag1Up    = value;

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PACKET_TYPE_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV4V6_E;
        break;
    case 1:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_MPLS_E;
        break;
    case 2:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E;
        break;
    case 3:
        logicFormatPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_COPY_TTL_E,
        value);
    logicFormatPtr->copyTtlExpFromTunnelHeader = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TUNNEL_TERMINATION_E,
        value);
    logicFormatPtr->tunnelTerminate         = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_HASH_MASK_INDEX_E,
        value);
    logicFormatPtr->hashMaskIndex           = value;

    /* Source ePort Assignment Enable bit */
    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT_ASSIGNMENT_ENABLE_E,
        value);
    logicFormatPtr->sourceEPortAssignmentEnable = BIT2BOOL_MAC(value);

    /* Source ePort: assigned by TTI entry when
      <Source ePort Assignment Enable> = Enabled */
    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_SOURCE_EPORT_E,
        value);
    logicFormatPtr->sourceEPort = value;

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_L_LSP_QOS_PROFILE_ENABLE_E,
        value);
    logicFormatPtr->mplsLLspQoSProfileEnable = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TT_HEADER_LENGTH_E,
        value);
    /* this is the Tunnel header length in units of Bytes. Granularity is in 2 Bytes. */
    value *= 2;
    logicFormatPtr->ttHeaderLength = value;

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_UP_CFI_TO_QOS_TABLE_SELECT_MODE_E,
        value);
    logicFormatPtr->qosUseUpAsIndexEnable = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TRUST_QOS_MAPPING_TABLE_INDEX_E,
        value);
    logicFormatPtr->qosMappingTableIndex = value;

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_SET_MAC2ME_E,
        value);
    logicFormatPtr->setMacToMe = BIT2BOOL_MAC(value);


    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_IS_PTP_PACKET_E,
        value);
    logicFormatPtr->isPtpPacket = BIT2BOOL_MAC(value);

    if (logicFormatPtr->isPtpPacket == GT_TRUE)
    {
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_TRIGGER_TYPE_E,
            value);

        switch (value)
        {
        case 0:
            logicFormatPtr->ptpTriggerType = CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_L2_E;
            break;
        case 1:
            logicFormatPtr->ptpTriggerType = CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_IPV4_UDP_E;
            break;
        case 2:
            logicFormatPtr->ptpTriggerType = CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_PTP_OVER_IPV6_UDP_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_PTP_PACKET_PTP_OFFSET_E,
            value);
        logicFormatPtr->ptpOffset = value;
    }
    else
    {
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_TIMESTAMP_ENABLE_E,
            value);
        logicFormatPtr->oamTimeStampEnable = BIT2BOOL_MAC(value);

        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_NON_PTP_PACKET_OFFSET_INDEX_E,
            value);
        logicFormatPtr->oamOffsetIndex = value;
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROCESSING_ENABLE_E,
        value);
    logicFormatPtr->oamProcessEnable = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_OAM_PROFILE_E,
        value);
    logicFormatPtr->oamProfile = value;

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_CHANNEL_TYPE_TO_OPCODE_MAPPING_EN_E,
        value);
    logicFormatPtr->oamChannelTypeToOpcodeMappingEnable = BIT2BOOL_MAC(value);

    /* The global enable bit is retained for BWC.  If enabled, it overrides the TTI Action */
    rc = cpssDxChPclL3L4ParsingOverMplsEnableGet(devNum,&enablePtr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (enablePtr == GT_FALSE)
    {
        LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
            LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_ENABLE_E,
            value);

        switch (value)
        {
        case 0:
            logicFormatPtr->passengerParsingOfTransitMplsTunnelMode = CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_DISABLE_E;
            break;
        case 1:
            logicFormatPtr->passengerParsingOfTransitMplsTunnelMode = CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_IP_E;
            break;
        case 2:
            logicFormatPtr->passengerParsingOfTransitMplsTunnelMode = CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_CW_ETH_E;
            break;
        case 3:
            logicFormatPtr->passengerParsingOfTransitMplsTunnelMode = CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_AS_ETH_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PASSENGER_PARSING_OF_NON_MPLS_TRANSIT_TUNNEL_ENABLE_E,
        value);
    logicFormatPtr->passengerParsingOfTransitNonMplsTransitTunnelEnable = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_RX_IS_PROTECTION_PATH_E,
        value);
    logicFormatPtr->rxIsProtectionPath = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_RX_ENABLE_PROTECTION_SWITCHING_E,
        value);
    logicFormatPtr->rxProtectionSwitchEnable = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PW_TAG_MODE_E,
        value);

    switch (value)
    {
    case 0:
        logicFormatPtr->pwTagMode = CPSS_DXCH_TTI_PW_TAG_DISABLED_MODE_E;
        break;
    case 1:
        logicFormatPtr->pwTagMode = CPSS_DXCH_TTI_PW_TAG_RAW_PW_MODE_E;
        break;
    case 2:
        logicFormatPtr->pwTagMode = CPSS_DXCH_TTI_PW_TAG_TAGGED_PW_MODE_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_ENABLE_TTI_1_LOOKUP_E,
        value);
    logicFormatPtr->continueToNextTtiLookup = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_CW_BASED_PSEUDO_WIRE_E,
        value);
    logicFormatPtr->cwBasedPw = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_TTL_EXPIRY_VCCV_ENABLE_E,
        value);
    logicFormatPtr->ttlExpiryVccvEnable = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PWE3_FLOW_LABEL_EXIST_E,
        value);
    logicFormatPtr->pwe3FlowLabelExist = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_PW_CW_BASED_E_TREE_ENABLE_E,
        value);
    logicFormatPtr->pwCwBasedETreeEnable = BIT2BOOL_MAC(value);

    LION3_TTI_ACTION_FIELD_GET_MAC(devNum,hwFormatArray,
        LION3_TTI_ACTION_TABLE_FIELDS_APPLY_NON_DATA_CW_COMMAND_E,
        value);
    logicFormatPtr->applyNonDataCwCommand = BIT2BOOL_MAC(value);

    return GT_OK;
}


/*******************************************************************************
* ttiActionLogic2HwFormat
*
* DESCRIPTION:
*       Converts a given tti action from logic format to hardware format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       actionType        - type of the action to use
*       logicFormatPtr    - points to tti action in logic format
*
* OUTPUTS:
*       hwFormatArray     - tti action in hardware format (4 words)
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiActionLogic2HwFormat
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT           *logicFormatPtr,
    OUT GT_U32                             *hwFormatArray
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);
    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);

    switch (actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            return ttiActionType1Logic2HwFormat(devNum,
                                                &logicFormatPtr->type1,
                                                hwFormatArray);
        }
        else
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            return ttiActionType2Logic2HwFormat(devNum,
                                                &logicFormatPtr->type2,
                                                hwFormatArray);
        }
        else
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    default:
        break;
    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

}

/*******************************************************************************
* prvCpssDxChTtiActionHw2LogicFormat
*
* DESCRIPTION:
*       Converts a given tti action type 2 from hardware format to
*       logic format.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       hwFormatArray     - tti action in hardware format (4 words)
*       actionType        - type of the action to use
*
* OUTPUTS:
*       logicFormatPtr    - points to tti action in logic format
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChTtiActionHw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *hwFormatArray,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *logicFormatPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(hwFormatArray);
    CPSS_NULL_PTR_CHECK_MAC(logicFormatPtr);

    switch (actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            return ttiActionType1Hw2LogicFormat(hwFormatArray,
                                                &logicFormatPtr->type1);
        }
        else
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            return ttiActionLion3Hw2LogicFormat(devNum, hwFormatArray,
                                                &logicFormatPtr->type2);
        }
        else
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            return ttiActionType2Hw2LogicFormat(devNum, hwFormatArray,
                                                &logicFormatPtr->type2);
        }
        else
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    default:
        break;
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* ttiGetIndexForKeyType
*
* DESCRIPTION:
*   Gets the index used in the HW for a specific key type.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       keyType              - the key type
*       isLegacyKeyTypeValid - GT_TRUE: a legacy keyType is valid
*                              GT_FALSE: a legacy keyType is not valid
*
* OUTPUTS:
*       indexPtr             - (pointer to) the index of the key type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong key type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiGetIndexForKeyType
(
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_BOOL                             isLegacyKeyTypeValid,
    OUT GT_U32                              *indexPtr
)
{
    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E:
        *indexPtr = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E:
        *indexPtr = 1;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_MPLS_E:
        *indexPtr = 2;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E:
        *indexPtr = 3;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E:
        *indexPtr = 4;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E:
        *indexPtr = 5;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_E:
        *indexPtr = 6;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E:
        *indexPtr = 7;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E:
        *indexPtr = 8;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE_E:
        *indexPtr = 9;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE1_E:
        *indexPtr = 10;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE2_E:
        *indexPtr = 11;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE3_E:
        *indexPtr = 12;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE4_E:
        *indexPtr = 13;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE5_E:
        *indexPtr = 14;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE6_E:
        *indexPtr = 15;
        break;
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        if (isLegacyKeyTypeValid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        *indexPtr = 16;
        break;
    case CPSS_DXCH_TTI_KEY_MPLS_E:
        if (isLegacyKeyTypeValid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        *indexPtr = 17;
        break;
    case CPSS_DXCH_TTI_KEY_MIM_E:
        if (isLegacyKeyTypeValid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        *indexPtr = 18;
        break;
    case CPSS_DXCH_TTI_KEY_ETH_E:
        if (isLegacyKeyTypeValid == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        *indexPtr = 19;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* ttiInitUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       devNum       - device number
*       keyType      - key Type
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*       offsetType   - the type of offset
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type. .(APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiInitUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_TTI_OFFSET_TYPE_ENT        offsetType,
    IN GT_U32                               offset
)
{
    GT_STATUS rc;              /* return code              */
    GT_U32    regValue;        /* value to update register */
    GT_U32    startBit;        /* start Bit                */
    GT_U32    udbEntryIndex;   /* HW UDB entry index       */
    GT_U32    portGroupId;     /*the port group Id         */
    GT_U32    udbEntryArray[UDB_CONFIG_NUM_WORDS_MAX_CNS];/* HW UDB entry */
    GT_U32  maxNumMetaDataBytes =  NUM_OF_META_DATA_BYTES_CNS;/* max number of bytes of metadata  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((offset >= BIT_7) || (udbIndex >= UDB_CONFIG_NUM_WORDS_MAX_CNS))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if(offsetType == CPSS_DXCH_TTI_OFFSET_METADATA_E && offset > maxNumMetaDataBytes)
    {
        /* the metadata not supports more then 32 bytes of info */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    rc = ttiGetIndexForKeyType(keyType, GT_TRUE, &udbEntryIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    regValue = (offset << 4);

    /* bit 0 means "valid" */
    switch (offsetType)
    {
        case CPSS_DXCH_TTI_OFFSET_L2_E:
            regValue |= (UDB_HW_L2_CNS << 1) | 1;
            break;
        case CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E:
            regValue |= (UDB_HW_L3_CNS << 1) | 1;
            break;
        case CPSS_DXCH_TTI_OFFSET_L4_E:
            regValue |= (UDB_HW_L4_CNS << 1) | 1;
            break;
        case CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E:
            regValue |= (UDB_HW_MPLS_CNS << 1) | 1;
            break;
        case CPSS_DXCH_TTI_OFFSET_METADATA_E:
            regValue |= (UDB_HW_METADATA_CNS << 1) | 1;
            break;
        case CPSS_DXCH_TTI_OFFSET_INVALID_E:
            /* L2 without valid bit*/
            regValue |= (UDB_HW_L2_CNS << 1);
            break;
        default: CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* read all packet type related 23-UDB entry */
        rc = prvCpssDxChPortGroupReadTableEntry(
            devNum, portGroupId,
            PRV_CPSS_DXCH_LION3_TABLE_TTI_UDB_CONFIG_E,
            udbEntryIndex, udbEntryArray);
        if (rc != GT_OK)
        {
            return rc;
        }

        startBit  =  (11 * udbIndex);
        /* update the needed 11 bits */
        U32_SET_FIELD_IN_ENTRY_MAC(udbEntryArray,startBit,11,regValue);

        /* Write updated packet type related 23-UDB entry */
        rc =  prvCpssDxChPortGroupWriteTableEntry(
            devNum, portGroupId,
            PRV_CPSS_DXCH_LION3_TABLE_TTI_UDB_CONFIG_E,
            udbEntryIndex, udbEntryArray);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;

}

/*******************************************************************************
* ttiInitUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       keyType      - key Type
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiInitUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_TTI_OFFSET_TYPE_ENT       *offsetTypePtr,
    OUT GT_U32                              *offsetPtr
)
{
    GT_STATUS rc;               /* return code              */
    GT_U32    regValue;         /* value to update register */
    GT_U32    startBit;         /* start Bit                */
    GT_U32    udbEntryIndex;    /* HW UDB entry index       */
    GT_U32    portGroupId;      /*the port group Id         */
    GT_U32    udbEntryArray[UDB_CONFIG_NUM_WORDS_MAX_CNS]; /* HW UDB entry */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(offsetTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (udbIndex >= UDB_CONFIG_NUM_WORDS_MAX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    rc = ttiGetIndexForKeyType(keyType, GT_TRUE, &udbEntryIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    /* read all packet type related 30-UDB entry */
    rc = prvCpssDxChPortGroupReadTableEntry(
        devNum, portGroupId,
        PRV_CPSS_DXCH_LION3_TABLE_TTI_UDB_CONFIG_E,
        udbEntryIndex, udbEntryArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    startBit = (11 * udbIndex);
    /* get the needed 11 bits */
    U32_GET_FIELD_IN_ENTRY_MAC(udbEntryArray,startBit,11,regValue);

    *offsetPtr = ((regValue >> 4) & 0x7F);

    if ((regValue & 1) == 0)
    {
        /* invalid UDB */
        *offsetTypePtr = CPSS_DXCH_TTI_OFFSET_INVALID_E;
    }
    else
    {
        switch ((regValue >> 1) & 7)
        {
         case UDB_HW_L2_CNS:
             *offsetTypePtr = CPSS_DXCH_TTI_OFFSET_L2_E;
             break;
         case UDB_HW_L3_CNS:
             *offsetTypePtr = CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E;
             break;
         case UDB_HW_L4_CNS:
             *offsetTypePtr = CPSS_DXCH_TTI_OFFSET_L4_E;
             break;
         case UDB_HW_MPLS_CNS:
             *offsetTypePtr = CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E;
             break;
         case UDB_HW_METADATA_CNS:
             *offsetTypePtr = CPSS_DXCH_TTI_OFFSET_METADATA_E;
             break;
         default:
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* ttiKeyLookupBitLengthGet
*
* DESCRIPTION:
*       This function gets field name and sub bit offset for the specified key
*       type.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       keyType       - TTI key type
*
* OUTPUTS:
*       fieldNamePtr      - (pointer to) field name (type) in table
*       subFieldOffsetPtr - (pointer to) sub field offset within the (primary) field.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiKeyLookupBitLengthGet
(
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_ENT *fieldNamePtr,
    OUT GT_U32                              *subFieldOffsetPtr
)
{
     /* check parameters */
    GT_U32 bit;
    LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_ENT fieldName;

    CPSS_NULL_PTR_CHECK_MAC(fieldNamePtr);
    CPSS_NULL_PTR_CHECK_MAC(subFieldOffsetPtr);

    bit = 0;/* by default start from bit 0 of the primary bit */

    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        fieldName = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_TUNNEL_TERMINATION_EN_E;
        break;
    case CPSS_DXCH_TTI_KEY_MPLS_E:
        fieldName = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_MPLS_TUNNEL_TERMINATION_EN_E;
        break;
    case CPSS_DXCH_TTI_KEY_ETH_E:
        fieldName = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_ETHERNET_CLASSIFIER_EN_E;
        break;
    case CPSS_DXCH_TTI_KEY_MIM_E:
        fieldName = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_EN_802_1AH_TTI_LOOKUP_E;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E:
    case CPSS_DXCH_TTI_KEY_UDB_MPLS_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E:
    case CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE1_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE2_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE3_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE4_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE5_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE6_E:
        fieldName = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_PKT_TYPE_UDB_KEY_I_ENABLE_E;
        bit = keyType - CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *subFieldOffsetPtr = bit;
    *fieldNamePtr = fieldName;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChTtiMacToMeSet
*
* DESCRIPTION:
*       function sets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *maskPtr,
    IN  CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
)
{
    return cpssDxChTtiPortGroupMacToMeSet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,entryIndex,valuePtr,maskPtr,interfaceInfoPtr);
}

/*******************************************************************************
* cpssDxChTtiMacToMeSet
*
* DESCRIPTION:
*       function sets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *maskPtr,
    IN  CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMacToMeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, valuePtr, maskPtr, interfaceInfoPtr));

    rc = internal_cpssDxChTtiMacToMeSet(devNum, entryIndex, valuePtr, maskPtr, interfaceInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, valuePtr, maskPtr, interfaceInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiMacToMeGet
*
* DESCRIPTION:
*       This function gets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    OUT CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
)
{
    return cpssDxChTtiPortGroupMacToMeGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,entryIndex,valuePtr,maskPtr,interfaceInfoPtr);
}

/*******************************************************************************
* cpssDxChTtiMacToMeGet
*
* DESCRIPTION:
*       This function gets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    OUT CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMacToMeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, valuePtr, maskPtr, interfaceInfoPtr));

    rc = internal_cpssDxChTtiMacToMeGet(devNum, entryIndex, valuePtr, maskPtr, interfaceInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, valuePtr, maskPtr, interfaceInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       keyType       - TTI key type
*       enable        - GT_TRUE: enable TTI lookup
*                       GT_FALSE: disable TTI lookup
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortLookupEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_BOOL                             enable
)
{

    GT_U32      value;      /* value to write */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc = GT_OK; /* function return code */
    LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_ENT fieldName;
    GT_U32      subFiledOffset; /* the sub field offset */

     /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

     /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        rc = ttiKeyLookupBitLengthGet(keyType, &fieldName, &subFiledOffset);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write to pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_SUB_FIELD_IN_FIELD_NAME_CNS,/* use of sub field ! */
                                            fieldName,   /* field name */
                                            PRV_CPSS_DXCH_TABLE_SUB_FIELD_OFFSET_AND_LENGTH_MAC(subFiledOffset,1),/* sub field info {offset,length}*/
                                            value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* DxCh3 */
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            switch (keyType)
            {
            case CPSS_DXCH_TTI_KEY_IPV4_E:
                bit = 49;
                break;
            case CPSS_DXCH_TTI_KEY_MPLS_E:
                bit = 50;
                break;
            case CPSS_DXCH_TTI_KEY_ETH_E:
                bit = 51;
                break;
            default:
                if ((keyType == CPSS_DXCH_TTI_KEY_MIM_E) && (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))) /* xCat A0 */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                else
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        else  /* xCat; xCat3; Lion; xCat2; Lion2 */
        {
            switch (keyType)
            {
            case CPSS_DXCH_TTI_KEY_IPV4_E:
                bit = 51;
                break;
            case CPSS_DXCH_TTI_KEY_MPLS_E:
                bit = 55;
                break;
            case CPSS_DXCH_TTI_KEY_ETH_E:
                bit = 57;
                break;
            case CPSS_DXCH_TTI_KEY_MIM_E:
                bit = 60;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }

        /* write value to hardware table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             bit,/* start bit */
                                             1,             /* length */
                                             value);
    }

    return rc;

}

/*******************************************************************************
* cpssDxChTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       keyType       - TTI key type
*       enable        - GT_TRUE: enable TTI lookup
*                       GT_FALSE: disable TTI lookup
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortLookupEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortLookupEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, keyType, enable));

    rc = internal_cpssDxChTtiPortLookupEnableSet(devNum, portNum, keyType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, keyType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTtiPortLookupEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of  the
*       TTI lookup for the specified key type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       keyType       - TTI key type
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup
*                       GT_TRUE: TTI lookup is enabled
*                       GT_FALSE: TTI lookup is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id, port or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortLookupEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc = GT_OK; /* function return code */
    LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_ENT fieldName;
    GT_U32      subFiledOffset; /* the sub field offset */

     /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        rc = ttiKeyLookupBitLengthGet(keyType, &fieldName, &subFiledOffset);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* read from pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_SUB_FIELD_IN_FIELD_NAME_CNS,/* use of sub field ! */
                                            fieldName,   /* field name */
                                            PRV_CPSS_DXCH_TABLE_SUB_FIELD_OFFSET_AND_LENGTH_MAC(subFiledOffset,1),/* sub field info {offset,length}*/
                                            &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        /* DxCh3 */
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            switch (keyType)
            {
            case CPSS_DXCH_TTI_KEY_IPV4_E:
                bit = 49;
                break;
            case CPSS_DXCH_TTI_KEY_MPLS_E:
                bit = 50;
                break;
            case CPSS_DXCH_TTI_KEY_ETH_E:
                bit = 51;
                break;
            default:
                if ((keyType == CPSS_DXCH_TTI_KEY_MIM_E) && (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))) /* xCat A0 */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
                else
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        else  /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        {
            switch (keyType)
            {
            case CPSS_DXCH_TTI_KEY_IPV4_E:
                bit = 51;
                break;
            case CPSS_DXCH_TTI_KEY_MPLS_E:
                bit = 55;
                break;
            case CPSS_DXCH_TTI_KEY_ETH_E:
                bit = 57;
                break;
            case CPSS_DXCH_TTI_KEY_MIM_E:
                bit = 60;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }

        /* read value from hardware table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             bit,     /* offset */
                                             1,             /* length */
                                             &value);
    }
    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortLookupEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of  the
*       TTI lookup for the specified key type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       keyType       - TTI key type
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup
*                       GT_TRUE: TTI lookup is enabled
*                       GT_FALSE: TTI lookup is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id, port or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortLookupEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortLookupEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, keyType, enablePtr));

    rc = internal_cpssDxChTtiPortLookupEnableGet(devNum, portNum, keyType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, keyType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for tunneled packets
*                       GT_FALSE: disable IPv4 TTI lookup only for tunneled packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc=GT_OK;   /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_TTI_FOR_TT_ONLY_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            bit = 54;
        }
        else  /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        {
            bit = 52;
        }

        /* write value to hardware table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             bit,    /* offset */
                                             1,             /* length */
                                             value);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for tunneled packets
*                       GT_FALSE: disable IPv4 TTI lookup only for tunneled packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortIpv4OnlyTunneledEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChTtiPortIpv4OnlyTunneledEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortIpv4OnlyTunneledEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only tunneled packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       tunneled packets
*                       GT_TRUE: IPv4 TTI lookup only for tunneled packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for tunneled packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortIpv4OnlyTunneledEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc=GT_OK;   /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read from pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_TTI_FOR_TT_ONLY_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &value);

    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            bit = 54;
        }
        else  /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        {
            bit = 52;
        }

        /* read hardware value */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            bit,/* start bit */
                                            1,              /* length */
                                            &value);
    }

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only tunneled packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       tunneled packets
*                       GT_TRUE: IPv4 TTI lookup only for tunneled packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for tunneled packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyTunneledEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortIpv4OnlyTunneledEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChTtiPortIpv4OnlyTunneledEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for mac to me packets
*                       GT_FALSE: disable IPv4 TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc=GT_OK;   /* function return code */

     /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E );

    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_IPV4_MAC_TO_ME_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            bit = 53;
        }
        else  /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        {
            bit = 54;
        }

        /* write value to hardware table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             bit,/* start bit */
                                             1,             /* length */
                                             value);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for mac to me packets
*                       GT_FALSE: disable IPv4 TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortIpv4OnlyMacToMeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: IPv4 TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc=GT_OK;   /* function return code */

     /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E );

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read from pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_IPV4_MAC_TO_ME_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            bit = 53;
        }
        else  /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        {
            bit = 54;
        }

        /* read hardware value */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            bit,/* start bit */
                                            1,              /* length */
                                            &value);
    }
    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: IPv4 TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpv4OnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortIpv4OnlyMacToMeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTtiIpv4McEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for IPv4 multicast
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: enable TTI lookup for IPv4 MC
*                       GT_FALSE: disable TTI lookup for IPv4 MC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiIpv4McEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{

    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    /* write value */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,23,1,value);
    }
    else /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIEngineConfig;
            rc = prvCpssHwPpSetRegField(devNum,regAddr,2,1,value);
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
            rc = prvCpssHwPpSetRegField(devNum,regAddr,5,1,value);
        }
    }

    return rc;


}

/*******************************************************************************
* cpssDxChTtiIpv4McEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for IPv4 multicast
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: enable TTI lookup for IPv4 MC
*                       GT_FALSE: disable TTI lookup for IPv4 MC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4McEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiIpv4McEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChTtiIpv4McEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiIpv4McEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of TTI lookup for
*       IPv4 multicast (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup for IPv4 MC
*                       GT_TRUE: TTI lookup for IPv4 MC enabled
*                       GT_FALSE: TTI lookup for IPv4 MC disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiIpv4McEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      value = 0;  /* hardware write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* read value */
    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        rc = prvCpssHwPpGetRegField(devNum,regAddr,23,1,&value);
    }
    else /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIEngineConfig;
            rc = prvCpssHwPpGetRegField(devNum,regAddr,2,1,&value);
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
            rc = prvCpssHwPpGetRegField(devNum,regAddr,5,1,&value);
        }
    }

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiIpv4McEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of TTI lookup for
*       IPv4 multicast (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup for IPv4 MC
*                       GT_TRUE: TTI lookup for IPv4 MC enabled
*                       GT_FALSE: TTI lookup for IPv4 MC disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpv4McEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiIpv4McEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChTtiIpv4McEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortMplsOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MPLS TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable MPLS TTI lookup only for mac to me packets
*                       GT_FALSE: disable MPLS TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortMplsOnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

     /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_MPLS_MAC_TO_ME_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            bit = 52;
        }
        else  /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        {
            bit = 56;
        }

        /* write value to hardware table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             bit,/* start bit */
                                             1,             /* length */
                                             value);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MPLS TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable MPLS TTI lookup only for mac to me packets
*                       GT_FALSE: disable MPLS TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortMplsOnlyMacToMeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChTtiPortMplsOnlyMacToMeEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortMplsOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MPLS TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: MPLS TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MPLS TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortMplsOnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* hardware value */
    GT_U32      bit;        /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read from pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_MPLS_MAC_TO_ME_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            bit = 52;
        }
        else  /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        {
            bit = 56;
        }

        /* read hardware value */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            bit,/* start bit */
                                            1,              /* length */
                                            &value);
    }

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MPLS TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: MPLS TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MPLS TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMplsOnlyMacToMeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortMplsOnlyMacToMeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChTtiPortMplsOnlyMacToMeEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortMimOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MIM TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE:  enable MIM TTI lookup only for mac to me packets
*                       GT_FALSE: disable MIM TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortMimOnlyMacToMeEnableSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_BOOL         enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      bit = 0;    /* bit in table   */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write to pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_802_1AH_MAC_TO_ME_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        bit = 61;

        /* write value to hardware table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                             bit,/* start bit */
                                             1,             /* length */
                                             value);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MIM TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE:  enable MIM TTI lookup only for mac to me packets
*                       GT_FALSE: disable MIM TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableSet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_BOOL         enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortMimOnlyMacToMeEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChTtiPortMimOnlyMacToMeEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortMimOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MIM TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable MIM TTI lookup only for
*                       mac to me packets
*                       GT_TRUE:  MIM TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MIM TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortMimOnlyMacToMeEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_U32      value=0;    /* hardware value */
    GT_U32      bit = 0;    /* bit in table   */
    GT_STATUS   rc=GT_OK;   /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read from pre-tti-lookup-ingress-eport table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_802_1AH_MAC_TO_ME_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &value);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

        bit = 61;

        /* read hardware value */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            bit,/* start bit */
                                            1,              /* length */
                                            &value);
    }

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MIM TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable MIM TTI lookup only for
*                       mac to me packets
*                       GT_TRUE:  MIM TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MIM TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortMimOnlyMacToMeEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortMimOnlyMacToMeEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChTtiPortMimOnlyMacToMeEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    return cpssDxChTtiPortGroupRuleSet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,
        ruleType,patternPtr,maskPtr,actionType,actionPtr);
}

/*******************************************************************************
* cpssDxChTtiRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiRuleSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, ruleType, patternPtr, maskPtr, actionType, actionPtr));

    rc = internal_cpssDxChTtiRuleSet(devNum, index, ruleType, patternPtr, maskPtr, actionType, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, ruleType, patternPtr, maskPtr, actionType, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_STATE             - a rule of different type was found in the specified index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    return cpssDxChTtiPortGroupRuleGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,ruleType,
        patternPtr,maskPtr,actionType,actionPtr);
}

/*******************************************************************************
* cpssDxChTtiRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_STATE             - a rule of different type was found in the specified index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiRuleGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, ruleType, patternPtr, maskPtr, actionType, actionPtr));

    rc = internal_cpssDxChTtiRuleGet(devNum, index, ruleType, patternPtr, maskPtr, actionType, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, ruleType, patternPtr, maskPtr, actionType, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       index            - Global index of the rule in the TCAM
*       actionType       - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    return cpssDxChTtiPortGroupRuleActionUpdate(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,
        actionType,actionPtr);
}

/*******************************************************************************
* cpssDxChTtiRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       index            - Global index of the rule in the TCAM
*       actionType       - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiRuleActionUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, actionType, actionPtr));

    rc = internal_cpssDxChTtiRuleActionUpdate(devNum, index, actionType, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, actionType, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  GT_BOOL                             valid
)
{
    return cpssDxChTtiPortGroupRuleValidStatusSet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,valid);
}

/*******************************************************************************
* cpssDxChTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    IN  GT_BOOL                             valid
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiRuleValidStatusSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, valid));

    rc = internal_cpssDxChTtiRuleValidStatusSet(devNum, index, valid);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, valid));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    OUT GT_BOOL                             *validPtr
)
{
    return cpssDxChTtiPortGroupRuleValidStatusGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,validPtr);
}

/*******************************************************************************
* cpssDxChTtiRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       index             - Global index of the rule in the TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              index,
    OUT GT_BOOL                             *validPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiRuleValidStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, validPtr));

    rc = internal_cpssDxChTtiRuleValidStatusGet(devNum, index, validPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, validPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMacModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
{
    return cpssDxChTtiPortGroupMacModeSet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,keyType,macMode);
}

/*******************************************************************************
* cpssDxChTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMacModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, macMode));

    rc = internal_cpssDxChTtiMacModeSet(devNum, keyType, macMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, macMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMacModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
{
    return cpssDxChTtiPortGroupMacModeGet(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,keyType,macModePtr);
}

/*******************************************************************************
* cpssDxChTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMacModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMacModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, macModePtr));

    rc = internal_cpssDxChTtiMacModeGet(devNum, keyType, macModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, macModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPclIdSet
*
* DESCRIPTION:
*       This function sets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type
*       pclId         - PCL ID value (10 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPclIdSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_U32                          pclId
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      offset;         /* offset in register */
    GT_STATUS   rc;             /* function return code */
    GT_U32      entryIndex;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if(pclId >= BIT_10)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPCLIDConfig0;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        }
        offset  = 0;
        break;
    case CPSS_DXCH_TTI_KEY_MPLS_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPCLIDConfig0;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        }
        offset  = 10;
        break;
    case CPSS_DXCH_TTI_KEY_ETH_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPCLIDConfig0;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        }
        offset  = 20;
        break;
    case CPSS_DXCH_TTI_KEY_MIM_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPCLIDConfig1;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig1;
        }
        offset  = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 0;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 1;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_MPLS_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 2;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 3;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 4;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 5;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 6;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 7;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 8;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 9;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE1_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 10;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE2_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 11;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE3_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 12;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE4_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 13;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE5_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 14;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE6_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 15;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* write value */
    rc = prvCpssHwPpSetRegField(devNum,regAddr,offset,10,pclId);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPclIdSet
*
* DESCRIPTION:
*       This function sets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type
*       pclId         - PCL ID value (10 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPclIdSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_U32                          pclId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPclIdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, pclId));

    rc = internal_cpssDxChTtiPclIdSet(devNum, keyType, pclId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, pclId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPclIdGet
*
* DESCRIPTION:
*       This function gets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type
*
* OUTPUTS:
*       pclIdPtr      - (points to) PCL ID value (10 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPclIdGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    OUT GT_U32                          *pclIdPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in register */
    GT_STATUS   rc;         /* function return code */
    GT_U32      entryIndex;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(pclIdPtr);

    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPCLIDConfig0;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        }
        offset  = 0;
        break;
    case CPSS_DXCH_TTI_KEY_MPLS_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPCLIDConfig0;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        }
        offset  = 10;
        break;
    case CPSS_DXCH_TTI_KEY_ETH_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPCLIDConfig0;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig0;
        }
        offset  = 20;
        break;
    case CPSS_DXCH_TTI_KEY_MIM_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPCLIDConfig1;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.pclIdConfig1;
        }
        offset  = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 0;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 1;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_MPLS_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 2;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 3;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 4;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 5;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 6;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 7;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 8;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 9;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE1_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 10;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE2_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 11;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE3_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 12;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE4_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 13;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE5_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 14;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 0;
        break;
    case CPSS_DXCH_TTI_KEY_UDB_UDE6_E:
        PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
        entryIndex = 15;
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];
        offset = 16;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* read value */
    rc = prvCpssHwPpGetRegField(devNum,regAddr,offset,10,pclIdPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPclIdGet
*
* DESCRIPTION:
*       This function gets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type
*
* OUTPUTS:
*       pclIdPtr      - (points to) PCL ID value (10 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPclIdGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType,
    OUT GT_U32                          *pclIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPclIdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, pclIdPtr));

    rc = internal_cpssDxChTtiPclIdGet(devNum, keyType, pclIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, pclIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* ttiExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command register address, offset and legnth.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       regAddressPtr - (pointer to) register address
*       offsetPtr     - (pointer to) offset in the register
*       lenPtr        - (pointer to) length in the register
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS ttiExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT GT_U32                              *regAddressPtr,
    OUT GT_U32                              *offsetPtr,
    OUT GT_U32                              *lenPtr
)
{
    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        *lenPtr = 3;
    }
    else
    {
        *lenPtr = 1;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* find register offset according to the exception type */
        switch (exceptionType)
        {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E:
            *offsetPtr = 0;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E:
            *offsetPtr = 3;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E:
            *offsetPtr = 9;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E:
            *offsetPtr = 12;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E:
            *offsetPtr = 15;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E:
            *offsetPtr = 18;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E:
            *offsetPtr = 21;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E:
            *offsetPtr = 24;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E:
            *offsetPtr = 27;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E:
            *offsetPtr = 0;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupMPLSExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E:
            *offsetPtr = 3;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupMPLSExceptionCommands;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E:
            *offsetPtr = 0;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E:
            *offsetPtr = 3;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E:
            *offsetPtr = 6;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E:
            *offsetPtr = 9;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E:
            *offsetPtr = 12;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E:
            *offsetPtr = 15;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E:
            *offsetPtr = 18;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E:
            *offsetPtr = 21;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E:
            *offsetPtr = 24;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E:
            *offsetPtr = 27;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E:
            *offsetPtr = 0;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands1;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E:
            *offsetPtr = 3;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands1;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E:
            *offsetPtr = 6;
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLExceptionCommands1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    /* DxCh3 */
    else if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* find register offset according to the exception type */
        switch (exceptionType)
        {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E:
            *offsetPtr = 19;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E:
            *offsetPtr = 20;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E:
            *offsetPtr = 21;
            break;
        default:
            if (((exceptionType == CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E) ||
                (exceptionType == CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E)) &&
                (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            else
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        *regAddressPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
    }
    else /* xCat; xCat3; Lion; xCat2; Lion2 */
    {
        /* find register offset according to the exception type */
        switch (exceptionType)
        {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E:
            *offsetPtr = 4;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E:
            *offsetPtr = 9;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E:
            *offsetPtr = 6;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E:
            *offsetPtr = 7;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E:
            *offsetPtr = 8;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        *regAddressPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChTtiExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Bobcat2, Caelum, Bobcat3:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*       For DxCh3, xCat, xCat3, Lion, xCat2, Lion2:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are:
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E
*           The commands are:
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
)
{
    GT_STATUS   rc = GT_OK; /* function return code */
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in the register */
    GT_U32      len;        /* length */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    /* note the commands for all exception types are the same  */
    /* get the value to write according to command             */
    switch (exceptionType)
    {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E:
            switch (command)
            {
                case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
                    {
                        PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,command);
                    }
                    else
                    {
                        value = 0;
                    }
                    break;
                case CPSS_PACKET_CMD_DROP_HARD_E:
                    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
                    {
                        PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,command);
                    }
                    else
                    {
                        value = 1;
                    }
                    break;
                default:
                    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
                    {
                        PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,command);
                    }
                    else
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
            }
            break;

        case CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E:
        case CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,command);
            }
            else
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = ttiExceptionCmdGet(devNum,exceptionType,&regAddr,&offset,&len);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* write command value */
    rc = prvCpssHwPpSetRegField(devNum,regAddr,offset,len,value);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Bobcat2, Caelum, Bobcat3:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*       For DxCh3, xCat, xCat3, Lion, xCat2, Lion2:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are:
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E
*           The commands are:
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiExceptionCmdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, exceptionType, command));

    rc = internal_cpssDxChTtiExceptionCmdSet(devNum, exceptionType, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, exceptionType, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Bobcat2, Caelum, Bobcat3:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*       For DxCh3, xCat, xCat3, Lion, xCat2, Lion2:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are:
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E
*           The commands are:
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
{
    GT_STATUS   rc;         /* function return code */
    GT_U32      value;      /* register value */
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in the register */
    GT_U32      len;        /* length */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(commandPtr);


    rc = ttiExceptionCmdGet(devNum,exceptionType,&regAddr,&offset,&len);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* read command value */
    rc = prvCpssHwPpGetRegField(devNum,regAddr,offset,len,&value);
    if (rc != GT_OK)
        return rc;

    switch (exceptionType)
    {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E:
        case CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC((*commandPtr),value);
            }
            else
            {
                switch (value)
                {
                case 0:
                    *commandPtr = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
                    break;
                case 1:
                    *commandPtr = CPSS_PACKET_CMD_DROP_HARD_E;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }
            break;

        default:
            PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(*commandPtr, value);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTtiExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Bobcat2, Caelum, Bobcat3:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*       For DxCh3, xCat, xCat3, Lion, xCat2, Lion2:
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_HEADER_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_OPTION_FRAG_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_IPV4_UNSUP_GRE_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_ILLEGAL_TTL_ERROR_E
*               CPSS_DXCH_TTI_EXCEPTION_MPLS_UNSUPPORTED_ERROR_E
*           The commands are:
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*           For the following exceptions:
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_IS_IS_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_TREE_ADJACENCY_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_VERSION_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_HOPCOUNT_IS_ZERO_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_CHBH_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_BAD_OUTER_VID0_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_NOT_TO_ME_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_UC_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OUTER_UC_INNER_MC_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_MC_WITH_BAD_OUTER_DA_E
*               CPSS_DXCH_TTI_EXCEPTION_TRILL_OPTIONS_TOO_LONG_OPTION_E
*           The commands are:
*               CPSS_PACKET_CMD_FORWARD_E
*               CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiExceptionCmdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, exceptionType, commandPtr));

    rc = internal_cpssDxChTtiExceptionCmdGet(devNum, exceptionType, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, exceptionType, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* ttiExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get tunnel termination exception CPU code register address and offset.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       regAddressPtr - (pointer to) register address
*       offsetPtr     - (pointer to) offset in the register
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ttiExceptionCpuCodeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT GT_U32                              *regAddressPtr,
    OUT GT_U32                              *offsetPtr
)
{
    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(regAddressPtr);
    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);

    switch (exceptionType)
    {
        case CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E:
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCodes0;
            *offsetPtr = 0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E:
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCodes0;
            *offsetPtr = 8;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E:
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCodes0;
            *offsetPtr = 16;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E:
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCodes0;
            *offsetPtr = 24;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E:
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCodes1;
            *offsetPtr = 0;
            break;
        case CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E:
            *regAddressPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTILookupIPExceptionCodes1;
            *offsetPtr = 8;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChTtiExceptionCpuCodeSet
*
* DESCRIPTION:
*       Set tunnel termination exception CPU code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set CPU code for.
*                       valid values:
*                       CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*       code          - the code for the exception type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiExceptionCpuCodeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    IN  CPSS_NET_RX_CPU_CODE_ENT            code
)
{
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;
    GT_U32 regAddr, offset;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = ttiExceptionCpuCodeGet(devNum, exceptionType, &regAddr, &offset);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChNetIfCpuToDsaCode(code, &dsaCpuCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 8, (GT_U32)dsaCpuCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiExceptionCpuCodeSet
*
* DESCRIPTION:
*       Set tunnel termination exception CPU code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set CPU code for.
*                       valid values:
*                       CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*       code          - the code for the exception type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCpuCodeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    IN  CPSS_NET_RX_CPU_CODE_ENT            code
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiExceptionCpuCodeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, exceptionType, code));

    rc = internal_cpssDxChTtiExceptionCpuCodeSet(devNum, exceptionType, code);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, exceptionType, code));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get tunnel termination exception CPU code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to get CPU code for.
*                       valid values:
*                       CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*
* OUTPUTS:
*       codePtr    - (points to) the code for the exception type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiExceptionCpuCodeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT CPSS_NET_RX_CPU_CODE_ENT            *codePtr
)
{
    GT_U32 regAddr, offset, value;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(codePtr);

    rc = ttiExceptionCpuCodeGet(devNum, exceptionType, &regAddr, &offset);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 8, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChNetIfDsaToCpuCode((PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)value,
                                      codePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get tunnel termination exception CPU code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to get CPU code for.
*                       valid values:
*                       CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*
* OUTPUTS:
*       codePtr    - (points to) the code for the exception type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiExceptionCpuCodeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_EXCEPTION_ENT         exceptionType,
    OUT CPSS_NET_RX_CPU_CODE_ENT            *codePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiExceptionCpuCodeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, exceptionType, codePtr));

    rc = internal_cpssDxChTtiExceptionCpuCodeGet(devNum, exceptionType, codePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, exceptionType, codePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function globally enables/disables bypassing IPv4 header length criteria checks as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_BOOL                             enable
)
{
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    /* check revision */
    if (GT_FALSE == PRV_CPSS_DXCH_XCAT_B1_ONLY_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

     /* prepare value to write */
    value = BOOL2BIT_MAC(enable);

    /* write value to hardware register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiMetalFix;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 18, 1, value);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function globally enables/disables bypassing IPv4 header length criteria checks as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function gets the globally bypassing IPv4 header length criteria check as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable bypass header length check
*                       GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
(
    IN  GT_U8                               devNum,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* check revision */
   if (GT_FALSE == PRV_CPSS_DXCH_XCAT_B1_ONLY_CHECK_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiMetalFix;
    rc = prvCpssHwPpGetRegField(devNum,regAddr,18,1,&value);

    if (rc != GT_OK)
        return rc;

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function gets the globally bypassing IPv4 header length criteria check as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable bypass header length check
*                       GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
(
    IN  GT_U8                               devNum,
    OUT GT_BOOL                             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortIpTotalLengthDeductionEnableSet
*
* DESCRIPTION:
*       For MACSEC packets over IPv4/6 tunnel, that are to be tunnel terminated,
*       this configuration enables aligning the IPv4/6 total header length to the
*       correct offset.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort.
*       enable  - Enable/disable Ipv4/6 Total Length Deduction.
*                 GT_TRUE: When enabled, and ePort default attribute
*                         <IPv4/6 Total Length Deduction Enable> == Enabled:
*                         - for IPv4:
*                           <IPv4 Total Length> = Packet IPv4 header <Total Length> -
*                           Global configuration < IPv4 Total Length Deduction Value>
*                         - for IPv6:
*                           <IPv6 Total Length> = Packet IPv6 header <Total Length> -
*                           Global configuration < IPv6 Total Length Deduction Value>
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortIpTotalLengthDeductionEnableSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_U32    hwData;      /* value to write */

     /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    hwData = BOOL2BIT_MAC(enable);

    /* set Pre-TTI-Lookup-Ingress-ePort table */
    return prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_6_TOTAL_LENGTH_DEDUCTION_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            hwData);
}

/*******************************************************************************
* cpssDxChTtiPortIpTotalLengthDeductionEnableSet
*
* DESCRIPTION:
*       For MACSEC packets over IPv4/6 tunnel, that are to be tunnel terminated,
*       this configuration enables aligning the IPv4/6 total header length to the
*       correct offset.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort.
*       enable  - Enable/disable Ipv4/6 Total Length Deduction.
*                 GT_TRUE: When enabled, and ePort default attribute
*                         <IPv4/6 Total Length Deduction Enable> == Enabled:
*                         - for IPv4:
*                           <IPv4 Total Length> = Packet IPv4 header <Total Length> -
*                           Global configuration < IPv4 Total Length Deduction Value>
*                         - for IPv6:
*                           <IPv6 Total Length> = Packet IPv6 header <Total Length> -
*                           Global configuration < IPv6 Total Length Deduction Value>
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpTotalLengthDeductionEnableSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortIpTotalLengthDeductionEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChTtiPortIpTotalLengthDeductionEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortIpTotalLengthDeductionEnableGet
*
* DESCRIPTION:
*       Get if IPv4/6 total header length is aligned to the correct offset
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr -  Enable/disable Ipv4/6 Total Length Deduction.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortIpTotalLengthDeductionEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    hwData;

     /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* read from Pre-TTI-Lookup-Ingress-ePort table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_6_TOTAL_LENGTH_DEDUCTION_EN_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &hwData);

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortIpTotalLengthDeductionEnableGet
*
* DESCRIPTION:
*       Get if IPv4/6 total header length is aligned to the correct offset
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr -  Enable/disable Ipv4/6 Total Length Deduction.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortIpTotalLengthDeductionEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortIpTotalLengthDeductionEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChTtiPortIpTotalLengthDeductionEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiIpTotalLengthDeductionValueSet
*
* DESCRIPTION:
*       Set Global configuration IPv4 or IPv6 Total Length Deduction Value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       ipType - IPv4 or Ipv6; valid values:
*                   CPSS_IP_PROTOCOL_IPV4_E
*                   CPSS_IP_PROTOCOL_IPV6_E
*       value  - IPv4 or IPv6 Total Length Deduction Value .(APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiIpTotalLengthDeductionValueSet
(
    IN GT_U8                                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT           ipType,
    IN GT_U32                               value
)
{
    GT_STATUS rc=GT_OK;    /* function return code */
    GT_U32    regAddr;     /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(value >= BIT_6)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).IPLengthChecks;

    switch (ipType)
    {
    case CPSS_IP_PROTOCOL_IPV4_E:
        rc = prvCpssHwPpSetRegField(devNum,regAddr,0,6,value);
        break;

    case CPSS_IP_PROTOCOL_IPV6_E:
        rc = prvCpssHwPpSetRegField(devNum,regAddr,6,6,value);
        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTtiIpTotalLengthDeductionValueSet
*
* DESCRIPTION:
*       Set Global configuration IPv4 or IPv6 Total Length Deduction Value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       ipType - IPv4 or Ipv6; valid values:
*                   CPSS_IP_PROTOCOL_IPV4_E
*                   CPSS_IP_PROTOCOL_IPV6_E
*       value  - IPv4 or IPv6 Total Length Deduction Value .(APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpTotalLengthDeductionValueSet
(
    IN GT_U8                                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT           ipType,
    IN GT_U32                               value
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiIpTotalLengthDeductionValueSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ipType, value));

    rc = internal_cpssDxChTtiIpTotalLengthDeductionValueSet(devNum, ipType, value);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ipType, value));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiIpTotalLengthDeductionValueGet
*
* DESCRIPTION:
*       Get Global configuration IPv4 or IPv6 Total Length Deduction Value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       ipType - IPv4 or Ipv6; valid values:
*                   CPSS_IP_PROTOCOL_IPV4_E
*                   CPSS_IP_PROTOCOL_IPV6_E
*
* OUTPUTS:
*       valuePtr -  (pointer to) IPv4 or IPv6 Total Length Deduction Value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiIpTotalLengthDeductionValueGet
(
    IN  GT_U8                               devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT           ipType,
    OUT GT_U32                              *valuePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    hwData;
    GT_U32    regAddr;    /* register address */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).IPLengthChecks;

    switch (ipType)
    {
    case CPSS_IP_PROTOCOL_IPV4_E:
        rc = prvCpssHwPpGetRegField(devNum,regAddr,0,6,&hwData);
        break;

    case CPSS_IP_PROTOCOL_IPV6_E:
        rc = prvCpssHwPpGetRegField(devNum,regAddr,6,6,&hwData);
        break;

    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(rc!=GT_OK)
        return rc;

    *valuePtr = hwData;

    return rc;
}

/*******************************************************************************
* cpssDxChTtiIpTotalLengthDeductionValueGet
*
* DESCRIPTION:
*       Get Global configuration IPv4 or IPv6 Total Length Deduction Value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       ipType - IPv4 or Ipv6; valid values:
*                   CPSS_IP_PROTOCOL_IPV4_E
*                   CPSS_IP_PROTOCOL_IPV6_E
*
* OUTPUTS:
*       valuePtr -  (pointer to) IPv4 or IPv6 Total Length Deduction Value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIpTotalLengthDeductionValueGet
(
    IN  GT_U8                               devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT           ipType,
    OUT GT_U32                              *valuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiIpTotalLengthDeductionValueGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ipType, valuePtr));

    rc = internal_cpssDxChTtiIpTotalLengthDeductionValueGet(devNum, ipType, valuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ipType, valuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiEthernetTypeSet
*
* DESCRIPTION:
*       This function sets the TTI Ethernet type value that is used
*       to identify packets for TTI triggering.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       ethertypeType - Ethernet type
*       ethertype     - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   If there are 2 registers used for ethertype configuration,
*   one for ethertype identification of incoming tunneled packets in TTI
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration, these registers are configured to have the same value.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiEthernetTypeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_TUNNEL_ETHERTYPE_TYPE_ENT      ethertypeType,
    IN  GT_U32                              ethertype
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in register */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if (ethertype >= BIT_16)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    switch (ethertypeType)
    {
    case CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE1_E:
        if (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E)
            offset = 0;
        else
            offset = 16;
        /* DxCh3 */
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* write value */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vntReg.vntGreEtherTypeConfReg;
            rc = prvCpssHwPpSetRegField(devNum,regAddr,offset,16,ethertype);
        }
        else /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        {
            /* write value */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIIPv4GREEthertype;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiIpv4GreEthertype;
            }
            rc = prvCpssHwPpSetRegField(devNum,regAddr,offset,16,ethertype);
        }
        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE0_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE1_E:
        if (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE0_E)
            offset = 0;
        else
            offset = 16;
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIIPv6GREEtherType;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,offset,16,ethertype);
        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_MULTICAST_E:
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH3_E);

        if (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E)
            offset = 0;
        else
            offset = 16;
        /* write value */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).MPLSEtherTypes;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.mplsEthertypes;
        }
        rc = prvCpssHwPpSetRegField(devNum,regAddr,offset,16,ethertype);
        if(rc != GT_OK)
            return rc;

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).MPLSEtherType;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltMplsEthertypes;
        }
        rc = prvCpssHwPpSetRegField(devNum,regAddr,offset,16,ethertype);
        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_MIM_E:
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH3_E);

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* write value */
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).specialEtherTypes;
            rc = prvCpssHwPpSetRegField(devNum,regAddr,0,16,ethertype);
            if(rc != GT_OK)
                return rc;
        }
        else
        {
            /* write value */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.specialEthertypes;
            rc = prvCpssHwPpSetRegField(devNum,regAddr,16,16,ethertype);
            if(rc != GT_OK)
                return rc;
        }

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).IEtherType;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltIEthertype;
        }
        rc = prvCpssHwPpSetRegField(devNum,regAddr,0,16,ethertype);
        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_TRILL_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* write value */
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLGlobalConfigs0;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,0,16,ethertype);
        if(rc != GT_OK)
            return rc;

        regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).TRILLEtherType;
        rc = prvCpssHwPpSetRegField(devNum,regAddr,0,16,ethertype);

        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED1_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED3_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED5_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED6_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if ((ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED1_E))
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUserDefinedEtherTypes[0];
        }
        else if ((ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED3_E))
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUserDefinedEtherTypes[1];
        }
        else if ((ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED5_E))
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUserDefinedEtherTypes[2];
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUserDefinedEtherTypes[3];
        }

        if ((ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED6_E))
        {
            offset = 0;
        }
        else
        {
            offset = 16;
        }
        rc = prvCpssHwPpSetRegField(devNum,regAddr,offset,16,ethertype);

        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTtiEthernetTypeSet
*
* DESCRIPTION:
*       This function sets the TTI Ethernet type value that is used
*       to identify packets for TTI triggering.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       ethertypeType - Ethernet type
*       ethertype     - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   If there are 2 registers used for ethertype configuration,
*   one for ethertype identification of incoming tunneled packets in TTI
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration, these registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiEthernetTypeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_TUNNEL_ETHERTYPE_TYPE_ENT      ethertypeType,
    IN  GT_U32                              ethertype
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiEthernetTypeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ethertypeType, ethertype));

    rc = internal_cpssDxChTtiEthernetTypeSet(devNum, ethertypeType, ethertype);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ethertypeType, ethertype));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiEthernetTypeGet
*
* DESCRIPTION:
*       This function gets the TTI Ethernet type value that is used
*       to identify packets for TTI triggering.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       ethertypeType - Ethernet type
*
* OUTPUTS:
*       ethertypePtr  - Points to Ethernet type value (range 16 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*   If there are 2 registers used for ethertype configuration,
*   one for ethertype identification of incoming tunneled packets in TTI
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration, these registers are configured to have the same value.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiEthernetTypeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_TUNNEL_ETHERTYPE_TYPE_ENT      ethertypeType,
    OUT GT_U32                              *ethertypePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      offset;     /* offset in register */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(ethertypePtr);

    switch (ethertypeType)
    {
    case CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE1_E:
        if (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E)
            offset = 0;
        else
            offset = 16;
        /* DxCh3 */
        if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* read value */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.vntReg.vntGreEtherTypeConfReg;
        }
        else /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
        {
            /* read value */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIIPv4GREEthertype;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiIpv4GreEthertype;
            }
        }
        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE0_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE1_E:
        if (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_IPV6_GRE0_E)
            offset = 0;
        else
            offset = 16;
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIIPv6GREEtherType;
        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_MULTICAST_E:
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH3_E);

        if (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E)
            offset = 0;
        else
            offset = 16;
        /* we are reading only mplsEthertypes register because it reflects the
           value in the hdrAltMplsEthertypes as well */
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).MPLSEtherTypes;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.mplsEthertypes;
        }
        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_MIM_E:
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH3_E);

        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            offset = 0;
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).specialEtherTypes;
        }
        else
        {
            offset = 16;
            /* we are reading only specialEthertypes register because it reflects the
               value in the hdrAltIEthertype as well */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.specialEthertypes;
        }
        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_TRILL_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            offset = 0;
            /* we are reading only trillEthertype register because it reflects the
               value in the hdrAltTrillEthertype as well */
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLGlobalConfigs0;
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
        }
        break;
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED1_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED3_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED5_E:
    case CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED6_E:
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if ((ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED1_E))
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUserDefinedEtherTypes[0];
        }
        else if ((ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED3_E))
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUserDefinedEtherTypes[1];
        }
        else if ((ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED5_E))
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUserDefinedEtherTypes[2];
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUserDefinedEtherTypes[3];
        }

        if ((ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED0_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED2_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED4_E) ||
            (ethertypeType == CPSS_TUNNEL_ETHERTYPE_TYPE_USER_DEFINED6_E))
        {
            offset = 0;
        }
        else
        {
            offset = 16;
        }
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssHwPpGetRegField(devNum,regAddr,offset,16,ethertypePtr);
}

/*******************************************************************************
* cpssDxChTtiEthernetTypeGet
*
* DESCRIPTION:
*       This function gets the TTI Ethernet type value that is used
*       to identify packets for TTI triggering.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       ethertypeType - Ethernet type
*
* OUTPUTS:
*       ethertypePtr  - Points to Ethernet type value (range 16 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*   If there are 2 registers used for ethertype configuration,
*   one for ethertype identification of incoming tunneled packets in TTI
*   and one for setting the ethertype for outgoing packets in tunnel start
*   header alteration, these registers are configured to have the same value.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiEthernetTypeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_TUNNEL_ETHERTYPE_TYPE_ENT      ethertypeType,
    OUT GT_U32                              *ethertypePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiEthernetTypeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ethertypeType, ethertypePtr));

    rc = internal_cpssDxChTtiEthernetTypeGet(devNum, ethertypeType, ethertypePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ethertypeType, ethertypePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiTrillCpuCodeBaseSet
*
* DESCRIPTION:
*       This function sets the Trill cpu code base.
*       TRILL CPU codes are relative to global configurable CPU code base value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       cpuCodeBase   - The base CPU code value for the TRILL engine .(APPLICABLE RANGES: 192..244)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiTrillCpuCodeBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          cpuCodeBase
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((cpuCodeBase < CPU_CODE_BASE_MIN_CNS) ||
        (cpuCodeBase > PRV_CPSS_DXCH_TTI_TRILL_CPU_CODE_BASE_MAX_CNS))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* write value */
    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLGlobalConfigs0;
    rc = prvCpssHwPpSetRegField(devNum,regAddr,18,8,cpuCodeBase);

    return rc;

}

/*******************************************************************************
* cpssDxChTtiTrillCpuCodeBaseSet
*
* DESCRIPTION:
*       This function sets the Trill cpu code base.
*       TRILL CPU codes are relative to global configurable CPU code base value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       cpuCodeBase   - The base CPU code value for the TRILL engine .(APPLICABLE RANGES: 192..244)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillCpuCodeBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          cpuCodeBase
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiTrillCpuCodeBaseSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCodeBase));

    rc = internal_cpssDxChTtiTrillCpuCodeBaseSet(devNum, cpuCodeBase);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCodeBase));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiTrillCpuCodeBaseGet
*
* DESCRIPTION:
*       This function sets the Trill cpu code base.
*       TRILL CPU codes are relative to global configurable CPU code base value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*
* OUTPUTS:
*       cpuCodeBasePtr - Points to the base CPU code value for the TRILL engine
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiTrillCpuCodeBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_U32          *cpuCodeBasePtr
)
{
    GT_U32      regAddr;    /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodeBasePtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* read value */
    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLGlobalConfigs0;
    return prvCpssHwPpGetRegField(devNum,regAddr,18,8,cpuCodeBasePtr);

}

/*******************************************************************************
* cpssDxChTtiTrillCpuCodeBaseGet
*
* DESCRIPTION:
*       This function sets the Trill cpu code base.
*       TRILL CPU codes are relative to global configurable CPU code base value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*
* OUTPUTS:
*       cpuCodeBasePtr - Points to the base CPU code value for the TRILL engine
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillCpuCodeBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_U32          *cpuCodeBasePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiTrillCpuCodeBaseGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCodeBasePtr));

    rc = internal_cpssDxChTtiTrillCpuCodeBaseGet(devNum, cpuCodeBasePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCodeBasePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiTrillAdjacencyCheckEntrySet
*
* DESCRIPTION:
*       This function sets entry in the TRILL Adjacency Check dedicated TCAM.
*       A TCAM lookup is performed for every TRILL packet processed by the TRILL engine.
*       The TRILL engine uses a single TCAM lookup to implement
*          the following TRILL adjacency checks:
*          1. TRILL IS-IS Adjacency check -
*             Checks that the single-destination TRILL frame arrives from a
*             {neighbor, port} for which an IS-IS adjacency exists.
*          2. TRILL Tree Adjacency Check -
*             Checks that the multi-destination TRILL frame arrives from a
*             {neighbor, port} that is a branch on the given TRILL distribution tree.
*       If there is TCAM MISS, invoke the respective UC or Multi-Target exception command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - entry index in TRILL Adjacency TCAM .(APPLICABLE RANGES: 0..255)
*       valuePtr          - points to TRILL Adjacency STC
*       maskPtr           - points to TRILL Adjacency STC's mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiTrillAdjacencyCheckEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *valuePtr,
    IN  CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *maskPtr
)
{
    GT_U32    entryDataXY[TTI_ADJACENCY_SIZE_CNS]; /* Adjacency config in hardware format (dataX + dataY) */
    GT_U32    i;
    GT_STATUS rc;
    GT_U32    entryKey[4];        /* entry to write (key)         */
    GT_U32    entryMask[4];       /* entry to write (mask)        */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (entryIndex > ADJACENCY_MAX_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (valuePtr->trillMBit >= BIT_1)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (valuePtr->trillEgressRbid >= BIT_16)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (valuePtr->srcHwDevice > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if (valuePtr->srcIsTrunk >= BIT_1)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    if ((valuePtr->srcIsTrunk == 0 && valuePtr->srcPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum)) ||
        (valuePtr->srcIsTrunk == 1 && valuePtr->srcPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum)))
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* clear output data */
    cpssOsMemSet(entryDataXY, 0, sizeof(GT_U32) * TTI_ADJACENCY_SIZE_CNS);
    cpssOsMemSet(&entryKey , 0, sizeof(entryKey));
    cpssOsMemSet(&entryMask , 0, sizeof(entryMask));

     /* handle word 0 (bits 0-31) */
    U32_SET_FIELD_MASKED_MAC(entryKey[0],0,8,valuePtr->outerMacSa.arEther[5]);
    U32_SET_FIELD_MASKED_MAC(entryKey[0],8,8,valuePtr->outerMacSa.arEther[4]);
    U32_SET_FIELD_MASKED_MAC(entryKey[0],16,8,valuePtr->outerMacSa.arEther[3]);
    U32_SET_FIELD_MASKED_MAC(entryKey[0],24,8,valuePtr->outerMacSa.arEther[2]);

    /* handle word 1 (bits 32-63) */
    U32_SET_FIELD_MASKED_MAC(entryKey[1],0,8,valuePtr->outerMacSa.arEther[1]);
    U32_SET_FIELD_MASKED_MAC(entryKey[1],8,8,valuePtr->outerMacSa.arEther[0]);
    U32_SET_FIELD_MASKED_MAC(entryKey[1],19,1,1); /* valid bit */

    /* handle word 2 (bits 64-95) */
    U32_SET_FIELD_MASKED_MAC(entryKey[2],0,1,BOOL2BIT_MAC(valuePtr->trillMBit));
    U32_SET_FIELD_MASKED_MAC(entryKey[2],1,16,valuePtr->trillEgressRbid);
    U32_SET_FIELD_MASKED_MAC(entryKey[2],17,12,valuePtr->srcHwDevice);
    U32_SET_FIELD_MASKED_MAC(entryKey[2],29,1,BOOL2BIT_MAC(valuePtr->srcIsTrunk));
    U32_SET_FIELD_MASKED_MAC(entryKey[2],30,2,valuePtr->srcPortTrunk);

    /* handle word 3 (bits 96-127) */
    U32_SET_FIELD_MASKED_MAC(entryKey[3],0,18,(valuePtr->srcPortTrunk >> 2));
    U32_SET_FIELD_MASKED_MAC(entryKey[3],19,1,1); /* valid bit */

    /* handle word 4 (bits 128-159) */
    U32_SET_FIELD_MASKED_MAC(entryMask[0],0,8,maskPtr->outerMacSa.arEther[5]);
    U32_SET_FIELD_MASKED_MAC(entryMask[0],8,8,maskPtr->outerMacSa.arEther[4]);
    U32_SET_FIELD_MASKED_MAC(entryMask[0],16,8,maskPtr->outerMacSa.arEther[3]);
    U32_SET_FIELD_MASKED_MAC(entryMask[0],24,8,maskPtr->outerMacSa.arEther[2]);

    /* handle word 5 (bits 160-191) */
    U32_SET_FIELD_MASKED_MAC(entryMask[1],0,8,maskPtr->outerMacSa.arEther[1]);
    U32_SET_FIELD_MASKED_MAC(entryMask[1],8,8,maskPtr->outerMacSa.arEther[0]);
    U32_SET_FIELD_MASKED_MAC(entryMask[1],18,2,3); /* control bits mask */

    /* handle word 6 (bits 192-223) */
    U32_SET_FIELD_MASKED_MAC(entryMask[2],0,1,BOOL2BIT_MAC(maskPtr->trillMBit));
    U32_SET_FIELD_MASKED_MAC(entryMask[2],1,16,maskPtr->trillEgressRbid);
    U32_SET_FIELD_MASKED_MAC(entryMask[2],17,12,maskPtr->srcHwDevice);
    U32_SET_FIELD_MASKED_MAC(entryMask[2],29,1,BOOL2BIT_MAC(maskPtr->srcIsTrunk));
    U32_SET_FIELD_MASKED_MAC(entryMask[2],30,2,maskPtr->srcPortTrunk);

    /* handle word 7 (bits 224-255) */
    U32_SET_FIELD_MASKED_MAC(entryMask[3],0,18,(maskPtr->srcPortTrunk >> 2));
    U32_SET_FIELD_MASKED_MAC(entryMask[3],18,2,3); /* control bits mask */

    /* convert entry from key/mask to X/Y format */
    for (i = 0 ; i <= 3  ; i++)
    {
        entryDataXY[i]   = PRV_BIT_PATTERN_MASK_TO_X_MAC(entryKey[i], entryMask[i]);
        entryDataXY[i+4] = PRV_BIT_PATTERN_MASK_TO_Y_MAC(entryKey[i], entryMask[i]);
    }

    /* write adjacency entry to the hardware */
    rc = prvCpssDxChWriteTableEntry(
        devNum,PRV_CPSS_DXCH_LION3_TABLE_ADJACENCY_E,entryIndex,entryDataXY);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiTrillAdjacencyCheckEntrySet
*
* DESCRIPTION:
*       This function sets entry in the TRILL Adjacency Check dedicated TCAM.
*       A TCAM lookup is performed for every TRILL packet processed by the TRILL engine.
*       The TRILL engine uses a single TCAM lookup to implement
*          the following TRILL adjacency checks:
*          1. TRILL IS-IS Adjacency check -
*             Checks that the single-destination TRILL frame arrives from a
*             {neighbor, port} for which an IS-IS adjacency exists.
*          2. TRILL Tree Adjacency Check -
*             Checks that the multi-destination TRILL frame arrives from a
*             {neighbor, port} that is a branch on the given TRILL distribution tree.
*       If there is TCAM MISS, invoke the respective UC or Multi-Target exception command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - entry index in TRILL Adjacency TCAM .(APPLICABLE RANGES: 0..255)
*       valuePtr          - points to TRILL Adjacency STC
*       maskPtr           - points to TRILL Adjacency STC's mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillAdjacencyCheckEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *valuePtr,
    IN  CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *maskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiTrillAdjacencyCheckEntrySet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, valuePtr, maskPtr));

    rc = internal_cpssDxChTtiTrillAdjacencyCheckEntrySet(devNum, entryIndex, valuePtr, maskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, valuePtr, maskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiTrillAdjacencyCheckEntryGet
*
* DESCRIPTION:
*       This function gets entry in the TRILL Adjacency Check dedicated TCAM.
*       A TCAM lookup is performed for every TRILL packet processed by the TRILL engine.
*       The TRILL engine uses a single TCAM lookup to implement
*          the following TRILL adjacency checks:
*          1. TRILL IS-IS Adjacency check -
*             Checks that the single-destination TRILL frame arrives from a
*             {neighbor, port} for which an IS-IS adjacency exists.
*          2. TRILL Tree Adjacency Check -
*             Checks that the multi-destination TRILL frame arrives from a
*             {neighbor, port} that is a branch on the given TRILL distribution tree.
*       If there is TCAM MISS, invoke the respective UC or Multi-Target exception command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - entry index in TRILL Adjacency TCAM .(APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       valuePtr          - points to TRILL Adjacency STC
*       maskPtr           - points to TRILL Adjacency STC's mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiTrillAdjacencyCheckEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *valuePtr,
    OUT CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *maskPtr
)
{

    GT_U32      entryDataXY[TTI_ADJACENCY_SIZE_CNS]; /* Adjacency config in hardware format (dataX + dataY) */
    GT_STATUS   rc;
    GT_U32      i;
    GT_U32      entryKey[4];        /* entry to write (key)         */
    GT_U32      entryMask[4];       /* entry to write (mask)        */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (entryIndex > ADJACENCY_MAX_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* read adjacency entry from the hardware */
    rc = prvCpssDxChReadTableEntry(
        devNum,PRV_CPSS_DXCH_LION3_TABLE_ADJACENCY_E,entryIndex,entryDataXY);
    if (rc != GT_OK)
        return rc;

    /* convert data from X/Y format to key/mask */
    for (i = 0 ; i <= 3 ; i++)
    {
        entryKey[i]  = PRV_BIT_X_Y_TO_PATTERN_MAC(entryDataXY[i],entryDataXY[i+4]);
        entryMask[i] = PRV_BIT_X_Y_TO_MASK_MAC(entryDataXY[i],entryDataXY[i+4]);
    }

    valuePtr->outerMacSa.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(entryKey[0], 0, 8);
    valuePtr->outerMacSa.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(entryKey[0], 8, 8);
    valuePtr->outerMacSa.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(entryKey[0], 16, 8);
    valuePtr->outerMacSa.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(entryKey[0], 24, 8);
    valuePtr->outerMacSa.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(entryKey[1], 0, 8);
    valuePtr->outerMacSa.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(entryKey[1], 8, 8);
    valuePtr->trillMBit = BIT2BOOL_MAC(U32_GET_FIELD_MAC(entryKey[2], 0, 1));
    valuePtr->trillEgressRbid = (GT_U16)U32_GET_FIELD_MAC(entryKey[2], 1, 16);
    valuePtr->srcHwDevice = (GT_HW_DEV_NUM)U32_GET_FIELD_MAC(entryKey[2], 17, 12);
    valuePtr->srcIsTrunk = BIT2BOOL_MAC(U32_GET_FIELD_MAC(entryKey[2], 29, 1));
    valuePtr->srcPortTrunk = U32_GET_FIELD_MAC(entryKey[2], 30, 2) |
                            (U32_GET_FIELD_MAC(entryKey[3], 0, 18) << 2);

    maskPtr->outerMacSa.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(entryMask[0], 0, 8);
    maskPtr->outerMacSa.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(entryMask[0], 8, 8);
    maskPtr->outerMacSa.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(entryMask[0], 16, 8);
    maskPtr->outerMacSa.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(entryMask[0], 24, 8);
    maskPtr->outerMacSa.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(entryMask[1], 0, 8);
    maskPtr->outerMacSa.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(entryMask[1], 8, 8);
    maskPtr->trillMBit = (GT_BOOL)U32_GET_FIELD_MAC(entryMask[2], 0, 1);
    maskPtr->trillEgressRbid = (GT_U16)U32_GET_FIELD_MAC(entryMask[2], 1, 16);
    maskPtr->srcHwDevice = (GT_HW_DEV_NUM)U32_GET_FIELD_MAC(entryMask[2], 17, 12);
    maskPtr->srcIsTrunk = (GT_BOOL)U32_GET_FIELD_MAC(entryMask[2], 29, 1);
    maskPtr->srcPortTrunk = U32_GET_FIELD_MAC(entryMask[2], 30, 2) |
                           (U32_GET_FIELD_MAC(entryMask[3], 0, 18) << 2);


    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiTrillAdjacencyCheckEntryGet
*
* DESCRIPTION:
*       This function gets entry in the TRILL Adjacency Check dedicated TCAM.
*       A TCAM lookup is performed for every TRILL packet processed by the TRILL engine.
*       The TRILL engine uses a single TCAM lookup to implement
*          the following TRILL adjacency checks:
*          1. TRILL IS-IS Adjacency check -
*             Checks that the single-destination TRILL frame arrives from a
*             {neighbor, port} for which an IS-IS adjacency exists.
*          2. TRILL Tree Adjacency Check -
*             Checks that the multi-destination TRILL frame arrives from a
*             {neighbor, port} that is a branch on the given TRILL distribution tree.
*       If there is TCAM MISS, invoke the respective UC or Multi-Target exception command.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - entry index in TRILL Adjacency TCAM .(APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       valuePtr          - points to TRILL Adjacency STC
*       maskPtr           - points to TRILL Adjacency STC's mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillAdjacencyCheckEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *valuePtr,
    OUT CPSS_DXCH_TTI_TRILL_ADJACENCY_STC   *maskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiTrillAdjacencyCheckEntryGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, entryIndex, valuePtr, maskPtr));

    rc = internal_cpssDxChTtiTrillAdjacencyCheckEntryGet(devNum, entryIndex, valuePtr, maskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, entryIndex, valuePtr, maskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTtiTrillMaxVersionSet
*
* DESCRIPTION:
*       This function sets the max Trill version.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       maxTrillVersion   - max TRILL version value (range 2 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiTrillMaxVersionSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          maxTrillVersion
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if (maxTrillVersion >= BIT_2)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* write value */
    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLGlobalConfigs0;
    rc = prvCpssHwPpSetRegField(devNum,regAddr,16,2,maxTrillVersion);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiTrillMaxVersionSet
*
* DESCRIPTION:
*       This function sets the max Trill version.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       maxTrillVersion   - max TRILL version value (range 2 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillMaxVersionSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          maxTrillVersion
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiTrillMaxVersionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, maxTrillVersion));

    rc = internal_cpssDxChTtiTrillMaxVersionSet(devNum, maxTrillVersion);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, maxTrillVersion));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiTrillMaxVersionGet
*
* DESCRIPTION:
*       This function gets the max Trill version.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       maxTrillVersionPtr   - Points to max TRILL version value (range 2 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiTrillMaxVersionGet
(
    IN  GT_U8           devNum,
    OUT GT_U32          *maxTrillVersionPtr
)
{
    GT_U32      regAddr;    /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    CPSS_NULL_PTR_CHECK_MAC(maxTrillVersionPtr);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* read value */
    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TRILLConfigs.TRILLGlobalConfigs0;
    return prvCpssHwPpGetRegField(devNum,regAddr,16,2,maxTrillVersionPtr);
}

/*******************************************************************************
* cpssDxChTtiTrillMaxVersionGet
*
* DESCRIPTION:
*       This function gets the max Trill version.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       maxTrillVersionPtr   - Points to max TRILL version value (range 2 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTrillMaxVersionGet
(
    IN  GT_U8           devNum,
    OUT GT_U32          *maxTrillVersionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiTrillMaxVersionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, maxTrillVersionPtr));

    rc = internal_cpssDxChTtiTrillMaxVersionGet(devNum, maxTrillVersionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, maxTrillVersionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortTrillEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TRILL engine on the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable TRILL engine on port
*                       GT_FALSE: disable TRILL engine on port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Configure TRILL Interface Enable bit for Egress ePort in table “HA Egress ePort Attribute Table 1”
*       Configure TRILL Engine Enable bit for Ingress ePort in table “Pre-TTI Lookup Ingress ePort Table”
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortTrillEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS   rc;         /* function return code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    CPSS_TBD_BOOKMARK_EARCH /* need to add 'direction' for this API .
        because ingress write 'Fefault eport' table that hold much less entries then the HA table.
        this also implies that the configuration is not symmetric . */

    if(portNum < PRV_CPSS_DXCH_MAX_DEFAULT_EPORT_NUMBER_MAC(devNum))
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* write value to Pre-TTI Lookup Ingress ePort Table */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                             LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRILL_ENGINE_EN_E, /* field name */
                                             PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                             BOOL2BIT_MAC(enable));

        if (rc != GT_OK)
            return rc;
    }

    {
        PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

        /* write value to HA Egress ePort Attribute Table 1 */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                             LION3_HA_EPORT_TABLE_1_FIELDS_TRILL_INTERFACE_ENABLE_E, /* field name */
                                             PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                             BOOL2BIT_MAC(enable));
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortTrillEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TRILL engine on the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       enable        - GT_TRUE: enable TRILL engine on port
*                       GT_FALSE: disable TRILL engine on port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Configure TRILL Interface Enable bit for Egress ePort in table “HA Egress ePort Attribute Table 1”
*       Configure TRILL Engine Enable bit for Ingress ePort in table “Pre-TTI Lookup Ingress ePort Table”
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortTrillEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    IN  GT_BOOL                             enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortTrillEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChTtiPortTrillEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortTrillEnableGet
*
* DESCRIPTION:
*       This function gets the current state enables/disables of TRILL engine
*       on the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TRILL
*                       GT_TRUE:  TRILL engine enabled on port
*                       GT_FALSE: TRILL engine disabled on port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Same value is configured for Egress ePort in table “HA Egress ePort Attribute Table 1”
*       and Ingress ePort in table “Pre-TTI Lookup Ingress ePort Table”
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortTrillEnableGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    OUT GT_BOOL             *enablePtr
)
{
    GT_U32    value;
    GT_STATUS rc;

     /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    CPSS_TBD_BOOKMARK_EARCH /* need to add 'direction' for this API .
        because ingress write 'Fefault eport' table that hold much less entries then the HA table.
        this also implies that the configuration is not symmetric . */

    if(portNum < PRV_CPSS_DXCH_MAX_DEFAULT_EPORT_NUMBER_MAC(devNum))
    {
        PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

        /* read value from hardware table */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                            LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRILL_ENGINE_EN_E, /* field name */
                                            PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                            &value);

        if (rc != GT_OK)
            return rc;
    }

    {
        PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

        /* read value to HA Egress ePort Attribute Table 1 */
        rc = prvCpssDxChReadTableEntryField(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_HA_EGRESS_EPORT_1_E,
                                             portNum,
                                             PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                             LION3_HA_EPORT_TABLE_1_FIELDS_TRILL_INTERFACE_ENABLE_E, /* field name */
                                             PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                             &value);
    }

    /* decode enable/disable state from hardware value */
    *enablePtr = (value != 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortTrillEnableGet
*
* DESCRIPTION:
*       This function gets the current state enables/disables of TRILL engine
*       on the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TRILL
*                       GT_TRUE:  TRILL engine enabled on port
*                       GT_FALSE: TRILL engine disabled on port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Same value is configured for Egress ePort in table “HA Egress ePort Attribute Table 1”
*       and Ingress ePort in table “Pre-TTI Lookup Ingress ePort Table”
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortTrillEnableGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_NUM         portNum,
    OUT GT_BOOL             *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortTrillEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChTtiPortTrillEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortTrillOuterVid0Set
*
* DESCRIPTION:
*       This function sets the Outer Tag0 VID that must be for all TRILL packets
*       from the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*       vlanId    - TRILL Outer Tag0 VID .(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortTrillOuterVid0Set
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_U16          vlanId
)
{
    GT_STATUS   rc;         /* function return code */

     /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if(vlanId >= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* write value from hardware table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                         PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                         portNum,
                                         PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                         LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRILL_INGRESS_OUTER_VID0_E, /* field name */
                                         PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                         vlanId);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortTrillOuterVid0Set
*
* DESCRIPTION:
*       This function sets the Outer Tag0 VID that must be for all TRILL packets
*       from the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*       vlanId    - TRILL Outer Tag0 VID .(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortTrillOuterVid0Set
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    IN  GT_U16          vlanId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortTrillOuterVid0Set);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, vlanId));

    rc = internal_cpssDxChTtiPortTrillOuterVid0Set(devNum, portNum, vlanId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, vlanId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortTrillOuterVid0Get
*
* DESCRIPTION:
*       This function gets the Outer Tag0 VID that must be for all TRILL packets
*       from the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       vlanIdPtr - (pointer to) TRILL Outer Tag0 VID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortTrillOuterVid0Get
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U16          *vlanIdPtr
)
{
    GT_U32      value;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(vlanIdPtr);

    /* read value from hardware table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRILL_INGRESS_OUTER_VID0_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &value);

    *vlanIdPtr = (GT_U16)value;

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortTrillOuterVid0Get
*
* DESCRIPTION:
*       This function gets the Outer Tag0 VID that must be for all TRILL packets
*       from the port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       vlanIdPtr - (pointer to) TRILL Outer Tag0 VID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortTrillOuterVid0Get
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U16          *vlanIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortTrillOuterVid0Get);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, vlanIdPtr));

    rc = internal_cpssDxChTtiPortTrillOuterVid0Get(devNum, portNum, vlanIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, vlanIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupMacToMeSet
*
* DESCRIPTION:
*       This function sets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    IN  CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
)
{
    GT_U32      hwMacToMeArray[TTI_MAC2ME_SIZE_CNS]; /* macToMe config in hardware format */
    GT_U32      hwMacToMeSrcInterface = 0;           /* macToMe Src Interface info in hardware format */
    GT_U32      hwMacToMeSrcInterfaceSelect = 0;     /* macToMe Src Interface select info in hardware format */
    GT_U32      portGroupId;                         /* port group Id */
    GT_U32      rc;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(valuePtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        CPSS_NULL_PTR_CHECK_MAC(interfaceInfoPtr);
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (entryIndex > 127)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if (entryIndex > 7)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (valuePtr->vlanId >= BIT_13)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    else if (valuePtr->vlanId >= BIT_12) /* MAC_TO_ME table supports only 12 bits for VLAN */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        switch (interfaceInfoPtr->includeSrcInterface)
        {
        case CPSS_DXCH_TTI_DONT_USE_SRC_INTERFACE_FIELDS_E:
            hwMacToMeSrcInterfaceSelect = 0;
            break;
        case CPSS_DXCH_TTI_USE_SRC_INTERFACE_FIELDS_E:
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                hwMacToMeSrcInterfaceSelect = 1;/*compare with device */

                if(interfaceInfoPtr->srcHwDevice > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                if ((interfaceInfoPtr->srcIsTrunk == GT_TRUE && interfaceInfoPtr->srcPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum)) ||
                    (interfaceInfoPtr->srcIsTrunk == GT_FALSE && interfaceInfoPtr->srcPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum)))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

                U32_SET_FIELD_MASKED_MAC(hwMacToMeSrcInterface,0,10,interfaceInfoPtr->srcHwDevice);
                U32_SET_FIELD_MASKED_MAC(hwMacToMeSrcInterface,10,1,BOOL2BIT_MAC(interfaceInfoPtr->srcIsTrunk));
                U32_SET_FIELD_MASKED_MAC(hwMacToMeSrcInterface,11,15,interfaceInfoPtr->srcPortTrunk);
            }
            else
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        case CPSS_DXCH_TTI_USE_SRC_INTERFACE_FIELDS_EXCLUDE_SRC_DEVICE_E:
            hwMacToMeSrcInterfaceSelect = 2;/*compare without device */

            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                if((interfaceInfoPtr->srcIsTrunk == GT_TRUE && interfaceInfoPtr->srcPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum)) ||
                    (interfaceInfoPtr->srcIsTrunk == GT_FALSE && interfaceInfoPtr->srcPortTrunk > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum)))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

                U32_SET_FIELD_MASKED_MAC(hwMacToMeSrcInterface,10,1,BOOL2BIT_MAC(interfaceInfoPtr->srcIsTrunk));
                U32_SET_FIELD_MASKED_MAC(hwMacToMeSrcInterface,11,15 ,interfaceInfoPtr->srcPortTrunk);
            }
            else
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* clear output data */
    cpssOsMemSet(hwMacToMeArray, 0, sizeof(GT_U32) * TTI_MAC2ME_SIZE_CNS);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        hwMacToMeArray[0] = (GT_HW_MAC_LOW16(&(valuePtr->mac)) << 16) | (valuePtr->vlanId & 0x1fff);
        hwMacToMeArray[1] =  GT_HW_MAC_HIGH32(&(valuePtr->mac));
        hwMacToMeArray[2] = (GT_HW_MAC_LOW16(&(maskPtr->mac)) << 16)  | (maskPtr->vlanId & 0x1fff);
        hwMacToMeArray[3] =  GT_HW_MAC_HIGH32(&(maskPtr->mac));
        hwMacToMeArray[4] =  hwMacToMeSrcInterface;
        hwMacToMeArray[5] =  hwMacToMeSrcInterfaceSelect;
    }
    else
    {
        hwMacToMeArray[0] = (GT_HW_MAC_LOW16(&(valuePtr->mac)) << 16) | ((valuePtr->vlanId & 0xfff) << 4);
        hwMacToMeArray[1] = GT_HW_MAC_HIGH32(&(valuePtr->mac));
        hwMacToMeArray[2] = (GT_HW_MAC_LOW16(&(maskPtr->mac)) << 16) | ((maskPtr->vlanId & 0xfff) << 4);
        hwMacToMeArray[3] = GT_HW_MAC_HIGH32(&(maskPtr->mac));
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* write mac to me entry to the hardware */
        rc =  prvCpssDxChPortGroupWriteTableEntry(
            devNum,portGroupId,PRV_CPSS_DXCH3_TABLE_MAC2ME_E,entryIndex,hwMacToMeArray);

        if (rc != GT_OK)
            return rc;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupMacToMeSet
*
* DESCRIPTION:
*       This function sets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacToMeSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    IN  CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupMacToMeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, entryIndex, valuePtr, maskPtr, interfaceInfoPtr));

    rc = internal_cpssDxChTtiPortGroupMacToMeSet(devNum, portGroupsBmp, entryIndex, valuePtr, maskPtr, interfaceInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, entryIndex, valuePtr, maskPtr, interfaceInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupMacToMeGet
*
* DESCRIPTION:
*       This function gets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    OUT CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
)
{
    GT_U32      hwMacToMeArray[TTI_MAC2ME_SIZE_CNS]; /* macToMe config in hardware format */
    GT_U32      hwMacToMeSrcInterface;           /* macToMe Src Interface info in hardware format */
    GT_U32      hwMacToMeSrcInterfaceSelect;     /* macToMe Src Interface select info in hardware format */
    GT_U32      portGroupId;                         /* port group Id */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(valuePtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        CPSS_NULL_PTR_CHECK_MAC(interfaceInfoPtr);
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (entryIndex > 127)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if (entryIndex > 7)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    /* read mac to me entry from the hardware */
    if (prvCpssDxChPortGroupReadTableEntry(
        devNum,portGroupId,PRV_CPSS_DXCH3_TABLE_MAC2ME_E,entryIndex,hwMacToMeArray)!= GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
    }
    valuePtr->mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[0], 16, 8);
    valuePtr->mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[0], 24, 8);
    valuePtr->mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[1],  0, 8);
    valuePtr->mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[1],  8, 8);
    valuePtr->mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[1], 16, 8);
    valuePtr->mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[1], 24, 8);

    maskPtr->mac.arEther[5] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[2], 16, 8);
    maskPtr->mac.arEther[4] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[2], 24, 8);
    maskPtr->mac.arEther[3] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[3],  0, 8);
    maskPtr->mac.arEther[2] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[3],  8, 8);
    maskPtr->mac.arEther[1] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[3], 16, 8);
    maskPtr->mac.arEther[0] = (GT_U8)U32_GET_FIELD_MAC(hwMacToMeArray[3], 24, 8);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        valuePtr->vlanId = (GT_U16)U32_GET_FIELD_MAC(hwMacToMeArray[0], 0, 13);
        maskPtr->vlanId  = (GT_U16)U32_GET_FIELD_MAC(hwMacToMeArray[2], 0, 13);

        hwMacToMeSrcInterface = hwMacToMeArray[4];
        hwMacToMeSrcInterfaceSelect = hwMacToMeArray[5];

         switch (hwMacToMeSrcInterfaceSelect & 0x3) /* includeSrcInterface */
         {
         case 0:
             interfaceInfoPtr->includeSrcInterface = CPSS_DXCH_TTI_DONT_USE_SRC_INTERFACE_FIELDS_E;
             break;
         case 1:
             interfaceInfoPtr->includeSrcInterface = CPSS_DXCH_TTI_USE_SRC_INTERFACE_FIELDS_E;
             interfaceInfoPtr->srcHwDevice = U32_GET_FIELD_MAC(hwMacToMeSrcInterface,0,10);
             interfaceInfoPtr->srcIsTrunk = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwMacToMeSrcInterface,10,1));
             interfaceInfoPtr->srcPortTrunk = U32_GET_FIELD_MAC(hwMacToMeSrcInterface,11,15);
             break;
         case 2:
             interfaceInfoPtr->includeSrcInterface = CPSS_DXCH_TTI_USE_SRC_INTERFACE_FIELDS_EXCLUDE_SRC_DEVICE_E;
             interfaceInfoPtr->srcIsTrunk = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwMacToMeSrcInterface,10,1));
             interfaceInfoPtr->srcPortTrunk = U32_GET_FIELD_MAC(hwMacToMeSrcInterface,11,15);
             break;
         default:
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
         }
    }
    else
    {
        valuePtr->vlanId = (GT_U16)U32_GET_FIELD_MAC(hwMacToMeArray[0], 4, 12);
        maskPtr->vlanId  = (GT_U16)U32_GET_FIELD_MAC(hwMacToMeArray[2], 4, 12);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupMacToMeGet
*
* DESCRIPTION:
*       This function gets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    OUT CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupMacToMeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, entryIndex, valuePtr, maskPtr, interfaceInfoPtr));

    rc = internal_cpssDxChTtiPortGroupMacToMeGet(devNum, portGroupsBmp, entryIndex, valuePtr, maskPtr, interfaceInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, entryIndex, valuePtr, maskPtr, interfaceInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupMacModeSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
{
    GT_U32      value;          /* value to write       */
    GT_U32      regAddr;        /* register address     */
    GT_U32      offset;         /* offset in register   */
    GT_STATUS   rc;             /* function return code */
    GT_U32      portGroupId;    /* port group Id        */
    GT_U32      index;          /* udb index */
    GT_U32      numOfUdbs;      /* number of UDBs needed for 'mac SA/DA'*/

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        switch (macMode)
        {
        case CPSS_DXCH_TTI_MAC_MODE_DA_E:
        case CPSS_DXCH_TTI_MAC_MODE_SA_E:
            /* do nothing */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
        case CPSS_DXCH_TTI_KEY_MPLS_E:
        case CPSS_DXCH_TTI_KEY_ETH_E:
        case CPSS_DXCH_TTI_KEY_MIM_E:
            /* do nothing */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* UDB8-13: Anchor=L2; Offset=0-5/6-11 */
        /* MAC_DA/MAC_SA
         */
        /* set the bytes of the SA/DA in little endian order */
        index = lion3LegacyMacSaDaIndexInKey;

        rc = lion3WellKnownPacketPartsUsedSet(devNum,keyType,
            (macMode == CPSS_DXCH_TTI_MAC_MODE_DA_E) ?
                LION3_TTI_BYTES_FROM_PACKET_L2_MAC_DA_E :
                LION3_TTI_BYTES_FROM_PACKET_L2_MAC_SA_E ,
            index,
            &numOfUdbs);
        if (rc != GT_OK)
        {
            return rc;
        }

        return GT_OK;
    }

    /* prepare value to write */
    switch (macMode)
    {
    case CPSS_DXCH_TTI_MAC_MODE_DA_E:
        value  = 0;
        break;
    case CPSS_DXCH_TTI_MAC_MODE_SA_E:
        value  = 1;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            offset = 25;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            offset = 24;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            offset = 26;
            break;
        default:
            if ((keyType == CPSS_DXCH_TTI_KEY_MIM_E) && (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            else
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* write value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssHwPpPortGroupSetRegField(
                devNum,portGroupId,regAddr,offset,1,value);

            if (rc != GT_OK)
                return rc;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

    }
    else /* xCat; xCat3; Lion; xCat2; Lion2 */
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            offset = 0;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            offset = 1;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            offset = 2;
            break;
        case CPSS_DXCH_TTI_KEY_MIM_E:
            offset = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* write value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,offset,1,value);

            if (rc != GT_OK)
                return rc;

        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacModeSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupMacModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, keyType, macMode));

    rc = internal_cpssDxChTtiPortGroupMacModeSet(devNum, portGroupsBmp, keyType, macMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, keyType, macMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupMacModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
{
    GT_U32      value;      /* value to write       */
    GT_U32      regAddr;    /* register address     */
    GT_U32      offset;     /* offset in register   */
    GT_STATUS   rc;         /* function return code */
    GT_U32      portGroupId;/* port group Id        */
    CPSS_DXCH_TTI_OFFSET_TYPE_ENT  dummyOffsetType;
    GT_U32      udbOffset;
    GT_U32      index;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(macModePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
        case CPSS_DXCH_TTI_KEY_MPLS_E:
        case CPSS_DXCH_TTI_KEY_ETH_E:
        case CPSS_DXCH_TTI_KEY_MIM_E:
            /* do nothing */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* UDB8-13: Anchor=L2; Offset=0-5/6-11 */
        /* MAC_DA/MAC_SA
         */
        /* Get the bytes of the SA/DA in little endian order */
        index = lion3LegacyMacSaDaIndexInKey;

        rc = ttiInitUserDefinedByteGet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,keyType,index,&dummyOffsetType,&udbOffset);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (udbOffset == 5)
            *macModePtr = CPSS_DXCH_TTI_MAC_MODE_DA_E; /* MSB byte of MAC DA is in offset 5 */
        else
            *macModePtr = CPSS_DXCH_TTI_MAC_MODE_SA_E; /* MSB byte of MAC SA is in offset 11 */

        return GT_OK;
    }

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            offset = 25;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            offset = 24;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            offset = 26;
            break;
        default:
            if ((keyType == CPSS_DXCH_TTI_KEY_MIM_E) && (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            else
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* read value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
        rc = prvCpssHwPpPortGroupGetRegField(
            devNum,portGroupId,regAddr,offset,1,&value);
    }
    else /* xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3 */
    {
        switch (keyType)
        {
        case CPSS_DXCH_TTI_KEY_IPV4_E:
            offset = 0;
            break;
        case CPSS_DXCH_TTI_KEY_MPLS_E:
            offset = 1;
            break;
        case CPSS_DXCH_TTI_KEY_ETH_E:
            offset = 2;
            break;
        case CPSS_DXCH_TTI_KEY_MIM_E:
            offset = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* read value */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.ttiEngineConfig;
        rc = prvCpssHwPpPortGroupGetRegField(
            devNum,portGroupId,regAddr,offset,1,&value);
    }

    if (rc != GT_OK)
        return rc;

    switch (value)
    {
    case 0:
        *macModePtr = CPSS_DXCH_TTI_MAC_MODE_DA_E;
        break;
    case 1:
        *macModePtr = CPSS_DXCH_TTI_MAC_MODE_SA_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E  (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not needed for TRILL key
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupMacModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupMacModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, keyType, macModePtr));

    rc = internal_cpssDxChTtiPortGroupMacModeGet(devNum, portGroupsBmp, keyType, macModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, keyType, macModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS   rc;                                           /* function return code    */
    GT_U32      hwTtiKeyArray[TTI_RULE_SIZE_CNS];             /* TTI key in hw format    */
    GT_U32      hwTtiMaskArray[TTI_RULE_SIZE_CNS];            /* TTI mask in hw format   */
    GT_U32      hwTtiActionArray[TTI_ACTION_TYPE_2_SIZE_CNS]; /* TTI action in hw format */
    GT_U32      portGroupId;                                  /* port group Id           */
    GT_U32      entryNumber = 0;                              /* Rule's index in TCAM    */
    GT_U32      dummy;
    CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize = CPSS_DXCH_TCAM_RULE_SIZE_10_B_E; /* default */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(patternPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,index);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* convert tti configuration pattern and mask to hardware format */
    rc = ttiConfigLogic2HwFormat(devNum,
                                 ruleType,
                                 patternPtr,
                                 maskPtr,
                                 hwTtiKeyArray,
                                 hwTtiMaskArray);
    if (rc != GT_OK)
        return rc;

    /* convert tti action to hardware format */
    rc = ttiActionLogic2HwFormat(devNum,
                                 actionType,
                                 actionPtr,
                                 hwTtiActionArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert rule's type to rule's size */
        PRV_CPSS_DXCH_TTI_CONVERT_RULE_TYPE_TO_TCAM_RULE_SIZE_VAL_MAC(ruleSize,ruleType);

        /* Check if an global TCAM index is valid for a TCAM rule's size */
        rc = prvCpssDxChTcamRuleIndexCheck(devNum,index,ruleSize);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* convert rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,index,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* write action */
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                                     portGroupId,
                                                     PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                     (entryNumber/2),
                                                     hwTtiActionArray);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* write rule to the TCAM */
            rc = cpssDxChTcamPortGroupRuleWrite(devNum,portGroupsBmp,index,GT_TRUE,ruleSize,hwTtiKeyArray,hwTtiMaskArray);

            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* now write entry to the router / tunnel termination TCAM */
            rc = prvCpssDxChRouterTunnelTermTcamSetLine(
                devNum,portGroupId,index,hwTtiKeyArray,
                hwTtiMaskArray,hwTtiActionArray);

            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
     PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
           devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionType        - type of the action to use
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupRuleSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index, ruleType, patternPtr, maskPtr, actionType, actionPtr));

    rc = internal_cpssDxChTtiPortGroupRuleSet(devNum, portGroupsBmp, index, ruleType, patternPtr, maskPtr, actionType, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index, ruleType, patternPtr, maskPtr, actionType, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_STATE             - a rule of different type was found in the specified index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{

    GT_STATUS   rc;                                                           /* function return code    */
    GT_U32      validArr[5];                                                  /* TCAM line valid bits    */
    GT_U32      compareModeArr[5];                                            /* TCAM line compare mode  */
    GT_U32      portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;               /* port group Id           */
    GT_U32      hwTtiKeyArray[TCAM_RULE_SIZE_CNS];                            /* TTI key in hw format    */
    GT_U32      hwTtiMaskArray[TCAM_RULE_SIZE_CNS];                           /* TTI mask in hw format   */
    GT_U32      hwTtiActionArray[TTI_ACTION_TYPE_2_SIZE_CNS];                 /* TTI action in hw format */
    GT_BOOL     validFound;                                                   /* is rule found valid     */
    CPSS_DXCH_TCAM_RULE_SIZE_ENT  ruleSize;                                   /* rule size for ruleType  */
    CPSS_DXCH_TCAM_RULE_SIZE_ENT  ruleSizeFound;                              /* rule size found in TCAM */
    GT_U32      entryNumber;                                                  /* Rule's index in TCAM    */
    GT_U32      dummy;

    /* check pararmeters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(patternPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,index);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

     /* zero out hw format */
    cpssOsMemSet(hwTtiKeyArray,0,sizeof(hwTtiKeyArray));
    cpssOsMemSet(hwTtiMaskArray,0,sizeof(hwTtiMaskArray));
    cpssOsMemSet(hwTtiActionArray,0,sizeof(hwTtiActionArray));

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert rule's type to rule's size */
        PRV_CPSS_DXCH_TTI_CONVERT_RULE_TYPE_TO_TCAM_RULE_SIZE_VAL_MAC(ruleSize,ruleType);

        /* Check if an global TCAM index is valid for a TCAM rule's size */
        rc = prvCpssDxChTcamRuleIndexCheck(devNum,index,ruleSize);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* convert rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,index,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read action */
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                                 portGroupId,
                                                 PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                 (entryNumber/2),
                                                 hwTtiActionArray);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* read rule from the TCAM */
        rc = cpssDxChTcamPortGroupRuleRead(devNum,portGroupsBmp,index,&validFound,&ruleSizeFound,hwTtiKeyArray,hwTtiMaskArray);

        if ((ruleSizeFound != ruleSize) && (validFound == GT_TRUE))
        {
            /* the rule is valid but has another size */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        rc = prvCpssDxChRouterTunnelTermTcamMultiPortGroupsGetLine(devNum,
                                                                   portGroupsBmp,
                                                                   index,
                                                                   &validArr[0],
                                                                   &compareModeArr[0],
                                                                   hwTtiKeyArray,
                                                                   hwTtiMaskArray,
                                                                   hwTtiActionArray);
    }
    else
    {
        /* read tunnel termination configuration and action from hardware */
        rc = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                    portGroupId,
                                                    index,
                                                    &validArr[0],
                                                    &compareModeArr[0],
                                                    hwTtiKeyArray,
                                                    hwTtiMaskArray,
                                                    hwTtiActionArray);
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
    {
        /* convert tti configuration from hw format to logic format */
        rc = ttiConfigHw2LogicFormat(devNum,ruleType,hwTtiKeyArray,patternPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* convert tti configuration mask from hw format to logic format */
        rc = ttiConfigHw2LogicFormat(devNum,ruleType,hwTtiMaskArray,maskPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* convert tti configuration 'old keys' as udbs from hw format to logic format */
        rc = ttiConfigHwUdb2LogicFormat(ruleType,GT_TRUE,hwTtiKeyArray,patternPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* convert tti configuration mask 'old keys' as udbs from hw format to logic format */
        rc = ttiConfigHwUdb2LogicFormat(ruleType,GT_FALSE,hwTtiMaskArray,maskPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* convert tti configuration for dual device id from hw format to logic format */
    rc = ttiConfigLogicFormatForDualDeviceId(ruleType, patternPtr, maskPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* convert tti action from hw format to logic format */
    rc = prvCpssDxChTtiActionHw2LogicFormat(devNum,hwTtiActionArray,actionType,actionPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*       ruleType          - TTI rule type
*       actionType        - type of the action to use
*
* OUTPUTS:
*       patternPtr        - points to the rule's pattern
*       maskPtr           - points to the rule's mask. The rule mask is "AND STYLED
*                           ONE". Mask bit's 0 and pattern bit's 0 means don't care bit
*                           (corresponding bit in the pattern is not used in the TCAM lookup).
*                           Mask bit's 0 and pattern bit's 1 means ALWAYS MISS (full entry miss).
*                           Mask bit's 1 means that corresponding bit in the pattern
*                           is using in the TCAM lookup.
*       actionPtr         - points to the TTI rule action that applied on packet
*                           if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_STATE             - a rule of different type was found in the specified index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         ruleType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupRuleGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index, ruleType, patternPtr, maskPtr, actionType, actionPtr));

    rc = internal_cpssDxChTtiPortGroupRuleGet(devNum, portGroupsBmp, index, ruleType, patternPtr, maskPtr, actionType, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index, ruleType, patternPtr, maskPtr, actionType, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index            - Index of the rule in the TCAM
*       actionType       - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS   rc;                                            /* return status                  */
    GT_U32      hwTtiActionArray[TTI_ACTION_TYPE_2_SIZE_CNS];  /* TTI action in hw format        */
    GT_U32      portGroupId;                                   /* port group Id                  */
    GT_U32      entryNumber = 0;                               /* Rule's index in TCAM           */
    GT_U32      dummy;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(actionPtr);
    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,index);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* convert tti action to hardware format */
    rc = ttiActionLogic2HwFormat(devNum,actionType,actionPtr,hwTtiActionArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* convert rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,index,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* write action */
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                                     portGroupId,
                                                     PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                     (entryNumber/2),
                                                     hwTtiActionArray);
        }
        else
        {
            /* write action */
            rc =  prvCpssDxChPortGroupWriteTableEntry(
                devNum,portGroupId,PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
                index,hwTtiActionArray);
        }

        if (rc != GT_OK)
        {
            return rc;
        }
    }
     PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
           devNum, portGroupsBmp, portGroupId)

     return GT_OK;

}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index            - Index of the rule in the TCAM
*       actionType       - type of the action to use
*       actionPtr        - points to the TTI rule action that applied on packet
*                          if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupRuleActionUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index, actionType, actionPtr));

    rc = internal_cpssDxChTtiPortGroupRuleActionUpdate(devNum, portGroupsBmp, index, actionType, actionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index, actionType, actionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  GT_BOOL                             valid
)
{
    GT_U32      keyArr[TTI_RULE_SIZE_CNS];          /* TCAM key in hw format            */
    GT_U32      maskArr[TTI_RULE_SIZE_CNS];         /* TCAM mask in hw format           */
    /* TCAM action (rule) in hw format  */
    GT_U32      actionArr[PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_MAX_LINE_ACTION_SIZE_CNS];
    GT_U32      validArr[5];        /* TCAM line valid bits             */
    GT_U32      compareModeArr[5];  /* TCAM line compare mode           */
    /* port group Id                    */
    GT_U32      portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    GT_U32      entryKey[4];        /* dummy                        */
    GT_U32      entryMask[4];       /* dummy                        */
    GT_BOOL     validFound;         /* is rule found valid          */
    CPSS_DXCH_TCAM_RULE_SIZE_ENT  ruleSizeFound; /* rule size found */

    GT_STATUS   rc = GT_OK;

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,index);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

     /* zero out hw format */
    cpssOsMemSet(keyArr,0,sizeof(keyArr));
    cpssOsMemSet(maskArr,0,sizeof(maskArr));
    cpssOsMemSet(actionArr,0,sizeof(actionArr));

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssDxChTcamPortGroupRuleReadEntry(devNum,portGroupsBmp,index,&validFound,&ruleSizeFound,entryKey,entryMask);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (valid == validFound)
        {
            /* the rule is aready in requred valid state */
            return GT_OK;
        }

        return cpssDxChTcamPortGroupRuleValidStatusSet(devNum,portGroupsBmp,index,valid);
    }


    if (valid == GT_TRUE)
    {
        if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            rc = prvCpssDxChRouterTunnelTermTcamMultiPortGroupsGetLine(devNum,
                                                                       portGroupsBmp,
                                                                       index,
                                                                       &validArr[0],        /* dummy */
                                                                       &compareModeArr[0],  /* dummy */
                                                                       keyArr,
                                                                       maskArr,
                                                                       actionArr);
        }
        else
        {
            /* read tunnel termination configuration and action from hardware */
            rc = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                        portGroupId,
                                                        index,
                                                        &validArr[0],       /* dummy */
                                                        &compareModeArr[0], /* dummy */
                                                        keyArr,
                                                        maskArr,
                                                        actionArr);
        }

        if (rc != GT_OK)
            return rc;

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            /* write the line (note it also set the line as valid) */
            rc = prvCpssDxChRouterTunnelTermTcamSetLine(devNum,
                                                        portGroupId,
                                                        index,
                                                        keyArr,
                                                        maskArr,
                                                        actionArr);
            if (rc != GT_OK)
                return rc;
        }

        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
           devNum, portGroupsBmp, portGroupId)
    }
    else /* (valid == GT_FALSE) */
    {
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            /* just invalidate the line */
            rc = prvCpssDxChRouterTunnelTermTcamInvalidateLine(
                devNum,portGroupId,index);

            if (rc != GT_OK)
                return rc;
        }

        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    IN  GT_BOOL                             valid
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupRuleValidStatusSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index, valid));

    rc = internal_cpssDxChTtiPortGroupRuleValidStatusSet(devNum, portGroupsBmp, index, valid);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index, valid));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    OUT GT_BOOL                             *validPtr
)
{
    GT_U32      keyArr[TTI_RULE_SIZE_CNS];          /* TCAM key in hw format            */
    GT_U32      maskArr[TTI_RULE_SIZE_CNS];         /* TCAM mask in hw format           */
      /* TCAM action (rule) in hw format  */
    GT_U32      actionArr[PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_MAX_LINE_ACTION_SIZE_CNS];
    GT_U32      validArr[6];        /* TCAM line valid bits             */
    GT_U32      compareModeArr[6];  /* TCAM line compare mode           */
    GT_U32      portGroupId;        /* port group Id                    */
    GT_STATUS   rc;                 /* function return code             */
    GT_U32      i;
    CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize; /* dummy */

    /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    PRV_CPSS_DXCH_TTI_INDEX_CHECK_MAC(devNum,index);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        return cpssDxChTcamPortGroupRuleValidStatusGet(devNum,portGroupsBmp,index,validPtr,&ruleSize);
    }

    rc = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,
                                                portGroupId,
                                                index,
                                                validArr,
                                                compareModeArr,
                                                keyArr,
                                                maskArr,
                                                actionArr);
    if (rc != GT_OK)
        return rc;

    /* line holds valid TTI entry if the following applies:
         - all entries are valid
         - the compare mode or all entries is row compare
         - keyArr[5] bit 31 must be 1 (to indicate TTI entry and not IPv6 entry) */
    *validPtr = GT_TRUE;
    for (i = 0 ; i < 4; i++)
    {
        /* if entry is not valid or is single compare mode, whole line is not valid */
        if ((validArr[i] == 0) || (compareModeArr[i] == 0))
        {
            *validPtr = GT_FALSE;
            break;
        }
    }
    /* if whole line is valid, verify it is indeed TTI entry and not IPv6 entry */
    if ((*validPtr == GT_TRUE) && (((keyArr[5] >> 31) & 0x1) != 1))
    {
        *validPtr = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPortGroupRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index             - Global index of the rule in the TCAM
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              index,
    OUT GT_BOOL                             *validPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupRuleValidStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, index, validPtr));

    rc = internal_cpssDxChTtiPortGroupRuleValidStatusGet(devNum, portGroupsBmp, index, validPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, index, validPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*       offsetType   - the type of offset
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type. .(APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_TTI_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((offset >= BIT_7) || (udbIndex >= UDB_CONFIG_NUM_WORDS_MAX_CNS))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E:
    case CPSS_DXCH_TTI_KEY_UDB_MPLS_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E:
    case CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE1_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE2_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE3_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE4_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE5_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE6_E:
        /* do nothing */
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return ttiInitUserDefinedByteSet(devNum,portGroupsBmp,keyType,udbIndex,offsetType,offset);

}

/*******************************************************************************
* cpssDxChTtiPortGroupUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*       offsetType   - the type of offset
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type. .(APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_TTI_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupUserDefinedByteSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, keyType, udbIndex, offsetType, offset));

    rc = internal_cpssDxChTtiPortGroupUserDefinedByteSet(devNum, portGroupsBmp, keyType, udbIndex, offsetType, offset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, keyType, udbIndex, offsetType, offset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortGroupUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortGroupUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_TTI_OFFSET_TYPE_ENT       *offsetTypePtr,
    OUT GT_U8                               *offsetPtr
)
{
    GT_STATUS   rc;
    GT_U32  offset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(offsetTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(offsetPtr);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);


    if (udbIndex >= UDB_CONFIG_NUM_WORDS_MAX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    switch (keyType)
    {
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E:
    case CPSS_DXCH_TTI_KEY_UDB_MPLS_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E:
    case CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E:
    case CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE1_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE2_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE3_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE4_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE5_E:
    case CPSS_DXCH_TTI_KEY_UDB_UDE6_E:
        /* do nothing */
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = ttiInitUserDefinedByteGet(devNum,portGroupsBmp,keyType,udbIndex,offsetTypePtr,&offset);
    *offsetPtr = (GT_U8)offset;

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPortGroupUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortGroupUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_TTI_OFFSET_TYPE_ENT       *offsetTypePtr,
    OUT GT_U8                               *offsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortGroupUserDefinedByteGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, keyType, udbIndex, offsetTypePtr, offsetPtr));

    rc = internal_cpssDxChTtiPortGroupUserDefinedByteGet(devNum, portGroupsBmp, keyType, udbIndex, offsetTypePtr, offsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, keyType, udbIndex, offsetTypePtr, offsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortPassengerOuterIsTag0Or1Set
*
* DESCRIPTION:
*       Set per port TTI passenger Outer Tag is Tag0 or Tag1.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       mode - recognize Outer Tag existence according to TPID1 or TPID0
*                       CPSS_VLAN_ETHERTYPE0_E: Outer Tag exists if passenger outer VLAN Tag TPID = Tag0 TPID
*                       CPSS_VLAN_ETHERTYPE1_E: Outer Tag exists if passenger outer VLAN tag TPID = Tag1 TPID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port , mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortPassengerOuterIsTag0Or1Set
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    IN  CPSS_ETHER_MODE_ENT          mode
)
{
    GT_STATUS   rc;
    GT_U32      hwValue;    /* hw value */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
        CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);

    switch(mode)
    {
        case CPSS_VLAN_ETHERTYPE0_E:
            hwValue = 0;
            break;
        case CPSS_VLAN_ETHERTYPE1_E:
            hwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* write to pre-tti-lookup-ingress-eport table */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_802_1AH_PASSENGER_STAG_IS_TAG0_1_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        hwValue);
    return rc;

}

/*******************************************************************************
* cpssDxChTtiPortPassengerOuterIsTag0Or1Set
*
* DESCRIPTION:
*       Set per port TTI passenger Outer Tag is Tag0 or Tag1.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       mode - recognize Outer Tag existence according to TPID1 or TPID0
*                       CPSS_VLAN_ETHERTYPE0_E: Outer Tag exists if passenger outer VLAN Tag TPID = Tag0 TPID
*                       CPSS_VLAN_ETHERTYPE1_E: Outer Tag exists if passenger outer VLAN tag TPID = Tag1 TPID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port , mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortPassengerOuterIsTag0Or1Set
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    IN  CPSS_ETHER_MODE_ENT          mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortPassengerOuterIsTag0Or1Set);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, mode));

    rc = internal_cpssDxChTtiPortPassengerOuterIsTag0Or1Set(devNum, portNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPortPassengerOuterIsTag0Or1Get
*
* DESCRIPTION:
*       Set per port TTI passenger Outer Tag is Tag0 or Tag1.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       modePtr - (pointer to)recognize Outer Tag existence according to TPID1 or TPID0
*                       CPSS_VLAN_ETHERTYPE0_E: Outer Tag exists if passenger outer VLAN Tag TPID = Tag0 TPID
*                       CPSS_VLAN_ETHERTYPE1_E: Outer Tag exists if passenger outer VLAN tag TPID = Tag1 TPID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPortPassengerOuterIsTag0Or1Get
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    OUT  CPSS_ETHER_MODE_ENT          *modePtr
)
{
    GT_STATUS   rc;
    GT_U32      hwValue;    /* hw value */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
        CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);
    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* read from pre-tti-lookup-ingress-eport table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TTI_802_1AH_PASSENGER_STAG_IS_TAG0_1_E, /* field name */
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &hwValue);

    *modePtr = (hwValue == 1) ?
                CPSS_VLAN_ETHERTYPE1_E :
                CPSS_VLAN_ETHERTYPE0_E;

    return rc;

}

/*******************************************************************************
* cpssDxChTtiPortPassengerOuterIsTag0Or1Get
*
* DESCRIPTION:
*       Set per port TTI passenger Outer Tag is Tag0 or Tag1.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       modePtr - (pointer to)recognize Outer Tag existence according to TPID1 or TPID0
*                       CPSS_VLAN_ETHERTYPE0_E: Outer Tag exists if passenger outer VLAN Tag TPID = Tag0 TPID
*                       CPSS_VLAN_ETHERTYPE1_E: Outer Tag exists if passenger outer VLAN tag TPID = Tag1 TPID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, port
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPortPassengerOuterIsTag0Or1Get
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    OUT  CPSS_ETHER_MODE_ENT          *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPortPassengerOuterIsTag0Or1Get);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, modePtr));

    rc = internal_cpssDxChTtiPortPassengerOuterIsTag0Or1Get(devNum, portNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* lion3MetaDataFieldInfoGet
*
* DESCRIPTION:
*       get the metadata field info for given field.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       metadataField - meta data field
*
* OUTPUTS:
*       fieldInfoPtrPtr - (pointer to) (pointer to) field info (start bit , length)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS  lion3MetaDataFieldInfoGet
(
    IN LION3_TTI_META_DATA_FIELDS_ENT  metadataField,
    OUT PRV_CPSS_ENTRY_FORMAT_TABLE_STC **fieldInfoPtrPtr
)
{
    if(metadataField >= LION3_TTI_META_DATA_FIELDS___LAST_VALUE___E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *fieldInfoPtrPtr = &(lion3TtiMetaDataFieldsFormat[metadataField]);

    return GT_OK;
}

/*******************************************************************************
* lion3MetaDataUdbNumGet
*
* DESCRIPTION:
*       get the number of UDBs needed for given list of fields.
*       and get a BMP of offset from the metadata start
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       fieldsArrPtr - (pointer to) array of meta data fields
*       numOfFields - number of fields in fieldsArrPtr
*
* OUTPUTS:
*       neededBytesBmpPtr - (pointer to) bmp of the needed bytes (each needed byte represented by single bit)
*                           since TTI metadata hold exactly 32 bytes , we can use single BMP of 32 bits.
*       numOfUdbsPtr - (pointer to) the number of UDBs needed for the fields
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS  lion3MetaDataUdbsGet
(
    IN LION3_TTI_META_DATA_FIELDS_ENT  *fieldsArrPtr,
    IN GT_U32           numOfFields,
    OUT GT_U32          *neededBytesBmpPtr,
    OUT GT_U32          *numOfUdbsPtr
)
{
    GT_U32  ii;
    GT_STATUS   rc;
    GT_U32  neededBytesBmp = 0;/* since TTI metadata hold exactly 32 bytes , we can use single BMP of 32 bits */
    PRV_CPSS_ENTRY_FORMAT_TABLE_STC *fieldInfoPtr;
    GT_U32  startByte , endByte , numBytes;
    GT_U32  maxNumMetaDataBytes =  NUM_OF_META_DATA_BYTES_CNS;/* max number of bytes of metadata  */

    if(maxNumMetaDataBytes > 32)
    {
        /* the current code written to support up to 32 bytes */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }


    for(ii = 0 ; ii < numOfFields ; ii++)
    {
        rc = lion3MetaDataFieldInfoGet(fieldsArrPtr[ii],&fieldInfoPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        startByte = (fieldInfoPtr->startBit / 8);
        endByte = (fieldInfoPtr->startBit + fieldInfoPtr->numOfBits - 1) / 8;
        numBytes = (endByte - startByte) + 1;

        /* state the needed bytes (each needed byte represented by single bit)*/
        U32_SET_FIELD_MASKED_MAC(neededBytesBmp ,
            startByte ,      /* start byte */
            numBytes,  /* number of bytes */
            0xFFFFFFFF);
    }

    if(neededBytesBmpPtr)
    {
        *neededBytesBmpPtr = neededBytesBmp;
    }

    if(numOfUdbsPtr)
    {
        /* get the number of bits that are set in neededBytesBmp */
        *numOfUdbsPtr = prvCpssPpConfigBitmapNumBitsGet(neededBytesBmp);
    }

    if(neededBytesBmp == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }


    return GT_OK;
}

/*******************************************************************************
* lion3WellKnownPacketPartUdbsGet
*
* DESCRIPTION:
*       get the number of UDBs needed for given packet part
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       field        - packet part
*
* OUTPUTS:
*       numOfUdbsPtr - (pointer to) the number of UDBs needed for the field
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS  lion3WellKnownPacketPartUdbsGet
(
    IN LION3_TTI_BYTES_FROM_PACKET_ENT  field,
    OUT GT_U32          *numOfUdbsPtr
)
{
    PACKET_PART_INFO_STC *currentPartPtr;
    GT_U32  startByte , endByte , numBytes;

    if(field >= LION3_TTI_BYTES_FROM_PACKET___LAST_VALUE___E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    currentPartPtr = & lion3TtiWellKnownPacketPartsArr[field];
    switch(currentPartPtr->partType)
    {
        case CPSS_DXCH_TTI_OFFSET_L2_E:
        case CPSS_DXCH_TTI_OFFSET_L3_MINUS_2_E:
        case CPSS_DXCH_TTI_OFFSET_L4_E:
        case CPSS_DXCH_TTI_OFFSET_MPLS_MINUS_2_E:
            break;
        case CPSS_DXCH_TTI_OFFSET_INVALID_E:
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    startByte = (currentPartPtr->startByteOffset / 8);
    /* state the needed bytes (each needed byte represented by single bit)*/
    if(currentPartPtr->flag != FLAG_VALUE_REVERED_E)
    {
        endByte = startByte + ((0 + (currentPartPtr->startBitInByte + currentPartPtr->numOfBits - 1)) / 8);
    }
    else /*FLAG_VALUE_REVERED_E*/
    {
        endByte = startByte + ((7 + ((currentPartPtr->numOfBits - 1) - currentPartPtr->startBitInByte)) / 8);
    }

    numBytes = (endByte - startByte) + 1;

    (*numOfUdbsPtr) = numBytes;

    return GT_OK;
}



/*******************************************************************************
* lion3MetaDataFeildsUsedSet
*
* DESCRIPTION:
*       init common values for specific keys in TTI UDBs Configuration Table.
*       for list of fields.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type
*       fieldsArrPtr - (pointer to) array of meta data fields
*       numOfFields - number of fields in fieldsArrPtr
*       startIndexInKey - the first index in the key that should be bound to the fields.
*
* OUTPUTS:
*       numOfUdbsPtr - (pointer to) the number of UDBs needed for the fields
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS  lion3MetaDataFeildsUsedSet
(
    IN GT_U8            devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType,
    IN LION3_TTI_META_DATA_FIELDS_ENT  *fieldsArrPtr,
    IN GT_U32              numOfFields,
    IN GT_U32              startIndexInKey,
    OUT GT_U32              *numOfUdbsPtr
)
{
    GT_STATUS   rc;
    GT_U32  neededBytesBmp;
    GT_U32  offset;/* byte offset in the metadata */
    GT_U32  index ;/* index in the key */
    GT_U32  maxNumMetaDataBytes =  NUM_OF_META_DATA_BYTES_CNS;/* max number of bytes of metadata  */

    rc = lion3MetaDataUdbsGet(fieldsArrPtr,
            numOfFields,&neededBytesBmp,numOfUdbsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    index = startIndexInKey;/* index in the key */

    for (offset = 0; offset < maxNumMetaDataBytes; offset++)
    {
        if(0 == ((1 << offset) & neededBytesBmp))
        {
            continue;
        }

        rc = ttiInitUserDefinedByteSet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
            keyType,index,CPSS_DXCH_TTI_OFFSET_METADATA_E,offset);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* start to the next index in the key */
        index ++;
    }

    return GT_OK;
}



/*******************************************************************************
* lion3WellKnownPacketPartsUsedSet
*
* DESCRIPTION:
*       set packets part for specific keys in TTI UDBs Configuration Table.
*       function bind parts in 'little endian' order.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type
*       field        - single part
*       startIndexInKey - the first index in the key that should be bound to the parts
*
* OUTPUTS:
*       numOfUdbsPtr - (pointer to) the number of UDBs needed for the part
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS  lion3WellKnownPacketPartsUsedSet
(
    IN GT_U8            devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType,
    IN LION3_TTI_BYTES_FROM_PACKET_ENT  field,
    IN GT_U32              startIndexInKey,
    OUT GT_U32             *numOfUdbsPtr
)
{
    GT_STATUS   rc;
    GT_U32  ii;
    GT_U32  index ;/* index in the key */
    GT_U32  anchorIndex;/*index in the anchor*/
    PACKET_PART_INFO_STC *currentPartPtr;

    rc = lion3WellKnownPacketPartUdbsGet(field,numOfUdbsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    index = startIndexInKey;/* index in the key */

    currentPartPtr = & lion3TtiWellKnownPacketPartsArr[field];

    /* anchor is in reverse because 'little endian' order */
    anchorIndex = currentPartPtr->startByteOffset + (*numOfUdbsPtr) - 1;

    for(ii = 0; ii < (*numOfUdbsPtr) ;index++ ,ii++)
    {
        rc = ttiInitUserDefinedByteSet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                keyType,index,
                currentPartPtr->partType,
                anchorIndex);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* anchor is in reverse because 'little endian' order */
        anchorIndex--;
    }

    return GT_OK;
}


/*******************************************************************************
* ttiInitUserDefinedByteCommonSet
*
* DESCRIPTION:
*       init common values for specific keys in TTI User Defined bytes Configuration Table
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type
*
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS  ttiInitUserDefinedByteCommonSet
(
    IN GT_U8                       devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType
)
{
    GT_STATUS  rc;
    GT_U32  numOfUdbs;
    GT_U32  index ;/* index in the key */

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    /* NOTE: this function sets

        UDBs 0..8 (9 UDBs) for metadata
        UDBs 9..13 (6 UDBs) for DA mac (place holder for SA/DA)
        UDBs 28..29 (2 UDBs) for metadata
        and another 2 lsb UDBS
        */



    /*  metadata bytes : 22..29 (8 UDBs)
        lion3LegacyKey_common_metadata22_29_fieldsArr[]
        PCL ID
        Local Source is Trunk
        DSA SrcIsTrunk
        CoreID
        eVLAN
        Local Source ePort/TrunkID[15:0]
        DSA Tag Source Port/Trunk[15:0]
    */
    index = 0;
    rc = lion3MetaDataFeildsUsedSet(devNum,
            keyType,
            PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(lion3LegacyKey_common_metadata22_29_fieldsArr),
            index,
            &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    /*  metadata bytes : 11 (1 UDB)
        lion3LegacyKey_common_metadata11_fieldsArr[]
        VLAN Tag0 Exists
    */
    rc = lion3MetaDataFeildsUsedSet(devNum,
            keyType,
            PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(lion3LegacyKey_common_metadata11_fieldsArr),
            index,
            &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    /* UDB8-13: Anchor=L2; Offset=0-5/6-11 */
    /* MAC_DA/MAC_SA
     */

    /* save the actual index */
    lion3LegacyMacSaDaIndexInKey = index;
    /* for BWC we set MAC DA */
    rc = lion3WellKnownPacketPartsUsedSet(devNum,keyType,
        LION3_TTI_BYTES_FROM_PACKET_L2_MAC_DA_E,
        index,
        &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    /*  metadata bytes : 30..31 (2 UDBs)
        lion3LegacyKey_common_metadata30_31_fieldsArr[]
       DSA Tag Source Device
    */
    index = 28;
    rc = lion3MetaDataFeildsUsedSet(devNum,
            keyType,
            PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(lion3LegacyKey_common_metadata30_31_fieldsArr),
            index,
            &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    return GT_OK;
}

/*******************************************************************************
* lion3BuildUdbsNeededForLegacyKey_ipv4
*
* DESCRIPTION:
*       build legacy key needed UDBs - ip key
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - the key type that need ipv4 legacy style
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lion3BuildUdbsNeededForLegacyKey_ipv4
(
    IN GT_U8   devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT keyType
)
{
    GT_STATUS  rc;
    GT_U32 index;
    GT_U32  startIndex;
    GT_U32  numOfUdbs;
    LION3_TTI_BYTES_FROM_PACKET_ENT partsArr[]=
    {
        LION3_TTI_BYTES_FROM_PACKET_L3_IPV4_SIP_E,
        LION3_TTI_BYTES_FROM_PACKET_L3_IPV4_DIP_E
    };
    GT_U32  ii;

    /* init UDBs for CPSS_DXCH_TTI_KEY_IPV4_E */
    ttiInitUserDefinedByteCommonSet(devNum,keyType);
    startIndex = lion3LegacyMacSaDaIndexInKey + 6;/* start after the 6 UDBs of mac sa/da*/

    index = startIndex;

    for(ii = 0 ; ii < (sizeof(partsArr)/sizeof(partsArr[0])) ; ii++)
    {
        rc = lion3WellKnownPacketPartsUsedSet(devNum,keyType,
            partsArr[ii],
            index,
            &numOfUdbs);
        if (rc != GT_OK)
        {
            return rc;
        }

        index += numOfUdbs;
    }
/*
    IS_ARP,
    TUNNELING_PROTOCOL
*/
    rc = lion3MetaDataFeildsUsedSet(devNum,
            keyType,
            PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(lion3LegacyKey_ip_metadata0_fieldsArr),
            index,
            &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    return GT_OK;
}

/*******************************************************************************
* lion3BuildUdbsNeededForLegacyKey_mpls
*
* DESCRIPTION:
*       build legacy key needed UDBs - mpls key
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - the key type that need mpls legacy style
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lion3BuildUdbsNeededForLegacyKey_mpls
(
    IN GT_U8   devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT keyType
)
{
    GT_STATUS  rc;
    GT_U32 index;
    GT_U32  startIndex;
    GT_U32  numOfUdbs;
    LION3_TTI_BYTES_FROM_PACKET_ENT partsArr[]=
    {
        LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_1_LABEL_E,
        LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_2_LABEL_E,
        LION3_TTI_BYTES_FROM_PACKET_MPLS_TAG_3_LABEL_E
    };
    GT_U32  ii;

    /* init UDBs for CPSS_DXCH_TTI_KEY_MPLS_E */
    ttiInitUserDefinedByteCommonSet(devNum,keyType);
    startIndex = lion3LegacyMacSaDaIndexInKey + 6;/* start after the 6 UDBs of mac sa/da*/

    index = startIndex;


    for(ii = 0 ; ii < (sizeof(partsArr)/sizeof(partsArr[0])) ; ii++)
    {
        rc = lion3WellKnownPacketPartsUsedSet(devNum,keyType,
            partsArr[ii],
            index,
            &numOfUdbs);
        if (rc != GT_OK)
        {
            return rc;
        }

        index += numOfUdbs;
    }

    /* Reserved Label Value
       Control Word Channel Type Profile */
    rc = lion3MetaDataFeildsUsedSet(devNum,
            keyType,
            PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(lion3LegacyKey_mpls_metadata10_fieldsArr),
            index,
            &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    /* Protocol after MPLS
       Reserved Label Exists
       Data after inner label */
    rc = lion3MetaDataFeildsUsedSet(devNum,
            keyType,
            PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(lion3LegacyKey_mpls_metadata9_fieldsArr),
            index,
            &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    return GT_OK;
}

/*******************************************************************************
* lion3BuildUdbsNeededForLegacyKey_ethernet
*
* DESCRIPTION:
*       build legacy key needed UDBs - ethernet key
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - the key type that need ethernet legacy style
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lion3BuildUdbsNeededForLegacyKey_ethernet
(
    IN GT_U8   devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT keyType
)
{
    GT_STATUS  rc;
    GT_U32 index;
    GT_U32  startIndex;
    GT_U32  numOfUdbs;
    LION3_TTI_BYTES_FROM_PACKET_ENT partsArr[]=
    {
        LION3_TTI_BYTES_FROM_PACKET_L3_ETHERTYPE_E
    };
    GT_U32  ii;

    /* init UDBs for CPSS_DXCH_TTI_KEY_ETH_E */
    ttiInitUserDefinedByteCommonSet(devNum,keyType);
    startIndex = lion3LegacyMacSaDaIndexInKey + 6;/* start after the 6 UDBs of mac sa/da*/

    index = startIndex;

    /* UDB15-16: Anchor=Desc; Offset=6,7 */
    /* VLAN1
       UP1
       CFI1 */
    rc = lion3MetaDataFeildsUsedSet(devNum,
            keyType,
            PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(lion3LegacyKey_ethernet_metadata14_15_fieldsArr),
            index,
            &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    for(ii = 0 ; ii < (sizeof(partsArr)/sizeof(partsArr[0])) ; ii++)
    {
        rc = lion3WellKnownPacketPartsUsedSet(devNum,keyType,
            partsArr[ii],
            index,
            &numOfUdbs);
        if (rc != GT_OK)
        {
            return rc;
        }

        index += numOfUdbs;
    }

    /* UDB19-22: Anchor=Desc; Offset=8-11 */
    /* VLAN Tag1 Exists
       Tag0 TPID Index
       Tag1 TPID Index
       DSA QoS Profile
       DSA Source ID [11:0] */
    rc = lion3MetaDataFeildsUsedSet(devNum,
            keyType,
            PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(lion3LegacyKey_ethernet_metadata16_19_fieldsArr),
            index,
            &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    return GT_OK;
}

/*******************************************************************************
* lion3BuildUdbsNeededForLegacyKey_mim
*
* DESCRIPTION:
*       build legacy key needed UDBs - mim key (802.1ah)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - the key type that need mim legacy style
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lion3BuildUdbsNeededForLegacyKey_mim
(
    IN GT_U8   devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT keyType
)
{
    GT_STATUS  rc;
    GT_U32 index;
    GT_U32  startIndex;
    GT_U32  numOfUdbs;
    LION3_TTI_BYTES_FROM_PACKET_ENT partsArr[]=
    {
        LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SID_E,
        LION3_TTI_BYTES_FROM_PACKET_L3_MIM_I_SERVICE_PRI_E
    };
    GT_U32  ii;

    /* init UDBs for CPSS_DXCH_TTI_KEY_MIM_E */
    ttiInitUserDefinedByteCommonSet(devNum,keyType);
    startIndex = lion3LegacyMacSaDaIndexInKey + 6;/* start after the 6 UDBs of mac sa/da*/

    index = startIndex;

    /* UDB14: Anchor=Desc; Offset=3 */
    /* UP0 (B-UP)
       CFI0 (B-DEI)
       MAC2ME
       Passenger S-tag DEI */
    for(ii = 0 ; ii < (sizeof(partsArr)/sizeof(partsArr[0])) ; ii++)
    {
        rc = lion3WellKnownPacketPartsUsedSet(devNum,keyType,
            partsArr[ii],
            index,
            &numOfUdbs);
        if (rc != GT_OK)
        {
            return rc;
        }

        index += numOfUdbs;
    }

    /* UDB19-20: Anchor=Desc; Offset=4,5 */
    /* Passenger S-tag VID
       Passenger S-tag UP
       Passenger S-tag exists */
    rc = lion3MetaDataFeildsUsedSet(devNum,
            keyType,
            PRV_CPSS_DXCH_TTI_ARRAY_AND_SIZE_MAC(lion3LegacyKey_mim_metadata12_13_fieldsArr),
            index,
            &numOfUdbs);
    if (rc != GT_OK)
    {
        return rc;
    }

    index += numOfUdbs;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChTtiDbTablesInit
*
* DESCRIPTION:
*       init TTI tables internal DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2 ; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS  prvCpssDxChTtiDbTablesInit
(
    IN GT_U8   devNum
)
{
    GT_STATUS  rc;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        rc = prvCpssFillFieldsStartBitInfo(LION3_TTI_META_DATA_FIELDS___LAST_VALUE___E,
                lion3TtiMetaDataFieldsFormat);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssFillFieldsStartBitInfo(LION3_TTI_LEGACY_KEY_FIELDS___LAST_VALUE___E,
                lion3TtiLegacyKeyFieldsFormat);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* state the needed parts in packet,metadata that needed for legacy IPv4 key */
        rc = lion3BuildUdbsNeededForLegacyKey_ipv4(devNum,CPSS_DXCH_TTI_KEY_IPV4_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* state the needed parts in packet,metadata that needed for legacy mpls key */
        rc = lion3BuildUdbsNeededForLegacyKey_mpls(devNum,CPSS_DXCH_TTI_KEY_MPLS_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* state the needed parts in packet,metadata that needed for legacy ethernet key */
        rc = lion3BuildUdbsNeededForLegacyKey_ethernet(devNum,CPSS_DXCH_TTI_KEY_ETH_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* state the needed parts in packet,metadata that needed for legacy mim key */
        rc = lion3BuildUdbsNeededForLegacyKey_mim(devNum,CPSS_DXCH_TTI_KEY_MIM_E);
        if(rc != GT_OK)
        {
            return rc;
        }

    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChTtiPacketTypeKeySizeSet
*
* DESCRIPTION:
*       function sets key type size.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       size          - key size in TCAM
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPacketTypeKeySizeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN CPSS_DXCH_TTI_KEY_SIZE_ENT           size
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      entryIndex;
    GT_U32      entrySize;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = ttiGetIndexForKeyType(keyType, GT_FALSE, &entryIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (size)
    {
    case CPSS_DXCH_TTI_KEY_SIZE_10_B_E:
        entrySize = 0;
        break;
    case CPSS_DXCH_TTI_KEY_SIZE_20_B_E:
        entrySize = 1;
        break;
    case CPSS_DXCH_TTI_KEY_SIZE_30_B_E:
        entrySize = 2;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];

    return prvCpssHwPpSetRegField(devNum, regAddr, 16 * (entryIndex % 2) + 10, 2, entrySize);

}

/*******************************************************************************
* cpssDxChTtiPacketTypeKeySizeSet
*
* DESCRIPTION:
*       function sets key type size.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       size          - key size in TCAM
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPacketTypeKeySizeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN CPSS_DXCH_TTI_KEY_SIZE_ENT           size
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPacketTypeKeySizeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, size));

    rc = internal_cpssDxChTtiPacketTypeKeySizeSet(devNum, keyType, size);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, size));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPacketTypeKeySizeGet
*
* DESCRIPTION:
*       function gets key type size.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*
* OUTPUTS:
*       sizePtr      - points to key size in TCAM
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPacketTypeKeySizeGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    OUT CPSS_DXCH_TTI_KEY_SIZE_ENT           *sizePtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      entryIndex;
    GT_U32      entrySize;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(sizePtr);

    rc = ttiGetIndexForKeyType(keyType, GT_FALSE, &entryIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIPktTypeKeys[entryIndex / 2];

    rc = prvCpssHwPpGetRegField(devNum, regAddr, (16 * (entryIndex % 2)) + 10, 2, &entrySize);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (entrySize)
    {
    case 0:
        *sizePtr = CPSS_DXCH_TTI_KEY_SIZE_10_B_E;
        break;
    case 1:
        *sizePtr = CPSS_DXCH_TTI_KEY_SIZE_20_B_E;
        break;
    case 2:
        *sizePtr = CPSS_DXCH_TTI_KEY_SIZE_30_B_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiPacketTypeKeySizeGet
*
* DESCRIPTION:
*       function gets key type size.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*
* OUTPUTS:
*       sizePtr      - points to key size in TCAM
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPacketTypeKeySizeGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    OUT CPSS_DXCH_TTI_KEY_SIZE_ENT           *sizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPacketTypeKeySizeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, sizePtr));

    rc = internal_cpssDxChTtiPacketTypeKeySizeGet(devNum, keyType, sizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, sizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* lion3TcamSegmentModeInfoGet
*
* DESCRIPTION:
*   Gets the info need to use for tcam segment configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType              - the key type
*
* OUTPUTS:
*       startBitPtr         - (pointer to) the start bit in HW
*       numOfBitsPtr        - (pointer to) the number of bits in HW
*       regAddrPtr          - (pointer to) the register to read/write
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong key type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lion3TcamSegmentModeInfoGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT GT_U32                              *startBitPtr,
    OUT GT_U32                              *numOfBitsPtr,
    OUT GT_U32                              *regAddrPtr
)
{
    GT_STATUS   rc;
    GT_U32      offset;

    rc = ttiGetIndexForKeyType(keyType, GT_TRUE, &offset);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        *startBitPtr = (offset % 10) * 3;
        *numOfBitsPtr = 3;
        if(offset > 10)
        {
            *regAddrPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIKeySegmentMode1;
        }
        else
        {
            *regAddrPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIKeySegmentMode0;
        }
    }
    else
    {
        *startBitPtr  = offset;
        *numOfBitsPtr = 1;
        *regAddrPtr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIEngine.TTIKeySegmentMode0;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChTtiTcamSegmentModeSet
*
* DESCRIPTION:
*       Sets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type
*       segmentMode   - TTI TCAM segment mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiTcamSegmentModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT               keyType,
    IN CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT  segmentMode
)
{
    GT_U32      regAddr;
    GT_U32      startBit;
    GT_U32      value;
    GT_U32      numOfBits;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    rc = lion3TcamSegmentModeInfoGet(devNum, keyType, &startBit, &numOfBits, &regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        switch (segmentMode)
        {
            case CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ONE_TCAM_E:
                value = 0;
                break;
            case CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_2_AND_2_E:
                value = 1;
                break;
            case CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_1_AND_3_E:
                value = 2;
                break;
            case CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_3_AND_1_E:
                value = 3;
                break;
            case CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_FOUR_TCAM_E:
                value = 4;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        switch (segmentMode)
        {
            case CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ONE_TCAM_E:
                value = 0;
                break;
            case CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_E:
                value = 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, startBit, numOfBits, value);
}

/*******************************************************************************
* cpssDxChTtiTcamSegmentModeSet
*
* DESCRIPTION:
*       Sets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type
*       segmentMode   - TTI TCAM segment mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTcamSegmentModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT               keyType,
    IN CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT  segmentMode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiTcamSegmentModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, segmentMode));

    rc = internal_cpssDxChTtiTcamSegmentModeSet(devNum, keyType, segmentMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, segmentMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChTtiTcamSegmentModeGet
*
* DESCRIPTION:
*       Gets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       keyType         - key type
*
* OUTPUTS:
*       segmentModePtr  - (pointer to) TTI TCAM segment mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on invalid HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiTcamSegmentModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT              keyType,
    OUT CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT *segmentModePtr
)
{
    GT_U32      regAddr;
    GT_U32      startBit;
    GT_U32      value;
    GT_U32      numOfBits;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(segmentModePtr);

    rc = lion3TcamSegmentModeInfoGet(devNum, keyType, &startBit, &numOfBits, &regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc =  prvCpssHwPpGetRegField(devNum, regAddr, startBit, numOfBits, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        switch (value)
        {
            case 0:
                *segmentModePtr = CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ONE_TCAM_E;
                break;
            case 1:
                *segmentModePtr = CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_2_AND_2_E;
                break;
            case 2:
                *segmentModePtr = CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_1_AND_3_E;
                break;
            case 3:
                *segmentModePtr = CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_3_AND_1_E;
                break;
            case 4:
                *segmentModePtr = CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_FOUR_TCAM_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        *segmentModePtr =
            ((value == 0) ?
             CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ONE_TCAM_E :
             CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_E);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTtiTcamSegmentModeGet
*
* DESCRIPTION:
*       Gets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       keyType         - key type
*
* OUTPUTS:
*       segmentModePtr  - (pointer to) TTI TCAM segment mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on invalid HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiTcamSegmentModeGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT              keyType,
    OUT CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT *segmentModePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiTcamSegmentModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, segmentModePtr));

    rc = internal_cpssDxChTtiTcamSegmentModeGet(devNum, keyType, segmentModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, segmentModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiMcTunnelDuplicationModeSet
*
* DESCRIPTION:
*       Set the tunnel duplication mode for a specific protocol on a specific
*       port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       protocol      - the protocol
*       mode          - the tunnel duplication mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMcTunnelDuplicationModeSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN CPSS_TUNNEL_MULTICAST_TYPE_ENT                   protocol,
    IN CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT     mode
)
{
    GT_U32      field;
    GT_U32      hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* check mode validity */
    if ((protocol == CPSS_TUNNEL_MULTICAST_IPV4_E) ||
        (protocol == CPSS_TUNNEL_MULTICAST_IPV6_E))
    {
        if (mode > CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_ANY_UDP_PORT_E)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    if ((protocol == CPSS_TUNNEL_MULTICAST_MPLS_E) ||
        (protocol == CPSS_TUNNEL_MULTICAST_TRILL_E) ||
        (protocol == CPSS_TUNNEL_MULTICAST_PBB_E))
    {
        if ((mode != CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_DISABLE_E) &&
            (mode != CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_ALL_E))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    switch (protocol)
    {
        case CPSS_TUNNEL_MULTICAST_IPV4_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_MC_DUPLICATION_MODE_E;
            break;

        case CPSS_TUNNEL_MULTICAST_IPV6_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV6_MC_DUPLICATION_MODE_E;
            break;

        case CPSS_TUNNEL_MULTICAST_MPLS_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_MPLS_MC_DUPLICATION_ENABLE_E;
            break;

        case CPSS_TUNNEL_MULTICAST_TRILL_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRILL_MC_DUPLICATION_ENABLE_E;
            break;

        case CPSS_TUNNEL_MULTICAST_PBB_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PBB_MC_DUPLICATION_ENABLE_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (mode)
    {
        case CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_DISABLE_E:
            hwData = 0;
            break;

        case CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_ALL_E:
            hwData = 1;
            break;

        case CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_SPECIFIC_GRE_E:
            hwData = 2;
            break;

        case CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_ANY_GRE_E:
            hwData = 3;
            break;

        case CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_SPECIFIC_UDP_PORT_E:
            hwData = 4;
            break;

        case CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_ANY_UDP_PORT_E:
            hwData = 5;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* write to pre-tti-lookup-ingress-eport table */
    return prvCpssDxChWriteTableEntryField(devNum,
                                           PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                           portNum,
                                           PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                           field,
                                           PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                           hwData);
}

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationModeSet
*
* DESCRIPTION:
*       Set the tunnel duplication mode for a specific protocol on a specific
*       port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       protocol      - the protocol
*       mode          - the tunnel duplication mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMcTunnelDuplicationModeSet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN CPSS_TUNNEL_MULTICAST_TYPE_ENT                   protocol,
    IN CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT     mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMcTunnelDuplicationModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, protocol, mode));

    rc = internal_cpssDxChTtiMcTunnelDuplicationModeSet(devNum, portNum, protocol, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, protocol, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiMcTunnelDuplicationModeGet
*
* DESCRIPTION:
*       Get the tunnel duplication mode for a specific protocol on a specific
*       port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       protocol      - the protocol
*
* OUTPUTS:
*       modePtr       - (pointer to) the tunnel duplication mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMcTunnelDuplicationModeGet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN  CPSS_TUNNEL_MULTICAST_TYPE_ENT                  protocol,
    OUT CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT    *modePtr
)
{
    GT_U32      field;
    GT_U32      hwData;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_DEFAULT_EPORT_CHECK_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    switch (protocol)
    {
        case CPSS_TUNNEL_MULTICAST_IPV4_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV4_MC_DUPLICATION_MODE_E;
            break;

        case CPSS_TUNNEL_MULTICAST_IPV6_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_IPV6_MC_DUPLICATION_MODE_E;
            break;

        case CPSS_TUNNEL_MULTICAST_MPLS_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_MPLS_MC_DUPLICATION_ENABLE_E;
            break;

        case CPSS_TUNNEL_MULTICAST_TRILL_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_TRILL_MC_DUPLICATION_ENABLE_E;
            break;

        case CPSS_TUNNEL_MULTICAST_PBB_E:
            field = LION3_TTI_DEFAULT_EPORT_TABLE_FIELDS_PBB_MC_DUPLICATION_ENABLE_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* read from pre-tti-lookup-ingress-eport table */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_LION3_TABLE_PRE_TTI_LOOKUP_INGRESS_EPORT_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_FIELD_NAME_CNS,
                                        field,
                                        PRV_CPSS_DXCH_TABLES_WORD_INDICATE_AUTO_CALC_LENGTH_CNS,
                                        &hwData);
    if (rc == GT_OK)
    {
        switch (hwData)
        {
            case 0:
                *modePtr = CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_DISABLE_E;
                break;

            case 1:
                *modePtr = CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_ALL_E;
                break;

            case 2:
                *modePtr = CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_SPECIFIC_GRE_E;
                break;

            case 3:
                *modePtr = CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_ANY_GRE_E;
                break;

            case 4:
                *modePtr = CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_SPECIFIC_UDP_PORT_E;
                break;

            case 5:
                *modePtr = CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENABLE_FOR_ANY_UDP_PORT_E;
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationModeGet
*
* DESCRIPTION:
*       Get the tunnel duplication mode for a specific protocol on a specific
*       port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*                       In eArch devices portNum is default ePort.
*       protocol      - the protocol
*
* OUTPUTS:
*       modePtr       - (pointer to) the tunnel duplication mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMcTunnelDuplicationModeGet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_NUM                                      portNum,
    IN  CPSS_TUNNEL_MULTICAST_TYPE_ENT                  protocol,
    OUT CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT    *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMcTunnelDuplicationModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, protocol, modePtr));

    rc = internal_cpssDxChTtiMcTunnelDuplicationModeGet(devNum, portNum, protocol, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, protocol, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiMcTunnelDuplicationUdpDestPortSet
*
* DESCRIPTION:
*       Set the UDP destination port used to trigger IPv4/IPv6 Multicast
*       replication
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       udpPort   - the UDP destination port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This configuration is relevant only when default ePort
*       <IPv4 MC Duplication IP Protocol Mode> = UDP or Default ePort
*       <IPv6 MC Duplication IP Protocol Mode> = UDP
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMcTunnelDuplicationUdpDestPortSet
(
    IN GT_U8    devNum,
    IN GT_U16   udpPort
)
{
    GT_U32      regAddr;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.duplicationConfigs1;

    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 16, (GT_U32)udpPort);
}

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationUdpDestPortSet
*
* DESCRIPTION:
*       Set the UDP destination port used to trigger IPv4/IPv6 Multicast
*       replication
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       udpPort   - the UDP destination port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This configuration is relevant only when default ePort
*       <IPv4 MC Duplication IP Protocol Mode> = UDP or Default ePort
*       <IPv6 MC Duplication IP Protocol Mode> = UDP
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMcTunnelDuplicationUdpDestPortSet
(
    IN GT_U8    devNum,
    IN GT_U16   udpPort
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMcTunnelDuplicationUdpDestPortSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, udpPort));

    rc = internal_cpssDxChTtiMcTunnelDuplicationUdpDestPortSet(devNum, udpPort);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, udpPort));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiMcTunnelDuplicationUdpDestPortGet
*
* DESCRIPTION:
*       Get the UDP destination port used to trigger IPv4/IPv6 Multicast
*       replication
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       udpPortPtr - (pointer to) the UDP destination port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This configuration is relevant only when default ePort
*       <IPv4 MC Duplication IP Protocol Mode> = UDP or Default ePort
*       <IPv6 MC Duplication IP Protocol Mode> = UDP
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMcTunnelDuplicationUdpDestPortGet
(
    IN GT_U8    devNum,
    OUT GT_U16  *udpPortPtr
)
{
    GT_U32      regAddr;
    GT_U32      value;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(udpPortPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.duplicationConfigs1;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 16, &value);
    if (rc == GT_OK)
    {
        *udpPortPtr = (GT_U16)value;
    }
    return rc;
}

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationUdpDestPortGet
*
* DESCRIPTION:
*       Get the UDP destination port used to trigger IPv4/IPv6 Multicast
*       replication
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       udpPortPtr - (pointer to) the UDP destination port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This configuration is relevant only when default ePort
*       <IPv4 MC Duplication IP Protocol Mode> = UDP or Default ePort
*       <IPv6 MC Duplication IP Protocol Mode> = UDP
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMcTunnelDuplicationUdpDestPortGet
(
    IN GT_U8    devNum,
    OUT GT_U16  *udpPortPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMcTunnelDuplicationUdpDestPortGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, udpPortPtr));

    rc = internal_cpssDxChTtiMcTunnelDuplicationUdpDestPortGet(devNum, udpPortPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, udpPortPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiMplsMcTunnelTriggeringMacDaSet
*
* DESCRIPTION:
*       Set the MPLS multicast MAC DA and mask used for duplication triggering
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       addressPtr  - (pointer to) the MAC DA
*       maskPtr     - (pointer to) the address mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMplsMcTunnelTriggeringMacDaSet
(
    IN GT_U8            devNum,
    IN GT_ETHERADDR     *addressPtr,
    IN GT_ETHERADDR     *maskPtr
)
{
    GT_U32      regAddr;
    GT_U32      hwData;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(addressPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.MPLSMCDALow;
    hwData = (addressPtr->arEther[5]) |
             (addressPtr->arEther[4] << 8) |
             (addressPtr->arEther[3] << 16);
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 24, hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.MPLSMCDAHigh;
    hwData = (addressPtr->arEther[2]) |
             (addressPtr->arEther[1] << 8) |
             (addressPtr->arEther[0] << 16);
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 24, hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.MPLSMCDALowMask;
    hwData = (maskPtr->arEther[5]) |
             (maskPtr->arEther[4] << 8) |
             (maskPtr->arEther[3] << 16);
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 24, hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.MPLSMCDAHighMask;
    hwData = (maskPtr->arEther[2]) |
             (maskPtr->arEther[1] << 8) |
             (maskPtr->arEther[0] << 16);
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 24, hwData);
}

/*******************************************************************************
* cpssDxChTtiMplsMcTunnelTriggeringMacDaSet
*
* DESCRIPTION:
*       Set the MPLS multicast MAC DA and mask used for duplication triggering
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       addressPtr  - (pointer to) the MAC DA
*       maskPtr     - (pointer to) the address mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMplsMcTunnelTriggeringMacDaSet
(
    IN GT_U8            devNum,
    IN GT_ETHERADDR     *addressPtr,
    IN GT_ETHERADDR     *maskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMplsMcTunnelTriggeringMacDaSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, addressPtr, maskPtr));

    rc = internal_cpssDxChTtiMplsMcTunnelTriggeringMacDaSet(devNum, addressPtr, maskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, addressPtr, maskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiMplsMcTunnelTriggeringMacDaGet
*
* DESCRIPTION:
*       Get the MPLS multicast MAC DA and mask used for duplication triggering
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       addressPtr - (pointer to) the MAC DA
*       maskPtr    - (pointer to) the address mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiMplsMcTunnelTriggeringMacDaGet
(
    IN GT_U8            devNum,
    OUT GT_ETHERADDR    *addressPtr,
    OUT GT_ETHERADDR    *maskPtr
)
{
    GT_U32      regAddr;
    GT_U32      hwData;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(addressPtr);
    CPSS_NULL_PTR_CHECK_MAC(maskPtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.MPLSMCDALow;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 24, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    addressPtr->arEther[5] = (GT_U8)(hwData & 0xFF);
    addressPtr->arEther[4] = (GT_U8)((hwData >> 8) & 0xFF);
    addressPtr->arEther[3] = (GT_U8)((hwData >> 16) & 0xFF);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.MPLSMCDAHigh;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 24, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    addressPtr->arEther[2] = (GT_U8)(hwData & 0xFF);
    addressPtr->arEther[1] = (GT_U8)((hwData >> 8) & 0xFF);
    addressPtr->arEther[0] = (GT_U8)((hwData >> 16) & 0xFF);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.MPLSMCDALowMask;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 24, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    maskPtr->arEther[5] = (GT_U8)(hwData & 0xFF);
    maskPtr->arEther[4] = (GT_U8)((hwData >> 8) & 0xFF);
    maskPtr->arEther[3] = (GT_U8)((hwData >> 16) & 0xFF);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).duplicationConfigs.MPLSMCDAHighMask;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 24, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }
    maskPtr->arEther[2] = (GT_U8)(hwData & 0xFF);
    maskPtr->arEther[1] = (GT_U8)((hwData >> 8) & 0xFF);
    maskPtr->arEther[0] = (GT_U8)((hwData >> 16) & 0xFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTtiMplsMcTunnelTriggeringMacDaGet
*
* DESCRIPTION:
*       Get the MPLS multicast MAC DA and mask used for duplication triggering
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       addressPtr - (pointer to) the MAC DA
*       maskPtr    - (pointer to) the address mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiMplsMcTunnelTriggeringMacDaGet
(
    IN GT_U8            devNum,
    OUT GT_ETHERADDR    *addressPtr,
    OUT GT_ETHERADDR    *maskPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiMplsMcTunnelTriggeringMacDaGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, addressPtr, maskPtr));

    rc = internal_cpssDxChTtiMplsMcTunnelTriggeringMacDaGet(devNum, addressPtr, maskPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, addressPtr, maskPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPwCwExceptionCmdSet
*
* DESCRIPTION:
*       Set a PW CW exception command
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       exceptionType   - exception type
*       command         - the packet command
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported mode
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPwCwExceptionCmdSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT    exceptionType,
    IN CPSS_PACKET_CMD_ENT                  command
)
{
    GT_U32      regAddr;
    GT_U32      offset;
    GT_U32      value;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    if ((PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_FALSE) &&
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    switch (exceptionType)
    {
        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_ILLEGAL_CONTROL_WORD_E:
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
            offset = 0;
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_EXPIRY_VCCV_E:
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
            offset = 3;
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_NON_DATA_CONTROL_WORD_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
                offset = 6;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiPwConfig;
                offset = 1;
            }
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_FRAGMENTED_E:
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
            offset = 9;
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_SEQUENCE_ERROR_E:
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
            offset = 12;
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_PASSENGER_PW_TAG0_NOT_FOUND_ERROR_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);
            switch (command)
            {
                case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
                case CPSS_PACKET_CMD_DROP_SOFT_E:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

                default:
                    /* nothing to do */
                    break;
            }
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiPwConfig;
            offset = 12;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(value,command);
    return prvCpssHwPpSetRegField(devNum, regAddr, offset, 3, value);
}

/*******************************************************************************
* cpssDxChTtiPwCwExceptionCmdSet
*
* DESCRIPTION:
*       Set a PW CW exception command
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       exceptionType   - exception type
*       command         - the packet command
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported mode
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwExceptionCmdSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT    exceptionType,
    IN CPSS_PACKET_CMD_ENT                  command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPwCwExceptionCmdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, exceptionType, command));

    rc = internal_cpssDxChTtiPwCwExceptionCmdSet(devNum, exceptionType, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, exceptionType, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPwCwExceptionCmdGet
*
* DESCRIPTION:
*       Get a PW CW exception command
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       exceptionType   - exception type
*
* OUTPUTS:
*       commandPtr - (pointer to) the packet command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported mode
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPwCwExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT   exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
{
    GT_U32      regAddr;
    GT_U32      offset;
    GT_U32      value;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    if ((PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_MAC(devNum) == GT_FALSE) &&
        (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    switch (exceptionType)
    {
        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_ILLEGAL_CONTROL_WORD_E:
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
            offset = 0;
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_EXPIRY_VCCV_E:
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
            offset = 3;
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_NON_DATA_CONTROL_WORD_E:
            if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
                offset = 6;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiPwConfig;
                offset = 1;
            }
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_FRAGMENTED_E:
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
            offset = 9;
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_SEQUENCE_ERROR_E:
            PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;
            offset = 12;
            break;

        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_PASSENGER_PW_TAG0_NOT_FOUND_ERROR_E:
            PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiPwConfig;
            offset = 12;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 3, &value);
    if (rc == GT_OK)
    {
        PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(*commandPtr,value);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChTtiPwCwExceptionCmdGet
*
* DESCRIPTION:
*       Get a PW CW exception command
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       exceptionType   - exception type
*
* OUTPUTS:
*       commandPtr - (pointer to) the packet command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported mode
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT   exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPwCwExceptionCmdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, exceptionType, commandPtr));

    rc = internal_cpssDxChTtiPwCwExceptionCmdGet(devNum, exceptionType, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, exceptionType, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPwCwCpuCodeBaseSet
*
* DESCRIPTION:
*       Set the base CPU code value for PWE3
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       cpuCodeBase - the base CPU code value for PWE3
*                     (APPLICABLE RANGES: 192..251)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPwCwCpuCodeBaseSet
(
    IN GT_U8    devNum,
    IN GT_U32   cpuCodeBase
)
{
    GT_U32      regAddr;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    if ((cpuCodeBase < CPU_CODE_BASE_MIN_CNS) ||
        (cpuCodeBase > PRV_CPSS_DXCH_TTI_PWE3_CPU_CODE_BASE_MAX_CNS))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;

    return prvCpssHwPpSetRegField(devNum, regAddr, 15, 8, cpuCodeBase);
}

/*******************************************************************************
* cpssDxChTtiPwCwCpuCodeBaseSet
*
* DESCRIPTION:
*       Set the base CPU code value for PWE3
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       cpuCodeBase - the base CPU code value for PWE3
*                     (APPLICABLE RANGES: 192..251)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwCpuCodeBaseSet
(
    IN GT_U8    devNum,
    IN GT_U32   cpuCodeBase
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPwCwCpuCodeBaseSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCodeBase));

    rc = internal_cpssDxChTtiPwCwCpuCodeBaseSet(devNum, cpuCodeBase);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCodeBase));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPwCwCpuCodeBaseGet
*
* DESCRIPTION:
*       Get the base CPU code value for PWE3
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*
* OUTPUTS:
*       cpuCodeBasePtr - (pointer to) the base CPU code value for PWE3
*                        (APPLICABLE RANGES: 192..255)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPwCwCpuCodeBaseGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *cpuCodeBasePtr
)
{
    GT_U32      regAddr;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodeBasePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;

    return prvCpssHwPpGetRegField(devNum, regAddr, 15, 8, cpuCodeBasePtr);
}

/*******************************************************************************
* cpssDxChTtiPwCwCpuCodeBaseGet
*
* DESCRIPTION:
*       Get the base CPU code value for PWE3
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*
* OUTPUTS:
*       cpuCodeBasePtr - (pointer to) the base CPU code value for PWE3
*                        (APPLICABLE RANGES: 192..255)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwCpuCodeBaseGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *cpuCodeBasePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPwCwCpuCodeBaseGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCodeBasePtr));

    rc = internal_cpssDxChTtiPwCwCpuCodeBaseGet(devNum, cpuCodeBasePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCodeBasePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPwCwExceptionCpuCodeSet
*
* DESCRIPTION:
*       Set tunnel termination exception CPU code for packets trapped or mirrored to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for. valid values:
*                           CPSS_DXCH_TTI_PW_CW_EXCEPTION_NON_DATA_CONTROL_WORD_E
*                           CPSS_DXCH_TTI_PW_CW_EXCEPTION_PASSENGER_PW_TAG0_NOT_FOUND_ERROR_E
*       cpuCode       - CPU code for packets trapped or mirrored to the CPU.
*                       (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwExceptionCpuCodeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT   exceptionType,
    IN  CPSS_NET_RX_CPU_CODE_ENT            cpuCode
)
{
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */
    GT_U32      offset;     /* offset in the register */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;   /* DSA  Cpu Code */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

    if(cpuCode >= BIT_9)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (exceptionType)
    {
        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_NON_DATA_CONTROL_WORD_E:
            offset = 4;
            break;
        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_PASSENGER_PW_TAG0_NOT_FOUND_ERROR_E:
            offset = 15;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChNetIfCpuToDsaCode(cpuCode,&dsaCpuCode);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* write value to hardware register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiPwConfig;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, offset, 8, (GT_U32)dsaCpuCode);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPwCwExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get tunnel termination exception CPU code for packets trapped or mirrored to the CPU.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for. valid values:
*                           CPSS_DXCH_TTI_PW_CW_EXCEPTION_NON_DATA_CONTROL_WORD_E
*                           CPSS_DXCH_TTI_PW_CW_EXCEPTION_PASSENGER_PW_TAG0_NOT_FOUND_ERROR_E
* OUTPUTS:
*       cpuCodePtr    - (pointer to) CPU code for packets trapped or mirrored by the feature.
*                       (APPLICABLE RANGES: 0..255)

*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwExceptionCpuCodeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT   exceptionType,
    OUT CPSS_NET_RX_CPU_CODE_ENT            *cpuCodePtr
)
{
    GT_U32      value;      /* value to write */
    GT_U32      regAddr;    /* register address */
    GT_STATUS   rc;         /* function return code */
    GT_U32      offset;     /* offset in the register */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E | CPSS_XCAT2_E |
         CPSS_LION2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);
    PRV_CPSS_DXCH_PP_HW_INFO_VPLS_MODE_SUPPORTED_CHECK_MAC(devNum);

    switch (exceptionType)
    {
        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_NON_DATA_CONTROL_WORD_E:
            offset = 4;
            break;
        case CPSS_DXCH_TTI_PW_CW_EXCEPTION_PASSENGER_PW_TAG0_NOT_FOUND_ERROR_E:
            offset = 15;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* read value from hardware register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.ttiPwConfig;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, offset, 8, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChNetIfDsaToCpuCode((PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)value, cpuCodePtr);
    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPwCwSequencingSupportEnableSet
*
* DESCRIPTION:
*       This function enables/disables the check for Pseudo Wire-Control Word
*       Data Word format <Sequence Number>==0 in terminated Pseudo Wires.
*       < Sequence Number > are the 16 LSB of "CW as Data Word format".
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: PW-CW sequencing supported
*                       GT_FALSE: PW-CW sequencing is not supported
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPwCwSequencingSupportEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_U32      regAddr;    /* register address */

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    /* write value */
    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;

    return prvCpssHwPpSetRegField(devNum,regAddr,23,1,(BOOL2BIT_MAC(enable)));

}

/*******************************************************************************
* cpssDxChTtiPwCwSequencingSupportEnableSet
*
* DESCRIPTION:
*       This function enables/disables the check for Pseudo Wire-Control Word
*       Data Word format <Sequence Number>==0 in terminated Pseudo Wires.
*       < Sequence Number > are the 16 LSB of "CW as Data Word format".
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: PW-CW sequencing supported
*                       GT_FALSE: PW-CW sequencing is not supported
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwSequencingSupportEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPwCwSequencingSupportEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChTtiPwCwSequencingSupportEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiPwCwSequencingSupportEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of the check
*       for Pseudo Wire-Control Word Data Word format <Sequence Number>==0
*       in terminated Pseudo Wires.
*       < Sequence Number > are the 16 LSB of "CW as Data Word format".
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable PW-CW sequencing support
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiPwCwSequencingSupportEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value = 0;  /* hardware write */
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* write value */
    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).pseudoWire.PWE3Config;

    rc = prvCpssHwPpGetRegField(devNum,regAddr,23,1,&value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChTtiPwCwSequencingSupportEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of the check
*       for Pseudo Wire-Control Word Data Word format <Sequence Number>==0
*       in terminated Pseudo Wires.
*       < Sequence Number > are the 16 LSB of "CW as Data Word format".
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable PW-CW sequencing support
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiPwCwSequencingSupportEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiPwCwSequencingSupportEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChTtiPwCwSequencingSupportEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiIPv6ExtensionHeaderSet
*
* DESCRIPTION:
*       Set one of the 2 configurable IPv6 extension headers.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       extensionHeaderId    - the configurable extension header index
*                              (APPLICABLE RANGES: 0..1)
*       extensionHeaderValue - the configurable extension header value
*                              (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The design assumes that the information following the IPv6 header is L4
*       unless there is a known next hop header. The design supports several
*       hard-coded next hop header values and two configurable ones.
*       If any of the following next hop values are identified, the <Is L4 Valid>
*       indication in the IPCL key is set to NOT VALID:
*       - HBH (0)
*       - IPv6 Routing header (43)
*       - IPv6 Fragment header (44)
*       - Encapsulation Security Payload (50)
*       - Authentication Header (51)
*       - IPv6 Destination Options (60)
*       - Mobility Header (135)
*       - <IPv6 Extension Value0>
*       - <IPv6 Extension Value1>
*       To disable a configurable header, set its value to one of the above
*       values.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiIPv6ExtensionHeaderSet
(
    IN GT_U8    devNum,
    IN GT_U32   extensionHeaderId,
    IN GT_U32   extensionHeaderValue
)
{
    GT_U32      regAddr;
    GT_U32      offset;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    switch (extensionHeaderId)
    {
        case 0:
            offset = 0;
            break;

        case 1:
            offset = 8;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (extensionHeaderValue > 255)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).IPv6ExtensionValue;

    return prvCpssHwPpSetRegField(devNum, regAddr, offset, 8, extensionHeaderValue);
}

/*******************************************************************************
* cpssDxChTtiIPv6ExtensionHeaderSet
*
* DESCRIPTION:
*       Set one of the 2 configurable IPv6 extension headers.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       extensionHeaderId    - the configurable extension header index
*                              (APPLICABLE RANGES: 0..1)
*       extensionHeaderValue - the configurable extension header value
*                              (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The design assumes that the information following the IPv6 header is L4
*       unless there is a known next hop header. The design supports several
*       hard-coded next hop header values and two configurable ones.
*       If any of the following next hop values are identified, the <Is L4 Valid>
*       indication in the IPCL key is set to NOT VALID:
*       - HBH (0)
*       - IPv6 Routing header (43)
*       - IPv6 Fragment header (44)
*       - Encapsulation Security Payload (50)
*       - Authentication Header (51)
*       - IPv6 Destination Options (60)
*       - Mobility Header (135)
*       - <IPv6 Extension Value0>
*       - <IPv6 Extension Value1>
*       To disable a configurable header, set its value to one of the above
*       values.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIPv6ExtensionHeaderSet
(
    IN GT_U8    devNum,
    IN GT_U32   extensionHeaderId,
    IN GT_U32   extensionHeaderValue
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiIPv6ExtensionHeaderSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, extensionHeaderId, extensionHeaderValue));

    rc = internal_cpssDxChTtiIPv6ExtensionHeaderSet(devNum, extensionHeaderId, extensionHeaderValue);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, extensionHeaderId, extensionHeaderValue));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiIPv6ExtensionHeaderGet
*
* DESCRIPTION:
*       Get one of the 2 configurable IPv6 extension headers.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*       extensionHeaderId        - the configurable extension header index
*                                  (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       extensionHeaderValuePtr  - the configurable extension header value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The design assumes that the information following the IPv6 header is L4
*       unless there is a known next hop header. The design supports several
*       hard-coded next hop header values and two configurable ones.
*       If any of the following next hop values are identified, the <Is L4 Valid>
*       indication in the IPCL key is set to NOT VALID:
*       - HBH (0)
*       - IPv6 Routing header (43)
*       - IPv6 Fragment header (44)
*       - Encapsulation Security Payload (50)
*       - Authentication Header (51)
*       - IPv6 Destination Options (60)
*       - Mobility Header (135)
*       - <IPv6 Extension Value0>
*       - <IPv6 Extension Value1>
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiIPv6ExtensionHeaderGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  extensionHeaderId,
    OUT GT_U32  *extensionHeaderValuePtr
)
{
    GT_U32      regAddr;
    GT_U32      offset;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(extensionHeaderValuePtr);

    switch (extensionHeaderId)
    {
        case 0:
            offset = 0;
            break;

        case 1:
            offset = 8;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).IPv6ExtensionValue;

    return prvCpssHwPpGetRegField(devNum, regAddr, offset, 8, extensionHeaderValuePtr);
}

/*******************************************************************************
* cpssDxChTtiIPv6ExtensionHeaderGet
*
* DESCRIPTION:
*       Get one of the 2 configurable IPv6 extension headers.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*       extensionHeaderId        - the configurable extension header index
*                                  (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       extensionHeaderValuePtr  - the configurable extension header value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The design assumes that the information following the IPv6 header is L4
*       unless there is a known next hop header. The design supports several
*       hard-coded next hop header values and two configurable ones.
*       If any of the following next hop values are identified, the <Is L4 Valid>
*       indication in the IPCL key is set to NOT VALID:
*       - HBH (0)
*       - IPv6 Routing header (43)
*       - IPv6 Fragment header (44)
*       - Encapsulation Security Payload (50)
*       - Authentication Header (51)
*       - IPv6 Destination Options (60)
*       - Mobility Header (135)
*       - <IPv6 Extension Value0>
*       - <IPv6 Extension Value1>
*
*******************************************************************************/
GT_STATUS cpssDxChTtiIPv6ExtensionHeaderGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  extensionHeaderId,
    OUT GT_U32  *extensionHeaderValuePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiIPv6ExtensionHeaderGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, extensionHeaderId, extensionHeaderValuePtr));

    rc = internal_cpssDxChTtiIPv6ExtensionHeaderGet(devNum, extensionHeaderId, extensionHeaderValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, extensionHeaderId, extensionHeaderValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiSourceIdBitsOverrideSet
*
* DESCRIPTION:
*       Set the SrcID bits that are overridden by the TTI action of SrcID
*       assignment.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       lookup          - the lookup number (APPLICABLE RANGES: 0..3)
*       overrideBitmap  - 12 bits bitmap. For each bit:
*                         0 - do not override
*                         1 - override
*                         (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiSourceIdBitsOverrideSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookup,
    IN GT_U32   overrideBitmap
)
{
    GT_U32      regAddr;
    GT_U32      maxNumOfLookups;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    maxNumOfLookups = PRV_CPSS_SIP_5_10_CHECK_MAC(devNum) ? 4 : 2;

    if(lookup > maxNumOfLookups-1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((CHECK_BITS_DATA_RANGE_MAC(overrideBitmap, 12)) == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = lookup < 2 ? PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).sourceIDAssignment0 :
                           PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).sourceIDAssignment1 ;

    return prvCpssHwPpSetRegField(devNum, regAddr, (lookup%2)*12, 12, overrideBitmap);
}

/*******************************************************************************
* cpssDxChTtiSourceIdBitsOverrideSet
*
* DESCRIPTION:
*       Set the SrcID bits that are overridden by the TTI action of SrcID
*       assignment.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       lookup          - the lookup number (APPLICABLE RANGES: 0..3)
*       overrideBitmap  - 12 bits bitmap. For each bit:
*                         0 - do not override
*                         1 - override
*                         (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiSourceIdBitsOverrideSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookup,
    IN GT_U32   overrideBitmap
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiSourceIdBitsOverrideSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lookup, overrideBitmap));

    rc = internal_cpssDxChTtiSourceIdBitsOverrideSet(devNum, lookup, overrideBitmap);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lookup, overrideBitmap));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiSourceIdBitsOverrideGet
*
* DESCRIPTION:
*       Get the SrcID bits that are overridden by the TTI action of SrcID
*       assignment.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       lookup              - the lookup number (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       overrideBitmapPtr   - 12 bits bitmap. For each bit:
*                             0 - do not override
*                             1 - override
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiSourceIdBitsOverrideGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  lookup,
    OUT GT_U32  *overrideBitmapPtr
)
{
    GT_U32      regAddr;
    GT_U32      maxNumOfLookups;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(overrideBitmapPtr);

    maxNumOfLookups = PRV_CPSS_SIP_5_10_CHECK_MAC(devNum) ? 4 : 2;

    if(lookup > maxNumOfLookups-1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regAddr = lookup < 2 ? PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).sourceIDAssignment0 :
                           PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).sourceIDAssignment1 ;

    return prvCpssHwPpGetRegField(devNum, regAddr, (lookup%2)*12, 12, overrideBitmapPtr);
}

/*******************************************************************************
* cpssDxChTtiSourceIdBitsOverrideGet
*
* DESCRIPTION:
*       Get the SrcID bits that are overridden by the TTI action of SrcID
*       assignment.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       lookup              - the lookup number (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       overrideBitmapPtr   - 12 bits bitmap. For each bit:
*                             0 - do not override
*                             1 - override
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiSourceIdBitsOverrideGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  lookup,
    OUT GT_U32  *overrideBitmapPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiSourceIdBitsOverrideGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, lookup, overrideBitmapPtr));

    rc = internal_cpssDxChTtiSourceIdBitsOverrideGet(devNum, lookup, overrideBitmapPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, lookup, overrideBitmapPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiGreExtensionsEnableSet
*
* DESCRIPTION:
*       Enable/Disable parsing of extensions (Checksum, Sequence, Key) on
*       IPv4/IPv6 GRE tunnels.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable/disable GRE extensions
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiGreExtensionsEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_U32      regAddr;
    GT_U32      hwData;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;

    hwData = ((enable == GT_FALSE) ? 1 : 0);

    return prvCpssHwPpSetRegField(devNum, regAddr, 25, 1, hwData);
}

/*******************************************************************************
* cpssDxChTtiGreExtensionsEnableSet
*
* DESCRIPTION:
*       Enable/Disable parsing of extensions (Checksum, Sequence, Key) on
*       IPv4/IPv6 GRE tunnels.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable/disable GRE extensions
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiGreExtensionsEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiGreExtensionsEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChTtiGreExtensionsEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiGreExtensionsEnableGet
*
* DESCRIPTION:
*       Gt the enabling status of the parsing of extensions (Checksum, Sequence,
*       Key) on IPv4/IPv6 GRE tunnels.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GRE extensions enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiGreExtensionsEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32      regAddr;
    GT_U32      hwData;
    GT_STATUS   rc;

    /* check parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).TTIUnitGlobalConfigs.TTIUnitGlobalConfigExt;

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 25, 1, &hwData);
    if (rc == GT_OK)
    {
        *enablePtr = ((hwData == 0) ? GT_TRUE : GT_FALSE);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChTtiGreExtensionsEnableGet
*
* DESCRIPTION:
*       Gt the enabling status of the parsing of extensions (Checksum, Sequence,
*       Key) on IPv4/IPv6 GRE tunnels.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GRE extensions enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiGreExtensionsEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiGreExtensionsEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChTtiGreExtensionsEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*       offsetType   - the type of offset
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type.(APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_TTI_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    return cpssDxChTtiPortGroupUserDefinedByteSet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        keyType, udbIndex, offsetType, offset);
}

/*******************************************************************************
* cpssDxChTtiUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*       offsetType   - the type of offset
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type.(APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_TTI_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiUserDefinedByteSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, udbIndex, offsetType, offset));

    rc = internal_cpssDxChTtiUserDefinedByteSet(devNum, keyType, udbIndex, offsetType, offset);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, udbIndex, offsetType, offset));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTtiUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTtiUserDefinedByteGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_U32                              udbIndex,
    OUT CPSS_DXCH_TTI_OFFSET_TYPE_ENT       *offsetTypePtr,
    OUT GT_U8                               *offsetPtr
)
{
    return cpssDxChTtiPortGroupUserDefinedByteGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        keyType, udbIndex, offsetTypePtr, offsetPtr);
}

/*******************************************************************************
* cpssDxChTtiUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTtiUserDefinedByteGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  GT_U32                              udbIndex,
    OUT CPSS_DXCH_TTI_OFFSET_TYPE_ENT       *offsetTypePtr,
    OUT GT_U8                               *offsetPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTtiUserDefinedByteGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, keyType, udbIndex, offsetTypePtr, offsetPtr));

    rc = internal_cpssDxChTtiUserDefinedByteGet(devNum, keyType, udbIndex, offsetTypePtr, offsetPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, keyType, udbIndex, offsetTypePtr, offsetPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


