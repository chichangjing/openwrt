/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCnc.h
*
* DESCRIPTION:
*       CPSS DxCh Centralized Counters (CNC) API.
*
* FILE REVISION NUMBER:
*       $Revision: 33 $
*******************************************************************************/

#ifndef __cpssDxChCnch
#define __cpssDxChCnch

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* get public types */
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/generic/cpssTypes.h>

/*
 * typedef: CPSS_DXCH_CNC_CLIENT_ENT
 *
 * Description:
 *      CNC clients.
 * Enumerations:
 *   CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E       - L2/L3 Ingress VLAN
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E    - Ingress PCL0 lookup 0
 *             For Bobcat2, Caelum, Bobcat3 relates parallel sublookup0.
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E  - Ingress PCL0 lookup 1
 *             (APPLICABLE DEVICES: xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3)
 *             For Bobcat2, Caelum, Bobcat3 relates parallel sublookup0.
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E    - Ingress PCL1 lookup
 *             For Bobcat2, Caelum, Bobcat3 relates parallel sublookup0.
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E  - Ingress VLAN Pass/Drop
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E   - Egress VLAN Pass/Drop
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E  - Egress Queue Pass/Drop
 *             Note: For Lion and above devices
 *                   the CN messages may be counted instead
 *                   of Egress Queue Pass/Drop, if CN mode enabled.
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E              - Egress PCL
 *             For Bobcat2, Caelum, Bobcat3 relates parallel sublookup0.
 *   CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E        - ARP Table access
 *   CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E            - Tunnel Start
 *   CPSS_DXCH_CNC_CLIENT_TTI_E                     - TTI (TTI action index)
 *             For Bobcat2, Caelum, Bobcat3 relates parallel sublookup0.
 *
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_SRC_EPORT_E       - 'source ePort' client - count per source EPort
 *                                                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_TRG_EPORT_E        - 'taregt ePort' client - count per target EPort
 *                                                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *   CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_0_E                  -
 *                   TTI Parallel lookup0(TTI action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                    TTI Parallel lookup2(TTI action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum)
 *                   NOTE: Bobcat3 uses 'TTI_PARALLEL_2' for 'sublookup2'
 *   CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_1_E                  -
 *                    TTI Parallel lookup1(TTI action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *                    TTI Parallel lookup3(TTI action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum)
 *                   NOTE: Bobcat3 uses 'TTI_PARALLEL_3' for 'sublookup3'
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_0_E -
 *                   Ingress PCL Lookup0, Parallel sublookup0(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_1_E -
 *                   Ingress PCL Lookup0, Parallel sublookup1(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_2_E -
 *                   Ingress PCL Lookup0, Parallel sublookup2(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_3_E -
 *                   Ingress PCL Lookup0, Parallel sublookup3(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_0_E -
 *                   Ingress PCL Lookup1, Parallel sublookup0(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_1_E -
 *                   Ingress PCL Lookup1, Parallel sublookup1(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_2_E -
 *                   Ingress PCL Lookup1, Parallel sublookup2(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_3_E -
 *                   Ingress PCL Lookup1, Parallel sublookup3(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_0_E -
 *                   Ingress PCL Lookup2, Parallel sublookup0(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_1_E -
 *                   Ingress PCL Lookup2, Parallel sublookup1(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_2_E -
 *                   Ingress PCL Lookup2, Parallel sublookup2(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_3_E -
 *                   Ingress PCL Lookup2, Parallel sublookup3(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_0_E           -
 *                   Egress PCL Parallel sublookup0(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_1_E           -
 *                   Egress PCL Parallel sublookup1(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_2_E           -
 *                   Egress PCL Parallel sublookup2(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_3_E           -
 *                   Egress PCL Parallel sublookup3(PCL action counter index)
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_PACKET_TYPE_PASS_DROP_E           -
 *                   Preegress counting packets by packet command.
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E                    -
 *                   Traffic Manager counting packets by TM Queue Id command.
 *                   (APPLICABLE DEVICES: Bobcat2)
 *   CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_2_E
 *                   TTI Parallel lookup2(TTI action counter index)
 *                   (APPLICABLE DEVICES: Bobcat3)
 *   CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_3_E
 *                   TTI Parallel lookup2(TTI action counter index)
 *                   (APPLICABLE DEVICES: Bobcat3)
 *
 *   CPSS_DXCH_CNC_CLIENT_LAST_E                            - amount of enum members
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_1_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_E,
    CPSS_DXCH_CNC_CLIENT_ARP_TABLE_ACCESS_E,
    CPSS_DXCH_CNC_CLIENT_TUNNEL_START_E,
    CPSS_DXCH_CNC_CLIENT_TTI_E,

    CPSS_DXCH_CNC_CLIENT_INGRESS_SRC_EPORT_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_TRG_EPORT_E,
    CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_0_E,
    CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_1_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_0_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_1_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_2_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_0_PARALLEL_3_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_0_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_1_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_2_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_1_PARALLEL_3_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_0_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_1_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_2_E,
    CPSS_DXCH_CNC_CLIENT_INGRESS_PCL_LOOKUP_2_PARALLEL_3_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_0_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_1_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_2_E,
    CPSS_DXCH_CNC_CLIENT_EGRESS_PCL_PARALLEL_3_E,
    CPSS_DXCH_CNC_CLIENT_PACKET_TYPE_PASS_DROP_E,
    CPSS_DXCH_CNC_CLIENT_TM_PASS_DROP_E,

    CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_2_E,
    CPSS_DXCH_CNC_CLIENT_TTI_PARALLEL_3_E,

    CPSS_DXCH_CNC_CLIENT_LAST_E
} CPSS_DXCH_CNC_CLIENT_ENT;

/*
 * typedef: CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT
 *
 * Description:
 *      CNC modes of Egress Queue counting.
 *
 * Enumerations:
 *   CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E -
 *         count egress queue pass and tail-dropped packets.
 *   CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E        -
 *         count Congestion Notification messages.
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E,
    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E
} CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT;

/*
 * typedef: CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT
 *
 * Description:
 *      CNC modes for byte count counters.
 *
 * Enumerations:
 *   CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E - The Byte Count counter counts the
 *                 entire packet byte count for all packet type
 *   CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E - Byte Count counters counts the
 *                 packet L3 fields (the entire packet
 *                 minus the L3 offset) and only
 *                 the passenger part for tunnel-terminated
 *                 packets or tunnel-start packets.
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E,
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E
} CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT;

/*
 * typedef: CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT
 *
 * Description:
 *      CNC modes Egress VLAN Drop counting.
 *
 * Enumerations:
 *      CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E -
 *          Drop-counter counts egress-filtered and tail-dropped traffic
 *      CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E          -
 *          Drop-counter counts egress filtered traffic only
 *      CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E              -
 *          Drop-counter counts tail-drop only
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E,
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_ONLY_E,
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E
} CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT
 *
 * Description:
 *      CNC counting enabled unit
 *
 * Enumerations:
 *      CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E - enable counting for TTI
 *                                                 client.
 *      CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E - enable counting for extended
 *                                                 PCL unit (this unit includes
 *                                                 PCL0_0, PCL0_1, PCL1 & L2/L3
 *                                                 Ingress VLAN clients).
 *
 * Comment:
 *      Other clients are implicitly enabled for counting and binding to
 *      CNC block operation is needed to perform counting.
 */
typedef enum
{
    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E,
    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E
} CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT;


/*
 * typedef: CPSS_DXCH_CNC_COUNTER_FORMAT_ENT
 *
 * Description:
 *      CNC modes of counter formats.
 *
 * Enumerations:
 *   CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E - PKT_29_BC_35;
 *         Partitioning of the 64 bits entry is as following:
 *         Packets counter: 29 bits, Byte Count counter: 35 bits
 *   CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E - PKT_27_BC_37;
 *         Partitioning of the 64 bits entry is as following:
 *         Packets counter: 27 bits, Byte Count counter: 37 bits
 *   CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E - PKT_37_BC_27;
 *         Partitioning of the 64 bits entry is as following:
 *         Packets counter: 37 bits, Byte Count counter: 27 bits
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E,
    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E,
    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E
} CPSS_DXCH_CNC_COUNTER_FORMAT_ENT;

/*
 * typedef: CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT
 *
 * Description:
 *      Index calculation modes of Ingress VLAN Pass/Drop
 *      and Egress VLAN Pass/DropCNC Clients.
 *
 * Enumerations:
 *   CPSS_DXCH_CNC_VLAN_INDEX_MODE_ORIGINAL_VID_E - original packet VID
 *   CPSS_DXCH_CNC_VLAN_INDEX_MODE_EVID_E         - assigned eVid
 *   CPSS_DXCH_CNC_VLAN_INDEX_MODE_TAG1_VID_E     - tag1 VID
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_VLAN_INDEX_MODE_ORIGINAL_VID_E,
    CPSS_DXCH_CNC_VLAN_INDEX_MODE_EVID_E,
    CPSS_DXCH_CNC_VLAN_INDEX_MODE_TAG1_VID_E
} CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT;

/*
 * typedef: CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT
 *
 * Description:
 *      Index calculation modes of To CPU packets for Packet Type Pass/Drop CNC Client.
 *
 * Enumerations:
 *   CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_SRC_PORT_E - use local physical source port
 *   CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_CPU_CODE_E - use CPU Code
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_SRC_PORT_E,
    CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_CPU_CODE_E
} CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT;

/*
 * typedef: CPSS_DXCH_CNC_TM_INDEX_MODE_ENT
 *
 * Description:
 *      Index calculation modes for Packet Traffic Manager CNC Client.
 *
 * Enumerations:
 *   CPSS_DXCH_CNC_TM_INDEX_MODE_0_E - Mode0: index is {queue_id[13:0],pass/drop}
 *   CPSS_DXCH_CNC_TM_INDEX_MODE_1_E - Mode1: index is {queue_id[12:0],DP[0],pass/drop}
 *   CPSS_DXCH_CNC_TM_INDEX_MODE_2_E - Mode2: index is {queue_id[11:0],DP[1:0],pass/drop}
 *   CPSS_DXCH_CNC_TM_INDEX_MODE_3_E - Mode3: index is {queue_id[12:0],DP[1],pass/drop}
 *
 * Comment:
 */
typedef enum
{
    CPSS_DXCH_CNC_TM_INDEX_MODE_0_E,
    CPSS_DXCH_CNC_TM_INDEX_MODE_1_E,
    CPSS_DXCH_CNC_TM_INDEX_MODE_2_E,
    CPSS_DXCH_CNC_TM_INDEX_MODE_3_E
} CPSS_DXCH_CNC_TM_INDEX_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_CNC_COUNTER_STC
 *
 * Description:
 *      The counter entry contents.
 *
 * Fields:
 *      byteCount      - byte count
 *      packetCount    - packets count
 *
 * Comment:
 *      See the possible counter HW formats of Lion and above devices in
 *      CPSS_DXCH_CNC_COUNTER_FORMAT_ENT description.
 *      For DxCh3,xCat and xCat3 devices the byte counter has 35 bits,
 *      the packets counter has 29 bits
 */
typedef struct
{
    GT_U64     byteCount;
    GT_U64     packetCount;
} CPSS_DXCH_CNC_COUNTER_STC;

/*******************************************************************************
* cpssDxChCncBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
);

/*******************************************************************************
* cpssDxChCncBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
);

/*******************************************************************************
* cpssDxChCncBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*       indexRangesBmp   - the counter index ranges bitmap
*                         DxCh3,xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
);

/*******************************************************************************
* cpssDxChCncBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) the counter index ranges bitmap
*                         DxCh3,xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U64                    *indexRangesBmpPtr
);

/*******************************************************************************
* cpssDxChCncPortClientEnableSet
*
* DESCRIPTION:
*   The function sets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_PHYSICAL_PORT_NUM      portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   enable
);

/*******************************************************************************
* cpssDxChCncPortClientEnableGet
*
* DESCRIPTION:
*   The function gets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_PHYSICAL_PORT_NUM      portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *enablePtr
);

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
*
* DESCRIPTION:
*   The function enables or disables counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
*
* DESCRIPTION:
*   The function gets status of counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChCncVlanClientIndexModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode
*   for the Vlan CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanClient         - VLAN client
*                            relevant clients:
*                            CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E
*                            CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E
*       indexMode          - index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncVlanClientIndexModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          vlanClient,
    IN  CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT indexMode
);

/*******************************************************************************
* cpssDxChCncVlanClientIndexModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode
*   for the Vlan CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanClient         - VLAN client
*                            relevant clients:
*                            CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E
*                            CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E
*
* OUTPUTS:
*       indexModePtr       - (pointer to)index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - enexpected HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncVlanClientIndexModeGet
(
    IN   GT_U8                             devNum,
    IN   CPSS_DXCH_CNC_CLIENT_ENT          vlanClient,
    OUT  CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT *indexModePtr
);

/*******************************************************************************
* cpssDxChCncPacketTypePassDropToCpuModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode of To CPU packets
*   for the Packet Type Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       toCpuMode        - to CPU packets index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPacketTypePassDropToCpuModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT toCpuMode
);

/*******************************************************************************
* cpssDxChCncPacketTypePassDropToCpuModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode of To CPU packets
*   for the Packet Type Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       toCpuModePtr        - to CPU packets index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPacketTypePassDropToCpuModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT *toCpuModePtr
);

/*******************************************************************************
* cpssDxChCncTmClientIndexModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode
*   for the Traffic manager CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2, Bobcat3.
*
* INPUTS:
*       devNum             - device number
*       indexMode          - index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncTmClientIndexModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_CNC_TM_INDEX_MODE_ENT indexMode
);

/*******************************************************************************
* cpssDxChCncTmClientIndexModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode
*   for the Traffic manager CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2, Caelum.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2, Bobcat3..
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       indexModePtr       - (pointer to)index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncTmClientIndexModeGet
(
    IN   GT_U8                           devNum,
    OUT  CPSS_DXCH_CNC_TM_INDEX_MODE_ENT *indexModePtr
);

/*******************************************************************************
* cpssDxChCncClientByteCountModeSet
*
* DESCRIPTION:
*   The function sets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*       countMode        - count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncClientByteCountModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    IN  CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode
);

/*******************************************************************************
* cpssDxChCncClientByteCountModeGet
*
* DESCRIPTION:
*   The function gets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*
* OUTPUTS:
*       countModePtr     - (pointer to) count mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncClientByteCountModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    OUT CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT *countModePtr
);

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeSet
*
* DESCRIPTION:
*   The function sets Egress VLAN Drop counting mode.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       mode             - Egress VLAN Drop counting mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressVlanDropCountModeSet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeGet
*
* DESCRIPTION:
*   The function gets Egress VLAN Drop counting mode.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       modePtr             - (pointer to) Egress VLAN Drop counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - on reserved value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressVlanDropCountModeGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT *modePtr
);

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableSet
*
* DESCRIPTION:
*   The function enable/disables clear by read mode of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableGet
*
* DESCRIPTION:
*   The function gets clear by read mode status of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueSet
*
* DESCRIPTION:
*   The function sets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadValueSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
);

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueGet
*
* DESCRIPTION:
*   The function gets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterClearByReadValueGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
);

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableSet
*
* DESCRIPTION:
*   The function enables/disables wraparound for all CNC counters
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableGet
*
* DESCRIPTION:
*   The function gets status of wraparound for all CNC counters
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChCncCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum       - device number
*       blockNum     - CNC block number
*                      valid range see in datasheet of specific device.
*       indexNumPtr  - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr  - (pointer to) actual size of array of indexes
*       indexesArr[] - (pointer to) array of indexes of counters wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterWraparoundIndexesGet
(
    IN    GT_U8    devNum,
    IN    GT_U32   blockNum,
    INOUT GT_U32   *indexNumPtr,
    OUT   GT_U32   indexesArr[]
);

/*******************************************************************************
* cpssDxChCncCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
);

/*******************************************************************************
* cpssDxChCncCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
);

/*******************************************************************************
* cpssDxChCncBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum
);

/*******************************************************************************
* cpssDxChCncBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                     *inProcessBlocksBmpPtr
);

/*******************************************************************************
* cpssDxChCncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  CNC counter value
*       GT_NOT_READY             - Upload started after upload trigger or
*                                  continued after queue rewind but yet
*                                  not paused due to queue full and yet not finished.
*                                  The part of queue memory does not contain uploaded counters yet.
*                                  No counters retrieved.
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                                  retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncUploadedBlockGet
(
    IN     GT_U8                             devNum,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
);

/*******************************************************************************
* cpssDxChCncCountingEnableSet
*
* DESCRIPTION:
*   The function enables counting on selected cnc unit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*       enable  - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCountingEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    IN  GT_BOOL                                 enable
);

/*******************************************************************************
* cpssDxChCncCountingEnableGet
*
* DESCRIPTION:
*   The function gets enable counting status on selected cnc unit.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable
*                     GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCountingEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    OUT GT_BOOL                                 *enablePtr
);

/*******************************************************************************
* cpssDxChCncCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
);

/*******************************************************************************
* cpssDxChCncCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
);

/*******************************************************************************
* cpssDxChCncEgressQueueClientModeSet
*
* DESCRIPTION:
*   The function sets Egress Queue client counting mode
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number
*       mode            - Egress Queue client counting mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressQueueClientModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChCncEgressQueueClientModeGet
*
* DESCRIPTION:
*   The function gets Egress Queue client counting mode
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       modePtr         - (pointer to) Egress Queue client counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncEgressQueueClientModeGet
(
    IN  GT_U8                                      devNum,
    OUT CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT *modePtr
);

/*******************************************************************************
* cpssDxChCncCpuAccessStrictPriorityEnableSet
*
* DESCRIPTION:
*   The function enables strict priority of CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*       enable          - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCpuAccessStrictPriorityEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* cpssDxChCncCpuAccessStrictPriorityEnableGet
*
* DESCRIPTION:
*   The function gets enable status of strict priority of
*   CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncCpuAccessStrictPriorityEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
);

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
);

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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
*       blockNum       - CNC block number
*                        valid range see in datasheet of specific device.
*       client         -  CNC client
*
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
);

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*       indexRangesBmp   - bitmap of counter index ranges
*                         DxCh3,xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on not relevant bits in indexRangesBmp
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
);

/*******************************************************************************
* cpssDxChCncPortGroupBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
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
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       client           - CNC client
*                          (APPLICABLE DEVICES:DxCh3; xCat; xCat3; Lion; xCat2; Lion2).
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) bitmap counter index ranges
*                         DxCh3,xCat and xCat3 devices have 10 ranges of counter indexes,
*                         each 2048 indexes (APPLICABLE RANGES: 0..2K-1, 2K..4K-1, ,18K..20K-1)
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Lion devices has 64 ranges (512 indexes each).
*                         xCat2 devices has 64 ranges (256 indexes each).
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*                         For Bobcat2, Caelum, Bobcat3 (APPLICABLE RANGES: 0..1K, 1K..2K-1,  ,63K..64K-1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U64                    *indexRangesBmpPtr
);

/*******************************************************************************
* cpssDxChCncPortGroupCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       indexNumPtr     - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr     - (pointer to) actual size of array of indexes
*       portGroupIdArr[]- (pointer to) array of port group Ids of
*                         counters wrapped around.
*                         The NULL pointer supported.
*       indexesArr[]    - (pointer to) array of indexes of counters
*                         wrapped around
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The wraparound indexes are cleared on read.
*       Since in the HW each pair of wraparound indexes resides in one register,
*       cleared by read, in case not all the 8 possible indexes are read at once,
*       some data may be lost.
*       Recommendation: Always request 8 indexes => *indexNumPtr=8 as input.
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterWraparoundIndexesGet
(
    IN    GT_U8                   devNum,
    IN    GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN    GT_U32                  blockNum,
    INOUT GT_U32                  *indexNumPtr,
    OUT   GT_U32                  portGroupIdArr[],
    OUT   GT_U32                  indexesArr[]
);

/*******************************************************************************
* cpssDxChCncPortGroupCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
);

/*******************************************************************************
* cpssDxChCncPortGroupCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
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
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       blockNum         - CNC block number
*                          valid range see in datasheet of specific device.
*       index            - counter index in the block
*                          valid range see in datasheet of specific device.
*       format           - CNC counter HW format,
*                          relevant only for Lion and above
*
* OUTPUTS:
*       counterPtr       - (pointer to) received CNC counter value.
*                          For multi port group devices
*                          the result counter contains the sum of
*                          counters read from all port groups.
*                          It contains more bits than in HW.
*                          For example the sum of 4 35-bit values may be
*                          37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
);

/*******************************************************************************
* cpssDxChCncPortGroupBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncPortGroupBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum
);

/*******************************************************************************
* cpssDxChCncPortGroupBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
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
*                          Checks all active port groups of the bitmap.
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    OUT GT_U32                      *inProcessBlocksBmpPtr
);

/*******************************************************************************
* cpssDxChCncPortGroupUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncPortGroupBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transferred counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncPortGroupUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncPortGroupUploadedBlockGet one more time to get rest of the block.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       portGroupsBmp         - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*       format                - CNC counter HW format,
*                               relevant only for Lion and above
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller. For multi port group devices
*                               the result counters contain the sum of
*                               counters read from all port groups.
*                               It contains more bits than in HW.
*                               For example the sum of 4 35-bit values may be
*                               37-bit value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  CNC counter value
*       GT_NOT_READY             - Upload started after upload trigger or
*                                  continued after queue rewind but yet
*                                  not paused due to queue full and yet not finished.
*                                  The part of queue memory does not contain uploaded counters yet.
*                                  No counters retrieved.
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - if the previous FU messages were not
*                                  retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*       GT_NOT_SUPPORTED         - block upload operation not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncPortGroupUploadedBlockGet.
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupUploadedBlockGet
(
    IN     GT_U8                             devNum,
    IN     GT_PORT_GROUPS_BMP                portGroupsBmp,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
);

/*******************************************************************************
* cpssDxChCncPortGroupCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
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
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
);

/*******************************************************************************
* cpssDxChCncPortGroupCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
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
*       blockNum        - CNC block number
*                         valid range see in datasheet of specific device.
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCncPortGroupCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
);

/*******************************************************************************
* cpssDxChCncOffsetForNatClientSet
*
* DESCRIPTION:
*      Set the CNC offset for NAT client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*       offset      - CNC offset for NAT client
*                     (APPLICABLE RANGES: 0..65535)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - on out off range offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The CNC clients ARP and NAT are muxed (since for a
*       given packet the user can access ARP entry or NAT entry).
*       This offset is added to the NAT when sending the pointer to the CNC.
*
*******************************************************************************/
GT_STATUS cpssDxChCncOffsetForNatClientSet
(
    IN   GT_U8    devNum,
    IN   GT_U32   offset
);

/*******************************************************************************
* cpssDxChCncOffsetForNatClientGet
*
* DESCRIPTION:
*      Get the CNC offset for NAT client.
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
*       offsetPtr   - (pointer to) CNC offset for NAT client
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The CNC clients ARP and NAT are muxed between them (since for a
*      given packet the user can access ARP entry or NAT entry).
*      This offset is added to the NAT when sending the pointer to the CNC.
*
*******************************************************************************/
GT_STATUS cpssDxChCncOffsetForNatClientGet
(
    IN   GT_U8    devNum,
    OUT  GT_U32   *offsetPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChCnch */



