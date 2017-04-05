/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssLog.h
*
* DESCRIPTION:
*       Includes definitions for CPSS log functions.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssLogh
#define __prvCpssLogh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <stdarg.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

/*
 * Typedef: enum PRV_CPSS_LOG_PARAM_DIRECTION_ENT
 *
 * Description:
 *      This enum describes API parameter direction.
 * Enumerations:
 *      PRV_CPSS_LOG_PARAM_IN_E -
 *          input API/function parameter
 *      PRV_CPSS_LOG_PARAM_OUT_E -
 *          output API/function parameter
 *      PRV_CPSS_LOG_PARAM_INOUT_E -
 *          input/output API/function parameter
 *
*/
typedef enum {
    PRV_CPSS_LOG_PARAM_IN_E,
    PRV_CPSS_LOG_PARAM_OUT_E,
    PRV_CPSS_LOG_PARAM_INOUT_E
}PRV_CPSS_LOG_PARAM_DIRECTION_ENT;

/* size of key array */
#define PRV_CPSS_LOG_PARAM_KEY_ARR_SIZE_STC 8

/*
 * Typedef: struct PRV_CPSS_LOG_PARAM_KEY_INFO_STC
 *
 * Description:
 *      This structure describes key for parameters parsing
 * Fields:
 *      paramKeyArr - parameter key's array which may be referenced in current API parameter logging logic
 *
*/
typedef struct {
    GT_UINTPTR  paramKeyArr[PRV_CPSS_LOG_PARAM_KEY_ARR_SIZE_STC];
}PRV_CPSS_LOG_PARAM_KEY_INFO_STC;
/*
 * Typedef: struct PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC
 *
 * Description:
 *      This structure describes input or output parameter info which may be
 *      used in API parameter logging logic.
 * Fields:
 *      paramKey -
 *          parameter key value which may be referenced in current API parameter logging logic
 *      formatPtr -
 *          (pointer to) parameter output format string
 *      paramValue - value of parameter. used for parameters those are pointers.
 *
*/
typedef struct {
    PRV_CPSS_LOG_PARAM_KEY_INFO_STC  paramKey;
    GT_CHAR                         * formatPtr;
    GT_VOID_PTR                      paramValue;
}PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC;


/*******************************************************************************
* prvCpssLogStcLogStart
*
* DESCRIPTION:
*       The function starts log structure
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       stcName - structure field name.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcLogStart
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 stcName
);

/*******************************************************************************
* prvCpssLogStcLogEnd
*
* DESCRIPTION:
*       The function ends log structure
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcLogEnd
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType
);

/*******************************************************************************
* prvCpssLogPacket
*
* DESCRIPTION:
*       The function logs packet as array of bytes
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       packetSize -
*           size of packet (in bytes)
*       packetPtr -
*           a pointer to the first byte of the packet
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogPacket
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_U32                      packetSize,
    IN GT_U8                       *packetPtr
);

/*******************************************************************************
* prvCpssLogEnum
*
* DESCRIPTION:
*       The function to log simple enum parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - parameter/structure field name.
*       enumArray - array of emumerator strings.
*       enumArrayIndex - log string access index in enum array.
*       enumArrayEntries - number of enum array entries.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogEnum
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN char                   * enumArray[],
    IN GT_U32                   enumArrayIndex,
    IN GT_U32                   enumArrayEntries
);

/*******************************************************************************
* prvCpssLogEnumMap
*
* DESCRIPTION:
*       The function to log mapped enum parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - parameter/structure field name.
*       keyValue - lookup enum value
*       enumArray - array of emumerator strings.
*       enumArrayEntries - number of enum array entries.
*       enumArrayEntrySize - array entry size
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogEnumMap
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U32                   keyValue,
    IN void                   * enumArray,
    IN GT_U32                   enumArrayEntries,
    IN GT_U32                   enumArrayEntrySize
);

/*******************************************************************************
* prvCpssEnumStringValueGet
*
* DESCRIPTION:
*       The function maps enum parameter to string value
*
* INPUTS:
*       fieldName - parameter/structure field name.
*       keyValue - lookup enum value
*       enumArray - array of emumerator strings.
*       enumArrayEntries - number of enum array entries.
*       enumArrayEntrySize - array entry size
*
* OUTPUTS:
*       keyStringPtr - (pointer to) key value mapped string
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssEnumStringValueGet
(
    IN GT_CHAR_PTR              fieldName,
    IN GT_U32                   keyValue,
    IN void                   * enumArray,
    IN GT_U32                   enumArrayEntries,
    IN GT_U32                   enumArrayEntrySize,
    OUT GT_U8                 * keyStringPtr
);

/*******************************************************************************
* prvCpssLogStcUintptr
*
* DESCRIPTION:
*       The function to log GT_UINTPTR parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcUintptr
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_UINTPTR               numValue
);

/*******************************************************************************
* prvCpssLogStcBool
*
* DESCRIPTION:
*       The function logs GT_BOOL structure field
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       bValue - field value.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcBool
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_BOOL                  bValue
);

/*******************************************************************************
* prvCpssLogStcNumber
*
* DESCRIPTION:
*       The function to log numeric parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcNumber
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U32                   numValue
);

/*******************************************************************************
* prvCpssLogStcByte
*
* DESCRIPTION:
*       The function to log byte parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcByte
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U8                    byteValue
);

/*******************************************************************************
* prvCpssLogParamFuncStc_GT_FLOAT32_PTR
*
* DESCRIPTION:
*       The function to log GT_FLOAT32 parameter by pointer
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       fieldPtr - (pointer to) field value
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFuncStc_GT_FLOAT32_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_VOID                * fieldPtr
);

/*******************************************************************************
* prvCpssLogStcFloat
*
* DESCRIPTION:
*       The function to log float parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcFloat
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN float                    numValue
);


/*******************************************************************************
* prvCpssLogStcPointer
*
* DESCRIPTION
*       The function to log pointer parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       ptrValue - (pointer to) field value.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcPointer
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN void *                   ptrValue
);

/*******************************************************************************
* prvCpssLogStcMac
*
* DESCRIPTION:
*       Function to log mac address parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       macPtr - pointer to ethernet address.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcMac
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U8                  * macPtr
);

/*******************************************************************************
* prvCpssLogStcIpV4
*
* DESCRIPTION:
*       Function to log IPV4 address parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       ipAddrPtr - pointer to IPV4 address.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcIpV4
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U8                  * ipAddrPtr
);

/*******************************************************************************
* prvCpssLogStcIpV6
*
* DESCRIPTION:
*       Function to log IPV6 address parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       ipV6Ptr - pointer to IPV6 address.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcIpV6
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U8                  * ipV6Ptr
);

/*******************************************************************************
* prvCpssLogParamDataCheck
*
* DESCRIPTION:
*       Parameters validation.
*
* INPUTS:
*       skipLog - skip log flag.
*       inOutParamInfoPtr - pointer to parameter log data.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL prvCpssLogParamDataCheck
(
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC  * inOutParamInfoPtr
);

/* Macros */

/* method to avoid compiler warning about unused variable */
#define PRV_CPSS_LOG_UNUSED_MAC(x) (void)x

#define PRV_CPSS_LOG_FUNC_TYPE_MAC(type_name) \
    prvCpssLogParamFunc_ ## type_name

/* Parameter(pointer) log functons */
#define PRV_CPSS_LOG_FUNC_TYPE_PTR_MAC(type_name) \
    prvCpssLogParamFunc_ ## type_name ## _PTR

/* Parameter(pointer to pointer) log functons */
#define PRV_CPSS_LOG_FUNC_TYPE_PTR_PTR_MAC(type_name) \
    prvCpssLogParamFunc_ ## type_name ## _PTR_PTR

/* Parameter(array of pointers) log functons */
#define PRV_CPSS_LOG_FUNC_TYPE_PTR_ARRAY_MAC(type_name) \
    prvCpssLogParamFunc_ ## type_name ## _PTR_ARRAY

/* Parameter(array of primitives) log functons */
#define PRV_CPSS_LOG_FUNC_TYPE_ARRAY_MAC(type_name) \
    prvCpssLogParamFunc_ ## type_name ## _ARRAY

/* Strurcture pointer log functons */
#define PRV_CPSS_LOG_FUNC_STC_TYPE_PTR_MAC(type_name) \
    prvCpssLogParamFuncStc_ ## type_name ## _PTR

/* Log array in structures */
#define PRV_CPSS_LOG_STC_TYPE_ARRAY_MAC(val_ptr, field_name, elem_num, field_type) \
    prvCpssLogParamFuncStcTypeArray(contextLib, logType, #field_name, (void *)val_ptr->field_name, elem_num, sizeof(field_type), PRV_CPSS_LOG_FUNC_STC_TYPE_PTR_MAC(field_type), inOutParamInfoPtr)

/* Log array of parameters */
#define PRV_CPSS_LOG_ARRAY_PARAMS_MAC(val_ptr, arr_name, elem_num, elem_type) \
    prvCpssLogParamFuncStcTypeArray(contextLib, logType, arr_name, (void *)val_ptr, elem_num, sizeof(elem_type), PRV_CPSS_LOG_FUNC_STC_TYPE_PTR_MAC(elem_type), inOutParamInfoPtr)

/* Log 2-dimentional array field in structures */
#define PRV_CPSS_LOG_STC_TYPE_ARRAY2_MAC(val_ptr, field_name, elem_num, elem_num2, field_type) \
    prvCpssLogParamFuncStcTypeArray2(contextLib, logType, #field_name, (void *)val_ptr->field_name, elem_num, elem_num2, sizeof(field_type), PRV_CPSS_LOG_FUNC_STC_TYPE_PTR_MAC(field_type), inOutParamInfoPtr)

/* 'Before' parameter function logic */
#define PRV_CPSS_LOG_FUNC_PRE_LOGIC_MAC(type_name) \
    prvCpssLogPreParamFunc_ ## type_name

/* 'After' parameter function logic */
#define PRV_CPSS_LOG_FUNC_POST_LOGIC_MAC(type_name) \
    prvCpssLogPostParamFunc_ ## type_name

/* Set parameter value to be used in parameter log function */
#define PRV_CPSS_LOG_SET_PARAM_VAL_MAC(param_type, paramVal) \
    param_type paramVal;\
    if (argsPtr == NULL) \
    {\
        cpssOsMemCpy(&paramVal, &(inOutParamInfoPtr->paramValue), sizeof(paramVal)); \
    }\
    else {\
        paramVal = va_arg(*argsPtr, param_type); \
        cpssOsMemCpy(&(inOutParamInfoPtr->paramValue), &paramVal, sizeof(inOutParamInfoPtr->paramValue)); \
    }\
    if (GT_FALSE == prvCpssLogParamDataCheck(skipLog, inOutParamInfoPtr)) \
        return

/* Set parameter field value to be used in parameter log function */
#define PRV_CPSS_LOG_SET_FIELD_VAL_PTR_MAC(field_type, valPtr) \
    field_type valPtr = (field_type) fieldPtr; \
    if (valPtr == NULL) {\
        prvCpssLogTabs(contextLib, logType);\
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr); \
        return;\
    }

/* The macro must be called before field log structure */
#define PRV_CPSS_LOG_START_FIELD_STC_MAC(stc_name, valPtr) \
    PRV_CPSS_LOG_SET_FIELD_VAL_PTR_MAC(stc_name, valPtr);  \
    prvCpssLogStcLogStart(contextLib, logType, namePtr)

/* The macro must be called before parameter log structure */
#define PRV_CPSS_LOG_START_PARAM_STC_MAC(stc_name, valPtr) \
    PRV_CPSS_LOG_SET_PARAM_VAL_MAC(stc_name, valPtr);  \
    if (valPtr == NULL) {\
        prvCpssLogTabs(contextLib, logType);\
        PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(contextLib, logType, "%s = NULL\n", namePtr); \
        return;\
    }

/* The macro for call the array of enums handler */
#define PRV_CPSS_LOG_ENUM_ARRAY_MAC(contextLib, logType, arr_name, paramVal, enum_type, param_info) \
    prvCpssLogArrayOfEnumsHandle(contextLib, logType, arr_name, paramVal, sizeof(enum_type), prvCpssLogEnum_##enum_type, prvCpssLogEnum_size_##enum_type, param_info)

/* The macro to log simple enum parameter */
#define PRV_CPSS_LOG_ENUM_MAC(fieldName, paramVal, fieldtype) \
    prvCpssLogEnum(contextLib, logType, fieldName, prvCpssLogEnum_##fieldtype, paramVal, prvCpssLogEnum_size_##fieldtype)

/* The macro to log simple enum field structure */
#define PRV_CPSS_LOG_STC_ENUM_MAC(valPtr, field, fieldtype) \
    prvCpssLogEnum(contextLib, logType, #field, prvCpssLogEnum_##fieldtype, valPtr->field, prvCpssLogEnum_size_##fieldtype)

/*  Macro to map enum key value to string */
#define PRV_CPSS_LOG_ENUM_STRING_GET_MAC(param, param_type, param_str) \
    prvCpssEnumStringValueGet(#param, param, (void *)prvCpssLogEnum_map_##param_type, prvCpssLogEnum_size_##param_type, sizeof(prvCpssLogEnum_map_##param_type[0]), param_str);

/*  Macro to log enum parameter using binary search */
#define PRV_CPSS_LOG_VAR_ENUM_MAP_MAC(param, param_type) \
    prvCpssLogEnumMap(contextLib, logType, #param, param, (void *)prvCpssLogEnum_map_##param_type, prvCpssLogEnum_size_##param_type, sizeof(prvCpssLogEnum_map_##param_type[0]));

/*  Macro to log enum parameter using binary search */
#define PRV_CPSS_LOG_ENUM_MAP_MAC(paramName, param, param_type) \
    prvCpssLogEnumMap(contextLib, logType, paramName, param, (void *)prvCpssLogEnum_map_##param_type, prvCpssLogEnum_size_##param_type, sizeof(prvCpssLogEnum_map_##param_type[0]));

/*  Macro to log enum field using binary search */
#define PRV_CPSS_LOG_STC_ENUM_MAP_MAC(valPtr, field, fieldtype) \
    prvCpssLogEnumMap(contextLib, logType, #field, valPtr->field, (void *)prvCpssLogEnum_map_##fieldtype, prvCpssLogEnum_size_##fieldtype, sizeof(prvCpssLogEnum_map_##fieldtype[0]));

#define PRV_CPSS_LOG_STC_ENUM_DECLARE_MAC(enum_type) \
extern char * prvCpssLogEnum_##enum_type[]; \
extern GT_U32 prvCpssLogEnum_size_##enum_type

#define PRV_CPSS_LOG_STC_ENUM_MAP_DECLARE_MAC(enum_type) \
extern PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC prvCpssLogEnum_map_##enum_type[]; \
extern GT_U32 prvCpssLogEnum_size_##enum_type

#define PRV_CPSS_LOG_STC_ENUM_ARRAY_SIZE_MAC(enum_type) \
GT_U32 prvCpssLogEnum_size_##enum_type = sizeof(prvCpssLogEnum_##enum_type)/sizeof(prvCpssLogEnum_##enum_type[0])

#define PRV_CPSS_LOG_STC_ENUM_MAP_ARRAY_SIZE_MAC(enum_type) \
GT_U32 prvCpssLogEnum_size_##enum_type = sizeof(prvCpssLogEnum_map_##enum_type)/sizeof(prvCpssLogEnum_map_##enum_type[0])

/* Log pointer field (as address in hex) */
#define PRV_CPSS_LOG_STC_PTR_MAC(valPtr, field)          \
    prvCpssLogStcPointer(contextLib, logType, #field, (void *)(GT_UINTPTR)valPtr->field);

/* Log boolean field */
#define PRV_CPSS_LOG_STC_BOOL_MAC(valPtr, field) \
    prvCpssLogStcBool(contextLib, logType, #field, valPtr->field);

/* Log numeric field */
#define PRV_CPSS_LOG_STC_NUMBER_MAC(valPtr, field) \
    prvCpssLogStcNumber(contextLib, logType, #field, valPtr->field);

/* Log UINTPTR field */
#define PRV_CPSS_LOG_STC_UINTPTR_MAC(valPtr, field) \
    prvCpssLogStcUintptr(contextLib, logType, #field, valPtr->field);

/* Log byte field */
#define PRV_CPSS_LOG_STC_BYTE_MAC(valPtr, field) \
    prvCpssLogStcByte(contextLib, logType, #field, valPtr->field);

/* Log structure field */
#define PRV_CPSS_LOG_STC_STC_MAC(valPtr, field, fieldtype) {\
    PRV_CPSS_LOG_STC_FUNC logFunc = PRV_CPSS_LOG_FUNC_STC_TYPE_PTR_MAC(fieldtype); \
    logFunc(contextLib, logType, #field, &valPtr->field, inOutParamInfoPtr);       \
  }

/* Log structure pointer field */
#define PRV_CPSS_LOG_STC_STC_PTR_MAC(valPtr, field, fieldtype) {\
    PRV_CPSS_LOG_STC_FUNC logFunc = PRV_CPSS_LOG_FUNC_STC_TYPE_PTR_MAC(fieldtype); \
    logFunc(contextLib, logType, #field, valPtr->field, inOutParamInfoPtr);        \
  }

/* Structure field value */
#define PRV_CPSS_LOG_STC_FIELD_VAL_MAC(valPtr, param_field) \
    valPtr->param_field

/* Log API parameters and save history in database */
#define PRV_CPSS_LOG_AND_HISTORY_PARAM_MAC(log_lib, log_type, ...) \
    prvCpssLogAndHistoryParam(log_lib, log_type, __VA_ARGS__)


/* Set a format string for logging pointer values */
#define PRV_CPSS_LOG_PTR_FORMAT_MAC(formatStr)                                   \
    formatStr = (prvCpssLogPointerFormat == CPSS_LOG_POINTER_FORMAT_PREFIX_E) ? \
            "%s = (addr)%p\n" :  "%s = %p\n";


/* CPSS LOG API output Format */
extern CPSS_LOG_API_FORMAT_ENT prvCpssLogFormat;
/* CPSS LOG TAB Index */
extern GT_U32 prvCpssLogTabIndex;

/* CPSS LOG pointer output Format */
extern CPSS_LOG_POINTER_FORMAT_ENT prvCpssLogPointerFormat;

/* Check zero value log condition */
#define PRV_CPSS_LOG_ZERO_VALUE_LOG_CHECK_MAC(field) \
    if (field == 0 && (prvCpssLogFormat == CPSS_LOG_API_FORMAT_NON_ZERO_PARAMS_E) && (prvCpssLogTabIndex != 0)) \
        return;

/* Check zero array log condition */
#define PRV_CPSS_LOG_ZERO_ARRAY_LOG_CHECK_MAC(arr, size) \
    {                                                    \
        GT_U32 i, val = 0;                               \
        for (i = 0; i < size; i++){                      \
            if (arr[i]) {                                \
                val = 1; break;                          \
            }                                            \
        }                                                \
        PRV_CPSS_LOG_ZERO_VALUE_LOG_CHECK_MAC(val);      \
    }

/* Check zero 2-dimentional array log condition */
#define PRV_CPSS_LOG_ZERO_ARRAY2_LOG_CHECK_MAC(arr, size_i, size_j)   \
    {                                                       \
        GT_U32 i, j, val = 0;                               \
        for (i = 0; i < size_i; i++){                       \
            for (j = 0; j < size_j; j++){                   \
                if (arr[i][j]) {                            \
                    val = 1; break;                         \
                }                                           \
            }                                               \
        }                                                   \
        PRV_CPSS_LOG_ZERO_VALUE_LOG_CHECK_MAC(val);         \
    }

/* String constants */
extern const GT_CHAR *prvCpssLogErrorMsgDeviceNotExist;
extern const GT_CHAR *prvCpssLogErrorMsgFdbIndexOutOfRange;
extern const GT_CHAR *prvCpssLogErrorMsgPortGroupNotValid;
extern const GT_CHAR *prvCpssLogErrorMsgPortGroupNotActive;
extern const GT_CHAR *prvCpssLogErrorMsgIteratorNotValid;
extern const GT_CHAR *prvCpssLogErrorMsgPortGroupBitmapNotValid;
extern const GT_CHAR *prvCpssLogErrorMsgDeviceNotInitialized;

/*******************************************************************************
* prvCpssLogTabs
*
* DESCRIPTION:
*       Log tab for recursive structures
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogTabs
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType
);

/*******************************************************************************
* PRV_CPSS_LOG_PARAM_FUNC
*
* DESCRIPTION:
*       Log API parameter function
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           pointer to parameter name
*       argsPtr -
*           pointer to parameter argiment list or NULL
*       fieldPtr -
*           pointer to field in structure or NULL
*       skipLog -
*           skip parameter/field logging
*       inOutParamInfoPtr -
*           pointer to additional parameter data
*
* OUTPUTS:
*       inOutParamInfoPtr -
*           pointer to additional parameter data
*
* RETURNS:
*       None
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
typedef GT_VOID  (*PRV_CPSS_LOG_PARAM_FUNC)
(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);

/*******************************************************************************
* PRV_CPSS_LOG_STC_FUNC
*
* DESCRIPTION:
*       Log structure function
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           pointer to structure field name
*       fieldPtr -
*           pointer to field in structure
*       inOutParamInfoPtr -
*           pointer to additional parameter data
*
* OUTPUTS:
*       None
* RETURNS:
*       None
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
typedef GT_VOID  (*PRV_CPSS_LOG_STC_FUNC)
(
  IN    CPSS_LOG_LIB_ENT                        contextLib,
  IN    CPSS_LOG_TYPE_ENT                       logType,
  IN    GT_CHAR_PTR                             namePtr,
  IN    void                                  * fieldPtr,
  INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);

/*
 * Typedef: struct PRV_CPSS_LOG_FUNC_PARAM_STC
 *
 * Description:
 *      This structure describes API parameter info.
 * Fields:
 *      paramNamePtr -
 *          pointer to parameter name
 *      direction -
 *          API parameter direction.
 *      logFunc -
 *          parameter log function
 */
typedef struct {
    GT_CHAR_PTR                         paramNamePtr;
    PRV_CPSS_LOG_PARAM_DIRECTION_ENT    direction;
    PRV_CPSS_LOG_PARAM_FUNC             logFunc;
}PRV_CPSS_LOG_FUNC_PARAM_STC;

/*******************************************************************************
* PRV_CPSS_LOG_PRE_LOG_HANDLER_FUN
*
* DESCRIPTION:
*       Fucntion called before API log.
*
* INPUTS:
*       args -
*           log API variable arguments
*
* OUTPUTS:
*       paramDataPtr -
*           pointer to API parameters common data
*
* RETURNS:
*       None
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
typedef GT_VOID  (*PRV_CPSS_LOG_PRE_LOG_HANDLER_FUN)
(
    IN va_list  args,
    OUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC * paramDataPtr
);

/*
 * Typedef: struct PRV_CPSS_LOG_FUNC_ENTRY_STC
 *
 * Description:
 *      This structure describes database parameter entry.
 * Fields:
 *      apiName -
 *          pointer to API name
 *      numOfParams -
 *          API parameters number
 *      paramsPtrPtr -
 *          (pointer to) pointer to API parameter info.
 *      prvCpssLogPreLogFunc -
 *          function may be called before API log function
 */
typedef struct {
    GT_CHAR_PTR                       apiName;
    GT_U32                            numOfParams;
    PRV_CPSS_LOG_FUNC_PARAM_STC     **paramsPtrPtr;
    PRV_CPSS_LOG_PRE_LOG_HANDLER_FUN  prvCpssLogPreLogFunc;
}PRV_CPSS_LOG_FUNC_ENTRY_STC;

/*******************************************************************************
* PRV_CPSS_LOG_PARAM_LIB_DB_GET_FUNC
*
* DESCRIPTION:
*       Get API log functions database from specific lib
*
* INPUTS:
*       None
*
* OUTPUTS:
*       logFuncDbPtr -
*           (pointer to) pointer to LIB API log function database
*       logFuncDbSizePtr -
*           pointer to LIB API log function database size
*
* RETURNS:
*       None
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
typedef GT_VOID  (*PRV_CPSS_LOG_PARAM_LIB_DB_GET_FUNC)
(
    OUT PRV_CPSS_LOG_FUNC_ENTRY_STC ** logFuncDbPtr,
    OUT GT_U32 * logFuncDbSizePtr
);

/*
 * Typedef: struct PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC
 *
 * Description:
 *      Enumerator key pair
 *
 * Fields:
 *      namePtr - enum name
 *      enumValue - enumerator value
 *
 */
typedef struct {
    GT_CHAR_PTR                 namePtr;
    GT_32                       enumValue;
}PRV_CPSS_ENUM_STRING_VALUE_PAIR_STC;

/* the status of the CPSS LOG - enable/disable */
extern GT_BOOL prvCpssLogEnabled;
#ifdef  WIN32
#define __FILENAME__ (cpssOsStrRChr(__FILE__, '\\') ? cpssOsStrRChr(__FILE__, '\\') + 1 : __FILE__)
#define __FUNCNAME__ __FUNCTION__
#else
#define __FILENAME__ (cpssOsStrRChr(__FILE__, '/') ? cpssOsStrRChr(__FILE__, '/') + 1 : __FILE__)
#define __FUNCNAME__ __func__
#endif
/* empty log error string */
#define LOG_ERROR_NO_MSG    ""

/* Log numeric format */
extern const GT_CHAR *prvCpssLogFormatNumFormat;
/* format parameter value by its name */
#define LOG_ERROR_ONE_PARAM_FORMAT_MAC(_param)      prvCpssLogFormatNumFormat, #_param, _param

#ifdef CPSS_LOG_ENABLE

/* macro to log function error by formatted string */
#define CPSS_LOG_ERROR_AND_RETURN_MAC(_rc, ...) \
    do { \
        if (prvCpssLogEnabled == GT_TRUE) prvCpssLogError(__FUNCNAME__, __FILENAME__, __LINE__, _rc, __VA_ARGS__); \
        return _rc; \
    } while (0)

/* macro to log function information by formatted string */
#define CPSS_LOG_INFORMATION_MAC(...) \
    if (prvCpssLogEnabled == GT_TRUE) prvCpssLogInformation(__FUNCNAME__, __FILENAME__, __LINE__, __VA_ARGS__);

#else
#define CPSS_LOG_ERROR_AND_RETURN_MAC(_rc, ...) return _rc
#define CPSS_LOG_INFORMATION_MAC(...)
#endif

/* macro to log info parameter : it's name and value */
#define CPSS_LOG_INFO_PARAM_MAC(param)  CPSS_LOG_INFORMATION_MAC("%s = [0x%x] \n", #param , param)

/* Get the name (string) and value of field */
#define PRV_CPSS_LOG_ENUM_NAME_AND_VALUE_MAC(field)   {#field , field}

/*******************************************************************************
* prvCpssLogApiEnter
*
* DESCRIPTION:
*       Internal API for API/Internal/Driver function call log - enter function.
*
* INPUTS:
*       functionId - function identifier (of the function we just entered into).
*       ... - the parameters values of the function we just entered into.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogApiEnter
(
    IN GT_U32 functionId,
    IN ...
);

/*******************************************************************************
* prvCpssLogApiExit
*
* DESCRIPTION:
*       Internal API for API/Internal function call log - exit function.
*
* INPUTS:
*       functionId - function identifier (of the function we just exit from).
*       rc - the return code of the function.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogApiExit
(
    IN GT_U32 functionId,
    IN GT_STATUS rc
);

/*******************************************************************************
* prvCpssLogError
*
* DESCRIPTION:
*       Internal API for API/Internal function error log
*
* INPUTS:
*       functionName    - name of function that generates error.
*       fileName        - name of file that generates error.
*       line            - line number in file, may be excluded from log by global configuration.
*       rc              - the return code of the function.
*       formatPtr       - cpssOsLog format starting.
*       ...               optional parameters according to formatPtr
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssLogError
(
    IN const char *functionName,
    IN const char *fileName,
    IN GT_U32 line,
    IN GT_STATUS rc,
    IN const char * formatPtr,
    ...
);

/*******************************************************************************
* prvCpssLogInformation
*
* DESCRIPTION:
*       Internal API for API/Internal function information log
*
* INPUTS:
*       functionName    - name of function that generates error.
*       fileName        - name of file that generates error.
*       line            - line number in file, may be excluded from log by global configuration.
*       formatPtr       - cpssOsLog format starting.
*       ...               optional parameters according to formatPtr
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssLogInformation
(
    IN const char *functionName,
    IN const char *fileName,
    IN GT_U32 line,
    IN const char * formatPtr,
    ...
);

/*******************************************************************************
* prvCpssLogAndHistoryParam
*
* DESCRIPTION:
*       Log output and log history of function's parameter
*
* INPUTS:
*       lib             - the function will print the log of the functions in "lib".
*       type            - the function will print the logs from type "type".
*       format          - usual printf format string.
*       ...             - additional parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssLogAndHistoryParam
(
    IN CPSS_LOG_LIB_ENT         lib,
    IN CPSS_LOG_TYPE_ENT        type,
    IN const char*              format,
    ...
);

/*******************************************************************************
* prvCpssLogParamFuncStcTypeArray
*
* DESCRIPTION:
*       Log array in structure
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       firstElementPtr -
*           pointer to first array element
*       elementNum -
*           number of array elements
*       elementSize -
*           size of array element in bytes
*       logFunc -
*           pointer to specific type log function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssLogParamFuncStcTypeArray
(
    IN CPSS_LOG_LIB_ENT        contextLib,
    IN CPSS_LOG_TYPE_ENT       logType,
    IN GT_CHAR_PTR             namePtr,
    IN GT_VOID               * firstElementPtr,
    IN GT_U32                  elementNum,
    IN GT_U32                  elementSize,
    IN PRV_CPSS_LOG_STC_FUNC   logFunc,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);

/*******************************************************************************
* prvCpssLogParamFuncStcTypeArray2
*
* DESCRIPTION:
*       Log two-dimentional array
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       firstElementPtr -
*           pointer to first array element
*       rowNum -
*           number of array rows
*       rowNum -
*           number of array columns
*       elementSize -
*           size of array element in bytes
*       logFunc -
*           pointer to specific type log function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssLogParamFuncStcTypeArray2
(
    IN CPSS_LOG_LIB_ENT        contextLib,
    IN CPSS_LOG_TYPE_ENT       logType,
    IN GT_CHAR_PTR             namePtr,
    IN GT_VOID               * firstElementPtr,
    IN GT_U32                  rowNum,
    IN GT_U32                  colNum,
    IN GT_U32                  elementSize,
    IN PRV_CPSS_LOG_STC_FUNC   logFunc,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);

/*******************************************************************************
* prvCpssLogArrayOfParamsHandle
*
* DESCRIPTION:
*       Log array of parameters (input and output arrays)
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       firstElementPtr -
*           pointer to first array element
*       elementSize -
*           size of array element in bytes
*       logFunc -
*           pointer to specific type log function
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*           inOutParamInfoPtr->paramKey.paramKeyArr[0] - pointer to output array size
*           inOutParamInfoPtr->paramKey.paramKeyArr[1] - the array max size
*           inOutParamInfoPtr->paramKey.paramKeyArr[2] - size of an input array
*           inOutParamInfoPtr->paramKey.paramKeyArr[3] - value of 1 is a sign that the output might be an array of strucutures
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssLogArrayOfParamsHandle
(
    IN CPSS_LOG_LIB_ENT        contextLib,
    IN CPSS_LOG_TYPE_ENT       logType,
    IN GT_CHAR_PTR             namePtr,
    IN GT_VOID               * firstElementPtr,
    IN GT_U32                  elementSize,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr,
    IN PRV_CPSS_LOG_STC_FUNC   logFunc
);

/*******************************************************************************
* GT_VOID prvCpssLogArrayOfEnumsHandle
*
* DESCRIPTION:
*       Log array of parameters (input and output arrays)
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name
*       firstElementPtr -
*           pointer to first enum in the array
*       enumSize -
*           size of the enum
*       enumArray -
*           key value array of the enum values
*       enumArrayEntries -
*           the size of the enum array
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*           inOutParamInfoPtr->paramKey.paramKeyArr[0] - pointer to output array size
*           inOutParamInfoPtr->paramKey.paramKeyArr[1] - the array max size
*           inOutParamInfoPtr->paramKey.paramKeyArr[2] - size of an input array
*           inOutParamInfoPtr->paramKey.paramKeyArr[3] - value of 1 is a sign that the output might be an array of strucutures 
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID prvCpssLogArrayOfEnumsHandle
(
    IN CPSS_LOG_LIB_ENT        contextLib,
    IN CPSS_LOG_TYPE_ENT       logType,
    IN GT_CHAR_PTR             namePtr,
    IN GT_VOID               * firstElementPtr,
    IN GT_U32                  enumSize,
    IN char                  * enumArray[],
    IN GT_U32                  enumArrayEntries,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);

/*******************************************************************************
* prvCpssLogInit
*
* DESCRIPTION:
*       Init log related info
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogInit
(
    GT_VOID
);

/*******************************************************************************
* prvCpssLogStateCheck
*
* DESCRIPTION:
*       Debug function to check log readiness before new API call
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - log is ready to run
*       GT_BAD_STATE    - log is state machine in bad state to run log
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssLogStateCheck
(
    GT_VOID
);

/*******************************************************************************
* prvCpssLogParamFunc_GT_U8_PTR_ARRAY
*
* DESCRIPTION:
*       log handler for parsing an array of GT_U8 pointers
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           va_list of the function arguments
*       skipLog -
*           to log or not to log
*       inOutParamInfoPtr -
*           pointer to information about the function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_U8_PTR_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);

/*******************************************************************************
* prvCpssLogParamFunc_GT_U8_ARRAY
*
* DESCRIPTION:
*       log handler for parsing an array of GT_U8 elements
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           va_list of the function arguments
*       skipLog -
*           to log or not to log
*       inOutParamInfoPtr -
*           pointer to information about the function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_U8_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);

/*******************************************************************************
* prvCpssLogParamFunc_GT_U32_ARRAY
*
* DESCRIPTION:
*       log handler for parsing an array of GT_U32 elements
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           va_list of the function arguments
*       skipLog -
*           to log or not to log
*       inOutParamInfoPtr -
*           pointer to information about the function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_U32_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
); 

/*******************************************************************************
* prvCpssLogParamFunc_PACKET_PTR
*
* DESCRIPTION:
*       log handler for parsing a packet
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           variable name.
*       argsPtr -
*           va_list of the function arguments
*       skipLog -
*           to log or not to log
*       inOutParamInfoPtr -
*           pointer to information about the function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_PACKET_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);

/*******************************************************************************
* prvCpssLogParamFunc_PACKET_PTR_ARRAY
*
* DESCRIPTION:
*       log handler for parsing an array of packets
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           va_list of the function arguments
*       skipLog -
*           to log or not to log
*       inOutParamInfoPtr -
*           pointer to information about the function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_PACKET_PTR_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);

/*******************************************************************************
* prvCpssLogParamFunc_GT_BOOL_ARRAY
*
* DESCRIPTION:
*       logs an array of GT_BOOL elements
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_BOOL_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);


/*******************************************************************************
* prvCpssLogParamFunc_DSA_PTR
*
* DESCRIPTION:
*       logs an array of GT_U8 as DSA (array of words)
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_DSA_PTR(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);

/*******************************************************************************
* prvCpssLogParamFunc_GT_CHAR_ARRAY
*
* DESCRIPTION:
*       logs an array of GT_CHAR as a string
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       argsPtr -
*           additional args
*       skipLog -
*           to skip the log or not
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFunc_GT_CHAR_ARRAY(
    IN CPSS_LOG_LIB_ENT            contextLib,
    IN CPSS_LOG_TYPE_ENT           logType,
    IN GT_CHAR_PTR                 namePtr,
    IN va_list                   * argsPtr,
    IN GT_BOOL                     skipLog,
    IN PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC   * inOutParamInfoPtr
);

/*******************************************************************************
* prvCpssLogStcHexNumber
*
* DESCRIPTION:
*       The function to log hex parameter
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       fieldName - structure field name
*       numValue - field value.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogStcHexNumber
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              fieldName,
    IN GT_U32                   numValue
);

/*******************************************************************************
* prvCpssLogParamFuncStc_GT_U32_HEX_PTR
*
* DESCRIPTION:
*       logs a variable from GT_U32 type in hexa format
*
* INPUTS:
*       contextLib -
*           lib log activity
*       logType -
*           type of the log
*       namePtr -
*           array name.
*       fieldPtr -
*           pointer to the field
*       inOutParamInfoPtr -
*           pointer to information about the array of parameters
*
* OUTPUTS:
*       None
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssLogParamFuncStc_GT_U32_HEX_PTR
(
    IN CPSS_LOG_LIB_ENT         contextLib,
    IN CPSS_LOG_TYPE_ENT        logType,
    IN GT_CHAR_PTR              namePtr,
    IN void                   * fieldPtr,
    INOUT PRV_CPSS_LOG_PARAM_ENTRY_INFO_STC     * inOutParamInfoPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssLogh */

