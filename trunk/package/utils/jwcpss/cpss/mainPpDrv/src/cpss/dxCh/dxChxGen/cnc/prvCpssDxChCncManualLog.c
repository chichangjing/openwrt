/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChCncManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/cnc/private/prvCpssDxChCncLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFunc_CPSS_DXCH_CNC_COUNTER_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_CNC_COUNTER_STC *, stcPtr);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) stcPtr, sizeof(CPSS_DXCH_CNC_COUNTER_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_CNC_COUNTER_STC_PTR);
}
/********* parameters log functions *********/
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_indexesArr = {
     "indexesArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_portGroupIdArr = {
     "portGroupIdArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncCounterWraparoundIndexesGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_blockNum,
    &INOUT_GT_U32_PTR_indexNumPtr,
    &OUT_GT_U32_ARRAY_indexesArr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChCncPortGroupCounterWraparoundIndexesGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_GT_U32_blockNum,
    &INOUT_GT_U32_PTR_indexNumPtr,
    &OUT_GT_U32_ARRAY_portGroupIdArr,
    &OUT_GT_U32_ARRAY_indexesArr
};
/********* api pre-log functions *********/
void cpssDxChCncUploadedBlockGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* devNum */
    va_arg(args, GT_U32);
    /* numOfCounterValuesPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    /* maxNumOfCounterValues */
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
void cpssDxChCncPortGroupUploadedBlockGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* numOfCounterValuesPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    /* maxNumOfCounterValues */
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
void cpssDxChCncCounterWraparoundIndexesGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* devNum */
    va_arg(args, GT_U32);
    /* blockNum */
    va_arg(args, GT_U32);
    /* indexNumPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    /* maxIndexNumPtr */
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
void cpssDxChCncPortGroupCounterWraparoundIndexesGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* devNum */
    va_arg(args, GT_U32);
    /* portGroupsBmp */
    va_arg(args, GT_PORT_GROUPS_BMP);
    /* blockNum */
    va_arg(args, GT_U32);
    /* indexNumPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    /* maxIndexNumPtr */
    if (paramDataPtr->paramKey.paramKeyArr[0] != 0)
        paramDataPtr->paramKey.paramKeyArr[1] = (GT_U32)*sizePtr;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
