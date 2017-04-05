/*************************************************************************
* wraplTgfTypeWrappers.c
*
* DESCRIPTION:
*       A lua type wrapper
*       It implements support for the following types:
*           enum    TGF_PACKET_TYPE_ENT
*           struct  TGF_PACKET_TRILL_GEN_OPTION_STC
*           enum    TGF_PROTOCOL_ENT
*           struct  TGF_PACKET_UDP_STC
*           struct  TGF_PACKET_MPLS_STC
*           struct  TGF_PACKET_PTP_V2_STC
*           struct  TGF_PACKET_ETHERTYPE_STC
*           struct  TGF_PACKET_TRILL_FIRST_OPTION_STC
*           struct  TGF_PACKET_ICMP_STC
*           struct  TGF_PACKET_VLAN_TAG_STC
*           enum    TGF_VFD_MODE_ENT
*           struct  TGF_PACKET_ARP_STC
*           struct  TGF_PACKET_TRILL_STC
*           struct  TGF_PACKET_L2_STC
*           struct  TGF_PACKET_ARPV6_STC
*           struct  TGF_PACKET_GRE_STC
*           enum    TGF_CAPTURE_MODE_ENT
*           enum    TGF_PACKET_PART_ENT
*           struct  TGF_PACKET_IPV6_STC
*           struct  TGF_PACKET_TCP_STC
*           struct  TGF_PACKET_IPV4_STC
*           enum    PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT
*
* DEPENDENCIES:
*
* COMMENTS:
*       Generated at Wed Apr 30 12:04:25 2014
*       ../../scripts/make_type_wrapper.py \
*               -i ../../data/type_info_file_dx \
*               -C /home/serg/cpss \
*               -p tgf_manual_types.lst \
*               -o wraplTgfTypeWrappers.c \
*               \
*               TGF_PACKET_PART_ENT PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT \
*               TGF_VFD_MODE_ENT TGF_PROTOCOL_ENT TGF_PACKET_L2_STC \
*               TGF_PACKET_VLAN_TAG_STC TGF_PACKET_ETHERTYPE_STC \
*               TGF_PACKET_MPLS_STC TGF_PACKET_TRILL_STC \
*               TGF_PACKET_TRILL_FIRST_OPTION_STC \
*               TGF_PACKET_TRILL_GEN_OPTION_STC TGF_PACKET_GRE_STC \
*               TGF_PACKET_IPV4_STC TGF_PACKET_IPV6_STC TGF_PACKET_TCP_STC \
*               TGF_PACKET_UDP_STC TGF_PACKET_ARP_STC TGF_PACKET_ARPV6_STC \
*               TGF_PACKET_ICMP_STC TGF_PACKET_PTP_V2_STC TGF_PACKET_TYPE_ENT \
*               TGF_CAPTURE_MODE_ENT
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
**************************************************************************/
#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <extUtils/trafficEngine/tgfTrafficGenerator.h>
#include <extUtils/trafficEngine/tgfTrafficEngine.h>

/***** declarations ********/

use_prv_enum(TGF_PACKET_TYPE_ENT);
use_prv_struct(TGF_PACKET_TRILL_GEN_OPTION_STC);
use_prv_enum(TGF_PROTOCOL_ENT);
use_prv_struct(TGF_PACKET_UDP_STC);
use_prv_struct(TGF_PACKET_MPLS_STC);
use_prv_struct(TGF_PACKET_PTP_V2_STC);
use_prv_array_type(TGF_IPV6_ADDR);
use_prv_struct(TGF_PACKET_ETHERTYPE_STC);
use_prv_struct(TGF_PACKET_TRILL_FIRST_OPTION_STC);
use_prv_struct(TGF_PACKET_ICMP_STC);
use_prv_array_type(TGF_IPV4_ADDR);
use_prv_array_type(TGF_MAC_ADDR);
use_prv_struct(TGF_PACKET_VLAN_TAG_STC);
use_prv_enum(TGF_VFD_MODE_ENT);
use_prv_struct(TGF_PACKET_ARP_STC);
use_prv_struct(TGF_PACKET_TRILL_STC);
use_prv_struct(TGF_PACKET_L2_STC);
use_prv_struct(TGF_PACKET_ARPV6_STC);
use_prv_struct(TGF_PACKET_GRE_STC);
use_prv_enum(TGF_CAPTURE_MODE_ENT);
use_prv_enum(TGF_PACKET_PART_ENT);
use_prv_struct(TGF_PACKET_IPV6_STC);
use_prv_struct(TGF_PACKET_TCP_STC);
use_prv_struct(TGF_PACKET_IPV4_STC);
use_prv_enum(PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT);

/***** declarations ********/

enumDescr enumDescr_TGF_PACKET_TYPE_ENT[] = {
    ENUM_ENTRY(TGF_PACKET_TYPE_REGULAR_E),
    ENUM_ENTRY(TGF_PACKET_TYPE_CAPTURE_E),
    ENUM_ENTRY(TGF_PACKET_TYPE_ANY_E),
    { 0, NULL }
};
add_mgm_enum(TGF_PACKET_TYPE_ENT);

void prv_lua_to_c_TGF_PACKET_TRILL_GEN_OPTION_STC(
    lua_State *L,
    TGF_PACKET_TRILL_GEN_OPTION_STC *val
)
{
    F_NUMBER(val, -1, info, GT_U32);
}

void prv_c_to_lua_TGF_PACKET_TRILL_GEN_OPTION_STC(
    lua_State *L,
    TGF_PACKET_TRILL_GEN_OPTION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, info, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_TRILL_GEN_OPTION_STC);

enumDescr enumDescr_TGF_PROTOCOL_ENT[] = {
    ENUM_ENTRY(TGF_PROTOCOL_HOPOPT_E),
    ENUM_ENTRY(TGF_PROTOCOL_ICMP_E),
    ENUM_ENTRY(TGF_PROTOCOL_IGMP_E),
    ENUM_ENTRY(TGF_PROTOCOL_IP_E),
    ENUM_ENTRY(TGF_PROTOCOL_TCP_E),
    ENUM_ENTRY(TGF_PROTOCOL_UDP_E),
    ENUM_ENTRY(TGF_PROTOCOL_ICMPV6_E),
    ENUM_ENTRY(TGF_PROTOCOL_MAX_E),
    { 0, NULL }
};
add_mgm_enum(TGF_PROTOCOL_ENT);

void prv_lua_to_c_TGF_PACKET_UDP_STC(
    lua_State *L,
    TGF_PACKET_UDP_STC *val
)
{
    F_NUMBER(val, -1, srcPort, TGF_L4_PORT);
    F_NUMBER(val, -1, dstPort, TGF_L4_PORT);
    F_NUMBER(val, -1, length, GT_U16);
    F_NUMBER(val, -1, csum, TGF_HEADER_CRC);
}

void prv_c_to_lua_TGF_PACKET_UDP_STC(
    lua_State *L,
    TGF_PACKET_UDP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, srcPort, TGF_L4_PORT);
    FO_NUMBER(val, t, dstPort, TGF_L4_PORT);
    FO_NUMBER(val, t, length, GT_U16);
    FO_NUMBER(val, t, csum, TGF_HEADER_CRC);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_UDP_STC);

void prv_lua_to_c_TGF_PACKET_MPLS_STC(
    lua_State *L,
    TGF_PACKET_MPLS_STC *val
)
{
    F_NUMBER(val, -1, label, TGF_MPLS_LBL);
    F_NUMBER(val, -1, exp, TGF_MPLS_EXP);
    F_NUMBER(val, -1, stack, TGF_MPLS_STACK);
    F_NUMBER(val, -1, timeToLive, TGF_TTL);
}

void prv_c_to_lua_TGF_PACKET_MPLS_STC(
    lua_State *L,
    TGF_PACKET_MPLS_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, label, TGF_MPLS_LBL);
    FO_NUMBER(val, t, exp, TGF_MPLS_EXP);
    FO_NUMBER(val, t, stack, TGF_MPLS_STACK);
    FO_NUMBER(val, t, timeToLive, TGF_TTL);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_MPLS_STC);

void prv_lua_to_c_TGF_PACKET_PTP_V2_STC(
    lua_State *L,
    TGF_PACKET_PTP_V2_STC *val
)
{
    F_NUMBER(val, -1, messageType, GT_U32);
    F_NUMBER(val, -1, transportSpecific, GT_U32);
    F_NUMBER(val, -1, ptpVersion, GT_U32);
    F_NUMBER(val, -1, reserved4, GT_U32);
    F_NUMBER(val, -1, messageLength, GT_U32);
    F_NUMBER(val, -1, domainNumber, GT_U32);
    F_NUMBER(val, -1, reserved8, GT_U32);
    F_NUMBER(val, -1, flagField, GT_U32);
    F_ARRAY_START(val, -1, correctionField);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            F_ARRAY_NUMBER(val, correctionField, idx, GT_U32);
        }
    }
    F_ARRAY_END(val, -1, correctionField);
    F_NUMBER(val, -1, reserved32, GT_U32);
    F_ARRAY_START(val, -1, sourcePortIdentify);
    {
        int idx;
        for (idx = 0; idx < 10; idx++) {
            F_ARRAY_NUMBER(val, sourcePortIdentify, idx, GT_U8);
        }
    }
    F_ARRAY_END(val, -1, sourcePortIdentify);
    F_NUMBER(val, -1, sequenceId, GT_U32);
    F_NUMBER(val, -1, controlField, GT_U32);
    F_NUMBER(val, -1, logMessageInterval, GT_U32);
}

void prv_c_to_lua_TGF_PACKET_PTP_V2_STC(
    lua_State *L,
    TGF_PACKET_PTP_V2_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, messageType, GT_U32);
    FO_NUMBER(val, t, transportSpecific, GT_U32);
    FO_NUMBER(val, t, ptpVersion, GT_U32);
    FO_NUMBER(val, t, reserved4, GT_U32);
    FO_NUMBER(val, t, messageLength, GT_U32);
    FO_NUMBER(val, t, domainNumber, GT_U32);
    FO_NUMBER(val, t, reserved8, GT_U32);
    FO_NUMBER(val, t, flagField, GT_U32);
    FO_ARRAY_START(val, t, correctionField);
    {
        int idx;
        for (idx = 0; idx < 2; idx++) {
            FO_ARRAY_NUMBER(val, correctionField, idx, GT_U32);
        }
    }
    FO_ARRAY_END(val, t, correctionField);
    FO_NUMBER(val, t, reserved32, GT_U32);
    FO_ARRAY_START(val, t, sourcePortIdentify);
    {
        int idx;
        for (idx = 0; idx < 10; idx++) {
            FO_ARRAY_NUMBER(val, sourcePortIdentify, idx, GT_U8);
        }
    }
    FO_ARRAY_END(val, t, sourcePortIdentify);
    FO_NUMBER(val, t, sequenceId, GT_U32);
    FO_NUMBER(val, t, controlField, GT_U32);
    FO_NUMBER(val, t, logMessageInterval, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_PTP_V2_STC);

void prv_lua_to_c_TGF_PACKET_ETHERTYPE_STC(
    lua_State *L,
    TGF_PACKET_ETHERTYPE_STC *val
)
{
    F_NUMBER(val, -1, etherType, TGF_ETHER_TYPE);
}

void prv_c_to_lua_TGF_PACKET_ETHERTYPE_STC(
    lua_State *L,
    TGF_PACKET_ETHERTYPE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, etherType, TGF_ETHER_TYPE);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_ETHERTYPE_STC);

void prv_lua_to_c_TGF_PACKET_TRILL_FIRST_OPTION_STC(
    lua_State *L,
    TGF_PACKET_TRILL_FIRST_OPTION_STC *val
)
{
    F_BOOL(val, -1, chbh);
    F_BOOL(val, -1, clte);
    F_NUMBER(val, -1, info, GT_U32);
}

void prv_c_to_lua_TGF_PACKET_TRILL_FIRST_OPTION_STC(
    lua_State *L,
    TGF_PACKET_TRILL_FIRST_OPTION_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, chbh);
    FO_BOOL(val, t, clte);
    FO_NUMBER(val, t, info, GT_U32);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_TRILL_FIRST_OPTION_STC);

void prv_lua_to_c_TGF_PACKET_ICMP_STC(
    lua_State *L,
    TGF_PACKET_ICMP_STC *val
)
{
    F_NUMBER(val, -1, typeOfService, TGF_TYPE_OF_SERVICE);
    F_NUMBER(val, -1, code, GT_U8);
    F_NUMBER(val, -1, csum, TGF_HEADER_CRC);
    F_NUMBER(val, -1, id, TGF_ICMP_ID_FIELD);
    F_NUMBER(val, -1, sequenceNum, GT_U16);
    F_NUMBER(val, -1, getwayAddr, GT_U32);
    F_NUMBER(val, -1, errorPointer, GT_U8);
}

void prv_c_to_lua_TGF_PACKET_ICMP_STC(
    lua_State *L,
    TGF_PACKET_ICMP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, typeOfService, TGF_TYPE_OF_SERVICE);
    FO_NUMBER(val, t, code, GT_U8);
    FO_NUMBER(val, t, csum, TGF_HEADER_CRC);
    FO_NUMBER(val, t, id, TGF_ICMP_ID_FIELD);
    FO_NUMBER(val, t, sequenceNum, GT_U16);
    FO_NUMBER(val, t, getwayAddr, GT_U32);
    FO_NUMBER(val, t, errorPointer, GT_U8);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_ICMP_STC);

void prv_lua_to_c_TGF_PACKET_VLAN_TAG_STC(
    lua_State *L,
    TGF_PACKET_VLAN_TAG_STC *val
)
{
    F_NUMBER(val, -1, etherType, TGF_ETHER_TYPE);
    F_NUMBER(val, -1, pri, TGF_PRI);
    F_NUMBER(val, -1, cfi, TGF_CFI);
    F_NUMBER(val, -1, vid, TGF_VLAN_ID);
}

void prv_c_to_lua_TGF_PACKET_VLAN_TAG_STC(
    lua_State *L,
    TGF_PACKET_VLAN_TAG_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, etherType, TGF_ETHER_TYPE);
    FO_NUMBER(val, t, pri, TGF_PRI);
    FO_NUMBER(val, t, cfi, TGF_CFI);
    FO_NUMBER(val, t, vid, TGF_VLAN_ID);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_VLAN_TAG_STC);

enumDescr enumDescr_TGF_VFD_MODE_ENT[] = {
    ENUM_ENTRY(TGF_VFD_MODE_OFF_E),
    ENUM_ENTRY(TGF_VFD_MODE_STATIC_E),
    ENUM_ENTRY(TGF_VFD_MODE_INCREMENT_E),
    ENUM_ENTRY(TGF_VFD_MODE_DECREMENT_E),
    ENUM_ENTRY(TGF_VFD_MODE_RANDOM_E),
    ENUM_ENTRY(TGF_VFD_MODE_ARRAY_PATTERN_E),
    ENUM_ENTRY(TGF_VFD_MODE_INCREMENT_VALUE_E),
    ENUM_ENTRY(TGF_VFD_MODE_DECREMENT_VALUE_E),
    ENUM_ENTRY(TGF_VFD_MODE_STATIC_EXPECTED_NO_MATCH_E),
    ENUM_ENTRY(TGF_VFD_MODE_STATIC_NO_MATCH_IGNORED_E),
    ENUM_ENTRY(TGF_VFD_MODE___LAST___E),
    { 0, NULL }
};
add_mgm_enum(TGF_VFD_MODE_ENT);

void prv_lua_to_c_TGF_PACKET_ARP_STC(
    lua_State *L,
    TGF_PACKET_ARP_STC *val
)
{
    F_NUMBER(val, -1, hwType, TGF_ARP_HW_TYPE);
    F_NUMBER(val, -1, protType, TGF_PROT_TYPE);
    F_NUMBER(val, -1, hwLen, GT_U8);
    F_NUMBER(val, -1, protLen, GT_U8);
    F_NUMBER(val, -1, opCode, GT_U16);
    F_ARRAY_TYPE_CUSTOM(val, -1, srcMac, TGF_MAC_ADDR);
    F_ARRAY_TYPE_CUSTOM(val, -1, srcIp, TGF_IPV4_ADDR);
    F_ARRAY_TYPE_CUSTOM(val, -1, dstMac, TGF_MAC_ADDR);
    F_ARRAY_TYPE_CUSTOM(val, -1, dstIp, TGF_IPV4_ADDR);
}

void prv_c_to_lua_TGF_PACKET_ARP_STC(
    lua_State *L,
    TGF_PACKET_ARP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, hwType, TGF_ARP_HW_TYPE);
    FO_NUMBER(val, t, protType, TGF_PROT_TYPE);
    FO_NUMBER(val, t, hwLen, GT_U8);
    FO_NUMBER(val, t, protLen, GT_U8);
    FO_NUMBER(val, t, opCode, GT_U16);
    FO_ARRAY_TYPE(val, t, srcMac, TGF_MAC_ADDR);
    FO_ARRAY_TYPE(val, t, srcIp, TGF_IPV4_ADDR);
    FO_ARRAY_TYPE(val, t, dstMac, TGF_MAC_ADDR);
    FO_ARRAY_TYPE(val, t, dstIp, TGF_IPV4_ADDR);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_ARP_STC);

void prv_lua_to_c_TGF_PACKET_TRILL_STC(
    lua_State *L,
    TGF_PACKET_TRILL_STC *val
)
{
    F_NUMBER(val, -1, version, TGF_PROT_VER);
    F_BOOL(val, -1, mBit);
    F_NUMBER(val, -1, opLength, GT_U32);
    F_NUMBER(val, -1, hopCount, GT_U32);
    F_NUMBER(val, -1, eRbid, GT_U16);
    F_NUMBER(val, -1, iRbid, GT_U16);
}

void prv_c_to_lua_TGF_PACKET_TRILL_STC(
    lua_State *L,
    TGF_PACKET_TRILL_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, version, TGF_PROT_VER);
    FO_BOOL(val, t, mBit);
    FO_NUMBER(val, t, opLength, GT_U32);
    FO_NUMBER(val, t, hopCount, GT_U32);
    FO_NUMBER(val, t, eRbid, GT_U16);
    FO_NUMBER(val, t, iRbid, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_TRILL_STC);

void prv_lua_to_c_TGF_PACKET_L2_STC(
    lua_State *L,
    TGF_PACKET_L2_STC *val
)
{
    F_ARRAY_TYPE_CUSTOM(val, -1, daMac, TGF_MAC_ADDR);
    F_ARRAY_TYPE_CUSTOM(val, -1, saMac, TGF_MAC_ADDR);
}

void prv_c_to_lua_TGF_PACKET_L2_STC(
    lua_State *L,
    TGF_PACKET_L2_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_TYPE(val, t, daMac, TGF_MAC_ADDR);
    FO_ARRAY_TYPE(val, t, saMac, TGF_MAC_ADDR);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_L2_STC);

void prv_lua_to_c_TGF_PACKET_ARPV6_STC(
    lua_State *L,
    TGF_PACKET_ARPV6_STC *val
)
{
    F_NUMBER(val, -1, hwType, TGF_ARP_HW_TYPE);
    F_NUMBER(val, -1, protType, TGF_PROT_TYPE);
    F_NUMBER(val, -1, hwLen, GT_U8);
    F_NUMBER(val, -1, protLen, GT_U8);
    F_NUMBER(val, -1, opCode, GT_U16);
    F_ARRAY_TYPE_CUSTOM(val, -1, srcMac, TGF_MAC_ADDR);
    F_ARRAY_TYPE_CUSTOM(val, -1, srcIp, TGF_IPV6_ADDR);
    F_ARRAY_TYPE_CUSTOM(val, -1, dstMac, TGF_MAC_ADDR);
    F_ARRAY_TYPE_CUSTOM(val, -1, dstIp, TGF_IPV6_ADDR);
}

void prv_c_to_lua_TGF_PACKET_ARPV6_STC(
    lua_State *L,
    TGF_PACKET_ARPV6_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, hwType, TGF_ARP_HW_TYPE);
    FO_NUMBER(val, t, protType, TGF_PROT_TYPE);
    FO_NUMBER(val, t, hwLen, GT_U8);
    FO_NUMBER(val, t, protLen, GT_U8);
    FO_NUMBER(val, t, opCode, GT_U16);
    FO_ARRAY_TYPE(val, t, srcMac, TGF_MAC_ADDR);
    FO_ARRAY_TYPE(val, t, srcIp, TGF_IPV6_ADDR);
    FO_ARRAY_TYPE(val, t, dstMac, TGF_MAC_ADDR);
    FO_ARRAY_TYPE(val, t, dstIp, TGF_IPV6_ADDR);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_ARPV6_STC);

void prv_lua_to_c_TGF_PACKET_GRE_STC(
    lua_State *L,
    TGF_PACKET_GRE_STC *val
)
{
    F_BOOL(val, -1, checkSumPresent);
    F_NUMBER(val, -1, reserved0, GT_U16);
    F_NUMBER(val, -1, version, TGF_PROT_VER);
    F_NUMBER(val, -1, protocol, GT_U16);
    F_NUMBER(val, -1, checksum, GT_U16);
    F_NUMBER(val, -1, reserved1, GT_U16);
}

void prv_c_to_lua_TGF_PACKET_GRE_STC(
    lua_State *L,
    TGF_PACKET_GRE_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_BOOL(val, t, checkSumPresent);
    FO_NUMBER(val, t, reserved0, GT_U16);
    FO_NUMBER(val, t, version, TGF_PROT_VER);
    FO_NUMBER(val, t, protocol, GT_U16);
    FO_NUMBER(val, t, checksum, GT_U16);
    FO_NUMBER(val, t, reserved1, GT_U16);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_GRE_STC);

enumDescr enumDescr_TGF_CAPTURE_MODE_ENT[] = {
    ENUM_ENTRY(TGF_CAPTURE_MODE_PCL_E),
    ENUM_ENTRY(TGF_CAPTURE_MODE_MIRRORING_E),
    { 0, NULL }
};
add_mgm_enum(TGF_CAPTURE_MODE_ENT);

enumDescr enumDescr_TGF_PACKET_PART_ENT[] = {
    ENUM_ENTRY(TGF_PACKET_PART_L2_E),
    ENUM_ENTRY(TGF_PACKET_PART_VLAN_TAG_E),
    ENUM_ENTRY(TGF_PACKET_PART_DSA_TAG_E),
    ENUM_ENTRY(TGF_PACKET_PART_ETHERTYPE_E),
    ENUM_ENTRY(TGF_PACKET_PART_MPLS_E),
    ENUM_ENTRY(TGF_PACKET_PART_IPV4_E),
    ENUM_ENTRY(TGF_PACKET_PART_IPV6_E),
    ENUM_ENTRY(TGF_PACKET_PART_TCP_E),
    ENUM_ENTRY(TGF_PACKET_PART_UDP_E),
    ENUM_ENTRY(TGF_PACKET_PART_ARP_E),
    ENUM_ENTRY(TGF_PACKET_PART_ICMP_E),
    ENUM_ENTRY(TGF_PACKET_PART_WILDCARD_E),
    ENUM_ENTRY(TGF_PACKET_PART_PAYLOAD_E),
    ENUM_ENTRY(TGF_PACKET_PART_TRILL_E),
    ENUM_ENTRY(TGF_PACKET_PART_TRILL_FIRST_OPTION_E),
    ENUM_ENTRY(TGF_PACKET_PART_TRILL_GENERAL_OPTION_E),
    ENUM_ENTRY(TGF_PACKET_PART_GRE_E),
    ENUM_ENTRY(TGF_PACKET_PART_TUNNEL_HEADER_FORCE_TILL_PASSENGER_E),
    ENUM_ENTRY(TGF_PACKET_PART_CRC_E),
    ENUM_ENTRY(TGF_PACKET_PART_PTP_V2_E),
    ENUM_ENTRY(TGF_PACKET_PART_TEMPLATE_E),
    ENUM_ENTRY(TGF_PACKET_PART_SKIP_E),
    ENUM_ENTRY(TGF_PACKET_PART_MAX_E),
    { 0, NULL }
};
add_mgm_enum(TGF_PACKET_PART_ENT);

void prv_lua_to_c_TGF_PACKET_IPV6_STC(
    lua_State *L,
    TGF_PACKET_IPV6_STC *val
)
{
    F_NUMBER(val, -1, version, TGF_PROT_VER);
    F_NUMBER(val, -1, trafficClass, TGF_TRAFFIC_CLASS);
    F_NUMBER(val, -1, flowLabel, TGF_FLOW_LABEL);
    F_NUMBER(val, -1, payloadLen, GT_U16);
    F_NUMBER(val, -1, nextHeader, TGF_NEXT_HEADER);
    F_NUMBER(val, -1, hopLimit, TGF_HOP_LIMIT);
    F_ARRAY_TYPE_CUSTOM(val, -1, srcAddr, TGF_IPV6_ADDR);
    F_ARRAY_TYPE_CUSTOM(val, -1, dstAddr, TGF_IPV6_ADDR);
}

void prv_c_to_lua_TGF_PACKET_IPV6_STC(
    lua_State *L,
    TGF_PACKET_IPV6_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, version, TGF_PROT_VER);
    FO_NUMBER(val, t, trafficClass, TGF_TRAFFIC_CLASS);
    FO_NUMBER(val, t, flowLabel, TGF_FLOW_LABEL);
    FO_NUMBER(val, t, payloadLen, GT_U16);
    FO_NUMBER(val, t, nextHeader, TGF_NEXT_HEADER);
    FO_NUMBER(val, t, hopLimit, TGF_HOP_LIMIT);
    FO_ARRAY_TYPE(val, t, srcAddr, TGF_IPV6_ADDR);
    FO_ARRAY_TYPE(val, t, dstAddr, TGF_IPV6_ADDR);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_IPV6_STC);

void prv_lua_to_c_TGF_PACKET_TCP_STC(
    lua_State *L,
    TGF_PACKET_TCP_STC *val
)
{
    F_NUMBER(val, -1, srcPort, TGF_L4_PORT);
    F_NUMBER(val, -1, dstPort, TGF_L4_PORT);
    F_NUMBER(val, -1, sequenceNum, GT_U32);
    F_NUMBER(val, -1, acknowledgeNum, GT_U32);
    F_NUMBER(val, -1, dataOffset, GT_U8);
    F_NUMBER(val, -1, reserved, GT_U8);
    F_NUMBER(val, -1, flags, TGF_TCP_FLAGS);
    F_NUMBER(val, -1, windowSize, GT_U16);
    F_NUMBER(val, -1, csum, TGF_HEADER_CRC);
    F_NUMBER(val, -1, urgentPtr, TGF_TCP_URGENT_PTR);
}

void prv_c_to_lua_TGF_PACKET_TCP_STC(
    lua_State *L,
    TGF_PACKET_TCP_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, srcPort, TGF_L4_PORT);
    FO_NUMBER(val, t, dstPort, TGF_L4_PORT);
    FO_NUMBER(val, t, sequenceNum, GT_U32);
    FO_NUMBER(val, t, acknowledgeNum, GT_U32);
    FO_NUMBER(val, t, dataOffset, GT_U8);
    FO_NUMBER(val, t, reserved, GT_U8);
    FO_NUMBER(val, t, flags, TGF_TCP_FLAGS);
    FO_NUMBER(val, t, windowSize, GT_U16);
    FO_NUMBER(val, t, csum, TGF_HEADER_CRC);
    FO_NUMBER(val, t, urgentPtr, TGF_TCP_URGENT_PTR);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_TCP_STC);

void prv_lua_to_c_TGF_PACKET_IPV4_STC(
    lua_State *L,
    TGF_PACKET_IPV4_STC *val
)
{
    F_NUMBER(val, -1, version, TGF_PROT_VER);
    F_NUMBER(val, -1, headerLen, GT_U8);
    F_NUMBER(val, -1, typeOfService, TGF_TYPE_OF_SERVICE);
    F_NUMBER(val, -1, totalLen, GT_U16);
    F_NUMBER(val, -1, id, TGF_IPV4_ID_FIELD);
    F_NUMBER(val, -1, flags, TGF_FLAG);
    F_NUMBER(val, -1, offset, GT_U16);
    F_NUMBER(val, -1, timeToLive, TGF_TTL);
    F_NUMBER(val, -1, protocol, TGF_PROT);
    F_NUMBER(val, -1, csum, TGF_HEADER_CRC);
    F_ARRAY_TYPE_CUSTOM(val, -1, srcAddr, TGF_IPV4_ADDR);
    F_ARRAY_TYPE_CUSTOM(val, -1, dstAddr, TGF_IPV4_ADDR);
}

void prv_c_to_lua_TGF_PACKET_IPV4_STC(
    lua_State *L,
    TGF_PACKET_IPV4_STC *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_NUMBER(val, t, version, TGF_PROT_VER);
    FO_NUMBER(val, t, headerLen, GT_U8);
    FO_NUMBER(val, t, typeOfService, TGF_TYPE_OF_SERVICE);
    FO_NUMBER(val, t, totalLen, GT_U16);
    FO_NUMBER(val, t, id, TGF_IPV4_ID_FIELD);
    FO_NUMBER(val, t, flags, TGF_FLAG);
    FO_NUMBER(val, t, offset, GT_U16);
    FO_NUMBER(val, t, timeToLive, TGF_TTL);
    FO_NUMBER(val, t, protocol, TGF_PROT);
    FO_NUMBER(val, t, csum, TGF_HEADER_CRC);
    FO_ARRAY_TYPE(val, t, srcAddr, TGF_IPV4_ADDR);
    FO_ARRAY_TYPE(val, t, dstAddr, TGF_IPV4_ADDR);
    lua_settop(L, t);
}
add_mgm_type(TGF_PACKET_IPV4_STC);

enumDescr enumDescr_PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT[] = {
    ENUM_ENTRY(PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E),
    ENUM_ENTRY(PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E),
    ENUM_ENTRY(PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E),
    ENUM_ENTRY(PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E),
    ENUM_ENTRY(PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E),
    ENUM_ENTRY(PRV_TGF_BRG_VLAN_PORT_PUSH_TAG0_CMD_E),
    ENUM_ENTRY(PRV_TGF_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E),
    ENUM_ENTRY(PRV_TGF_BRG_VLAN_PORT_DO_NOT_MODIFY_TAG_CMD_E),
    { 0, NULL }
};
add_mgm_enum(PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT);

