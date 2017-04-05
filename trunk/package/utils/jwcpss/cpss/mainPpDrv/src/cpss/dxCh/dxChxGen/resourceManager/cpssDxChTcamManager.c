/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTcamManager.c
*
* DESCRIPTION:
*       This file contains TCAM manager module.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 26 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/resourceManager/private/prvCpssDxChResourceManagerLog.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/systemRecovery/hsu/private/prvCpssGenHsu.h>

/* Validity code that is kept within TCAM manager object to verify
   that TCAM manager handler is valid */
#define PRV_TCAM_MANAGER_VALIDITY_CODE_CNS          0x18273645

/* Number of TCAM entries types (according to CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT) */
#define PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS     4

/* Indicated last free token */
#define PRV_TCAM_MANAGER_NO_MORE_TOKENS_CNS         2000000000

/* Indicate that token is part of free token list */
#define PRV_TCAM_MANAGER_IN_FREE_TOKEN_LIST_CNS     1000000000

/* number of HSU stages */
#define PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS   5

/* Check the validity code of TCAM manager object */
#define PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(_tcamManagerPtr)                    \
    if ((_tcamManagerPtr)->validityValue != PRV_TCAM_MANAGER_VALIDITY_CODE_CNS) \
    {                                                                           \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                    \
    }

/* Check the range of client ID */
#define PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(_clientId)                 \
    if ((_clientId) >= CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)          \
    {                                                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

/* Check the client is registered */
#define PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(_clientPtr)        \
    if ((_clientPtr)->isRegistered == GT_FALSE)                         \
    {                                                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_SUCH, LOG_ERROR_NO_MSG);                                              \
    }

/* Check TCAM type */
#define PRV_TCAM_MANAGER_TCAM_TYPE_CHECK_MAC(_tcamType)                 \
    switch ((_tcamType))                                                \
    {                                                                   \
    case CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_ROUTER_TCAM_E:           \
        break;                                                          \
    case CPSS_DXCH_TCAM_MANAGER_XCAT_AND_ABOVE_PCL_TCAM_E:              \
        break;                                                          \
    default:                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

/* Check TCAM range */
#define PRV_TCAM_MANAGER_TCAM_RANGE_CHECK_MAC(_rangePtr)                \
    if (((_rangePtr)->firstLine > (_rangePtr)->lastLine) ||             \
        ((_rangePtr)->firstLine >= (GT_U32)(BIT_18)) ||                 \
        ((_rangePtr)->lastLine >= (GT_U32)(BIT_18)))                    \
    {                                                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

/* Check reservation type */
#define PRV_TCAM_MANAGER_RESERVATION_TYPE_CHECK_MAC(_reservationType)   \
    switch ((_reservationType))                                         \
    {                                                                   \
    case CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E:           \
        break;                                                          \
    case CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E:          \
        break;                                                          \
    default:                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

/* Check entry type */
#define PRV_TCAM_MANAGER_ENTRY_TYPE_CHECK_MAC(_entryType)               \
    switch ((_entryType))                                               \
    {                                                                   \
    case CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E:                         \
        break;                                                          \
    case CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E:                           \
        break;                                                          \
    case CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E:                         \
        break;                                                          \
    case CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E:                           \
        break;                                                          \
    default:                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

/* Check allocation method */
#define PRV_TCAM_MANAGER_ALLOC_METHOD_CHECK_MAC(_allocMethod)           \
    switch ((_allocMethod))                                             \
    {                                                                   \
    case CPSS_DXCH_TCAM_MANAGER_ALLOC_MIN_E:                            \
        break;                                                          \
    case CPSS_DXCH_TCAM_MANAGER_ALLOC_MID_E:                            \
        break;                                                          \
    case CPSS_DXCH_TCAM_MANAGER_ALLOC_MAX_E:                            \
        break;                                                          \
    default:                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                            \
    }

/* check dump type */
#define PRV_TCAM_MANAGER_DUMP_TYPE_CHECK_MAC(_dumpType)                     \
    switch ((_dumpType))                                                    \
    {                                                                       \
    case CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_E:                             \
        break;                                                              \
    case CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E:         \
        break;                                                              \
    case CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_ALL_CLIENTS_E:             \
        break;                                                              \
    case CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_AND_ENTRIES_FOR_ALL_CLIENTS_E: \
        break;                                                              \
    case CPSS_DXCH_TCAM_MANAGER_DUMP_INTERNAL_INFO_E:                       \
        break;                                                              \
    default:                                                                \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                \
    }

/* check range update method */
#define PRV_TCAM_MANAGER_RANGE_UPDATE_METHOD_CHECK_MAC(_rangeUpdateMethod)          \
    switch ((_rangeUpdateMethod))                                                   \
    {                                                                               \
    case CPSS_DXCH_TCAM_MANAGER_DO_NOT_MOVE_RANGE_UPDATE_METHOD_E:                  \
        break;                                                                      \
    case CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_COMPRESS_RANGE_UPDATE_METHOD_E:        \
        break;                                                                      \
    case CPSS_DXCH_TCAM_MANAGER_MOVE_BOTTOM_AND_COMPRESS_RANGE_UPDATE_METHOD_E:     \
        break;                                                                      \
    case CPSS_DXCH_TCAM_MANAGER_MOVE_MIDDLE_AND_COMPRESS_RANGE_UPDATE_METHOD_E:     \
        break;                                                                      \
    case CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_KEEP_OFFSETS_RANGE_UPDATE_METHOD_E:    \
        break;                                                                      \
    default:                                                                        \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                        \
    }


/* Indicate not used function parameters (and prevent compiler warnings) */
#define NOT_USED_MAC(x)   (x) = (x);

/*
 * typedef: struct PRV_CPSS_DXCH_TCAM_MANAGER_INDEX_RANGE_STC
 *
 * Description:
 *      Holds indexes range.
 *
 * Fields:
 *      firstIndex    - first index
 *      lastIndex     - last index
 */
typedef struct
{
    GT_U32  firstIndex;
    GT_U32  lastIndex;

} PRV_CPSS_DXCH_TCAM_MANAGER_INDEX_RANGE_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC
 *
 * Description:
 *      Holds TCAM manager client information.
 *
 * Fields:
 *
 *      isRegistered          - whether this entry represent registered client
 *      name                  - string to represent the client (only used for dumps)
 *      callbackFunc          - client callback functions
 *
 *      isReservationTypeLocked   - whether the reservation type is locked;
 *                              currently reservation type can't be change at run
 *                              time; on client registration, the reservation type
 *                              is set to dynamic reservation; after first reservation
 *                              set or first entry allocation the reservation type
 *                              is locked and can't be changed
 *      reservationType       - the reservation type
 *      reservationRange      - the reservation range; relevant only when reservation
 *                              type is static reservation
 *      entryPoolRangeArray   - on static reservation, the reservation TCAM range
 *                              is partitioned during the reservation to the
 *                              different entries type; this array holds the
 *                              index ranges allocated for each entry type;
 *                              the array has one element for each entry type that
 *                              is supported by the TCAM manager
 *
 *      reservedEntriesArray  - holds number of reserved entries;
 *                              the array has one element for each entry type that
 *                              is supported by the TCAM manager
 *      allocatedEntriesArray - hold number of entries currently allocated;
 *                              the array has one element for each entry type that
 *                              is supported by the TCAM manager
 *
 */
typedef struct
{
    GT_BOOL                                             isRegistered;

    GT_CHAR                                             name[CPSS_DXCH_TCAM_MANAGER_CLIENT_NAME_NAX_LENGTH_CNS];
    CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC              callbackFunc;

    GT_BOOL                                             isReservationTypeLocked;
    CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT reservationType;
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC                    reservationRange;
    PRV_CPSS_DXCH_TCAM_MANAGER_INDEX_RANGE_STC          entryPoolRangeArray[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];

    GT_U32                                              reservedEntriesArray[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];
    GT_U32                                              allocatedEntriesArray[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];

} PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC
 *
 * Description:
 *      Holds information for each of the entries in the TCAM.
 *
 * Fields:
 *      isUsed            - whether this entry is allocated or not
 *      type              - the entry type
 *      clientId          - the client that allocated this entry
 *      clientCookiePtr   - client cookie; used in client callback functions
 *      token             - token pointing to this entry
 *
 */
typedef struct
{
    GT_BOOL                                     isUsed;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT       type;
    GT_U32                                      clientId;
    GT_VOID                                     *clientCookiePtr;
    GT_U32                                      token;

} PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_TCAM_MANAGER_STC
 *
 * Description:
 *      Holds TCAM manager information.
 *
 * Fields:
 *      tcamType              - the TCAM type
 *      tcamRange             - the TCAM range allocated to the TCAM manager
 *
 *      validityValue         - holds constant value to verify that the TCAM
 *                              manager handler is valid
 *
 *      tcamLines             - number of TCAM lines in the TCAM manager
 *      tcamEntries           - number of TCAM entries in the TCAM manager
 *
 *      clientsArray          - holds information about the TCAM mananger clients
 *
 *      entriesInfoArray      - holds information about each entry within the TCAM
 *                              manager; in case of entry type that requires more
 *                              than single TCAM entry, the entry information will
 *                              be kept in the first entry assiciated with the
 *                              entry type
 *      entriesUsagePtr       - points to bitmap that indicates which entries in
 *                              the TCAM are in use (1 indicates entry is used,
 *                              0 indicates the entry is free); the bitmap is
 *                              according to TCAM search order;
 *                              for example if the second word is 0x00000201 then
 *                              it means that in the range [32-63] the entries
 *                              32 and 41 are used while all other are free.
 *      tokenToIndexArraySize     - size of token to index array
 *      nextFreeToken             - next free token in the token to index array
 *      orderedStaticReservations - static reservations ranges sorted by TCAM order
 *
 */
typedef struct
{
    CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT        tcamType;
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC            tcamRange;

    GT_U32                                      validityValue;

    GT_U32                                      tcamLines;
    GT_U32                                      tcamEntries;

    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       clientsArray[CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS];

    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entriesInfoArray;
    GT_U32                                      *entriesUsagePtr;

    GT_U32                                      *tokenToIndexArray;
    GT_U32                                      tokenToIndexArraySize;
    GT_U32                                      nextFreeToken;

    PRV_CPSS_DXCH_TCAM_MANAGER_INDEX_RANGE_STC  orderedStaticReservations[CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS];

} PRV_CPSS_DXCH_TCAM_MANAGER_STC;

/*******************************************************************************
* entryTypeSizeGet
*
* DESCRIPTION:
*       Returns the number of TCAM entries that entry type takes.
*
* INPUTS:
*       entryType             - the entry type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The number of TCAM entries that the entry type takes.
*
* COMMENTS:
*       The entryType must be valid one.
*
*******************************************************************************/
static GT_U32 entryTypeSizeGet
(
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType
)
{
    if (entryType == CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E)
        return 1;
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E)
        return 2;
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E)
        return 3;
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E)
        return 4;
    else /* should not happen - entryType must be valid */
        return 0;
}

/*******************************************************************************
* entryTypeNameGet
*
* DESCRIPTION:
*       Returns string that represets the entry type name.
*
* INPUTS:
*       entryType             - the entry type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The name of the entry type.
*
* COMMENTS:
*       The entryType must be valid one.
*
*******************************************************************************/
static GT_CHAR *entryTypeNameGet
(
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType
)
{
    static GT_CHAR *entryTypeNames[] = {"SINGLE","DUAL","TRIPLE","QUAD"};

    if (entryType == CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E)
        return entryTypeNames[0];
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E)
        return entryTypeNames[1];
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E)
        return entryTypeNames[2];
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E)
        return entryTypeNames[3];
    else /* should not happen - entryType must be valid */
        return NULL;
}

/*******************************************************************************
* entryTypeToArrayIndex
*
* DESCRIPTION:
*       Converts entry type to array index. Used in ordered arrays, that have
*       specific array index per entry type.
*
* INPUTS:
*       entryType             - the entry type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The index in the array.
*
* COMMENTS:
*       The entryType must be valid one.
*
*******************************************************************************/
static GT_U32 entryTypeToArrayIndex
(
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType
)
{
    if (entryType == CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E)
        return 0;
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E)
        return 1;
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E)
        return 2;
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E)
        return 3;
    else /* should not happen - entryType must be valid */
        return 0;
}

/*******************************************************************************
* arrayIndexToEntryType
*
* DESCRIPTION:
*       Converts array index to entry type. Used in ordered arrays, that have
*       specific array index per entry type.
*
* INPUTS:
*       arrayIndex            - the entry type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The entry type.
*
* COMMENTS:
*       The arrayIndex must be valid.
*
*******************************************************************************/
static CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT arrayIndexToEntryType
(
    IN  GT_U32      arrayIndex
)
{
    if (arrayIndex == 0)
        return CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
    else if (arrayIndex == 1)
        return CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E;
    else if (arrayIndex == 2)
        return CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E;
    else if (arrayIndex == 3)
        return CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E;
    else /* should not happen - arrayIndex must be valid */
        return CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E;
}

/*******************************************************************************
* convertTokenToIndex
*
* DESCRIPTION:
*       Converts token to index of the entry. The index is relative to the first
*       line of the TCAM section managed by the TCAM manager.
*       The TCAM manager maintains array of entries information. The token is
*       implemented as a pointer to the element in the array that is
*       associated with the entry.
*
* INPUTS:
*       tcamManagerPtr        - the TCAM manager object
*       token                 - the token
*
* OUTPUTS:
*       indexPtr              - points to the index associated with the token
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - token is part of tokens free list or is reserved token
*       GT_OUT_OF_RANGE       - token is pointing outside the entries info array
*       GT_FAIL               - on error
*
* COMMENTS:
*       Note that token is array index + 1 as token value 0 represents no boundary
*       in allocation function.
*
*******************************************************************************/
GT_STATUS convertTokenToIndex
(
    IN  PRV_CPSS_DXCH_TCAM_MANAGER_STC  *tcamManagerPtr,
    IN  GT_U32                          token,
    OUT GT_U32                          *indexPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(tcamManagerPtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    /* token 0 is reserved to indicate no boundary during token allocation */
    if (token == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    if (token >= tcamManagerPtr->tokenToIndexArraySize)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    *indexPtr = tcamManagerPtr->tokenToIndexArray[token];

    /* check the token was previously allocated and not in the token free list */
    if (*indexPtr >= PRV_TCAM_MANAGER_IN_FREE_TOKEN_LIST_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* sanity check, should never happen */
    if (*indexPtr >= tcamManagerPtr->tcamEntries)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    return GT_OK;
}

/*******************************************************************************
* convertIndexToToken
*
* DESCRIPTION:
*       Converts index of token. The index is relative to the first
*       line of the TCAM section managed by the TCAM manager.
*       The TCAM manager maintains array of entries information. The token is
*       implemented as a pointer to the element in the array that is
*       associated with the entry.
*
* INPUTS:
*       tcamManagerPtr        - the TCAM manager object
*       index                 - the index
*
* OUTPUTS:
*       tokenPtr              - points to token representing the index
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS convertIndexToToken
(
    IN  PRV_CPSS_DXCH_TCAM_MANAGER_STC  *tcamManagerPtr,
    IN  GT_U32                          index,
    OUT GT_U32                          *tokenPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(tcamManagerPtr);
    CPSS_NULL_PTR_CHECK_MAC(tokenPtr);

    if (index >= tcamManagerPtr->tcamEntries)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    *tokenPtr = (tcamManagerPtr->entriesInfoArray[index].token);

    return GT_OK;
}

/*******************************************************************************
* compareCandidatesForFree
*
* DESCRIPTION:
*       Compares two candidates to free during defragment.
*
* INPUTS:
*       tcamManagerPtr        - the TCAM manager object
*       firstCandidate        - index of first candidate
*       compareToSecond       - GT_TRUE:  compare to second candidate
*                               GT_FALSE: compare to worst case candidate
*       secondCandidate       - index of second candidate, relevant only
*                               when compareToSecond == GT_TRUE
*       entryType             - entry type to free space for
*
* OUTPUTS:
*       resultPtr             - points to the comparison results:
*                               GT_GREATER: first candidate is better
*                               GT_EQUAL:   candidates are equal
*                               GT_SMALLER: first candidate is worse
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS compareCandidatesForFree
(
    IN  PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr,
    IN  GT_U32                                  firstCandidate,
    IN  GT_BOOL                                 compareToSecond,
    IN  GT_U32                                  secondCandidate,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType,
    OUT GT_COMP_RES                             *resultPtr
)
{
    GT_U32      bitsInFirstCandidate   = 0;
    GT_U32      bitsInSecondCandidate  = 0;
    GT_U32      entryTypeBitmap;
    GT_U32      candidateUsage;

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerPtr);
    PRV_TCAM_MANAGER_ENTRY_TYPE_CHECK_MAC(entryType);
    CPSS_NULL_PTR_CHECK_MAC(resultPtr);

    entryTypeBitmap  = 0xF >> (4 - entryTypeSizeGet(entryType));

    candidateUsage = (tcamManagerPtr->entriesUsagePtr[firstCandidate / 32] >> (firstCandidate % 32)) & entryTypeBitmap;
    while (candidateUsage != 0)
    {
        if ((candidateUsage & 0x1) != 0)
            bitsInFirstCandidate++;
        candidateUsage >>= 1;
    }

    if (compareToSecond == GT_TRUE)
    {
        candidateUsage = (tcamManagerPtr->entriesUsagePtr[secondCandidate / 32] >> (secondCandidate % 32)) & entryTypeBitmap;
        while (candidateUsage != 0)
        {
            if ((candidateUsage & 0x1) != 0)
                bitsInSecondCandidate++;
            candidateUsage >>= 1;
        }
    }
    else
    {
        bitsInSecondCandidate = entryTypeSizeGet(entryType);
    }

    if (bitsInFirstCandidate < bitsInSecondCandidate)
        *resultPtr = GT_GREATER;
    else if (bitsInFirstCandidate == bitsInSecondCandidate)
        *resultPtr = GT_EQUAL;
    else
        *resultPtr = GT_SMALLER;

    return GT_OK;
}

/*******************************************************************************
* freeIndex
*
* DESCRIPTION:
*       Free space starting at specific index for an entry type.
*
* INPUTS:
*       tcamManagerPtr        - the TCAM manager object
*       entryType             - entry type to free space for
*       indexToFree           - starting index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS freeIndex
(
    IN  PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType,
    IN  GT_U32                                  indexToFree
)
{
    GT_U32                                      sizeToFree;
    GT_U32                                      entryUsageBackup;
    GT_U32                                      entryTypeBitmap;
    GT_U32                                      bitmapToRestore;
    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entryInfoPtr;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr;
    GT_U32                                      i;
    GT_STATUS                                   rc = GT_OK;
    GT_BOOL                                     isDefaultEntryLocation=GT_FALSE;

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerPtr);

    sizeToFree       = entryTypeSizeGet(entryType);
    entryTypeBitmap  = ((~(0xF << sizeToFree)) & 0xF);

    /* check that none of the indexes is occupied by a default IP - in this case the move is not allowed */
    for (i=0;i<sizeToFree;i++)
    {
        entryInfoPtr = &tcamManagerPtr->entriesInfoArray[indexToFree + i];
        /* if the index is not used, continue to next index */
        if (entryInfoPtr->isUsed == GT_FALSE)
            continue;

        /* call client callback to check if the indexes are default indexes that are located in the default location,
           in this case no move will be done */
        clientPtr = &tcamManagerPtr->clientsArray[entryInfoPtr->clientId];
        if (clientPtr->callbackFunc.checkIfDefaultLocationFuncPtr != NULL)
        {
            rc = clientPtr->callbackFunc.checkIfDefaultLocationFuncPtr(entryInfoPtr->clientCookiePtr,&isDefaultEntryLocation);
            if (rc!=GT_OK)
            {
                return rc;
            }
            if (isDefaultEntryLocation == GT_TRUE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }
    }

    /* backup the current usage of the indexes to be free */
    entryUsageBackup = (tcamManagerPtr->entriesUsagePtr[indexToFree / 32] >> (indexToFree % 32)) & entryTypeBitmap;

    /* mark the indexes to be free as used before start moving the entries that
       reside there so those entries will not be allocated in the same indexes */
    tcamManagerPtr->entriesUsagePtr[indexToFree / 32] |= (entryTypeBitmap << (indexToFree % 32));

    /* for each of the entries that resides in the indexes to free, call client
       callback function to move the entry */
    for (i = 0; i < sizeToFree; i++)
    {
        /* get entry info associated with current index to free */
        entryInfoPtr = &tcamManagerPtr->entriesInfoArray[indexToFree + i];

        /* if the index is not used, continue to next index */
        if (entryInfoPtr->isUsed == GT_FALSE)
            continue;

        /* call client callback to move used indexes */
        clientPtr = &tcamManagerPtr->clientsArray[entryInfoPtr->clientId];
        if (clientPtr->callbackFunc.moveToAnywhereFuncPtr != NULL)
            rc = clientPtr->callbackFunc.moveToAnywhereFuncPtr(entryInfoPtr->clientCookiePtr);
        else
            rc = GT_NOT_READY;

        if(rc == GT_OK)
        {
            /* the client moved the entry and freed the associated token;
               therefore added free indexes in the place need to be freed;
               mark this again as used so this index will not be allocated to
               additional entries that needs to be rellocated */
            tcamManagerPtr->entriesUsagePtr[indexToFree / 32] |= (entryTypeBitmap << (indexToFree % 32));
        }
        else
        {
            /* if the client failed to move the entry, then update the entriesUsagePtr
               to the proper status (update index associated with entries that were
               successfully moved and restore the state of the other indexes) */
            bitmapToRestore = entryTypeBitmap >> i;

            tcamManagerPtr->entriesUsagePtr[(indexToFree + i) / 32] &= (~(bitmapToRestore << ((indexToFree + i) % 32)));
            tcamManagerPtr->entriesUsagePtr[(indexToFree + i) / 32] |= ((entryUsageBackup >> i) << ((indexToFree + i) % 32));

            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* setEntryUsage
*
* DESCRIPTION:
*       Mark entry that resides at specific index as used/unused. If marking
*       the entry as used, also fill in the entry information.
*
* INPUTS:
*       tcamManagerPtr        - the TCAM manager object
*       index                 - index to mark
*       entryType             - entry type to free space for
*       isUsed                - GT_TRUE:  mark as used
*                               GT_FALSE: mark as free
*       clientId              - client that allocated this entry
*       clientCookiePtr       - client cookie; used in client callback functions
*       token                 - token assiciated with this entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on NULL pointer
*       GT_OUT_OF_RANGE       - index is out of the range managed by TCAM manager
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS setEntryUsage
(
    IN  PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr,
    IN  GT_U32                                  index,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType,
    IN  GT_BOOL                                 isUsed,
    IN  GT_U32                                  clientId,
    IN  GT_VOID                                 *clientCookiePtr,
    IN  GT_U32                                  token
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entryInfoPtr;
    GT_U32                                      entryTypeBitmap;

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerPtr);

    /* make sure the index is in range */
    if (index >= tcamManagerPtr->tcamEntries)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);

    /* fill the entry information */
    entryInfoPtr = &tcamManagerPtr->entriesInfoArray[index];
    entryInfoPtr->isUsed = isUsed;
    if (isUsed == GT_TRUE)
    {
        entryInfoPtr->type              = entryType;
        entryInfoPtr->clientId          = clientId;
        entryInfoPtr->clientCookiePtr   = clientCookiePtr;
        entryInfoPtr->token             = token;
    }

    /* update the usage bitmap */
    if (entryType == CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E)
        entryTypeBitmap = 0x1;
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E)
        entryTypeBitmap = 0x3;
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E)
        entryTypeBitmap = 0x7;
    else if (entryType == CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E)
        entryTypeBitmap = 0xF;
    else /* can't happen */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    if (isUsed == GT_TRUE)
        tcamManagerPtr->entriesUsagePtr[index / 32] |= (entryTypeBitmap << (index % 32));
    else
        tcamManagerPtr->entriesUsagePtr[index / 32] &= (~(entryTypeBitmap << (index % 32)));

    return GT_OK;
}

/*******************************************************************************
* tcamEntriesNeededForAllocatedAndReservedEntries
*
* DESCRIPTION:
*       Gets the number of TCAM entries needed to hold the already allocated
*       entries as well as the reserved entries. For static reservations,
*       the reservation range can't be reused and the TCAM must have enough
*       entries for it (in addition to other clients).
*
* INPUTS:
*       tcamManagerPtr        - the TCAM manager object
*
* OUTPUTS:
*       tcamEntriesNeededPtr   - points to tcam entries needed
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS tcamEntriesNeededForAllocatedAndReservedEntries
(
    IN  PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr,
    OUT GT_U32                                  *tcamEntriesNeededPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    GT_U32                                      entriesNeededPerType[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];
    GT_U32                                      totalEntriesNeeded;
    GT_U32                                      singleEntryIndex;
    GT_U32                                      tripleEntryIndex;
    GT_U32                                      quadEntryIndex;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT       entryType;
    GT_U32                                      i;
    GT_U32                                      j;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(tcamManagerPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamEntriesNeededPtr);

    /* init entriesNeededPerType */
    for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        entriesNeededPerType[i] = 0;

    singleEntryIndex = entryTypeToArrayIndex(CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E);
    tripleEntryIndex = entryTypeToArrayIndex(CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E);
    quadEntryIndex   = entryTypeToArrayIndex(CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E);

    /**********************************************************/
    /* scan clients and get number of entries needed per type */
    /**********************************************************/
    for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
    {
        clientPtr = &tcamManagerPtr->clientsArray[i];

        /* skip unregistered clients */
        if (clientPtr->isRegistered == GT_FALSE)
            continue;

        /* static reservation can't be reused so it will be consider as if
           all the range is used by quad entries */
        if (clientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
        {
            entriesNeededPerType[quadEntryIndex] =
                (clientPtr->reservationRange.lastLine - clientPtr->reservationRange.firstLine + 1);
        }
        /* for dynamic reservasions, for each entry type take the max between
           allocated entries and reserved entries */
        else if (clientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E)
        {
            for (j = 0 ; j < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; j++)
            {
                if (clientPtr->allocatedEntriesArray[j] > clientPtr->reservedEntriesArray[j])
                    entriesNeededPerType[j] += clientPtr->allocatedEntriesArray[j];
                else
                    entriesNeededPerType[j] += clientPtr->reservedEntriesArray[j];
            }
        }
        else /* can't happen */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    /**********************************************/
    /* now calculate the number of entries needed */
    /**********************************************/
    totalEntriesNeeded = 0;

    /* line that holds triple entry can only hold single entry in addition
       to the triple entry;
       therefore each pair of one triple entry and one single entry can be
       replaced by one quad entry;
       triple entries that are left without matching single entries can be
       replaced by quad entry because the extra entry is needed for the triple
       entry and can't be used by other entry types */
    if (entriesNeededPerType[tripleEntryIndex] > 0)
    {
        entriesNeededPerType[quadEntryIndex] += entriesNeededPerType[tripleEntryIndex];
        if (entriesNeededPerType[tripleEntryIndex] > entriesNeededPerType[singleEntryIndex])
            entriesNeededPerType[singleEntryIndex] = 0;
        else
            entriesNeededPerType[singleEntryIndex] -= entriesNeededPerType[tripleEntryIndex];
        entriesNeededPerType[tripleEntryIndex] = 0;
    }

    /* now calc total entries needed */
    for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
    {
        entryType = arrayIndexToEntryType(i);
        totalEntriesNeeded += (entriesNeededPerType[i] * entryTypeSizeGet(entryType));
    }

    *tcamEntriesNeededPtr = totalEntriesNeeded;

    return GT_OK;
}

/*******************************************************************************
* isLineWithinRange
*
* DESCRIPTION:
*       Checks if a line is within a range of lines.
*
* INPUTS:
*       line                  - the line
*       rangePtr              - points to the range
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE:    the line is within the range
*       GT_FALSE:   otherwise
*
* COMMENTS:
*       The range must be a valid range.
*
*******************************************************************************/
static GT_BOOL isLineWithinRange
(
    IN  GT_U32                              line,
    IN  CPSS_DXCH_TCAM_MANAGER_RANGE_STC    *rangePtr
)
{
    if ((line >= rangePtr->firstLine) && (line <= rangePtr->lastLine))
        return GT_TRUE;
    else
        return GT_FALSE;
}

/*******************************************************************************
* entryAllocatePrepareSearchParameters
*
* DESCRIPTION:
*       Prepares search parameters for entry allocation function.
*
* INPUTS:
*       tcamManagerPtr      - the TCAM manager handler
*       clientPtr           - ptr to the TCAM manager client
*       entryType           - entry type required
*       lowerBoundToken     - lower bounding token
*       upperBoundToken     - upper bounding token
*       allocMethod         - allocation method required
*
* OUTPUTS:
*       lowerBoundIndexPtr      - lower bounding index
*       upperBoundIndexPtr      - upper bounding index
*       lowerBoundLineIndexPtr  - lower bounding line index
*       upperBoundLineIndexPtr  - upper bounding line index
*       searchLineIndexPtr      - search line index
*       midAllocLowerPtr        - middle allocation of lower index
*       midAllocUpperPtr        - middle allocation of upper index
*
* RETURNS:
*       GT_OK               - on success
*       GT_BAD_PARAM        - on wrong parameters
*       GT_OUT_OF_PP_MEM    - if failed to allocate TCAM memory.
*       GT_FAIL             - on failure
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS entryAllocatePrepareSearchParameters
(
    IN  PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr,
    IN  PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC   *clientPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType,
    IN  GT_U32                                  lowerBoundToken,
    IN  GT_U32                                  upperBoundToken,
    IN  CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT allocMethod,
    OUT GT_U32                                  *lowerBoundIndexPtr,
    OUT GT_U32                                  *upperBoundIndexPtr,
    OUT GT_U32                                  *lowerBoundLineIndexPtr,
    OUT GT_U32                                  *upperBoundLineIndexPtr,
    OUT GT_U32                                  *searchLineIndexPtr,
    OUT GT_U32                                  *midAllocLowerPtr,
    OUT GT_U32                                  *midAllocUpperPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_INDEX_RANGE_STC  sectionIndexRange;
    GT_U32                                      lowerBoundIndex;
    GT_U32                                      upperBoundIndex;
    GT_U32                                      lowerBoundLineIndex;
    GT_U32                                      upperBoundLineIndex;
    GT_U32                                      searchLineIndex;
    GT_U32                                      midAllocLower = 0;
    GT_U32                                      midAllocUpper = 0;
    GT_U32                                      i;
    PRV_CPSS_DXCH_TCAM_MANAGER_INDEX_RANGE_STC  *currentIndexRangePtr;
    GT_U32                                      boundariesMiddleIndex;
    GT_STATUS                                   rc = GT_OK;

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientPtr);
    CPSS_NULL_PTR_CHECK_MAC(lowerBoundIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(upperBoundIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(lowerBoundLineIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(upperBoundLineIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(searchLineIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(midAllocLowerPtr);
    CPSS_NULL_PTR_CHECK_MAC(midAllocUpperPtr);

    /*************************************************************************/
    /* Convert the bounding tokens to indexes that can be allocated to the   */
    /* new entries (not including the boundaries). If one of both boundaries */
    /* is not supplied, then the boundary is set according to the client     */
    /* reservation type. In case of dynamic reservation, the boundary will   */
    /* be set to TCAM manager range boundaries and in case of static         */
    /* reservation, the boundaries will be set to the static range allocated */
    /* to the specific entry type in the static reservation.                 */
    /*************************************************************************/

    /* get the index range for this allocation according to entry type and
       client reservation type */
    if (clientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
    {
        sectionIndexRange = clientPtr->entryPoolRangeArray[entryTypeToArrayIndex(entryType)];
    }
    else if (clientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E)
    {
        sectionIndexRange.firstIndex = 0;
        sectionIndexRange.lastIndex  = tcamManagerPtr->tcamEntries - 1;
    }
    else
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* convert lower bound token */
    if (lowerBoundToken == 0)
    {
        lowerBoundIndex = sectionIndexRange.firstIndex;
    }
    else
    {
        rc = convertTokenToIndex(tcamManagerPtr,lowerBoundToken,&lowerBoundIndex);
        if (rc != GT_OK)
            return rc;
        if (lowerBoundIndex >= sectionIndexRange.lastIndex)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        lowerBoundIndex++;
    }

    /* convert upper bound token */
    if (upperBoundToken == 0)
    {
        upperBoundIndex = sectionIndexRange.lastIndex;
    }
    else
    {
        rc = convertTokenToIndex(tcamManagerPtr,upperBoundToken,&upperBoundIndex);
        if (rc != GT_OK)
            return rc;
        if (upperBoundIndex <= sectionIndexRange.firstIndex)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        upperBoundIndex--;
    }

    /* verify that there is space between two boundaries */
    if (lowerBoundIndex > upperBoundIndex)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);

    lowerBoundLineIndex = lowerBoundIndex / 4 * 4;
    upperBoundLineIndex = upperBoundIndex / 4 * 4;

    /*****************************/
    /* prepare search parameters */
    /*****************************/

    /* set search initial index according to search method */
    switch (allocMethod)
    {
    case CPSS_DXCH_TCAM_MANAGER_ALLOC_MIN_E:
        searchLineIndex = lowerBoundLineIndex;
        for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
        {
            currentIndexRangePtr = &tcamManagerPtr->orderedStaticReservations[i];
            /* if last static reservation */
            if (currentIndexRangePtr->lastIndex < currentIndexRangePtr->firstIndex)
                break;
            /* check if the searchLineIndex in the static reservation range */
            if ((searchLineIndex >= currentIndexRangePtr->firstIndex) && (searchLineIndex <= currentIndexRangePtr->lastIndex))
                searchLineIndex = currentIndexRangePtr->lastIndex + 4;
        }
        /* verify that range between boundaries is not all covered with static reservations */
        if (searchLineIndex > upperBoundLineIndex)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        break;

    case CPSS_DXCH_TCAM_MANAGER_ALLOC_MID_E:
        /* get the middle of the range */
        boundariesMiddleIndex = (lowerBoundIndex + upperBoundIndex) / 2;
        /* align to line start */
        searchLineIndex = (boundariesMiddleIndex / 4) * 4;
        midAllocLower = searchLineIndex;
        midAllocUpper = searchLineIndex + 4;
        for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
        {
            currentIndexRangePtr = &tcamManagerPtr->orderedStaticReservations[i];
            /* if last static reservation */
            if (currentIndexRangePtr->lastIndex < currentIndexRangePtr->firstIndex)
                break;
            /* check if the searchLineIndex in the static reservation range */
            if ((midAllocUpper >= currentIndexRangePtr->firstIndex) && (midAllocUpper <= currentIndexRangePtr->lastIndex))
                midAllocUpper = currentIndexRangePtr->lastIndex + 4;
        }
        for (i = CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i > 0 ; i--)
        {
            currentIndexRangePtr = &tcamManagerPtr->orderedStaticReservations[i-1];
            /* skip non valid array entries */
            if (currentIndexRangePtr->lastIndex < currentIndexRangePtr->firstIndex)
                continue;
            /* check if the searchLineIndex in the static reservation range */
            if ((midAllocLower >= currentIndexRangePtr->firstIndex) && (midAllocLower <= currentIndexRangePtr->lastIndex))
                midAllocLower = currentIndexRangePtr->firstIndex - 4;
        }

        /* now update searchLineIndex if needed */

        /* both midAllocLower & midAllocUpper are out of boundaries, meaning
           that all range is covered with static reservations */
        if ((midAllocLower < lowerBoundLineIndex) && (midAllocLower > upperBoundLineIndex) && (midAllocUpper > upperBoundLineIndex))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        }
        /* midAllocLower is out of boundaries, start search with midAllocUpper */
        else if ((midAllocLower < lowerBoundLineIndex) && (midAllocLower > upperBoundLineIndex))
        {
            searchLineIndex = midAllocUpper;
        }
        /* midAllocUpper is out of boundaries, start search with midAllocLower */
        else if ((midAllocUpper > upperBoundLineIndex))
        {
            searchLineIndex = midAllocLower;
        }
        /* both midAllocLower & midAllocUpper are within boundaries, choose the
           one closer to the middle */
        else
        {
            if ((midAllocUpper - boundariesMiddleIndex) < (boundariesMiddleIndex - midAllocLower))
            {
                searchLineIndex = midAllocUpper;
            }
            else
            {
                searchLineIndex = midAllocLower;
            }
        }
        break;

    case CPSS_DXCH_TCAM_MANAGER_ALLOC_MAX_E:
        searchLineIndex = upperBoundLineIndex;
        for (i = CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i > 0 ; i--)
        {
            currentIndexRangePtr = &tcamManagerPtr->orderedStaticReservations[i-1];
            /* skip non valid array entries */
            if (currentIndexRangePtr->lastIndex < currentIndexRangePtr->firstIndex)
                continue;
            /* check if the searchLineIndex in the static reservation range */
            if ((searchLineIndex >= currentIndexRangePtr->firstIndex) && (searchLineIndex <= currentIndexRangePtr->lastIndex))
                searchLineIndex = currentIndexRangePtr->firstIndex - 4;
        }
        /* verify that range between boundaries is not all covered with static
           reservations (take in account possible unsigned integer rollover) */
        if ((searchLineIndex < lowerBoundLineIndex) || (searchLineIndex > upperBoundLineIndex))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        break;
    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */default: /* can't happen */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    *lowerBoundIndexPtr     = lowerBoundIndex;
    *upperBoundIndexPtr     = upperBoundIndex;
    *lowerBoundLineIndexPtr = lowerBoundLineIndex;
    *upperBoundLineIndexPtr = upperBoundLineIndex;
    *searchLineIndexPtr     = searchLineIndex;
    *midAllocLowerPtr       = midAllocLower;
    *midAllocUpperPtr       = midAllocUpper;
    return GT_OK;
}

/*******************************************************************************
* entryAllocateGetNextSearchLine
*
* DESCRIPTION:
*       Prepares search parameters for entry allocation function.
*
* INPUTS:
*       tcamManagerPtr      - the TCAM manager handler
*       clientPtr           - ptr to the TCAM manager client
*       lowerBoundIndex     - lower bounding index
*       upperBoundIndex     - upper bounding index
*       allocMethod         - allocation method required
*       searchLineIndexPtr  - search line index
*       midAllocLowerPtr    - middle allocation of lower ptr
*       midAllocUpperPtr    - middle allocation of upper ptr
*
* OUTPUTS:
*       searchLineIndexPtr  - search line index ptr
*       midAllocLowerPtr    - middle allocation of lower ptr
*       midAllocUpperPtr    - middle allocation of upper ptr
*
* RETURNS:
*       GT_OK               - on success
*       GT_BAD_PARAM        - on wrong parameters
*       GT_OUT_OF_PP_MEM    - if failed to allocate TCAM memory.
*       GT_FAIL             - on failure
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS entryAllocateGetNextSearchLine
(
    IN    PRV_CPSS_DXCH_TCAM_MANAGER_STC            *tcamManagerPtr,
    IN    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC     *clientPtr,
    IN    GT_U32                                    lowerBoundIndex,
    IN    GT_U32                                    upperBoundIndex,
    IN    CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT   allocMethod,
    INOUT GT_U32                                    *searchLineIndexPtr,
    INOUT GT_U32                                    *midAllocLowerPtr,
    INOUT GT_U32                                    *midAllocUpperPtr
)
{
    GT_U32                                      lowerBoundLineIndex;
    GT_U32                                      upperBoundLineIndex;
    GT_U32                                      searchLineIndex;
    GT_U32                                      midAllocLower;
    GT_U32                                      midAllocUpper;
    GT_U32                                      i;
    PRV_CPSS_DXCH_TCAM_MANAGER_INDEX_RANGE_STC  *currentIndexRangePtr;
    GT_U32                                      boundariesMiddleIndex;

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientPtr);
    CPSS_NULL_PTR_CHECK_MAC(searchLineIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(midAllocLowerPtr);
    CPSS_NULL_PTR_CHECK_MAC(midAllocUpperPtr);

    lowerBoundLineIndex = lowerBoundIndex / 4 * 4;
    upperBoundLineIndex = upperBoundIndex / 4 * 4;

    searchLineIndex  = *searchLineIndexPtr;
    midAllocLower    = *midAllocLowerPtr;
    midAllocUpper    = *midAllocUpperPtr;

    switch (allocMethod)
    {
    case CPSS_DXCH_TCAM_MANAGER_ALLOC_MIN_E:
        searchLineIndex += 4;
        for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
        {
            currentIndexRangePtr = &tcamManagerPtr->orderedStaticReservations[i];
            /* if last static reservation */
            if (currentIndexRangePtr->lastIndex < currentIndexRangePtr->firstIndex)
                break;
            /* check if the searchLineIndex in the static reservation range */
            if ((searchLineIndex >= currentIndexRangePtr->firstIndex) && (searchLineIndex <= currentIndexRangePtr->lastIndex))
                searchLineIndex = currentIndexRangePtr->lastIndex + 4;
        }
        /* verify the next line to search is still within boundaries */
        if (searchLineIndex > upperBoundLineIndex)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        break;

    case CPSS_DXCH_TCAM_MANAGER_ALLOC_MID_E:
        /* get the middle of the range */
        boundariesMiddleIndex = (lowerBoundIndex + upperBoundIndex) / 2;

        /* if last searched line was midAllocUpper, then get next midAllocUpper
           and skip static reservations on the way */
        if (searchLineIndex == midAllocUpper)
        {
            midAllocUpper += 4;
            for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
            {
                currentIndexRangePtr = &tcamManagerPtr->orderedStaticReservations[i];
                /* if last static reservation */
                if (currentIndexRangePtr->lastIndex < currentIndexRangePtr->firstIndex)
                    break;
                /* check if the searchLineIndex in the static reservation range */
                if ((midAllocUpper >= currentIndexRangePtr->firstIndex) && (midAllocUpper <= currentIndexRangePtr->lastIndex))
                    midAllocUpper = currentIndexRangePtr->lastIndex + 4;
            }
        }
        /* the last searched line was midAllocLower, so get next midAllocLower
           and skip static reservations on the way */
        else
        {
            /* last line searched was midAllocLower, decrease it */
            midAllocLower -= 4;
            for (i = CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i > 0 ; i--)
            {
                currentIndexRangePtr = &tcamManagerPtr->orderedStaticReservations[i-1];
                /* skip non valid array entries */
                if (currentIndexRangePtr->lastIndex < currentIndexRangePtr->firstIndex)
                    continue;
                /* check if the searchLineIndex in the static reservation range */
                if ((midAllocLower >= currentIndexRangePtr->firstIndex) && (midAllocLower <= currentIndexRangePtr->lastIndex))
                    midAllocLower = currentIndexRangePtr->firstIndex - 4;
            }
        }

        /* now update searchLineIndex */

        /* verify the next line to search is still within boundaries */
        if ((midAllocLower < lowerBoundLineIndex) && (midAllocLower > upperBoundLineIndex) && (midAllocUpper > upperBoundLineIndex))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        }
        /* midAllocLower is out of boundaries, continue search with midAllocUpper */
        else if ((midAllocLower < lowerBoundLineIndex) && (midAllocLower > upperBoundLineIndex))
        {
            searchLineIndex = midAllocUpper;
        }
        /* midAllocUpper is out of boundaries, continue search with midAllocLower */
        else if ((midAllocUpper > upperBoundLineIndex))
        {
            searchLineIndex = midAllocLower;
        }
        /* both midAllocLower & midAllocUpper are within boundaries, choose the
           one closer to the middle */
        else
        {
            if ((midAllocUpper - boundariesMiddleIndex) < (boundariesMiddleIndex - midAllocLower))
            {
                searchLineIndex = midAllocUpper;
            }
            else
            {
                searchLineIndex = midAllocLower;
            }
        }
        break;

    case CPSS_DXCH_TCAM_MANAGER_ALLOC_MAX_E:
        searchLineIndex -= 4;
        for (i = CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i > 0 ; i--)
        {
            currentIndexRangePtr = &tcamManagerPtr->orderedStaticReservations[i-1];
            /* skip non valid array entries */
            if (currentIndexRangePtr->lastIndex < currentIndexRangePtr->firstIndex)
                continue;
            /* check if the searchLineIndex in the static reservation range */
            if ((searchLineIndex >= currentIndexRangePtr->firstIndex) && (searchLineIndex <= currentIndexRangePtr->lastIndex))
                searchLineIndex = currentIndexRangePtr->firstIndex - 4;
        }
        /* verify the next line to search is still within boundaries (take in
           account possible unsigned integer rollover) */
        if ((searchLineIndex < lowerBoundLineIndex) || (searchLineIndex > upperBoundLineIndex))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        break;

    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[dead_error_begin] */default: /* can't happen */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    *searchLineIndexPtr = searchLineIndex;
    *midAllocLowerPtr   = midAllocLower;
    *midAllocUpperPtr   = midAllocUpper;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerCreate
*
* DESCRIPTION:
*       Creates TCAM manager and returns handler for the created TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamType              - the TCAM type that the TCAM manager will handle
*       rangePtr              - the TCAM range allocated for this TCAM manager
*
* OUTPUTS:
*       tcamManagerHandlerPtrPtr  - points to pointer of the created TCAM manager object
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_OUT_OF_CPU_MEM     - if no CPU memory for memory allocation
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerCreate
(
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT    tcamType,
    IN  CPSS_DXCH_TCAM_MANAGER_RANGE_STC        *rangePtr,
    OUT GT_VOID                                 **tcamManagerHandlerPtrPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC      *tcamManagerPtr = NULL;
    GT_U32                              i;
    GT_U32                              entriesUsageArraySize;

    /********************/
    /* check parameters */
    /********************/

    PRV_TCAM_MANAGER_TCAM_TYPE_CHECK_MAC(tcamType);
    CPSS_NULL_PTR_CHECK_MAC(rangePtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtrPtr);
    PRV_TCAM_MANAGER_TCAM_RANGE_CHECK_MAC(rangePtr);

    /***********************************************/
    /* allocate memory for the TCAM manager object */
    /***********************************************/

    tcamManagerPtr = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_STC));
    if (tcamManagerPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* zero out allocated memory */
    cpssOsMemSet(tcamManagerPtr,0,sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_STC));

    /***************************/
    /* initialize TCAM manager */
    /***************************/

    /* store the TCAM mananger parameters */
    tcamManagerPtr->tcamType            = tcamType;
    tcamManagerPtr->tcamRange.firstLine = rangePtr->firstLine;
    tcamManagerPtr->tcamRange.lastLine  = rangePtr->lastLine;

    /* initialize validity value code */
    tcamManagerPtr->validityValue       = PRV_TCAM_MANAGER_VALIDITY_CODE_CNS;

    /* initialize TCAM range information */
    tcamManagerPtr->tcamLines           = rangePtr->lastLine - rangePtr->firstLine + 1;
    tcamManagerPtr->tcamEntries         = tcamManagerPtr->tcamLines * 4;

    /* initialize clients array */
    for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
    {
        tcamManagerPtr->clientsArray[i].isRegistered = GT_FALSE;
    }

    /* allocate entries usage bitmap; the bitmap holds 1 bit per TCAM
       entry (32 bits per word) */
    entriesUsageArraySize = (tcamManagerPtr->tcamEntries + 31) / 32;
    tcamManagerPtr->entriesUsagePtr = cpssOsMalloc(entriesUsageArraySize * sizeof(GT_U32));
    if (tcamManagerPtr->entriesUsagePtr == NULL)
    {
        cpssOsFree(tcamManagerPtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* allocate the entries info array */
    tcamManagerPtr->entriesInfoArray = cpssOsMalloc(tcamManagerPtr->tcamEntries *
                                                    sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC));
    if (tcamManagerPtr->entriesInfoArray == NULL)
    {
        cpssOsFree(tcamManagerPtr->entriesUsagePtr);
        cpssOsFree(tcamManagerPtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* initialize all entries as not used */
    cpssOsMemSet(tcamManagerPtr->entriesUsagePtr,0,
                 (entriesUsageArraySize * sizeof(GT_U32)));
    cpssOsMemSet(tcamManagerPtr->entriesInfoArray,0,
                 (tcamManagerPtr->tcamEntries * sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC)));
    for (i = 0 ; i < tcamManagerPtr->tcamEntries; i++)
        tcamManagerPtr->entriesInfoArray[i].isUsed = GT_FALSE;

    /* allocate memory for tokenToIndexArray (max tokens is tcamEntries) token
       value 0 is reserved as it represents no boundary in allocation function */
    tcamManagerPtr->tokenToIndexArray = cpssOsMalloc((tcamManagerPtr->tcamEntries+1) * sizeof(GT_U32));
    if (tcamManagerPtr->tokenToIndexArray == NULL)
    {
        cpssOsFree(tcamManagerPtr->entriesInfoArray);
        cpssOsFree(tcamManagerPtr->entriesUsagePtr);
        cpssOsFree(tcamManagerPtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* initialize the tokenToIndexArray as link list of free tokens, token 0
       is reserved as it represents no boundary in allocation function */
    tcamManagerPtr->tokenToIndexArray[0] = 0;
    for (i = 1 ; i < tcamManagerPtr->tcamEntries ; i++)
        tcamManagerPtr->tokenToIndexArray[i] = PRV_TCAM_MANAGER_IN_FREE_TOKEN_LIST_CNS + i + 1;
    tcamManagerPtr->tokenToIndexArray[i] = PRV_TCAM_MANAGER_NO_MORE_TOKENS_CNS;
    tcamManagerPtr->tokenToIndexArraySize = tcamManagerPtr->tcamEntries+1;
    tcamManagerPtr->nextFreeToken = 1;

    /* initialize the ordered static reservation array */
    for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
    {
        /* last line is smaller then first line, meaning non valid range */
        tcamManagerPtr->orderedStaticReservations[i].firstIndex = 1;
        tcamManagerPtr->orderedStaticReservations[i].lastIndex  = 0;
    }

    /******************************************/
    /* initialization compeleted successfully */
    /******************************************/

    *tcamManagerHandlerPtrPtr = tcamManagerPtr;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerCreate
*
* DESCRIPTION:
*       Creates TCAM manager and returns handler for the created TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamType              - the TCAM type that the TCAM manager will handle
*       rangePtr              - the TCAM range allocated for this TCAM manager
*
* OUTPUTS:
*       tcamManagerHandlerPtrPtr  - points to pointer of the created TCAM manager object
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_OUT_OF_CPU_MEM     - if no CPU memory for memory allocation
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerCreate
(
    IN  CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT    tcamType,
    IN  CPSS_DXCH_TCAM_MANAGER_RANGE_STC        *rangePtr,
    OUT GT_VOID                                 **tcamManagerHandlerPtrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamType, rangePtr, tcamManagerHandlerPtrPtr));

    rc = internal_cpssDxChTcamManagerCreate(tcamType, rangePtr, tcamManagerHandlerPtrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamType, rangePtr, tcamManagerHandlerPtrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerConfigGet
*
* DESCRIPTION:
*       Gets the configuration of the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       tcamTypePtr           - points to TCAM type managed by the TCAM manager
*       rangePtr              - points to the TCAM range managed by the TCAM manager
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerConfigGet
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    OUT CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT    *tcamTypePtr,
    OUT CPSS_DXCH_TCAM_MANAGER_RANGE_STC        *rangePtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC      *tcamManagerPtr = NULL;

    /* check parameters */

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(rangePtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    /* fill output values */

    *tcamTypePtr        = tcamManagerPtr->tcamType;
    rangePtr->firstLine = tcamManagerPtr->tcamRange.firstLine;
    rangePtr->lastLine  = tcamManagerPtr->tcamRange.lastLine;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerConfigGet
*
* DESCRIPTION:
*       Gets the configuration of the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       tcamTypePtr           - points to TCAM type managed by the TCAM manager
*       rangePtr              - points to the TCAM range managed by the TCAM manager
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerConfigGet
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    OUT CPSS_DXCH_TCAM_MANAGER_TCAM_TYPE_ENT    *tcamTypePtr,
    OUT CPSS_DXCH_TCAM_MANAGER_RANGE_STC        *rangePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerConfigGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, tcamTypePtr, rangePtr));

    rc = internal_cpssDxChTcamManagerConfigGet(tcamManagerHandlerPtr, tcamTypePtr, rangePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, tcamTypePtr, rangePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerDelete
*
* DESCRIPTION:
*       Deletes the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       All entries allocated by the TCAM manager must be freed prior of calling
*       this API. Clients may be registered upon calling the API.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerDelete
(
    IN  GT_VOID         *tcamManagerHandlerPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC      *tcamManagerPtr = NULL;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    /******************************************************************/
    /* verify that there are no allocated entries in the TCAM manager */
    /******************************************************************/


    /**************************************************/
    /* delete the TCAM manager and free its resources */
    /**************************************************/

    cpssOsFree(tcamManagerPtr->tokenToIndexArray);
    cpssOsFree(tcamManagerPtr->entriesUsagePtr);
    cpssOsFree(tcamManagerPtr->entriesInfoArray);
    tcamManagerPtr->validityValue = 0; /* to verify that using same pointer again will fail */
    cpssOsFree(tcamManagerPtr);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerDelete
*
* DESCRIPTION:
*       Deletes the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       All entries allocated by the TCAM manager must be freed prior of calling
*       this API. Clients may be registered upon calling the API.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerDelete
(
    IN  GT_VOID         *tcamManagerHandlerPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr));

    rc = internal_cpssDxChTcamManagerDelete(tcamManagerHandlerPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerRangeUpdate
*
* DESCRIPTION:
*       Updates the range allocated for the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       rangePtr              - the new TCAM range allocated for this TCAM manager
*       rangeUpdateMethod     - update method to use during the range update
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. Since the ranges for static elients are explicitly stated by the clients,
*          then static entries will never be moved (regardless of the range update
*          method). If any static entry (either allocated or reserved by static
*          client) is outside the new range, the function will fail.
*       2. In case of smaller new range, the new range must be big enough to
*          hold all current allocated and reserved entries.
*       3. In CPSS_DXCH_TCAM_MANAGER_DO_NOT_MOVE_RANGE_UPDATE_METHOD_E update
*          method, if any of existing entries is outside of the new range,
*          the API will fail.
*       4. In CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_KEEP_OFFSETS_RANGE_UPDATE_METHOD_E
*          update method, if any of the existing entries will not fit in the new
*          range then the API will fail.
*       5. Since TCAM manager keeps ordering between same type of entries, then
*          upon range update, ordering between entries of different types may
*          be changed.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerRangeUpdate
(
    IN  GT_VOID                                         *tcamManagerHandlerPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_RANGE_STC                *rangePtr,
    IN  CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT  rangeUpdateMethod
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    GT_U32                                      i;
    GT_U32                                      j;
    GT_U32                                      entriesNeeded;
    GT_U32                                      neededEntries[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];
    GT_STATUS                                   rc = GT_OK;

    PRV_CPSS_DXCH_TCAM_MANAGER_STC              tempTcamManager;

    GT_U32                                      newEntriesUsageArraySize;
    GT_U32                                      entryIndex;
    GT_U32                                      oldIndex;
    GT_U32                                      newIndex;
    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entryPtr;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    oldLocation;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    newLocation;
    GT_U32                                      oldToken;
    GT_U32                                      newToken;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(rangePtr);
    PRV_TCAM_MANAGER_RANGE_UPDATE_METHOD_CHECK_MAC(rangeUpdateMethod);
    PRV_TCAM_MANAGER_TCAM_RANGE_CHECK_MAC(rangePtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    /* verify that no static reservation reside outside the new TCAM manager range */
    for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
    {
        clientPtr = &tcamManagerPtr->clientsArray[i];
        /* verify the client is registered */
        if (clientPtr->isRegistered == GT_FALSE)
            continue;
        /* verify the reservation type is static one */
        if (clientPtr->reservationType != CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
            continue;
        /* verify the static reservation range is not outside the new TCAM manager range */
        if ((clientPtr->reservationRange.firstLine < rangePtr->firstLine) ||
            (clientPtr->reservationRange.firstLine > rangePtr->lastLine) ||
            (clientPtr->reservationRange.lastLine < rangePtr->firstLine) ||
            (clientPtr->reservationRange.lastLine > rangePtr->lastLine))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* verify that the new range has enough space to hold all currently
       allocated/reserved entries */
    entriesNeeded = 0;
    for (j = 0 ; j < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; j++)
        neededEntries[j] = 0;

    for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
    {
        clientPtr = &tcamManagerPtr->clientsArray[i];

        /* skip unregistered clients */
        if (clientPtr->isRegistered == GT_FALSE)
            continue;

        if (clientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
        {
            entriesNeeded += ((clientPtr->reservationRange.lastLine - clientPtr->reservationRange.firstLine + 1) * 4);
        }
        else if (clientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E)
        {
            for (j = 0 ; j < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; j++)
            {
                if (clientPtr->allocatedEntriesArray[j] > clientPtr->reservedEntriesArray[j])
                    neededEntries[j] += clientPtr->allocatedEntriesArray[j];
                else
                    neededEntries[j] += clientPtr->reservedEntriesArray[j];
            }
        }
        else /* can't happen */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        entriesNeeded += (neededEntries[i] * entryTypeSizeGet(arrayIndexToEntryType(i)));
    if (entriesNeeded > tcamManagerPtr->tcamEntries)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);

    tempTcamManager.tcamRange.firstLine = rangePtr->firstLine;
    tempTcamManager.tcamRange.lastLine  = rangePtr->lastLine;
    tempTcamManager.tcamLines           = rangePtr->lastLine - rangePtr->firstLine + 1;
    tempTcamManager.tcamEntries         = tempTcamManager.tcamLines * 4;

    /* currently only do not move method will be applied */
    if (rangeUpdateMethod == CPSS_DXCH_TCAM_MANAGER_DO_NOT_MOVE_RANGE_UPDATE_METHOD_E)
    {
        if ((tempTcamManager.tcamRange.firstLine > tcamManagerPtr->tcamRange.firstLine) &&
            (tempTcamManager.tcamRange.firstLine < tcamManagerPtr->tcamRange.lastLine))
        {
            /* make sure no entries are allocated at the range not included in
               the new range */
            entryIndex = (tempTcamManager.tcamRange.firstLine - tcamManagerPtr->tcamRange.firstLine) * 4;
            for (i = 0 ; i < entryIndex ; i++)
            {
                if (tcamManagerPtr->entriesInfoArray[i].isUsed == GT_TRUE)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        if ((tempTcamManager.tcamRange.lastLine > tcamManagerPtr->tcamRange.firstLine) &&
            (tempTcamManager.tcamRange.lastLine < tcamManagerPtr->tcamRange.lastLine))
        {
            entryIndex = (tempTcamManager.tcamRange.lastLine - tcamManagerPtr->tcamRange.firstLine + 1) * 4;
            for (i = entryIndex ; i < tcamManagerPtr->tcamEntries ; i++)
            {
                if (tcamManagerPtr->entriesInfoArray[i].isUsed == GT_TRUE)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }
    else
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);

    /* create new resources for the new range */

    newEntriesUsageArraySize = (tempTcamManager.tcamEntries + 31) / 32;
    tempTcamManager.entriesUsagePtr = cpssOsMalloc(newEntriesUsageArraySize * sizeof(GT_U32));
    if (tempTcamManager.entriesUsagePtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    tempTcamManager.entriesInfoArray = cpssOsMalloc(tempTcamManager.tcamEntries * sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC));
    if (tempTcamManager.entriesInfoArray == NULL)
    {
        cpssOsFree(tempTcamManager.entriesUsagePtr);
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* initialize all entries as not used */
    cpssOsMemSet(tempTcamManager.entriesUsagePtr,0,(newEntriesUsageArraySize * sizeof(GT_U32)));
    cpssOsMemSet(tempTcamManager.entriesInfoArray,0,tempTcamManager.tcamEntries * sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC));
    for (i = 0 ; i < tempTcamManager.tcamEntries; i++)
        tempTcamManager.entriesInfoArray[i].isUsed = GT_FALSE;

    /* copy all entries from old array to new array */
    if (tcamManagerPtr->tcamRange.firstLine < tempTcamManager.tcamRange.firstLine)
    {
        oldIndex = (tempTcamManager.tcamRange.firstLine - tcamManagerPtr->tcamRange.firstLine) * 4;
        newIndex = 0;
    }
    else
    {
        oldIndex = 0;
        newIndex = (tcamManagerPtr->tcamRange.firstLine - tempTcamManager.tcamRange.firstLine) * 4;
    }

    for ( ; (oldIndex < tcamManagerPtr->tcamEntries) && (newIndex < tempTcamManager.tcamEntries) ; oldIndex++ , newIndex++)
    {
        entryPtr = &tcamManagerPtr->entriesInfoArray[oldIndex];
        if (entryPtr->isUsed == GT_TRUE)
        {
            /* update usage entry */
            rc = setEntryUsage(&tempTcamManager,newIndex,
                               entryPtr->type,entryPtr->isUsed,entryPtr->clientId,
                               entryPtr->clientCookiePtr,entryPtr->token);
            if (rc != GT_OK)
                break;
            /* convert old index to token */
            rc = convertIndexToToken(tcamManagerPtr,oldIndex,&oldToken);
            if (rc != GT_OK)
                break;
            /* get location of old token */
            rc = cpssDxChTcamManagerTokenToTcamLocation(tcamManagerHandlerPtr,entryPtr->clientId,oldToken,&oldLocation);
            if (rc != GT_OK)
                break;
            /* get new token */
            rc = convertIndexToToken(&tempTcamManager,newIndex,&newToken);
            if (rc != GT_OK)
                break;
            /* new location is the same as old one */
            newLocation.row    = oldLocation.row;
            newLocation.column = oldLocation.column;
            /* now call client callback function */
            clientPtr = &tcamManagerPtr->clientsArray[entryPtr->clientId];
            if (clientPtr->callbackFunc.moveToLocationFuncPtr != NULL)
                rc = clientPtr->callbackFunc.moveToLocationFuncPtr(&oldLocation,&newLocation,entryPtr->clientCookiePtr);
            else
                rc = GT_NOT_READY;
            if (rc != GT_OK)
                break;
        }
    }
    if (rc != GT_OK)
    {
        cpssOsFree(tempTcamManager.entriesUsagePtr);
        cpssOsFree(tempTcamManager.entriesInfoArray);
        return rc;
    }

    /* Check whether new TCAM range is bigger than tokenToIndexArraySize. If so
       then tokenToIndexArray will be enlarged. The array will not be reduced
       as there might be tokens in use in the range bigger than new tcamEntries */
    if ((tempTcamManager.tcamEntries + 1) > tcamManagerPtr->tokenToIndexArraySize)
    {
        tcamManagerPtr->tokenToIndexArray = cpssOsRealloc(tcamManagerPtr->tokenToIndexArray,
                                                          sizeof(GT_U32)*(tempTcamManager.tcamEntries+1));
        if (tcamManagerPtr->tokenToIndexArray == NULL)
        {
            cpssOsFree(tcamManagerPtr->entriesUsagePtr);
            cpssOsFree(tcamManagerPtr->entriesInfoArray);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        /* init the new tokens as free tokens */
        for (i = tcamManagerPtr->tokenToIndexArraySize ; i < tempTcamManager.tcamEntries ; i++)
            tcamManagerPtr->tokenToIndexArray[i] = PRV_TCAM_MANAGER_IN_FREE_TOKEN_LIST_CNS + i + 1;
        /* connect the new free tokens before the previous free token list */
        tcamManagerPtr->tokenToIndexArray[i] = tcamManagerPtr->nextFreeToken + PRV_TCAM_MANAGER_IN_FREE_TOKEN_LIST_CNS;
        tcamManagerPtr->nextFreeToken = tcamManagerPtr->tokenToIndexArraySize;
        /* update array size to new size */
        tcamManagerPtr->tokenToIndexArraySize = (tempTcamManager.tcamEntries + 1);
    }

    /* release old resources and copy new information */
    cpssOsFree(tcamManagerPtr->entriesUsagePtr);
    cpssOsFree(tcamManagerPtr->entriesInfoArray);

    tcamManagerPtr->tcamRange.firstLine = tempTcamManager.tcamRange.firstLine;
    tcamManagerPtr->tcamRange.lastLine  = tempTcamManager.tcamRange.lastLine;
    tcamManagerPtr->tcamLines           = tempTcamManager.tcamLines;
    tcamManagerPtr->tcamEntries         = tempTcamManager.tcamEntries;
    tcamManagerPtr->entriesUsagePtr     = tempTcamManager.entriesUsagePtr;
    tcamManagerPtr->entriesInfoArray    = tempTcamManager.entriesInfoArray;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerRangeUpdate
*
* DESCRIPTION:
*       Updates the range allocated for the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       rangePtr              - the new TCAM range allocated for this TCAM manager
*       rangeUpdateMethod     - update method to use during the range update
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. Since the ranges for static elients are explicitly stated by the clients,
*          then static entries will never be moved (regardless of the range update
*          method). If any static entry (either allocated or reserved by static
*          client) is outside the new range, the function will fail.
*       2. In case of smaller new range, the new range must be big enough to
*          hold all current allocated and reserved entries.
*       3. In CPSS_DXCH_TCAM_MANAGER_DO_NOT_MOVE_RANGE_UPDATE_METHOD_E update
*          method, if any of existing entries is outside of the new range,
*          the API will fail.
*       4. In CPSS_DXCH_TCAM_MANAGER_MOVE_TOP_AND_KEEP_OFFSETS_RANGE_UPDATE_METHOD_E
*          update method, if any of the existing entries will not fit in the new
*          range then the API will fail.
*       5. Since TCAM manager keeps ordering between same type of entries, then
*          upon range update, ordering between entries of different types may
*          be changed.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerRangeUpdate
(
    IN  GT_VOID                                         *tcamManagerHandlerPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_RANGE_STC                *rangePtr,
    IN  CPSS_DXCH_TCAM_MANAGER_RANGE_UPDATE_METHOD_ENT  rangeUpdateMethod
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerRangeUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, rangePtr, rangeUpdateMethod));

    rc = internal_cpssDxChTcamManagerRangeUpdate(tcamManagerHandlerPtr, rangePtr, rangeUpdateMethod);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, rangePtr, rangeUpdateMethod));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerClientRegister
*
* DESCRIPTION:
*       Register a client in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientNameArr         - string that represent the client
*       clientFuncPtr         - points to callback functions supplied by the client
*
* OUTPUTS:
*       clientIdPtr           - points to client Id allocated by the TCAM manager
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FULL               - when max number of clients already registered to
*                               the TCAM manager
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. Up to CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS clients is supported.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerClientRegister
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  GT_CHAR                                 clientNameArr[CPSS_DXCH_TCAM_MANAGER_CLIENT_NAME_NAX_LENGTH_CNS],
    IN  CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC  *clientFuncPtr,
    OUT GT_U32                                  *clientIdPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC   *clientPtr = NULL;
    GT_U32                                  clientId;
    GT_U32                                  i;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientNameArr);
    CPSS_NULL_PTR_CHECK_MAC(clientFuncPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientFuncPtr->moveToLocationFuncPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientFuncPtr->moveToAnywhereFuncPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientFuncPtr->checkIfDefaultLocationFuncPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientIdPtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    /* search for free slot in the clients array */
    for (clientId = 0 ; clientId < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; clientId++)
    {
        if (tcamManagerPtr->clientsArray[clientId].isRegistered == GT_FALSE)
            break;
    }
    if (clientId == CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);

    /* store client information in the client array */
    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    clientPtr->isRegistered = GT_TRUE;
    for (i = 0 ; (i < CPSS_DXCH_TCAM_MANAGER_CLIENT_NAME_NAX_LENGTH_CNS) && (clientNameArr[i] != '\0') ; i++)
        clientPtr->name[i] = clientNameArr[i];
    clientPtr->name[CPSS_DXCH_TCAM_MANAGER_CLIENT_NAME_NAX_LENGTH_CNS-1] = '\0';
    cpssOsMemCpy(&(clientPtr->callbackFunc),clientFuncPtr,sizeof(CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC));
    clientPtr->isReservationTypeLocked = GT_FALSE;
    clientPtr->reservationType         = CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E;

    /* return client id */
    *clientIdPtr = clientId;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerClientRegister
*
* DESCRIPTION:
*       Register a client in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientNameArr         - string that represent the client
*       clientFuncPtr         - points to callback functions supplied by the client
*
* OUTPUTS:
*       clientIdPtr           - points to client Id allocated by the TCAM manager
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_FULL               - when max number of clients already registered to
*                               the TCAM manager
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. Up to CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS clients is supported.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerClientRegister
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  GT_CHAR                                 clientNameArr[CPSS_DXCH_TCAM_MANAGER_CLIENT_NAME_NAX_LENGTH_CNS],
    IN  CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC  *clientFuncPtr,
    OUT GT_U32                                  *clientIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerClientRegister);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientNameArr, clientFuncPtr, clientIdPtr));

    rc = internal_cpssDxChTcamManagerClientRegister(tcamManagerHandlerPtr, clientNameArr, clientFuncPtr, clientIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientNameArr, clientFuncPtr, clientIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerClientUnregister
*
* DESCRIPTION:
*       Unregister a client in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerClientUnregister
(
    IN  GT_VOID     *tcamManagerHandlerPtr,
    IN  GT_U32      clientId
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC   *clientPtr = NULL;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /* verify that no entries are allocated for the client */

    /* mark client entry in TCAM client array as free */
    cpssOsMemSet(clientPtr,0,sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC));
    clientPtr->isRegistered = GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerClientUnregister
*
* DESCRIPTION:
*       Unregister a client in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerClientUnregister
(
    IN  GT_VOID     *tcamManagerHandlerPtr,
    IN  GT_U32      clientId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerClientUnregister);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId));

    rc = internal_cpssDxChTcamManagerClientUnregister(tcamManagerHandlerPtr, clientId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerClientFuncUpdate
*
* DESCRIPTION:
*       Updates client callback functions in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       clientFuncPtr         - points to callback functions supplied by the client
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - on not registered clientId
*       GT_FAIL               - on error
*
* COMMENTS:
*       After TCAM manager import, the callback functions of all clients are set
*       to NULL (to prevent using non valid function pointers). Untill the client
*       updates the callback functions, the client will be considered inactive
*       and will be limited.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerClientFuncUpdate
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  GT_U32                                  clientId,
    IN  CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC  *clientFuncPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC   *clientPtr = NULL;

    /* check parameters */
    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);
    CPSS_NULL_PTR_CHECK_MAC(clientFuncPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientFuncPtr->moveToLocationFuncPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientFuncPtr->moveToAnywhereFuncPtr);
    CPSS_NULL_PTR_CHECK_MAC(clientFuncPtr->checkIfDefaultLocationFuncPtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    cpssOsMemCpy(&(clientPtr->callbackFunc),clientFuncPtr,sizeof(CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerClientFuncUpdate
*
* DESCRIPTION:
*       Updates client callback functions in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       clientFuncPtr         - points to callback functions supplied by the client
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - on not registered clientId
*       GT_FAIL               - on error
*
* COMMENTS:
*       After TCAM manager import, the callback functions of all clients are set
*       to NULL (to prevent using non valid function pointers). Untill the client
*       updates the callback functions, the client will be considered inactive
*       and will be limited.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerClientFuncUpdate
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  GT_U32                                  clientId,
    IN  CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC  *clientFuncPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerClientFuncUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, clientFuncPtr));

    rc = internal_cpssDxChTcamManagerClientFuncUpdate(tcamManagerHandlerPtr, clientId, clientFuncPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, clientFuncPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerEntriesReservationSet
*
* DESCRIPTION:
*       Reserves entries for a client in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       reservationType       - type of the reservation
*       requestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*       reservationRangePtr   - the TCAM range allocated for this reservation;
*                               relevant only for static reservation type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - on not registered clientId
*       GT_ALREADY_EXIST      - on static reservation with range that collides
*                               with other already existing static reservations
*       GT_NO_RESOURCE        - the reservation can't fit in the TCAM
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. The TCAM manager will save enough space so that at any given time,
*          the client will have enough space for up to the number of reserved
*          entries.
*       2. In case of static entries reservation, the TCAM range allocated to
*          the static reservation must be within the range allocated to the
*          TCAM manager. The requested range can't contain entries reserved for
*          other static reservation.
*       3. Each client can have only one reservation. New reservation for a
*          client will replace the previous reservation.
*       4. Elements in the requestedEntriesArray must refer to unique entries
*          types. Meaning that no two array elements can have same entry type.
*          In addition the amount of requested entries can't be 0.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerEntriesReservationSet
(
    IN  GT_VOID                                             *tcamManagerHandlerPtr,
    IN  GT_U32                                              clientId,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT reservationType,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             requestedEntriesArray[],
    IN  GT_U32                                              sizeOfArray,
    IN  CPSS_DXCH_TCAM_MANAGER_RANGE_STC                    *reservationRangePtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *tempClientPtr = NULL;
    GT_U32                                      i;
    GT_U32                                      j;
    GT_U32                                      linesInStaticRange;

    GT_U32  requestedEntriesPerType[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];
    GT_U32  currentReservationBackup[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];
    GT_U32  totalEntriesNeeded;
    GT_U32  arrayIndex;

    GT_U32                                      neededEntries[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];

    GT_U32                                      currentIndex = 0;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT       entryType;
    PRV_CPSS_DXCH_TCAM_MANAGER_INDEX_RANGE_STC  oldEntryPoolRangeArray[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];
    GT_BOOL                                     needToMoveEntries;
    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entryPtr;
    GT_U32                                      oldIndex;
    GT_U32                                      newIndex;
    GT_U32                                      oldToken;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    oldLocation;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    newLocation;

    GT_STATUS                                   rc = GT_OK;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);
    PRV_TCAM_MANAGER_RESERVATION_TYPE_CHECK_MAC(reservationType);
    CPSS_NULL_PTR_CHECK_MAC(requestedEntriesArray);
    if (reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
    {
        CPSS_NULL_PTR_CHECK_MAC(reservationRangePtr);
        if (sizeOfArray == 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /* convert requestedEntriesArray to ordered requestedEntriesPerType;
       verify that requested entries are non zero and unique */
    for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        requestedEntriesPerType[i] = 0;
    for (i = 0 ; i < sizeOfArray; i++)
    {
        PRV_TCAM_MANAGER_ENTRY_TYPE_CHECK_MAC(requestedEntriesArray[i].entryType);
        if (requestedEntriesArray[i].amount == 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        arrayIndex = entryTypeToArrayIndex(requestedEntriesArray[i].entryType);
        if (requestedEntriesPerType[arrayIndex] != 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        else
            requestedEntriesPerType[arrayIndex] = requestedEntriesArray[i].amount;
    }

    /* reservation type can't be changed after it was locked */
    if (clientPtr->isReservationTypeLocked == GT_TRUE)
    {
        if (clientPtr->reservationType != reservationType)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /**********************/
    /* static reservation */
    /**********************/
    if (reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
    {
        /********************************************/
        /* additional checks for static reservation */
        /********************************************/

        /* verify that the static range is within the TCAM manager range */
        if ((isLineWithinRange(reservationRangePtr->firstLine,&tcamManagerPtr->tcamRange) == GT_FALSE) ||
            (isLineWithinRange(reservationRangePtr->lastLine,&tcamManagerPtr->tcamRange) == GT_FALSE))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        /* triple entries are not supported for static reservation */
        arrayIndex = entryTypeToArrayIndex(CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E);
        if (requestedEntriesPerType[arrayIndex] != 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        /* verify that the requested reservation range doesn't collide with
           allready existing static reservations of other clients */
        for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
        {
            if (i == clientId)
                continue;
            tempClientPtr = &tcamManagerPtr->clientsArray[i];
            /* skip unregistered clients */
            if (tempClientPtr->isRegistered == GT_FALSE)
                continue;
            if (tempClientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
            {
                if ((isLineWithinRange(reservationRangePtr->firstLine,&tempClientPtr->reservationRange) == GT_TRUE) ||
                    (isLineWithinRange(reservationRangePtr->lastLine,&tempClientPtr->reservationRange) == GT_TRUE))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
            }
        }

        linesInStaticRange = (reservationRangePtr->lastLine - reservationRangePtr->firstLine + 1);

        /* verify that the range is big enough to hold the requested entries */
        totalEntriesNeeded = 0;
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        {
            entryType = arrayIndexToEntryType(i);
            totalEntriesNeeded += (requestedEntriesPerType[i] * entryTypeSizeGet(entryType));
        }
        if (totalEntriesNeeded > (linesInStaticRange * 4))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

        /* verify that currently allocated entries can fit in new reservation */
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        {
            if (clientPtr->allocatedEntriesArray[i] > requestedEntriesPerType[i])
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* The static reservation will need full lines according to the requested
           range. Verify that the TCAM manager has enough space */
        totalEntriesNeeded = 0;
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
            neededEntries[i] = 0;
        for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
        {
            tempClientPtr = &tcamManagerPtr->clientsArray[i];

            if (tempClientPtr->isRegistered == GT_FALSE)
                continue;

            if (i == clientId)
            {
                totalEntriesNeeded += (linesInStaticRange * 4);
                continue;
            }

            if (tempClientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
            {
                totalEntriesNeeded += ((tempClientPtr->reservationRange.lastLine - tempClientPtr->reservationRange.firstLine + 1) * 4);
                continue;
            }

            for (j = 0 ; j < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; j++)
            {
                if (clientPtr->allocatedEntriesArray[j] > clientPtr->reservedEntriesArray[j])
                    neededEntries[j] += clientPtr->allocatedEntriesArray[j];
                else
                    neededEntries[j] += clientPtr->reservedEntriesArray[j];
            }
        }
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        {
            entryType = arrayIndexToEntryType(i);
            totalEntriesNeeded += (neededEntries[i] * entryTypeSizeGet(entryType));
        }
        if (totalEntriesNeeded > tcamManagerPtr->tcamEntries)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);

        /* check that currently allocated entries can fit new reservation */
        clientPtr = &tcamManagerPtr->clientsArray[clientId];
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        {
            if (clientPtr->allocatedEntriesArray[i] > requestedEntriesPerType[i])
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* save the current pool before allocating another pool */
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        {
            oldEntryPoolRangeArray[i].firstIndex = clientPtr->entryPoolRangeArray[i].firstIndex;
            oldEntryPoolRangeArray[i].lastIndex  = clientPtr->entryPoolRangeArray[i].lastIndex;
        }

        /* additional tests in case of reservation update (can be detected
           when resrevation is locked) */
        if (clientPtr->isReservationTypeLocked == GT_TRUE)
        {
            /* new range and old range must start at the same line */
            /* TBD: remove this restriction */
            if (reservationRangePtr->firstLine != clientPtr->reservationRange.firstLine)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

            /* updated reservation can support single entries only */
            /* TBD: remove this restriction */
            for (i = 1 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
            {
                if (requestedEntriesPerType[i] != 0)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* all currently allocated entries must be single entries */
            /* TBD: remove this restriction */
            for (i = 1 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
            {
                if (clientPtr->allocatedEntriesArray[i] != 0)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }

        /* all tests passes, allocate index ranges to each entry type, QUAD first,
           then dual and last single */
        currentIndex = (reservationRangePtr->firstLine - tcamManagerPtr->tcamRange.firstLine) * 4;
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        {
            j = PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS - i - 1;
            entryType = arrayIndexToEntryType(j);
            if (requestedEntriesPerType[j] > 0)
            {
                clientPtr->entryPoolRangeArray[j].firstIndex = currentIndex;
                currentIndex += ((requestedEntriesPerType[j] - 1) * entryTypeSizeGet(entryType));
                clientPtr->entryPoolRangeArray[j].lastIndex = currentIndex;
                currentIndex += entryTypeSizeGet(entryType);
            }
        }

        if (clientPtr->isReservationTypeLocked == GT_TRUE)
        {
            /* check if there are currently allocated entries that are outside the
               new pool (TBD: for single entries) */
            needToMoveEntries = GT_FALSE;
            for (i = oldEntryPoolRangeArray[0].firstIndex ; i <= oldEntryPoolRangeArray[0].lastIndex ; i++)
            {
                /* skip the area also allocated in the new range */
                if ((i >= clientPtr->entryPoolRangeArray[0].firstIndex) &&
                    (i <= clientPtr->entryPoolRangeArray[0].lastIndex))
                {
                    i = clientPtr->entryPoolRangeArray[0].lastIndex;
                    continue;
                }

                if (tcamManagerPtr->entriesInfoArray[i].isUsed == GT_TRUE)
                {
                    needToMoveEntries = GT_TRUE;
                    break;
                }
            }

            /* if there is a need to move entries to fit the new reservation range...
               do it now */
            if (needToMoveEntries == GT_TRUE)
            {
                /* compress all entries to the top, remember that all static reservation
                   must start at TCAM manager range start so compressing to top will
                   not break TCAM order */
                oldIndex = oldEntryPoolRangeArray[0].firstIndex;
                newIndex = clientPtr->entryPoolRangeArray[0].firstIndex;

                for ( ; oldIndex <= oldEntryPoolRangeArray[0].lastIndex ; oldIndex++)
                {
                    entryPtr = &tcamManagerPtr->entriesInfoArray[oldIndex];
                    if (entryPtr->isUsed == GT_TRUE)
                    {
                        /* if same index, nothing to do */
                        if (oldIndex == newIndex)
                        {
                            newIndex++;
                            continue;
                        }

                        /* convert old index to token */
                        rc = convertIndexToToken(tcamManagerPtr,oldIndex,&oldToken);
                        if (rc != GT_OK)
                            break;
                        /* get location of old token */
                        rc = cpssDxChTcamManagerTokenToTcamLocation(tcamManagerHandlerPtr,entryPtr->clientId,oldToken,&oldLocation);
                        if (rc != GT_OK)
                            break;
                        newLocation.row    = (newIndex / 4) + tcamManagerPtr->tcamRange.firstLine;
                        newLocation.column = (newIndex % 4);
                        /* now call client callback function */
                        if (clientPtr->callbackFunc.moveToLocationFuncPtr != NULL)
                            rc = clientPtr->callbackFunc.moveToLocationFuncPtr(&oldLocation,&newLocation,entryPtr->clientCookiePtr);
                        else
                            rc = GT_NOT_READY;
                        if (rc != GT_OK)
                            break;
                        /* move succeed, update usage entry */
                        rc = setEntryUsage(tcamManagerPtr,newIndex,
                                           entryPtr->type,entryPtr->isUsed,entryPtr->clientId,
                                           entryPtr->clientCookiePtr,entryPtr->token);
                        if (rc != GT_OK)
                            break;
                        rc = setEntryUsage(tcamManagerPtr,oldIndex,entryPtr->type,GT_FALSE,
                                           entryPtr->clientId,entryPtr->clientCookiePtr,entryPtr->token);
                        if (rc != GT_OK)
                            break;
                        tcamManagerPtr->tokenToIndexArray[oldToken] = newIndex;

                        newIndex++;
                    }
                }
            }
        }
    }
    else if (reservationType == CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E)
    {
        /* for dynamic reservation just verify that the TCAM manager has
           enough space for the new/updated reservation */

        /* backup the current reservation state of the client; note that for
           first reservation the current reservation is all 0 as it was set
           during client registration */
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
            currentReservationBackup[i] = clientPtr->reservedEntriesArray[i];

        /* temporary set requested entries to the currently reserved entries */
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
            clientPtr->reservedEntriesArray[i] = requestedEntriesPerType[i];

        /* check the number of entries needed to hold the updated reservation */
        rc = tcamEntriesNeededForAllocatedAndReservedEntries(tcamManagerPtr,&totalEntriesNeeded);

        /* regardless of the result restore the client reservation state */
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
            clientPtr->reservedEntriesArray[i] = currentReservationBackup[i];

        /* now check function return code */
        if (rc != GT_OK)
            return rc;

        /* check the TCAM manager has enough entries */
        if (totalEntriesNeeded > tcamManagerPtr->tcamEntries)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }
    else /* can't happen */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    /* reservation was successfull, if the reservation type isn't already locked,
       lock it now */
    clientPtr->reservationType = reservationType;
    if (clientPtr->isReservationTypeLocked == GT_FALSE)
        clientPtr->isReservationTypeLocked = GT_TRUE;

    /* save the reservation information */
    clientPtr->reservationRange = *reservationRangePtr;
    for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        clientPtr->reservedEntriesArray[i] = requestedEntriesPerType[i];

    return rc;
}

/*******************************************************************************
* cpssDxChTcamManagerEntriesReservationSet
*
* DESCRIPTION:
*       Reserves entries for a client in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       reservationType       - type of the reservation
*       requestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*       reservationRangePtr   - the TCAM range allocated for this reservation;
*                               relevant only for static reservation type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - on not registered clientId
*       GT_ALREADY_EXIST      - on static reservation with range that collides
*                               with other already existing static reservations
*       GT_NO_RESOURCE        - the reservation can't fit in the TCAM
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. The TCAM manager will save enough space so that at any given time,
*          the client will have enough space for up to the number of reserved
*          entries.
*       2. In case of static entries reservation, the TCAM range allocated to
*          the static reservation must be within the range allocated to the
*          TCAM manager. The requested range can't contain entries reserved for
*          other static reservation.
*       3. Each client can have only one reservation. New reservation for a
*          client will replace the previous reservation.
*       4. Elements in the requestedEntriesArray must refer to unique entries
*          types. Meaning that no two array elements can have same entry type.
*          In addition the amount of requested entries can't be 0.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerEntriesReservationSet
(
    IN  GT_VOID                                             *tcamManagerHandlerPtr,
    IN  GT_U32                                              clientId,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT reservationType,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             requestedEntriesArray[],
    IN  GT_U32                                              sizeOfArray,
    IN  CPSS_DXCH_TCAM_MANAGER_RANGE_STC                    *reservationRangePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerEntriesReservationSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, reservationType, requestedEntriesArray, sizeOfArray, reservationRangePtr));

    rc = internal_cpssDxChTcamManagerEntriesReservationSet(tcamManagerHandlerPtr, clientId, reservationType, requestedEntriesArray, sizeOfArray, reservationRangePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, reservationType, requestedEntriesArray, sizeOfArray, reservationRangePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerEntriesReservationGet
*
* DESCRIPTION:
*       Gets number of reserved entries for a client in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       sizeOfArrayPtr        - points to the allocated size of the array
*
* OUTPUTS:
*       reservationTypePtr    - points to reservation type
*       reservedEntriesArray  - array of amount of entries reserved to client
*       sizeOfArrayPtr        - points to number of elements filled in the array
*       reservationRangePtr   - points to the reservation range (relevant only
*                               for static entries reservation)
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerEntriesReservationGet
(
    IN      GT_VOID                                             *tcamManagerHandlerPtr,
    IN      GT_U32                                              clientId,
    OUT     CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT *reservationTypePtr,
    OUT     CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             reservedEntriesArray[],
    INOUT   GT_U32                                              *sizeOfArrayPtr,
    OUT     CPSS_DXCH_TCAM_MANAGER_RANGE_STC                    *reservationRangePtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC   *clientPtr = NULL;
    GT_U32                                  i;
    GT_U32                                  arraySize;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);
    CPSS_NULL_PTR_CHECK_MAC(reservationTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(reservedEntriesArray);
    CPSS_NULL_PTR_CHECK_MAC(sizeOfArrayPtr);
    if (*sizeOfArrayPtr == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    CPSS_NULL_PTR_CHECK_MAC(reservationRangePtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /* fill in the reservation information */
    *reservationTypePtr = clientPtr->reservationType;

    arraySize = 0;
    for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
    {
        if (clientPtr->reservedEntriesArray[i] > 0)
        {
            if (arraySize < *sizeOfArrayPtr)
            {
                reservedEntriesArray[arraySize].entryType = arrayIndexToEntryType(i);
                reservedEntriesArray[arraySize].amount    = clientPtr->reservedEntriesArray[i];
                arraySize++;
            }
            else
            {
                /* the reservedEntriesArray is not big enough */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }
    *sizeOfArrayPtr = arraySize;

    if (clientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
    {
        reservationRangePtr->firstLine = clientPtr->reservationRange.firstLine;
        reservationRangePtr->lastLine  = clientPtr->reservationRange.lastLine;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntriesReservationGet
*
* DESCRIPTION:
*       Gets number of reserved entries for a client in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       sizeOfArrayPtr        - points to the allocated size of the array
*
* OUTPUTS:
*       reservationTypePtr    - points to reservation type
*       reservedEntriesArray  - array of amount of entries reserved to client
*       sizeOfArrayPtr        - points to number of elements filled in the array
*       reservationRangePtr   - points to the reservation range (relevant only
*                               for static entries reservation)
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerEntriesReservationGet
(
    IN      GT_VOID                                             *tcamManagerHandlerPtr,
    IN      GT_U32                                              clientId,
    OUT     CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT *reservationTypePtr,
    OUT     CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             reservedEntriesArray[],
    INOUT   GT_U32                                              *sizeOfArrayPtr,
    OUT     CPSS_DXCH_TCAM_MANAGER_RANGE_STC                    *reservationRangePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerEntriesReservationGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, reservationTypePtr, reservedEntriesArray, sizeOfArrayPtr, reservationRangePtr));

    rc = internal_cpssDxChTcamManagerEntriesReservationGet(tcamManagerHandlerPtr, clientId, reservationTypePtr, reservedEntriesArray, sizeOfArrayPtr, reservationRangePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, reservationTypePtr, reservedEntriesArray, sizeOfArrayPtr, reservationRangePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerUsageGet
*
* DESCRIPTION:
*       Returns the current number of entries from each type used by all
*       clients in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       usageOfSpecificClient - GT_TRUE:  get usage of specific client
*                               GT_FALSE: get commulative usage of all clients
*       clientId              - the client Id
*                               relevant if usageOfSpecificClient is GT_TRUE
*       sizeOfReservedArrayPtr    - points to the allocated size of the
*                                   reserved entries array
*       sizeofAllocatedArrayPtr   - points to the allocated size of the
*                                   allocated entries array
*
* OUTPUTS:
*       reservedEntriesArray      - array of amount of entries reserved
*       sizeOfReservedArrayPtr    - points to the allocated size of the
*                                   reserved entries array
*       allocatedEntriesArray     - array of amount of entries allocated
*       sizeofAllocatedArrayPtr   - points to the allocated size of the
*                                   allocated entries array
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerUsageGet
(
    IN      GT_VOID                                 *tcamManagerHandlerPtr,
    IN      GT_BOOL                                 usageOfSpecificClient,
    IN      GT_U32                                  clientId,
    OUT     CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC reservedEntriesArray[],
    INOUT   GT_U32                                  *sizeOfReservedArrayPtr,
    OUT     CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC allocatedEntriesArray[],
    INOUT   GT_U32                                  *sizeofAllocatedArrayPtr
)
{
    NOT_USED_MAC(tcamManagerHandlerPtr)
    NOT_USED_MAC(usageOfSpecificClient)
    NOT_USED_MAC(clientId)
    NOT_USED_MAC(reservedEntriesArray)
    NOT_USED_MAC(sizeOfReservedArrayPtr)
    NOT_USED_MAC(allocatedEntriesArray)
    NOT_USED_MAC(sizeofAllocatedArrayPtr)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerUsageGet
*
* DESCRIPTION:
*       Returns the current number of entries from each type used by all
*       clients in the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       usageOfSpecificClient - GT_TRUE:  get usage of specific client
*                               GT_FALSE: get commulative usage of all clients
*       clientId              - the client Id
*                               relevant if usageOfSpecificClient is GT_TRUE
*       sizeOfReservedArrayPtr    - points to the allocated size of the
*                                   reserved entries array
*       sizeofAllocatedArrayPtr   - points to the allocated size of the
*                                   allocated entries array
*
* OUTPUTS:
*       reservedEntriesArray      - array of amount of entries reserved
*       sizeOfReservedArrayPtr    - points to the allocated size of the
*                                   reserved entries array
*       allocatedEntriesArray     - array of amount of entries allocated
*       sizeofAllocatedArrayPtr   - points to the allocated size of the
*                                   allocated entries array
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerUsageGet
(
    IN      GT_VOID                                 *tcamManagerHandlerPtr,
    IN      GT_BOOL                                 usageOfSpecificClient,
    IN      GT_U32                                  clientId,
    OUT     CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC reservedEntriesArray[],
    INOUT   GT_U32                                  *sizeOfReservedArrayPtr,
    OUT     CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC allocatedEntriesArray[],
    INOUT   GT_U32                                  *sizeofAllocatedArrayPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerUsageGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, usageOfSpecificClient, clientId, reservedEntriesArray, sizeOfReservedArrayPtr, allocatedEntriesArray, sizeofAllocatedArrayPtr));

    rc = internal_cpssDxChTcamManagerUsageGet(tcamManagerHandlerPtr, usageOfSpecificClient, clientId, reservedEntriesArray, sizeOfReservedArrayPtr, allocatedEntriesArray, sizeofAllocatedArrayPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, usageOfSpecificClient, clientId, reservedEntriesArray, sizeOfReservedArrayPtr, allocatedEntriesArray, sizeofAllocatedArrayPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerDump
*
* DESCRIPTION:
*       Dumps the state of the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       dumpType              - type of dump
*       clientId              - the client Id; relevant only when dump
*                               type is  CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerDump
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT    dumpType,
    IN  GT_U32                                  clientId
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    GT_U32                                      i;
    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entryPtr;
    CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    entryLocation;
    GT_STATUS                                   rc = GT_OK;
    GT_U32                                      entryToken;
    GT_U32                                      entriesUsageArraySize;
    GT_U32                                      nextFreeToken;
    GT_U32                                      numOfUsedEntries;
    GT_U32                                      numOfUnusedEntries;
    GT_U32                                      freeTokenListSize;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_DUMP_TYPE_CHECK_MAC(dumpType);
    if (dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E)
        PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    if (dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E)
    {
        clientPtr = &tcamManagerPtr->clientsArray[clientId];
        PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);
    }

    /**********************************/
    /* dump list of clients if needed */
    /**********************************/
    if ((dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_E) ||
        (dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_AND_ENTRIES_FOR_ALL_CLIENTS_E))
    {
        cpssOsPrintf("\nClients list:\n");
        for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
        {
            clientPtr = &tcamManagerPtr->clientsArray[i];
            if (clientPtr->isRegistered == GT_FALSE)
                continue;
            cpssOsPrintf("  Id: %2d, Name: %s\n", i, clientPtr->name);
        }
    }

    /*******************************************/
    /* dump entries for all or specific client */
    /*******************************************/
    if ((dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E) ||
        (dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_ALL_CLIENTS_E) ||
        (dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_CLIENTS_AND_ENTRIES_FOR_ALL_CLIENTS_E))
    {
        /* print header */
        if (dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E)
        {
            clientPtr = &tcamManagerPtr->clientsArray[clientId];
            cpssOsPrintf("\nAllocated entries of client %d (%s):\n", clientId, clientPtr->name);
        }
        else
        {
            cpssOsPrintf("\nAllocated entries:\n");
        }

        /* scan all entries */
        entryPtr = &tcamManagerPtr->entriesInfoArray[0];
        for (i = 0 ; i < tcamManagerPtr->tcamEntries ; i++, entryPtr++)
        {
            /* skip unused entries */
            if (entryPtr->isUsed == GT_FALSE)
                continue;
            /* in case of dump entries for specific client only, skip entries
               not allocated by that client */
            if ((dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E) && (entryPtr->clientId != clientId))
                continue;

            /* get TCAM location */
            rc = convertIndexToToken(tcamManagerPtr,i,&entryToken);
            if (rc != GT_OK)
                return rc;
            rc = cpssDxChTcamManagerTokenToTcamLocation(tcamManagerHandlerPtr,entryPtr->clientId,entryToken,&entryLocation);
            if (rc != GT_OK)
                return rc;

            /* print out the data */
            cpssOsPrintf("[(%4d,%2d) %7s (id: %2d)  ]\n",entryLocation.row,entryLocation.column,entryTypeNameGet(entryPtr->type),entryPtr->clientId);
        }
    }

    /**************************/
    /* dump all internal info */
    /**************************/
    if (dumpType == CPSS_DXCH_TCAM_MANAGER_DUMP_INTERNAL_INFO_E)
    {
        cpssOsPrintf("\nTcam manager object:\n");
        cpssOsPrintf("  tcam type: %d\n",tcamManagerPtr->tcamType);
        cpssOsPrintf("  tcam range: [%d - %d]\n",tcamManagerPtr->tcamRange.firstLine,tcamManagerPtr->tcamRange.lastLine);
        cpssOsPrintf("  tcam line: %d\n",tcamManagerPtr->tcamLines);
        cpssOsPrintf("  tcam entries: %d\n",tcamManagerPtr->tcamEntries);

        cpssOsPrintf("\nValid clients list:\n");
        for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
        {
            clientPtr = &tcamManagerPtr->clientsArray[i];
            if (clientPtr->isRegistered == GT_FALSE)
                continue;
            cpssOsPrintf("  Client [%d]\n",i);
            cpssOsPrintf("    Name: %s\n",clientPtr->name);
            cpssOsPrintf("    Callback functions: moveToLocationFuncPtr 0x%08X, moveToAnywhereFuncPtr 0x%08X\n, checkIfDefaultLocationFuncPtr 0x%08X\n",clientPtr->callbackFunc.moveToLocationFuncPtr,clientPtr->callbackFunc.moveToAnywhereFuncPtr,clientPtr->callbackFunc.checkIfDefaultLocationFuncPtr);
            cpssOsPrintf("    Reservation type: %d\n",clientPtr->reservationType);
            cpssOsPrintf("    Reservation range: [%d - %d]\n",clientPtr->reservationRange.firstLine,clientPtr->reservationRange.lastLine);
            cpssOsPrintf("    Entries pool range:\n");
            cpssOsPrintf("      Single: [%04d - %04d]\n",clientPtr->entryPoolRangeArray[0].firstIndex,clientPtr->entryPoolRangeArray[0].lastIndex);
            cpssOsPrintf("      Dual:   [%04d - %04d]\n",clientPtr->entryPoolRangeArray[1].firstIndex,clientPtr->entryPoolRangeArray[1].lastIndex);
            cpssOsPrintf("      Triple: [%04d - %04d]\n",clientPtr->entryPoolRangeArray[2].firstIndex,clientPtr->entryPoolRangeArray[2].lastIndex);
            cpssOsPrintf("      Quad:   [%04d - %04d]\n",clientPtr->entryPoolRangeArray[3].firstIndex,clientPtr->entryPoolRangeArray[3].lastIndex);
            cpssOsPrintf("    Allocated/Reserved entries:\n");
            cpssOsPrintf("      Single: [%04d/%04d]\n",clientPtr->allocatedEntriesArray[0],clientPtr->reservedEntriesArray[0]);
            cpssOsPrintf("      Dual:   [%04d/%04d]\n",clientPtr->allocatedEntriesArray[1],clientPtr->reservedEntriesArray[1]);
            cpssOsPrintf("      Triple: [%04d/%04d]\n",clientPtr->allocatedEntriesArray[2],clientPtr->reservedEntriesArray[2]);
            cpssOsPrintf("      Quad:   [%04d/%04d]\n",clientPtr->allocatedEntriesArray[3],clientPtr->reservedEntriesArray[3]);
        }

        cpssOsPrintf("\nEntries Usage bitmap:\n");
        entriesUsageArraySize = (tcamManagerPtr->tcamEntries + 31) / 32;
        for (i = 0 ; i < entriesUsageArraySize ; i++)
        {
            cpssOsPrintf("  [%03d]: 0x%08X\n",i,tcamManagerPtr->entriesUsagePtr[i]);
        }
        cpssOsPrintf("\nEntries info array:\n");
        numOfUsedEntries = 0;
        numOfUnusedEntries = 0;
        for (i = 0 ; i < tcamManagerPtr->tcamEntries ; i++)
        {
            entryPtr = &tcamManagerPtr->entriesInfoArray[i];
            cpssOsPrintf("  [%04d]: used: %3s, type: %6s, clientId: %d, cookie: 0x%08X, token: %d\n",
                         i,(entryPtr->isUsed?"Yes":"No"),entryTypeNameGet(entryPtr->type),
                         entryPtr->clientId,entryPtr->clientCookiePtr,entryPtr->token);
            if (entryPtr->isUsed == GT_TRUE)
                numOfUsedEntries++;
            else
                numOfUnusedEntries++;
        }
        cpssOsPrintf("  -------------------------------\n");
        cpssOsPrintf("  Total used entries: %d\n",numOfUsedEntries);
        cpssOsPrintf("  Total unused entries: %d\n",numOfUnusedEntries);

        cpssOsPrintf("\nToken to Index array size: %d\n",tcamManagerPtr->tokenToIndexArraySize);
        cpssOsPrintf("Next free token: %d\n",tcamManagerPtr->nextFreeToken);
        cpssOsPrintf("\nToken to index array:\n");
        for (i = 0 ; i < tcamManagerPtr->tokenToIndexArraySize ; i++)
        {
            cpssOsPrintf("  [%04d]: %4d\n",i,tcamManagerPtr->tokenToIndexArray[i]);
        }

        cpssOsPrintf("\nToken free list:\n");
        nextFreeToken = tcamManagerPtr->nextFreeToken;
        freeTokenListSize = 0;
        cpssOsPrintf("  %d\n",nextFreeToken);
        while ((nextFreeToken != PRV_TCAM_MANAGER_NO_MORE_TOKENS_CNS) &&
               (++freeTokenListSize < tcamManagerPtr->tokenToIndexArraySize))
        {
            nextFreeToken = tcamManagerPtr->tokenToIndexArray[nextFreeToken];
            if (nextFreeToken != PRV_TCAM_MANAGER_NO_MORE_TOKENS_CNS)
                nextFreeToken -= PRV_TCAM_MANAGER_IN_FREE_TOKEN_LIST_CNS;
            cpssOsPrintf("  %d\n",nextFreeToken);
        }
        cpssOsPrintf("  ----------\n",nextFreeToken);
        cpssOsPrintf("  Free Token list size: %d\n",freeTokenListSize);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerDump
*
* DESCRIPTION:
*       Dumps the state of the TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       dumpType              - type of dump
*       clientId              - the client Id; relevant only when dump
*                               type is  CPSS_DXCH_TCAM_MANAGER_DUMP_ENTRIES_FOR_SPECIFIC_CLIENT_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - the clientId is not registered
*       GT_FAIL               - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerDump
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  CPSS_DXCH_TCAM_MANAGER_DUMP_TYPE_ENT    dumpType,
    IN  GT_U32                                  clientId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerDump);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, dumpType, clientId));

    rc = internal_cpssDxChTcamManagerDump(tcamManagerHandlerPtr, dumpType, clientId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, dumpType, clientId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerEntryAllocate
*
* DESCRIPTION:
*       Allocates TCAM entry for client. The entry is allocated in a range
*       bounded by two other entries (bounded in the meaning of TCAM search
*       order). The entry is allocated according to specific allocation method.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryType             - entry type to allocate
*       lowerBoundToken       - token representing the lower boundary; the allocated
*                               entry will reside after this token (in the meaning
*                               of TCAM search order); if 0 then no lower boundary
*       upperBoundToken       - toekn representing the upper boundary; the allocated
*                               entry will reside before this token (in the meaning
*                               of TCAM search order); if 0 then no upper boundary
*       allocMethod           - the allocation method
*       defragEnable          - whether to defrag entries in case there is no
*                               free place for this entry cause of entries
*                               fragmentation;
*       clientCookiePtr       - client cookie; used in client callback functions
*
* OUTPUTS:
*       allocEntryTokenPtr    - token representing the allocated entry
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerEntryAllocate
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  GT_U32                                  clientId,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType,
    IN  GT_U32                                  lowerBoundToken,
    IN  GT_U32                                  upperBoundToken,
    IN  CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                 defragEnable,
    IN  GT_VOID                                 *clientCookiePtr,
    OUT GT_U32                                  *allocEntryTokenPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    GT_STATUS                                   rc = GT_OK;
    GT_U32                                      lowerBoundIndex;
    GT_U32                                      upperBoundIndex;
    GT_U32                                      lowerBoundLineIndex;
    GT_U32                                      upperBoundLineIndex;
    GT_U32                                      searchLineIndex;
    GT_U32                                      lineToFree;
    GT_U32                                      midAllocLower = 0;
    GT_U32                                      midAllocUpper = 0;
    GT_U32                                      currentLineUsage;
    GT_COMP_RES                                 compareResult;

    GT_U32                                      freeIndexesInLine[4];
    GT_U32                                      numOfFreeIndexesInLine;
    GT_U32                                      allocatedIndex = 0;
    GT_BOOL                                     candidateToFreeFound = GT_FALSE;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);
    PRV_TCAM_MANAGER_ENTRY_TYPE_CHECK_MAC(entryType);
    PRV_TCAM_MANAGER_ALLOC_METHOD_CHECK_MAC(allocMethod);
    CPSS_NULL_PTR_CHECK_MAC(allocEntryTokenPtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /**********************************************************************/
    /* If client reservation type wasn't determined yet (meaning that the */
    /* client didn't set reservation after registration), then the first  */
    /* allocation determine the client reservation type to dynamic        */
    /**********************************************************************/

    if (clientPtr->isReservationTypeLocked == GT_FALSE)
    {
        clientPtr->reservationType         = CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E;
        clientPtr->isReservationTypeLocked = GT_TRUE;
    }

    /*********************************/
    /* prepare the search parameters */
    /*********************************/

    rc = entryAllocatePrepareSearchParameters(tcamManagerPtr,clientPtr,entryType,
                                              lowerBoundToken,upperBoundToken,
                                              allocMethod,
                                              &lowerBoundIndex,&upperBoundIndex,
                                              &lowerBoundLineIndex,&upperBoundLineIndex,
                                              &searchLineIndex,
                                              &midAllocLower,&midAllocUpper);
    if (rc != GT_OK)
        return rc;

    lineToFree = searchLineIndex;
    numOfFreeIndexesInLine = 0;

    /*******************************************************/
    /* start scan lines one after the other for free space */
    /*******************************************************/

    while ((lowerBoundLineIndex <= searchLineIndex) && (searchLineIndex <= upperBoundLineIndex))
    {
        /* get the current line entries usage                  */
        /* remember each word in line usage array holds usage  */
        /* of 8 lines and each line is represented with 4 bits */
        currentLineUsage = (tcamManagerPtr->entriesUsagePtr[searchLineIndex / 32] >> (searchLineIndex % 32)) & 0xF;

        /* in case the boundary index is in the middle of the line mark the rest of the line as in use */
        if (lowerBoundIndex > searchLineIndex)
            currentLineUsage |= (0xF >> (4 + searchLineIndex - lowerBoundIndex));
        if (upperBoundIndex < (searchLineIndex + 4))
            currentLineUsage |= (0xF << (upperBoundIndex - searchLineIndex + entryTypeSizeGet(entryType)) & 0xF);

        /* check if current line has space for the required entry type */
        if (entryType == CPSS_DXCH_TCAM_MANAGER_SINGLE_ENTRY_E)
        {
            if ((currentLineUsage & 0x1) == 0)
                freeIndexesInLine[numOfFreeIndexesInLine++] = searchLineIndex;
            if ((currentLineUsage & 0x2) == 0)
                freeIndexesInLine[numOfFreeIndexesInLine++] = searchLineIndex + 1;
            if ((currentLineUsage & 0x4) == 0)
                freeIndexesInLine[numOfFreeIndexesInLine++] = searchLineIndex + 2;
            if ((currentLineUsage & 0x8) == 0)
                freeIndexesInLine[numOfFreeIndexesInLine++] = searchLineIndex + 3;
        }
        else if (entryType == CPSS_DXCH_TCAM_MANAGER_DUAL_ENTRY_E)
        {
            if ((currentLineUsage & 0x3) == 0)
                freeIndexesInLine[numOfFreeIndexesInLine++] = searchLineIndex;
            if ((currentLineUsage & 0xC) == 0)
                freeIndexesInLine[numOfFreeIndexesInLine++] = searchLineIndex + 2;
        }
        else if (entryType == CPSS_DXCH_TCAM_MANAGER_TRIPLE_ENTRY_E)
        {
            if ((currentLineUsage & 0x7) == 0)
                freeIndexesInLine[numOfFreeIndexesInLine++] = searchLineIndex;
        }
        else if (entryType == CPSS_DXCH_TCAM_MANAGER_QUAD_ENTRY_E)
        {
            if ((currentLineUsage & 0xF) == 0)
                freeIndexesInLine[numOfFreeIndexesInLine++] = searchLineIndex;
        }
        else /* can't happen */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

        /* currently scanned line has free space for the requested entry type */
        if (numOfFreeIndexesInLine > 0)
            break;

        /* line doesn't have empty space, check if it is better candidate
           for free in case of defragmentation enabled */
        if (defragEnable == GT_TRUE)
        {
            rc = compareCandidatesForFree(tcamManagerPtr,searchLineIndex,candidateToFreeFound,lineToFree,entryType,&compareResult);
            if (rc != GT_OK)
                return rc;
            if (compareResult == GT_GREATER)
            {
                lineToFree = searchLineIndex;
                candidateToFreeFound = GT_TRUE;
            }
        }

        /* free line wasn't found, get the next line according to the search method */
        rc = entryAllocateGetNextSearchLine(tcamManagerPtr,clientPtr,
                                            lowerBoundIndex,upperBoundIndex,
                                            allocMethod,&searchLineIndex,
                                            &midAllocLower,&midAllocUpper);
        /* check if there is no more lines within range */
        if (rc == GT_OUT_OF_PP_MEM)
            break;
        /* check there was no other error */
        if (rc != GT_OK)
            return rc;
    }

    /*******************************************/
    /* scan was completed, analyze the results */
    /*******************************************/

    /* check if a free line was found */
    if (numOfFreeIndexesInLine > 0)
    {
        if (allocMethod == CPSS_DXCH_TCAM_MANAGER_ALLOC_MIN_E)
            allocatedIndex = freeIndexesInLine[0];
        else if (allocMethod == CPSS_DXCH_TCAM_MANAGER_ALLOC_MID_E)
            allocatedIndex = freeIndexesInLine[(numOfFreeIndexesInLine - 1) / 2];
        else if (allocMethod == CPSS_DXCH_TCAM_MANAGER_ALLOC_MAX_E)
            allocatedIndex = freeIndexesInLine[numOfFreeIndexesInLine-1];
        else /* can't happen */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    else
    {
        /**************************/
        /* no free line was found */
        /**************************/

        if (defragEnable == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        }
        else /* defragEnable == GT_TRUE */
        {
            /* if no candidate to free was found, there is no space even with defrag */
            if (candidateToFreeFound == GT_FALSE)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);

            /* free the line that was choosen as line to free */
            rc = freeIndex(tcamManagerPtr,entryType,lineToFree);
            if (rc != GT_OK)
                return rc;

            /* now the new allocated line is the line that was just freed */
            allocatedIndex = lineToFree;
        }
    }

    /* get the next available token to associate with this entry, note that
       nubmer of tokens is equal or larger than tcamEntries so if there is free
       entry then there must be free token too */
    if (tcamManagerPtr->nextFreeToken == PRV_TCAM_MANAGER_NO_MORE_TOKENS_CNS)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    *allocEntryTokenPtr = tcamManagerPtr->nextFreeToken;
    tcamManagerPtr->nextFreeToken = tcamManagerPtr->tokenToIndexArray[tcamManagerPtr->nextFreeToken] - PRV_TCAM_MANAGER_IN_FREE_TOKEN_LIST_CNS;

    /* mark the allocated line as used and update the entries bitmap as well */
    rc = setEntryUsage(tcamManagerPtr,allocatedIndex,entryType,GT_TRUE,clientId,clientCookiePtr,*allocEntryTokenPtr);
    if (rc != GT_OK)
    {
        /* return allocated token to tokens free list */
        tcamManagerPtr->nextFreeToken = *allocEntryTokenPtr;
        return rc;
    }

    tcamManagerPtr->tokenToIndexArray[*allocEntryTokenPtr] = allocatedIndex;

    /* increase the number of allocated entries for this client */
    clientPtr->allocatedEntriesArray[entryTypeToArrayIndex(entryType)]++;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntryAllocate
*
* DESCRIPTION:
*       Allocates TCAM entry for client. The entry is allocated in a range
*       bounded by two other entries (bounded in the meaning of TCAM search
*       order). The entry is allocated according to specific allocation method.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryType             - entry type to allocate
*       lowerBoundToken       - token representing the lower boundary; the allocated
*                               entry will reside after this token (in the meaning
*                               of TCAM search order); if 0 then no lower boundary
*       upperBoundToken       - toekn representing the upper boundary; the allocated
*                               entry will reside before this token (in the meaning
*                               of TCAM search order); if 0 then no upper boundary
*       allocMethod           - the allocation method
*       defragEnable          - whether to defrag entries in case there is no
*                               free place for this entry cause of entries
*                               fragmentation;
*       clientCookiePtr       - client cookie; used in client callback functions
*
* OUTPUTS:
*       allocEntryTokenPtr    - token representing the allocated entry
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerEntryAllocate
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  GT_U32                                  clientId,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT   entryType,
    IN  GT_U32                                  lowerBoundToken,
    IN  GT_U32                                  upperBoundToken,
    IN  CPSS_DXCH_TCAM_MANAGER_ALLOC_METHOD_ENT allocMethod,
    IN  GT_BOOL                                 defragEnable,
    IN  GT_VOID                                 *clientCookiePtr,
    OUT GT_U32                                  *allocEntryTokenPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerEntryAllocate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, entryType, lowerBoundToken, upperBoundToken, allocMethod, defragEnable, clientCookiePtr, allocEntryTokenPtr));

    rc = internal_cpssDxChTcamManagerEntryAllocate(tcamManagerHandlerPtr, clientId, entryType, lowerBoundToken, upperBoundToken, allocMethod, defragEnable, clientCookiePtr, allocEntryTokenPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, entryType, lowerBoundToken, upperBoundToken, allocMethod, defragEnable, clientCookiePtr, allocEntryTokenPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerEntryClientCookieUpdate
*
* DESCRIPTION:
*       Updates client cookie for specific entry token.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry to be updated
*       clientCookiePtr       - new client cookie; used in client callback functions
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerEntryClientCookieUpdate
(
    IN  GT_VOID             *tcamManagerHandlerPtr,
    IN  GT_U32              clientId,
    IN  GT_U32              entryToken,
    IN  GT_VOID             *clientCookiePtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entryInfoPtr = NULL;
    GT_STATUS                                   rc;
    GT_U32                                      tokenIndex;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /* convert tokens to indexes */
    rc = convertTokenToIndex(tcamManagerHandlerPtr,entryToken,&tokenIndex);
    if (rc != GT_OK)
        return rc;

    entryInfoPtr = &tcamManagerPtr->entriesInfoArray[tokenIndex];

    if ((entryInfoPtr->isUsed == GT_FALSE) || (entryInfoPtr->clientId != clientId))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    entryInfoPtr->clientCookiePtr = clientCookiePtr;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntryClientCookieUpdate
*
* DESCRIPTION:
*       Updates client cookie for specific entry token.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry to be updated
*       clientCookiePtr       - new client cookie; used in client callback functions
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerEntryClientCookieUpdate
(
    IN  GT_VOID             *tcamManagerHandlerPtr,
    IN  GT_U32              clientId,
    IN  GT_U32              entryToken,
    IN  GT_VOID             *clientCookiePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerEntryClientCookieUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, entryToken, clientCookiePtr));

    rc = internal_cpssDxChTcamManagerEntryClientCookieUpdate(tcamManagerHandlerPtr, clientId, entryToken, clientCookiePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, entryToken, clientCookiePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerEntryFree
*
* DESCRIPTION:
*       Frees entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The entry must be previously allocated by this client.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerEntryFree
(
    IN  GT_VOID             *tcamManagerHandlerPtr,
    IN  GT_U32              clientId,
    IN  GT_U32              entryToken
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entryInfoPtr = NULL;
    GT_STATUS                                   rc;
    GT_U32                                      tokenIndex;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT       entryType;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /* convert tokens to indexes */
    rc = convertTokenToIndex(tcamManagerHandlerPtr,entryToken,&tokenIndex);
    if (rc != GT_OK)
        return rc;

    entryInfoPtr = &tcamManagerPtr->entriesInfoArray[tokenIndex];
    entryType    = entryInfoPtr->type;

    if ((entryInfoPtr->isUsed == GT_FALSE) || (entryInfoPtr->clientId != clientId))
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    rc = setEntryUsage(tcamManagerPtr,tokenIndex,entryInfoPtr->type,GT_FALSE,clientId,NULL,entryToken);
    if (rc != GT_OK)
        return rc;

    /* free the token and add it to token free list */
    tcamManagerPtr->tokenToIndexArray[entryToken] = tcamManagerPtr->nextFreeToken + PRV_TCAM_MANAGER_IN_FREE_TOKEN_LIST_CNS;
    tcamManagerPtr->nextFreeToken = entryToken;

    /* decrease the number of allocated entries for this client */
    clientPtr->allocatedEntriesArray[entryTypeToArrayIndex(entryType)]--;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerEntryFree
*
* DESCRIPTION:
*       Frees entry.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The entry must be previously allocated by this client.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerEntryFree
(
    IN  GT_VOID             *tcamManagerHandlerPtr,
    IN  GT_U32              clientId,
    IN  GT_U32              entryToken
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerEntryFree);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, entryToken));

    rc = internal_cpssDxChTcamManagerEntryFree(tcamManagerHandlerPtr, clientId, entryToken);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, entryToken));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerAvailableEntriesCheck
*
* DESCRIPTION:
*       Checks whether the TCAM manager has enough available entries for the
*       requested amount of entries.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       requestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - on not registered clientId
*       GT_NO_RESOURCE        - if there is not enough space in the TCAM manager
*                               for the requested entries
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. The number of reserved entries of the specific client is also taken
*          into calculations.
*       2. Elements in the requestedEntriesArray must refer to unique entries
*          types. Meaning that no two array elements can have same entry type.
*          In addition the amount of requested entries can't be 0.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerAvailableEntriesCheck
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  GT_U32                                  clientId,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC requestedEntriesArray[],
    IN  GT_U32                                  sizeOfArray
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    GT_STATUS                                   rc = GT_OK;

    GT_U32  requestedEntriesPerType[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];
    GT_U32  currentAllocationBackup[PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS];
    GT_U32  totalEntriesNeeded;
    GT_U32  arrayIndex;
    GT_U32  i;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);
    CPSS_NULL_PTR_CHECK_MAC(requestedEntriesArray);
    if (sizeOfArray == 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /* convert requestedEntriesArray to ordered requestedEntriesPerType;
       verify that requested entries are non zero and unique */
    for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        requestedEntriesPerType[i] = 0;
    for (i = 0 ; i < sizeOfArray; i++)
    {
        PRV_TCAM_MANAGER_ENTRY_TYPE_CHECK_MAC(requestedEntriesArray[i].entryType);
        if (requestedEntriesArray[i].amount == 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        arrayIndex = entryTypeToArrayIndex(requestedEntriesArray[i].entryType);
        if (requestedEntriesPerType[arrayIndex] != 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        else
            requestedEntriesPerType[arrayIndex] = requestedEntriesArray[i].amount;
    }

    /*************************/
    /* check available space */
    /*************************/

    /* for static reservation all that is needed is to verify that there are
       enough reserved entries */
    if (clientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
    {
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
        {
            if ((clientPtr->allocatedEntriesArray[i] + requestedEntriesPerType[i]) > clientPtr->reservedEntriesArray[i])
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
        }
    }
    /* for dynamic reservation, temporary increase the number of entries and
       verify the TCAM manager has enough entries for all allocated/reserved
       entries needed */
    else if (clientPtr->reservationType == CPSS_DXCH_TCAM_MANAGER_DYNAMIC_ENTRIES_RESERVATION_E)
    {
        /* backup the current allocation state of the client */
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
            currentAllocationBackup[i] = clientPtr->allocatedEntriesArray[i];

        /* temporary add requested entries to the currently allocated entries */
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
            clientPtr->allocatedEntriesArray[i] += requestedEntriesPerType[i];

        /* check the number of entries needed to hold the updated allocation */
        rc = tcamEntriesNeededForAllocatedAndReservedEntries(tcamManagerPtr,&totalEntriesNeeded);

        /* regardless of the result restore the client allocation state */
        for (i = 0 ; i < PRV_TCAM_MANAGER_NUM_OF_ENTRY_TYPES_CNS ; i++)
            clientPtr->allocatedEntriesArray[i] = currentAllocationBackup[i];

        /* now check function return code */
        if (rc != GT_OK)
            return rc;

        /* check the TCAM manager has enough entries */
        if (totalEntriesNeeded > tcamManagerPtr->tcamEntries)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_PP_MEM, LOG_ERROR_NO_MSG);
    }
    else /* can't happen */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerAvailableEntriesCheck
*
* DESCRIPTION:
*       Checks whether the TCAM manager has enough available entries for the
*       requested amount of entries.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       requestedEntriesArray - array of amount of entries requested by client
*       sizeOfArray           - number of elemenets in the array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PARAM          - on wrong parameter
*       GT_BAD_PTR            - on NULL pointer
*       GT_NO_SUCH            - on not registered clientId
*       GT_NO_RESOURCE        - if there is not enough space in the TCAM manager
*                               for the requested entries
*       GT_FAIL               - on error
*
* COMMENTS:
*       1. The number of reserved entries of the specific client is also taken
*          into calculations.
*       2. Elements in the requestedEntriesArray must refer to unique entries
*          types. Meaning that no two array elements can have same entry type.
*          In addition the amount of requested entries can't be 0.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerAvailableEntriesCheck
(
    IN  GT_VOID                                 *tcamManagerHandlerPtr,
    IN  GT_U32                                  clientId,
    IN  CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC requestedEntriesArray[],
    IN  GT_U32                                  sizeOfArray
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerAvailableEntriesCheck);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, requestedEntriesArray, sizeOfArray));

    rc = internal_cpssDxChTcamManagerAvailableEntriesCheck(tcamManagerHandlerPtr, clientId, requestedEntriesArray, sizeOfArray);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, requestedEntriesArray, sizeOfArray));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerTokenToTcamLocation
*
* DESCRIPTION:
*       Converts entry token to hardware TCAM location.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry
*
* OUTPUTS:
*       tcamLocationPtr       - points to the hardware TCAM location
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The entry must be previously allocated by the same client.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerTokenToTcamLocation
(
    IN  GT_VOID                                     *tcamManagerHandlerPtr,
    IN  GT_U32                                      clientId,
    IN  GT_U32                                      entryToken,
    OUT CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *tcamLocationPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC   *clientPtr = NULL;
    GT_STATUS                               rc;
    GT_U32                                  tokenIndex;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);
    CPSS_NULL_PTR_CHECK_MAC(tcamLocationPtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /* convert tokens to indexes */
    rc = convertTokenToIndex(tcamManagerHandlerPtr,entryToken,&tokenIndex);
    if (rc != GT_OK)
        return rc;

    tcamLocationPtr->row    = (tokenIndex / 4) + tcamManagerPtr->tcamRange.firstLine;
    tcamLocationPtr->column = tokenIndex % 4;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerTokenToTcamLocation
*
* DESCRIPTION:
*       Converts entry token to hardware TCAM location.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry
*
* OUTPUTS:
*       tcamLocationPtr       - points to the hardware TCAM location
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The entry must be previously allocated by the same client.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerTokenToTcamLocation
(
    IN  GT_VOID                                     *tcamManagerHandlerPtr,
    IN  GT_U32                                      clientId,
    IN  GT_U32                                      entryToken,
    OUT CPSS_DXCH_TCAM_MANAGER_TCAM_LOCATION_STC    *tcamLocationPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerTokenToTcamLocation);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, entryToken, tcamLocationPtr));

    rc = internal_cpssDxChTcamManagerTokenToTcamLocation(tcamManagerHandlerPtr, clientId, entryToken, tcamLocationPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, entryToken, tcamLocationPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerTokenCompare
*
* DESCRIPTION:
*       Compares two tokens and returns the relative order (according to TCAM
*       order).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       firstToken            - first token to compare
*       secondToken           - second token to compare
*
* OUTPUTS:
*       resultPtr             - points to the compare result,
*                               GT_EQUAL    - tokens represent same tcam location
*                               GT_GREATER  - first token is after second token
*                                             according to TCAM search order
*                               GT_SMALLER  - first tokem is before second token
*                                             according to TCAM search order
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       The tokens must be previously allocated by the same client.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerTokenCompare
(
    IN  GT_VOID                                     *tcamManagerHandlerPtr,
    IN  GT_U32                                      clientId,
    IN  GT_U32                                      firstToken,
    IN  GT_U32                                      secondToken,
    OUT GT_COMP_RES                                 *resultPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC          *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC   *clientPtr = NULL;
    GT_STATUS                               rc;
    GT_U32                                  firstTokenIndex;
    GT_U32                                  secondTokenIndex;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);
    CPSS_NULL_PTR_CHECK_MAC(resultPtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /* convert tokens to indexes */
    rc = convertTokenToIndex(tcamManagerHandlerPtr,firstToken,&firstTokenIndex);
    if (rc != GT_OK)
        return rc;

    rc = convertTokenToIndex(tcamManagerHandlerPtr,secondToken,&secondTokenIndex);
    if (rc != GT_OK)
        return rc;

    /* compare the indexes */
    if (firstTokenIndex > secondTokenIndex)
        *resultPtr = GT_GREATER;
    else if (firstTokenIndex == secondTokenIndex)
        *resultPtr = GT_EQUAL;
    else
        *resultPtr = GT_SMALLER;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerTokenCompare
*
* DESCRIPTION:
*       Compares two tokens and returns the relative order (according to TCAM
*       order).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       firstToken            - first token to compare
*       secondToken           - second token to compare
*
* OUTPUTS:
*       resultPtr             - points to the compare result,
*                               GT_EQUAL    - tokens represent same tcam location
*                               GT_GREATER  - first token is after second token
*                                             according to TCAM search order
*                               GT_SMALLER  - first tokem is before second token
*                                             according to TCAM search order
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       The tokens must be previously allocated by the same client.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerTokenCompare
(
    IN  GT_VOID                                     *tcamManagerHandlerPtr,
    IN  GT_U32                                      clientId,
    IN  GT_U32                                      firstToken,
    IN  GT_U32                                      secondToken,
    OUT GT_COMP_RES                                 *resultPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerTokenCompare);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, firstToken, secondToken, resultPtr));

    rc = internal_cpssDxChTcamManagerTokenCompare(tcamManagerHandlerPtr, clientId, firstToken, secondToken, resultPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, firstToken, secondToken, resultPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerTokenInfoGet
*
* DESCRIPTION:
*       Returns token information.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry
*
* OUTPUTS:
*       entryTypePtr          - points to type of entry associated with the token
*       clientCookiePtrPtr    - points to pointer of cookie associated with the token
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The entry must be previously allocated by the same client.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerTokenInfoGet
(
    IN  GT_VOID                                     *tcamManagerHandlerPtr,
    IN  GT_U32                                      clientId,
    IN  GT_U32                                      entryToken,
    OUT CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT       *entryTypePtr,
    OUT GT_VOID                                     **clientCookiePtrPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entryInfoPtr = NULL;
    GT_STATUS                                   rc;
    GT_U32                                      tokenIndex;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    PRV_TCAM_MANAGER_CLIENT_ID_CHECK_MAC(clientId);
    CPSS_NULL_PTR_CHECK_MAC(entryTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(clientCookiePtrPtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    clientPtr = &tcamManagerPtr->clientsArray[clientId];
    PRV_TCAM_MANAGER_CLIENT_REGISTERED_CHECK_MAC(clientPtr);

    /* convert tokens to indexes */
    rc = convertTokenToIndex(tcamManagerHandlerPtr,entryToken,&tokenIndex);
    if (rc != GT_OK)
        return rc;

    entryInfoPtr = &tcamManagerPtr->entriesInfoArray[tokenIndex];

    if (entryInfoPtr->clientId != clientId)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    *entryTypePtr       = entryInfoPtr->type;
    *clientCookiePtrPtr = entryInfoPtr->clientCookiePtr;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTcamManagerTokenInfoGet
*
* DESCRIPTION:
*       Returns token information.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       clientId              - the client Id
*       entryToken            - token representing the entry
*
* OUTPUTS:
*       entryTypePtr          - points to type of entry associated with the token
*       clientCookiePtrPtr    - points to pointer of cookie associated with the token
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_OUT_OF_PP_MEM      - if there is no space left between the upper
*                               and lower bounds
*       GT_OUT_OF_RANGE       - if the indexes were out of range
*
* COMMENTS:
*       1. The entry must be previously allocated by the same client.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerTokenInfoGet
(
    IN  GT_VOID                                     *tcamManagerHandlerPtr,
    IN  GT_U32                                      clientId,
    IN  GT_U32                                      entryToken,
    OUT CPSS_DXCH_TCAM_MANAGER_ENTRY_TYPE_ENT       *entryTypePtr,
    OUT GT_VOID                                     **clientCookiePtrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerTokenInfoGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, clientId, entryToken, entryTypePtr, clientCookiePtrPtr));

    rc = internal_cpssDxChTcamManagerTokenInfoGet(tcamManagerHandlerPtr, clientId, entryToken, entryTypePtr, clientCookiePtrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, clientId, entryToken, entryTypePtr, clientCookiePtrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerHsuSizeGet
*
* DESCRIPTION:
*       Gets the memory size needed for TCAM manager HSU export information.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       sizePtr               - points to memory size needed for export data,
*                               including HSU data block header (calculated in bytes)
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - on wrong parameter
*       GT_FAIL               - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerHsuSizeGet
(
    IN  GT_VOID     *tcamManagerHandlerPtr,
    OUT GT_U32      *sizePtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC  *tcamManagerPtr = NULL;
    GT_STATUS                       rc = GT_OK;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(sizePtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    /* get size of all memories used by TCAM manager object */
    *sizePtr = 0;

    /* data block header */
    *sizePtr += sizeof(PRV_CPSS_HSU_DATA_BLOCK_HEADER_STC);

    /* size of PRV_CPSS_DXCH_TCAM_MANAGER_STC */
    *sizePtr += sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_STC);

    /* size of PRV_CPSS_DXCH_TCAM_MANAGER_STC.entriesInfoArray */
    *sizePtr += (tcamManagerPtr->tcamEntries * sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC));

    /* size of PRV_CPSS_DXCH_TCAM_MANAGER_STC.entriesUsagePtr */
    *sizePtr += (((tcamManagerPtr->tcamEntries + 31) / 32) * sizeof(GT_U32));

    /* size of PRV_CPSS_DXCH_TCAM_MANAGER_STC.tokenToIndexArray */
    *sizePtr += (tcamManagerPtr->tokenToIndexArraySize * sizeof(GT_U32));

    return rc;
}

/*******************************************************************************
* cpssDxChTcamManagerHsuSizeGet
*
* DESCRIPTION:
*       Gets the memory size needed for TCAM manager HSU export information.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       sizePtr               - points to memory size needed for export data,
*                               including HSU data block header (calculated in bytes)
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - on wrong parameter
*       GT_FAIL               - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerHsuSizeGet
(
    IN  GT_VOID     *tcamManagerHandlerPtr,
    OUT GT_U32      *sizePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerHsuSizeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, sizePtr));

    rc = internal_cpssDxChTcamManagerHsuSizeGet(tcamManagerHandlerPtr, sizePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, sizePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerHsuExport
*
* DESCRIPTION:
*       Exports TCAM manager HSU data.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       iteratorPtr           - points to iteration value; The iteration value
*                               is used by the TCAM manager to track the export
*                               progress.
*                               At first iteration set (*iteratorPtr) = 0
*                               At next iterations supply value returned by
*                               previous iteration.
*                               Relevant only when single operation export is not used
*                               (hsuBlockMemSizePtr != CPSS_HSU_SINGLE_ITERATION_CNS)
*       hsuBlockMemSizePtr    - points to the size of the memory block supplied
*                               for the current iteration (in bytes);
*                                - Size must be at least 1k bytes.
*                                - Use the value CPSS_HSU_SINGLE_ITERATION_CNS value
*                                  to indicate single iteration export. In such case
*                                  it is assumed that hsuBlockMemPtr is large enough
*                                  for the entire HSU data.
*       hsuBlockMemPtr        - points to memory block to hold HSU data in
*                               current iteration
*
* OUTPUTS:
*       iteratorPtr           - points to iteration value; The iteration value
*                               is used by the TCAM manager to track the export
*                               progress.
*                               Relevant only if exportCompletePtr == GT_FALSE
*       hsuBlockMemSizePtr    - points to the size of the memory block that was
*                               not used in the current iteration (in bytes)
*                               Relevant only when single iteration export is not used
*                               (input valus was not CPSS_HSU_SINGLE_ITERATION_CNS)
*       exportCompletePtr     - GT_TRUE:  HSU data export is completed.
*                               GT_FALSE: HSU data export is not completed.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - on wrong parameter
*       GT_FAIL               - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Several export processes can be performed simultaneously.
*       2. It is assumed that in case of multiple iterations export operation,
*          there are no calls to other TCAM manager APIs until export operation
*          is completed. Such calls may modify the data stored within the TCAM
*          manager and cause unexpected behavior.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerHsuExport
(
    IN    GT_VOID   *tcamManagerHandlerPtr,
    INOUT GT_UINTPTR *iteratorPtr,
    INOUT GT_U32    *hsuBlockMemSizePtr,
    IN    GT_U8     *hsuBlockMemPtr,
    OUT   GT_BOOL   *exportCompletePtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC      *tcamManagerPtr = NULL;
    PRV_CPSS_HSU_DATA_BLOCK_HEADER_STC  dataBlockHeader;
    GT_STATUS                           rc = GT_OK;
    GT_U32                              dataBlockSize = 0;

    /* number of bytes already exported from the current stage */
    GT_U32      stageMemCopied = 0;
    /* number of bytes that weren't exported yet in the current stage */
    GT_U32      stageMemLeft = 0;
    /* number of bytes to be exported in current stage in current iteration */
    GT_U32      stageMemToCopy;
    /* number of bytes already exported from the beginning of the current stage
       (also includes data already exported from next stages too) */
    GT_U32      memExported;
    /* current stage */
    GT_U32      stage;
    /* start address of data for each stage */
    GT_U8       *stageMemAddress[PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS];
    /* size in bytes of each stage */
    GT_U32      stageMemSize[PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS];

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(iteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemSizePtr);
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemPtr);
    CPSS_NULL_PTR_CHECK_MAC(exportCompletePtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    /* by default export is marked as not completed */
    *exportCompletePtr = GT_FALSE;

    /* prepare data block header */
    rc = cpssDxChTcamManagerHsuSizeGet(tcamManagerHandlerPtr,&dataBlockSize);
    if (rc != GT_OK)
        return rc;
    dataBlockHeader.type      = CPSS_HSU_DATA_TYPE_TCAM_MANAGER_E;
    dataBlockHeader.length    = dataBlockSize;
    dataBlockHeader.version   = 0;
    dataBlockHeader.reserved1 = 0;
    dataBlockHeader.reserved2 = 0;

    /* prepare stages information */
    stageMemAddress[0] = (GT_U8*)&dataBlockHeader;
    stageMemSize[0]    = sizeof(PRV_CPSS_HSU_DATA_BLOCK_HEADER_STC);

    stageMemAddress[1] = (GT_U8*)tcamManagerPtr;
    stageMemSize[1]    = sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_STC);

    stageMemAddress[2] = (GT_U8*)tcamManagerPtr->entriesInfoArray;
    stageMemSize[2]    = (tcamManagerPtr->tcamEntries * sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC));

    stageMemAddress[3] = (GT_U8*)tcamManagerPtr->entriesUsagePtr;
    stageMemSize[3]    = (((tcamManagerPtr->tcamEntries + 31) / 32) * sizeof(GT_U32));

    stageMemAddress[4] = (GT_U8*)tcamManagerPtr->tokenToIndexArray;
    stageMemSize[4]    = (tcamManagerPtr->tokenToIndexArraySize * sizeof(GT_U32));

    /* the iterator holds the number of data bytes already exported in the
       previous iterations and is updated during the export function upon
       exporting more data */

    /* in case of single iteration export, zero the iterator as no data
       was exported yet in this single iteration export */
    if (*hsuBlockMemSizePtr == CPSS_HSU_SINGLE_ITERATION_CNS)
        *iteratorPtr = 0;

    /* at first the iterator holds number of bytes already exported from the
       data block beginning */
    memExported = (GT_U32)(*iteratorPtr);

    /* the total number of bytes exported in previous iterations can't be equal
       or bigger than data block size; if it is, it indicates wrong iterator */
    if (memExported >= dataBlockSize)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    /* loop over all stages, pass stages already exported */
    for (stage = 0 ; stage < PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS ; stage++)
    {
        /* check if stage was fully exported already */
        if (memExported >= stageMemSize[stage])
        {
            memExported -= stageMemSize[stage];
            /* state where all data was already exported can't happen at this point */
            if (stage == (PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS-1))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            continue;
        }

        /* this stage was not fully exported */
        stageMemCopied = memExported;
        stageMemLeft   = stageMemSize[stage] - stageMemCopied;
        stageMemToCopy = (stageMemLeft < *hsuBlockMemSizePtr) ? stageMemLeft : *hsuBlockMemSizePtr;

        /* export stage data */
        cpssOsMemCpy(hsuBlockMemPtr,stageMemAddress[stage]+stageMemCopied,stageMemToCopy);

        /* update variables, note that memExported is set to 0 as there is no
           data that was already exported beyond this point */
        hsuBlockMemPtr      += stageMemToCopy;
        *hsuBlockMemSizePtr -= stageMemToCopy;
        *iteratorPtr        += stageMemToCopy;
        stageMemLeft        -= stageMemToCopy;
        memExported         = 0;

        /* check if this was the last stage and it was fully exported */
        if ((stage == (PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS-1)) && (stageMemLeft == 0))
        {
            *exportCompletePtr = GT_TRUE;
            *iteratorPtr = 0;
            break;
        }

        /* check if all iteration memory was used */
        if (*hsuBlockMemSizePtr == 0)
            break;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTcamManagerHsuExport
*
* DESCRIPTION:
*       Exports TCAM manager HSU data.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       iteratorPtr           - points to iteration value; The iteration value
*                               is used by the TCAM manager to track the export
*                               progress.
*                               At first iteration set (*iteratorPtr) = 0
*                               At next iterations supply value returned by
*                               previous iteration.
*                               Relevant only when single operation export is not used
*                               (hsuBlockMemSizePtr != CPSS_HSU_SINGLE_ITERATION_CNS)
*       hsuBlockMemSizePtr    - points to the size of the memory block supplied
*                               for the current iteration (in bytes);
*                                - Size must be at least 1k bytes.
*                                - Use the value CPSS_HSU_SINGLE_ITERATION_CNS value
*                                  to indicate single iteration export. In such case
*                                  it is assumed that hsuBlockMemPtr is large enough
*                                  for the entire HSU data.
*       hsuBlockMemPtr        - points to memory block to hold HSU data in
*                               current iteration
*
* OUTPUTS:
*       iteratorPtr           - points to iteration value; The iteration value
*                               is used by the TCAM manager to track the export
*                               progress.
*                               Relevant only if exportCompletePtr == GT_FALSE
*       hsuBlockMemSizePtr    - points to the size of the memory block that was
*                               not used in the current iteration (in bytes)
*                               Relevant only when single iteration export is not used
*                               (input valus was not CPSS_HSU_SINGLE_ITERATION_CNS)
*       exportCompletePtr     - GT_TRUE:  HSU data export is completed.
*                               GT_FALSE: HSU data export is not completed.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - on wrong parameter
*       GT_FAIL               - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Several export processes can be performed simultaneously.
*       2. It is assumed that in case of multiple iterations export operation,
*          there are no calls to other TCAM manager APIs until export operation
*          is completed. Such calls may modify the data stored within the TCAM
*          manager and cause unexpected behavior.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerHsuExport
(
    IN    GT_VOID   *tcamManagerHandlerPtr,
    INOUT GT_UINTPTR *iteratorPtr,
    INOUT GT_U32    *hsuBlockMemSizePtr,
    IN    GT_U8     *hsuBlockMemPtr,
    OUT   GT_BOOL   *exportCompletePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerHsuExport);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, exportCompletePtr));

    rc = internal_cpssDxChTcamManagerHsuExport(tcamManagerHandlerPtr, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, exportCompletePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, exportCompletePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerHsuImport
*
* DESCRIPTION:
*       Imports TCAM manager HSU data.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       iteratorPtr           - points to iteration value; The iteration value
*                               is used by the TCAM manager to track the import
*                               progress.
*                               At first iteration set (*iteratorPtr) = 0
*                               At next iterations supply value returned by
*                               previous iteration.
*                               Relevant only when single operation import is not used
*                               (hsuBlockMemSizePtr != CPSS_HSU_SINGLE_ITERATION_CNS)
*       hsuBlockMemSizePtr    - points to the size of the memory block supplied
*                               for the current iteration (in bytes);
*                                - Size must be at least 1k bytes.
*                                - Use the value CPSS_HSU_SINGLE_ITERATION_CNS value
*                                  to indicate single iteration import. In such case
*                                  it is assumed that hsuBlockMemPtr contains all HSU data.
*       hsuBlockMemPtr        - points to memory block that holds HSU data for
*                               current iteration
*
* OUTPUTS:
*       iteratorPtr           - points to iteration value; The iteration value
*                               is used by the TCAM manager to track the import
*                               progress.
*                               Relevant only if importCompletePtr == GT_FALSE
*       hsuBlockMemSizePtr    - points to the size of the memory block that was
*                               not used in the current iteration (in bytes)
*                               Relevant only when single iteration import is not used
*                               (input valus was not CPSS_HSU_SINGLE_ITERATION_CNS)
*       importCompletePtr     - GT_TRUE:  HSU data import is completed.
*                               GT_FALSE: HSU data import is not completed.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - on wrong parameter
*       GT_FAIL               - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. The attributes of the TCAM manager that were supplied during TCAM
*          manager creation of the TCAM manager object that performs the
*          import (TCAM type and TCAM range) must match the attributes of
*          the TCAM manager object that performed the export.
*       2. The import function will override all the TCAM manager data and set
*          it to the data of the TCAM manager that performed the export at the
*          time of the export. All changes made between the export and the import
*          will be discarded (only change is that during import callback functions
*          for registered clients will be set to NULL).
*       3. It is assumed that in case of multiple iterations import operation,
*          there are no calls to other TCAM manager APIs until import is
*          completed. Such calls may modify the data stored within the TCAM
*          manager and cause unexpected behavior.
*       4. It is assumed that no different import processes are performed
*          simultaneously.
*       5. Marvell recommends importing TCAM manager prior to importing
*          registered TCAM manager clients.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerHsuImport
(
    IN    GT_VOID   *tcamManagerHandlerPtr,
    INOUT GT_UINTPTR *iteratorPtr,
    INOUT GT_U32    *hsuBlockMemSizePtr,
    IN    GT_U8     *hsuBlockMemPtr,
    OUT   GT_BOOL   *importCompletePtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    static PRV_CPSS_DXCH_TCAM_MANAGER_STC       tempTcamManager;
    static PRV_CPSS_HSU_DATA_BLOCK_HEADER_STC   dataBlockHeader;
    GT_U32                                      dataBlockSize = 0;
    GT_STATUS                                   rc = GT_OK;
    GT_U32                                      i;

    /* number of bytes already imported from the current stage */
    GT_U32      stageMemCopied = 0;
    /* number of bytes that weren't imported yet in the current stage */
    GT_U32      stageMemLeft = 0;
    /* number of bytes to be imported in current stage in current iteration */
    GT_U32      stageMemToCopy;
    /* number of bytes already imported from the beginning of the current stage
       (also includes data already imported in next stages too) */
    GT_U32      memImported;
    /* current stage */
    GT_U32      stage;
    /* start address of data for each stage */
    GT_U8       *stageMemAddress[PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS];
    /* size in bytes of each stage */
    GT_U32      stageMemSize[PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS];

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);
    CPSS_NULL_PTR_CHECK_MAC(iteratorPtr);
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemSizePtr);
    CPSS_NULL_PTR_CHECK_MAC(hsuBlockMemPtr);
    CPSS_NULL_PTR_CHECK_MAC(importCompletePtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    /* by default import is marked as not completed */
    *importCompletePtr = GT_FALSE;

    /* prepare stages information */
    stageMemAddress[0] = (GT_U8*)&dataBlockHeader;
    stageMemSize[0]    = sizeof(PRV_CPSS_HSU_DATA_BLOCK_HEADER_STC);

    stageMemAddress[1] = (GT_U8*)(&tempTcamManager);
    stageMemSize[1]    = sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_STC);

    stageMemAddress[2] = (GT_U8*)tcamManagerPtr->entriesInfoArray;
    stageMemSize[2]    = (tcamManagerPtr->tcamEntries * sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC));

    stageMemAddress[3] = (GT_U8*)tcamManagerPtr->entriesUsagePtr;
    stageMemSize[3]    = (((tcamManagerPtr->tcamEntries + 31) / 32) * sizeof(GT_U32));

    stageMemAddress[4] = (GT_U8*)tcamManagerPtr->tokenToIndexArray;
    stageMemSize[4]    = (tcamManagerPtr->tokenToIndexArraySize * sizeof(GT_U32));

    /* the iterator holds the number of data bytes already imported in the
       previous iterations and is updated during the import function upon
       importing more data */

    /* in case of single iteration import, zero the iterator as no data
       was imported yet in this single iteration import */
    if (*hsuBlockMemSizePtr == CPSS_HSU_SINGLE_ITERATION_CNS)
        *iteratorPtr = 0;

    /* at first the iterator holds number of bytes already imported from the
       data block beginning */
    memImported = (GT_U32)(*iteratorPtr);

    if (*iteratorPtr > 0)
    {
        /* if this is not first iteration, then data block header must have
           been imported */
        if (memImported < sizeof(dataBlockHeader))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        /* the total number of bytes imported in previous iterations can't be equal
           or bigger than data block size; if it is, it indicates wrong iterator */
        if (memImported >= dataBlockHeader.length)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* loop over all stages, pass stages already imported */
    for (stage = 0 ; stage < PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS ; stage++)
    {
        /* check if stage was fully imported already */
        if (memImported >= stageMemSize[stage])
        {
            memImported -= stageMemSize[stage];
            /* state where all data was already imported can't happen at this point */
            if (stage == (PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS-1))
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            continue;
        }

        /* this stage was not fully imported */
        stageMemCopied = memImported;
        stageMemLeft   = stageMemSize[stage] - stageMemCopied;
        stageMemToCopy = (stageMemLeft < *hsuBlockMemSizePtr) ? stageMemLeft : *hsuBlockMemSizePtr;

        /* import stage data */
        cpssOsMemCpy(stageMemAddress[stage]+stageMemCopied,hsuBlockMemPtr,stageMemToCopy);

        /* update variables, note that memImported is set to 0 as there is no
           data that was already imported beyond this point */
        hsuBlockMemPtr      += stageMemToCopy;
        *hsuBlockMemSizePtr -= stageMemToCopy;
        *iteratorPtr        += stageMemToCopy;
        stageMemLeft        -= stageMemToCopy;
        memImported         = 0;

        /* check if this was first stage (data block header) and it was fully imported */
        if ((stage == 0) && (stageMemLeft == 0))
        {
            /* verify this is TCAM manager HSU data block */
            if ((dataBlockHeader.type != CPSS_HSU_DATA_TYPE_TCAM_MANAGER_E) ||
                (dataBlockHeader.reserved1 != 0) ||
                (dataBlockHeader.reserved2 != 0))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* upgrading from different TCAM manager versions is not supported */
            if (dataBlockHeader.version != 0)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        /* check if this was second stage (TCAM manager object import stage)
           and it was fully imported */
        if ((stage == 1) && (stageMemLeft == 0))
        {
            /* verify the imported TCAM manager object is valid */
            PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(&tempTcamManager);

            /* verify the imported TCAM manager object attributes match those
               of the TCAM manager object performing the import */
            if ((tempTcamManager.tcamType != tcamManagerPtr->tcamType) ||
                (tempTcamManager.tcamRange.firstLine != tcamManagerPtr->tcamRange.firstLine) ||
                (tempTcamManager.tcamRange.lastLine != tcamManagerPtr->tcamRange.lastLine))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* verify that the size of data block match imported TCAM manager object */
            rc = cpssDxChTcamManagerHsuSizeGet(&tempTcamManager,&dataBlockSize);
            if (rc != GT_OK)
                return rc;
            if (dataBlockHeader.length != dataBlockSize)
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

            /* since tcamType and tcamRange are the same for both TCAM manager
               objects, then entriesInfoArray and entriesUsagePtr have same
               size, so use the memory tcamManagerPtr already allocated for
               these arrays */
            tempTcamManager.entriesInfoArray = tcamManagerPtr->entriesInfoArray;
            tempTcamManager.entriesUsagePtr  = tcamManagerPtr->entriesUsagePtr;

            /* the size of tokenToIndexArray is the maximum number of TCAM entries
               during TCAM manager object lifetime; therefore it may be different
               between the two objects; if it the same, use the memory already
               allocated by tcamManagerPtr; otherwise allocate new memory and
               free the one allocated by tcamManagerPtr */
            if (tempTcamManager.tokenToIndexArraySize == tcamManagerPtr->tokenToIndexArraySize)
            {
                tempTcamManager.tokenToIndexArray = tcamManagerPtr->tokenToIndexArray;
            }
            else
            {
                tempTcamManager.tokenToIndexArray = cpssOsMalloc(tempTcamManager.tokenToIndexArraySize * sizeof(GT_U32));
                if (tempTcamManager.tokenToIndexArray == NULL)
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
                cpssOsFree(tcamManagerPtr->tokenToIndexArray);
            }

            /* mark all clients as inactive */
            for (i = 0 ; i < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; i++)
            {
                tempTcamManager.clientsArray[i].callbackFunc.moveToLocationFuncPtr = NULL;
                tempTcamManager.clientsArray[i].callbackFunc.moveToAnywhereFuncPtr = NULL;
                tempTcamManager.clientsArray[i].callbackFunc.checkIfDefaultLocationFuncPtr = NULL;
            }

            /* finally copy imported object to tcamManagerPtr object */
            cpssOsMemCpy(tcamManagerPtr,&tempTcamManager,sizeof(PRV_CPSS_DXCH_TCAM_MANAGER_STC));
        }

        /* check if this was the last stage and it was fully imported */
        if ((stage == (PRV_TCAM_MANAGER_NUMBER_OF_HSU_STAGES_CNS-1)) && (stageMemLeft == 0))
        {
            cpssOsMemSet(&dataBlockHeader,0,sizeof(dataBlockHeader));
            cpssOsMemSet(&tempTcamManager,0,sizeof(tempTcamManager));
            *iteratorPtr = 0;
            *importCompletePtr = GT_TRUE;
            break;
        }

        /* check if all iteration memory was used */
        if (*hsuBlockMemSizePtr == 0)
            break;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTcamManagerHsuImport
*
* DESCRIPTION:
*       Imports TCAM manager HSU data.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*       iteratorPtr           - points to iteration value; The iteration value
*                               is used by the TCAM manager to track the import
*                               progress.
*                               At first iteration set (*iteratorPtr) = 0
*                               At next iterations supply value returned by
*                               previous iteration.
*                               Relevant only when single operation import is not used
*                               (hsuBlockMemSizePtr != CPSS_HSU_SINGLE_ITERATION_CNS)
*       hsuBlockMemSizePtr    - points to the size of the memory block supplied
*                               for the current iteration (in bytes);
*                                - Size must be at least 1k bytes.
*                                - Use the value CPSS_HSU_SINGLE_ITERATION_CNS value
*                                  to indicate single iteration import. In such case
*                                  it is assumed that hsuBlockMemPtr contains all HSU data.
*       hsuBlockMemPtr        - points to memory block that holds HSU data for
*                               current iteration
*
* OUTPUTS:
*       iteratorPtr           - points to iteration value; The iteration value
*                               is used by the TCAM manager to track the import
*                               progress.
*                               Relevant only if importCompletePtr == GT_FALSE
*       hsuBlockMemSizePtr    - points to the size of the memory block that was
*                               not used in the current iteration (in bytes)
*                               Relevant only when single iteration import is not used
*                               (input valus was not CPSS_HSU_SINGLE_ITERATION_CNS)
*       importCompletePtr     - GT_TRUE:  HSU data import is completed.
*                               GT_FALSE: HSU data import is not completed.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - on wrong parameter
*       GT_FAIL               - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. The attributes of the TCAM manager that were supplied during TCAM
*          manager creation of the TCAM manager object that performs the
*          import (TCAM type and TCAM range) must match the attributes of
*          the TCAM manager object that performed the export.
*       2. The import function will override all the TCAM manager data and set
*          it to the data of the TCAM manager that performed the export at the
*          time of the export. All changes made between the export and the import
*          will be discarded (only change is that during import callback functions
*          for registered clients will be set to NULL).
*       3. It is assumed that in case of multiple iterations import operation,
*          there are no calls to other TCAM manager APIs until import is
*          completed. Such calls may modify the data stored within the TCAM
*          manager and cause unexpected behavior.
*       4. It is assumed that no different import processes are performed
*          simultaneously.
*       5. Marvell recommends importing TCAM manager prior to importing
*          registered TCAM manager clients.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerHsuImport
(
    IN    GT_VOID   *tcamManagerHandlerPtr,
    INOUT GT_UINTPTR *iteratorPtr,
    INOUT GT_U32    *hsuBlockMemSizePtr,
    IN    GT_U8     *hsuBlockMemPtr,
    OUT   GT_BOOL   *importCompletePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerHsuImport);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, importCompletePtr));

    rc = internal_cpssDxChTcamManagerHsuImport(tcamManagerHandlerPtr, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, importCompletePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr, iteratorPtr, hsuBlockMemSizePtr, hsuBlockMemPtr, importCompletePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChTcamManagerHsuInactiveClientDelete
*
* DESCRIPTION:
*       Delete all inactive clients and free the resources used by them. Inactive
*       client is a client that didn't update its callback functions after HSU
*       import operation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - on wrong parameter
*       GT_FAIL               - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Only resources in the TCAM manager are freed. The TCAM hardware will not
*       be changed.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChTcamManagerHsuInactiveClientDelete
(
    IN  GT_VOID     *tcamManagerHandlerPtr
)
{
    PRV_CPSS_DXCH_TCAM_MANAGER_STC              *tcamManagerPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_CLIENT_STC       *clientPtr = NULL;
    PRV_CPSS_DXCH_TCAM_MANAGER_ENTRY_INFO_STC   *entryInfoPtr = NULL;
    GT_STATUS                                   rc = GT_OK;
    GT_U32                                      clientId;
    GT_U32                                      i;

    /********************/
    /* check parameters */
    /********************/

    CPSS_NULL_PTR_CHECK_MAC(tcamManagerHandlerPtr);

    tcamManagerPtr = (PRV_CPSS_DXCH_TCAM_MANAGER_STC*)tcamManagerHandlerPtr;
    PRV_TCAM_MANAGER_VALIDITY_CHECK_MAC(tcamManagerPtr);

    /* scan all clients for inactive clients */
    for (clientId = 0 ; clientId < CPSS_DXCH_TCAM_MANAGER_MAX_CLIENTS_CNS ; clientId++)
    {
        clientPtr = &tcamManagerPtr->clientsArray[clientId];
        /* skip clients that are not registered */
        if (clientPtr->isRegistered == GT_FALSE)
            continue;
        /* skip active clients */
        if ((clientPtr->callbackFunc.moveToLocationFuncPtr != NULL) &&
            (clientPtr->callbackFunc.moveToAnywhereFuncPtr != NULL) &&
            (clientPtr->callbackFunc.checkIfDefaultLocationFuncPtr != NULL))
            continue;

        /* for each inactive client, scan the entriesInfoArray to locate
           entries associated with this client; for each entry associated
           with the client, retrieve the token and free the entry */
        for (i = 0 ; i < tcamManagerPtr->tcamEntries ; i++)
        {
            entryInfoPtr = &tcamManagerPtr->entriesInfoArray[i];
            /* skip entries not in used or used by other clients */
            if ((entryInfoPtr->isUsed == GT_FALSE) || (entryInfoPtr->clientId != clientId))
                continue;

            rc = cpssDxChTcamManagerEntryFree(tcamManagerHandlerPtr,clientId,entryInfoPtr->token);
            if (rc != GT_OK)
                return rc;
        }

        /* all entries used by the client are now free; unregister the client */
        rc = cpssDxChTcamManagerClientUnregister(tcamManagerHandlerPtr,clientId);
        if (rc != GT_OK)
            return rc;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTcamManagerHsuInactiveClientDelete
*
* DESCRIPTION:
*       Delete all inactive clients and free the resources used by them. Inactive
*       client is a client that didn't update its callback functions after HSU
*       import operation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       tcamManagerHandlerPtr - the TCAM manager handler
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if allocation succedded
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - on wrong parameter
*       GT_FAIL               - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Only resources in the TCAM manager are freed. The TCAM hardware will not
*       be changed.
*
*******************************************************************************/
GT_STATUS cpssDxChTcamManagerHsuInactiveClientDelete
(
    IN  GT_VOID     *tcamManagerHandlerPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChTcamManagerHsuInactiveClientDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, tcamManagerHandlerPtr));

    rc = internal_cpssDxChTcamManagerHsuInactiveClientDelete(tcamManagerHandlerPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, tcamManagerHandlerPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



