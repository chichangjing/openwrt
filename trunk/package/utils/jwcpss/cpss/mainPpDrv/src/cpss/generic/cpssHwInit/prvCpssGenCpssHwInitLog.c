/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenCpssHwInitLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/cpssHwInit/cpssLedCtrl.h>
#include <cpss/generic/cpssHwInit/private/prvCpssGenCpssHwInitLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenPpTypesLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_HW_PP_INIT_STAGE_ENT[]  =
{
    "CPSS_HW_PP_INIT_STAGE_INIT_DURING_RESET_E",
    "CPSS_HW_PP_INIT_STAGE_EEPROM_DONE_INT_MEM_DONE_E",
    "CPSS_HW_PP_INIT_STAGE_EEPROM_NOT_DONE_INT_MEM_DONE_E",
    "CPSS_HW_PP_INIT_STAGE_FULLY_FUNC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_HW_PP_INIT_STAGE_ENT);
char * prvCpssLogEnum_CPSS_HW_PP_RESET_SKIP_TYPE_ENT[]  =
{
    "CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E",
    "CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E",
    "CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E",
    "CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E",
    "CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E",
    "CPSS_HW_PP_RESET_SKIP_TYPE_ALL_E",
    "CPSS_HW_PP_RESET_SKIP_TYPE_ALL_EXCLUDE_PEX_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_HW_PP_RESET_SKIP_TYPE_ENT);
char * prvCpssLogEnum_CPSS_MAC_SA_LSB_MODE_ENT[]  =
{
    "CPSS_SA_LSB_PER_PORT_E",
    "CPSS_SA_LSB_PER_PKT_VID_E",
    "CPSS_SA_LSB_PER_VLAN_E",
    "CPSS_SA_LSB_FULL_48_BIT_GLOBAL"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_MAC_SA_LSB_MODE_ENT);
char * prvCpssLogEnum_CPSS_PP_INTERFACE_CHANNEL_ENT[]  =
{
    "CPSS_CHANNEL_PCI_E",
    "CPSS_CHANNEL_SMI_E",
    "CPSS_CHANNEL_TWSI_E",
    "CPSS_CHANNEL_PEX_E",
    "CPSS_CHANNEL_PEX_MBUS_E",
    "CPSS_CHANNEL_PEX_KERNEL_E",
    "CPSS_CHANNEL_LAST_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_PP_INTERFACE_CHANNEL_ENT);
char * prvCpssLogEnum_CPSS_AU_MESSAGE_LENGTH_ENT[]  =
{
    "CPSS_AU_MESSAGE_LENGTH_4_WORDS_E",
    "CPSS_AU_MESSAGE_LENGTH_8_WORDS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_AU_MESSAGE_LENGTH_ENT);
char * prvCpssLogEnum_CPSS_LED_BLINK_DURATION_ENT[]  =
{
    "CPSS_LED_BLINK_DURATION_0_E",
    "CPSS_LED_BLINK_DURATION_1_E",
    "CPSS_LED_BLINK_DURATION_2_E",
    "CPSS_LED_BLINK_DURATION_3_E",
    "CPSS_LED_BLINK_DURATION_4_E",
    "CPSS_LED_BLINK_DURATION_5_E",
    "CPSS_LED_BLINK_DURATION_6_E",
    "CPSS_LED_BLINK_DURATION_7_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_LED_BLINK_DURATION_ENT);
char * prvCpssLogEnum_CPSS_LED_BLINK_DUTY_CYCLE_ENT[]  =
{
    "CPSS_LED_BLINK_DUTY_CYCLE_0_E",
    "CPSS_LED_BLINK_DUTY_CYCLE_1_E",
    "CPSS_LED_BLINK_DUTY_CYCLE_2_E",
    "CPSS_LED_BLINK_DUTY_CYCLE_3_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_LED_BLINK_DUTY_CYCLE_ENT);
char * prvCpssLogEnum_CPSS_LED_BLINK_SELECT_ENT[]  =
{
    "CPSS_LED_BLINK_SELECT_0_E",
    "CPSS_LED_BLINK_SELECT_1_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_LED_BLINK_SELECT_ENT);
char * prvCpssLogEnum_CPSS_LED_CLASS_13_SELECT_ENT[]  =
{
    "CPSS_LED_CLASS_13_SELECT_LINK_DOWN_E",
    "CPSS_LED_CLASS_13_SELECT_COPPER_LINK_UP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_LED_CLASS_13_SELECT_ENT);
char * prvCpssLogEnum_CPSS_LED_CLASS_5_SELECT_ENT[]  =
{
    "CPSS_LED_CLASS_5_SELECT_HALF_DUPLEX_E",
    "CPSS_LED_CLASS_5_SELECT_FIBER_LINK_UP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_LED_CLASS_5_SELECT_ENT);
char * prvCpssLogEnum_CPSS_LED_CLOCK_OUT_FREQUENCY_ENT[]  =
{
    "CPSS_LED_CLOCK_OUT_FREQUENCY_500_E",
    "CPSS_LED_CLOCK_OUT_FREQUENCY_1000_E",
    "CPSS_LED_CLOCK_OUT_FREQUENCY_2000_E",
    "CPSS_LED_CLOCK_OUT_FREQUENCY_3000_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_LED_CLOCK_OUT_FREQUENCY_ENT);
char * prvCpssLogEnum_CPSS_LED_ORDER_MODE_ENT[]  =
{
    "CPSS_LED_ORDER_MODE_BY_PORT_E",
    "CPSS_LED_ORDER_MODE_BY_CLASS_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_LED_ORDER_MODE_ENT);
char * prvCpssLogEnum_CPSS_LED_PULSE_STRETCH_ENT[]  =
{
    "CPSS_LED_PULSE_STRETCH_0_NO_E",
    "CPSS_LED_PULSE_STRETCH_1_E",
    "CPSS_LED_PULSE_STRETCH_2_E",
    "CPSS_LED_PULSE_STRETCH_3_E",
    "CPSS_LED_PULSE_STRETCH_4_E",
    "CPSS_LED_PULSE_STRETCH_5_E",
    "CPSS_LED_PULSE_STRETCH_6_E",
    "CPSS_LED_PULSE_STRETCH_7_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_LED_PULSE_STRETCH_ENT);
char * prvCpssLogEnum_CPSS_SYS_HA_MODE_ENT[]  =
{
    "CPSS_SYS_HA_MODE_ACTIVE_E",
    "CPSS_SYS_HA_MODE_STANDBY_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_SYS_HA_MODE_ENT);
char * prvCpssLogEnum_CPSS_DRAM_BUS_WIDTH_ENT[]  =
{
    "CPSS_DRAM_BUS_WIDTH_4_E",
    "CPSS_DRAM_BUS_WIDTH_8_E",
    "CPSS_DRAM_BUS_WIDTH_16_E",
    "CPSS_DRAM_BUS_WIDTH_32_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DRAM_BUS_WIDTH_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_DRAM_FREQUENCY_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_100_MHZ_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_125_MHZ_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_166_MHZ_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_200_MHZ_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_250_MHZ_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_300_MHZ_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_667_MHZ_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_800_MHZ_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_933_MHZ_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_FREQ_1066_MHZ_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_DRAM_FREQUENCY_ENT);
PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_CPSS_DRAM_SIZE_ENT[]  =
{PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_NOT_USED_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_64KB_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_128KB_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_8MB_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_16MB_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_32MB_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_64MB_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_128MB_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_256MB_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_512MB_E),
PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(CPSS_DRAM_1GB_E)
};
PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(CPSS_DRAM_SIZE_ENT);
char * prvCpssLogEnum_CPSS_DRAM_SPEED_BIN_ENT[]  =
{
    "CPSS_DRAM_SPEED_BIN_DDR3_800D_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_800E_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1066E_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1066F_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1066G_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1333F_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1333G_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1333H_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1333J_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1600G_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1600H_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1600J_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1600K_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1866J_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1866K_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1866L_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_1866M_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_2133K_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_2133L_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_2133M_E",
    "CPSS_DRAM_SPEED_BIN_DDR3_2133N_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DRAM_SPEED_BIN_ENT);
char * prvCpssLogEnum_CPSS_DRAM_TEMPERATURE_ENT[]  =
{
    "CPSS_DRAM_TEMPERATURE_NORMAL_E",
    "CPSS_DRAM_TEMPERATURE_HIGH_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DRAM_TEMPERATURE_ENT);
char * prvCpssLogEnum_CPSS_RX_BUFF_ALLOC_METHOD_ENT[]  =
{
    "CPSS_RX_BUFF_DYNAMIC_ALLOC_E",
    "CPSS_RX_BUFF_STATIC_ALLOC_E",
    "CPSS_RX_BUFF_NO_ALLOC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_RX_BUFF_ALLOC_METHOD_ENT);
char * prvCpssLogEnum_CPSS_TX_BUFF_ALLOC_METHOD_ENT[]  =
{
    "CPSS_TX_BUFF_DYNAMIC_ALLOC_E",
    "CPSS_TX_BUFF_STATIC_ALLOC_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_TX_BUFF_ALLOC_METHOD_ENT);
char * prvCpssLogEnum_CPSS_TX_SDMA_QUEUE_MODE_ENT[]  =
{
    "CPSS_TX_SDMA_QUEUE_MODE_NORMAL_E",
    "CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_TX_SDMA_QUEUE_MODE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_LED_CLASS_MANIPULATION_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_LED_CLASS_MANIPULATION_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, invertEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, blinkEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, blinkSelect, CPSS_LED_BLINK_SELECT_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, forceEnable);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, forceData);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, pulseStretchEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, disableOnLinkDown);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_LED_CONF_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_LED_CONF_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ledOrganize, CPSS_LED_ORDER_MODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, disableOnLinkDown);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, blink0DutyCycle, CPSS_LED_BLINK_DUTY_CYCLE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, blink0Duration, CPSS_LED_BLINK_DURATION_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, blink1DutyCycle, CPSS_LED_BLINK_DUTY_CYCLE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, blink1Duration, CPSS_LED_BLINK_DURATION_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, pulseStretch, CPSS_LED_PULSE_STRETCH_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ledStart);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, ledEnd);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, clkInvert);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, class5select, CPSS_LED_CLASS_5_SELECT_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, class13select, CPSS_LED_CLASS_13_SELECT_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, invertEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ledClockFrequency, CPSS_LED_CLOCK_OUT_FREQUENCY_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_LED_GROUP_CONF_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_LED_GROUP_CONF_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, classA);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, classB);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, classC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, classD);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_REG_VALUE_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_REG_VALUE_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, addrType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, regMask);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, regVal);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, repeatCount);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_AUQ_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_AUQ_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, auDescBlock);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, auDescBlockSize);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_MULTI_NET_IF_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_MULTI_NET_IF_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_TYPE_ARRAY2_MAC(valPtr, rxSdmaQueuesConfig, CPSS_MAX_PORT_GROUPS_CNS, CPSS_MAX_RX_QUEUE_CNS,  CPSS_MULTI_NET_IF_RX_SDMA_QUEUE_STC);
    PRV_CPSS_LOG_STC_TYPE_ARRAY2_MAC(valPtr, txSdmaQueuesConfig, CPSS_MAX_PORT_GROUPS_CNS, CPSS_MAX_TX_QUEUE_CNS,  CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_NET_IF_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_NET_IF_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, txDescBlock);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, txDescBlockSize);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, rxDescBlock);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxDescBlockSize);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, rxBufInfo, CPSS_RX_BUF_INFO_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_PP_PORT_GROUP_INIT_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_PP_PORT_GROUP_INIT_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, busBaseAddr);
    PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, internalPciBase);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, hwAddr, CPSS_HW_ADDR_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, intVecNum);
    PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, intMask);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_MULTI_NET_IF_RX_SDMA_QUEUE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_MULTI_NET_IF_RX_SDMA_QUEUE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, buffAndDescAllocMethod, CPSS_RX_BUFF_ALLOC_METHOD_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfRxDesc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfRxBuff);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, buffSize);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, buffHeaderOffset);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, buffersInCachedMem);
    prvCpssLogStcLogStart(contextLib,  logType, "memData");
    prvCpssLogStcLogStart(contextLib,  logType, "staticAlloc");
    PRV_CPSS_LOG_STC_PTR_MAC((&(&valPtr->memData)->staticAlloc), buffAndDescMemPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->memData)->staticAlloc), buffAndDescMemSize);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogStart(contextLib,  logType, "dynamicAlloc");
    PRV_CPSS_LOG_STC_PTR_MAC((&(&valPtr->memData)->dynamicAlloc), buffAndDescMallocFunc);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, queueMode, CPSS_TX_SDMA_QUEUE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, buffAndDescAllocMethod, CPSS_TX_BUFF_ALLOC_METHOD_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfTxDesc);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfTxBuff);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, buffSize);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, buffersInCachedMem);
    prvCpssLogStcLogStart(contextLib,  logType, "memData");
    prvCpssLogStcLogStart(contextLib,  logType, "staticAlloc");
    PRV_CPSS_LOG_STC_PTR_MAC((&(&valPtr->memData)->staticAlloc), buffAndDescMemPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->memData)->staticAlloc), buffAndDescMemSize);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogStart(contextLib,  logType, "dynamicAlloc");
    PRV_CPSS_LOG_STC_PTR_MAC((&(&valPtr->memData)->dynamicAlloc), buffAndDescMallocFunc);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_RX_BUF_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_RX_BUF_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, allocMethod, CPSS_RX_BUFF_ALLOC_METHOD_ENT);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, bufferPercentage, CPSS_MAX_RX_QUEUE_CNS, GT_U32);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rxBufSize);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, headerOffset);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, buffersInCachedMem);
    prvCpssLogStcLogStart(contextLib,  logType, "buffData");
    prvCpssLogStcLogStart(contextLib,  logType, "staticAlloc");
    PRV_CPSS_LOG_STC_PTR_MAC((&(&valPtr->buffData)->staticAlloc), rxBufBlockPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->buffData)->staticAlloc), rxBufBlockSize);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogStart(contextLib,  logType, "dynamicAlloc");
    PRV_CPSS_LOG_STC_PTR_MAC((&(&valPtr->buffData)->dynamicAlloc), mallocFunc);
    PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->buffData)->dynamicAlloc), numOfRxBuffers);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_HW_PP_INIT_STAGE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_HW_PP_INIT_STAGE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_HW_PP_INIT_STAGE_ENT);
}
void prvCpssLogParamFunc_CPSS_HW_PP_RESET_SKIP_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_HW_PP_RESET_SKIP_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_HW_PP_RESET_SKIP_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_LED_CLASS_MANIPULATION_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_LED_CLASS_MANIPULATION_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_LED_CLASS_MANIPULATION_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_LED_CONF_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_LED_CONF_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_LED_CONF_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_LED_GROUP_CONF_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_LED_GROUP_CONF_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_LED_GROUP_CONF_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_MAC_SA_LSB_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_SA_LSB_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_MAC_SA_LSB_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_MAC_SA_LSB_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MAC_SA_LSB_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_MAC_SA_LSB_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_PP_INTERFACE_CHANNEL_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_PP_INTERFACE_CHANNEL_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_PP_INTERFACE_CHANNEL_ENT);
}

