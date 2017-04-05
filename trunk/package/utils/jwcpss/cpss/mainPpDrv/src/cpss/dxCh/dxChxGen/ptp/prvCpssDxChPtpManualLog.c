/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPtpManualLog.c
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
#include <cpss/dxCh/dxChxGen/ptp/private/prvCpssDxChPtpLog.h>

/********* enums *********/
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_ARRAY_domainIdArr = {
     "domainIdArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_domainIdArr = {
     "domainIdArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpDomainV1IdSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_domainIndex,
    &IN_GT_U32_ARRAY_domainIdArr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChPtpDomainV1IdGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_domainIndex,
    &OUT_GT_U32_ARRAY_domainIdArr
};
/********* structure fields log functions *********/

/********* parameters log functions *********/

void cpssDxChPtpDomainV1IdSet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    /* devNum */
    va_arg(args, GT_U32);
    /* setting the size of domainIdArr - constant */
    paramDataPtr->paramKey.paramKeyArr[2] = 4; 
}
