/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChIpfixLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/ipfix/cpssDxChIpfix.h>
#include <cpss/dxCh/dxChxGen/ipfix/private/prvCpssDxChIpfixLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT[]  =
{
    "CPSS_DXCH_IPFIX_DROP_COUNT_MODE_METER_ONLY_E",
    "CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT[]  =
{
    "CPSS_DXCH_IPFIX_SAMPLING_ACTION_ALARM_E",
    "CPSS_DXCH_IPFIX_SAMPLING_ACTION_MIRROR_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT[]  =
{
    "CPSS_DXCH_IPFIX_SAMPLING_DIST_DETERMINISTIC_E",
    "CPSS_DXCH_IPFIX_SAMPLING_DIST_RANDOM_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT[]  =
{
    "CPSS_DXCH_IPFIX_SAMPLING_MODE_DISABLE_E",
    "CPSS_DXCH_IPFIX_SAMPLING_MODE_PACKET_E",
    "CPSS_DXCH_IPFIX_SAMPLING_MODE_BYTE_E",
    "CPSS_DXCH_IPFIX_SAMPLING_MODE_TIME_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT[]  =
{
    "CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ABSOLUTE_E",
    "CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_INCREMENTAL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT[]  =
{
    "CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_NONE_E",
    "CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_CLEAR_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_IPFIX_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IPFIX_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, timeStamp);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, packetCount);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, byteCount, GT_U64);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dropCounter);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, randomOffset);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, lastSampledValue, GT_U64);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, samplingWindow, GT_U64);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, samplingAction, CPSS_DXCH_IPFIX_SAMPLING_ACTION_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, logSamplingRange);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, randomFlag, CPSS_DXCH_IPFIX_SAMPLING_DIST_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, samplingMode, CPSS_DXCH_IPFIX_SAMPLING_MODE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, cpuSubCode);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IPFIX_TIMER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IPFIX_TIMER_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, nanoSecondTimer);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, secondTimer, GT_U64);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, uploadMode, CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_MODE_ENT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, timer, CPSS_DXCH_IPFIX_TIMER_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, action, CPSS_DXCH_IPFIX_WRAPAROUND_ACTION_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dropThreshold);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, packetThreshold);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, byteThreshold, GT_U64);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_IPFIX_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IPFIX_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IPFIX_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IPFIX_TIMER_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IPFIX_TIMER_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IPFIX_TIMER_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IPFIX_ENTRY_STC_PTR_ipfixEntryPtr = {
     "ipfixEntryPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPFIX_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC_PTR_uploadPtr = {
     "uploadPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC_PTR_confPtr = {
     "confPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_endIndex = {
     "endIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_startIndex = {
     "startIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IPFIX_ENTRY_STC_PTR_ipfixEntryPtr = {
     "ipfixEntryPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPFIX_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IPFIX_TIMER_STC_PTR_timerPtr = {
     "timerPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPFIX_TIMER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC_PTR_uploadPtr = {
     "uploadPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC_PTR_confPtr = {
     "confPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_uploadStatusPtr = {
     "uploadStatusPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_bmpPtr = {
     "bmpPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_eventsArr = {
     "eventsArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_eventsNumPtr = {
     "eventsNumPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixDropCountModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixTimestampUploadSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC_PTR_uploadPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixWraparoundConfSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC_PTR_confPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixCpuCodeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_CPSS_NET_RX_CPU_CODE_ENT_cpuCode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_IPFIX_ENTRY_STC_PTR_ipfixEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_GT_BOOL_reset,
    &OUT_CPSS_DXCH_IPFIX_ENTRY_STC_PTR_ipfixEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixWraparoundStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_startIndex,
    &IN_GT_U32_endIndex,
    &IN_GT_BOOL_reset,
    &OUT_GT_U32_PTR_bmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixDropCountModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_IPFIX_DROP_COUNT_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixTimerGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_IPFIX_TIMER_STC_PTR_timerPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixTimestampUploadGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_IPFIX_TIMESTAMP_UPLOAD_CONF_STC_PTR_uploadPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixWraparoundConfGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_DXCH_IPFIX_WRAPAROUND_CFG_STC_PTR_confPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixCpuCodeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_CPSS_NET_RX_CPU_CODE_ENT_PTR_cpuCodePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixPortGroupEntrySet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_CPSS_DXCH_IPFIX_ENTRY_STC_PTR_ipfixEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixPortGroupEntryGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_entryIndex,
    &IN_GT_BOOL_reset,
    &OUT_CPSS_DXCH_IPFIX_ENTRY_STC_PTR_ipfixEntryPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixPortGroupWraparoundStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &IN_GT_U32_startIndex,
    &IN_GT_U32_endIndex,
    &IN_GT_BOOL_reset,
    &OUT_GT_U32_PTR_bmpPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixTimestampUploadStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_uploadStatusPtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixAlarmEventsGet_PARAMS[];
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixPortGroupAlarmEventsGet_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChIpfixAlarmEventsGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChIpfixPortGroupAlarmEventsGet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChIpfixLogLibDb[] = {
    {"cpssDxChIpfixEntrySet", 4, cpssDxChIpfixEntrySet_PARAMS, NULL},
    {"cpssDxChIpfixEntryGet", 5, cpssDxChIpfixEntryGet_PARAMS, NULL},
    {"cpssDxChIpfixDropCountModeSet", 3, cpssDxChIpfixDropCountModeSet_PARAMS, NULL},
    {"cpssDxChIpfixDropCountModeGet", 3, cpssDxChIpfixDropCountModeGet_PARAMS, NULL},
    {"cpssDxChIpfixCpuCodeSet", 3, cpssDxChIpfixCpuCodeSet_PARAMS, NULL},
    {"cpssDxChIpfixCpuCodeGet", 3, cpssDxChIpfixCpuCodeGet_PARAMS, NULL},
    {"cpssDxChIpfixAgingEnableSet", 3, prvCpssLogGenDevNumStageEnable_PARAMS, NULL},
    {"cpssDxChIpfixAgingEnableGet", 3, prvCpssLogGenDevNumStageEnablePtr_PARAMS, NULL},
    {"cpssDxChIpfixWraparoundConfSet", 3, cpssDxChIpfixWraparoundConfSet_PARAMS, NULL},
    {"cpssDxChIpfixWraparoundConfGet", 3, cpssDxChIpfixWraparoundConfGet_PARAMS, NULL},
    {"cpssDxChIpfixWraparoundStatusGet", 6, cpssDxChIpfixWraparoundStatusGet_PARAMS, NULL},
    {"cpssDxChIpfixAgingStatusGet", 6, cpssDxChIpfixWraparoundStatusGet_PARAMS, NULL},
    {"cpssDxChIpfixTimestampUploadSet", 3, cpssDxChIpfixTimestampUploadSet_PARAMS, NULL},
    {"cpssDxChIpfixTimestampUploadGet", 3, cpssDxChIpfixTimestampUploadGet_PARAMS, NULL},
    {"cpssDxChIpfixTimestampUploadTrigger", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssDxChIpfixTimestampUploadStatusGet", 2, cpssDxChIpfixTimestampUploadStatusGet_PARAMS, NULL},
    {"cpssDxChIpfixTimerGet", 3, cpssDxChIpfixTimerGet_PARAMS, NULL},
    {"cpssDxChIpfixAlarmEventsGet", 4, cpssDxChIpfixAlarmEventsGet_PARAMS, cpssDxChIpfixAlarmEventsGet_preLogic},
    {"cpssDxChIpfixPortGroupEntrySet", 5, cpssDxChIpfixPortGroupEntrySet_PARAMS, NULL},
    {"cpssDxChIpfixPortGroupEntryGet", 6, cpssDxChIpfixPortGroupEntryGet_PARAMS, NULL},
    {"cpssDxChIpfixPortGroupWraparoundStatusGet", 7, cpssDxChIpfixPortGroupWraparoundStatusGet_PARAMS, NULL},
    {"cpssDxChIpfixPortGroupAgingStatusGet", 7, cpssDxChIpfixPortGroupWraparoundStatusGet_PARAMS, NULL},
    {"cpssDxChIpfixPortGroupAlarmEventsGet", 5, cpssDxChIpfixPortGroupAlarmEventsGet_PARAMS, cpssDxChIpfixPortGroupAlarmEventsGet_preLogic},
    {"cpssDxChIpfixTimestampToCpuEnableSet", 2, prvCpssLogGenDevNumEnable_PARAMS, NULL},
    {"cpssDxChIpfixTimestampToCpuEnableGet", 2, prvCpssLogGenDevNumEnablePtr_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_IPFIX(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChIpfixLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChIpfixLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

