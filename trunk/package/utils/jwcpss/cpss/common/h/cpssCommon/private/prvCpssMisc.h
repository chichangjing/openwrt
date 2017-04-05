/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssMisc.h
*
* DESCRIPTION:
*       Miscellaneous operations for CPSS.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#ifndef __prvCpssMisch
#define __prvCpssMisch

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* indication to set PRV_CPSS_ENTRY_FORMAT_TABLE_STC::startBit during run time */
#define PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS   0xFFFFFFFF
/* indication in PRV_CPSS_ENTRY_FORMAT_TABLE_STC::previousFieldType that 'current'
field is consecutive to the previous field */
#define PRV_CPSS_FIELD_CONSECUTIVE_CNS      0xFFFFFFFF

/*
 * Typedef: struct PRV_CPSS_ENTRY_FORMAT_TABLE_STC
 *
 * Description: A structure to hold info about field in entry of table
 *
 * Fields:
 *      startBit  - start bit of the field. filled in runtime when value != PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS
 *      numOfBits - number of bits in the field
 *      previousFieldType - 'point' to the previous field for calculation of startBit.
 *                          used when != PRV_CPSS_FIELD_CONSECUTIVE_CNS
 *
 */
typedef struct{
    GT_U32      startBit;
    GT_U32      numOfBits;
    GT_U32      previousFieldType;
}PRV_CPSS_ENTRY_FORMAT_TABLE_STC;

/* macro to fill instance of PRV_CPSS_ENTRY_FORMAT_TABLE_STC with value that good for 'standard' field.
'standard' field is a field that comes after the field that is defined before it in the array of fields

the macro gets the <numOfBits> of the current field.
*/
#define PRV_CPSS_STANDARD_FIELD_MAC(numOfBits)     \
        {PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS,        \
         numOfBits,                       \
         PRV_CPSS_FIELD_CONSECUTIVE_CNS}


/* macro to set <startBit> and <numOfBits> in instance of PRV_CPSS_ENTRY_FORMAT_TABLE_STC
the macro gets the <numOfBits> of the current field.
*/
#define PRV_CPSS_EXPLICIT_FIELD_MAC(startBit,numOfBits)     \
        {startBit,        \
         numOfBits,                       \
         0/*don't care*/}


/*******************************************************************************
* prvCpssPeriodicFieldValueSet
*
* DESCRIPTION:
*       Set a value of a field into entry that lay in array of words.
*       the entry length is in bits and can be any non-zero number
*
* INPUTS:
*       dataArray - array of words to set the field value into.
*       entryIndex - entry index inside dataArray[]
*       entryNumBits - number of bits for each entry.(non-zero)
*       fieldStartBit - start bit of field . this bit is counted from the start
*                   of the entry(and not from entry[0]).
*       fieldNumBits - number of bits to set (field length) (1..32)
*       fieldValue - the value of the field that need to set.
*
* OUTPUTS:
*       dataArray - array of words with updated data on the field.
*
* RETURNS :
*       GT_OK           - on success
*       GT_BAD_PARAM    - bad parameter entryNumBits or fieldNumBits
*                         or fieldNumBits > entryNumBits
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPeriodicFieldValueSet(
    INOUT GT_U32    dataArray[],
    IN GT_U32       entryIndex,
    IN GT_U32       entryNumBits,
    IN GT_U32       fieldStartBit,
    IN GT_U32       fieldNumBits,
    IN GT_U32       fieldValue
);

/*******************************************************************************
* prvCpssPeriodicFieldValueGet
*
* DESCRIPTION:
*       Get a value of a field from entry that lay in array of words.
*       the entry length is in bits and can be any non-zero number
*
* INPUTS:
*       dataArray - array of words to Get the field value from.
*       entryIndex - entry index inside dataArray[]
*       entryNumBits - number of bits for each entry.(non-zero)
*       fieldStartBit - start bit of field . this bit is counted from the start
*                   of the entry(and not from entry[0]).
*       fieldNumBits - number of bits to get (field length) (1..32)
*
* OUTPUTS:
*       fieldValue - (pointer to)the value of the field that need to set.
*
* RETURNS :
*       GT_OK           - on success
*       GT_BAD_PARAM    - bad parameter entryNumBits or fieldNumBits
*                         or fieldNumBits > entryNumBits
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPeriodicFieldValueGet(
    IN GT_U32       dataArray[],
    IN GT_U32       entryIndex,
    IN GT_U32       entryNumBits,
    IN GT_U32       fieldStartBit,
    IN GT_U32       fieldNumBits,
    OUT GT_U32      *fieldValuePtr
);

/*******************************************************************************
*   prvCpssFieldValueGet
*
* DESCRIPTION:
*        get the value of field (up to 32 bits) that located in any start bit in
*       memory
* INPUTS:
*        startMemPtr - pointer to memory
*        startBit  - start bit of field (0..)
*        numBits   - number of bits of field (1..32)
* OUTPUTS:
*        valuePtr - (pointer to) value get
* Returns:
*
* COMMENTS:
*       GT_OK - no error
*       GT_BAD_PTR - on NULL pointer
*       GT_BAD_PARAM - on bad parameter : numBits > 32 or numBits == 0
*
*******************************************************************************/
GT_STATUS  prvCpssFieldValueGet
(
    IN GT_U32                  *startMemPtr,
    IN GT_U32                  startBit,
    IN GT_U32                  numBits,
    OUT GT_U32                 *valuePtr
);

/*******************************************************************************
*   prvCpssFieldValueSet
*
* DESCRIPTION:
*       set the value of field (up to 32 bits) that located in any start bit in
*       memory
* INPUTS:
*        startMemPtr - pointer to memory
*        startBit  - start bit of field (0..)
*        numBits   - number of bits of field (1..32)
*        value - the value to set
* Returns:
*
* COMMENTS:
*       GT_OK - no error
*       GT_BAD_PTR - on NULL pointer
*       GT_BAD_PARAM - on bad parameter : numBits > 32 or numBits == 0
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS  prvCpssFieldValueSet
(
    IN GT_U32                  *startMemPtr,
    IN GT_U32                  startBit,
    IN GT_U32                  numBits,
    IN GT_U32                  value
);

/*******************************************************************************
* prvCpssFieldFromEntry_GT_U32_Get
*
* DESCRIPTION:
*        Get GT_U32 value of a field from the table entry.
*
* INPUTS:
*       entryPtr        - pointer to memory.
*       fieldsInfoArr[] - array of fields info
*       fieldIndex      - the index of the field (used as index in fieldsInfoArr[])
*
* OUTPUTS:
*        valuePtr - (pointer to) value get
*
* RETURN:
*       GT_OK   -   on success
*       GT_BAD_PTR  - on NULL pointer
*       GT_BAD_PARAM - on bad parameter : fieldInfo.numBits > 32 or fieldInfo.numBits == 0
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFieldFromEntry_GT_U32_Get(
    IN GT_U32                           *entryPtr,
    IN PRV_CPSS_ENTRY_FORMAT_TABLE_STC  fieldsInfoArr[],
    IN GT_U32                           fieldIndex,
    OUT GT_U32                          *valuePtr
);

/*******************************************************************************
* prvCpssFieldToEntry_GT_U32_Set
*
* DESCRIPTION:
*        Set GT_U32 value to a field in a table entry.
*
*        NOTE:the value MUST not be bigger then the max value for the field !
*
* INPUTS:
*       entryPtr        - pointer to memory.
*       fieldsInfoArr[] - array of fields info
*       fieldIndex      - the index of the field (used as index in fieldsInfoArr[])
*       value           - the value to set
*
* OUTPUTS:
*        None.
*
* RETURN:
*       GT_OK   -   on success
*       GT_BAD_PTR  - on NULL pointer
*       GT_BAD_PARAM    - on bad parameter : info.length > 32 or info.length == 0
*                           or  val > maxValue
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssFieldToEntry_GT_U32_Set(
    IN GT_U32                           *entryPtr,
    IN PRV_CPSS_ENTRY_FORMAT_TABLE_STC  fieldsInfoArr[],
    IN GT_U32                           fieldIndex,
    IN GT_U32                           value
);

/*******************************************************************************
* prvCpssFieldToEntry_GT_Any_Set
*
* DESCRIPTION:
*        Set 'any number of bits' value to a field in a table entry.
*        the bits are in little Endian order in the array of words.
*
*        NOTE:the value MUST not be bigger then the max value for the field !
*
* INPUTS:
*       entryPtr        - pointer to memory.
*       fieldsInfoArr[] - array of fields info
*       fieldIndex      - the index of the field (used as index in fieldsInfoArr[])
*       valueArr[]      - the array of values to set
*
* OUTPUTS:
*        None.
*
* RETURN:
*       GT_OK   -   on success
*       GT_BAD_PTR  - on NULL pointer
*       GT_BAD_PARAM    - on bad parameter : info.length == 0 or  valueArr[...] > maxValue
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssFieldToEntry_GT_Any_Set(
    IN GT_U32                           *entryPtr,
    IN PRV_CPSS_ENTRY_FORMAT_TABLE_STC  fieldsInfoArr[],
    IN GT_U32                           fieldIndex,
    IN GT_U32                           valueArr[]
);

/*******************************************************************************
* prvCpssFieldToEntry_GT_Any_Get
*
* DESCRIPTION:
*        Get 'any number of bits' value to a field in a table entry.
*        the bits returned are in little Endian order in the array of words.
*
* INPUTS:
*       entryPtr        - pointer to memory.
*       fieldsInfoArr[] - array of fields info
*       fieldIndex      - the index of the field (used as index in fieldsInfoArr[])
*
* OUTPUTS:
*       valueArr[]      - the array of values to get
*
* RETURN:
*       GT_OK   -   on success
*       GT_BAD_PTR  - on NULL pointer
*       GT_BAD_PARAM    - on bad parameter : info.length == 0
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssFieldToEntry_GT_Any_Get(
    IN GT_U32                           *entryPtr,
    IN PRV_CPSS_ENTRY_FORMAT_TABLE_STC  fieldsInfoArr[],
    IN GT_U32                           fieldIndex,
    OUT GT_U32                          valueArr[]
);

/*******************************************************************************
* prvCpssFillFieldsStartBitInfo
*
* DESCRIPTION:
*        Fill during init the 'start bit' of the fields in the table format.
*
* INPUTS:
*       numOfFields     - the number of elements in in fieldsInfoArr[].
*       fieldsInfoArr[] - array of fields info
*
* OUTPUTS:
*       fieldsInfoArr[] - array of fields info , after modify the <startBit> of the fields.
*
* RETURN:
*       GT_OK   -   on success
*       GT_BAD_PTR  - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssFillFieldsStartBitInfo(
    IN GT_U32                           numOfFields,
    INOUT PRV_CPSS_ENTRY_FORMAT_TABLE_STC   fieldsInfoArr[]
);


/*******************************************************************************
* prvCpssFieldInEntryInfoGet
*
* DESCRIPTION:
*        Get the start bit and the length of specific field in entry format .
*
*
* INPUTS:
*       fieldsInfoArr[] - array of fields info
*       fieldIndex      - the index of the field (used as index in fieldsInfoArr[])
*
* OUTPUTS:
*       startBitPtr     - (pointer to) the start bit of the field.
*       numOfBitsPtr    - (pointer to) the number of bits of the field.
*
* RETURN:
*       GT_OK   -   on success
*       GT_BAD_PTR  - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssFieldInEntryInfoGet(
    IN PRV_CPSS_ENTRY_FORMAT_TABLE_STC  fieldsInfoArr[],
    IN GT_U32                           fieldIndex,
    OUT GT_U32                          *startBitPtr,
    OUT GT_U32                          *numOfBitsPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssMisch */

