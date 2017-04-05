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
* prvCpssDxChBrgFdbHash.h
*
* DESCRIPTION:
*       MAC hash struct implementation.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/
#ifndef __prvCpssDxChBrgFdbHashh
#define __prvCpssDxChBrgFdbHashh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>

/*
 * Typedef: PRV_CPSS_DXCH_MAC_HASH_STC
 *
 * Description: struct contains the hardware parameters for hash mac Address
 *              calculation.
 *
 * Fields:
 *       vlanMode    - the VLAN lookup mode.
 *       hashMode    - the FDB hash function mode.
 *       size        - the entries number in the hash table.
 *       numOfBanks  - number of banks in the FDB.
 *                     ch1,2,3,xcat,xcat2,lion,lion2 - 4 banks. single hash.
 *                     Bobcat2,Caelum,Bobcat3 - 16 banks. support 2 hash modes:
 *                              legacy (crc/xor) - single hash , 'emulation' as 4 banks.
 *                              CRC multi hash - 16 hash functions.
 *                     this parameter currently used only when 'CRC multi hash'
 *       fid16BitHashEn - indication that the FDB hash uses 16 bits of FID.
 *                      GT_TRUE - use 16 bits FID
 *                      GT_FALSE - use 12 bits FID
 *       crcHashUpperBitsMode - 16 MSbits mode for of DATA into the hash function
 *                      relevant to hashMode = CRC_MULTI_HASH
 *
 */
typedef struct
{
    CPSS_MAC_VL_ENT                 vlanMode;
    CPSS_MAC_HASH_FUNC_MODE_ENT     hashMode;
    CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT  size;
    GT_U32                          numOfBanks;
    GT_BOOL                         fid16BitHashEn;
    CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT crcHashUpperBitsMode;
}PRV_CPSS_DXCH_MAC_HASH_STC;

/* macro to check boundary on FDB bank index - return '_retVal' on error */
#define PRV_CPSS_DXCH_FDB_BANK_INDEX_CHECK_MAC(_dev,_bankIndex,_retVal)          \
    if(_bankIndex >= PRV_CPSS_DXCH_PP_MAC(_dev)->bridge.fdbHashParams.numOfBanks)\
    {                                                                            \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                                     \
    }

/* macro to check boundary on FDB bank index - return GT_BAD_PARAM on error */
#define PRV_CPSS_DXCH_FDB_BANK_INDEX_BAD_PARAM_CHECK_MAC(_dev,_bankIndex)    \
    PRV_CPSS_DXCH_FDB_BANK_INDEX_CHECK_MAC(_dev,_bankIndex,GT_BAD_PARAM)

/* macro to check boundary on FDB bank index - return GT_OUT_OF_RANGE on error */
#define PRV_CPSS_DXCH_FDB_BANK_INDEX_OUT_OF_RANGE_CHECK_MAC(_dev,_bankIndex)    \
    PRV_CPSS_DXCH_FDB_BANK_INDEX_CHECK_MAC(_dev,_bankIndex,GT_OUT_OF_RANGE)

#define PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS  0xFFFFFFFF
/*
 * typedef: structure PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC
 *
 * Description:
 *       Muxed fields from the FDB that depend on :
 *       1. vid1_assignment_mode
 *       2. src_id_length_in_fdb
 *
 *      field with value : PRV_CPSS_DXCH_LION3_FDB_FIELD_NOT_USED_CNS , means 'not used'
 * Fields:
 *
 *      srcId           - source ID
 *      udb             - user defined byte
 *      origVid1        - original VID1
 *      daAccessLevel   - DA access level
 *      saAccessLevel   - SA access level
*/
typedef struct{
    GT_U32              srcId;
    GT_U32              udb;
    GT_U32              origVid1;
    GT_U32              daAccessLevel;
    GT_U32              saAccessLevel;
}PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC;

/* number of words in the Au (address update) message */
#define CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS   8


/*******************************************************************************
* prvCpssDxChBrgFdbHashRequestSend
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
GT_STATUS prvCpssDxChBrgFdbHashRequestSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *entryKeyPtr
);

/*******************************************************************************
* prvCpssDxChBrgFdbLion3FdbAuMsgSpecialMuxedFieldsGet
*
* DESCRIPTION:
*       Get Muxed fields from the (FDB unit) Au Msg format that depend on :
*       1. vid1_assignment_mode
*       2. src_id_length_in_fdb
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       hwDataArr       - (pointer to) Au Msg format
*       entryType       - entry type :
*                           0 -- mac entry
*                           1 -- ipmcV4 entry
*                           2 -- ipmcV6 entry
*
* OUTPUTS:
*       specialFieldsPtr  - (pointer to) special muxed fields values
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbLion3FdbAuMsgSpecialMuxedFieldsGet
(
    IN GT_U8                        devNum,
    IN GT_U32                       hwDataArr[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS],
    IN GT_U32                       entryType,
    OUT PRV_CPSS_DXCH_LION3_FDB_SPECIAL_MUXED_FIELDS_STC *specialFieldsPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChBrgFdbHashh */
