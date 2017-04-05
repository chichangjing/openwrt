/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPcl.h
*
* DESCRIPTION:
*       CPSS DxCh PCL lib API.
*
* FILE REVISION NUMBER:
*       $Revision: 91 $
*******************************************************************************/
#ifndef __cpssDxChPclh
#define __cpssDxChPclh

#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/generic/pcl/cpssPcl.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Size of Bobcat2; Caelum; Bobcat3 action is 8 words                    */
/* Size of DxCh3, xCat, xCat3, Lion, xCat2 , Lion2 action is 4 words */
#define CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS 8

/* Rule entry size in words - defined by Bobcat2; Caelum; Bobcat3 size 7 x 3 words */
#define CPSS_DXCH_PCL_RULE_SIZE_IN_WORDS_CNS  21

/*
 * Typedef: enum CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT
 *
 * Description:
 *     This enum describes the type of second sublookup of Ingress PCL lookup0.
 * Enumerations:
 *     CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_PARTIAL_E  - Partial lookup -
 *                  in IPCL0-1 the Configuration table is accesses with the
 *                  same index as the IPCL0-0 index.
 *     CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_FULL_E     - Full lookup -
 *                  IPCL0-1 lookup is a full lookup that includes a Policy
 *                  Configuration table lookup and a new key creation.
 *                  IPCL0-1 accesses the IPCL0 Configuration table, and uses the
                    <Enable Lookup0-1> and <PCL-ID for Lookup0-1> fields.
 *                  Notice that IPCL0-1 and IPCL0-0 may access different entries
 *                  in the configuration table, depending on the configuration
 *                  table lookup mechanism.
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_PARTIAL_E,
    CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_FULL_E
} CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_RULE_OPTION_ENT
 *
 * Description:
 *      Enumerator for the TCAM rule write option flags.
 *
 * Enumerations:
 *      CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E   - write all fields
 *                  of rule to TCAM but rule state is invalid
 *                  (no match during lookups).
 *                  The cpssDxChPclRuleValidStatusSet can turn the rule
 *                  to valid state.
 *                  (APPLICABLE DEVICES: DxCh3, xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E      = (0x1 << 0)

} CPSS_DXCH_PCL_RULE_OPTION_ENT;

/* Action definitions for Policy Rules */

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_MIRROR_STC
 *
 * Description:
 *     This structure defines the mirroring related Actions.
 *
 * Fields:
 *       mirrorToRxAnalyzerPort - Enables mirroring the packet to
 *              the ingress analyzer interface.
 *              GT_FALSE - Packet is not mirrored to ingress analyzer interface.
 *              GT_TRUE  - Packet is mirrored to ingress analyzer interface.
 *       ingressMirrorToAnalyzerIndex - index of analyzer interface
 *              (APPLICABLE RANGES: 0..6)
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                  Relevant only when (mirrorToRxAnalyzerPort == GT_TRUE)
 *       cpuCode                - The CPU code assigned to packets
 *              Mirrored to CPU or Trapped to CPU due
 *              to a match in the Policy rule entry
 *
 *              NOTE: the CPU code also acts as 'drop code' for soft/hard drop commands.
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *       mirrorTcpRstAndFinPacketsToCpu - TCP RST and FIN packets
 *              (i.e. TCP disconnect cases) can be mirrored to CPU
 *              GT_TRUE  - TCP RST and FIN packets mirrored to CPU.
 *              GT_FALSE - TCP RST and FIN packets not mirrored to CPU.
 *              Relevant for xCat and above devices only.
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_NET_RX_CPU_CODE_ENT      cpuCode;
    GT_BOOL                       mirrorToRxAnalyzerPort;
    GT_U32                        ingressMirrorToAnalyzerIndex;
    GT_BOOL                       mirrorTcpRstAndFinPacketsToCpu;
} CPSS_DXCH_PCL_ACTION_MIRROR_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC
 *
 * Description:
 *     This structure defines the using of rule match counter.
 *
 * Fields:
 *       enableMatchCount  - Enables the binding of this
 *                  policy action entry to the Policy Rule Match Counter<n>
 *                  (0<=n<32) indexed by matchCounterIndex
 *                  GT_FALSE  = Match counter binding is disabled.
 *                  GT_TRUE = Match counter binding is enabled.
 *
 *       matchCounterIndex - A index one of the 32 Policy Rule Match Counter<n>
 *                 (0<=n<32) The counter is incremented for every packet
 *                 satisfying both of the following conditions:
 *                    - Matching this rule.
 *                    - The previous packet command is not hard drop.
 *                  NOTE: for DxCh3 and above (APPLICABLE RANGES: 0..(16K-1))
 *
 *  Comments:
 *      DxCh3 and above: the CNC mechanism is used for the match counters.
 *
 */
typedef struct
{
    GT_BOOL   enableMatchCount;
    GT_U32    matchCounterIndex;
} CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC;


/*
 * Typedef: enum CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ENT
 *
 * Description: Enumerator for ingress Tag1 UP assignment command.
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_DO_NOT_MODIFY_E -
 *                         Do not modify the <UP1> assigned to the packet
 *                         by previous engines.
 *  CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_TAG1_UNTAGGED_E -
 *                         If packet does not contain Tag1
 *                         assign according to action entry’s <UP1>,
 *                         else retain previous engine <UP1> assignment
 *  CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_TAG0_UNTAGGED_E -
 *                         If packet contains Tag0 use UP0,
 *                         else use action entry’s <UP1> field.
 *  CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ALL_E                      -
 *                         Assign action entry’s <UP1> field to all packets.
 *
 * Comments:
 *   If the packet is Tunnel-terminated,
 *   the options above refer to the passenger packet.
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_DO_NOT_MODIFY_E,
    CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_TAG1_UNTAGGED_E,
    CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_TAG0_UNTAGGED_E,
    CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ALL_E
} CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT
 *
 * Description: Enumerator for egress Tag0 VID and UP assignment command.
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E             -
 *                         don't assign Tag0 value from entry.
 *  CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E     -
 *                         assign Tag0 value from entry to outer Tag.
 *                         It is Tag0 for not TS packets
 *                         or Tunnel header Tag for TS packets.
 *  CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_TAG0_E     -
 *                         assign Tag0 value from entry to Tag0
 *                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E,
    CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E,
    CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_TAG0_E
} CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_ENT
 *
 * Description: Enumerator for egress DSCP assignment command.
 *                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E             -
 *                         Keep previous packet DSCP/EXP settings.
 *  CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_MODIFY_OUTER_E     - Modify Outer
 *                         If packet is MPLS, modify EXP of
 *                         packet's outer label to <DSCP EXP>.
 *                         If the packet is tunneled, this refers to the tunnel header
 *                         Otherwise, If packet is IP, modify the packet's DSCP to <DSCP EXP>.
 *                         If the packet is tunneled, this refers to the tunnel header.
 *                         (Backward Compatible mode).
 *  CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_MODIFY_INNER_E     - Modify Inner DSCP;
 *                          If packet is IP, modify the packet's DSCP to <DSCP EXP>.
 *                          If the packet is tunneled, this refers to the passenger header.
 *                          This mode is not applicable for MPLS packets;
 *
 */
typedef enum
{
   CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E = GT_FALSE,
   CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_MODIFY_OUTER_E = GT_TRUE,
   CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_MODIFY_INNER_E
} CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_ENT;


/*
 * Typedef: union CPSS_DXCH_PCL_ACTION_QOS_UNT
 *
 * Description:
 *     This union defines the packet's QoS attributes mark Actions.
 *
 * Fields:
 *
 *    ingress members:
 *    modifyDscp             - The Modify DSCP (or EXP for MPLS) QoS attribute of the packet.
 *                             Enables modification of the packet's DSCP field.
 *                             Only relevant if QoS precedence of the previous
 *                             QoS assignment mechanisms (Port, Protocol Based
 *                             QoS, and previous matching rule) is Soft.
 *                             Relevant for IPv4/IPv6 packets, only.
 *                             ModifyDSCP enables the following:
 *                               - Keep previous DSCP modification command.
 *                               - Enable modification of the DSCP field in
 *                                 the packet.
 *                               - Disable modification of the DSCP field in
 *                                 the packet.
 *    modifyUp              - The Modify UP QoS attribute of the packet.
 *                            Enables modification of the packet's
 *                            802.1p User Priority field.
 *                            Only relevant if QoS precedence of the previous
 *                            QoS assignment mechanisms  (Port, Protocol Based
 *                            QoS, and previous matching rule) is Soft.
 *                            ModifyUP enables the following:
 *                            - Keep previous QoS attribute <ModifyUP> setting.
 *                            - Set the QoS attribute <modifyUP> to 1.
 *                            - Set the QoS attribute <modifyUP> to 0.
 *    profileIndex          - The QoS Profile Attribute of the packet.
 *                             Only relevant if the QoS precedence of the
 *                             previous QoS Assignment Mechanisms (Port,
 *                             Protocol Based QoS, and previous matching rule)
 *                             is Soft and profileAssignIndex is set
 *                             to GT_TRUE.
 *                             The QoSProfile is used to index the QoSProfile
 *                             Table Entry and assign the QoS Parameters
 *                             which are TC, DP, UP, EXP and DSCP
 *                             to the packet.
 *                             Valid Range - 0..71 for DxCh1
 *                             0..127 for DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2
 *                             0..255 for Bobcat2; Caelum; Bobcat3.
 *    profileAssignIndex     - Enable marking of QoS Profile Attribute of
 *                             the packet.
 *                             GT_TRUE - Assign <profileIndex> to the packet.
 *                             GT_FALSE - Preserve previous QoS Profile setting.
 *    profilePrecedence      - Marking of the QoSProfile Precedence.
 *                             Setting this bit locks the QoS parameters setting
 *                             from being modified by subsequent QoS
 *                             assignment engines in the ingress pipe.
 *                             QoSPrecedence enables the following:
 *                             - QoS precedence is soft and the packet's QoS
 *                               parameters may be overridden by subsequent
 *                               QoS assignment engines.
 *                             - QoS precedence is hard and the packet's QoS
 *                               parameters setting is performed until
 *                               this stage is locked. It cannot be overridden
 *                               by subsequent QoS assignment engines.
 *
 *    up1Cmd                 - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                              command for setting the Tag1 UP value.
 *    up1                    - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                              The UP1 value to assign to the packet
 *                              according to up1Cmd.
 *    egress members:
 *    modifyDscp             - enable modify DSCP (or EXP for MPLS)
 *                             For xCat; xCat3; Lion; xCat2; Lion2 as Boolean:
 *                             CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_CMD_KEEP_E - don't modify DSCP.
 *                             any other - modify DSCP.
 *                             For Bobcat2; Caelum; Bobcat3 see enum comments.
 *    dscp                   - The DSCP field (or EXP for MPLS) set to the transmitted packets.
 *    modifyUp               - This field determines the Tag0 UP assigned
 *                             to the packet matching the EPCL entry.
 *    up                     - The 802.1p UP field set to the transmitted packets.
 *    up1ModifyEnable        - This field determines the Tag1 UP assigned
 *                             to the packet matching the EPCL entry.
 *                             GT_FALSE - Keep: Keep previous Tag1 UP
 *                                assigned to the packet.
 *                             GT_TRUE  - Modify: EPCL Action entry <UP1>
 *                                is assigned to tag 1 UP of the packet.
 *                            (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *    up1                    - The UP1 value to assign to Tag1 of
 *                             the packet according to <up1ModifyEnable>
 *                            (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *  Comments:
 *           Egress Policy related actions supported only for DxCh2 and above
 *           devices
 *
 */
typedef union
{
    struct
    {
        CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT         modifyDscp;
        CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT         modifyUp;
        GT_U32                                        profileIndex;
        GT_BOOL                                       profileAssignIndex;
        CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT   profilePrecedence;
        CPSS_DXCH_PCL_ACTION_INGRESS_UP1_CMD_ENT      up1Cmd;
        GT_U32                                        up1;
    } ingress;
    struct
    {
        CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_ENT  modifyDscp;
        GT_U32                                        dscp;
        CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT      modifyUp;
        GT_U32                                        up;
        GT_BOOL                                       up1ModifyEnable;
        GT_U32                                        up1;
    } egress;
} CPSS_DXCH_PCL_ACTION_QOS_UNT;

/*
 * Typedef: enum CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT
 *
 * Description: enumerator for PCL redirection target
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E   - no redirection
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E - redirection to output interface
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E - Ingress control pipe not
 *                        bypassed. If packet is triggered for routing,
 *                        Redirect the packet to the Router Lookup Translation
 *                        Table Entry specified in <routerLttIndex> bypassing
 *                        DIP lookup.
 *                        (APPLICABLE DEVICES: DxCh2, DxCh3, xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3).
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E - redirect to virtual
 *                        router with the specified Id (VRF ID)
 *                        (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3).
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_REPLACE_MAC_SA_E - replace MAC source address
 *                        with specified value.
 *                        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3).
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_LOGICAL_PORT_ASSIGN_E - assign logical source port
 *                        (APPLICABLE DEVICES: xCat; xCat3) xCat device: applicable starting from revision C0

 */
typedef enum
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_REPLACE_MAC_SA_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_LOGICAL_PORT_ASSIGN_E

} CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT
 *
 * Description: enumerator for PCL redirection types of the passenger packet.
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E - The passenger packet
 *                                                         is Ethernet.
 *  CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E - The passenger packet is IP.
 *
 */

typedef enum
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E,
    CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E

}CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_REDIRECT_STC
 *
 * Description:
 *     This structure defines the redirection related Actions.
 *
 * Fields:
 *       redirectCmd   - redirection command
 *
 *       data.outIf   - out interface redirection data
 *                      relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_IF_E
 *                      packet redirected to output interface
 *       outInterface - output interface (port, trunk, VID, VIDX)
 *       vntL2Echo    - Enables Virtual Network Tester Layer 2 Echo
 *                     (APPLICABLE DEVICES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      GT_TRUE - swap MAC SA and MAC DA in redirected packet
 *                      GT_FALSE - don't swap MAC SA and MAC DA
 *
 *       tunnelStart  - Indicates this action is a Tunnel Start point
 *                      GT_TRUE - Packet is redirected to an Egress Interface
 *                                and is tunneled as an  Ethernet-over-MPLS.
 *                      GT_FALSE - Packet is redirected to an Egress Interface
 *                                and is not tunneled.
 *                     (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond,
 *                                DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *       tunnelPtr    - the pointer to "Tunnel-start" entry
 *                      (relevant only if tunnelStart == GT_TRUE)
 *                     (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond,
 *                                DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *       tunnelType   -  tunnel type - Ethernet or IP,
 *                      (relevant only if tunnelStart == GT_TRUE)
 *                      (APPLICABLE DEVICES: DxCh3, xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *       arpPtr       - index in ARP table
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *       modifyMacDa  -  When enabled, the packet is marked
 *                       for MAC DA modification,
 *                       and the <ARP Index> specifies the new MAC DA.
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *       modifyMacSa  -  When enabled, the packet is marked
 *                       for MAC SA modification - similar to routed packets.
 *                      (relevant only if tunnelStart == GT_FALSE)
 *                      (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *       data.routerLttIndex  - For Bobcat2, Caelum, Bobcat3 this is a pointer to a leaf in
 *                       the LPM RAM. For other devices this is an index of IP router Lookup
 *                       Translation Table entry.
 *                       relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E
 *                      (APPLICABLE DEVICES: DxCh2, DxCh3, xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3).
 *       data.vrfId   - virtual router ID
 *                 relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E
 *                      (APPLICABLE DEVICES: DxCh3, xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3).
 *                      (APPLICABLE RANGES: 0..4095)
 *       data.modifyMacSa.arpPtr       - index in ARP table
 *                 relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_REPLACE_MAC_SA_E
 *                      (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3).
 *       data.modifyMacSa.macSa   - source MAC address update value.
 *                 relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_REPLACE_MAC_SA_E
 *                      (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3).
 *
 *       data.noRedirect              - used only for MAC DA modificaion when packet not redirected
 *       data.noRedirect.arpPtr       - index in ARP table
 *                      (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3).
 *       data.noRedirect.modifyMacDa  -  When enabled, the packet is marked
 *                       for MAC DA modification,
 *                       and the <ARP Index> specifies the new MAC DA.
 *                      (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3).
 *
 *       data.logicalSourceInterface        - logical Source Port
 *                 Relevant when VPLS mode init parameter is enabled.
 *                 relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_LOGICAL_PORT_ASSIGN_E
 *                 (APPLICABLE DEVICES: xCat, xCat3) xCat device: applicable starting from revision C0
 *                 members:
 *       data.logicalSourceInterface.logicalInterface       - logical interface
 *                 Type must be Port, Device Number and Port Number - relevant data.
 *       data.logicalSourceInterface.sourceMeshIdSetEnable  - if set,
 *                 then the <sourceMeshId> is assign to the packet.
 *       data.logicalSourceInterface.sourceMeshId           - Source Mesh ID
 *                 assigned to the packet if <sourceMeshIdSetEnable> is GT_TRUE
 *                 (APPLICABLE RANGES: 0..3).
 *                 0 used to indicate packet arrived on AC, and Split Horizon filtering
 *                 is not performed in L2MLL.
 *                 1,2,3 = if matching MLL action entry<MeshID> descriptor is not duplicated.
 *                 Used for split horizon filtering.
 *       data.logicalSourceInterface.userTagAcEnable        - When set,
 *                 To retain the packet's Tag0 VLAN-ID assignment for egress tagging
 *                 and VLAN filtering, this bit should be set.
 *                 For non-service delimiting ACs, the Tag0 VLAN-ID is the packet's
 *                 C-Tag if it arrived tagged, or the original port default VLAN-ID assignment
 *                 if it arrived untagged.
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT  redirectCmd;
    union
    {
        struct
        {
            CPSS_INTERFACE_INFO_STC  outInterface;
            GT_BOOL                  vntL2Echo;
            GT_BOOL                  tunnelStart;
            GT_U32                   tunnelPtr;
            CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT tunnelType;
            GT_U32                   arpPtr;
            GT_BOOL                  modifyMacDa;
            GT_BOOL                  modifyMacSa;
        } outIf;
        GT_U32                       routerLttIndex;
        GT_U32                       vrfId;
        struct
        {
            GT_U32                   arpPtr;
            GT_ETHERADDR             macSa;
        } modifyMacSa;
        struct
        {
            GT_U32                   arpPtr;
            GT_BOOL                  modifyMacDa;
        } noRedirect;
        struct
        {
            CPSS_INTERFACE_INFO_STC  logicalInterface;
            GT_BOOL                  sourceMeshIdSetEnable;
            GT_U32                   sourceMeshId;
            GT_BOOL                  userTagAcEnable;
        } logicalSourceInterface;
    } data;

} CPSS_DXCH_PCL_ACTION_REDIRECT_STC;


/*
 * Typedef: enum CPSS_DXCH_PCL_POLICER_ENABLE_ENT
 *
 * Description: enumerator for policer options.
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E              - Meter and Counter are disabled.
 *  CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E - Both Meter and Counter enabled.
 *  CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E        - Meter only enabled.
 *  CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E      - Counter only enabled.
 *
 */

typedef enum
{
    CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E              = GT_FALSE,
    CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E = GT_TRUE,
    CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E,
    CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E
} CPSS_DXCH_PCL_POLICER_ENABLE_ENT;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_POLICER_STC
 *
 * Description:
 *     This structure defines the policer related Actions.
 *
 * Fields:
 *       policerEnable  - policer enable,
 *                DxCh1, DxCh2 and DxCh3 supports only the following commands:
 *                - CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E or GT_FALSE
 *                - CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E or GT_TRUE
 *                xCat and above devices support all values of the enum
 *       policerId      - policers table entry index
 *
 *  Comments:
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_POLICER_ENABLE_ENT  policerEnable;
    GT_U32   policerId;

} CPSS_DXCH_PCL_ACTION_POLICER_STC;

/*
 * Typedef: enum CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ENT
 *
 * Description: Enumerator for ingress Tag1 VID assignment command.
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_DO_NOT_MODIFY_E -
 *                         Do not modify Tag1 VID.
 *  CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_UNTAGGED_E     -
 *                         The entry’s <Tag1 VID> is assigned as Tag1 VID for
 *                         packets that do not have Tag1
 *                         or Tag1-priority tagged packets.
 *                         For packets received with Tag1 VID,
 *                         retain previous engine assignment.
 *  CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ALL_E                      -
 *                         Assign action entry’s <VID1> field to all packets.
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_DO_NOT_MODIFY_E,
    CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_UNTAGGED_E,
    CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ALL_E
} CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ENT;

/*
 * Typedef: union CPSS_DXCH_PCL_ACTION_VLAN_UNT
 *
 * Description:
 *     This union defines the VLAN modification related Actions.
 *
 * Fields:
 *       INGRESS fields:
 *       modifyVlan  - VLAN id modification command
 *
 *       nestedVlan  -  When this field is set to GT_TRUE, this rule matching
 *                      flow is defined as an access flow. The VID of all
 *                      packets received on this flow is discarded and they
 *                      are assigned with a VID using the device's VID
 *                      assignment algorithms, as if they are untagged. When
 *                      a packet received on an access flow is transmitted via
 *                      a core port or a Cascading port, a VLAN tag is added
 *                      to the packet (on top of the existing VLAN tag, if
 *                      any). The VID field is the VID assigned to the packet
 *                      as a result of all VLAN assignment algorithms. The
 *                      802.1p User Priority field of this added tag may be
 *                      one of the following, depending on the ModifyUP QoS
 *                      parameter set to the packet at the end of the Ingress
 *                      pipe:
 *                      - If ModifyUP is GT_TRUE, it is the UP extracted
 *                      from the QoSProfile to QoS Table Entry<n>
 *                      - If ModifyUP is GT_FALSE, it is the original packet
 *                      802.1p User Priority field if it is tagged and is UP
 *                      if the original packet is untagged.
 *
 *       vlanId      - VLAN id used for VLAN id modification if command
 *                     not CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
 *       precedence  - The VLAN Assignment precedence for the subsequent
 *                     VLAN assignment mechanism, which is the Policy engine
 *                     next policy-pass rule. Only relevant if the
 *                     VID precedence set by the previous VID assignment
 *                     mechanisms (Port, Protocol Based VLANs, and previous
 *                     matching rule) is Soft.
 *                     - Soft precedence The VID assignment can be overridden
 *                       by the subsequent VLAN assignment mechanism,
 *                       which is the Policy engine.
 *                     - Hard precedence The VID assignment is locked to the
 *                       last VLAN assigned to the packet and cannot be overridden.
 *       vlanId1Cmd  - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                     This field determines the Tag1 VID assigned
 *                     to the packet matching the IPCL entry.
 *       vlanId1     - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                     The Tag1 VID to assign to the packet
 *                     according to <vlanId1Cmd>.
 *       EGRESS fields:
 *       vlanCmd     - This field determines the Tag0 VID command
 *                     assigned to the packet matching the EPCL entry.
 *       vlanId      - The VID to assign to Tag0
 *                     of the packet according to <vlanCmd>
 *       vlanId1ModifyEnable - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                     This field determines the Tag1 VID
 *                     assigned to the packet matching the EPCL entry.
 *                     GT_TRUE - modify, GT_FALSE - don't modify.
 *       vlanId1     - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                     The VID to assign to Tag1 of the packet
 *                     according to <vlanId1ModifyEnable>.
 *
 *  Comments:
 *
 */
typedef union
{
    struct
    {
        CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT           modifyVlan;
        GT_BOOL                                        nestedVlan;
        GT_U32                                         vlanId;
        CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT    precedence;
        CPSS_DXCH_PCL_ACTION_INGRESS_VLAN_ID1_CMD_ENT  vlanId1Cmd;
        GT_U32                                         vlanId1;
    } ingress;
    struct
    {
        CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_ENT       vlanCmd;
        GT_U32                                         vlanId;
        GT_BOOL                                        vlanId1ModifyEnable;
        GT_U32                                         vlanId1;
    } egress;
} CPSS_DXCH_PCL_ACTION_VLAN_UNT;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC
 *
 * Description:
 *     This structure defines the IP unicast route parameters.
 *     Dedicated to override the relevant field of
 *     general action definitions.
 *
 * APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, xCat, xCat3, Lion, xCat2, Lion2.
 *
 * Fields:
 *       doIpUcRoute       - Configure IP Unicast Routing Actions
 *                           GT_TRUE - the action used for IP unicast routing
 *                           GT_FALSE - the action is not used for IP unicast
 *                                      routing, all data of the structure
 *                                      ignored.
 *       arpDaIndex        - Route Entry ARP Index to the ARP Table (10 bit)
 *       decrementTTL      - Decrement IPv4 <TTL> or IPv6 <Hop Limit> enable
 *                           GT_TRUE - TTL Decrement for routed packets is enabled
 *                           GT_FALSE - TTL Decrement for routed packets is disabled
 *       bypassTTLCheck    - Bypass Router engine TTL and Options Check
 *                           GT_TRUE - the router engine bypasses the
 *                                     IPv4 TTL/Option check and the IPv6 Hop
 *                                     Limit/Hop-by-Hop check. This is used for
 *                                     IP-TO-ME host entries, where the packet
 *                                     is destined to this device
 *                           GT_FALSE - the check is not bypassed
 *       icmpRedirectCheck - ICMP Redirect Check Enable
 *                           GT_TRUE - the router engine checks if the next hop
 *                                     VLAN is equal to the packet VLAN
 *                                     assignment, and if so, the packet is
 *                                     mirrored to the CPU, in order to send an
 *                                     ICMP Redirect message back to the sender.
 *                          GT_FALSE - the check disabled
 *
 *  Comments:
 *  To configure IP Unicast route entry next elements of the action struct
 *  should be configured
 *    1. doIpUcRoute set to GT_TRUE
 *    2. redirection action set to CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E
 *      Out Interface data should be configured too.
 *    3. Policer should be disabled
 *    4. VLAN command CPSS_DXCH_PCL_ACTION_VLAN_CMD_MODIFY_ALL_E
 *    5. Nested VLAN should be disabled
 *    6. The packet command <pktCmd> should be set according to route entry
 *        purpose:
 *        - SOFT_DROP       - packet is dropped
 *        - HARD_DROP       - packet is dropped
 *        - TRAP            - packet is trapped to CPU with CPU code
 *                            IPV4_UC_ROUTE or IPV6_UC_ROUTE
 *        - FORWARD         - packet is routed
 *        - MIRROR_TO_CPU   - packet is routed and mirrored to the CPU with
 *                            CPU code IPV4_UC_ROUTE or IPV6_UC_ROUTE
 *
 */
typedef struct
{
    GT_BOOL  doIpUcRoute;
    GT_U32   arpDaIndex;
    GT_BOOL  decrementTTL;
    GT_BOOL  bypassTTLCheck;
    GT_BOOL  icmpRedirectCheck;

} CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC
 *
 * Description:
 *     This structure defines packet Source Id assignment.
 *
 * Fields:
 *     assignSourceId  - assign Source Id enable:
 *                     - GT_TRUE  - Assign Source Id.
 *                     - GT_FALSE - Don't assign Source Id.
 *     sourceIdValue   - the Source Id value to be assigned
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL  assignSourceId;
    GT_U32   sourceIdValue;
} CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_OAM_STC
 *
 * Description:
 *     This structure defines packet OAM relevant fields.
 *
 * Fields:
 *     timeStampEnable  - indicates that a timestamp should be inserted into the
 *                        packet:
 *                      - GT_TRUE  - enable timestamp insertion.
 *                      - GT_FALSE - disable timestamp insertion.
 *     offsetIndex      - When <Timestamp Enable> is enabled, indicates
 *                        the offset index for the timestamp offset table.
 *                        (APPLICABLE RANGES: 0..15)
 *     oamProcessEnable - binds the packet to an entry in the OAM Table.
 *                      - GT_TRUE  - enable OAM packet processing.
 *                      - GT_FALSE - disable OAM packet processing.
 *     oamProfile       - determines the set of UDBs where the key attributes
 *                        (opcode, MEG level, RDI, MEG level) of the OAM message
 *                        are taken from.
 *                        (APPLICABLE RANGES: 0..1)
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL  timeStampEnable;
    GT_U32   offsetIndex;
    GT_BOOL  oamProcessEnable;
    GT_U32   oamProfile;
} CPSS_DXCH_PCL_ACTION_OAM_STC;

/*
 * Typedef: enum CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT
 *
 * Description: enumerator Controls the index used for IPCL lookup.
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E   - Retain;
 *                         use SrcPort/VLAN <Lookup PCL Cfg Mode>.
 *  CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E - Override;
 *                         use TTI/PCL Action <ipclConfigIndex>.
 *
 */

typedef enum
{
    CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E,
    CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E
} CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC
 *
 * Description:
 *     This structure defines future lookups configuration.
 *
 * Fields:
 *      ipclConfigIndex - index of Pcl Configuration table for next lookup.
 *                     0 means no value to update.
 *      pcl0_1OverrideConfigIndex - algorithm of selection
 *                     index of PCL Cfg Table for IPCL lookup0_1
 *      pcl1OverrideConfigIndex   - algorithm of selection
 *                     index of PCL Cfg Table for IPCL lookup1
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_U32                                 ipclConfigIndex;
    CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT  pcl0_1OverrideConfigIndex;
    CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT  pcl1OverrideConfigIndex;
} CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC
 *
 * Description:
 *     This structure defines Source ePort Assignment.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Fields:
 *      assignSourcePortEnable - GT_FALSE - keep the incoming source ePort.
 *                             - GT_TRUE  - Assign source ePort from sourcePortValue.
 *      sourcePortValue        - Source ePort value.
 *
 *  Comments:
 *
 */
typedef struct
{
    GT_BOOL  assignSourcePortEnable;
    GT_U32   sourcePortValue;
} CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_ACTION_STC
 *
 * Description:
 *      Policy Engine Action
 *
 * Fields:
 *      pktCmd       -  packet command (forward, mirror-to-cpu,
 *                      hard-drop, soft-drop, or trap-to-cpu)
 *      actionStop   - Action Stop
 *                     GT_TRUE  - to skip the following PCL lookups
 *                     GT_FALSE - to continue with following PCL lookups
 *                     Supported by DxCh3 and xCat above devices.
 *                     DxCh1 and DxCh2 ignores the field.
 *                     Relevant to Policy Action Entry only.
 *      bypassBridge   - the Bridge engine processed or bypassed
 *                     GT_TRUE  - the Bridge engine is bypassed.
 *                     GT_FALSE - the Bridge engine is processed.
 *      bypassIngressPipe - the ingress pipe bypassed or not.
 *                     GT_TRUE  - the ingress pipe is bypassed.
 *                     GT_FALSE - the ingress pipe is not bypassed.
 *      egressPolicy - GT_TRUE  - Action is used for the Egress Policy
 *                     GT_FALSE - Action is used for the Ingress Policy
 *      lookupConfig - configuration of IPCL lookups.
 *      mirror       -  packet mirroring configuration
 *                      Relevant for ingress actions only.
 *      matchCounter -  match counter configuration
 *      qos          -  packet QoS attributes modification configuration
 *      redirect     -  packet Policy redirection configuration
 *                      Relevant for ingress actions only.
 *      policer      -  packet Policing configuration
 *                      Relevant to Policy Action Entry only.
 *      vlan         -  packet VLAN modification configuration
 *      ipUcRoute    -  special DxCh (not relevant for DxCh2 and above) Ip Unicast Route
 *                      action parameters configuration.
 *                      Relevant to Unicast Route Entry only.
 *                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; xCat; xCat3; Lion; xCat2; Lion2)
 *      sourceId     -  packet source Id assignment
 *                      Relevant to Policy Action Entry only.
 *      oam          -  OAM relevant fields. (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      flowId       -  flow Id. (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                      Relevant for ingress actions only.
 *      sourcePort   -  source ePort Assignment.
 *                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      setMacToMe   -  GT_FALSE - Do not override Mac2Me mechanism.
 *                   -  GT_TRUE  - Override the Mac2Me mechanism and set Mac2Me to 0x1.
 *                      Ingress PCL Only.
 *                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      channelTypeToOpcodeMapEnable - When enabled, the MPLS G-ACh Channel Type is mapped
 *                      to an OAM Opcode that is used by the OAM engine.
 *                   -  GT_FALSE - Disable.
 *                   -  GT_TRUE  - Enable.
 *                      Egress PCL Only.
 *                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      tmQueueId    -  TM Queue ID assignment.
 *                      This value is assigned to the outgoing descriptor.
 *                      It may be subsequently modified by the TM Q-Mapper.
 *                      Egress PCL Only.
 *                      (APPLICABLE DEVICES: Bobcat2; Caelum)
 *      unknownSaCommandEnable - GT_TRUE - Assign the unknown-SA-command by unknownSaCommand
 *                               GT_FALSE configure action to preserve previous assignment.
 *                      Relevant when VPLS mode init parameter is enabled.
 *                      (APPLICABLE DEVICES: xCat, xCat3) xCat device: applicable starting from revision C0
 *      unknownSaCommand - Assigns the unknown-SA-command of a packet per incoming traffic flow.
 *                      Relevant when unknownSaCommandEnable == GT_TRUE.
 *                      Relevant when VPLS mode init parameter is enabled.
 *                      (APPLICABLE DEVICES: xCat, xCat3) xCat device: applicable starting from revision C0
 *
 * Comment:
 */
typedef struct
{
    CPSS_PACKET_CMD_ENT                    pktCmd;
    GT_BOOL                                actionStop;
    GT_BOOL                                bypassBridge;
    GT_BOOL                                bypassIngressPipe;
    GT_BOOL                                egressPolicy;
    CPSS_DXCH_PCL_ACTION_LOOKUP_CONFIG_STC lookupConfig;
    CPSS_DXCH_PCL_ACTION_MIRROR_STC        mirror;
    CPSS_DXCH_PCL_ACTION_MATCH_COUNTER_STC matchCounter;
    CPSS_DXCH_PCL_ACTION_QOS_UNT           qos;
    CPSS_DXCH_PCL_ACTION_REDIRECT_STC      redirect;
    CPSS_DXCH_PCL_ACTION_POLICER_STC       policer;
    CPSS_DXCH_PCL_ACTION_VLAN_UNT          vlan;
    CPSS_DXCH_PCL_ACTION_IP_UC_ROUTE_STC   ipUcRoute;
    CPSS_DXCH_PCL_ACTION_SOURCE_ID_STC     sourceId;
    CPSS_DXCH_PCL_ACTION_OAM_STC           oam;
    GT_U32                                 flowId;
    CPSS_DXCH_PCL_ACTION_SOURCE_PORT_STC   sourcePort;
    GT_BOOL                                setMacToMe;
    GT_BOOL                                channelTypeToOpcodeMapEnable;
    GT_U32                                 tmQueueId;
    GT_BOOL                                unknownSaCommandEnable;
    CPSS_PACKET_CMD_ENT                    unknownSaCommand;
} CPSS_DXCH_PCL_ACTION_STC;


/*
 * Typedef: enum CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT
 *
 * Description:
 *     This enum describes possible formats of Policy rules.
 * Enumerations:
 *     key formats for Ingress
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E    - Standard (24B) L2
 *                          AKA - ingress Key #0
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E - Standard (24B)
 *                          L2+IPv4/v6 QoS
 *                          AKA - ingress Key #1
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E   - Standard (24B)
 *                          IPv4+L4
 *                          AKA - ingress Key #2
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E  - Standard (24B)
 *                          DxCh specific IPV6 DIP (used for routing)
 *                          AKA - ingress Key #12
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E  - Extended (48B)
 *                          L2+IPv4 + L4
 *                          AKA - ingress Key #4
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E   - Extended (48B)
 *                          L2+IPv6
 *                          AKA - ingress Key #5
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E   - Extended (48B)
 *                          L4+IPv6
 *                          AKA - ingress Key #6
 *     key formats for Egress (DxCh2 and above device)
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E     - Standard (24B) L2
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E  - Standard (24B)
 *                          L2+IPv4/v6 QoS
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E    - Standard (24B)
 *                          IPv4+L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E   - Extended (48B)
 *                          L2+IPv4 + L4
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E    - Extended (48B)
 *                          L2+IPv6
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E    - Extended (48B)
 *                          L4+IPv6
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E       - standard UDB key (24B)
 *                          AKA - ingress Key #3
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E       - extended UDB key (48B)
 *                          AKA - ingress Key #7
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E  -   (ingress)
 *        Standard (24B) Routed ACL+QoS+IPv4
 *        AKA - ingress Key #10
 *        CH3 : key for IPv4 packets used in first lookup
 *        (APPLICABLE DEVICES: DxCh3, xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E  -    (ingress)
 *        Extended (48B) Port/VLAN+QoS+IPv4
 *        AKA - ingress Key #8
 *        CH3 : key for IPv4 packets use in second lookup
 *        (APPLICABLE DEVICES: DxCh3, xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E -   (ingress)
 *        Ultra (72B) Port/VLAN+QoS+IPv6
 *        AKA - ingress Key #9
 *        CH3 : key for IPv6 packets used in first lookup
 *        (APPLICABLE DEVICES: DxCh3, xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E -  (ingress)
 *        Ultra (72B) Routed ACL+QoS+IPv6
 *        AKA - ingress Key #11
 *        CH3 : key for IPv6 packets use in second lookup.
 *        (APPLICABLE DEVICES: DxCh3, xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E      -     (egress)
 *        Extended (48B) RACL/VACL IPv4
 *        CH3 : Custom egress key for not-IP, IPV4, ARP
 *        (APPLICABLE DEVICES: DxCh3, xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E    -     (egress)
 *        Ultra (72B) RACL/VACL IPv6
 *        CH3 : Custom egress key for IPV6
 *        (APPLICABLE DEVICES: DxCh3, xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E                 -     (ingress)
 *        10 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E                 -     (ingress)
 *        20 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E                 -     (ingress)
 *        30 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E                 -     (ingress)
 *        40 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E                 -     (ingress)
 *        50 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E                 -     (ingress)
 *        60 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E                  -     (egress)
 *        10 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E                  -     (egress)
 *        20 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E                  -     (egress)
 *        30 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E                  -     (egress)
 *        40 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E                  -     (egress)
 *        50 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E                  -     (egress)
 *        60 byte UDB only key
 *        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *
 *     CPSS_DXCH_PCL_RULE_FORMAT_LAST_E                            - last element to
 *                                                                   calculate amount
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E                     /*0*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E                  /*1*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E                    /*2*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E                   /*3*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E                   /*4*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E                    /*5*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E                    /*6*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E                      /*7*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E                   /*8*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E                     /*9*/   ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E                    /*10*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E                     /*11*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E                     /*12*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E                        /*13*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E                        /*14*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E        /*15*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E         /*16*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E       /*17*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E      /*18*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E              /*19*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E            /*20*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E                         /*21*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E                         /*22*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E                         /*23*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E                         /*24*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E                         /*25*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E                         /*26*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E                          /*27*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E                          /*28*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E                          /*29*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E                          /*30*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E                          /*31*/  ,
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E                          /*32*/  ,

    CPSS_DXCH_PCL_RULE_FORMAT_LAST_E /* must be last */
} CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_OFFSET_TYPE_ENT
 *
 * Description: Offset types for packet headers parsing used for user defined
 *              bytes configuration
 *
 * Enumerations:
 *    CPSS_DXCH_PCL_OFFSET_L2_E       - offset from start of mac(L2) header
 *
 *    CPSS_DXCH_PCL_OFFSET_L3_E       - offset from start of L3 header.
 *                 (APPLICABLE DEVICES: DxCh1, DxCh2, DxCh3)
 *
 *    CPSS_DXCH_PCL_OFFSET_L4_E       - offset from start of L4 header
 *
 *    CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E - offset from start of
 *                                          IPV6 Extension Header .
 *                 (APPLICABLE DEVICES: DxCh1, DxCh2, DxCh3)
 *
 *    CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E - the user defined byte used
 *                   for TCP or UDP comparator, not for byte from packet
 *                 (APPLICABLE DEVICES: DxCh1, DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2)
 *
 *    CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E - offset from start of
 *                  L3 header minus 2. Ethertype of IP.
 *                 (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 *    CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E   - offset from
 *                  Ethertype of MPLS minus 2.
 *                 (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 *    CPSS_DXCH_PCL_OFFSET_TUNNEL_L2_E   - offset from Tunnel L2
 *                 For IPCL refers to the beginning of the original packet’s MAC header.
 *                 (prior to tunnel-termination).
 *                 If packet was not tunnel-terminated - this anchor is invalid.
 *                 For EPCL refers to the beginning of the outgoing packet’s MAC header
 *                 (after tunnel-start).
 *                 If packet was not tunnel-start this anchor is invalid;
 *                 (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *    CPSS_DXCH_PCL_OFFSET_TUNNEL_L3_MINUS_2_E   - offset from Tunnel L3
 *                  The beginning of the L3 header in tunnel minus 2 bytes.
 *                  For IPCL refers to the beginning of original packet's header.
 *                  (prior to tunnel termination).
 *                  If packet was not tunnel terminated this anchor is invalid.
 *                  Only fields in the tunnel can be selected.
 *                  Offset cannot exceed 56 bytes.
 *                  For EPCL valid only if packet is tunnel start.
 *                  Points to the beginning of L4 header in the added tunnel header
 *                  minus 2 bytes. Offset cannot exceed 64 bytes.
 *                 (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *    CPSS_DXCH_PCL_OFFSET_METADATA_E   - offset from Metadata.
 *                 (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *    CPSS_DXCH_PCL_OFFSET_INVALID_E - invalid UDB - contains 0 always
 *                 (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *
 * Comments:
 *   1. all possible valid UDB configurations for DxCh1, DxCh2, DxCh3 devices:
 *      key-format, UDB-index, (valid offset types list)
 *      STD_NOT_IP,    0,   (L2, L3)
 *      STD_NOT_IP,    1,   (L2, L3)
 *      STD_NOT_IP,    2,   (L3)
 *      STD_L2_IP_QOS, 0,   (L2, L3, L4, IPV6_EH, TCP_UDP_COMPR)
 *      STD_L2_IP_QOS, 1,   (L2, L3, L4, IPV6_EH)
 *      STD_IPV4_L4,   0,   (L2, L3, L4, TCP_UDP_COMPR)
 *      STD_IPV4_L4,   1,   (L2, L3, L4)
 *      STD_IPV4_L4,   2,   (L3, L4)
 *      EXT_NOT_IPV6,  0,   (L2, L3, L4)
 *      EXT_NOT_IPV6,  1,   (L2, L3, L4)
 *      EXT_NOT_IPV6,  2,   (L3, L4, TCP_UDP_COMPR)
 *      EXT_NOT_IPV6,  3,   (L3, L4)
 *      EXT_NOT_IPV6,  4,   (L3, L4)
 *      EXT_NOT_IPV6,  5,   (L3, L4)
 *      EXT_IPV6_L2,   0,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   1,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   2,   (L3, L4, IPV6_EH, TCP_UDP_COMPR)
 *      EXT_IPV6_L2,   3,   (L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   4,   (L3, L4, IPV6_EH)
 *      EXT_IPV6_L2,   5,   (L3, L4, IPV6_EH)
 *      EXT_IPV6_L4,   0,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L4,   1,   (L2, L3, L4, IPV6_EH)
 *      EXT_IPV6_L4,   2,   (L3, L4, TCP_UDP_COMPR)
 *   2.  All other devices support the following offset types in any UDBs:
 *       L2, L3-2, L4, MPLS-2, TCP_UDP_COMPR.
 */
typedef enum
{
    CPSS_DXCH_PCL_OFFSET_L2_E,
    CPSS_DXCH_PCL_OFFSET_L3_E,
    CPSS_DXCH_PCL_OFFSET_L4_E,
    CPSS_DXCH_PCL_OFFSET_IPV6_EXT_HDR_E,
    CPSS_DXCH_PCL_OFFSET_TCP_UDP_COMPARATOR_E,
    CPSS_DXCH_PCL_OFFSET_L3_MINUS_2_E,
    CPSS_DXCH_PCL_OFFSET_MPLS_MINUS_2_E,
    CPSS_DXCH_PCL_OFFSET_TUNNEL_L2_E,
    CPSS_DXCH_PCL_OFFSET_TUNNEL_L3_MINUS_2_E,
    CPSS_DXCH_PCL_OFFSET_METADATA_E,
    CPSS_DXCH_PCL_OFFSET_INVALID_E

} CPSS_DXCH_PCL_OFFSET_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_PACKET_TYPE_ENT
 *
 * Description: Packet types.
 *
 * Enumerations:
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E       - IPV4 TCP
 *                  (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E       - IPV4 UDP
 *                  (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_MPLS_E           - MPLS
 *                  (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_FRAGMENT_E  - IPV4 Fragment
 *                  (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_OTHER_E     - IPV4 Other
 *                  (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E - Ethernet Other
 *                  (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE_E            - User Defined Ethertype
 *                  (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E           - IPV6
 *                  (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2)
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E           - User Defined Ethertype1
 *                  (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E           - User Defined Ethertype2
 *                  (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E           - User Defined Ethertype3
 *                  (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E           - User Defined Ethertype4
 *                  (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_TCP_E       - IPV6 TCP
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_UDP_E       - IPV6 UDP
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_OTHER_E       - IPV6 not UDP and not TCP
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE5_E           - User Defined Ethertype5
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_PACKET_TYPE_UDE6_E           - User Defined Ethertype6
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *    CPSS_DXCH_PCL_PACKET_TYPE_LAST_E - last value (for internal use)
 * Comments:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_TCP_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_UDP_E,
    CPSS_DXCH_PCL_PACKET_TYPE_MPLS_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_FRAGMENT_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV4_OTHER_E,
    CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE1_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE2_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE3_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE4_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_TCP_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_UDP_E,
    CPSS_DXCH_PCL_PACKET_TYPE_IPV6_OTHER_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE5_E,
    CPSS_DXCH_PCL_PACKET_TYPE_UDE6_E,


    CPSS_DXCH_PCL_PACKET_TYPE_LAST_E/* must be last used for array size */

} CPSS_DXCH_PCL_PACKET_TYPE_ENT;

/* maximal UDB index                   */
/* (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3 */
#define CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS 50

/* amount of replaceable UDBs in Ingress UDB only keys */
/* (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3                 */
#define CPSS_DXCH_PCL_INGRESS_UDB_REPLACE_MAX_CNS  12

/*
 * Typedef: struct CPSS_DXCH_PCL_UDB_SELECT_STC
 *
 * Description:
 *     This structure defines User Defined Bytes Selection
 *     for UDB only keys.
 *     (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 * Fields:
 *     udbSelectArr       - The UDB number in the global UDB pool that
 *                          is selected as UDB number-n in this UDB only key.
 *                          The index in this array is the number of UDB-position
 *                          in appropriate UDB only key.
 *     ingrUdbReplaceArr  - enabling replacing UDBs in the key by specific not-UDB data.
 *                          GT_TRUE - enable, GT_FALSE - disable.
 *                          Relevant for Ingress PCL only.
 *                          udbReplaceArr[0]  - UDB0  replaced with {PCL-ID[7:0]}
 *                          udbReplaceArr[1]  - UDB1  replaced with {UDB Valid,reserved,PCL-ID[9:8]}
 *                          udbReplaceArr[2]  - UDB2  replaced with eVLAN[7:0]
 *                          udbReplaceArr[3]  - UDB3  replaced with eVLAN[12:8]
 *                          udbReplaceArr[4]  - UDB4  replaced with Source-ePort[7:0]
 *                          udbReplaceArr[5]  - UDB5  replaced with Source-ePort[12:8]
 *                          udbReplaceArr[6]  - UDB6  replaced with Tag1-VID[7:0]
 *                          udbReplaceArr[7]  - UDB7  replaced with MAC2ME and Tag1-{UP[2:0],VID[11:8]}
 *                          udbReplaceArr[8]  - UDB8  replaced with QoS-Profile[7:0]
 *                          udbReplaceArr[9]  - UDB9  replaced with QoS-Profile[9:8]
 *                          udbReplaceArr[10] - UDB10 replaced with Flow-ID[7:0]
 *                          udbReplaceArr[11] - UDB11 replaced with Flow-ID[11:8]
 *     egrUdb01Replace    - UDBs 0,1 are replaced with {PCL-ID[9:0]},
 *                          padded with leading zeros.
 *                          GT_TRUE - enable, GT_FALSE - disable.
 *                          Relevant for  Egress PCL only.
 *     egrUdbBit15Replace - <UDB Valid> indication replaces bit 15 of the key,
 *                          overriding any other configuration of bit 15 content,
 *                          including the configuration in egrUdb01Replace.
 *                          GT_TRUE - enable, GT_FALSE - disable.
 *                          Relevant for  Egress PCL only.
 *
 *  Comments:
 */
typedef struct
{
    GT_U32      udbSelectArr[CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS];
    GT_BOOL     ingrUdbReplaceArr[CPSS_DXCH_PCL_INGRESS_UDB_REPLACE_MAX_CNS];
    GT_BOOL     egrUdb01Replace;
    GT_BOOL     egrUdbBit15Replace;
} CPSS_DXCH_PCL_UDB_SELECT_STC;

/*
 * Typedef: enum CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT
 *
 * Description: User Defined Bytes override types.
 *
 * Enumerations:
 *
 *    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_MSB_E  - VRF ID MSB
 *    (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_LSB_E  - VRF ID LSB
 *    (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_QOS_PROFILE_E - Qos profile index
 *    (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_TRUNK_HASH_E  - Trunk Hash
 *    (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_TAG1_INFO_E   - Tag1 info
 *    (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_MODE_E - VRF ID MODE
 *                                                  common for all key types
 *    (APPLICABLE DEVICES: xCat, xCat3)
 * Comments:
 *    Following are the UDB positions affected by this Enum (per key type):
 *     Standard Not Ip Key:
 *           VRF ID's LSB in UDB15
 *           VRF ID's MSB in UDB16
 *           Tag1 info in UDB16
 *           Trunk hash in UDB17
 *     Standard Ip L2 Qos Key:
 *           VRF ID's LSB in UDB18
 *           VRF ID's MSB in UDB19
 *     Standard Ipv4 L4 Key:
 *           VRF ID's LSB in UDB20
 *           VRF ID's MSB in UDB21
 *           Trunk hash in UDB22
 *     Standard UDB Key:
 *           VRF ID's LSB in UDB0
 *           VRF ID's MSB in UDB1
 *           QoS profile in UDB2
 *           Trunk hash in UDB3
 *     Extended Not Ipv6 Key:
 *           VRF ID's MSB in UDB1
 *           Trunk hash in UDB3
 *           VRF ID's LSB in UDB5
 *     Extended Ipv6 L2 Key:
 *           VRF ID's MSB in UDB6
 *           Trunk hash in UDB7
 *           VRF ID's LSB in UDB11
 *     Extended Ipv6 L4 Key:
 *           VRF ID's MSB in UDB12
 *           Trunk hash in UDB13
 *           VRF ID's LSB in UDB14
 *     Extended UDB Key:
 *           VRF ID's LSB in UDB1
 *           VRF ID's MSB in UDB2
 *           Trunk hash in UDB3
 *           QoS profile in UDB5
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_MSB_E,
    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_LSB_E,
    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_QOS_PROFILE_E,
    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_TRUNK_HASH_E,
    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_TAG1_INFO_E,
    CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_VRF_ID_MODE_E
} CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT;

/*
 * Typedef: enum CPSS_DXCH_UDB_ERROR_CMD_ENT
 *
 * Description:
 *     This enum defines packet command taken in the UDB error case.
 * Enumerations:
 *     CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E         - continue Policy Lookup
 *     CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E    - trap packet to CPU
 *     CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E      - hard drop packet
 *     CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E      - soft drop packet
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_UDB_ERROR_CMD_LOOKUP_E,
    CPSS_DXCH_UDB_ERROR_CMD_TRAP_TO_CPU_E,
    CPSS_DXCH_UDB_ERROR_CMD_DROP_HARD_E,
    CPSS_DXCH_UDB_ERROR_CMD_DROP_SOFT_E
} CPSS_DXCH_UDB_ERROR_CMD_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT
 *
 * Description: Type of access to Ingress / Egress PCL configuration Table.
 *              Type of access defines how device calculates index of PCL
 *              configuration Table for packet.
 *
 * Enumerations:
 *   CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - by (physical) PORT access mode.
 *              Ingress port or trunk id is used to calculate index of the PCL
 *              configuration table
 *   CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - by VLAN access mode
 *              VLAN ID is used to calculate index of the PCL configuration table.
 *   CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_EPORT_E - by ePORT access mode.
 *              eport is used to calculate index of the PCL configuration table.
 *              for ingress : local device source ePort is used (12 LSBits)
 *              for egress : target ePort is used (12 LSBits)
 *              (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 * Comments:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E,
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E,
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_EPORT_E
} CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT
 *
 * Description:
 *     This enum defines the ip-total-length check algorithm
 *     that used for calculate the key "IP Header OK" bit.
 * Enumerations:
 *       CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E    - compare
 *             ip-total-lengts with the size of L3 level part of the packet
 *       CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E    - compare
 *             ip-total-lengts with the total size of the packet
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L3_E,
    CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_L2_E
} CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT
 *
 * Description:
 *     EPCL Key<SrcPort> field source selector for DSA tagged packets.
 * Enumerations:
 *       CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ORIGINAL_E -
 *             For DSA tagged packet, <SrcPort> is taken from the DSA tag.
 *       CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_LOCAL_E    -
 *             For all packets (DSA tagged or not), <SrcPort> indicates the
 *             local device ingress physical port.
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ORIGINAL_E,
    CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_LOCAL_E
} CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT;


/*
 * Typedef: enum CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT
 *
 * Description:
 *     EPCL Key<Target port> field source selector.
 * Enumerations:
 *       CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_LOCAL_E    -
 *                                              Local Device Physical Port.
 *       CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_FINAL_E    -
 *                                              Final Destination Port.
 *
 *  Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_LOCAL_E,
    CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_FINAL_E
} CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT;

/*-------------------------------------------------------------------------*/
/*                           Policy Rules Formats                          */
/*-------------------------------------------------------------------------*/

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC
 *
 * Description:
 *     This structure describes the common segment of all key formats.
 *
 * Fields:
 *     pclId       - The PCL-ID attribute assigned to this lookup.
 *                   Only 2 MSBs of pclId are used when portListBmp is used
 *                   (see notes below about portListBmp).
 *     macToMe     - MAC To Me
 *                    1 - packet matched by MAC-To-Me lookup, 0 - not matched
 *                   The field relevant only for DxCh3 and above devices.
 *                   DxCh1 and DxCh2 ignore the field
 *     sourcePort  - The port number from which the packet ingressed the device.
 *                   Port 63 is the CPU port.
 *                   Field muxing description(APPLICABLE DEVICES: xCat2, Lion2, Bobcat2, Caelum, Bobcat3):
 *                   sourcePort is muxed with portListBmp[15:8]
 *     sourceDevice  - Together with sourcePort indicates the network port at which the packet
 *                   was received.
 *                   Field muxing description(APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3):
 *                   sourceDevice is muxed with portListBmp[27:16].
 *     portListBmp   - Port list bitmap. (APPLICABLE DEVICES: xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *                   Relevant only when PCL lookup works in port-list mode.
 *                   Field muxing description:
 *                   portListBmp[7:0] is muxed with pclId[7:0],
 *                   portListBmp[15:8] is muxed with sourcePort[7:0]
 *                   portListBmp[27:16] is muxed with sourceTrunkId (APPLICABLE DEVICES: Lion2)
 *                   or sourceDevice (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3).
 *                   The portListBmp bits in rule's mask and pattern
 *                   should be set to 0 (don't care) for matched ports.
 *                   The portListBmp bits in rule's mask should be 1 and
 *                   pattern should be set to 0 for not matched ports.
 *                   Applying a rule for a packet received from the CPU requires
 *                   the portListBmp pattern with all bits equal to zero. Hence
 *                   rules cannot be shared between the CPU port and other ports
 *                   in port-list mode.
 *     isTagged    - Flag indicating the packet Tag state
 *                   For Ingress PCL Keys:
 *                   For non-tunnel terminated packets:
 *                   - If packet was received on DSA-tagged, this field is set
 *                     to the DSA tag <SrcTagged> field.
 *                   - If packet was received non-DSA-tagged, this field is set
 *                     to 1 if the packet was received  VLAN or Priority-tagged.
 *                   For Ethernet-Over-xxx tunnel-terminated packets: Passenger
 *                   packet VLAN tag format.
 *
 *                   For Egress PCL Keys:
 *                   For non Tunnel Start packets: Transmitted packet's VLAN tag
 *                   format after tag addition/removal/modification
 *                   For Ethernet-Over-xxx Tunnel Start packets:
 *                   Passenger packet's VLAN tag format after tag
 *                   addition/removal/modification.
 *                   0 = Packet is untagged.
 *                   1 = Packet is tagged.
 *
 *     vid         - VLAN ID assigned to the packet.
 *                   For DxCh, DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2
 *                   VLAN Id is 12 bit.
 *                   For Bobcat2; Caelum; Bobcat3 devices support of 13-bit VLAN Id.
 *                   Standard keys contain bits 11:0 only.
 *                   Some keys contains all bits.
 *     up          - The packet's 802.1p User Priority field.
 *     qosProfile  - The QoS Profile assigned to the packet until this lookup
 *                   stage, as assigned by the device,
 *                   according to the QoS marking algorithm
 *     isIp        - An indication that the packet is IP
 *                   0 = Non IP packet.
 *                   1 = IPv4/6 packet.
 *     isL2Valid   - Indicates that Layer 2 information in the search key is valid.
 *                   0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                       Egress PCL (EPCL) are not valid
 *                   1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                   For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                   IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                   is based on the passenger IP packet which does not include
 *                   a Layer 2 header.
 *                   For Egress PCL Keys: Layer 2 fields are not valid in
 *                   the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                   Start, and its PCL Key is based on the passenger IP packet
 *                   which does not include Layer 2 data.
 *                     Supported by DxCh2 and above.
 *                   DxCh1 devices ignores the field.
 *
 *     isUdbValid  - User-defined bytes (UDBs) are valid.
 *                   Indicates that all user-defined bytes used in that search
 *                   key were successfully parsed.
 *                   0 = At least 1 user-defined byte couldn't be parsed.
 *                   1 = All user-defined bytes used in this search key
 *                   were successfully parsed.
 *
 *  Comments:
 */
typedef struct
{
    GT_U32               pclId;
    GT_U8                macToMe;
    GT_PHYSICAL_PORT_NUM sourcePort;
    GT_U32               sourceDevice;
    CPSS_PORTS_BMP_STC   portListBmp;
    GT_U8                isTagged;
    GT_U32               vid;
    GT_U32               up;
    GT_U32               qosProfile;
    GT_U8                isIp;
    GT_U8                isL2Valid;
    GT_U8                isUdbValid;
} CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *
 * Description:
 *     This structure describes the common segment of all extended key formats.
 *
 * Fields:
 *     isIpv6     - An indication that the packet is IPv6
 *                  (if packet was identified as IP packet):
 *                  0 = Non IPv6 packet.
 *                  1 = IPv6 packet.
 *                  An indication that the packet is FCoE
 *                  (if packet was not identified as IP packet,
 *                  APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3):
 *                  0 = Non FCoE packet.
 *                  1 = FCoE packet.
 *     ipProtocol - IP protocol/Next Header type.
 *     dscp       - The DSCP field of the IPv4/6 header.
 *     isL4Valid  - Layer 4 information is valid.
 *                  This field indicates that all the Layer 4
 *                  information required for the search key is available
 *                  and the IP header is valid.
 *                  0 = Layer 4 information is not valid.
 *                  1 = Layer 4 information is valid.
 *                  Layer 4 information may not be available for any of the
 *                   following reasons:
 *                  - Layer 4 information is not included in the packet.
 *                     For example, Layer 4 information
 *                     isn't available in non-IP packets, or in IPv4
 *                     non-initial-fragments.
 *                  - Parsing failure: Layer 4 information is beyond
 *                     the first 128B of the packet, or beyond
 *                     IPv6 extension headers parsing capabilities.
 *                  - IP header is invalid.
 *     l4Byte0    - The following Layer 4 information is available for
 *                  UDP and TCP packets - L4 Header Byte0 through Byte3,
 *                  which contain the UDP/TCP destination and source ports.
 *                  For TCP also L4 Header Byte13, which contains the TCP flags.
 *                  For IGMP L4 Header Byte0, which contain the IGMP Type.
 *                  For ICMP L4 Header Byte0 and Byte1,
 *                  which contain the ICMP Type and Code fields
 *                  and L4 Header Byte4<in l4Byte2> and Byte5 <in l4Byte3>,
 *                  which contain the ICMP authentication field.
 *                  The following Layer 4 information is available for packets
 *                  that are not TCP, UDP, IGMP or ICMP:
 *                  L4 Header Byte0 through Byte3 L4 Header Byte13
 *     l4Byte1    - see l4Byte0
 *     l4Byte2    - see l4Byte0
 *     l4Byte3    - see l4Byte0
 *     l4Byte13   - see l4Byte0
 *     ipHeaderOk - Indicates a valid IP header.
 *                  0 = Packet IP header is invalid.
 *                  1 = Packet IP header is valid.
 *
 *  Comments:
 */
typedef struct
{
    GT_U8      isIpv6;
    GT_U32     ipProtocol;
    GT_U32     dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte0;
    GT_U8      l4Byte1;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      l4Byte13;
    GT_U8      ipHeaderOk;

} CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *
 * Description:
 *     This structure describes the common segment
 *     of all standard IP key formats.
 *
 * Fields:
 *     isIpv4     - An indication that the packet is IPv4
 *                  (if packet was identified as IP packet):
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *                  An indication that the packet is FCoE
 *                  (if packet was not identified as IP packet,
 *                  APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3):
 *                  0 = Non FCoE packet.
 *                  1 = FCoE packet.
 *     ipProtocol  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     dscp        - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     isL4Valid   - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte2     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte3     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     ipHeaderOk  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     ipv4Fragmented - Identifies an IPv4 fragment.
 *                      0 = Not an IPv4 packet or not an IPv4 fragment.
 *                      1 = Packet is an IPv4 fragment (could be
 *                      the first fragment or any subsequent fragment)
 *
 *  Comments:
 */
typedef struct
{
    GT_U8      isIpv4;
    GT_U32     ipProtocol;
    GT_U32     dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      ipHeaderOk;
    GT_U8      ipv4Fragmented;

} CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *
 * Description:
 *     This structure describes the standard not-IP key.
 * Fields:
 *     common     - the common part for all formats (see above)
 *     isIpv4     - An indication that the packet is IPv4
 *                  (if packet was identified as IP packet):
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *                  An indication that the packet is FCoE
 *                  (if packet was not identified as IP packet,
 *                  APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3):
 *                  0 = Non FCoE packet.
 *                  1 = FCoE packet.
 *     etherType  - Valid when <IsL2Valid> =1.
 *                  If <L2 Encap Type> = 0, this field contains the
 *                  <Dsap-Ssap> of the LLC NON-SNAP packet.
 *                  If <L2 Encap Type> = 1, this field contains the <EtherType/>
 *                  of the Ethernet V2 or LLC with SNAP packet.
 *     isArp      - An indication that the packet is an ARP packet
 *                   (identified by EtherType == 0x0806.)
 *                  0 = Non ARP packet.
 *                  1 = ARP packet.
 *     l2Encap    - The Layer 2 encapsulation of the packet.
 *                  For xCat2 and above
 *                     0 = The L2 Encapsulation is LLC NON-SNAP.
 *                     1 = The L2 Encapsulation is Ethernet V2 without LLC.
 *                     2 = Reserved.
 *                     3 = LLC with SNAP.
 *                  For DxCh1, DxCh2, xCat, xCat3, Lion.
 *                     0 = The L2 Encapsulation is LLC NON-SNAP.
 *                     1 = The L2 Encapsulation is Ethernet V2 or LLC with SNAP.
 *     macDa      - Ethernet Destination MAC address.
 *     macSa      - Ethernet Source MAC address.
 *     udb15_17   - User Defined Bytes
 *                     (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                     User Defined Bytes 15,16,17
 *                     (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3)
 *                     User Defined Bytes  0,1,2
 *                  UDB 15  muxed with  vrfId[7:0]
 *                  UDB 16  muxed with  vrfId[11:8]
 *                  UDB 17  muxed with  trunkHash
 *     vrfId      - VRF Id (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *                  vrfId[7:0]  muxed with UDB 15
 *                  vrfId[11:8] muxed with UDB 16
 *     trunkHash  - Trunk Hash (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                  trunkHash is muxed with UDB 17
 *     tag1Exist  - 0 - Tag1 exists in packet.
 *                  1 - Tag1 does not exist in packet.
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                  muxed with UDB 16
 *     vid1       - VID from Tag1, valid only if Tag1 exist.
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                  bits [11:6] muxed with UDB 16
 *     up1        - UP  from Tag1, valid only if Tag1 exist.
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1       - CFI from Tag1, valid only if Tag1 exist.
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                  muxed with UDB 16
 *     udb23_26   - User Defined Bytes 23-26
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC                common;
    GT_U8                                               isIpv4;
    GT_U16                                              etherType;
    GT_U8                                               isArp;
    GT_U32                                              l2Encap;
    GT_ETHERADDR                                        macDa;
    GT_ETHERADDR                                        macSa;
    GT_U8                                               udb15_17[3];
    GT_U32                                              vrfId;
    GT_U32                                              trunkHash;
    GT_U32                                              tag1Exist;
    GT_U32                                              vid1;
    GT_U32                                              up1;
    GT_U32                                              cfi1;
    GT_U8                                               udb23_26[4];
} CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *
 * Description:
 *     This structure describes the standard IPV4/V6 L2_QOS key.
 *
 * Fields:
 *     common            - the common part for all formats (see upper)
 *     commonStdIp       - the common part for all standard IP formats
 *     isArp             - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isIpv6ExtHdrExist - IPv6 extension header exists.
 *                         Indicates that an IPv6 extension exists.
 *                         0 = Non-Ipv6 packet or IPv6 extension header does
 *                         not exist.
 *                         1 = Packet is IPv6 and extension header exists.
 *     isIpv6HopByHop    - Indicates that the IPv6 Original Extension Header
 *                          is Hop-by-Hop.
 *                         0 = Non-IPv6 packet or IPv6 extension header type is
 *                          not Hop-by-Hop Option Header.
 *                         1 = Packet is IPv6 and extension header type
 *                          is Hop-by-Hop Option Header.
 *     macDa             - Ethernet Destination MAC address.
 *     macSa             - Ethernet Source MAC address.
 *     udb18_19          - User Defined Bytes
 *                         (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                         User Defined Bytes 18,19
 *                         (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3)
 *                         User Defined Bytes  0,1
 *                         UDB 18 muxed with vrfId[7:0]
 *                         UDB 19 muxed with vrfId[11:8]
 *     vrfId             - VRF Id (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *                         vrfId[7:0]  muxed with UDB 18
 *                         vrfId[11:8] muxed with UDB 19
 *     udb27_30          - User Defined Bytes 27-30
 *                         (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 *  udb[0] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC                common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC         commonStdIp;
    GT_U8                                               isArp;
    GT_U8                                               isIpv6ExtHdrExist;
    GT_U8                                               isIpv6HopByHop;
    GT_ETHERADDR                                        macDa;
    GT_ETHERADDR                                        macSa;
    GT_U8                                               udb18_19[2];
    GT_U32                                              vrfId;
    GT_U8                                               udb27_30[4];
} CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC
 *
 * Description:
 *     This structure describes the standard IPV4_L4 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonStdIp - the common part for all standard IP formats
 *     isArp       - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isBc        - Ethernet Broadcast packet.
 *                   Valid when <IsL2Valid> =1.
 *                   Indicates an Ethernet Broadcast packet
 *                    (<MAC_DA> == FF:FF:FF:FF:FF:FF).
 *                   0 = MAC_DA is not Broadcast.
 *                   1 = MAC_DA is Broadcast.
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     l4Byte0     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte1     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte13    - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     udb20_22    - User Defined Bytes
 *                   (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   User Defined Bytes 20,21,11
 *                   (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3)
 *                   User Defined Bytes  0,1,2
 *                   UDB 20 muxed with vrfId[7:0]
 *                   UDB 21 muxed with vrfId[11:8]
 *                   UDB 22 muxed with trunkHash
 *     vrfId       - VRF Id (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   vrfId[7:0]  muxed with UDB 20
 *                   vrfId[11:8] muxed with UDB 21
 *     trunkHash   - Trunk Hash (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   trunkHash muxed with UDB 22
 *     udb31_34    - User Defined Bytes 31-34
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 *  udb[0] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC                common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC         commonStdIp;
    GT_U8                                               isArp;
    GT_U8                                               isBc;
    GT_IPADDR                                           sip;
    GT_IPADDR                                           dip;
    GT_U8                                               l4Byte0;
    GT_U8                                               l4Byte1;
    GT_U8                                               l4Byte13;
    GT_U8                                               udb20_22[3];
    GT_U32                                              vrfId;
    GT_U32                                              trunkHash;
    GT_U8                                               udb31_34[4];
} CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC
 *
 * Description:
 *     This structure describes the standard IPV6 DIP key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonStdIp - the common part for all standard IP formats
 *     isArp       - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isIpv6ExtHdrExist - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     isIpv6HopByHop    - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     dip         - 16 bytes IPV6 destination address.
 *     udb47_49    - User Defined Bytes 47-49
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     udb0        - User Defined Byte 0
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 *  Not supported by DxCh2 and DxCh3
 *  Used for Unicast IPV6 routing, lookup1 only
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC                common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC         commonStdIp;
    GT_U8                                               isArp;
    GT_U8                                               isIpv6ExtHdrExist;
    GT_U8                                               isIpv6HopByHop;
    GT_IPV6ADDR                                         dip;
    GT_U8                                               udb47_49[3];
    GT_U8                                               udb0;
} CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC
 *
 * Description:
 *     This structure describes the extended not-IPV6 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 header destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     etherType   - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     l2Encap     - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     ipv4Fragmented - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *     udb0_5      - User Defined Bytes
 *                   (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   User Defined Bytes 0,1,2,3,4,5
 *                   (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3)
 *                   User Defined Bytes  0,1,2,3,4,5
 *                   UDB 1 muxed with vrfId[11:8]
 *                   UDB 3 muxed with trunkHash
 *                   UDB 5 muxed with vrfId[7:0]
 *     tag1Exist   - 0 - Tag1 exists in packet.
 *                   1 - Tag1 does not exist in packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     vid1        - VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         - UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1        - CFI from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     vrfId       - VRF Id (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   vrfId[11:8] muxed with UDB 1
 *                   vrfId[7:0]  muxed with UDB 5
 *     trunkHash   - Trunk Hash (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   trunkHash muxed with UDB 3
 *     udb39_46    - User Defined Bytes 39-46
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 *  udb[2] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC                common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC            commonExt;
    GT_IPADDR                                           sip;
    GT_IPADDR                                           dip;
    GT_U16                                              etherType;
    GT_U32                                              l2Encap;
    GT_ETHERADDR                                        macDa;
    GT_ETHERADDR                                        macSa;
    GT_U8                                               ipv4Fragmented;
    GT_U8                                               udb0_5[6];
    GT_U32                                              tag1Exist;
    GT_U32                                              vid1;
    GT_U32                                              up1;
    GT_U32                                              cfi1;
    GT_U32                                              vrfId;
    GT_U32                                              trunkHash;
    GT_U8                                               udb39_46[8];
} CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the extended IPV6+L2  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dipBits127to120   - IPV6 destination address highest 8 bits.
 *     isIpv6ExtHdrExist - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     isIpv6HopByHop    - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     udb6_11     - User Defined Bytes
 *                   (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   User Defined Bytes 6,7,8,9,10,11
 *                   (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3)
 *                   User Defined Bytes 0,1,2,3,4,5
 *                   UDB 11 muxed with vrfId[7:0]
 *                   UDB 6  muxed with vrfId[11:8]
 *                   UDB 7  muxed with trunkHash
 *     tag1Exist   - 0 - Tag1 exists in packet.
 *                   1 - Tag1 does not exist in packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     vid1        - VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         - UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     vrfId       - VRF Id (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   vrfId[7:0]  muxed with UDB 11
 *                   vrfId[11:8] muxed with UDB 6
 *     trunkHash   - Trunk Hash (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   trunkHash muxed with UDB 7
 *     udb47_49    - User Defined Bytes 47-49
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     udb0_4      - User Defined Bytes 0-4
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 *  udb[2] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC                common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC            commonExt;
    GT_IPV6ADDR                                         sip;
    GT_U8                                               dipBits127to120;
    GT_U8                                               isIpv6ExtHdrExist;
    GT_U8                                               isIpv6HopByHop;
    GT_ETHERADDR                                        macDa;
    GT_ETHERADDR                                        macSa;
    GT_U8                                               udb6_11[6];
    GT_U32                                              tag1Exist;
    GT_U32                                              vid1;
    GT_U32                                              up1;
    GT_U32                                              vrfId;
    GT_U32                                              trunkHash;
    GT_U8                                               udb47_49[3];
    GT_U8                                               udb0_4[5];
} CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the extended IPV6+L4  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dip         - IPv6 destination IP address field.
 *     isIpv6ExtHdrExist - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     isIpv6HopByHop    - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
 *     udb12_14    - User Defined Bytes
 *                   (APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   User Defined Bytes 12,13,14
 *                   (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3)
 *                   User Defined Bytes  0,1,2
 *                   UDB 14 muxed with vrfId[7:0]
 *                   UDB 12 muxed with vrfId[11:8]
 *                   UDB 13 muxed with trunkHash
 *     tag1Exist   - 0 - Tag1 exists in packet.
 *                   1 - Tag1 does not exist in packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     vid1        - VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         - UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     vrfId       - VRF Id (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   vrfId[7:0]  muxed with UDB 14
 *                   vrfId[11:8] muxed with UDB 12
 *     trunkHash   - Trunk Hash (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                   trunkHash muxed with UDB 13
 *     udb15_22    - User Defined Bytes 15-22
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 *  udb[2] may contain the TCP/UDP comparator result
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC                common;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC            commonExt;
    GT_IPV6ADDR                                         sip;
    GT_IPV6ADDR                                         dip;
    GT_U8                                               isIpv6ExtHdrExist;
    GT_U8                                               isIpv6HopByHop;
    GT_U8                                               udb12_14[3];
    GT_U32                                              tag1Exist;
    GT_U32                                              vid1;
    GT_U32                                              up1;
    GT_U32                                              vrfId;
    GT_U32                                              trunkHash;
    GT_U8                                               udb15_22[8];
} CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC
 *
 * Description: PCL Rule Key fields common to ingress "UDB" key formats.
 *              xCat and above devices only
 *
 * Fields:
 * pclId            - The PCL-ID attribute assigned to this lookup.
 *                    Only 2 MSBs of pclId are used when portListBmp is used
 *                    (see notes below about portListBmp).
 * macToMe          - MAC To Me
 *                    1 - packet matched by MAC-To-Me lookup, 0 - not matched
 *                    The field relevant only for DxCh3 and above devices.
 *                    DxCh1 and DxCh2 ignore the field
 * sourcePort       - The port number from which the packet ingressed the device.
 *                    Port 63 is the CPU port.
 *                    Field muxing description(APPLICABLE DEVICES: xCat2, Lion2, Bobcat2, Caelum, Bobcat3):
 *                    sourcePort is muxed with portListBmp[15:8]
 * sourceDevice     - Together with sourcePort indicates the network port at which the packet
 *                    was received.
 *                    Field muxing description(APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3):
 *                    sourceDevice is muxed with portListBmp[27:16].
 * portListBmp      - Port list bitmap. (APPLICABLE DEVICES: xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *                    Relevant only when PCL lookup works in port-list mode.
 *                    Field muxing description:
 *                    portListBmp[7:0] is muxed with pclId[7:0],
 *                    portListBmp[15:8] is muxed with sourcePort[7:0]
 *                    portListBmp[27:16] is muxed with sourceTrunkId (APPLICABLE DEVICES: Lion2)
 *                    or sourceDevice (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3).
 *                    The portListBmp bits in rule's mask and pattern
 *                    should be set to 0 (don't care) for matched ports.
 *                    The portListBmp bits in rule's mask should be 1 and
 *                    pattern should be set to 0 for not matched ports.
 *                    Applying a rule for a packet received from the CPU requires
 *                    the portListBmp pattern with all bits equal to zero. Hence
 *                    rules cannot be shared between the CPU port and other ports
 *                    in port-list mode.
 * vid              - VLAN ID assigned to the packet.
 * up               - The packet's 802.1p User Priority field.
 * isIp             - An indication that the packet is IP
 *                    0 = Non IP packet.
 *                    1 = IPv4/6 packet.
 * dscpOrExp        - IP DSCP or MPLS EXP
 * isL2Valid        - Indicates that Layer 2 information in the search key is valid.
 *                    0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                        Egress PCL (EPCL) are not valid
 *                    1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                    For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                    IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                    is based on the passenger IP packet which does not include
 *                    a Layer 2 header.
 *                    For Egress PCL Keys: Layer 2 fields are not valid in
 *                    the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                    Start, and its PCL Key is based on the passenger IP packet
 *                    which does not include Layer 2 data.
 *
 * isUdbValid      - User-defined bytes (UDBs) are valid.
 *                   Indicates that all user-defined bytes used in that search
 *                   key were successfully parsed.
 *                   0 = At least 1 user-defined byte couldn't be parsed.
 *                   1 = All user-defined bytes used in this search key
 *                   were successfully parsed.
 * pktTagging       -  The packet's VLAN Tag format . Valid when <IsL2Valid> = 1.
 *                     0 = Untagged
 *                     1 = Reserved.
 *                     2 = PriorityTagged
 *                     3 = IsTagged
 * l3OffsetInvalid  -  This flag indicates that the Layer 3 offset was
 *                     successfully found. 0=Valid; 1=Invalid
 * l4ProtocolType   -  0=Other/Ivalid;
 *                     1=TCP;
 *                     2=ICMP;
 *                     3=UDP;
 * pktType          -  1- IPv4 TCP
 *                     2- IPv4 UDP
 *                     4- MPLS
 *                     8- IPv4 Fragment
 *                     16 IPv4 Other
 *                     32- Ethernet Other
 *                     64- IPv6
 *                     128- UDE
 * ipHeaderOk       -  0 - invalid, 1 - valid - only for ingress Policy formats
 * macDaType        -  0=Unicast; Known and Unknown Unicast
 *                     1=Multicast; Known and Unknown Multicast
 *                     2=NonARP BC; Not ARP Broadcast packet
 *                     3=ARP BC; ARP Broadcast packet
 * l4OffsetInvalid  -  This flag indicates that the Layer 4 offset was
 *                     successfully found. 0=Valid; 1=Invalid
 * l2Encapsulation  -  0=LLC not Snap;
 *                     1=Ethernet V2;
 *                     2=reserved
 *                     3=LLC Snap
 * isIpv6Eh         -  Indicates that an IPv6 extension exists
 *                     0=NotExists; Non-IPv6 packet or IPv6 extension header
 *                     does not exists.
 *                     1=Exists; Packet is IPv6 and extension header exists.
 * isIpv6HopByHop   -  Indicates that the IPv6 Original Extension Header
 *                     is hop-by-hop 0=NonHopByHop; Non-IPv6 packet or
 *                     IPv6 extension header type is not Hop-by-Hop Option Header.
 *                     1=HopByHop; Packet is IPv6 and extension header type is
 *                     Hop-by-Hop Option Header.
 */
typedef struct
{
    GT_U32               pclId;
    GT_U8                macToMe;
    GT_PHYSICAL_PORT_NUM sourcePort;
    GT_U32               sourceDevice;
    CPSS_PORTS_BMP_STC   portListBmp;
    GT_U32               vid;
    GT_U32               up;
    GT_U8                isIp;
    GT_U32               dscpOrExp;
    GT_U8                isL2Valid;
    GT_U8                isUdbValid;
    GT_U32               pktTagging;
    GT_U8                l3OffsetInvalid;
    GT_U32               l4ProtocolType;
    GT_U32               pktType;
    GT_U8                ipHeaderOk;
    GT_U32               macDaType;
    GT_U8                l4OffsetInvalid;
    GT_U32               l2Encapsulation;
    GT_U8                isIpv6Eh;
    GT_U8                isIpv6HopByHop;
} CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC
 *
 * Description: PCL Rule ingress standard "UDB" Key fields.
  *              xCat and above devices only
*
 * Fields:
 * commonIngrUdb    -  fields common for ingress "UDB" styled keys
 * isIpv4           - An indication that the packet is IPv4
 *                    (if packet was identified as IP packet):
 *                     0 = Non IPv4 packet.
 *                     1 = IPv4 packet.
 *                    An indication that the packet is FCoE
 *                    (if packet was not identified as IP packet,
 *                    APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3):
 *                     0 = Non FCoE packet.
 *                     1 = FCoE packet.
 * udb0_15          -  user defined bytes 0-15,
 *                     UDB 0 muxed with vrfId[7:0]
 *                     UDB 1 muxed with vrfId[11:8]
 *                     UDB 2 muxed with qosProfile
 *                     UDB 3 muxed with trunkHash
 * vrfId            -  VRF Id (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *                     vrfId[7:0]  muxed with UDB 0
 *                     vrfId[11:8] muxed with UDB 1
 * qosProfile       -  QoS Profile(APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                     qosProfile muxed with UDB2
 * trunkHash        -  Trunk Hash (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                     trunkHash muxed with UDB 3
 * udb35_38         -  User Defined Bytes 35-38
 *                     (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC    commonIngrUdb;
    GT_U8                                               isIpv4;
    GT_U8                                               udb0_15[16];
    GT_U32                                              vrfId;
    GT_U32                                              qosProfile;
    GT_U32                                              trunkHash;
    GT_U8                                               udb35_38[4];
} CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC
 *
 * Description: PCL Rule ingress extended "UDB" Key fields.
  *              xCat and above devices only
*
 * Fields:
 * commonIngrUdb    -  fields common for ingress "UDB" styled keys
 * isIpv6           - An indication that the packet is IPv6
 *                    (if packet was identified as IP packet):
 *                     0 = Non IPv6 packet.
 *                     1 = IPv6 packet.
 *                    An indication that the packet is FCoE
 *                    (if packet was not identified as IP packet,
 *                    APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3):
 *                     0 = Non FCoE packet.
 *                     1 = FCoE packet.
 * ipProtocol       -  IP protocol/Next Header type.
 * sipBits31_0      -  IPV6 SIP bits 31:0 or IPV4 SIP
 * sipBits79_32orMacSa  -  IPV6 SIP bits 79:32 for IPV6 packets
 *                     MAC_SA for not IPV6 packets
 * sipBits127_80orMacDa -  IPV6 SIP bits 127:80 for IPV6 packets
 *                     MAC_DA for not IPV6 packets
 * dipBits127_112   -  IPV6 DIP bits 127:112
 * dipBits31_0      -  IPV6 DIP bits 31:0 or IPV4 DIP
 * udb0_15          -  user defined bytes 0-15,
 *                     UDB 1 muxed with vrfId[7:0]
 *                     UDB 2 muxed with vrfId[11:8]
 *                     UDB 3 muxed with trunkHash
 *                     UDB 5 muxed with qosProfile
 * tag1Exist        -  0 - Tag1 exists in packet.
 *                     1 - Tag1 does not exist in packet.
 *                     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 * vid1             -  VID from Tag1, valid only if Tag1 exist.
 *                     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 * up1              -  UP  from Tag1, valid only if Tag1 exist.
 *                     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 * cfi1             -  CFI from Tag1, valid only if Tag1 exist.
 *                     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 * vrfId            -  VRF Id (APPLICABLE DEVICES: xCat, xCat3, Lion, Lion2, Bobcat2, Caelum, Bobcat3)
 *                     vrfId[7:0]  muxed with UDB 1
 *                     vrfId[11:8] muxed with UDB 2
 * qosProfile       -  QoS Profile(APPLICABLE DEVICES: xCat, xCat3, Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                     qosProfile  muxed with UDB5
 * trunkHash        -  Trunk Hash (APPLICABLE DEVICES: Lion, xCat2, Lion2, Bobcat2, Caelum, Bobcat3)
 *                     trunkHash muxed with UDB 3
 * udb23_30         -  User Defined Bytes 23-30
 *                     (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC    commonIngrUdb;
    GT_U8                                               isIpv6;
    GT_U32                                              ipProtocol;
    GT_U8                                               sipBits31_0[4];
    GT_U8                                               sipBits79_32orMacSa[6];
    GT_U8                                               sipBits127_80orMacDa[6];
    GT_U8                                               dipBits127_112[2];
    GT_U8                                               dipBits31_0[4];
    GT_U8                                               udb0_15[16];
    GT_U32                                              tag1Exist;
    GT_U32                                              vid1;
    GT_U32                                              up1;
    GT_U32                                              cfi1;
    GT_U32                                              vrfId;
    GT_U32                                              qosProfile;
    GT_U32                                              trunkHash;
    GT_U8                                               udb23_30[8];
} CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC;

/* EGRESS KEY FORMATS */

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC
 *
 * Description:
 *     This structure describes the common segment of all egress key formats.
 *
 * Fields:
 *     valid       - the 0-th bit of Egress PCL keys and it' s always 1.
 *                   This field can be used by DxCh3 and above device
 *                   application to determinate the Egress PCL key.
 *                   The field should be masked (don't care state) or
 *                   pattern is set to 1 in order to get match by the Egress PCL lookups.
 *                   The Ingress PCL keys of these devices contain
 *                   0-th bit of PCL ID in the position of the valid bit.
 *                   Ingress PCL rules should comprise 0 in the 0-th bit of PCL ID
 *                   in order to avoid match by Egress PCL lookups.
 *                   (APPLICABLE DEVICES: DxCh1, DxCh1_Diamond, DxCh2, DxCh3, xCat, xCat3, Lion).
 *                   This bit is not control bit. It's values not related to
 *                   cpssDxChPclRuleValidStatusSet and
 *                   cpssDxChPclRuleStateGet functions.
 *     pclId       - The PCL-ID attribute assigned to this lookup.
 *                   Only 2 MSBs of pclId are used when portListBmp is used
 *                   (see notes below about portListBmp).
 *     sourcePort  - The port number from which the packet ingressed the device.
 *                   Port 63 is the CPU port.
 *     portListBmp   - Port list bitmap. (APPLICABLE DEVICES: xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *                   Relevant only when PCL lookup works in port-list mode.
 *                   This is the field muxing:
 *                   portListBmp   muxed with pclId[7:0]
 *                   portListBmp also muxed with egrPacketType
 *                   depended fields and with sourcePort[7:6].
 *                   The muxing with sourcePort[7:6] depends from
 *                   global configuration.
 *                   (APPLICABLE DEVICES: Lion2, Bobcat2, Caelum, Bobcat3)
 *                   The portListBmp bits in rule's mask and pattern
 *                   should be set to 0 (don't care) for matched ports.
 *                   The portListBmp bits in rule's mask should be 1 and
 *                   pattern should be set to 0 for not matched ports.
 *     isTagged    - Flag indicating the packet Tag state
 *                   For Ingress PCL Keys:
 *                   For non-tunnel terminated packets:
 *                   - If packet was received on DSA-tagged, this field is set
 *                     to the DSA tag <SrcTagged> field.
 *                   - If packet was received non-DSA-tagged, this field is set
 *                     to 1 if the packet was received  VLAN or Priority-tagged.
 *                   For Ethernet-Over-xxx tunnel-terminated packets: Passenger
 *                   packet VLAN tag format.
 *
 *                   For Egress PCL Keys:
 *                   For non Tunnel Start packets: Transmitted packet's VLAN tag
 *                   format after tag addition/removal/modification
 *                   For Ethernet-Over-xxx Tunnel Start packets:
 *                   Passenger packet's VLAN tag format after tag
 *                   addition/removal/modification.
 *                   0 = Packet is untagged.
 *                   1 = Packet is tagged.
 *
 *     vid         - VLAN ID assigned to the packet.
 *                   For DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2
 *                   VLAN Id is 12 bit.
 *                   For Bobcat2; Caelum; Bobcat3 devices support of 16-bit VLAN Id.
 *                   Standard keys contain bits 11:0 only.
 *     up          - The packet's 802.1p User Priority field.
 *     isIp        - An indication that the packet is IP
 *                   0 = Non IP packet.
 *                   1 = IPv4/6 packet.
 *     isL2Valid   - Indicates that Layer 2 information in the search key is valid.
 *                   0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                       Egress PCL (EPCL) are not valid
 *                   1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                   For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                   IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                   is based on the passenger IP packet which does not include
 *                   a Layer 2 header.
 *                   For Egress PCL Keys: Layer 2 fields are not valid in
 *                   the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                   Start, and its PCL Key is based on the passenger IP packet
 *                   which does not include Layer 2 data.
 *     egrPacketType - two bits field:
 *                   0 - packet to CPU
 *                   1 - packet from CPU
 *                   2 - packet to ANALYZER
 *                   3 - forward DATA packet
 *     cpuCode     - The CPU Code forwarded to the CPU.
 *                   Relevant only when packet is of type is TO_CPU.
 *     srcTrg      - The <SrcTrg> field extracted from the TO_CPU DSA<SrcTrg>:
 *                   0 = The packet was sent to the CPU by the ingress pipe and
 *                   the DSA tag contain attributes related to the
 *                    packet ingress.
 *                   1 = The packet was sent to the CPU by the egress pipe
 *                   and the DSA tag contains attributes related to the
 *                   packet egress.
 *                   Relevant only when packet is of type is TO_CPU.
 *     tc -          The Traffic Class of the FROM_CPU packet extracted
 *                   from DSA<TC>.
 *     dp -          The Drop Precedence of the FROM_CPU packet extracted
 *                   from DSA<DP>.
 *     egrFilterEnable - The <EgressFilterEn> extracted from
 *                   FROM_CPU DSA<EgressFilterEn>:
 *                   0 = FROM_CPU packet is subject to egress filtering,
 *                   e.g. data traffic from the CPU
 *                   1 = FROM_CPU packet is not subject egress filtering,
 *                   e.g. control traffic from the CPU
 *     isUnknown -   Indicates that the packet’s MAC DA
 *                   was not found in the FDB.
 *                   0 = The packet’s MAC DA was found in the FDB.
 *                   1 = the packet’s MAC DA was not found in the FDB.
 *                   Relevant only when packet is of type is FORWARD.
 *     rxSniff -     Indicates if this packet is Ingress or Egress
 *                   mirrored to the Analyzer.
 *                   0 = This packet is Egress mirrored to the analyzer.
 *                   1 = This packet is Ingress mirrored to the analyzer.
 *                   Relevant only when packet is of type is TO_ANALYZER.
 *     qosProfile  - The QoS Profile assigned to the packet until this lookup
 *                   stage, as assigned by the device,
 *                   according to the QoS marking algorithm
 *                   Relevant only when packet is of type is FORWARD.
 *     srcTrunkId  - Id of source trunk (see srcIsTrunk)
 *                   Relevant only when packet is of type is FORWARD.
 *                   Fields srcTrunkId and srcHwDev are muxed.
 *                   For DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2
 *                   Trunk Id is 7 bit.
 *                   For Bobcat2; Caelum; Bobcat3 devices supported 12-bit Trunk Id.
 *                   Standard keys contain bits 6:0 only.
 *     srcIsTrunk  - Indicates packets received from a Trunk on a
 *                   remote device or on the local device
 *                   0 = Source is not a Trunk due to one of the following:
 *                   - A non-DSA-tagged packet received from a network port
 *                   which is not a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD
 *                   and DSA<SrcIsTrunk> =0
 *                   1 = Source is a Trunk due to one of the following
 *                   - A non-DSA-tagged packet received from a network port
 *                    which is a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD and
 *                    DSA<SrcIsTrunk =1
 *                   Relevant only when packet is of type is FORWARD.
 *     isRouted    - Indicates that the packet has been routed either by the
 *                   local device or some previous device in a cascaded system.
 *                   0 = The packet has not been routed.
 *                   1 = The packet has been routed.
 *                   Relevant only when packet is of type is FORWARD.
 *     srcHwDev    - Together with <SrcPort> and <srcHwDev> indicates the
 *                   network port at which the packet was received.
 *                   Relevant for all packet types exclude the
 *                   FWD_DATA packed entered to the PP from Trunk
 *                   Fields srcTrunkId and srcHwDev are muxed.
 *                   For DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2
 *                   HW Device Id is 5 bit.
 *                   For Bobcat2; Caelum; Bobcat3 devices supported 12-bit HW Device Ids.
 *                   Standard keys contain bits 4:0 only.
 *     sourceId    - The Source ID assigned to the packet.
 *                   For DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2
 *                   Source Id is 5 bit.
 *                   For Bobcat2; Caelum; Bobcat3 devices supported 12-bit Source Id.
 *                   Standard keys contain bits 4:0 only.
 *     isVidx      - Indicates that a packet is forwarded to a Multicast group.
 *                   0 = The packet is a Unicast packet forwarded to a
 *                   specific target port or trunk.
 *                   1 = The packet is a multi-destination packet forwarded
 *                   to a Multicast group.
 *     tag1Exist   - 0 - Tag1 exists in packet.
 *                   1 - Tag1 does not exist in packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     isUdbValid  - User-defined bytes (UDBs) are valid.
 *                   Indicates that all user-defined bytes used in that search
 *                   key were successfully parsed.
 *                   0 = At least 1 user-defined byte couldn't be parsed.
 *                   1 = All user-defined bytes used in this search key
 *                   were successfully parsed.
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 *  Substructures toCpu, fromCpu, toAnalyzer and fwdData muxed in HW.
 *  Application should use members of only one of them and with
 *  the egrPacketType field.
 *  (0 - toCpu, 1 - fromCpu, 2 - toAnalyzer and 3 - fwdData).
 *  All members of other substructures must be zeros
 *  both in mask and in pattern (Don't care).
 *
 */
typedef struct
{
    GT_U8                 valid;
    GT_U32                pclId;
    GT_PHYSICAL_PORT_NUM  sourcePort;
    CPSS_PORTS_BMP_STC    portListBmp;
    GT_U8                 isTagged;
    GT_U32                vid;
    GT_U32                up;
    GT_U8                 isIp;
    GT_U8                 isL2Valid;
    GT_U32                egrPacketType;
    struct
    {
        GT_U32            cpuCode;
        GT_U8             srcTrg;
    } toCpu;
    struct
    {
        GT_U32            tc;
        GT_U32            dp;
        GT_U8             egrFilterEnable;
    } fromCpu;
    struct
    {
        GT_U8             rxSniff;
    } toAnalyzer;
    struct
    {
        GT_U32            qosProfile;
        GT_TRUNK_ID       srcTrunkId;
        GT_U8             srcIsTrunk;
        GT_U8             isUnknown;
        GT_U8             isRouted;
    } fwdData;
    GT_HW_DEV_NUM         srcHwDev;
    GT_U32                sourceId;
    GT_U8                 isVidx;
    GT_U8                 tag1Exist;
    GT_U8                 isUdbValid;
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC
 *
 * Description:
 *     This structure describes the common segment of all extended
 *     egress key formats.
 *
 * Fields:
 *     isIpv6     - An indication that the packet is IPv6.
 *                  0 = Non IPv6 packet.
 *                  1 = IPv6 packet.
 *     ipProtocol - IP protocol/Next Header type.
 *     dscp       - The DSCP field of the IPv4/6 header.
 *     isL4Valid  - Layer 4 information is valid.
 *                  This field indicates that all the Layer 4
 *                  information required for the search key is available
 *                  and the IP header is valid.
 *                  0 = Layer 4 information is not valid.
 *                  1 = Layer 4 information is valid.
 *                  Layer 4 information may not be available for any of the
 *                   following reasons:
 *                  - Layer 4 information is not included in the packet.
 *                     For example, Layer 4 information
 *                     isn't available in non-IP packets, or in IPv4
 *                     non-initial-fragments.
 *                  - Parsing failure: Layer 4 information is beyond
 *                     the first 128B of the packet, or beyond
 *                     IPv6 extension headers parsing capabilities.
 *                  - IP header is invalid.
 *     l4Byte0    - The following Layer 4 information is available for
 *                  UDP and TCP packets - L4 Header Byte0 through Byte3,
 *                  which contain the UDP/TCP destination and source ports.
 *                  For TCP also L4 Header Byte13, which contains the TCP flags.
 *                  For IGMP L4 Header Byte0, which contain the IGMP Type.
 *                  For ICMP L4 Header Byte0 and Byte1,
 *                  which contain the ICMP Type and Code fields
 *                  and L4 Header Byte4<in l4Byte2> and Byte5 <in l4Byte3>,
 *                  which contain the ICMP authentication field.
 *                  The following Layer 4 information is available for packets
 *                  that are not TCP, UDP, IGMP or ICMP:
 *                  L4 Header Byte0 through Byte3 L4 Header Byte13
 *     l4Byte1    - see l4Byte0
 *     l4Byte2    - see l4Byte0
 *     l4Byte3    - see l4Byte0
 *     l4Byte13   - see l4Byte0
 *
 *     egrTcpUdpPortComparator - For non Tunnel Start packets:
 *                  The transmitted packet TCP/UDP comparator result
 *                  For Tunnel Start packets:
 *                  The transmitted passenger packet TCP/UDP comparator result
 *
 *     isUdbValid  - User-defined bytes (UDBs) are valid.
 *                   Indicates that all user-defined bytes used in that search
 *                   key were successfully parsed.
 *                   0 = At least 1 user-defined byte couldn't be parsed.
 *                   1 = All user-defined bytes used in this search key
 *                   were successfully parsed.
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */
typedef struct
{
    GT_U8      isIpv6;
    GT_U32     ipProtocol;
    GT_U32     dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte0;
    GT_U8      l4Byte1;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      l4Byte13;
    GT_U32     egrTcpUdpPortComparator;
    GT_U8      isUdbValid;
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC
 *
 * Description:
 *     This structure describes the common segment
 *     of all egress standard IP key formats.
 *
 * Fields:
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     ipProtocol  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     dscp        - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     isL4Valid   - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte2     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte3     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     ipv4Fragmented - Identifies an IPv4 fragment.
 *                      0 = Not an IPv4 packet or not an IPv4 fragment.
 *                      1 = Packet is an IPv4 fragment (could be
 *                      the first fragment or any subsequent fragment)
 *
 *     egrTcpUdpPortComparator - For non Tunnel Start packets:
 *                  The transmitted packet TCP/UDP comparator result
 *                  For Tunnel Start packets:
 *                  The transmitted passenger packet TCP/UDP comparator result
 *
 *  Comments:
 */
typedef struct
{
    GT_U8      isIpv4;
    GT_U32     ipProtocol;
    GT_U32     dscp;
    GT_U8      isL4Valid;
    GT_U8      l4Byte2;
    GT_U8      l4Byte3;
    GT_U8      ipv4Fragmented;
    GT_U32     egrTcpUdpPortComparator;

} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC;


/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC
 *
 * Description:
 *     This structure describes the standard egress not-IP key.
 * Fields:
 *     common     - the common part for all formats (see above)
 *     isIpv4      - An indication that the packet is IPv4.
 *                  0 = Non IPv4 packet.
 *                  1 = IPv4 packet.
 *     etherType  - Valid when <IsL2Valid> =1.
 *                  If <L2 Encap Type> = 0, this field contains the
 *                  <Dsap-Ssap> of the LLC NON-SNAP packet.
 *                  If <L2 Encap Type> = 1, this field contains the <EtherType/>
 *                  of the Ethernet V2 or LLC with SNAP packet.
 *     isArp      - An indication that the packet is an ARP packet
 *                   (identified by EtherType == 0x0806.)
 *                  0 = Non ARP packet.
 *                  1 = ARP packet.
 *     l2Encap    - The Layer 2 encapsulation of the packet.
 *                   0 = The L2 Encapsulation is LLC NON-SNAP.
 *                   1 = The L2 Encapsulation is Ethernet V2 or LLC with SNAP.
 *     macDa      - Ethernet Destination MAC address.
 *     macSa      - Ethernet Source MAC address.
 *     vid1       -  VID from Tag1, valid only if Tag1 exist.
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1        -  UP  from Tag1, valid only if Tag1 exist.
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1       -  CFI from Tag1, valid only if Tag1 exist.
 *                  (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     udb0_3     - User Defined Bytes 0-3
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */

typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC        common;
    GT_U8                                              isIpv4;
    GT_U16                                             etherType;
    GT_U8                                              isArp;
    GT_U32                                             l2Encap;
    GT_ETHERADDR                                       macDa;
    GT_ETHERADDR                                       macSa;
    GT_U32                                             vid1;
    GT_U32                                             up1;
    GT_U32                                             cfi1;
    GT_U8                                              udb0_3[4];
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC
 *
 * Description:
 *     This structure describes the standard egress IPV4/V6 L2_QOS key.
 *
 * Fields:
 *     common            - the common part for all formats (see upper)
 *     commonStdIp       - the common part for all standard IP formats
 *     isArp             - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     dipBits0to31      - for IPV4 destination Ip address, for IPV6 the
 *                         31 LSBs of destination Ip address (network order)
 *     l4Byte13          - see in CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC
 *     macDa             - Ethernet Destination MAC address.
 *     udb4_7            - User Defined Bytes 4-7
 *                        (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC           common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC    commonStdIp;
    GT_U8                                                 isArp;
    GT_U8                                                 dipBits0to31[4];
    GT_U8                                                 l4Byte13;
    GT_ETHERADDR                                          macDa;
    GT_U8                                                 udb4_7[4];
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC
 *
 * Description:
 *     This structure describes the standard egress IPV4_L4 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonStdIp - the common part for all standard IP formats
 *     isArp       - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     isBc        - Ethernet Broadcast packet.
 *                   Valid when <IsL2Valid> =1.
 *                   Indicates an Ethernet Broadcast packet
 *                    (<MAC_DA> == FF:FF:FF:FF:FF:FF).
 *                   0 = MAC_DA is not Broadcast.
 *                   1 = MAC_DA is Broadcast.
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     l4Byte0     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte1     - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte13    - see l4Byte0 in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     udb8_11     - User Defined Bytes 8-11
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC           common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STD_IP_STC    commonStdIp;
    GT_U8                                                 isArp;
    GT_U8                                                 isBc;
    GT_IPADDR                                             sip;
    GT_IPADDR                                             dip;
    GT_U8                                                 l4Byte0;
    GT_U8                                                 l4Byte1;
    GT_U8                                                 l4Byte13;
    GT_U8                                                 udb8_11[4];
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC
 *
 * Description:
 *     This structure describes the egress extended not-IPV6 key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 header destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     etherType   - see in CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
 *     l2Encap    - The Layer 2 encapsulation of the packet.
 *                   0 = The L2 Encapsulation is LLC NON-SNAP.
 *                   1 = The L2 Encapsulation is Ethernet V2 or LLC with SNAP.
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     ipv4Fragmented - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
 *     vid1        -  VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         -  UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1        -  CFI from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     isMpls      - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                   1 - MPLS, 0 - not MPLS.
 *     numOfMplsLabels - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                   number of MPLS Labels (APPLICABLE RANGES: 0..3).
 *     protocolTypeAfterMpls  - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                   inner protocol type (2-bit value)
 *     mplsLabel0  - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)  MPLS Label0.
 *     mplsExp0    - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)  MPLS Exp0.
 *     mplsLabel1  - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)  MPLS Label1.
 *     mplsExp1    - (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3) MPLS Exp1.
 *     udb12_19    - User Defined Bytes 12-19
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC           common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC       commonExt;
    GT_IPADDR                                             sip;
    GT_IPADDR                                             dip;
    GT_U16                                                etherType;
    GT_U32                                                l2Encap;
    GT_ETHERADDR                                          macDa;
    GT_ETHERADDR                                          macSa;
    GT_U8                                                 ipv4Fragmented;
    GT_U32                                                vid1;
    GT_U32                                                up1;
    GT_U32                                                cfi1;
    GT_U8                                                 isMpls;
    GT_U32                                                numOfMplsLabels;
    GT_U32                                                protocolTypeAfterMpls;
    GT_U32                                                mplsLabel0;
    GT_U32                                                mplsExp0;
    GT_U32                                                mplsLabel1;
    GT_U32                                                mplsExp1;
    GT_U8                                                 udb12_19[8];
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the egress extended IPV6+L2  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dipBits127to120   - IPV6 destination address highest 8 bits.
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     vid1        - VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         - UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1        - CFI from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     isIpv6ExtHdrExist - IPv6 extension header exists.
 *                         Indicates that an IPv6 extension exists.
 *                         0 = Non-Ipv6 packet or IPv6 extension header does
 *                             not exist.
 *                         1 = Packet is IPv6 and extension header exists.
 *                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     isIpv6HopByHop    - Indicates that the IPv6 Original Extension Header
 *                         is Hop-by-Hop.
 *                         0 = Non-IPv6 packet or IPv6 extension header type is
 *                             not Hop-by-Hop Option Header.
 *                         1 = Packet is IPv6 and extension header type
 *                             is Hop-by-Hop Option Header.
 *                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     udb20_27    - User Defined Bytes 20-27
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC           common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC       commonExt;
    GT_IPV6ADDR                                           sip;
    GT_U8                                                 dipBits127to120;
    GT_ETHERADDR                                          macDa;
    GT_ETHERADDR                                          macSa;
    GT_U32                                                vid1;
    GT_U32                                                up1;
    GT_U32                                                cfi1;
    GT_U8                                                 isIpv6ExtHdrExist;
    GT_U8                                                 isIpv6HopByHop;
    GT_U8                                                 udb20_27[8];
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC
 *
 * Description:
 *     This structure describes the egress extended IPV6+L4  key.
 *
 * Fields:
 *     common      - the common part for all formats (see upper)
 *     commonExt   - the common part for all extended formats
 *     sip         - IPv6 source IP address field.
 *     dip         - IPv6 destination IP address field.
 *     isIpv6ExtHdrExist - IPv6 extension header exists.
 *                         Indicates that an IPv6 extension exists.
 *                         0 = Non-Ipv6 packet or IPv6 extension header does
 *                             not exist.
 *                         1 = Packet is IPv6 and extension header exists.
 *                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     isIpv6HopByHop    - Indicates that the IPv6 Original Extension Header
 *                         is Hop-by-Hop.
 *                         0 = Non-IPv6 packet or IPv6 extension header type is
 *                             not Hop-by-Hop Option Header.
 *                         1 = Packet is IPv6 and extension header type
 *                             is Hop-by-Hop Option Header.
 *                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     udb28_35    - User Defined Bytes 28-35
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_STC           common;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_COMMON_EXT_STC       commonExt;
    GT_IPV6ADDR                                           sip;
    GT_IPV6ADDR                                           dip;
    GT_U8                                                 isIpv6ExtHdrExist;
    GT_U8                                                 isIpv6HopByHop;
    GT_U8                                                 udb28_35[8];
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC
 *
 * Description:
 *     This structure describes the common fields of ingress keys.
 *     The fields placed at the same bits of the keys.
 *
 * Fields:
 *     pclId       - The PCL-ID attribute assigned to this lookup.
 *                   Only 2 MSBs of pclId are used when portListBmp is used
 *                   (see notes below about portListBmp).
 *     macToMe     - MAC To Me
 *                    1 - packet matched by MAC-To-Me lookup, 0 - not matched
 *                   The field relevant only for DxCh3 and above devices.
 *                   DxCh1 and DxCh2 ignore the field
 *     sourcePort  - The port number from which the packet ingressed the device.
 *                   Port 63 is the CPU port.
 *                   Field muxing description(APPLICABLE DEVICES: xCat2, Lion2, Bobcat2, Caelum, Bobcat3):
 *                   sourcePort is muxed with portListBmp[15:8]
 *     sourceDevice  - Together with sourcePort indicates the network port at which the packet
 *                   was received.
 *                   Field muxing description(APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3):
 *                   sourceDevice is muxed with portListBmp[27:16].
 *     portListBmp   - Port list bitmap. (APPLICABLE DEVICES: xCat2, Lion2, Bobcat2, Caelum, Bobcat3).
 *                   Relevant only when PCL lookup works in port-list mode.
 *                   Field muxing description:
 *                   portListBmp[7:0] is muxed with pclId[7:0],
 *                   portListBmp[15:8] is muxed with sourcePort[7:0]
 *                   portListBmp[27:16] is muxed with sourceTrunkId (APPLICABLE DEVICES: Lion2)
 *                   or sourceDevice (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3).
 *                   The portListBmp bits in rule's mask and pattern
 *                   should be set to 0 (don't care) for matched ports.
 *                   The portListBmp bits in rule's mask should be 1 and
 *                   pattern should be set to 0 for not matched ports.
 *                   Applying a rule for a packet received from the CPU requires
 *                   the portListBmp pattern with all bits equal to zero. Hence
 *                   rules cannot be shared between the CPU port and other ports
 *                   in port-list mode.
 *     isTagged    - Flag indicating the packet Tag state
 *                   For Ingress PCL Keys:
 *                   For non-tunnel terminated packets:
 *                   - If packet was received on DSA-tagged, this field is set
 *                     to the DSA tag <SrcTagged> field.
 *                   - If packet was received non-DSA-tagged, this field is set
 *                     to 1 if the packet was received  VLAN or Priority-tagged.
 *                   For Ethernet-Over-xxx tunnel-terminated packets: Passenger
 *                   packet VLAN tag format.
 *
 *     vid         - VLAN ID assigned to the packet.
 *     up          - The packet's 802.1p User Priority field.
 *     tos         - 8-bit TOS (contains DSCP in bits 7:2)
 *     ipProtocol  - IP protocol/Next Header type.
 *
 *     isL4Valid  - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte0    - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte1    - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte2    - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte3    - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *     l4Byte13   - see in CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
 *
 *  Comments:
 */
typedef struct
{
    GT_U32          pclId;
    GT_U8           macToMe;
    GT_PHYSICAL_PORT_NUM   sourcePort;
    GT_U32                 sourceDevice;
    CPSS_PORTS_BMP_STC   portListBmp;

    /* L2 */
    GT_U8           isTagged;
    GT_U32          vid;
    GT_U32          up;
    /* L3 */
    GT_U32          tos;
    GT_U32          ipProtocol;
    /* L4 */
    GT_U8           isL4Valid;
    GT_U8           l4Byte0;
    GT_U8           l4Byte1;
    GT_U8           l4Byte2;
    GT_U8           l4Byte3;
    GT_U8           l4Byte13;

} CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC
 *
 * Description:
 *     This structure describes the ingress Standard Routed Qos IPV4 Key.
 *     CH3 : ingress standard IPV4 key format, used in first lookup.
 *
 *
 * Fields:
 *     ingressIpCommon   - The common fields of ingress IP keys.
 *     pktType   - packet type
 *                   0 = Non of the following.
 *                   1 = ARP.
 *                   2 = IPv6 Over MPLS.
 *                   3 = IPv4 Over MPLS.
 *                   4 = MPLS.
 *                   6 = IPv6.
 *                   7 = IPv4.
 *     ipFragmented - IP Fragmented
 *                   0 = no fragment
 *                   1 = first fragment
 *                   2 = mid fragment
 *                   3 = last fragment
 *     ipHeaderInfo - IP Header information
 *                   0 = Normal
 *                   1 = IP options present
 *                   2 = IP header validation fail
 *                   3 = Is small offset
 *     ipPacketLength - IP Packet length 14-bit
 *     ttl          - time to live
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     vrfId      - VRF Id (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                  vrfId[7:0]  muxed with UDB 5
 *
 *     tcpUdpPortComparators - The transmitted packet TCP/UDP
 *                  comparator result.
 *                  Supported only for DxCh3 device and ignored for other.
 *     isUdbValid - User-defined bytes (UDBs) are valid.
 *                  Indicates that all user-defined bytes used in that search
 *                  key were successfully parsed.
 *                  0 = At least 1 user-defined byte couldn't be parsed.
 *                  1 = All user-defined bytes used in this search key
 *                  were successfully parsed.
 *     udb5       - UDB5
 *                  Supported only for DxChXCat and above devices
 *                  and ignored for DxCh3 devices.
 *     udb41_44   - User Defined Bytes 41-44
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC        ingressIpCommon;
    GT_U32                                         pktType;
    /* L3 */
    GT_U8                                          ipFragmented;
    GT_U32                                         ipHeaderInfo;
    GT_U32                                         ipPacketLength;
    GT_U32                                         ttl;
    GT_IPADDR                                      sip;
    GT_IPADDR                                      dip;
    GT_U32                                         vrfId;
    /* L4 */
    GT_U32                                         tcpUdpPortComparators;
    GT_U8                                          isUdbValid;
    GT_U8                                          udb5;
    GT_U8                                          udb41_44[4];
} CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC
 *
 * Description:
 *     This structure describes the ingress Extended Port/VLAN Qos IPV4 Key.
 *     CH3 : ingress extended IPV4 key format, used in second lookup.
 *
 *
 * Fields:
 *     ingressIpCommon   - The common fields of ingress IP keys.
 *     isL2Valid   - Indicates that Layer 2 information in the search key is valid.
 *                   0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                       Egress PCL (EPCL) are not valid
 *                   1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                   For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                   IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                   is based on the passenger IP packet which does not include
 *                   a Layer 2 header.
 *                   For Egress PCL Keys: Layer 2 fields are not valid in
 *                   the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                   Start, and its PCL Key is based on the passenger IP packet
 *                   which does not include Layer 2 data.
 *     isBc        - Ethernet Broadcast packet.
 *                   Valid when <IsL2Valid> =1.
 *                   Indicates an Ethernet Broadcast packet
 *                    (<MAC_DA> == FF:FF:FF:FF:FF:FF).
 *                   0 = MAC_DA is not Broadcast.
 *                   1 = MAC_DA is Broadcast.
 *     isIp        - An indication that the packet is IP
 *                   0 = Non IP packet.
 *                   1 = IPv4/6 packet.
 *     isArp       - An indication that the packet is an ARP packet
 *                   (identified by EtherType == 0x0806.)
 *                   0 = Non ARP packet.
 *                   1 = ARP packet.
 *     l2Encap     - The Layer 2 encapsulation of the packet.
 *                   0 = The L2 Encapsulation is LLC NON-SNAP.
 *                   1 = The L2 Encapsulation is Ethernet V2 or LLC with SNAP.
 *     etherType   - Valid when <IsL2Valid> =1.
 *                  If <L2 Encap Type> = 0, this field contains the
 *                  <Dsap-Ssap> of the LLC NON-SNAP packet.
 *                  If <L2 Encap Type> = 1, this field contains the <EtherType/>
 *                  of the Ethernet V2 or LLC with SNAP packet.
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     tag1Exist   - 0 - Tag1 exists in packet.
 *                   1 - Tag1 does not exist in packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     vid1        - VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         - UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1        - CFI from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     ipFragmented - IP Fragmented
 *                   0 = no fragment
 *                   1 = first fragment
 *                   2 = mid fragment
 *                   3 = last fragment
 *     ipHeaderInfo - IP Header information
 *                   0 = Normal
 *                   1 = IP options present
 *                   2 = IP header validation fail
 *                   3 = Is small offset
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     vrfId       - VRF Id (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                   vrfId[11:8] muxed with UDB 14
 *                   vrfId[7:0]  muxed with UDB 13
 *     trunkHash   - Trunk Hash (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                   trunkHash muxed with UDB 15
 *
 *     isUdbValid - User-defined bytes (UDBs) are valid.
 *                  Indicates that all user-defined bytes used in that search
 *                  key were successfully parsed.
 *                  0 = At least 1 user-defined byte couldn't be parsed.
 *                  1 = All user-defined bytes used in this search key
 *                  were successfully parsed.
 *     UdbStdIpL2Qos    - UDBs 0-1 of standard Ipv4/6 L2 Qos key
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbStdIpV4L4     - UDBs 0-2 of standard Ipv4 L2 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbExtIpv6L2     - UDBs 0-3 of Extended Ipv6 L2 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbExtIpv6L4     - UDBs 0-2 of Extended Ipv6 L4 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     udb5_16          -  UDBs 5-16
 *                  Supported only for DxChXCat and above devices
 *                  and ignored for DxCh3 devices.
 *     udb31_38         - User Defined Bytes 31-38
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC        ingressIpCommon;
    /* L2 */
    GT_U8                                          isL2Valid;
    GT_U8                                          isBc;
    GT_U8                                          isIp;
    GT_U8                                          isArp;
    GT_U32                                         l2Encap;
    GT_U16                                         etherType;
    GT_ETHERADDR                                   macDa;
    GT_ETHERADDR                                   macSa;
    GT_U32                                         tag1Exist;
    GT_U32                                         vid1;
    GT_U32                                         up1;
    GT_U32                                         cfi1;
    /* L3 */
    GT_U8                                          ipFragmented;
    GT_U32                                         ipHeaderInfo;
    GT_IPADDR                                      sip;
    GT_IPADDR                                      dip;
    GT_U32                                         vrfId;
    GT_U32                                         trunkHash;
    /* UDB */
    GT_U8                                          isUdbValid;
    GT_U8                                          UdbStdIpL2Qos[2];
    GT_U8                                          UdbStdIpV4L4[3];
    GT_U8                                          UdbExtIpv6L2[4];
    GT_U8                                          UdbExtIpv6L4[3];
    GT_U8                                          udb5_16[12];
    GT_U8                                          udb31_38[8];

} CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC
 *
 * Description:
 *     This structure describes the ingress Ultra Port/VLAN Qos IPV6 Key.
 *     CH3: ingress ultra IPV6 L2 key format, used in first lookup.
 *
 * Fields:
 *     ingressIpCommon   - The common fields of ingress IP keys.
 *     isL2Valid   - Indicates that Layer 2 information in the search key is valid.
 *                   0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                       Egress PCL (EPCL) are not valid
 *                   1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                   For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                   IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                   is based on the passenger IP packet which does not include
 *                   a Layer 2 header.
 *                   For Egress PCL Keys: Layer 2 fields are not valid in
 *                   the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                   Start, and its PCL Key is based on the passenger IP packet
 *                   which does not include Layer 2 data.
 *     isNd        - IS ND
 *     isBc        - Ethernet Broadcast packet.
 *     isIp        - An indication that the packet is IP
 *                   0 = Non IP packet.
 *                   1 = IPv4/6 packet.
 *     l2Encap     - The Layer 2 encapsulation of the packet.
 *                   0 = The L2 Encapsulation is LLC NON-SNAP.
 *                   1 = The L2 Encapsulation is Ethernet V2 or LLC with SNAP.
 *     etherType   - Valid when <IsL2Valid> =1.
 *                  If <L2 Encap Type> = 0, this field contains the
 *                  <Dsap-Ssap> of the LLC NON-SNAP packet.
 *                  If <L2 Encap Type> = 1, this field contains the <EtherType/>
 *                  of the Ethernet V2 or LLC with SNAP packet.
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     tag1Exist   - 0 - Tag1 exists in packet.
 *                   1 - Tag1 does not exist in packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     vid1        - VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         - UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1        - CFI from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     sip         - IPv6 source IP address field.
 *                   For ARP packets this field holds the sender's IPv6 address.
 *     dip         - IPv6 destination IP address field.
 *                   For ARP packets this field holds the target IPv6 address.
 *     isIpv6ExtHdrExist - IPv6 extension header exists.
 *                   Indicates that an IPv6 extension exists.
 *                   0 = Non-Ipv6 packet or IPv6 extension header does
 *                   not exist.
 *                   1 = Packet is IPv6 and extension header exists.
 *     isIpv6HopByHop    - Indicates that the IPv6 Original Extension Header
 *                   is Hop-by-Hop.
 *                   0 = Non-IPv6 packet or IPv6 extension header type is
 *                   not Hop-by-Hop Option Header.
 *                   1 = Packet is IPv6 and extension header type
 *                   is Hop-by-Hop Option Header.
 *
 *     ipHeaderOk - Indicates a valid IP header.
 *                  0 = Packet IP header is invalid.
 *                  1 = Packet IP header is valid.
 *     vrfId       - VRF Id (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                   vrfId[11:8] muxed with UDB 0
 *                   vrfId[7:0]  muxed with UDB 12
 *     isUdbValid - User-defined bytes (UDBs) are valid.
 *                  Indicates that all user-defined bytes used in that search
 *                  key were successfully parsed.
 *                  0 = At least 1 user-defined byte couldn't be parsed.
 *                  1 = All user-defined bytes used in this search key
 *                  were successfully parsed.
 *     UdbExtNotIpv6    - UDBs 0-3 of Extended Not Ipv6 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbExtIpv6L2     - UDBs 0-4 of Extended Ipv6 L2 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbExtIpv6L4     - UDBs 0-3 of Extended Ipv6 L4 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     udb0_11          -  UDBs 0-11
 *                  Supported only for DxChXCat and above devices
 *                  and ignored for DxCh3 devices.
 *     udb12      - UDB 12 (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     udb39_40   - UDBs 39-40 (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     srcPortOrTrunk  -(The source port or trunk assigned to the packet.
 *                      APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     srcIsTrunk - Indicates packets received from a Trunk on a
 *                  remote device or on the local device
 *                  0 = Source is not a Trunk due to one of the following:
 *                   - A non-DSA-tagged packet received from a network port
 *                     which is not a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD
 *                     and DSA<SrcIsTrunk> =0
 *                  1 = Source is a Trunk due to one of the following:
 *                   - A non-DSA-tagged packet received from a network port
 *                    which is a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD and
 *                    DSA<SrcIsTrunk =1
 *                    Relevant only when packet is of type is FORWARD.
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     trunkHash  - Trunk Hash (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                  trunkHash muxed with UDB 1
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC        ingressIpCommon;
    /* L2 */
    GT_U8                                          isL2Valid;
    GT_U8                                          isNd;
    GT_U8                                          isBc;
    GT_U8                                          isIp;
    GT_U32                                         l2Encap;
    GT_U16                                         etherType;
    GT_ETHERADDR                                   macDa;
    GT_ETHERADDR                                   macSa;
    GT_U32                                         tag1Exist;
    GT_U32                                         vid1;
    GT_U32                                         up1;
    GT_U32                                         cfi1;
    /* L3 */
    GT_IPV6ADDR                                    sip;
    GT_IPV6ADDR                                    dip;
    GT_U8                                          isIpv6ExtHdrExist;
    GT_U8                                          isIpv6HopByHop;
    GT_U8                                          ipHeaderOk;
    GT_U32                                         vrfId;
    /* UDB */
    GT_U8                                          isUdbValid;
    GT_U8                                          UdbExtNotIpv6[4];
    GT_U8                                          UdbExtIpv6L2[5];
    GT_U8                                          UdbExtIpv6L4[3];
    GT_U8                                          udb0_11[12];
    GT_U8                                          udb12;
    GT_U8                                          udb39_40[2];

    GT_U32                                         srcPortOrTrunk;
    GT_U8                                          srcIsTrunk;
    GT_U32                                         trunkHash;

} CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC
 *
 * Description:
 *     This structure describes the ingress Ultra Routed Qos IPV6 Key.
 *     CH3 : ingress ultra IPV6 MPLS key format, used in second lookup.
 *
 * Fields:
 *     ingressIpCommon  - The common fields of all ingress keys.
 *     pktType     - packet type
 *                   0 = Non of the following.
 *                   1 = ARP.
 *                   2 = IPv6 Over MPLS.
 *                   3 = IPv4 Over MPLS.
 *                   4 = MPLS.
 *                   6 = IPv6.
 *                   7 = IPv4.
 *     isNd        - IS ND
 *     tag1Exist   - 0 - Tag1 exists in packet.
 *                   1 - Tag1 does not exist in packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     vid1        - VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         - UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1        - CFI from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     mplsOuterLabel   - MPLS Outer Label
 *     mplsOuterLabExp  - MPLS Outer Label EXP
 *     mplsOuterLabSBit - MPLS Outer Label S-Bit
 *
 *     ipPacketLength - IP Packet Length
 *     ipv6HdrFlowLabel - IPv6 Header <Flow Label>
 *     ttl          - time to live
 *     sip         - IPv6 source IP address field.
 *                   For ARP packets this field holds the sender's IPv6 address.
 *     dip         - IPv6 destination IP address field.
 *                   For ARP packets this field holds the target IPv6 address.
 *     isIpv6ExtHdrExist - IPv6 extension header exists.
 *                   Indicates that an IPv6 extension exists.
 *                   0 = Non-Ipv6 packet or IPv6 extension header does
 *                   not exist.
 *                   1 = Packet is IPv6 and extension header exists.
 *     isIpv6HopByHop    - Indicates that the IPv6 Original Extension Header
 *                   is Hop-by-Hop.
 *                   0 = Non-IPv6 packet or IPv6 extension header type is
 *                   not Hop-by-Hop Option Header.
 *                   1 = Packet is IPv6 and extension header type
 *                   is Hop-by-Hop Option Header.
 *     isIpv6LinkLocal - Is IPv6 Link local
 *     isIpv6Mld       - Is IPv6 MLD
 *     ipHeaderOk - Indicates a valid IP header.
 *     vrfId       - VRF Id (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                   vrfId[11:8] muxed with UDB 19
 *                   vrfId[7:0]  muxed with UDB 18
 *     isUdbValid - User-defined bytes (UDBs) are valid.
 *                  Indicates that all user-defined bytes used in that search
 *                  key were successfully parsed.
 *                  0 = At least 1 user-defined byte couldn't be parsed.
 *                  1 = All user-defined bytes used in this search key
 *                  were successfully parsed.
 *     UdbStdNotIp      - UDBs 0-1 of standard Not Ip key
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbStdIpL2Qos    - UDBs 0-2 of standard Ipv4/6 L2 Qos key ???
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbStdIpV4L4     - UDBs 0-2 of standard Ipv4 L2 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbExtNotIpv6    - UDBs 0-3 of Extended Not Ipv6 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbExtIpv6L2     - UDBs 0-2 of Extended Ipv6 L2 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     UdbExtIpv6L4     - UDBs 0-2 of Extended Ipv6 L4 Key
 *                  Supported only for DxCh3 device and ignored for other.
 *     udb0_11          - UDBs 0-11
 *                  Supported only for DxChXCat and above devices
 *                  and ignored for DxCh3 devices.
 *     udb17_22         - UDBs 17-22
 *                  Supported only for DxChXCat and above devices
 *                  and ignored for DxCh3 devices.
 *     udb45_46   - UDBs 45-46 (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     srcPortOrTrunk  -(The source port or trunk assigned to the packet.
 *                      APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     srcIsTrunk - Indicates packets received from a Trunk on a
 *                  remote device or on the local device
 *                  0 = Source is not a Trunk due to one of the following:
 *                   - A non-DSA-tagged packet received from a network port
 *                     which is not a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD
 *                     and DSA<SrcIsTrunk> =0
 *                  1 = Source is a Trunk due to one of the following:
 *                   - A non-DSA-tagged packet received from a network port
 *                    which is a trunk member
 *                   - A DSA-tagged packet with DSA<TagCmd> =FORWARD and
 *                    DSA<SrcIsTrunk =1
 *                    Relevant only when packet is of type is FORWARD.
 *                  (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     trunkHash  - Trunk Hash (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                  trunkHash muxed with UDB 1
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_IP_COMMON_STC        ingressIpCommon;
    GT_U32                                         pktType;
    /* L2 */
    GT_U8                                          isNd;
    GT_U32                                         tag1Exist;
    GT_U32                                         vid1;
    GT_U32                                         up1;
    GT_U32                                         cfi1;
    /* MPLS */
    GT_U32                                         mplsOuterLabel;
    GT_U32                                         mplsOuterLabExp;
    GT_U8                                          mplsOuterLabSBit;
    /* L3 */
    GT_U32                                         ipPacketLength;
    GT_U32                                         ipv6HdrFlowLabel;
    GT_U32                                         ttl;
    GT_IPV6ADDR                                    sip;
    GT_IPV6ADDR                                    dip;
    GT_U8                                          isIpv6ExtHdrExist;
    GT_U8                                          isIpv6HopByHop;
    GT_U8                                          isIpv6LinkLocal;
    GT_U8                                          isIpv6Mld;
    GT_U8                                          ipHeaderOk;
    GT_U32                                         vrfId;
    /* UDB */
    GT_U8                                          isUdbValid;
    GT_U8                                          UdbStdNotIp[2];
    GT_U8                                          UdbStdIpL2Qos[3];
    GT_U8                                          UdbStdIpV4L4[3];
    GT_U8                                          UdbExtNotIpv6[4];
    GT_U8                                          UdbExtIpv6L2[3];
    GT_U8                                          UdbExtIpv6L4[3];
    GT_U8                                          udb0_11[12];
    GT_U8                                          udb17_22[6];
    GT_U8                                          udb45_46[2];

    GT_U32                                         srcPortOrTrunk;
    GT_U8                                          srcIsTrunk;
    GT_U32                                         trunkHash;
} CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC
 *
 * Description:
 *     This structure describes the common part of egress IP keys.
 *     The fields placed at the same bits of the keys.
 *
 * Fields:
 *     pclId       - The PCL-ID attribute assigned to this lookup.
 *     sourcePort  - The port number from which the packet ingressed the device.
 *                   Port 63 is the CPU port.
 *     qosProfile  - The QoS Profile assigned to the packet until this lookup
 *     isL2Valid   - Indicates that Layer 2 information in the search key is valid.
 *                   0 = Layer 2 fields in the Ingress PCL (IPCL) or
 *                       Egress PCL (EPCL) are not valid
 *                   1 = Layer 2 field in the Ingress PCL or Egress PCL key is valid.
 *                   For Ingress PCL Keys: Layer 2 fields are not valid in the
 *                   IPCL keys when IP-Over-xxx is tunnel-terminated and its key
 *                   is based on the passenger IP packet which does not include
 *                   a Layer 2 header.
 *                   For Egress PCL Keys: Layer 2 fields are not valid in
 *                   the EPCL keys when an IP-Over-xxx is forwarded to a Tunnel
 *                   Start, and its PCL Key is based on the passenger IP packet
 *                   which does not include Layer 2 data.
 *     originalVid - original VLAN ID assigned to the packet.
 *                   For DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2
 *                   VLAN Id is 12 bit.
 *                   For Bobcat2, Caelum, Bobcat3 devices support of 16-bit VLAN Id.
 *     isSrcTrunk   - Is Source information (srcDevOrTrunkId) is Trunk ID
 *                    0 = srcDevOrTrunkId is source device number
 *                    1 = srcDevOrTrunkId is trunk ID
 *     srcDevOrTrunkId - Source device number or trunk ID. See isSrcTrunk.
 *                       For Bobcat2, Caelum, Bobcat3 devices used only for TrunkId,
 *                       for srcHwDev value see separate field srcDev.
 *     srcHwDev      - Together with <SrcPort> and <srcHwDev> indicates the
 *                   network port at which the packet was received.
 *                   Relevant for all packet types exclude the
 *                   FWD_DATA packed entered to the PP from Trunk
 *                   Fields srcTrunkId and srcHwDev are muxed.
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     isIp        - An indication that the packet is IP
 *                   0 = Non IP packet.
 *                   1 = IPv4/6 packet.
 *     isArp       - An indication that the packet is an ARP packet
 *                   (identified by EtherType == 0x0806.)
 *                   0 = Non ARP packet.
 *                   1 = ARP packet.
 *     macDa       - Ethernet Destination MAC address.
 *     macSa       - Ethernet Source MAC address.
 *     tag1Exist   - 0 - Tag1 exists in packet.
 *                   1 - Tag1 does not exist in packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     sourceId    - The Source ID assigned to the packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     tos         - 8-bit TOS (contains DSCP in bits 7:2)
 *     ipProtocol  - IP protocol/Next Header type.
 *     ttl         - time to live
 *
 *     isL4Valid  - Layer 4 information is valid.
 *                  This field indicates that all the Layer 4
 *                  information required for the search key is available
 *                  and the IP header is valid.
 *                  0 = Layer 4 information is not valid.
 *                  1 = Layer 4 information is valid.
 *                  Layer 4 information may not be available for any of the
 *                   following reasons:
 *                  - Layer 4 information is not included in the packet.
 *                     For example, Layer 4 information
 *                     isn't available in non-IP packets, or in IPv4
 *                     non-initial-fragments.
 *                  - Parsing failure: Layer 4 information is beyond
 *                     the first 128B of the packet, or beyond
 *                     IPv6 extension headers parsing capabilities.
 *                  - IP header is invalid.
 *     l4Byte0    - The following Layer 4 information is available for
 *                  UDP and TCP packets - L4 Header Byte0 through Byte3,
 *                  which contain the UDP/TCP destination and source ports.
 *                  For TCP also L4 Header Byte13, which contains the TCP flags.
 *                  For IGMP L4 Header Byte0, which contain the IGMP Type.
 *                  For ICMP L4 Header Byte0 and Byte1,
 *                  which contain the ICMP Type and Code fields
 *                  and L4 Header Byte4<in l4Byte2> and Byte5 <in l4Byte3>,
 *                  which contain the ICMP authentication field.
 *                  The following Layer 4 information is available for packets
 *                  that are not TCP, UDP, IGMP or ICMP:
 *                  L4 Header Byte0 through Byte3 L4 Header Byte13
 *     l4Byte1    - see l4Byte0
 *     l4Byte2    - see l4Byte0
 *     l4Byte3    - see l4Byte0
 *     l4Byte13   - see l4Byte0
 *     tcpUdpPortComparators - The transmitted packet TCP/UDP comparator result
 *     tc         - traffic class 3-bits
 *     dp         - drop priority 2-bits
 *     egrPacketType - two bits field:
 *                   0 - packet to CPU
 *                   1 - packet from CPU
 *                   2 - packet to ANALYZER
 *                   3 - forward DATA packet
 *     srcTrgOrTxMirror - Source/Target or Tx Mirror
 *                   For TO CPU packets this field is <SrcTrg> of DSA tag.
 *                   For TO_TARGET_SNIFFER packets this field is inverted
 *                   <RxSniff> of DSA tag.
 *                   For FORWARD and FROM_CPU packets this field is 0
 *     assignedUp -  3-bit user priority
 *     trgPhysicalPort    -  target physical port
 *     rxSniff -     Indicates if this packet is Ingress or Egress
 *                   mirrored to the Analyzer.
 *                   0 = This packet is Egress mirrored to the analyzer.
 *                   1 = This packet is Ingress mirrored to the analyzer.
 *                   Relevant only when packet is of type is TO_ANALYZER.
 *     isRouted    - Indicates that the packet has been routed either by the
 *                   local device or some previous device in a cascaded system.
 *                   0 = The packet has not been routed.
 *                   1 = The packet has been routed.
 *                   Relevant only when packet is of type is FORWARD.
 *     isIpv6     - An indication that the packet is IPv6.
 *                  0 = Non IPv6 packet.
 *                  1 = IPv6 packet.
 *     cpuCode     - The CPU Code forwarded to the CPU.
 *                   Relevant only when packet is of type is TO_CPU.
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     srcTrg      - The <SrcTrg> field extracted from the TO_CPU DSA<SrcTrg>:
 *                   0 = The packet was sent to the CPU by the ingress pipe and
 *                   the DSA tag contain attributes related to the
 *                    packet ingress.
 *                   1 = The packet was sent to the CPU by the egress pipe
 *                   and the DSA tag contains attributes related to the
 *                   packet egress.
 *                   Relevant only when packet is of type is TO_CPU.
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     egrFilterEnable - The <EgressFilterEn> extracted from
 *                   FROM_CPU DSA<EgressFilterEn>:
 *                   0 = FROM_CPU packet is subject to egress filtering,
 *                   e.g. data traffic from the CPU
 *                   1 = FROM_CPU packet is not subject egress filtering,
 *                   e.g. control traffic from the CPU
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */
typedef struct
{
    GT_U32          pclId;
    GT_PHYSICAL_PORT_NUM  sourcePort;
    GT_U32           qosProfile;
    /* L2 */
    GT_U8           isL2Valid;
    GT_U32          originalVid;
    GT_U8           isSrcTrunk;
    GT_U32          srcDevOrTrunkId;
    GT_HW_DEV_NUM   srcHwDev;
    GT_U8           isIp;
    GT_U8           isArp;
    GT_ETHERADDR    macDa;
    GT_ETHERADDR    macSa;
    GT_U8           tag1Exist;
    GT_U32          sourceId;
    /* L3 */
    GT_U32          tos;
    GT_U32          ipProtocol;
    GT_U32          ttl;
    /* L4 */
    GT_U8           isL4Valid;
    GT_U8           l4Byte0;
    GT_U8           l4Byte1;
    GT_U8           l4Byte2;
    GT_U8           l4Byte3;
    GT_U8           l4Byte13;
    GT_U32          tcpUdpPortComparators;
    /* egress */
    GT_U32          tc;
    GT_U32          dp;
    GT_U32          egrPacketType;
    GT_U8           srcTrgOrTxMirror;
    GT_U32          assignedUp;
    GT_PHYSICAL_PORT_NUM  trgPhysicalPort;
    GT_U8           rxSniff;
    GT_U8           isRouted;
    GT_U8           isIpv6;
    GT_U32          cpuCode;
    GT_U8           srcTrg;
    GT_U8           egrFilterEnable;
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC;


/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC
 *
 * Description:
 *     This structure describes the Egress Extended (48B) RACL/VACL IPv4 key format.
 *      CH3 : egress extended (48B) IPV4 key format.
 *
 * Fields:
 *     egressIpCommon   - The common fields of egress IP keys.
 *     sip         - IPv4 source IP address field.
 *                   For ARP packets this field holds the sender's IPv4 address.
 *     dip         - IPv4 destination IP address field.
 *                   For ARP packets this field holds the target IPv4 address.
 *     ipv4Options - IP V4 Options
 *                   For non-tunnel start packets:
 *                   0 = The transmitted packet is non-Ipv4 or an IPv4 options
 *                       do not exist.
 *                   1 = The transmitted packet is IPv4 and an IPv4 options
 *                       exist.
 *                   For tunnel-start packets:
 *                   - always 0
 *     isVidx      - Indicates that a packet is forwarded to a Multicast group.
 *                   0 = The packet is a Unicast packet forwarded to a
 *                   specific target port or trunk.
 *                   1 = The packet is a multi-destination packet forwarded
 *                   to a Multicast group.
 *                   Relevant only for DxChXcat and above devices.
 *     vid1        - VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         - UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1        - CFI from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     isUdbValid  - User-defined bytes (UDBs) are valid.
 *                   Indicates that all user-defined bytes used in that search
 *                   key were successfully parsed.
 *                   0 = At least 1 user-defined byte couldn't be parsed.
 *                   1 = All user-defined bytes used in this search key
 *                   were successfully parsed.
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     udb36_49    - UDBs 36-49 (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     udb0        - UDB 0 (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC egressIpCommon;
    GT_IPADDR                                     sip;
    GT_IPADDR                                     dip;
    GT_U8                                         ipv4Options;
    GT_U8                                         isVidx;
    GT_U32                                        vid1;
    GT_U32                                        up1;
    GT_U32                                        cfi1;
    GT_U8                                         isUdbValid;
    GT_U8                                         udb36_49[14];
    GT_U8                                         udb0;
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC
 *
 * Description:
 *     This structure describes the Egress Ultra (72B) RACL/VACL IPv6 key format.
 *      CH3 : egress Ultra (72B) IPV6 key format.
 *
 * Fields:
 *     egressIpCommon   - The common fields of egress IP keys.
 *     sip         - IPv6 source IP address field.
 *                   For ARP packets this field holds the sender's IPv6 address.
 *     dip         - IPv6 destination IP address field.
 *     isNd        - Is ND
 *     isIpv6ExtHdrExist - IPv6 extension header exists.
 *                   Indicates that an IPv6 extension exists.
 *                   0 = Non-Ipv6 packet or IPv6 extension header does
 *                   not exist.
 *                   1 = Packet is IPv6 and extension header exists.
 *     isIpv6HopByHop    - Indicates that the IPv6 Original Extension Header
 *                   is Hop-by-Hop.
 *                   0 = Non-IPv6 packet or IPv6 extension header type is
 *                   not Hop-by-Hop Option Header.
 *                   1 = Packet is IPv6 and extension header type
 *                   is Hop-by-Hop Option Header.
 *     isVidx      - Indicates that a packet is forwarded to a Multicast group.
 *                   0 = The packet is a Unicast packet forwarded to a
 *                   specific target port or trunk.
 *                   1 = The packet is a multi-destination packet forwarded
 *                   to a Multicast group.
 *                   Relevant only for DxChXcat and above devices.
 *     vid1        - VID from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     up1         - UP  from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     cfi1        - CFI from Tag1, valid only if Tag1 exist.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     srcPort     - Source port assign to the packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     trgPort     - Egress port of the packet.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     isUdbValid  - User-defined bytes (UDBs) are valid.
 *                   Indicates that all user-defined bytes used in that search
 *                   key were successfully parsed.
 *                   0 = At least 1 user-defined byte couldn't be parsed.
 *                   1 = All user-defined bytes used in this search key
 *                   were successfully parsed.
 *                   (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *     udb1_4      - UDBs 1-4 (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *  Comments:
 */
typedef struct
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_IP_COMMON_STC egressIpCommon;
    GT_IPV6ADDR                                    sip;
    GT_IPV6ADDR                                    dip;
    GT_U8                                          isNd;
    GT_U8                                          isIpv6ExtHdrExist;
    GT_U8                                          isIpv6HopByHop;
    GT_U8                                          isVidx;
    GT_U32                                         vid1;
    GT_U32                                         up1;
    GT_U32                                         cfi1;
    GT_U32                                         srcPort;
    GT_U32                                         trgPort;
    GT_U8                                          isUdbValid;
    GT_U8                                          udb1_4[4];
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC
 *
 * Description:
 *     This structure describes the Ingress UDB only keys fields
 *     replacing the user defined bytes and being enabled/disabled
 *     by cpssDxChPclUserDefinedBytesSelectSet.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Fields:
 *     pclId        - pclId UDB0  muxed with pclId[7:0],
 *                    UDB1[1:0] muxed with pclId[9:8]
 *                    (APPLICABLE RANGES: 0..0x3FF)
 *     isUdbValid   - UDB Valid: 1 when all UDBs in the key valid, otherwise 0,
 *                    muxed with UDB1[7]
 *                    (APPLICABLE RANGES: 0..1)
 *     vid          - eVlan, UDB2 muxed with vid[7:0],
 *                    UDB3 muxed with vid[12:8]
 *                    (APPLICABLE RANGES: 0..0x1FFF)
 *     srcPort      - source ePort, UDB4 muxed with srcPort[7:0],
 *                    UDB5 muxed with srcPort[12:8]
 *                    (APPLICABLE RANGES: 0..0x1FFF)
 *     srcDevIsOwn  - source Device Is Own
 *                    (APPLICABLE RANGES: 0..1)
 *     vid1         - tag1 VID, UDB6 muxed with vid1[7:0],
 *                    UDB7[3:0] muxed with vid1[11:8]
 *                    (APPLICABLE RANGES: 0..0xFFF)
 *     up1          - tag1 UP, UDB7[6:4] muxed with up1[2:0]
 *                    (APPLICABLE RANGES: 0..7)
 *     macToMe       - mac to me, UDB7[7] muxed with mac2me
 *                    (APPLICABLE RANGES: 0..1)
 *     qosProfile   - qosProfile, UDB8 muxed with qosProfile[7:0],
 *                    UDB9[1:0] muxed with qosProfile[9:8]
 *                    (APPLICABLE RANGES: 0..0x3FF)
 *     flowId       - flow Id, UDB10 muxed with flowId[7:0]
 *                    UDB11[3:0] muxed with flowId[11:8]
 *                    (APPLICABLE RANGES: 0..0xFFF)
 *
 *  Comments:
 */
typedef struct
{
    GT_U32    pclId;
    GT_U8     isUdbValid;
    GT_U32    vid;
    GT_U32    srcPort;
    GT_U8     srcDevIsOwn;
    GT_U32    vid1;
    GT_U32    up1;
    GT_U8     macToMe;
    GT_U32    qosProfile;
    GT_U32    flowId;
} CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC
 *
 * Description:
 *     This structure describes the Ingress UDB60 keys fixed fields.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Fields:
 *     isUdbValid   - UDB Valid
 *                    (APPLICABLE RANGES: 0..1)
 *     pclId        - PCL-ID
 *                    (APPLICABLE RANGES: 0..0x3FF)
 *     vid          - eVlan
 *                    (APPLICABLE RANGES: 0..0x1FFF)
 *     srcPort      - source ePort
 *                    (APPLICABLE RANGES: 0..0x1FFF)
 *     srcDevIsOwn  - source Device Is Own
 *                    (APPLICABLE RANGES: 0..1)
 *     vid1         - tag1 VID
 *                    (APPLICABLE RANGES: 0..0xFFF)
 *     up1          - tag1 UP
 *                    (APPLICABLE RANGES: 0..7)
 *     macToMe      - mac to me
 *                    (APPLICABLE RANGES: 0..1)
 *     qosProfile   - qosProfile
 *                    (APPLICABLE RANGES: 0..0x3FF)
 *     flowId       - flow Id
 *                    (APPLICABLE RANGES: 0..0xFFF)
 *
 *  Comments:
 */
typedef struct
{
    GT_U8     isUdbValid;
    GT_U32    pclId;
    GT_U32    vid;
    GT_U32    srcPort;
    GT_U8     srcDevIsOwn;
    GT_U32    vid1;
    GT_U32    up1;
    GT_U8     macToMe;
    GT_U32    qosProfile;
    GT_U32    flowId;
} CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC
 *
 * Description:
 *     This structure describes the Ingress UDB only keys formats.
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Fields:
 *     udb           - UDBs.
 *                     For UDB_10 key relevant only udb[0] - udb[9]
 *                     For UDB_20 key relevant only udb[0] - udb[19]
 *                     For UDB_30 key relevant only udb[0] - udb[29]
 *                     For UDB_40 key relevant only udb[0] - udb[39]
 *                     For UDB_50 and UDB_60 key relevant all udb[0] - udb[49]
 *                     Not relevant UDBs are ignored.
 *     replacedFld   - replaced fixed fields.
 *     udb60FixedFld - fixed fields,
 *                     relevant only for CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E keys.
 *
 *  Comments:
 */
typedef struct
{
    GT_U8                                                udb[CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS];
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_REPLACED_STC   replacedFld;
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB60_FIXED_STC    udb60FixedFld;
} CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC
 *
 * Description:
 *     This structure describes the Egress UDB only keys fields
 *     replacing the user defined bytes and being enabled/disabled
 *     by cpssDxChPclUserDefinedBytesSelectSet.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Fields:
 *     pclId        - PCL-ID UDB0  muxed with PCL-ID[7:0],
 *                    UDB1[1:0] muxed with PCL-ID[9:8]
 *                    (APPLICABLE RANGES: 0..0x3FF)
 *     isUdbValid   - UDB Valid: 1 when all UDBs in the key valid, otherwise 0,
 *                    muxed with UDB1[7]
 *                    (APPLICABLE RANGES: 0..1)
 *
 *  Comments:
 */
typedef struct
{
    GT_U32    pclId;
    GT_U8     isUdbValid;
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC
 *
 * Description:
 *     This structure describes the Egress UDB60 keys fixed fields.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Fields:
 *     pclId        - PCL-ID
 *                    (APPLICABLE RANGES: 0..0x3FF)
 *     isUdbValid   - UDB Valid
 *                    (APPLICABLE RANGES: 0..1)
 *     vid          - eVlan
 *                    (APPLICABLE RANGES: 0..0x1FFF)
 *     srcPort      - source ePort
 *                    (APPLICABLE RANGES: 0..0x1FFF)
 *     trgPort      - target ePort
 *                    (APPLICABLE RANGES: 0..0x1FFF)
 *     srcDev       - source Device
 *                    (APPLICABLE RANGES: 0..0x3FF)
 *     trgDev       - target Device
 *                    (APPLICABLE RANGES: 0..0x3FF)
 *     localDevTrgPhyPort  - local device target physical port
 *                    (APPLICABLE RANGES: 0..0xFF)
 *
 *  Comments:
 */
typedef struct
{
    GT_U32                  pclId;
    GT_U8                   isUdbValid;
    GT_U32                  vid;
    GT_U32                  srcPort;
    GT_U32                  trgPort;
    GT_U32                  srcDev;
    GT_U32                  trgDev;
    GT_PHYSICAL_PORT_NUM    localDevTrgPhyPort;
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC;

/*
 * Typedef: struct CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC
 *
 * Description:
 *     This structure describes the Egress UDB only keys formats.
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
 *     CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Fields:
 *     udb          - UDBs.
 *                    For UDB_10 key relevant only udb[0] - udb[9]
 *                    For UDB_20 key relevant only udb[0] - udb[19]
 *                    For UDB_30 key relevant only udb[0] - udb[29]
 *                    For UDB_40 key relevant only udb[0] - udb[39]
 *                    For UDB_50 and UDB_60 key relevant all udb[0] - udb[49]
 *                    Not relevant UDBs are ignored.
 *     replacedFld   - replaced fixed fields.
 *     udb60FixedFld - fixed fields,
 *                     relevant only for CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E keys.
 *
 *  Comments:
 */
typedef struct
{
    GT_U8                                                udb[CPSS_DXCH_PCL_UDB_MAX_NUMBER_CNS];
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_REPLACED_STC    replacedFld;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB60_FIXED_STC     udb60FixedFld;
} CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC;


/*
 * Typedef: union CPSS_DXCH_PCL_RULE_FORMAT_UNT
 *
 * Description:
 *     This union describes the PCL key.
 *
 * Fields:
 *     Ingress Policy Key format structures
 *     ruleStdNotIp       - Standard Not IP packet key
 *     ruleStdIpL2Qos     - Standard IPV4 and IPV6 packets
 *                          L2 and QOS styled key
 *     ruleStdIpv4L4      - Standard IPV4 packet L4 styled key
 *     ruleStdIpv6Dip     - Standard IPV6 packet DIP styled key (DxCh only)
 *     ruleIngrStdUdb     - Ingress Standard UDB styled packet key
 *     ruleExtNotIpv6     - Extended Not IP and IPV4 packet key
 *     ruleExtIpv6L2      - Extended IPV6 packet L2 styled key
 *     ruleExtIpv6L4      - Extended IPV6 packet L2 styled key
 *     ruleIngrExtUdb     - Ingress Extended UDB styled packet key
 *     ruleEgrStdNotIp    - Egress Standard Not IP packet key
 *     ruleEgrStdIpL2Qos  - Egress Standard IPV4 and IPV6 packets
 *                          L2 and QOS styled key
 *     ruleEgrStdIpv4L4   - Egress Standard IPV4 packet L4 styled key
 *     ruleEgrExtNotIpv6  - Egress Extended Not IP and IPV4 packet key
 *     ruleEgrExtIpv6L2   - Egress Extended IPV6 packet L2 styled key
 *     ruleEgrExtIpv6L4   - Egress Extended IPV6 packet L2 styled key
 *     ruleStdIpv4RoutedAclQos   - see CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E
 *     ruleExtIpv4PortVlanQos    - see CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E
 *     ruleUltraIpv6PortVlanQos  - see CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E
 *     ruleUltraIpv6RoutedAclQos - see CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E
 *     ruleEgrExtIpv4RaclVacl    - see CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E
 *     ruleEgrUltraIpv6RaclVacl  - see CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E
 *
 *     ruleIngrUdbOnly    - Ingress UDB only formats.
 *                          CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
 *     ruleEgrUdbOnly     - Egress UDB only formats.
 *                          CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
 *                          CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
 *
 *  Comments:
 */
typedef union
{
    CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC             ruleStdNotIp;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC          ruleStdIpL2Qos;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC            ruleStdIpv4L4;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC           ruleStdIpv6Dip;
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC   ruleIngrStdUdb;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC           ruleExtNotIpv6;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC            ruleExtIpv6L2;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC            ruleExtIpv6L4;
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC   ruleIngrExtUdb;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_STC      ruleEgrStdNotIp;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_STC   ruleEgrStdIpL2Qos;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_STC     ruleEgrStdIpv4L4;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_STC    ruleEgrExtNotIpv6;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_STC     ruleEgrExtIpv6L2;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_STC     ruleEgrExtIpv6L4;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_ROUTED_ACL_QOS_STC     ruleStdIpv4RoutedAclQos;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV4_PORT_VLAN_QOS_STC      ruleExtIpv4PortVlanQos;
    CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_PORT_VLAN_QOS_STC    ruleUltraIpv6PortVlanQos;
    CPSS_DXCH_PCL_RULE_FORMAT_ULTRA_IPV6_ROUTED_ACL_QOS_STC   ruleUltraIpv6RoutedAclQos;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_STC   ruleEgrExtIpv4RaclVacl;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_STC ruleEgrUltraIpv6RaclVacl;
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_ONLY_STC       ruleIngrUdbOnly;
    CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_ONLY_STC        ruleEgrUdbOnly;

} CPSS_DXCH_PCL_RULE_FORMAT_UNT;

/*
 * typedef: enum CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT
 *
 * Description: packet types that can be enabled/disabled for Egress PCL.
 *              By default on all ports and on all packet types are disabled
 *
 * Enumerations:
 *   CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E - Enable/Disable Egress Policy
 *                   for Control packets FROM CPU.
 *   CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E    - Enable/Disable Egress Policy
 *                   for data packets FROM CPU.
 *   CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E           - Enable/Disable Egress Policy
 *                   on TO-CPU packets
 *   CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E      - Enable/Disable Egress Policy
 *                   for TO ANALYZER packets.
 *   CPSS_DXCH_PCL_EGRESS_PKT_TS_E               - Enable/Disable Egress Policy
 *                   for data packets that are tunneled in this device.
 *   CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E           - Enable/Disable Egress Policy
 *                   for data packets that are not tunneled in this device.
 *
 * Comments:
 *    relevant only for DxCh2 and above devices
 */
typedef enum
{
    CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_CONTROL_E,
    CPSS_DXCH_PCL_EGRESS_PKT_FROM_CPU_DATA_E,
    CPSS_DXCH_PCL_EGRESS_PKT_TO_CPU_E,
    CPSS_DXCH_PCL_EGRESS_PKT_TO_ANALYZER_E,
    CPSS_DXCH_PCL_EGRESS_PKT_TS_E,
    CPSS_DXCH_PCL_EGRESS_PKT_NON_TS_E
} CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT
 *
 * Description: Global Egress PCL or Ingress Policy
 *              Configuration Table Access mode
 *
 * Enumerations:
 *   CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E - access by local port number
 *   CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E - access by remote device Id
 *             and Port number or by trunk Id,
 *             Used for Value Blade systems based on DX switch device.
 *             The remote port and device are taken from DSA tag and
 *             represent real source port and device for a packets.
 *
 * Comments:
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_LOCAL_PORT_E,
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_NON_LOCAL_PORT_E
} CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT;


/*
 * typedef: enum CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT
 *
 * Description: size of Ingress Policy / Egress PCL Configuration
 *              table segment accessed by "non-local device and port"
 *
 * Enumerations:
 *   CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E - support 32 port remote devices
 *   CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E - support 64 port remote devices
 *
 * Comments:
 *    relevant only for DxCh2 and above devices
*/
typedef enum
{
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_32_E,
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_64_E
} CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT
 *
 * Description: base of Ingress Policy / Egress PCL Configuration
 *              table segment accessed
 *              by "non-local device and port"
 *
 * Enumerations:
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E    - the base is 0
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E - the base is 1024 (only 32-port)
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E - the base is 2048
 *   CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E - the base is 3072 (only 32-port)
 *
 * Comments:
 *    relevant only for DxCh2 and above devices
 *    Both egress and ingress PCL Configuration tables contain 4096
 *    entries block "entry per VLAN".
 *    In Non-local mode the packets that mapped by device-and-port served
 *    by entry from MAX-DEVICES(i.e.32)* MAX-PORTS(i.e. 32 * 64) entries block
 *    that overlaps the "entry per VLAN" block, but least than it
 *    (the size is 1024 or 2048 entries). The values upper dedicated to
 *    configure it's position.
 *
 */
typedef enum
{
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE0_E,
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE1024_E,
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE2048_E,
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE3072_E
} CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT;

/*
 * typedef: enum CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_ENT
 *
 * Description: TCAM segment mode.
 *     Defines whether the 4 TCAM segments assigned to the PCL are
 *     regarded as one logical TCAM returning a single reply,
 *     two separate TCAM segments returning two replies,
 *     or four separate TCAM segments returning four replies.
 *     The segment size measured in quarters of TCAM.
 *     (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Enumerations:
 *   CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_1_TCAM_E          - all TCAM segments are one logical TCAM
 *   CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_2_AND_2_E - segmentation into 2 lists,
 *                    logically distributed 50% and 50%
 *   CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_1_AND_3_E - segmentation into 2 lists,
 *                    logically distributed 25% [first] and 75% [second];
 *   CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_3_AND_1_E - segmentation into 2 lists,
 *                    logically distributed 75% [first] and 25% [second];
 *   CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_4_TCAMS_E         - segmentation into 4 lists, each - 25%
 *
 * Comments:
 *
*/
typedef enum
{
    CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_1_TCAM_E,
    CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_2_AND_2_E,
    CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_1_AND_3_E,
    CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_2_TCAMS_3_AND_1_E,
    CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_4_TCAMS_E
} CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_PCL_LOOKUP_CFG_STC
 *
 * Description: PCL Configuration Table Entry Per Lookup Attributes.
 *
 * Fields:
 *    enableLookup           - Enable Lookup
 *                             GT_TRUE - enable Lookup
 *                             GT_FALSE - disable Lookup
 *                             The enableLookup enables or disables only
 *                             Lookup0_0 for xCat and above devices and
 *                             when the CPSS_PCL_LOOKUP_0_E lookup is used.
 *                             The dualLookup enables or disables Lookup0_1
 *                             in this case.
 *    pclId                  - PCL-ID bits in the TCAM search key.
 *    dualLookup             - Relevant for CPSS_PCL_LOOKUP_0_E  lookup only.
 *                             For DxCh3 devices
 *                                 GT_TRUE -  lookup0 has 2 sublookups.
 *                                 GT_FALSE - lookup0 has only 1 sublookup.
 *                                 lookup0_0 use pclId as is.
 *                                 lookup0_1 uses pclId with inverted LSB.
 *                             For xCat and above devices and above
 *                                 GT_TRUE -  lookup0_1 enabled.
 *                                 lookup0_1 uses pclIdL01.
 *                                 GT_FALSE - lookup0_1 disabled.
 *    pclIdL01               - only for xCat and above devices.
 *                             relevant for CPSS_PCL_LOOKUP_0_E  lookup only.
 *                             independent PCL ID for lookup0_1
 *    groupKeyTypes.nonIpKey - type of TCAM search key for NON-IP packets.
 *    groupKeyTypes.ipv4Key  - type of TCAM search key for IPV4 packets
 *    groupKeyTypes.ipv6Key  - type of TCAM search key for IPV6 packets
 *    udbKeyBitmapEnable     - replace 32 UDBs in the key by 256-bit bitmap
 *                             GT_FALSE - Disable; No key change;
 *                             GT_TRUE  - Enable.
 *                             For Ingress PCL
 *                             Enables replacing the first 32 UDBs in a UDB only key
 *                             (minimal key size of 40B) with a bitmap vector of 256 bits
 *                             that has only one bit set,representing the value of UDB23:
 *                             UDB Bitmap = 2^UDB23.
 *                             Related keys:
 *                             CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
 *                             CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
 *                             CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
 *                             For Egress PCL
 *                             Bitmap vector of 256 bits that has only one bit set,
 *                             representing the value of UDB49. That is: UDB Bitmap = 2^UDB49
 *                             If the selected key is a UDB only key with at least 50 UDBs,
 *                             UDBs 14-45 are replaced with bitmap representing the value of UDB49
 *                             (2^UDB49);
 *                             Related keys:
 *                             CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
 *                             CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
 *                             (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *    tcamSegmentMode        - TCAM Segment Mode (for parallel lookup)
 *                             (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 Comments:
 *    1. For DxCh2 and above devices the entry contains
 *    - "lookup enable" bit
 *    - 10 "PCL-ID" bits
 *    - 1 bit "not IP key" type standard or extended
 *    - 2 bits "IPV4 key" type std-L2, std-L4, ext
 *    - 2 bits "IPV6 key" type std-L2, ext-L2, ext-L4
 *    2. For DxCh1 devices the entry contains
 *    - "lookup enable" bit
 *    - 10 "PCL-ID" bits
 *    - 1 bit - key standard or extended (for all types)
 *    - 1 bit - key L2 or L4 styled for standard IPv4 or extended IPv6 packets
 *    - 1 bit - (lookup1 only) IPV6 std-DIP key
 *    2.1 Acceptable key types for DxCh1 devices.
 *    2.1.1  Standard Keys.
 *          nonIpKey - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E
 *          ipv4Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
 *                       L2 style of key for IPv4 packets
 *                     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E
 *                       L4 style of key for IPv4 packets
 *          ipv6Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E
 *                       L2 style of key for IPv6 packets
 *                     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E
 *                      IPv6 DIP key for lookup#1 only.
 *    2.1.2 Extended Keys.
 *          nonIpKey - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
 *          ipv4Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E
 *          ipv6Key  - CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E
 *                       L2 style of key for IPv6 packets
 *                     CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E
 *                       L4 style of key for IPv6 packets
 *    3. The SW structure can represent any possible HW configuration
 *    If an impossible configuration passed an appropriate APIs
 *    GT_BAD_PARAM is returned.
 *
 *    4. For xCat2 if CPSS_PCL_LOOKUP_0_E specified
 *    the groupKeyTypes packet types configured both for
 *    lookup_0_0 and for lookup_0_1 by the same
 *    values. To configure them separately specify CPSS_PCL_LOOKUP_0_0_E
 *    and CPSS_PCL_LOOKUP_0_1_E.
 */
typedef struct
{
    GT_BOOL                                   enableLookup;
    GT_U32                                    pclId;
    GT_BOOL                                   dualLookup;
    GT_U32                                    pclIdL01;
    struct
    {
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    nonIpKey;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    ipv4Key;
        CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    ipv6Key;
    }                                         groupKeyTypes;
    GT_BOOL                                   udbKeyBitmapEnable;
    CPSS_DXCH_PCL_TCAM_SEGMENT_MODE_ENT       tcamSegmentMode;

} CPSS_DXCH_PCL_LOOKUP_CFG_STC;

/*
 * typedef: struct CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC
 *
 * Description: global access mode to Ingress or Egress
 *              PCL configuration tables.
 *
 * Fields:
 *    ipclAccMode     - Ingress Policy local/non-local mode
 *    ipclMaxDevPorts - Ingress Policy support 32/64 port remote devices
 *    ipclDevPortBase - Ingress Policy Configuration table access base
 *    epclAccMode     - Egress PCL local/non-local mode
 *    epclMaxDevPorts - Egress PCL support 32/64 port remote devices
 *    epclDevPortBase - Egress PCL Configuration table access base
 * Comments:
 *   - for DxCh2 and above all fields are relevant
 *   - for DxCh1 relevant only ipclAccMode member
 *
 */
typedef struct
{
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT   ipclAccMode;
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT ipclMaxDevPorts;
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT ipclDevPortBase;
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT   epclAccMode;
    CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT epclMaxDevPorts;
    CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT epclDevPortBase;

} CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC;

/*
 * Typedef: enum CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT
 *
 * Description: enumerator for Egress PCL
 *              VID and UP key fields content mode
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E   - extract from packet Tag0
 *  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E   - extract from packet Tag1
 * Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG0_E,
    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_FROM_TAG1_E
} CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT
 *
 * Description: enumerator for OAM packet MEG Level mode
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_UDB_E      - MEG Level is UDB2[7:5] (OAM profile0)
 *                                                              or UDB3[7:5] (OAM profile1)
 *  CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_MAC_DA_E   - MEG Level is MAC DA[2:0]
 *  CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_EXP_E      - MEG Level is MPLS EXP[2:0] (egress only)
 *  CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_QOS_E      - MEG Level is QOS profile[2:0] (ingress only)
 * Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_UDB_E,
    CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_MAC_DA_E,
    CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_EXP_E,
    CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_QOS_E
} CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT;

/*
 * Typedef: enum CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT
 *
 * Description: enumerator for OAM Packet Detection Mode mode
 *
 * Enumerations:
 *  CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_STANDARD_E   - OAM packets
 *                     are detected based on EPCL rules.
 *  CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E  - OAM packets
 *                     are detected based on EPCL rule AND a dedicated
 *                     Source ID value. This mode allows the CPU to mark
 *                     OAM packets by sending them with a dedicated Source ID.
 * Comments:
 */
typedef enum
{
    CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_STANDARD_E,
    CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E
} CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT;

/*******************************************************************************
* cpssDxChPclInit
*
* DESCRIPTION:
*   The function initializes the device for following configuration
*   and using Policy engine
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclInit
(
    IN GT_U8                           devNum
);

/*******************************************************************************
* cpssDxChPclUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN CPSS_PCL_DIRECTION_ENT               direction,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
);

/*******************************************************************************
* cpssDxChPclUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
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
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_DIRECTION_ENT               direction,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
);

/*******************************************************************************
* cpssDxChPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has up to 3.5K (3584) rows.
*                          See datasheet of particular device for TCAM size.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                               Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The pattern and mask input structures should be set to zero
*      by an application before assignment of fields.
*      The function places all muxed fields to rule patern and mask
*      by using bitwise OR operation.
*      The function does not check validity of muxed (overlapped) fields.
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleSet
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT      ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclRuleParsedGet
*
* DESCRIPTION:
*   The function gets the Policy Rule valid status, Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*
* OUTPUTS:
*       isRuleValidPtr   - GT_TRUE - rule is valid, GT_FALSE - rule is invalid
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - when size is trash or not related to ruleFormat.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleParsedGet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN  GT_U32                                           ruleIndex,
    IN  CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    OUT GT_BOOL                                          *isRuleValidPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *maskPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                    *patternPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC                         *actionPtr
);

/*******************************************************************************
* cpssDxChPclRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN CPSS_PCL_DIRECTION_ENT              direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclRuleSet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleInvalidate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
);

/*******************************************************************************
* cpssDxChPclRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
);

/*******************************************************************************
* cpssDxChPclRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclRuleCopy should be
*  called. And after this cpssDxChPclRuleInvalidate or
*  cpssDxChPclRuleValidStatusSet should be used
*  to invalidate Rule in old position.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of Rule.
*       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
*                           mask and action are taken.
*       ruleDstIndex     - index of the rule in the TCAM to which pattern,
*                           mask and action are placed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleCopy
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
);

/*******************************************************************************
* cpssDxChPclRuleMatchCounterGet
*
* DESCRIPTION:
*       Get rule matching counters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       counterPtr   - pointer to the counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleMatchCounterGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    OUT GT_U32                        *counterPtr
);

/*******************************************************************************
* cpssDxChPclRuleMatchCounterSet
*
* DESCRIPTION:
*       Set rule matching counters.
*       To reset value of counter use counterValue = 0
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (APPLICABLE RANGES: 0..31)
*       counterValue - counter value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleMatchCounterSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    IN  GT_U32                        counterValue
);

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnable
*
* DESCRIPTION:
*    Enables/disables ingress policy per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number
*    enable          - GT_TRUE - Ingress Policy enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressPolicyEnable
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnableGet
*
* DESCRIPTION:
*    Get the Enable/Disable ingress policy status per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number
*
* OUTPUTS:
*    enablePtr       - Pointer to ingress policy status.
*                      GT_TRUE - Ingress Policy is enabled.
*                      GT_FALSE - Ingress Policy is disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressPolicyEnableGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChPclPortLookupCfgTabAccessModeSet
*
* DESCRIPTION:
*    Configures VLAN/PORT/ePort access mode to Ingress or Egress PCL
*    configuration table per lookup.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number.
*                      In eArch devices
*                      - for each ingress lookup portNum is default ePort.
*                      - for egress lookup portNum is physical port.
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  xCat and above devices.
*                      Ignored for other devices.
*                      xCat and above devices supports two sub lookups only for
*                      ingress lookup CPSS_PCL_LOOKUP_0_E , (APPLICABLE RANGES: 0..1).
*                      All other lookups have not sub lookups,
*                      acceptable value is 0.
*    mode            - PCL Configuration Table access mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - on bad entryIndex
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - since dxCh1 supports Ingress direction only, this API set the same access
*     type for both lookups regardless <lookupNum> parameter value for dxCh1.
*******************************************************************************/
GT_STATUS cpssDxChPclPortLookupCfgTabAccessModeSet
(
    IN GT_U8                                          devNum,
    IN GT_PORT_NUM                                    portNum,
    IN CPSS_PCL_DIRECTION_ENT                         direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN GT_U32                                         subLookupNum,
    IN CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChPclPortLookupCfgTabAccessModeGet
*
* DESCRIPTION:
*    Gets VLAN/PORT/ePort access mode to Ingress or Egress PCL
*    configuration table per lookup.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portNum         - port number
*                      In eArch devices
*                      - for each ingress lookup portNum is default ePort.
*                      - for egress lookup portNum is physical port.
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  xCat and above devices.
*                      Ignored for other devices.
*                      xCat and above devices supports two sub lookups only for
*                      ingress lookup CPSS_PCL_LOOKUP_0_E , (APPLICABLE RANGES: 0..1).
*                      All other lookups have not sub lookups,
*                      acceptable value is 0.
* OUTPUTS:
*    modePtr         - (pointer to)PCL Configuration Table access mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only and set same access type
*     for both lookups regardless the <lookupNum> parameter value
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortLookupCfgTabAccessModeGet
(
    IN  GT_U8                                          devNum,
    IN  GT_PORT_NUM                                    portNum,
    IN  CPSS_PCL_DIRECTION_ENT                         direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT                     lookupNum,
    IN  GT_U32                                         subLookupNum,
    OUT CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT *modePtr
);

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeSet
*
* DESCRIPTION:
*    Configures global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    accModePtr      - global configuration of access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - for DxCh2 and above - full support,
*   - for DxCh1 supports Ingress direction only with 32 ports per device and the
*     non-local-device-entries-segment-base == 0
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblAccessModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
);

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeGet
*
* DESCRIPTION:
*    Get global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    accModePtr      - global configuration of access mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    see cpssDxChPclCfgTblAccessModeSet
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblAccessModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC   *accModePtr
);

/*******************************************************************************
* cpssDxChPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblSet
(
    IN GT_U8                           devNum,
    IN CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
);

/*******************************************************************************
* cpssDxChPclCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
);

/*******************************************************************************
* cpssDxChPclIngressPolicyEnable
*
* DESCRIPTION:
*    Enables Ingress Policy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable ingress policy
*                      GT_TRUE  - enable,
*                      GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIngressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChPclIngressPolicyEnableGet
*
* DESCRIPTION:
*    Get status (Enable or Disable) of Ingress Policy.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum           - device number
*
* OUTPUTS:
*    enablePtr        - (pointer to) enable Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIngressPolicyEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPclTcamRuleSizeModeSet
*
* DESCRIPTION:
*    Set TCAM Rules size mode.
*    The rules TCAM may be configured to contain short rules (24 bytes), long
*    rules (48 bytes), or a mix of short rules and long rules.
*    The rule size may be set by global configuration or may be defined
*    dynamical according to PCL Configuration table settings.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum               - device number
*    mixedRuleSizeMode    - TCAM rule size mode
*                           GT_TRUE  - mixed Rule Size Mode, rule size defined
*                                      by PCL configuration table
*                           GT_FALSE - Rule size defined by ruleSize parameter
*    ruleSize             - Rule size for not mixed mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamRuleSizeModeSet
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     mixedRuleSizeMode,
    IN CPSS_PCL_RULE_SIZE_ENT      ruleSize
);

/*******************************************************************************
* cpssDxChPclTcamRuleSizeModeGet
*
* DESCRIPTION:
*    Get TCAM Rules size mode.
*    The rules TCAM may be configured to contain short rules (24 bytes), long
*    rules (48 bytes), or a mix of short rules and long rules.
*    The rule size may be set by global configuration or may be defined
*    dynamical according to PCL Configuration table settings.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* NOT APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum               - device number
*
* OUTPUTS:
*    mixedRuleSizeModePtr - TCAM rule size mode
*                           GT_TRUE  - mixed Rule Size Mode, rule size defined
*                                      by PCL configuration table
*                           GT_FALSE - Rule size defined by ruleSize parameter
*    ruleSizePtr          - Rule size for not mixed mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamRuleSizeModeGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *mixedRuleSizeModePtr,
    OUT CPSS_PCL_RULE_SIZE_ENT      *ruleSizePtr
);

/*******************************************************************************
* cpssDxChPclUdeEtherTypeSet
*
* DESCRIPTION:
*       This function sets UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index,
*                       (APPLICABLE RANGES: Lion, xCat2, Lion2 0..4; Bobcat2, Caelum, Bobcat3 0..6)
*       ethType       - Ethertype value (APPLICABLE RANGES:  0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - on out of range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUdeEtherTypeSet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    IN  GT_U32          ethType
);

/*******************************************************************************
* cpssDxChPclUdeEtherTypeGet
*
* DESCRIPTION:
*       This function gets the UDE Ethertype.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index,
*                       (APPLICABLE RANGES: Lion, xCat2, Lion2 0..4; Bobcat2, Caelum, Bobcat3 0..6)
*
* OUTPUTS:
*       ethTypePtr    - points to Ethertype value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUdeEtherTypeGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_U32          *ethTypePtr
);

/*******************************************************************************
* cpssDxChPclTwoLookupsCpuCodeResolution
*
* DESCRIPTION:
*    Resolve the result CPU Code if both lookups has action commands
*    are either both TRAP or both MIRROR To CPU
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum               - device number
*    lookupNum            - lookup number from which the CPU Code is selected
*                           when action commands are either both TRAP or
*                           both MIRROR To CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTwoLookupsCpuCodeResolution
(
    IN GT_U8                       devNum,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum
);

/*******************************************************************************
* cpssDxChPclInvalidUdbCmdSet
*
* DESCRIPTION:
*    Set the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    udbErrorCmd    - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclInvalidUdbCmdSet
(
    IN GT_U8                        devNum,
    IN CPSS_DXCH_UDB_ERROR_CMD_ENT  udbErrorCmd
);

/*******************************************************************************
* cpssDxChPclInvalidUdbCmdGet
*
* DESCRIPTION:
*    Get the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*
* OUTPUTS:
*    udbErrorCmdPtr - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclInvalidUdbCmdGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_DXCH_UDB_ERROR_CMD_ENT  *udbErrorCmdPtr
);

/*******************************************************************************
* cpssDxChPclIpLengthCheckModeSet
*
* DESCRIPTION:
*    Set the mode of checking IP packet length.
*    To determine if an IP packet is a valid IP packet, one of the checks is a
*    length check to find out if the total IP length field reported in the
*    IP header is less or equal to the packet's length.
*    This function determines the check mode.
*    The results of this check sets the policy key <IP Header OK>.
*    The result is also used by the router engine to determine whether
*    to forward or trap/drop the packet.
*    There are two check modes:
*     1. For IPv4: ip_total_length <= packet's byte count
*        For IPv6: ip_total_length + 40 <= packet's byte count
*     2. For IPv4: ip_total_length + L3 Offset + 4 (CRC) <= packet's byte count,
*        For IPv6: ip_total_length + 40 +L3 Offset + 4 (CRC) <= packet's
*        byte count
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum           - device number
*    mode             - IP packet length checking mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpLengthCheckModeSet
(
    IN GT_U8                                   devNum,
    IN CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChPclIpLengthCheckModeGet
*
* DESCRIPTION:
*    Get the mode of checking IP packet length.
*    To determine if an IP packet is a valid IP packet, one of the checks is a
*    length check to find out if the total IP length field reported in the
*    IP header is less or equal to the packet's length.
*    This function determines the check mode.
*    The results of this check sets the policy key <IP Header OK>.
*    The result is also used by the router engine to determine whether
*    to forward or trap/drop the packet.
*    There are two check modes:
*     1. For IPv4: ip_total_length <= packet's byte count
*        For IPv6: ip_total_length + 40 <= packet's byte count
*     2. For IPv4: ip_total_length + L3 Offset + 4 (CRC) <= packet's byte count,
*        For IPv6: ip_total_length + 40 +L3 Offset + 4 (CRC) <= packet's
*        byte count
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*    devNum           - device number
*
* OUTPUTS:
*    modePtr          - IP packet length checking mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpLengthCheckModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxCh2PclEgressPolicyEnable
*
* DESCRIPTION:
*    Enables Egress Policy.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum           - device number
*    enable           - enable Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported enable parameter value
*       GT_FAIL                  - otherwise
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclEgressPolicyEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxCh2PclEgressPolicyEnableGet
*
* DESCRIPTION:
*    Get status of Egress Policy.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum           - device number
*
* OUTPUTS:
*    enablePtr        - status of Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL poinet
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclEgressPolicyEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableSet
*
* DESCRIPTION:
*       Enable/Disable the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
*       enable         - enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableGet
*
* DESCRIPTION:
*       Gets Enable/Disable of the Egress PCL processing for the packets,
*       which Tunnel Start already done on remote ingress device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressForRemoteTunnelStartEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
);

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
*
* DESCRIPTION:
*       Sets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       cfgTabAccMode  - PCL Configuration Table access mode
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - VLAN ID
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - Normal
*                        Normal means that access to PCL configuration table is done
*                        according to settings done by cpssDxChPclCfgTblAccessModeSet
*                        and cpssDxChPclPortLookupCfgTabAccessModeSet
*                        as non tunnel start packets.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
(
    IN  GT_U8                                            devNum,
    IN  CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   cfgTabAccMode
);

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
*
* DESCRIPTION:
*       Gets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       cfgTabAccModePtr   - (pointer to) PCL Configuration Table access mode
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_VLAN_E - VLAN ID
*                        CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_BY_PORT_E - Normal
*                        Normal means that access to PCL configuration table is done
*                        according to settings done by cpssDxChPclCfgTblAccessModeSet
*                        and cpssDxChPclPortLookupCfgTabAccessModeSet
*                        as non tunnel start packets.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
(
    IN  GT_U8                                            devNum,
    OUT CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT   *cfgTabAccModePtr
);

/*******************************************************************************
* cpssDxChPclEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
);

/*******************************************************************************
* cpssDxChPclEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
);

/*******************************************************************************
* cpssDxCh2EgressPclPacketTypesSet
*
* DESCRIPTION:
*   Enables/disables Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*   enable        - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*   - after reset EPCL disabled for all packet types on all ports
*
*
*******************************************************************************/
GT_STATUS cpssDxCh2EgressPclPacketTypesSet
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              port,
    IN CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    IN GT_BOOL                           enable
);

/*******************************************************************************
* cpssDxCh2EgressPclPacketTypesGet
*
* DESCRIPTION:
*   Get status of Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*
* OUTPUTS:
*   enablePtr     - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxCh2EgressPclPacketTypesGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              port,
    IN  CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT pktType,
    OUT GT_BOOL                           *enablePtr
);

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableSet
*
* DESCRIPTION:
*   The function enables/disables forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       enable         - force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable of the forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - according to ingress port
*                                   per lookup settings
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh3PclTunnelTermForceVlanModeEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                       *enablePtr
);

/*******************************************************************************
* cpssDxCh2PclTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*    l4PortType        - TCP/UDP port type, source or destination port
*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
);

/*******************************************************************************
* cpssDxCh2PclTcpUdpPortComparatorGet
*
* DESCRIPTION:
*        Get TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*    l4PortTypePtr      - TCP/UDP port type, source or destination port
*    compareOperatorPtr - compare operator FALSE, LTE, GTE, NEQ
*    valuePtr           - 16 bit value to compare with
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxCh2PclTcpUdpPortComparatorGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_PCL_DIRECTION_ENT            direction,
    IN  CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN  GT_U8                             entryIndex,
    OUT CPSS_L4_PROTOCOL_PORT_TYPE_ENT    *l4PortTypePtr,
    OUT CPSS_COMPARE_OPERATOR_ENT         *compareOperatorPtr,
    OUT GT_U16                            *valuePtr
);

/*******************************************************************************
* cpssDxChPclRuleStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*       Old function for DxCh1 and DxCh2 devices and for
*       standard rules on DxCh3 devices.
*       The general function is cpssDxChPclRuleAnyStateGet
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
);

/*******************************************************************************
* cpssDxChPclRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - size of rule
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
);

/*******************************************************************************
* cpssDxChPclRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 21 words.
*       patternArr       - rule pattern       - 21 words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclRuleGet
(
    IN  GT_U8                  devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
);

/*******************************************************************************
* cpssDxChPclCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
GT_STATUS cpssDxChPclCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
);

/*******************************************************************************
* cpssDxChPclLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* cpssDxChPclLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*
*                          For DxCh and DxCh2 devices:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*                          For DxCh3 device:
*                          The TCAM has 3.5K (3584) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 14335.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 7167.
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n < 3584
*                            is placed in the space of two standard rules with
*                            indexes n and n + 3584.
*                          - Extended rule with index  3584 <= n < 7168
*                            is placed in the space of two standard rules with
*                            indexes n + 3584, n + 7168.
*
*                          For xCat and above devices:
*                          See datasheet of particular device for TCAM size.
*                          For example describe the TCAM that has 1/4K (256) rows.
*                          Each row can be used as:
*                              - 4 standard rules
*                              - 2 extended rules
*                              - 1 extended and 2 standard rules
*                              - 2 standard and 1 extended rules
*                          The TCAM partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1K (1024).
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 0.5K (512).
*                            Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index 2n (even index)
*                            is placed in the space of two standard rules with
*                            indexes 4n and 4n + 1.
*                          - Extended rule with index 2n+1 (odd index)
*                            is placed in the space of two standard rules with
*                            indexes 4n+2 and 4n + 3.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                               Write rule to TCAM not using port-list format.
*                               Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The pattern and mask input structures should be set to zero
*      by an application before assignment of fields.
*      The function places all muxed fields to rule patern and mask
*      by using bitwise OR operation.
*      The function does not check validity of muxed (overlapped) fields.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT      ruleOptionsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleParsedGet
*
* DESCRIPTION:
*   The function gets the Policy Rule valid state, Mask, Pattern and Action
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
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
*                            - read only from first active port group of the bitmap.
*       ruleFormat       - format of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*
* OUTPUTS:
*       isRuleValidPtr   - GT_TRUE - rule is valid, GT_FALSE - rule is invalid
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - when size is trash or not related to ruleFormat.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleParsedGet
(
    IN GT_U8                                            devNum,
    IN GT_PORT_GROUPS_BMP                               portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT               ruleFormat,
    IN GT_U32                                           ruleIndex,
    IN CPSS_DXCH_PCL_RULE_OPTION_ENT                    ruleOptionsBmp,
    OUT GT_BOOL                                         *isRuleValidPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                   *maskPtr,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_UNT                   *patternPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC                        *actionPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize        - size of Rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule.
*   For DxCh and DxCh2 devices start indirect rule write operation
*   with "garbage" pattern and mask content and "valid-flag" == 0
*   For DxCh3 devices calculates the TCAM position by ruleSize and ruleIndex
*   parameters and updates the first "valid" bit matching X/Y pair to (1,1)
*   i.e. don't match any value. If origin of valid or invalid rule found at the
*   specified TCAM position it will be invalid and available to back validation.
*   If the specified TCAM position contains the not first 24 byte segment of
*   extended rule, the rule also will be invalidated,
*   but unavailable to back validation.
*   If the garbage found in TCAM the X/Y bits will be updated to be as
*   in invalid rule.
*   The function does not check the TCAM contents.
*   GT_OK will be returned in each of described cases.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM. See cpssDxChPclPortGroupRuleSet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Backward compatible styled API.
*       The cpssDxChPclRuleValidStatusSet recommended for using instead.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleInvalidate
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize        - size of Rule.(APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - if in TCAM found rule of size different
*                        from the specified
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize       - size of rule. (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleAnyStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclPortGroupRuleCopy should be
*  called. And after this cpssDxChPclPortGroupRuleInvalidate or
*  cpssDxChPclPortGroupRuleValidStatusSet should
*  be used to invalidate Rule in old position.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize         - size of Rule.
*       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
*                           mask and action are taken.
*       ruleDstIndex     - index of the rule in the TCAM to which pattern,
*                           mask and action are placed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleCopy
(
    IN GT_U8                              devNum,
    IN GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
);

/*******************************************************************************
* cpssDxChPclPortGroupRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       maskArr          - rule mask          - 21  words.
*       patternArr       - rule pattern       - 21  words.
*       actionArr        - Policy rule action - 4 words.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupRuleGet
(
    IN  GT_U8                  devNum,
    IN  GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
);

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for setting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblSet
(
    IN GT_U8                           devNum,
    IN GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN CPSS_INTERFACE_INFO_STC         *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       interfaceInfoPtr  - interface data: port, VLAN, or index for getting a
*                           specific PCL Configuration Table entry
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           For Ingress PCL: lookup0 or lookup1
*                           For Egress PCL ignored
*
* OUTPUTS:
*       lookupCfgPtr      - (pointer to) lookup configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*       For Bobcat2, Caelum, Bobcat3 that use INDEX interface type
*           to configure entry for ePort, index = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum          - device number
*    portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    lookupNum       - Lookup number: lookup0 or lookup1
*                      DxCh1-DxCh3 devices ignores the parameter
*                      xCat and above supports the parameter
*                      xCat and above devices has separate HW entries
*                      for both ingress lookups.
*                      Only lookup 0 is supported for egress.
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_STATE             - in case of already bound pclHwId with same
*                            combination of slot/direction
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       DxCh devices support ingress direction only
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*    l4PortType        - TCP/UDP port type, source or destination port

*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
);

/*******************************************************************************
* cpssDxChPclPortGroupTcpUdpPortComparatorGet
*
* DESCRIPTION:
*        Get TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*    devNum            - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*    l4PortTypePtr      - TCP/UDP port type, source or destination port
*    compareOperatorPtr - compare operator FALSE, LTE, GTE, NEQ
*    valuePtr           - 16 bit value to compare with
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupTcpUdpPortComparatorGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT            direction,
    IN  CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN  GT_U8                             entryIndex,
    OUT CPSS_L4_PROTOCOL_PORT_TYPE_ENT    *l4PortTypePtr,
    OUT CPSS_COMPARE_OPERATOR_ENT         *compareOperatorPtr,
    OUT GT_U16                            *valuePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN CPSS_PCL_DIRECTION_ENT               direction,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
);

/*******************************************************************************
* cpssDxChPclPortGroupUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for xCat and above devices.
*                      For Bobcat2; Caelum; Bobcat3 relevant for ingress only.
*       direction    - Policy direction:
*                      Ingress or Egress.(APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      Ignored, configure ingress.
*                      (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2)
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_DIRECTION_ENT               direction,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
);

/*******************************************************************************
* cpssDxChPclUserDefinedBytesSelectSet
*
* DESCRIPTION:
*   Set the User Defined Byte (UDB) Selection Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Valid only UDB only key formats.
*                      The parameter used also to determinate Ingress or Egress.
*                      Relevant values are:
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
*       packetType   - packet Type
*       lookupNum    - Lookup number. Ignored for egress.
*       udbSelectPtr - (pointer to) structure with UDB Selection configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on out-of-range parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedBytesSelectSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT           lookupNum,
    IN  CPSS_DXCH_PCL_UDB_SELECT_STC         *udbSelectPtr
);

/*******************************************************************************
* cpssDxChPclUserDefinedBytesSelectGet
*
* DESCRIPTION:
*   Get the User Defined Byte (UDB) Selection Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Valid only UDB only key formats.
*                      The parameter used also to determinate Ingress or Egress.
*                      Relevant values are:
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E
*                      CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E
*       packetType   - packet Type
*       lookupNum    - Lookup number. Ignored for egress.
*
* OUTPUTS:
*       udbSelectPtr - (pointer to) structure with UDB Selection configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on unsupported HW state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclUserDefinedBytesSelectGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT           lookupNum,
    OUT CPSS_DXCH_PCL_UDB_SELECT_STC         *udbSelectPtr
);

/*******************************************************************************
* cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  GT_PORT_GROUPS_BMP                         portGroupsBmp,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
);

/*******************************************************************************
* cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       portGroupsBmp      - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    IN   GT_PORT_GROUPS_BMP                         portGroupsBmp,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
);

/*******************************************************************************
* cpssDxChPclPortGroupLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
);

/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableSet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_BOOL                            enable
);

/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableGet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclL3L4ParsingOverMplsEnableGet
(
    IN  GT_U8                              devNum,
    OUT GT_BOOL                            *enablePtr
);

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet
*
* DESCRIPTION:
*        The function enables or disables overriding of the
*        UDB content for specific UDB in specific key format
*        where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*    enable             - GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB configuration is used.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesEnableSet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    IN  GT_BOOL                              enable
);

/*******************************************************************************
* cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet
*
* DESCRIPTION:
*        The function get status (enabled or disabled) of the
*        overriding of the UDB content for specific UDB
*        in specific key format where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    portGroupsBmp     - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*
* OUTPUTS:
*    enablePtr          - (pointer to)
*                         GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB content may be overriden
*                         by content of another udbOverrideType.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortGroupOverrideUserDefinedBytesEnableGet
(
    IN  GT_U8                                devNum,
    IN  GT_PORT_GROUPS_BMP                   portGroupsBmp,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    OUT GT_BOOL                              *enablePtr
);

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesEnableSet
*
* DESCRIPTION:
*        The function enables or disables overriding of the
*        UDB content for specific UDB in specific key format
*        where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*    enable             - GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB configuration is used.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesEnableSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    IN  GT_BOOL                              enable
);

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesEnableGet
*
* DESCRIPTION:
*        The function get status (enabled or disabled) of the
*        overriding of the UDB content for specific UDB
*        in specific key format where it supported by HW.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum            - device number
*    ruleFormat         - rule format
*    udbOverrideType    - The type of the UDB configuration overriding.
*
* OUTPUTS:
*    enablePtr          - (pointer to)
*                         GT_TRUE - enable overriding of UDB content
*                         by udbOverrideType.
*                         GT_FALSE - disable overriding of UDB content
*                         by udbOverrideType, UDB content may be overriden
*                         by content of another udbOverrideType.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclOverrideUserDefinedBytesEnableGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_UDB_OVERRIDE_TYPE_ENT  udbOverrideType,
    OUT GT_BOOL                              *enablePtr
);

/*******************************************************************************
* cpssDxChPclLookup0ForRoutedPacketsEnableSet
*
* DESCRIPTION:
*       Enables/disables PCL lookup0 for routed packets.
*       For not routed packets lookup0 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookup0ForRoutedPacketsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_NUM                        portNum,
    IN  GT_BOOL                            enable
);


/*******************************************************************************
* cpssDxChPclLookup0ForRoutedPacketsEnableGet
*
* DESCRIPTION:
*       Gets Enable/disable status of PCL lookup0 for routed packets.
*       For not routed packets lookup0 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookup0ForRoutedPacketsEnableGet
(
    IN  GT_U8                              devNum,
    IN  GT_PORT_NUM                        portNum,
    OUT GT_BOOL                            *enablePtr
);

/*******************************************************************************
* cpssDxChPclLookup1ForNotRoutedPacketsEnableSet
*
* DESCRIPTION:
*       Enables/disables PCL lookup1 for not routed packets.
*       For routed packets lookup1 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookup1ForNotRoutedPacketsEnableSet
(
    IN  GT_U8                              devNum,
    IN  GT_BOOL                            enable
);

/*******************************************************************************
* cpssDxChPclLookup1ForNotRoutedPacketsEnableGet
*
* DESCRIPTION:
*       Gets Enable/disable status of PCL lookup1 for not routed packets.
*       For routed packets lookup1 always enable.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclLookup1ForNotRoutedPacketsEnableGet
(
    IN  GT_U8                              devNum,
    OUT GT_BOOL                            *enablePtr
);

/*******************************************************************************
* cpssDxChPclTcamEccDaemonEnableSet
*
* DESCRIPTION:
*       Enables or disables PCL TCAM ECC Daemon.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*       enalbe        - GT_TRUE  - PCL TCAM ECC Daemon enable
*                       GT_FALSE - PCL TCAM ECC Daemon disable
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamEccDaemonEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enalbe
);

/*******************************************************************************
* cpssDxChPclTcamEccDaemonEnableGet
*
* DESCRIPTION:
*       Gets status (Enabled/Disabled) of PCL TCAM ECC Daemon.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - (pointer to) PCL TCAM ECC Daemon status:
*                       GT_TRUE  - PCL TCAM ECC Daemon enable
*                       GT_FALSE - PCL TCAM ECC Daemon disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclTcamEccDaemonEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPclIpMinOffsetSet
*
* DESCRIPTION:
*       Set the IP Minimum Offset parameter.
*       If packet is IPV4 fragment and has non zero offset
*       and it's offset less then IP Minimum Offset
*       the packet is counted as small offset.
*       The ipHeaderInfo field in PCL search key is set to be 3 i.e. "Ip Small Offset".
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
*       ipMinOffset - IP Minimum Offset  parameter (APPLICABLE RANGES: 0..0x1FFF)
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range minIpOffset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpMinOffsetSet
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             ipMinOffset
);

/*******************************************************************************
* cpssDxChPclIpMinOffsetGet
*
* DESCRIPTION:
*       Get the IP Minimum Offset parameter.
*       If packet is IPV4 fragment and has non zero offset
*       and it's offset less then IP Minimum Offset
*       the packet is counted as small offset.
*       The ipHeaderInfo field in PCL search key is set to be 3 i.e. "Ip Small Offset".
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - device number
* OUTPUTS:
*       ipMinOffsetPtr - (pointer to)IP Minimum Offset  parameter
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range minIpOffset
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpMinOffsetGet
(
    IN  GT_U8                              devNum,
    OUT GT_U32                             *ipMinOffsetPtr
);

/*******************************************************************************
* cpssDxChPclPortIngressLookup0Sublookup1TypeSet
*
* DESCRIPTION:
*    Configures Ingress PCL Lookup0 sublookup1 type.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum          - device number
*    lookupType      - partial or full IPCL lookup0 sublookup1 type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressLookup0Sublookup1TypeSet
(
    IN GT_U8                                      devNum,
    IN CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT lookupType
);

/*******************************************************************************
* cpssDxChPclPortIngressLookup0Sublookup1TypeGet
*
* DESCRIPTION:
*    Configures Ingress PCL Lookup0 sublookup1 type.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    lookupTypePtr   - (pointer to)partial or full IPCL lookup0 sublookup1 type
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortIngressLookup0Sublookup1TypeGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_IPCL01_LOOKUP_TYPE_ENT *lookupTypePtr
);

/*******************************************************************************
* cpssDxChPclPortListGroupingEnableSet
*
* DESCRIPTION:
*   Enable/disable port grouping mode per direction. When enabled, the 4 MSB
*   bits of the <Port List> field in the PCL keys are replaced by a 4-bit
*   <Port Group>.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*       enable      - enable/disable port grouping mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortListGroupingEnableSet
(
    IN GT_U8                    devNum,
    IN CPSS_PCL_DIRECTION_ENT   direction,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChPclPortListGroupingEnableGet
*
* DESCRIPTION:
*   Get the port grouping mode enabling status per direction. When enabled,
*   the 4 MSB bits of the <Port List> field in the PCL keys are replaced by a
*   4-bit <Port Group>.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*
* OUTPUTS:
*       enablePtr   - (pointer to) the port grouping mode enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortListGroupingEnableGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PCL_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChPclPortListPortMappingSet
*
* DESCRIPTION:
*   Set port group mapping and offset in port list for PCL working in Physical
*   Port List.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*       portNum     - port number
*       enable      - port participate in the port list vector.
*       group       - PCL port group
*                     Relevant only if <enable> == GT_TRUE.
*                     (APPLICABLE RANGES: 0..15)
*       offset      - offset in the port list bit vector.
*                     Relevant only if <enable> == GT_TRUE.
*                     (APPLICABLE RANGES: 0..27)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When working in PCL Port List mode with port grouping enabled, <offset>
*       value in the range 23..27 has no influence.
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortListPortMappingSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PCL_DIRECTION_ENT  direction,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable,
    IN  GT_U32                  group,
    IN  GT_U32                  offset
);

/*******************************************************************************
* cpssDxChPclPortListPortMappingGet
*
* DESCRIPTION:
*   Get port group mapping and offset in port list for PCL working in Physical
*   Port List.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       direction   - Policy direction
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) port participate in the port list vector.
*       groupPtr    - (pointer to) PCL port group
*       offsetPtr   - (pointer to) offset in the port list bit vector.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclPortListPortMappingGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_PCL_DIRECTION_ENT  direction,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr,
    OUT GT_U32                  *groupPtr,
    OUT GT_U32                  *offsetPtr
);

/*******************************************************************************
* cpssDxChPclOamChannelTypeProfileToOpcodeMappingSet
*
* DESCRIPTION:
*   Set Mapping of MPLS Channel Type Profile to OAM opcode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       channelTypeProfile     - MPLS G-ACh Channel Type Profile
*                                (APPLICABLE RANGES: 1..15)
*       opcode                 - OAM opcode (APPLICABLE RANGES: 0..255)
*       RBitAssignmentEnable   - replace Bit0 of opcode by packet R-flag
*                                GT_TRUE  - Bit0 of result opcode is packet R-flag
*                                GT_FALSE - Bit0 of result opcode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamChannelTypeProfileToOpcodeMappingSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    channelTypeProfile,
    IN  GT_U32                    opcode,
    IN  GT_BOOL                   RBitAssignmentEnable
);

/*******************************************************************************
* cpssDxChPclOamChannelTypeProfileToOpcodeMappingGet
*
* DESCRIPTION:
*   Get Mapping of MPLS Channel Type Profile to OAM opcode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       channelTypeProfile     - MPLS G-ACh Channel Type Profile
*                                (APPLICABLE RANGES: 1..15)
*
* OUTPUTS:
*       opcodePtr               - (pointer to)OAM opcode
*       RBitAssignmentEnablePtr - (pointer to)replace Bit0 of opcode by packet R-flag
*                                GT_TRUE  - Bit0 of result opcode is packet R-flag
*                                GT_FALSE - Bit0 of result opcode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamChannelTypeProfileToOpcodeMappingGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    channelTypeProfile,
    OUT GT_U32                    *opcodePtr,
    OUT GT_BOOL                   *RBitAssignmentEnablePtr
);

/*******************************************************************************
* cpssDxChPclOamMegLevelModeSet
*
* DESCRIPTION:
*   Set OAM MEG Level Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*       megLevelMode           - OAM MEG Level Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamMegLevelModeSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_PCL_DIRECTION_ENT                    direction,
    IN  GT_U32                                    oamProfile,
    IN  CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT  megLevelMode
);

/*******************************************************************************
* cpssDxChPclOamMegLevelModeGet
*
* DESCRIPTION:
*   Get OAM MEG Level Mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       megLevelModePtr        - (pointer to) OAM MEG Level Mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_STATE             - on unexpected value in HW
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamMegLevelModeGet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_PCL_DIRECTION_ENT                    direction,
    IN  GT_U32                                    oamProfile,
    OUT CPSS_DXCH_PCL_OAM_PKT_MEG_LEVEL_MODE_ENT  *megLevelModePtr
);

/*******************************************************************************
* cpssDxChPclOamRdiMatchingSet
*
* DESCRIPTION:
*   Set OAM RDI(Remote Direct Indicator) Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*       rdiMask                - Mask for RDI bit.(APPLICABLE RANGES: 0..255)
*       rdiPattern             - Pattern for RDI bit.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB4 (profile0) or UDB5 (profile1)
*                                Result = ((packetByte & rdiMask) == rdiPattern) ? 0 : 1.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamRdiMatchingSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  GT_U32                        oamProfile,
    IN  GT_U32                        rdiMask,
    IN  GT_U32                        rdiPattern
);

/*******************************************************************************
* cpssDxChPclOamRdiMatchingGet
*
* DESCRIPTION:
*   Get OAM RDI(Remote Direct Indicator) Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       direction              - Policy direction.
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       rdiMaskPtr             - (pointer to)Mask for RDI bit.(APPLICABLE RANGES: 0..255)
*       rdiPatternPtr          - (pointer to)Pattern for RDI bit.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB4 (profile0) or UDB5 (profile1)
*                                Result = ((packetByte & rdiMask) == rdiPattern) ? 0 : 1.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamRdiMatchingGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  GT_U32                        oamProfile,
    OUT GT_U32                        *rdiMaskPtr,
    OUT GT_U32                        *rdiPatternPtr
);

/*******************************************************************************
* cpssDxChPclOamEgressPacketDetectionSet
*
* DESCRIPTION:
*   Set OAM Egress Packet Detection parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       mode                   - Egress Packet Detection mode.
*       sourceId               - packet Source Id. When mode ==
*                                CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E
*                                and packet source ID equal to sourceId value
*                                the packet treated as OAM packet
*                                regardless to PCL action results.
*                                (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamEgressPacketDetectionSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT  mode,
    IN  GT_U32                                    sourceId
);

/*******************************************************************************
* cpssDxChPclOamEgressPacketDetectionGet
*
* DESCRIPTION:
*   Get OAM Egress Packet Detection parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       modePtr                - (pointer to)Egress Packet Detection mode.
*       sourceIdPtr            - (pointer to)packet Source Id. When mode ==
*                                CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_SOURCE_ID_E
*                                and packet source ID equal to sourceId value
*                                the packet treated as OAM packet
*                                regardless to PCL action results.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamEgressPacketDetectionGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_OAM_EGRESS_DETECT_MODE_ENT  *modePtr,
    OUT GT_U32                                    *sourceIdPtr
);

/*******************************************************************************
* cpssDxChPclOamRFlagMatchingSet
*
* DESCRIPTION:
*   Set OAM R-Flag Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*       rFlagMask              - Mask for R-Flag.(APPLICABLE RANGES: 0..255)
*       rFlagPattern           - Pattern for R-Flag.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB8 (profile0) or UDB9 (profile1)
*                                Result = ((packetByte & rFlagMask) == rFlagPattern) ? 0 : 1.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_OUT_OF_RANGE          - for out-of-range parameter values.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamRFlagMatchingSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        oamProfile,
    IN  GT_U32                        rFlagMask,
    IN  GT_U32                        rFlagPattern
);

/*******************************************************************************
* cpssDxChPclOamRFlagMatchingGet
*
* DESCRIPTION:
*   Get OAM R-Flag Matching parameters.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number
*       oamProfile             - OAM profile (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       rFlagMaskPtr           - (pointer to)Mask for R-Flag.(APPLICABLE RANGES: 0..255)
*       rFlagPatternPtr        - (pointer to)Pattern for R-Flag.(APPLICABLE RANGES: 0..255)
*                                Extracted packet byte pointed by UDB8 (profile0) or UDB9 (profile1)
*                                Result = ((packetByte & rFlagMask) == rFlagPattern) ? 0 : 1.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPclOamRFlagMatchingGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        oamProfile,
    OUT GT_U32                        *rFlagMaskPtr,
    OUT GT_U32                        *rFlagPatternPtr
);

/*******************************************************************************
* cpssDxChPclEgressSourcePortSelectionModeSet
*
* DESCRIPTION:
*   Set EPCL source port field selection mode for DSA tagged packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portSelectionMode   - Source port selection mode for DSA packets
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       EPCL key <SrcPort> always reflects the local device source physical
*       ingress port, even for DSA-tagged packets. To maintain backward
*       compatibility for DSA-tagged packets, a global mode allows working in
*       the backward compatible mode where <SrcPort> is taken from the DSA tag
*       if packet was received DSA tagged.
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressSourcePortSelectionModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT portSelectionMode
);

/*******************************************************************************
* cpssDxChPclEgressSourcePortSelectionModeGet
*
* DESCRIPTION:
*   Get EPCL source port field selection mode for DSA tagged packets.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       portSelectionModePtr    - (pointer to) Source port selection mode for
*                                 DSA packets
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       EPCL key <SrcPort> always reflects the local device source physical
*       ingress port, even for DSA-tagged packets. To maintain backward
*       compatibility for DSA-tagged packets, a global mode allows working in
*       the backward compatible mode where <SrcPort> is taken from the DSA tag
*       if packet was received DSA tagged.
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressSourcePortSelectionModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_PCL_EGRESS_SOURCE_PORT_SELECTION_MODE_ENT *portSelectionModePtr
);

/*******************************************************************************
* cpssDxChPclEgressTargetPortSelectionModeSet
*
* DESCRIPTION:
*   Set EPCL target port field selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portSelectionMode   - Target port selection mode
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTargetPortSelectionModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT portSelectionMode
);

/*******************************************************************************
* cpssDxChPclEgressTargetPortSelectionModeGet
*
* DESCRIPTION:
*   Get EPCL target port field selection mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       portSelectionModePtr    - (pointer to) Target port selection mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressTargetPortSelectionModeGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_DXCH_PCL_EGRESS_TARGET_PORT_SELECTION_MODE_ENT
                                                        *portSelectionModePtr
);

/*******************************************************************************
* cpssDxChPclSourceIdMaskSet
*
* DESCRIPTION:
*   Set mask so only certain bits in the source ID will be modified due to PCL
*   action.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum         - device number
*    lookupNum      - lookup number
*    mask           - Source ID mask (APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       none.

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclSourceIdMaskSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum,
    IN  GT_U32                      mask
);

/*******************************************************************************
* cpssDxChPclSourceIdMaskGet
*
* DESCRIPTION:
*   Get mask used for source ID modify due to PCL action.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum         - device number
*    lookupNum      - lookup number
*
* OUTPUTS:
*    maskPtr        - (pointer to) Source ID mask (APPLICABLE RANGES: 0..4095)

* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPclSourceIdMaskGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum,
    OUT GT_U32                      *maskPtr
);

/*******************************************************************************
* cpssDxChPclIpPayloadMinSizeSet
*
* DESCRIPTION:
*       Set the IP payload minimum size. This value will determine the Layer 4
*       validity
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       ipPayloadMinSize    - the IP payload minimum size
*                             (APPLICABLE RANGES: 0..16383)
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
*       The L4_Valid in the packet descriptor will be cleared if:
*       For IPv4: total length - header length > IP payload minimum size
*       For IPv6: payload length > IP payload minimum size
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpPayloadMinSizeSet
(
    IN GT_U8        devNum,
    IN GT_U32       ipPayloadMinSize
);

/*******************************************************************************
* cpssDxChPclIpPayloadMinSizeGet
*
* DESCRIPTION:
*       Get the IP payload minimum size. This value will determine the Layer 4
*       validity
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       ipPayloadMinSizePtr - the IP payload minimum size
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The L4_Valid in the packet descriptor will be cleared if:
*       For IPv4: total length - header length > IP payload minimum size
*       For IPv6: payload length > IP payload minimum size
*
*******************************************************************************/
GT_STATUS cpssDxChPclIpPayloadMinSizeGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *ipPayloadMinSizePtr
);


/*******************************************************************************
* cpssDxChPclEgressRxAnalyzerUseOrigVidEnableSet
*
* DESCRIPTION:
*    Function enables/disables for ingress analyzed packet the using of VID from
*    the incoming packet to access the EPCL Configuration table and for lookup
*    keys generation.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum          - device number
*    enable          - GT_TRUE  - use original VID
*                      GT_FALSE - use VLAN assigned by the processing pipe
*
* OUTPUTS:
*    none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    None
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressRxAnalyzerUseOrigVidEnableSet
(
    IN  GT_U8        devNum,
    IN  GT_BOOL      enable
);


/*******************************************************************************
* cpssDxChPclEgressRxAnalyzerUseOrigVidEnableGet
*
* DESCRIPTION:
*    Get state of the flag in charge of the using of VID of the incoming packet
*    for ingress analyzed packet to access the EPCL Configuration table and for
*    lookup keys generation.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    enablePtr       - (pointer to)enable/disable the using of original VID.
*                      GT_TRUE  - use original VID
*                      GT_FALSE - use VLAN assigned by the processing pipe
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*    None
*
*******************************************************************************/
GT_STATUS cpssDxChPclEgressRxAnalyzerUseOrigVidEnableGet
(
    IN  GT_U8         devNum,
    OUT GT_BOOL       *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPclh */


