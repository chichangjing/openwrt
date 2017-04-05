/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChVntLog.h
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __prvCpssDxChVntLogh
#define __prvCpssDxChVntLogh
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>


/********* enums *********/

PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT);


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);
void prvCpssLogParamFunc_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/********* API fields DB *********/

extern PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT_mode;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT_PTR_modePtr;
extern PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_timeStampValuePtr;


/********* lib API DB *********/

enum {
    PRV_CPSS_LOG_FUNC_cpssDxChVntOamPortLoopBackModeEnableSet_E = (CPSS_LOG_LIB_VNT_E << 16),
    PRV_CPSS_LOG_FUNC_cpssDxChVntOamPortLoopBackModeEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntOamPortUnidirectionalEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntOamPortUnidirectionalEnableGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntCfmEtherTypeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntCfmEtherTypeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntCfmLbrOpcodeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntCfmLbrOpcodeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntCfmReplyModeSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntCfmReplyModeGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntPortGroupLastReadTimeStampGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntLastReadTimeStampGet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntOamPortPduTrapEnableSet_E,
    PRV_CPSS_LOG_FUNC_cpssDxChVntOamPortPduTrapEnableGet_E
};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvCpssDxChVntLogh */
