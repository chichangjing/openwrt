/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTrafficParser.c
*
* DESCRIPTION:
*
* FILE REVISION NUMBER:
*       $Revision: 2.*
*******************************************************************************/
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <extUtils/trafficEngine/tgfTrafficEngine.h>
#include <extUtils/trafficEngine/private/prvTgfTrafficParser.h>
#include <extUtils/trafficEngine/tgfTrafficTable.h>

/******************************************************************************\
 *                              Define section                                *
\******************************************************************************/
#define TGF_MAX_PAYLOAD_SIZE_CNS  TGF_RX_BUFFER_MAX_SIZE_CNS /* support jumbo */


/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

/*******************************************************************************
* prvTgfTrafficParseL2Part
*
* DESCRIPTION:
*       Parse buffer to into L2 packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
*
* COMMENTS:
*         0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
*       |            DA         |           SA          |EthType| DATA...
*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficParseL2Part
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_L2_STC l2Part;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (TGF_L2_HEADER_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* fill L2 stc */
    cpssOsMemCpy(l2Part.daMac, bufferPtr, sizeof(TGF_MAC_ADDR));
    cpssOsMemCpy(l2Part.saMac, bufferPtr + sizeof(TGF_MAC_ADDR), sizeof(TGF_MAC_ADDR));

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &l2Part;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_L2_E;
    packetInfoPtr->totalLen += TGF_L2_HEADER_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseEtherTypePart
*
* DESCRIPTION:
*       Parse buffer to into EtherType packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
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
static GT_STATUS prvTgfTrafficParseEtherTypePart
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_ETHERTYPE_STC etherPart;
    GT_U8 *partPtr = bufferPtr + packetInfoPtr->totalLen;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (TGF_ETHERTYPE_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* fill EtherType stc */
    etherPart.etherType = (GT_U16) ((partPtr[0] << 8) | partPtr[1]);

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &etherPart;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_ETHERTYPE_E;
    packetInfoPtr->totalLen += TGF_ETHERTYPE_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseVlanTagPart
*
* DESCRIPTION:
*       Parse buffer to into VLAN Tag packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
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
static GT_STATUS prvTgfTrafficParseVlanTagPart
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_VLAN_TAG_STC vlanTagPart;
    GT_U8 *partPtr = bufferPtr + packetInfoPtr->totalLen;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (TGF_VLAN_TAG_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* fill VALN Tag stc */
    vlanTagPart.etherType = (TGF_ETHER_TYPE) ((partPtr[0] << 8) | partPtr[1]);
    vlanTagPart.pri = (TGF_PRI) ((partPtr[2] & 0xE0) >> 5);
    vlanTagPart.cfi = (TGF_CFI) ((partPtr[2] & 0x10) >> 4);
    vlanTagPart.vid = (TGF_VLAN_ID) (((partPtr[2] & 0x0F) << 8) | partPtr[3]);

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &vlanTagPart;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_VLAN_TAG_E;
    packetInfoPtr->totalLen += TGF_VLAN_TAG_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseMplsPart
*
* DESCRIPTION:
*       Parse buffer to into MPLS packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
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
static GT_STATUS prvTgfTrafficParseMplsPart
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_MPLS_STC mplsPart;
    GT_U8 *partPtr = bufferPtr + packetInfoPtr->totalLen;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (TGF_MPLS_HEADER_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* fill MPLS stc */
    mplsPart.label = (TGF_MPLS_LBL) (partPtr[0] << 12) | (partPtr[1] << 4 | ((partPtr[3] & 0xF0) >> 4));
    mplsPart.exp = (TGF_MPLS_STACK) ((partPtr[3] & 0x0E) >> 1);
    mplsPart.stack = (TGF_MPLS_STACK) (partPtr[3] & 0x01);
    mplsPart.timeToLive = (TGF_TTL) partPtr[4];

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &mplsPart;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_MPLS_E;
    packetInfoPtr->totalLen += TGF_MPLS_HEADER_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseArpPart
*
* DESCRIPTION:
*       Parse buffer to into ARP packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
*
* COMMENTS:
*    Ethernet transmission layer (not necessarily accessible to the user):
*        48.bit: Ethernet address of destination
*        48.bit: Ethernet address of sender
*        16.bit: Protocol type = ether_type$ADDRESS_RESOLUTION (0806)
*
*    Ethernet packet data:
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
*    Ethernet MAC address & IPV6 addres: n=6 bytes, m=16 bytes
*       0                   1                   2                   3
*       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |              HW Type          |    Protocol (IPv6= 0x86DD)    |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |   HW Len = 6  | Proto Len = 16|            Opcode             |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       | 6 bytes:                  MAC Src Addr                        |
*       +                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |                               | 16 bytes:                     |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
*       |                                                               |
*       +                                                               +
*       |                          IPv6 Src Addr                        |
*       +                                                               +
*       |                                                               |
*       +                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |                               | 6 bytes:                      |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
*       |                           MAC Dst Addr                        |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       | 16 bytes:                                                     |
*       +                                                               +
*       |                          IPv6 Dst Addr                        |
*       +                                                               +
*       |                                                               |
*       +                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*       |                               |
*       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficParseArpPart
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    GT_U8        *partPtr  = bufferPtr + packetInfoPtr->totalLen;
    GT_U32        offset   = 8;
    TGF_PROT_TYPE protType = 0;
    GT_VOID      *arpPartPtr;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* get protocol type */
    protType = (TGF_PROT_TYPE) ((partPtr[2] << 8) | partPtr[3]);

    /* set variable params */
    switch (protType)
    {
        case TGF_ETHERTYPE_0800_IPV4_TAG_CNS:
        {
            static TGF_PACKET_ARP_STC arpPart;

            /* check buffer size */
            if (TGF_ARP_HEADER_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
            {
                return GT_BAD_SIZE;
            }

            /* fill ARP stc */
            arpPart.hwType = (TGF_ARP_HW_TYPE) ((partPtr[0] << 8) | partPtr[1]);
            arpPart.protType = (TGF_PROT_TYPE) ((partPtr[2] << 8) | partPtr[3]);
            arpPart.hwLen = (GT_U8) partPtr[4];
            arpPart.protLen = (GT_U8) partPtr[5];
            arpPart.opCode = (GT_U16) ((partPtr[6] << 8) | partPtr[7]);

            cpssOsMemCpy(arpPart.srcMac, &partPtr[offset], sizeof(arpPart.srcMac));
            offset += sizeof(arpPart.srcMac);
            cpssOsMemCpy(arpPart.srcIp,  &partPtr[offset], sizeof(arpPart.srcIp));
            offset += sizeof(arpPart.srcIp);
            cpssOsMemCpy(arpPart.dstMac, &partPtr[offset], sizeof(arpPart.dstMac));
            offset += sizeof(arpPart.dstMac);
            cpssOsMemCpy(arpPart.dstIp,  &partPtr[offset], sizeof(arpPart.srcIp));

            arpPartPtr = &arpPart;

            break;
        }

        case TGF_ETHERTYPE_86DD_IPV6_TAG_CNS:
        {
            static TGF_PACKET_ARPV6_STC arpV6Part;

            /* check buffer size */
            if (TGF_ARPV6_HEADER_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
            {
                return GT_BAD_SIZE;
            }

            /* fill ARP stc */
            arpV6Part.hwType = (TGF_ARP_HW_TYPE) ((partPtr[0] << 8) | partPtr[1]);
            arpV6Part.protType = (TGF_PROT_TYPE) ((partPtr[2] << 8) | partPtr[3]);
            arpV6Part.hwLen = (GT_U8) partPtr[4];
            arpV6Part.protLen = (GT_U8) partPtr[5];
            arpV6Part.opCode = (GT_U16) ((partPtr[6] << 8) | partPtr[7]);

            cpssOsMemCpy(arpV6Part.srcMac, &partPtr[offset], sizeof(arpV6Part.srcMac));
            offset += sizeof(arpV6Part.srcMac);
            cpssOsMemCpy(arpV6Part.srcIp,  &partPtr[offset], sizeof(arpV6Part.srcIp));
            offset += sizeof(arpV6Part.srcIp);
            cpssOsMemCpy(arpV6Part.dstMac, &partPtr[offset], sizeof(arpV6Part.dstMac));
            offset += sizeof(arpV6Part.dstMac);
            cpssOsMemCpy(arpV6Part.dstIp,  &partPtr[offset], sizeof(arpV6Part.srcIp));

            arpPartPtr = &arpV6Part;

            break;
        }

        default:
            /* get unsupported ether type */
            return GT_NOT_SUPPORTED;
    }

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = arpPartPtr;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_ARP_E;
    packetInfoPtr->totalLen += (TGF_ETHERTYPE_0800_IPV4_TAG_CNS == protType) ? TGF_ARP_HEADER_SIZE_CNS
                                                                             : TGF_ARPV6_HEADER_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseIpv4Part
*
* DESCRIPTION:
*       Parse buffer to into IPv4 packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
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
static GT_STATUS prvTgfTrafficParseIpv4Part
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_IPV4_STC ipv4Part;
    GT_U8 *partPtr = bufferPtr + packetInfoPtr->totalLen;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (TGF_IPV4_HEADER_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* fill IPv4 stc */
    ipv4Part.version = (TGF_PROT_VER) (partPtr[0] >> 4);
    ipv4Part.headerLen = (GT_U8) (partPtr[0] & 0x0F);
    ipv4Part.typeOfService = (TGF_TYPE_OF_SERVICE) partPtr[1];
    ipv4Part.totalLen = (GT_U16) ((partPtr[2] << 8) | partPtr[3]);
    ipv4Part.id = (TGF_IPV4_ID_FIELD) ((partPtr[4] << 8) | partPtr[5]);
    ipv4Part.flags = (TGF_FLAG) (partPtr[6] >> 5);
    ipv4Part.offset = (GT_U16) (((partPtr[6] & 0x1F) << 8) | partPtr[7]);
    ipv4Part.timeToLive = (TGF_TTL) partPtr[8];
    ipv4Part.protocol = (TGF_PROT) partPtr[9];
    ipv4Part.csum = (TGF_HEADER_CRC) ((partPtr[10] << 8) | partPtr[11]);

    cpssOsMemCpy(ipv4Part.srcAddr, &partPtr[12], sizeof(ipv4Part.srcAddr));
    cpssOsMemCpy(ipv4Part.dstAddr,
                 &partPtr[12] + sizeof(ipv4Part.srcAddr),
                 sizeof(ipv4Part.dstAddr));

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &ipv4Part;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_IPV4_E;
    packetInfoPtr->totalLen += TGF_IPV4_HEADER_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseIpv6Part
*
* DESCRIPTION:
*       Parse buffer to into IPv6 packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
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
static GT_STATUS prvTgfTrafficParseIpv6Part
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_IPV6_STC ipv6Part;
    GT_U8 *partPtr = bufferPtr + packetInfoPtr->totalLen;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (TGF_IPV6_HEADER_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* fill IPv6 stc */
    ipv6Part.version = (TGF_PROT_VER) (partPtr[0] >> 4);
    ipv6Part.trafficClass = (TGF_TRAFFIC_CLASS) ((partPtr[0] << 4) | (partPtr[1] >> 4));
    ipv6Part.flowLabel = (TGF_FLOW_LABEL) (((partPtr[1] & 0x0F) << 16) | (partPtr[2] <<  8) | partPtr[3]);
    ipv6Part.payloadLen = (GT_U16) ((partPtr[4] << 8) | partPtr[5]);
    ipv6Part.nextHeader = (TGF_NEXT_HEADER) (partPtr[6]);
    ipv6Part.hopLimit = (TGF_HOP_LIMIT) (partPtr[7]);

    cpssOsMemCpy(ipv6Part.srcAddr, &bufferPtr[8], sizeof(ipv6Part.srcAddr));
    cpssOsMemCpy(ipv6Part.dstAddr,
                 &bufferPtr[8] + sizeof(ipv6Part.srcAddr),
                 sizeof(ipv6Part.dstAddr));

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &ipv6Part;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_IPV6_E;
    packetInfoPtr->totalLen += TGF_IPV6_HEADER_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseTcpPart
*
* DESCRIPTION:
*       Parse buffer to into TCP packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
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
static GT_STATUS prvTgfTrafficParseTcpPart
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_TCP_STC tcpPart;
    GT_U8 *partPtr = bufferPtr + packetInfoPtr->totalLen;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (TGF_TCP_HEADER_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* fill TCP stc */
    tcpPart.srcPort = (TGF_L4_PORT) ((partPtr[0] << 8) | partPtr[1]);
    tcpPart.dstPort = (TGF_L4_PORT) ((partPtr[2] << 8) | partPtr[3]);
    tcpPart.sequenceNum = (GT_U32) ((partPtr[4] << 24) | (partPtr[6] << 8) |
                                    (partPtr[5] << 16) |  partPtr[7]);
    tcpPart.acknowledgeNum = (GT_U32) ((partPtr[8] << 24) | (partPtr[10] << 8) |
                                       (partPtr[9] << 16) |  partPtr[11]);
    tcpPart.dataOffset = (GT_U8) ((partPtr[12] & 0xF0) >> 4);
    tcpPart.reserved = (GT_U8) (((partPtr[12] & 0x0F) << 2) | ((partPtr[13] & 0x0C) >> 6));
    tcpPart.flags = (GT_U8) (partPtr[13] & 0x3F);
    tcpPart.windowSize = (GT_U16) ((partPtr[14] << 8) | partPtr[15]);
    tcpPart.csum = (TGF_HEADER_CRC) ((partPtr[16] << 8) | partPtr[17]);
    tcpPart.urgentPtr = (TGF_TCP_URGENT_PTR) ((partPtr[18] << 8) | partPtr[19]);

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &tcpPart;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_TCP_E;
    packetInfoPtr->totalLen += TGF_TCP_HEADER_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseUdpPart
*
* DESCRIPTION:
*       Parse buffer to into UDP packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
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
static GT_STATUS prvTgfTrafficParseUdpPart
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_UDP_STC udpPart;
    GT_U8 *partPtr = bufferPtr + packetInfoPtr->totalLen;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (TGF_UDP_HEADER_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* fill UDP stc */
    udpPart.srcPort = (TGF_L4_PORT) ((partPtr[0] << 8) | partPtr[1]);
    udpPart.dstPort = (TGF_L4_PORT) ((partPtr[2] << 8) | partPtr[3]);
    udpPart.length = (GT_U16) ((partPtr[4] << 8) | partPtr[5]);
    udpPart.csum = (TGF_HEADER_CRC) ((partPtr[6] << 8) | partPtr[7]);

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &udpPart;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_UDP_E;
    packetInfoPtr->totalLen += TGF_UDP_HEADER_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseIcmpPart
*
* DESCRIPTION:
*       Parse buffer to into ICMP packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_SIZE - on wrong size.
*       GT_BAD_PTR  - on NULL pointer
*
* COMMENTS:
*       Type:
*        0  Echo Reply Message
*        8  Echo Message
*       15  Information Request Message
*       16  Information Reply Message
*           0                   1                   2                   3
*           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |     Type      |     Code      |          Checksum             |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |           Identifier          |        Sequence Number        |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |     Data ...
*          +-+-+-+-+-
*
*        3  Destination Unreachable Message
*       11  Time Exceeded Message
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |     Type      |     Code      |          Checksum             |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |                             unused                            |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |      Internet Header + 64 bits of Original Data Datagram      |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*        4  Source Quench Message
*       12  Parameter Problem Message
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |     Type      |     Code      |          Checksum             |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |    Pointer    |                   unused                      |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |      Internet Header + 64 bits of Original Data Datagram      |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*        5  Redirect Message
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |     Type      |     Code      |          Checksum             |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |                 Gateway Internet Address                      |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |      Internet Header + 64 bits of Original Data Datagram      |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*       13  Timestamp Message
*       14  Timestamp Reply Message
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |     Type      |      Code     |          Checksum             |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |           Identifier          |        Sequence Number        |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |     Originate Timestamp                                       |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |     Receive Timestamp                                         |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*          |     Transmit Timestamp                                        |
*          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficParseIcmpPart
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_ICMP_STC icmpPart;
    GT_U8 *partPtr = bufferPtr + packetInfoPtr->totalLen;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (TGF_UDP_HEADER_SIZE_CNS > bufferLength - packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* fill ICMP stc */
    icmpPart.typeOfService = (TGF_TYPE_OF_SERVICE) partPtr[0];
    icmpPart.code = (GT_U8) partPtr[1];
    icmpPart.csum = (TGF_HEADER_CRC) ((partPtr[2] << 8) | partPtr[3]);

    switch (icmpPart.typeOfService) {
        case TGF_ICMP_TYPE_ECHO_REPLY_E:              /*  0 */
        case TGF_ICMP_TYPE_ECHO_E:                    /*  8 */
        case TGF_ICMP_TYPE_INFORMATION_E:             /* 15 */
        case TGF_ICMP_TYPE_INFORMATION_REPLY_E:       /* 16 */
        case TGF_ICMP_TYPE_TIMESTAMP_E:               /* 13 */
        case TGF_ICMP_TYPE_TIMESTAMP_REPLY_E:         /* 14 */
            icmpPart.id = (TGF_ICMP_ID_FIELD) ((partPtr[4] << 8) | partPtr[5]);
            icmpPart.sequenceNum = (GT_U16) ((partPtr[6] << 8) | partPtr[7]);
            icmpPart.getwayAddr = 0;
            icmpPart.errorPointer = 0;

            break;

        case TGF_ICMP_TYPE_SOURCE_QUENCH_E:           /*  4 */
        case TGF_ICMP_TYPE_PARAMETER_PROBLEM_E:       /* 12 */
            icmpPart.id = 0;
            icmpPart.sequenceNum = 0;
            icmpPart.getwayAddr = 0;
            icmpPart.errorPointer = (GT_U8) partPtr[4];

            break;

        case TGF_ICMP_TYPE_REDIRECT_E:                /*  5 */
            icmpPart.id = 0;
            icmpPart.sequenceNum = 0;
            icmpPart.getwayAddr = (GT_U32) ((partPtr[4] << 24) | (partPtr[5] << 16) |
                                            (partPtr[6] <<  8) |  partPtr[7]);
            icmpPart.errorPointer = 0;

            break;

        case TGF_ICMP_TYPE_DESTINATION_UNREACHABLE_E: /*  3 */
        case TGF_ICMP_TYPE_TIME_EXCEEDED_E:           /* 11 */
        default:
            icmpPart.id = 0;
            icmpPart.sequenceNum = 0;
            icmpPart.getwayAddr = 0;
            icmpPart.errorPointer = 0;
    }

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &icmpPart;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_ICMP_E;
    packetInfoPtr->totalLen += TGF_ICMP_HEADER_SIZE_CNS;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParseWildcardPart
*
* DESCRIPTION:
*       Parse buffer to into Wildcard packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer
*       GT_NO_RESOURCE  - on ERROR of memory allocation
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficParseWildcardPart
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_WILDCARD_STC wildcardPart;
    static GT_U8 dataPtr[TGF_MAX_PAYLOAD_SIZE_CNS];

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (bufferLength == 0 || packetInfoPtr->totalLen != 0)
    {
        return GT_BAD_SIZE;
    }

    /* set parameters for the wildcardPart */
    wildcardPart.numOfBytes = bufferLength;
    wildcardPart.bytesPtr   = dataPtr;

    /* copy bytes from the buffer to the wildcardPart */
    cpssOsMemCpy(dataPtr, bufferPtr, bufferLength);

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &wildcardPart;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_WILDCARD_E;
    packetInfoPtr->totalLen = bufferLength;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrafficParsePayloadPart
*
* DESCRIPTION:
*       Parse buffer to into Payload packet part
*
* INPUTS:
*       bufferPtr     - (pointer to) the buffer that hold the packet bytes
*       bufferLength  - the number of bytes in the buffer (not include CRC bytes)
*       packetInfoPtr - (pointer to) packet fields info
*
* OUTPUTS:
*       packetInfoPtr - (pointer to) packet fields info
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer
*       GT_NO_RESOURCE  - on ERROR of memory allocation
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfTrafficParsePayloadPart
(
    IN    GT_U8             *bufferPtr,
    IN    GT_U32             bufferLength,
    INOUT TGF_PACKET_STC    *packetInfoPtr
)
{
    static TGF_PACKET_PAYLOAD_STC payloadPart;
    static GT_U8 dataPtr[TGF_MAX_PAYLOAD_SIZE_CNS];

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr->partsArray);

    /* check buffer size */
    if (bufferLength < packetInfoPtr->totalLen)
    {
        return GT_BAD_SIZE;
    }

    /* set parameters for the payloadPart */
    payloadPart.dataLength  = bufferLength - packetInfoPtr->totalLen;
    payloadPart.dataPtr     = dataPtr;

    /* copy bytes from the buffer to the payloadPart */
    cpssOsMemCpy(payloadPart.dataPtr,
                 &bufferPtr[packetInfoPtr->totalLen],
                 payloadPart.dataLength);

    /* set parameters for the next part */
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].partPtr = &payloadPart;
    packetInfoPtr->partsArray[packetInfoPtr->numOfParts].type = TGF_PACKET_PART_PAYLOAD_E;
    packetInfoPtr->totalLen += payloadPart.dataLength;
    packetInfoPtr->numOfParts++;

    return GT_OK;
}

/******************************************************************************\
 *                              API implementation                            *
\******************************************************************************/

/*******************************************************************************
* prvTgfTrafficEnginePacketParse
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
GT_STATUS prvTgfTrafficEnginePacketParse
(
    IN  GT_U8              *bufferPtr,
    IN  GT_U32              bufferLength,
    OUT TGF_PACKET_STC    **packetInfoPtrPtr
)
{
    TGF_ETHER_TYPE              etherType  = 0;
    TGF_PROTOCOL_ENT            nextHeader = TGF_PROTOCOL_MAX_E;
    GT_STATUS                   rc         = GT_OK;
    static TGF_PACKET_STC       packetInfo;
    static TGF_PACKET_PART_STC  partsArray[TGF_PACKET_PART_MAX_E];

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtrPtr);

    /* Initialize of packetInfo and packetInfo.partsArray */
    cpssOsMemSet(partsArray, 0, sizeof(partsArray));
    *packetInfoPtrPtr = &packetInfo;
    packetInfo.totalLen = 0;
    packetInfo.numOfParts = 0;
    packetInfo.partsArray = partsArray;

    /* ============ It is only copy of buffer for small data ================ */
    if (bufferLength < TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS)
    {
        /* copy the all packet from buff to the wilcard pointer */
        rc = prvTgfTrafficParseWildcardPart(bufferPtr, bufferLength, &packetInfo);
        TGF_RC_CHECK_MAC(rc);

        return rc;
    }

    /* === PART 0 =========== L2 - SRC MAC & DST MAC ======================== */
    /* fill TGF_PACKET_L2_STC from buff*/
    rc = prvTgfTrafficParseL2Part(bufferPtr, bufferLength, &packetInfo);
    TGF_RC_CHECK_MAC(rc);

    /* === PART 1(2,...) ==== L2: VLAN, MPLS, EHERNET ... TAGs ============== */
    /* test - vlan or mpls tag before ethernet tag */
    do
    {
        etherType = (GT_U16) ((bufferPtr[packetInfo.totalLen] << 8) |
                              bufferPtr[packetInfo.totalLen + 1]);

        switch (etherType)
        {
            case TGF_ETHERTYPE_8100_VLAN_TAG_CNS:
                rc = prvTgfTrafficParseVlanTagPart(bufferPtr, bufferLength,
                                                   &packetInfo);
                break;

            case TGF_ETHERTYPE_8847_MPLS_TAG_CNS:
                rc = prvTgfTrafficParseMplsPart(bufferPtr, bufferLength,
                                                &packetInfo);
                break;

            default:
                rc = prvTgfTrafficParseEtherTypePart(bufferPtr, bufferLength,
                                                     &packetInfo);
        }

        TGF_RC_CHECK_MAC(rc);

    } while ((etherType == TGF_ETHERTYPE_8100_VLAN_TAG_CNS ||
              etherType == TGF_ETHERTYPE_8847_MPLS_TAG_CNS) &&
              packetInfo.totalLen < bufferLength);

    /* ================================ L3 ================================== */
    switch (etherType)
    {
        case TGF_ETHERTYPE_0800_IPV4_TAG_CNS:
        {
            TGF_PACKET_IPV4_STC *ipv4HeaderPtr;

            rc = prvTgfTrafficParseIpv4Part(bufferPtr, bufferLength, &packetInfo);
            TGF_RC_CHECK_MAC(rc);

            ipv4HeaderPtr = (TGF_PACKET_IPV4_STC*)
                packetInfo.partsArray[packetInfo.numOfParts - 1].partPtr;

            nextHeader = (TGF_PROTOCOL_ENT) ipv4HeaderPtr->protocol;
            break;
        }

        case TGF_ETHERTYPE_86DD_IPV6_TAG_CNS:
        {
            TGF_PACKET_IPV6_STC *ipv6HeaderPtr;

            rc = prvTgfTrafficParseIpv6Part(bufferPtr, bufferLength, &packetInfo);
            TGF_RC_CHECK_MAC(rc);

            ipv6HeaderPtr = (TGF_PACKET_IPV6_STC*)
                packetInfo.partsArray[packetInfo.numOfParts - 1].partPtr;

            nextHeader = (TGF_PROTOCOL_ENT) ipv6HeaderPtr->nextHeader;
            break;
        }

        case TGF_ETHERTYPE_0806_ARP_TAG_CNS:
            rc = prvTgfTrafficParseArpPart(bufferPtr, bufferLength, &packetInfo);
            TGF_RC_CHECK_MAC(rc);

            break;

        case TGF_ETHERTYPE_8902_OAM_TAG_CNS:
            /* Support as payload part */
            break;
        default:
            /* got an unsupported ether type */
            /* interpret as payload */
            packetInfo.numOfParts--;
            packetInfo.totalLen -= TGF_ETHERTYPE_SIZE_CNS;
            rc = prvTgfTrafficParsePayloadPart(bufferPtr, bufferLength, &packetInfo);
            return rc;
    }

    /* =============================== L4 =================================== */
    do
    {
        switch (nextHeader)
        {
            case TGF_PROTOCOL_HOPOPT_E:
                nextHeader = TGF_PROTOCOL_MAX_E;

                break;

            case TGF_PROTOCOL_ICMP_E:
                rc = prvTgfTrafficParseIcmpPart(bufferPtr, bufferLength, &packetInfo);
                TGF_RC_CHECK_MAC(rc);

                nextHeader = TGF_PROTOCOL_MAX_E;

                break;

            case TGF_PROTOCOL_TCP_E:
                rc = prvTgfTrafficParseTcpPart(bufferPtr, bufferLength, &packetInfo);
                TGF_RC_CHECK_MAC(rc);

                nextHeader = TGF_PROTOCOL_MAX_E;

                break;

            case TGF_PROTOCOL_UDP_E:
                rc = prvTgfTrafficParseUdpPart(bufferPtr, bufferLength, &packetInfo);
                TGF_RC_CHECK_MAC(rc);

                nextHeader = TGF_PROTOCOL_MAX_E;

                break;

            case TGF_PROTOCOL_ICMPV6_E:
                nextHeader = TGF_PROTOCOL_MAX_E;

                break;

            default:
                /* copy the remains of the packet from buff to static PayloadPart */
                rc = prvTgfTrafficParsePayloadPart(bufferPtr, bufferLength, &packetInfo);
                TGF_RC_CHECK_MAC(rc);
        }
    } while (packetInfo.totalLen < bufferLength);

    return rc;
}

