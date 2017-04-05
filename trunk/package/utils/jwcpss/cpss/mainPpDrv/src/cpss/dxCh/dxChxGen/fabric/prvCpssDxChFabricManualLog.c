/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChFabricManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/fabric/cpssDxChFabricHGLink.h>
#include <cpss/dxCh/dxChxGen/fabric/private/prvCpssDxChFabricLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>

/********* enums *********/
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_BOOL_ARRAY_laneSyncOkArr = {
     "laneSyncOkArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_BOOL)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChFabricHGLinkPcsRxStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PHYSICAL_PORT_NUM_portNum,
    &OUT_GT_BOOL_ARRAY_laneSyncOkArr
};
/********* structure fields log functions *********/
void prvCpssLogParamFunc_CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC*, paramVal);
    prvCpssLogArrayOfParamsHandle(contextLib,logType,namePtr,(void*) paramVal, sizeof(CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC), inOutParamInfoPtr,
                                  prvCpssLogParamFuncStc_CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC_PTR);
}

/********* parameters log functions *********/

void cpssDxChFabricHGLinkPcsRxStatusGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* laneSyncOkArr size - constant */
    paramDataPtr->paramKey.paramKeyArr[2] = (GT_U32)CPSS_DXCH_FABRIC_HGL_SERDES_LANES_NUM_CNS;
}

void cpssDxChFabricHGLinkRxErrorCntrGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* rxErrorsCntrsPtr array size - constant */
    paramDataPtr->paramKey.paramKeyArr[2] = 6;
}
