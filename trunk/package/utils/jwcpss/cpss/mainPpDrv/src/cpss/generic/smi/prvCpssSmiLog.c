/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssSmiLog.c
*       WARNING!!! this is a generated file, please don't edit it manually
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <cpss/generic/log/cpssLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/log/prvCpssGenCommonTypesLog.h>
#include <cpss/generic/log/prvCpssGenDbLog.h>
#include <cpss/generic/smi/private/prvCpssGenSmiLog.h>


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_phyDev = {
     "phyDev", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_smiAddr = {
     "smiAddr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_xsmiAddr = {
     "xsmiAddr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssXsmiRegisterWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PHY_XSMI_INTERFACE_ENT_xsmiInterface,
    &IN_GT_U32_xsmiAddr,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_phyDev,
    &IN_GT_U16_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssXsmiRegisterRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_PHY_XSMI_INTERFACE_ENT_xsmiInterface,
    &IN_GT_U32_xsmiAddr,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_phyDev,
    &OUT_GT_U16_PTR_dataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssSmiRegisterWriteShort_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PHY_SMI_INTERFACE_ENT_smiInterface,
    &IN_GT_U32_smiAddr,
    &IN_GT_U32_regAddr,
    &IN_GT_U16_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssSmiRegisterWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PHY_SMI_INTERFACE_ENT_smiInterface,
    &IN_GT_U32_smiAddr,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssSmiRegisterReadShort_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PHY_SMI_INTERFACE_ENT_smiInterface,
    &IN_GT_U32_smiAddr,
    &IN_GT_U32_regAddr,
    &OUT_GT_U16_PTR_dataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssSmiRegisterRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PHY_SMI_INTERFACE_ENT_smiInterface,
    &IN_GT_U32_smiAddr,
    &IN_GT_U32_regAddr,
    &OUT_GT_U32_PTR_dataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssXsmiPortGroupRegisterWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PHY_XSMI_INTERFACE_ENT_xsmiInterface,
    &IN_GT_U32_xsmiAddr,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_phyDev,
    &IN_GT_U16_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssXsmiPortGroupRegisterRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_PORT_GROUPS_BMP_portGroupsBmp,
    &IN_CPSS_PHY_XSMI_INTERFACE_ENT_xsmiInterface,
    &IN_GT_U32_xsmiAddr,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_phyDev,
    &OUT_GT_U16_PTR_dataPtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChSmiLogLibDb[] = {
    {"cpssSmiRegisterReadShort", 6, cpssSmiRegisterReadShort_PARAMS, NULL},
    {"cpssSmiRegisterWriteShort", 6, cpssSmiRegisterWriteShort_PARAMS, NULL},
    {"cpssSmiRegisterRead", 6, cpssSmiRegisterRead_PARAMS, NULL},
    {"cpssSmiRegisterWrite", 6, cpssSmiRegisterWrite_PARAMS, NULL},
    {"cpssXsmiPortGroupRegisterWrite", 7, cpssXsmiPortGroupRegisterWrite_PARAMS, NULL},
    {"cpssXsmiPortGroupRegisterRead", 7, cpssXsmiPortGroupRegisterRead_PARAMS, NULL},
    {"cpssXsmiRegisterWrite", 6, cpssXsmiRegisterWrite_PARAMS, NULL},
    {"cpssXsmiRegisterRead", 6, cpssXsmiRegisterRead_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_SMI(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChSmiLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChSmiLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

