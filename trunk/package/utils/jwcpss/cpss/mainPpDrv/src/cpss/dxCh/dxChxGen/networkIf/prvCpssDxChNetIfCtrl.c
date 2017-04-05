/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChNetIf.c
*
* DESCRIPTION:
*       Include private DxCh network interface API functions
*
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>

/*******************************************************************************
* prvCpssDxChNetIfCpuToDsaCode
*
* DESCRIPTION:
*       Convert the CPU code to the DSA CPU code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       cpuCode      - CPU code.
*
* OUTPUTS:
*       dsaCpuCodePtr - DSA CPU code
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - None DSA code for this CPU code.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not all cpu codes translation implemented
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfCpuToDsaCode
(
    IN  CPSS_NET_RX_CPU_CODE_ENT    cpuCode,
    OUT PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT *dsaCpuCodePtr
)
{

    if ((cpuCode >= CPSS_NET_FIRST_USER_DEFINED_E)
        && (cpuCode <= CPSS_NET_LAST_USER_DEFINED_E))
    {
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_FIRST_USER_DEFINED_E +
                        ((cpuCode - CPSS_NET_FIRST_USER_DEFINED_E) % 64);
    }
    else
    {
        switch (cpuCode)
        {
            case CPSS_NET_ETH_BRIDGED_LLT_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_REGULAR_FORWARD_E;
                break;
            case CPSS_NET_CONTROL_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_CONTROL_TO_CPU_E;
                break;
            case CPSS_NET_CONTROL_BPDU_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BPDU_E;
                break;
            case CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_MAC_ADDR_TRAP_E;
                break;
                /* CONTROL_MAC_RANGE_TRAP_E is not supported by Cheetah */
                /* code used as default for PCL TRAP */
            case CPSS_NET_CLASS_KEY_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_MAC_RANGE_TRAP_E;
                break;
            case CPSS_NET_INTERVENTION_ARP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_ARP_BROADCAST_E;
                break;
            case CPSS_NET_INTERVENTION_IGMP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IGMP_PACKET_E;
                break;
            case CPSS_NET_INTERVENTION_SA_DA_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_INTERVENTION_MAC_ADDR_E;
                break;
            case CPSS_NET_INTERVENTION_PORT_LOCK_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UNK_SRC_MAC_ADDR_E;
                break;
            case CPSS_NET_CLASS_KEY_MIRROR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_MAC_RANGE_TO_CPU_E;
                break;
            case CPSS_NET_LOCK_PORT_MIRROR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_LOCK_TO_CPU_E;
                break;
            case CPSS_NET_UNKNOWN_UC_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_E;
                break;
            case CPSS_NET_UNREG_MC_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UNREGISTERED_MULTICAST_E;
                break;
            case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_E;
                break;
            case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_1_E;
                break;
            case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_2_E;
                break;
            case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_3_E;
                break;
            case CPSS_NET_IPV6_ICMP_PACKET_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_ICMP_PACKET_E;
                break;
            case CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_IPV4_RIPV1_TO_CPU_E;
                break;
            case CPSS_NET_IPV6_NEIGHBOR_SOLICITATION_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_NEIGHBOR_SOLICITATION_E;
                break;
            case CPSS_NET_IPV4_BROADCAST_PACKET_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_BROADCAST_PACKET_E;
                break;
            case CPSS_NET_NON_IPV4_BROADCAST_PACKET_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_NON_IPV4_BROADCAST_PACKET_E;
                break;
            case CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_CISCO_MCAST_MAC_RANGE_E;
                break;
            case CPSS_NET_UNREGISTERED_MULTICAST_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BRG_UNREGD_MCAST_E;
                break;
            case CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV4_UNREG_MCAST_E;
                break;
            case CPSS_NET_UNKNOWN_UNICAST_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_EXT_E;
                break;
            case CPSS_NET_UDP_BC_MIRROR_TRAP0_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP0_E;
                break;
            case CPSS_NET_UDP_BC_MIRROR_TRAP1_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP1_E;
                break;
            case CPSS_NET_UDP_BC_MIRROR_TRAP2_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP2_E;
                break;
            case CPSS_NET_UDP_BC_MIRROR_TRAP3_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP3_E;
                break;

            case CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV6_UNREG_MCAST_E;
                break;
            case CPSS_NET_TARGET_NULL_PORT_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TARGET_NULL_PORT_E;
                break;
            case CPSS_NET_TARGET_EPORT_MTU_EXCEEDED_OR_RX_PROTECTION_SWITCHING_OR_INGRESS_FORWARDING_RESTRICTIONS_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TARGET_EPORT_MTU_EXCEEDED_OR_RX_PROTECTION_SWITCHING_OR_INGRESS_FORWARDING_RESTRICTIONS_E;
                break;
            case CPSS_NET_ROUTED_PACKET_FORWARD_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_ROUTED_PACKET_FORWARD_E;
                break;
            case CPSS_NET_BRIDGED_PACKET_FORWARD_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BRIDGED_PACKET_FRW_E;
                break;
            case CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_INGRESS_MIRRORED_TO_ANALYZER_E;
                break;
            case CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_EGRESS_MIRRORED_TO_ANALYZER_E;
                break;
            case CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_MAIL_FROM_NEIGHBOR_CPU_E;
                break;
            case CPSS_NET_CPU_TO_CPU_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_CPU_TO_CPU_E;
                break;
            case CPSS_NET_EGRESS_SAMPLED_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_EGRESS_SAMPLED_E;
                break;
            case CPSS_NET_INGRESS_SAMPLED_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_INGRESS_SAMPLED_E;
                break;
            case CPSS_NET_INVALID_PCL_KEY_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_INVALID_PCL_KEY_TRAP_E;
                break;
            case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
                break;
            case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;
                break;
            case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;
                break;
            case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E;
                break;
            case CPSS_NET_IP_ZERO_TTL_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_TTL_EXCEEDED_E;
                break;
            case CPSS_NET_IPV6_TTL_TRAP_E :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_HOP_LIMIT_EXCEEDED_E;
                break;
            case CPSS_NET_IP_HDR_ERROR_E :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_HEADER_ERROR_E;
                break;
            case CPSS_NET_IPV6_BAD_HEADER_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_HEADER_ERROR_E;
                break;
            case CPSS_NET_OPTIONS_IN_IP_HDR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_OPTIONS_E;
                break;
            case CPSS_NET_IPV6_HOP_BY_HOP_E :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_OPTIONS_E;
                break;
            case CPSS_NET_ROUTE_ENTRY_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE0_E;
                break;
            case CPSS_NET_IPV6_ROUTE_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE0_E;
                break;
            case CPSS_NET_IP_MTU_EXCEED_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MTU_EXCEEDED_E;
                break;
            case CPSS_NET_ILLEGAL_DIP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_ILLEGAL_ADDR_ERROR_E;
                break;
            case CPSS_NET_IP_DIP_DA_MISMATCH_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_DIP_DA_MISMATCH_E;
                break;
            case CPSS_NET_IP_UC_SIP_SA_MISMATCH_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_UC_SIP_SA_MISMATCH_E;
                break;
            case CPSS_NET_IPV6_EXT_HEADER_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_NON_HBH_OPTION_E;
                break;
            case CPSS_NET_IPV4_6_SIP_FILTERING_E                        :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_SIP_FILTERING_E;
                break;
            case CPSS_NET_IPV4_6_SIP_IS_ZERO_E                          :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_SIP_IS_ZERO_E;
                break;
            case CPSS_NET_ACCESS_MATRIX_E                               :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_ACCESS_MATRIX_E;
                break;
            case CPSS_NET_FCOE_DIP_LOOKUP_NOT_FOUND_OR_FCOE_EXCEPTION_E :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_FCOE_DIP_LOOKUP_NOT_FOUND_OR_FCOE_EXCEPTION_E;
                break;
            case CPSS_NET_FCOE_SIP_NOT_FOUND_E                          :
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_FCOE_SIP_NOT_FOUND_E;
                break;
            case CPSS_NET_IPV6_SCOP_FAIL_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_SCOPE_E;
                break;
            case CPSS_NET_IPV4_UC_ROUTE1_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE1_E;
                break;
            case CPSS_NET_IPV4_UC_ROUTE2_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE2_E;
                break;
            case CPSS_NET_IPV4_UC_ROUTE3_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE3_E;
                break;
            case CPSS_NET_IPV4_MC_ROUTE0_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE0_E;
                break;
            case CPSS_NET_IPV4_MC_ROUTE1_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE1_E;
                break;
            case CPSS_NET_IPV4_MC_ROUTE2_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE2_E;
                break;
            case CPSS_NET_IPV4_MC_ROUTE3_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE3_E;
                break;
            case CPSS_NET_IPV6_UC_ROUTE1_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE1_E;
                break;
            case CPSS_NET_IPV6_UC_ROUTE2_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE2_E;
                break;
            case CPSS_NET_IPV6_UC_ROUTE3_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE3_E;
                break;
            case CPSS_NET_IPV6_MC_ROUTE0_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE0_E;
                break;
            case CPSS_NET_IPV6_MC_ROUTE1_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE1_E;
                break;
            case CPSS_NET_IPV6_MC_ROUTE2_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE2_E;
                break;
            case CPSS_NET_IPV6_MC_ROUTE3_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE3_E;
                break;
            case CPSS_NET_IP_UC_RPF_FAIL_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_UC_RPF_FAIL_E;
                break;
            case CPSS_NET_RPF_CHECK_FAILED_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MC_ROUTE_RPF_FAIL_E;
                break;
            case CPSS_NET_MLL_RPF_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MC_MLL_RPF_FAIL_E;
                break;
            case CPSS_NET_ARP_BC_TO_ME_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_ARP_BC_TO_ME_E;
                break;
            case CPSS_NET_MIRROR_IPV4_UC_ICMP_REDIRECT_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ICMP_REDIRECT_E;
                break;
            case CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ICMP_REDIRECT_E;
                break;
            case CPSS_NET_IP_MC_ROUTE_BIDIR_RPF_FAIL_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IP_MC_ROUTE_BIDIR_RPF_FAIL_E;
                break;
            case CPSS_NET_MET_RED_DROP_CODE_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_MET_RED_DROP_CODE_E;
                break;
            case CPSS_NET_ECC_DROP_CODE_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_ECC_DROP_CODE_E;
                break;
            case CPSS_NET_PACKET_TO_VIRTUAL_ROUTER_PORT_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_PACKET_TO_VIRTUAL_ROUTER_PORT_E;
                break;
            case CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_SEC_AUTO_LEARN_UNK_SRC_TRAP_E;
                break;
            case CPSS_NET_RXDMA_DROP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_RXDMA_DROP_E;
                break;
            case CPSS_NET_IPV4_TT_HEADER_ERROR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_HEADER_ERROR_E;
                break;
            case CPSS_NET_IPV4_TT_OPTION_FRAG_ERROR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_OPTION_FRAG_ERROR_E;
                break;
            case CPSS_NET_IPV4_TT_UNSUP_GRE_ERROR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_UNSUP_GRE_ERROR_E;
                break;
            case CPSS_NET_BAD_ANALYZER_INDEX_DROP_ERROR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BAD_ANALYZER_INDEX_DROP_ERROR_E;
                break;
            case CPSS_NET_TT_MPLS_HEADER_CHECK_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TT_MPLS_HEADER_CHECK_E;
                break;
            case CPSS_NET_TT_MPLS_TTL_EXCEED_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TT_MPLS_TTL_EXCEED_E;
                break;
            case CPSS_NET_UNTAGGED_OVERSIZE_DROP_CODE_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UNTAGGED_OVERSIZE_DROP_CODE_E;
                break;
            case CPSS_NET_IPV6_TT_UNSUP_GRE_ERROR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_TT_UNSUP_GRE_ERROR_E;
                break;
            case CPSS_NET_OAM_PDU_TRAP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_OAM_PDU_TRAP_E;
                break;

            case CPSS_NET_ARP_REPLY_TO_ME_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_ARP_REPLY_TO_ME_E;
                break;
            case CPSS_NET_CPU_TO_ALL_CPUS_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_CPU_TO_ALL_CPUS_E;
                break;
            case CPSS_NET_TCP_SYN_TO_CPU_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TCP_SYN_TO_CPU_E;
                break;
            case CPSS_NET_PTP_HEADER_ERROR_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_PTP_HEADER_ERROR_E;
                break;
            case CPSS_NET_CCFC_PKT_DROP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_CCFC_PKT_DROP_E;
                break;
            case CPSS_NET_BAD_FC_PKT_DROP_E:
                *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BAD_FC_PKT_DROP_E;
                break;

            default:

                /* non-converted DSA opcode -->
                   do conversion to the special range */

                if(cpuCode >= CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E &&
                   cpuCode <= CPSS_NET_LAST_UNKNOWN_HW_CPU_CODE_E )
                {
                    *dsaCpuCodePtr =  (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)
                            (cpuCode - CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E);
                }
                else
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfDsaToCpuCode
*
* DESCRIPTION:
*       Convert the DSA CPU code to the CPU code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dsaCpuCode - DSA CPU code
*
* OUTPUTS:
*       cpuCodePtr - CPU code.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - None DSA code for this CPU code.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not all cpu codes translation implemented
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfDsaToCpuCode
(
    IN  PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT    dsaCpuCode,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *cpuCodePtr
)
{
    if ((dsaCpuCode >= PRV_CPSS_DXCH_NET_DSA_TAG_FIRST_USER_DEFINED_E) &&
       (dsaCpuCode <= PRV_CPSS_DXCH_NET_DSA_TAG_LAST_USER_DEFINED_E))
    {
        *cpuCodePtr = (CPSS_NET_RX_CPU_CODE_ENT)(CPSS_NET_FIRST_USER_DEFINED_E +
                (dsaCpuCode - PRV_CPSS_DXCH_NET_DSA_TAG_FIRST_USER_DEFINED_E));
        return GT_OK;
    }

    switch (dsaCpuCode)
    {
        case PRV_CPSS_DXCH_NET_DSA_TAG_CONTROL_TO_CPU_E:
            *cpuCodePtr = CPSS_NET_CONTROL_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_BPDU_E:
            *cpuCodePtr = CPSS_NET_CONTROL_BPDU_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_MAC_ADDR_TRAP_E:
            *cpuCodePtr = CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E;
            break;

        /* CONTROL_MAC_RANGE_TRAP is not supported by Cheetah */
        /* code used as default for PCL TRAP */
        case PRV_CPSS_DXCH_NET_DSA_TAG_MAC_RANGE_TRAP_E:
            *cpuCodePtr = CPSS_NET_CLASS_KEY_TRAP_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_ARP_BROADCAST_E:
            *cpuCodePtr = CPSS_NET_INTERVENTION_ARP_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_IGMP_PACKET_E:
            *cpuCodePtr = CPSS_NET_INTERVENTION_IGMP_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_INTERVENTION_MAC_ADDR_E:
            *cpuCodePtr = CPSS_NET_INTERVENTION_SA_DA_E;
             break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_UNK_SRC_MAC_ADDR_E:
            *cpuCodePtr = CPSS_NET_INTERVENTION_PORT_LOCK_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_MAC_RANGE_TO_CPU_E:
            *cpuCodePtr = CPSS_NET_CLASS_KEY_MIRROR_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_LOCK_TO_CPU_E:
            *cpuCodePtr = CPSS_NET_LOCK_PORT_MIRROR_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_E:
            *cpuCodePtr = CPSS_NET_UNKNOWN_UC_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_UNREGISTERED_MULTICAST_E:
            *cpuCodePtr = CPSS_NET_UNREG_MC_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_REGULAR_FORWARD_E:
            *cpuCodePtr = CPSS_NET_ETH_BRIDGED_LLT_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP0_E:
             *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
             break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP1_E:
             *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP1_E;
             break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP2_E:
             *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP2_E;
             break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_UDP_BC_MIRROR_TRAP3_E:
             *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP3_E;
             break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_E:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_1_E:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_2_E:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_3_E:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_ICMP_PACKET_E:
            *cpuCodePtr = CPSS_NET_IPV6_ICMP_PACKET_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E :
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E :
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E :
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E :
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_MIRROR_IPV4_RIPV1_TO_CPU_E :
            *cpuCodePtr = CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_NEIGHBOR_SOLICITATION_E:
            *cpuCodePtr = CPSS_NET_IPV6_NEIGHBOR_SOLICITATION_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_BROADCAST_PACKET_E:
            *cpuCodePtr = CPSS_NET_IPV4_BROADCAST_PACKET_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_NON_IPV4_BROADCAST_PACKET_E:
            *cpuCodePtr = CPSS_NET_NON_IPV4_BROADCAST_PACKET_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_CISCO_MCAST_MAC_RANGE_E:
            *cpuCodePtr = CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_BRG_UNREGD_MCAST_E:
            *cpuCodePtr = CPSS_NET_UNREGISTERED_MULTICAST_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV4_UNREG_MCAST_E:
            *cpuCodePtr = CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV6_UNREG_MCAST_E:
            *cpuCodePtr = CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_EXT_E:
            *cpuCodePtr = CPSS_NET_UNKNOWN_UNICAST_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_TARGET_NULL_PORT_E:
            *cpuCodePtr = CPSS_NET_TARGET_NULL_PORT_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_TARGET_EPORT_MTU_EXCEEDED_OR_RX_PROTECTION_SWITCHING_OR_INGRESS_FORWARDING_RESTRICTIONS_E:
            *cpuCodePtr = CPSS_NET_TARGET_EPORT_MTU_EXCEEDED_OR_RX_PROTECTION_SWITCHING_OR_INGRESS_FORWARDING_RESTRICTIONS_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_ROUTED_PACKET_FORWARD_E:
            *cpuCodePtr = CPSS_NET_ROUTED_PACKET_FORWARD_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_BRIDGED_PACKET_FRW_E:
            *cpuCodePtr = CPSS_NET_BRIDGED_PACKET_FORWARD_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_INGRESS_MIRRORED_TO_ANALYZER_E:
            *cpuCodePtr = CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_EGRESS_MIRRORED_TO_ANALYZER_E :
            *cpuCodePtr = CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_MAIL_FROM_NEIGHBOR_CPU_E:
            *cpuCodePtr = CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_CPU_TO_CPU_E:
            *cpuCodePtr = CPSS_NET_CPU_TO_CPU_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_EGRESS_SAMPLED_E:
            *cpuCodePtr = CPSS_NET_EGRESS_SAMPLED_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_INGRESS_SAMPLED_E:
            *cpuCodePtr = CPSS_NET_INGRESS_SAMPLED_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_INVALID_PCL_KEY_TRAP_E:
            *cpuCodePtr = CPSS_NET_INVALID_PCL_KEY_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_TTL_EXCEEDED_E:
            *cpuCodePtr = CPSS_NET_IP_ZERO_TTL_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MTU_EXCEEDED_E:
            *cpuCodePtr = CPSS_NET_IP_MTU_EXCEED_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_HOP_LIMIT_EXCEEDED_E:
            *cpuCodePtr = CPSS_NET_IPV6_TTL_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_ILLEGAL_ADDR_ERROR_E:
            *cpuCodePtr = CPSS_NET_ILLEGAL_DIP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_HEADER_ERROR_E:
            *cpuCodePtr = CPSS_NET_IP_HDR_ERROR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_DIP_DA_MISMATCH_E:
            *cpuCodePtr = CPSS_NET_IP_DIP_DA_MISMATCH_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_HEADER_ERROR_E:
            *cpuCodePtr = CPSS_NET_IPV6_BAD_HEADER_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_UC_SIP_SA_MISMATCH_E:
            *cpuCodePtr = CPSS_NET_IP_UC_SIP_SA_MISMATCH_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_OPTIONS_E:
            *cpuCodePtr = CPSS_NET_OPTIONS_IN_IP_HDR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_NON_HBH_OPTION_E:
            *cpuCodePtr = CPSS_NET_IPV6_EXT_HEADER_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_OPTIONS_E:
            *cpuCodePtr = CPSS_NET_IPV6_HOP_BY_HOP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_SIP_FILTERING_E                        :
            *cpuCodePtr = CPSS_NET_IPV4_6_SIP_FILTERING_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_SIP_IS_ZERO_E                          :
            *cpuCodePtr = CPSS_NET_IPV4_6_SIP_IS_ZERO_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_ACCESS_MATRIX_E                               :
            *cpuCodePtr = CPSS_NET_ACCESS_MATRIX_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_FCOE_DIP_LOOKUP_NOT_FOUND_OR_FCOE_EXCEPTION_E :
            *cpuCodePtr = CPSS_NET_FCOE_DIP_LOOKUP_NOT_FOUND_OR_FCOE_EXCEPTION_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_FCOE_SIP_NOT_FOUND_E                          :
            *cpuCodePtr = CPSS_NET_FCOE_SIP_NOT_FOUND_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_SCOPE_E:
            *cpuCodePtr = CPSS_NET_IPV6_SCOP_FAIL_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE0_E:
            *cpuCodePtr = CPSS_NET_ROUTE_ENTRY_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE1_E:
            *cpuCodePtr = CPSS_NET_IPV4_UC_ROUTE1_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE2_E:
            *cpuCodePtr = CPSS_NET_IPV4_UC_ROUTE2_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ROUTE3_E:
            *cpuCodePtr = CPSS_NET_IPV4_UC_ROUTE3_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE0_E:
            *cpuCodePtr = CPSS_NET_IPV4_MC_ROUTE0_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE1_E:
            *cpuCodePtr = CPSS_NET_IPV4_MC_ROUTE1_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE2_E:
            *cpuCodePtr = CPSS_NET_IPV4_MC_ROUTE2_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_MC_ROUTE3_E:
            *cpuCodePtr = CPSS_NET_IPV4_MC_ROUTE3_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE0_E:
            *cpuCodePtr = CPSS_NET_IPV6_ROUTE_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE1_E:
            *cpuCodePtr = CPSS_NET_IPV6_UC_ROUTE1_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE2_E:
            *cpuCodePtr = CPSS_NET_IPV6_UC_ROUTE2_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ROUTE3_E:
            *cpuCodePtr = CPSS_NET_IPV6_UC_ROUTE3_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE0_E:
            *cpuCodePtr = CPSS_NET_IPV6_MC_ROUTE0_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE1_E:
            *cpuCodePtr = CPSS_NET_IPV6_MC_ROUTE1_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE2_E:
            *cpuCodePtr = CPSS_NET_IPV6_MC_ROUTE2_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_MC_ROUTE3_E:
            *cpuCodePtr = CPSS_NET_IPV6_MC_ROUTE3_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_UC_RPF_FAIL_E:
            *cpuCodePtr = CPSS_NET_IP_UC_RPF_FAIL_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MC_ROUTE_RPF_FAIL_E:
            *cpuCodePtr = CPSS_NET_RPF_CHECK_FAILED_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_6_MC_MLL_RPF_FAIL_E:
            *cpuCodePtr = CPSS_NET_MLL_RPF_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_ARP_BC_TO_ME_E:
            *cpuCodePtr = CPSS_NET_ARP_BC_TO_ME_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_UC_ICMP_REDIRECT_E:
            *cpuCodePtr = CPSS_NET_MIRROR_IPV4_UC_ICMP_REDIRECT_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_UC_ICMP_REDIRECT_E:
            *cpuCodePtr = CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IP_MC_ROUTE_BIDIR_RPF_FAIL_E:
            *cpuCodePtr = CPSS_NET_IP_MC_ROUTE_BIDIR_RPF_FAIL_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_MET_RED_DROP_CODE_E:
            *cpuCodePtr = CPSS_NET_MET_RED_DROP_CODE_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_ECC_DROP_CODE_E:
            *cpuCodePtr = CPSS_NET_ECC_DROP_CODE_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_PACKET_TO_VIRTUAL_ROUTER_PORT_E:
            *cpuCodePtr = CPSS_NET_PACKET_TO_VIRTUAL_ROUTER_PORT_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_SEC_AUTO_LEARN_UNK_SRC_TRAP_E:
            *cpuCodePtr = CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_RXDMA_DROP_E:
            *cpuCodePtr = CPSS_NET_RXDMA_DROP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_HEADER_ERROR_E:
            *cpuCodePtr = CPSS_NET_IPV4_TT_HEADER_ERROR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_OPTION_FRAG_ERROR_E:
            *cpuCodePtr = CPSS_NET_IPV4_TT_OPTION_FRAG_ERROR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_TT_UNSUP_GRE_ERROR_E:
            *cpuCodePtr = CPSS_NET_IPV4_TT_UNSUP_GRE_ERROR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_BAD_ANALYZER_INDEX_DROP_ERROR_E:
            *cpuCodePtr = CPSS_NET_BAD_ANALYZER_INDEX_DROP_ERROR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_TT_MPLS_HEADER_CHECK_E:
            *cpuCodePtr = CPSS_NET_TT_MPLS_HEADER_CHECK_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_TT_MPLS_TTL_EXCEED_E:
            *cpuCodePtr = CPSS_NET_TT_MPLS_TTL_EXCEED_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_UNTAGGED_OVERSIZE_DROP_CODE_E:
            *cpuCodePtr = CPSS_NET_UNTAGGED_OVERSIZE_DROP_CODE_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_IPV6_TT_UNSUP_GRE_ERROR_E:
            *cpuCodePtr = CPSS_NET_IPV6_TT_UNSUP_GRE_ERROR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_OAM_PDU_TRAP_E:
            *cpuCodePtr = CPSS_NET_OAM_PDU_TRAP_E;
            break;

        case PRV_CPSS_DXCH_NET_DSA_TAG_ARP_REPLY_TO_ME_E:
            *cpuCodePtr = CPSS_NET_ARP_REPLY_TO_ME_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_CPU_TO_ALL_CPUS_E:
            *cpuCodePtr = CPSS_NET_CPU_TO_ALL_CPUS_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_TCP_SYN_TO_CPU_E:
            *cpuCodePtr = CPSS_NET_TCP_SYN_TO_CPU_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_PTP_HEADER_ERROR_E:
            *cpuCodePtr = CPSS_NET_PTP_HEADER_ERROR_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_CCFC_PKT_DROP_E:
            *cpuCodePtr = CPSS_NET_CCFC_PKT_DROP_E;
            break;
        case PRV_CPSS_DXCH_NET_DSA_TAG_BAD_FC_PKT_DROP_E:
            *cpuCodePtr = CPSS_NET_BAD_FC_PKT_DROP_E;
            break;

        default :
            /* not supported ?? */
            *cpuCodePtr = (CPSS_NET_RX_CPU_CODE_ENT)
                (CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E + dsaCpuCode);
            break;
    }


    return GT_OK;
}



