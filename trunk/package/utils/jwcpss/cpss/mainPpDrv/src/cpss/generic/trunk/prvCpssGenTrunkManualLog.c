/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenTrunkManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpss/generic/trunk/private/prvCpssGenTrunkLog.h>
#include <cpss/generic/log/prvCpssGenPpTypesLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>

/********* enums *********/
/********* structure fields log functions *********/
/********* parameters log functions *********/
void prvCpssLogParamFunc_CPSS_TRUNK_MEMBER_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_TRUNK_MEMBER_STC * , paramVal); 
    if (inOutParamInfoPtr->paramKey.paramKeyArr[4] == 2) 
    {
        inOutParamInfoPtr->paramKey.paramKeyArr[4] = 1;
    }
    else
    {
        if(inOutParamInfoPtr->paramKey.paramKeyArr[4] == 1)
        {
            inOutParamInfoPtr->paramKey.paramKeyArr[2] = inOutParamInfoPtr->paramKey.paramKeyArr[5];
        }
    }
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_TRUNK_MEMBER_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_TRUNK_MEMBER_STC_PTR);
}
void prvCpssLogParamFunc_CPSS_TRUNK_WEIGHTED_MEMBER_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_TRUNK_WEIGHTED_MEMBER_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_TRUNK_WEIGHTED_MEMBER_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_TRUNK_WEIGHTED_MEMBER_STC_PTR);
}
/********* api pre-log functions *********/
