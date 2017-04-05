/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChMultiPortGroupManualLog.c
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
#include <cpss/dxCh/dxChxGen/multiPortGroup/cpssDxChMultiPortGroup.h>
#include <cpss/dxCh/dxChxGen/multiPortGroup/private/prvCpssDxChMultiPortGroupLog.h>
#include <cpss/generic/cpssTypes.h>

/********* enums *********/
/********* structure fields log functions *********/
void prvCpssLogParamFuncStc_CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, srcPortGroupId);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, numOfRingPorts);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, ringPortsPtr, valPtr->numOfRingPorts, GT_PHYSICAL_PORT_NUM);
    PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, trunkIdNum);
    PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(valPtr, trunkIdPtr, valPtr->trunkIdNum, GT_TRUNK_ID);
    prvCpssLogStcLogEnd(contextLib, logType);
}
void prvCpssLogParamFunc_CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC*, paramVal);
    inOutParamInfoPtr->paramKey.paramKeyArr[2] = inOutParamInfoPtr->paramKey.paramKeyArr[3];
    if (inOutParamInfoPtr->paramKey.paramKeyArr[3] == 0) 
    {
        inOutParamInfoPtr->paramKey.paramKeyArr[3] = 1; 
    }
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC_PTR);
}

/********* parameters log functions *********/
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_PORT_NUM_ARRAY_uplinkPortsArr = {
     "uplinkPortsArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_PORT_NUM)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChMultiPortGroupConfigSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_numOfUplinkPorts,
    &IN_GT_PORT_NUM_ARRAY_uplinkPortsArr,
    &IN_GT_U32_ringInfoNum,
    &IN_CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC_PTR_ringInfoArr
};
/********* api pre-log functions *********/

void cpssDxChMultiPortGroupConfigSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* numOfUplinkPorts */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
    /* uplinkPortsArr */
    va_arg(args, GT_PORT_NUM*);
    /* ringInfoNum */
    paramDataPtr->paramKey.paramKeyArr[3] = (GT_U32)va_arg(args, GT_U32);
}
void cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* trunkId */
    va_arg(args, GT_U32);
    /* numOfPorts */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)va_arg(args, GT_U32);
}
