/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChDiagManualLog.c
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
#include <cpss/dxCh/dxChxGen/oam/private/prvCpssDxChOamLog.h>
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOamSrv.h>
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOam.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetworkIfLog.h>

/********* enums *********/
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_groupStatusArr = {
     "groupStatusArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChOamExceptionGroupStatusGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_OAM_STAGE_TYPE_ENT_stage,
    &IN_CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT_exceptionType,
    &OUT_GT_U32_ARRAY_groupStatusArr
};
/********* structure fields log functions *********/

/********* parameters log functions *********/

void cpssDxChOamExceptionGroupStatusGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* setting the size of groupStatusArr - constant */
    paramDataPtr->paramKey.paramKeyArr[2] = CPSS_DXCH_OAM_GROUP_STATUS_SIZE_IN_WORDS_CNS; 
}
