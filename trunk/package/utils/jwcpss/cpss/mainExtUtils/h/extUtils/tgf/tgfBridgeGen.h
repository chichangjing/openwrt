/*******************************************************************************
 *              (C), Copyright 2001, Marvell International Ltd.                 *
 * THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
 * NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
 * OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
 * DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
 * THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
 * IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
 ********************************************************************************
 * tgfBridgeGen.h
 *
 * DESCRIPTION:
*       Generic API for Bridge
 *
 *
 * FILE REVISION NUMBER:
 *       $Revision: 1 $
 *
 *******************************************************************************/
#ifndef __tgfBridgeGen
#define __tgfBridgeGen

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/bridge/cpssGenBrgGen.h>
#include <extUtils/tgf/tgfCommonList.h>
#include <extUtils/tgf/tgfCommon.h>

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
#endif  /* CHX_FAMILY */

/* indication that the FDB shadow used */
extern GT_BOOL prvTgfFdbShadowUsed;

/*
 * Typedef: enum PRV_TGF_FDB_ENTRY_TYPE_ENT
 *
 * Description:
 *      This enum defines the type of the FDB Entry
 * Fields:
 *     PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E   - The entry is MAC Address
 *     PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E - The entry is IPv4 Multicast
 *     PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E - The entry is IPv6 Multicast
 *     PRV_TGF_FDB_ENTRY_TYPE_VPLS_E       - The entry is VPLS entry
 *     PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E    - The entry is IPv4 UC entry
 *     PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E - The entry is IPv6 UC address entry
 *     PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E - The entry is IPv6 UC data entry
 *     PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_FID_VID1_E    - The entry is MAC Address
 *                                        Double Tag FDB Lookup Key Mode is used
 *     PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_FID_VID1_E  - The entry is IPv4 Multicast
 *                                        Double Tag FDB Lookup Key Mode is used
 *     PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_FID_VID1_E  - The entry is IPv6 Multicast
 *                                        Double Tag FDB Lookup Key Mode is used

 */
typedef enum
{
    PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E,
    PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E,
    PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E,
    PRV_TGF_FDB_ENTRY_TYPE_VPLS_E,
    PRV_TGF_FDB_ENTRY_TYPE_IPV4_UC_E,
    PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_ADDR_ENTRY_E,
    PRV_TGF_FDB_ENTRY_TYPE_IPV6_UC_DATA_ENTRY_E,
    PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_FID_VID1_E,
    PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_FID_VID1_E,
    PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_FID_VID1_E

} PRV_TGF_FDB_ENTRY_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_PACKET_CMD_ENT
 *
 * Description:
 *     This enum defines the packet command.
 *
 * Fields:
 *     PRV_TGF_PACKET_CMD_FORWARD_E           - forward packet
 *     PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E     - mirror packet to CPU
 *     PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E       - trap packet to CPU
 *     PRV_TGF_PACKET_CMD_DROP_HARD_E         - hard drop packet
 *     PRV_TGF_PACKET_CMD_ROUTE_E             - IP Forward the packets
 *     PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E  - Packet is routed and mirrored
 *     PRV_TGF_PACKET_CMD_DROP_SOFT_E         - soft drop packet
 *     PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E - Bridge and Mirror to CPU.
 *     PRV_TGF_PACKET_CMD_BRIDGE_E            - Bridge only
 *     PRV_TGF_PACKET_CMD_INTERV_E            - intervention to CPU
 *     PRV_TGF_PACKET_CMD_CNTL_E              - control (unconditionally trap to CPU)
 *     PRV_TGF_PACKET_CMD_NONE_E              - Do nothing. (disable)
 *
 */
typedef enum
{
    PRV_TGF_PACKET_CMD_FORWARD_E,
    PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E,
    PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E,
    PRV_TGF_PACKET_CMD_DROP_HARD_E,
    PRV_TGF_PACKET_CMD_DROP_SOFT_E,
    PRV_TGF_PACKET_CMD_ROUTE_E,
    PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E,
    PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E,
    PRV_TGF_PACKET_CMD_BRIDGE_E,
    PRV_TGF_PACKET_CMD_INTERV_E,
    PRV_TGF_PACKET_CMD_CNTL_E,
    PRV_TGF_PACKET_CMD_NONE_E
} PRV_TGF_PACKET_CMD_ENT;


/*
 * typedef: struct PRV_TGF_MAC_ENTRY_KEY_STC
 *
 * Description: Key parameters of the MAC Address Entry
 *
 * Fields:
 *  entryType   - the type of the entry MAC Addr or IP Multicast
 *  macAddr     - mac address (for <entry type> = MAC Address entry)
 *  vlanId      - Vlan Id (for both types of the entry) [0..0xFFF]
 *                for ExMxPm , Lion2B - used as "fId" (Forwarding Id) [0..0xFFFF]
 *
 *  dip         - For IPv4 entry type it's destination address
 *  sip         - For IPv4 entry type it's source address
 *
 * ipv4Unicast - IP Unicast entry information (for <entry type> = IPv4)
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *         dip          -   For IPv4 entry type it's destination address bytes in the
 *                          Network order. For example for IP address 1.2.3.4
 *                          ipv4Unicast.dip = {1,2,3,4}.
 *         vrfId        -   Thr VRF-ID is part of the lookup key
 * ipv6Unicast - IP Unicast entry information (for <entry type> = IPv6)
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *         dip          -   For IPv6 entry type it's destination address bytes in the
 *                          Network order. For example for IP address 1.2.3.5...16
 *                          ipv6Unicast.dip = {1,2,3,4, .... 16}.
 *         vrfId        -   Thr VRF-ID is part of the lookup key
 *
*/
typedef struct
{
    PRV_TGF_FDB_ENTRY_TYPE_ENT         entryType;
    GT_U32                             vid1;
    union  {
       struct {
           GT_ETHERADDR     macAddr;
           GT_U16           vlanId;
       } macVlan;

       struct {
           GT_U8            sip[4];
           GT_U8            dip[4];
           GT_U16           vlanId;
       } ipMcast;

       struct{
           GT_U8            dip[4];
           GT_U32           vrfId;
       }ipv4Unicast;

       struct{
           GT_U8            dip[16];
           GT_U32           vrfId;
       }ipv6Unicast;

    } key;

} PRV_TGF_MAC_ENTRY_KEY_STC;

/*
 * typedef: struct PRV_TGF_BRG_MAC_ENTRY_STC
 *
 * Description: FDB Entry
 *
 * Fields:
 *   key             - key data
 *   dstInterface    - interface info
 *   age             - Age flag that is used for the two-step Aging process.
 *                     GT_FALSE - The entry will be aged out in the next pass.
 *                     GT_TRUE - The entry will be aged-out in two age-passes.
 *                     Notes: used only for DxCh devices.
 *                            IP UC entries are not aged out, but only marked as such
 *   dstOutlif       - outlif info: port/trunk/vidx/dit
 *   isStatic        - static/dynamic entry
 *   daCommand       - action taken when a packet's DA matches this entry
 *   saCommand       - action taken when a packet's SA matches this entry
 *   daRoute         - send packet to the IPv4 or MPLS
 *   mirrorToRxAnalyzerPortEn - configurable analyzer port.
 *   sourceId        - Source ID
 *   userDefined     - user defined field
 *   daQosIndex      - destination Qos attribute
 *   saQosIndex      - source Qos attribute
 *   daSecurityLevel - security level assigned to the MAC DA
 *   saSecurityLevel - security level assigned to the MAC SA
 *   appSpecificCpuCode - specific CPU code
 *   pwId            - Pseudo-Wire ID
 *   spUnknown       - relevant when auto-learning is disabled
 *   vid1            - Tag1 VID value. (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 */
typedef struct
{
       PRV_TGF_MAC_ENTRY_KEY_STC    key;
       CPSS_INTERFACE_INFO_STC      dstInterface;
       GT_BOOL                      age;

       /* Fields relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST */
       PRV_TGF_OUTLIF_INFO_STC      dstOutlif;
       GT_BOOL                      isStatic;
       PRV_TGF_PACKET_CMD_ENT       daCommand;
       PRV_TGF_PACKET_CMD_ENT       saCommand;
       GT_BOOL                      daRoute;
       GT_BOOL                      mirrorToRxAnalyzerPortEn;
       GT_U32                       userDefined;
       GT_U32                       daQosIndex;
       GT_U32                       saQosIndex;
       GT_U32                       daSecurityLevel;
       GT_U32                       saSecurityLevel;
       GT_BOOL                      appSpecificCpuCode;
       GT_U32                       pwId;
       GT_BOOL                      spUnknown;
       GT_U32                       sourceId;

       /* Relevant only for key of type IPV4_UC/IPV6_UC */

       CPSS_FDB_UC_ROUTING_INFO_STC    fdbRoutingInfo;
} PRV_TGF_BRG_MAC_ENTRY_STC;


/*
 * typedef: struct PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART1_STC
 *
 * Description: An item of linked list used to store FDB AU messages
 *
 * Fields:
 *   item            - a basic linked list struct. Should be always a first
 *                     struct's field to make it possible a using of
 *                     this structure as descendant of PRV_TGF_LIST_ITEM_STC
 *   devNum          - a device number, generated the AU message
 *   message         - an FDB AU message
 *
 */
typedef struct {
    PRV_TGF_LIST_ITEM_STC       item;
    GT_U8                       devNum;
    CPSS_MAC_UPDATE_MSG_EXT_STC message;
}PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART1_STC;

extern PRV_TGF_LIST_HEAD_STC brgFdbAuqMessagesPart1;

#ifdef CHX_FAMILY

extern GT_U32 prvTgfBrgFdbDoNaShadow_part2;

/*
 * typedef: struct PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC
 *
 * Description: An item of linked list used to store FDB entries
 *
 * Fields:
 *   item      - a basic linked list struct. Should be always a first struct's
 *               field to make it possible a using of this structure as
 *               descendant of PRV_TGF_LIST_ITEM_STC
 *   devNum    - a device number wielding FDB table
 *   deleted   - is required when a node should be deleted logically but can't
 *               be deleted phisically yet. For example, you need to read some
 *               field of this node after calling the function which should
 *               "think" this node is deleted
 *   message   - a mac entry. In case of FDB IP UC Entries added by index this
 *               field will hold the macEntryIndex where the entry was added in
 *               the HW. If the entry is added by message this field value will
 *               be 0xFFFFFFFF
 *
 */
typedef struct{
    PRV_TGF_LIST_ITEM_STC     item;
    GT_U8                     devNum;
    GT_BOOL                   deleted;
    GT_U32                    index;
    PRV_TGF_BRG_MAC_ENTRY_STC message;
}PRV_TGF_BRG_FDB_AUQ_MESSAGE_PART2_STC;

extern PRV_TGF_LIST_HEAD_STC brgFdbAuqMessagesPart2;
#endif /*CHX_FAMILY*/


/*************************** functions declaration ****************************/
/*******************************************************************************
* prvTgfBrgFdbInit
*
* DESCRIPTION:
*       Init the TGF FDB lib. needed for GM FDB shadow.
*       (but with flag allowAnyDevice = true , we can use it for any device)
*
* INPUTS:
*       devNum - the device number
*       allowAnyDevice - allow any device to use the shadow.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS   prvTgfBrgFdbInit(IN GT_U8 devNum , IN GT_BOOL allowAnyDevice);

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChMacEntryKey
*
* DESCRIPTION:
*       Convert generic MAC entry key into device specific MAC entry key
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) MAC entry key parameters
*
* OUTPUTS:
*       dxChMacEntryKeyPtr - (pointer to) DxCh MAC entry key parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToDxChMacEntryKey
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *macEntryKeyPtr,
    OUT CPSS_MAC_ENTRY_EXT_KEY_STC   *dxChMacEntryKeyPtr
);


/*******************************************************************************
* prvTgfConvertCpssToGenericMacKey
*
* DESCRIPTION:
*       Convert DX MAC entry key into device generic MAC entry key
*
* INPUTS:
*       dxChMacEntryKeyPtr - (pointer to) DxCh MAC entry key parameters
*
* OUTPUTS:
*       macEntryKeyPtr - (pointer to) MAC entry key parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertCpssToGenericMacKey
(
    IN CPSS_MAC_ENTRY_EXT_KEY_STC       *dxChMacEntryKeyPtr,
    OUT  PRV_TGF_MAC_ENTRY_KEY_STC      *macEntryKeyPtr
);

/*******************************************************************************
* prvTgfConvertGenericToDxChMacEntry
*
* DESCRIPTION:
*       Convert generic MAC entry into device specific MAC entry
*
* INPUTS:
*       devNum      - device number
*       macEntryPtr - (pointer to) MAC entry parameters
*
* OUTPUTS:
*       dxChMacEntryPtr - (pointer to) DxCh MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToDxChMacEntry
(
    IN  GT_U8                         devNum,
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC       *dxChMacEntryPtr
);

/*******************************************************************************
* prvTgfConvertDxChToGenericMacEntry
*
* DESCRIPTION:
*       Convert device specific MAC entry into generic MAC entry
*
* INPUTS:
*       dxChMacEntryPtr - (pointer to) DxCh MAC entry parameters
*
* OUTPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertDxChToGenericMacEntry
(
    IN  CPSS_MAC_ENTRY_EXT_STC       *dxChMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);


/*******************************************************************************
* prvTgfBrgFdbMacEntryDelete_unregister
*
* DESCRIPTION:
*       Remove the MAC entry from the linked list brgFdbAuqMessagesPart2
*
* INPUTS:
*       devNum         - a device number
*       macEntryKeyPtr - a MAC entry struct pointer
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryDelete_unregister
(
    IN GT_U8                          devNum,
    IN PRV_TGF_MAC_ENTRY_KEY_STC     *macEntryKeyPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntrySet_register
*
* DESCRIPTION:
*       Add the MAC entry to the linked list brgFdbAuqMessagesPart2
*
* INPUTS:
*       devNum         - a device number
*       index          - an FDB entry index
*       macEntryKeyPtr - a MAC entry struct pointer
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntrySet_register
(
    IN GT_U8                         devNum,
    IN GT_U32                        index,
    IN PRV_TGF_BRG_MAC_ENTRY_STC     *macEntryPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntrySet_register
*
* DESCRIPTION:
*       Add/Remove the MAC entry to/from the linked list brgFdbAuqMessagesPart2
*       depending on the 'skip' parameter.
*
* INPUTS:
*       devNum         - a device number
*       index          - an FDB entry index
*       skip           - whether to delete (GT_TRUE) or add (GT_FASE) a mac
*                        entry to the linked list
*       macEntryKeyPtr - a MAC entry struct pointer
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryWrite_register
(
    IN GT_U8                         devNum,
    IN GT_U32                        index,
    IN GT_BOOL                       skip,
    IN PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);

/*******************************************************************************
* prvTgfBrgFdbFlushByShadowPart1
*
* DESCRIPTION:
*       Go through the linked list brgFdbAuqMessagesPart1 look for NA
*       messages and delete appropriate FDB entries
*
* INPUTS:
*       devNum     - a device number
*       fdbDelMode - FDB deletion mode
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_OUT_OF_RANGE - index out of range

*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbFlushByShadowPart1
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT fdbDelMode
);

/*******************************************************************************
* prvTgfBrgFdbFlushByShadowPart2
*
* DESCRIPTION:
*       Go through the linked list brgFdbAuqMessagesPart2
*       and delete FDB entries that  apporopriate to the list's items
*
* INPUTS:
*       devNum - a device number
*       fdbDelMode - FDB deletion mode
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_OUT_OF_RANGE - index out of range

*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbFlushByShadowPart2
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT fdbDelMode
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart1
*
* DESCRIPTION:
*       get next entry from the FDB shadow 'part 1'.
*
* INPUTS:
*       devNum     - a device number
*       fdbDelMode - FDB deletion mode
*       cookiePtrPtr - (pointer to) cookiePtr. the 'cookie' as IN
*                   (identify the previous entry returned)
*                   when (*cookiePtrPtr)==NULL means to start from first element
* OUTPUTS:
*       cookiePtrPtr - (pointer to) cookiePtr. the 'cookie' as OUT
*                   (identify the current entry returned)
*       OUT_macEntryPtr - (pointer to) the fdb entry found in the shadow.
*
* RETURNS:
*       GT_OK      - on success,
*       GT_NO_MORE - no more entries in this FDB shadow.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart1
(
    IN GT_U8                         devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT mode,
    INOUT GT_U32                   *cookiePtr,
    OUT CPSS_MAC_ENTRY_EXT_STC    **OUT_macEntryPtr
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart2
*
* DESCRIPTION:
*       get next entry from the FDB shadow 'part 2'.
*
* INPUTS:
*       devNum     - a device number
*       fdbDelMode - FDB deletion mode
*       cookiePtrPtr - (pointer to) cookiePtr. the 'cookie' as IN
*                   (identify the previous entry returned)
*                   when (*cookiePtrPtr)==NULL means to start from first element
* OUTPUTS:
*       cookiePtrPtr - (pointer to) cookiePtr. the 'cookie' as OUT
*                   (identify the current entry returned)
*       OUT_macEntryPtr - (pointer to) the fdb entry found in the shadow.
*                       NOTE: the format is 'TGF'
*
* RETURNS:
*       GT_OK      - on success,
*       GT_NO_MORE - no more entries in this FDB shadow.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryFromShadowGetNext_shadowPart2
(
    IN GT_U8                         devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT mode,
    INOUT GT_U32                      *cookiePtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    **OUT_macEntryPtr
);

#endif /*CHX_FAMILY*/


#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToExMxPmMacEntry
*
* DESCRIPTION:
*       Convert generic MAC entry into device specific MAC entry
*
* INPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* OUTPUTS:
*       exMxPmMacEntryPtr - (pointer to) ExMxPm MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertGenericToExMxPmMacEntry
(
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr,
    OUT CPSS_EXMXPM_FDB_ENTRY_STC    *exMxPmMacEntryPtr
);

/*******************************************************************************
* prvTgfConvertExMxPmToGenericMacEntry
*
* DESCRIPTION:
*       Convert device specific MAC entry into generic MAC entry
*
* INPUTS:
*       exMxPmMacEntryPtr - (pointer to) ExMxPm MAC entry parameters
*
* OUTPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertExMxPmToGenericMacEntry
(
    IN  CPSS_EXMXPM_FDB_ENTRY_STC    *exMxPmMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);


#endif /* EXMXPM_FAMILY */


/*******************************************************************************
* prvTgfBrgFdbMacEntryInvalidatePerDev
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index
*       of specified device.
*       the invalidation done by resetting to first word of the entry
*
* INPUTS:
*       devNum - a device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index  - hw mac entry index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryInvalidatePerDev
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               index
);

/*******************************************************************************
* prvTgfBrgFdbHashCrcMultiResultsCalc
*
* DESCRIPTION:
*       This function calculates the CRC multiple hash results
*       (indexes into the FDB table).
*       NOTE: the function do not access the HW , and do only SW calculations.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - (pointer to) entry key
*       multiHashStartBankIndex  - the first bank for 'multi hash' result.
*                   (APPLICABLE RANGES: 0..15)
*       numOfBanks    - number of banks for 'multi hash' result.
*                   this value indicates the number of elements that will be
*                   retrieved by crcMultiHashArr[]
*                   restriction of (numOfBanks + multiHashStartBankIndex) <= 16 .
*                   (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*      crcMultiHashArr[] - (array of) 'multi hash' CRC results. index in this array is 'bank Id'
*                           (index 0 will hold value relate to bank multiHashStartBankIndex).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or parameters in entryKeyPtr.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbHashCrcMultiResultsCalc
(
    IN  GT_U8                       devNum,
    IN  PRV_TGF_MAC_ENTRY_KEY_STC   *macEntryKeyPtr,
    IN  GT_U32                      multiHashStartBankIndex,
    IN  GT_U32                      numOfBanks,
    OUT GT_U32                      crcMultiHashArr[]
);

/*******************************************************************************
* prvTgfBrgFdbMacEntryDeletePerDev
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table of specified device
*
* INPUTS:
*       devNum         - a device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr - (pointer to) key parameters of the mac entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vlanId
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryDeletePerDev
(
    IN GT_U8                          devNum,
    IN GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN PRV_TGF_MAC_ENTRY_KEY_STC     *macEntryKeyPtr
);

/*******************************************************************************
* prvTgfConvertCpssToGenericMacEntry
*
* DESCRIPTION:
*       Convert device specific MAC entry into generic MAC entry
*
* INPUTS:
*       cpssMacEntryPtr - (pointer to) cpss MAC entry parameters
*
* OUTPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfConvertCpssToGenericMacEntry
(
    IN  CPSS_MAC_ENTRY_EXT_STC       *cpssMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);

/*******************************************************************************
* prvTgfBrgFdbAuqCbRegister
*
* DESCRIPTION:
*       register a CB (callback) function for a specific AUQ message type.
*       the registered function (cbFuncPtr) will be called for every AUQ message
*       that match the messageType.
*
* INPUTS:
*       messageType - AUQ message type
*       bindOrUnbind    - bind/unbind   :
                        GT_TRUE - bind
                        GT_FALSE - unbind
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbAuqCbRegister
(
    IN CPSS_UPD_MSG_TYPE_ENT       messageType,
    IN GT_BOOL                     bindOrUnbind
);

/*******************************************************************************
* tgfCpssDxChBrgFdbFromCpuAuMsgStatusPerPortGroupsGet
*
* DESCRIPTION:
*       Wait an FDB Address Update (AU) message processing is finished and get
*       it status for every specified port group.
*
* INPUTS:
*       devNum - a device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       mustSucceed   - expected status of the message processing
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    - if all portGroups finished message processing
*                                  and expected status is the same as expected
*       GT_FAIL                  - on error
*       GT_TIMEOUT               - on timeout expired
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS tgfCpssDxChBrgFdbFromCpuAuMsgStatusPerPortGroupsGet
(
    IN GT_U8              devNum,
    IN GT_PORT_GROUPS_BMP portGroupsBmp,
    IN GT_BOOL            mustSucceed
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfBridgeGen */
