/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssInitLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/init/private/prvCpssGenInitLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssSystemDualDeviceIdModeEnableGet_PARAMS[] =  {
    &OUT_GT_BOOL_PTR_enablePtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChInitLogLibDb[] = {
    {"cpssPpDump", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssSystemDualDeviceIdModeEnableSet", 1, prvCpssLogGenEnable_PARAMS, NULL},
    {"cpssSystemDualDeviceIdModeEnableGet", 1, cpssSystemDualDeviceIdModeEnableGet_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_INIT(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChInitLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChInitLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

