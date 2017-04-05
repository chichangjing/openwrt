/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChMultiPortGroupLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/multiPortGroup/cpssDxChMultiPortGroup.h>
#include <cpss/dxCh/dxChxGen/multiPortGroup/private/prvCpssDxChMultiPortGroupLog.h>
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunkLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT[]  =
{
    "CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E",
    "CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E",
    "CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT);
char * prvCpssLogEnum_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT[]  =
{
    "CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E",
    "CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, unknownDaCommand, CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT);
    PRV_CPSS_LOG_STC_STC_MAC(valPtr, nextRingInterface, CPSS_INTERFACE_INFO_STC);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT_fdbMode = {
     "fdbMode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR_infoPtr = {
     "infoPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC_PTR_ringInfoArr = {
     "ringInfoArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC_PTR_portsArr = {
     "portsArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_PTR_uplinkPortsArr = {
     "uplinkPortsArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_PORT_NUM)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfPorts = {
     "numOfPorts", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_numOfUplinkPorts = {
     "numOfUplinkPorts", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ringInfoNum = {
     "ringInfoNum", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_sourcePortGroupId = {
     "sourcePortGroupId", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT_PTR_fdbModePtr = {
     "fdbModePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR_infoPtr = {
     "infoPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMultiPortGroupFdbModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT_fdbMode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMultiPortGroupLookupNotFoundTableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_sourcePortGroupId,
    &IN_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR_infoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_sourcePortGroupId,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMultiPortGroupLookupNotFoundTableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_sourcePortGroupId,
    &OUT_CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC_PTR_infoPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_sourcePortGroupId,
    &OUT_GT_BOOL_PTR_enablePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_TRUNK_ID_trunkId,
    &IN_GT_U32_numOfPorts,
    &IN_CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC_PTR_portsArr,
    &IN_CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMultiPortGroupFdbModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT_PTR_fdbModePtr
};
extern PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMultiPortGroupConfigSet_PARAMS[];


/********* lib API DB *********/

extern void cpssDxChMultiPortGroupConfigSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);
extern void cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet_preLogic(IN va_list  args, OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr);

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChMultiPortGroupLogLibDb[] = {
    {"cpssDxChMultiPortGroupConfigSet", 5, cpssDxChMultiPortGroupConfigSet_PARAMS, cpssDxChMultiPortGroupConfigSet_preLogic},
    {"cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet", 5, cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet_PARAMS, cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet_preLogic},
    {"cpssDxChMultiPortGroupPortLookupEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChMultiPortGroupPortLookupEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChMultiPortGroupPortRingEnableSet", 3, prvCpssLogGenDevNumPortNumEnable2_PARAMS, NULL},
    {"cpssDxChMultiPortGroupPortRingEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr2_PARAMS, NULL},
    {"cpssDxChMultiPortGroupLookupNotFoundTableSet", 4, cpssDxChMultiPortGroupLookupNotFoundTableSet_PARAMS, NULL},
    {"cpssDxChMultiPortGroupLookupNotFoundTableGet", 4, cpssDxChMultiPortGroupLookupNotFoundTableGet_PARAMS, NULL},
    {"cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet", 4, cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet_PARAMS, NULL},
    {"cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet", 4, cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet_PARAMS, NULL},
    {"cpssDxChMultiPortGroupFdbModeSet", 2, cpssDxChMultiPortGroupFdbModeSet_PARAMS, NULL},
    {"cpssDxChMultiPortGroupFdbModeGet", 2, cpssDxChMultiPortGroupFdbModeGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_MULTI_PORT_GROUP(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChMultiPortGroupLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChMultiPortGroupLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

