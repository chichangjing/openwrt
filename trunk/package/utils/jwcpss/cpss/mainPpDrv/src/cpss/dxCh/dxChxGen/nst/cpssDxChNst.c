/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNst.c
*
* DESCRIPTION:
*       Function implementation for Network Shield Technology configuration.
*
* DEPENDENCIES:
*
*       $Revision: 23 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/nst/private/prvCpssDxChNstLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNst.h>
/* check errata */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChErrataMng.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

#define PRV_CPSS_DXCH_NST_MAC_ACCESS_LEVELS_MAX_NUM_CNS (8)

/* check access level validity */
#define PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(accessLevel)        \
    if((accessLevel) >= PRV_CPSS_DXCH_NST_MAC_ACCESS_LEVELS_MAX_NUM_CNS)  \
    {                                                                     \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                              \
    }

/* set default access level field offset according to the type */
#define PRV_CPSS_DXCH2_NST_DEFAULT_ACCESS_LEVEL_FIELD_OFFSET_SET_MAC(        \
    _paramType, _fieldOffset)                                                \
    switch (_paramType)                                                      \
    {                                                                        \
       case CPSS_NST_AM_SA_AUTO_LEARNED_E:                                   \
           _fieldOffset = 8;                                                 \
           break;                                                            \
       case CPSS_NST_AM_DA_AUTO_LEARNED_E:                                   \
           _fieldOffset = 12;                                                \
           break;                                                            \
       case CPSS_NST_AM_SA_UNKNOWN_E:                                        \
           _fieldOffset = 0;                                                 \
           break;                                                            \
       case CPSS_NST_AM_DA_UNKNOWN_E:                                        \
           _fieldOffset = 4;                                                 \
           break;                                                            \
       default:                                                              \
           CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                              \
    }

/* set sanity check field offset and index for register address setting
   according sanity check type */
#define PRV_CPSS_DXCH2_NST_SANITY_CHECK_TYPE_FIELD_OFFSET_SET_MAC(           \
    _checkType, _index, _fieldOffset)                                        \
    switch(_checkType)                                                       \
    {                                                                        \
        case CPSS_NST_CHECK_TCP_SYN_DATA_E:                                  \
            _fieldOffset = 29;                                               \
            _index = 0;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_OVER_MAC_MC_BC_E:                            \
            _fieldOffset = 17;                                               \
            _index = 0;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAG_ZERO_E:                                   \
            _fieldOffset = 4;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAGS_FIN_URG_PSH_E:                         \
            _fieldOffset = 3;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E:                             \
            _fieldOffset = 2;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAGS_SYN_RST_E:                             \
            _fieldOffset = 1;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_UDP_PORT_ZERO_E:                             \
            _fieldOffset = 0;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_FRAG_IPV4_ICMP_E:                                \
            _fieldOffset = 5;                                                \
            _index = 3;                                                      \
            break;                                                           \
        case CPSS_NST_CHECK_ARP_MAC_SA_MISMATCH_E:                           \
            _fieldOffset = 16;                                               \
            _index = 0;                                                      \
            break;                                                           \
        default:                                                             \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                             \
    }

/* set sanity check field offset and index for register address setting
   according sanity check type */
#define PRV_CPSS_LION3_NST_SANITY_CHECK_TYPE_FIELD_OFFSET_SET_MAC(           \
    _checkType, _regAddr, _fieldOffset)                                      \
    switch(_checkType)                                                       \
    {                                                                        \
        case CPSS_NST_CHECK_TCP_SYN_DATA_E:                                  \
            _fieldOffset = 18;                                               \
            _regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).         \
                bridgeEngineConfig.bridgeCommandConfig3;                     \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_OVER_MAC_MC_BC_E:                            \
            _fieldOffset = 15;                                               \
            _regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).         \
                bridgeEngineConfig.bridgeCommandConfig3;                     \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAG_ZERO_E:                                 \
            _fieldOffset = 12;                                               \
            _regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).         \
                bridgeEngineConfig.bridgeCommandConfig3;                     \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAGS_FIN_URG_PSH_E:                         \
            _fieldOffset = 9;                                                \
            _regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).         \
                bridgeEngineConfig.bridgeCommandConfig3;                     \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAGS_SYN_FIN_E:                             \
            _fieldOffset = 6;                                                \
            _regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).         \
                bridgeEngineConfig.bridgeCommandConfig3;                     \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_FLAGS_SYN_RST_E:                             \
            _fieldOffset = 3;                                                \
            _regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).         \
                bridgeEngineConfig.bridgeCommandConfig3;                     \
            break;                                                           \
        case CPSS_NST_CHECK_TCP_UDP_PORT_ZERO_E:                             \
            _fieldOffset = 0;                                                \
            _regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).         \
                bridgeEngineConfig.bridgeCommandConfig3;                     \
            break;                                                           \
        case CPSS_NST_CHECK_FRAG_IPV4_ICMP_E:                                \
            _fieldOffset = 0;                                                \
            _regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).         \
                bridgeEngineConfig.bridgeCommandConfig1;                     \
            break;                                                           \
        case CPSS_NST_CHECK_ARP_MAC_SA_MISMATCH_E:                           \
            _fieldOffset = 21;                                               \
            _regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).         \
                bridgeEngineConfig.bridgeCommandConfig0;                     \
            break;                                                           \
        default:                                                             \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                             \
    }

/* set ingress forward filter register address according to filter type */
#define PRV_CPSS_DXCH2_NST_PORT_INGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(      \
    _devNum, _filterType, _regAddr)                                          \
   switch(_filterType)                                                       \
   {                                                                         \
       case CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E:                        \
           _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                             bridgeRegs.nstRegs.netIngrFrwFltConfReg;        \
           break;                                                            \
       case CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E:                            \
           _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                             bridgeRegs.nstRegs.cpuIngrFrwFltConfReg;        \
           break;                                                            \
       case CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E:                       \
           _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                             bridgeRegs.nstRegs.analyzerIngrFrwFltConfReg;   \
           break;                                                            \
       default:                                                              \
           CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                              \
   }


/* set egress forward filter register address according to filter type */
#define PRV_CPSS_DXCH2_NST_PORT_EGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(         \
    _devNum, _filterType, _regAddr)                                            \
   switch(_filterType)                                                         \
   {                                                                           \
       case CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E:                             \
             _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                                     bridgeRegs.nstRegs.cpuPktsFrwFltConfReg;  \
            break;                                                             \
       case CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E:                              \
            _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                   \
                                     bridgeRegs.nstRegs.brgPktsFrwFltConfReg;  \
            break;                                                             \
       case CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E:                               \
            _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                   \
                                     bridgeRegs.nstRegs.routePktsFrwFltConfReg;\
            break;                                                             \
       default:                                                                \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                               \
   }

/* for TXQ ver 1 : get egress forward filter register address and bit offset
    according to filter type , and port */
#define TXQ_DISTIBUTER_FORWARD_RESTRICTED_FILTER_REG_ADDR_AND_BIT_MAC(         \
    _devNum, _portNum,_filterType, _regAddr,_bitOffset)                        \
                                                                               \
   _bitOffset = OFFSET_TO_BIT_MAC(_portNum);                                   \
   switch(_filterType)                                                         \
   {                                                                           \
       case CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E:                             \
             _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                  \
                        txqVer1.queue.distributor.                             \
                        fromCpuForwardRestricted[OFFSET_TO_WORD_MAC(_portNum)];\
            break;                                                             \
       case CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E:                              \
            _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                   \
                        txqVer1.queue.distributor.                             \
                        bridgedForwardRestricted[OFFSET_TO_WORD_MAC(_portNum)];\
            break;                                                             \
       case CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E:                               \
            _regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(_devNum)->                   \
                        txqVer1.queue.distributor.                             \
                        routedForwardRestricted[OFFSET_TO_WORD_MAC(_portNum)]; \
            break;                                                             \
       default:                                                                \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                               \
   }


/*******************************************************************************
* prvCpssDxChNstAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set bridge/ip access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      command        - command assigned to a packet
*      isBridge       - whether to set the bridge or the ip access matrix.
*
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel,
*                            command.
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNstAccessMatrixCmdSet
(
    IN GT_U8                       devNum,
    IN GT_U32                      saAccessLevel,
    IN GT_U32                      daAccessLevel,
    IN CPSS_PACKET_CMD_ENT         command,
    IN GT_BOOL                     isBridge
)
{
    GT_U32    matrixAccessRegAddr;/* register address */
    GT_U32    hwValueOfCmd;       /* Hardware value of command  */
    GT_U32    hwData;             /* data that is read/written from/to register */
    GT_STATUS rc;                 /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(saAccessLevel);
    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(daAccessLevel);

    switch(command)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            hwValueOfCmd = 0;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            hwValueOfCmd = 1;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            hwValueOfCmd = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (isBridge == GT_TRUE)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(hwValueOfCmd,command);

            matrixAccessRegAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                bridgeAccessMatrix.bridgeAccessMatrixLine[saAccessLevel];

            /* Set bridge access matrix entry */
            rc =  prvCpssHwPpSetRegField(devNum,matrixAccessRegAddr,
                                      daAccessLevel * 3, 3, hwValueOfCmd);

            return rc;
        }
        else
        {
            matrixAccessRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                               bridgeRegs.nstRegs.brgAccessMatrix;
        }
    }
    else
    {
        if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            /* Write twice for backward compatibility:
               first time to IPv4 table, second time to IPv6 table.
               This should be changed since there are different tables to IPv4,
               IPv6 and FCoE */
            CPSS_TBD_BOOKMARK

            /* IPv4 */
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                 PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E,
                                                 saAccessLevel,
                                                 PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                 2 * daAccessLevel,
                                                 2,
                                                 hwValueOfCmd);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* IPv6 */
            rc = prvCpssDxChWriteTableEntryField(devNum,
                                                 PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E,
                                                 64 + saAccessLevel,
                                                 PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                                 2 * daAccessLevel,
                                                 2,
                                                 hwValueOfCmd);
            return rc;
        }
        else
        {
            matrixAccessRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                               ipRegs.routerAccessMatrixBase;
        }
    }

    /* FEr#1006 - Bridge Access Matrix Line <7> register is write only.
       Shadow of the register should be used by SW.*/
    if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E) == GT_TRUE) &&
          (isBridge == GT_TRUE) && (saAccessLevel == 7))
    {
        hwData =  (PRV_CPSS_DXCH_PP_MAC(devNum))->
                                          errata.bridgeAccessMatrixLine7RegData;

        U32_SET_FIELD_MASKED_MAC(hwData,(daAccessLevel * 2),2,hwValueOfCmd);

        rc = prvCpssHwPpWriteRegister(devNum,
                                 (matrixAccessRegAddr + saAccessLevel * 0x4),
                                 hwData);
        if(rc != GT_OK)
            return rc;
        (PRV_CPSS_DXCH_PP_MAC(devNum))->
            errata.bridgeAccessMatrixLine7RegData = hwData;

        return GT_OK;
    }
    else
    {

        /* Set bridge access matrix entry */
        return prvCpssHwPpSetRegField(devNum,
                                 (matrixAccessRegAddr + saAccessLevel * 0x4),
                                  daAccessLevel * 2, 2, hwValueOfCmd);
    }

}

/*******************************************************************************
* prvCpssDxChNstAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get bridge/ip access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      isBridge       - weather to set the bridge or the ip access matrix.
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNstAccessMatrixCmdGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    OUT CPSS_PACKET_CMD_ENT         *commandPtr,
    IN  GT_BOOL                     isBridge
)
{
    GT_U32    matrixAccessRegAddr;/* register address */
    GT_U32    hwValueOfCmd;       /* Hardware value of command  */
    GT_U32    regData;            /* register data */
    GT_STATUS rc;                 /* return value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(saAccessLevel);
    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(daAccessLevel);
    CPSS_NULL_PTR_CHECK_MAC(commandPtr);

    matrixAccessRegAddr = 0;    /* prevent compiler warning */
    if (isBridge == GT_TRUE)
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
        {
            matrixAccessRegAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                bridgeAccessMatrix.bridgeAccessMatrixLine[saAccessLevel];

            /* Set bridge access matrix entry */
            rc =  prvCpssHwPpGetRegField(devNum,matrixAccessRegAddr,
                                      daAccessLevel * 3, 3, &hwValueOfCmd);
            if(rc != GT_OK)
                return rc;

            PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC((*commandPtr),hwValueOfCmd);

            return GT_OK;
        }
        else
        {
            matrixAccessRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                           bridgeRegs.nstRegs.brgAccessMatrix;
        }
    }
    else
    {
        if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
        {
            matrixAccessRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                               ipRegs.routerAccessMatrixBase;
        }
    }

    /* Get bridge access matrix entry */
    /* FEr#1006 - Bridge Access Matrix Line <7> register is write only.
       Shadow of the register should be used by SW.*/
    if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
          PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E) == GT_TRUE) &&
          (isBridge == GT_TRUE) && (saAccessLevel == 7))
    {
        regData =  (PRV_CPSS_DXCH_PP_MAC(devNum))->
                                          errata.bridgeAccessMatrixLine7RegData;
        hwValueOfCmd = U32_GET_FIELD_MAC(regData,(daAccessLevel * 2),2);

    }
    else if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_FALSE)
    {
        /* Get bridge access matrix entry */
        rc = prvCpssHwPpGetRegField(devNum,
                                 (matrixAccessRegAddr + saAccessLevel * 0x4),
                                  daAccessLevel * 2, 2, &hwValueOfCmd);
        if(rc != GT_OK)
            return rc;
    }
    else    /* eArch */
    {
        /* Read from IPv4 table. This should be changed since there are
           different tables to IPv4, IPv6 and FCoE */
        CPSS_TBD_BOOKMARK

        rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_IPVX_ROUTER_ACCESS_MATRIX_E,
                                            saAccessLevel,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            2 * daAccessLevel,
                                            2,
                                            &hwValueOfCmd);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    switch(hwValueOfCmd)
    {
        case 0:
            *commandPtr = CPSS_PACKET_CMD_FORWARD_E;
            break;
        case 1:
            *commandPtr = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 2:
            *commandPtr = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        default:
            *commandPtr = hwValueOfCmd;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChNstBridgeAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      command        - command assigned to a packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstBridgeAccessMatrixCmdSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    IN  CPSS_PACKET_CMD_ENT         command
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    return prvCpssDxChNstAccessMatrixCmdSet(devNum,saAccessLevel,daAccessLevel,
                                            command,GT_TRUE);

}

/*******************************************************************************
* cpssDxChNstBridgeAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      command        - command assigned to a packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstBridgeAccessMatrixCmdSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    IN  CPSS_PACKET_CMD_ENT         command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstBridgeAccessMatrixCmdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, saAccessLevel, daAccessLevel, command));

    rc = internal_cpssDxChNstBridgeAccessMatrixCmdSet(devNum, saAccessLevel, daAccessLevel, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, saAccessLevel, daAccessLevel, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstBridgeAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstBridgeAccessMatrixCmdGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    OUT CPSS_PACKET_CMD_ENT         *commandPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    return prvCpssDxChNstAccessMatrixCmdGet(devNum,saAccessLevel,daAccessLevel,
                                            commandPtr,GT_TRUE);
}

/*******************************************************************************
* cpssDxChNstBridgeAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 8  SA and up to 8 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstBridgeAccessMatrixCmdGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      saAccessLevel,
    IN  GT_U32                      daAccessLevel,
    OUT CPSS_PACKET_CMD_ENT         *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstBridgeAccessMatrixCmdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, saAccessLevel, daAccessLevel, commandPtr));

    rc = internal_cpssDxChNstBridgeAccessMatrixCmdGet(devNum, saAccessLevel, daAccessLevel, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, saAccessLevel, daAccessLevel, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstDefaultAccessLevelsSet
*
* DESCRIPTION:
*     Set default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to
*                       configure default access level .
*      accessLevel  -  default security access level for parameter
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,paramType,accessLevel
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstDefaultAccessLevelsSet
(
    IN GT_U8                    devNum,
    IN CPSS_NST_AM_PARAM_ENT    paramType,
    IN GT_U32                   accessLevel
)
{
    GT_U32 secureLevelConfRegAddr;    /* register address */
    GT_U32 fieldOffset;               /* the start bit number in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH2_NST_CHECK_MAC_ACCESS_LEVEL_MAC(accessLevel);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch (paramType)
        {
           case CPSS_NST_AM_SA_AUTO_LEARNED_E:
               secureLevelConfRegAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.
                                FDBGlobalConfig.bridgeAccessLevelConfig;
               fieldOffset = 0;
               break;
           case CPSS_NST_AM_DA_AUTO_LEARNED_E:
               secureLevelConfRegAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.
                                FDBGlobalConfig.bridgeAccessLevelConfig;
               fieldOffset = 4;
               break;
           case CPSS_NST_AM_SA_UNKNOWN_E:
                secureLevelConfRegAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                bridgeAccessMatrix.bridgeAccessMatrixDefault;
               fieldOffset = 0;
               break;
           case CPSS_NST_AM_DA_UNKNOWN_E:
                secureLevelConfRegAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                bridgeAccessMatrix.bridgeAccessMatrixDefault;
               fieldOffset = 4;
               break;
           default:
               CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

    }
    else
    {
        secureLevelConfRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.nstRegs.brgSecureLevelConfReg;

        PRV_CPSS_DXCH2_NST_DEFAULT_ACCESS_LEVEL_FIELD_OFFSET_SET_MAC(paramType,
                                                                 fieldOffset);
    }

    return /* Set default access levels*/
        prvCpssHwPpSetRegField(devNum,secureLevelConfRegAddr,fieldOffset,
                                                               3,accessLevel);

}

/*******************************************************************************
* cpssDxChNstDefaultAccessLevelsSet
*
* DESCRIPTION:
*     Set default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to
*                       configure default access level .
*      accessLevel  -  default security access level for parameter
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,paramType,accessLevel
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstDefaultAccessLevelsSet
(
    IN GT_U8                    devNum,
    IN CPSS_NST_AM_PARAM_ENT    paramType,
    IN GT_U32                   accessLevel
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstDefaultAccessLevelsSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, paramType, accessLevel));

    rc = internal_cpssDxChNstDefaultAccessLevelsSet(devNum, paramType, accessLevel);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, paramType, accessLevel));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstDefaultAccessLevelsGet
*
* DESCRIPTION:
*     Get default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to
*                       configure default access level .
* OUTPUTS:
*      accessLevelPtr - (pointer to) default security access level for parameter
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,paramType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstDefaultAccessLevelsGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_NST_AM_PARAM_ENT    paramType,
    OUT GT_U32                   *accessLevelPtr
)
{
    GT_U32 secureLevelConfRegAddr;    /* register address */
    GT_U32 fieldOffset;               /* the start bit number in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(accessLevelPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch (paramType)
        {
           case CPSS_NST_AM_SA_AUTO_LEARNED_E:
               secureLevelConfRegAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.
                                FDBGlobalConfig.bridgeAccessLevelConfig;
               fieldOffset = 0;
               break;
           case CPSS_NST_AM_DA_AUTO_LEARNED_E:
               secureLevelConfRegAddr = PRV_DXCH_REG1_UNIT_FDB_MAC(devNum).FDBCore.
                                FDBGlobalConfig.bridgeAccessLevelConfig;
               fieldOffset = 4;
               break;
           case CPSS_NST_AM_SA_UNKNOWN_E:
                secureLevelConfRegAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                bridgeAccessMatrix.bridgeAccessMatrixDefault;
               fieldOffset = 0;
               break;
           case CPSS_NST_AM_DA_UNKNOWN_E:
                secureLevelConfRegAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                                bridgeAccessMatrix.bridgeAccessMatrixDefault;
               fieldOffset = 4;
               break;
           default:
               CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

    }
    else
    {
        PRV_CPSS_DXCH2_NST_DEFAULT_ACCESS_LEVEL_FIELD_OFFSET_SET_MAC(paramType,
                                                                 fieldOffset);

        secureLevelConfRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.nstRegs.brgSecureLevelConfReg;
    }


    /* Get default access levels */
    return  prvCpssHwPpGetRegField(devNum,secureLevelConfRegAddr,
                                       fieldOffset,3,accessLevelPtr);

}

/*******************************************************************************
* cpssDxChNstDefaultAccessLevelsGet
*
* DESCRIPTION:
*     Get default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge acess matrix global parameter to
*                       configure default access level .
* OUTPUTS:
*      accessLevelPtr - (pointer to) default security access level for parameter
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,paramType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstDefaultAccessLevelsGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_NST_AM_PARAM_ENT    paramType,
    OUT GT_U32                   *accessLevelPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstDefaultAccessLevelsGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, paramType, accessLevelPtr));

    rc = internal_cpssDxChNstDefaultAccessLevelsGet(devNum, paramType, accessLevelPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, paramType, accessLevelPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChNstProtSanityCheckSet
*
* DESCRIPTION:
*      Set packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
*      enable    - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,checkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstProtSanityCheckSet
(
    IN GT_U8                 devNum,
    IN CPSS_NST_CHECK_ENT    checkType,
    IN GT_BOOL               enable
)
{
    GT_U32 regAddr;       /* register address */
    GT_U32 data;          /* reg subfield data */
    GT_U32 fieldOffset;   /* the start bit number in register */
    GT_U32 index;         /* number of bridge global configuration register */
    GT_STATUS rc;         /* return code */
    GT_U32 tmpValue;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        CPSS_TBD_BOOKMARK_EARCH
        /* need to add support for 5 commands */

        data = (enable == GT_TRUE) ? CPSS_PACKET_CMD_DROP_HARD_E : CPSS_PACKET_CMD_FORWARD_E;

        /* set all TCP Sanity checks */
        if(checkType == CPSS_NST_CHECK_TCP_ALL_E)
        {
            regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                bridgeEngineConfig.bridgeCommandConfig3;

            tmpValue = data;

            data = 0;
            /*duplicate the value to 10 fields*/
            for(index = 0 ; index < 10; index++)
            {
                U32_SET_FIELD_MAC(data,(index*3),3,tmpValue);
            }

            return prvCpssHwPpSetRegField(devNum,regAddr,0,30,data);
        }

        PRV_CPSS_LION3_NST_SANITY_CHECK_TYPE_FIELD_OFFSET_SET_MAC(
            checkType, regAddr, fieldOffset);

        return /* set sanity check */
            prvCpssHwPpSetRegField(devNum,regAddr,fieldOffset,3,data);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* set all TCP Sanity checks */
    if(checkType == CPSS_NST_CHECK_TCP_ALL_E)
    {
        data = (enable == GT_TRUE) ? 0xFFFFFFFF : 0;

        rc = prvCpssHwPpWriteRegBitMask(devNum,regAddr,0x20020000,data);
        if(rc != GT_OK)
            return rc;

        return prvCpssHwPpWriteRegBitMask(devNum,
                                             regAddr + 0xC,0x0000001F,data);
    }

    data = BOOL2BIT_MAC(enable);

    PRV_CPSS_DXCH2_NST_SANITY_CHECK_TYPE_FIELD_OFFSET_SET_MAC(
        checkType, index, fieldOffset);

    return /* set sanity check */
        prvCpssHwPpSetRegField(devNum,regAddr + index * 4,fieldOffset,1,data);

}

/*******************************************************************************
* cpssDxChNstProtSanityCheckSet
*
* DESCRIPTION:
*      Set packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
*      enable    - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,checkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstProtSanityCheckSet
(
    IN GT_U8                 devNum,
    IN CPSS_NST_CHECK_ENT    checkType,
    IN GT_BOOL               enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstProtSanityCheckSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, checkType, enable));

    rc = internal_cpssDxChNstProtSanityCheckSet(devNum, checkType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, checkType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstProtSanityCheckGet
*
* DESCRIPTION:
*      Get packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
* OUTPUTS:
*      enablePtr - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,checkType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstProtSanityCheckGet
(
    IN  GT_U8                 devNum,
    IN  CPSS_NST_CHECK_ENT    checkType,
    OUT GT_BOOL               *enablePtr
)
{
    GT_U32 regAddr;       /* register address */
    GT_U32 data;          /* reg subfield data */
    GT_U32 fieldOffset;   /* the start bit number in register */
    GT_U32 index;         /* number of bridge global configuration register */
    GT_STATUS rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        CPSS_TBD_BOOKMARK_EARCH
        /* need to add support for 5 commands */

        if(checkType == CPSS_NST_CHECK_TCP_ALL_E)
        {
            regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                bridgeEngineConfig.bridgeCommandConfig3;

            rc = prvCpssHwPpGetRegField(devNum,regAddr,0,30,&data);
            if(rc != GT_OK)
                return rc;

            for(index = 0 ; index < (10-1); index++)
            {
                if(U32_GET_FIELD_MAC(data,(index*3),3) != U32_GET_FIELD_MAC(data,(index+1)*3,3))
                {
                    /* not all fields with the same value */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }
            }

            /* all fields with the same value */
        }
        else
        {
            PRV_CPSS_LION3_NST_SANITY_CHECK_TYPE_FIELD_OFFSET_SET_MAC(
                checkType, regAddr, fieldOffset);

             /* get sanity check */
            rc = prvCpssHwPpGetRegField(devNum,regAddr,fieldOffset,3,&data);
            if(rc != GT_OK)
                return rc;
        }

        *enablePtr = (data == CPSS_PACKET_CMD_DROP_HARD_E) ? GT_TRUE : GT_FALSE;

        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                     bridgeRegs.bridgeGlobalConfigRegArray[0];


    PRV_CPSS_DXCH2_NST_SANITY_CHECK_TYPE_FIELD_OFFSET_SET_MAC(
        checkType, index, fieldOffset);
     /* get sanity check */
    rc = prvCpssHwPpGetRegField(devNum,regAddr + index * 4,
                                   fieldOffset,1,&data);
    if(rc != GT_OK)
        return rc;

    *enablePtr = BIT2BOOL_MAC(data);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChNstProtSanityCheckGet
*
* DESCRIPTION:
*      Get packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
* OUTPUTS:
*      enablePtr - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum,checkType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstProtSanityCheckGet
(
    IN  GT_U8                 devNum,
    IN  CPSS_NST_CHECK_ENT    checkType,
    OUT GT_BOOL               *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstProtSanityCheckGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, checkType, enablePtr));

    rc = internal_cpssDxChNstProtSanityCheckGet(devNum, checkType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, checkType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIngressFrwFilterSet
*
* DESCRIPTION:
*     Set port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - ingress filter type
*       enable      - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIngressFrwFilterSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    IN GT_BOOL                          enable
)
{
    GT_STATUS   rc  = GT_OK;
    GT_U32      regAddr;           /* register address */
    GT_U32      data;              /* reg subfield data */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32       localPort;/* local port - support multi-port-groups device */
    GT_U32      bitOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    data = (enable == GT_TRUE) ? 0 : 1;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch(filterType)
        {
            case CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E:
                bitOffset = 1;
                break;
           case CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E:
               bitOffset = 0;
               break;
           case CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E:
               bitOffset = 2;
               break;
           default:
               CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

         /* set EQ-ingress-eport table */
         rc = prvCpssDxChWriteTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            bitOffset, /* start bit */
                                            1,  /* 1 bit */
                                            data);

    }
    else
    {

        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        PRV_CPSS_DXCH2_NST_PORT_INGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(devNum,
                                                                   filterType,
                                                                   regAddr);
        /* Set port ingress forwarding filter */
        rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, localPort, 1, data);
    }

    if(rc != GT_OK)
        return rc;

    return rc;
}

/*******************************************************************************
* cpssDxChNstPortIngressFrwFilterSet
*
* DESCRIPTION:
*     Set port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - ingress filter type
*       enable      - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIngressFrwFilterSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    IN GT_BOOL                          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIngressFrwFilterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, filterType, enable));

    rc = internal_cpssDxChNstPortIngressFrwFilterSet(devNum, portNum, filterType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, filterType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortIngressFrwFilterGet
*
* DESCRIPTION:
*     Get port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - ingress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortIngressFrwFilterGet
(
    IN  GT_U8                            devNum,
    IN  GT_PORT_NUM                      portNum,
    IN  CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    OUT GT_BOOL                          *enablePtr
)
{
    GT_U32      regAddr;           /* register address */
    GT_STATUS   rc;                /* return code */
    GT_U32      data;              /* reg subfield data */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32       localPort;/* local port - support multi-port-groups device */
    GT_U32      bitOffset;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch(filterType)
        {
            case CPSS_NST_INGRESS_FRW_FILTER_TO_NETWORK_E:
                bitOffset = 1;
                break;
           case CPSS_NST_INGRESS_FRW_FILTER_TO_CPU_E:
               bitOffset = 0;
               break;
           case CPSS_NST_INGRESS_FRW_FILTER_TO_ANALYZER_E:
               bitOffset = 2;
               break;
           default:
               CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

         /* get EQ-ingress-eport table */
         rc = prvCpssDxChReadTableEntryField(devNum,
                                            PRV_CPSS_DXCH_LION3_TABLE_EQ_INGRESS_EPORT_E,
                                            portNum,
                                            PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                            bitOffset, /* start bit */
                                            1,  /* 1 bit */
                                            &data);

    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        PRV_CPSS_DXCH2_NST_PORT_INGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(devNum,
                                                                   filterType,
                                                                   regAddr);
        /* Set port ingress forwarding filter */
        rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, localPort, 1, &data);
        if(rc != GT_OK)
            return rc;
    }
    *enablePtr = (data == 1) ? GT_FALSE : GT_TRUE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChNstPortIngressFrwFilterGet
*
* DESCRIPTION:
*     Get port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destinied to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - ingress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable ingress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restiriction
*                     according to filterType
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortIngressFrwFilterGet
(
    IN  GT_U8                            devNum,
    IN  GT_PORT_NUM                      portNum,
    IN  CPSS_NST_INGRESS_FRW_FILTER_ENT  filterType,
    OUT GT_BOOL                          *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortIngressFrwFilterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, filterType, enablePtr));

    rc = internal_cpssDxChNstPortIngressFrwFilterGet(devNum, portNum, filterType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, filterType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChNstPortEgressFrwFilterSet
*
* DESCRIPTION:
*     Set port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - egress filter type
*       enable      - GT_TRUE - enable egress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restiriction
*                     according to filterType.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortEgressFrwFilterSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    IN GT_BOOL                          enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg subfield data */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */
    GT_U32  bitOffset;/* bit offset in the register */
    PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ENT fieldType;/*field type*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);

    data = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch(filterType)
        {
            case CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E:
                fieldType = PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_FROM_CPU_FORWARD_RESTRICTED_E;
                break;
            case CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E:
                fieldType = PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_BRIDGED_FORWARD_RESTRICTED_E;
               break;
            case CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E:
                fieldType = PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ROUTED_FORWARD_RESTRICTED_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        return prvCpssDxChHwEgfShtFieldSet(devNum,portNum,
                fieldType,
                GT_TRUE, /*accessPhysicalPort*/
                GT_TRUE, /*accessEPort*/
                data);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            /* Set port egress forwarding filter */
            PRV_CPSS_DXCH2_NST_PORT_EGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(devNum,
                                                                      filterType,
                                                                      regAddr);
            return prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, localPort, 1, data);
        }
        else
        {
            TXQ_DISTIBUTER_FORWARD_RESTRICTED_FILTER_REG_ADDR_AND_BIT_MAC(devNum,
                                                                      portNum,
                                                                      filterType,
                                                                      regAddr,
                                                                      bitOffset);
            return prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, bitOffset, 1, data);
        }
    }
}

/*******************************************************************************
* cpssDxChNstPortEgressFrwFilterSet
*
* DESCRIPTION:
*     Set port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - egress filter type
*       enable      - GT_TRUE - enable egress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restiriction
*                     according to filterType.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortEgressFrwFilterSet
(
    IN GT_U8                            devNum,
    IN GT_PORT_NUM                      portNum,
    IN CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    IN GT_BOOL                          enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortEgressFrwFilterSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, filterType, enable));

    rc = internal_cpssDxChNstPortEgressFrwFilterSet(devNum, portNum, filterType, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, filterType, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortEgressFrwFilterGet
*
* DESCRIPTION:
*     Get port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - egress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable egress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restiriction
*                     according to filterType.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortEgressFrwFilterGet
(
    IN  GT_U8                            devNum,
    IN  GT_PORT_NUM                      portNum,
    IN  CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    OUT GT_BOOL                          *enablePtr
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg subfield data */
    GT_STATUS rc;                /* return code */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */
    GT_U32  bitOffset;/* bit offset in the register */
    PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ENT fieldType;/*field type*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch(filterType)
        {
            case CPSS_NST_EGRESS_FRW_FILTER_FROM_CPU_E:
                fieldType = PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_FROM_CPU_FORWARD_RESTRICTED_E;
                break;
            case CPSS_NST_EGRESS_FRW_FILTER_BRIDGED_E:
                fieldType = PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_BRIDGED_FORWARD_RESTRICTED_E;
               break;
            case CPSS_NST_EGRESS_FRW_FILTER_ROUTED_E:
                fieldType = PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ROUTED_FORWARD_RESTRICTED_E;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChHwEgfShtFieldGet(devNum,portNum,
                fieldType,
                GT_FALSE, /*accessPhysicalPort*/
                &data);
    }
    else
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            PRV_CPSS_DXCH2_NST_PORT_EGRESS_FRW_FILTER_REG_ADDRESS_SET_MAC(devNum,
                                                                      filterType,
                                                                      regAddr);
            /* Get port egress forwarding filter */
            rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, localPort, 1, &data);
        }
        else
        {
            TXQ_DISTIBUTER_FORWARD_RESTRICTED_FILTER_REG_ADDR_AND_BIT_MAC(devNum,
                                                                      portNum,
                                                                      filterType,
                                                                      regAddr,
                                                                      bitOffset);
            rc =  prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, bitOffset, 1, &data);
        }
    }

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;

}

/*******************************************************************************
* cpssDxChNstPortEgressFrwFilterGet
*
* DESCRIPTION:
*     Get port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - port number
*       filterType  - egress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable egress forwarding restiriction according
*                     to filterType, filterred packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restiriction
*                     according to filterType.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, port, filterType
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortEgressFrwFilterGet
(
    IN  GT_U8                            devNum,
    IN  GT_PORT_NUM                      portNum,
    IN  CPSS_NST_EGRESS_FRW_FILTER_ENT   filterType,
    OUT GT_BOOL                          *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortEgressFrwFilterGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, filterType, enablePtr));

    rc = internal_cpssDxChNstPortEgressFrwFilterGet(devNum, portNum, filterType, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, filterType, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstIngressFrwFilterDropCntrSet
*
* DESCRIPTION:
*     Set the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       ingressCnt  - the counter value
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstIngressFrwFilterDropCntrSet
(
    IN GT_U8       devNum,
    IN GT_U32      ingressCnt
)
{
    return cpssDxChNstPortGroupIngressFrwFilterDropCntrSet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                           ingressCnt);
}

/*******************************************************************************
* cpssDxChNstIngressFrwFilterDropCntrSet
*
* DESCRIPTION:
*     Set the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       ingressCnt  - the counter value
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstIngressFrwFilterDropCntrSet
(
    IN GT_U8       devNum,
    IN GT_U32      ingressCnt
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstIngressFrwFilterDropCntrSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ingressCnt));

    rc = internal_cpssDxChNstIngressFrwFilterDropCntrSet(devNum, ingressCnt);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ingressCnt));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*      ingressCntPtr - the counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstIngressFrwFilterDropCntrGet
(
    IN GT_U8       devNum,
    OUT GT_U32     *ingressCntPtr
)
{
    return cpssDxChNstPortGroupIngressFrwFilterDropCntrGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                           ingressCntPtr);
}

/*******************************************************************************
* cpssDxChNstIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*      ingressCntPtr - the counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstIngressFrwFilterDropCntrGet
(
    IN GT_U8       devNum,
    OUT GT_U32     *ingressCntPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstIngressFrwFilterDropCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ingressCntPtr));

    rc = internal_cpssDxChNstIngressFrwFilterDropCntrGet(devNum, ingressCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ingressCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstRouterAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP.
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*      devNum          - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*      command         - command assigned to a packet
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstRouterAccessMatrixCmdSet
(
    IN  GT_U8                      devNum,
    IN  GT_U32                     sipAccessLevel,
    IN  GT_U32                     dipAccessLevel,
    IN  CPSS_PACKET_CMD_ENT        command
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Set router access matrix entry */
    return prvCpssDxChNstAccessMatrixCmdSet(devNum,sipAccessLevel,dipAccessLevel,
                                            command,GT_FALSE);
}

/*******************************************************************************
* cpssDxChNstRouterAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP.
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*      devNum          - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*      command         - command assigned to a packet
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstRouterAccessMatrixCmdSet
(
    IN  GT_U8                      devNum,
    IN  GT_U32                     sipAccessLevel,
    IN  GT_U32                     dipAccessLevel,
    IN  CPSS_PACKET_CMD_ENT        command
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstRouterAccessMatrixCmdSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, sipAccessLevel, dipAccessLevel, command));

    rc = internal_cpssDxChNstRouterAccessMatrixCmdSet(devNum, sipAccessLevel, dipAccessLevel, command);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, sipAccessLevel, dipAccessLevel, command));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstRouterAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP.
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*      devNum         - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstRouterAccessMatrixCmdGet
(
    IN  GT_U8                      devNum,
    IN  GT_U32                     sipAccessLevel,
    IN  GT_U32                     dipAccessLevel,
    OUT  CPSS_PACKET_CMD_ENT       *commandPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_XCAT2_E);


    /* Get router access matrix entry */
    return prvCpssDxChNstAccessMatrixCmdGet(devNum,sipAccessLevel,dipAccessLevel,
                                            commandPtr,GT_FALSE);
}

/*******************************************************************************
* cpssDxChNstRouterAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get Routers access matrix entry.
*     The packet is assigned two access levels based on its SIP and DIP.
*     The device supports up to 8  SIP and up to 8 DIP levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*      devNum         - physical device number
*      sipAccessLevel  - the sip access level
*      dipAccessLevel  - the dip access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstRouterAccessMatrixCmdGet
(
    IN  GT_U8                      devNum,
    IN  GT_U32                     sipAccessLevel,
    IN  GT_U32                     dipAccessLevel,
    OUT  CPSS_PACKET_CMD_ENT       *commandPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstRouterAccessMatrixCmdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, sipAccessLevel, dipAccessLevel, commandPtr));

    rc = internal_cpssDxChNstRouterAccessMatrixCmdGet(devNum, sipAccessLevel, dipAccessLevel, commandPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, sipAccessLevel, dipAccessLevel, commandPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortGroupIngressFrwFilterDropCntrSet
*
* DESCRIPTION:
*     Set the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       ingressCnt      - the counter value
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
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortGroupIngressFrwFilterDropCntrSet
(
    IN GT_U8                   devNum,
    IN GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN GT_U32                  ingressCnt
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
        CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                      bridgeRegs.nstRegs.ingressFrwDropCounter;

    /* set ingress drop counter */
    return prvCpssPortGroupsBmpCounterSet(devNum, portGroupsBmp,
                                              regAddr,0, 32, ingressCnt);
}

/*******************************************************************************
* cpssDxChNstPortGroupIngressFrwFilterDropCntrSet
*
* DESCRIPTION:
*     Set the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       ingressCnt      - the counter value
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
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortGroupIngressFrwFilterDropCntrSet
(
    IN GT_U8                   devNum,
    IN GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN GT_U32                  ingressCnt
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortGroupIngressFrwFilterDropCntrSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ingressCnt));

    rc = internal_cpssDxChNstPortGroupIngressFrwFilterDropCntrSet(devNum, portGroupsBmp, ingressCnt);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ingressCnt));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNstPortGroupIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum          - physical device number
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
*      ingressCntPtr - the counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNstPortGroupIngressFrwFilterDropCntrGet
(
    IN GT_U8                    devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *ingressCntPtr
)
{
    GT_U32    regAddr;           /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(ingressCntPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
                          ingrForwardingRestrictions.ingrForwardingRestrictionsDroppedPktsCntr;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                          bridgeRegs.nstRegs.ingressFrwDropCounter;
    }

    /* read ingress drop counter */
    return prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp, regAddr,
                                                  0, 32,
                                                  ingressCntPtr, NULL);
}

/*******************************************************************************
* cpssDxChNstPortGroupIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum          - physical device number
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
*      ingressCntPtr - the counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNstPortGroupIngressFrwFilterDropCntrGet
(
    IN GT_U8                    devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *ingressCntPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNstPortGroupIngressFrwFilterDropCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, ingressCntPtr));

    rc = internal_cpssDxChNstPortGroupIngressFrwFilterDropCntrGet(devNum, portGroupsBmp, ingressCntPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, ingressCntPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


