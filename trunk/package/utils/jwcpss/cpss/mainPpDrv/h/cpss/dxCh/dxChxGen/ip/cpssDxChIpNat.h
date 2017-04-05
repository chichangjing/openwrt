/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpNat.h
*
* DESCRIPTION:
*       The CPSS DXCH IP NAT structures and APIs
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#ifndef __cpssDxChIpNath
#define __cpssDxChIpNath

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>

/*
 * typedef: struct CPSS_DXCH_IP_NAT44_ENTRY_STC
 *
 * Description: Struct for NAT44 entry.
 *
 * Fields:
 *
 *  macDa               - The MAC DA of the passenger packet
 *  modifyDip           - Selector for modifying the packet Destination IP address 
 *                          GT_FALSE: Do not modify the Destination IP address of the packet
 *                          GT_TRUE:  Modify the Destination IP address of the packet
 *  newDip              - The new Destination IP address of the packet
 *  modifySip           - Selector for modifying the packet Source IP address 
 *                          GT_FALSE: Do not modify the Source IP address of the packet
 *                          GT_TRUE:  Modify the Source IP address of the packet
 *  newSip              - The new Source IP address of the packet
 *  modifyTcpUdpDstPort - Selector for modifying the packet TCP / UDP Destination port 
 *                          GT_FALSE: Do not modify the TCP / UDP Destination port of the packet
 *                          GT_TRUE:  Modify the TCP / UDP Destination port of the packet
 *  newTcpUdpDstPort    - The New TCP / UDP Destination Port of the packet
 *                        (APPLICABLE RANGES: 0..65535) 
 *  modifyTcpUdpSrcPort - Selector for modifying the packet TCP / UDP Source port 
 *                          GT_FALSE: Do not modify the TCP / UDP Source port of the packet
 *                          GT_TRUE:  Modify the TCP / UDP Source port of the packet
 *  newTcpUdpSrcPort    - The New TCP / UDP Source Port of the packet
 *                       (APPLICABLE RANGES: 0..65535)
 */
typedef struct
{
    GT_ETHERADDR    macDa;
    GT_BOOL         modifyDip;
    GT_IPADDR       newDip;
    GT_BOOL         modifySip;
    GT_IPADDR       newSip;
    GT_BOOL         modifyTcpUdpDstPort;    
    GT_U32          newTcpUdpDstPort;
    GT_BOOL         modifyTcpUdpSrcPort;    
    GT_U32          newTcpUdpSrcPort;   
} CPSS_DXCH_IP_NAT44_ENTRY_STC; 

/*
 * typedef: enum CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT
 *
 * Description: Enumeration of NAT Modify Command.
 *
 * Enumerations:
 *
 * CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E   -  Replace the SIP prefix with the NAT<Address>.                                                 
 * CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E   -  Replace the DIP prefix with the NAT<Address>.                                                  
 * CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_ADDRESS_E  -  Replace the SIP address with NAT<Address>; 
 * CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_ADDRESS_E  -  Replace the DIP address with NAT<Address>;
 *
 */
typedef enum {
    CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E,
    CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E,
    CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_ADDRESS_E,
    CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_ADDRESS_E
} CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT;

/*
 * typedef: struct CPSS_DXCH_IP_NAT66_ENTRY_STC
 *
 * Description: Struct for NAT66 entry.
 *
 * Fields:
 *
 *  macDa               - The MAC DA of the passenger packet
 *  modifyCommand       - Translation command
 *                        if command is CPSS_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E, CPSS_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E,
 *                        The prefix size is taken from the <Prefix size> field in this entry
 *                        if command is CPSS_IP_NAT_TYPE_NAT66_MODIFY_SIP_ADDRESS_E, CPSS_IP_NAT_TYPE_NAT66_MODIFY_DIP_ADDRESS_E,
 *                        The address is taken from the <address> field in this entry                      
 *  address             - New SIP/DIP address used to replace a packet’s SIP/DIP according to modifyCommand.
 *                        if command is CPSS_IP_NAT_TYPE_NAT66_MODIFY_SIP_ADDRESS_E, CPSS_IP_NAT_TYPE_NAT66_MODIFY_DIP_ADDRESS_E,
 *                        then this is the new SIP or DIP.
 *                        if command is CPSS_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E, CPSS_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E,
 *                        then replace the <Prefix size> MSB of the SIP or DIP with the prefix of this entry (the prefix is aligned to the left)                  
 *                        This means that the MSBs of the NAT Address will be assigned as the new packet Addr (SIP/DIP). 
 *  prefixSize          - Address prefix size for NAT modifyCommand == Modify SIP prefix / Modify DIP prefix (according to RFC6296).
 *                        (APPLICABLE RANGES: 1..64)
 */
typedef struct
{
    GT_ETHERADDR                            macDa;
    CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT   modifyCommand;
    GT_IPV6ADDR                             address;
    GT_U32                                  prefixSize;   
} CPSS_DXCH_IP_NAT66_ENTRY_STC;

/*
 * typedef: enum CPSS_IP_NAT_TYPE_ENT
 *
 * Description: Enumeration of NAT types.
 *
 * Enumerations:
 *
 *  CPSS_IP_NAT_TYPE_NAT44_E      - NAT44: translation of ipv4 address to ipv4 address
 *  CPSS_IP_NAT_TYPE_NAT66_E      - NAT44: translation of ipv6 address to ipv6 address
 *
 */
typedef enum {
    CPSS_IP_NAT_TYPE_NAT44_E,
    CPSS_IP_NAT_TYPE_NAT66_E,
    CPSS_IP_NAT_TYPE_LAST_E
} CPSS_IP_NAT_TYPE_ENT;

/*
 * typedef: union CPSS_DXCH_IP_NAT_ENTRY_UNT
 *
 * Description: Union for NAT entry
 *
 * Fields:
 *
 *  nat44Entry        - entry for NAT44
 *  nat66Entry        - entry for NAT66
 * 
 */
typedef union
{
    CPSS_DXCH_IP_NAT44_ENTRY_STC            nat44Entry;    
    CPSS_DXCH_IP_NAT66_ENTRY_STC            nat66Entry;  

} CPSS_DXCH_IP_NAT_ENTRY_UNT;

/*******************************************************************************
* cpssDxChIpNatEntrySet
*
* DESCRIPTION:
*       Set a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
*       natType       - type of the NAT
*       entryPtr      - (pointer to) NAT entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table, Tunnel start entries table and router ARP addresses 
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
* 
*
*******************************************************************************/
GT_STATUS cpssDxChIpNatEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              natIndex,
    IN  CPSS_IP_NAT_TYPE_ENT                natType,
    IN  CPSS_DXCH_IP_NAT_ENTRY_UNT          *entryPtr
);

/*******************************************************************************
* cpssDxChIpNatEntryGet
*
* DESCRIPTION:
*       Get a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
* OUTPUTS:
*       natTypePtr   - (pointer to) type of the NAT 
*       entryPtr     - (pointer to) NAT entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table, Tunnel start entries table and router ARP addresses 
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
* 
*******************************************************************************/
GT_STATUS cpssDxChIpNatEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             natIndex,  
    OUT  CPSS_IP_NAT_TYPE_ENT               *natTypePtr,
    OUT  CPSS_DXCH_IP_NAT_ENTRY_UNT         *entryPtr
);

/*******************************************************************************
* cpssDxChIpNatDroppedPacketsCntGet
*
* DESCRIPTION:
*      Get the counter that counts the packets that were dropped since NAT could not be applied.
*      If the L4 header is not within the header 64 bytes (For tunnel-terminated packet it must
*      be within the passenger header 64 bytes), the packet is dropped and it is counted by
*      the NAT Drop Counter.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       natDropPktsPtr - (pointer to) the number of dropped packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Clear on read.
*
*******************************************************************************/
GT_STATUS cpssDxChIpNatDroppedPacketsCntGet
(
    IN   GT_U8    devNum,
    OUT  GT_U32   *natDropPktsPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChIpNath */

