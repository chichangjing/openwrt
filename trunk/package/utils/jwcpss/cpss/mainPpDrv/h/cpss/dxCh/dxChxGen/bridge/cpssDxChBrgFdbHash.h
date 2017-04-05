/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChBrgFdbHash.h
*
* DESCRIPTION:
*       Hash calculate for MAC address table implementation.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __cpssDxChBrgFdbHashh
#define __cpssDxChBrgFdbHashh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>

/*******************************************************************************
* cpssDxChBrgFdbHashCalc
*
* DESCRIPTION:
*       This function calculates the hash index for the FDB table.
*       The FDB table holds 4 types of entries :
*       1. for specific mac address and VLAN id.
*       2. for specific src IP, dst IP and VLAN id.
*       3. for specific mac address, VLAN id and VID1.
*          (APPLICABLE DEVICES:  Bobcat2; Caelum; Bobcat3)
*       4. for specific src IP, dst IP, VLAN id and VID1.
*          (APPLICABLE DEVICES:  Bobcat2; Caelum; Bobcat3)
*       for more details see CPSS_MAC_ENTRY_EXT_KEY_STC description.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       hashPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum, bad vlan-Id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported hash function for key type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashCalc
(
    IN  GT_U8           devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr,
    OUT GT_U32          *hashPtr
);

/*******************************************************************************
* cpssDxChBrgFdbHashCrcMultiResultsCalc
*
* DESCRIPTION:
*       This function calculates the CRC multiple hash results
*       (indexes into the FDB table).
*       NOTE: the function do not access the HW , and do only SW calculations.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       entryKeyPtr  - (pointer to) entry key
*       multiHashStartBankIndex  - the first bank for 'multi hash' result.
*                   (APPLICABLE RANGES: 0..15)
*      numOfBanks    - number of banks for 'multi hash' result.
*                   this value indicates the number of elements that will be
*                   retrieved by crcMultiHashArr[]
*                   restriction of (numOfBanks + multiHashStartBankIndex) <= 16 .
*                   (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*      crcMultiHashArr[] - (array of) 'multi hash' CRC results. index in this array is 'bank Id'
*                           (index 0 will hold value relate to bank multiHashStartBankIndex).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or parameters in entryKeyPtr.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashCrcMultiResultsCalc
(
    IN  GT_U8           devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *entryKeyPtr,
    IN  GT_U32       multiHashStartBankIndex,
    IN  GT_U32       numOfBanks,
    OUT GT_U32       crcMultiHashArr[]
);

/*******************************************************************************
* cpssDxChBrgFdbHashRequestSend
*
* DESCRIPTION:
*       The function Send Hash request (HR) message to PP to generate all values
*       of hash results that relate to the 'Entry KEY'.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that HR message was processed by PP.
*       For the results Application can call cpssDxChBrgFdbHashResultsGet.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       entryKeyPtr  - (pointer to) entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or entryKeyPtr->entryType
*       GT_OUT_OF_RANGE          - one of the parameters of entryKeyPtr is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the HR message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashRequestSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *entryKeyPtr
);

/*******************************************************************************
* cpssDxChBrgFdbHashResultsGet
*
* DESCRIPTION:
*       The function return from the HW the result of last performed AU message
*       from the CPU to PP.
*       Meaning the hash results updated after every call to one of the next:
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Application can check that any of those message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*       Retrieving the hash calc are optional per type:
*           1. xorHashPtr = NULL --> no retrieve XOR calc
*           2. crcHashPtr = NULL --> no retrieve CRC calc
*           3. numOfBanks = 0    --> no retrieve of 'multi hash' CRC results
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       multiHashStartBankIndex  - the first bank for 'multi hash' result.
*                             this parameter relevant only when numOfBanks != 0
*                   (APPLICABLE RANGES: 0..15)
*      numOfBanks    - number of banks for 'multi hash' result.
*                   this value indicates the number of elements that will be
*                   retrieved by crcMultiHashArr[]
*                   value 0 means that no need to retrieve any 'multi hash' result.
*                   restriction of (numOfBanks + multiHashStartBankIndex) <= 16 when numOfBanks !=0.
*                   (APPLICABLE RANGES: 0..16)
*      xorHashPtr   - (pointer to) to receive the XOR hash result.
*                     NULL value means that caller not need to retrieve this value.
*      crcHashPtr   - (pointer to) to receive the CRC hash result.
*                     NULL value means that caller not need to retrieve this value.
*
* OUTPUTS:
*      crcMultiHashArr[] - (array of) 'multi hash' CRC results. index in this array is 'bank Id'.
*                     NULL is allowed only when numOfBanks = 0
*                           (index 0 will hold value relate to bank multiHashStartBankIndex).
*      xorHashPtr   - (pointer to) the XOR hash result.
*                      ignored when NULL pointer.
*      crcHashPtr   - (pointer to) the CRC hash result.
*                      ignored when NULL pointer.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or multiHashStartBankIndex or numOfBanks
*       GT_OUT_OF_RANGE          - one of the parameters of entryKeyPtr is out of range
*       GT_BAD_PTR               - crcMultiHashArr[] is NULL pointer when numOfBanks != 0
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashResultsGet
(
    IN  GT_U8        devNum,
    IN  GT_U32       multiHashStartBankIndex,
    IN  GT_U32       numOfBanks,
    OUT GT_U32       crcMultiHashArr[],
    INOUT GT_U32     *xorHashPtr,
    INOUT GT_U32     *crcHashPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgFdbHashh */

