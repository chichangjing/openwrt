/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTrafficEngine.c
*
* DESCRIPTION:
*    -- packet generator , parser.
*    -- with trace capabilities
*    -- each packet can be built from :
*        -- L2 - mac da,sa
*        -- vlan tag (optional) -- single/double tag(Q-in-Q)
*        -- Ethernet encapsulation (optional) -- LLC/SNAP/etherII/Novell
*        -- ether type
*        -- L3 (optional) - ipv4/6 / mpls / arp / ..
*        -- L4 (optional) - tcp / udp / icmp / IGMPv2,3,6 / MLD / RIPv1,2 ..
*        -- payload
*        -- CRC32
*    -- support tunneling (tunnel + passenger)
*        -- each passenger is like a new packet
*    -- each packet has - total length
*    -- other specific info:
*        -- table index , num packets , waitTime
*        -- Tx parameters (CPSS_DXCH_NET_TX_PARAMS_STC):
*            - DSA tag info , txSynch , invokeTxBufferQueueEvent , txQueue ...
*        -- for Rx traffic : CPU code (for 'to_cpu')
*            -- the engine need CB function to get the CPU code of this frame.
*
*    -- special CPU codes:
*    list of CPU codes that can be attached to CB for advanced purposes:
*        distinguish between the 'rx in cpu'(any CPU code) and the 'Tx port capture' (tx_analyzer cpu code)
*
*-- basic init test capabilities:
*    -- set port in loopback mode
*    -- set port with force link up
*-- emulate SMB port counters:
*    -- read/clear port counter : all/rx/tx all/packets/bytes
*    -- 'capture' - set port as 'Egress mirror port' / use STC Sampling to cpu
*                 - set the CPU as the 'Tx analyzer port'
*                 - CPU will get those frames with specific CPU code.
*
*-- emulate SMB send to port:
*    -- fill table of frames to 'Ingress the device' from the port
*       (port should be in: loopback mode and in link up)
*    -- send all / specific table index
*    -- done in specific task.
*        -- this also allow use to break the sending if we want from terminal
*        -- set FLAG 'Break Sending' from terminal so task will stop sending
*
*-- support for multi devices types (Dx/ExMxPm/ExMx):
*    -- the engine of build/parse should not care about it.
*    -- the send/receive of packet (a 'table')in CPU will call CB with specific
*        Tx parameters (CPSS_DXCH_NET_TX_PARAMS_STC) /
*        Rx parameter (CPSS_DXCH_NET_RX_PARAMS_STC)
*        and attach those parameter to the frame -->
*            each entry in table point to ' cookie + CB' that hold the specific Rx/Tx info
*
*    -- SMP port emulation:
*        -- read/clear counters - attach CB for read counters (per device , per port)
*        -- 'capture' - attach CB for set CPU as analyzer , CB for set port as tx mirrored port
*            CB for set Tx port with Sampling to CPU
*
*-- NOTE : all traffic that received in the CPU saved in the same 'table'
*          'test' can distinguish between : rx and ' capture of tx' by the
*          mechanism of : 'Special CPU codes'
*
*   -SMB trigger emulation - TBD
*   -Expected results  - TBD
*   -Predefined traffic flows  - TBD
*
*
* FILE REVISION NUMBER:
*       $Revision: 28 $
*
*******************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/cpssFormatConvert.h>
#include <extUtils/trafficEngine/tgfTrafficEngine.h>
#include <extUtils/trafficEngine/tgfTrafficGenerator.h>
#include <extUtils/trafficEngine/private/prvTgfTrafficParser.h>
#include <extUtils/trafficEngine/prvTgfLog.h>


/******************************************************************************\
 *                              Define section                                *
\******************************************************************************/

/* flag that state we need to trace field */
#define TGF_TRACE_FIELD_FLAG_CNS        BIT_0

/* flag that state we need to trace bytes */
#define TGF_TRACE_BYTES_FLAG_CNS        BIT_1

/* max number of fields supported for trace */
#define TGF_TRACE_MAX_FIELDS_NUM_CNS    20

/* offset of the member of structure or union from the origin in bytes */
#define TGF_OFFSET_IN_STC_MAC(stcType, field)                                  \
    CPSS_FORMAT_CONVERT_STRUCT_MEMBER_OFFSET_MAC(stcType, field)

/* macro to trace mac address */
#define TGF_TRACE_MAC_ADDR_MAC(name, value)                                    \
    PRV_TGF_LOG7_MAC("%s [%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x]\n", #name,      \
                     value[0],value[1],value[2],value[3],value[4],value[5])

/* macro to trace IPv4 */
#define TGF_TRACE_IPV4_ADDR_MAC(name, value)                                   \
    PRV_TGF_LOG5_MAC("%s [%d:%d:%d:%d]\n", #name, value[0], value[1],          \
                     value[2],value[3])

/* macro to trace IPv6 */
#define TGF_TRACE_IPV6_ADDR_MAC(name, value)                                   \
    PRV_TGF_LOG9_MAC("%s [%d:%d:%d:%d:%d:%d:%d:%d]\n", #name,value[0],value[1],\
                     value[2],value[3],value[4],value[5],value[6],value[7])

/* macro to trace numeric field in decimal way */
#define TGF_TRACE_NUMERIC_DEC_MAC(name, value)                                 \
    PRV_TGF_LOG2_MAC("%s [%ld]\n", #name, (GT_U32) value)

/* macro to trace numeric field in HEX way */
#define TGF_TRACE_NUMERIC_HEX_MAC(name, value)                                 \
    PRV_TGF_LOG2_MAC("%s [0x%lx]\n", #name, (GT_U32) value)

/* macro to set numeric field's info in trace DB */
#define TGF_TRACE_DB_FIELDS_VAL_SET_MAC(index, fieldNum, field, value)         \
    prvTgfTraceArr[index].fieldsArr[fieldNum].field = value

/* macro to set literal field's info in trace DB */
#define TGF_TRACE_DB_FIELDS_NAME_SET_MAC(index, fieldNum, name)                \
    prvTgfTraceArr[index].fieldsArr[fieldNum].nameString = #name

/* macro to set field's info in trace DB */
#define TGF_TRACE_DB_FIELDS_MAC(stcType,field,offset,fldsNum,index,traceType)  \
    offset = TGF_OFFSET_IN_STC_MAC  (stcType, field);                          \
    TGF_TRACE_DB_FIELDS_VAL_SET_MAC (index, fldsNum, offsetInStc, offset);     \
    TGF_TRACE_DB_FIELDS_VAL_SET_MAC (index, fldsNum, doTrace, 0);              \
    TGF_TRACE_DB_FIELDS_NAME_SET_MAC(index, fldsNum, field);                   \
    TGF_TRACE_DB_FIELDS_VAL_SET_MAC (index, fldsNum++, fieldType, traceType)


/******************************************************************************\
 *                            Private types section                           *
\******************************************************************************/

/* enumeration that hold the types of fields */
typedef enum
{
    TGF_TRACE_NUMERIC_DEC_E,
    TGF_TRACE_NUMERIC_HEX_E,
    TGF_TRACE_IPV4_ADDR_E,
    TGF_TRACE_IPV6_ADDR_E,
    TGF_TRACE_MAC_ADDR_E,

    TGF_TRACE_TYPE_MAX_E
} TGF_TRACE_FIELD_TYPE_ENT;

/* struct that holds field trace info */
typedef struct
{
    GT_U32                   offsetInStc;
    GT_U8                    doTrace;
    GT_CHAR*                 nameString;
    TGF_TRACE_FIELD_TYPE_ENT fieldType;
} TGF_TRACE_FIELD_INFO_STC;

/* DB of fields info about their trace */
typedef struct
{
    TGF_TRACE_FIELD_INFO_STC     fieldsArr[TGF_TRACE_MAX_FIELDS_NUM_CNS];
    GT_U32                       fieldsArrLength;
} TGF_TRACE_FIELD_INFO_DB_STC;

/* global trace controller bitmap */
static GT_U32 prvTgfTraceGlobalBmp = TGF_TRACE_FIELD_FLAG_CNS;

/* DB of fields info about their trace */
static TGF_TRACE_FIELD_INFO_DB_STC prvTgfTraceArr[TGF_PACKET_PART_MAX_E] =
        {{{{0}}, 0}};

/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

/*******************************************************************************
* prvTgfTrafficTraceOutput
*
* DESCRIPTION:
*       Trace packet to output
*
* INPUTS:
*       packetPartPtr - (pointer to) packet part to trace
*
* OUTPUTS:
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficTraceOutput
(
    IN GT_VOID               *packetPartPtr,
    IN TGF_PACKET_PART_ENT   type

)
{
    GT_U32  arrIndex = 0;
    GT_U32  arrIter  = 0;
    GT_U32  offset   = 0;


    CPSS_NULL_PTR_CHECK_MAC(packetPartPtr);

    switch (type)
    {
        case TGF_PACKET_PART_L2_E:
        case TGF_PACKET_PART_VLAN_TAG_E:
        case TGF_PACKET_PART_DSA_TAG_E:
        case TGF_PACKET_PART_ETHERTYPE_E:
        case TGF_PACKET_PART_MPLS_E:
        case TGF_PACKET_PART_IPV4_E:
        case TGF_PACKET_PART_IPV6_E:
        case TGF_PACKET_PART_TCP_E:
        case TGF_PACKET_PART_UDP_E:
        case TGF_PACKET_PART_ARP_E:
        case TGF_PACKET_PART_ICMP_E:
        case TGF_PACKET_PART_WILDCARD_E:
        case TGF_PACKET_PART_PAYLOAD_E:
        case TGF_PACKET_PART_TUNNEL_HEADER_FORCE_TILL_PASSENGER_E:
        case TGF_PACKET_PART_GRE_E:
        case TGF_PACKET_PART_CRC_E:
        case TGF_PACKET_PART_PTP_V2_E:
        case TGF_PACKET_PART_TRILL_E:
        case TGF_PACKET_PART_TRILL_FIRST_OPTION_E:
        case TGF_PACKET_PART_TRILL_GENERAL_OPTION_E:
            arrIndex = (GT_U32) type;
            break;
        case TGF_PACKET_PART_SKIP_E:
            return GT_OK;/* skip this part */
        default:
            return GT_BAD_PARAM;
    }

    for (arrIter = 0; arrIter < prvTgfTraceArr[arrIndex].fieldsArrLength; arrIndex++)
    {
        if (prvTgfTraceArr[arrIndex].fieldsArr[arrIter].doTrace)
        {
            offset = prvTgfTraceArr[arrIndex].fieldsArr[arrIter].offsetInStc;

            switch (prvTgfTraceArr[arrIndex].fieldsArr[arrIter].fieldType)
            {
                case TGF_TRACE_MAC_ADDR_E:
                    TGF_TRACE_MAC_ADDR_MAC(prvTgfTraceArr[arrIndex].fieldsArr[arrIter].nameString,
                                           *((TGF_MAC_ADDR*)packetPartPtr + offset));
                    break;

                case TGF_TRACE_IPV4_ADDR_E:
                    TGF_TRACE_IPV4_ADDR_MAC(prvTgfTraceArr[arrIndex].fieldsArr[arrIter].nameString,
                                           *((TGF_IPV4_ADDR*) packetPartPtr + offset));
                    break;

                case TGF_TRACE_IPV6_ADDR_E:
                    TGF_TRACE_IPV6_ADDR_MAC(prvTgfTraceArr[arrIndex].fieldsArr[arrIter].nameString,
                                            *((TGF_IPV6_ADDR*) packetPartPtr + offset));
                    break;

                case TGF_TRACE_NUMERIC_DEC_E:
                    TGF_TRACE_NUMERIC_DEC_MAC(prvTgfTraceArr[arrIndex].fieldsArr[arrIter].nameString,
                                              *((GT_U32*) packetPartPtr + offset));
                    break;

                case TGF_TRACE_NUMERIC_HEX_E:
                    TGF_TRACE_NUMERIC_HEX_MAC(prvTgfTraceArr[arrIndex].fieldsArr[arrIter].nameString,
                                              *((GT_U32*) packetPartPtr + offset));
                    break;

                default:
                    return GT_BAD_PARAM;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfMemCopy
*
* DESCRIPTION:
*       Copy part of packet from source to destination
*
* INPUTS:
*       dstPtr       - destination of copy
*       srcPtr       - source of copy
*       size         - size of memory to copy
*       startBytePtr - (pointer to) the first byte in dstPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in dstPtr for the
*                       next builder to write to
*       dstPtr       - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfMemCopy
(
    OUT      GT_U8           *dstPtr,
    IN const GT_U8           *srcPtr,
    IN       GT_U32           size,
    INOUT    GT_U32          *startBytePtr,
    IN       GT_U32           endByte
)
{
    GT_U32  index = 0;

    if(endByte == 0)
    {
        /* the function was called only to calculate the length of the field ,
           ignore endByte */
        (*startBytePtr) += size;
        return;
    }

    for (index = 0; index < size && (*startBytePtr) <= endByte; index++)
    {
        dstPtr[(*startBytePtr)] = srcPtr[index];
        (*startBytePtr)++;
    }

    return;
}

/*******************************************************************************
* prvTgfBuildL2Part
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte index in bufferPtr for the
*                      next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*         0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*       |            DA         |           SA          |EthType| DATA...
*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---
*
*******************************************************************************/
static GT_STATUS prvTgfBuildL2Part
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_L2_STC* l2Ptr = partPtr;
    GT_STATUS          rc    = GT_OK;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    rc = prvTgfTrafficTraceOutput(l2Ptr, TGF_PACKET_PART_L2_E);

    prvTgfMemCopy(bufferPtr,
                  &l2Ptr->daMac[0],
                  sizeof(l2Ptr->daMac),
                  startBytePtr,
                  endByte);

    prvTgfMemCopy(bufferPtr,
                  &l2Ptr->saMac[0],
                  sizeof(l2Ptr->saMac),
                  startBytePtr,
                  endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildEtherTypePart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr        - part fields info
*       startBytePtr   - (pointer to) the first byte in bufferPtr to write to
*       endByte        - the max index allowed to be modified in bufferPtr
*       calledFromVlan - called from vlan
* OUTPUTS:
*       startBytePtr   - (pointer to) the first byte in bufferPtr for the
*                         next builder to write to
*       bufferPtr      - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       Ethertype         Exp. Ethernet   Description           References
*       -------------     -------------   -----------           ----------
*       decimal  Hex      decimal  octal
*        2048   0800        513   1001   Internet IP (IPv4)        [IANA]
*        2054   0806        -      -     ARP                       [IANA]
*       33024   8100        -      -     IEEE 802.1Q VLAN-tagged frames
*       34525   86DD        -      -     IPv6                      [IANA]
*       34887   8847        -      -     MPLS Unicast              [Rosen]
*       34888   8848        -      -     MPLS Multicast            [Rosen]
*
*         0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*       |            DA         |           SA          |EthType| DATA...
*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---
*
*******************************************************************************/
static GT_STATUS prvTgfBuildEtherTypePart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr,
    IN    GT_BOOL       calledFromVlan
)
{
    TGF_PACKET_ETHERTYPE_STC* etherTypePtr = partPtr;
    GT_STATUS                 rc           = GT_OK;
    GT_U8   etherType[2];

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    etherType[1] = (GT_U8)(etherTypePtr->etherType & 0xFF) ;
    etherType[0] = (GT_U8)((etherTypePtr->etherType >> 8) & 0xFF);

    if(GT_FALSE == calledFromVlan)
    {
        rc = prvTgfTrafficTraceOutput(etherTypePtr, TGF_PACKET_PART_ETHERTYPE_E);
    }

    prvTgfMemCopy(bufferPtr,
                  &etherType[0],
                  sizeof(etherType),
                  startBytePtr,
                  endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildVlanTagPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       TPID - defined value of 8100 in hex. When a frame has the EtherType
*           equal to 8100, this frame carries the tag IEEE 802.1Q / 802.1P.
*       TCI - Tag Control Information field including user priority,
*           Canonical format indicator and VLAN ID.
*           User Priority - Defines user priority, giving eight (2^3)
*           priority levels. IEEE 802.1P defines the operation
*           for these 3 user priority bits.
*       CFI - Canonical Format Indicator is always set to zero
*           for Ethernet switches. CFI is used for compatibility reason
*           between Ethernet type network and Token Ring type network.
*           If a frame received at an Ethernet port has a CFI set to 1, then
*           that frame should not be forwarded as it is to an untagged port.
*       VID - VLAN ID is the identification of the VLAN, which is basically
*           used by the standard 802.1Q. It has 12 bits and allow
*           the identification of 4096 (2^12) VLANs. Of the 4096 possible VIDs,
*           a VID of 0 is used to identify priority frames and value 4095 (FFF)
*           is reserved, so the maximum possible VLAN configurations are 4,094.
*
*         0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*       |            DA         |           SA          |   VLAN TAG    |
*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*       |EthType|    DATA for EthType...
*       +---+---+---+---+---+---+---+---+---
*
* VLAN TAG (bytes 12,13,14,15) 32 bits:
*         0                   1                   2                   3
*        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |           TPID = 8100         | TCI |C|         VID           |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*                        16 bit          3 bit 1 bit     12 bit
*
*******************************************************************************/
static GT_STATUS prvTgfBuildVlanTagPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U8     vtag[2];
    TGF_PACKET_VLAN_TAG_STC* vlanTagPtr = partPtr;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    /* the vlan tag start with ethertype field */
    rc = prvTgfBuildEtherTypePart(partPtr,
                                  startBytePtr,
                                  endByte,
                                  bufferPtr,
                                  GT_TRUE);
    if (GT_OK != rc)
    {
        return rc;
    }

    vtag[1] = (GT_U8)(vlanTagPtr->vid & 0xFF) ;
    vtag[0] = (GT_U8)(((vlanTagPtr->vid >> 8) & 0x0F)  |
                      ((vlanTagPtr->cfi & 1) << 4)     |
                      ((vlanTagPtr->pri & 7) << 5));

    rc = prvTgfTrafficTraceOutput(vlanTagPtr, TGF_PACKET_PART_VLAN_TAG_E);

    prvTgfMemCopy(bufferPtr, &vtag[0], sizeof(vtag),
                  startBytePtr, endByte);

    return rc;
}

static void doError(IN GT_U32 e , IN GT_U32 r , IN GT_CHAR* errStr)
{
    TGF_VERIFY_EQUAL0_STRING_MAC(e, r, errStr);
}
/*******************************************************************************
* prvTgfBuildDsaTagPart
*
* DESCRIPTION:
*       Build DSA tag part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfBuildDsaTagPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    GT_STATUS rc;
    static GT_U8     dsaTag[TGF_DSA_LAST_TYPE_E * 4];
    TGF_PACKET_DSA_TAG_STC* dsaTagPtr = partPtr;
    GT_U32  actualNumBytes;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    /* the actual num of bytes for the DSA tag */
    actualNumBytes = TGF_DSA_NUM_BYTES_MAC(dsaTagPtr);

    /* build the DSA tag bytes into dsaTag[] */
    rc = prvTgfTrafficGeneratorPacketDsaTagBuild(dsaTagPtr,&dsaTag[0]);
    if(rc != GT_OK)
    {
        doError(GT_OK, rc, "prvTgfTrafficGeneratorPacketDsaTagBuild: build DSA tag FAILED with rc != GT_OK");
        return rc;
    }

    /* copy bytes from dsaTag[] to bufferPtr ... make sure not to copy more then allowed */
    prvTgfMemCopy(bufferPtr, &dsaTag[0], actualNumBytes,
                  startBytePtr, endByte);

    return GT_OK;
}


/*******************************************************************************
* prvTgfBuildMplsPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                         next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       Label: Label Value, 20 bits
*       Exp:   Experimental Use, 3 bits
*       S:     Bottom of Stack, 1 bit
*       TTL:   Time to Live, 8 bits
*
*        0                   1                   2                   3
*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ Label
*      |                Label                  | Exp |S|       TTL     | Stack
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ Entry
*
*******************************************************************************/
static GT_STATUS prvTgfBuildMplsPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_MPLS_STC* mplsPtr = partPtr;
    GT_STATUS            rc      = GT_OK;
    GT_U8                mplsHeader[4];

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    mplsHeader[0] = (GT_U8)((mplsPtr->label >> 12) & 0xFF);
    mplsHeader[1] = (GT_U8)((mplsPtr->label >> 4) & 0xFF);
    mplsHeader[2] = (GT_U8)(((mplsPtr->label << 4) & 0xF0) |
                            ((mplsPtr->exp << 1) & 0x0E)   |
                            (mplsPtr->stack & 1));
    mplsHeader[3] = (GT_U8)(mplsPtr->timeToLive & 0xFF);

    rc = prvTgfTrafficTraceOutput(mplsPtr, TGF_PACKET_PART_MPLS_E);

    prvTgfMemCopy(bufferPtr, &mplsHeader[0], sizeof(mplsHeader),
                  startBytePtr, endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildIpv4Part
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                         next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*       checksumInfoArr - (array of) info about instance of fields that need auto checksum build
*       checksumExtraInfoPtr -  (array of) extra info for auto checksum build
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       0                   1                   2                   3
*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |Version|  IHL  |Type of Service|          Total Length         |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |         Identification        |Flags|      Fragment Offset    |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |  Time to Live |    Protocol   |         Header Checksum       |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |                       Source Address                          |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |                    Destination Address                        |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |                    Options                    |    Padding    |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*******************************************************************************/
static GT_STATUS prvTgfBuildIpv4Part
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr,
    OUT TGF_AUTO_CHECKSUM_FULL_INFO_STC *checksumInfoPtr,
    OUT TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
)
{
    TGF_PACKET_IPV4_STC* ipv4Ptr = partPtr;
    GT_STATUS            rc      = GT_OK;
    GT_U8                ipv4Header[12];
    GT_U32               ii = 0;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    if(checksumInfoPtr)
    {
        for(ii = 0 ;ii < TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS ; ii++)
        {
            if(checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_IPV4_E].startPtr)
            {
                /* this instance already used */
                continue;
            }

            /* indicate that the index is used , to keep synch between the ipv4 header and the TCP/UDP header (for pseudo header calc) */
            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_IPV4_E].startPtr = &bufferPtr[*startBytePtr];

            if(ipv4Ptr->csum != TGF_PACKET_AUTO_CALC_CHECKSUM_CNS)
            {
                /* no more to do */
                break;
            }

            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_IPV4_E].enable = GT_TRUE;
            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_IPV4_E].numBytes = ipv4Ptr->headerLen * 4;
            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_IPV4_E].checkSumPtr = &bufferPtr[*startBytePtr + 10];
            break;
        }

        if(ii == TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS)
        {
            /* need to enlarge the constant of TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS */
            return GT_OUT_OF_RANGE;
        }
    }

    if(checksumExtraInfoPtr)
    {
        checksumExtraInfoPtr[ii].isIpv4 = GT_TRUE;
        checksumExtraInfoPtr[ii].startL3HeadrPtr = &bufferPtr[*startBytePtr];
    }

    ipv4Header[11] = (GT_U8)(ipv4Ptr->csum & 0xFF);
    ipv4Header[10] = (GT_U8)((ipv4Ptr->csum >> 8) & 0xFF);
    ipv4Header[9] = (GT_U8)(ipv4Ptr->protocol & 0xFF);
    ipv4Header[8] = (GT_U8)(ipv4Ptr->timeToLive & 0xFF);
    ipv4Header[7] = (GT_U8)(ipv4Ptr->offset & 0xFF);
    ipv4Header[6] = (GT_U8)(((ipv4Ptr->offset >> 8) & 0x1F) |
                            (ipv4Ptr->flags << 5));
    ipv4Header[5] = (GT_U8)(ipv4Ptr->id & 0xFF);
    ipv4Header[4] = (GT_U8)((ipv4Ptr->id >> 8) & 0xFF);
    ipv4Header[3] = (GT_U8)(ipv4Ptr->totalLen & 0xFF);
    ipv4Header[2] = (GT_U8)((ipv4Ptr->totalLen >> 8) & 0xFF);
    ipv4Header[1] = (GT_U8)(ipv4Ptr->typeOfService & 0xFF);
    ipv4Header[0] = (GT_U8)((ipv4Ptr->headerLen & 0xF) |
                           ((ipv4Ptr->version & 0xF) << 4));

    rc = prvTgfTrafficTraceOutput(ipv4Ptr, TGF_PACKET_PART_IPV4_E);

    prvTgfMemCopy(bufferPtr, &ipv4Header[0], sizeof(ipv4Header),
                  startBytePtr, endByte);

    prvTgfMemCopy(bufferPtr,
                  &ipv4Ptr->srcAddr[0],
                  sizeof(ipv4Ptr->srcAddr),
                  startBytePtr,
                  endByte);

    prvTgfMemCopy(bufferPtr,
                  &ipv4Ptr->dstAddr[0],
                  sizeof(ipv4Ptr->dstAddr),
                  startBytePtr,
                  endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildGrePart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                         next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       0                   1                   2                   3
*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |C|      Reserved=0       |Ver=0|       Protocol Type           |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       C = (Checksum Present=0)
*
*******************************************************************************/
static GT_STATUS prvTgfBuildGrePart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr,
    OUT   TGF_AUTO_CHECKSUM_FULL_INFO_STC *checksumInfoPtr
)
{
    TGF_PACKET_GRE_STC* grePtr = partPtr;
    GT_STATUS            rc      = GT_OK;
    GT_U8                greHeader[8];
    GT_U8                checkSumPresent;
    GT_U32               ii;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    checkSumPresent = (grePtr->checkSumPresent ? 1 : 0);
    if (checkSumPresent)
    {
        CPSS_NULL_PTR_CHECK_MAC(checksumInfoPtr);
        for (ii = 0 ;ii < TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS ; ii++)
        {
            if (checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_GRE_E].startPtr)
            {
                /* this instance is already used */
                continue;
            }

            if (grePtr->checksum == TGF_PACKET_AUTO_CALC_CHECKSUM_CNS)
            {
                checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_GRE_E].enable = GT_TRUE;
                checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_GRE_E].startPtr = &bufferPtr[*startBytePtr];
                checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_GRE_E].numBytes = 4;
                checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_GRE_E].checkSumPtr = &bufferPtr[*startBytePtr + 4];
            }
            break;
        }

        if (ii == TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS)
        {
            return GT_OUT_OF_RANGE;
        }
    }
    greHeader[0] = ((checkSumPresent << 7) | ((grePtr->reserved0 & 0xFE0) >> 5));
    greHeader[1] = (((grePtr->reserved0 & 0x1F) << 3) | (grePtr->version));
    greHeader[2] = (GT_U8)((grePtr->protocol >> 8) & 0xFF);
    greHeader[3] = (GT_U8)(grePtr->protocol & 0xFF);

    rc = prvTgfTrafficTraceOutput(grePtr, TGF_PACKET_PART_GRE_E);

    prvTgfMemCopy(bufferPtr, &greHeader[0], sizeof(GT_U32),
                  startBytePtr, endByte);

    if (checkSumPresent)
    {
        greHeader[4] = (GT_U8)((grePtr->checksum >> 8) & 0xFF);
        greHeader[5] = (GT_U8)(grePtr->checksum & 0xFF);
        greHeader[6] = (GT_U8)((grePtr->reserved1 >> 8) & 0xFF);
        greHeader[7] = (GT_U8)(grePtr->reserved1 & 0xFF);
        prvTgfMemCopy(bufferPtr, &greHeader[4], sizeof(GT_U32),
                      startBytePtr, endByte);
    }

    return rc;
}

/*******************************************************************************
* prvTgfBuildIpv6Part
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*       checksumExtraInfoPtr -  (pointer to) single extra info for auto checksum build
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |Version| Traffic Class |           Flow Label                  |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |         Payload Length        |  Next Header  |   Hop Limit   |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |                                                               |
*       +                                                               +
*       |                                                               |
*       +                         Source Address                        +
*       |                                                               |
*       +                                                               +
*       |                                                               |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |                                                               |
*       +                                                               +
*       |                                                               |
*       +                      Destination Address                      +
*       |                                                               |
*       +                                                               +
*       |                                                               |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*******************************************************************************/
static GT_STATUS prvTgfBuildIpv6Part
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr,
    IN  TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
)
{
    TGF_PACKET_IPV6_STC* ipv6Ptr = partPtr;
    GT_STATUS            rc      = GT_OK;
    GT_U8                ipv6Header[8];
    GT_U8                ipv6Addr[16];
    GT_U8                iter;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    if(checksumExtraInfoPtr)
    {
        checksumExtraInfoPtr->isIpv4 = GT_FALSE;
        checksumExtraInfoPtr->startL3HeadrPtr = &bufferPtr[*startBytePtr];
    }

    ipv6Header[7] = (GT_U8)(ipv6Ptr->hopLimit & 0xFF);
    ipv6Header[6] = (GT_U8)(ipv6Ptr->nextHeader & 0xFF);
    ipv6Header[5] = (GT_U8)(ipv6Ptr->payloadLen & 0xFF);
    ipv6Header[4] = (GT_U8)((ipv6Ptr->payloadLen >> 8) & 0xFF);
    ipv6Header[3] = (GT_U8)(ipv6Ptr->flowLabel & 0xFF);
    ipv6Header[2] = (GT_U8)((ipv6Ptr->flowLabel >> 8) & 0xFF);
    ipv6Header[1] = (GT_U8)(((ipv6Ptr->flowLabel >> 16) & 0xF) |
                            ((ipv6Ptr->trafficClass & 0xF) << 4));
    ipv6Header[0] = (GT_U8)(((ipv6Ptr->trafficClass >> 4) & 0xF) |
                            ((ipv6Ptr->version & 0xF) << 4));

    rc = prvTgfTrafficTraceOutput(ipv6Ptr, TGF_PACKET_PART_IPV6_E);

    prvTgfMemCopy(bufferPtr, &ipv6Header[0], sizeof(ipv6Header),
                  startBytePtr, endByte);


    /* SRC IP ADDR */
    /* convert ipv6Ptr->srcAddr with saving network order */
    for (iter = 0; iter < 8; iter++)
    {
        ipv6Addr[2 * iter]       = (GT_U8)(ipv6Ptr->srcAddr[iter] >> 8);
        ipv6Addr[(2 * iter) + 1] = (GT_U8)(ipv6Ptr->srcAddr[iter] & 0xFF);
    }

    prvTgfMemCopy(bufferPtr,
                  (GT_U8*)ipv6Addr,
                  sizeof(ipv6Addr),
                  startBytePtr,
                  endByte);

    /* DST IP ADDR */
    /* convert ipv6Ptr->dstAddr with saving network order */

    for (iter = 0; iter < 8; iter++)
    {
        ipv6Addr[2 * iter]       = (GT_U8)(ipv6Ptr->dstAddr[iter] >> 8);
        ipv6Addr[(2 * iter) + 1] = (GT_U8)(ipv6Ptr->dstAddr[iter] & 0xFF);
    }


    prvTgfMemCopy(bufferPtr,
                  (GT_U8*)ipv6Addr,
                  sizeof(ipv6Addr),
                  startBytePtr,
                  endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildTcpPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*       checksumInfoArr - (array of) info about instance of fields that need auto checksum build
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       0                   1                   2                   3
*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |          Source Port          |       Destination Port        |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |                        Sequence Number                        |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |                    Acknowledgment Number                      |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |  Data |           |U|A|P|R|S|F|                               |
*      | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
*      |       |           |G|K|H|T|N|N|                               |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |           Checksum            |         Urgent Pointer        |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |                    Options                    |    Padding    |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*      |                             data                              |
*      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*******************************************************************************/
static GT_STATUS prvTgfBuildTcpPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr,
    OUT TGF_AUTO_CHECKSUM_FULL_INFO_STC *checksumInfoPtr
)
{
    TGF_PACKET_TCP_STC* tcpPtr = partPtr;
    GT_STATUS           rc     = GT_OK;
    GT_U8               tcpHeader[20];
    GT_U32              ii;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    if(checksumInfoPtr)
    {
        for(ii = 0 ;ii < TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS ; ii++)
        {
            if(checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_TCP_E].startPtr)
            {
                /* this instance already used */
                continue;
            }

            /* indicate that the index is used , to keep synch between the ipv4 header and the TCP header (for pseudo header calc) */
            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_TCP_E].startPtr = &bufferPtr[*startBytePtr];

            if(tcpPtr->csum != TGF_PACKET_AUTO_CALC_CHECKSUM_CNS)
            {
                /* no more to do */
                break;
            }

            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_TCP_E].enable = GT_TRUE;
            /* TCP not hold length , so take it from ipv4/6 info */
            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_TCP_E].numBytes = 0;
            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_TCP_E].checkSumPtr = &bufferPtr[*startBytePtr + 16];
            break;
        }

        if(ii == TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS)
        {
            /* need to enlarge the constant of TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS */
            return GT_OUT_OF_RANGE;
        }
    }

    tcpHeader[19] = (GT_U8)(tcpPtr->urgentPtr & 0xFF);
    tcpHeader[18] = (GT_U8)((tcpPtr->urgentPtr >> 8) & 0xFF);
    tcpHeader[17] = (GT_U8)(tcpPtr->csum & 0xFF);
    tcpHeader[16] = (GT_U8)((tcpPtr->csum >> 8) & 0xFF);
    tcpHeader[15] = (GT_U8)(tcpPtr->windowSize & 0xFF);
    tcpHeader[14] = (GT_U8)((tcpPtr->windowSize >> 8) & 0xFF);
    tcpHeader[13] = (GT_U8)(tcpPtr->flags & 0xFF);
    tcpHeader[12] = (GT_U8)((tcpPtr->reserved & 0xF) |
                           ((tcpPtr->dataOffset & 0xF) << 4));
    tcpHeader[11] = (GT_U8)(tcpPtr->acknowledgeNum & 0xFF);
    tcpHeader[10] = (GT_U8)((tcpPtr->acknowledgeNum >> 8) & 0xFF);
    tcpHeader[9] = (GT_U8)((tcpPtr->acknowledgeNum >> 16) & 0xFF);
    tcpHeader[8] = (GT_U8)((tcpPtr->acknowledgeNum >> 24) & 0xFF);
    tcpHeader[7] = (GT_U8)(tcpPtr->sequenceNum & 0xFF);
    tcpHeader[6] = (GT_U8)((tcpPtr->sequenceNum >> 8) & 0xFF);
    tcpHeader[5] = (GT_U8)((tcpPtr->sequenceNum >> 16) & 0xFF);
    tcpHeader[4] = (GT_U8)((tcpPtr->sequenceNum >> 24) & 0xFF);
    tcpHeader[3] = (GT_U8)(tcpPtr->dstPort & 0xFF);
    tcpHeader[2] = (GT_U8)((tcpPtr->dstPort >> 8) & 0xFF);
    tcpHeader[1] = (GT_U8)(tcpPtr->srcPort & 0xFF);
    tcpHeader[0] = (GT_U8)((tcpPtr->srcPort >> 8) & 0xFF);

    rc = prvTgfTrafficTraceOutput(tcpPtr, TGF_PACKET_PART_TCP_E);

    prvTgfMemCopy(bufferPtr, &tcpHeader[0], sizeof(tcpHeader),
                  startBytePtr, endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildUdpPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*       checksumInfoArr - (array of) info about instance of fields that need auto checksum build
*       checksumExtraInfoPtr -  (array of) extra info for auto checksum build
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*                  0      7 8     15 16    23 24    31
*                 +--------+--------+--------+--------+
*                 |     Source      |   Destination   |
*                 |      Port       |      Port       |
*                 +--------+--------+--------+--------+
*                 |                 |                 |
*                 |     Length      |    Checksum     |
*                 +--------+--------+--------+--------+
*                 |
*                 |          data octets ...
*                 +---------------- ...
*
*******************************************************************************/
static GT_STATUS prvTgfBuildUdpPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr,
    OUT TGF_AUTO_CHECKSUM_FULL_INFO_STC *checksumInfoPtr,
    INOUT  TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
)
{
    TGF_PACKET_UDP_STC* udpPtr = partPtr;
    GT_STATUS           rc     = GT_OK;
    GT_U8               udpHeader[8];
    GT_U16              udpLength;
    GT_U32              ii = 0;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    udpLength = udpPtr->length;

    if(checksumInfoPtr)
    {
        for(ii = 0 ;ii < TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS ; ii++)
        {
            if(checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_UDP_E].startPtr)
            {
                /* this instance already used */
                continue;
            }

            /* indicate that the index is used , to keep synch between the ipv4 header and the UDP header (for pseudo header calc) */
            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_UDP_E].startPtr = &bufferPtr[*startBytePtr];

            if(udpPtr->csum != TGF_PACKET_AUTO_CALC_CHECKSUM_CNS)
            {
                /* no more to do */
                break;
            }

            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_UDP_E].enable = GT_TRUE;
            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_UDP_E].numBytes = udpLength;
            checksumInfoPtr[ii].singleInfoArr[TGF_AUTO_CHECKSUM_FIELD_UDP_E].checkSumPtr = &bufferPtr[*startBytePtr + 6];
            break;
        }

        if(ii == TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS)
        {
            /* need to enlarge the constant of TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS */
            return GT_OUT_OF_RANGE;
        }
    }

    if(udpLength == TGF_PACKET_AUTO_CALC_LENGTH_CNS)
    {
        /* need to get info from the IPv4/6 */
        if(checksumExtraInfoPtr)
        {
            checksumExtraInfoPtr[ii].udpLengthFieldPtr = &bufferPtr[*startBytePtr + 4];
        }
    }

    udpHeader[7] = (GT_U8)(udpPtr->csum & 0xFF);
    udpHeader[6] = (GT_U8)((udpPtr->csum >> 8) & 0xFF);
    udpHeader[5] = (GT_U8)(udpLength & 0xFF);
    udpHeader[4] = (GT_U8)((udpLength >> 8) & 0xFF);
    udpHeader[3] = (GT_U8)(udpPtr->dstPort & 0xFF);
    udpHeader[2] = (GT_U8)((udpPtr->dstPort >> 8) & 0xFF);
    udpHeader[1] = (GT_U8)(udpPtr->srcPort & 0xFF);
    udpHeader[0] = (GT_U8)((udpPtr->srcPort >> 8) & 0xFF);

    rc = prvTgfTrafficTraceOutput(udpPtr, TGF_PACKET_PART_UDP_E);

    prvTgfMemCopy(bufferPtr, &udpHeader[0], sizeof(udpHeader),
                  startBytePtr, endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildArpPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*    Ethernet transmission layer (not necessarily accessible to the user):
*        48.bit: Ethernet address of destination
*        48.bit: Ethernet address of sender
*        16.bit: Protocol type = ether_type$ADDRESS_RESOLUTION (0806)
*
*    Ethernet packet data:
*
*       0                   1                   2                   3
*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |              HW Type          |    Protocol (IPv4= 0x0800)    |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |   HW Len = 6  | Proto Len = 4 |            Opcode             |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       | nbytes: HW Address of Sender
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*       | mbytes: Protocol Address of Sender
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*       | nbytes: HW Address of Target
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*       | mbytes: Protocol Address of Target
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*
*******************************************************************************/
static GT_STATUS prvTgfBuildArpPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_ARP_STC* arpPtr = partPtr;
    GT_STATUS           rc     = GT_OK;
    GT_U8               arpHeader[8];

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    arpHeader[7] = (GT_U8)(arpPtr->opCode & 0xFF);
    arpHeader[6] = (GT_U8)((arpPtr->opCode >> 8) & 0xFF);
    arpHeader[5] = (GT_U8)(arpPtr->protLen & 0xFF);
    arpHeader[4] = (GT_U8)(arpPtr->hwLen & 0xFF);
    arpHeader[3] = (GT_U8)(arpPtr->protType & 0xFF);
    arpHeader[2] = (GT_U8)((arpPtr->protType >> 8) & 0xFF);
    arpHeader[1] = (GT_U8)(arpPtr->hwType & 0xFF);
    arpHeader[0] = (GT_U8)((arpPtr->hwType >> 8) & 0xFF);

    rc = prvTgfTrafficTraceOutput(arpPtr, TGF_PACKET_PART_ARP_E);

    prvTgfMemCopy(bufferPtr, &arpHeader[0], sizeof(arpHeader),
                  startBytePtr, endByte);

    prvTgfMemCopy(bufferPtr,
                  &arpPtr->srcMac[0],
                  sizeof(arpPtr->srcMac),
                  startBytePtr,
                  endByte);

    prvTgfMemCopy(bufferPtr,
                  &arpPtr->srcIp[0],
                  sizeof(arpPtr->srcIp),
                  startBytePtr,
                  endByte);

    prvTgfMemCopy(bufferPtr,
                  &arpPtr->dstMac[0],
                  sizeof(arpPtr->dstMac),
                  startBytePtr,
                  endByte);

    prvTgfMemCopy(bufferPtr,
                  &arpPtr->dstIp[0],
                  sizeof(arpPtr->dstIp),
                  startBytePtr,
                  endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildIcmpPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfBuildIcmpPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_ICMP_STC* icmpPtr = partPtr;
    GT_STATUS            rc      = GT_OK;
    GT_U8                icmpHeader[8];

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    icmpHeader[7] = (GT_U8)(icmpPtr->sequenceNum & 0xFF);
    icmpHeader[6] = (GT_U8)((icmpPtr->sequenceNum >> 8) & 0xFF);
    icmpHeader[5] = (GT_U8)(icmpPtr->id & 0xFF);
    icmpHeader[4] = (GT_U8)((icmpPtr->id >> 8) & 0xFF);
    icmpHeader[3] = (GT_U8)(icmpPtr->csum & 0xFF);
    icmpHeader[2] = (GT_U8)((icmpPtr->csum >> 8) & 0xFF);
    icmpHeader[1] = (GT_U8)(icmpPtr->code & 0xFF);
    icmpHeader[0] = (GT_U8)(icmpPtr->typeOfService & 0xFF);

    rc = prvTgfTrafficTraceOutput(icmpPtr, TGF_PACKET_PART_ICMP_E);

    prvTgfMemCopy(bufferPtr, &icmpHeader[0], sizeof(icmpHeader),
                  startBytePtr, endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildWildCardPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfBuildWildCardPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_WILDCARD_STC* wildCardPtr = partPtr;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    prvTgfMemCopy(bufferPtr,
                  &wildCardPtr->bytesPtr[0],
                  wildCardPtr->numOfBytes,
                  startBytePtr, endByte);

    return GT_OK;
}

/*******************************************************************************
* prvTgfBuildPayloadPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfBuildPayloadPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_PAYLOAD_STC* payloadPtr = partPtr;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    prvTgfMemCopy(bufferPtr,
                  &payloadPtr->dataPtr[0],
                  payloadPtr->dataLength,
                  startBytePtr, endByte);

    return GT_OK;
}

/*******************************************************************************
* prvTgfBuildGenTunnelTemplatePart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfBuildGenTunnelTemplatePart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_TEMPLATE16_STC* templatePtr = partPtr;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    prvTgfMemCopy(bufferPtr,
                  &templatePtr->dataPtr[0],
                  templatePtr->dataLength,
                  startBytePtr, endByte);

    return GT_OK;
}

/*******************************************************************************
* prvTgfBuildPtpV2Part
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfBuildPtpV2Part
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_PTP_V2_STC* ptpV2Ptr = (TGF_PACKET_PTP_V2_STC*)partPtr;
    GT_U8                  *bufPtr = &(bufferPtr[*startBytePtr]);
    GT_STATUS              rc;
    GT_U32                 i;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    if (endByte == 0)
    {
        /* measuring only */
        (*startBytePtr) += TGF_PTP_V2_HDR_SIZE_CNS;
        return GT_OK;
    }

    rc = prvTgfTrafficTraceOutput(ptpV2Ptr, TGF_PACKET_PART_PTP_V2_E);


    if (endByte < (*startBytePtr + TGF_PTP_V2_HDR_SIZE_CNS))
    {
        /* not enough place for data */
        return GT_BAD_PARAM;
    }

    (*startBytePtr) += TGF_PTP_V2_HDR_SIZE_CNS;

    bufPtr[0]  = (GT_U8)
        ((ptpV2Ptr->messageType & 0xF) | ((ptpV2Ptr->transportSpecific & 0xF) << 4));
    bufPtr[1]  = (GT_U8)
        ((ptpV2Ptr->ptpVersion & 0xF) | ((ptpV2Ptr->reserved4 & 0xF) << 4));
    bufPtr[2]  = (GT_U8)((ptpV2Ptr->messageLength >> 8) & 0xFF);
    bufPtr[3]  = (GT_U8)(ptpV2Ptr->messageLength & 0xFF);
    bufPtr[4]  = (GT_U8)(ptpV2Ptr->domainNumber & 0xFF);
    bufPtr[5]  = (GT_U8)(ptpV2Ptr->reserved8 & 0xFF);
    bufPtr[6]  = (GT_U8)((ptpV2Ptr->flagField >> 8) & 0xFF);
    bufPtr[7]  = (GT_U8)(ptpV2Ptr->flagField & 0xFF);
    bufPtr[8]  = (GT_U8)((ptpV2Ptr->correctionField[1] >> 24) & 0xFF);
    bufPtr[9]  = (GT_U8)((ptpV2Ptr->correctionField[1] >> 16) & 0xFF);
    bufPtr[10] = (GT_U8)((ptpV2Ptr->correctionField[1] >> 8) & 0xFF);
    bufPtr[11] = (GT_U8)(ptpV2Ptr->correctionField[1] & 0xFF);
    bufPtr[12] = (GT_U8)((ptpV2Ptr->correctionField[0] >> 24) & 0xFF);
    bufPtr[13] = (GT_U8)((ptpV2Ptr->correctionField[0] >> 16) & 0xFF);
    bufPtr[14] = (GT_U8)((ptpV2Ptr->correctionField[0] >> 8) & 0xFF);
    bufPtr[15] = (GT_U8)(ptpV2Ptr->correctionField[0] & 0xFF);
    bufPtr[16] = (GT_U8)((ptpV2Ptr->reserved32 >> 24) & 0xFF);
    bufPtr[17] = (GT_U8)((ptpV2Ptr->reserved32 >> 16) & 0xFF);
    bufPtr[18] = (GT_U8)((ptpV2Ptr->reserved32 >> 8) & 0xFF);
    bufPtr[19] = (GT_U8)(ptpV2Ptr->reserved32 & 0xFF);
    for (i = 0; (i < 10); i++)
    {
        bufPtr[20 + i] = ptpV2Ptr->sourcePortIdentify[i];
    }
    bufPtr[30]  = (GT_U8)((ptpV2Ptr->sequenceId >> 8) & 0xFF);
    bufPtr[31]  = (GT_U8)(ptpV2Ptr->sequenceId & 0xFF);
    bufPtr[32]  = (GT_U8)(ptpV2Ptr->controlField & 0xFF);
    bufPtr[33]  = (GT_U8)(ptpV2Ptr->logMessageInterval & 0xFF);

    return rc;
}

/*******************************************************************************
* prvTgfBuildTrillPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*        0                   1                   2                   3
*        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       | V | R |M|Op-Length| Hop Count |   Egress RBridge Nickname     |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |    Ingress RBridge Nickname   |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*******************************************************************************/
static GT_STATUS prvTgfBuildTrillPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_TRILL_STC* trillPtr = partPtr;
    GT_STATUS  rc = GT_OK;
    GT_U8 trillHeader[6];

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    trillHeader[0] = (GT_U8)(((trillPtr->version & 0x3) << 6) |
                             ((trillPtr->mBit & 0x1) << 3) |
                             ((trillPtr->opLength >> 2) & 0x7));
    trillHeader[1] = (GT_U8)(((trillPtr->opLength & 0x3) << 6) |
                             (trillPtr->hopCount & 0x3F));
    trillHeader[2] = (GT_U8)((trillPtr->eRbid >> 8) & 0xFF);
    trillHeader[3] = (GT_U8)(trillPtr->eRbid & 0xFF);
    trillHeader[4] = (GT_U8)((trillPtr->iRbid >> 8) & 0xFF);
    trillHeader[5] = (GT_U8)(trillPtr->iRbid & 0xFF);

    rc = prvTgfTrafficTraceOutput(trillPtr, TGF_PACKET_PART_TRILL_E);

    prvTgfMemCopy(bufferPtr, &trillHeader[0], sizeof(trillHeader),
                  startBytePtr, endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildTrillFirstOptionPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*        0                   1                   2                     3
*        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9   0    1
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+----+----+
*       |           Data for Option                                 |CHbH|CItE|
*       +----+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+----+----+
*                        30 bit                                     1 bit 1 bit
*
*******************************************************************************/
static GT_STATUS prvTgfBuildTrillFirstOptionPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_TRILL_FIRST_OPTION_STC* trillFirstOptionPtr = partPtr;
    GT_STATUS  rc = GT_OK;
    GT_U8 trillFirstOptionHeader[4];

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    trillFirstOptionHeader[0] = (GT_U8)((trillFirstOptionPtr->info >> 22) & 0xFF);
    trillFirstOptionHeader[1] = (GT_U8)((trillFirstOptionPtr->info >> 14) & 0xFF);
    trillFirstOptionHeader[2] = (GT_U8)((trillFirstOptionPtr->info >> 6) & 0xFF);
    trillFirstOptionHeader[3] = (GT_U8)(((trillFirstOptionPtr->info << 6) & 0xF) |
                                        ((trillFirstOptionPtr->chbh << 1) & 1) |
                                        (trillFirstOptionPtr->chbh & 1));

    rc = prvTgfTrafficTraceOutput(trillFirstOptionPtr, TGF_PACKET_PART_TRILL_FIRST_OPTION_E);

    prvTgfMemCopy(bufferPtr, &trillFirstOptionHeader[0], sizeof(trillFirstOptionHeader),
                  startBytePtr, endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildTrillGeneralOptionPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
*
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*        0                   1                   2                   3
*        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |           Data for Options                                    |
*       +----+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*                        32 bit
*
*******************************************************************************/
static GT_STATUS prvTgfBuildTrillGeneralOptionPart
(
    IN    GT_VOID      *partPtr,
    INOUT GT_U32       *startBytePtr,
    IN    GT_U32        endByte,
    OUT   GT_U8        *bufferPtr
)
{
    TGF_PACKET_TRILL_GEN_OPTION_STC* trillGenOptionPtr = partPtr;
    GT_STATUS  rc = GT_OK;
    GT_U8 trillGenOptionHeader[4];

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    trillGenOptionHeader[0] = (GT_U8)((trillGenOptionPtr->info >> 24) & 0xFF);
    trillGenOptionHeader[1] = (GT_U8)((trillGenOptionPtr->info >> 16) & 0xFF);
    trillGenOptionHeader[2] = (GT_U8)((trillGenOptionPtr->info >> 8) & 0xFF);
    trillGenOptionHeader[3] = (GT_U8)((trillGenOptionPtr->info & 0xFF));

    rc = prvTgfTrafficTraceOutput(trillGenOptionPtr, TGF_PACKET_PART_TRILL_GENERAL_OPTION_E);

    prvTgfMemCopy(bufferPtr, &trillGenOptionHeader[0], sizeof(trillGenOptionHeader),
                  startBytePtr, endByte);

    return rc;
}

/*******************************************************************************
* prvTgfBuildPacketPart
*
* DESCRIPTION:
*       Build part of packet from specific part input fields
*
* INPUTS:
*       partPtr      - part fields info
*       startBytePtr - (pointer to) the first byte in bufferPtr to write to
*       endByte      - the max index allowed to be modified in bufferPtr
* OUTPUTS:
*       startBytePtr - (pointer to) the first byte in bufferPtr for the
*                       next builder to write to
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*       checksumInfoArr - (array of) info about instance of fields that need auto checksum build
*       checksumExtraInfoPtr -  (array of) extra info for auto checksum build
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfBuildPacketPart
(
    IN    TGF_PACKET_PART_STC   *partPtr,
    INOUT GT_U32                *startBytePtr,
    IN    GT_U32                 endByte,
    OUT   GT_U8                 *bufferPtr,
    OUT TGF_AUTO_CHECKSUM_FULL_INFO_STC *checksumInfoPtr,
    OUT TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
)
{
    GT_STATUS rc = GT_OK;

    CPSS_NULL_PTR_CHECK_MAC(partPtr);
    /*  removed to allow calculation of length without building the packet :
        CPSS_NULL_PTR_CHECK_MAC(startBytePtr);
        CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    */

    switch(partPtr->type)
    {
        case TGF_PACKET_PART_L2_E:
            rc = prvTgfBuildL2Part(partPtr->partPtr, startBytePtr, endByte, bufferPtr);
            break;

        case TGF_PACKET_PART_VLAN_TAG_E:
            rc = prvTgfBuildVlanTagPart(partPtr->partPtr, startBytePtr, endByte, bufferPtr);
            break;

        case TGF_PACKET_PART_DSA_TAG_E:
            rc = prvTgfBuildDsaTagPart(partPtr->partPtr, startBytePtr, endByte, bufferPtr);
            break;

        case TGF_PACKET_PART_ETHERTYPE_E:
            rc = prvTgfBuildEtherTypePart(partPtr->partPtr, startBytePtr, endByte, bufferPtr, GT_FALSE);
            break;

        case TGF_PACKET_PART_MPLS_E:
            rc = prvTgfBuildMplsPart(partPtr->partPtr, startBytePtr, endByte, bufferPtr);
            break;

        case TGF_PACKET_PART_IPV4_E:
            rc = prvTgfBuildIpv4Part(partPtr->partPtr, startBytePtr, endByte, bufferPtr,checksumInfoPtr,checksumExtraInfoPtr);
            break;

        case TGF_PACKET_PART_IPV6_E:
            rc = prvTgfBuildIpv6Part(partPtr->partPtr, startBytePtr, endByte, bufferPtr,checksumExtraInfoPtr);
            break;
        case TGF_PACKET_PART_TCP_E:
            rc = prvTgfBuildTcpPart(partPtr->partPtr, startBytePtr, endByte, bufferPtr,checksumInfoPtr);
            break;

        case TGF_PACKET_PART_UDP_E:
            rc = prvTgfBuildUdpPart(partPtr->partPtr, startBytePtr, endByte, bufferPtr,checksumInfoPtr,checksumExtraInfoPtr);
            break;

        case TGF_PACKET_PART_ARP_E:
            rc = prvTgfBuildArpPart(partPtr->partPtr, startBytePtr, endByte, bufferPtr);
            break;

        case TGF_PACKET_PART_ICMP_E:
            rc = prvTgfBuildIcmpPart(partPtr->partPtr, startBytePtr, endByte, bufferPtr);
            break;

        case TGF_PACKET_PART_WILDCARD_E:
        case TGF_PACKET_PART_TUNNEL_HEADER_FORCE_TILL_PASSENGER_E:
        case TGF_PACKET_PART_CRC_E:
            rc = prvTgfBuildWildCardPart(partPtr->partPtr,startBytePtr,endByte,bufferPtr);
            break;

        case TGF_PACKET_PART_PTP_V2_E:
            rc = prvTgfBuildPtpV2Part(partPtr->partPtr,startBytePtr,endByte,bufferPtr);
            break;

        case TGF_PACKET_PART_PAYLOAD_E:
            rc = prvTgfBuildPayloadPart(partPtr->partPtr,startBytePtr,endByte,bufferPtr);
            break;

        case TGF_PACKET_PART_GRE_E:
            rc = prvTgfBuildGrePart(partPtr->partPtr,startBytePtr,endByte,bufferPtr,checksumInfoPtr);
            break;

        case TGF_PACKET_PART_TRILL_E:
            rc = prvTgfBuildTrillPart(partPtr->partPtr,startBytePtr,endByte,bufferPtr);
            break;

        case TGF_PACKET_PART_TRILL_FIRST_OPTION_E:
            rc = prvTgfBuildTrillFirstOptionPart(partPtr->partPtr,startBytePtr,endByte,bufferPtr);
            break;

        case TGF_PACKET_PART_TRILL_GENERAL_OPTION_E:
            rc = prvTgfBuildTrillGeneralOptionPart(partPtr->partPtr,startBytePtr,endByte,bufferPtr);
            break;
        case TGF_PACKET_PART_TEMPLATE_E:
            rc = prvTgfBuildGenTunnelTemplatePart(partPtr->partPtr,startBytePtr,endByte,bufferPtr);
        break;

        case TGF_PACKET_PART_SKIP_E:
            rc = GT_OK;/* skip this part */
            break;
        default:
            return GT_BAD_PARAM;
    }

    return rc;
}


/******************************************************************************\
 *                              API implementation                            *
\******************************************************************************/

static TGF_PACKET_PART_STC   outsidePart;
static TGF_PACKET_PART_STC   *outsidePartPtr = &outsidePart;
static GT_BOOL  forceDsaTag = GT_FALSE;
static GT_BOOL  forceDsaTagReplaceVlanTagIfExists = GT_FALSE;

/*******************************************************************************
* tgfTrafficEnginePacketForceDsaOnPacket
*
* DESCRIPTION:
*       force DSA tag into a packet that do not hold DSA.
*       also allow the DSA to replace the outer VLAN (if exists)
*       this function allow the 'send packet' and the 'compare packet' functions
*       to not be aware to the DSA that need to be in the packet.
*
*       this is useful before: prvTgfStartTransmitingEth , tgfTrafficGeneratorPortTxEthCaptureCompare
*       since both use function tgfTrafficEnginePacketBuild
*
* INPUTS:
*       dsaPartPtr - pointer to the DSA part.
*       replaceVlanTagIfExists - indication to replace the outer VLAN (if exists)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*
* COMMENTS:
*
*       Caller must call it with dsaPartPtr = NULL , to remove the configurations.
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketForceDsaOnPacket
(
    IN TGF_PACKET_DSA_TAG_STC   *dsaPartPtr,
    IN GT_BOOL                  replaceVlanTagIfExists
)
{

    forceDsaTagReplaceVlanTagIfExists = replaceVlanTagIfExists;

    if(dsaPartPtr)
    {
        outsidePart.type = TGF_PACKET_PART_DSA_TAG_E;
        outsidePart.partPtr = dsaPartPtr;

        forceDsaTag = GT_TRUE;
    }
    else
    {
        outsidePart.type = TGF_PACKET_PART_SKIP_E;
        outsidePart.partPtr = NULL;
        forceDsaTag = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficEnginePacketBuild
*
* DESCRIPTION:
*       Build packet from input fields
*
* INPUTS:
*       packetInfoPtr - packet fields info
*       numOfBytesInBuffPtr - (pointer to)number of bytes in the bufferPtr
*
* OUTPUTS:
*       bufferPtr - (pointer to) the buffer that hold the packet bytes
*       numOfBytesInBuffPtr - (pointer to)number of bytes actually set in the bufferPtr
*       checksumInfoArr - (array of) info about instance of fields that need auto checksum build
*       checksumExtraInfoPtr -  (array of) extra info for auto checksum build
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
GT_STATUS tgfTrafficEnginePacketBuild
(
    IN  TGF_PACKET_STC    *packetInfoPtr,
    OUT GT_U8             *bufferPtr,
    INOUT  GT_U32         *numOfBytesInBuffPtr,
    OUT TGF_AUTO_CHECKSUM_FULL_INFO_STC *checksumInfoPtr,
    OUT TGF_AUTO_CHECKSUM_EXTRA_INFO_STC *checksumExtraInfoPtr
)
{
    GT_STATUS   rc       = GT_OK;
    GT_U32      currByte = 0;
    GT_U32      partIter = 0;
    GT_U32      maxBytesToWrite;/* max bytes to write to bufferPtr[] */
    GT_U32      replacedPartIndex;
    GT_U32      addedPartIndex;
    GT_BOOL     useOutSidePart = GT_FALSE;
    TGF_PACKET_PART_STC   *partPtr;

    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(numOfBytesInBuffPtr);

    if(checksumExtraInfoPtr)
    {
        cpssOsMemSet(checksumExtraInfoPtr,0,
                     sizeof(TGF_AUTO_CHECKSUM_EXTRA_INFO_STC)*TGF_AUTO_CHECKSUM_NUM_OF_HEADERS_CNS);
    }

    /* start writing to byte 0 in the buffer */
    currByte = 0;

    if(((*numOfBytesInBuffPtr) < packetInfoPtr->totalLen) &&
       (packetInfoPtr->totalLen != TGF_PACKET_AUTO_CALC_LENGTH_CNS))
    {
        /* total length needed is longer then the given buffer length */
        return GT_BAD_PARAM;
    }

    if(packetInfoPtr->totalLen == TGF_PACKET_AUTO_CALC_LENGTH_CNS)
    {
        maxBytesToWrite = (*numOfBytesInBuffPtr);
    }
    else
    {
        maxBytesToWrite = packetInfoPtr->totalLen;
    }

    replacedPartIndex = 0xFFFFFFFF;
    addedPartIndex    = 0xFFFFFFFF;

    /* support replacing the vlan tag with DSA*/
    if(forceDsaTag == GT_TRUE && outsidePartPtr &&
       packetInfoPtr->partsArray[0].type == TGF_PACKET_PART_L2_E)
    {
        if(packetInfoPtr->totalLen != TGF_PACKET_AUTO_CALC_LENGTH_CNS)
        {
            maxBytesToWrite += 16;/* max of eDSA */
        }

        if(forceDsaTagReplaceVlanTagIfExists == GT_TRUE &&
           packetInfoPtr->partsArray[1].type == TGF_PACKET_PART_VLAN_TAG_E)
        {
            /* we replace the outer vlan with DSA */
            replacedPartIndex = 1;

            if(packetInfoPtr->totalLen != TGF_PACKET_AUTO_CALC_LENGTH_CNS)
            {
                maxBytesToWrite -= 4;/*since replace DSA , we can reduce 4 bytes */
            }
        }
        else
        {
            /* we add the DSA without replace any part in the packet */
            addedPartIndex = 1;
        }

        useOutSidePart = GT_TRUE;
    }

    /* loop on all parts and add each part after the other */
    for(partIter = 0; partIter < packetInfoPtr->numOfParts; partIter++)
    {
        if(replacedPartIndex == partIter || addedPartIndex == partIter)
        {
            /* add part that was given outside this function and need to be in the packet */
            partPtr = outsidePartPtr;
        }
        else
        {
            /* add the needed part of the packet */
            partPtr = &packetInfoPtr->partsArray[partIter];
        }

        rc = prvTgfBuildPacketPart(partPtr,
                                   &currByte,
                                   (maxBytesToWrite - 1),
                                   bufferPtr,
                                   checksumInfoPtr,
                                   checksumExtraInfoPtr);

        if(rc != GT_OK)
        {
            return rc;
        }

        if(currByte == maxBytesToWrite)
        {
            /* no need to put any extra info , since total packet length is
               reached */
            break;
        }
        else if(currByte > maxBytesToWrite)
        {
            /* we caused overflow by writing to out of buffer */
            return GT_BAD_STATE;
        }

        if(addedPartIndex == partIter)
        {
            addedPartIndex = 0xFFFFFFFF;
            /* allow now to do the actual part that in this index */
            partIter --;
        }

    }

    if(packetInfoPtr->totalLen != TGF_PACKET_AUTO_CALC_LENGTH_CNS)
    {
        if(useOutSidePart == GT_FALSE &&
            currByte != packetInfoPtr->totalLen &&
           (currByte + 4) != packetInfoPtr->totalLen ) /* it seems that the totalLen already include CRC*/
        {
            /* need to find the bug in calculation */
            return GT_GET_ERROR;
        }

        (*numOfBytesInBuffPtr) = packetInfoPtr->totalLen;
    }
    else
    {
        /* we do not change value in packetInfoPtr->totalLen to support
            sending this packet with changeable packet formats
            (more vlan tags,MPLS labels,TRILL options..) */
        (*numOfBytesInBuffPtr) = currByte;
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficEnginePacketParse
*
* DESCRIPTION:
*       Parse packet from bytes to parts and fields
*
* INPUTS:
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*       bufferLength - length of the buffer
*
* OUTPUTS:
*       packetInfoPtrPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*       The packetInfoPtrPtr is pointer to static memory that is 'reused' for
*       every new packet that need 'parse' , so use this packet info before
*       calling to new 'parse'
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketParse
(
    IN  GT_U8              *bufferPtr,
    IN  GT_U32              bufferLength,
    OUT TGF_PACKET_STC    **packetInfoPtrPtr
)
{
    return prvTgfTrafficEnginePacketParse(bufferPtr, bufferLength, packetInfoPtrPtr);
}

/*******************************************************************************
* tgfTrafficEnginePacketVfdApply
*
* DESCRIPTION:
*       Apply VFD info on the buffer
*
* INPUTS:
*       vfdPtr       - (pointer to) VFD info
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*                       before modification
*       bufferLength - the number of bytes in the buffer (not include CRC bytes)
* OUTPUTS:
*       bufferPtr - (pointer to) the buffer that hold the packet bytes
*                    after modification
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketVfdApply
(
    IN    TGF_VFD_INFO_STC       *vfdPtr,
    INOUT GT_U8                  *bufferPtr,
    IN    GT_U32                  bufferLength
)
{
    GT_U32  cycleIter = 0;

    CPSS_NULL_PTR_CHECK_MAC(vfdPtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    if(vfdPtr->mode == TGF_VFD_MODE_OFF_E)
    {
        return GT_OK;
    }
    else
    {
        if(vfdPtr->cycleCount > TGF_VFD_PATTERN_BYTES_NUM_CNS)
        {
            return GT_BAD_PARAM;
        }

        if((vfdPtr->offset + vfdPtr->cycleCount) > bufferLength)
        {
            return GT_BAD_PARAM;
        }
    }

    switch(vfdPtr->mode)
    {
        case TGF_VFD_MODE_RANDOM_E:
            for(cycleIter = 0; cycleIter < vfdPtr->cycleCount; cycleIter++)
            {
                bufferPtr[vfdPtr->offset + cycleIter] = (GT_U8) cpssOsRand();
            }
            break;

        case TGF_VFD_MODE_INCREMENT_E:
            if(vfdPtr->modeExtraInfo == 0)
            {
                /* for the first time not arithmetic needed, copy the bytes and return */
                for(cycleIter = 0 ; cycleIter < vfdPtr->cycleCount; cycleIter++)
                {
                    bufferPtr[vfdPtr->offset + cycleIter] = vfdPtr->patternPtr[cycleIter];
                }
                vfdPtr->modeExtraInfo++;

                break;
            }

            /* for the other times arithmetic needed, increment/decrement */
            vfdPtr->modeExtraInfo++;

            cycleIter = vfdPtr->cycleCount - 1;
            bufferPtr[vfdPtr->offset + cycleIter]++;

            while(1)
            {
                if ((bufferPtr[vfdPtr->offset + cycleIter] != 0) ||
                   ((vfdPtr->offset + cycleIter) == 0) || (cycleIter == 0))
                {
                    break;
                }

                cycleIter--;
                bufferPtr[vfdPtr->offset + cycleIter]++;
            }
            break;

        case TGF_VFD_MODE_DECREMENT_E:
            if(vfdPtr->modeExtraInfo == 0)
            {
                /* for the first time not arithmetic needed , copy the bytes and return */
                for(cycleIter = 0; cycleIter < vfdPtr->cycleCount; cycleIter++)
                {
                    bufferPtr[vfdPtr->offset + cycleIter] = vfdPtr->patternPtr[cycleIter];
                }
                vfdPtr->modeExtraInfo++;

                break;
            }

            /* for the other times arithmetic needed, increment/decrement */
            vfdPtr->modeExtraInfo++;

            cycleIter = vfdPtr->cycleCount - 1;
            bufferPtr[vfdPtr->offset + cycleIter]--;

            while(1)
            {
                if ((bufferPtr[vfdPtr->offset + cycleIter] != 0xFF) ||
                   ((vfdPtr->offset + cycleIter) == 0) || (cycleIter == 0))
                {
                    break;
                }

                cycleIter--;
                bufferPtr[vfdPtr->offset + cycleIter]--;
            }
            break;

        case TGF_VFD_MODE_STATIC_E:
            if(vfdPtr->modeExtraInfo == 0)
            {
                /* for the first time copy the bytes and return */
                for(cycleIter = 0; cycleIter < vfdPtr->cycleCount; cycleIter++)
                {
                    bufferPtr[vfdPtr->offset + cycleIter] = vfdPtr->patternPtr[cycleIter];
                }
            }

            /* for the next times, do nothing */
            vfdPtr->modeExtraInfo++;
            break;

        case TGF_VFD_MODE_INCREMENT_VALUE_E:
            {
                GT_U32 cycleCounterMax = vfdPtr->modeExtraInfo ? vfdPtr->incValue : 1;

                vfdPtr->mode = TGF_VFD_MODE_INCREMENT_E;
                for(cycleIter = 0; cycleIter < cycleCounterMax; cycleIter++)

                {
                    tgfTrafficEnginePacketVfdApply(vfdPtr,bufferPtr,bufferLength);
                }
                vfdPtr->mode = TGF_VFD_MODE_INCREMENT_VALUE_E;
            }
            break;
        case TGF_VFD_MODE_DECREMENT_VALUE_E:
            vfdPtr->mode = TGF_VFD_MODE_DECREMENT_E;
            for(cycleIter = 0 ; cycleIter < vfdPtr->incValue; cycleIter++)
            {
                tgfTrafficEnginePacketVfdApply(vfdPtr,bufferPtr,bufferLength);
            }
            vfdPtr->mode = TGF_VFD_MODE_DECREMENT_VALUE_E;
            break;

        case TGF_VFD_MODE_ARRAY_PATTERN_E:
            for (cycleIter = 0; cycleIter < vfdPtr->cycleCount; cycleIter++)
            {
                bufferPtr[vfdPtr->offset + cycleIter] =
                    vfdPtr->arrayPatternPtr[(vfdPtr->arrayPatternOffset + vfdPtr->modeExtraInfo) * vfdPtr->arrayPatternSize  + cycleIter];
            }
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

static GT_U32  supportNewFeature = 1;
/* give error indication + printings when the 'expected to pass' did not pass */
static void tgfVerifyByteInPacket(
    IN GT_U8  expected,
    IN GT_U8  result,
    IN GT_U32 byteOffset
)
{
    if(supportNewFeature)
    {
        TGF_VERIFY_EQUAL3_STRING_MAC(
            expected ,result,
            "In capture packet byte offset[%d] with value [0x%2.2x] but should be [0x%2.2x] \n",
            byteOffset , result, expected);
    }
}

/* give error indication + printings when the 'expected to fail' did not failed */
static void tgfVerifyByteNoMatchExpected(
    IN  TGF_VFD_INFO_STC       *vfdPtr
)
{
    GT_U32  cycleIter = 0;

    if(supportNewFeature)
    {
        TGF_VERIFY_EQUAL0_STRING_MAC(0,1,
            "expected the VFD compare to 'not match' (at least one of the bytes) , see relevant packet bytes: \n");

        for (cycleIter = 0; cycleIter < vfdPtr->cycleCount; cycleIter++)
        {
            PRV_TGF_LOG2_MAC("offset [%d] with value [%2.2x] \n",
                (vfdPtr->offset + cycleIter),
                vfdPtr->patternPtr[cycleIter]);
        }
    }
}

/*******************************************************************************
* tgfTrafficEnginePacketVfdCheck
*
* DESCRIPTION:
*       Check if the VFD parameters match the buffer
*
* INPUTS:
*       vfdPtr       - (pointer to) VFD info
*       bufferPtr    - (pointer to) the buffer that hold the packet bytes
*       bufferLength - the number of bytes in the buffer (not include CRC bytes)
* OUTPUTS:
*       matchPtr - (pointer to) do we have a match
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       vfdPtr->mode should be TGF_VFD_MODE_STATIC_E or
*           TGF_VFD_MODE_STATIC_EXPECTED_NO_MATCH_E or
*           TGF_VFD_MODE_STATIC_NO_MATCH_IGNORED_E
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketVfdCheck
(
    IN  TGF_VFD_INFO_STC       *vfdPtr,
    IN  GT_U8                  *bufferPtr,
    IN  GT_U32                  bufferLength,
    OUT GT_BOOL                *matchPtr
)
{
    GT_U32  cycleIter = 0;

    CPSS_NULL_PTR_CHECK_MAC(vfdPtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(matchPtr);

    switch(vfdPtr->mode)
    {
        case TGF_VFD_MODE_STATIC_E:
        case TGF_VFD_MODE_STATIC_EXPECTED_NO_MATCH_E:
        case TGF_VFD_MODE_STATIC_NO_MATCH_IGNORED_E:
            if(vfdPtr->cycleCount > TGF_VFD_PATTERN_BYTES_NUM_CNS)
            {
                return GT_BAD_PARAM;
            }

            if(vfdPtr->offset + vfdPtr->cycleCount >= bufferLength)
            {
                return GT_BAD_PARAM;
            }

            *matchPtr = GT_TRUE;

            for (cycleIter = 0; cycleIter < vfdPtr->cycleCount; cycleIter++)
            {
                /* bitwise check bufferPtr with patternPtr for all cleared bits in bitMask */
                if ( (bufferPtr[vfdPtr->offset + cycleIter] & (~vfdPtr->bitMaskPtr[cycleIter])) !=
                     (vfdPtr->patternPtr[cycleIter] & (~vfdPtr->bitMaskPtr[cycleIter])) )
                {
                    *matchPtr = GT_FALSE;

                    if(vfdPtr->mode == TGF_VFD_MODE_STATIC_E)
                    {
                        /* continue to check other bytes , to allow DUMP of all 'not matched' */
                        tgfVerifyByteInPacket(
                            vfdPtr->patternPtr[cycleIter] ,        /*expected*/
                            bufferPtr[vfdPtr->offset + cycleIter], /*result*/
                            vfdPtr->offset + cycleIter);           /* index in packet */
                    }
               }
            }


            break;

        default:
            return GT_BAD_PARAM;
    }

    if(vfdPtr->mode == TGF_VFD_MODE_STATIC_EXPECTED_NO_MATCH_E)
    {
        if((*matchPtr) == GT_TRUE)
        {
            tgfVerifyByteNoMatchExpected(vfdPtr);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficEnginePacketCheckSum16BitsCalc
*
* DESCRIPTION:
*       Calculate checksum of 16 bits
*
* INPUTS:
*       bytesPtr - (pointer) to start of section in packet need to be calculated
*       numBytes - number of bytes need to be included in the calculation
*       pseudoBytesPtr - (pointer to ) start of pseudo info
*       pseudoNumBytes - number of pseudo bytes
* OUTPUTS:
*       checkSumArray - array of 2 bytes with the calculated check sum
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       If there's a field CHECKSUM within the input-buffer
*       it supposed to be zero before calling this function.
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketCheckSum16BitsCalc
(
    IN  GT_U8              *bytesPtr,
    IN  GT_U32              numBytes,
    IN  GT_U8              *pseudoBytesPtr,
    IN  GT_U32              pseudoNumBytes,
    OUT GT_U8               checkSumArray[2]
)
{
    GT_U32 vResult;
    GT_U32 sum;
    GT_U32 byteIndex;

    CPSS_NULL_PTR_CHECK_MAC(bytesPtr);
    CPSS_NULL_PTR_CHECK_MAC(checkSumArray);

    sum = 0;

    /* add up all of the 16 bit quantities */
    for (byteIndex = 0 ; byteIndex < numBytes ;  byteIndex += 2)
    {
        sum += (bytesPtr[byteIndex   + 0] << 8) |
                bytesPtr[byteIndex   + 1] ;
    }

    if (numBytes & 1)
    {
        /* an odd number of bytes */
        sum += (bytesPtr[byteIndex   + 0] << 8);
    }

    /* sum together the two 16 bits sections*/
    vResult = (GT_U16)(sum >> 16) + (GT_U16)sum;

    /* vResult can be bigger then a word (example : 0003 +
    *  fffe = 0001 0001), so sum its two words again.
    */
    if (vResult & 0x10000)
    {
        vResult -= 0x0ffff;
    }

    /*invert the bits*/
    vResult = ~vResult;

    checkSumArray[0] = (GT_U8)(vResult >> 8);
    checkSumArray[1] = (GT_U8)(vResult);

    if(pseudoNumBytes)
    {
        GT_U8   pseudoBytesArr[TGF_AUTO_CHECKSUM_PSEUDO_NUM_BYTES_CNS + 2];/* pseudo + 2 bytes */

        if(pseudoNumBytes > TGF_AUTO_CHECKSUM_PSEUDO_NUM_BYTES_CNS)
        {
            /* not supported */
            return GT_NOT_SUPPORTED;
        }

        /* set the pseudo info to local array */
        cpssOsMemCpy(pseudoBytesArr,pseudoBytesPtr,pseudoNumBytes);

        /* add the main checksum value into the local buff */
        pseudoBytesArr[pseudoNumBytes    ] = (GT_U8)(~checkSumArray[0]);
        pseudoBytesArr[pseudoNumBytes + 1] = (GT_U8)(~checkSumArray[1]);

        return tgfTrafficEnginePacketCheckSum16BitsCalc(
            pseudoBytesArr,
            pseudoNumBytes + 2,
            NULL,
            0,
            checkSumArray);
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficTracePacketBytes
*
* DESCRIPTION:
*       Trace the packet bytes
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       length        - length of packet
*       isCrcIncluded - enable\disable printout CRC in log
* OUTPUTS:
*
* RETURNS:
*       GT_OK      - on success.
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*       Before use this fucntion set 'GT_TRUE' in tgfTrafficTracePacketByteSet
*
*******************************************************************************/
GT_STATUS tgfTrafficTracePacket
(
    IN GT_U8           *bufferPtr,
    IN GT_U32           length,
    IN GT_BOOL          isCrcIncluded
)
{
    GT_U32  iter = 0;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    if (prvTgfTraceGlobalBmp & TGF_TRACE_BYTES_FLAG_CNS)
    {
        if (GT_TRUE == isCrcIncluded)
        {
            PRV_TGF_LOG1_MAC("[TGF]: tgfTrafficTracePacket: packet trace, length [%d](include CRC)\n", length);
        }
        else
        {
            PRV_TGF_LOG1_MAC("[TGF]: tgfTrafficTracePacket: packet trace, length [%d](CRC not included)\n", length);
        }

        for(iter = 0; iter < length; iter++)
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
        }

        PRV_TGF_LOG0_MAC("\n\n");
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficTraceFieldInPartInPacketSet
*
* DESCRIPTION:
*       Open/Close the tracing for a field
*
* INPUTS:
*       packetPartType - the part that represent the 'context' of field
*       trace          - trace enable/disable
*       fieldNum       - field's number in structere
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong packetPartType parameter.
*       GT_OUT_OF_RANGE - on out of range fieldNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTraceFieldInPartInPacketSet
(
    IN TGF_PACKET_PART_ENT   packetPartType,
    IN GT_BOOL               trace,
    IN GT_U32                fieldNum
)
{
    switch(packetPartType)
    {
        case TGF_PACKET_PART_L2_E:
        case TGF_PACKET_PART_VLAN_TAG_E:
        case TGF_PACKET_PART_DSA_TAG_E:
        case TGF_PACKET_PART_ETHERTYPE_E:
        case TGF_PACKET_PART_MPLS_E:
        case TGF_PACKET_PART_IPV4_E:
        case TGF_PACKET_PART_IPV6_E:
        case TGF_PACKET_PART_TCP_E:
        case TGF_PACKET_PART_UDP_E:
        case TGF_PACKET_PART_ARP_E:
        case TGF_PACKET_PART_ICMP_E:
        case TGF_PACKET_PART_PAYLOAD_E:
        case TGF_PACKET_PART_WILDCARD_E:
        case TGF_PACKET_PART_TUNNEL_HEADER_FORCE_TILL_PASSENGER_E:
        case TGF_PACKET_PART_GRE_E:
        case TGF_PACKET_PART_CRC_E:
        case TGF_PACKET_PART_TRILL_E:
        case TGF_PACKET_PART_TRILL_FIRST_OPTION_E:
        case TGF_PACKET_PART_TRILL_GENERAL_OPTION_E:

            break;
        case TGF_PACKET_PART_SKIP_E:
            return GT_OK;/* skip this part */
        default:
            return GT_BAD_PARAM;
    }

    if (fieldNum > prvTgfTraceArr[(GT_U32) packetPartType].fieldsArrLength)
    {
        return GT_OUT_OF_RANGE;
    }

    prvTgfTraceArr[(GT_U32) packetPartType].fieldsArr[fieldNum].doTrace = (GT_U8)(BOOL2BIT_MAC(trace));

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficTraceAllFieldsInPartSet
*
* DESCRIPTION:
*       Open/Close the tracing for all fields in specific packet
*
* INPUTS:
*       packetPartType - the part that represent the 'context' of field
*       trace          - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTraceAllFieldsInPartSet
(
    IN TGF_PACKET_PART_ENT   packetPartType,
    IN GT_BOOL               trace
)
{
    GT_U32  fieldIndex = 0;

    switch(packetPartType)
    {
        case TGF_PACKET_PART_L2_E:
        case TGF_PACKET_PART_VLAN_TAG_E:
        case TGF_PACKET_PART_DSA_TAG_E:
        case TGF_PACKET_PART_ETHERTYPE_E:
        case TGF_PACKET_PART_MPLS_E:
        case TGF_PACKET_PART_IPV4_E:
        case TGF_PACKET_PART_IPV6_E:
        case TGF_PACKET_PART_TCP_E:
        case TGF_PACKET_PART_UDP_E:
        case TGF_PACKET_PART_ARP_E:
        case TGF_PACKET_PART_ICMP_E:
        case TGF_PACKET_PART_PAYLOAD_E:
        case TGF_PACKET_PART_WILDCARD_E:
        case TGF_PACKET_PART_TUNNEL_HEADER_FORCE_TILL_PASSENGER_E:
        case TGF_PACKET_PART_GRE_E:
        case TGF_PACKET_PART_CRC_E:
        case TGF_PACKET_PART_TRILL_E:
        case TGF_PACKET_PART_TRILL_FIRST_OPTION_E:
        case TGF_PACKET_PART_TRILL_GENERAL_OPTION_E:
            break;
        case TGF_PACKET_PART_SKIP_E:
            return GT_OK;/* skip this part */

        default:
            return GT_BAD_PARAM;
    }

    for (fieldIndex = 0; fieldIndex < prvTgfTraceArr[(GT_U32) packetPartType].fieldsArrLength; fieldIndex++)
    {
        if(GT_OK != tgfTrafficTraceFieldInPartInPacketSet(packetPartType, trace, fieldIndex))
        {
            return GT_FAIL;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficTraceAllFieldsInPartsInPacketSet
*
* DESCRIPTION:
*       Open/Close the tracing for all fields in packet (all parts)
*
* INPUTS:
*       trace - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTraceAllFieldsInPartsInPacketSet
(
    IN GT_BOOL          trace
)
{
    GT_U32  packetPartType = 0;

    for(packetPartType = 0; packetPartType < TGF_PACKET_PART_MAX_E; packetPartType++)
    {
        if(GT_OK != tgfTrafficTraceAllFieldsInPartSet(packetPartType, trace))
        {
            return GT_FAIL;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficTracePacketByteSet
*
* DESCRIPTION:
*       Open/Close the tracing on bytes of packets
*
* INPUTS:
*       trace - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTracePacketByteSet
(
    IN GT_BOOL          trace
)
{
    prvTgfTraceGlobalBmp = trace ? prvTgfTraceGlobalBmp | TGF_TRACE_BYTES_FLAG_CNS
                                 : prvTgfTraceGlobalBmp & ~TGF_TRACE_BYTES_FLAG_CNS;
    return GT_OK;
}

/*******************************************************************************
* tgfTrafficTraceInit
*
* DESCRIPTION:
*       Initialize the DB of the trace utility
*
* INPUTS:
*       None
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK         - on success.
*       GT_INIT_ERROR - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficTraceInit
(
    GT_VOID
)
{
    GT_U32      packetIter  = 0;
    GT_U32      fieldsNum   = 0;
    GT_U32      fieldOffset = 0;


    /* trace init for L2 part */
    packetIter = TGF_PACKET_PART_L2_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_L2_STC, daMac, fieldOffset, fieldsNum,
                            packetIter, TGF_TRACE_MAC_ADDR_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_L2_STC, saMac, fieldOffset, fieldsNum,
                            packetIter, TGF_TRACE_MAC_ADDR_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for VLAN tag part */
    packetIter = TGF_PACKET_PART_VLAN_TAG_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_VLAN_TAG_STC, etherType, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_HEX_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_VLAN_TAG_STC, pri, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_VLAN_TAG_STC, cfi, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_VLAN_TAG_STC, vid, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_HEX_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for DSA tag part : TBD - not implemented */
    packetIter = TGF_PACKET_PART_DSA_TAG_E;
    fieldsNum  = 0;
    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for Ethertype part */
    packetIter = TGF_PACKET_PART_ETHERTYPE_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ETHERTYPE_STC, etherType, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_HEX_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for MPLS part */
    packetIter = TGF_PACKET_PART_MPLS_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_MPLS_STC, timeToLive, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_MPLS_STC, stack, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_MPLS_STC, exp, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_MPLS_STC, label, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for IPv4 part */
    packetIter = TGF_PACKET_PART_IPV4_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, version, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, headerLen, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, typeOfService, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, totalLen, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, id, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, flags, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, offset, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, timeToLive, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, protocol, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, csum, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, srcAddr, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_IPV4_ADDR_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV4_STC, dstAddr, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_IPV4_ADDR_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for IPv6 part */
    packetIter = TGF_PACKET_PART_IPV6_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV6_STC, version, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV6_STC, trafficClass, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV6_STC, flowLabel, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV6_STC, payloadLen, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV6_STC, nextHeader, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV6_STC, hopLimit, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV6_STC, srcAddr, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_IPV4_ADDR_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_IPV6_STC, dstAddr, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_IPV4_ADDR_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for TCP part */
    packetIter = TGF_PACKET_PART_TCP_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, srcPort, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, dstPort, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, sequenceNum, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, acknowledgeNum, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, dataOffset, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, reserved, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, flags, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, windowSize, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, csum, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_TCP_STC, urgentPtr, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for UDP part */
    packetIter = TGF_PACKET_PART_UDP_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_UDP_STC, srcPort, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_UDP_STC, dstPort, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_UDP_STC, length, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_UDP_STC, csum, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for ARP part */
    packetIter = TGF_PACKET_PART_ARP_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ARP_STC, hwType, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ARP_STC, protType, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ARP_STC, hwLen, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ARP_STC, protLen, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ARP_STC, opCode, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ARP_STC, srcMac, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_MAC_ADDR_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ARP_STC, srcIp, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_IPV4_ADDR_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ARP_STC, dstMac, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_MAC_ADDR_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ARP_STC, dstIp, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_IPV4_ADDR_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for ICMP part */
    packetIter = TGF_PACKET_PART_ICMP_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ICMP_STC, typeOfService, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ICMP_STC, code, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ICMP_STC, csum, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ICMP_STC, id, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_ICMP_STC, sequenceNum, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    /* trace init for PTP_V2 part */
    packetIter = TGF_PACKET_PART_PTP_V2_E;
    fieldsNum  = 0;

    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_PTP_V2_STC, messageType, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_PTP_V2_STC, transportSpecific, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_PTP_V2_STC, ptpVersion, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_PTP_V2_STC, messageLength, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_PTP_V2_STC, domainNumber, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_DEC_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_PTP_V2_STC, flagField, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_HEX_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_PTP_V2_STC, correctionField[0], fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_HEX_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_PTP_V2_STC, correctionField[1], fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_HEX_E);
    TGF_TRACE_DB_FIELDS_MAC(TGF_PACKET_PTP_V2_STC, reserved32, fieldOffset,
                            fieldsNum, packetIter, TGF_TRACE_NUMERIC_HEX_E);

    prvTgfTraceArr[packetIter].fieldsArrLength = fieldsNum;

    return GT_OK;
}

/*******************************************************************************
* tgfTrafficEnginePacketLengthCalc
*
* DESCRIPTION:
*       Calculate length of packet by summary the length of the parts and ignoring
*       packetInfoPtr->totalLen
*       (the function will automatically ADD 4 bytes for CRC)
*
* INPUTS:
*       packetInfoPtr - packet fields info (the function will automatically ADD 4 bytes for CRC)
*
* OUTPUTS:
*       lengthPtr      - (pointer to) the calculated length of the packet.
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong parameter.
*       GT_BAD_PTR   - on NULL pointer
*       GT_BAD_STATE - parser caused bad state that should not happen
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfTrafficEnginePacketLengthCalc
(
    IN  TGF_PACKET_STC    *packetInfoPtr,
    OUT GT_U32            *lengthPtr
)
{
    GT_STATUS   rc;
    GT_U32      partIter;
    static      GT_U8 dummyBuffer[128];/* dummy buffer ! nothing is going to be written to it */

    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);

    /* the send of packet send with extra 4 bytes for CRC */
    *lengthPtr = TGF_CRC_LEN_CNS;

    /* loop on all parts and add each part after the other */
    for(partIter = 0; partIter < packetInfoPtr->numOfParts; partIter++)
    {
        rc = prvTgfBuildPacketPart(&packetInfoPtr->partsArray[partIter],
                                   lengthPtr,
                                   0,
                                   &dummyBuffer[0],
                                   NULL,
                                   NULL);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;

}


