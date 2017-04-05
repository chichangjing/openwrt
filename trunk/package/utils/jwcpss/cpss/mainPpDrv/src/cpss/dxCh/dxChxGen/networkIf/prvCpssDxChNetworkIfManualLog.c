/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChNetworkIfManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetworkIfLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfLog.h>
#include <cpss/generic/cos/private/prvCpssGenCosLog.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgLog.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_FROM_CPU_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_INTERFACE_INFO_STC dstInterface;
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_DSA_FROM_CPU_STC *, valPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, dstInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tc);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dp, CPSS_DP_LEVEL_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, egrFilterEn);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, cascadeControl);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, egrFilterRegistered);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcHwDev);
    dstInterface = PRV_CPSS_LOG_STC_FIELD_VAL_MAC(valPtr, dstInterface);
    prvCpssLogStcLogStart(contextLib,  logType, "extDestInfo");
    switch (dstInterface.type) 
    {
        case CPSS_INTERFACE_VIDX_E:
        case CPSS_INTERFACE_VID_E:
            prvCpssLogStcLogStart(contextLib,  logType, "multiDest");
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->extDestInfo)->multiDest), excludeInterface);
            PRV_CPSS_LOG_STC_STC_MAC((&(&valPtr->extDestInfo)->multiDest), excludedInterface, CPSS_INTERFACE_INFO_STC);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->extDestInfo)->multiDest), mirrorToAllCPUs);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->extDestInfo)->multiDest), excludedIsPhyPort);
            PRV_CPSS_LOG_STC_ENUM_MAC((&(&valPtr->extDestInfo)->multiDest), srcIsTagged, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        case CPSS_INTERFACE_PORT_E:
            prvCpssLogStcLogStart(contextLib,  logType, "devPort");
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->extDestInfo)->devPort), dstIsTagged);
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->extDestInfo)->devPort), mailBoxToNeighborCPU);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        default:
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTrgPhyPortValid);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dstEport);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag0TpidIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_FORWARD_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_DSA_FORWARD_STC *, valPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, srcIsTagged, CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcHwDev);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, srcIsTrunk);
    prvCpssLogStcLogStart(contextLib,  logType, "source");
    if (valPtr->srcIsTrunk)
    {
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->source), trunkId);
    }
    else
    {
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->source), portNum);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcId);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, egrFilterRegistered);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, wasRouted);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, qosProfileIndex);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, dstInterface, CPSS_INTERFACE_INFO_STC);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTrgPhyPortValid);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, dstEport);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag0TpidIndex);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, origSrcPhyIsTrunk);
    prvCpssLogStcLogStart(contextLib,  logType, "origSrcPhy");
    if (valPtr->origSrcPhyIsTrunk)
    {
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->origSrcPhy), trunkId);
    }
    else
    {
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->origSrcPhy), portNum);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, phySrcMcFilterEnable);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_TO_ANALYZER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_DSA_TO_ANALYZER_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, rxSniffer);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTagged);
    prvCpssLogStcLogStart(contextLib,  logType, "devPort");
    PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->devPort), hwDevNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->devPort), portNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->devPort), ePort);
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, analyzerTrgType, CPSS_INTERFACE_TYPE_ENT);
    prvCpssLogStcLogStart(contextLib,  logType, "extDestInfo");
    switch (valPtr->analyzerTrgType) 
    {
        case CPSS_INTERFACE_VIDX_E:
            prvCpssLogStcLogStart(contextLib,  logType, "multiDest");
            PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->extDestInfo)->multiDest), analyzerEvidx);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        case CPSS_INTERFACE_PORT_E:
            prvCpssLogStcLogStart(contextLib,  logType, "devPort");
            PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->extDestInfo)->devPort), analyzerIsTrgPortValid);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->extDestInfo)->devPort), analyzerHwTrgDev);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->extDestInfo)->devPort), analyzerTrgPort);
            PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->extDestInfo)->devPort), analyzerTrgEport);
            prvCpssLogStcLogEnd(contextLib, logType);
            break;
        default:
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag0TpidIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_TO_CPU_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_DSA_TO_CPU_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isEgressPipe);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, isTagged);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, hwDevNum);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, srcIsTrunk);
    prvCpssLogStcLogStart(contextLib,  logType, "interface");
    PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->interface), srcTrunkId);
    PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->interface), portNum);
    PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->interface), ePort);
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, cpuCode, CPSS_NET_RX_CPU_CODE_ENT);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, wasTruncated);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, originByteCount);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, timestamp);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, packetIsTT);
    prvCpssLogStcLogStart(contextLib,  logType, "flowIdTtOffset");
    if (valPtr->packetIsTT)
    {
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->flowIdTtOffset), ttOffset);
    }
    else
    {
        PRV_CPSS_LOG_STC_NUMBER_MAC((&valPtr->flowIdTtOffset), flowId);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, tag0TpidIndex);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_NET_DSA_PARAMS_STC *, valPtr);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, commonParams, CPSS_DXCH_NET_DSA_COMMON_STC);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, dsaType, CPSS_DXCH_NET_DSA_CMD_ENT);
    prvCpssLogStcLogStart(contextLib,  logType, "dsaInfo");
    switch (valPtr->dsaType) 
    {
    case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
        PRV_CPSS_LOG_STC_STC_MAC((&valPtr->dsaInfo), toCpu, CPSS_DXCH_NET_DSA_TO_CPU_STC); 
        break;
    case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
        PRV_CPSS_LOG_STC_STC_MAC((&valPtr->dsaInfo), fromCpu, CPSS_DXCH_NET_DSA_FROM_CPU_STC);
        break;
    case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
        PRV_CPSS_LOG_STC_STC_MAC((&valPtr->dsaInfo), toAnalyzer, CPSS_DXCH_NET_DSA_TO_ANALYZER_STC);
        break;
    case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
        PRV_CPSS_LOG_STC_STC_MAC((&valPtr->dsaInfo), forward, CPSS_DXCH_NET_DSA_FORWARD_STC);
        break;
    default:
            break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}

/********* parameters log functions *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PACKET_PTR_ARRAY_rxBuffList = {
     "rxBuffList", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_ARRAY_MAC(PACKET)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U8_PTR_ARRAY_rxBuffList = {
     "rxBuffList", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_ARRAY_MAC(GT_U8)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_rxBuffSizeList = {
     "rxBuffSizeList", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_PACKET_PTR_ARRAY_packetBuffsArrPtr = {
     "packetBuffsArrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_ARRAY_MAC(PACKET)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_buffLenArr = {
     "buffLenArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_PACKET_PTR_ARRAY_buffList = {
     "buffList", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_ARRAY_MAC(PACKET)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_buffLenList = {
     "buffLenList", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_PACKET_PTR_ARRAY_packetBuffsArrPtr = {
     "packetBuffsArrPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_ARRAY_MAC(PACKET)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_buffLenArr = {
     "buffLenArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_PACKET_PTR_packetDataPtr = {
     "packetDataPtr", PRV_CPSS_LOG_PARAM_IN_E,  prvCpssLogParamFunc_PACKET_PTR
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_DSA_PTR_dsaBytesPtr = {
     "dsaBytesPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(DSA)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_DSA_PTR_dsaBytesPtr = {
     "dsaBytesPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(DSA)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfRxBufFreeWithSize_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_rxQueue,
    &IN_GT_PACKET_PTR_ARRAY_rxBuffList,
    &IN_GT_U32_ARRAY_rxBuffSizeList,
    &IN_GT_U32_buffListLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiRxBufFree_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_rxQueue,
    &IN_GT_U8_PTR_ARRAY_rxBuffList,
    &IN_GT_U32_buffListLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiRxPacketGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_queueIdx,
    &INOUT_GT_U32_PTR_numOfBuffPtr,
    &OUT_PACKET_PTR_ARRAY_packetBuffsArrPtr,
    &OUT_GT_U32_ARRAY_buffLenArr,
    &OUT_CPSS_DXCH_NET_RX_PARAMS_STC_PTR_rxParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaRxPacketGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_queueIdx,
    &INOUT_GT_U32_PTR_numOfBuffPtr,
    &OUT_PACKET_PTR_ARRAY_packetBuffsArrPtr,
    &OUT_GT_U32_ARRAY_buffLenArr,
    &OUT_CPSS_DXCH_NET_RX_PARAMS_STC_PTR_rxParamsPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxPacketSend_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_pcktParamsPtr,
    &IN_PACKET_PTR_ARRAY_buffList,
    &IN_GT_U32_ARRAY_buffLenList,
    &IN_GT_U32_numOfBufs
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaSyncTxPacketSend_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_pcktParamsPtr,
    &IN_PACKET_PTR_ARRAY_buffList,
    &IN_GT_U32_ARRAY_buffLenList,
    &IN_GT_U32_numOfBufs
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiTxPacketSend_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_pcktParamsPtr,
    &IN_PACKET_PTR_ARRAY_packetBuffsArrPtr,
    &IN_GT_U32_ARRAY_buffLenArr,
    &IN_GT_U32_numOfBufs
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfMiiSyncTxPacketSend_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_pcktParamsPtr,
    &IN_PACKET_PTR_ARRAY_packetBuffsArrPtr,
    &IN_GT_U32_ARRAY_buffLenArr,
    &IN_GT_U32_numOfBufs
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorPacketAdd_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_packetParamsPtr,
    &IN_PACKET_PTR_packetDataPtr,
    &IN_GT_U32_packetDataLength,
    &OUT_GT_U32_PTR_packetIdPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfSdmaTxGeneratorPacketUpdate_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_packetId,
    &IN_CPSS_DXCH_NET_TX_PARAMS_STC_PTR_packetParamsPtr,
    &IN_PACKET_PTR_packetDataPtr,
    &IN_GT_U32_packetDataLength
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfDsaTagBuild_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR_dsaInfoPtr,
    &OUT_DSA_PTR_dsaBytesPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfDsaTagParse_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_DSA_PTR_dsaBytesPtr,
    &OUT_CPSS_DXCH_NET_DSA_PARAMS_STC_PTR_dsaInfoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChNetIfRxBufFree_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U8_rxQueue,
    &IN_GT_U8_PTR_ARRAY_rxBuffList,
    &IN_GT_U32_buffListLen
};
/********* api pre-log functions *********/
void cpssDxChNetIfRxBufFree_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* rxQueue */
    va_arg(args, GT_U32);
    /* rxBuffList */
    va_arg(args, GT_U8**);
    /* buffListLen */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChNetIfRxBufFreeWithSize_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *firstLenPtr;

    /* devNum */
    va_arg(args, GT_U32);
    /* rxQueue */
    va_arg(args, GT_U32);
    /* rxBuffList */
    va_arg(args, GT_U8**);
    /* rxBuffSizeList */
    firstLenPtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[4] = (GT_UINTPTR)firstLenPtr;
    /* buffListLen */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChNetIfSdmaRxPacketGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *firstLenPtr;
    GT_U32  *sizePtr;

    /* devNum */
    va_arg(args, GT_U32);
    /* queueIdx */
    va_arg(args, GT_U32);
    /* numOfBuffPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    /* numOfBuff */
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
    /* packetBuffsArrPtr */
    (GT_U8 **)va_arg(args, GT_U8 **);
    /* buffLenArr */
    firstLenPtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[4] = (GT_UINTPTR)firstLenPtr;
}

void cpssDxChNetIfSdmaTxPacketSend_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *firstLenPtr;

    /* devNum */
    va_arg(args, GT_U32);
    /* pcktParamsPtr */
    va_arg(args, CPSS_DXCH_NET_TX_PARAMS_STC*);
    /* buffList */
    va_arg(args, GT_U8**);
    /* buffLenList */
    firstLenPtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[4] = (GT_UINTPTR)firstLenPtr;
    /* numOfBufs */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}

void cpssDxChNetIfSdmaTxGeneratorPacketAdd_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* packetParamsPtr */
    va_arg(args, CPSS_DXCH_NET_TX_PARAMS_STC*);
    /* packetDataPtr */
    va_arg(args, GT_U8*);
    /* packetDataLength */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChNetIfSdmaTxGeneratorPacketUpdate_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* packetId */
    va_arg(args, GT_U32);
    /* packetParamsPtr */
    va_arg(args, CPSS_DXCH_NET_TX_PARAMS_STC*);
    /* packetDataPtr */
    va_arg(args, GT_U8*);
    /* packetDataLength */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
