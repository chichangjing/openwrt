/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenDragoniteManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/dragonite/private/prvCpssGenDragoniteLog.h>
#include <cpss/generic/dragonite/cpssGenDragonite.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_GEN_DRAGONITE_DATA_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_GEN_DRAGONITE_DATA_TYPE_ENT dataType;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_GEN_DRAGONITE_DATA_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, dataType, CPSS_GEN_DRAGONITE_DATA_TYPE_ENT);
    dataType = valPtr->dataType;
    prvCpssLogStcLogStart(contextLib,  logType, "dragoniteData");
    switch (dataType) {
        case CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E:
            PRV_CPSS_LOG_STC_STC_MAC((&valPtr->dragoniteData), system, CPSS_GEN_DRAGONITE_SYSTEM_STC);
            break;
        case CPSS_GEN_DRAGONITE_DATA_TYPE_CHIP_E:
            PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->dragoniteData), chip, CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS, CPSS_GEN_DRAGONITE_CHIP_STC);
            break; 
        case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_COMMANDS_E:
            PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->dragoniteData), portCmd, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_COMMAND_STC);
            break;
        case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_CNTRS_E:
            PRV_CPSS_LOG_STC_STC_MAC((&valPtr->dragoniteData), portCntrs, CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC);
            break;
        case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_STATUS_E:
            PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->dragoniteData), portStat, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_STATUSES_STC);
            break;
        case CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_MEASURE_E:
            PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->dragoniteData), portMeasure, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC);
            break;
        case CPSS_GEN_DRAGONITE_DATA_TYPE_CONFIG_E:
            PRV_CPSS_LOG_STC_STC_MAC((&valPtr->dragoniteData), config, CPSS_GEN_DRAGONITE_CONFIG_STC);
            break;
        case CPSS_GEN_DRAGONITE_DATA_TYPE_LAYER2_E:
            PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->dragoniteData), layer2, CPSS_GEN_DRAGONITE_PORTS_NUM_CNS, CPSS_GEN_DRAGONITE_PORT_LAYER2_STC);
            break;
        case CPSS_GEN_DRAGONITE_DATA_TYPE_DEBUG_E:
            PRV_CPSS_LOG_STC_PTR_MAC((&valPtr->dragoniteData), debugDataPtr);
            break;    
        case CPSS_GEN_DRAGONITE_DATA_TYPE_OTHER_E:
            PRV_CPSS_LOG_STC_PTR_MAC((&valPtr->dragoniteData), otherDataPtr);
            break; 
        default:
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}

/********* parameters log functions *********/

/********* api pre-log functions *********/


