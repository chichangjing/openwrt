/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChL2Mll.h
*
* DESCRIPTION:
*       The CPSS DXCH L2 Mll definitions
*
* FILE REVISION NUMBER:
*       $Revision: 21 $
*******************************************************************************/
#ifndef __cpssDxChL2Mllh
#define __cpssDxChL2Mllh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>

/*
 * Typedef: struct CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC
 *
 * Description: L2 MLL Exception Counters
 *
 * Fields:
 *      skip    - number of L2 MLL entries skipped.
 *      ttl     - number of TTL exceptions in the L2 MLL.
 */
typedef struct
{
    GT_U32 skip;
    GT_U32 ttl;
}CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC;

/*
 * typedef: enum CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT
 *
 * Description: the counter Set Port/Trunk mode
 *
 * Enumerations:
 *  CPSS_DXCH_L2MLL_DISREGARD_PORT_TRUNK_CNT_MODE_E - This counter-set counts all
 *                                                 packets regardless of their
 *                                                 In / Out port or Trunk.
 *  CPSS_DXCH_L2MLL_PORT_CNT_MODE_E - This counter-set counts packets Received /
 *                                 Transmitted via binded Port + Dev.
 *  CPSS_DXCH_L2MLL_TRUNK_CNT_MODE_E - This counter-set counts packets Received /
 *                                 Transmitted via binded Trunk.
 */
typedef enum
{
    CPSS_DXCH_L2MLL_DISREGARD_PORT_TRUNK_CNT_MODE_E = 0,
    CPSS_DXCH_L2MLL_PORT_CNT_MODE_E                 = 1,
    CPSS_DXCH_L2MLL_TRUNK_CNT_MODE_E                = 2
}CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT
 *
 * Description: the counter Set vlan mode
 *
 * Enumerations:
 *  CPSS_DXCH_L2MLL_DISREGARD_VLAN_CNT_MODE_E - This counter-set counts all packets
 *                                           regardless of their In / Out vlan.
 *  CPSS_DXCH_L2MLL_USE_VLAN_CNT_MODE_E - This counter-set counts packets Received
 *                                     / Transmitted via binded vlan.
 */
typedef enum
{
    CPSS_DXCH_L2MLL_DISREGARD_VLAN_CNT_MODE_E = 0,
    CPSS_DXCH_L2MLL_USE_VLAN_CNT_MODE_E       = 1
}CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT;

/*
 * Typedef: struct CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC
 *
 * Description: L2 MLL  Counter Set interface mode configuration
 *
 * Fields:
 *      portTrunkCntMode - the counter Set Port/Trunk mode
 *      ipMode           - the cnt set IP interface mode , which protocol stack
 *                         ipv4 ,ipv6 or both (which is actully disregarding the
 *                         ip protocol)
 *      vlanMode         - the counter Set vlan mode.
 *      hwDevNum           - the binded HW devNum , relevant only if
 *                         portTrunkCntMode = CPSS_DXCH_IP_PORT_CNT_MODE_E
 *      port             - the binded por , relevant only if
 *                         portTrunkCntMode = CPSS_DXCH_IP_PORT_CNT_MODE_E
 *      trunk            - the binded trunk , relevant only if
 *                         portTrunkCntMode = CPSS_DXCH_IP_TRUNK_CNT_MODE_E
 *      portTrunk        - the above port/trunk
 *      vlanId           - the binded vlan , relevant only if
 *                         vlanMode = CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E
 */
typedef struct CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STCT
{
    CPSS_DXCH_L2MLL_PORT_TRUNK_CNT_MODE_ENT portTrunkCntMode;
    CPSS_IP_PROTOCOL_STACK_ENT           ipMode;
    CPSS_DXCH_L2MLL_VLAN_CNT_MODE_ENT       vlanMode;
    GT_HW_DEV_NUM                        hwDevNum;
    union
    {
        GT_PORT_NUM                      port;
        GT_TRUNK_ID                      trunk;
    }portTrunk;
    GT_U16                               vlanId;
}CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC;

/*
 * typedef: enum CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT
 *
 * Description: Selects the start entry of the MLL Table's next Link List element
 *
 * Enumerations:
 *
 *      CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_FIRST_MLL_E - first mll is the next Linked List element
 *      CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_SECOND_MLL_E - second mll is the next Linked List element
 */
typedef enum
{
    CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_FIRST_MLL_E   = 0,
    CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_SECOND_MLL_E
}CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT;


/*
 * Typedef: struct CPSS_DXCH_L2_MLL_LTT_ENTRY_STC
 *
 * Description: L2 MLL Lookup Translation Table (LTT) Entry
 *
 * Fields:
 *      mllPointer           - pointer to the MLL entry.
 *                             valid range see in datasheet of specific device.
 *      entrySelector        - Determines which of the two entries in the
 *                             memory line pointed by mllPointer is the first
 *                             linked-list member
 *      mllMaskProfileEnable - GT_TRUE: enable MLL entry according to match
 *                                      with its profile mask.
 *                             GT_FALSE: MLL entry is used while ignoring
 *                                       the profile mask.
 *      mllMaskProfile       - The mask profile. (APPLICABLE RANGES: 0..14)
 *                             Relevant for mllMaskProfileEnable == GT_TRUE
 */
typedef struct
{
   GT_U32                                     mllPointer;
   CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT   entrySelector;
   GT_BOOL                                    mllMaskProfileEnable;
   GT_U32                                     mllMaskProfile;
} CPSS_DXCH_L2_MLL_LTT_ENTRY_STC;

/*
 * Typedef: struct CPSS_DXCH_L2_MLL_ENTRY_STC
 *
 * Description: L2 MLL Entry
 *
 * Fields:
 *      last                    - GT_TRUE: the last MLL entry in this list.
 *                                GT_FALSE: there are more MLL entries in this
 *                                          list.
 *      unknownUcFilterEnable   - GT_TRUE: unknown UC traffic is not replicated
 *                                         by this MLL entry.
 *                                GT_FALSE: unknown UC traffic is replicated by
 *                                          this MLL entry.
 *      unregMcFilterEnable     - GT_TRUE: unregistered MC traffic is not
 *                                         replicated by this MLL entry.
 *                                GT_FALSE: unregistered MC traffic is
 *                                          replicated by this MLL entry.
 *      bcFilterEnable          - GT_TRUE: Broadcast traffic is not replicated
 *                                         by this MLL entry.
 *                                GT_FALSE: Broadcast traffic is replicated by
 *                                          this MLL entry.
 *      mcLocalSwitchingEnable - GT_TRUE: allow replication to the source
 *                                        ePort of multicast traffic.
 *                               GT_FALSE: avoid replication to the source
 *                                         ePort of multicast traffic.
 *                               (For xCat, Xcat3 ePort stands for logical port)
 *      maxHopCountEnable      - GT_TRUE: hop count of a replicated packet is
 *                                        no more than maxOutgoingHopCount.
 *                               GT_FALSE: no hop count limit.
 *                                         (relevant for TRILL).
 *                               (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      maxOutgoingHopCount    - The hop count of a replicated packet is no
 *                               more than this value.
 *                               Relevant only if maxHopCountEn set to GT_TRUE.
 *                               (APPLICABLE RANGES: 0..63)
 *                               (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      egressInterface        - target Egress Interface. Valid types are:
 *                               CPSS_INTERFACE_PORT_E
 *                               CPSS_INTERFACE_TRUNK_E
 *                               CPSS_INTERFACE_VIDX_E
 *                               CPSS_INTERFACE_VID_E
 *      maskBitmap             - A per-profile mask indicator. In each MLL entry
 *                               the relevant bit in this bitmap indicates
 *                               whether the current MLL entry is masked or not.
 *                               In each bit, a value of 0 indicates that the
 *                               entry is skipped (masked), while a value of 1
 *                               indicates that the current entry is active.
 *                               This bitmap is indexed by the L2 MLL LTT entry
 *                               field <mllMaskProfile>.
 *                               (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      ttlThreshold           - if the packet's TTl is less than this field,
 *                               the packet is not replicated. (APPLICABLE RANGES: 0..255)
 *                               (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      bindToMllCounterEnable - GT_TRUE: bind entry to MLL counter indexed by
 *                                        <mllCounterIndex> field.
 *                               GT_FALSE: entry is not binded to one of the
 *                                         MLL counters.
 *                               (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      mllCounterIndex        - The MLL counter to bind the entry to.
 *                               Relevant only if <bindToMllCounterEnable> == GT_TRUE.
 *                               (APPLICABLE RANGES: 0..2)
 *                               (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      onePlusOneFilteringEnable - When enabled, and <MC Local Switching Enable>
 *                               is disabled, source filtering ignores the least
 *                               significant bit of the ePort.
 *                               The assumption is that the two ePorts in the 1+1
 *                               scheme are two contiguous ePort numbers, that
 *                               differ only in the lsbit.
 *                               (For xCat,Xcat3 ePort stands for logical port)
 *      meshId                 - Mesh ID number
 *                               Used for split horizon filtering (e.g. in VPLS,
 *                               TRILL, MiM).
 *                               Bobcat2; Caelum; Bobcat3: (APPLICABLE RANGES: 0..255)
 *                               xCat, xCat3: (APPLICABLE RANGES: 0..3)
 *                               xCat device: applicable starting from revision C0
 *      tunnelStartEnable      - whether this nexthop is tunnel start entry, in
 *                               which case the outInteface & mac are irrlevant and
 *                               the tunnel id is used.
 *                               (APPLICABLE DEVICES: xCat, xCat3)
 *                               xCat device: applicable starting from revision C0
 *      tunnelStartPointer     - the tunnel pointer in case this is a tunnel start
 *                               <tunnelStartEnable> is GT_TRUE.
 *                               (APPLICABLE DEVICES: xCat, xCat3)
 *                               xCat device: applicable starting from revision C0
 *      tunnelStartPassengerType - the type of the tunneled, passenger packet.
 *                                 (APPLICABLE DEVICES: xCat, xCat3)
 *                                 xCat device: applicable starting from revision C0
 */
typedef struct
{
   GT_BOOL  last;
   GT_BOOL  unknownUcFilterEnable;
   GT_BOOL  unregMcFilterEnable;
   GT_BOOL  bcFilterEnable;
   GT_BOOL  mcLocalSwitchingEnable;
   GT_BOOL  maxHopCountEnable;
   GT_U32   maxOutgoingHopCount;
   CPSS_INTERFACE_INFO_STC egressInterface;
   GT_U32   maskBitmap;
   GT_U32   ttlThreshold;
   GT_BOOL  bindToMllCounterEnable;
   GT_U32   mllCounterIndex;
   GT_BOOL  onePlusOneFilteringEnable;
   GT_U32   meshId;
   GT_BOOL  tunnelStartEnable;
   GT_U32   tunnelStartPointer;
   CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT tunnelStartPassengerType;
} CPSS_DXCH_L2_MLL_ENTRY_STC;

/*
 * Typedef: struct CPSS_DXCH_L2_MLL_PAIR_STC
 *
 * Description:     Representation of the l2 muticast Link List pair in HW,
 *
 * Fields:
 *
 *  firstMllNode   - the first Mll of the pair
 *  secondMllNode  - the second Mll of the pair
 *  nextPointer    - pointer to the next MLL entry
 *  entrySelector  - Selects the start entry of the MLL Table's next
 *                   Linked List element
 *                   (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 */
typedef struct
{
    CPSS_DXCH_L2_MLL_ENTRY_STC                  firstMllNode;
    CPSS_DXCH_L2_MLL_ENTRY_STC                  secondMllNode;

    GT_U32                                      nextPointer;
    CPSS_DXCH_L2_MLL_PAIR_ENTRY_SELECTOR_ENT    entrySelector;
}CPSS_DXCH_L2_MLL_PAIR_STC;

/*******************************************************************************
* cpssDxChL2MllLookupForAllEvidxEnableSet
*
* DESCRIPTION:
*       Enable or disable MLL lookup for all multi-target packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: MLL lookup is performed for all multi-target packets.
*                 GT_FALSE: MLL lookup is performed only for multi-target
*                           packets with eVIDX >= 4K. For packets with
*                           eVidx < 4K L2 MLL is not accessed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllLookupForAllEvidxEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChL2MllLookupForAllEvidxEnableGet
*
* DESCRIPTION:
*       Get enabling status of MLL lookup for all multi-target packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE: MLL lookup is performed for all multi-target
*                             packets.
*                    GT_FALSE: MLL lookup is performed only for multi-target
*                              packets with eVIDX >= 4K. For packets with
*                              eVidx < 4K L2 MLL is not accessed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllLookupForAllEvidxEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChL2MllLookupMaxVidxIndexSet
*
* DESCRIPTION:
*       Set the maximal VIDX value that refers to a port distribution list(VIDX).
*       By default the value is 4K-1
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       maxVidxIndex  - maximal VIDX value.
*                       When cpssDxChL2MllLookupForAllEvidxEnableSet == FALSE
*                       L2 replication is performed to all eVidx > maxVidxIndex
*                       (APPLICABLE RANGES: 0..0xFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_OUT_OF_RANGE          - parameter is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when cpssDxChL2MllLookupForAllEvidxEnableSet == FALSE
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllLookupMaxVidxIndexSet
(
    IN GT_U8   devNum,
    IN GT_U32  maxVidxIndex
);

/*******************************************************************************
* cpssDxChL2MllLookupMaxVidxIndexGet
*
* DESCRIPTION:
*       Get the maximal VIDX value that refers to a port distribution list(VIDX).
*       By default the value is 4K-1
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       maxVidxIndexPtr - (pointer to) maximal VIDX value.
*                         When <Enable MLL Lookup for All eVidx> == FALSE
*                         L2 replication is performed to all eVidx > <Max VIDX Index>
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when cpssDxChL2MllLookupForAllEvidxEnableGet== FALSE
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllLookupMaxVidxIndexGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *maxVidxIndexPtr
);

/*******************************************************************************
* cpssDxChL2MllTtlExceptionConfigurationSet
*
* DESCRIPTION:
*       Set configuration for L2 MLL TTL Exceptions.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       trapEnable  - GT_TRUE: packet is trapped to the CPU with <cpuCode>
*                              if packet's TTL is less than MLL entry field
*                              <TTL Threshold>
*                     GT_FALSE: no packet trap to CPU due to a TTL exception
*                               in the MLL.
*       cpuCode     - CPU code of packets that are trapped to CPU due to a
*                     TTL exception in the MLL.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllTtlExceptionConfigurationSet
(
    IN GT_U8                    devNum,
    IN GT_BOOL                  trapEnable,
    IN CPSS_NET_RX_CPU_CODE_ENT cpuCode
);

/*******************************************************************************
* cpssDxChL2MllTtlExceptionConfigurationGet
*
* DESCRIPTION:
*       Get configuration of L2 MLL TTL Exceptions.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       trapEnablePtr   - (pointer to)
*                         GT_TRUE: packet is trapped to the CPU with <cpuCode>
*                                  if packet's TTL is less than MLL entry field
*                                  <TTL Threshold>
*                         GT_FALSE: no packet trap to CPU due to a TTL
*                                   exception in the MLL.
*       cpuCodePtr      - (pointer to) CPU code of packets that are trapped to
*                         CPU due to a TTL exception in the MLL.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllTtlExceptionConfigurationGet
(
    IN  GT_U8                    devNum,
    OUT GT_BOOL                  *trapEnablePtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT *cpuCodePtr
);

/*******************************************************************************
* cpssDxChL2MllExceptionCountersGet
*
* DESCRIPTION:
*       Get L2 MLL exception counters.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       countersPtr   - (pointer to) L2 MLL exception counters.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllExceptionCountersGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC  *countersPtr
);

/*******************************************************************************
* cpssDxChL2MllPortGroupExceptionCountersGet
*
* DESCRIPTION:
*       Get L2 MLL exception counters.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       countersPtr   - (pointer to) L2 MLL exception counters.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllPortGroupExceptionCountersGet
(
    IN  GT_U8                                   devNum,
    IN GT_PORT_GROUPS_BMP                       portGroupsBmp,
    OUT CPSS_DXCH_L2_MLL_EXCEPTION_COUNTERS_STC  *countersPtr
);

/*******************************************************************************
* cpssDxChL2MllCounterGet
*
* DESCRIPTION:
*       Get L2 MLL counter.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       index   - counter number. (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..2)
*
* OUTPUTS:
*       counterPtr   - (pointer to) L2 MLL counter value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*******************************************************************************/
GT_STATUS cpssDxChL2MllCounterGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  index,
    OUT GT_U32  *counterPtr
);

/*******************************************************************************
* cpssDxChL2MllPortGroupCounterGet
*
* DESCRIPTION:
*       Get L2 MLL counter.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index   - counter number. (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)(APPLICABLE RANGES: 0..2)
*
*
* OUTPUTS:
*       counterPtr   - (pointer to) L2 MLL counter value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*******************************************************************************/
GT_STATUS cpssDxChL2MllPortGroupCounterGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32              index,
    OUT GT_U32              *counterPtr
);


/*******************************************************************************
* cpssDxChL2MllSetCntInterfaceCfg
*
* DESCRIPTION:
*      Sets a L2 mll counter set's bounded inteface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       mllCntSet       - l2 mll counter set
*                         (APPLICABLE RANGES: 0..1)
*       interfaceCfgPtr - the L2 mll counter interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_OUT_OF_RANGE          - parameter is out of range
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllSetCntInterfaceCfg
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           mllCntSet,
    IN CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC    *interfaceCfgPtr
);

/*******************************************************************************
* cpssDxChL2MllGetCntInterfaceCfg
*
* DESCRIPTION:
*      Gets a L2 mll counter set's bounded inteface.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       mllCntSet       - l2 mll counter set
*                         (APPLICABLE RANGES: 0..1)
* OUTPUTS:
*       interfaceCfgPtr - the L2 mll counter interface configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_OUT_OF_RANGE          - parameter is out of range
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllGetCntInterfaceCfg
(
    IN  GT_U8                                            devNum,
    IN  GT_U32                                           mllCntSet,
    OUT CPSS_DXCH_L2MLL_COUNTER_SET_INTERFACE_CFG_STC    *interfaceCfgPtr
);

/*******************************************************************************
* cpssDxChL2MllPortGroupMcCntGet
*
* DESCRIPTION:
*      Get the L2MLL MC counter
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - the device number.
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       mllCntSet       - l2 mll counter set
*                         (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       mllOutMCPktsPtr - According to the configuration of this cnt set, The
*                      number of Multicast packets Duplicated by the
*                      L2 MLL Engine
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllPortGroupMcCntGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  mllCntSet,
    OUT GT_U32                  *mllOutMCPktsPtr
);

/*******************************************************************************
* cpssDxChL2MllMcCntGet
*
* DESCRIPTION:
*      Get the L2MLL MC counter
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - the device number.
*       mllCntSet       - l2 mll counter set
*                         (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       mllOutMCPktsPtr - According to the configuration of this cnt set, The
*                      number of Multicast packets Duplicated by the
*                      L2 MLL Engine
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllMcCntGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mllCntSet,
    OUT GT_U32  *mllOutMCPktsPtr
);


/*******************************************************************************
* cpssDxChL2MllSilentDropCntGet
*
* DESCRIPTION:
*      Get the silent drops in the L2 MLL replication block.
*      A drop occurs for a packet that was:
*      - Replicated in the TTI/IP MLL
*        AND
*      - All the elements of the linked list are filtered
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       silentDropPktsPtr - (pointer to) the number of counted silent dropped packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllSilentDropCntGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *silentDropPktsPtr
);

/*******************************************************************************
* cpssDxChL2MllPortGroupSilentDropCntGet
*
* DESCRIPTION:
*      Get the silent drops in the L2 MLL replication block.
*      A drop occurs for a packet that was:
*      - Replicated in the TTI/IP MLL
*        AND
*      - All the elements of the linked list are filtered
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - the device number
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       silentDropPktsPtr - (pointer to) the number of counted silent dropped packets.

* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllPortGroupSilentDropCntGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *silentDropPktsPtr
);

/*******************************************************************************
* cpssDxChL2MllLttEntrySet
*
* DESCRIPTION:
*       Set L2 MLL Lookup Translation Table (LTT) entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the LTT index. eVidx range.
*       lttEntryPtr - (pointer to) L2 MLL LTT entry.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - LTT entry's parameter is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllLttEntrySet
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_DXCH_L2_MLL_LTT_ENTRY_STC   *lttEntryPtr
);

/*******************************************************************************
* cpssDxChL2MllLttEntryGet
*
* DESCRIPTION:
*       Get L2 MLL Lookup Translation Table (LTT) entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       index       - the LTT index. eVidx range.
*
* OUTPUTS:
*       lttEntryPtr - (pointer to) L2 MLL LTT entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllLttEntryGet
(
    IN  GT_U8                            devNum,
    IN  GT_U32                           index,
    OUT CPSS_DXCH_L2_MLL_LTT_ENTRY_STC   *lttEntryPtr
);

/*******************************************************************************
* cpssDxChL2MllPairWrite
*
* DESCRIPTION:
*       Write L2 MLL pair entry to L2 MLL Table.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairWriteForm  - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - the L2 link list pair entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - MLL entry's parameter is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllPairWrite
(
    IN GT_U8                                        devNum,
    IN GT_U32                                       mllPairEntryIndex,
    IN CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT           mllPairWriteForm,
    IN CPSS_DXCH_L2_MLL_PAIR_STC                    *mllPairEntryPtr
);

/*******************************************************************************
* cpssDxChL2MllPairRead
*
* DESCRIPTION:
*       Read L2 MLL pair entry from L2 MLL Table.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2.
**
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairReadForm   - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the L2 link list pair entry

*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllPairRead
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      mllPairEntryIndex,
    IN  CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT          mllPairReadForm,
    OUT CPSS_DXCH_L2_MLL_PAIR_STC                  *mllPairEntryPtr
);


/*******************************************************************************
* cpssDxChL2MllMultiTargetPortEnableSet
*
* DESCRIPTION:
*       Enable multi-target port mapping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - multi-target port mapping status:
*                     GT_TRUE: enable multi-target port mapping
*                     GT_FALSE: disable multi-target port mapping
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function is useful when unicast traffic is forwarded over multiple
*       interfaces, for example, in 1+1 protection schemes where traffic is
*       forwarded to a logical unicast target that is represented by the target
*       eport but is physically sent over 2 paths: the working path and the
*       protection path
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllMultiTargetPortEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChL2MllMultiTargetPortEnableGet
*
* DESCRIPTION:
*       Get the multi-target port mapping enabling status
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
*       enablePtr   - (pointer to) multi-target port mapping status:
*                     GT_TRUE: multi-target port mapping is enabled
*                     GT_FALSE: multi-target port mapping is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function is useful when unicast traffic is forwarded over multiple
*       interfaces, for example, in 1+1 protection schemes where traffic is
*       forwarded to a logical unicast target that is represented by the target
*       eport but is physically sent over 2 paths: the working path and the
*       protection path
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllMultiTargetPortEnableGet
(
    IN  GT_U8                       devNum,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChL2MllMultiTargetPortSet
*
* DESCRIPTION:
*       Set the multi-target port value and mask
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       value       - multi-target port value
*                     (APPLICABLE RANGES: 0..0x1ffff)
*       mask        - multi-target port mask
*                     (APPLICABLE RANGES: 0..0x1ffff)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on out of range value or mask
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function is useful when unicast traffic is forwarded over multiple
*       interfaces, for example, in 1+1 protection schemes where traffic is
*       forwarded to a logical unicast target that is represented by the target
*       eport but is physically sent over 2 paths: the working path and the
*       protection path
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllMultiTargetPortSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      value,
    IN  GT_U32                      mask
);

/*******************************************************************************
* cpssDxChL2MllMultiTargetPortGet
*
* DESCRIPTION:
*       Get the multi-target port value and mask
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
*       valuePtr    - (pointer to) multi-target port value
*       maskPtr     - (pointer to) multi-target port mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function is useful when unicast traffic is forwarded over multiple
*       interfaces, for example, in 1+1 protection schemes where traffic is
*       forwarded to a logical unicast target that is represented by the target
*       eport but is physically sent over 2 paths: the working path and the
*       protection path
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllMultiTargetPortGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *valuePtr,
    OUT GT_U32                      *maskPtr
);

/*******************************************************************************
* cpssDxChL2MllMultiTargetPortBaseSet
*
* DESCRIPTION:
*       Set the multi-target port base
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portBase    - multi-target port base
*                     (APPLICABLE RANGES: 0..0x1ffff)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on out of range port base
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function is useful when unicast traffic is forwarded over multiple
*       interfaces, for example, in 1+1 protection schemes where traffic is
*       forwarded to a logical unicast target that is represented by the target
*       eport but is physically sent over 2 paths: the working path and the
*       protection path
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllMultiTargetPortBaseSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portBase
);

/*******************************************************************************
* cpssDxChL2MllMultiTargetPortBaseGet
*
* DESCRIPTION:
*       Get the multi-target port base
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
*       portBasePtr - (pointer to) multi-target port base
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function is useful when unicast traffic is forwarded over multiple
*       interfaces, for example, in 1+1 protection schemes where traffic is
*       forwarded to a logical unicast target that is represented by the target
*       eport but is physically sent over 2 paths: the working path and the
*       protection path
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllMultiTargetPortBaseGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *portBasePtr
);

/*******************************************************************************
* cpssDxChL2MllPortToVidxBaseSet
*
* DESCRIPTION:
*       Set the port to VIDX base
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       vidxBase    - port VIDX base
*                     (APPLICABLE RANGES: 0..0xffff)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE          - on out of range vidx
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function is useful when unicast traffic is forwarded over multiple
*       interfaces, for example, in 1+1 protection schemes where traffic is
*       forwarded to a logical unicast target that is represented by the target
*       eport but is physically sent over 2 paths: the working path and the
*       protection path
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllPortToVidxBaseSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      vidxBase
);

/*******************************************************************************
* cpssDxChL2MllPortToVidxBaseGet
*
* DESCRIPTION:
*       Get the port to VIDX base
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
*       vidxBasePtr - (pointer to) port VIDX base
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function is useful when unicast traffic is forwarded over multiple
*       interfaces, for example, in 1+1 protection schemes where traffic is
*       forwarded to a logical unicast target that is represented by the target
*       eport but is physically sent over 2 paths: the working path and the
*       protection path
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllPortToVidxBaseGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                      *vidxBasePtr
);

/*******************************************************************************
* cpssDxChL2MllSourceBasedFilteringConfigurationSet
*
* DESCRIPTION:
*       Set configuration for Source Based L2 MLL Filtering.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_FALSE: MESH ID filtering is not enabled
*                      GT_TRUE:  MESH ID filtering is enabled
*       meshIdOffset - The MESH ID assigned to a packet is conveyed in the SrcID
*                      assigned to the packet.
*                      This configuration specifies the location of the LSB of
*                      the MESH ID in the SrcID field assigned to a packet
*                      (APPLICABLE RANGES: 0..11)
*
*       meshIdSize   - Specifies the number of bits that are used for
*                      the MESH ID in the SrcID field.
*                      Together with <MESH ID offset>, the MESH ID assigned to
*                      a packet can be extracted:
*                      Packet's MESH ID = <SST ID> (<MESH ID size> - 1 +
*                      <MESH ID offset> : <MESH ID offset>)
*                      (APPLICABLE RANGES: 1..8)
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_OUT_OF_RANGE          - on wrong meshIdOffset and meshIdSize
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllSourceBasedFilteringConfigurationSet
(
    IN GT_U8                    devNum,
    IN GT_BOOL                  enable,
    IN GT_U32                   meshIdOffset,
    IN GT_U32                   meshIdSize
);

/*******************************************************************************
* cpssDxChL2MllSourceBasedFilteringConfigurationGet
*
* DESCRIPTION:
*       Get configuration for Source Based L2 MLL Filtering.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to)
*                       GT_FALSE: MESH ID filtering is not enabled
*                       GT_TRUE:  MESH ID filtering is enabled
*       meshIdOffsetPtr - (pointer to) The MESH ID assigned to a packet is conveyed
*                       in the SrcID assigned to the packet.
*                       This configuration specifies the location of the LSB of
*                       the MESH ID in the SrcID field assigned to a packet
*                       (APPLICABLE RANGES: 0..11)
*
*       meshIdSizePtr   - (pointer to) Specifies the number of bits that are used
*                       for the MESH ID in the SrcID field.
*                       Together with <MESH ID offset>, the MESH ID assigned to
*                       a packet can be extracted:
*                       Packet's MESH ID = <SST ID> (<MESH ID size> - 1 +
*                       <MESH ID offset> : <MESH ID offset>)
*                       (APPLICABLE RANGES: 1..8)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllSourceBasedFilteringConfigurationGet
(
    IN  GT_U8                    devNum,
    OUT GT_BOOL                  *enablePtr,
    OUT GT_U32                   *meshIdOffsetPtr,
    OUT GT_U32                   *meshIdSizePtr
);

/*******************************************************************************
* cpssDxChL2MLLVirtualPortToMllMappingTableEntrySet
*
* DESCRIPTION:
*       Set virtual port mapping table entry. This entry maps
*       virtual target port  to L2MLL entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number.
*       targetDevNum   - target device number. (APPLICABLE RANGES: 0..15)
*       targetPortNum  - target port number.   (APPLICABLE RANGES: 0..63)
*       mllPointer     - pointer to L2 MLL entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - parameter not in valid range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       If the target supports APS 1+1, there are 2 L2MLL entries in this list, one
*       with a target logical port for the working path and one with a target
*       logical port for the protection path. It is required that these 2 target
*       logical ports be consecutive and differ only in the least significant bit of
*       the port number.
*
*       xCat device: applicable starting from revision C0
*******************************************************************************/
GT_STATUS cpssDxChL2MLLVirtualPortToMllMappingTableEntrySet
(
    IN GT_U8                             devNum,
    IN GT_U8                             targetDevNum,
    IN GT_U8                             targetPortNum,
    IN GT_U32                            mllPointer
);

/*******************************************************************************
* cpssDxChL2MLLVirtualPortToMllMappingTableEntryGet
*
* DESCRIPTION:
*       Get virtual port mapping table entry. This entry maps
*       virtual target port  to L2MLL entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number.
*       targetDevNum   - target device number. (APPLICABLE RANGES: 0..15)
*       targetPortNum  - target port number.   (APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       mllPointer     - (pointer to) L2 MLL entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - parameter not in valid range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChL2MLLVirtualPortToMllMappingTableEntryGet
(
    IN  GT_U8                                devNum,
    IN  GT_U8                                targetDevNum,
    IN  GT_U8                                targetPortNum,
    OUT GT_U32                              *mllPointer
);

/*******************************************************************************
* cpssDxChL2MllVidxEnableSet
*
* DESCRIPTION:
*       Enable/disable MLL lookup for given vidx on the specified device.
*       When enabled the VIDX value is used as the L2MLL index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       vidx    - VIDX value. Valid range see datasheet for specific device.
*       enable  - GT_TRUE: MLL lookup per VIDX is enabled.
*                 GT_FALSE: MLL lookup per VIDX is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllVidxEnableSet
(
    IN GT_U8   devNum,
    IN GT_U16  vidx,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChL2MllVidxEnableGet
*
* DESCRIPTION:
*       Get enabling status of MLL lookup for given vidx on the specified device.
*       When enabled the VIDX value is used as the L2MLL index.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - device number
*       vidx    - VIDX value. Valid range see datasheet for specific device.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                     GT_TRUE: MLL lookup per VIDX is enabled.
*                     GT_FALSE: MLL lookup per VIDX is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       xCat device: applicable starting from revision C0
*
*******************************************************************************/
GT_STATUS cpssDxChL2MllVidxEnableGet
(
    IN GT_U8     devNum,
    IN GT_U16    vidx,
    OUT GT_BOOL *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChL2Mllh */

