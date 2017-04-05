/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmRamMng.h
*
* DESCRIPTION:
*       Implementation of the LPM algorithm
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmMngh
#define __prvCpssDxChLpmMngh

#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*#define LPM_DEBUG*/
#ifdef PRESTERA_DEBUG
#define LPM_DEBUG
#endif /* PRESTERA_DEBUG */

/*******************************************************************************
* prvCpssDxChLpmRamMngRangeInNextPointerFormatSet
*
* DESCRIPTION:
*       This sets the given range in next pointer format
*
* INPUTS:
*       rangePtr        - the range to get from.
*       lpmEngineMemPtr - points to a an PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC which
*                         holds all the memory information needed for where
*                         and how to treat the search memory.
*
* OUTPUTS:
*       nextPointerArrayPtr - the next pointer
*
* RETURNS:
*       The range Hw value.
*       0 for error in the retrival.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngRangeInNextPointerFormatSet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC    *rangePtr,
    IN  PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC        *lpmEngineMemPtr,
    OUT CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC       *nextPointerArrayPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMngCreateNew
*
* DESCRIPTION:
*       This function creates a new LPM structure, with the pair (0,0) as the
*       default route entry.
*
* INPUTS:
*       nextHopPtr          - A pointer to the next hop entry to be set in the
*                             nextHopEntry field.
*       firstLevelPrefixLen - The first lpm level prefix.
*
*
* RETURNS:
*       A pointer to the new created Bucket, or NULL if allocation failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC* prvCpssDxChLpmRamMngCreateNew
(
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopPtr,
    IN GT_U8 firstLevelPrefixLen
);

/*******************************************************************************
* prvCpssDxChLpmRamMngInsert
*
* DESCRIPTION:
*       Insert a new entry to the LPM tables.
*
* INPUTS:
*       bucketPtr         - Pointer to the root bucket.
*       addrCurBytePtr    - The Byte Array represnting the Address associated
*                           with the entry.
*       prefix            - The address prefix.
*       nextPtr           - A pointer to a next hop/next bucket entry.
*       nextPtrType       - The type of nextPtr (can be next hop, ECMP/QoS entry
*                           or bucket)
*       rootBucketFlag    - Indicates the way to deal with a root bucket (if it is).
*       lpmEngineMemPtrPtr- points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                           which holds all the memory information needed for where and
*                           how to allocate search memory for each of the lpm levels
*       updateOldPtr      - (GT_TRUE) this is an update for an already existing entry.
*       bucketTypePtr     - The bucket type of the bucket inserted on the root.
*                           Possible values:
*                           REGULAR_BUCKET, COMPRESSED_1, or COMPRESSED_2.
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*       insertMode        - is the function called in the course of Hot Sync,bulk
*                           operation or regular
*       isMcSrcTree       - indicates whether bucketPtr is the toor bucket of a
*                           multicast source tree
*
* OUTPUTS:
*       updateOldPtr      - (GT_TRUE) this is an update for an already existing entry.
*       bucketTypePtr     - The bucket type of the bucket inserted on the root.
*                           Possible values:
*                           REGULAR_BUCKET, COMPRESSED_1, or COMPRESSED_2.
*
* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - if failed to allocate PP memory.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngInsert
(
    IN    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC       *bucketPtr,
    IN    GT_U8                                         *addrCurBytePtr,
    IN    GT_U32                                        prefix,
    IN    GT_PTR                                        nextPtr,
    IN    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT           nextPtrType,
    IN    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT    rootBucketFlag,
    IN    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC            **lpmEngineMemPtrPtr,
    INOUT GT_BOOL                                       *updateOldPtr,
    INOUT CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT           *bucketTypePtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC              *shadowPtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_TRIE_INSERT_MODE_ENT    insertMode,
    IN    GT_BOOL                                       isMcSrcTree
);

/*******************************************************************************
* prvCpssDxChLpmRamMngSearch
*
* DESCRIPTION:
*       Search for the existence of a given (address,Prefix) in the given LPM
*       structure.
*
* INPUTS:
*       bucketPtr         - A pointer to the bucket to search in.
*       addrCurBytePtr    - The Byte Array represnting the Address to search for.
*       prefix            - address prefix length.
*
* OUTPUTS:
*       nextHopPtr        - A pointer to the found next hop entry, or NULL if not
*                           found.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND if not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngSearch
(
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr,
    IN  GT_U8                                    *addrCurBytePtr,
    IN  GT_U32                                   prefix,
    OUT GT_PTR                                   *nextHopPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMngDoLpmSearch
*
* DESCRIPTION:
*       Search for the existence of a given (address,Prefix) in the given LPM
*       structure. This is a LPM search - meaning if exact search hasn't found
*       data - it is taken from range.
*
* INPUTS:
*       bucketPtr         - A pointer to the bucket to search in.
*       addrCurBytePtr    - The Byte Array represnting the address to search for.
*       protocolStack     - protocol stack to work on.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopPtr        - A pointer to the found next hop entry, or NULL if not
*                           found.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_NOT_FOUND             - if not found.
*       GT_BAD_PARAM             - on wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngDoLpmSearch
(
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr,
    IN  GT_U8                                    *addrCurBytePtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT     protocolStack,
    OUT GT_U32                                   *prefixLenPtr,
    OUT GT_PTR                                   *nextHopPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMngEntryGet
*
* DESCRIPTION:
*       This function returns the entry indexed 'index', entries are sorted by
*       (address,prefix) key.
*
* INPUTS:
*       bucketPtr   - A pointer to the bucket to search in.
*       protocol    - the protocol
*       addrPtr     - The address associated with the returned next hop pointer.
*       prefixPtr   - The address prefix length.
*
* OUTPUTS:
*       addrPtr     - The address associated with the returned next hop pointer.
*       prefixPtr   - The address prefix length.
*       nextPtr     - A pointer to the found next hop entry, or NULL if not
*                     found.
*
* RETURNS:
*       GT_OK if the required entry was found, or
*       GT_NOT_FOUND - if no more entries where found in the structure.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngEntryGet
(
    IN    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr,
    IN    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT     protocol,
    INOUT GT_U8                                    *addrPtr,
    INOUT GT_U32                                   *prefixPtr,
    OUT   GT_PTR                                   *nextPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMngBucketDelete
*
* DESCRIPTION:
*       This function deletes an empty bucket structure from memory.
*
* INPUTS:
*       bucketPtr   - A pointer to the bucket to be deleted.
*       levelPrefix - The current lpm level prefix.
*
* OUTPUTS:
*       pNextPtr    - A pointer to the data stored in the Trie.
*
* RETURNS:
*       GT_OK
*       GT_BAD_STATE - if the bucket is not empty (has more then 1 range)
*
* COMMENTS:
*       This function is called only for non-root buckets, which are fully empty
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngBucketDelete
(
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr,
    IN GT_U8                                    levelPrefix,
    OUT GT_PTR                                  *pNextPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMngEntryDelete
*
* DESCRIPTION:
*       Delete an entry from the LPM tables.
*
* INPUTS:
*       lpmPtr            - Pointer to the root bucket.
*       addrCurBytePtr    - The Byte Array represnting the Address associated
*                           with the entry.
*       prefix            - The address prefix.
*       rootBucketFlag    - Indicates the way to deal with a root bucket (if it is)
*       updateHwAndMem    - whether an HW update and memory alloc should take place
*       lpmEngineMemPtrPtr- points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                           which holds all the memory information needed for where and
*                           how to allocate search memory for each of the lpm levels
*       bucketTypePtr     - The bucket type of the root bucket after deletion.
*                           Possible values:
*                           REGULAR_BUCKET, COMPRESSED_1, COMPRESSED_2, or NEXT_HOP
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*       parentWriteFuncPtr- a bucket's parent write function in case there is
*                           a need to update the packet's parent ptr data and
*                           the parent is not a LPM trie range.
*       isMcSrcTree       - indicates whether bucketPtr is the toor bucket of a
*                           multicast source tree
*       rollBack          - GT_TRUE: rollback is taking place
*                           GT_FALSE: otherwise
*
* OUTPUTS:
*       lpmPtr            - Pointer to the root bucket.
*       bucketTypePtr     - The bucket type of the root bucket after deletion.
*                           Possible values:
*                           REGULAR_BUCKET, COMPRESSED_1, COMPRESSED_2, or NEXT_HOP
*       delEntryPtr       - The next_pointer structure of the entry associated with
*                           the deleted (address,prefix).
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - If (address,prefix) not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngEntryDelete
(
    INOUT PRV_CPSS_DXCH_LPM_RAM_POINTER_SHADOW_UNT                  *lpmPtr,
    IN    GT_U8                                                     *addrCurBytePtr,
    IN    GT_U32                                                    prefix,
    IN    PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT                rootBucketFlag,
    IN    GT_BOOL                                                   updateHwAndMem,
    IN    PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC                        **lpmEngineMemPtrPtr,
    INOUT CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT                       *bucketTypePtr,
    OUT   GT_PTR                                                    *delEntryPtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC                          *shadowPtr,
    IN    PRV_CPSS_DXCH_LPM_RAM_BUCKET_PARENT_WRITE_FUNC_INFO_STC   *parentWriteFuncPtr,
    IN    GT_BOOL                                                   isMcSrcTree,
    IN    GT_BOOL                                                   rollBack
);

/*******************************************************************************
* prvCpssDxChLpmRamMngAllocAvailableMemCheck
*
* DESCRIPTION:
*       Check if there is enough available memory to insert a new 
*       Unicast or Multicast address. and if there is allocate it
*       for further use in the insertion.
*
* INPUTS:
*       bucketPtr       - The LPM bucket to check on the LPM insert.
*       destArr         - If holds the unicast address to be inserted.
*       prefix          - Holds the prefix length of destArr.
*       rootBucketFlag  - Indicates the way to deal with a root bucket (if it is).
*       lpmEngineMemPtrPtr - points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                         which holds all the memory information needed for where and
*                         how to allocate search memory for each of the lpm levels
*       shadowPtr       - the shadow relevant for the devices asked to act on.
*       protocol        - type of ip protocol stack to work on. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - If there is enough memory for the insertion.
*       GT_OUT_OF_PP_MEM         - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngAllocAvailableMemCheck
(
    IN PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC      *bucketPtr,
    IN GT_U8                                        destArr[],
    IN GT_U32                                       prefix,
    IN PRV_CPSS_DXCH_LPM_RAM_ROOT_BUCKET_FLAG_ENT   rootBucketFlag,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC           **lpmEngineMemPtrPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol
);

/*******************************************************************************
* prvCpssDxChLpmRamMngAllocatedAndBoundMemFree
*
* DESCRIPTION:
*       Free memory that was pre allocated or bound in prvCpssDxChLpmRamMngAllocAvailableMemCheck
*       function. used in case of error in the insertion.
*
* INPUTS:
*       shadowPtr       - the shadow relevant for the devices asked to act on.
*       protocolStack     - protocol stack to work on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - operation finish successfully
*       GT_BAD_PARAM - Bad protocolStack input parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngAllocatedAndBoundMemFree
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocolStack   
);

/*******************************************************************************
* prvCpssDxChLpmRamMngBucketTreeWrite
*
* DESCRIPTION:
*       write an lpm bucket tree to the HW, and if neccessary allocate memory
*       for it - assuming there is enough memory
*
* INPUTS:
*       rootRangePtr   - the range which holds the root bucket of the lpm.
*       lpmEngineMemPtrPtr - points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                          which holds all the memory information needed for where and
*                         how to allocate search memory for each of the lpm levels
*       shadowPtr      - the shadow relevant for the devices asked to act on.
*       updateType     - is this an update only for the trie, overwrite of
*                        the whole tree , or update and allocate memory.
*       protocolStack  - the protocol Stack (relvant only if updateType ==
*                        PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_AND_ALLOC_MEM_E)
*       vrId           - VR Id (relvant only if updateType ==
*                        PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_HW_AND_ALLOC_MEM_E)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - If there is enough memory for the insertion.
*       GT_OUT_OF_PP_MEM         - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngBucketTreeWrite
(
    IN PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC       *rootRangePtr,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC           **lpmEngineMemPtrPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN PRV_CPSS_DXCH_LPM_RAM_TRIE_UPDATE_TYPE_ENT   updateType,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocolStack,
    IN GT_U32                                       vrId
);

/*******************************************************************************
* prvCpssDxChLpmRamMngMemTraverse
*
* DESCRIPTION:
*       a tree traverse function to set/retrive all the memory allocations in this
*       lpm bucket tree. it is done in DFS fashion.
*
* INPUTS:
*       traverseOp               - the traverse operation done on the lpm trie.
*       memAllocArrayPtr         - the array that holds the information of memory
*                                  allocations.
*       memAllocArrayIndexPtr    - the index in the array this function should start
*                                  using.
*       memAllocArraySize        - the size of the above array.
*       rootRangePtr             - the range which holds the root bucket of the lpm.
*       setRecoredRootBucketMem  - whether to set/record the root bucket's memory.
*       lpmEngineMemPtrPtr       - points to a an array of PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC
*                                  which holds all the memory information needed for where and
*                                  how to allocate search memory for each of the lpm levels
*
* OUTPUTS:
*       stopPointIterPtr         - an iterator that is returned to point where we stopped
*                                  in the lpm tree, so we can continue from there in the
*                                  next call.
*
* RETURNS:
*       GT_OK                    - If there is enough memory for the insertion.
*       GT_OUT_OF_PP_MEM         - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       if *stopPointIter ==0 we start at the begining.
*       and if the returned *stopPointIter==0 the we finished with this tree.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngMemTraverse
(
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_TRAVERSE_OP_ENT    traverseOp,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_ALLOC_INFO_STC     *memAllocArrayPtr,
    IN GT_U32                                       *memAllocArrayIndexPtr,
    IN GT_U32                                       memAllocArraySize,
    IN PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC       *rootRangePtr,
    IN GT_BOOL                                      setRecoredRootBucketMem,
    IN PRV_CPSS_DXCH_LPM_RAM_MEM_INFO_STC           **lpmEngineMemPtrPtr,
    OUT GT_UINTPTR                                  *stopPointIterPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMngRootBucketCreate
*
* DESCRIPTION:
*       Create a shadow root bucket for a specific virtual router/forwarder Id
*       and protocol, and write it to the HW.
*
* INPUTS:
*       shadowPtr                   - the shadow to work on
*       vrId                        - The virtual router/forwarder ID
*       protocol                    - the protocol
*       defUcNextHopEntryPtr        - the default unicast nexthop
*       defReservedNextHopEntryPtr  - the default reserved range nexthop
*       defMcNextHopEntryPtr        - the default multicast nexthop
*       updateHw                    - whether to update the HW
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                   - on success
*       GT_BAD_PARAM            - on bad input parameters
*       GT_OUT_OF_CPU_MEM       - no memory
*       GT_FAIL                 - on other failure
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngRootBucketCreate
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defUcNextHopEntryPtr,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defReservedNextHopEntryPtr,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *defMcNextHopEntryPtr,
    IN GT_BOOL                                      updateHw
);

/*******************************************************************************
* prvCpssDxChLpmRamMngRootBucketDelete
*
* DESCRIPTION:
*       This function deletes the root bucket structure from memory.
*
* INPUTS:
*       shadowPtr               - the shadow to work on
*       vrId                    - The virtual router/forwarder ID
*       protocol                - the protocol
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_STATE - if the bucket is not empty
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngRootBucketDelete
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr,
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol
);

/*******************************************************************************
* prvCpssDxChLpmRamMngVrfEntryUpdate
*
* DESCRIPTION:
*       perform an update of the VRF table
*
* INPUTS:
*
*       vrId              - the vrId of the updated VR
*       protocol          - the protocol
*       shadowPtr         - the shadow relevant for the devices asked to act on.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngVrfEntryUpdate
(
    IN GT_U32                                       vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamMngFindValidMemoryBlockForProtocol
*
* DESCRIPTION:
*       Search for a memory block.
*       
* INPUTS:
*       shadowPtr       - (pointer to) the shadow information
*       protocol        - the protocol
*       octetIndex      - the octetIndex to whom we want to bind the found block 
*       blockStart      - start searching from this block
*
* OUTPUTS:
*       blockIndexPtr   - (pointer to) the index of the first empty block
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - an available block was not found
*
* COMMENTS:
*       We try to associate to a given protocol a block that will not 
*       violate full wire speed.
*       If we did not find a block and the allocation mode is
*       PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E
*       we do allocate a block that may cause full wire speed violation.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMngFindValidMemoryBlockForProtocol
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC        *shadowPtr,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol,
    IN  GT_U32                                  octetIndex,
    IN  GT_U32                                  blockStart,
    OUT GT_U32                                  *blockIndexPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmMngh */

