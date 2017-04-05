/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChVntLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>
#include <cpss/dxCh/dxChxGen/vnt/private/prvCpssDxChVntLog.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT[]  =
{
    "CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_OPCODE_E",
    "CPSS_DXCH_VNT_CFM_REPLY_MODE_CHANGE_LSBIT_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT);
}
void prvCpssLogParamFunc_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_MAC(namePtr, *paramVal, CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT_mode = {
     "mode", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT_PTR_modePtr = {
     "modePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_timeStampValuePtr = {
     "timeStampValuePtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVntCfmReplyModeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT_mode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVntPortGroupLastReadTimeStampGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &OUT_GT_U32_PTR_timeStampValuePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVntCfmLbrOpcodeSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_opcode
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVntCfmReplyModeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT_PTR_modePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVntCfmLbrOpcodeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_opcodePtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChVntLastReadTimeStampGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_timeStampValuePtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChVntLogLibDb[] = {
    {"cpssDxChVntOamPortLoopBackModeEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChVntOamPortLoopBackModeEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
    {"cpssDxChVntOamPortUnidirectionalEnableSet", 3, prvCpssLogGenDevNumPortEnable_PARAMS, NULL},
    {"cpssDxChVntOamPortUnidirectionalEnableGet", 3, prvCpssLogGenDevNumPortEnablePtr_PARAMS, NULL},
    {"cpssDxChVntCfmEtherTypeSet", 2, prvCpssLogGenDevNumEtherType_PARAMS, NULL},
    {"cpssDxChVntCfmEtherTypeGet", 2, prvCpssLogGenDevNumEtherTypePtr_PARAMS, NULL},
    {"cpssDxChVntCfmLbrOpcodeSet", 2, cpssDxChVntCfmLbrOpcodeSet_PARAMS, NULL},
    {"cpssDxChVntCfmLbrOpcodeGet", 2, cpssDxChVntCfmLbrOpcodeGet_PARAMS, NULL},
    {"cpssDxChVntCfmReplyModeSet", 2, cpssDxChVntCfmReplyModeSet_PARAMS, NULL},
    {"cpssDxChVntCfmReplyModeGet", 2, cpssDxChVntCfmReplyModeGet_PARAMS, NULL},
    {"cpssDxChVntPortGroupLastReadTimeStampGet", 3, cpssDxChVntPortGroupLastReadTimeStampGet_PARAMS, NULL},
    {"cpssDxChVntLastReadTimeStampGet", 2, cpssDxChVntLastReadTimeStampGet_PARAMS, NULL},
    {"cpssDxChVntOamPortPduTrapEnableSet", 3, prvCpssLogGenDevNumPortNumEnable_PARAMS, NULL},
    {"cpssDxChVntOamPortPduTrapEnableGet", 3, prvCpssLogGenDevNumPortNumEnablePtr_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_VNT(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChVntLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChVntLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

