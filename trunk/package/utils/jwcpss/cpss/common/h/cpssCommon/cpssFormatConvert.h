/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssFormatConvert.h
*
* DESCRIPTION:
*       General functions for the bidirectional structured to raw format
*       data conversion. The structured data may be either C language struct
*       or union.The utility uses array of 32 bits words for raw data structures.
*       All functions place the data to the appropriate positions
*       of destination buffer using OR operation.
*       The destination buffer must be cleared by a caller.
*       The sequence of calls with the same destination buffer is
*       legitimate if it accumulates the different data.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/

#ifndef __cpssFormatConverth
#define __cpssFormatConverth

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* the MACRO calculates an relative offset of a member of structure or union.
   Offset is in bytes and calculated from the start of structure or union.
   Parameters:
   IN _s  - name of struct or union
   IN _m  - name of member
 */
#define CPSS_FORMAT_CONVERT_STRUCT_MEMBER_OFFSET_MAC(_s, _m) \
    (int)((char*)&(((_s*)0)->_m) - (char*)0)

/* the MACRO calculates size of the member of structure or union.
   Size is in bytes and calculated from the start of structure or union.
   Parameters:
   IN _s  - name of struct or union
   IN _m  - name of member
*/
#define CPSS_FORMAT_CONVERT_STRUCT_MEMBER_SIZE_MAC(_s, _m) \
    sizeof(((_s*)0)->_m)

/*
 * Typedef: enum CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT
 *
 * Description: enumerator for structured data type or sizeof integer data.
 *             The sizeof type defined as a result of C-language sizeof operator
 *
 *  CPSS_FORMAT_CONVERT_FIELD_SIZEOF_1BYTE_E       - 1 BYTE field in SW structure
 *  CPSS_FORMAT_CONVERT_FIELD_SIZEOF_2BYTES_E      - 2 BYTES field in SW structure
 *  CPSS_FORMAT_CONVERT_FIELD_SIZEOF_4BYTES_E      - 4 BYTES field in SW structure
 *  CPSS_FORMAT_CONVERT_FIELD_SIZEOF_8BYTES_E      - 8 BYTES field in SW structure
 *                             reserved for 64-bit CPU
 *   BOOLs....
 *  CPSS_FORMAT_CONVERT_FIELD_BOOL_E               - GT_BOOL field in SW structure
 *  CPSS_FORMAT_CONVERT_FIELD_INVERTED_BOOL_E      - GT_BOOL field in SW structure
 *                             that must be inverted during retrieve
 *                              stc data | raw data BOOL |raw data Inverted BOOL
 *                               GT_TRUE |    1          |     0
 *                              GT_FALSE |    0          |     1
 *
 *  External memory data
 *  CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_1BYTE_E   - 1 byte word in ext memory
 *  CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_2BYTES_E  - 2 bytes word in ext memory
 *  CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_4BYTES_E  - 4 bytes word in ext memory
 *  SW structure Byte array, substructure
 *  CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E - array of bytes in network order
 *               represent value that is in the little-endian order in raw format.
 *               E.g. MAC address MSB byte#0 is in the relative position bits 40:47,
 *               MAC address LSB byte#5 is in the relative position bits 0:7.
 *  CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_ASCENT_E - array of bytes is
 *              in ascent order in raw format.
 *              E.g. byte#0 is in the relative position bits 0:7,
 *              byte#1 is in bits 8:15.
 *  CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E    - structure or union field in SW structure.
 *  CPSS_FORMAT_CONVERT_FIELD_CONSTANT_VALUE_E - constant with value
 *                                              in "stcByteOffsetOrConst"
 *  CPSS_FORMAT_CONVERT_FIELD_DUMMY_E      - entry only to set context value
 *  CPSS_FORMAT_CONVERT_FIELD_ENDLIST_E    - constant to represent end of array
 *
 * Fields:
 */
typedef enum
{
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_1BYTE_E        = 1,
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_2BYTES_E       = 2,
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_4BYTES_E       = 4,
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_8BYTES_E       = 8,
    CPSS_FORMAT_CONVERT_FIELD_BOOL_E,
    CPSS_FORMAT_CONVERT_FIELD_INVERTED_BOOL_E,
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_1BYTE_E,
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_2BYTES_E,
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_4BYTES_E,
    CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E,
    CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_ASCENT_E,
    CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E,
    CPSS_FORMAT_CONVERT_FIELD_CONSTANT_VALUE_E,
    CPSS_FORMAT_CONVERT_FIELD_DUMMY_E,
    CPSS_FORMAT_CONVERT_FIELD_ENDLIST_E
} CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT;

/*
 * Typedef: enum CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT
 *
 * Description: the enum defines type of the context value.
 *      The format convert state machine contains 4 values that not
 *      present in CPSS_FORMAT_CONVERT_FIELD_INFO_STC structure but may be
 *      used in each entry. These values are context of a conversion:
 *       - field's value conversion table.
 *         The utility may convert a value of particular field before write
 *         it to raw or structured format. It may be used to convert enum fields
 *         to appropriate HW value.
 *         The conversion table is simple array of the pair
 *         {structured value, raw value} (see CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC).
 *         The utility linearly searches the table
 *         by appropriate value and take converted one from a found pair.
 *       - field's value conversion function.
 *         The utility may use the callback function to convert values.
 *       - condition.
 *         The condition is structure that calculates BOOLEAN value from a
 *         field of converted structure. It may be used for the conversion
 *         of union.
 *       - maximal value for validity check of a field in structured data.
 *      These values (context) are initially NULL for each sub table
 *      (i.e substructure fields conversion table).
 *      For any field type different from CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
 *      the "extraInfoPtr" member of the CPSS_FORMAT_CONVERT_FIELD_INFO_STC can
 *      be used for context data but only for the current field processing.
 *      Or/and the "extraInfoPtr" member may be saved into the context for
 *      following fields in a conversion table.
 *
 *
 * Fields:
 *      CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_TAB_E  - conversion table pointer
 *      CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_FUNC_E - conversion function pointer
 *      CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONDITION_E    - condition structure pointer
 *                     The conversion function once checks condition and saves it
 *                     result - not pointer. For raw to struct conversion the condition
 *                     referred field must be retrieved before condition set
 *                     (not before use)
 *      CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E     - 32-bit max-value for structured
 *                                                         field value check
 */
typedef enum
{
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_TAB_E  = 0,
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_FUNC_E = 1,
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONDITION_E    = 2,
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E     = 3
} CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT;

/*
 * Typedef: enum CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT
 *
 * Description: the context management commands related to the
 *      "extraInfoPtr" member of the CPSS_FORMAT_CONVERT_FIELD_INFO_STC
 *      assumed to contain the CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT typed data
 *
 * Fields:
 *     "CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_NONE_E          - nothing to do
 *     "CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_USE_NOT_SAVE_E  - use only for the
 *                                current entry, don't save to the context
 *     "CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_SAVE_NOT_USE_E  - for the current entry
 *                                use the old context value, but save this for
 *                                the next entries
 *     "CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_USE_AND_SAVE_E  - save this value in
 *                                the context and use in for
 *                                both this and the next entries
 *
 */
typedef enum
{
    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_NONE_E = 0,
    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_USE_NOT_SAVE_E = 1,
    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_SAVE_NOT_USE_E = 2,
    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_USE_AND_SAVE_E = 3
} CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT;

/*
 * Typedef: enum CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT
 *
 * Description: the value convert commands
 *
 * Fields:
 *    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E          - don't convert
 *    CPSS_FORMAT_CONVERT_FIELD_CONVERT_BY_TABLE_E      - convert by table
 *    CPSS_FORMAT_CONVERT_FIELD_CONVERT_BY_FUNCTION_E   - convert by function
 *
 */
typedef enum
{
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E          = 0,
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_BY_TABLE_E      = 1,
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_BY_FUNCTION_E   = 2
} CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT;

/*
 * Typedef: enum CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT
 *
 * Description: the value check commands
 *
 * Fields:
 *    CPSS_FORMAT_CONVERT_FIELD_CHECK_BITS_E         - check stc value by maximal
 *                                                   number of bits in raw field
 *     CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E        - don't check
 *    CPSS_FORMAT_CONVERT_FIELD_CHECK_BY_MAXVALUE_E  - check the value to be less
 *              or equal to context "MAXVALUE"
 *
 */
typedef enum
{
    CPSS_FORMAT_CONVERT_FIELD_CHECK_BITS_E         = 0,
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E         = 1,
    CPSS_FORMAT_CONVERT_FIELD_CHECK_BY_MAXVALUE_E  = 2
} CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT;

/* the defines below describes format of GT_U16 "fieldCommand" of the
   CPSS_FORMAT_CONVERT_FIELD_INFO_STC
 it contains:
 - field type or sizeof                      - 5 bits CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT
 - the "extraInfoPtr" context manage command - 3 bits CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT
 - the "extraInfoPtr" context data type      - 3 bits CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT
 - the "condition-dependence"                - 1 bit  BOOL
                                               value 1 - field is converted only
                                                 when condition value of the context is TRUE
                                               value 0 - field is converted always
                                                 regardless of condition value of the context
 - the "conversion-command"                  - 3 bits CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT
 - the  value-check command                  - 3 bits CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT
 - */
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_FIELD_TYPE_BASE_CNS       0
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_FIELD_TYPE_MASK_CNS    0x1F
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CMD_BASE_CNS          5
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CMD_MASK_CNS          3
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CONTEXT_BASE_CNS      7
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CONTEXT_MASK_CNS      3
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CONDITION_BIT_CNS         9
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CONVERT_CMD_BASE_CNS     10
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CONVERT_CMD_MASK_CNS     3
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CHECK_CMD_BASE_CNS       12
#define CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CHECK_CMD_MASK_CNS        3

/* the macros to pack/unpack the fieldCommand" */
/*
   Unpack value of the fieldCommand in the CPSS_FORMAT_CONVERT_FIELD_INFO_STC
   IN    _fieldCommand   - value of the fieldCommand
   OUT  _fieldTypeOrSize - type of a field or sizeof integer field
        _ptrCmd          - the "extraInfoPtr" context manage command,
                           see CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT
        _ptrType         - the "extraInfoPtr" context data type
                           see CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT
        _conditionDep    - the "condition-dependence"
                           value 1 - field is converted only
                             when condition value of the context is TRUE
                           value 0 - field is converted always
                             regardless of condition value of the context
        _convertCmd      - the field vale conversion command,
                           see CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT
        _checkCmd        - the field value check command
                           see CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT
*/
#define CPSS_FORMAT_CONVERT_FIELD_COMMAND_UNPACK_MAC(                               \
    _fieldCommand, _fieldTypeOrSize, _ptrCmd, _ptrType, _conditionDep,              \
    _convertCmd, _checkCmd)                                                         \
    { _fieldTypeOrSize = CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_FIELD_TYPE_MASK_CNS      \
       & (_fieldCommand >> CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_FIELD_TYPE_BASE_CNS);  \
      _ptrCmd = CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CMD_MASK_CNS                  \
        & (_fieldCommand >> CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CMD_BASE_CNS);    \
      _ptrType = CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CONTEXT_MASK_CNS             \
        & (_fieldCommand >> CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CONTEXT_BASE_CNS);\
     _conditionDep = 1                                                              \
        & (_fieldCommand >> CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CONDITION_BIT_CNS);   \
     _convertCmd = CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CONVERT_CMD_MASK_CNS           \
        & (_fieldCommand >> CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CONVERT_CMD_BASE_CNS);\
     _checkCmd = CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CHECK_CMD_MASK_CNS               \
        & (_fieldCommand >> CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CHECK_CMD_BASE_CNS);}

/*
   Pack value of the fieldCommand in the CPSS_FORMAT_CONVERT_FIELD_INFO_STC
   OUT  _fieldTypeOrSize - type of a field or sizeof integer field
        _ptrCmd          - the "extraInfoPtr" context manage command,
                           see CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT
        _ptrType         - the "extraInfoPtr" context data type
                           see CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT
        _conditionDep    - the "condition-dependence"
                           value 1 - field is converted only
                             when condition value of the context is TRUE
                           value 0 - field is converted always
                             regardless of condition value of the context
        _convertCmd      - the field vale conversion command,
                           see CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT
        _checkCmd        - the field value check command
                           see CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT
   RETURN: "fieldCommand" of the CPSS_FORMAT_CONVERT_FIELD_INFO_STC

 */
#define CPSS_FORMAT_CONVERT_FIELD_COMMAND_PACK_MAC(                              \
    _fieldTypeOrSize, _ptrCmd, _ptrType, _conditionDep,                          \
    _convertCmd, _checkCmd)                                                      \
    (((_fieldTypeOrSize & CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_FIELD_TYPE_MASK_CNS) \
       << CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_FIELD_TYPE_BASE_CNS) |               \
     ((_ptrCmd & CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CMD_MASK_CNS)             \
        << CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CMD_BASE_CNS) |                 \
     ((_ptrType & CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CONTEXT_MASK_CNS)        \
        << CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_PTR_CONTEXT_BASE_CNS) |             \
     ((_conditionDep & 1)                                                        \
        << CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CONDITION_BIT_CNS) |                \
     ((_convertCmd & CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CONVERT_CMD_MASK_CNS)     \
        << CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CONVERT_CMD_BASE_CNS) |             \
     ((_checkCmd & CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CHECK_CMD_MASK_CNS)         \
        << CPSS_FORMAT_CONVERT_FIELD_CMD_BMP_CHECK_CMD_BASE_CNS))


/* end of CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC array
  this entry must be last one in a value conversion table */
#define CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_TAB_END_CNS {0xFFFFFFFF, 0xFFFFFFFF}

/*
 * typedef: struct CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC
 *
 * Description: Field SW structure to raw value conversion
 *
 * Fields:
 *   stcValue - structured data Value
 *   rawValue - raw data Value
 *
 * Comments:
 */
typedef struct
{
    GT_U32      stcValue;
    GT_U32      rawValue;

} CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_STC;

/*******************************************************************************
* CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_FUNC
*
* DESCRIPTION:
*   The user defined function to convert data
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       stcToRaw   - GT_TRUE - stc to raw coversion, GT_FALSE - raw to stc
*       srcData    - value to be converted
*
* OUTPUTS:
*       dstDataPtr         - (pointer to) conversion result
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND   - on wrong data
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_FORMAT_CONVERT_FIELD_VAL_CONVERT_FUNC)
(
    IN  GT_BOOL     stcToRaw,
    IN  GT_U32      srcValue,
    OUT GT_U32      *dstValuePtr
);

/*
 * typedef: struct CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC
 *
 * Description: Field to calculate and check condition
 *     condition is optional and typicaly used for substructures in unions
 *     the condition is BOOLEN value and equal to
 *     <value> == (<mask> & <pattern>)
 *
 * Fields:
 *      fieldTypeOrSizeOf      - type of field which contains value to
 *                               compare with the pattern
 *      stcOrExtMemByteOffset  - offset (in bytes from SW structure origin) of
 *                               field which contains value to
 *                               compare with the pattern
 *                               the field may be in structured data or
 *                               external memory
 *      mask                   - mask in structured data format
 *      pattern                - pattern in structured data format
 *
 * Comments:
 */
typedef struct
{
    CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT     fieldTypeOrSizeOf;
    GT_U16                                 stcOrExtMemByteOffset;
    GT_U32                                 mask;
    GT_U32                                 pattern;

} CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC;

/*
 * typedef: struct CPSS_FORMAT_CONVERT_FIELD_INFO_STC
 *
 * Description: Conversion table's entry for stc to raw bidirectional conversion
 *
 * Fields:
 *      extraInfoPtr    - extra information for complex conversions.
 *                        the content of the field is defined by fieldCommand
 *                        The value of the field may be:
 *                         - pointer to another conversion table for substructures
 *                         - GT_U32 maximal value for check purpose
 *                         - pointer to condition structure
 *                         - pointer to stc<->raw conversion array
 *                         - pointer to stc<->raw conversion function
 *      fieldCommand    -  this field contains packed information of:
 *                         - type of field or sizeof as type
 *                         - the "extraInfoPtr" context manage command
 *                         - the "extraInfoPtr" context data type
 *                         - the  condition-dependence bit
 *                         - the  conversion-command
 *                         - the  value-check command
 *      stcByteOffsetOrConst  - offset in bytes from SW structure origin
 *                         for CONST type contains constant
 *      rawBitOffset    - offset (in bits) in the raw data
 *                         when field type is
 *                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
 *                         used as offset in raw data array pointer
 *                         passed by recursive call
 *                         (this offset measured in 32-bit words)
 *      rawBitLength    - amounts of bits in the raw data
 *      stcBitOffset    - the offset (in bits) from the origin of the
 *                         structure field (for field splited to
 *                         several segments)
 *
 * Comments:
 */
typedef struct __CPSS_FORMAT_CONVERT_FIELD_INFO_STC
{
    GT_VOID    *extraInfoPtr;
    GT_U16     fieldCommand;
    GT_U16     stcByteOffsetOrConst;
    GT_U16     rawBitOffset;
    GT_U8      rawBitLength;
    GT_U8      stcBitOffset;

} CPSS_FORMAT_CONVERT_FIELD_INFO_STC;

/******************************************************************************/
/****  USEFULL MACROS to create CPSS_FORMAT_CONVERT_FIELD_INFO_STC  entries ***/

/* The macro creates CPSS_FORMAT_CONVERT_FIELD_INFO_STC entry
   All parameters are INPUT
   _fieldTypeOrSize  - Stc field type or sizeof. The value must be one of
                       CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT
  _stcByteOffsetOrConst - Stc field relative byte offset or
                           constant value for CPSS_FORMAT_CONVERT_FIELD_CONSTANT_VALUE_E
                           The offset may be get by CPSS_FORMAT_CONVERT_STRUCT_MEMBER_OFFSET_MAC
  _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer
 _rawBitLength         - length in bits for a field in raw data

 _stcBitOffset         - offset in bits within Stc field. Used for splited fields.

 _conditionDep         - the condition dependence
                           value 1 - field is converted only
                             when condition value of the context is TRUE
                           value 0 - field is converted always
                             regardless of condition value of the context

 _convertCmd           - conversion command. CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT

 _checkCmd             - Stc field check command. CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT

 _ptrCmd               - context managment command. CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_ENT

 _ptrType              - extra info type. CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT

 _extraInfoPtr         - extra info
                       */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_MAC(                                      \
    _fieldTypeOrSize, _stcByteOffsetOrConst,                                     \
    _rawBitOffset, _rawBitLength, _stcBitOffset,                                 \
    _conditionDep, _convertCmd, _checkCmd,                                       \
    _ptrCmd, _ptrType, _extraInfoPtr                                             \
    )                                                                            \
    {                                                                            \
         _extraInfoPtr,                                                          \
       (GT_U16)CPSS_FORMAT_CONVERT_FIELD_COMMAND_PACK_MAC(                       \
             _fieldTypeOrSize, _ptrCmd, _ptrType, _conditionDep,                 \
             _convertCmd, _checkCmd),                                            \
       (GT_U16)_stcByteOffsetOrConst,                                            \
       (GT_U16)_rawBitOffset,                                                    \
       (GT_U8)_rawBitLength,                                                     \
       (GT_U8)_stcBitOffset                                                      \
    }

/* Macro that must be used to fill in last entry in the format conversion table
   (CPSS_FORMAT_CONVERT_FIELD_INFO_STC array)
   The macro is without parameters and creates last entry in a conversion table */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_END_MAC                                   \
    CPSS_FORMAT_CONVERT_FIELD_INFO_MAC(                                          \
    CPSS_FORMAT_CONVERT_FIELD_ENDLIST_E, 0/*_stcByteOffsetOrConst*/,             \
    0/*_rawBitOffset*/, 0/*_rawBitLength*/, 0/*_stcBitOffset*/,                  \
    0/*_conditionDep*/, CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,                                \
    NULL/*_extraInfoPtr*/)

/* Macro creates entry without conversion but for the conditional exit/return
   from format conversion table (CPSS_FORMAT_CONVERT_FIELD_INFO_STC array)
   Parameters:
   IN _conditionPtr - pointer to condition structure
   */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_COND_RETURN_MAC(_conditionPtr)            \
    CPSS_FORMAT_CONVERT_FIELD_INFO_MAC(                                          \
    CPSS_FORMAT_CONVERT_FIELD_ENDLIST_E, 0/*_stcByteOffsetOrConst*/,             \
    0/*_rawBitOffset*/, 0/*_rawBitLength*/, 0/*_stcBitOffset*/,                  \
    1/*_conditionDep*/, CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_USE_NOT_SAVE_E,                            \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONDITION_E,                               \
    _conditionPtr/*_extraInfoPtr*/)

/* macros only to save to the context                                           */
/* conversion table                                                             */
/* conversion function                                                          */
/* condition result                                                             */
/* maximal value                                                                */

/* macro creates entry that only save value to the context
   All parameters are INPUT
   _ptrType              - extra info type. CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT
   _extraInfoPtr         - extra info, may be one of the list below and must
                           accord to _ptrType:
                           - pointer to value conversion table
                           - pointer to value conversion function
                           - pointer to condition structure
                           - maximal value of field for validity check
 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_GEN_MAC(_ptrType, _valInPtr)         \
    CPSS_FORMAT_CONVERT_FIELD_INFO_MAC(                                          \
    CPSS_FORMAT_CONVERT_FIELD_DUMMY_E, 0/*_stcByteOffsetOrConst*/,               \
    0/*_rawBitOffset*/, 0/*_rawBitLength*/, 0/*_stcBitOffset*/,                  \
    0/*_conditionDep*/, CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_SAVE_NOT_USE_E,                            \
    _ptrType, (GT_VOID*)_valInPtr/*_extraInfoPtr*/)

/* create entry that saves the conversion table address to the context */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_CNV_TAB_MAC(_cnvTabPtr)              \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_GEN_MAC(                                 \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_TAB_E, _cnvTabPtr)

/* create entry that saves the conversion function address to the context */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_CNV_FUNC_MAC(_cnvFuncPtr)            \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_GEN_MAC(                                 \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_FUNC_E, _cnvFuncPtr)

/* create entry that saves the max value to the context */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_MAX_VALUE_MAC(_maxValue)             \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_GEN_MAC(                                 \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E, _maxValue)

/* create entry that saves the condition result to the context */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_CONDITION_MAC(_conditionPtr)         \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SAVE_GEN_MAC(                                 \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONDITION_E, _conditionPtr)

/* the macro calculates PTR context manage command by the following
   INPUT parameters
    _use  - 1 - the conversion entry uses value of extraInfoPtr
            0 - the conversion entry does not use values of extraInfoPtr
    _save - 1 - the conversion entry saves valu of extraInfoPtr to context
            0 - the conversion entry does not save value of extraInfoPtr to context
*/
#define CPSS_FORMAT_CONVERT_FIELD_INFO_PTR_CMD_MAC(_use, _save)                  \
    ((_use & _save) ? CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_USE_AND_SAVE_E           \
    : (_use ? CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_USE_NOT_SAVE_E                   \
       : (_save ? CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_SAVE_NOT_USE_E               \
         : CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_NONE_E)))

/* the macro creates conversion entry for constant.
   the value of constant is placed to raw data according to context.
   All parameters are INPUT
 _value              - value of the constant

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer
 _rawBitLength         - length in bits for a field in raw data

 _conditionDep         - condition dependency bit. 1 - use condition context.
                                                   0 - don't use

 _ptrUse              - Use extraInfoPtr command
                        1 - the conversion entry uses value of extraInfoPtr
                        0 - the conversion entry does not use value of extraInfoPtr

 _ptrSave             - Save extraInfoPtr to context command
                        1 - the conversion entry saves value of extraInfoPtr to context
                        0 - the conversion entry does not save value of extraInfoPtr to context

 _ptrType              - extra info type. CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT

 _extraInfoPtr         - extra info
 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_CONSTANT_GEN_MAC(                         \
    _value, _rawBitOffset, _rawBitLength, _conditionDep,                         \
    _ptrUse, _ptrSave, _ptrType, _extraInfoPtr)                                  \
    CPSS_FORMAT_CONVERT_FIELD_INFO_MAC(                                          \
    CPSS_FORMAT_CONVERT_FIELD_CONSTANT_VALUE_E,                                  \
    _value/*_stcByteOffsetOrConst*/,                                             \
    _rawBitOffset, _rawBitLength, 0/*_stcBitOffset*/,                            \
   _conditionDep, CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                      \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    CPSS_FORMAT_CONVERT_FIELD_INFO_PTR_CMD_MAC(_ptrUse, _ptrSave),               \
    _ptrType,                                                                    \
    _extraInfoPtr)


/* the macro creates conversion entry for constant that dependant or not from
   previous saved condition .
   the value of constant is placed to raw data according to previous saved
   condition if _conditionDep = 1. Or the value of constant is placed to raw
   data unconditionally if _conditionDep = 0.
   All parameters are INPUT
 _value              - value of the constant

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer
 _rawBitLength         - length in bits for a field in raw data

 _conditionDep         - condition dependency bit. 1 - use condition context.
                                                   0 - don't use
*/
#define CPSS_FORMAT_CONVERT_FIELD_INFO_CONSTANT_MAC(                             \
    _value, _rawBitOffset, _rawBitLength, _conditionDep)                         \
    CPSS_FORMAT_CONVERT_FIELD_INFO_CONSTANT_GEN_MAC(                             \
    _value, _rawBitOffset, _rawBitLength, _conditionDep,                         \
     0/*_ptrUse*/, 0/*_ptrSave*/,                                                \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,                                \
    NULL/*_extraInfoPtr*/)

/* the macro creates conversion entry for constant that dependant from
   present by _conditionPtr condition .
   the value of constant is placed to raw data according to
    _conditionPtr condition. The value of contition may be stored to context
   according to _ptrSave parameter.

   All parameters are INPUT
 _value              - value of the constant

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer
 _rawBitLength         - length in bits for a field in raw data

 _conditionPtr         - pointer to condition struct

 _ptrSave             - Save condition to context command
                        1 - the conversion entry saves value of condition to context
                        0 - the conversion entry does not save value of condition to context

 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_CONSTANT_COND_MAC(                        \
    _value, _rawBitOffset, _rawBitLength, _conditionPtr, _ptrSave)               \
    CPSS_FORMAT_CONVERT_FIELD_INFO_CONSTANT_GEN_MAC(                             \
    _value, _rawBitOffset, _rawBitLength, 1/*_conditionDep*/,                    \
     1/*_ptrUse*/, _ptrSave,                                                     \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONDITION_E,                               \
    _conditionPtr/*_extraInfoPtr*/)

/* the macro creates conversion entry for sub structure/sub union
   the utility performs conversion of sub struct/union according to conversion
   table defined by _substructCnvPtr.Conversion of sub struct/union may be
   performed with condition in the context check if _conditionDep = 1
   All parameters are INPUT
   _structType - name of struct/union
   _member     - name of sub struct/union that is member of _structType
   _rawWordsOffset - base offset in (32-bit words) in raw data to be used
                     as origin in reffered subtree
   _conditionDep - condition dependant BOOL bit
                   1 - check condition stored in the context
                   0 - don't check condition
  _substructCnvPtr - pointer to conversion table of sub struct/union (defined
                   by _member)
 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCTURE_GEN_WITH_RAW_OFFSET_MAC(        \
    _structType, _member,_rawWordsOffset, _conditionDep, _substructCnvPtr)       \
    CPSS_FORMAT_CONVERT_FIELD_INFO_MAC(                                          \
    CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E,                                         \
    CPSS_FORMAT_CONVERT_STRUCT_MEMBER_OFFSET_MAC(_structType, _member),          \
    _rawWordsOffset/*_rawBitOffset*/, 0/*_rawBitLength*/, 0/*_stcBitOffset*/,    \
   _conditionDep,                                                                \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,                                \
    _substructCnvPtr/*_extraInfoPtr*/)


/* the macro creates conversion entry for sub structure/sub union
   the utility performs conversion of sub struct/union according to conversion
   table defined by _substructCnvPtr.Conversion of sub struct/union may be
   performed with condition in the context check if _conditionDep = 1
   All parameters are INPUT
   _structType - name of struct/union
   _member     - name of sub struct/union that is member of _structType
   _conditionDep - condition dependant BOOL bit
                   1 - check condition stored in the context
                   0 - don't check condition
  _substructCnvPtr - pointer to conversion table of sub struct/union (defined
                   by _member)
 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCTURE_GEN_MAC(                        \
    _structType, _member, _conditionDep, _substructCnvPtr)                       \
    CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCTURE_GEN_WITH_RAW_OFFSET_MAC(            \
    _structType, _member, 0/*_rawWordsOffset*/,                                  \
    _conditionDep, _substructCnvPtr)

/* subtree for the same structure */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SAME_STRUCT_SUBTREE_WITH_RAW_OFFSET_MAC(  \
    _rawWordsOffset, _conditionDep, _substructCnvPtr)                            \
    CPSS_FORMAT_CONVERT_FIELD_INFO_MAC(                                          \
    CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E,                                         \
    0/*_stcByteOffsetOrConst*/,                                                  \
    _rawWordsOffset/*_rawBitOffset*/, 0/*_rawBitLength*/,                        \
    0/*_stcBitOffset*/, _conditionDep,                                           \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    CPSS_FORMAT_CONVERT_FIELD_PTR_CMD_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,                                \
    _substructCnvPtr/*_extraInfoPtr*/)

#define CPSS_FORMAT_CONVERT_FIELD_INFO_SAME_STRUCT_SUBTREE_MAC(                  \
    _conditionDep, _substructCnvPtr)                                             \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SAME_STRUCT_SUBTREE_WITH_RAW_OFFSET_MAC(      \
    0/*_rawWordsOffset*/, _conditionDep, _substructCnvPtr)


/* the macro creates conversion entry for member of struct or union.
   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _type                - size of integer member or type for other kind of member.
                        see CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _stcBitOffset        - the offset (in bits) from the origin of the
                        structure field (for field splitted to
                        several segments)

 _conditionDep        - the condition dependence bit
                        1 - field is converted only
                            when condition value of the context is TRUE
                        0 - field is converted always
                            regardless of condition value of the context

 _convertCmd          - the field value conversion command,
                        see CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT

 _checkCmd            - the field value check command
                        see CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT

 _ptrUse              - Use extraInfoPtr command
                        1 - the conversion entry uses value of extraInfoPtr
                        0 - the conversion entry does not use value of extraInfoPtr

 _ptrSave             - Save extraInfoPtr to context command
                        1 - the conversion entry saves value of extraInfoPtr to context
                        0 - the conversion entry does not save value of extraInfoPtr to context

 _ptrType             - extra info type. CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT

 _extraInfoPtr        - extra info. see possible values in CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT
 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCT_MEMBER_GEN_MAC(                    \
    _structType, _member, _type,                                                 \
    _rawBitOffset, _rawBitLength, _stcBitOffset,                                 \
    _conditionDep, _convertCmd, _checkCmd,                                       \
    _ptrUse, _ptrSave, _ptrType, _extraInfoPtr)                                  \
    CPSS_FORMAT_CONVERT_FIELD_INFO_MAC(                                          \
    _type, CPSS_FORMAT_CONVERT_STRUCT_MEMBER_OFFSET_MAC(_structType, _member),   \
    _rawBitOffset, _rawBitLength, _stcBitOffset,                                 \
    _conditionDep, _convertCmd, _checkCmd,                                       \
    CPSS_FORMAT_CONVERT_FIELD_INFO_PTR_CMD_MAC(_ptrUse, _ptrSave),               \
    _ptrType,                                                                    \
    _extraInfoPtr)

/* the macro creates conversion entry for integer member of struct or union.
   integer member may be type of GT_U8,GT_U16,GT_U32,GT_8,GT_16,GT_32 or enum
   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _stcBitOffset        - the offset (in bits) from the origin of the
                        structure field (for field splitted to
                        several segments)

 _conditionDep        - the condition dependence bit
                        1 - field is converted only
                            when condition value of the context is TRUE
                        0 - field is converted always
                            regardless of condition value of the context

 _convertCmd          - the field value conversion command,
                        see CPSS_FORMAT_CONVERT_FIELD_CONVERT_CMD_ENT

 _checkCmd            - the field value check command
                        see CPSS_FORMAT_CONVERT_FIELD_CHECK_ENT

 _ptrUse              - Use extraInfoPtr command
                        1 - the conversion entry uses value of extraInfoPtr
                        0 - the conversion entry does not use value of extraInfoPtr

 _ptrSave             - Save extraInfoPtr to context command
                        1 - the conversion entry saves value of extraInfoPtr to context
                        0 - the conversion entry does not save value of extraInfoPtr to context

 _ptrType             - extra info type. CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT

 _extraInfoPtr        - extra info. see possible values in CPSS_FORMAT_CONVERT_FIELD_CONTEXT_ENT
 */

#define CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_GEN_MAC(                        \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, _stcBitOffset,                                 \
    _conditionDep, _convertCmd, _checkCmd,                                       \
    _ptrUse, _ptrSave, _ptrType, _extraInfoPtr)                                  \
     CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCT_MEMBER_GEN_MAC(                       \
    _structType, _member,                                                        \
    CPSS_FORMAT_CONVERT_STRUCT_MEMBER_SIZE_MAC(_structType, _member),            \
    _rawBitOffset, _rawBitLength, _stcBitOffset,                                 \
    _conditionDep, _convertCmd, _checkCmd,                                       \
    _ptrUse, _ptrSave, _ptrType, _extraInfoPtr)

/* the macro creates conversion entry for integer member of struct or union.
   integer member may be type of GT_U8,GT_U16,GT_U32,GT_8,GT_16,GT_32 or enum.
   the conversion is done unconditionally.
   the stc to raw conversion check field by size of raw field.
   no value conversion is performed.
   cannot be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 */

#define CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_MAC(                            \
    _structType, _member, _rawBitOffset, _rawBitLength)                          \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_GEN_MAC(                            \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, 0/*_stcBitOffset*/,                            \
    0/*_conditionDep*/,                                                          \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_BITS_E,                                      \
    0/*_ptrUse*/, 0/*_ptrSave*/, CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,   \
    NULL/*_extraInfoPtr*/)

/* the macro creates conversion entry for member of struct or union with specific type.
   the type may be one of:
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_1BYTE_E
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_2BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_4BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_BOOL_E
    CPSS_FORMAT_CONVERT_FIELD_INVERTED_BOOL_E
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_1BYTE_E
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_2BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_4BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E

   the conversion is done unconditionally.
   the stc to raw conversion does not check field.
   no value conversion is performed.
   cannot be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _type                - size of integer member or type for other kind of member.
                        see CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 */

#define CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_MEMBER_MAC(                         \
    _structType, _member, _type, _rawBitOffset, _rawBitLength)                   \
    CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCT_MEMBER_GEN_MAC(                        \
    _structType, _member, _type,                                                 \
    _rawBitOffset, _rawBitLength, 0/*_stcBitOffset*/,                            \
    0/*_conditionDep*/,                                                          \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    0/*_ptrUse*/, 0/*_ptrSave*/, CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,   \
    NULL/*_extraInfoPtr*/)

/* the macro creates conversion entry for integer member of struct or union.
   integer member may be type of GT_U8,GT_U16,GT_U32,GT_8,GT_16,GT_32 or enum.
   the conversion is done unconditionally.
   the stc to raw conversion check field by size of raw field.
   no value conversion is performed.
   May be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _stcBitOffset        - the offset (in bits) from the origin of the
                        structure field (for field splitted to
                        several segments)
 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_MAC(                      \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, _stcBitOffset)                                 \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_GEN_MAC(                            \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, _stcBitOffset,                                 \
    0/*_conditionDep*/,                                                          \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    0/*_ptrUse*/, 0/*_ptrSave*/, CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,   \
    0/*_extraInfoPtr*/)

/* the macro creates conversion entry for integer member of struct or union.
   integer member may be type of GT_U8,GT_U16,GT_U32,GT_8,GT_16,GT_32 or enum.
   the conversion is done only if condition in _conditionPtr is TRUE.
   the stc to raw conversion check field by size of raw field.
   no value conversion is performed.
   May be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _stcBitOffset        - the offset (in bits) from the origin of the
                        structure field (for field splitted to
                        several segments)
 _conditionPtr        - pointer to condition struct
 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_COND_MAC(                 \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, _stcBitOffset, _conditionPtr)                  \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_GEN_MAC(                            \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, _stcBitOffset,                                 \
    1/*_conditionDep*/,                                                          \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    1/*_ptrUse*/, 0/*_ptrSave*/, CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONDITION_E,  \
    _conditionPtr/*_extraInfoPtr*/)

/* the macro creates conversion entry for integer member of struct or union.
   integer member may be type of GT_U8,GT_U16,GT_U32,GT_8,GT_16,GT_32 or enum.
   the conversion is done only if condition in context is TRUE.
   the stc to raw conversion check field by size of raw field.
   no value conversion is performed.
   May be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _stcBitOffset        - the offset (in bits) from the origin of the
                        structure field (for field splitted to
                        several segments)
 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_SPLIT_CONTEXT_COND_MAC(         \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, _stcBitOffset)                                 \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_GEN_MAC(                            \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, _stcBitOffset,                                 \
    1/*_conditionDep*/,                                                          \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    0/*_ptrUse*/, 0/*_ptrSave*/, CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,  \
    NULL/*_extraInfoPtr*/)

/* the macro creates conversion entry for integer member of struct or union.
   integer member may be type of GT_U8,GT_U16,GT_U32,GT_8,GT_16,GT_32 or enum.
   the conversion is done only if condition in _conditionPtr is TRUE.
   the stc to raw conversion check field by size of raw field.
   no value conversion is performed.
   Cannot be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _conditionPtr        - pointer to condition struct
 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_COND_MAC(                       \
    _structType, _member, _rawBitOffset, _rawBitLength, _conditionPtr)           \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_GEN_MAC(                            \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, 0/*_stcBitOffset*/,                            \
    1/*_conditionDep*/,                                                          \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_BITS_E,                                      \
    1/*_ptrUse*/, 0/*_ptrSave*/, CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONDITION_E,  \
    _conditionPtr/*_extraInfoPtr*/)

/* the macro creates conversion entry for integer member of struct or union.
   integer member may be type of GT_U8,GT_U16,GT_U32,GT_8,GT_16,GT_32 or enum.
   the conversion is done unconditionaly if _conditionDep == 0  and
   if condition in context is TRUE if _conditionDep == 1.
   the stc to raw conversion check field by size of raw field.
   no value conversion is performed.
   Cannot be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _conditionDep          1 - depended from context condition, 0 - independent

*/
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONTEXT_COND_MAC(               \
    _structType, _member, _rawBitOffset, _rawBitLength, _conditionDep)           \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_GEN_MAC(                            \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, 0/*_stcBitOffset*/,                            \
    _conditionDep,                                                               \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                    \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_BITS_E,                                      \
    0/*_ptrUse*/, 0/*_ptrSave*/, CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,   \
    NULL /*_extraInfoPtr*/)

/* the macro creates conversion entry for member of struct or union with specific type.
   the type may be one of:
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_1BYTE_E
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_2BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_4BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_BOOL_E
    CPSS_FORMAT_CONVERT_FIELD_INVERTED_BOOL_E
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_1BYTE_E
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_2BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_4BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E

   the conversion is done only if condition in _conditionPtr is TRUE.
   the stc to raw conversion does not check field.
   no value conversion is performed.
   cannot be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _type                - size of integer member or type for other kind of member.
                        see CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _conditionPtr         - pointer to field's condition structure

 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_COND_MAC(                          \
    _structType, _member, _type, _rawBitOffset, _rawBitLength, _conditionPtr)   \
    CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCT_MEMBER_GEN_MAC(                       \
    _structType, _member, _type,                                                \
    _rawBitOffset, _rawBitLength, 0/*_stcBitOffset*/,                           \
    1/*_conditionDep*/,                                                         \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                   \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                     \
    1/*_ptrUse*/, 0/*_ptrSave*/, CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONDITION_E, \
    _conditionPtr/*_extraInfoPtr*/)

/* the macro creates conversion entry for member of struct or union with specific type.
   the type may be one of:
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_1BYTE_E
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_2BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_SIZEOF_4BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_BOOL_E
    CPSS_FORMAT_CONVERT_FIELD_INVERTED_BOOL_E
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_1BYTE_E
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_2BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_EXT_MEM_SIZEOF_4BYTES_E
    CPSS_FORMAT_CONVERT_FIELD_BYTE_ARRAY_E

   the conversion is done only if condition in context is TRUE.
   the stc to raw conversion does not check field.
   no value conversion is performed.
   cannot be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _type                - size of integer member or type for other kind of member.
                        see CPSS_FORMAT_CONVERT_FIELD_TYPE_ENT

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 */
#define CPSS_FORMAT_CONVERT_FIELD_INFO_TYPED_CONTEXT_COND_MAC(                  \
    _structType, _member, _type, _rawBitOffset, _rawBitLength)                  \
    CPSS_FORMAT_CONVERT_FIELD_INFO_STRUCT_MEMBER_GEN_MAC(                       \
    _structType, _member, _type,                                                \
    _rawBitOffset, _rawBitLength, 0/*_stcBitOffset*/,                           \
    1/*_conditionDep*/,                                                         \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_NONE_E,                                   \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                     \
    0/*_ptrUse*/, 0/*_ptrSave*/, CPSS_FORMAT_CONVERT_FIELD_CONTEXT_MAXVALUE_E,  \
    NULL/*_extraInfoPtr*/)

/* the macro creates conversion entry for integer member of struct or union.
   integer member may be type of GT_U8,GT_U16,GT_U32,GT_8,GT_16,GT_32 or enum.
   the conversion is done unconditionaly if _conditionDep == 0  and
   if condition in context is TRUE if _conditionDep == 1.
   the stc to raw conversion check field by size of raw field.
   value conversion is performed by table that defined by _convTabPtr
   Cannot be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _conditionDep          1 - depended from context condition, 0 - independent

 _convTabPtr           - pointer to field's value conversion table

*/
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_MAC(                       \
    _structType, _member, _rawBitOffset, _rawBitLength,                          \
    _conditionDep, _convTabPtr)                                                  \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_GEN_MAC(                            \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, 0/*_stcBitOffset*/, _conditionDep,             \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_BY_TABLE_E,                                \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    1/*_ptrUse*/, 0/*_ptrSave*/,                                                 \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_TAB_E,                             \
    _convTabPtr/*_extraInfoPtr*/)

/* the macro creates conversion entry for integer member of struct or union.
   integer member may be type of GT_U8,GT_U16,GT_U32,GT_8,GT_16,GT_32 or enum.
   the conversion is done unconditionaly if _conditionDep == 0  and
   if condition in context is TRUE if _conditionDep == 1.
   the stc to raw conversion check field by size of raw field.
   value conversion is performed by function that defined by _convFunPtr
   Cannot be used for splitted fields.

   All parameters are INPUT
 _structType          - name of struct or union

 _member              - name of member of struct or union

 _rawBitOffset        - offset in bits for a field in raw data.
                         e.g. offset 48 is in the second word bit 16.
                              offset 87 is in the third word bit 23.
                         when field type is
                         CPSS_FORMAT_CONVERT_FIELD_STC_PTR_E
                         used as offset in raw data array pointer

 _rawBitLength        - length in bits for a field in raw data

 _conditionDep          1 - depended from context condition, 0 - independent

 _convFunPtr           - pointer to field's value conversion function

*/
#define CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_CONV_FUNC_MAC(                  \
    _structType, _member, _rawBitOffset, _rawBitLength,                          \
    _conditionDep, _convFunPtr)                                                  \
    CPSS_FORMAT_CONVERT_FIELD_INFO_SIZED_INT_GEN_MAC(                            \
    _structType, _member,                                                        \
    _rawBitOffset, _rawBitLength, 0/*_stcBitOffset*/, _conditionDep,             \
    CPSS_FORMAT_CONVERT_FIELD_CONVERT_BY_FUNCTION_E,                             \
    CPSS_FORMAT_CONVERT_FIELD_CHECK_NONE_E,                                      \
    1/*_ptrUse*/, 0/*_ptrSave*/,                                                 \
    CPSS_FORMAT_CONVERT_FIELD_CONTEXT_CONVERT_FUNC_E,                            \
    _convFunPtr/*_extraInfoPtr*/)

/* macro creates condition entry based on struct data integer/enum member.
  see format CPSS_FORMAT_CONVERT_FIELD_CONDITION_STC
   All parameters are INPUT
 _structType           - name of the structure

 _member               - name of the member in the structure

 _mask                 - GT_U32 mask value

 _pattern              - GT_U32 patter value

  */
#define CPSS_FORMAT_CONVERT_COND_STC_MEMBER_MAC(_struct, _member, _mask, _pattern)  \
    {CPSS_FORMAT_CONVERT_STRUCT_MEMBER_SIZE_MAC(_struct, _member),  \
     (GT_U16)CPSS_FORMAT_CONVERT_STRUCT_MEMBER_OFFSET_MAC(_struct, _member), \
    _mask, _pattern}


/*******************************************************************************
* cpssFormatConvertStcToRaw
*
* DESCRIPTION:
*   The function converts data from SW structure or union to
*   raw data format
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       fieldsInfoArrPtr   - field descriptors array
*       stcDataPtr         - this is pointer to first byte of SW structure
*       extMemoPtr         - memory to store:
*                    condition variables for fields with condition checks
*                    field's HW values for fields that cannot be
*                    converted by simple SW to HW conversion
*
* OUTPUTS:
*       rawDataPtr         - raw formatted Data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong data
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssFormatConvertStcToRaw
(
    IN  CPSS_FORMAT_CONVERT_FIELD_INFO_STC *fieldsInfoArrPtr,
    IN  GT_VOID                            *stcDataPtr,
    IN  GT_VOID                            *extMemoPtr,
    OUT GT_U32                             *rawDataPtr
);

/*******************************************************************************
* cpssFormatConvertRawToStc
*
* DESCRIPTION:
*   The function converts data from raw formatted memory entry format to
*   SW structure or union.
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       fieldsInfoArrPtr   - field descriptors array
*       rawDataPtr         - raw formatted Data
*       extMemoPtr         - memory to store:
*              condition variables for fields with condition checks
*
* OUTPUTS:
*       stcDataPtr         - software formatted Data
*       extMemoPtr         - memory to get:
*              field's HW values for fields that cannot be converted by
*              simple HW to SW conversion
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong data
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
GT_STATUS cpssFormatConvertRawToStc
(
    IN    CPSS_FORMAT_CONVERT_FIELD_INFO_STC *fieldsInfoArrPtr,
    IN    GT_U32                             *rawDataPtr,
    OUT   GT_VOID                            *stcDataPtr,
    INOUT GT_VOID                            *extMemoPtr
) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __cpssFormatConverth */


