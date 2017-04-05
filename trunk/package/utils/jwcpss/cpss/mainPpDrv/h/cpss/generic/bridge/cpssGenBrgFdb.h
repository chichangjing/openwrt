/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenBrgFdb.h
*
* DESCRIPTION:
*       definitions for Multiple Forwarding Databases for 802.1Q Transparent
*       devices.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*******************************************************************************/

#ifndef __cpssGenBrgFdbh
#define __cpssGenBrgFdbh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/generic/cpssCommonDefs.h>

/*
 * typedef: enum CPSS_FDB_AGE_TRUNK_MODE_ENT
 *
 * Description: Enumeration of FDB aging mode for trunk entries.
 *
 * Enumerations:
 *    CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E - the PP when doing trigger/auto aging
 *                  can age trunk entry - regardless to the associated device
 *                  number.
 *    CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E -
 *                  the PP when doing trigger/auto aging can age trunk entry ,
 *                  only if the associated device number is "own devNum"
 *
 */
typedef enum{
    CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E,
    CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E
}CPSS_FDB_AGE_TRUNK_MODE_ENT;

/*
 * typedef: enum CPSS_MAC_TABLE_CMD_ENT
 *
 * Description: Enumeration of MAC Table entry actions taken when a packet's
 *              MAC address (DA/SA) matches this entry
 *
 * Enumerations:
 *      CPSS_MAC_TABLE_FRWRD_E    - forward (if address is automatically learned)
 *      CPSS_MAC_TABLE_DROP_E     - drop (filtering on destination/source address)
 *      CPSS_MAC_TABLE_INTERV_E   - intervention to CPU (may be dropped by other
 *                                   mechanisms)
 *      CPSS_MAC_TABLE_CNTL_E     - control (unconditionally trap to CPU)
 *      CPSS_MAC_TABLE_MIRROR_TO_CPU_E - mirror to the CPU (in addition to sending
 *                                       the packet to its destination) - not
 *                                       supported in ExMx device
 *      CPSS_MAC_TABLE_SOFT_DROP_E     - soft drop (does not prevent the packet from
 *                                       being sent to the CPU) - supported in
 *                                       DxCh devices
 */
typedef enum
{
    CPSS_MAC_TABLE_FRWRD_E = 0,
    CPSS_MAC_TABLE_DROP_E,
    CPSS_MAC_TABLE_INTERV_E,
    CPSS_MAC_TABLE_CNTL_E,
    CPSS_MAC_TABLE_MIRROR_TO_CPU_E,
    CPSS_MAC_TABLE_SOFT_DROP_E
} CPSS_MAC_TABLE_CMD_ENT;


/*
 * typedef: struct CPSS_FDB_HASH_OPTION_STC
 *
 * Description: FDB Hash Options parameters
 *
 * Fields:
 *   macLookupMask  - the MAC lookup mask indicates the MAC address
 *                    bits that are used in the MAC lookup table.
 *                    Default PP HW value of macLookupMasc is 0xFFFFFFFFFFFF
 *                    (all bits are used).
 *
 *                    Note: The Application SW should support MAC address
 *                          Network byte-order (big-endian format). Therefore,
 *                          the element with index [0] will contain the most
 *                          significant byte of MAC lookup mask, and the element
 *                          with index [5] will contain the least significant
 *                          byte of MAC lookup mask.
 *
 *   vidLookupMask  - the VID lookup mask indicates the VLAN ID bits that are
 *                    used in the MAC lookup table.
 *                    Default PP HW value of vidLookupMask is 0xFFF
 *                    (all bits are used).
 *
 *   macShiftLeft   - indicates the cyclic shift left done on the MAC address
 *                    to determine which bits are to be the most valuable in
 *                    lookup and learning (in the bytes).
 *                        Due to the 6 bytes size of MAC address, the range of this
 *                    parameter is [0:5].
 *                        Default PP HW value of macShiftLeft is 0 bytes.
 *
 *  vidShiftLeft    - indicates the cyclic shift left done on the VID to
 *                    determine which bits are to be the most valuable in
 *                    lookup and learning (in the nibbles).
 *                    Due to the 3 nibbles size of VLAN ID, the range of this
 *                    parameter is [0:2].
 *                    Default PP HW value of vidShiftLeft is 0 nibbles.
 *
 */
typedef struct
{
    GT_ETHERADDR    macLookupMask;
    GT_U16          vidLookupMask;
    GT_U32          macShiftLeft;
    GT_U32          vidShiftLeft;
} CPSS_FDB_HASH_OPTION_STC;

/*
 * typedef: enum CPSS_MAC_VL_ENT
 *
 * Description: Enumeration of VLAN Learning modes
 *
 * Enumerations:
 *      CPSS_IVL_E - Independent VLAN Learning
 *      CPSS_SVL_E - Shared VLAN Learning
 */
typedef enum
{
    CPSS_IVL_E = 1,
    CPSS_SVL_E
} CPSS_MAC_VL_ENT;

/*
 * typedef: enum CPSS_ADDR_LOOKUP_MODE_ENT
 *
 * Description: Enumeration of Address lookup modes
 *
 * Enumerations:
 *      CPSS_MAC_SQN_VLAN_SQN_E - Optimized for sequential MAC addresses and
 *                            sequential VLAN id's.
 *      CPSS_MAC_RND_VLAN_SQN_E - Optimized for random MAC addresses and
 *                            sequential VLAN id's.
 *      CPSS_MAC_SQN_VLAN_RND_E - Optimized for sequential MAC addresses and
 *                            random VLAN id's.  Reserved.
 *      CPSS_MAC_RND_VLAN_RND_E - Optimized for random MAC addresses and
 *                            random VLAN id's.  Reserved.
 */
typedef enum
{
    CPSS_MAC_SQN_VLAN_SQN_E = 0,
    CPSS_MAC_RND_VLAN_SQN_E,
    CPSS_MAC_SQN_VLAN_RND_E,
    CPSS_MAC_RND_VLAN_RND_E

} CPSS_ADDR_LOOKUP_MODE_ENT;

/*
 * typedef: enum CPSS_MAC_ACTION_MODE_ENT
 *
 * Description: Enumeration of MAC address table action mode
 *
 * Enumerations:
 *      CPSS_ACT_AUTO_E - Action is done automatically
 *      CPSS_ACT_TRIG_E - Action is done via trigger from CPU
 */
typedef enum
{
    CPSS_ACT_AUTO_E = 0,
    CPSS_ACT_TRIG_E

} CPSS_MAC_ACTION_MODE_ENT;


/*
 * typedef: enum CPSS_MAC_AU2CPU_TYPE_ENT
 *
 * Description: Enumeration of MAC address update message sent to the CPU.
 *
 * Enumerations:
 *      CPSS_MAC_AU_MESSAGE_SA_LEARNING_E
 *                                  - Security address messages due to VLAN
 *                                    learning disabled.
 *      CPSS_MAC_AU_MESSAGE_SA_SECURITY_E
 *                                  - Other security address messages.
 *      CPSS_MAC_AU_MESSAGE_TA_AA_E - Transplanting and aged address messages.
 *      CPSS_MAC_AU_MESSAGE_QA_E    - Enable Query address to CPU
 *      CPSS_MAC_AU_MESSAGE_ALL_E   - All types of TA, AA, QA , and SA messages.
 *      CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E
 *                                  - All types of TA, AA, and SA messages
 *                                    exclude QA and NA - DXSAL Only
 *      CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E
 *                                  - When enabled EXMX device is send QA to the
 *                                    other EXMX devices (via Uplink) , and
 *                                    optionally to the CPU on every packet with
 *                                    unknown DA !
 *                                    To disable the CPU from getting those
 *                                    messages , disable this feature or the
 *                                    CPSS_MAC_AU_MESSAGE_QA_E.
 *                                    NOTE:
 *                                          By default - feature is enabled.
 *
 */
typedef enum
{
    CPSS_MAC_AU_MESSAGE_SA_LEARNING_E = 0,
    CPSS_MAC_AU_MESSAGE_SA_SECURITY_E,
    CPSS_MAC_AU_MESSAGE_TA_AA_E,
    CPSS_MAC_AU_MESSAGE_QA_E,
    CPSS_MAC_AU_MESSAGE_ALL_E,
    CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E,
    CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E
} CPSS_MAC_AU2CPU_TYPE_ENT;

/*
 * typedef: enum CPSS_MAC_FLT_CMD_ENT
 *
 * Description: Enumeration of MAC Filter Commands taken when a packet's
 *              MAC address (DA/SA) matches specified range
 *
 * Enumerations:
 *      CPSS_MAC_FLT_FRWRD_E               - Forward
 *      CPSS_MAC_FLT_DROP_E                - Drop
 *      CPSS_MAC_FLT_CNTL_E                - Control (unconditionally trap to CPU)
 *      CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E - Forward and mirror to the CPU
 */
typedef enum
{
    CPSS_MAC_FLT_FRWRD_E = 1,
    CPSS_MAC_FLT_DROP_E,
    CPSS_MAC_FLT_CNTL_E,
    CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E
} CPSS_MAC_FLT_CMD_ENT;


/*
 * typedef: enum CPSS_MAC_ENTRY_STATUS_ENT
 *
 * Description: Enumeration of MAC Table entry status
 *
 * Enumerations:
 *   CPSS_LRND_E       - The value of the corresponding instance was learned
 *   CPSS_MGMT_E       - The value of the corresponding instance was created by
 *                       CPU like static.
 *   CPSS_NOT_STATIC_E - The value of the corresponding instance was created by
 *                       CPU like NOT static.
 */
typedef enum
{
    CPSS_LRND_E = 0,
    CPSS_MGMT_E,
    CPSS_NOT_STATIC_E
} CPSS_MAC_ENTRY_STATUS_ENT;

/*
 * typedef: enum CPSS_PORT_LOCK_CMD_ENT
 *
 * Description: Enum for Locked port forward commands
 *
 * Enumerations:
 *      CPSS_LOCK_FRWRD_E  - Do not drop (forward as usual).
 *      CPSS_LOCK_DROP_E   - Drop (hard drop).
 *      CPSS_LOCK_TRAP_E   - Trap to CPU (Intervention).
 *      CPSS_LOCK_MIRROR_E - Forward and Mirror to CPU. Applicable to Dx devices
 *      CPSS_LOCK_SOFT_DROP_E - Soft Drop(can still get the CPU).
 *                              Applicable to DxCh devices.
 */
typedef enum
{
    CPSS_LOCK_FRWRD_E = 1,
    CPSS_LOCK_DROP_E,
    CPSS_LOCK_TRAP_E,
    CPSS_LOCK_MIRROR_E,
    CPSS_LOCK_SOFT_DROP_E
} CPSS_PORT_LOCK_CMD_ENT;

/*
 * typedef: enum CPSS_LEARNING_MODE_ENT
 *
 * Description: Enum for Learning modes
 *
 * Enumerations:
 *      CPSS_AUTOMATIC_E        - Automatic.
 *      CPSS_CONTROLED_MSSG_E   - Using message default
 *      CPSS_CONTROLED_TAG_E    - Using DSA tag . Applicable to Dx devices.
 */
typedef enum
{
    CPSS_AUTOMATIC_E        = 0,
    CPSS_CONTROLED_MSSG_E   = 1,
    CPSS_CONTROLED_TAG_E    = 2
} CPSS_LEARNING_MODE_ENT;

/*
 * typedef: struct CPSS_MAC_ENTRY_STC
 *
 * Description: MAC Address Entry
 *
 * Fields:
 *   macAddr    - MAC address
 *   dstInterface - Destination interface : port/trunk/vidx
 *   isStatic   - static/dynamic entry
 *   vlanId     - Vlan Id
 *   srcTc      - Traffic class assigned to a packet with this source MAC
 *   dstTc      - Traffic class assigned to a packet with this destination MAC
 *   daCommand  - Action taken when a packet's DA matches this entry
 *   saCommand  - Action taken when a packet's SA matches this entry
 *   saClass    - GT_TRUE and packet's SA matches this entry,
 *                send packet to the Multi-Field Classifier
 *   daClass    - GT_TRUE, and packet's DA matches this entry,
 *                send packet to the Multi-Field Classifier
 *   saCib      - GT_TRUE, and packet's SA matches this entry,
 *                send packet to Customer Interface Bus
 *   daCib      - GT_TRUE, and packet's DA matches this entry,
 *                send packet to Customer Interface Bus
 *   daRoute    - GT_TRUE, and packet's DA matches this entry,
 *                send packet to the IPv4 or MPLS
 *   cosIpv4En  - If the packet SA or DA lookup matches the FDB entry,
 *                and this field is set to GT_TRUE, the packet is assigned COS
 *                parameters according to assignment made by the CoS library.
 *                Applicable to DxSal devices
 *   mirrorToRxAnalyzerPortEn - GT_TRUE, and packet's DA matches this entry,
 *                              mirror packet to a configurable analyzer port.
*/
typedef struct
{
    GT_ETHERADDR            macAddr;
    CPSS_INTERFACE_INFO_STC dstInterface;
    GT_BOOL                 isStatic;
    GT_U16                  vlanId;
    GT_U8                   srcTc;
    GT_U8                   dstTc;
    CPSS_MAC_TABLE_CMD_ENT  daCommand;
    CPSS_MAC_TABLE_CMD_ENT  saCommand;
    GT_BOOL                 saClass;
    GT_BOOL                 daClass;
    GT_BOOL                 saCib;
    GT_BOOL                 daCib;
    GT_BOOL                 daRoute;
    GT_BOOL                 cosIpv4En;
    GT_BOOL                 mirrorToRxAnalyzerPortEn;
} CPSS_MAC_ENTRY_STC;


/*
 * typedef: enum CPSS_UPD_MSG_TYPE_ENT
 *
 * Description: Enumeration of MAC Table update messages that can be sent
 *              between Prestera and CPU.
 *
 * Enumerations:
 *   CPSS_NA_E - new address (NA) message.
 *           message from PP (to CPU)-
 *                  in auto learn : meaning that the PP learned new source
 *                    address(or 'Station movement').
 *                  in controlled learn : meaning the PP notify
 *                    the CPU about a new source address(or 'Station movement').
 *              NOTE: for ExMx devices , in controlled learning the device send
 *                    SA and not NA messages
 *           message from CPU (to PP) - CPU 'ask' PP to add/update FDB entry.
 *                  this message can be used to 'skip' entry.
 *                  NOTE:
 *                  1. operation may fail due to bucket full.
 *                  2. in ExMxPm devices operation will fail if bucket and TCAM
 *                     are full , or when working in external FDB and the LUT
 *                     (lookup table) entry is full.
 *
 *   CPSS_QA_E - query address (QA) message
 *           message from CPU (to PP) - when the CPU wants to get full FDB entry
 *              info about an entry identified by the 'Mac+vid' (key of entry).
 *           NOTE: the PP will reply with the info in QR (Query reply) message.
 *
 *   CPSS_QR_E - query reply (QR)
 *           message from PP (to CPU) - the PP sends info about a query send
 *           from CPU. the query was one of : QA / QI
 *           reply to QA - the PP will return the full FDB entry , and indication
 *              weather entry was found.
 *           reply to QI - the PP will return the Index of the FDB entry, and
 *              indication weather entry was found.
 *           NOTE:
 *           1. in DxCh,ExMxPm devices the Index is offset in the bucket
 *                    and not index in the FDB table. (CPU needs to calculate
 *                    the index of the start of bucket)
 *           2. in DxCh,ExMxPm devices the QR always holds all the information:
 *              full FDB entry and the index (offset in the bucket)
 *
 *   CPSS_AA_E - aged address (AA) message
 *           message from PP (to CPU) -
 *                  in age with removal : meaning the PP aged out an entry.
 *                  in age without removal : meaning the PP notify the CPU about
 *                      entry that should be aged out.
 *              NOTE: the PP will send those messages also in the process of
 *                    'FDB flush' , or 'FDB triggered aging' for entries that need
 *                    to be aged out.
 *          message from CPU (to PP) - CPU force the PP to invalidate an FDB entry
 *
 *   CPSS_TA_E - transplanted address (TA) message.
 *           message from PP (to CPU) - the PP in the process of 'FDB transplanting'
 *                  notify the CPU that an entry transplanted to associate with
 *                  new destination.
 *
 *   CPSS_SA_E - security address message
 *           message from PP (to CPU)-
 *                  in controlled learn : meaning the PP notify
 *                    the CPU about a new source address(or 'Station movement').
 *              NOTE:
 *           1. In auto learning the device send NA and not SA messages
 *           2. applicable devices : All ExMx devices.
 *
 *   CPSS_QI_E - query address index message
 *           message from CPU (to PP) - When the CPU wants to get index of FDB
 *                  entry , info about an entry identified by the 'Mac+vid'
 *                  (key of entry).
 *           NOTE:
 *           1. the PP will reply with the info in QR (Query reply) message.
 *           2. applicable devices : All ExMx devices.
 *
 *   CPSS_FU_E - FDB Upload (FU) message.
 *           message from PP (to CPU) - the PP notify the CPU about valid and
 *              non skipped FDB entries.
 *           NOTE:
 *           1. this done in the process of 'FDB upload triggering' .
 *           2. applicable devices : DxCh2 and above devices.
 *              applicable devices : All ExMxPm devices.
 *
 *  CPSS_HR_E - Hash Request (HR) message.
 *          message that is sent from the CPU to the PP , when FDB hash results
 *          that are calculated by the PP needed for the 'search key'.
 *          see function cpssDxChBrgFdbHashRequestSend.
 *          (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *  CPSS_NA_MOVED_E - CPU to PP message to move 'valid' entry from <origFdbIndex>
 *          in the FDB table to 'new index'.
 *          see function cpssDxChBrgFdbMacEntryMove.
 *          (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 */
typedef enum
{
    CPSS_NA_E = 0,
    CPSS_QA_E,
    CPSS_QR_E,
    CPSS_AA_E,
    CPSS_TA_E,
    CPSS_SA_E,
    CPSS_QI_E,
    CPSS_FU_E,
    CPSS_HR_E,
    CPSS_NA_MOVED_E

} CPSS_UPD_MSG_TYPE_ENT;



/*
 * typedef: struct CPSS_FDB_MAC_UPDATE_MSG_STC
 *
 * Description: MAC Table Update message exchanged between the Prestera
 *              device and CPU.
 *
 * Fields:
 *    updType    - update message type (NA, QA, QR, AA, TA, SA, QI)
 *    entryWasFound - entry found or not flag (relevant only ot QR/QI msg to CPU)
 *    macEntryIndex - the entry index in the MAC table in which the MAC address
 *                    resides
 *                    relevant only for QI msg & entryFound = 1)
 *    macEntry - FDB entry format
 *    skip     - the skip bit - GT_TRUE  - entry not used by HW
 *                              GT_FALSE - entry used by HW
 *    aging    - the age bit - GT_TRUE  - the entry is "refresh" (not "aged" by
 *                             aging cycle since last traffic on the entry)
 *                             GT_FALSE - the entry has "aged" by 1 aging cycle
 *    associatedDevNum - the device num that entry associated with
 *                NOTE: this value is valid for all type of entries:
 *                      unicast on port /  unicast on trunk /using Vidx
 *    queryDevNum - The Device-ID of the QA/QR message sender.
 *                  Relevant ONLY for QA/QR messages between Prestera Packet
 *                  Processors.
 *    msgCpu      - GT_FALSE - Message is from/to other Prestera packet processor.
 *                  GT_TRUE  - Message is from/to CPU.
 *                  NOTE: CPU must set this bit to '1'.
 *
 */
typedef struct
{
    CPSS_UPD_MSG_TYPE_ENT   updType;
    GT_BOOL                 entryWasFound;
    GT_U32                  macEntryIndex;
    CPSS_MAC_ENTRY_STC      macEntry;
    GT_BOOL                 skip;
    GT_BOOL                 aging;
    GT_U8                   associatedDevNum;
    GT_U8                   queryDevNum;
    GT_BOOL                 msgCpu;
} CPSS_MAC_UPDATE_MSG_STC;

/*
 * Typedef: enum CPSS_BRG_IPM_MODE_ENT
 *
 * Description:
 *      This enum defines IPM Bridging mode
 * Fields:
 *     CPSS_BRG_IPM_SGV_E - This mode is used for  Source-Specific
 *                          Multicast (SSM) snooping. The FDB lookup
 *                          is based on the packet source IP (SIP),
 *                          group destination IP (DIP), and VLAN-ID
 *
 *     CPSS_BRG_IPM_GV_E  - This mode is used for Any-Source Multicast
 *                          (ASM) snooping. The FDB lookup is based on
 *                          the packet group destination IP (DIP),
 *                          and VLAN-ID
 *  Comments:
 */
typedef enum
{
    CPSS_BRG_IPM_SGV_E,
    CPSS_BRG_IPM_GV_E
} CPSS_BRG_IPM_MODE_ENT;

/*
 * Typedef: enum CPSS_MAC_ENTRY_EXT_TYPE_ENT
 *
 * Description:
 *      This enum defines the type of the FDB Entry
 * Fields:
 *     CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E - The entry is MAC Address
 *                          entry (hushed by Mac Address and vlan ID).
 *
 *     CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E  - The entry is IPv4 Multicast
 *                                              entry (IGMP Snooping).
 *
 *     CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E  - The entry is IPv6 Multicast
 *                                              entry (MLD Snooping).
 *
 *     CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E  - The entry is IPv4 Unicast entry.
 *                                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *     CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E  - The entry is IPv6 Unicast address entry
 *                                          (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *     CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E  - The entry is IPv6 Unicast data entry
 *                                          (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E - The entry is MAC Address entry
 *                              (hashed by Mac Address, FID and VID1).
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *     CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E  - The entry is IPv4 Multicast
 *                             entry. Hashed by SIP, DIP, FID, and VID1.
 *                             (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *     CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E  - The entry is IPv6 Multicast
 *                             entry. Hashed by SIP, DIP, FID, and VID1.
 *                             (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 * Comments:
 *     Order and value should not be changed.
 */
typedef enum
{
    CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E = 0,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E,
    CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E,
    CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E
} CPSS_MAC_ENTRY_EXT_TYPE_ENT;


/*
 * typedef: struct CPSS_MAC_ENTRY_EXT_KEY_STC
 *
 * Description: Key parameters of the MAC Address Entry
 *
 * Fields:
 *  entryType   - the type of the entry MAC Addr or IP Multicast
 *  vid1        - Tag1 VID value (APPLICABLE RANGES: 0..0xFFF). Relevant only for
 *                key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST. Is used as part of a key
 *                if double tag FDB lookup key mode is enabled i.e. entry type is
 *                one of:
 *                    CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E,
 *                    CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E,
 *                    CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E
 *                (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *  key     - mac entry key.
 *            for type CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ONLY_ENTRY_E no key info
 *     macVlan     - mac VLAN entry information (for <entry type> = MAC Address entry)
 *        macAddr     - mac address
 *        vlanId      - Vlan Id (APPLICABLE RANGES: 0..0xFFF)
 *     ipMcast - IP Multicast entry information (for <entry type> = IPv4/IPv6 MC entry)
 *         dip         - For IPv4 entry type it's destination address bytes in the
 *                       Network order. For example for IP address 224.1.2.3
 *                       ipMcast.dip = {224,1,2,3}. For IPv6 entry type it's four
 *                       selected bytes in the Network order.
 *                       E.g. ipMcast.dip[4] = byte0, ipMcast.dip[3] = byte1 and so on.
 *                       IPv6 Bytes selection is done by related API, for DXCH devices
 *                       use cpssDxChBrgMcIpv6BytesSelectSet.
 *         sip         - For IPv4 entry type it's source address bytes in the Network
 *                        order. For example for IP address 224.1.2.3
 *                       ipMcast.sip = {224,1,2,3}. For IPv6 entry type it's four
 *                       selected bytes in the Network order.
 *                       E.g. ipMcast.sip[4] = byte0, ipMcast.sip[3] = byte1 and so on.
 *                       IPv6 Bytes selection is done by related API, for DXCH devices
 *                       use cpssDxChBrgMcIpv6BytesSelectSet.
 *        vlanId      - Vlan Id (APPLICABLE RANGES: 0..0xFFF)
 *     ipv4Unicast - IP Unicast entry information (for <entry type> = IPv4)
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *         dip          -   For IPv4 entry type it's destination address bytes in the
 *                          Network order. For example for IP address 1.2.3.4
 *                          ipv4Unicast.dip = {1,2,3,4}.
 *         vrfId        -   Thr VRF-ID is part of the lookup key
 *     ipv6Unicast - IP Unicast entry information (for <entry type> = IPv6)
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *         dip          -   For IPv6 entry type it's destination address bytes in the
 *                          Network order. For example for IP address 1.2.3.5...16
 *                          ipv6Unicast.dip = {1,2,3,4, .... 16}.
 *         vrfId        -   Thr VRF-ID is part of the lookup key
 *
 *
*/
typedef struct
{
    CPSS_MAC_ENTRY_EXT_TYPE_ENT         entryType;
    GT_U32                              vid1;
    union {
       struct{
           GT_ETHERADDR     macAddr;
           GT_U16           vlanId;
       }macVlan;

       struct{
           GT_U8            sip[4];
           GT_U8            dip[4];
           GT_U16           vlanId;
       }ipMcast;

       struct{
           GT_U8            dip[4];
           GT_U32           vrfId;
       }ipv4Unicast;

       struct{
           GT_U8            dip[16];
           GT_U32           vrfId;
       }ipv6Unicast;

    } key;

}CPSS_MAC_ENTRY_EXT_KEY_STC;


/*
 * typedef: struct CPSS_FDB_UC_ROUTING_INFO_STC
 *          (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Description: Routing Address Entry
 *
 * Fields:
 *  ttlHopLimitDecEnable - Enable TTL/Hop Limit Decrement
 *  ttlHopLimDecOptionsExtChkByPass- TTL/HopLimit Decrement and option/
 *                       Extention check bypass.
 *  ingressMirror      - mirror to ingress analyzer.
 *  ingressMirrorToAnalyzerIndex - One of seven possible analyzers. Relevant
 *                       when ingressMirror is GT_TRUE.
 *                       If a previous engine in the pipe assigned a different
 *                       analyzer index, the higher index wins.
 *                       (APPLICABLE RANGES: 0..6)
 *  qosProfileMarkingEnable- Enable Qos profile assignment.
 *  qosProfileIndex       - the qos profile to assign in case of
 *                          qosProfileMarkingEnable = GT_TRUE
 *  qosPrecedence         - whether this packet Qos parameters can be overridden
 *                          after this assigment.
 *  modifyUp              - whether to change the packets UP and how.
 *  modifyDscp            - whether to change the packets DSCP and how.
 *  countSet              - The counter set this route entry is linked to
 *  trapMirrorArpBcEnable - enable Trap/Mirror ARP Broadcasts with DIP matching
 *                          this entry
 *  dipAccessLevel        - The security level associated with the DIP.
 *  ICMPRedirectEnable    - Enable performing ICMP Redirect Exception Mirroring.
 *  mtuProfileIndex       - One of the eight global configurable MTU sizes
 *                          (APPLICABLE RANGES: 0..7)
 *  isTunnelStart         - weather this nexthop is tunnel start enrty, in which
 *                          case the outInteface & mac are irrlevant and the tunnel
 *                          id is used.
 *                          only if isTunnelStart = GT_FALSE then dstInterface in
 *                          CPSS_MAC_ENTRY_EXT_STC is relevant
 *  nextHopVlanId         - the output vlan id (used also for SIP RPF check, and
 *                          ICMP check)
 *  nextHopARPPointer     - The ARP Pointer indicating the routed packet MAC DA,
 *                          relevant only if the isTunnelStart = GT_FALSE
 *  nextHopTunnelPointer  - the tunnel pointer in case this is a tunnel start
 *                          isTunnelStart = GT_TRUE
 *  nextHopDataBankNumber - The bank number of the associated IPv6-Route-Data entry
 *                          NOTE: Relevant only for entries of type CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E
 *  scopeCheckingEnable   - Enable IPv6 Scope Checking.
 *  siteId                - The site id of this route entry.
 *
 */
typedef struct
{
    /* field used for IPv4 and IPv6*/
    GT_BOOL                             ttlHopLimitDecEnable;
    GT_BOOL                             ttlHopLimDecOptionsExtChkByPass;
    GT_BOOL                             ingressMirror;
    GT_U32                              ingressMirrorToAnalyzerIndex;
    GT_BOOL                             qosProfileMarkingEnable;
    GT_U32                              qosProfileIndex;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT qosPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT       modifyDscp;
    CPSS_IP_CNT_SET_ENT                 countSet;
    GT_BOOL                             trapMirrorArpBcEnable;
    GT_U32                              dipAccessLevel;
    GT_BOOL                             ICMPRedirectEnable;
    GT_U32                              mtuProfileIndex;
    GT_BOOL                             isTunnelStart;
    GT_U16                              nextHopVlanId;
    GT_U32                              nextHopARPPointer;
    GT_U32                              nextHopTunnelPointer;

    /* fields used for ipv6 only */
    GT_U32                              nextHopDataBankNumber;
    GT_BOOL                             scopeCheckingEnable;
    CPSS_IP_SITE_ID_ENT                 siteId;

} CPSS_FDB_UC_ROUTING_INFO_STC;

/*
 * typedef: struct CPSS_MAC_ENTRY_EXT_STC
 *
 * Description: Extension to MAC Address Entry
 *
 * Fields:
 *   key        - key data, depends on the type of the MAC entry
 *                MAC entry -> mac address + vlan ID
 *                IP Multicast entry -> srcIP + dstIP + vlan ID
 *                IPv4 UC  entry -> dstIP + prefixLen + VRF_ID
 *                IPv6 UC  Full entry -> dstIP + prefixLen + VRF_ID
 *   dstInterface - destination interface : port/trunk/vidx
 *                  For IPv4 and IPv6 multicast entries only VLAN or
 *                  VIDX is used.
 *                  Relevant for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST/IPV4_UC/IPV6_UC
 *                  for key of type IPV4_UC/IPV6_UC this field is the next hop interface
 *   isStatic   - static/dynamic entry
 *                Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   daCommand  - action taken when a packet's DA matches this entry
 *                Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   saCommand  - action taken when a packet's SA matches this entry.
 *                For IPv4 and IPv6 multicast entries these field is not used,
 *                it is ignored.
 *                Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   daRoute    - GT_TRUE, and packet's DA matches this entry,
 *                send packet to the IPv4 or MPLS
 *                Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   mirrorToRxAnalyzerPortEn - GT_TRUE, and packet's DA matches this entry,
 *                              mirror packet to a configurable analyzer port.
 *                Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   sourceID   - Source ID. For IPv4 and IPv6 multicast entries these field
 *                is not used, it is ignored.
 *                Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   userDefined  - user defined field [0..0xF].
 *                  for Lion2 devices: if the use of <MyCoreId> Field In Fdb
 *                  Entry is enabled then udb0 is used for <myCoreId> and the
 *                  number of bits of udb field is decreased by 1
 *                  Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   daQosIndex - Qos attribute set applied to the packet if there is a
 *                destination lookup mode. If both FDB lookups find a matching
 *                entry, whose attribute index is not NULL, a global
 *                FDB QOS Marking Conflict Resolution command selects one
 *                of the Qos attributes source or destination [0..0x7].
 *                Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   saQosIndex - Qos attribute set applied to the packet if there is a
 *                source lookup mode. If both FDB lookups find a matching
 *                entry, whose attribute index is not NULL, a global
 *                FDB QOS Marking Conflict Resolution command selects one
 *                of the Qos attributes source or destination [0..0x7].
 *                Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 * Cheetah2 fields:
 *   daSecurityLevel - security level assigned to the MAC DA that matches
 *                     this entry [0..0x7].
 *                     Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   saSecurityLevel - security level assigned to the MAC SA that matches
 *                     this entry [0..0x7].
 *                     Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   appSpecificCpuCode - If set, and the entry <DA command> is TRAP or MIRROR
 *                        then the CPU code may be overwritten according
 *                        to the Application Specific CPU Code assignment
 *                        mechanism.
 *                        Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   age             - Age flag that is used for the two-step Aging process.
 *                     GT_FALSE - The entry will be aged out in the next pass.
 *                     GT_TRUE - The entry will be aged-out in two age-passes.
 *                     Notes: used only for DxCh devices.
 *                            IP UC entries are not aged out, but only marked as such
 *                     Relevant for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST/IPV4_UC/IPV6_UC
 *   spUnknown       - Relevant when auto-learning is disabled and
 *                     New Address (NA) storm prevention is enabled
 *                     GT_FALSE - Regular Entry;
 *                     GT_TRUE  - Storm Prevention Entry;
 *                     This is a storm prevention entry indicating
 *                     that a NA message has been sent to the CPU
 *                     but the CPU has not yet learned this MAC Address
 *                     on its current location. The device does not send
 *                     further NA messages to the CPU for this source
 *                     MAC Address. Should a MAC DA lookup match this entry,
 *                     it is treated as an unknown Unicast packet.
 *                     Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *   saMirrorToRxAnalyzerPortEn - GT_TRUE, and packet's SA matches this entry,
 *                              mirror packet to a configurable analyzer port.
 *                              Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   daMirrorToRxAnalyzerPortEn - GT_TRUE, and packet's DA matches this entry,
 *                              mirror packet to a configurable analyzer port.
 *                              Relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST
 *                              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *  fdbRoutingInfo   - All information needed for FDB Routing
 *                     Relevant only for key of type IPV4_UC/IPV6_UC
 *                     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 */
typedef struct
{
       CPSS_MAC_ENTRY_EXT_KEY_STC   key;
       CPSS_INTERFACE_INFO_STC      dstInterface;
       GT_BOOL                      age;

       /* Fields relevant only for key of type MAC_ADDR/IPV4_MCAST/IPV6_MCAST */
           GT_BOOL                      isStatic;
           CPSS_MAC_TABLE_CMD_ENT       daCommand;
           CPSS_MAC_TABLE_CMD_ENT       saCommand;
           GT_BOOL                      daRoute;
           GT_BOOL                      mirrorToRxAnalyzerPortEn;
           GT_U32                       sourceID;
           GT_U32                       userDefined;
           GT_U32                       daQosIndex;
           GT_U32                       saQosIndex;
           GT_U32                       daSecurityLevel;
           GT_U32                       saSecurityLevel;
           GT_BOOL                      appSpecificCpuCode;
           GT_BOOL                      spUnknown;
           GT_BOOL                      saMirrorToRxAnalyzerPortEn;
           GT_BOOL                      daMirrorToRxAnalyzerPortEn;

       /* Relevant only for key of type IPV4_UC/IPV6_UC */
           CPSS_FDB_UC_ROUTING_INFO_STC    fdbRoutingInfo;

} CPSS_MAC_ENTRY_EXT_STC;


/*
 * typedef: struct CPSS_MAC_UPDATE_MSG_EXT_STC
 *
 * Description: MAC Table Update message exchanged between the Prestera
 *              device and CPU.
 *
 * Fields:
 *    updType    - update message type (NA, QA, QR, AA, TA, SA, QI)
 *    entryWasFound - entry found or not flag (relevant only to QR/QI msg to CPU)
 *    macEntryIndex - the entry index in the MAC table in which the MAC address
 *                    resides
 *                    relevant only for QI msg & entryFound = 1)
 *    macEntry - FDB entry format for MAC Address and IP Multicast entries
 *    skip     - the skip bit - GT_TRUE  - entry not used by HW
 *                              GT_FALSE - entry used by HW
 *    aging    - the age bit - GT_TRUE  - the entry is "refresh" (not "aged" by
 *                             aging cycle since last traffic on the entry)
 *                             GT_FALSE - the entry has "aged" by 1 aging cycle
 *    associatedHwDevNum  - the HW device num that entry associated with
 *                      NOTE: this value is valid for all type of entries:
 *                      unicast on port /  unicast on trunk /using Vidx
 *    queryHwDevNum       - The HW Device-ID of the QA/QR message sender.
 *                      Relevant ONLY for QA/QR messages between Prestera Packet
 *                      Processors.
 *    naChainIsTooLong - indicates that NA message contains a MAC Address
 *                       that cannot be retained because the chain is too long.
 *                       GT_TRUE - the length of the FDB chain is too long
 *                       GT_FALSE - the length of the FDB  chain is not too long
 *    entryOffset      - The FDB entry offset relative to the hash index for
 *                       this address. This is valid in the following cases:
 *                       1. NA messages, when naChainIsTooLong = 0.
 *                       2. QR messages, when entryFound = 1.
 *    portGroupId      - port group Id.indicating the port group from which the AU/FU message
 *                       came.
 *                       relevant ONLY to multi-port groups device.
 *    vid1             - Relevant when message type is NA.
 *                       VLAN Id of Tag1 [0..0xFFF], assigned by the device's
 *                       ingress pipe VLAN assignment mechanisms.
 *                       APPLICABLE DEVICES: xCat2
 *    up0              - Relevant when message type is NA.
 *                       Tag0 User Priority value [0..7], assigned by the device's
 *                       ingress pipe QOS assignment mechanisms.
 *                       APPLICABLE DEVICES: xCat2
 *    isMoved          - Relevant when message type is NA.
 *                       GT_TRUE: this address is a moved one (the MAC address
 *                       is associated with new device/port or trunk).
 *                       GT_FALSE: MAC address is new one or already exists in FDB
 *                       and it's source is not moved.
 *                       APPLICABLE DEVICES: xCat2
 *    oldSrcId         - Relevant when isMoved = GT_TRUE.
 *                       Source Id of the old address [0..0x1F].
 *                       APPLICABLE DEVICES: xCat2
 *    oldDstInterface  - Relevant when isMoved = GT_TRUE.
 *                       The associated information with old entry.
 *                       Valid interface types: port or trunk.
 *                       APPLICABLE DEVICES: xCat2
 *    oldAssociatedHwDevNum - Relevant when isMoved = GT_TRUE.
 *                       The old HW device num that entry associated with.
 *                       APPLICABLE DEVICES: xCat2
 */
typedef struct
{
    CPSS_UPD_MSG_TYPE_ENT       updType;
    GT_BOOL                     entryWasFound;
    GT_U32                      macEntryIndex;
    CPSS_MAC_ENTRY_EXT_STC      macEntry;
    GT_BOOL                     skip;
    GT_BOOL                     aging;
    GT_HW_DEV_NUM               associatedHwDevNum;
    GT_HW_DEV_NUM               queryHwDevNum;
    GT_BOOL                     naChainIsTooLong;
    GT_U32                      entryOffset;
    GT_U32                      portGroupId;
    GT_U16                      vid1;
    GT_U32                      up0;
    GT_BOOL                     isMoved;
    GT_U32                      oldSrcId;
    CPSS_INTERFACE_INFO_STC     oldDstInterface;
    GT_HW_DEV_NUM               oldAssociatedHwDevNum;
} CPSS_MAC_UPDATE_MSG_EXT_STC;

/*
 * typedef: enum CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT
 *
 * Description: Enum for Secure Automatic Learning modes.
 *
 * Enumerations:
 *      CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E - Disable Secure Automatic learning.
 *      CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E - enable secure auto learning, trap
 *                             to CPU packets with unknown MAC DA Trap to CPU.
 *      CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E - enable secure auto learning,
 *                                      soft drop packets with unknown MAC DA.
 *      CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E - enable secure auto learning,
 *                                      hard drop packets with unknown MAC DA.
 *      CPSS_MAC_SECURE_AUTO_LEARN_UNK_FORWARD_E - enable secure auto learning,
 *                                      packet forwarded without Automatic learning.
 *                                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3; Puma2; Puma3)
 *      CPSS_MAC_SECURE_AUTO_LEARN_UNK_MIRROR_TO_CPU_E - enable secure auto learning,
 *                                      packet forwarded and mirrored to CPU without
 *                                      Automatic learning.
 *                                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 */

typedef enum
{
   CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E,
   CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E,
   CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E,
   CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E,
   CPSS_MAC_SECURE_AUTO_LEARN_UNK_FORWARD_E,
   CPSS_MAC_SECURE_AUTO_LEARN_UNK_MIRROR_TO_CPU_E
} CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT;


/*
 * typedef: enum CPSS_MAC_HASH_FUNC_MODE_ENT
 *
 * Description: Enum for FDB Hash function modes.
 *
 * Enumerations:
 *      CPSS_MAC_HASH_FUNC_XOR_E - XOR based hash function mode,
 *                                 provides optimal hash index distribution
 *                                 for controlled testing scenarios, where
 *                                 sequential addresses and vlans are often used.
 *      CPSS_MAC_HASH_FUNC_CRC_E - CRC based hash function mode,
 *                                 provides the best hash index distribution
 *                                 for random addresses and vlans.
 *      CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E - Different CRC hash functions per FDB bank.
 *                                 (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 */

typedef enum
{
   CPSS_MAC_HASH_FUNC_XOR_E,
   CPSS_MAC_HASH_FUNC_CRC_E,
   CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E

} CPSS_MAC_HASH_FUNC_MODE_ENT;

/*
 * typedef: enum CPSS_FDB_ACTION_MODE_ENT
 *
 * Description: Enum for FDB Action modes.
 *
 * Enumerations:
 *      CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E - Automatic aging with removal of
 *                                           aged out entries.
 *      CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E - Automatic or triggered aging
 *                                           without removal of aged out
 *                                           entries or triggered FDB Upload.
 *      CPSS_FDB_ACTION_DELETING_E - Triggered Address deleting
 *      CPSS_FDB_ACTION_TRANSPLANTING_E - Triggered Address Transplanting
 */
typedef enum{
    CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E,
    CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E,
    CPSS_FDB_ACTION_DELETING_E,
    CPSS_FDB_ACTION_TRANSPLANTING_E
} CPSS_FDB_ACTION_MODE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenBrgFdbh */

