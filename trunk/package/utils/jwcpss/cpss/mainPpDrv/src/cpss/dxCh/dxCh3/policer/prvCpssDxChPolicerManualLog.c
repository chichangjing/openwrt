/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPolicerManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxCh3/policer/private/prvCpssDxChPolicerLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>
#include <cpss/generic/policer/private/prvCpssGenPolicerLog.h>

/* flag for mark illegal value of the union key*/
#define ILLEGAL_UNION_KEY_CNS      200
/* max length of parameter name (include array element) */ 
#define PRV_LOG_STRING_NAME_BUFFER_SIZE_CNS 160

/********* enums *********/
/********* structure fields log functions *********/
/********* parameters log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH3_POLICER_METER_MODE_ENT meterMode;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT *, valPtr);
    meterMode = (CPSS_DXCH3_POLICER_METER_MODE_ENT)inOutParamInfoPtr->paramKey.paramKeyArr[4];

    switch (meterMode)
    {
        case CPSS_DXCH3_POLICER_METER_MODE_SR_TCM_E:
            prvCpssLogStcLogStart(contextLib,  logType, "srTcmParams");
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->srTcmParams), cir);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->srTcmParams), cbs);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->srTcmParams), ebs);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        case CPSS_DXCH3_POLICER_METER_MODE_TR_TCM_E:
        case CPSS_DXCH3_POLICER_METER_MODE_MEF0_E:
        case CPSS_DXCH3_POLICER_METER_MODE_MEF1_E:
            prvCpssLogStcLogStart(contextLib,  logType, "trTcmParams");
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->trTcmParams), cir);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->trTcmParams), cbs);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->trTcmParams), pir);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->trTcmParams), pbs);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        case CPSS_DXCH3_POLICER_METER_MODE_START_OF_ENVELOPE_E:
        case CPSS_DXCH3_POLICER_METER_MODE_NOT_START_OF_ENVELOPE_E:
            prvCpssLogStcLogStart(contextLib,  logType, "envelope");
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->envelope), cir);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->envelope), cbs);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->envelope), eir);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->envelope), ebs);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->envelope), maxCir);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->envelope), maxEir);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        default:
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT_PTR);
}
void prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH3_POLICER_METERING_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, countingEntryIndex);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, mngCounterSet, CPSS_DXCH3_POLICER_MNG_CNTR_SET_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, meterColorMode, CPSS_POLICER_COLOR_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, meterMode, CPSS_DXCH3_POLICER_METER_MODE_ENT);
    inOutParamInfoPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH3_POLICER_METER_MODE_ENT)valPtr->meterMode;
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, tokenBucketParams, CPSS_DXCH3_POLICER_METER_TB_PARAMS_UNT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyUp, CPSS_DXCH_POLICER_MODIFY_UP_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyDscp, CPSS_DXCH_POLICER_MODIFY_DSCP_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyDp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyExp, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, modifyTc, CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, greenPcktCmd, CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, yellowPcktCmd, CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, redPcktCmd, CPSS_DXCH3_POLICER_NON_CONFORM_CMD_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosProfile);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, remarkMode, CPSS_DXCH_POLICER_REMARK_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, byteOrPacketCountingMode, CPSS_DXCH3_POLICER_METER_RESOLUTION_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, packetSizeMode, CPSS_POLICER_PACKET_SIZE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, tunnelTerminationPacketSizeMode, CPSS_DXCH3_POLICER_TT_PACKET_SIZE_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dsaTagCountingMode, CPSS_DXCH3_POLICER_DSA_TAG_COUNTING_MODE_ENT);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, timeStampCountingMode, CPSS_DXCH3_POLICER_TS_TAG_COUNTING_MODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, yellowEcnMarkingEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, couplingFlag);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, maxRateIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFunc_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH3_POLICER_METERING_ENTRY_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH3_POLICER_METERING_ENTRY_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH3_POLICER_METERING_ENTRY_STC_PTR);
}

/********* api pre-log functions *********/
void cpssDxCh3PolicerMeteringEntrySet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC *tmp;
    /* devNum + stage + entryIndex*/
    va_arg(args, GT_U32);
    va_arg(args, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
    va_arg(args, GT_U32);
    /* meterMode */
    tmp = (CPSS_DXCH3_POLICER_METERING_ENTRY_STC *)va_arg(args, CPSS_DXCH3_POLICER_METERING_ENTRY_STC *);
    if (tmp != NULL) 
        paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH3_POLICER_METER_MODE_ENT)tmp->meterMode; 
    else
        paramDataPtr->paramKey.paramKeyArr[4] = ILLEGAL_UNION_KEY_CNS;
    /* setting number of unions to be 1 */  
    paramDataPtr->paramKey.paramKeyArr[1] = 1;
}

void cpssDxChPolicerPortGroupMeteringEntrySet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    CPSS_DXCH3_POLICER_METERING_ENTRY_STC *tmp;
    /* devNum + portGroupsBmp + stage + entryIndex */
    va_arg(args, GT_U32);
    va_arg(args, GT_PORT_GROUPS_BMP);
    va_arg(args, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
    va_arg(args, GT_U32);
    /* meterMode */
    tmp = (CPSS_DXCH3_POLICER_METERING_ENTRY_STC *)va_arg(args, CPSS_DXCH3_POLICER_METERING_ENTRY_STC *);
    if (tmp != NULL) 
        paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH3_POLICER_METER_MODE_ENT)tmp->meterMode; 
    else
        paramDataPtr->paramKey.paramKeyArr[4] = ILLEGAL_UNION_KEY_CNS;
    /* setting number of unions to be 1 */   
    paramDataPtr->paramKey.paramKeyArr[1] = 1;
}
void cpssDxCh3PolicerEntryMeterParamsCalculate_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum + stage */
    va_arg(args, GT_U32);
    va_arg(args, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
    /* meterMode */
    paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH3_POLICER_METER_MODE_ENT)va_arg(args, CPSS_DXCH3_POLICER_METER_MODE_ENT);
    /* setting number of unions to be 1 */  
    paramDataPtr->paramKey.paramKeyArr[1] = 1;
}
void cpssDxChPolicerMeteringEntryEnvelopeSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum + stage + startEntryIndex */
    va_arg(args, GT_U32);
    va_arg(args, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
    va_arg(args, GT_U32);
    /* envelopeSize */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
    /* meterMode */
    paramDataPtr->paramKey.paramKeyArr[4] = CPSS_DXCH3_POLICER_METER_MODE_START_OF_ENVELOPE_E;
}
void cpssDxChPolicerMeteringEntryEnvelopeGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum + stage + startEntryIndex */
    va_arg(args, GT_U32);
    va_arg(args, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
    va_arg(args, GT_U32);
    /* maxEnvelopeSize */
    paramDataPtr->paramKey.paramKeyArr[1] = (GT_UINTPTR)va_arg(args, GT_U32);
    /* envelopeSizePtr */
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)va_arg(args, GT_U32 *);
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
