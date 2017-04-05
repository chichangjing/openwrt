/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCfgInit.h
*
* DESCRIPTION:
*       CPSS DxCh initialization of PPs and shadow data structures, and
*       declarations of global variables.
*
* FILE REVISION NUMBER:
*       $Revision: 40 $
*
*******************************************************************************/
#ifndef __cpssDxChCfgInith
#define __cpssDxChCfgInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpmTypes.h>

/*
 * typedef: enum CPSS_DXCH_CFG_TABLES_ENT
 *
 * Description:
 *      the type of tables that the DXCH devices hold.
 *      NOTE: some tables may shared on same memory space (like router and IPCL)
 *
 *
 * Enumerations:
 *  CPSS_DXCH_CFG_TABLE_VLAN_E - table type represent the VLAN table
 *  CPSS_DXCH_CFG_TABLE_FDB_E  - table type represent the FDB table
 *  CPSS_DXCH_CFG_TABLE_PCL_ACTION_E - table type represent the PCL action table
 *  CPSS_DXCH_CFG_TABLE_PCL_TCAM_E - table type represent the PCL Tcam table
 *  CPSS_DXCH_CFG_TABLE_ROUTER_NEXT_HOP_E - table type represent the router next hop table
 *  CPSS_DXCH_CFG_TABLE_ROUTER_LTT_E - table type represent the router lookup translation table (LTT)
 *  CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E - table type represent the Router Tcam table
 *                                Note: take in account that cpssDxChCfgTableNumEntriesGet
 *                                      function will return number of entries for IPv6,
 *                                      while actual number of IPv4 entries is 4(5 for DxCh2) times bigger.
 *  CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_QOS_E - table type represent the L3 ECMP/QoS table
 *  CPSS_DXCH_CFG_TABLE_TTI_TCAM_E - table type represent the TTI table
 *  CPSS_DXCH_CFG_TABLE_MLL_PAIR_E - table type represent the MLL pair table
 *                                      (the MLLs reside as pair in each entry)
 *  CPSS_DXCH_CFG_TABLE_POLICER_METERS_E - table type represent the policer metering table
 *  CPSS_DXCH_CFG_TABLE_POLICER_BILLING_COUNTERS_E - table type represent the policer billing counters table
 *  CPSS_DXCH_CFG_TABLE_VIDX_E - table type represent the VIDX (multicast groups) table
 *  CPSS_DXCH_CFG_TABLE_ARP_E - table type represent the ARP entries in ARP/Tunnel Start table
 *  CPSS_DXCH_CFG_TABLE_TUNNEL_START_E - table type represent Tunnel Start entries in the ARP/Tunnel Start table
 *  CPSS_DXCH_CFG_TABLE_STG_E - table type represent the STG (spanning tree groups) table
 *  CPSS_DXCH_CFG_TABLE_QOS_PROFILE_E - table type represent the QOS profile table
 *  CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E - table type represent the Mac to me table
 *  CPSS_DXCH_CFG_TABLE_CNC_E - table type represent the centralized counters (CNC) table
 *                              (the number of counters (X per block) , so 8 blocks means 8*X counters).
 *                              Refer to device datasheet to see number of CNC blocks and the number of
 *                              counters per CNC block.
 *  CPSS_DXCH_CFG_TABLE_CNC_BLOCK_E - table type represent CNC block (the number of conters per CNC block).
 *                                    Refer to device datasheet to see number of counters per CNC block.
 *  CPSS_DXCH_CFG_TABLE_TRUNK_E - table type represent trunk table.
 *  CPSS_DXCH_CFG_TABLE_LPM_RAM_E - table type represent LPM RAM
 *  CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_E - table type represent router ECMP table
 *  CPSS_DXCH_CFG_TABLE_L2_MLL_LTT_E - table type represent L2 MLL LTT
 *  CPSS_DXCH_CFG_TABLE_EPORT_E - table type represent ePorts table
 *  CPSS_DXCH_CFG_TABLE_DEFAULT_EPORT_E - table type represent default ePorts table
 *  CPSS_DXCH_CFG_TABLE_PHYSICAL_PORT_E - table type represent physical Ports table
 *
 *  CPSS_DXCH_CFG_TABLE_LAST_E - indication of the last table (not represents a table)
 */
typedef enum
{
    CPSS_DXCH_CFG_TABLE_VLAN_E,
    CPSS_DXCH_CFG_TABLE_FDB_E,
    CPSS_DXCH_CFG_TABLE_PCL_ACTION_E,
    CPSS_DXCH_CFG_TABLE_PCL_TCAM_E,
    CPSS_DXCH_CFG_TABLE_ROUTER_NEXT_HOP_E,
    CPSS_DXCH_CFG_TABLE_ROUTER_LTT_E,
    CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E,
    CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_QOS_E,
    CPSS_DXCH_CFG_TABLE_TTI_TCAM_E,
    CPSS_DXCH_CFG_TABLE_MLL_PAIR_E,
    CPSS_DXCH_CFG_TABLE_POLICER_METERS_E,
    CPSS_DXCH_CFG_TABLE_POLICER_BILLING_COUNTERS_E,
    CPSS_DXCH_CFG_TABLE_VIDX_E,
    CPSS_DXCH_CFG_TABLE_ARP_E,
    CPSS_DXCH_CFG_TABLE_TUNNEL_START_E,
    CPSS_DXCH_CFG_TABLE_STG_E,
    CPSS_DXCH_CFG_TABLE_QOS_PROFILE_E,
    CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E,
    CPSS_DXCH_CFG_TABLE_CNC_E,
    CPSS_DXCH_CFG_TABLE_CNC_BLOCK_E,
    CPSS_DXCH_CFG_TABLE_TRUNK_E,
    CPSS_DXCH_CFG_TABLE_LPM_RAM_E,
    CPSS_DXCH_CFG_TABLE_ROUTER_ECMP_E,
    CPSS_DXCH_CFG_TABLE_L2_MLL_LTT_E,
    CPSS_DXCH_CFG_TABLE_EPORT_E,
    CPSS_DXCH_CFG_TABLE_DEFAULT_EPORT_E,
    CPSS_DXCH_CFG_TABLE_PHYSICAL_PORT_E,

    CPSS_DXCH_CFG_TABLE_LAST_E /* must be last */
} CPSS_DXCH_CFG_TABLES_ENT;

/*
 * typedef: enum CPSS_DXCH_CFG_ROUTING_MODE_ENT
 *
 * Description: Routing mode
 *
 * Enumerations:
 *  CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E  - PBR (Policy based routing)
 *                                           using PCL action as next hop.
 *  CPSS_DXCH_TCAM_ROUTER_BASED_E          - Regular routing using router engine
 *                                           or PBR using next hop table.
*/
typedef enum
{
    CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E = 0,
    CPSS_DXCH_TCAM_ROUTER_BASED_E         = 1
}CPSS_DXCH_CFG_ROUTING_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT
 *
 * Description: how egress pipe get 6 bits hash from the ingress 12 bits hash. (EQ-->TXQ)
 *
 * Enumerations:
 *      CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_LSB_E -
 *                  Use 6 Lsbits
 *                  egressHash[5:0] = ingressHash[5:0]
 *
 *      CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_MSB_E -
 *                 Use 6 Msbits
 *                 egressHash[5:0] = ingressHash[11:6]
 *
 *      CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_12_BITS_E -
 *                  Use 12 bits; The 6 lsbits are XORed with the 6 msbits.
 *                  egressHash[5:0] = ingressHash[5:0] ^ ingressHash[11:6]
 *
 */
typedef enum {
    CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_LSB_E,
    CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_6_MSB_E,
    CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_12_BITS_E
}CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT
 *
 * Description: Ingress Drop Counter Mode types.
 *
 * Enumerations:
 *      CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ALL_E -
 *                  count all ingress dropped packets.
 *
 *      CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E -
 *                  Count all ingress dropped packets assigned  with
 *                  a specific VLAN id.
  *
 *      CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E -
 *                  Count all ingress dropped packets received
 *                  on a specific port
 *
 */
typedef enum {
    CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ALL_E,
    CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E,
    CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E
}CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT;

/*
 * typedef: struct CPSS_DXCH_PP_CONFIG_INIT_STC
 *
 * Description: DxCh Packet Processor (PP) init configuration Parameters
 *              structure
 *
 * Fields:
 *    maxNumOfVirtualRouters - max numbers of IPv4 virtual routers.
 *    maxNumOfIpNextHop      - max number of IPv4 and Ipv6 next hop entries in the
 *                             PP.
 *    maxNumOfIpv4Prefixes   - max number of IPv4 prefixes in the PP.
 *    maxNumOfIpv4McEntries  - max number of IPv4 and IPv6 Multicast Entries in the PP.
 *    maxNumOfMll            - max number of Downstream IPv4 and Ipv6 Multicast
 *                             interface entries (MLLs) in the PP.
 *    maxNumOfIpv6Prefixes   - max number of IPv6 prefixes in the PP.
 *    maxNumOfTunnelEntries  - max number of tunnel (of all types) entries
 *    maxNumOfIpv4TunnelTerms - the maximum number of ipv4 tunnel termination
 *                                  entries
 *    routingMode            - PP routing mode: Policy based routing Or TCAM based Routing
 *                             (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
 *                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
 *    maxNumOfPbrEntries     - max number of LPM leafs that can be allocated in
 *                             the RAM for policy based routing
 *                             (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *
 *    COMMENTS:
 */
typedef struct
{
    /*Lib init params. */

    /* IP routing mode*/
    CPSS_DXCH_CFG_ROUTING_MODE_ENT routingMode;

    /* LPM */
    GT_U32          maxNumOfPbrEntries;
} CPSS_DXCH_PP_CONFIG_INIT_STC;

/*
 * typedef: struct CPSS_DXCH_CFG_DEV_INFO_STC
 *
 * Description: DxCh device info structure
 *
 * Fields:
 *    genDevInfo - generic device information
 *
 */
typedef struct
{
    CPSS_GEN_CFG_DEV_INFO_STC    genDevInfo;
}CPSS_DXCH_CFG_DEV_INFO_STC;

/*
 * typedef: struct CPSS_DXCH_CFG_GLOBAL_EPORT_STC
 *
 * Description: DxCh structure for Global ePort range configuration.
 *
 * Fields:
 *    enable    - GT_TRUE :enable Global ePort configuration based on
 *                         the <pattern> & <mask> fields.
 *                GT_FALSE: no Global ePort configured. HW values of all 0's
 *                          and all 1's will be used for <mask> and < pattern>
 *                          respectively.
 *    pattern   - Global ePort range value. All bits which are not set to 1 in
 *                <mask> field must be cleared.
 *                (relevant only if enable == GT_TRUE).
 *    mask      - A bitmap that determines which bits in pattern are used for
 *                calculating Global ePort value.
 *                (relevant only if enable == GT_TRUE).
 */
typedef struct
{
    GT_BOOL enable;
    GT_U32  pattern;
    GT_U32  mask;
}CPSS_DXCH_CFG_GLOBAL_EPORT_STC;

/*******************************************************************************
* cpssDxChCfgDevRemove
*
* DESCRIPTION:
*
*       Remove the device from the CPSS.
*       This function we release the DB of CPSS that associated with the PP ,
*       and will "know" that the device is not longer valid.
*       This function will not do any HW actions , and therefore can be called
*       before or after the HW was actually removed.
*
*       After this function is called the devNum is no longer valid for any API
*       until this devNum is used in the initialization sequence for a "new" PP.
*
*       NOTE: 1. Application may want to disable the Traffic to the CPU , and
*             messages (AUQ) , prior to this operation (if the device still exists).
*             2. Application may want to a reset the HW PP , and there for need
*             to call the "hw reset" function (before calling this function)
*             and after calling this function to do the initialization
*             sequence all over again for the device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to remove.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevRemove
(
    IN GT_U8   devNum
);

/*******************************************************************************
* cpssDxChCfgDevEnable
*
* DESCRIPTION:
*       This routine sets the device state.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to set.
*       enable - GT_TRUE device enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChCfgDevEnableGet
*
* DESCRIPTION:
*       This routine gets the device state.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum - device number to set.
*
* OUTPUTS:
*       enablePtr - GT_TRUE device enable, GT_FALSE disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChCfgNextDevGet
*
* DESCRIPTION:
*
*       Return the number of the next existing device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to start from. For the first one devNum should be 0xFF.
*
* OUTPUTS:
*       nextDevNumPtr - number of next device after devNum.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NO_MORE               - devNum is the last device. nextDevNumPtr will be set to 0xFF.
*       GT_BAD_PARAM             - devNum > max device number
*       GT_BAD_PTR               - nextDevNumPtr pointer is NULL.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgNextDevGet
(
    IN  GT_U8 devNum,
    OUT GT_U8 *nextDevNumPtr
);

/*******************************************************************************
* cpssDxChCfgPpLogicalInit
*
* DESCRIPTION:
*       This function Performs PP RAMs divisions to memory pools, according to
*       the supported modules in system.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       ppConfigPtr - (pointer to)includes PP specific logical initialization
*                     params.
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_PP_MEM -  If the given configuration can't fit into the given
*                           PP's memory capabilities, or
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  This function should perform all PP initializations, which
*           includes:
*               -   Init the Driver's part (by calling driverPpInit()).
*               -   Ram structs init, and Ram size check.
*               -   Initialization of the interrupt mechanism data structure.
*       2.  When done, the function should leave the PP in disable state, and
*           forbid any packets forwarding, and any interrupts generation.
*       3.  The execution flow for preparing for the initialization of core
*           modules is described below:
*
*
*             +-----------------+
*             | Init the 3 RAMs |   This initialization includes all structs
*             | conf. structs   |   fields but the base address field.
*             |                 |
*             +-----------------+
*                     |
*   (Ram is big       |                 +-------------------------+
*    enough to hold   +--(No)-----------| return GT_OUT_OF_PP_MEM |
*    all tables?)     |                 +-------------------------+
*                     |
*                   (Yes)
*                     |
*                     V
*             +-------------------+
*             | Set the Rams base |
*             | addr. according   |
*             | to the location   |
*             | fields.           |
*             +-------------------+
*                     |
*                     |
*                     V
*             +----------------------------------+
*             | Init the                         |
*             | PRV_CPSS_DXCH_MODULE_CONFIG_STC  |
*             | struct, according                |
*             | to the info in RAM               |
*             | conf. struct.                    |
*             +----------------------------------+
*                     |
*                     |
*                     |
*                     V
*             +---------------------------------+
*             | set the prepared                |
*             | module Cfg struct               |
*             | in the appropriate              |
*             | PRV_CPSS_DXCH_PP_CONFIG_STC     |
*             | struct.                         |
*             +---------------------------------+
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgPpLogicalInit

(
    IN      GT_U8                           devNum,
    IN      CPSS_DXCH_PP_CONFIG_INIT_STC    *ppConfigPtr
);

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
*
* DESCRIPTION:
*       Sets device ID modification for Routed packets.
*       Enables/Disables FORWARD DSA tag modification of the <source device>
*       and <source port> fields of packets routed by the local device.
*       The <source device> is set to the local device ID and the <source port>
*       is set to 61 (the virtual router port).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - Device number.
*       portNum         - Port number (or CPU port)
*       modifyEnable    - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 modifyEnable
);

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet
*
* DESCRIPTION:
*       Gets device ID modification for Routed packets.
*       See Set Api comments.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - Device number.
*       portNum          - Port number (or CPU port)
*
* OUTPUTS:
*       modifyEnablePtr  - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *modifyEnablePtr
);

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortModifySet
*
* DESCRIPTION:
*       Enables/Disables Device ID Modification upon packet sending to another
*       stack unit.
*
*       When Connecting DxCh Devices to SOHO in a Value Blade model to enable
*       DxCh grade Capabilities for FE Ports, in a staking system, we must be
*       able to overcome the 32 devices limitation.
*       To do that, SOHO Device Numbers are not unique and packets received
*       from the SOHO by the DxCh and are relayed to other stack units
*       have their Device ID changed to the DxCh Device ID.
*       On Upstream (packet from SOHO to DxCh):
*           The SOHO Sends a packet to the DxCh and the packet is sent back
*           to the SOHO. In this case we don't want to change the Device ID in
*           the DSA Tag so that the SOHO is able to filter the Src Port and is
*           able to send back the packet to its source when doing cross chip
*           flow control.
*       On Downsteam (packet from SOHO to SOHO):
*           The SOHO receives a packet from another SOHO in this case we
*           want to change the Device ID so that the packet is not filtered.
*
*       On top of the above, for forwarding packets between the DxCh devices
*       and for Auto Learning, the Port Number must also be changed.
*       In addition Changing the Device ID is needed only for FORWARD DSA Tag.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*         Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - Device number.
*       modifedDsaSrcDev    - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortModifySet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     modifedDsaSrcDev
);

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortModifyGet
*
* DESCRIPTION:
*       Get status of Device ID Modification upon packet sending to another
*       stack unit. See Set Api comments.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*         Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum               - Device number.
*
* OUTPUTS:
*       modifedDsaSrcDevPtr  - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortModifyGet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     *modifedDsaSrcDevPtr
);

/*******************************************************************************
* cpssDxChCfgTableNumEntriesGet
*
*
* DESCRIPTION:
*       the function return the number of entries of each individual table in
*       the HW
*
*       when several type of entries like ARP and tunnelStart resize in the same
*       table (memory) , the function returns the number of entries for the least
*       denominator type of entry --> in this case number of ARP entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number
*       table        - type of the specific table
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgTableNumEntriesGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_CFG_TABLES_ENT    table,
    OUT GT_U32                      *numEntriesPtr
);

/*******************************************************************************
* cpssDxChCfgReNumberDevNum
*
* DESCRIPTION:
*       function allow the caller to modify the DB of the cpss ,
*       so all info that was 'attached' to 'oldDevNum' will be moved to 'newDevNum'.
*
*       NOTE:
*       1. it is the responsibility of application to update the 'devNum' of
*       HW entries from 'oldDevNum' to 'newDevNum' using appropriate API's
*       such as 'Trunk members','Fdb entries','NH entries','PCE entries'...
*       2. it's application responsibility to update the HW device number !
*          see API cpssDxChCfgHwDevNumSet
*       3. no HW operations involved in this API
*
*       NOTE:
*       this function MUST be called under 'Interrupts are locked' and under
*       'Tasks lock'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       oldDevNum  - old device number
*       newDevNum  - new device number (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - the device oldDevNum not exist
*       GT_OUT_OF_RANGE - the device newDevNum value > 0x1f (0..31)
*       GT_ALREADY_EXIST - the new device number is already used
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgReNumberDevNum
(
    IN GT_U8   oldDevNum,
    IN GT_U8   newDevNum
);

/*******************************************************************************
* cpssDxChCfgDevInfoGet
*
* DESCRIPTION:
*       the function returns device static information
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number


*
* OUTPUTS:
*       devInfoPtr   - (pointer to) device information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*

*******************************************************************************/
GT_STATUS cpssDxChCfgDevInfoGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_DXCH_CFG_DEV_INFO_STC   *devInfoPtr
);


/*******************************************************************************
* cpssDxChCfgBindPortPhymacObject
*
* DESCRIPTION:
*         The function binds port phymac pointer
*         to 1540M PHYMAC object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    portNum        - port number
*    macDrvObjPtr   - port phymac object pointer
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgBindPortPhymacObject(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_MACDRV_OBJ_STC * const macDrvObjPtr
);

/*******************************************************************************
* cpssDxChCfgIngressDropCntrModeSet
*
* DESCRIPTION:
*       Set the Ingress Drop Counter Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device number.
*       mode      - Ingress Drop Counter Mode.
*       portNum   - port number monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E
*                   counter mode.
*       vlan      - VLAN ID monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E
*                   counter mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or mode.
*       GT_OUT_OF_RANGE          - on wrong portNum or vlan.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrModeSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT mode,
    IN  GT_PORT_NUM                                 portNum,
    IN  GT_U16                                      vlan
);

/*******************************************************************************
* cpssDxChCfgIngressDropCntrModeGet
*
* DESCRIPTION:
*       Get the Ingress Drop Counter Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device number.
*
* OUTPUTS:
*       modePtr      - pointer to Ingress Drop Counter Mode.
*       portNumPtr   - pointer to port number monitored by Ingress Drop Counter.
*                      This parameter is applied upon
*                      CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E
*                      counter mode.
*       vlanPtr      - pointer to VLAN ID monitored by Ingress Drop Counter.
*                      This parameter is applied upon
*                      CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E
*                      counter mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - illegal state of configuration
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrModeGet
(
    IN   GT_U8                                       devNum,
    OUT  CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT *modePtr,
    OUT  GT_PORT_NUM                                 *portNumPtr,
    OUT  GT_U16                                      *vlanPtr
);


/*******************************************************************************
* cpssDxChCfgIngressDropCntrSet
*
* DESCRIPTION:
*       Set the Ingress Drop Counter value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - Device number.
*       counter     - Ingress Drop Counter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      counter
);


/*******************************************************************************
* cpssDxChCfgIngressDropCntrGet
*
* DESCRIPTION:
*       Get the Ingress Drop Counter value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device number.
*
* OUTPUTS:
*       counterPtr  - (pointer to) Ingress Drop Counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *counterPtr
);

/*******************************************************************************
* cpssDxChCfgPortDefaultSourceEportNumberSet
*
* DESCRIPTION:
*       Set Port default Source ePort number.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*       ePort         - ePort number
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
*       1. Default value is physical port number. In Multi-core
*          architecture it is the global physical port number (i.e. Local Core
*          Port number bitwise ORed with LocalCoreID<<4)
*       2. Can be overriden by TTI Action/RBridge
*
*******************************************************************************/
GT_STATUS cpssDxChCfgPortDefaultSourceEportNumberSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_PORT_NUM                         ePort
);

/*******************************************************************************
* cpssDxChCfgPortDefaultSourceEportNumberGet
*
* DESCRIPTION:
*       Get Port default Source ePort number.
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
*
* OUTPUTS:
*       ePortPtr      - pointer to ePort number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Default value is physical port number. In Multi-core
*          architecture it is the global physical port number (i.e. Local Core
*          Port number bitwise ORed with LocalCoreID<<4)
*       2. Can be overriden by TTI Action/RBridge
*
*******************************************************************************/
GT_STATUS cpssDxChCfgPortDefaultSourceEportNumberGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT GT_PORT_NUM                         *ePortPtr
);

/*******************************************************************************
* cpssDxChCfgEgressHashSelectionModeSet
*
* DESCRIPTION:
*       Set how egress pipe get 6 bits hash from the ingress 12 bits hash. (EQ-->TXQ)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       selectionMode       - hash selection mode
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
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCfgEgressHashSelectionModeSet
(
    IN GT_U8                                                        devNum,
    IN CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT    selectionMode
);

/*******************************************************************************
* cpssDxChCfgEgressHashSelectionModeGet
*
* DESCRIPTION:
*       Return how egress pipe get 6 bits hash from the ingress 12 bits hash. (EQ-->TXQ)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*
* OUTPUTS:
*       selectionModePtr        - (pointer to)hash selection mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgEgressHashSelectionModeGet
(
    IN  GT_U8                                                       devNum,
    OUT CPSS_DXCH_CFG_EGRESS_HASH_SELECTION_FROM_INGRESS_MODE_ENT   *selectionModePtr
);

/*******************************************************************************
* cpssDxChCfgGlobalEportSet
*
* DESCRIPTION:
*       Set Global ePorts ranges configuration.
*       Global ePorts are ePorts that are global in the entire system, and are
*       not unique per device as regular ePorts are.
*
*       Global ePorts are used for Representing:
*       1. a Trunk.(map target ePort to trunkId by the E2PHY)
*       2. a ePort ECMP Group.(map 'primary' target ePort to 'secondary' target ePort)
*       3. an ePort Replication Group (eVIDX).(map target ePort to eVIDX group)
*
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
*       globalPtr   - (pointer to) Global EPorts for the Bridge,L2Mll to use.
*                           In the Bridge Used for:
*                           a) Whether the bridge engine looks at the device number
*                              when performing MAC SA lookup or local switching check.
*                           b) That the local device ID (own device) is learned in the
*                              FDB entry with the global ePort
*
*                           In the L2MLL Used for:
*                           Enabling the L2MLL replication engine to ignore the
*                           device ID when comparing source and target interfaces
*                           for source filtering.
*                           Typically configured to include global ePorts
*                           representing a trunk or an ePort ECMP group
*
*                       NOTE: For defining the EPorts that used for 'Multi-Target ePort to eVIDX Mapping'
*                           use API cpssDxChL2MllMultiTargetPortSet(...)
*                           (see cpssDxChL2Mll.h file for more related functions)
*
*       l2EcmpPtr   - (pointer to) L2 ECMP Primary ePorts.
*                     Used for: The 'Primary ePorts' that need to be converted to
*                      one of their 'Secondary ePorts'.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgGlobalEportSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *globalPtr,
    IN  CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *l2EcmpPtr
);

/*******************************************************************************
* cpssDxChCfgGlobalEportGet
*
* DESCRIPTION:
*       Get Global ePorts ranges configuration.
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
*       globalPtr   - (pointer to) Global EPorts for the Bridge,L2MLL to use.
*       l2EcmpPtr   - (pointer to) L2 ECMP Primary ePorts.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgGlobalEportGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *globalPtr,
    OUT CPSS_DXCH_CFG_GLOBAL_EPORT_STC  *l2EcmpPtr
);

/*******************************************************************************
* cpssDxChCfgLearnOnRingPortEnableSet
*
* DESCRIPTION:
*      function enable/disable learning on 'Ring port'.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: Learning is disabled on ring ports.
*                               In addition NA messages are not send to the CPU
*                     GT_TRUE: Learning is enabled on ring ports
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChCfgLearnOnRingPortEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChCfgLearnOnRingPortEnableGet
*
* DESCRIPTION:
*      function gets whether learning is enabled/disabled on 'Ring port'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: Learning is disabled on ring ports.
*                               In addition NA messages are not send to the CPU
*                     GT_TRUE: Learning is enabled on ring ports
*
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or portNum
*    GT_BAD_PTR   - one of the parameters is NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChCfgLearnOnRingPortEnableGet
(
    IN  GT_U8                   devNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_READ_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines table entry read callback function for Hw access.
 *
 * INPUTS:
 *       devNum               - the device number
 *       convertedPortGroupId - the port group Id after conversion if needed, to support
 *                              multi-port-group devices that need to access specific port group
 *       tableType            - the specific table name
 *       convertedEntryIndex  - index in the table after conversion if needed.
 *       inPortGroupId        - original incoming port group.
 *       stage                - stage for callback run.
 *       status               - current status of parent CPSS caller.
 *
 * OUTPUTS:
 *       entryValuePtr - (pointer to) the data read from the table
 *                       may be NULL in the case of indirect table.
 *
 *
 * RETURNS:
 *       GT_OK         - callback success.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_READ_FUNC)
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  convertedPortGroupId,
    IN  PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN  GT_U32                  convertedEntryIndex,
    OUT GT_U32                 *entryValuePtr,
    IN  GT_U32                  inPortGroupId,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT    stage,
    IN  GT_STATUS                       status
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_WRITE_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines table entry read callback function for Hw access.
 *
 * INPUTS:
 *       devNum               - the device number
 *       convertedPortGroupId - the port group Id after conversion if needed, to support
 *                              multi-port-group devices that need to access specific port group
 *       tableType            - the specific table name
 *       convertedEntryIndex  - index in the table after conversion if needed.
 *       entryValuePtr        - (pointer to) the data that will be written to the table
 *                               may be NULL in the case of indirect table.
 *       inPortGroupId        - original incoming port group.
 *       stage                - stage for callback run.
 *       status               - current status of parent CPSS caller.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK         - callback success.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_WRITE_FUNC)
(
    IN GT_U8                   devNum,
    IN GT_U32                  convertedPortGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  convertedEntryIndex,
    IN GT_U32                 *entryValuePtr,
    IN GT_U32                 inPortGroupId,
    IN CPSS_DRV_HW_ACCESS_STAGE_ENT    stage,
    IN  GT_STATUS                       status
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_FIELD_READ_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines table entry field read callback function for Hw access.
 *
 * INPUTS:
 *       devNum                - the device number
 *       convertedPortGroupId  - the port group Id after conversion if needed, to support
 *                               multi-port-group devices that need to access specific port group
 *       tableType             - the specific table name
 *       convertedEntryIndex   - index in the table after conversion if needed.
 *       fieldWordNum          - field word number
 *                               use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
 *                               if need global offset in the field of fieldOffset
 *       fieldOffset           - field offset
 *       fieldLength           - field length
 *       inPortGroupId         - original incoming port group.
 *       stage                 - stage for callback run.
 *       status                - current status of parent CPSS caller.
 *
 * OUTPUTS:
 *       entryValuePtr - (pointer to) the data read from the table
 *                       may be NULL in the case of indirect table.
 *
 *
 * RETURNS:
 *       GT_OK         - callback success.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_FIELD_READ_FUNC)
(
    IN GT_U8                   devNum,
    IN GT_U32                  convertedPortGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  convertedEntryIndex,
    IN GT_U32                  fieldWordNum,
    IN GT_U32                  fieldOffset,
    IN GT_U32                  fieldLength,
    OUT GT_U32                *fieldValuePtr,
    IN GT_U32                  inPortGroupId,
    IN  CPSS_DRV_HW_ACCESS_STAGE_ENT    stage,
    IN  GT_STATUS                       status
);

/*******************************************************************************
 * typedef: GT_STATUS (*CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_FIELD_WRITE_FUNC)
 *
 *
 * DESCRIPTION:
 *    Defines table entry field write callback function for Hw access.
 *
 * INPUTS:
 *       devNum                - the device number
 *       convertedPortGroupId  - the port group Id after conversion if needed, to support
 *                               multi-port-group devices that need to access specific port group
 *       tableType             - the specific table name
 *       convertedEntryIndex   - index in the table after conversion if needed.
 *       fieldWordNum          - field word number
 *                               use PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS
 *                               if need global offset in the field of fieldOffset
 *       fieldOffset           - field offset
 *       fieldLength           - field length
 *       entryValuePtr         - (pointer to) the data that will be written to the table
 *                                may be NULL in the case of indirect table.
 *       inPortGroupId         - original incoming port group.
 *       stage                 - stage for callback run.
 *       status                - current status of parent CPSS caller.
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK         - callback success.
 *       GT_FAIL       - callback failed.
 *       GT_ABORTED    - operation aborted by calback.
 *
 * COMMENTS:
 *   When stage equals to CPSS_DRV_HW_ACCESS_STAGE_PRE_E:
 *      1. If callback returns GT_OK the parrent CPSS function continue processing.
 *      2. If callback returns GT_ABORTED, this indicates to the parent CPSS function to stop
 *          and return GT_OK to its caller.
 *
****************************************************************************** */
typedef GT_STATUS (*CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_FIELD_WRITE_FUNC)
(
    IN GT_U8                   devNum,
    IN GT_U32                  convertedPortGroupId,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32                  convertedEntryIndex,
    IN GT_U32                 fieldWordNum,
    IN GT_U32                 fieldOffset,
    IN GT_U32                 fieldLength,
    IN GT_U32                 fieldValue,
    IN GT_U32                 inPortGroupId,
    IN CPSS_DRV_HW_ACCESS_STAGE_ENT    stage,
    IN GT_STATUS                       status
);


/*
 * typedef: struct CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC
 *
 * Description: the structure holds the tables access callback routines.
 *
 * Fields:
 *    hwAccessTableEntryReadFunc  - table entry read callback function
 *    hwAccessTableEntryWriteFunc - table entry write callback function
 *    hwAccessTableEntryFieldReadFunc  - table entry field read callback function
 *    hwAccessTableEntryFieldWriteFunc - table entry field write callback function
 *
 *  COMMENTS:
 *  ----------------------------------------------------------------------------------------
 *             CPSS Function                                Hw access Callback
 *------------------------------------------------------------------------------------------
 *  1   prvCpssDxChPortGroupReadTableEntry              hwAccessTableEntryReadFunc;
 *  2   prvCpssDxChPortGroupWriteTableEntry              hwAccessTableEntryWriteFunc;
 *  3   prvCpssDxChPortGroupReadTableEntryField        hwAccessTableEntryFieldReadFunc;
 *  4   prvCpssDxChPortGroupWriteTableEntryField       hwAccessTableEntryFieldWriteFunc
 *---------------------------------------------------------------------------------------------
 */
typedef struct{
    CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_READ_FUNC        hwAccessTableEntryReadFunc;       /* 1 */
    CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_WRITE_FUNC       hwAccessTableEntryWriteFunc;      /* 2 */
    CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_FIELD_READ_FUNC  hwAccessTableEntryFieldReadFunc;  /* 3 */
    CPSS_DXCH_CFG_ACCESS_PORT_GROUP_TABLE_ENTRY_FIELD_WRITE_FUNC hwAccessTableEntryFieldWriteFunc; /* 4 */
} CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC;

/*******************************************************************************
* cpssDxChCfgHwAccessObjectBind
*
* DESCRIPTION:
*         The function binds/unbinds a callback routines for HW access.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    cfgAccessObjPtr - HW access object pointer.
*    bind            -  GT_TRUE - bind callback routines.
*                      GT_FALSE - unbind callback routines.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgHwAccessObjectBind
(
    IN CPSS_DXCH_CFG_HW_ACCESS_OBJ_STC *cfgAccessObjPtr,
    IN GT_BOOL                     bind
);

/*******************************************************************************
* cpssDxChCfgHitlessWriteMethodEnableSet
*
* DESCRIPTION:
*       Enable/disable hitless write methood.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE:  hitless write methood is enable. In this case write operation
*                                 would be performed only if writing data is not equal with
*                                 affected memory contents.
*                       GT_FALSE: hitless write methood is disable. In this case write operation
*                                 is done at any rate.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgHitlessWriteMethodEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_BOOL                             enable
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChCfgInith */

