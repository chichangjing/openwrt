/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChIpfixManualLog.c
*       Manually implemented CPSS Log type wrappers
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/ipfix/cpssDxChIpfix.h>
#include <cpss/dxCh/dxChxGen/ipfix/private/prvCpssDxChIpfixLog.h>
#include <cpss/generic/log/prvCpssGenLog.h>

/********* enums *********/
/********* structure fields log functions *********/
/********* parameters log functions *********/
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_ARRAY_eventsArr = {
     "eventsArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(GT_U32)
};

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixAlarmEventsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_GT_U32_ARRAY_eventsArr,
    &OUT_GT_U32_PTR_eventsNumPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDxChIpfixPortGroupAlarmEventsGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_DXCH_POLICER_STAGE_TYPE_ENT_stage,
    &OUT_GT_U32_ARRAY_eventsArr,
    &OUT_GT_U32_PTR_eventsNumPtr
};
/********* api pre-log functions *********/
void cpssDxChIpfixAlarmEventsGet_preLogic
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
)
{
    GT_U32  *sizePtr; /*pointer to array size*/

    /* devNum */
    va_arg(args, GT_U32);
    /* stage */
    va_arg(args, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
    /* eventsArr */
    va_arg(args, GT_U32*);
    /* eventsNumPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    paramDataPtr->paramKey.paramKeyArr[1] = CPSS_DXCH_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
void cpssDxChIpfixPortGroupAlarmEventsGet_preLogic
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
    /* stage */
    va_arg(args, CPSS_DXCH_POLICER_STAGE_TYPE_ENT);
    /* eventsArr */
    va_arg(args, GT_U32*);
    /* eventsNumPtr */
    sizePtr = (GT_U32 *)va_arg(args, GT_U32 *);
    paramDataPtr->paramKey.paramKeyArr[0] = (GT_UINTPTR)sizePtr;
    paramDataPtr->paramKey.paramKeyArr[1] = CPSS_DXCH_IPFIX_ALARM_EVENTS_ARRAY_LENGTH_CNS;
    /* sign that the output might be an array of strucutures */
    paramDataPtr->paramKey.paramKeyArr[3] = 1;
}
