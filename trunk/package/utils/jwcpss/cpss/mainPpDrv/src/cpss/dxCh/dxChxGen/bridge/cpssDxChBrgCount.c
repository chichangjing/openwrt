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
*       $Revision: 19 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>
/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* get vid checking */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgCount.h>

/*******************************************************************************
* prvBrgCntDropCntrModeToCpuCode
*
* DESCRIPTION:
*       convert Drop Counter Mode to Dsa Tag Cpu Code.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       dropMode  - Drop Counter mode.
*
* OUTPUTS:
*       dsaCpuCodePtr - (pointer to) dsa tag cpu code.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or dropMode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvBrgCntDropCntrModeToCpuCode
(
    IN  CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT dropMode,
    OUT PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT *dsaCpuCodePtr
)
{
    switch (dropMode)
    {
    case CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_REGULAR_FORWARD_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_FDB_ENTRY_CMD_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_MAC_ADDR_TRAP_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UNK_SRC_MAC_ADDR_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_INVALID_SA_E:
        *dsaCpuCodePtr =  PRV_CPSS_DXCH_NET_DSA_TAG_INVALID_SA_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_INVALID_VLAN_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_VLAN_NOT_VALID_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_PORT_NOT_VLAN_MEM_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_VLAN_RANGE_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_VLAN_RANGE_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_STATIC_ADDR_MOVED_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_ARP_SA_MISMATCH_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_ARP_SA_MISMATCH_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_SYN_WITH_DATA_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_SYN_WITH_DATA_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TCP_OVER_MC_BC_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_ACCESS_MATRIX_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BRIDGE_ACCESS_MATRIX_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_SEC_LEARNING_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_SEC_AUTO_LEARN_UNK_SRC_TRAP_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_ACCEPT_FRAME_TYPE_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_FRAG_ICMP_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_FRAGMENT_ICMP_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TCP_FLAGS_ZERO_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TCP_FLAGS_FIN_URG_PSH_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TCP_FLAGS_SYN_FIN_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TCP_FLAGS_SYN_RST_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_TCP_UDP_SRC_DEST_ZERO_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_VLAN_MRU_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_VLAN_MRU_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_RATE_LIMITING_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_LOCAL_PORT_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_LOCAL_PORT_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BPDU_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_IP_MC_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IP_MC_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_NON_IP_MC_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_NON_IP_MC_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_DSA_TAG_LOCAL_DEV_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_IEEE_RESERVED_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BRG_UNREGD_MCAST_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV6_UNREG_MCAST_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV4_UNREG_MCAST_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_L2_UC_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_EXT_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_BROADCAST_PACKET_E;
        break;
    case CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E:
        *dsaCpuCodePtr = PRV_CPSS_DXCH_NET_DSA_TAG_NON_IPV4_BROADCAST_PACKET_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvBrgCntDropCntrModeToCpuCode
*
* DESCRIPTION:
*       convert Drop Counter Mode to Dsa Tag Cpu Code.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       dsaCpuCode - dsa tag cpu code.
*
* OUTPUTS:
*       dropModePtr  - (pointer to ) Drop Counter mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or dropMode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvBrgCntCpuCodeToDropCntrMode
(
    IN PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode,
    OUT  CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT *dropModePtr
)
{
    switch (dsaCpuCode)
    {
    case PRV_CPSS_DXCH_NET_DSA_TAG_REGULAR_FORWARD_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_MAC_ADDR_TRAP_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_FDB_ENTRY_CMD_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_UNK_SRC_MAC_ADDR_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_INVALID_SA_E:
        *dropModePtr =  CPSS_DXCH_BRG_DROP_CNTR_INVALID_SA_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_VLAN_NOT_VALID_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_INVALID_VLAN_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_PORT_NOT_VLAN_MEM_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_VLAN_RANGE_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_VLAN_RANGE_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_STATIC_ADDR_MOVED_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_ARP_SA_MISMATCH_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_ARP_SA_MISMATCH_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_SYN_WITH_DATA_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_SYN_WITH_DATA_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_TCP_OVER_MC_BC_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_BRIDGE_ACCESS_MATRIX_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_ACCESS_MATRIX_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_SEC_AUTO_LEARN_UNK_SRC_TRAP_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_SEC_LEARNING_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_ACCEPT_FRAME_TYPE_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_FRAGMENT_ICMP_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_FRAG_ICMP_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_TCP_FLAGS_ZERO_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_TCP_FLAGS_FIN_URG_PSH_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_TCP_FLAGS_SYN_FIN_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_TCP_FLAGS_SYN_RST_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_TCP_UDP_SRC_DEST_ZERO_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_VLAN_MRU_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_VLAN_MRU_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_RATE_LIMITING_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_LOCAL_PORT_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_LOCAL_PORT_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_BPDU_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_IP_MC_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_IP_MC_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_NON_IP_MC_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_NON_IP_MC_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_DSA_TAG_LOCAL_DEV_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_IEEE_RESERVED_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_BRG_UNREGD_MCAST_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV6_UNREG_MCAST_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_BRG_IPV4_UNREG_MCAST_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_UNKNOWN_UNICAST_EXT_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_L2_UC_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_IPV4_BROADCAST_PACKET_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E;
        break;
    case PRV_CPSS_DXCH_NET_DSA_TAG_NON_IPV4_BROADCAST_PACKET_E:
        *dropModePtr = CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E;
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChBrgCntDropCntrModeSet
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
static GT_STATUS internal_cpssDxChBrgCntDropCntrModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT dropMode
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */
    GT_U32      fieldLength; /* the number of bits to be written to register */
    GT_U32      fieldOffset; /* The start bit number in the register */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode; /* DSA tag cpu code */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if (dropMode > CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E ||
        dropMode < CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                             bridgeGlobalConfigRegArray[1];
    }

    /*******************************************/
    /* Calculate HW value of Drop Counter Mode */
    /*******************************************/
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        if (dropMode > CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E)
        {
            if (dropMode < CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
            else
            {
                regValue = ((GT_U32)dropMode - 13);
            }
        }
        else
        {
            regValue = (GT_U32)dropMode;
        }

        fieldLength = 5;
        fieldOffset = 20;
    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            rc = prvBrgCntDropCntrModeToCpuCode(dropMode,&dsaCpuCode);
            if (rc != GT_OK)
            {
                return rc;
            }
            regValue = dsaCpuCode;

            fieldLength = 8;
            fieldOffset = 18;
        }
        else if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            regValue = (GT_U32)dropMode;
            fieldLength = 6;
            fieldOffset = 20;
        }
        else
        {
            regValue = ((GT_U32)dropMode) & 0x1F;
            retStatus = prvCpssHwPpSetRegField(devNum,
                                                  regAddr, 0, 1, (dropMode >> 5));
            if (GT_OK != retStatus)
            {
                return retStatus;
            }

            fieldLength = 5;
            fieldOffset = 20;
        }

    }

    return prvCpssHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, regValue);

}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntDropCntrModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropMode));

    rc = internal_cpssDxChBrgCntDropCntrModeSet(devNum, dropMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntDropCntrModeGet
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
static GT_STATUS internal_cpssDxChBrgCntDropCntrModeGet
(
    IN   GT_U8                                   devNum,
    OUT  CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT     *dropModePtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dropModePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    bridgeEngineConfig.bridgeGlobalConfig1;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                         bridgeGlobalConfigRegArray[1];
    }

    retStatus = prvCpssHwPpReadRegister(devNum, regAddr, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /************************************/
    /* Gather Drop Counter Mode from HW */
    /************************************/
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        regValue = (regValue >> 20) & 0x1F;

        if (regValue > 20)
        {
            retStatus = GT_NOT_SUPPORTED;
        }
        else
        {
            if (regValue > 7)
            {
                regValue = regValue + 13;
            }
        }

    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regValue = U32_GET_FIELD_MAC(regValue,18,8);

            return prvBrgCntCpuCodeToDropCntrMode(regValue,dropModePtr);

        }
        else if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            regValue = (regValue >> 20) & 0x3F;
        }
        else
        {
            regValue = ((regValue >> 20) & 0x1F) | ((regValue & 0x1) << 5);
        }

        if (regValue > CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E)
        {
            retStatus = GT_NOT_SUPPORTED;
        }
    }

    /* In any case, the HW reg value will be stored in the *dropModePtr */
    *dropModePtr = (CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT)regValue;

    return retStatus;

}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntDropCntrModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropModePtr));

    rc = internal_cpssDxChBrgCntDropCntrModeGet(devNum, dropModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntMacDaSaSet
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
static GT_STATUS internal_cpssDxChBrgCntMacDaSaSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *saAddrPtr,
    IN  GT_ETHERADDR    *daAddrPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(saAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(daAddrPtr);

    /* Set 4 LSB of MAC DA to the HW */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.MACAddrCount0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.brgMacCntr0;
    }
    regValue = GT_HW_MAC_LOW32(daAddrPtr);
    retStatus = prvCpssHwPpWriteRegister(devNum, regAddr, regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /* Set 2 MSB of MAC DA and 2 LSB of MAC SA to the HW */
    regValue = GT_HW_MAC_HIGH16(daAddrPtr) | (GT_HW_MAC_LOW16(saAddrPtr) << 16);
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.MACAddrCount1;
    }
    else
    {
        regAddr += 4;
    }

    retStatus = prvCpssHwPpWriteRegister(devNum, regAddr , regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }
    /* Set 4 MSB of MAC SA to the HW */
    regValue = GT_HW_MAC_HIGH32(saAddrPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.MACAddrCount2;
    }
    else
    {
        regAddr += 4;
    }

    retStatus = prvCpssHwPpWriteRegister(devNum, regAddr, regValue);

    return retStatus;

}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntMacDaSaSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, saAddrPtr, daAddrPtr));

    rc = internal_cpssDxChBrgCntMacDaSaSet(devNum, saAddrPtr, daAddrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, saAddrPtr, daAddrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntMacDaSaGet
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
static GT_STATUS internal_cpssDxChBrgCntMacDaSaGet
(
    IN   GT_U8           devNum,
    OUT  GT_ETHERADDR    *saAddrPtr,
    OUT  GT_ETHERADDR    *daAddrPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(saAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(daAddrPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.MACAddrCount0;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.brgMacCntr0;
    }

    /* Get 4 LSB of MAC DA */
    retStatus = prvCpssHwPpReadRegister(devNum, regAddr, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }
    daAddrPtr->arEther[5] = (GT_U8)U32_GET_FIELD_MAC(regValue, 0, 8);
    daAddrPtr->arEther[4] = (GT_U8)U32_GET_FIELD_MAC(regValue, 8, 8);
    daAddrPtr->arEther[3] = (GT_U8)U32_GET_FIELD_MAC(regValue, 16, 8);
    daAddrPtr->arEther[2] = (GT_U8)U32_GET_FIELD_MAC(regValue, 24, 8);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.MACAddrCount1;
    }
    else
    {
        regAddr += 4;
    }

    /* Get 2 MSB of MAC DA and 2 LSB of MAC SA */
    retStatus = prvCpssHwPpReadRegister(devNum, regAddr, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }
    daAddrPtr->arEther[1] = (GT_U8)U32_GET_FIELD_MAC(regValue, 0, 8);
    daAddrPtr->arEther[0] = (GT_U8)U32_GET_FIELD_MAC(regValue, 8, 8);
    saAddrPtr->arEther[5] = (GT_U8)U32_GET_FIELD_MAC(regValue, 16, 8);
    saAddrPtr->arEther[4] = (GT_U8)U32_GET_FIELD_MAC(regValue, 24, 8);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.MACAddrCount2;
    }
    else
    {
        regAddr += 4;
    }

    /* Get 4 MSB of MAC SA */
    retStatus = prvCpssHwPpReadRegister(devNum, regAddr, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }
    saAddrPtr->arEther[3] = (GT_U8)U32_GET_FIELD_MAC(regValue, 0, 8);
    saAddrPtr->arEther[2] = (GT_U8)U32_GET_FIELD_MAC(regValue, 8, 8);
    saAddrPtr->arEther[1] = (GT_U8)U32_GET_FIELD_MAC(regValue, 16, 8);
    saAddrPtr->arEther[0] = (GT_U8)U32_GET_FIELD_MAC(regValue, 24, 8);

    return retStatus;

}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntMacDaSaGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, saAddrPtr, daAddrPtr));

    rc = internal_cpssDxChBrgCntMacDaSaGet(devNum, saAddrPtr, daAddrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, saAddrPtr, daAddrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntBridgeIngressCntrModeSet
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
static GT_STATUS internal_cpssDxChBrgCntBridgeIngressCntrModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    IN  CPSS_BRIDGE_INGR_CNTR_MODES_ENT     setMode,
    IN  GT_PORT_NUM                         port,
    IN  GT_U16                              vlan
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_U32      mode;        /* local count mode value */
    GT_U32  portGroupId,portPortGroupId;/*the port group Id - support multi-port-groups device */
    GT_PORT_NUM   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* Check Counter Set ID */
    switch(cntrSetId)
    {
        case CPSS_DXCH_BRG_CNT_SET_ID_0_E:
        case CPSS_DXCH_BRG_CNT_SET_ID_1_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /************************************************/
    /* Calculate Counter Set Configuration HW value */
    /************************************************/
    switch (setMode)
    {
        case CPSS_BRG_CNT_MODE_0_E:
            mode = 0;
            port = 0;
            vlan = 0;
            break;
        case CPSS_BRG_CNT_MODE_1_E:
            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                PRV_CPSS_HW_DEV_NUM_MAC(devNum),port);

            localPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                PRV_CPSS_HW_DEV_NUM_MAC(devNum),
                port);

            if(localPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            mode = 1;
            vlan = 0;
            break;
        case CPSS_BRG_CNT_MODE_2_E:
            if (vlan > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            mode = 2;
            port = 0;
            break;
        case CPSS_BRG_CNT_MODE_3_E:
            if (vlan > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VID_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                PRV_CPSS_HW_DEV_NUM_MAC(devNum),port);

            localPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                PRV_CPSS_HW_DEV_NUM_MAC(devNum),
                port);

            if(localPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_E_PORT_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            mode = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.cntrsSetConfig0[cntrSetId];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                             brgCntrSet[cntrSetId].cntrSetCfg;
    }

    if((mode & 1) &&
       (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE))
    {
        /* hold 'port filter' for not eArch devices.
           The eArch devices uses ePort for filter.
           Therefore same ePort value is in all port groups */

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portPortGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);
        /* loop on all port groups :
            on the port group that 'own' the port , set the needed configuration
            on other port groups put 'NULL port'
        */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

        {
            if(portPortGroupId == portGroupId)
            {
                localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,port);
            }
            else
            {
                localPort = PRV_CPSS_DXCH_NULL_PORT_NUM_CNS;
            }

            regValue = mode | (localPort << 2) | (vlan << 8);

            rc = prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr,regValue);

            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

        rc = GT_OK;
    }
    else
    {
        portPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_FALSE)
        {
            regValue = mode | (port << 2) | (vlan << 8);
        }
        else
        {
            regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                        layer2BridgeMIBCntrs.cntrsSetConfig1[cntrSetId];
            regValue = vlan;
            /* set vlan */
            rc = prvCpssHwPpWriteRegister(devNum, regAddr, regValue);
            if(rc != GT_OK)
            {
                return rc;
            }

            regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                        layer2BridgeMIBCntrs.cntrsSetConfig0[cntrSetId];
            /* set mode + port */
            regValue = mode | ((port & (BIT_13-1)) << 2);
        }

        rc = prvCpssHwPpWriteRegister(devNum, regAddr, regValue);
    }

    if(rc == GT_OK)
    {
        if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == GT_FALSE)
        {
            portPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
        /* save the info for 'get' configuration and 'read' counters */
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                bridgeIngressCntrMode[cntrSetId].portGroupId = portPortGroupId;
    }

    return rc;

}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntBridgeIngressCntrModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cntrSetId, setMode, port, vlan));

    rc = internal_cpssDxChBrgCntBridgeIngressCntrModeSet(devNum, cntrSetId, setMode, port, vlan);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cntrSetId, setMode, port, vlan));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntBridgeIngressCntrModeGet
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
static GT_STATUS internal_cpssDxChBrgCntBridgeIngressCntrModeGet
(
    IN   GT_U8                               devNum,
    IN   CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT  CPSS_BRIDGE_INGR_CNTR_MODES_ENT     *setModePtr,
    OUT  GT_PORT_NUM                         *portPtr,
    OUT  GT_U16                              *vlanPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_U32      regValue;    /* register value */
    GT_U32      mode;        /* local count mode value */
    GT_STATUS   retStatus;   /* generic return status code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(setModePtr);

    /* Check Counter Set ID */
    switch(cntrSetId)
    {
        case CPSS_DXCH_BRG_CNT_SET_ID_0_E:
        case CPSS_DXCH_BRG_CNT_SET_ID_1_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get the info from DB */
    portGroupId = PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.
                bridgeIngressCntrMode[cntrSetId].portGroupId;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.cntrsSetConfig0[cntrSetId];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                             brgCntrSet[cntrSetId].cntrSetCfg;
    }

    retStatus = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /* Gather count mode of specified Bridge Ingress Counters Set */
    mode = regValue & 0x3;
    switch (mode)
    {
        case 0:
            *setModePtr = CPSS_BRG_CNT_MODE_0_E;
            break;
        case 1:
            *setModePtr = CPSS_BRG_CNT_MODE_1_E;
            break;
        case 2:
            *setModePtr = CPSS_BRG_CNT_MODE_2_E;
            break;
        case 3:
            *setModePtr = CPSS_BRG_CNT_MODE_3_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /* Gather port number monitored by specified Bridge Ingress Counters Set */
    if (portPtr != NULL)
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            /* eArch device support 13 bit ePort */
            *portPtr = (regValue >> 2) & (BIT_13-1);
        }
        else
        {
            *portPtr = (regValue >> 2) & 0x3F;
        }


        if(portGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
        {
            /* support multi-port-groups device , convert local port to global port */
            *portPtr = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,(*portPtr));
        }
    }

    /* Gather VLAN ID  monitored by specified Bridge Ingress Counters Set */
    if (vlanPtr != NULL)
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
        {
            regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                        layer2BridgeMIBCntrs.cntrsSetConfig1[cntrSetId];
            /* set vlan */
            retStatus = prvCpssHwPpReadRegister(devNum, regAddr, &regValue);
            if(retStatus != GT_OK)
            {
                return retStatus;
            }

            /*support 13 bits*/
            *vlanPtr = (GT_U16)(regValue& (BIT_13-1));
        }
        else
        {
            *vlanPtr = (GT_U16)((regValue >> 8) & 0xFFF);
        }
    }

    return retStatus;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntBridgeIngressCntrModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cntrSetId, setModePtr, portPtr, vlanPtr));

    rc = internal_cpssDxChBrgCntBridgeIngressCntrModeGet(devNum, cntrSetId, setModePtr, portPtr, vlanPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cntrSetId, setModePtr, portPtr, vlanPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntDropCntrGet
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
static GT_STATUS internal_cpssDxChBrgCntDropCntrGet
(
    IN   GT_U8      devNum,
    OUT  GT_U32     *dropCntPtr
)
{
    return cpssDxChBrgCntPortGroupDropCntrGet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              dropCntPtr);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntDropCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropCntPtr));

    rc = internal_cpssDxChBrgCntDropCntrGet(devNum, dropCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntDropCntrSet
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
static GT_STATUS internal_cpssDxChBrgCntDropCntrSet
(
    IN  GT_U8      devNum,
    IN  GT_U32     dropCnt
)
{
    return cpssDxChBrgCntPortGroupDropCntrSet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              dropCnt);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntDropCntrSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropCnt));

    rc = internal_cpssDxChBrgCntDropCntrSet(devNum, dropCnt);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropCnt));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntHostGroupCntrsGet
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
static GT_STATUS internal_cpssDxChBrgCntHostGroupCntrsGet
(
    IN   GT_U8                              devNum,
    OUT  CPSS_DXCH_BRIDGE_HOST_CNTR_STC     *hostGroupCntPtr
)
{
    return cpssDxChBrgCntPortGroupHostGroupCntrsGet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                    hostGroupCntPtr);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntHostGroupCntrsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, hostGroupCntPtr));

    rc = internal_cpssDxChBrgCntHostGroupCntrsGet(devNum, hostGroupCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, hostGroupCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntMatrixGroupCntrsGet
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
static GT_STATUS internal_cpssDxChBrgCntMatrixGroupCntrsGet
(
    IN   GT_U8      devNum,
    OUT  GT_U32     *matrixCntSaDaPktsPtr
)
{
    return cpssDxChBrgCntPortGroupMatrixGroupCntrsGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                      matrixCntSaDaPktsPtr);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntMatrixGroupCntrsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, matrixCntSaDaPktsPtr));

    rc = internal_cpssDxChBrgCntMatrixGroupCntrsGet(devNum, matrixCntSaDaPktsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, matrixCntSaDaPktsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntBridgeIngressCntrsGet
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
static GT_STATUS internal_cpssDxChBrgCntBridgeIngressCntrsGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT CPSS_BRIDGE_INGRESS_CNTR_STC        *ingressCntrPtr
)
{
    return cpssDxChBrgCntPortGroupBridgeIngressCntrsGet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                      cntrSetId,ingressCntrPtr);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntBridgeIngressCntrsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cntrSetId, ingressCntrPtr));

    rc = internal_cpssDxChBrgCntBridgeIngressCntrsGet(devNum, cntrSetId, ingressCntrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cntrSetId, ingressCntrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntLearnedEntryDiscGet
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
static GT_STATUS internal_cpssDxChBrgCntLearnedEntryDiscGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *countValuePtr
)
{
    return cpssDxChBrgCntPortGroupLearnedEntryDiscGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                      countValuePtr);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntLearnedEntryDiscGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, countValuePtr));

    rc = internal_cpssDxChBrgCntLearnedEntryDiscGet(devNum, countValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, countValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntPortGroupDropCntrGet
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
static GT_STATUS internal_cpssDxChBrgCntPortGroupDropCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT  GT_U32              *dropCntPtr
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dropCntPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /************************************/
    /* Read Bridge Ingress Drop Counter */
    /************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
            bridgeDropCntrAndSecurityBreachDropCntrs.bridgeFilterCntr;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.dropIngrCntr;
    }

    return prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32,
                                                  dropCntPtr, NULL);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntPortGroupDropCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, dropCntPtr));

    rc = internal_cpssDxChBrgCntPortGroupDropCntrGet(devNum, portGroupsBmp, dropCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, dropCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntPortGroupDropCntrSet
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
static GT_STATUS internal_cpssDxChBrgCntPortGroupDropCntrSet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN  GT_U32               dropCnt
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /************************************/
    /* Set Bridge Ingress Drop Counter */
    /************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
            bridgeDropCntrAndSecurityBreachDropCntrs.bridgeFilterCntr;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.dropIngrCntr;
    }

    return prvCpssPortGroupsBmpCounterSet(devNum, portGroupsBmp,
                                          regAddr, 0, 32, dropCnt);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntPortGroupDropCntrSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, dropCnt));

    rc = internal_cpssDxChBrgCntPortGroupDropCntrSet(devNum, portGroupsBmp, dropCnt);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, dropCnt));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntPortGroupHostGroupCntrsGet
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
static GT_STATUS internal_cpssDxChBrgCntPortGroupHostGroupCntrsGet
(
    IN   GT_U8                              devNum,
    IN   GT_PORT_GROUPS_BMP                 portGroupsBmp,
    OUT  CPSS_DXCH_BRIDGE_HOST_CNTR_STC     *hostGroupCntPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(hostGroupCntPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /************************************************/
    /* Read value of Host Incoming Packets Counter. */
    /************************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.hostIncomingPktsCount;
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.hostInPckt;
    }
    retStatus = prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                       regAddr, 0, 32,
                                              &(hostGroupCntPtr->gtHostInPkts),
                                                       NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /************************************************/
    /* Read value of Host Outgoing Packets Counter. */
    /************************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.hostOutgoingPktsCount;
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.hostOutPckt;
    }
    retStatus = prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                       regAddr, 0, 32,
                                           &(hostGroupCntPtr->gtHostOutPkts),
                                                       NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /**********************************************************/
    /* Read value of Host Outgoing Multicast Packets Counter. */
    /**********************************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.hostOutgoingMcPktCount;
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.hostOutMcPckt;
    }
    retStatus = prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                       regAddr, 0,  32,
                                  &(hostGroupCntPtr->gtHostOutMulticastPkts),
                                                       NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /**********************************************************/
    /* Read value of Host Outgoing Broadcast Packets Counter. */
    /**********************************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.hostOutgoingBcPktCount;
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.hostOutBrdPckt;
    }
    return prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32,
                                  &(hostGroupCntPtr->gtHostOutBroadcastPkts),
                                                  NULL);

}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntPortGroupHostGroupCntrsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, hostGroupCntPtr));

    rc = internal_cpssDxChBrgCntPortGroupHostGroupCntrsGet(devNum, portGroupsBmp, hostGroupCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, hostGroupCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntPortGroupMatrixGroupCntrsGet
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
static GT_STATUS internal_cpssDxChBrgCntPortGroupMatrixGroupCntrsGet
(
    IN   GT_U8                     devNum,
    IN   GT_PORT_GROUPS_BMP        portGroupsBmp,
    OUT  GT_U32                    *matrixCntSaDaPktsPtr
)
{
    GT_U32      regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(matrixCntSaDaPktsPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /************************************/
    /* Read Matrix SA/DA Packet Counter */
    /************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.matrixSourceDestinationPktCount;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.matrixPckt;
    }

    return prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32,
                                                  matrixCntSaDaPktsPtr, NULL);

}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntPortGroupMatrixGroupCntrsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, matrixCntSaDaPktsPtr));

    rc = internal_cpssDxChBrgCntPortGroupMatrixGroupCntrsGet(devNum, portGroupsBmp, matrixCntSaDaPktsPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, matrixCntSaDaPktsPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntPortGroupBridgeIngressCntrsGet
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
static GT_STATUS internal_cpssDxChBrgCntPortGroupBridgeIngressCntrsGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  CPSS_DXCH_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT CPSS_BRIDGE_INGRESS_CNTR_STC        *ingressCntrPtr
)
{
    GT_U32      regAddr;     /* register address */
    GT_STATUS   retStatus;   /* generic return status code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ingressCntrPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* Check Counter Set ID */
    switch(cntrSetId)
    {
        case CPSS_DXCH_BRG_CNT_SET_ID_0_E:
        case CPSS_DXCH_BRG_CNT_SET_ID_1_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /***********************************************/
    /* Read value of Bridge Ingress Frames Counter */
    /***********************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.setIncomingPktCount[cntrSetId];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                             brgCntrSet[cntrSetId].inPckt;
    }
    retStatus = prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                       regAddr, 0, 32,
                                          &ingressCntrPtr->gtBrgInFrames, NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /******************************************************/
    /* Read value of VLAN Ingress Filtered Packet Counter */
    /******************************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.setVLANIngrFilteredPktCount[cntrSetId];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                             brgCntrSet[cntrSetId].inFltPckt;
    }
    retStatus = prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                    regAddr, 0, 32,
                                 &ingressCntrPtr->gtBrgVlanIngFilterDisc, NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /**************************************************/
    /* Read value of Security Filtered Packet Counter */
    /**************************************************/
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.setSecurityFilteredPktCount[cntrSetId];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                             brgCntrSet[cntrSetId].secFltPckt;
    }
    retStatus = prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                    regAddr, 0, 32,
                                  &ingressCntrPtr->gtBrgSecFilterDisc, NULL);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /************************************************/
    /* Read value of Bridge Filtered Packet Counter */
    /************************************************/

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    layer2BridgeMIBCntrs.setBridgeFilteredPktCount[cntrSetId];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.
                                             brgCntrSet[cntrSetId].brgFltPckt;
    }
    retStatus = prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                    regAddr, 0, 32,
                                     &ingressCntrPtr->gtBrgLocalPropDisc, NULL);

    return retStatus;

}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntPortGroupBridgeIngressCntrsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, cntrSetId, ingressCntrPtr));

    rc = internal_cpssDxChBrgCntPortGroupBridgeIngressCntrsGet(devNum, portGroupsBmp, cntrSetId, ingressCntrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, cntrSetId, ingressCntrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChBrgCntPortGroupLearnedEntryDiscGet
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
static GT_STATUS internal_cpssDxChBrgCntPortGroupLearnedEntryDiscGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *countValuePtr
)
{
    GT_U32 regAddr;     /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(countValuePtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.supportSingleFdbInstance)
        {
            /* force the API to be called with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
            PRV_CPSS_MULTI_PORT_GROUPS_BMP_FORCE_UNAWARE_CHECK_MAC(devNum,portGroupsBmp);
        }
        regAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.sourceAddrNotLearntCntr.learnedEntryDiscardsCount;
    }
    else
    {
        /* Read value of Learned Entry Discards Counter */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.learnedDisc;
    }
    return prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32,
                                                  countValuePtr, NULL);
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChBrgCntPortGroupLearnedEntryDiscGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, countValuePtr));

    rc = internal_cpssDxChBrgCntPortGroupLearnedEntryDiscGet(devNum, portGroupsBmp, countValuePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, countValuePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
