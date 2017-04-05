/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmHw.c
*
* DESCRIPTION:
*       LPM HW releated internal functions
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/private/lpm/hw/prvCpssDxChLpmHw.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>

/* LPM bucket - fields offsets in bits */
#define PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_OFFSET_CNS                       0
#define PRV_CPSS_DXCH_LPM_RAM_POINT_TO_LOOKUP_1_OFFSET_CNS                  2
#define PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_GROUP_SCOPE_LEVEL_OFFSET_CNS          2
#define PRV_CPSS_DXCH_LPM_RAM_UC_RPF_CHECK_OFFSET_CNS                       4
#define PRV_CPSS_DXCH_LPM_RAM_UC_SIP_SA_CHECK_MISMATCH_OFFSET_CNS           5
#define PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_TYPE_OFFSET_CNS                   3
#define PRV_CPSS_DXCH_LPM_RAM_FIFTH_RANGE_OFFSET_CNS                        5
#define PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_POINTER_OFFSET_CNS                5
#define PRV_CPSS_DXCH_LPM_RAM_NEXTHOP_OR_ECMP_POINTER_OFFSET_CNS            7
#define PRV_CPSS_DXCH_LPM_RAM_COMPRESSED_2_NEXT_BUCKET_POINTER_OFFSET_CNS   13

/* LPM bucket - fields length in bits */
#define PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_LENGTH_CNS                       2
#define PRV_CPSS_DXCH_LPM_RAM_POINT_TO_LOOKUP_1_LENGTH_CNS                  1
#define PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_GROUP_SCOPE_LEVEL_LENGTH_CNS          2
#define PRV_CPSS_DXCH_LPM_RAM_UC_RPF_CHECK_LENGTH_CNS                       1
#define PRV_CPSS_DXCH_LPM_RAM_UC_SIP_SA_CHECK_MISMATCH_LENGTH_CNS           1
#define PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_TYPE_LENGTH_CNS                   2
#define PRV_CPSS_DXCH_LPM_RAM_FIFTH_RANGE_LENGTH_CNS                        8
#define PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_POINTER_LENGTH_CNS                24
#define PRV_CPSS_DXCH_LPM_RAM_NEXTHOP_OR_ECMP_POINTER_LENGTH_CNS            15
#define PRV_CPSS_DXCH_LPM_RAM_COMPRESSED_2_NEXT_BUCKET_POINTER_LENGTH_CNS   19

/*******************************************************************************
* prvCpssDxChLpmHwVrfEntryWrite
*
* DESCRIPTION:
*       Write a VRF table entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       vrId                - the virtual router id
*       protocol            - the protocol
*       nodeType            - the type of the node
*       rootNodeAddr        - the line number of the root node
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_OUT_OF_RANGE          - rootNodeAddr is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       VRF root nodeType can be only CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E or
*       CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmHwVrfEntryWrite
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol,
    IN  CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT     nodeType,
    IN  GT_U32                                  rootNodeAddr
)
{
    PRV_CPSS_DXCH_TABLE_ENT         tableType;
    GT_U32                          bucketTypeValue, data;
    GT_STATUS                       rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);

    switch (protocol)
    {
        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E:
            tableType = PRV_CPSS_DXCH_LION3_TABLE_IPV4_VRF_E;
            break;

        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E:
            tableType = PRV_CPSS_DXCH_LION3_TABLE_IPV6_VRF_E;
            break;

        case PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E:
            tableType = PRV_CPSS_DXCH_LION3_TABLE_FCOE_VRF_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (nodeType)
    {
        case CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E:
            bucketTypeValue = 0;
            break;
    
        case CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E:
            bucketTypeValue = 1;
            break;
    
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    data = ((rootNodeAddr & 0xFFFFFF) | (bucketTypeValue << 24));
    rc = prvCpssDxChWriteTableEntry(devNum,
                                    tableType,
                                    vrId,
                                    &data);
    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmHwVrfEntryRead
*
* DESCRIPTION:
*       Read a VRF table entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       vrId                - the virtual router id
*       protocol            - the protocol
*
* OUTPUTS:
*       nodeTypePtr         - the type of the node
*       rootNodeAddrPtr     - the line number of the root node
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       VRF root nodeTypePtr can be only CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E
*       or CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmHwVrfEntryRead
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol,
    OUT CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT     *nodeTypePtr,
    OUT GT_U32                                  *rootNodeAddrPtr
)
{
    PRV_CPSS_DXCH_TABLE_ENT tableType;
    GT_U32                  data;
    GT_STATUS               rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
         CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(nodeTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(rootNodeAddrPtr);

    switch (protocol)
    {
        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E:
            tableType = PRV_CPSS_DXCH_LION3_TABLE_IPV4_VRF_E;
            break;

        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E:
            tableType = PRV_CPSS_DXCH_LION3_TABLE_IPV6_VRF_E;
            break;

        case PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E:
            tableType = PRV_CPSS_DXCH_LION3_TABLE_FCOE_VRF_E;
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChReadTableEntry(devNum,
                                   tableType,
                                   vrId,
                                   &data);
    /* the bucket type bit */
    if ((data >> 24) & 0x1)
    {
        *nodeTypePtr = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;
    }
    else
    {
        *nodeTypePtr = CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E;
    }
    *rootNodeAddrPtr = (data & 0xFFFFFF);

    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmHwEcmpEntryWrite
*
* DESCRIPTION:
*       Write an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmHwEcmpEntryWrite
(
    IN GT_U8                                devNum,
    IN GT_U32                               ecmpEntryIndex,
    IN PRV_CPSS_DXCH_LPM_ECMP_ENTRY_STC     *ecmpEntryPtr
)
{
    GT_U32  data;
    GT_U32  numOfNextHopEntries;

    /* there are only 15 bits in the base index field*/
    numOfNextHopEntries = MIN(BIT_15, PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop);

    if ((ecmpEntryPtr->numOfPaths == 0) || (ecmpEntryPtr->numOfPaths > BIT_12))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (ecmpEntryPtr->nexthopBaseIndex >= numOfNextHopEntries)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((ecmpEntryPtr->nexthopBaseIndex + ecmpEntryPtr->numOfPaths) > numOfNextHopEntries)
    {
        /* last entry must not be out of the NH table */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    data = ((BOOL2BIT_MAC(ecmpEntryPtr->randomEnable) << 15) |
            ((ecmpEntryPtr->numOfPaths - 1) << 16) |
            (ecmpEntryPtr->nexthopBaseIndex));

    return prvCpssDxChWriteTableEntry(devNum,
                                      PRV_CPSS_DXCH_LION3_TABLE_ECMP_E,
                                      ecmpEntryIndex,
                                      &data);
}

/*******************************************************************************
* prvCpssDxChLpmHwEcmpEntryRead
*
* DESCRIPTION:
*       Read an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*
* OUTPUTS:
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmHwEcmpEntryRead
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ecmpEntryIndex,
    OUT PRV_CPSS_DXCH_LPM_ECMP_ENTRY_STC    *ecmpEntryPtr
)
{
    GT_U32      data;
    GT_STATUS   rc;

    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION3_TABLE_ECMP_E,
                                   ecmpEntryIndex,
                                   &data);
    if (rc == GT_OK)
    {
        ecmpEntryPtr->randomEnable = BIT2BOOL_MAC((data >> 15) & 0x1);
        ecmpEntryPtr->numOfPaths = ((data >> 16) & 0xfff) + 1;
        ecmpEntryPtr->nexthopBaseIndex = (data & 0x7fff);
    }
    return rc;
}


/*******************************************************************************
* prvCpssDxChLpmLeafEntryCalcLpmHwIndexFromSwIndex
*
* DESCRIPTION:
*       Calculate HW Index for an LPM leaf entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum                   - the device number
*       leafIndex                - leaf index within the LPM PBR block
*
* OUTPUTS:
*       lpmIndexPtr              - (pointer to) leaf index within the LPM PBR block in HW representation 
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmLeafEntryCalcLpmHwIndexFromSwIndex
(
    IN GT_U8            devNum,
    IN GT_U32           leafIndex,
    OUT GT_U32          *lpmIndexPtr
)
{
    GT_U32              pbrBase_k;                   /* the base address of PBR - SW address */
    GT_U32              numOfLinesInFirstPbrBlock_m; /* number of lines occupied by PBR in the first PBR block, 
                                                        rest of the lines are used for IP LPM*/
    GT_U32              numOfLinesInBlock_n;         /* number of legal lines in the memory block*/
    GT_U32              totalNumOfLinesInBlockIncludingGap_h;/* number of lines in the memory block 
                                                                including gap of lines that cannot be used */
    GT_U32              blockIdFromStartPbr_e;      /* block id from the start of PBR blocks. 
                                                       if we have 3 blocks for example then the 
                                                       blocks ID will be 0, 1, 2 */
    GT_U32              addrOfLastPbrBlock;         /* address of the last PBR found that 
                                                       is suitable for adding the needed PBR */
    GT_U32              offsetInLastPbrBlock;       /* offset of the new PBR in the last PBR block found */
    

    /* check that leafIndex is a legal PBR index: 0...(maxNumOfPbrEntries-1) */
    if(leafIndex>=PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.ip.maxNumOfPbrEntries)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    pbrBase_k=PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.indexForPbr;
    numOfLinesInBlock_n = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfLinesInBlock;
    totalNumOfLinesInBlockIncludingGap_h = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.totalNumOfLinesInBlockIncludingGap;
    numOfLinesInFirstPbrBlock_m = numOfLinesInBlock_n-(pbrBase_k%totalNumOfLinesInBlockIncludingGap_h);         

    if (leafIndex<numOfLinesInFirstPbrBlock_m)
    {
        *lpmIndexPtr = pbrBase_k+leafIndex;
    }
    else
    {

        /* LPM with PBR blocks in case numOfLinesInBlock_n < totalNumOfLinesInBlockIncludingGap_h
         
           blocks:  0       1               17          18          19 --> total 20 blocks
                    -----   -----   .....   -----  e--> ----- e-->  -----  --
                    |   |   |   |           |n-m|       |   |       |   |  ^
                    | n |   | n |      k--> |---|       | n |       | n |  ^
                    |   |   |   |           | m |       |   |       |   |  ^
                    |---|   |---|           |---|       |---|       |---|  ^ h
                    |   |   |   |           |   |       |   |       |   |  ^
                    |   |   |   |           |   |       |   |       |   |  ^
                    -----   -----           -----       -----       -----  -- 
        */
        /* the formula for finding the lpmIndex (hw index)calculated from leafIndex (sw index ) is:
           lpmIndex = (e+(k/h))*h + ((x-m)%n)
           e = (leafIndex - m)/n + 1 --> when the leafIndex is bigger the size allocated for the
           PBR in the first PBR block (m), we need to deduct the size in the first PBR block (m) from the leafIndex,
           then we need to divide it by the size of the block (n) in order to get the block index where it should be set.
           since we get values from 0... we need to add + 1 in order to get to the correct block index that can be
           used in the next formula calculation */
        blockIdFromStartPbr_e = ((leafIndex-numOfLinesInFirstPbrBlock_m)/numOfLinesInBlock_n)+1;
        /* addrOfLastPbrBlock = (e+(k/h))* h --> k is the base address of PBR (SW address),
           h is the total size of a block including the gap, k/h gives the block index where the PBR starts,
           then we need to progress to the block calculated above (e) where our index will reside.
           in the end we have the PBR block index from the beginning of the LPM blocks where we will set the data.
           now we want to translate it to an address so we multiply it by the total size of the block including the gap (h)*/
        addrOfLastPbrBlock = (blockIdFromStartPbr_e+(pbrBase_k/totalNumOfLinesInBlockIncludingGap_h))*totalNumOfLinesInBlockIncludingGap_h;
        /* offsetInLastPbrBlock = (leafIndex - m) % n --> in order to get the offset of the PBR in the
           last suitable block we found, we need to deduct from leafIndex the size in the first PBR block,
           and to do modulo n in order to get the remaining that will fit the last suitable PBR block */
        offsetInLastPbrBlock = ((leafIndex-numOfLinesInFirstPbrBlock_m)%numOfLinesInBlock_n);
        *lpmIndexPtr = addrOfLastPbrBlock + offsetInLastPbrBlock;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmLeafEntryCalcLpmSwIndexFromHwIndex
*
* DESCRIPTION:
*       Calculate SW Index for an LPM leaf HW index
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum                   - the device number
*       leafHwIndex              - leaf HW index within the LPM PBR block in HW representation 
* 
* OUTPUTS:
*       leafSwIndexPtr           - (pointer to)leaf index within the LPM PBR block SW representation
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmLeafEntryCalcLpmSwIndexFromHwIndex
(
    IN GT_U8            devNum,    
    IN GT_U32           leafHwIndex,
    OUT GT_U32          *leafSwIndexPtr
)
{
    GT_U32              pbrBase_k;                   /* the base address of PBR - SW address */
    GT_U32              numOfLinesInFirstPbrBlock_m; /* number of lines occupied by PBR in the first PBR block, 
                                                        rest of the lines are used for IP LPM*/
    GT_U32              numOfLinesInBlock_n;         /* number of legal lines in the memory block*/
    GT_U32              totalNumOfLinesInBlockIncludingGap_h;/* number of lines in the memory block 
                                                                including gap of lines that cannot be used */
    GT_U32              numberOfGapsFromBeginnigOfPbrBase; /* number of "holes" from the beggining of the 
                                                              PBR to the checked HW Index */
    GT_U32              gapSize_w;                         /* the size of the "hole" in each LPM block */
    
    pbrBase_k=PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.indexForPbr;
    numOfLinesInBlock_n = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfLinesInBlock;
    totalNumOfLinesInBlockIncludingGap_h = PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.totalNumOfLinesInBlockIncludingGap;
    numOfLinesInFirstPbrBlock_m = numOfLinesInBlock_n-(pbrBase_k%totalNumOfLinesInBlockIncludingGap_h);         

    if (leafHwIndex<numOfLinesInFirstPbrBlock_m)
    {
        *leafSwIndexPtr = leafHwIndex;
    }
    else
    {

        /* LPM with PBR blocks in case numOfLinesInBlock_n < totalNumOfLinesInBlockIncludingGap_h
         
           blocks:  0       1               17          18          19 --> total 20 blocks
                    -----   -----   .....   -----  e--> ----- e-->  -----  --
                    |   |   |   |           |n-m|       |   |       |   |  ^
                    | n |   | n |      k--> |---|       | n |       | n |  ^
                    |   |   |   |           | m |       |   |       |   |  ^
                    |---|   |---|           |---|       |---|       |---|  ^ h
                    |   |   |   |           |   |       |   |       |   |  ^
                    | w |   | w |           | w |       | w |       | w |  ^
                    -----   -----           -----       -----       -----  -- 
        */

        numberOfGapsFromBeginnigOfPbrBase = (leafHwIndex/totalNumOfLinesInBlockIncludingGap_h);
        if (numberOfGapsFromBeginnigOfPbrBase==0)
        {
            /* in case the HW Index value is in bigger the numOfLinesInFirstPbrBlock_m
               but smaller then the size of one block (h)--> the index resize in the first
               full block after the block partialy occupied by m */
            numberOfGapsFromBeginnigOfPbrBase=1;
        }
        gapSize_w = totalNumOfLinesInBlockIncludingGap_h - numOfLinesInBlock_n;
        *leafSwIndexPtr = (leafHwIndex - (numberOfGapsFromBeginnigOfPbrBase * gapSize_w)); 
    }

    return;
}

/*******************************************************************************
* prvCpssDxChLpmNextPointerDataBuild
*
* DESCRIPTION:
*       Local function for LPM bucket next pointer HW format build.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       dev                 - dev number
*       nextPointerData     - the next pointer data.
*
* OUTPUTS:
*       hwDataPtr           - node data in HW format
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - if any parameter is wrong
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmNextPointerDataBuild
(
    IN  GT_U8                                       dev,
    IN  CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC         nextPointerData,
    OUT GT_U32                                      *hwDataPtr
)
{
    GT_U32      data = 0, maxMemSize;
    GT_U32      entryIndex;

    switch (nextPointerData.pointerType)
    {
        case CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E:
        case CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E:
        case CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E:
            if (nextPointerData.pointerType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)
            {
                if ((nextPointerData.pointerData.nextNodePointer.range5Index < 1)
                     || (nextPointerData.pointerData.nextNodePointer.range5Index > 255))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                /* convert to bytes */
                maxMemSize = PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_BYTES_CNS *
                    (1 << PRV_CPSS_DXCH_LPM_RAM_COMPRESSED_2_NEXT_BUCKET_POINTER_LENGTH_CNS);

                if (nextPointerData.pointerData.nextNodePointer.nextPointer >= maxMemSize)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }

            /* convert to bytes */
            maxMemSize =
                PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_BYTES_CNS *
                PRV_CPSS_DXCH_PP_MAC(dev)->accessTableInfoPtr[PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E].maxNumOfEntries;


            if (nextPointerData.pointerData.nextNodePointer.nextPointer >= maxMemSize)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if (nextPointerData.pointerType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)
            {
                U32_SET_FIELD_MAC(data,
                                  PRV_CPSS_DXCH_LPM_RAM_FIFTH_RANGE_OFFSET_CNS,
                                  PRV_CPSS_DXCH_LPM_RAM_FIFTH_RANGE_LENGTH_CNS,
                                  nextPointerData.pointerData.nextNodePointer.range5Index);
                U32_SET_FIELD_MAC(data,
                                  PRV_CPSS_DXCH_LPM_RAM_COMPRESSED_2_NEXT_BUCKET_POINTER_OFFSET_CNS,
                                  PRV_CPSS_DXCH_LPM_RAM_COMPRESSED_2_NEXT_BUCKET_POINTER_LENGTH_CNS,
                                  nextPointerData.pointerData.nextNodePointer.nextPointer);
            }
            else
            {
                U32_SET_FIELD_MAC(data,
                                  PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_POINTER_OFFSET_CNS,
                                  PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_POINTER_LENGTH_CNS,
                                  nextPointerData.pointerData.nextNodePointer.nextPointer);
            }
            U32_SET_FIELD_MAC(data,
                              PRV_CPSS_DXCH_LPM_RAM_POINT_TO_LOOKUP_1_OFFSET_CNS,
                              PRV_CPSS_DXCH_LPM_RAM_POINT_TO_LOOKUP_1_LENGTH_CNS,
                              nextPointerData.pointerData.nextNodePointer.pointToSipTree);
            U32_SET_FIELD_MAC(data,
                              PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_OFFSET_CNS,
                              PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_LENGTH_CNS,
                              0);
            U32_SET_FIELD_MAC(data,
                              PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_TYPE_OFFSET_CNS,
                              PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_TYPE_LENGTH_CNS,
                              nextPointerData.pointerType);
            break;

        case CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E:
        case CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E:
        case CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E:
            entryIndex = nextPointerData.pointerData.nextHopOrEcmpPointer.entryIndex;
            if (nextPointerData.pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
            {
                maxMemSize = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerNextHop;
            }
            else
            {
                maxMemSize = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.ecmpQos;
            }
            if (entryIndex >= maxMemSize)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            U32_SET_FIELD_MAC(data,
                              PRV_CPSS_DXCH_LPM_RAM_NEXTHOP_OR_ECMP_POINTER_OFFSET_CNS,
                              PRV_CPSS_DXCH_LPM_RAM_NEXTHOP_OR_ECMP_POINTER_LENGTH_CNS,
                              entryIndex);
            U32_SET_FIELD_MAC(data,
                              PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_GROUP_SCOPE_LEVEL_OFFSET_CNS,
                              PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_GROUP_SCOPE_LEVEL_LENGTH_CNS,
                              nextPointerData.pointerData.nextHopOrEcmpPointer.ipv6McGroupScopeLevel);
            U32_SET_FIELD_MAC(data,
                              PRV_CPSS_DXCH_LPM_RAM_UC_RPF_CHECK_OFFSET_CNS,
                              PRV_CPSS_DXCH_LPM_RAM_UC_RPF_CHECK_LENGTH_CNS,
                              nextPointerData.pointerData.nextHopOrEcmpPointer.ucRpfCheckEnable);
            U32_SET_FIELD_MAC(data,
                              PRV_CPSS_DXCH_LPM_RAM_UC_SIP_SA_CHECK_MISMATCH_OFFSET_CNS,
                              PRV_CPSS_DXCH_LPM_RAM_UC_SIP_SA_CHECK_MISMATCH_LENGTH_CNS,
                              nextPointerData.pointerData.nextHopOrEcmpPointer.srcAddrCheckMismatchEnable);
            switch (nextPointerData.pointerType)
            {
                case CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E:
                    U32_SET_FIELD_MAC(data,
                                      PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_OFFSET_CNS,
                                      PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_LENGTH_CNS,
                                      1);
                    break;
                case CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E:
                    U32_SET_FIELD_MAC(data,
                                      PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_OFFSET_CNS,
                                      PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_LENGTH_CNS,
                                      2);
                    break;
                case CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E:
                    U32_SET_FIELD_MAC(data,
                                      PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_OFFSET_CNS,
                                      PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_LENGTH_CNS,
                                      3);
                    break;
                default:
                    break;
            }
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    *hwDataPtr = data;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmNextPointerDataDecode
*
* DESCRIPTION:
*       Local function for LPM bucket next pointer HW format build
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       hwDataPtr           - node data in HW format
*
* OUTPUTS:
*       nextPointerDataPtr - (pointer to) next pointer data.
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on any parameter is wrong
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmNextPointerDataDecode
(
    IN  GT_U32                                      *hwDataPtr,
    OUT CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC         *nextPointerDataPtr
)
{
    GT_U32  nextBucketTypeField;
    GT_U32  pointerTypeField;

    pointerTypeField = U32_GET_FIELD_MAC(*hwDataPtr,
                                         PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_OFFSET_CNS,
                                         PRV_CPSS_DXCH_LPM_RAM_POINTER_TYPE_LENGTH_CNS);
    switch (pointerTypeField)
    {
        case 0:
            nextBucketTypeField = U32_GET_FIELD_MAC(*hwDataPtr,
                                                    PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_TYPE_OFFSET_CNS,
                                                    PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_TYPE_LENGTH_CNS);
            switch (nextBucketTypeField)
            {
                case 0:
                    nextPointerDataPtr->pointerType = CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E;
                    break;
                case 1:
                    nextPointerDataPtr->pointerType = CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E;
                    break;
                case 2:
                    nextPointerDataPtr->pointerType = CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case 1:
            nextPointerDataPtr->pointerType = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
            break;
        case 2:
            nextPointerDataPtr->pointerType = CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E;
            break;
        case 3:
            nextPointerDataPtr->pointerType = CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (nextPointerDataPtr->pointerType)
    {
        case CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E:
        case CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E:
        case CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E:
            if (nextPointerDataPtr->pointerType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)
            {
                nextPointerDataPtr->pointerData.nextNodePointer.range5Index =
                            U32_GET_FIELD_MAC(*hwDataPtr,
                                              PRV_CPSS_DXCH_LPM_RAM_FIFTH_RANGE_OFFSET_CNS,
                                              PRV_CPSS_DXCH_LPM_RAM_FIFTH_RANGE_LENGTH_CNS);
                nextPointerDataPtr->pointerData.nextNodePointer.nextPointer =
                            U32_GET_FIELD_MAC(*hwDataPtr,
                                              PRV_CPSS_DXCH_LPM_RAM_COMPRESSED_2_NEXT_BUCKET_POINTER_OFFSET_CNS,
                                              PRV_CPSS_DXCH_LPM_RAM_COMPRESSED_2_NEXT_BUCKET_POINTER_LENGTH_CNS);
            }
            else
            {
                nextPointerDataPtr->pointerData.nextNodePointer.nextPointer =
                            U32_GET_FIELD_MAC(*hwDataPtr,
                                              PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_POINTER_OFFSET_CNS,
                                              PRV_CPSS_DXCH_LPM_RAM_NEXT_BUCKET_POINTER_LENGTH_CNS);
            }
            nextPointerDataPtr->pointerData.nextNodePointer.pointToSipTree =
                        U32_GET_FIELD_MAC(*hwDataPtr,
                                          PRV_CPSS_DXCH_LPM_RAM_POINT_TO_LOOKUP_1_OFFSET_CNS,
                                          PRV_CPSS_DXCH_LPM_RAM_POINT_TO_LOOKUP_1_LENGTH_CNS);
            break;

        case CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E:
        case CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E:
        case CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E:
            nextPointerDataPtr->pointerData.nextHopOrEcmpPointer.entryIndex =
                        U32_GET_FIELD_MAC(*hwDataPtr,
                                          PRV_CPSS_DXCH_LPM_RAM_NEXTHOP_OR_ECMP_POINTER_OFFSET_CNS,
                                          PRV_CPSS_DXCH_LPM_RAM_NEXTHOP_OR_ECMP_POINTER_LENGTH_CNS);
            nextPointerDataPtr->pointerData.nextHopOrEcmpPointer.ipv6McGroupScopeLevel =
                        U32_GET_FIELD_MAC(*hwDataPtr,
                                          PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_GROUP_SCOPE_LEVEL_OFFSET_CNS,
                                          PRV_CPSS_DXCH_LPM_RAM_IPV6_MC_GROUP_SCOPE_LEVEL_LENGTH_CNS);
            nextPointerDataPtr->pointerData.nextHopOrEcmpPointer.ucRpfCheckEnable =
                        U32_GET_FIELD_MAC(*hwDataPtr,
                                          PRV_CPSS_DXCH_LPM_RAM_UC_RPF_CHECK_OFFSET_CNS,
                                          PRV_CPSS_DXCH_LPM_RAM_UC_RPF_CHECK_LENGTH_CNS);
            nextPointerDataPtr->pointerData.nextHopOrEcmpPointer.srcAddrCheckMismatchEnable =
                        U32_GET_FIELD_MAC(*hwDataPtr,
                                          PRV_CPSS_DXCH_LPM_RAM_UC_SIP_SA_CHECK_MISMATCH_OFFSET_CNS,
                                          PRV_CPSS_DXCH_LPM_RAM_UC_SIP_SA_CHECK_MISMATCH_LENGTH_CNS);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

