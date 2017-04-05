/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssMisc.c
*
* DESCRIPTION:
*       Miscellaneous operations for CPSS.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>

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
)
{
    /* the field bits that can be spread on 2 registers max */
    GT_U32  firstWordIndex;/* temporary index of the word to update in dataArray[] */
    GT_U32  offset;/* start bit index , in the first updated word */
    GT_U32  indexOfStartingBit; /* index of bit to start with , indexed from the
                                   start of the array dataArray[]*/
    GT_U32  freeBits;/*number of bits in the first updated word from the offset
                       to end of word --> meaning (32-offset)
                       this value allow as to understand if we update single
                       word or 2  words.
                       */

    CPSS_NULL_PTR_CHECK_MAC(dataArray);

    if(fieldNumBits > 32  || fieldNumBits == 0 ||
       entryNumBits == 0 ||
       ((fieldNumBits + fieldStartBit) > entryNumBits))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    indexOfStartingBit = entryNumBits * entryIndex + fieldStartBit;

    firstWordIndex =  indexOfStartingBit >> 5 /* /32 */;

    offset = indexOfStartingBit & 0x1f /* % 32 */;
    freeBits = 32 - offset;
    if(freeBits >= fieldNumBits)
    {
        /* set the value in the field -- all in single word */
        U32_SET_FIELD_MASKED_MAC(dataArray[firstWordIndex],offset,fieldNumBits,fieldValue);
    }
    else
    {
        /* set the start of value in the first word */
        U32_SET_FIELD_MASKED_MAC(dataArray[firstWordIndex],offset,freeBits,fieldValue);
        /* set the rest of the value in the second word */
        U32_SET_FIELD_MASKED_MAC(dataArray[firstWordIndex + 1],0,fieldNumBits - freeBits,(fieldValue>>freeBits));
    }

    return GT_OK;
}

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
)
{
    /* the field bits that can be spread on 2 registers max */
    GT_U32  firstWordIndex;/* temporary index of the word to read from dataArray[] */
    GT_U32  offset;/* start bit index , in the first read word */
    GT_U32  indexOfStartingBit; /* index of bit to start with , indexed from the
                                   start of the array dataArray[]*/
    GT_U32  freeBits;/*number of bits in the first read word from the offset
                       to end of word --> meaning (32-offset)
                       this value allow as to understand if we read single
                       word or 2  words.
                       */

    CPSS_NULL_PTR_CHECK_MAC(dataArray);
    CPSS_NULL_PTR_CHECK_MAC(fieldValuePtr);

    if(fieldNumBits > 32  || fieldNumBits == 0 ||
       entryNumBits == 0 ||
       ((fieldNumBits + fieldStartBit)  > entryNumBits))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    indexOfStartingBit = entryNumBits * entryIndex + fieldStartBit;

    firstWordIndex =  indexOfStartingBit >> 5 /* /32 */;

    offset = indexOfStartingBit & 0x1f /* % 32 */;
    freeBits = 32 - offset;
    if(freeBits >= fieldNumBits)
    {
        /* get the value of the field -- all in single word */
        *fieldValuePtr = U32_GET_FIELD_MAC(dataArray[firstWordIndex],offset,fieldNumBits);
    }
    else
    {
        /* get the start of value from the first word */
        *fieldValuePtr = U32_GET_FIELD_MAC(dataArray[firstWordIndex],offset,freeBits);
        /* get the rest of the value from the second word */
        *fieldValuePtr |=
            ((U32_GET_FIELD_MAC(dataArray[firstWordIndex + 1],0,(fieldNumBits - freeBits))) << freeBits);
    }

    return GT_OK;
}


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
)
{
    GT_U32  value;    /* value of field */
    GT_U32  wordIndex = OFFSET_TO_WORD_MAC(startBit);/* word index in startMemPtr[]*/
    GT_U32  bitIndex  = OFFSET_TO_BIT_MAC(startBit); /* bit index in startMemPtr[wordIndex]*/
    GT_U32  len0;       /* length of field in first startMemPtr[wordIndex] */
    GT_U32  tmpValue;   /* temo value to help calculation */

    CPSS_NULL_PTR_CHECK_MAC(startMemPtr);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    if(numBits > 32 || numBits == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(wordIndex == OFFSET_TO_WORD_MAC(startBit + numBits - 1))
    {
        /* read in the same word */
        if(numBits == 32)
        {
            value = startMemPtr[wordIndex];
        }
        else
        {
            value = U32_GET_FIELD_MAC(startMemPtr[wordIndex],bitIndex,(numBits));
        }
    }
    else
    {
        len0 = (32 - bitIndex);
        /* read from 2 words */
        /* in first word , start from bitIndex , and read to end of word */
        value = U32_GET_FIELD_MAC(startMemPtr[wordIndex],bitIndex,len0);

        /* in second word , start from bit 0 , and read the rest of data */
        tmpValue = U32_GET_FIELD_MAC(startMemPtr[wordIndex + 1],0,(numBits - len0));
        value |= tmpValue << len0;
    }

    *valuePtr =  value;

    return GT_OK;
}

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
)
{
    GT_U32  wordIndex = OFFSET_TO_WORD_MAC(startBit);/* word index in startMemPtr[]*/
    GT_U32  bitIndex  = OFFSET_TO_BIT_MAC(startBit); /* bit index in startMemPtr[wordIndex]*/
    GT_U32  len0;       /* length of field in first startMemPtr[wordIndex] */

    CPSS_NULL_PTR_CHECK_MAC(startMemPtr);

    if(numBits > 32 || numBits == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(wordIndex == OFFSET_TO_WORD_MAC(startBit + numBits - 1))
    {
        /* write in the same word */
        U32_SET_FIELD_MASKED_MAC(startMemPtr[wordIndex],bitIndex,(numBits),(value));
    }
    else
    {
        len0 = (32 - bitIndex);
        /* write in 2 words */
        /* in first word , start from bitIndex , and write to end of word */
        U32_SET_FIELD_MASKED_MAC(startMemPtr[wordIndex],bitIndex,len0,value);

        /* in second word , start from bit 0 , and write the rest of data */
        U32_SET_FIELD_MASKED_MAC(startMemPtr[wordIndex + 1],0,(numBits - len0),(value >> len0));
    }

    return GT_OK;
}

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
)
{
    CPSS_NULL_PTR_CHECK_MAC(fieldsInfoArr);
    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    if(fieldsInfoArr[fieldIndex].startBit == PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    return prvCpssFieldValueGet(entryPtr,fieldsInfoArr[fieldIndex].startBit,fieldsInfoArr[fieldIndex].numOfBits,valuePtr);
}

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
)
{
    CPSS_NULL_PTR_CHECK_MAC(fieldsInfoArr);

    if(fieldsInfoArr[fieldIndex].startBit == PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if(fieldsInfoArr[fieldIndex].numOfBits < 32 &&
        (value > BIT_MASK_MAC(fieldsInfoArr[fieldIndex].numOfBits)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return prvCpssFieldValueSet(entryPtr,fieldsInfoArr[fieldIndex].startBit,fieldsInfoArr[fieldIndex].numOfBits,value);
}

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
)
{
    GT_STATUS   rc;
    GT_U32  ii;
    GT_U32  numOfWords;/* number of words in the field */
    GT_U32  startBit;/* start bit of current section of the field */
    GT_U32  numOfBits;/* number of bits of current section of the field */
    GT_U32  value;/* value of current section of the field */

    CPSS_NULL_PTR_CHECK_MAC(fieldsInfoArr);
    CPSS_NULL_PTR_CHECK_MAC(valueArr);

    if(fieldsInfoArr[fieldIndex].startBit == PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if(fieldsInfoArr[fieldIndex].numOfBits == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    numOfWords = (fieldsInfoArr[fieldIndex].numOfBits + 31) / 32;
    startBit = fieldsInfoArr[fieldIndex].startBit;
    numOfBits = 32;

    /* handle the 'Full words' (32 bits) sections */
    for(ii = 0 ; ii < (numOfWords - 1); ii++ , startBit += 32)
    {
        value = valueArr[ii];
        rc = prvCpssFieldValueSet(entryPtr,startBit,numOfBits,value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    value = valueArr[ii];

    /* handle the last word */
    numOfBits = fieldsInfoArr[fieldIndex].numOfBits & 0x1f;
    if(numOfBits == 0)
    {
        /* last word is full 32 bits */
        numOfBits = 32;
    }

    return prvCpssFieldValueSet(entryPtr,startBit,numOfBits,value);
}

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
)
{
    GT_STATUS   rc;
    GT_U32  ii;
    GT_U32  numOfWords;/* number of words in the field */
    GT_U32  startBit;/* start bit of current section of the field */
    GT_U32  numOfBits;/* number of bits of current section of the field */
    GT_U32  value;/* value of current section of the field */

    CPSS_NULL_PTR_CHECK_MAC(fieldsInfoArr);
    CPSS_NULL_PTR_CHECK_MAC(valueArr);

    if(fieldsInfoArr[fieldIndex].startBit == PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if(fieldsInfoArr[fieldIndex].numOfBits == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    numOfWords = (fieldsInfoArr[fieldIndex].numOfBits + 31) / 32;
    startBit = fieldsInfoArr[fieldIndex].startBit;
    numOfBits = 32;

    /* handle the 'Full words' (32 bits) sections */
    for(ii = 0 ; ii < (numOfWords - 1); ii++ , startBit += 32)
    {
        rc = prvCpssFieldValueGet(entryPtr,startBit,numOfBits,&value);
        if(rc != GT_OK)
        {
            return rc;
        }

        valueArr[ii] = value;
    }

    /* handle the last word */
    numOfBits = fieldsInfoArr[fieldIndex].numOfBits & 0x1f;
    if(numOfBits == 0)
    {
        /* last word is full 32 bits */
        numOfBits = 32;
    }

    rc = prvCpssFieldValueGet(entryPtr,startBit,numOfBits,&value);

    valueArr[ii] = value;

    return rc;
}



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
)
{
    GT_U32                      ii;
    PRV_CPSS_ENTRY_FORMAT_TABLE_STC *currentFieldInfoPtr;
    PRV_CPSS_ENTRY_FORMAT_TABLE_STC *prevFieldInfoPtr;
    GT_U32                      prevIndex;

    CPSS_NULL_PTR_CHECK_MAC(fieldsInfoArr);

    for( ii = 0 ; ii < numOfFields ; ii++)
    {
        currentFieldInfoPtr = &fieldsInfoArr[ii];

        prevIndex = currentFieldInfoPtr->previousFieldType;

        if(currentFieldInfoPtr->startBit == PRV_CPSS_FIELD_SET_IN_RUNTIME_CNS)
        {
            if(ii == 0)
            {
                /* first field got no options other then to start in bit 0 */
                currentFieldInfoPtr->startBit = 0;
            }
            else /* use the previous field info */
            {
                if(prevIndex == PRV_CPSS_FIELD_CONSECUTIVE_CNS)
                {
                    /* this field is consecutive to the previous field */
                    prevIndex = ii-1;
                }
                else
                {
                    /* this field come after other previous field */
                }

                prevFieldInfoPtr = &fieldsInfoArr[prevIndex];
                currentFieldInfoPtr->startBit = prevFieldInfoPtr->startBit + prevFieldInfoPtr->numOfBits;
            }
        }
        else
        {
            /* no need to calculate the start bit -- it is FORCED by the entry format */
        }
    }


    return GT_OK;
}

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
)
{
    CPSS_NULL_PTR_CHECK_MAC(fieldsInfoArr);
    CPSS_NULL_PTR_CHECK_MAC(startBitPtr);
    CPSS_NULL_PTR_CHECK_MAC(numOfBitsPtr);

    *startBitPtr = fieldsInfoArr[fieldIndex].startBit;
    *numOfBitsPtr = fieldsInfoArr[fieldIndex].numOfBits;

    return GT_OK;
}


