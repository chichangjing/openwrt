/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChTrunkManualLog.c
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
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunkLog.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC_PTR);
}
/********* parameters log functions *********/
/********* api pre-log functions *********/
void cpssDxChTrunkTableEntrySet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* trunkId */
    va_arg(args, GT_U32);
    /* numMembers - size of membersArray */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32); 
}
void cpssDxChTrunkTableEntryGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* devNum */
    va_arg(args, GT_U32);
    /* trunkId */
    va_arg(args, GT_U32);
    /* numMembersPtr - pointer to the actual size of membersArray */
    sizePtr = (GT_U32*)va_arg(args, GT_U32*);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    paramDataPtr->paramKey.paramKeyArr[1] = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;     
}
void cpssDxChTrunkDbEnabledMembersGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* devNum */
    va_arg(args, GT_U32);
    /* trunkId */
    va_arg(args, GT_U32);
    /* maxNumOfEnabledMembersPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
void cpssDxChTrunkMembersSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* trunkId */
    va_arg(args, GT_U32);
    /* numOfEnabledMembers */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
    /* enabledMembersArray */
    va_arg(args, CPSS_TRUNK_MEMBER_STC*);
    /* numOfDisabledMembers */
    paramDataPtr->paramKey.paramKeyArr[5] = (GT_U32)va_arg(args, GT_U32);
    /* sign that there are two arrays of strucutures */
    paramDataPtr->paramKey.paramKeyArr[4] = 2;
}

