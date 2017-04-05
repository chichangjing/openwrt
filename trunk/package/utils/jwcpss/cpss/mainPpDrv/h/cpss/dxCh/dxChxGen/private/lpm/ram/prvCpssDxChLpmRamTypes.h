/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmRamTypes.h
*
* DESCRIPTION:
*       Private cpss DXCH LPM RAM library type definitions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmRamTypesh
#define __prvCpssDxChLpmRamTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTrie.h>

/* Defines */ 
/* maximal prefix length and trie depth for level (octet) */
#define PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS  (8)

/* 3 unicast protocols: IPv4, IPv6, FCoE */
#define PRV_CPSS_DXCH_LPM_RAM_NUM_OF_UC_PROTOCOLS_CNS   (3)

/* 2 multicast protocols: IPv4, IPv6 */
#define PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MC_PROTOCOLS_CNS   (2)

/* the lower possible value of IPv4 MC first octet */
#define PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS   (224)

/* the higher possible value of IPv4 MC first octet */
#define PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV4_MC_ADDRESS_SPACE_CNS   (239)

/* the lower possible value of IPv4 reserved space first octet */
#define PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_RESERVED_SPACE_ADDRESS_SPACE_CNS (240)

/* the higher possible value of IPv4 reserved space first octet */
#define PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV4_RESERVED_SPACE_ADDRESS_SPACE_CNS   (255)

/* the lower possible value of IPv6 MC first octet */
#define PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS   (255)

/* the higher possible value of IPv6 MC first octet */
#define PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV6_MC_ADDRESS_SPACE_CNS   (255)

/* the prefix that covers the IPv4 MC address space */
#define PRV_CPSS_DXCH_LPM_RAM_IPV4_MC_ADDRESS_SPACE_PREFIX_CNS (4)

/* the prefix that covers the IPv4 reserved space space */
#define PRV_CPSS_DXCH_LPM_RAM_IPV4_RESERVED_ADDRESS_SPACE_PREFIX_CNS (4)

/* the prefix that covers the IPv6 MC address space */
#define PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_ADDRESS_SPACE_PREFIX_CNS (8)

/* size of one LPM entry in bytes (in the address space) */
#define PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_BYTES_CNS  (4)

/* size of one LPM entry in words  */
#define PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS  \
                (PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_BYTES_CNS / 4)

/* number of ranges in LPM line */
#define PRV_CPSS_DXCH_LPM_RAM_NUM_OF_RANGES_IN_LPM_LINE_CNS   (4)

/* Number of entries in the NH and ECMP tables that are kept for default */
#define PRV_CPSS_DXCH_LPM_RAM_NUM_OF_DEFAULT_ENTRIES          (3)

/* Bit vector for one-line compressed bucket */
#define PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS      1

/* Bit vector for two-lines compressed bucket */
#define PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS      2

/* Bit vector for regular bucket */
#define PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS           11

/* Number of RAM memory blocks */
#define PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS   20

/* maximum size of LPM bucket (256 ranges + 11 lines for bitmap) */
#define PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS       \
    (256 + PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS)
#define PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_BYTES_CNS           \
    (PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS *        \
     PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_BYTES_CNS)

/* size of LPM block */    
#define PRV_CPSS_DXCH_LPM_RAM_BLOCK_SIZE_IN_LINES_CNS    (16384)
#define PRV_CPSS_DXCH_LPM_RAM_BLOCK_SIZE_IN_BYTES_CNS    (PRV_CPSS_DXCH_LPM_RAM_BLOCK_SIZE_IN_LINES_CNS * PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_BYTES_CNS)

/* offset from the start of the memory block in lines */
#define PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(handler)  (DMM_GET_OFFSET(handler)>> 2)
/* size of memory block in lines */
#define PRV_CPSS_DXCH_LPM_RAM_GET_LPM_SIZE_FROM_DMM_MAC(handler)    (DMM_GET_SIZE(handler) >> 2)

/* Typedefs */
/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_MERGE_METHOD_ENT
 *
 * Description: The different 4 types of range merging when inserting a new
 *              prefix to the LPM structure.
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_LPM_RAM_MERGE_OVERWRITE_E - overwrite merge
 *  PRV_CPSS_DXCH_LPM_RAM_MERGE_HIGH_E      - high merge
 *  PRV_CPSS_DXCH_LPM_RAM_MERGE_LOW_E       - low merge
 *  PRV_CPSS_DXCH_LPM_RAM_MERGE_MID_E       - mid merge
 */
typedef enum _mergeMethod
{
    PRV_CPSS_DXCH_LPM_RAM_MERGE_OVERWRITE_E = 0,
    PRV_CPSS_DXCH_LPM_RAM_MERGE_HIGH_E      = 1,
    PRV_CPSS_DXCH_LPM_RAM_MERGE_LOW_E       = 2,
    PRV_CPSS_DXCH_LPM_RAM_MERGE_MID_E       = 3
}PRV_CPSS_DXCH_LPM_RAM_MERGE_METHOD_ENT;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_SPLIT_METHOD_ENT
 *
 * Description: The different 4 types of range splitting when deleting a
 *              prefix to the LPM structure.
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_LPM_RAM_SPLIT_OVERWRITE_E       - overwrite split
 *  PRV_CPSS_DXCH_LPM_RAM_SPLIT_HIGH_SPLIT_E      - high split
 *  PRV_CPSS_DXCH_LPM_RAM_SPLIT_LOW_SPLIT_E       - low split
 *  PRV_CPSS_DXCH_LPM_RAM_SPLIT_MID_SPLIT_E       - mid split
 */
typedef enum _splitMethod
{
    PRV_CPSS_DXCH_LPM_RAM_SPLIT_OVERWRITE_E   = 0,
    PRV_CPSS_DXCH_LPM_RAM_SPLIT_HIGH_SPLIT_E  = 1,
    PRV_CPSS_DXCH_LPM_RAM_SPLIT_LOW_SPLIT_E   = 2,
    PRV_CPSS_DXCH_LPM_RAM_SPLIT_MID_SPLIT_E   = 3
}PRV_CPSS_DXCH_LPM_RAM_SPLIT_METHOD_ENT;

/*
 * Typedef: union PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT
 *
 * Description: A pointer to the next LPM level.
 *
 * Fields:
 *      nextBucket   - The next level is another bucket when type is
 *                     CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E or
 *                     CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E or
 *                     CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E
 *      nextHopEntry - The next level is a next hop entry when type is
 *                     CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E or
 *                     CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E or
 *                     CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E
 */
typedef union
{
    struct PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STCT   *nextBucket;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC         *nextHopEntry;
} PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STCT
 *
 * Description: SW representation of a range in bucket.
 *
 * Fields:
 *      lowerLpmPtr     - The pointer to the lower LPM bucket / next hop / ECMP
 *                        entry, according to the value of pointerType.
 *      next            - A pointer to the next range in the ranges list.
 *      startAddr       - The lower address of the range.
 *      mask            - A bit mask indicating the length of the prefixes
 *                        covering this range.
 *      updateRangeInHw - indicates whether to update this range in the HW
 *      pointerType     - the type of pointer, See CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT.
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC
{
    PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT         lowerLpmPtr;
    struct PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC    *next;
    GT_U8                                            startAddr;
    GT_U8                                            mask;
    GT_BOOL                                          updateRangeInHw;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT              pointerType; 
} PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT
 *
 * Description:
 *      Indicates what kind of insert to the lpm tree is being preformed.
 *
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E  - a regular lpm insertion
 *                           where the shadow, memory alloc, and hw are updated.
 *      PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E - a hot sync styple lpm
 *                           insertion, where only the shadow is updated.
 *      PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_MODE_E - a bulk style lpm insertion.
 *                           where the shadow and the memory allocation are the
 *                           only ones updated , without the HW.
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_HW_MODE_E,
    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_ONLY_MODE_E,
    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_SDW_MEM_MODE_E
}PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_TYPE_ENT
 *
 * Description: the type of update the lpm trie needs
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_RAM_TRIE_WRITE_HW_AND_ALLOC_MEM_E - write the whole trie
 *              to the HW and allocate memory if needed.
 *      PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_ONLY_E - update the Trie without any
 *              memory allocation.(memory should already be allocated correctly)
 *      PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_AND_ALLOC_MEM_E - update the trie and
 *              allocate memory if needed.
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_RAM_TRIE_WRITE_HW_AND_ALLOC_MEM_E   = 0,
    PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_ONLY_E,
    PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_AND_ALLOC_MEM_E
}PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_TYPE_ENT;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_UPDATE_STATUS_ENT
 *
 * Description: the status of the bucket in HW
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E -
 *          the bucket is fully updated in the HW
 *
 *      PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_NEEDS_UPDATE_E -
 *          the bucket needs update in HW of some of it's pointers
 *
 *      PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_WAS_RESIZED_AND_NEEDS_REWRITING_E -
 *          the bucket was resized and needs rewriting in the HW
 *
 *      PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_UPDATE_SIBLINGS_E -
 *          the bucket is fully updated in HW, but it's siblings needs HW update
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_E = 0,
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_NEEDS_UPDATE_E,
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_WAS_RESIZED_AND_NEEDS_REWRITING_E,
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_OK_UPDATE_SIBLINGS_E
} PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_UPDATE_STATUS_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STCT
 *
 * Description: A struct representing an LPM level's bucket.
 *
 * Fields:
 *      pointingRangeMemAddr  - the parnet bucket's range memory address (required
 *                              mainly for bulk operations and swap mem usage,
 *                              0 indicates that this bucket is pointed from the
 *                              root bucke and may have several place it's pointed
 *                              from)
 *      trieRoot              - The root of the Trie to save all insertions history.
 *      rangeList             - A pointer to the first range in this bucket.
 *      hwBucketOffsetHandle  - The handle of HW Address of the bucket. The
 *                              address in this field is an offset from
 *                              structsBase.
 *      rangeCash             - cash used to make searching faster.
 *      numOfRanges           - Number of ranges in the list
 *      bucketHwUpdateStat    - Hw bucket update status
 *      bucketType            - this bucket type, can be
 *                              CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E or
 *                              CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E or
 *                              CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E
 *      fifthAddress          - the fifth address of the bucket in case of a
 *                              two-lines compressed bucket
 *      isImplicitGroupDefault- for MC group buckets:
 *                              GT_TRUE: group default was added implicitly
 *                              GT_FALSE: group default was added explicitly
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STCT
{
    GT_U32                                              pointingRangeMemAddr;
    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC                 trieRoot;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC              *rangeList;
    GT_UINTPTR                                          hwBucketOffsetHandle;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC              *rangeCash;
    GT_U16                                              numOfRanges;
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_HW_UPDATE_STATUS_ENT   bucketHwUpdateStat;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT                 bucketType;
    GT_U8                                               fifthAddress;
    GT_BOOL                                             isImplicitGroupDefault;
} PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STCT
 *
 * Description: LPM Engine memory configuration information
 *
 * Fields:
 *
 *      ramIndex         - the HW RAM index in the shadow memory configuration
 *      structBase       - the base addresses of the LPM block in lines
 *      structsMemPool   - The memory pool Id that manages the memory for search
 *                         structures allocation.
 *      nextMemInfoPtr   - (pointer to) the next memory info structure 
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STCT
{
    GT_U32      ramIndex;
    GT_U32      structsBase;
    GT_UINTPTR  structsMemPool;
    struct PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STCT* nextMemInfoPtr;
} PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_TREE_ITERATOR_STC
 *
 * Description: A struct that holds an lpm tree iterator info
 *
 * Fields:
 *      rangePtrArray   - an array of pointers to ranges in the tree .
 *      currRangePtr - a pointer to the current range we are dealing with.
 *      currLpmEnginePtr - the pointer to the current place in the lpm engine
 *                         array.
 */
typedef struct _lpmTreeIterator
{
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC    *rangePtrArray[17];
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC    **currRangePtr;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC        **currLpmEnginePtr;
}PRV_CPSS_DXCH_LPM_RAM_TREE_ITERATOR_STC,*PRV_CPSS_DXCH_LPM_RAM_TREE_ITERATOR_STC_PTR;

/*******************************************************************************
* PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_PTR
*
* DESCRIPTION:
*       Writes a bucket parent ptr data
*
* INPUTS:
*       data - the data for the function.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error
*
* COMMENTS:
*       Configures an entry in the prefix look up table
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_PTR)
(
    IN  GT_PTR                  data
);

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC
 *
 * Description: A struct that holds an lpm bucket parent date write function
 *              information
 *
 * Fields:
 *      bucketParentWriteFunc   - pointer to function .
 *      data - a pointer to data the function should get.
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STCT
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_PTR  bucketParentWriteFunc;
    GT_PTR                                              data;
}PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_VR_TBL_UPDATE_TYPE_ENT
 *
 * Description:
 *      Indicates which the entries should be updated
 *      and the way they should be updated in the VR router table.
 *
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_RAM_VR_TBL_UPDATE_UC_E - UC entries only.
 *      PRV_CPSS_DXCH_LPM_RAM_VR_TBL_UPDATE_MC_E - MC entries only.
 *      PRV_CPSS_DXCH_LPM_RAM_VR_TBL_UPDATE_UC_MC_E  - UC & MC entries.
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_RAM_VR_TBL_UPDATE_UC_E = 0,
    PRV_CPSS_DXCH_LPM_RAM_VR_TBL_UPDATE_MC_E,
    PRV_CPSS_DXCH_LPM_RAM_VR_TBL_UPDATE_UC_MC_E
}PRV_CPSS_DXCH_LPM_RAM_VR_TBL_UPDATE_TYPE_ENT;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_OP_ENT
 *
 * Description: Defines the different operation done while traversing the ip
 *              structures
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_SET_E - set the memory in the LPM structure
 *      PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_REC_E - record the memory in the LPM structure
 *      PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_E - count the amount of memory entries in LPM structure.
 *      PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_SET_WITH_PCL_E - set the memory in the LPM structure and LPM
 *                                     PCL structure (IPv6 MC)
 *      PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_REC_WITH_PCL_E - record the memory in the LPM structure and
 *                                     LPM PCL structure (IPv6 MC)
 *      PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_WITH_PCL_E - count the amount of memory entries in
 *                                       LPM structure and LPM PCL structure (IPv6 MC)
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_SET_E,
    PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_REC_E,
    PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_E,
    PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_SET_WITH_PCL_E,
    PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_REC_WITH_PCL_E,
    PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_COUNT_WITH_PCL_E
}PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_OP_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STCT
 *
 * Description: IPv4 UC Prefix information for bulk operations
 *
 * Fields:
 *      vrId                    - the virtual router id
 *      ipAddr                  - the destination IPv4 address of this prefix
 *      prefixLen               - the prefix length of ipAddr
 *      nextHopInfo             - the route entry info accosiated with this UC
 *                                prefix
 *      override                - GT_TRUE:  override an existing prefix.
 *                                GT_FALSE: don't override an existing prefix,
 *                                and return an error
 *      returnStatus            - the return status for this prefix
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STCT
{
    GT_U32                                     vrId;
    GT_IPADDR                                  ipAddr;
    GT_U32                                     prefixLen;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  nextHopInfo;
    GT_BOOL                                    override;
    GT_STATUS                                  returnStatus;
} PRV_CPSS_DXCH_LPM_RAM_IPV4_UC_PREFIX_BULK_OPERATION_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STCT
 *
 * Description: IPv6 UC Prefix information for bulk operations
 *
 * Fields:
 *      vrId                    - the virtual router id
 *      ipAddr                  - the destination IPv6 address of this prefix
 *      prefixLen               - the prefix length of ipAddr
 *      nextHopInfo             - the route entry info accosiated with this UC
 *                                prefix
 *      override                - GT_TRUE:  override an existing prefix.
 *                                GT_FALSE: don't override an existing prefix,
 *                                and return an error
 *      returnStatus            - the return status for this prefix
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STCT
{
    GT_U32                                     vrId;
    GT_IPV6ADDR                                ipAddr;
    GT_U32                                     prefixLen;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  nextHopInfo;
    GT_BOOL                                    override;
    GT_STATUS                                  returnStatus;
} PRV_CPSS_DXCH_LPM_RAM_IPV6_UC_PREFIX_BULK_OPERATION_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STCT
 *
 * Description: IPv6 UC Prefix information for bulk operations
 *
 * Fields:
 *      supportIpv4Uc           - whether this VR supports UC Ipv4.
 *      defaultUcIpv4RouteEntry - the route entry info associated to the default
 *                                Ipv4 UC route entry.
 *      supportIpv4Mc           - whether this VR support MC Ipv4.
 *      defaultMcIpv4RouteEntry - the route entry info associated to the default
 *                                Ipv4 MC route entry.
 *      supportIpv6Uc           - whether this VR supports UC Ipv6.
 *      defaultUcIpv6RouteEntry - the route entry info associated to the default
 *                                Ipv6 UC route entry.
 *      supportIpv6Mc           - whether this VR supports MC Ipv6
 *      defaultMcIpv6RouteEntry - the route entry info associated to the default
 *                                Ipv6 MC route entry.
 *      supportFcoe             - whether this VR supports FCoE
 *      defaultFcoeForwardingEntry - the forwarding entry info associated to the
 *                                default FCoE forwarding entry.
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STCT
{
    GT_BOOL                                    supportUcIpv4;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  defaultUcIpv4RouteEntry;
    GT_BOOL                                    supportMcIpv4;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  defaultMcIpv4RouteEntry;
    GT_BOOL                                    supportUcIpv6;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  defaultUcIpv6RouteEntry;
    GT_BOOL                                    supportMcIpv6;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  defaultMcIpv6RouteEntry;
    GT_BOOL                                    supportFcoe;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  defaultFcoeForwardingEntry;
} PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_SHADOW_DEVS_LIST_STCT
 *
 * Description: LPM shadow shared device list
 *
 * Fields:
 *
 *      shareDevNum     - device numbers of devices sharing this shadow.
 *      shareDevs       - Device numbers of PPs sharing the shadow.
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_SHADOW_DEVS_LIST_STCT
{
    GT_U32   shareDevNum;
    GT_U8    *shareDevs;
} PRV_CPSS_DXCH_LPM_RAM_SHADOW_DEVS_LIST_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STCT
 *
 * Description:
 *      This structure holds a list entry of memory block to be freed after
 *      an operation on the UC/MC search structures.
 *
 * Fields:
 *      memAddr     - Pp Memory address to be freed.
 *      next        - the next entry in this free link list
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STCT
{
    GT_UINTPTR                                         memAddr;
    struct PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STCT   *next;
} PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_STC
 *
 * Description: Virtual Router information
 *
 * Fields:
 *
 *      valid                - is this vrId valid
 *      needsHwUpdate        - whether this vrId needs HW update
 *      isUnicastSupported   - whether unicast is supported in this VR
 *      isMulticastSupported - whether multicast is supported in this VR
 *      rootBucketType       - the root bucket type
 *      rootBucket           - the root bucket
 *      multicastDefault     - the default nexthop for (G,*), where G is 224/4
 *                             for IPv4 or 255/8 for IPv6
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_STCT
{
    GT_BOOL         valid;
    GT_BOOL         needsHwUpdate;
    GT_BOOL         isUnicastSupported[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_UC_PROTOCOLS_CNS];
    GT_BOOL         isMulticastSupported[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MC_PROTOCOLS_CNS];
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT
                    rootBucketType[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_UC_PROTOCOLS_CNS];
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC
                    *rootBucket[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_UC_PROTOCOLS_CNS];
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC
                    *multicastDefault[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MC_PROTOCOLS_CNS];
} PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_STC;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT
 *
 * Description:
 *      Defines the method of LPM blocks allocation
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E -
 *          the blocks are allocated dynamically, memory blocks are never shared
 *          among different octets of the same protocol
 *      PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E -
 *          the blocks are allocated dynamically, memory blocks are shared among
 *          different octets of the same protocol in case of missing free block.
 *          This can cause violation of full wire-speed.
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E  = 0,
    PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E     = 1
} PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_CONFIG_STCT
 *
 * Description: Memory configurations for RAM based LPM shadow
 *
 * Fields:
 *      numOfBlocks             - the number of RAM blocks that LPM uses. 
 *                                (APPLICABLE VALUES: 1..20)
 *      blocksSizeArray         - array that holds the sizes of the RAM blocks in lines
 *      blocksAllocationMethod  - the method of blocks allocation
 *
 * Comments:
 *      None
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_CONFIG_STCT
{
    GT_U32                                              numOfBlocks;
    GT_U32                                              blocksSizeArray[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS];
    PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT  blocksAllocationMethod;
} PRV_CPSS_DXCH_LPM_RAM_CONFIG_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STCT
 *
 * Description: Octets to Block Mapping information.
 *
 * Fields:
 *      isBlockUsed                 - specify if the block is in use by any protocol ,
 *                                  meaning octets reside in the specific block 
 *                                  GT_TRUE: block is used
 *                                  GT_FALSE: block is not used
 * 
 *      octetsToBlockMappingBitmap - for each protocol we hold a bitmap that specify
 *                                   what octets are using this block.
 *                                   This bitmap is common for UC and MC,
 *                                   for example ipv4 UC/MC_Src and MC_Group will use
 *                                   bits 0-3 (octet0, octet1, octet2, octet3) to
 *                                   specify they are using this block.
 *                                   if the bitmap per protocol is equal to 0 then
 *                                   the protocol is not using the block.
 * 
 * Comments:
 *      None
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STCT
{
    GT_BOOL                                             isBlockUsed;    
    GT_U32                                              octetsToBlockMappingBitmap[PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E];    
} PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_SHADOW_STCT
 *
 * Description: LPM Shadow.
 *
 * Fields:
 *
 *      shadowType      - type of shadow
 *      lpmMemInfoArray - the memory info
 *      numOfLpmMemories- the number of memories used by LPM engines.
 *      lpmRamBlocksSizeArrayPtr          - (pointer to) array that holds the number of entries in each RAM blocks in lines
 *      lpmRamTotalBlocksSizeIncludingGap - number of 'entries' between each blocks in lines
 *                                          (this size should be equal or biger then lpmRamBlocksSize)
 *      lpmRamBlocksAllocationMethod  - the method of blocks allocation
 *      lpmRamStructsMemPoolPtr       - (pointer to) array that holds the memPoolId for the allocated memory blocks in SW 
 *      lpmRamOctetsToBlockMappingPtr - (pointer to) Holds the mapping of octets+protocol to block
 *                                      What are the octets binded to each block (per protocol)
 *      mcSearchMemArrayPtr - the MC search memory information for each
 *                        of the LPM levels. it combined 4 grp + 4 src (and one
 *                        extra for safety)
 *      ucSearchMemArrayPtr - the UC search memory information for each
 *                        of the LPM levels per protocol stack.
 *      swapMemoryAddr  - the address of the swap area
 *      vrRootBucketArray - The VR root bucket information
 *      vrfTblSize      - VRF table size (number of elements in vrRootBucketArray)
 *      workDevListPtr  - pointer to the device list to preform the actions on.
 *      shadowDevList   - the device list of the devices sharing this shadow.
 *      freeMemListEndOfUpdate - Free memory blocks at the end of update
 *      freeMemListDuringUpdate - Free memory blocks during update
 *      neededMemoryBlocks - used for memory needs
 *      neededMemoryBlocksSizes - sizes of neededMemoryBlocks
 *      neededMemoryListLen - number of elements in neededMemoryBlocks
 *      neededMemoryCurIdx - current index when running on neededMemoryBlocks
 *      isProtocolInitialized - is the protocol initialized
 *      allNewNextMemInfoAllocatedPerOctetArrayPtr - (pointer to) an array that holds for each octet 
 *                                                   the allocated new element that need to be freed.
 *                                                   Size of the array is 16 for case of IPV6.
 *                                                   We use this array to hold the new allocated RAM space.
 *                                                   in case of an error we free all bound memory.
 *                                                   If an allocation of a new prefix succeed then we
 *                                                   reset this array for next prefix addition.
 *                                                   
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_SHADOW_STCT
{
    PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT            shadowType;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC           lpmMemInfoArray[PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E][PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS];
    GT_U32                                       numOfLpmMemories;
    GT_U32                                              *lpmRamBlocksSizeArrayPtr; 
    GT_U32                                              lpmRamTotalBlocksSizeIncludingGap;    
    PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_ENT  lpmRamBlocksAllocationMethod; 
    GT_UINTPTR                                          *lpmRamStructsMemPoolPtr;
    PRV_CPSS_DXCH_LPM_RAM_OCTETS_TO_BLOCK_MAPPING_STC   *lpmRamOctetsToBlockMappingPtr;
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC*          mcSearchMemArrayPtr[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MC_PROTOCOLS_CNS][32];
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC*          ucSearchMemArrayPtr[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_UC_PROTOCOLS_CNS][16];
    GT_UINTPTR                                   swapMemoryAddr; 
    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_STC        *vrRootBucketArray;
    GT_U32                                       vrfTblSize;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_DEVS_LIST_STC   *workDevListPtr;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_DEVS_LIST_STC   shadowDevList;
    PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STC     *freeMemListEndOfUpdate;
    PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STC     *freeMemListDuringUpdate;
    GT_UINTPTR                                   neededMemoryBlocks[1024];
    GT_U32                                       neededMemoryBlocksSizes[1024];
    GT_U32                                       neededMemoryListLen;
    GT_U32                                       neededMemoryCurIdx;
    GT_BOOL                                      isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E];
    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC           *allNewNextMemInfoAllocatedPerOctetArrayPtr[16];     
} PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT
 *
 * Description: Defines the way a bucket is treated in the lpm insertion/ deletion
 *              process.
 *
 * Fields:
 *      PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITH_RAM_UPDATE_E - the bucket is a root
 *              bucket that needs ram update.
 *      PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_RAM_UPDATE_E - the bucket is a root
 *              bucket that needs no ram update.
 *      PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E - the bucket is root
 *              bucket that cannot use compressed 2 style (needs always a ram
 *              update).
 *      PRV_CPSS_DXCH_LPM_RAM_NOT_A_ROOT_BUCKET_E -   the bucket is a not a root bucket.
 */
typedef enum PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENTT
{
    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITH_RAM_UPDATE_E,
    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_RAM_UPDATE_E,
    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_WITHOUT_COMPRESSED_2_E,
    PRV_CPSS_DXCH_LPM_RAM_NOT_A_ROOT_BUCKET_E
}PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT;

/* Typedef: struct IP_MEM_ALLOC_INFO_STC
 *
 * Description: A structure that holds bucket memory allocation information
 *
 * Fields:
 *      memAddr            - the address of the memory allocated.
 *      memSize            - the size of the memory allocated.
 *      bucketHwUpdateStat - bucket hw status
 */
typedef struct IP_MEM_ALLOC_INFO_STCT
{
    GT_U32        memAddr;
    GT_U32        memSize;
    GT_U32        bucketHwUpdateStat;
} PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC
 *
 * Description: virtual router HSU/Hotsyc info
 *
 * Fields:
 *   vrId                - virtual router id
 *   vrIpUcSupport       - Boolean array stating UC support for every protocol stack
 *   vrIpMcSupport       - Boolean array stating MC support for every protocol stack
 *   defUcRouteEntrey    - the array shows default UC next hop ipv4/v6 information for
 *                         this virtual
 *   defMcRouteEntrey    - the array shows default MC ipv4/v6 route for this
 *                         virtual router.
 *   protocolStack       - types of IP stack used in this virtual router.
 *   defIpv6McRuleIndex  - in the case a Ipv6 VR is initialized, this is the
 *                         Rule index for the ipv6 Default Mc Group.
 *   vrIpv6McPclId       - in the case a Ipv6 VR is initialized, this is the
 *                         Pcl id for the ipv6 Mc Group entries.
 *   isLast              - is this the last
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STCT
{
    GT_U32                                  vrId;
    GT_BOOL     vrIpUcSupport[CPSS_IP_PROTOCOL_IPV4V6_E];
    GT_BOOL     vrIpMcSupport[CPSS_IP_PROTOCOL_IPV4V6_E];
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  defUcRouteEntrey[CPSS_IP_PROTOCOL_IPV4V6_E];
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC  defMcRouteEntrey[CPSS_IP_PROTOCOL_IPV4V6_E];
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack;
    GT_U32                                  defIpv6McRuleIndex;
    GT_U32                                  vrIpv6McPclId;
    GT_BOOL                                 isLast;
} PRV_CPSS_DXCH_LPM_RAM_VR_INFO_STC;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_OP_ENT
 *
 * Description: Defines the different operation that may be done be the
 *       memory block list
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E    - Reset the memory list.
 *      PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ADD_MEM_OP_E  - Add a memory block to the
 *                                                     to be freed memory list.
 *      PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E - Free the memory form the
 *                                                     collected memory list.
 *
 * Note:
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E = 0,
    PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ADD_MEM_OP_E,
    PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E
}PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_OP_ENT;

/*
 * Typedef: enum PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_ENT
 *
 * Description: A next LPM Trie
 *
 * Fields:
 *      PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E - A next LPM Trie (used in
 *                                              MC src LPM structure)
 */
enum PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_ENT
{
    PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E = 255
};

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC
 *
 * Description: LPM DB data
 *
 * Fields:
 *
 *      shadowArray           - the LPM DB array of shadow it holds
 *      numOfShadowCfg        - number of shadows in the shadowArray
 *      protocolBitmap        - the protocols this LPM DB supports
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STCT
{
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC   *shadowArray;
    GT_U32                             numOfShadowCfg;
    PRV_CPSS_DXCH_LPM_PROTOCOL_BMP     protocolBitmap;
} PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmRamTypesh */

