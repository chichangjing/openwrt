/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgCount.c
*
* DESCRIPTION:
*       CPSS DxCh Ingress Bridge Counters facility implementation.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/

#ifndef __cpssDxChBrgCounth
#define __cpssDxChBrgCounth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgGen.h>

/*
 * typedef: enum CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT
 *
 * Description: Enumeration of Bridge Drop Counter reasons to be counted.
 *
 * Enumerations:
 *  #######################################################################
 *  ##  The Drop Counter Modes below are supported by all DXCh devices:  ##
 *  #######################################################################
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E           - Count All.
 *    CPSS_DXCH_BRG_DROP_CNTR_FDB_ENTRY_CMD_E       - FDB Entry command Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E      - Unknown MAC SA Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_INVALID_SA_E          - Invalid SA Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_INVALID_VLAN_E        - VLAN not valid Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E    - Port not Member in VLAN
 *                                                    Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_VLAN_RANGE_E          - VLAN Range Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E   - Moved Static Address Drop.
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E          - Rate Limiting Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_LOCAL_PORT_E          - Local Port Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E   - Spanning Tree port state
 *                                                    Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_IP_MC_E               - IP Multicast Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_NON_IP_MC_E           - NON-IP Multicast Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E    - DSATag due to Local dev
 *                                                    Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_IEEE_RESERVED_E       - IEEE Reserved Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E  - Unregistered L2 NON-IPM
 *                                                    Multicast Drop.
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E     - Unregistered L2 IPv6
 *                                                    Multicast Drop.
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E     - Unregistered L2 IPv4
 *                                                    Multicast Drop.
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_L2_UC_E       - Unknown L2 Unicast Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E     - Unregistered L2 IPv4
 *                                                    Brodcast Drop.
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E - Unregistered L2 NON-IPv4
 *                                                    Brodcast Drop.
 *
 *  ###########################################################################
 *  ##  The following Drop Counter Modes are not supported by Dx Ch device.  ##
 *  ###########################################################################
 *  -----------------------------------------------------------------------
 *    CPSS_DXCH_BRG_DROP_CNTR_ARP_SA_MISMATCH_E     - ARP SA mismatch Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_SYN_WITH_DATA_E       - SYN with data Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E   - TCP over MCAST or BCAST
 *                                                    Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_ACCESS_MATRIX_E       - Bridge Access Matrix Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_SEC_LEARNING_E        - Secure Learning Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E   - Acceptable Frame Type
 *                                                    Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_FRAG_ICMP_E           - Fragmented ICMP Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E      - TCP Flags Zero Drop.
 *    CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E   - TCP Flags FIN, URG and PSH
 *                                                    are all set Drop.
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E    - TCP Flags SYN and FIN are
 *                                                    set Drop.
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E    - TCP Flags SYN and RST are
 *                                                    set Drop.
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E   - TCP/UDP Source or
 *                                                    Destination Port is Zero
 *                                                    Drop.
 *
 *    CPSS_DXCH_BRG_DROP_CNTR_VLAN_MRU_E            - VLAN MRU Drop.
 */
typedef enum
{
    CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E = 0,                /* 0  */
    CPSS_DXCH_BRG_DROP_CNTR_FDB_ENTRY_CMD_E,                /* 1  */
    CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E,               /* 2  */
    CPSS_DXCH_BRG_DROP_CNTR_INVALID_SA_E,                   /* 3  */
    CPSS_DXCH_BRG_DROP_CNTR_INVALID_VLAN_E,                 /* 4  */
    CPSS_DXCH_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E,             /* 5  */
    CPSS_DXCH_BRG_DROP_CNTR_VLAN_RANGE_E,                   /* 6  */
    CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E,            /* 7  */

    /**************************************************************/
    /* List of Drop Modes not supported by the Dx Ch device       */
    CPSS_DXCH_BRG_DROP_CNTR_ARP_SA_MISMATCH_E,              /* 8  */
    CPSS_DXCH_BRG_DROP_CNTR_SYN_WITH_DATA_E,                /* 9  */
    CPSS_DXCH_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E,            /* 10 */
    CPSS_DXCH_BRG_DROP_CNTR_ACCESS_MATRIX_E,                /* 11 */
    CPSS_DXCH_BRG_DROP_CNTR_SEC_LEARNING_E,                 /* 12 */
    CPSS_DXCH_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E,            /* 13 */
    CPSS_DXCH_BRG_DROP_CNTR_FRAG_ICMP_E,                    /* 14 */
    CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E,               /* 15 */
    CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E,            /* 16 */
    CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E,             /* 17 */
    CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E,             /* 18 */
    CPSS_DXCH_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E,            /* 19 */
    CPSS_DXCH_BRG_DROP_CNTR_VLAN_MRU_E,                     /* 20 */
    /* End of Drop Modes List not supported by the Dx Ch device   */
    /**************************************************************/

    CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E,                   /* 21 */
    CPSS_DXCH_BRG_DROP_CNTR_LOCAL_PORT_E,                   /* 22 */
    CPSS_DXCH_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E,            /* 23 */
    CPSS_DXCH_BRG_DROP_CNTR_IP_MC_E,                        /* 24 */
    CPSS_DXCH_BRG_DROP_CNTR_NON_IP_MC_E,                    /* 25 */
    CPSS_DXCH_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E,             /* 26 */
    CPSS_DXCH_BRG_DROP_CNTR_IEEE_RESERVED_E,                /* 27 */
    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E,           /* 28 */
    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E,              /* 29 */
    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E,              /* 30 */
    CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_L2_UC_E,                /* 31 */
    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E,              /* 32 */
    CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E           /* 33 */
} CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT;


/*
 * typedef: enum CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT
 *
 * Description: Enumeration of counter set number.
 *
 * Enumerations:
 *    CPSS_DXCH_BRG_CNT_SET_ID_0_E  - id for counter set 0
 *    CPSS_DXCH_BRG_CNT_SET_ID_1_E  - id for counter set 1
 *
 */
typedef enum
{
    CPSS_DXCH_BRG_CNT_SET_ID_0_E = 0,
    CPSS_DXCH_BRG_CNT_SET_ID_1_E
} CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT;

/*
 * typedef: struct CPSS_DXCH_BRIDGE_HOST_CNTR_STC
 *
 * Description: Structure of Host Group counters is maintained for a CPU-
 *              configured MAC source address and MAC destination address.
 *
 * Fields:
 *   gtHostInPkts           - number of paskets (good packets only) with a
 *                            MAC DA matching the CPU-configured MAC DA.
 *   gtHostOutPkts          - number of paskets (good packets only) with a
 *                            MAC SA matching the CPU-configured MAC SA.
 *   gtHostOutBroadcastPkts - number of Broadcast paskets (good packets only)
 *                            with a MAC SA matching the CPU-configured MAC SA.
 *   gtHostOutMulticastPkts - number of Multicast paskets (good packets only)
 *                            with a MAC SA matching the CPU-configured MAC SA.
 *
 * Note:
 *   The "Good Packets" are error-free Ethernet packetdfs
 *   that have a valid frame length, per RFC 2819.
 */
typedef struct
{
    GT_U32 gtHostInPkts;
    GT_U32 gtHostOutPkts;
    GT_U32 gtHostOutBroadcastPkts;
    GT_U32 gtHostOutMulticastPkts;
} CPSS_DXCH_BRIDGE_HOST_CNTR_STC;

/*******************************************************************************
* cpssDxChBrgCntDropCntrModeSet
*
* DESCRIPTION:
*       Sets Drop Counter Mode (configures a Bridge Drop Counter "reason").
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number.
*       dropMode  - Drop Counter mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum or dropMode
*       GT_NOT_SUPPORTED         - on non-supported dropMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntDropCntrModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT dropMode
);

/*******************************************************************************
* cpssDxChBrgCntDropCntrModeGet
*
* DESCRIPTION:
*       Gets the Drop Counter Mode (Bridge Drop Counter "reason").
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*
* OUTPUTS:
*       dropModePtr - pointer to the Drop Counter mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_SUPPORTED         - on non-supported dropMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntDropCntrModeGet
(
    IN   GT_U8                                   devNum,
    OUT  CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT     *dropModePtr
);

/*******************************************************************************
* cpssDxChBrgCntMacDaSaSet
*
* DESCRIPTION:
*       Sets a specific MAC DA and SA to be monitored by Host
*       and Matrix counter groups on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       saAddrPtr   - source MAC address (MAC SA).
*       daAddrPtr   - destination MAC address (MAC DA).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntMacDaSaSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *saAddrPtr,
    IN  GT_ETHERADDR    *daAddrPtr
);

/*******************************************************************************
* cpssDxChBrgCntMacDaSaGet
*
* DESCRIPTION:
*       Gets a MAC DA and SA are monitored by Host
*       and Matrix counter groups on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*
* OUTPUTS:
*       saAddrPtr   - source MAC address (MAC SA).
*       daAddrPtr   - destination MAC address (MAC DA).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntMacDaSaGet
(
    IN   GT_U8           devNum,
    OUT  GT_ETHERADDR    *saAddrPtr,
    OUT  GT_ETHERADDR    *daAddrPtr
);

/*******************************************************************************
* cpssDxChBrgCntBridgeIngressCntrModeSet
*
* DESCRIPTION:
*       Configures a specified Set of Bridge Ingress
*       counters to work in requested mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       cntrSetId - Counter Set ID
*       setMode   - count mode of specified Set of Bridge Ingress Counters.
*       port      - port number monitored by Set of Counters.
*                   This parameter is applied upon CPSS_BRG_CNT_MODE_1_E and
*                   CPSS_BRG_CNT_MODE_3_E counter modes.
*       vlan      - VLAN ID monitored by Counters Set.
*                   This parameter is applied upon CPSS_BRG_CNT_MODE_2_E and
*                   CPSS_BRG_CNT_MODE_3_E counter modes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum, setMode or cntrSetId.
*       GT_OUT_OF_RANGE          - on port number or vlan out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntBridgeIngressCntrModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    IN  CPSS_BRIDGE_INGR_CNTR_MODES_ENT     setMode,
    IN  GT_PORT_NUM                         port,
    IN  GT_U16                              vlan
);

/*******************************************************************************
* cpssDxChBrgCntBridgeIngressCntrModeGet
*
* DESCRIPTION:
*       Gets the mode (port number and VLAN Id as well) of specified
*       Bridge Ingress counters Set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       cntrSetId - Counter Set ID
*
* OUTPUTS:
*       setModePtr   - pointer to the count mode of specified Set of Bridge
*                      Ingress Counters.
*       portPtr      - pointer to the port number monitored by Set of Counters.
*                      This parameter is applied upon CPSS_BRG_CNT_MODE_1_E and
*                      CPSS_BRG_CNT_MODE_3_E counter modes.
*                      This parameter can be NULL;
*       vlanPtr      - pointer to the VLAN ID monitored by Counters Set.
*                      This parameter is applied upon CPSS_BRG_CNT_MODE_2_E and
*                      CPSS_BRG_CNT_MODE_3_E counter modes.
*                      This parameter can be NULL;
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum or counter set number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntBridgeIngressCntrModeGet
(
    IN   GT_U8                               devNum,
    IN   CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT  CPSS_BRIDGE_INGR_CNTR_MODES_ENT     *setModePtr,
    OUT  GT_PORT_NUM                         *portPtr,
    OUT  GT_U16                              *vlanPtr
);

/*******************************************************************************
* cpssDxChBrgCntDropCntrGet
*
* DESCRIPTION:
*       Gets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*
* OUTPUTS:
*       dropCntPtr  - pointer to the number of packets that were dropped
*                     due to drop reason configured
*                     by the cpssDxChBrgCntDropCntrModeSet().
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntDropCntrGet
(
    IN   GT_U8      devNum,
    OUT  GT_U32     *dropCntPtr
);

/*******************************************************************************
* cpssDxChBrgCntDropCntrSet
*
* DESCRIPTION:
*       Sets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number.
*       dropCnt  - number of packets that were dropped due to drop reason
*                  configured by the cpssDxChBrgCntDropCntrModeSet().
*                  The parameter enables an application to initialize the counter,
*                  for the desired counter value.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM - on wrong devNum or invalid Drop Counter Value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntDropCntrSet
(
    IN  GT_U8      devNum,
    IN  GT_U32     dropCnt
);

/*******************************************************************************
* cpssDxChBrgCntHostGroupCntrsGet
*
* DESCRIPTION:
*       Gets Bridge Host group counters value of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       hostGroupCntPtr - structure with current counters value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Host group of counters are clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntHostGroupCntrsGet
(
    IN   GT_U8                              devNum,
    OUT  CPSS_DXCH_BRIDGE_HOST_CNTR_STC     *hostGroupCntPtr
);

/*******************************************************************************
* cpssDxChBrgCntMatrixGroupCntrsGet
*
* DESCRIPTION:
*       Gets Bridge Matrix counter value of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                - physical device number.
*
* OUTPUTS:
*       matrixCntSaDaPktsPtr  - number of packets (good only) with a MAC SA/DA
*                               matching of the CPU-configured MAC SA/DA.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Matrix counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntMatrixGroupCntrsGet
(
    IN   GT_U8      devNum,
    OUT  GT_U32     *matrixCntSaDaPktsPtr
);

/*******************************************************************************
* cpssDxChBrgCntBridgeIngressCntrsGet
*
* DESCRIPTION:
*       Gets a Bridge ingress Port/VLAN/Device counters from
*       specified counter set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSetId       - counter set number.
*
* OUTPUTS:
*       ingressCntrPtr  - structure of bridge ingress counters current values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum, on counter set number
*                         that is out of range of [0-1]
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Ingress group of counters are clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntBridgeIngressCntrsGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT CPSS_BRIDGE_INGRESS_CNTR_STC        *ingressCntrPtr
);

/*******************************************************************************
* cpssDxChBrgCntLearnedEntryDiscGet
*
* DESCRIPTION:
*       Gets the total number of source addresses the were
*       not learned due to bridge internal congestion.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       countValuePtr   - The value of the counter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hw error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntLearnedEntryDiscGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *countValuePtr
);

/*******************************************************************************
* cpssDxChBrgCntPortGroupDropCntrGet
*
* DESCRIPTION:
*       Gets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
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
*       dropCntPtr  - pointer to the number of packets that were dropped
*                     due to drop reason configured
*                     by the cpssDxChBrgCntDropCntrModeSet().
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupDropCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT  GT_U32              *dropCntPtr
);

/*******************************************************************************
* cpssDxChBrgCntPortGroupDropCntrSet
*
* DESCRIPTION:
*       Sets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       dropCnt  - number of packets that were dropped due to drop reason
*                  configured by the cpssDxChBrgCntDropCntrModeSet().
*                  The parameter enables an application to initialize the counter,
*                  for the desired counter value.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM - on wrong devNum or invalid Drop Counter Value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupDropCntrSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN  GT_U32               dropCnt
);

/*******************************************************************************
* cpssDxChBrgCntPortGroupHostGroupCntrsGet
*
* DESCRIPTION:
*       Gets Bridge Host group counters value of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number.
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
*       hostGroupCntPtr - structure with current counters value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Host group of counters are clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupHostGroupCntrsGet
(
    IN   GT_U8                              devNum,
    IN   GT_PORT_GROUPS_BMP                 portGroupsBmp,
    OUT  CPSS_DXCH_BRIDGE_HOST_CNTR_STC     *hostGroupCntPtr
);

/*******************************************************************************
* cpssDxChBrgCntPortGroupMatrixGroupCntrsGet
*
* DESCRIPTION:
*       Gets Bridge Matrix counter value of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number.
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
*       matrixCntSaDaPktsPtr  - number of packets (good only) with a MAC SA/DA
*                               matching of the CPU-configured MAC SA/DA.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Matrix counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupMatrixGroupCntrsGet
(
    IN   GT_U8                     devNum,
    IN   GT_PORT_GROUPS_BMP        portGroupsBmp,
    OUT  GT_U32                    *matrixCntSaDaPktsPtr
);

/*******************************************************************************
* cpssDxChBrgCntPortGroupBridgeIngressCntrsGet
*
* DESCRIPTION:
*       Gets a Bridge ingress Port/VLAN/Device counters from
*       specified counter set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       cntrSetId       - counter set number.
*
* OUTPUTS:
*       ingressCntrPtr  - structure of bridge ingress counters current values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum, on counter set number
*                         that is out of range of [0-1]
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Bridge Ingress group of counters are clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupBridgeIngressCntrsGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT CPSS_BRIDGE_INGRESS_CNTR_STC        *ingressCntrPtr
);

/*******************************************************************************
* cpssDxChBrgCntPortGroupLearnedEntryDiscGet
*
* DESCRIPTION:
*       Gets the total number of source addresses the were
*       not learned due to bridge internal congestion.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portGroupsBmp   - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       countValuePtr   - The value of the counter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hw error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgCntPortGroupLearnedEntryDiscGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *countValuePtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgCounth */
