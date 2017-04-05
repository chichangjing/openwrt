/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChResourceManagerManualLog.c
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
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManagerTypes.h>
#include <cpss/dxCh/dxChxGen/resourceManager/private/prvCpssDxChResourceManagerLog.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>

/********* enums *********/
static void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_sizeSet(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC*, paramVal);
    inOutParamInfoPtr->paramKey.paramKeyArr[0] = inOutParamInfoPtr->paramKey.paramKeyArr[4];
    inOutParamInfoPtr->paramKey.paramKeyArr[1] = inOutParamInfoPtr->paramKey.paramKeyArr[5];
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR);
}

PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_2_allocatedEntriesArray = {
     "allocatedEntriesArray", PRV_CPSS_LOG_PARAM_OUT_E, prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_sizeSet 
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_CHAR_ARRAY_clientNameArr = {
     "clientNameArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_CHAR)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerUsageGet_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_BOOL_usageOfSpecificClient,
    &IN_GT_U32_clientId,
    &OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_reservedEntriesArray,
    &INOUT_GT_U32_PTR_sizeOfReservedArrayPtr,
    &OUT_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR_2_allocatedEntriesArray,
    &INOUT_GT_U32_PTR_sizeofAllocatedArrayPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChTcamManagerClientRegister_PARAMS[] =  {
    &IN_GT_VOID_PTR_tcamManagerHandlerPtr,
    &IN_GT_CHAR_ARRAY_clientNameArr,
    &IN_CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC_PTR_clientFuncPtr,
    &OUT_GT_U32_PTR_clientIdPtr
};
/********* structure fields log functions *********/
void prvCpssLogParamFunc_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC_PTR);
}
/********* parameters log functions *********/

void cpssDxChTcamManagerEntriesReservationSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* tcamManagerHandlerPtr */
    va_arg(args, GT_VOID*);
    /* clientId */
    va_arg(args, GT_U32);
    /* reservationType */
    va_arg(args, CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT);
    /* requestedEntriesArray */
    va_arg(args, CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC*);
    /* sizeOfArray */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChTcamManagerEntriesReservationGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* tcamManagerHandlerPtr */
    va_arg(args, GT_VOID*);
    /* clientId */
    va_arg(args, GT_U32);
    /* reservationType */
    va_arg(args, CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT);
    /* requestedEntriesArray */
    va_arg(args, CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC*);
    /* maxSizeOfArrayPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
void cpssDxChTcamManagerUsageGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* tcamManagerHandlerPtr */
    va_arg(args, GT_VOID*);
    /* usageOfSpecificClient */
    va_arg(args, GT_BOOL);
    /* clientId */
    va_arg(args, GT_U32);
    /* reservedEntriesArray */
    va_arg(args, CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC*);
    /* maxSizeOfReservedArrayPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
    /* allocatedEntriesArray */
    va_arg(args, CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC*);
    /* maxSizeofAllocatedArrayPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[4] = (GT_UINTPTR)sizePtr;
    if (paramDataPtr->paramKey.paramKeyArr[4] != 0)
        paramDataPtr->paramKey.paramKeyArr[5] = (GT_U32)*sizePtr;
}
void cpssDxChTcamManagerAvailableEntriesCheck_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* tcamManagerHandlerPtr */
    va_arg(args, GT_VOID*);
    /* clientId */
    va_arg(args, GT_U32);
    /* requestedEntriesArray */
    va_arg(args, CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC*);
    /* sizeOfArray */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}

