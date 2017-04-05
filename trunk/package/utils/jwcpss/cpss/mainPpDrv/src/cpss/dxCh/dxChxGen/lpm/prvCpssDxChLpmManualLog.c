/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmManualLog.c
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
#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpmTypes.h>
#include <cpss/dxCh/dxChxGen/lpm/private/prvCpssDxChLpmLog.h>
#include <cpss/generic/ip/private/prvCpssGenIpLog.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, pointerType, CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT);
    prvCpssLogStcLogStart(contextLib,  logType, "pointerData");
    if (valPtr->pointerType == CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E || valPtr->pointerType == CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E
        || valPtr->pointerType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E) 
    {
        prvCpssLogStcLogStart(contextLib,  logType, "nextNodePointer"); 
        PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->pointerData)->nextNodePointer), nextPointer);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->pointerData)->nextNodePointer), range5Index);
        PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->pointerData)->nextNodePointer), pointToSipTree);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    else
    {
        prvCpssLogStcLogStart(contextLib,  logType, "nextHopOrEcmpPointer");
        PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->pointerData)->nextHopOrEcmpPointer), ucRpfCheckEnable);
        PRV_CPSS_LOG_STC_BOOL_MAC((&(&valPtr->pointerData)->nextHopOrEcmpPointer), srcAddrCheckMismatchEnable);
        PRV_CPSS_LOG_STC_ENUM_MAC((&(&valPtr->pointerData)->nextHopOrEcmpPointer), ipv6McGroupScopeLevel, CPSS_IPV6_PREFIX_SCOPE_ENT);
        PRV_CPSS_LOG_STC_NUMBER_MAC((&(&valPtr->pointerData)->nextHopOrEcmpPointer), entryIndex);
        prvCpssLogStcLogEnd(contextLib, logType);
    }
    prvCpssLogStcLogEnd(contextLib, logType);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFuncStc_CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT pointerType;

    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT *, valPtr);
    pointerType = inOutParamInfoPtr->paramKey.paramKeyArr[4];
    switch (pointerType) {
    case CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E:
        prvCpssLogStcLogStart(contextLib,  logType, "compressed1BitVector"); 
        PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->compressed1BitVector), ranges1_4, 4, GT_U8);
        prvCpssLogStcLogEnd(contextLib, logType);
        break;
    case CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E:
        prvCpssLogStcLogStart(contextLib,  logType, "compressed2BitVector"); 
        PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->compressed2BitVector), ranges1_4, 4, GT_U8);
        PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->compressed2BitVector), ranges6_9, 4, GT_U8);
        prvCpssLogStcLogEnd(contextLib, logType);
        break;
    case CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E:
        prvCpssLogStcLogStart(contextLib,  logType, "regularBitVector");
        PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC((&valPtr->regularBitVector), bitVectorEntry, 11, CPSS_DXCH_LPM_REGULAR_NODE_BIT_VECTOR_ENTRY_STC);
        prvCpssLogStcLogEnd(contextLib, logType);
        break;
    default:
        break;
    }
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFunc_CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC_PTR);
}
/********* parameters log functions *********/
/********* api pre-log functions *********/

void cpssDxChLpmNodeWrite_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* lpmLineOffset */
    va_arg(args, GT_U32);
    /* nodeType */
    paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT)va_arg(args, CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT);
    /* rangeSelectSecPtr */
    va_arg(args, CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT*);
    /* numOfRanges */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChLpmNodeRead_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* devNum */
    va_arg(args, GT_U32);
    /* lpmLineOffset */
    va_arg(args, GT_U32);
    /* nodeType */
    paramDataPtr->paramKey.paramKeyArr[4] = (CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT)va_arg(args, CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT);
    /* rangeSelectSecPtr */
    va_arg(args, CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT*);
    /* numOfRanges */
    sizePtr = (GT_U32*)va_arg(args, GT_U32*);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* a sign that the array is an output parameter */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
void cpssDxChLpmNodeNextPointersWrite_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* lpmLineOffset */
    va_arg(args, GT_U32);
    /* numOfNextPointers */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}

