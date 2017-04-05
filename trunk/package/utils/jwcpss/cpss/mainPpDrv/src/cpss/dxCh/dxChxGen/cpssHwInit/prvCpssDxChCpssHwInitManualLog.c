/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCpssHwInitManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/generic/log/prvCpssGenLog.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChCpssHwInitLog.h>

/********* enums *********/
void prvCpssLogParamFunc_CPSS_DXCH_IMPLEMENT_WA_ENT_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DXCH_IMPLEMENT_WA_ENT*, paramVal);
    if (paramVal == NULL)
    {
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr);
        return;
    }
    PRV_CPSS_LOG_ENUM_ARRAY_MAC(contextLib, logType, namePtr, paramVal, CPSS_DXCH_IMPLEMENT_WA_ENT,inOutParamInfoPtr);
}
/********* structure fields log functions *********/
/********* parameters log functions *********/
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_additionalInfoBmpArr = {
     "additionalInfoBmpArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChHwPpImplementWaInit_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_numOfWa,
    &IN_CPSS_DXCH_IMPLEMENT_WA_ENT_PTR_waArr,
    &IN_GT_U32_ARRAY_additionalInfoBmpArr
};

/********* api pre-log functions *********/
void cpssDxChHwPpStartInit_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* loadFromEeprom */
    va_arg(args, GT_BOOL);
    /* initDataListPtr */
    va_arg(args, CPSS_REG_VALUE_INFO_STC*);
    /* initDataListLen */
    paramDataPtr->paramKey.paramKeyArr[2] = va_arg(args, GT_U32);
}
void cpssDxChHwPpImplementWaInit_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* numOfWa */
    paramDataPtr->paramKey.paramKeyArr[2] = va_arg(args, GT_U32);
}
