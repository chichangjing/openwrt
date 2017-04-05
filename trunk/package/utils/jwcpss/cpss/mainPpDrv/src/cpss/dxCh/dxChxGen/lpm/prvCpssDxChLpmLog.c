/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpm.h>
#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpmTypes.h>
#include <cpss/dxCh/dxChxGen/lpm/private/prvCpssDxChLpmLog.h>
#include <cpss/generic/ip/private/prvCpssGenIpLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT[]  =
{
    "CPSS_DXCH_LPM_EXCEPTION_HIT_E",
    "CPSS_DXCH_LPM_EXCEPTION_LPM_ECC_E",
    "CPSS_DXCH_LPM_EXCEPTION_ECMP_ECC_E",
    "CPSS_DXCH_LPM_EXCEPTION_PBR_BUCKET_E",
    "CPSS_DXCH_LPM_EXCEPTION_CONTINUE_TO_LOOKUP_E",
    "CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_0_E",
    "CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_1_E",
    "CPSS_DXCH_LPM_EXCEPTION_DST_G_IPV4_PACKETS_E",
    "CPSS_DXCH_LPM_EXCEPTION_SRC_G_IPV4_PACKETS_E",
    "CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_0_IPV6_PACKETS_E",
    "CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_1_IPV6_PACKETS_E",
    "CPSS_DXCH_LPM_EXCEPTION_DST_G_IPV6_PACKETS_E",
    "CPSS_DXCH_LPM_EXCEPTION_SRC_G_IPV6_PACKETS_E",
    "CPSS_DXCH_LPM_EXCEPTION_FCOE_D_ID_LOOKUP_E",
    "CPSS_DXCH_LPM_EXCEPTION_FCOE_S_ID_LOOKUP_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT);
char * prvCpssLogEnum_CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT[]  =
{
    "CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E",
    "CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E",
    "CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E",
    "CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E",
    "CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E",
    "CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT[]  =
{
    "CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E",
    "CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_LPM_LEAF_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_LPM_LEAF_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, entryType, CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, index);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, ucRPFCheckEnable);
    PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, sipSaCheckMismatchEnable);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, ipv6MCGroupScopeLevel, CPSS_IPV6_PREFIX_SCOPE_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_LPM_RAM_CONFIG_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_LPM_RAM_CONFIG_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfBlocks);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, blocksSizeArray, CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS, GT_U32);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, blocksAllocationMethod, CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, bitMap);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, rangeCounter);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_LPM_LEAF_ENTRY_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_LPM_LEAF_ENTRY_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_LPM_LEAF_ENTRY_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LPM_LEAF_ENTRY_STC_PTR_leafPtr = {
     "leafPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LPM_LEAF_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT_nodeType = {
     "nodeType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC_PTR_nextPointerArrayPtr = {
     "nextPointerArrayPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT_PTR_rangeSelectSecPtr = {
     "rangeSelectSecPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_BOOL_bypassEnabled = {
     "bypassEnabled", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_leafIndex = {
     "leafIndex", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_lpmLineOffset = {
     "lpmLineOffset", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfNextPointers = {
     "numOfNextPointers", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfRanges = {
     "numOfRanges", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT_PTR_hitExceptionPtr = {
     "hitExceptionPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LPM_LEAF_ENTRY_STC_PTR_leafPtr = {
     "leafPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LPM_LEAF_ENTRY_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC_PTR_nextPointerArrayPtr = {
     "nextPointerArrayPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT_PTR_rangeSelectSecPtr = {
     "rangeSelectSecPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_PTR_bypassEnabledPtr = {
     "bypassEnabledPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_BOOL)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_baseOffsetPtr = {
     "baseOffsetPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfRangesPtr = {
     "numOfRangesPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLpmLastLookupStagesBypassEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_BOOL_bypassEnabled
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLpmLeafEntryWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_leafIndex,
    &IN_CPSS_DXCH_LPM_LEAF_ENTRY_STC_PTR_leafPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLpmLeafEntryRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_leafIndex,
    &OUT_CPSS_DXCH_LPM_LEAF_ENTRY_STC_PTR_leafPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLpmNodeWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_lpmLineOffset,
    &IN_CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT_nodeType,
    &IN_CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT_PTR_rangeSelectSecPtr,
    &IN_GT_U32_numOfRanges,
    &IN_CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC_PTR_nextPointerArrayPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLpmNodeRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_lpmLineOffset,
    &IN_CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT_nodeType,
    &OUT_CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT_PTR_rangeSelectSecPtr,
    &OUT_GT_U32_PTR_numOfRangesPtr,
    &OUT_CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC_PTR_nextPointerArrayPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLpmNodeNextPointersWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_lpmLineOffset,
    &IN_GT_U32_numOfNextPointers,
    &IN_CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC_PTR_nextPointerArrayPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLpmExceptionStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT_PTR_hitExceptionPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLpmLastLookupStagesBypassEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_BOOL_PTR_bypassEnabledPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChLpmPbrBaseAndSizeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_baseOffsetPtr,
    &OUT_GT_U32_PTR_sizePtr
};


/********* lib API DB *********/

extern void cpssDxChLpmNodeWrite_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChLpmNodeRead_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChLpmNodeNextPointersWrite_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChLpmLogLibDb[] = {
    {"cpssDxChLpmNodeWrite", 6, cpssDxChLpmNodeWrite_PARAMS, cpssDxChLpmNodeWrite_preLogic},
    {"cpssDxChLpmNodeRead", 6, cpssDxChLpmNodeRead_PARAMS, cpssDxChLpmNodeRead_preLogic},
    {"cpssDxChLpmNodeNextPointersWrite", 4, cpssDxChLpmNodeNextPointersWrite_PARAMS, cpssDxChLpmNodeNextPointersWrite_preLogic},
    {"cpssDxChLpmLastLookupStagesBypassEnableSet", 2, cpssDxChLpmLastLookupStagesBypassEnableSet_PARAMS, NULL},
    {"cpssDxChLpmLastLookupStagesBypassEnableGet", 2, cpssDxChLpmLastLookupStagesBypassEnableGet_PARAMS, NULL},
    {"cpssDxChLpmPortSipLookupEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChLpmPortSipLookupEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChLpmLeafEntryWrite", 3, cpssDxChLpmLeafEntryWrite_PARAMS, NULL},
    {"cpssDxChLpmLeafEntryRead", 3, cpssDxChLpmLeafEntryRead_PARAMS, NULL},
    {"cpssDxChLpmPbrBaseAndSizeGet", 3, cpssDxChLpmPbrBaseAndSizeGet_PARAMS, NULL},
    {"cpssDxChLpmExceptionStatusGet", 2, cpssDxChLpmExceptionStatusGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_LPM(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChLpmLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChLpmLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

