/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDriverLog.c
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
#include <cpssDriver/log/private/prvCpssDriverLog.h>
#include <cpssDriver/pp/config/generic/cpssDrvPpGenDump.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>


/********* enums *********/

char * prvCpssLogEnum_CPSS_DRV_HW_TRACE_TYPE_ENT[]  =
{
    "CPSS_DRV_HW_TRACE_TYPE_READ_E",
    "CPSS_DRV_HW_TRACE_TYPE_WRITE_E",
    "CPSS_DRV_HW_TRACE_TYPE_BOTH_E",
    "CPSS_DRV_HW_TRACE_TYPE_WRITE_DELAY_E",
    "CPSS_DRV_HW_TRACE_TYPE_ALL_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_DRV_HW_TRACE_TYPE_ENT);
char * prvCpssLogEnum_CPSS_MEMORY_DUMP_TYPE_ENT[]  =
{
    "CPSS_MEMORY_DUMP_BYTE_E",
    "CPSS_MEMORY_DUMP_SHORT_E",
    "CPSS_MEMORY_DUMP_WORD_E"
};
PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(CPSS_MEMORY_DUMP_TYPE_ENT);


/********* structure fields log functions *********/

void prvCpssLogParamFuncStc_CPSS_DRV_HW_ACCESS_OBJ_STC_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_FIELD_STC_MAC(CPSS_DRV_HW_ACCESS_OBJ_STC *, valPtr);
    PRV_CPSS_LOG_UNUSED_MAC(inOutParamInfoPtr);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRegisterReadFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRegisterWriteFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRegisterFieldReadFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRegisterFieldWriteFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRegisterBitMaskReadFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRegisterBitMaskWriteFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRamReadFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRamWriteFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRamWriteInReverseFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessVectorReadFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessVectorWriteFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRegisterIsrReadFunc);
    PRV_CPSS_LOG_STC_PTR_MAC(valPtr, hwAccessRegisterIsrWriteFunc);
    prvCpssLogStcLogEnd(contextLib, logType);
}


/********* parameters log functions *********/

void prvCpssLogParamFunc_CPSS_DRV_HW_ACCESS_OBJ_STC_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_START_PARAM_STC_MAC(CPSS_DRV_HW_ACCESS_OBJ_STC*, paramVal);
    prvCpssLogParamFuncStc_CPSS_DRV_HW_ACCESS_OBJ_STC_PTR(contextLib, logType, namePtr, paramVal, inOutParamInfoPtr);
}
void prvCpssLogParamFunc_CPSS_DRV_HW_TRACE_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_DRV_HW_TRACE_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_DRV_HW_TRACE_TYPE_ENT);
}
void prvCpssLogParamFunc_CPSS_MEMORY_DUMP_TYPE_ENT(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
)
{
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(CPSS_MEMORY_DUMP_TYPE_ENT, paramVal);
    PRV_CPSS_LOG_ENUM_MAC(namePtr, paramVal, CPSS_MEMORY_DUMP_TYPE_ENT);
}


/********* API fields DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DRV_HW_ACCESS_OBJ_STC_PTR_hwAccessObjPtr = {
     "hwAccessObjPtr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(CPSS_DRV_HW_ACCESS_OBJ_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_DRV_HW_TRACE_TYPE_ENT_traceType = {
     "traceType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_DRV_HW_TRACE_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_CPSS_MEMORY_DUMP_TYPE_ENT_dumpType = {
     "dumpType", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(CPSS_MEMORY_DUMP_TYPE_ENT)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_addrArr = {
     "addrArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_data = {
     "data", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_PTR_dataArr = {
     "dataArr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_addr = {
     "addr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_arrLen = {
     "arrLen", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_dumpLength = {
     "dumpLength", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_fieldData = {
     "fieldData", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_fieldLength = {
     "fieldLength", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_fieldOffset = {
     "fieldOffset", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC IN_GT_U32_startAddr = {
     "startAddr", PRV_CPSS_LOG_PARAM_IN_E,  PRV_CPSS_LOG_FUNC_TYPE_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_CPSS_INTERRUPT_SCAN_STC_PTR_PTR_treeRootPtrPtr = {
     "treeRootPtrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_PTR_MAC(CPSS_INTERRUPT_SCAN_STC)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_PTR_notAccessibleBeforeStartInitPtrPtr = {
     "notAccessibleBeforeStartInitPtrPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_data = {
     "data", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_dataArr = {
     "dataArr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_fieldData = {
     "fieldData", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfElementsPtr = {
     "numOfElementsPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};
PRV_CPSS_LOG_FUNC_PARAM_STC OUT_GT_U32_PTR_numOfInterruptRegistersNotAccessibleBeforeStartInitPtr = {
     "numOfInterruptRegistersNotAccessibleBeforeStartInitPtr", PRV_CPSS_LOG_PARAM_OUT_E,  PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(GT_U32)
};


/********* API prototypes DB *********/

PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvHwAccessObjectBind_PARAMS[] =  {
    &IN_CPSS_DRV_HW_ACCESS_OBJ_STC_PTR_hwAccessObjPtr,
    &IN_GT_BOOL_bind
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwTraceEnable_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_CPSS_DRV_HW_TRACE_TYPE_ENT_traceType,
    &IN_GT_BOOL_enable
};
PRV_CPSS_LOG_FUNC_PARAM_STC * prvCpssDrvHwPpWriteRam_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_addr,
    &IN_GT_U32_length,
    &IN_GT_U32_PTR_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * prvCpssDrvHwPpReadRam_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_addr,
    &IN_GT_U32_length,
    &OUT_GT_U32_PTR_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * prvCpssDrvHwPpWriteVec_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_PTR_addrArr,
    &IN_GT_U32_PTR_dataArr,
    &IN_GT_U32_arrLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * prvCpssDrvHwPpReadVec_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_PTR_addrArr,
    &OUT_GT_U32_PTR_dataArr,
    &IN_GT_U32_arrLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwRamWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_addr,
    &IN_GT_U32_length,
    &IN_GT_U32_PTR_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwRamRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_addr,
    &IN_GT_U32_length,
    &OUT_GT_U32_PTR_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwVectorWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_PTR_addrArr,
    &IN_GT_U32_PTR_dataArr,
    &IN_GT_U32_arrLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwVectorRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_PTR_addrArr,
    &OUT_GT_U32_PTR_dataArr,
    &IN_GT_U32_arrLen
};
PRV_CPSS_LOG_FUNC_PARAM_STC * prvCpssDrvHwPpPortGroupWriteInternalPciReg_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwRegFieldSet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_fieldOffset,
    &IN_GT_U32_fieldLength,
    &IN_GT_U32_fieldData
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwRegFieldGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_fieldOffset,
    &IN_GT_U32_fieldLength,
    &OUT_GT_U32_PTR_fieldData
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwRegBitMaskWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_mask,
    &IN_GT_U32_value
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwRegBitMaskRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_mask,
    &OUT_GT_U32_PTR_dataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwRegisterWrite_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_value
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvPpHwRegisterRead_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_portGroupId,
    &IN_GT_U32_regAddr,
    &OUT_GT_U32_PTR_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvHwPpResetAndInitControllerWriteReg_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvHwPpResetAndInitControllerSetRegField_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_fieldOffset,
    &IN_GT_U32_fieldLength,
    &IN_GT_U32_fieldData
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvHwPpResetAndInitControllerGetRegField_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_fieldOffset,
    &IN_GT_U32_fieldLength,
    &OUT_GT_U32_PTR_fieldData
};
PRV_CPSS_LOG_FUNC_PARAM_STC * prvCpssDrvHwPpWriteRegBitMask_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_mask,
    &IN_GT_U32_value
};
PRV_CPSS_LOG_FUNC_PARAM_STC * prvCpssDrvHwPpReadRegBitMask_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_mask,
    &OUT_GT_U32_PTR_dataPtr
};
PRV_CPSS_LOG_FUNC_PARAM_STC * prvCpssDrvHwPpWriteRegister_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_regAddr,
    &IN_GT_U32_value
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvHwPpResetAndInitControllerReadReg_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_regAddr,
    &OUT_GT_U32_PTR_data
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssPpDumpMemory_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &IN_GT_U32_startAddr,
    &IN_CPSS_MEMORY_DUMP_TYPE_ENT_dumpType,
    &IN_GT_U32_dumpLength
};
PRV_CPSS_LOG_FUNC_PARAM_STC * cpssDrvInterruptsTreeGet_PARAMS[] =  {
    &IN_GT_U8_devNum,
    &OUT_GT_U32_PTR_numOfElementsPtr,
    &OUT_CPSS_INTERRUPT_SCAN_STC_PTR_PTR_treeRootPtrPtr,
    &OUT_GT_U32_PTR_numOfInterruptRegistersNotAccessibleBeforeStartInitPtr,
    &OUT_GT_U32_PTR_PTR_notAccessibleBeforeStartInitPtrPtr
};


/********* lib API DB *********/

static PRV_CPSS_LOG_FUNC_ENTRY_STC prvCpssDxChCpssDriverLogLibDb[] = {
    {"cpssDrvInterruptsTreeGet", 5, cpssDrvInterruptsTreeGet_PARAMS, NULL},
    {"cpssPpDumpRegisters", 1, prvCpssLogGenDevNum_PARAMS, NULL},
    {"cpssPpDumpMemory", 4, cpssPpDumpMemory_PARAMS, NULL},
    {"cpssDrvPpHwRegisterRead", 4, cpssDrvPpHwRegisterRead_PARAMS, NULL},
    {"cpssDrvPpHwRegisterWrite", 4, cpssDrvPpHwRegisterWrite_PARAMS, NULL},
    {"cpssDrvPpHwRegFieldGet", 6, cpssDrvPpHwRegFieldGet_PARAMS, NULL},
    {"cpssDrvPpHwRegFieldSet", 6, cpssDrvPpHwRegFieldSet_PARAMS, NULL},
    {"cpssDrvPpHwRegBitMaskRead", 5, cpssDrvPpHwRegBitMaskRead_PARAMS, NULL},
    {"cpssDrvPpHwRegBitMaskWrite", 5, cpssDrvPpHwRegBitMaskWrite_PARAMS, NULL},
    {"cpssDrvPpHwRamRead", 5, cpssDrvPpHwRamRead_PARAMS, NULL},
    {"cpssDrvPpHwRamWrite", 5, cpssDrvPpHwRamWrite_PARAMS, NULL},
    {"cpssDrvPpHwVectorRead", 5, cpssDrvPpHwVectorRead_PARAMS, NULL},
    {"cpssDrvPpHwVectorWrite", 5, cpssDrvPpHwVectorWrite_PARAMS, NULL},
    {"cpssDrvPpHwRamInReverseWrite", 5, cpssDrvPpHwRamWrite_PARAMS, NULL},
    {"cpssDrvHwPpResetAndInitControllerReadReg", 3, cpssDrvHwPpResetAndInitControllerReadReg_PARAMS, NULL},
    {"cpssDrvHwPpResetAndInitControllerWriteReg", 3, cpssDrvHwPpResetAndInitControllerWriteReg_PARAMS, NULL},
    {"cpssDrvHwPpResetAndInitControllerGetRegField", 5, cpssDrvHwPpResetAndInitControllerGetRegField_PARAMS, NULL},
    {"cpssDrvHwPpResetAndInitControllerSetRegField", 5, cpssDrvHwPpResetAndInitControllerSetRegField_PARAMS, NULL},
    {"cpssDrvPpHwTraceEnable", 3, cpssDrvPpHwTraceEnable_PARAMS, NULL},
    {"cpssDrvHwAccessObjectBind", 2, cpssDrvHwAccessObjectBind_PARAMS, NULL},
    {"prvCpssDrvHwPpReadRegister", 3, cpssDrvHwPpResetAndInitControllerReadReg_PARAMS, NULL},
    {"prvCpssDrvHwPpWriteRegister", 3, prvCpssDrvHwPpWriteRegister_PARAMS, NULL},
    {"prvCpssDrvHwPpGetRegField", 5, cpssDrvHwPpResetAndInitControllerGetRegField_PARAMS, NULL},
    {"prvCpssDrvHwPpSetRegField", 5, cpssDrvHwPpResetAndInitControllerSetRegField_PARAMS, NULL},
    {"prvCpssDrvHwPpReadRegBitMask", 4, prvCpssDrvHwPpReadRegBitMask_PARAMS, NULL},
    {"prvCpssDrvHwPpWriteRegBitMask", 4, prvCpssDrvHwPpWriteRegBitMask_PARAMS, NULL},
    {"prvCpssDrvHwPpReadRam", 4, prvCpssDrvHwPpReadRam_PARAMS, NULL},
    {"prvCpssDrvHwPpWriteRam", 4, prvCpssDrvHwPpWriteRam_PARAMS, NULL},
    {"prvCpssDrvHwPpReadVec", 4, prvCpssDrvHwPpReadVec_PARAMS, NULL},
    {"prvCpssDrvHwPpWriteVec", 4, prvCpssDrvHwPpWriteVec_PARAMS, NULL},
    {"prvCpssDrvHwPpWriteRamInReverse", 4, prvCpssDrvHwPpWriteRam_PARAMS, NULL},
    {"prvCpssDrvHwPpReadInternalPciReg", 3, cpssDrvHwPpResetAndInitControllerReadReg_PARAMS, NULL},
    {"prvCpssDrvHwPpWriteInternalPciReg", 3, cpssDrvHwPpResetAndInitControllerWriteReg_PARAMS, NULL},
    {"prvCpssDrvHwPpResetAndInitControllerReadReg", 3, cpssDrvHwPpResetAndInitControllerReadReg_PARAMS, NULL},
    {"prvCpssDrvHwPpResetAndInitControllerGetRegField", 5, cpssDrvHwPpResetAndInitControllerGetRegField_PARAMS, NULL},
    {"prvCpssDrvHwPpResetAndInitControllerWriteReg", 3, cpssDrvHwPpResetAndInitControllerWriteReg_PARAMS, NULL},
    {"prvCpssDrvHwPpResetAndInitControllerSetRegField", 5, cpssDrvHwPpResetAndInitControllerSetRegField_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupReadRegister", 4, cpssDrvPpHwRegisterRead_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupWriteRegister", 4, cpssDrvPpHwRegisterWrite_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupGetRegField", 6, cpssDrvPpHwRegFieldGet_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupSetRegField", 6, cpssDrvPpHwRegFieldSet_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupReadRegBitMask", 5, cpssDrvPpHwRegBitMaskRead_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupWriteRegBitMask", 5, cpssDrvPpHwRegBitMaskWrite_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupReadRam", 5, cpssDrvPpHwRamRead_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupWriteRam", 5, cpssDrvPpHwRamWrite_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupWriteRamInReverse", 5, cpssDrvPpHwRamWrite_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupReadVec", 5, cpssDrvPpHwVectorRead_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupWriteVec", 5, cpssDrvPpHwVectorWrite_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupReadInternalPciReg", 4, cpssDrvPpHwRegisterRead_PARAMS, NULL},
    {"prvCpssDrvHwPpPortGroupWriteInternalPciReg", 4, prvCpssDrvHwPpPortGroupWriteInternalPciReg_PARAMS, NULL},
};

/******** DB Access Function ********/
void prvCpssLogParamLibDbGet_CPSS_LOG_LIB_APP_DRIVER_CALL(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtrPtr,
    OUT GT_U32 * logFuncDbSizePtr
)
{
    *logFuncDbPtrPtr = prvCpssDxChCpssDriverLogLibDb;
    *logFuncDbSizePtr = sizeof(prvCpssDxChCpssDriverLogLibDb) / sizeof(PRV_CPSS_LOG_FUNC_ENTRY_STC);
}

