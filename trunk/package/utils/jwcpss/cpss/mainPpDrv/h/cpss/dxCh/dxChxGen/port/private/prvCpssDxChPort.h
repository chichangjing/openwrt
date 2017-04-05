/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPort.h
*
* DESCRIPTION:
*       Includes structures definition for the use of CPSS DxCh Port lib .
*
*
* FILE REVISION NUMBER:
*       $Revision: 58 $
*
*******************************************************************************/
#ifndef __prvCpssDxChPorth
#define __prvCpssDxChPorth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/generic/port/cpssPortTx.h>
#include <cpss/generic/port/private/prvCpssPortTypes.h>

/* max number ports in cheetah */
#define PRV_CPSS_CHEETAH_MAX_PORTS_NUM_CNS      27

/* max number ports in cheetah 2 */
#define PRV_CPSS_CHEETAH2_MAX_PORTS_NUM_CNS     28

/* The size of XLG ports MAC MIB counters entry in words */
#define PRV_CPSS_XLG_MIB_COUNTERS_ENTRY_SIZE_CNS 32

/* The size of RX ILKN ports MAC MIB counters entry in words */
#define PRV_CPSS_ILKN_RX_MIB_COUNTERS_ENTRY_SIZE_CNS 16


/* defines the maximal number of Port Drop Profiles according
   to Cheetah device type */
#define PRV_CPSS_DXCH_PORT_TX_DROP_PROFILE_MAX_MAC(_devNum)                     \
   (((PRV_CPSS_PP_MAC(_devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E )?       \
        ((!PRV_CPSS_SIP_5_CHECK_MAC(devNum)) ? \
        CPSS_PORT_TX_DROP_PROFILE_8_E : CPSS_PORT_TX_DROP_PROFILE_16_E) :       \
        CPSS_PORT_TX_DROP_PROFILE_4_E)

/* check the Drop Profile set range */
#define PRV_CPSS_DXCH_DROP_PROFILE_CHECK_MAC(profileSet,devNum) \
        if ((GT_U32)(profileSet) >                                      \
            (GT_U32)PRV_CPSS_DXCH_PORT_TX_DROP_PROFILE_MAX_MAC(devNum)) \
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* defines maximal number of Ports, depending on the type of the Cheetah */
#define PRV_CPSS_DXCH_CPU_INDEX_MAC(_devNum) \
        ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E) ? \
         PRV_CPSS_CHEETAH2_MAX_PORTS_NUM_CNS : \
         PRV_CPSS_CHEETAH_MAX_PORTS_NUM_CNS)

/* get the port mac register address - for register with name passed as parameter */
#define PRV_CPSS_DXCH_PORT_MAC_VAR_REG_MAC(devNum,portNum,portMacType,regName,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.\
                        macRegsPerType[PRV_CPSS_PORT_GE_E].regName;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].\
                            macRegsPerType[portMacType].regName

/* get the register address - port mac control */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.\
                        macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].\
                            macRegsPerType[portMacType].macCtrl

/* get the register address - port mac control register1 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs. \
            macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl1;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].\
                            macRegsPerType[portMacType].macCtrl1

/* get the register address - port mac control register2 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.  \
            macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl2; \
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum]. \
        macRegsPerType[portMacType].macCtrl2

/* get the register address - port mac control register3 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.  \
             macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl3; \
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum]. \
        macRegsPerType[portMacType].macCtrl3

/* get the register address - port mac control register4 */
#define PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.  \
             macRegsPerType[PRV_CPSS_PORT_GE_E].macCtrl4; \
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum]. \
        macRegsPerType[portMacType].macCtrl4

#define PRV_CPSS_DXCH_PORT_MAC_CTRL5_REG_MAC(devNum,portNum,portMacType,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_SW_PTR_ENTRY_UNUSED;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum]. \
        macRegsPerType[portMacType].macCtrl5;\

/* get the register address - Auto-Negotiation Configuration Register */
#define PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.autoNegCtrl;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].autoNegCtrl

/* get the register address - Port<n> Status Register0 */
#define PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.macStatus;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].macStatus

/* get the register address - Port<n> Serial Parameters Configuration Register */
#define PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portNum,regAddrPtr)\
    if(((portNum) == CPSS_CPU_PORT_NUM_CNS) && (PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.dedicatedCpuMac.isNotSupported == GT_FALSE)) \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.cpuPortRegs.serialParameters;\
    else                                                                        \
        *(regAddrPtr) = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters

/* get the register address - port TXQ configuration */
#define PRV_CPSS_DXCH_PORT_TXQ_REG_MAC(devNum,portNum,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,0,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TX_CONFIG_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - port Token bucket configuration */
#define PRV_CPSS_DXCH_PORT_TOKEN_BUCKET_REG_MAC(devNum,portNum,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,0,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - port , Queue -- Token bucket configuration */
#define PRV_CPSS_DXCH_PORT_Q_TOKEN_BUCKET_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,tcQueue,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - port Descriptors Counter Register */
#define PRV_CPSS_DXCH_PORT_DESCR_COUNT_REG_MAC(devNum,portNum,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,0,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_DESCR_COUNTER_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - port Buffers Counter Register */
#define PRV_CPSS_DXCH_PORT_BUFFER_COUNT_REG_MAC(devNum,portNum,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,0,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_BUFFER_COUNTER_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - port TC Descriptors Counter Register */
#define PRV_CPSS_DXCH_PORT_TC_DESCR_COUNT_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,tcQueue,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_DESCR_COUNTER_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - port TC Buffers Counter Register */
#define PRV_CPSS_DXCH_PORT_TC_BUFFER_COUNT_REG_MAC(devNum,portNum,tcQueue,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,portNum,tcQueue,PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_BUFFER_COUNTER_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - WRR_WEIGHTS_0 Register */
#define PRV_CPSS_DXCH_PORT_WRR_WEIGHTS_0_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_0_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - WRR_WEIGHTS_1 Register */
#define PRV_CPSS_DXCH_PORT_WRR_WEIGHTS_1_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_1_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - DESCR_LIMIT Register */
#define PRV_CPSS_DXCH_PORT_WRR_STRICT_PRIO_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - DESCR_LIMIT_EN Register */
#define PRV_CPSS_DXCH_PORT_WRR_STRICT_PRIO_EN_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_EN_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* get the register address - DESCR_LIMIT Register */
#define PRV_CPSS_DXCH_PORT_DESCR_LIMIT_REG_MAC(devNum,profile,regAddrPtr)\
    if(GT_OK != prvCpssDxChPortTxRegisterAddrGet(devNum,profile,0,PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_DESCR_LIMIT_E,regAddrPtr))\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG)

/* defines the maximal number of Scheduler Profiles according
   to Cheetah device type */
#define PRV_CPSS_DXCH_PORT_TX_SCHEDULER_PROFILE_MAX_MAC(_devNum)                     \
    (((PRV_CPSS_PP_MAC(_devNum)->devFamily) != CPSS_PP_FAMILY_CHEETAH_E) ?           \
     ((PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(_devNum)) ? CPSS_PORT_TX_SCHEDULER_PROFILE_16_E : CPSS_PORT_TX_SCHEDULER_PROFILE_8_E) : \
      CPSS_PORT_TX_SCHEDULER_PROFILE_4_E)

/* check the Tx Queue scheduler profile set range */
#define PRV_CPSS_DXCH_SCHEDULER_PROFILE_CHECK_MAC(profileSet,devNum) \
        if ((GT_U32)(profileSet) >                                           \
            (GT_U32)PRV_CPSS_DXCH_PORT_TX_SCHEDULER_PROFILE_MAX_MAC(devNum)) \
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

typedef enum{
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TX_CONFIG_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TOKEN_BUCK_LEN_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_PRIO_TOKEN_BUCK_LEN_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_DESCR_COUNTER_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_BUFFER_COUNTER_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_DESCR_COUNTER_E,
    PRV_CPSS_DXCH_PORT_TX_REG_TYPE_TC_BUFFER_COUNTER_E,

    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_0_E,
    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_WEIGHTS_1_E,
    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_E,
    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_WRR_STRICT_PRIO_EN_E,
    PRV_CPSS_DXCH_PROFILE_TX_REG_TYPE_DESCR_LIMIT_E
}PRV_CPSS_DXCH_PORT_TX_REG_TYPE_ENT;

/*
 * typedef: enum PRV_CPSS_DXCH_PORT_SERDES_REG_OFFSET_ENT
 *
 * Description: Corresponds to place of serdes configuration register address
 *              in _lpSerdesConfig and must reflect changes there
 *
 * Enumerations:
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG1_OFFSET_E - PLL/INTP Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_OFFSET_E - PLL/INTP Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_OFFSET_E - PLL/INTP Register 3
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG4_OFFSET_E - PLL/INTP Register 4
 *   PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG5_OFFSET_E - PLL/INTP Register 5
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_OFFSET_E - Callibration Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG1_OFFSET_E - Calibration Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG2_OFFSET_E - Calibration Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_OFFSET_E - Calibration Register 3
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG5_OFFSET_E - Calibration Register 5
 *   PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_OFFSET_E - Calibration Register 7
 *   PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_OFFSET_E   - Transmit Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_OFFSET_E   - Transmit Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_OFFSET_E   - Transmit Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG0_OFFSET_E   - Reciever Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG1_OFFSET_E   - Reciever Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG2_OFFSET_E   - Reciever Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_OFFSET_E        - FFE Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_ANALOG_REG0_OFFSET_E     - Analog Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_SLC_REG_OFFSET_E         - SLC Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_REFERENCE_REG1_OFFSET_E  - Reference Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_POWER_REG0_OFFSET_E       - Power Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_OFFSET_E      - Reset Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_DIGITAL_IF_REG0_OFFSET_E - Digital interface Reg. 0
 *   PRV_CPSS_DXCH_PORT_SERDES_SA_CTRL_REG0_OFFSET_E - Standalone Control Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_OFFSET_E - Serdes External Configuration 1(1 for xCat\0 for Lion)
 *   PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_OFFSET_E - Serdes External Configuration 2(2 for xCat\1 For Lion)
 *   PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG3_OFFSET_E - Serdes External Configuration 3(2 For Lion)
 *   PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_REG0_OFFSET_E - PHY Test Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_REG1_OFFSET_E - PHY Test Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_DATA_REG5_OFFSET_E - PHY Test Data Register 5
 *   PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_PRBS_CNTR_REG2_OFFSET_E - PHY Test PRBS Counter Register 2
 *   PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_PRBS_ERROR_CNTR_REG0_OFFSET_E - PHY Test
 *                                                  PRBS Error Counter Register 0
 *   PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_PRBS_ERROR_CNTR_REG1_OFFSET_E - PHY Test
 *                                                  PRBS Error Counter Register 1
 *   PRV_CPSS_DXCH_PORT_SERDES_DFE_F0F1_COEFFICIENT_CTRL_OFFSET_E - DFE F0-F1 Coefficient Control Reg.
 *   PRV_CPSS_DXCH_PORT_SERDES_REG_OFFSET_NUM_E         - Number of serdes registers
 *                                                          in structure
 */
typedef enum
{
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG1_OFFSET_E,   /* 0 */
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_OFFSET_E,   /* 1 */
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_OFFSET_E,   /* 2 */
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG4_OFFSET_E,   /* 3 */
    PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG5_OFFSET_E,   /* 4 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_OFFSET_E, /* 5 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG1_OFFSET_E, /* 6 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG2_OFFSET_E, /* 7 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_OFFSET_E, /* 8 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG5_OFFSET_E, /* 9 */
    PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_OFFSET_E, /* 10 */
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_OFFSET_E,   /* 11 */
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_OFFSET_E,   /* 12 */
    PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_OFFSET_E,   /* 13 */
    PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG0_OFFSET_E,   /* 14 */
    PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG1_OFFSET_E,   /* 15 */
    PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG2_OFFSET_E,   /* 16 */
    PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_OFFSET_E,        /* 17 */
    PRV_CPSS_DXCH_PORT_SERDES_ANALOG_REG0_OFFSET_E,     /* 18 */
    PRV_CPSS_DXCH_PORT_SERDES_SLC_REG_OFFSET_E,         /* 19 */
    PRV_CPSS_DXCH_PORT_SERDES_REFERENCE_REG1_OFFSET_E,  /* 20 */
    PRV_CPSS_DXCH_PORT_SERDES_POWER_REG0_OFFSET_E,      /* 21 */
    PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_OFFSET_E,      /* 22 */
    PRV_CPSS_DXCH_PORT_SERDES_DIGITAL_IF_REG0_OFFSET_E, /* 23 */
    PRV_CPSS_DXCH_PORT_SERDES_SA_CTRL_REG0_OFFSET_E,    /* 24 */
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_OFFSET_E,    /* 25 */
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_OFFSET_E,    /* 26 */
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG3_OFFSET_E,    /* 27 */
    PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_REG0_OFFSET_E,   /* 28 */
    PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_REG1_OFFSET_E,   /* 29 */
    PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_DATA_REG5_OFFSET_E, /* 30 */
    PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_PRBS_CNTR_REG2_OFFSET_E, /* 31 */
    PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_PRBS_ERROR_CNTR_REG0_OFFSET_E, /* 32 */
    PRV_CPSS_DXCH_PORT_SERDES_PHY_TEST_PRBS_ERROR_CNTR_REG1_OFFSET_E, /* 33 */
    PRV_CPSS_DXCH_PORT_SERDES_DFE_F0F1_COEFFICIENT_CTRL_OFFSET_E, /* 34 */
    PRV_CPSS_DXCH_PORT_SERDES_REG_OFFSET_NUM_E
} PRV_CPSS_DXCH_PORT_SERDES_REG_OFFSET_ENT;

/*
 * typedef: enum PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_ENT
 *
 * Description: Defines SERDES external configuration:
 *              Pre-init and post-init.
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_0_E  - Serdes External Configuration 2/0 Register
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E    - Serdes External Configuration 1 Register
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_1_E  - Serdes External Configuration 2/1 Register
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_2_E  - Serdes External Configuration 2/2 Register
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_3_E  - Serdes External Configuration 2/3 Register
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_4_E  - Serdes External Configuration 2/4 Register
 *  PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E     - Number of Serdes External Registers
 */
typedef enum
{
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_0_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_1_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_2_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_3_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_4_E,
    PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E
} PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_ENT;

/* type for pointer to ...SerdesPowerUpSequence matrix */
typedef GT_U32 PRV_CPSS_DXCH_PORT_SERDES_POWER_UP_ARRAY[CPSS_DXCH_PORT_SERDES_SPEED_NA_E+2];

/* type for pointer to lpSerdesExtConfig... matrix */
typedef GT_U32 PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_ARRAY[2];

/* Values for xCat A2 and above Short Reach 125MHz SerDes RefClk */
extern GT_U32 xcatSerdesPowerUpSequence[][CPSS_DXCH_PORT_SERDES_SPEED_NA_E+2];

/* Values for xCat A1 Short Reach 125MHz SerDes RefClk */
extern GT_U32 xcatA1SerdesPowerUpSequence[][CPSS_DXCH_PORT_SERDES_SPEED_NA_E+2];

/* Values for xCat2 Short Reach 125MHz SerDes RefClk */
extern GT_U32 xcat2SerdesPowerUpSequence[][CPSS_DXCH_PORT_SERDES_SPEED_NA_E+2];
/* Values for Lion B0 Long Reach 156.25MHz SerDes RefClk */
extern GT_U32 lionSerdesPowerUpSequence[][CPSS_DXCH_PORT_SERDES_SPEED_NA_E+2];

/* Values for xCat External registers configuration */
extern GT_U32 lpSerdesExtConfig[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2];
/* Values for Lion B0 External registers configuration */
extern GT_U32 lpSerdesExtConfig_Lion_B0_156Clk_LR[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2];
/* Values for xCat2 External registers configuration */
extern GT_U32 lpSerdesExtConfig_xCat2_125Clk[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2];

/*******************************************************************************
* PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN
*
* DESCRIPTION:
*       Sets Interface mode on a specified port.
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       ifMode - interface mode
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN)
(
    IN  GT_U8                           dev,
    IN  GT_PHYSICAL_PORT_NUM            port,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode
);

/*******************************************************************************
* PRV_CPSS_DXCH_PORT_INTERFACE_MODE_GET_FUN
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       ifModePtr - (ptr to) current interface mode
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_BAD_PTR        - on null pointer
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_PORT_INTERFACE_MODE_GET_FUN)
(
    IN  GT_U8                           dev,
    IN  GT_PHYSICAL_PORT_NUM            port,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    *ifModePtr
);

/*******************************************************************************
* PRV_CPSS_DXCH_PORT_SPEED_SET_FUN
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_PORT_SPEED_SET_FUN)
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

/*******************************************************************************
* PRV_CPSS_DXCH_PORT_SPEED_GET_FUN
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*
* OUTPUTS:
*       speedPtr - port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_PORT_SPEED_GET_FUN)
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_PORT_SPEED_ENT  *speedPtr
);

/******************************************************************************
* PRV_CPSS_DXCH_PORT_SERDES_POWER_STATUS_SET_FUN
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes according to port capabilities.
*       XG / XAUI ports: All 4 (TX or RX) lanes are set simultanuously.
*       HX / QX ports: lanes 0,1 or TX and RX may be set separately.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       direction - may be either CPSS_PORT_DIRECTION_RX_E,
*                                 CPSS_PORT_DIRECTION_TX_E, or
*                                 CPSS_PORT_DIRECTION_BOTH_E.
*       lanesBmp  - bitmap of SERDES lanes (bit 0-> lane 0, etc.)
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     DxCh3 and above devices supports only CPSS_PORT_DIRECTION_BOTH_E.
*
******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_PORT_SERDES_POWER_STATUS_SET_FUN)
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  lanesBmp,
    IN  GT_BOOL                 powerUp
);

/*******************************************************************************
 * Typedef: struct PRV_CPSS_DXCH_PORT_OBJ_STC
 *
 * Description: A structure to hold PORT methods entry points
 *
 *
 * Fields:
 *      setPortInterfaceMode    - set port interface mode
 *      getPortInterfaceMode    - get current interface mode of port
 *      setPortSpeed            - set port speed
 *      getPortSpeed            - get port speed
 *      setSerdesPowerStatus    - set serdes power status
 *                                  (power down or run power up sequence)
 *
 */
typedef struct
{
    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_SET_FUN   setPortInterfaceMode[CPSS_PORT_INTERFACE_MODE_NA_E];
    PRV_CPSS_DXCH_PORT_INTERFACE_MODE_GET_FUN   getPortInterfaceMode;
    PRV_CPSS_DXCH_PORT_SPEED_SET_FUN            setPortSpeed[PRV_CPSS_XG_PORT_OPTIONS_MAX_E];
    PRV_CPSS_DXCH_PORT_SPEED_GET_FUN            getPortSpeed;
    PRV_CPSS_DXCH_PORT_SERDES_POWER_STATUS_SET_FUN setSerdesPowerStatus;

} PRV_CPSS_DXCH_PORT_OBJ_STC, *PRV_CPSS_DXCH_PORT_OBJ_PTR;

/*******************************************************************************
* prvCpssDxChLpPreInitSerdesSequence
*
* DESCRIPTION:
*       Make pre-init sequence for given serdes.
*
* INPUTS:
*       devNum              - physical device number
*       portGroupId         - ports group number
*       serdesNum           - given SERDES number
*       serdesExtCnfg1Value - value for Serdes External Cnfg 1 register .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpPreInitSerdesSequence
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               serdesNum,
    IN  GT_U32               serdesExtCnfg1Value
);

/*******************************************************************************
* prvCpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state    - Reset state
*               GT_TRUE   - Port MAC is under Reset
*               GT_FALSE - Port MAC is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL   state
);

/*******************************************************************************
* prvCpssDxChPortMacResetStateGet
*
* DESCRIPTION:
*       Get MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (including CPU)
*
* OUTPUTS:
*       statePtr - Reset state
*               GT_TRUE   - Port MAC is under Reset
*               GT_FALSE - Port MAC is Not under Reset, normal operation
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - statePtr == NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacResetStateGet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL   *statePtr
);

/*******************************************************************************
* prvCpssDxChPortXgPcsResetStateSet
*
* DESCRIPTION:
*       Set XPCS Reset state of XG unit including internal features.
*
* APPLICABLE DEVICES:
*       DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       state    - Reset state
*               GT_TRUE   - Port Xpcs is under Reset
*               GT_FALSE - Port Xpcs is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       XLG doesn't need such treatment, because it has just one bit for reset.
*       This API for internal usage purposed to run on start and finish of port
*           ifMode & speed reconfiguration.
*******************************************************************************/
GT_STATUS prvCpssDxChPortXgPcsResetStateSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 state
);

/*******************************************************************************
* prvCpssDxChPortXgPcsResetStateGet
*
* DESCRIPTION:
*       Set XPCS Reset state of XG unit including internal features.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat2.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*
* OUTPUTS:
*       statePtr - Current reset state
*               GT_TRUE   - Port Xpcs is under Reset
*               GT_FALSE - Port Xpcs is Not under Reset, normal operation
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - statePtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortXgPcsResetStateGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *statePtr
);

/*******************************************************************************
* prvCpssDxChLpCheckAndInitNeededSerdes
*
* DESCRIPTION:
*       Make serdes initialization and enable access to serdes registers
*
* INPUTS:
*       devNum             - physical device number
*       portGroupId        - ports group number
*       startSerdes        - start SERDES number
*       serdesesNum        - number of  SERDES per port
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpCheckAndInitNeededSerdes
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               startSerdes,
    IN  GT_U32               serdesesNum
);

/*******************************************************************************
* prvCpssDxCh3PortSerdesPowerUpDownGet
*
* DESCRIPTION:
*       Get power up or down state to port and serdes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       powerUpPtr     - (pointer to) power up state.
*                           GT_TRUE  - power up
*                           GT_FALSE - power down
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3PortSerdesPowerUpDownGet
(
    IN  GT_U8      devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL   *powerUpPtr
);

/*******************************************************************************
* prvCpssDxChPortTxRegisterAddrGet
*
* DESCRIPTION:
*       get register address for 'txq per port' .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number.
*       primaryIndex - primary index , can be used as :
*               port number (also CPU port)
*               or as profile number
*       secondaryIndex - secondary index , can be used as TC
*       registerType - register type
* OUTPUTS:
*       regAddrPtr - (pointer to) register address
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTxRegisterAddrGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   primaryIndex,
    IN  GT_U32   secondaryIndex,
    IN  PRV_CPSS_DXCH_PORT_TX_REG_TYPE_ENT  registerType,
    OUT GT_U32   *regAddrPtr
);

/*******************************************************************************
* prvCpssDxChPortPfcCascadeEnableSet
*
* DESCRIPTION:
*       Enable/Disable insertion of DSA tag for PFC frames
*       transmitted on given port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*       enable     - GT_TRUE - PFC packets are transmitted with a DSA tag.
*                    GT_FALSE - PFC packets are transmitted without DSA tag.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPfcCascadeEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL enable
);

/*******************************************************************************
* prvCpssDxChPortIfModeCheckAndSerdesNumGet
*
* DESCRIPTION:
*       Get number of first serdes and quantity of serdeses occupied by given
*       port in given interface mode
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - port media interface mode
*
* OUTPUTS:
*       startSerdesPtr         - first used serdes number
*       numOfSerdesLanesPtr    - quantity of serdeses occupied
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - if port doesn't support given interface mode
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortIfModeCheckAndSerdesNumGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    OUT GT_U32                          *startSerdesPtr,
    OUT GT_U32                          *numOfSerdesLanesPtr
);

/*******************************************************************************
* prvCpssDxChPortIfModeSerdesNumGet
*
* DESCRIPTION:
*       Get number of first serdes and quantity of serdeses occupied by given
*       port in given interface mode. Extended function used directly only in
*       special cases when naturally error would be returned.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - port media interface mode
*
* OUTPUTS:
*       startSerdesPtr      - first used serdes number
*       numOfSerdesLanesPtr - quantity of serdeses occupied
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - if port doesn't support given interface mode
*       GT_NOT_SUPPORTED         - wrong device family
*
* COMMENTS:
*       While working on Lion2/3 prior to call this function, port speed must
*       be defined.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortIfModeSerdesNumGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    OUT GT_U32                          *startSerdesPtr,
    OUT GT_U32                          *numOfSerdesLanesPtr
);

/*******************************************************************************
* prvCpssDxChPortNumberOfSerdesLanesGet
*
* DESCRIPTION:
*       Get number of first SERDES and quantity of SERDESes occupied by given
*       port.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       firstSerdesPtr         - (pointer to) first used SERDES number
*       numOfSerdesLanesPtr    - (pointer to) quantity of SERDESes occupied
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_SUPPORTED         - wrong device family
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortNumberOfSerdesLanesGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    OUT GT_U32                          *firstSerdesPtr,
    OUT GT_U32                          *numOfSerdesLanesPtr
);

/*******************************************************************************
* prvCpssDxChPortTxQueueingEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical or CPU port number
*       tcQueue - traffic class queue on this device (0..7)
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTxQueueingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U8     tcQueue,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* prvCpssDxChPortEnableSet
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL   enable
);

/*******************************************************************************
* prvCpssDxChPortPcsLoopbackEnableSet
*
* DESCRIPTION:
*       Set the PCS Loopback state in the packet processor MAC port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPcsLoopbackEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* prvCpssDxChPortEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status of a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       statePtr - Pointer to the Get Enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_PHYSICAL_PORT_NUM     portNum,
    OUT  GT_BOOL   *statePtr
);

/*******************************************************************************
* prvCpssTxPortBindPortToDpSet
*
* DESCRIPTION:
*       Bind a Physical Port to a specific Drop Profile Set.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - physical or CPU port number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortTxBindPortToDpSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   profileSet
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChPorth */

